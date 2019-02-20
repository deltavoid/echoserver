#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include "util.h"



int requests = 1e4;
int threads = 10;
int mistakes = 0;

int socks[MAX_CONS];
pthread_t tids[MAX_CONS];
int con_num = 0;

void* work(void* arg)
{
    int sockfd = *(int*)arg;
    int req;
    int send_len, recv_len;
    char buf[BUF_SIZE];

    for (int i = 0; i < BUF_SIZE; i += 4)
    {   *((int*)(buf + i)) = 0x12345678;
    }
    
    while ((req = --requests) > 0)
    {
        //printf("req: %d\n", req);

        send_len = send(sockfd, buf, BUF_SIZE, 0);

        recv_len = recvfull(sockfd, buf, BUF_SIZE, 0);
 
        //printf("%d %d\n", recv_len, *(int*)buf);
        if  (*(int*)buf != 0x12345678)  ++mistakes;
    }
    
    return NULL;
}


int main(int argc, char** argv)
{
    struct hostent* he;
    struct sockaddr_in their_addr;

    if  (argc < 3)  error("usage: client <hostname> <request num> [<thread num>]");
    sscanf(argv[2], "%d", &requests);
    printf("client %s %d\n", argv[1], requests);

    if  ((he = gethostbyname(argv[1])) == NULL)  error("gethostbyname");

    if  (argc >= 4)  sscanf(argv[3], "%d", &threads);
    
    
    for (int i = 0; i < threads; i++)
    {
        if  ((socks[con_num] = socket(AF_INET, SOCK_STREAM, 0)) == -1)  error("socket");

        their_addr.sin_family = AF_INET;
        their_addr.sin_port = htons(PORT);
        their_addr.sin_addr = *((struct in_addr*)he->h_addr);
        bzero(&(their_addr.sin_zero), sizeof(their_addr.sin_zero));

        if  (connect(socks[con_num], (struct sockaddr*)&their_addr, sizeof(struct sockaddr)) == -1)  error("connect");

        if  (pthread_create(&tids[con_num], NULL, work, &socks[con_num]) == -1)  error("pthread_create");

        con_num++;

        if  (con_num >= MAX_CONS)  error("too many connections");

    }

    for (int i = 0; i < threads; i++)
    {
        pthread_join(tids[i], NULL);
    }

    printf("mistakes: %d\n", mistakes);

    return 0;
    
}

