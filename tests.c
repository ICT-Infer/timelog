#include <sys/param.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

int main()
{
  int errors = 0; /* Errors in the test program itself. */
  int nt_passed = 0; /* Number of tests passed. */
  int nt_failed = 0; /* Number of tests failed. */

  char odir[MAXPATHLEN];
  if (getcwd(odir, sizeof(odir)) == NULL)
  {
    fprintf(stderr, "`getcwd' failed.\n");
    exit(EXIT_FAILURE);
  }

  char tlb[MAXPATHLEN];
  struct stat stlb;
  if (strlcpy(tlb, odir, sizeof(tlb)) > sizeof(tlb) ||
    strlcat(tlb, "/bin/tl", sizeof(tlb)) > sizeof(tlb) ||
    stat(tlb, &stlb) != 0)
  {
    fprintf(stderr, "Creation of path to `tl' binary failed.\n");
    exit(EXIT_FAILURE);
  }

  /* Temporary directory in which tests will be run. */
  char template[] = "/tmp/tl-XXXXXXXX";
  char* tmpdir = mkdtemp(template);
  if (chdir(tmpdir) != 0)
  {
    fprintf(stderr, "`chdir' into temporary directory `%s' failed.\n", tmpdir);
    exit(EXIT_FAILURE);
  }

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
