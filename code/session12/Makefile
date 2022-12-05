# #include <linux/bpf.h> must first load

build:
	clang -g -O2 -target bpf -D__TARGET_ARCH_x86 -I/usr/include/x86_64-linux-gnu -I. -c sockops.bpf.c -o sockops.bpf.o
	clang -g -O2 -target bpf -D__TARGET_ARCH_x86 -I/usr/include/x86_64-linux-gnu -I. -c sockredir.bpf.c -o sockredir.bpf.o

run:
	sudo bpftool prog load sockops.bpf.o /sys/fs/bpf/sockops type sockops pinmaps /sys/fs/bpf
	sudo bpftool prog load sockredir.bpf.o /sys/fs/bpf/sockredir type sk_msg map name sock_ops_map pinned /sys/fs/bpf/sock_ops_map
	# sudo bpftool cgroup attach /sys/fs/cgroup/ sock_ops pinned /sys/fs/bpf/sockops
	sudo bpftool cgroup attach /sys/fs/cgroup/unified sock_ops pinned /sys/fs/bpf/sockops
	sudo bpftool prog attach pinned /sys/fs/bpf/sockredir msg_verdict pinned /sys/fs/bpf/sock_ops_map

map:
	sudo bpftool map dump name sock_ops_map

clean:
	sudo bpftool prog detach pinned /sys/fs/bpf/sockredir msg_verdict pinned /sys/fs/bpf/sock_ops_map
	# sudo bpftool cgroup detach /sys/fs/cgroup/ sock_ops name bpf_sockmap
	sudo bpftool cgroup detach /sys/fs/cgroup/unified sock_ops name bpf_sockmap
	sudo rm -rf /sys/fs/bpf/sockops /sys/fs/bpf/sockredir /sys/fs/bpf/sock_ops_map

# fatal error: 'gnu/stubs-32.h' file not found
# https://stackoverflow.com/questions/7412548/error-gnu-stubs-32-h-no-such-file-or-directory-while-compiling-nachos-source
# yum install glibc-devel.i686 -y
# apt-get install libc6-dev -y
