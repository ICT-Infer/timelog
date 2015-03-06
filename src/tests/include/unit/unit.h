#ifndef UNIT_H
#define UNIT_H

#include <stdbool.h>
#include <unistd.h>

typedef struct _unit
{
  const char desc[80];
  const struct _unit **subunits;
  int (* const setup)(void);
  bool (* const test)(void);
} unit;

#endif
