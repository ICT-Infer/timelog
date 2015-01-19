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
  char* desc; /* Description. */
  char** argv;
  bool xsuccess; /* Expecting success (true) or failure (false)? */
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

  tltest tests[24];

  tests[0].desc = "`tl init'";
  char* argv_init[3] = {tlb, "init", NULL};
  tests[0].argv = argv_init;
  tests[0].xsuccess = true;

  tests[1].desc = "`tl init' where `tl init' has been done";
  tests[1].argv = argv_init; /* Reusing argv_init. */
  tests[1].xsuccess = false;

  tests[2].desc = "`tl' without arguments";
  char* argv_noarg[2] = {tlb, NULL};
  tests[2].argv = argv_noarg;
  tests[2].xsuccess = false;

  tests[3].desc = "`tl x' (invalid command)";
  char* argv_inv[3] = {tlb, "x", NULL};
  tests[3].argv = argv_inv;
  tests[3].xsuccess = false;

  tests[4].desc = "`tl timepoint'";
  char* argv_tp[3] = {tlb, "timepoint", NULL};
  tests[4].argv = argv_tp;
  tests[4].xsuccess = true;

  tests[5].desc = "`tl pending'";
  char* argv_pend[3] = {tlb, "pending", NULL};
  tests[5].argv = argv_pend;
  tests[5].xsuccess = true;

  tests[6].desc = "`tl pop-drop'";
  char* argv_pd[3] = {tlb, "pop-drop", NULL};
  tests[6].argv = argv_pd;
  tests[6].xsuccess = true;

  tests[7].desc = "`tl pop-drop' with empty stack";
  tests[7].argv = argv_pd; /* Reusing argv_pd. */
  tests[7].xsuccess = false;

  tests[8].desc = "`tl merge-add' with empty stack";
  char* argv_tpma[3] = {tlb, "merge-add", NULL};
  tests[8].argv = argv_tpma;
  tests[8].xsuccess = false;

  tests[9].desc = "`tl timepoint -l <loc>' with valid length loc";
  char* argv_tpvl[5] = {tlb, "timepoint", "-l", "xxxxxxxxxxxxxxxxxy", NULL};
  tests[9].argv = argv_tpvl;
  tests[9].xsuccess = true;

  tests[10].desc = "`tl timepoint -l <loc>' with invalid length loc";
  char* argv_tpil[5] = {tlb, "timepoint", "-l", "xxxxxxxxxxxxxxxxxyz", NULL};
  tests[10].argv = argv_tpil;
  tests[10].xsuccess = false;

  tests[11].desc = "`tl timepoint -m <msg>' with valid length msg";
  char* argv_tpvm[5] = {tlb, "timepoint", "-m", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxy",
    NULL};
  tests[11].argv = argv_tpvm;
  tests[11].xsuccess = true;

  tests[12].desc = "`tl timepoint -m <msg>' with invalid length msg";
  char* argv_tpim[5] = {tlb, "timepoint", "-m", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxyz",
    NULL};
  tests[12].argv = argv_tpim;
  tests[12].xsuccess = false;

  tests[13].desc = "`tl timepoint -t <ts>' with valid ts #1";
  char* argv_tpvts1[5] = {tlb, "timepoint", "-t", "22:22", NULL};
  tests[13].argv = argv_tpvts1;
  tests[13].xsuccess = true;

  tests[14].desc = "`tl timepoint -t <ts>' with valid ts #2";
  char* argv_tpvts2[5] = {tlb, "timepoint", "-t", "2015-01-10T22:22", NULL};
  tests[14].argv = argv_tpvts2;
  tests[14].xsuccess = true;

  tests[15].desc = "`tl timepoint -t <ts>' with invalid ts #1";
  char* argv_tpits1[5] = {tlb, "timepoint", "-t", "T22:22", NULL};
  tests[15].argv = argv_tpits1;
  tests[15].xsuccess = false;

  tests[16].desc = "`tl timepoint -t <ts>' with invalid ts #2";
  char* argv_tpits2[5] = {tlb, "timepoint", "-t", "22:22:00", NULL};
  tests[16].argv = argv_tpits2;
  tests[16].xsuccess = false;

  tests[17].desc = "`tl timepoint -t <ts>' with invalid ts #3";
  char* argv_tpits3[5] = {tlb, "timepoint", "-t", "2015-01-10t22:22", NULL};
  tests[17].argv = argv_tpits3;
  tests[17].xsuccess = false;

  tests[18].desc = "`tl timepoint -t <ts>' with invalid ts #4";
  char* argv_tpits4[5] = {tlb, "timepoint", "-t", "2014-02-31T22:22", NULL};
  tests[18].argv = argv_tpits4;
  tests[18].xsuccess = false;

  tests[19].desc = "`tl merge-add' with multiple points on stack";
  tests[19].argv = argv_tpma; /* Reusing argv_tpma. */
  tests[19].xsuccess = true;

  tests[20].desc = "`tl unlog' 2nd log entry with only one entry in log";
  char* argv_uls[4] = {tlb, "unlog", "2", NULL};
  tests[20].argv = argv_uls;
  tests[20].xsuccess = false;

  tests[21].desc = "`tl unlog' 1st log entry";
  char* argv_ulf[4] = {tlb, "unlog", "1", NULL};
  tests[21].argv = argv_ulf;
  tests[21].xsuccess = true;

  tests[22].desc = "`tl unlog' 1st log entry again";
  tests[22].argv = argv_ulf; /* Reusing argv_ulf. */
  tests[22].xsuccess = false;

  tests[23].desc = "`tl report'";
  char* argv_rep[3] = {tlb, "report", NULL};
  tests[23].argv = argv_rep;
  tests[23].xsuccess = true;

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
