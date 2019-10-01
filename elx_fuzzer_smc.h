#ifndef _ELX_FUZZER_SMC_H_
#define _ELX_FUZZER_SMC_H_



struct elx_args_s {
  uint32_t num_of_args;
  uint64_t __user *args;
};

struct smc_args_s {
  uint64_t value;
  uint64_t size;
};

#endif /* _ELX_FUZZER_SMC_H_  */
