#ifndef UTIL_H
#define UTIL_H
#include <stdio.h>
#include <stdlib.h>


#define PORT  3490
#define MAX_CONS 1024
#define BUF_SIZE 1024

void error(const char* msg)
{
    printf("%s\n", msg);
    exit(1);
}


int sendfull(int fd, const char* msg, int len, int flags) 
{
    int remaining = len;
    const char* cur = msg;
    int sent;

    while (remaining > 0) {
        if  ((sent = send(fd, cur, remaining, flags)) == -1)  error("send");
        cur += sent;
        remaining -= sent;
    }

    return (len - remaining);
}

int recvfull(int fd, char* msg, int len, int flags) {
    int remaining = len;
    char* cur = msg;
    int recvd;

    while (remaining > 0) {
        recvd = recv(fd, cur, remaining, flags);
        if ((recvd == -1) || (recvd == 0)) break;
        cur += recvd;
        remaining -= recvd;
    }

    return (len - remaining);
}


#endif //UTIL_H