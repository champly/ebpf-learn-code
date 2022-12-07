container:
	docker run -itd --name=lb --privileged -v /sys/kernel/debug:/sys/kernel/debug alpine

build:
	clang -g -O2 -target bpf -D__TARGET_ARCH_x86 -I/usr/include/x86_64-linux-gnu -I. -c xdp-proxy.bpf.c -o xdp-proxy.bpf.o
	bpftool gen skeleton xdp-proxy.bpf.o > xdp-proxy.skel.h
	clang -g -O2 -Wall -I. -c xdp-proxy.c -o xdp-proxy.o
	clang -Wall -O2 -g xdp-proxy.o -static -lbpf -lelf -lz -o xdp-proxy

run:
	# 复制字节码到容器中
	docker cp xdp-proxy.bpf.o lb:/
	# 在容器中安装iproute2命令行工具
	docker exec -it lb apk add iproute2 --update
	# 在容器中挂载XDP程序到eth0网卡
	docker exec -it lb ip link set dev eth0 xdpgeneric object xdp-proxy.bpf.o sec xdp

