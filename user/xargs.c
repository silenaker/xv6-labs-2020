#include "kernel/types.h"
#include "user.h"

#define BLOCK_SIZE 512
#define LINE_MAX 2048

int readline(char *line, int size, char *buf, int bufsize)
{
  int i = 0;

  while (i < size && i < bufsize)
  {
    *line = *buf;
    i++;
    if (*line == '\n')
    {
      break;
    }
    line++;
    buf++;
  }

  return i;
}

int main(int argc, char *argv[])
{
  char buf[BLOCK_SIZE];
  char line[LINE_MAX];
  int nr;
  int n;
  int buf_offset;
  int line_size;
  int pid;
  int i;
  char *child_argv[argc + 1];

  for (i = 0; i < argc; i++)
  {
    child_argv[i] = argv[i + 1];
  }
  line_size = 0;
  line[0] = '\0';
  while ((nr = read(0, buf, BLOCK_SIZE)) > 0)
  {
    buf_offset = 0;
    while ((n = readline(line + line_size, LINE_MAX - 1 - line_size,
                         buf + buf_offset, nr - buf_offset)) &&
           n <= nr - buf_offset)
    {
      line_size += n;
      buf_offset += n;
      if (line[line_size - 1] == '\n')
      {
        line[line_size - 1] = '\0';
        if ((pid = fork()) < 0)
        {
          printf("fork error\n");
          exit(1);
        }
        if (pid == 0)
        {
          child_argv[argc - 1] = line;
          child_argv[argc] = 0;
          exec(child_argv[0], child_argv);
          printf("exec error\n");
          exit(1);
        }
        line_size = 0;
        line[0] = '\0';
      }
      else if (line_size == LINE_MAX - 1)
      {
        printf("line too long\n");
        exit(1);
      }
    }
  }

  while (wait(0) != -1)
    ;

  if (nr < 0)
  {
    printf("read error\n");
    exit(1);
  }
  exit(0);
}
