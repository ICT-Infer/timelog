#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <db_185.h>
#include <fcntl.h>
#include <limits.h>

void usage(char* pname[])
{
  fprintf(stderr, "Usage:\n");
  fprintf(stderr, " %s begin <location> [-c <comment>] [<ts>]\n", *pname);
  fprintf(stderr, " %s end <id> <location> [-c <comment>] [<ts>]\n", *pname);
  fprintf(stderr, " %s report [<ts> [<ts>]]\n", *pname);
}

int main (int argc, char* argv[])
{
  char** pname = &(argv[0]);

  if (argc < 2)
  {
    fprintf(stderr, "%s: No command provided.\n\n", *pname);
    usage(pname);
    exit(EXIT_FAILURE);
  }

  char* f = "tl_default.db";
  DB* db = dbopen(f, O_CREAT | O_RDWR, 0, DB_HASH, NULL);
  if (db == NULL)
  {
    fprintf(stderr, "dbopen failed.\n");
    exit(EXIT_FAILURE);
  }
  exit(EXIT_SUCCESS);
}
