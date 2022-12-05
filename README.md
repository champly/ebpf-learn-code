# ebpf-learn-code

This project is mainly the code of the [《eBPF 核心技术与实战》](https://time.geekbang.org/column/intro/100104501) course, which can be compiled through the `make run` environment, and some can be run directly.

Some of these directory descriptions

- bpf: from `/usr/include/bpf` or `/usr/src/${uname -r}/tools/lib/bpf` or `https://github.com/libbpf/libbpf/tree/master/src`
- vmlinux.h: from `bpftool btf dump file /sys/kernel/btf/vmlinux format c > vmlinux.h`
