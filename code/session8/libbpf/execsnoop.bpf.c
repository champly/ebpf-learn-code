/* https://github.com/iovisor/bcc/blob/master/libbpf-tools/execsnoop.bpf.c */

#include "vmlinux.h"
#include <bpf/bpf_core_read.h>
#include <bpf/bpf_helpers.h>

#include "execsnoop.h"

const volatile uid_t targ_uid = INVALID_UID;

static const struct event empty_event = {};

struct {
  __uint(type, BPF_MAP_TYPE_HASH);
  __uint(max_entries, 10240);
  __type(key, pid_t);
  __type(value, struct event);
} execs SEC(".maps");

struct {
  __uint(type, BPF_MAP_TYPE_PERF_EVENT_ARRAY);
  __uint(key_size, sizeof(u32));
  __uint(value_size, sizeof(u32));
} events SEC(".maps");

SEC("tracepoint/syscalls/sys_enter_execve")
int tracepoint__syscalls__sys_enter_execve(
    struct trace_event_raw_sys_enter *ctx) {
  struct event *event;
  const char **args = (const char **)(ctx->args[1]);
  const char *argp;

  u64 id = bpf_get_current_pid_tgid();
  pid_t pid = (pid_t)id;

  if (bpf_map_update_elem(&execs, &pid, &empty_event, BPF_NOEXIST)) {
    return 0;
  }

  event = bpf_map_lookup_elem(&execs, &pid);
  if (!event) {
    return 0;
  }

  event->pid = pid;
  event->args_count = 0;
  event->args_size = 0;

  unsigned int ret =
      bpf_probe_read_user_str(event->args, ARGSIZE, (const char *)ctx->args[0]);
  if (ret <= ARGSIZE) {
    event->args_size += ret;
  }

  event->args_count++;
#pragma unroll
  for (int i = 1; i < TOTAL_MAX_ARGS; i++) {
    bpf_probe_read_user(&argp, sizeof(argp), &args[i]);
    if (!argp)
      return 0;

    if (event->args_size > LAST_ARG)
      return 0;

    ret =
        bpf_probe_read_user_str(&event->args[event->args_size], ARGSIZE, argp);

    if (ret > ARGSIZE)
      return 0;

    event->args_count++;
    event->args_size += ret;
  }

  bpf_probe_read_user_str(&argp, sizeof(argp), &args[TOTAL_MAX_ARGS]);
  if (!argp)
    return 0;

  event->args_count++;
  return 0;
}

SEC("tracepoint/syscalls/sys_exit_execve")
int tracepoint__syscalls__sys_exit_execve(
    struct trace_event_raw_sys_exit *ctx) {
  u64 id;
  pid_t pid;
  int ret;
  struct event *event;

  id = bpf_get_current_pid_tgid();
  pid = (pid_t)id;
  event = bpf_map_lookup_elem(&execs, &pid);
  if (!event) {
    return 0;
  }

  ret = ctx->ret;
  event->retval = ret;
  bpf_get_current_comm(&event->comm, sizeof(event->comm));

  size_t len = EVENT_SIZE(event);
  if (len <= sizeof(*event))
    bpf_perf_event_output(ctx, &events, BPF_F_CURRENT_CPU, event, len);

  bpf_map_delete_elem(&execs, &pid);
  return 0;
}

char LICENSE[] SEC("license") = "Dual BSD/GPL";
