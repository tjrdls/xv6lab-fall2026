// user/freetest.c  — new file
#include "kernel/types.h"
#include "user/user.h"

#define PAGE 4096

int
main(void)
{
  uint64 before = freemem();
  printf("before : %ld bytes = %ld pages\n", before, before / PAGE);

  char *p = sbrk(10 * PAGE);
  if (p == (char *)-1) {
    printf("sbrk failed\n");
    exit(1);
  }

  // TODO ③  touch one byte in each of the 10 pages

  uint64 after = freemem();
  printf("after  : %ld bytes = %ld pages\n", after, after / PAGE);
  printf("used   : %ld pages\n", (before - after) / PAGE);
  exit(0);
}
