#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user.h"

#define PATH_MAX 1024

int match(char *, char *);

void find(char *dirpath, char *pattern)
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

    if (match(pattern, de.name))
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
      find(filepath, pattern);
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

// Regexp matcher from Kernighan & Pike,
// The Practice of Programming, Chapter 9.

int matchhere(char *, char *);
int matchstar(int, char *, char *);

int match(char *re, char *text)
{
  if (re[0] == '^')
    return matchhere(re + 1, text);
  do
  { // must look at empty string
    if (matchhere(re, text))
      return 1;
  } while (*text++ != '\0');
  return 0;
}

// matchhere: search for re at beginning of text
int matchhere(char *re, char *text)
{
  if (re[0] == '\0')
    return 1;
  if (re[1] == '*')
    return matchstar(re[0], re + 2, text);
  if (re[0] == '$' && re[1] == '\0')
    return *text == '\0';
  if (*text != '\0' && (re[0] == '.' || re[0] == *text))
    return matchhere(re + 1, text + 1);
  return 0;
}

// matchstar: search for c*re at beginning of text
int matchstar(int c, char *re, char *text)
{
  do
  { // a * matches zero or more instances
    if (matchhere(re, text))
      return 1;
  } while (*text != '\0' && (*text++ == c || c == '.'));
  return 0;
}