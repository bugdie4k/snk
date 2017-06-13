#ifndef _NONBLOCK_H
#define _NONBLOCK_H

#include <stdio.h>   
#include <stdlib.h>  
#include <unistd.h>  
#include <termios.h>

int kbhit();
void nonblock();
void test_nonblock();

#endif
