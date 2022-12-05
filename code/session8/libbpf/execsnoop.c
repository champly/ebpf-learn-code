#include <bpf/libbpf.h>
#include <linux/bpf.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/types.h> // for size_t
#include <time.h>
#include <unistd.h>

#include "execsnoop.h"
#include "execsnoop.skel.h"

static int libbpf_print_fn(enum libbpf_print_level level, const char *format,
                           va_list args) {
#ifdef DEBUGBPF
  return vfprintf(stderr, format, args);
#else
  return 0;
#endif
}

void handle_host_events(void *ctx, int cpu, __u64 lost_cnt) {
  fprintf(stderr, "Lost %llu events on CPU #%d!\n", lost_cnt, cpu);
}

static void inline quoted_symbol(char c) {
  switch (c) {
  case '"':
    putchar('\\');
    putchar('"');
    break;
  case '\t':
    putchar('\\');
    putchar('t');
    break;
  case '\n':
    putchar('\\');
    putchar('n');
    break;
  default:
    putchar(c);
    break;
  }
}

static void print_args(const struct event *e, bool quote) {
  int args_counter = 0;
  if (quote) {
    putchar('"');
  }

  for (int i = 0; i < e->args_size && args_counter < e->args_count; i++) {
    char c = e->args[i];
    if (quote) {
      if (c == '\0') {
        args_counter++;
        putchar('"');
        putchar(' ');
        if (args_counter < e->args_count) {
          putchar('"');
        }
      } else {
        quoted_symbol(c);
      }
    } else {
      if (c == '\0') {
        args_counter++;
        putchar(' ');
      } else {
        putchar(c);
      }
    }
  }

  if (e->args_count > TOTAL_MAX_ARGS) {
    fputs(" ...", stdout);
  }
}

void handle_event(void *ctx, int cpu, void *data, __u32 data_sz) {
  const struct event *e = data;
  time_t t;
  struct tm *tm;
  char ts[32];

  time(&t);
  tm = localtime(&t);
  strftime(ts, sizeof(ts), "%H:%M:%S", tm);

  printf("%-16s %-6d %-6d %3d ", e->comm, e->pid, e->ppid, e->retval);
  print_args(e, true);
  putchar('\n');
}

static void bump_memlock_rlimit(void) {
  struct rlimit rlim_new = {
      .rlim_cur = RLIM_INFINITY,
      .rlim_max = RLIM_INFINITY,
  };

  if (setrlimit(RLIMIT_MEMLOCK, &rlim_new)) {
    fprintf(stderr, "Failed to increase RLIMIT_MEMLOCK limit!\n");
    exit(1);
  }
}

int main(int argc, char **argv) {
  struct execsnoop_bpf *skel;
  struct perf_buffer_opts pb_opts;
  struct perf_buffer *pb = NULL;
  int err;

  libbpf_set_print(libbpf_print_fn);

  bump_memlock_rlimit();

  skel = execsnoop_bpf__open();

  err = execsnoop_bpf__load(skel);

  err = execsnoop_bpf__attach(skel);

  pb_opts.sample_cb = handle_event;
  pb = perf_buffer__new(bpf_map__fd(skel->maps.events), 64, &pb_opts);

  while ((err = perf_buffer__poll(pb, 100)) >= 0) {
  };
}
