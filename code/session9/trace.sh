# main
bpftrace -l "uprobe:/root/code/session9/main:*" | grep callbackTP
bpftrace -e '
	uprobe:./main:main.callbackTP { printf("main.callbackTP i: %d => ", reg("ax")) }
	uretprobe:./main:main.callbackTP { printf("return %d\n", retval) }
'

# bash
bpftrace -e 'uretprobe:/usr/bin/bash:readline { printf("User %d executed \"%s\" command\n", uid, str(retval)); }'
