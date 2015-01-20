/*
 * Copyright (c) 2015 Erik Nordstroem <contact@erikano.net>
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

#include <sys/param.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <sys/wait.h>

typedef struct _tltest
{
  bool xsuccess; /* Expecting success (true) or failure (false)? */
  char* desc; /* Description. */
  char** argv;
} tltest;

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

  tltest tests[] =
  {
    {true, "`tl init'",
      (char *[]){tlb, "init", NULL}},

    {false, "`tl init' in directory with existing timelog",
      (char *[]){tlb, "init", NULL}},

    {false, "`tl' without arguments",
      (char *[]){tlb, NULL}},

    {false, "`tl x' (invalid command)",
      (char *[]){tlb, "x", NULL}},

    {true, "`tl pending' with empty stack",
      (char *[]){tlb, "pending", NULL}},

    {false, "`tl pop-drop' with empty stack",
      (char *[]){tlb, "pop-drop", NULL}},

    {false, "`tl merge-add' with empty stack",
      (char *[]){tlb, "merge-add", NULL}},

    {true, "`tl timepoint'",
      (char *[]){tlb, "timepoint", NULL}},

    {true, "`tl pending' with one element on stack",
      (char *[]){tlb, "pending", NULL}},

    {true, "`tl pop-drop' with one element on stack",
      (char *[]){tlb, "pop-drop", NULL}},

    {true, "`tl timepoint -l <loc>' with valid length loc",
      (char *[]){tlb, "timepoint", "-l", "xxxxxxxxxxxxxxxxxy", NULL}},

    {false, "`tl timepoint -l <loc>' with invalid length loc",
      (char *[]){tlb, "timepoint", "-l", "xxxxxxxxxxxxxxxxxyz", NULL}},

    {true, "`tl timepoint -m <msg>' with valid length msg",
      (char *[]){tlb, "timepoint", "-m",
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxy", NULL}},

    {false, "`tl timepoint -m <msg>' with invalid length msg",
      (char *[]){tlb, "timepoint", "-m",
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxyz", NULL}},

    {true, "`tl timepoint -t <ts>' with valid ts #1",
      (char *[]){tlb, "timepoint", "-t", "22:22", NULL}},

    {true, "`tl timepoint -t <ts>' with valid ts #2",
      (char *[]){tlb, "timepoint", "-t", "2015-01-10T22:22", NULL}},

    {false, "`tl timepoint -t <ts>' with invalid ts #1",
      (char *[]){tlb, "timepoint", "-t", "T22:22", NULL}},

    {false, "`tl timepoint -t <ts>' with invalid ts #2",
      (char *[]){tlb, "timepoint", "-t", "22:22:00", NULL}},

    {false, "`tl timepoint -t <ts>' with invalid ts #3",
      (char *[]){tlb, "timepoint", "-t", "2015-01-10t22:22", NULL}},

    {false, "`tl timepoint -t <ts>' with invalid ts #4",
      (char *[]){tlb, "timepoint", "-t", "2014-02-31T22:22", NULL}},

    {true, "`tl merge-add' with multiple points on stack",
      (char *[]){tlb, "merge-add", NULL}},

    {true, "`tl report' with one entry in log",
      (char *[]){tlb, "report", NULL}},

    {false, "`tl unlog' 2nd log entry with only one entry in log",
      (char *[]){tlb, "unlog", "2", NULL}},

    {true, "`tl unlog' 1st log entry",
      (char *[]){tlb, "unlog", "1", NULL}},

    {false, "`tl unlog' 1st log entry again",
      (char *[]){tlb, "unlog", "1", NULL}},

    {true, "`tl report' with empty log",
      (char *[]){tlb, "report", NULL}}
  };

  int i;
  for (i = 0; i < sizeof(tests)/sizeof(tests[0]); i++)
  {
    pid_t pid = fork();
    if (pid < 0)
    {
      fprintf(stderr, "Failed to run test #%d.\n", i + 1);
      errors++;
    }
    else if (pid == 0)
    {
      fclose(stdin);
      fclose(stdout);
      fclose(stderr);
      return execve(tlb, tests[i].argv, NULL);
    }
    else
    {
      int r;
      waitpid(pid, &r, 0);
      if ((r == 0 && tests[i].xsuccess) || (r != 0 && !(tests[i].xsuccess)))
      {
        nt_passed++;
      }
      else
      {
        fprintf(stderr, "Test: %s. Failed.\n", tests[i].desc);
        nt_failed++;
      }
    }
  }

  const char f_tldir[] = ".tl/";
  const char f_tldb[] = "tl.db";
  const char f_tps[] = "tps.db";
  if (chdir(f_tldir) == 0)
  {
    unlink(f_tldb);
    unlink(f_tps);
    chdir("..");
    rmdir(f_tldir);
  }
  if (rmdir(tmpdir) != 0)
  {
    fprintf(stderr, "Failed to remove temporary directory `%s'.\n", tmpdir);
    errors++;
  }

  printf("Ran %d test. %d passed. %d failed.\n",
    nt_passed + nt_failed, nt_passed, nt_failed);

  exit(errors + nt_failed);
}
