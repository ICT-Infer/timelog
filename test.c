#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
  int errors = 0; /* Errors in the test program itself. */
  int nt_passed = 0; /* Number of tests passed. */
  int nt_failed = 0; /* Number of tests failed. */

  /* Temporary directory in which tests will be run. */
  char template[] = "/tmp/tl-XXXXXXXX";
  char* tmpdir = mkdtemp(template);

  /* TODO: Actual tests. */

  if (rmdir(tmpdir) != 0)
  {
    fprintf(stderr, "Failed to remove temporary directory `%s'.\n", tmpdir);
    errors++;
  }

  printf("%d / %d tests passed, %d failed.\n",
    nt_passed, nt_passed + nt_failed, nt_failed);

  exit(errors + nt_failed);
}
