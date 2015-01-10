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
  char msg[100]; /* Message */
  char loc[18]; /* Name of location. */
} timepoint;

typedef struct _tl_entry
{
  timepoint begin;
  timepoint end;
} tl_entry;

void usage(const char* pname)
{
  fprintf(stderr, "Usage:\n");
  fprintf(stderr, " %s init\n", pname);
  fprintf(stderr, " %s timepoint [-l <loc>] [-m <msg>] [-t <ts>]\n", pname);
  fprintf(stderr, " %s pending\n", pname);
  fprintf(stderr, " %s pop-drop\n", pname);
  fprintf(stderr, " %s merge-add\n", pname);
  fprintf(stderr, " %s unlog [-d] <id>\n", pname);
  fprintf(stderr, " %s report\n", pname);
}

void tl_init(const char* pname)
{
  /* TODO: Directory `.tl/'. */

  char f_tldb[] = "tl.db";
  char f_tpsdb[] = "tps.db";

  DB* tl_db = dbopen(f_tldb, O_CREAT | O_EXCL | O_RDWR, 00644, DB_RECNO, NULL);
  if (tl_db == NULL)
  {
    fprintf(stderr, "%s: Failed to create tl db `%s'.\n", pname, f_tldb);
    exit(EXIT_FAILURE);
  }
  tl_db->close(tl_db);

  DB* tl_tpsdb = dbopen(f_tpsdb, O_CREAT | O_EXCL | O_RDWR | R_NOKEY,
    00644, DB_RECNO, NULL);
  if (tl_tpsdb == NULL)
  {
    fprintf(stderr, "%s: Failed to create stack db `%s'.\n", pname, f_tpsdb);
    exit(EXIT_FAILURE);
  }
  tl_tpsdb->close(tl_tpsdb);
}

timepoint* tptpopulate(timepoint* tpt, const char* loc, const char* msg,
  const char* ts, const char* pname)
{
  bool errors = false;

  /* TODO: TZ env var and sys tz. */

  if (ts != NULL)
  {
    fprintf(stderr, "%s: tptpopulate: Timestamp parsing not implemented.\n",
      pname);
    exit(EXIT_FAILURE);
  }
  else
  {
    time(&(tpt->ts));
    char buf[1024];
    char format[] = "%Y-%m-%dT%H:%M:%S";
    (void)strftime(buf, sizeof(buf), format, localtime(&(tpt->ts)));
    fprintf(stderr, "%s: Using current time `%s' for timestamp.\n",
      pname, buf);
  }

  if (msg != NULL)
  {
    size_t n_msg = strlcpy(tpt->msg, msg, sizeof(tpt->msg));
    if (n_msg > sizeof(tpt->msg))
    {
      fprintf(stderr, "%s: tptpopulate: msg too long.\n", pname);
      errors = true;
    }
  }

  if (loc != NULL)
  {
    size_t n_loc = strlcpy(tpt->loc, loc, sizeof(tpt->loc));
    if (n_loc > sizeof(tpt->loc))
    {
      fprintf(stderr, "%s: tptpopulate: loc too long.\n", pname);
      errors = true;
    }
  }

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
    if (strcmp(cmd, "timepoint") == 0)
    {
      timepoint tpt;

      /* TODO: Message and location. */
      char* loc = NULL;
      char* msg = NULL;
      char* ts = NULL;

      bool ap_loc = false;
      bool ap_msg = false;
      bool ap_ts = false;
      int cmd_argc_parse = cmd_argc - 1;
      char** cmd_argv_parse = &(cmd_argv[1]);
      while (cmd_argc_parse > 0)
      {
        if (strcmp(cmd_argv_parse[0], "-l") == 0)
        {
          if (ap_loc)
          {
            fprintf(stderr, "%s: %s: Duplicate `-l'.\n", pname, cmd);
            exit(EXIT_FAILURE);
          }
          if (cmd_argc_parse < 2 || strncmp(cmd_argv_parse[1], "-", 1) == 0)
          {
            fprintf(stderr, "%s: %s: `-l': Missing location.\n", pname, cmd);
            exit(EXIT_FAILURE);
          }
          ap_loc = true;
          loc = cmd_argv_parse[1];
          cmd_argc_parse--;
          cmd_argv_parse++;
        }
        else if (strcmp(cmd_argv_parse[0], "-m") == 0)
        {
          if (ap_msg)
          {
            fprintf(stderr, "%s: %s: Duplicate `-m'.\n", pname, cmd);
            exit(EXIT_FAILURE);
          }
          if (cmd_argc_parse < 2 || strncmp(cmd_argv_parse[1], "-", 1) == 0)
          {
            fprintf(stderr, "%s: %s: `-m': Missing message.\n", pname, cmd);
            exit(EXIT_FAILURE);
          }
          ap_msg = true;
          msg = cmd_argv_parse[1];
          cmd_argc_parse--;
          cmd_argv_parse++;
        }
        else if (strcmp(cmd_argv_parse[0], "-t") == 0)
        {
          if (ap_ts)
          {
            fprintf(stderr, "%s: %s: Duplicate `-t'.\n", pname, cmd);
            exit(EXIT_FAILURE);
          }
          if (cmd_argc_parse < 2 || strncmp(cmd_argv_parse[1], "-", 1) == 0)
          {
            fprintf(stderr, "%s: %s: `-t': Missing timestamp.\n", pname, cmd);
            exit(EXIT_FAILURE);
          }
          ap_ts = true;
          ts = cmd_argv_parse[1];
          cmd_argc_parse--;
          cmd_argv_parse++;
        }
        else
        {
          fprintf(stderr, "%s: %s: Invalid argument `%s'.\n",
            pname, cmd, cmd_argv_parse[0]);
          exit(EXIT_FAILURE);
        }
        cmd_argc_parse--;
        cmd_argv_parse++;
      }

      timepoint* tpt_res = tptpopulate(&tpt, loc, msg, ts, pname);

      if (tpt_res == NULL)
      {
        exit(EXIT_FAILURE);
      }

      fprintf(stderr, "%s: %s: Not implemented.\n", pname, cmd);
      exit(EXIT_FAILURE);
    }
    else if (strcmp(cmd, "pending") == 0)
    {
      fprintf(stderr, "%s: %s: Not implemented.\n", pname, cmd);
      exit(EXIT_FAILURE);
    }
    else if (strcmp(cmd, "pop-drop") == 0)
    {
      fprintf(stderr, "%s: %s: Not implemented.\n", pname, cmd);
      exit(EXIT_FAILURE);
    }
    else if (strcmp(cmd, "merge-add") == 0)
    {
      fprintf(stderr, "%s: %s: Not implemented.\n", pname, cmd);
      exit(EXIT_FAILURE);
    }
    else if (strcmp(cmd, "unlog") == 0)
    {
      fprintf(stderr, "%s: %s: Not implemented.\n", pname, cmd);
      exit(EXIT_FAILURE);
    }
    else if (strcmp(cmd, "report") == 0)
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
