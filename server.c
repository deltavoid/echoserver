
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "util.h"




int socks[MAX_CONS];
pthread_t tids[MAX_CONS];
int con_num = 0;


typedef void* (*ThreadEntry)(void* arg);


void* work(void* arg)
{
    int sockfd = *(int*)arg;
    int recv_len, send_len;
    char buf[BUF_SIZE];

    while (1)
    {
        recv_len = recv(sockfd, buf, BUF_SIZE, 0);
        if  (recv_len == 0)  break;

        send_len = send(sockfd, buf, recv_len, 0);
    }
    
    return NULL;
}




#define BACKLOG 10
int listenfd;

int main()
{
    int newfd;
    struct sockaddr_in my_addr;
    struct sockaddr_in their_addr;
    int sin_size;

    if  ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)  error("socket");

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(PORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(my_addr.sin_zero), sizeof(my_addr.sin_zero));

    if  (bind(listenfd, (struct sockaddr*)&my_addr, sizeof(struct sockaddr)) == -1)  error("bind");

    if  (listen(listenfd, BACKLOG) == -1)  error("listen");

    while (1)
    {
        sin_size = sizeof(struct sockaddr_in);

        if  ((socks[con_num] = accept(listenfd, (struct sockaddr*)&their_addr, (socklen_t*)&sin_size)) == -1)  error("accept");
        
        printf("connection %d from %s\n", con_num, inet_ntoa(their_addr.sin_addr));

        if  (pthread_create(&tids[con_num], NULL, work, &socks[con_num]) == -1)  error("pthread_create");

        con_num++;

        if  (con_num >= MAX_CONS)  error("too many connections");
    }



    return 0;
}


