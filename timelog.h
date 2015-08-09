#ifndef TIMELOG_H
#define TIMELOG_H

/*
 * It bears to note that the integer values used
 * in the tz enum are _NOT_ related to time zone offset.
 */
typedef enum
{
  TZ_UTC         = 0,
  TZ_EUROPE_OSLO = 1,
} tz;

typedef struct
{
  /* Note: parent_catid = 0 means that the category has no parent. */
  unsigned int parent_catid;

  unsigned int id;
  char[255]    name;
  char[255]    comment;
  char[32]     slug;
} category;

typedef struct
{
  unsigned int uid;
  unsigned int catid;

  unsigned int id;
  time_t       t_begin;
  tz           tz_begin;
  time_t       t_end;
  tz           tz_end;
  char[255]    comment;
} entry;

#endif
