#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>

#include <sys/types.h>
#include <db.h>
#include <fcntl.h>
#include <limits.h>

#include <time.h>

typedef struct _timedesc
{
  time_t ts;
  char comment[160];
  char locname[20];
  double loclatdeg; /* Latitude in degrees. */
  double loclongdeg; /* Longitude in degrees. */
  double locuncertm; /* Uncertainty in meters. */
} timedesc;

typedef struct _tl_entry
{
  timedesc begin;
  timedesc end;
} tl_entry;

void usage(const char* pname)
{
  fprintf(stderr, "Usage:\n");
  fprintf(stderr, " %s create-db [-f <file.db>]\n", pname);
  fprintf(stderr, " %s insert -b [-l <location>] [-c <comment>] -t [<ts>] "
    "[-f <file.db>]\n", pname);
  fprintf(stderr, " %s update <id> "
    "-b [-l <location>] [-c <comment>] [-t [<ts>]] [-f <file.db>]\n", pname);
  fprintf(stderr, " %s update <id> "
    "-e [-l <location>] [-c <comment>] [-t [<ts>]] [-f <file.db>]\n", pname);
  fprintf(stderr, " %s delete <id> [-f <file.db>]\n", pname);
  fprintf(stderr, " %s show-report [<ts> [<ts>]] [-f <file.db>]\n", pname);
}

char* tl_dbfile(char** fp, const char* pname)
{
  if (*fp == NULL)
  {
    *fp = (char*) malloc(14);
    strcpy(*fp, "tl_default.db");
    fprintf(stderr, "%s: Using tl database `%s'.\n", pname, *fp);
  }

  return *fp;
}

DB* tl_dbcreate(char* f, const char* pname)
{
  tl_dbfile(&f, pname);

  DB* tl_db = dbopen(f, O_CREAT | O_EXCL | O_RDWR | R_NOKEY,
    00644, DB_RECNO, NULL);
  if (tl_db == NULL)
  {
    fprintf(stderr, "%s: Failed to create tl database `%s'.\n", pname, f);
    exit(EXIT_FAILURE);
  }

  return tl_db;
}

DB* tl_dbopen(char* f, const char* pname)
{
  tl_dbfile(&f, pname);

  DB* tl_db = dbopen(f, O_RDWR | R_NOKEY, 00644, DB_RECNO, NULL);
  if (tl_db == NULL)
  {
    fprintf(stderr, "%s: Failed to open tl database `%s'.\n", pname, f);
    exit(EXIT_FAILURE);
  }

  return tl_db;
}

void tl_einsert(int cmd_argc, char* cmd_argv[], char* f, const char* pname)
{
  DB* tl_db = tl_dbopen(f, pname);
}

void tl_eupdate(int cmd_argc, char* cmd_argv[], char* f, const char* pname)
{
  DB* tl_db = tl_dbopen(f, pname);
}

void tl_edelete(int cmd_argc, char* cmd_argv[], char* f, const char* pname)
{
  DB* tl_db = tl_dbopen(f, pname);
}

void tl_preport(int cmd_argc, char* cmd_argv[], char* f, const char* pname)
{
  DB* tl_db = tl_dbopen(f, pname);
}

int main (int argc, char* argv[])
{
  char* pname = argv[0];
  if (argc < 2)
  {
    fprintf(stderr, "%s: No command provided.\n\n", pname);
    usage(pname);
    exit(EXIT_FAILURE);
  }

  char* f = NULL;
  int cmd_argc = argc;
  if (argc > 3 && strcmp(argv[argc - 2], "-f") == 0)
  {
    char fc[MAXPATHLEN];
    strlcpy(fc, argv[argc - 1], sizeof(fc));
    f = fc;
    cmd_argc -= 2;
  }

  char* cmd = argv[1];
  cmd_argc -= 1;
  char** cmd_argv = &(argv[1]);
  if (strcmp(cmd, "create-db") == 0)
  {
    if (cmd_argc > 1)
    {
      fprintf(stderr, "%s: create-db: %d additional argument(s) passed. "
        "First: `%s'.\n\n", pname, cmd_argc - 1, cmd_argv[1]);
      usage(pname);
      exit(EXIT_FAILURE);
    }
    DB* tl_db = tl_dbcreate(f, pname);
  }
  else
  {
    timedesc td;
    time(&(td.ts));

    /* TODO: TZ env var and sys tz. */

    char buf[1024];
    char format[] = "%Y-%m-%dT%H:%M:%S";
    (void)strftime(buf, sizeof(buf), format, localtime(&(td.ts)));
    fprintf(stderr, "%s: DEBUG: Timestamp `%s'.\n",
      pname, buf);

    if (strcmp(cmd, "insert") == 0)
    {
      tl_einsert(cmd_argc, cmd_argv, f, pname);
    }
    else if (strcmp(cmd, "update") == 0)
    {
      tl_eupdate(cmd_argc, cmd_argv, f, pname);
    }
    else if (strcmp(cmd, "delete") == 0)
    {
      tl_edelete(cmd_argc, cmd_argv, f, pname);
    }
    else if (strcmp(cmd, "show-report") == 0)
    {
      tl_preport(cmd_argc, cmd_argv, f, pname);
    }
    else
    {
      fprintf(stderr, "%s: Unknown command `%s'.\n\n", pname, cmd);
      usage(pname);
      exit(EXIT_FAILURE);
    }
  }


  exit(EXIT_SUCCESS);
}
