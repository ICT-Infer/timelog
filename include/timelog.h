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

/* Point in time. */
typedef struct _timepoint
{
  int64_t cts;  /* Calendar time. Number of seconds since the Epoch. */
  char hts[17]; /* Human readable timestamp of local time at location. */
  char etz[49]; /* Contents of environment variable TZ. */
  char rtz[49]; /* Resulting time zone. */
  char loc[49]; /* Name of location. */
  char msg[65]; /* Message. */
} timepoint;

typedef struct _tlentry
{
  timepoint begin;
  timepoint end;
} tlentry;

/* Information about a time log directory and its files. */
typedef struct _dottl
{
  const char *f_dir;
  const char *f_tps;
  const char *f_tl;
  const RECNOINFO info_tps;
  const RECNOINFO info_tl;
  DB *tps;
  DB *tl;
} dottl;

int tl_init(dottl *);
DB *open_flat(const char *, const RECNOINFO *);
int tpt_init(timepoint *, const char *, const char *, const char *);
char **tpt_ppprint(const timepoint *, char **);
recno_t tps_prev(const DB *);
recno_t tps_head(const DB *);
int tps_push(const DB *, timepoint *);
int tps_peek(const DB *, timepoint *, DBT *);
int tps_pop(const DB *, timepoint *);
