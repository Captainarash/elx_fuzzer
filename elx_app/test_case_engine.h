#ifndef TEST_CASE_ENGINE_H
#define TEST_CASE_ENGINE_H

#include <stdint.h>

struct elx_test_case_s {
  int tc_magic;
  uint32_t num_of_args;
  uint64_t args[0];     //symbolic member which contains an array of TLVs
}__attribute__((__packed__));

struct elx_app_smc_args_s {
  uint64_t type;
  uint64_t length;
  uint64_t value[1];
};

#define MAX_SCM_ARGS     10U
#define MAX_ELX_SCM_ARGS (MAX_SCM_ARGS + 1)
#define TLV_MIN_SIZE     (sizeof())
#define ELX_MAGIC        (0x00584c45)
#define MAX_BUF_SIZE     (0x10000)
#define TYPE_INT         (0x1)
#define TYPE_BUF         (0x0)  //not used really

int get_tc(const char *filename);
size_t get_fsize(int tc_fd);
size_t get_remaining_fbytes(int tc_fd);
void start_fuzzing(int tc_fd, int mutate);

static inline int check_magic(struct elx_test_case_s *tc)
{
  if (tc->tc_magic != ELX_MAGIC) {
    return -1;
  }
  return 0;
}

#endif
