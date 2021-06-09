#include "kernel/types.h"
#include "user.h"

int main(int argc, char *argv[])
{
  int tick;

  if ((tick = uptime()) < 0)
  {
    printf("uptime error\n");
    exit(1);
  }

  printf("uptime: %d\n", tick);
  exit(0);
}