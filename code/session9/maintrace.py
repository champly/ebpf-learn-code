
# 引入BCC库
from bcc import BPF
from time import strftime

# 加载eBPF 程序
b = BPF(src_file="maintrace.c")

# 挂载uretprobe
b.attach_uretprobe(name="/usr/bin/bash", sym="readline", fn_name="bash_readline")

# 定义性能事件回调（输出时间、UID以及Bash中执行的命令）
def print_event(cpu, data, size):
    event = b["events"].event(data)
    print("%-9s %-6d %s" % (strftime("%H:%M:%S"), event.uid, event.command.decode("utf-8")))

# 打印头
print("%-9s %-6s %s" % ("TIME", "UID", "COMMAND"))

# 绑定性能事件映射和输出函数，并从映射中循环读取数据
b["events"].open_perf_buffer(print_event)
while 1:
    try:
        b.perf_buffer_poll()
    except KeyboardInterrupt:
        exit()
