#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "test_case_engine.h"

int get_tc(const char *filename)
{
  int tc_fd = 0;
  if (access(filename, F_OK | R_OK) == -1) {
    return -1;
  }
  tc_fd = open(filename, O_RDONLY);
  return tc_fd;
}

size_t get_fsize(int tc_fd)
{
  size_t file_size, orig_off;
  orig_off = lseek(tc_fd, 0, SEEK_CUR);
  file_size = lseek(tc_fd, 0, SEEK_END);
  lseek(tc_fd, orig_off, SEEK_SET);
  return file_size;
}

size_t get_remaining_fbytes(int tc_fd)
{
  size_t cur_off = 0;
  size_t rem_size = 0;
  int file_size = 0;

  cur_off = lseek(tc_fd, 0L, SEEK_CUR);
  file_size = lseek(tc_fd, 0L, SEEK_END);

  rem_size = file_size - cur_off;

  lseek(tc_fd, cur_off, SEEK_SET);

  return rem_size;
}
