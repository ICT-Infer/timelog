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

/* TODO MAYBE: When an error occurs, report to user what went wrong. */

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
  char rtz[49]; /* Resulting time zone. */
  int64_t cts; /* Calendar time. Number of seconds since the Epoch. */
} timepoint;

typedef struct _tlentry
{
  timepoint begin;
  timepoint end;
} tlentry;

/* Information about a time log directory and its files. */
typedef struct _dottl
{
  const char* f_dir;
  const char* f_tps;
  const char* f_tl;
  const RECNOINFO info_tps;
  const RECNOINFO info_tl;
  DB* tps;
  DB* tl;
} dottl;

/* Holds a command name and the function to call. */
typedef struct _cmd
{
  char* name;
  int (*f)(int, char**, const char*, const char*, dottl*);
} cmd;

/*
 * Prints a short help for how to use the program.
 */
void usage (const char* pname)
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
int tl_init (dottl* cdtl)
{
  int rem = 4; /* Stages remaining */

  if (cdtl->tl != NULL || cdtl->tps != NULL)
  {
    goto rollback_init;
  }
  rem--;

  if (mkdir(cdtl->f_dir, 00755) != 0)
  {
    goto rollback_init;
  }
  rem--;

  if ((cdtl->tl = dbopen(cdtl->f_tl, O_CREAT | O_EXCL | O_RDWR | O_EXLOCK,
    00644, DB_RECNO, (void*)&(cdtl->info_tl))) == NULL)
  {
    goto rollback_init;
  }
  rem--;

  if ((cdtl->tps = dbopen(cdtl->f_tps, O_CREAT | O_EXCL | O_RDWR | O_EXLOCK,
    00644, DB_RECNO, (void*)&(cdtl->info_tps))) == NULL)
  {
    goto rollback_init;
  }
  rem--;

  return rem;

rollback_init:
  switch (rem)
  {
    case 1:
      unlink(cdtl->f_tps);
      cdtl->tl->close(cdtl->tl);
      /* FALLTHROUGH */
    case 2:
      unlink(cdtl->f_tl);
      if (rmdir(cdtl->f_dir) != 0)
      {
        return -rem;
      }
      /* FALLTHROUGH */
    case 3:
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

/*
 * Open a flatfile database.
 */
DB* open_flat (const char* fname, const RECNOINFO* info)
{
  return dbopen(fname, O_RDWR | O_EXLOCK, 00644, DB_RECNO, (void*)info);
}

/*
 * Initialize timepoint.
 */
int tpt_init (timepoint* tpt,
  const char* loc, const char* msg, const char* ts)
{
  struct tm sts; /* Timestamp. */
  time_t currtime;
  bool docmpts = false; /* Flag used to indicate when ts should be compared. */
  char format[] = "%Y-%m-%dT%H:%M"; /* Fmt of the human readable timestamp. */

  memset(tpt, 0, sizeof(*tpt));

  /* Get current local time, set seconds to 0. */
  (void)time(&currtime);
  (void)localtime_r(&currtime, &sts);
  sts.tm_sec = 0;

  /* If a timestamp was provided, overwrite with user-provided values. */
  if (ts != NULL)
  {
    if (!(strlen(ts) == 5
      && sscanf(ts, "%2d:%2d",
        &(sts.tm_hour), &(sts.tm_min)) == 2
      && sts.tm_hour >= 0 && sts.tm_hour <= 23
      && sts.tm_min >= 0 && sts.tm_hour <= 59))
    {
      if (strlen(ts) == 16
        && sscanf(ts, "%4d-%2d-%2dT%2d:%2d",
          &(sts.tm_year), &(sts.tm_mon), &(sts.tm_mday),
          &(sts.tm_hour), &(sts.tm_min)) == 5)
      {
        /* See ctime(3) */
        sts.tm_year -= 1900;
        sts.tm_mon -= 1;

        docmpts = true;
      }
      else
      {
        return 1;
      }
    }
  }

  /* If the user provided the date, we check it now. */
  tpt->cts = (int64_t) mktime(&sts);
  if (docmpts)
  {
    (void)strftime(tpt->hts, sizeof(tpt->hts), format,
      localtime((time_t*) &tpt->cts));
    if (strcmp(ts, tpt->hts) != 0)
    {
      return 2;
    }
  }
  else
  {
    (void)strftime(tpt->hts, sizeof(tpt->hts), format, &sts);
  }

  if (strlcpy(tpt->rtz, sts.tm_zone, sizeof(tpt->rtz)) >= sizeof(tpt->rtz)
    || (msg != NULL
      && strlcpy(tpt->msg, msg, sizeof(tpt->msg)) >= sizeof(tpt->msg))
    || (loc != NULL
      && strlcpy(tpt->loc, loc, sizeof(tpt->loc)) >= sizeof(tpt->loc)))
  {
    return 3;
  }

  return 0;
}

/*
 * Prepare pretty print of a timepoint.
 */
char** tpt_ppprint (const timepoint* tpt, char** buf)
{
  size_t msize =
    sizeof(tpt->hts)
    + sizeof(char) * 2 + sizeof(tpt->rtz) + sizeof(char)
    + sizeof(char) * 2 + sizeof(tpt->loc) + sizeof(char)
    + sizeof(char) * 3 + sizeof(tpt->msg) + sizeof(char) * 3;
  if((*buf = (char*)malloc(msize)) == NULL)
  {
    return NULL;
  }

  strlcpy(*buf, tpt->hts, msize);
  strlcat(*buf, " (", msize);
  strlcat(*buf, tpt->rtz, msize);
  strlcat(*buf, ")", msize);
  if (*(tpt->loc) != 0x00)
  {
    strlcat(*buf, ", ", msize);
    strlcat(*buf, tpt->loc, msize);
  }
  strlcat(*buf, "\n", msize);
  if (*(tpt->msg) != 0x00)
  {
    strlcat(*buf, "\n  ", msize);
    strlcat(*buf, tpt->msg, msize);
    strlcat(*buf, "\n\n", msize);
  }
  return buf;
}

/*
 * Push a timepoint to a timepoint stack.
 */
int push_tpt (const DB* stack, timepoint* tpt)
{
  int rval;
  DBT data;
  DBT key;

  if (tpt == NULL || tpt->hts[0] == 0)
  {
    return 2;
  }

  data.size = sizeof(*tpt);
  data.data = tpt;

  if (stack->seq(stack, &key, NULL, R_LAST) == 0)
  {
    rval = stack->put(stack, &key, &data, R_CURSOR);
  }
  else
  {
    recno_t kval = 1;
    key.size = sizeof(&kval);
    key.data = &kval;
    rval = stack->put(stack, &key, &data, R_SETCURSOR);
  }
  return rval;
}

/*
 * Peek into the timepoint stack ("cheating").
 */
int peek_tpt (const DB* stack, timepoint* tpt, const int n)
{
  int i = n + 1;
  DBT key;
  DBT data;

  if (tpt == NULL || stack->seq(stack, &key, NULL, R_LAST) != 0)
  {
    return 4;
  }
  do
  {
    if (stack->seq(stack, &key, &data, R_PREV) != 0)
    {
      return 3;
    }
    if (((timepoint*)data.data)->hts[0] != 0)
    {
      i--;
    }
  } while (i > 0 && *(int*)key.data > 1);

  if (data.size != sizeof(*tpt))
  {
    return 2;
  }
  memcpy(tpt, data.data, sizeof(*tpt));

  return 0;
}

/*
 * Pop a timepoint off of a timepoint stack.
 */
int pop_tpt (const DB* stack, timepoint* tpt)
{
  DBT key;
  DBT data;

  if (stack->seq(stack, &key, NULL, R_LAST) != 0)
  {
    return 4;
  }
  do {
    if (stack->seq(stack, &key, &data, R_PREV) != 0)
    {
      return 3;
    }
  } while (((timepoint*)data.data)->hts[0] == 0);

  if (tpt != NULL)
  {
    if (data.size != sizeof(*tpt))
    {
      return 2;
    }
    memcpy(tpt, data.data, sizeof(*tpt));
  }

  return stack->del(stack, &key, R_CURSOR);
}

/*
 * Dummy command
 *
 * Use for commands which have not been implemented.
 */
int cmd_dummy (int cargc, char** cargv,
  const char* pname, const char* cmd, dottl* cdtl)
{
  fprintf(stderr, "%s: %s: Not implemented.\n", pname, cmd);
  return 1;
}

/*
 * Command: init
 *
 * Initialize time log.
 */
int cmd_init (int cargc, char** cargv,
  const char* pname, const char* cmd, dottl* cdtl)
{
  int r_init;

  if (cargc > 1)
  {
    fprintf(stderr, "%s: %s: %d additional argument(s) passed. "
      "First: `%s'.\n\n", pname, cmd, cargc - 1, cargv[1]);
    usage(pname);
    return 1;
  }

  if ((r_init = tl_init(cdtl)) != 0)
  {
    fprintf(stderr, "%s: %s: Failed. Error: `%d'.\n",
      pname, cmd, r_init);
    return 2;
  }
  cdtl->tl->close(cdtl->tl);
  cdtl->tps->close(cdtl->tps);

  return 0;
}

/*
 * Command: timepoint
 *
 * Add a timepoint to timepoint stack.
 */
int cmd_timepoint (int cargc, char** cargv,
  const char* pname, const char* cmd, dottl* cdtl)
{
  timepoint tpt;
  char* loc = NULL;
  char* msg = NULL;
  char* ts = NULL;

  bool ap_loc = false;
  bool ap_msg = false;
  bool ap_ts = false;
  int cargc_parse = cargc - 1;
  char** cargv_parse = &(cargv[1]);
  while (cargc_parse > 0)
  {
    if (strcmp(cargv_parse[0], "-l") == 0)
    {
      if (ap_loc)
      {
        fprintf(stderr, "%s: %s: Duplicate `-l'.\n", pname, cmd);
        return 1;
      }
      if (cargc_parse < 2 || strncmp(cargv_parse[1], "-", 1) == 0)
      {
        fprintf(stderr, "%s: %s: `-l': Missing location.\n", pname, cmd);
        return 2;
      }
      ap_loc = true;
      loc = cargv_parse[1];
      cargc_parse--;
      cargv_parse++;
    }
    else if (strcmp(cargv_parse[0], "-m") == 0)
    {
      if (ap_msg)
      {
        fprintf(stderr, "%s: %s: Duplicate `-m'.\n", pname, cmd);
        return 3;
      }
      if (cargc_parse < 2 || strncmp(cargv_parse[1], "-", 1) == 0)
      {
        fprintf(stderr, "%s: %s: `-m': Missing message.\n", pname, cmd);
        return 4;
      }
      ap_msg = true;
      msg = cargv_parse[1];
      cargc_parse--;
      cargv_parse++;
    }
    else if (strcmp(cargv_parse[0], "-t") == 0)
    {
      if (ap_ts)
      {
        fprintf(stderr, "%s: %s: Duplicate `-t'.\n", pname, cmd);
        return 5;
      }
      if (cargc_parse < 2 || strncmp(cargv_parse[1], "-", 1) == 0)
      {
        fprintf(stderr, "%s: %s: `-t': Missing timestamp.\n", pname, cmd);
        return 6;
      }
      ap_ts = true;
      ts = cargv_parse[1];
      cargc_parse--;
      cargv_parse++;
    }
    else
    {
      fprintf(stderr, "%s: %s: Invalid argument `%s'.\n",
        pname, cmd, cargv_parse[0]);
      return 7;
    }
    cargc_parse--;
    cargv_parse++;
  }

  if ((cdtl->tps = open_flat(cdtl->f_tps, &(cdtl->info_tps))) == NULL)
  {
    fprintf(stderr, "%s: %s: Failed to open tpt stack.\n", pname, cmd);
    return 8;
  }

  if (tpt_init(&tpt, loc, msg, ts) != 0)
  {
    fprintf(stderr, "%s: %s: Failed to initialize tpt.\n", pname, cmd);
    return 9;
  }

  if (push_tpt(cdtl->tps, &tpt) != 0)
  {
    fprintf(stderr, "%s: %s: Failed to put tpt on tpt stack.\n", pname, cmd);
    return 10;
  }

  fprintf(stderr, "Timepoint at `%s' in TZ `%s'.\n", tpt.hts, tpt.rtz);
  cdtl->tps->close(cdtl->tps);

  return 0;
}

/*
 * Command: pending
 *
 * Print timepoint stack in order from most recent to oldest
 * (without removing anything from the stack).
 */
int cmd_pending (int cargc, char** cargv,
  const char* pname, const char* cmd, dottl* cdtl)
{
  int n, i;
  timepoint tpt;
  char* buf = NULL;

  if (cargc > 1)
  {
    fprintf(stderr, "%s: %s: %d additional argument(s) passed. "
      "First: `%s'.\n\n", pname, cmd, cargc - 1, cargv[1]);
    usage(pname);
    return 1;
  }

  if ((cdtl->tps = open_flat(cdtl->f_tps, &(cdtl->info_tps))) == NULL)
  {
    return 2;
  }

  /*
  for (i = (n = num_tpt(cdtl->tps)) - 1 ; i >= 0 ; i--)
  {
    if (peek_tpt(cdtl->tps, &tpt, i) != 0 ||
      tpt_ppprint(&tpt, &buf) == NULL)
    {
      cdtl->tps->close(cdtl->tps);
      if (i < n)
      {
        fprintf(stderr, "%s: %s: ERROR.", pname, cmd);
      }
      return 3;
    }
    printf("%s", buf);
    free(buf);
  }
  */
  cdtl->tps->close(cdtl->tps);

  return 0;
}

/*
 * Command: pop-drop
 *
 * Pop a timepoint off the timepoint stack and print it.
 */
int cmd_popdrop (int cargc, char** cargv,
  const char* pname, const char* cmd, dottl* cdtl)
{
  timepoint tpt;
  char* buf = NULL;

  if (cargc > 1)
  {
    fprintf(stderr, "%s: %s: %d additional argument(s) passed. "
      "First: `%s'.\n\n", pname, cmd, cargc - 1, cargv[1]);
    usage(pname);
    return 1;
  }

  if ((cdtl->tps = open_flat(cdtl->f_tps, &(cdtl->info_tps))) == NULL)
  {
    return 2;
  }

  if (peek_tpt(cdtl->tps, &tpt, 0) != 0 ||
    tpt_ppprint(&tpt, &buf) == NULL ||
    pop_tpt(cdtl->tps, NULL) != 0)
  {
    cdtl->tps->close(cdtl->tps);
    return 3;
  }
  printf("%s", buf);
  free(buf);
  cdtl->tps->close(cdtl->tps);

  return 0;
}

/*
 * Call function implementing requested command.
 */
int main (int argc, char* argv[])
{
  char* pname = argv[0];

  char* cmd_req;
  int cmd_argc;
  char** cmd_argv;

  /* Current dottl. */
  dottl cdtl = {".tl/", ".tl/tps.db", ".tl/tl.db",
    {R_FIXEDLEN, 0, 0, 0, sizeof(timepoint), 0x00, NULL},
    {R_FIXEDLEN, 0, 0, 0, sizeof(tlentry), 0x00, NULL},
    NULL, NULL};

  /* Commands. */
  cmd cmds[] =
  {
    {"init", &cmd_init},
    {"timepoint", &cmd_timepoint},
    {"pending", &cmd_pending},
    {"pop-drop", &cmd_popdrop},
    {"merge-add", &cmd_dummy},
    {"unlog", &cmd_dummy},
    {"report", &cmd_dummy},
  };
  cmd* cmd_cur;

  if (argc < 2)
  {
    fprintf(stderr, "%s: No command provided.\n\n", pname);
    usage(pname);
    exit(EXIT_FAILURE);
  }

  cmd_req = argv[1];
  cmd_argc = argc - 1;
  cmd_argv = &(argv[1]);

  for (cmd_cur = cmds ; cmd_cur < &cmds[sizeof(cmds)/sizeof(cmds[0])] ; cmd_cur++)
  {
    if (strcmp(cmd_cur->name, cmd_req) == 0)
    {
      exit((*(cmd_cur->f))(cmd_argc, cmd_argv, pname, cmd_req, &cdtl));
    }
  }

  fprintf(stderr, "%s: Unknown command `%s'.\n\n", pname, cmd_req);
  usage(pname);
  exit(EXIT_FAILURE);
}
