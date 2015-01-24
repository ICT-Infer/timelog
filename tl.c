/*
 * Copyright (c) 2014, 2015 Erik Nordstroem <contact@erikano.net>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/stat.h>
#include <sys/types.h>

#include <db.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/* Point in time. */
typedef struct _timepoint
{
  char loc[49]; /* Name of location. */
  char msg[65]; /* Message. */
  char hts[17]; /* Human readable timestamp of local time at location. */
  char etz[49]; /* Contents of environment variable TZ. */
  struct tm ts; /* Timestamp. */
} timepoint;

typedef struct _tlentry
{
  timepoint begin;
  timepoint end;
} tlentry;

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

/*
 * Initialize time log directory and files.
 * Returns zero on success.
 *
 * The absolute value of a non-zero return value
 * indicates the stage of tl_init where failure occured.
 *
 * On failure, rollback is attempted.  Should rollback fail,
 * the return value is sign-swapped, thus becoming negative.
 */
int tl_init()
{
  const char f_tldir[] = ".tl/";
  const char f_tldb[] = "tl.db";
  const char f_tps[] = "tps.db";
  DB* tl_db;
  DB* tl_tps;

  int rem = 4; /* Stages remaining */

  if (mkdir(f_tldir, 00755) != 0)
  {
    goto rollback_init;
  }
  rem--;

  if (chdir(f_tldir) != 0)
  {
    goto rollback_init;
  }
  rem--;

  tl_db = dbopen(f_tldb, O_CREAT | O_EXCL | O_RDWR, 00644, DB_RECNO, NULL);
  if (tl_db == NULL)
  {
    goto rollback_init;
  }
  rem--;
  tl_db->close(tl_db);

  tl_tps = dbopen(f_tps, O_CREAT | O_EXCL | O_RDWR, 00644, DB_RECNO, NULL);
  if (tl_tps == NULL)
  {
    goto rollback_init;
  }
  rem--;
  tl_tps->close(tl_tps);

  return rem;

rollback_init:
  switch (rem)
  {
    case 1:
      if (unlink(f_tldb) != 0)
      {
        return -rem;
      }
      /* FALLTHROUGH */
    case 2:
      /* FALLTHROUGH */
    case 3:
      if (rmdir(f_tldir) != 0)
      {
        return -rem;
      }
      /* FALLTHROUGH */
    case 4:
      return rem;
      break;
    default:
      if (rem > 0)
      {
        return -rem;
      }
      return -5;
  }
}

timepoint* tl_timepoint(timepoint* tpt, const char* loc, const char* msg,
  const char* ts)
{
  /*
   * FIXME MAYBE: Some OS' accept an invalid TZ and silently use UTC.
   *              Can we do something about that?
   */
  time_t currtime;
  bool docmpts = false; /* Flag used to indicate when ts should be compared. */
  char format[] = "%Y-%m-%dT%H:%M"; /* Fmt of the human readable timestamp. */

  const char f_tldir[] = ".tl/";
  const char f_tps[] = "tps.db";
  DB* tl_tps;

  /*RECNOINFO info;*/
  struct stat st_tps;
  recno_t kval;
  DBT key;
  DBT data;

  if (chdir(f_tldir) != 0)
  {
    return NULL;
  }

  /* Get current local time, set seconds to 0. */
  (void)time(&currtime);
  (void)localtime_r(&currtime, &(tpt->ts));
  tpt->ts.tm_sec = 0;

  /* If a timestamp was provided, overwrite with user-provided values. */
  if (ts != NULL)
  {
    if (!(strlen(ts) == 5
      && sscanf(ts, "%2d:%2d",
        &(tpt->ts.tm_hour), &(tpt->ts.tm_min)) == 2
      && tpt->ts.tm_hour >= 0 && tpt->ts.tm_hour <= 23
      && tpt->ts.tm_min >= 0 && tpt->ts.tm_hour <= 59))
    {
      if (strlen(ts) == 16
        && sscanf(ts, "%4d-%2d-%2dT%2d:%2d",
          &(tpt->ts.tm_year), &(tpt->ts.tm_mon), &(tpt->ts.tm_mday),
          &(tpt->ts.tm_hour), &(tpt->ts.tm_min)) == 5)
      {
        /* See ctime(3) */
        tpt->ts.tm_year -= 1900;
        tpt->ts.tm_mon -= 1;

        docmpts = true;
      }
      else
      {
        return NULL;
      }
    }
  }

  /* If the user provided the date, we check it now. */
  if (docmpts)
  {
    time_t cmp = mktime(&(tpt->ts));
    (void)strftime(tpt->hts, sizeof(tpt->hts), format, localtime(&cmp));
    if (strcmp(ts, tpt->hts) != 0)
    {
      return NULL;
    }
  }
  else
  {
    (void)strftime(tpt->hts, sizeof(tpt->hts), format, &(tpt->ts));
  }

  if (msg != NULL)
  {
    if (strlcpy(tpt->msg, msg, sizeof(tpt->msg)) >= sizeof(tpt->msg))
    {
      return NULL;
    }
  }

  if (loc != NULL)
  {
    if (strlcpy(tpt->loc, loc, sizeof(tpt->loc)) >= sizeof(tpt->loc))
    {
      return NULL;
    }
  }

  tl_tps = dbopen(f_tps, O_RDWR | O_EXLOCK, 00644, DB_RECNO, NULL);
  if (tl_tps == NULL)
  {
    return NULL;
  }

  if (stat(f_tps, &st_tps) != 0)
  {
    tl_tps->close(tl_tps);
    return NULL;
  }

  kval = (st_tps.st_size/sizeof(*tpt));
  key.size = sizeof(&kval);
  key.data = &kval;
  data.size = sizeof(*tpt);
  data.data = tpt;

  tl_tps->put(tl_tps, &key, &data, R_IAFTER);
  tl_tps->close(tl_tps);

  return tpt;
}

int main (int argc, char* argv[])
{
  char* pname = argv[0];
  int r_init;
  char* cmd;
  int cmd_argc;
  char** cmd_argv;
  timepoint* tpt_res;

  if (argc < 2)
  {
    fprintf(stderr, "%s: No command provided.\n\n", pname);
    usage(pname);
    exit(EXIT_FAILURE);
  }

  cmd = argv[1];
  cmd_argc = argc - 1;
  cmd_argv = &(argv[1]);

  if (strcmp(cmd, "init") == 0)
  {
    if (cmd_argc > 1)
    {
      fprintf(stderr, "%s: %s: %d additional argument(s) passed. "
        "First: `%s'.\n\n", pname, cmd, cmd_argc - 1, cmd_argv[1]);
      usage(pname);
      exit(EXIT_FAILURE);
    }

    r_init = tl_init();
    if (r_init != 0)
    {
      fprintf(stderr, "%s: %s: Failed. Error: `%d'.\n", pname, cmd, r_init);
      exit(EXIT_FAILURE);
    }
  }
  else
  {
    if (strcmp(cmd, "timepoint") == 0)
    {
      timepoint tpt;

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

      tpt_res = tl_timepoint(&tpt, loc, msg, ts);

      if (tpt_res == NULL)
      {
        /* TODO MAYBE: Indicate what went wrong and report to user. */
        fprintf(stderr, "%s: %s: Failed.\n", pname, cmd);
        exit(EXIT_FAILURE);
      }
      fprintf(stderr, "%s: Using datetime `%s' with time zone `%s' "
        "for timestamp.\n", pname, tpt.hts, tpt.ts.tm_zone);

      exit(EXIT_SUCCESS);
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
