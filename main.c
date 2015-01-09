#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>

#include <sys/types.h>
#include <db.h>
#include <fcntl.h>
#include <limits.h>

#include <time.h>
#include <stdbool.h>

/* Point in time. */
typedef struct _timepoint
{
  time_t ts; /* Timestamp */
  char msg[160]; /* Message */
  char loc[20]; /* Name of location. */
} timepoint_t;

typedef struct _tl_entry
{
  timepoint_t begin;
  timepoint_t end;
} tl_entry_t;

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
    fprintf(stderr, "%s: Failed to create stack db `%s'.\n", pname, f_stackdb);
    exit(EXIT_FAILURE);
  }
  tl_stackdb->close(tl_stackdb);
}

timepoint_t* timepoint(timepoint_t* tpt, const char* msg, const char* loc,
  const char* pname)
{
  bool errors = false;
  time(&(tpt->ts));

  if (msg != NULL)
  {
    size_t n_msg = strlcpy(tpt->msg, msg, sizeof(tpt->msg));
    if (n_msg > sizeof(tpt->msg))
    {
      fprintf(stderr, "%s: timepoint: msg too long.\n", pname);
      errors = true;
    }
  }

  if (loc != NULL)
  {
    size_t n_loc = strlcpy(tpt->loc, loc, sizeof(tpt->loc));
    if (n_loc > sizeof(tpt->loc))
    {
      fprintf(stderr, "%s: timepoint: loc too long.\n", pname);
      errors = true;
    }
  }

  /* TODO: TZ env var and sys tz. */

  if (errors)
  {
    return NULL;
  }
  return tpt;
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
    if (strcmp(cmd, "push-point") == 0)
    {
      timepoint_t tpt;

      /* TODO: Message and location. */
      char* msg = NULL;
      char* loc = NULL;

      timepoint_t* tpt_res = timepoint(&tpt, msg, loc, pname);

      if (tpt_res == NULL)
      {
        exit(EXIT_FAILURE);
      }

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
