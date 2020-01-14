#ifndef TEST_CASE_ENGINE_H
#define TEST_CASE_ENGINE_H


int get_tc(const char *filename);
size_t get_fsize(int tc_fd);
size_t get_remaining_fbytes(int tc_fd);


#endif
