#include "unit/tl.h"

#include <stdio.h>

int countnumtests(const unit *cur)
{
  if (cur->subunits == NULL)
  {
    return 1;
  }
  else
  {
    int i, c = 0;
    for (i = 0; i < sizeof(cur->subunits) / sizeof(cur->subunits[0]); i++)
    {
      c += countnumtests(cur->subunits[i]);
    }
    return c;
  }
}

int main ()
{
  int i, r = 0, s = 0, p = 0, f = 0;
  const unit *units[] = {&init, &mergeadd, &pending, &popdrop, &report, &timepoint, &unlog};

  for (i = 0; i < sizeof(units) / sizeof(units[0]); i++)
  {
    int cur_r = 0, cur_s = 0, cur_p = 0, cur_f = 0;

    if (units[i]->setup() != 0)
    {
      cur_s = countnumtests(units[i]);
      fprintf(stderr, "Unit test(s) for `%s': Setup failed. "
              "Skipping %d test(s).\n", units[i]->desc, cur_s);
              
    }
    else
    {
    }

    r += cur_r;
    s += cur_s;
    p += cur_p;
    f += cur_f;
  }

  return s + f;
}
