docker run -it --rm \
	--name ebpf-for-mac \
	--env BPF_CLANG=clang \
	--privileged \
	-v "$(pwd)/code:/root/code" \
	--pid=host \
	champly/ebpf-for-mac:v1.0.0
