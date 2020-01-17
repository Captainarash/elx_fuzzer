#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "test_case_engine.h"
#include "elx_err.h"

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
    err_exit("null pointer caught while parsing tlv.\n");
  }

  if (tlv->length > MAX_BUF_SIZE || tlv->length > rem_sz) {
    err_exit("can't allocate more than 16 pages!\n");
  }

  buf = (char *)malloc(tlv->length);
  if (!buf) {
    err_exit("Out of memory: can't parse tlv.\n");
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
    err_exit("test case is invalid: too small!\n");
  }

  tc = (struct elx_test_case_s *)tc_buf;

  if (check_magic(tc)) {
    err_exit("test case is invalid: wrong magic value!\n");
  }

  if (tc->num_of_args > MAX_ELX_SCM_ARGS) {
    err_exit("test case is invalid: too many arguments!\n");
  }

  if (tc->num_of_args == 0) {
    err_exit("test case is invalid: zero arguments!\n");
  }

  args_sz = tc_sz - sizeof(tc->tc_magic) - sizeof(tc->num_of_args);
  rem_sz = args_sz;

  if (args_sz < tc->num_of_args * sizeof(struct elx_app_smc_args_s)) {
    err_exit("test case is invalid: args_size is less that minimum allowed\n");
  }

  final_buf = (uint64_t *)malloc(tc->num_of_args * sizeof(uint64_t) * 2);
  if (!final_buf) {
    err_exit("Out of memory!\n");
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


  tc_sz = get_fsize(tc_fd);
  if (!tc_sz) {
    err_exit("test case is empty? can't do fuzzing with an empty test case\n");
  }

  tc_buf = (char *)malloc(tc_sz);
  if (!tc_fd) {
    err_exit("test case too large!\n");
  }

  read(tc_fd, tc_buf, tc_sz);

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
