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

#include "timelog.h"

/* Holds a command name and the function to call. */
typedef struct _cmd
{
  char *name;
  int (*f)(int, char **, const char *, const char *, dottl *);
} cmd;

/*
 * Prints a short help for how to use the program.
 */
void usage(const char *pname)
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
 * Dummy command
 *
 * Use for commands which have not been implemented.
 */
int cmd_dummy(int cargc, char **cargv, const char *pname, const char *cmd,
              dottl *cdtl)
{
  fprintf(stderr, "%s: %s: Not implemented.\n", pname, cmd);
  return 1;
}

/*
 * Command: init
 *
 * Initialize time log.
 */
int cmd_init(int cargc, char **cargv, const char *pname, const char *cmd,
             dottl *cdtl)
{
  int r_init;

  if (cargc > 1)
  {
    fprintf(stderr, "%s: %s: %d additional argument(s) passed. "
                    "First: `%s'.\n\n",
            pname, cmd, cargc - 1, cargv[1]);
    usage(pname);
    return 1;
  }

  if ((r_init = tl_init(cdtl)) != 0)
  {
    fprintf(stderr, "%s: %s: Failed. Error: `%d'.\n", pname, cmd, r_init);
    return 2;
  }

  return 0;
}

/*
 * Command: timepoint
 *
 * Create timepoint and push it on to timepoint-stack.
 */
int cmd_timepoint(int cargc, char **cargv, const char *pname, const char *cmd,
                  dottl *cdtl)
{
  timepoint tpt;
  char *loc = NULL;
  char *msg = NULL;
  char *ts = NULL;

  bool ap_loc = false;
  bool ap_msg = false;
  bool ap_ts = false;
  int cargc_parse = cargc - 1;
  char **cargv_parse = &(cargv[1]);
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
      fprintf(stderr, "%s: %s: Invalid argument `%s'.\n", pname, cmd,
              cargv_parse[0]);
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

  if (tps_push(cdtl->tps, &tpt) != 0)
  {
    fprintf(stderr, "%s: %s: Failed to put tpt on tpt stack.\n", pname, cmd);
    return 10;
  }

  fprintf(stderr, "Timepoint at `%s' in TZ `%s'.\n", tpt.hts, tpt.rtz);

  return 0;
}

/*
 * Command: pending
 *
 * Print timepoint-stack in order from most recent to oldest
 * (without removing anything from the stack).
 */
int cmd_pending(int cargc, char **cargv, const char *pname, const char *cmd,
                dottl *cdtl)
{
  recno_t kval;
  DBT key;
  timepoint tpt;
  char *buf = NULL;

  if (cargc > 1)
  {
    fprintf(stderr, "%s: %s: %d additional argument(s) passed. "
                    "First: `%s'.\n\n",
            pname, cmd, cargc - 1, cargv[1]);
    usage(pname);
    return 1;
  }

  if ((cdtl->tps = open_flat(cdtl->f_tps, &(cdtl->info_tps))) == NULL)
  {
    return 2;
  }

  key.size = sizeof(&kval);
  for (kval = tps_head(cdtl->tps); kval > 0; kval = tps_prev(cdtl->tps))
  {
    key.data = &kval;
    if (tps_peek(cdtl->tps, &tpt, &key) || tpt_ppprint(&tpt, &buf) == NULL)
    {
      fprintf(stderr, "%s: %s: Error during printing.\n", pname, cmd);
      return 3;
    }
    printf("%s", buf);
    free(buf);
  }

  return 0;
}

/*
 * Command: pop-drop
 *
 * Pop timepoint off the timepoint-stack and print it.
 */
int cmd_popdrop(int cargc, char **cargv, const char *pname, const char *cmd,
                dottl *cdtl)
{
  recno_t kval;
  DBT key;
  timepoint tpt;
  char *buf = NULL;

  if (cargc > 1)
  {
    fprintf(stderr, "%s: %s: %d additional argument(s) passed. "
                    "First: `%s'.\n\n",
            pname, cmd, cargc - 1, cargv[1]);
    usage(pname);
    return 1;
  }

  if ((cdtl->tps = open_flat(cdtl->f_tps, &(cdtl->info_tps))) == NULL)
  {
    return 2;
  }

  if ((kval = tps_head(cdtl->tps)) == 0)
  {
    return 3;
  }
  key.size = sizeof(&kval);
  key.data = &kval;

  if (tps_peek(cdtl->tps, &tpt, &key) != 0 ||
      tpt_ppprint(&tpt, &buf) == NULL || tps_pop(cdtl->tps, NULL) != 0)
  {
    return 4;
  }
  printf("%s", buf);
  free(buf);

  return 0;
}

/*
 * Call function implementing requested command.
 */
int main(int argc, char *argv[])
{
  char *pname = argv[0];

  char *cmd_req;
  int cmd_argc;
  char **cmd_argv;

  /* Current dottl. */
  dottl cdtl = {".tl/",
                ".tl/tps.db",
                ".tl/tl.db",
                {R_FIXEDLEN, 0, 0, 0, sizeof(timepoint), 0x00, NULL},
                {R_FIXEDLEN, 0, 0, 0, sizeof(tlentry), 0x00, NULL},
                NULL,
                NULL};

  /* Commands. */
  cmd cmds[] = {
      {"init", &cmd_init},
      {"timepoint", &cmd_timepoint},
      {"pending", &cmd_pending},
      {"pop-drop", &cmd_popdrop},
      {"merge-add", &cmd_dummy},
      {"unlog", &cmd_dummy},
      {"report", &cmd_dummy},
  };
  cmd *cmd_cur;

  if (argc < 2)
  {
    fprintf(stderr, "%s: No command provided.\n\n", pname);
    usage(pname);
    return EXIT_FAILURE;
  }

  cmd_req = argv[1];
  cmd_argc = argc - 1;
  cmd_argv = &(argv[1]);

  for (cmd_cur = cmds; cmd_cur < &cmds[sizeof(cmds) / sizeof(cmds[0])];
       cmd_cur++)
  {
    if (strcmp(cmd_cur->name, cmd_req) == 0)
    {
      int rval = (*(cmd_cur->f))(cmd_argc, cmd_argv, pname, cmd_req, &cdtl);
      if (cdtl.tps != NULL && cdtl.tps->close(cdtl.tps) != 0)
      {
        rval++;
      }
      if (cdtl.tl != NULL && cdtl.tl->close(cdtl.tl) != 0)
      {
        rval++;
      }
      return rval;
    }
  }

  fprintf(stderr, "%s: Unknown command `%s'.\n\n", pname, cmd_req);
  usage(pname);
  return EXIT_FAILURE;
}
