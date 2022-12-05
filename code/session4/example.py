#!/usr/bin/env python3

from bcc import BPF

# load BPF program
b = BPF(src_file="example.c")
b.trace_print()
