# wget https://github.com/iovisor/bpftrace/releases/download/v0.16.0/bpftrace
bpftrace -e 'kprobe:kfree_skb /comm=="curl"/ {printf("kstack: %s\n", kstack);}'

bpftrace -e 'tracepoint:net:* { printf("%s(%d): %s %s\n", comm, pid, probe, kstack()); }'

perf trace --no-syscalls -e 'net:*' curl -s baidu.com > /dev/null
