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

void tl_begin(int cmd_argc, char* cmd_argv[], DB* tl_db, char* pname[])
{
}

void tl_end(int cmd_argc, char* cmd_argv[], DB* tl_db, char* pname[])
{
}

void tl_report(int cmd_argc, char* cmd_argv[], DB* tl_db, char* pname[])
{
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
  DB* tl_db = dbopen(f, O_CREAT | O_RDWR, 0, DB_HASH, NULL);
  if (tl_db == NULL)
  {
    fprintf(stderr, "dbopen failed.\n");
    exit(EXIT_FAILURE);
  }

  char** cmd = &(argv[1]);
  int cmd_argc = argc - 2;
  char** cmd_argv = &(argv[2]);
  if (strcmp(*cmd, "begin") == 0)
  {
    tl_begin(cmd_argc, cmd_argv, tl_db, pname);
  } else if (strcmp(*cmd, "end") == 0)
  {
    tl_end(cmd_argc, cmd_argv, tl_db, pname);
  } else if (strcmp(*cmd, "report") == 0)
  {
    tl_report(cmd_argc, cmd_argv, tl_db, pname);
  } else
  {
    fprintf(stderr, "%s: Unknown command %s.\n\n", *pname, *cmd);
    usage(pname);
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}
