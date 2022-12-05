APPS = execsnoop

.PHONY: all
all: $(APPS)

$(APPS):
	clang -g -O2 -target bpf -D__TARGET_ARCH_x86_64 -I/usr/include/x86_64-linux-gnu -I. -c $@.bpf.c -o $@.bpf.o
	bpftool gen skeleton $@.bpf.o > $@.skel.h
	clang -g -O2 -Wall -I . -c $@.c -o $@.o
	clang -Wall -O2 -g $@.o -static -lbpf -lelf -lz -o $@

vmlinux:
	$(bpftool) btf dump file /sys/kernel/btf/vmlinux format c > vmlinux.h
