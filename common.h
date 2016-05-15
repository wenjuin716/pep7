#include <stdio.h>
#include <ctype.h>
 
#ifndef HEXDUMP_COLS
#define HEXDUMP_COLS 8
#endif

/******** Common debug function *********/
void hexdump(void *mem, unsigned int len);
