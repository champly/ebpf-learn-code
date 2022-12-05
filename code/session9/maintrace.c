// 包含头文件
#include <uapi/linux/ptrace.h>

// 定义数据结构和性能事件映射
struct data_t {
  u32 uid;
  char command[64];
};
BPF_PERF_OUTPUT(events);

// 定义uretprobe处理函数
int bash_readline(struct pt_regs *ctx) {
  // 查询uid
  struct data_t data = {};
  data.uid = bpf_get_current_uid_gid();

  // 从PT_REGS_RC(ctx)读取返回值
  bpf_probe_read_user(&data.command, sizeof(data.command),
                      (void *)PT_REGS_RC(ctx));

  // 提交性能事件
  events.perf_submit(ctx, &data, sizeof(data));
  return 0;
}
