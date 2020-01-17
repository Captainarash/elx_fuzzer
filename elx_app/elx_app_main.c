#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "test_case_engine.h"
#include "elx_err.h"

void help(void)
{
  printf("Help will be added here :D\n");
  return;
}

void usage(void)
{
  printf("Need at least 1 argument!\n");
  printf("Examples of usage:\n");
  printf("Do one test based on the input file:\t./elx_app -i <filename>\n");
  printf("Do X tests and mutate:\t\t\t./elx_app -i <filename> -m <X>\n");
  printf("Create a test case:\t\t\t./elx_app -c\n");
  printf("Create X test cases:\t\t\t./elx_app -c -m <X>\n");
  printf("For detailed info of the arguments:\t./elx_app -h\n");
  return;
}

int main(int argc, char **argv)
{
  int opt = 0;
  int in_fd = 0, out_fd = 0;
  int mutate = 0;
  char *in_file = 0;
  if (argc < 2) {
    usage();
  }

  while((opt = getopt(argc, argv, ":i:m:c:h")) != -1)
  {
    switch(opt) {
    case 'i':
      printf("testcase: %s\n", optarg);
      in_file = strdup(optarg);
      in_fd = get_tc(in_file);
      break;
    case 'm':
      printf("mutate: %s times\n", optarg);
      mutate = atoi(optarg);
      break;
    case 'c':
      printf("generating a test case: %s\n", optarg);
      //call test case generation logic
      break;
    case 'h':
      help();
      break;
    case ':':
      printf("option needs a value\n");
      usage();
      break;
    case '?':
      printf("unknown option: %c\n", optopt);
      usage();
      break;
    }
  }

  for(; optind < argc; optind++) {
    printf("invalid arguments: %s\n", argv[optind]);
    return -1;
  }

  if (in_fd) {
    start_fuzzing(in_fd, mutate);
  }

  if (out_fd) {
    // implement the logic to create a test case.
  }

  return 0;
}
