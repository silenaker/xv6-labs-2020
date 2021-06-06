#include "kernel/types.h"
#include "user.h"

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("usage: sleep <tick>\n");
    exit(1);
  }
  sleep(atoi(argv[1]));
  exit(0);
}
