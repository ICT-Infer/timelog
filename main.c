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
  char message[160];
  char locname[20];
} timedesc;

typedef struct _tl_entry
{
  timedesc begin;
  timedesc end;
} tl_entry;

void usage(const char* pname)
{
  fprintf(stderr, "Usage:\n");
  fprintf(stderr, " %s init\n", pname);
  fprintf(stderr, " %s push-point [-l <loc>] [-m <msg>] [-t <ts>]\n", pname);
  fprintf(stderr, " %s cheat-dump-stack\n", pname);
  fprintf(stderr, " %s pop-drop-point\n", pname);
  fprintf(stderr, " %s pop-twice-merge-points-log\n", pname);
  fprintf(stderr, " %s show-report\n", pname);
}

void tl_init(const char* pname)
{
  /* TODO: Directory `.tl/'. */

  char f_tldb[] = "tl.db";
  char f_stackdb[] = "stack.db";

  DB* tl_db = dbopen(f_tldb, O_CREAT | O_EXCL | O_RDWR, 00644, DB_RECNO, NULL);
  if (tl_db == NULL)
  {
    fprintf(stderr, "%s: Failed to create tl db `%s'.\n", pname, f_tldb);
    exit(EXIT_FAILURE);
  }
  tl_db->close(tl_db);

  DB* tl_stackdb = dbopen(f_stackdb, O_CREAT | O_EXCL | O_RDWR | R_NOKEY,
    00644, DB_RECNO, NULL);
  if (tl_stackdb == NULL)
  {
    fprintf(stderr, "%s: Failed to create tl stack `%s'.\n", pname, f_stackdb);
    exit(EXIT_FAILURE);
  }
  tl_stackdb->close(tl_stackdb);
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

  char* cmd = argv[1];
  int cmd_argc = argc - 1;
  char** cmd_argv = &(argv[1]);

  if (strcmp(cmd, "init") == 0)
  {
    if (cmd_argc > 1)
    {
      fprintf(stderr, "%s: %s: %d additional argument(s) passed. "
        "First: `%s'.\n\n", pname, cmd, cmd_argc - 1, cmd_argv[1]);
      usage(pname);
      exit(EXIT_FAILURE);
    }
    tl_init(pname);
  }
  else
  {
    timedesc td;
    time(&(td.ts));

    /* TODO: TZ env var and sys tz. */

    /*
    char buf[1024];
    char format[] = "%Y-%m-%dT%H:%M:%S";
    (void)strftime(buf, sizeof(buf), format, localtime(&(td.ts)));
    fprintf(stderr, "%s: DEBUG: Timestamp `%s'.\n",
      pname, buf);
    */

    if (strcmp(cmd, "push-point") == 0)
    {
      fprintf(stderr, "%s: %s: Not implemented.\n", pname, cmd);
      exit(EXIT_FAILURE);
    }
    else if (strcmp(cmd, "cheat-dump-stack") == 0)
    {
      fprintf(stderr, "%s: %s: Not implemented.\n", pname, cmd);
      exit(EXIT_FAILURE);
    }
    else if (strcmp(cmd, "pop-drop-point") == 0)
    {
      fprintf(stderr, "%s: %s: Not implemented.\n", pname, cmd);
      exit(EXIT_FAILURE);
    }
    else if (strcmp(cmd, "pop-twice-merge-points-log") == 0)
    {
      fprintf(stderr, "%s: %s: Not implemented.\n", pname, cmd);
      exit(EXIT_FAILURE);
    }
    else if (strcmp(cmd, "show-report") == 0)
    {
      fprintf(stderr, "%s: %s: Not implemented.\n", pname, cmd);
      exit(EXIT_FAILURE);
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
