#include "unit/unit.h"

static int setup ()
{
  return 1;
}

static bool test ()
{
  return 1;
}

unit const unlog = {"tl unlog", NULL, setup, test};
