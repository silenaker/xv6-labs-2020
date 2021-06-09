#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user.h"

#define PATH_MAX 1024

void find(char *dirpath, char *filename)
{
  int fd;
  int n;
  struct dirent de;
  struct stat st;
  char filepath[PATH_MAX];
  int slen;

  if ((fd = open(dirpath, O_RDONLY)) < 0)
  {
    printf("open error: %s\n", dirpath);
    exit(1);
  }

  strcpy(filepath, dirpath);
  slen = strlen(filepath);
  if (filepath[slen - 1] != '/')
  {
    filepath[slen++] = '/';
    filepath[slen] = '\0';
  }

  while ((n = read(fd, &de, sizeof(struct dirent))) > 0)
  {
    if (strcmp(de.name, "..") == 0 || strcmp(de.name, ".") == 0)
    {
      continue;
    }

    strcpy(filepath + slen, de.name);

    if (strcmp(de.name, filename) == 0)
    {
      printf("%s\n", filepath);
    }

    if (stat(filepath, &st) < 0)
    {
      printf("stat error: %s\n", filepath);
      exit(1);
    }

    if (st.type == T_DIR)
    {
      if (de.inum == 0)
      {
        continue;
      }
      find(filepath, filename);
    }
  }

  if (n < 0)
  {
    printf("read error: %s\n", dirpath);
    exit(1);
  }
}

int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    printf("usage: find <dir> <filename>\n");
    exit(1);
  }

  find(argv[1], argv[2]);
  exit(0);
}
