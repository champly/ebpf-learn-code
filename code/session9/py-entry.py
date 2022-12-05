#  from bcc import BPF, USDT

#  u = USDT(pid=pid)
#  u.enable_probe(probe="function__entry", fn_name="print_functions")
#  b = BPF(src_file="py-entry.c", usdt_contexts=[u])

#!/usr/bin/python3

import sys
from bcc import BPF, USDT

if len(sys.argv) < 2:
    print("Usage: %s <tracee_pid>" % sys.argv[0])
    sys.exit(1)

u = USDT(pid=int(sys.argv[1]))
u.enable_probe(probe="function__entry", fn_name="print_functions")
b = BPF(src_file="py-entry.c", usdt_contexts=[u])


def print_event(cpu, data, size):
    event = b["events"].event(data)
    print("%-9s %-6d %s" % (event.filename, event.lineno, event.funcname))


print("%-9s %-6s %s" % ("FILENAME", "LINENO", "FUNCTION"))

b["events"].open_perf_buffer(print_event)
while True:
    try:
        b.perf_buffer_poll()
    except KeyboardInterrupt:
        exit()

