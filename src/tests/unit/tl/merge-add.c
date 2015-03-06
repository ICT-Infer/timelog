#include "unit/unit.h"

static int setup ()
{
  return 1;
}

static bool test ()
{
  return 1;
}

unit const mergeadd = {"tl merge-add", NULL, setup, test};
