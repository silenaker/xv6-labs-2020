#include "kernel/types.h"
#include "user.h"

int main(int argc, char *argv[])
{
  int n = 2;
  int max = 35;
  int pipe_fds[2];
  int rfd = -1;
  int wfd = -1;
  int pid;
  int prime;
  int mul;

  if (pipe(pipe_fds) < 0)
  {
    printf("pipe error\n");
    exit(1);
  }

  if ((pid = fork()) < 0)
  {
    printf("fork error\n");
    exit(1);
  }

  if (pid > 0)
  {
    close(pipe_fds[0]);
    for (; n <= max; n++)
    {
      if (write(pipe_fds[1], &n, sizeof(int)) < 0)
      {
        printf("write error\n");
        exit(1);
      }
    }
    close(pipe_fds[1]);
    wait(0);
    exit(0);
  }

  rfd = pipe_fds[0];
  close(pipe_fds[1]);
  goto next;

next:
  if (read(rfd, &prime, sizeof(int)) < 0)
  {
    printf("read error\n");
    exit(1);
  }

  printf("prime %d\n", prime);

  while ((n = read(rfd, &mul, sizeof(int))) > 0)
  {
    if (mul % prime == 0)
    {
      continue;
    }

    if (wfd < 0)
    {
      if (pipe(pipe_fds) < 0)
      {
        printf("pipe error\n");
        exit(1);
      }

      if ((pid = fork()) < 0)
      {
        printf("fork error\n");
        exit(1);
      }

      if (pid == 0)
      {
        rfd = pipe_fds[0];
        close(pipe_fds[1]);
        goto next;
      }

      wfd = pipe_fds[1];
      close(pipe_fds[0]);
    }

    if (write(wfd, &mul, sizeof(int)) < 0)
    {
      printf("write error\n");
      exit(1);
    }
  }

  if (n < 0)
  {
    printf("read error\n");
    exit(1);
  }

  if (wfd >= 0)
  {
    close(wfd);
    wait(0);
  }
  exit(0);
}
