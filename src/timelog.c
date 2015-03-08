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

#include <openssl/evp.h>

#include "timelog.h"

/*
 * Default dottl.
 *
 * The dottl holds information relating to a timelog directory in use.
 * The fields of the dottl type are explained in the declaration
 * of said type in timelog.h (which is to be found in include/).
 */
dottl dottl_default(void)
{
  dottl ddtl = {".tl/", ".tl/tps.db", ".tl/tl.db", NULL, NULL};

  return ddtl;
}

/* See recno(3). */
const RECNOINFO i_tps = {R_FIXEDLEN, 0, 0, 0, sizeof(timepoint), 0x00, NULL};
const RECNOINFO i_tl = {R_FIXEDLEN, 0, 0, 0, sizeof(tlentry), 0x00, NULL};

/*
 * Initialize time log directory and files.
 *
 * Returns zero on success.
 *
 * Returns a positive, non-zero number for invalid arguments.
 *
 * Returns a negative, non-zero number on failure.
 * If applicable to the failure condition, rollback will be attempted
 * and the negative, non-zero return value will indicate
 * whether (-1) or not (<= -2) rollback succeeded.
 */
int tl_init(dottl *cdtl)
{
  if (cdtl->tl != NULL || cdtl->tps != NULL)
  {
    return 1;
  }

  if (mkdir(cdtl->f_dir, 00755) != 0)
  {
    return -1;
  }

  /*
   * It is tempting to combine these if-statements with "or" since
   * their bodies do the same thing. Doing so would hurt readability IMO.
   * So we don't.
   */
  if ((cdtl->tl = dbopen(cdtl->f_tl, O_CREAT | O_EXCL | O_RDWR | O_EXLOCK,
                         00644, DB_RECNO, (void *)&(i_tl))) == NULL)
  {
    goto rollback_init;
  }
  if ((cdtl->tps = dbopen(cdtl->f_tps, O_CREAT | O_EXCL | O_RDWR | O_EXLOCK,
                          00644, DB_RECNO, (void *)&(i_tps))) == NULL)
  {
    goto rollback_init;
  }

  return 0;

rollback_init:
  unlink(cdtl->f_tps);
  cdtl->tl->close(cdtl->tl);
  unlink(cdtl->f_tl);
  if (rmdir(cdtl->f_dir) != 0)
  {
    return -2;
  }
  return -1;
}

/*
 * Open timepoint-stack flat file database.
 */
DB *open_tps(const char *fname)
{
  return dbopen(fname, O_RDWR | O_EXLOCK, 00644, DB_RECNO, (void *)&i_tps);
}

/*
 * Open timelog flat file database.
 */
DB *open_tl(const char *fname)
{
  return dbopen(fname, O_RDWR | O_EXLOCK, 00644, DB_RECNO, (void *)&i_tl);
}

/*
 * Initialize timepoint.
 */
