TRACEPOINT_PROBE(syscalls, sys_enter_bpf) {
  bpf_trace_printk("%d\\n", args->cmd);
  return 0;
}
