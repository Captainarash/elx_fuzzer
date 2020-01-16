#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

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

uint64_t parse_tlv_buf(struct elx_app_smc_args_s *tlv, uint64_t rem_sz)
{
  char *buf;
  if (!tlv) {
    printf("null pointer caught while parsing tlv\n");
    exit(0);
  }

  if (tlv->length > MAX_BUF_SIZE || tlv->length > rem_sz) {
    printf("can't allocate more than 16 pages!\n");
    exit(0);
  }

  buf = (char *)malloc(tlv->length);
  if (!buf) {
    printf("Out of memory: can't parse tlv\n");
    exit(0);
  }
  memcpy(buf, (void *)tlv->value, tlv->length);
  return (uint64_t)buf;
}

char *serialize_tc(char *tc_buf, size_t tc_sz)
{
  uint64_t *final_buf;
  struct elx_test_case_s *tc;
  size_t args_sz, rem_sz;
  struct elx_app_smc_args_s *curr_tlv;

  if (tc_sz < sizeof(struct elx_test_case_s)) {
    printf("test case is invalid: too small!\n");
    exit(0);
  }

  tc = (struct elx_test_case_s *)tc_buf;

  if (check_magic(tc)) {
    printf("test case is invalid: wrong magic value!\n");
    exit(0);
  }

  if (tc->num_of_args > MAX_ELX_SCM_ARGS) {
    printf("test case is invalid: too many arguments!\n");
    exit(0);
  }

  if (tc->num_of_args == 0) {
    printf("test case is invalid: zero arguments!\n");
    exit(0);
  }

  args_sz = tc_sz - sizeof(tc->tc_magic) - sizeof(tc->num_of_args);
  rem_sz = args_sz;

  if (args_sz < tc->num_of_args * sizeof(struct elx_app_smc_args_s)) {
    printf("test case is invalid: args_size is less that minimum allowed\n");
    exit(0);
  }

  final_buf = (uint64_t *)malloc(tc->num_of_args * sizeof(uint64_t) * 2);
  if (!final_buf) {
    printf("Out of memory!\n");
    exit(0);
  }

  curr_tlv = (struct elx_app_smc_args_s *)(tc->args);
  for (size_t i = 0; i < tc->num_of_args * 2; i += 1) {
    if (curr_tlv->type == TYPE_INT) {
      final_buf[i] = curr_tlv->value[0];
      final_buf[i+1] = 0;
      curr_tlv++;
      rem_sz -= sizeof(struct elx_app_smc_args_s);
    }
    else {
      final_buf[i] = parse_tlv_buf(curr_tlv, rem_sz);
      final_buf[i+1] = curr_tlv->length;
      curr_tlv++; //skip this tlv header
      //then skip the buffer (remember tlv->length also includes 8 bytes for value field!!!)
      curr_tlv = ((struct elx_app_smc_args_s *)((char *)curr_tlv + curr_tlv->length - sizeof(uint64_t)));
      rem_sz -= (sizeof(struct elx_app_smc_args_s) + curr_tlv->length - sizeof(uint64_t));
    }
  }
  return (char *)final_buf;
}

void start_fuzzing(int tc_fd, int mutate)
{
  size_t tc_sz;
  char *tc_buf;
  char *driver_buf;
  int read_retried = 0;


  tc_sz = get_fsize(tc_fd);
  if (!tc_sz) {
    printf("test case is empty? can't do fuzzing with an empty test case\n");
    return;
  }

  tc_buf = (char *)malloc(tc_sz);
  if (!tc_fd) {
    printf("test case too large!\n");
    return;
  }

  while (read(tc_fd, tc_buf, tc_sz) != tc_sz && !read_retried) {
    printf("reading the test case failed, trying once more...\n");
    read_retried = 1;
  }

  do {
    /** Parse the test case and fill the buffer needed by the driver.
     ** Then, pass it to the driver. If no crash, mutate it, save and retry
     **/
    driver_buf = serialize_tc(tc_buf, tc_sz);
    if (driver_buf != 0) {
      /** call the driver **/
    }
    free(driver_buf);
    //mutate_tc(tc_buf, tc_sz);
  } while (mutate > 0);
}