int tpt_init(timepoint *tpt, const char *loc, const char *msg, const char *ts)
{
  char *etz;
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
    if (!(strlen(ts) == 5 &&
          sscanf(ts, "%2d:%2d", &(sts.tm_hour), &(sts.tm_min)) == 2 &&
          sts.tm_hour >= 0 && sts.tm_hour <= 23 && sts.tm_min >= 0 &&
          sts.tm_hour <= 59))
    {
      if (strlen(ts) == 16 &&
          sscanf(ts, "%4d-%2d-%2dT%2d:%2d", &(sts.tm_year), &(sts.tm_mon),
                 &(sts.tm_mday), &(sts.tm_hour), &(sts.tm_min)) == 5)
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

  tpt->cts = (int64_t)mktime(&sts);
  /* If the user provided the date, we check it now. */
  if (docmpts)
  {
    time_t cts_tmp = (time_t)tpt->cts;
    (void)strftime(tpt->hts, sizeof(tpt->hts), format, localtime(&cts_tmp));
    if (strcmp(ts, tpt->hts) != 0)
    {
      return 2;
    }
  }
  else
  {
    (void)strftime(tpt->hts, sizeof(tpt->hts), format, &sts);
  }

  /* Fields etz, rtz, loc and msg. */
  if (((etz = getenv("TZ")) != NULL &&
       strlcpy(tpt->etz, etz, sizeof(tpt->etz)) >= sizeof(tpt->etz)) ||
      strlcpy(tpt->rtz, sts.tm_zone, sizeof(tpt->rtz)) >= sizeof(tpt->rtz) ||
      (loc != NULL &&
       strlcpy(tpt->loc, loc, sizeof(tpt->loc)) >= sizeof(tpt->loc)) ||
      (msg != NULL &&
       strlcpy(tpt->msg, msg, sizeof(tpt->msg)) >= sizeof(tpt->msg)))
  {
    return 3;
  }

  return 0;
}

/*
 * Set cursor to point at previous record in timepoint-stack.
 *
 * Returns index of record.
 * Returns 0 if no previous record can be found (none exists or error).
 */
recno_t tps_prev(const DB *stack)
{
  DBT data;
  DBT key;

  if (stack->seq(stack, &key, &data, R_PREV) != 0)
  {
    return 0;
  }

  return *(recno_t *)key.data;
}

/*
 * Set cursor to point at head of timepoint-stack.
 *
 * Returns index of head.
 * Returns 0 if stack is empty or if there was an error.
 */
recno_t tps_head(const DB *stack)
{
  if (stack->seq(stack, NULL, NULL, R_LAST) != 0)
  {
    return 0;
  }

  return tps_prev(stack);
}

/*
 * Push timepoint onto timepoint-stack.
 */
int tps_push(const DB *stack, timepoint *tpt)
{
  DBT data;
  DBT key;
  recno_t kval;

  if (tpt == NULL || tpt->hts[0] == 0)
  {
    return 2;
  }

  data.size = sizeof(*tpt);
  data.data = tpt;
  kval = tps_head(stack) + 1;
  key.size = sizeof(&kval);
  key.data = &kval;

  return stack->put(stack, &key, &data, R_SETCURSOR);
}

/*
 * Peek at the timepoint-stack, retrieving a timepoint
 * from anywhere in the stack without pop-ing anything.
 * ("Cheating".)
 */
int tps_peek(const DB *stack, timepoint *tpt, DBT *key)
{
  DBT data;

  if (stack->get(stack, key, &data, 0) != 0)
  {
    return 2;
  }
  memcpy(tpt, data.data, sizeof(*tpt));

  return 0;
}

/*
 * Pop timepoint off of timepoint-stack.
 */
int tps_pop(const DB *stack, timepoint *tpt)
{
  recno_t kval;
  DBT key;
  struct stat sb;

  if ((kval = tps_head(stack)) == 0)
  {
    return 3;
  }
  key.size = sizeof(&kval);
  key.data = &kval;

  if (tpt != NULL && tps_peek(stack, tpt, &key) != 0)
  {
    return 2;
  }

  /*
   * They said in dbopen(3) that the fd returned by the fd routine is
   * "not necessarily associated with any of the underlying files
   *  used by the access method".
   * TODO: Ensure the below truncation is safe.
   */
  fstat(stack->fd(stack), &sb);
  return ftruncate(stack->fd(stack), sb.st_size - sizeof(*tpt));
}

/*
 * Initialize tlentry.
 */
int tle_init(tlentry *tle, timepoint *tpt_h, timepoint *tpt_p)
{
  if (tpt_h->cts < tpt_p->cts)
  {
    /* Reverse order. */
    tle->begin = *tpt_h;
    tle->end = *tpt_p;
  }
  else
  {
    /* Default order. */
    tle->begin = *tpt_p;
    tle->end = *tpt_h;
  }

  return 0;
}

/*
 * Set cursor to point at previous record in time log.
 *
 * Returns index of record.
 * Returns 0 if no previous record can be found (none exists or error).
 */
recno_t tl_prev(const DB *tl)
{
  DBT data;
  DBT key;

  if (tl->seq(tl, &key, &data, R_PREV) != 0)
  {
    return 0;
  }

  if (!((tlentry *)data.data)->iae)
  {
    return tl_prev(tl);
  }

  return *(recno_t *)key.data;
}

/*
 * Set cursor to point at head of time log.
 *
 * Returns index of head.
 * Returns 0 if stack is empty or if there was an error.
 */
recno_t tl_head(const DB *tl)
{
  if (tl->seq(tl, NULL, NULL, R_LAST) != 0)
  {
    return 0;
  }

  return tl_prev(tl);
}

/*
 * Insert tlentry into time log.
 *
 * Returns pointer to SHA-1 digest of inserted record on success.
 * Returns NULL pointer on failure.
 */
unsigned char *tl_insert(const DB *tl, tlentry *tle)
{
  /* TODO: Ensure entry is not overlapping others in an invalid way. */
  /* TODO: Insert in ordered position. */
  DBT data;
  DBT key;
  recno_t kval;
  EVP_MD_CTX *mctx;

  if (tle == NULL || tle->begin.hts[0] == 0 || tle->end.hts[0] == 0)
  {
    return NULL;
  }

  tle->iae = true;
  data.size = sizeof(*tle);
  data.data = tle;
  kval = tl_head(tl) + 1;
  key.size = sizeof(&kval);
  key.data = &kval;

  mctx = EVP_MD_CTX_create();
  EVP_DigestInit_ex(mctx, EVP_sha1(), NULL);
  EVP_DigestUpdate(mctx, (const void *) &(tle->begin), sizeof(tle->begin));
  EVP_DigestUpdate(mctx, (const void *) &(tle->end), sizeof(tle->end));
  EVP_DigestFinal_ex(mctx, tle->id, NULL);
  EVP_MD_CTX_destroy(mctx);

  if (tl->put(tl, &key, &data, R_SETCURSOR) != 0)
  {
    return NULL;
  }

  return tle->id;
}

/*
 * Drop tlentry by ID from time log.
 *
 * Returns 0 on success.
 * Returns non-zero on failure.
 */
int tl_drop(const DB *tl, unsigned char *id)
{
  /* TODO */
  return 1;
}
