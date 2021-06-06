#include "kernel/types.h"
#include "user.h"

int main(int argc, char *argv[])
{
  int pipe1_fds[2];
  int pipe2_fds[2];
  int pid;
  char buf[5]; // ping/pong
  int n;

  if (pipe(pipe1_fds) < 0)
  {
    printf("pipe error\n");
    exit(1);
  }

  if (pipe(pipe2_fds) < 0)
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
    close(pipe1_fds[1]);
    close(pipe2_fds[0]);

    if ((n = read(pipe1_fds[0], buf, 4)) < 0)
    {
      printf("read error\n");
      exit(1);
    }
    if (n < 4)
    {
      printf("received unexpected\n");
      exit(1);
    }
    buf[4] = '\0';
    if (strcmp(buf, "ping") != 0)
    {
      printf("received unexpected\n");
      exit(1);
    }
    printf("%d: received ping\n", getpid());
    if (write(pipe2_fds[1], "pong", 4) < 0)
    {
      printf("write error\n");
      exit(1);
    }
    exit(0);
  }

  close(pipe1_fds[0]);
  close(pipe2_fds[1]);

  if (write(pipe1_fds[1], "ping", 4) < 0)
  {
    printf("write error\n");
    exit(1);
  }

  if ((n = read(pipe2_fds[0], buf, 4)) < 0)
  {
    printf("read error\n");
    exit(1);
  }
  if (n < 4)
  {
    printf("received unexpected\n");
    exit(1);
  }
  buf[4] = '\0';
  if (strcmp(buf, "pong") != 0)
  {
    printf("received unexpected\n");
    exit(1);
  }
  printf("%d: received pong\n", getpid());
  wait(0);
  exit(0);
}
