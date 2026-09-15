#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/sysinfo.h"
#include "user/user.h"

int
main(void)
{
  struct sysinfo info;

  if (sysinfo(&info) < 0) {
    printf("sysinfo failed\n");
    exit(1);
  }
  printf("freepages = %lu pages\n", info.freepages);
  printf("nproc     = %lu\n", info.nproc);

  // TODO ③  fork a child, call sysinfo again, and see nproc change
  int pid = fork();
  if (pid < 0) exit(1);

  if (pid == 0) {
    if (sysinfo(&info) < 0) {
      printf("sysinfo failed\n");exit(1);
    }
    printf("[child]freepages = %lu pages\n", info.freepages);
    printf("[child]nproc     = %lu\n", info.nproc);
  } else wait(0);

  exit(0);
}
