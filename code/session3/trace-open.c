#include <linux/sched.h>
#include <uapi/linux/openat2.h>

struct data_t {
  u32 pid;
  u64 ts;
  char comm[TASK_COMM_LEN];
  char fname[NAME_MAX];
};

BPF_PERF_OUTPUT(events);

// define kprobe function
int hello_world2(struct pt_regs *ctx, int dfd, const char __user *filename,
                 struct open_how *how) {
  struct data_t data = {};

  // get PID & time
  data.pid = bpf_get_current_pid_tgid();
  data.ts = bpf_ktime_get_ns();

  // get program name
  if (bpf_get_current_comm(&data.comm, sizeof(data.comm)) == 0) {
    bpf_probe_read(&data.fname, sizeof(data.fname), (void *)filename);
  }

  // submit event
  events.perf_submit(ctx, &data, sizeof(data));
  return 0;
}
