#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "test_case_engine.h"

int get_test_case(const char *filename)
{
  int tc_fd = 0;
  if (access(filename, F_OK | R_OK) == -1) {
    return -1;
  }
  tc_fd = open(filename, O_RDONLY);
  return tc_fd;
}
