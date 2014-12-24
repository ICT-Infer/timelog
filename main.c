#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <db_185.h>
#include <fcntl.h>
#include <limits.h>

int main (int argc, char *argv[])
{
  char* f = "tl_default.db";
  DB *db = dbopen(f, O_CREAT | O_RDWR, 0, DB_HASH, NULL);
  if (db == NULL)
  {
    fprintf(stderr, "dbopen failed.\n");
    exit(EXIT_FAILURE);
  }
}
