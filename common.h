#include <stdio.h>
#include <ctype.h>
 
#ifndef HEXDUMP_COLS
#define HEXDUMP_COLS 8
#endif

void hexdump(void *mem, unsigned int len);
