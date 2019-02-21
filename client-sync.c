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


#define MAX_CONS 1024
#define MAX_BUF_SIZE 40960
int requests = 1e4;
int cons = 10;
int buf_size = 4096;
int mistakes = 0;

int socks[MAX_CONS];
pthread_t tids[MAX_CONS];
int con_num = 0;

void* work(void* arg)
{
    int sockfd = *(int*)arg;
    int req = requests;
    int send_len, recv_len;
    char buf[MAX_BUF_SIZE];

    for (int i = 0; i < buf_size; i += 4)
    {   *((int*)(buf + i)) = 0x12345678;
    }
    
    //while ((req = --requests) > 0)
    while (req-- > 0)
    {
        //printf("req: %d\n", req);

        send_len = send(sockfd, buf, buf_size, 0);

        recv_len = recvfull(sockfd, buf, buf_size, 0);
 
        //printf("%d %d\n", recv_len, *(int*)buf);
        if  (*(int*)buf != 0x12345678)  ++mistakes;
    }
    
    return NULL;
}


int main(int argc, char** argv)
{
    struct hostent* he;
    struct sockaddr_in their_addr;

    if  (argc < 3)  error("usage: client <hostname> <request num> [<thread num> <buf size>]");
    sscanf(argv[2], "%d", &requests);
    printf("client %s %d\n", argv[1], requests);

    if  ((he = gethostbyname(argv[1])) == NULL)  error("gethostbyname");

    if  (argc >= 4)  sscanf(argv[3], "%d", &cons);
    if  (argc >= 5)  sscanf(argv[4], "%d", &buf_size);
    
    
    for (int i = 0; i < cons; i++)
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

    for (int i = 0; i < cons; i++)
    {
        pthread_join(tids[i], NULL);
    }

    printf("mistakes: %d\n", mistakes);

    return 0;
    
}

