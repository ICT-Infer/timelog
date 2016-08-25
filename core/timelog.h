/*
 * Copyright (c) 2015, 2016 Erik Nordstrøm <erikn@ict-infer.no>
 *
 * Permission to use, copy, modify, and distribute this software for any
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

#ifndef TIMELOG_H
#define TIMELOG_H

#include <sys/types.h>

/*
 * XXX: Integer values used in the tz enum
 *      are _NOT_ related to time zone offset.
 *
 * XXX: Might need to rethink internal representation of time zones.
 */
typedef enum
{
  TZ_UTC         = 0,
  TZ_EUROPE_OSLO = 1,
} tz;

typedef struct
{
  // XXX: parent_catid = 0 means that the category has no parent.
  unsigned int parent_catid;

  unsigned int id;
  char[255]    name;
  char[255]    comment;
  char[32]     slug;
} category;

typedef struct
{
  uid_t        pw_uid;
  unsigned int catid;

  unsigned int id;
  time_t       t_begin;
  tz           tz_begin;
  time_t       t_end;
  tz           tz_end;
  char[255]    comment;
} entry;

#endif
