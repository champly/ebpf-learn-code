#include <uapi/linux/ptrace.h>

struct data_t {
  char filename[128];
  char funcname[64];
  int lineno;
};

BPF_PERF_OUTPUT(events);

int print_functions(struct pt_regs *ctx) {
  uint64_t argptr;
  struct data_t data = {};

  bpf_usdt_readarg(1, ctx, &argptr);
  bpf_probe_read_user(&data.filename, sizeof(data.filename), (void *)argptr);

  bpf_usdt_readarg(2, ctx, &argptr);
  bpf_probe_read_user(&data.funcname, sizeof(data.funcname), (void *)argptr);

  bpf_usdt_readarg(3, ctx, &data.lineno);

  events.perf_submit(ctx, &data, sizeof(data));
  return 0;
};
