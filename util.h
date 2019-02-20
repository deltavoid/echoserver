#ifndef UTIL_H
#define UTIL_H
#include <stdio.h>
#include <stdlib.h>


#define PORT  3490
#define MAX_CONS 1024
#define BUF_SIZE 16

void error(const char* msg)
{
    printf("%s\n", msg);
    exit(1);
}


#endif //UTIL_H