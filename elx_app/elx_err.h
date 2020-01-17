#ifndef ELX_ERR_H
#define ELX_ERR_H
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define err_exit(msg) do {perror(msg); exit(EXIT_FAILURE);} while (0);
#endif
