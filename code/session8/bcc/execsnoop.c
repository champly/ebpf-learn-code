#include <linux/fs.h>
#include <linux/sched.h>
#include <uapi/linux/ptrace.h>

#define ARGSIZE 64
#define TOTAL_MAX_ARGS 5
#define FULL_MAX_ARGS_ARR (TOTAL_MAX_ARGS * ARGSIZE)
#define LAST_ARG (FULL_MAX_ARGS_ARR - ARGSIZE)

struct data_t {
  u32 pid;
  char comm[TASK_COMM_LEN];
  int retval;
  unsigned int args_size;
  char argv[FULL_MAX_ARGS_ARR];
};

static int __bpf_read_arg_str(struct data_t *data, const char *ptr) {
  if (data->args_size > LAST_ARG) {
    return -1;
  }

  int ret = bpf_probe_read_user_str(&data->argv[data->args_size], ARGSIZE,
                                    (void *)ptr);
  if (ret > ARGSIZE || ret < 0) {
    return -1;
  }

  data->args_size += (ret - 1);
  return 0;
}

BPF_PERF_OUTPUT(events);
BPF_HASH(tasks, u32, struct data_t);

TRACEPOINT_PROBE(syscalls, sys_enter_execve) {
  unsigned int ret = 0;
  const char **argv = (const char **)(args->argv);

  struct data_t data = {};
  u32 pid = bpf_get_current_pid_tgid();
  data.pid = pid;
  bpf_get_current_comm(&data.comm, sizeof(data.comm));

  if (__bpf_read_arg_str(&data, (const char *)argv[0]) < 0) {
    goto out;
  }

#pragma unroll
  for (int i = 1; i < TOTAL_MAX_ARGS; i++) {
    if (__bpf_read_arg_str(&data, (const char *)argv[i]) < 0) {
      goto out;
    }
  }

out:
  tasks.update(&pid, &data);
  return 0;
}

TRACEPOINT_PROBE(syscalls, sys_exit_execve) {
  u32 pid = bpf_get_current_pid_tgid();
  struct data_t *data = tasks.lookup(&pid);

  if (data != NULL) {
    data->retval = args->ret;
    events.perf_submit(args, data, sizeof(struct data_t));
    tasks.delete(&pid);
  }

  return 0;
}
