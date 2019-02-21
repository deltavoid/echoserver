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
//int send_req, recv_req;
int cons = 10;
int buf_size = 4096;
int mistakes = 0;

int socks[MAX_CONS];
pthread_t senders[MAX_CONS], recvers[MAX_CONS];
int con_num = 0;



// void* work(void* arg)
// {
//     int sockfd = *(int*)arg;
//     int req;
//     int send_len, recv_len;
//     char buf[MAX_BUF_SIZE];

//     for (int i = 0; i < buf_size; i += 4)
//     {   *((int*)(buf + i)) = 0x12345678;
//     }
    
//     while ((req = --requests) > 0)
//     {
//         //printf("req: %d\n", req);

//         send_len = send(sockfd, buf, buf_size, 0);

//         recv_len = recvfull(sockfd, buf, buf_size, 0);
 
//         //printf("%d %d\n", recv_len, *(int*)buf);
//         if  (*(int*)buf != 0x12345678)  ++mistakes;
//     }
    
//     return NULL;
// }


void* sender(void* arg)
{
    int sockfd = *(int*)arg;
    int req = requests;
    int send_len;
    char buf[MAX_BUF_SIZE];

    for (int i = 0; i < buf_size; i += 4)
    {   *((int*)(buf + i)) = 0x12345678;
    }
    
    while (req-- > 0)
    {
        //printf("sender: %d\n", req);
        send_len = sendfull(sockfd, buf, buf_size, 0);
    }

    return NULL;
}

void* recver(void* arg)
{
    int sockfd = *(int*)arg;
    int req = requests;
    int recv_len;
    char buf[MAX_BUF_SIZE];

    while (req-- > 0)
    {
        //printf("recver: %d\n", req);
        recv_len = recvfull(sockfd, buf, buf_size, 0);


        if  (*(int*)buf != 0x12345678)  ++mistakes;
    }

    return NULL;
}


int main(int argc, char** argv)
{
    struct hostent* he;
    struct sockaddr_in their_addr;

    if  (argc < 3)  error("usage: client <hostname> <request num> [<connection num> <buf size>]");

    if  ((he = gethostbyname(argv[1])) == NULL)  error("gethostbyname");

    sscanf(argv[2], "%d", &requests);
    //send_req = requests;  recv_req = requests;
    printf("client %s %d\n", argv[1], requests);


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

        //if  (pthread_create(&tids[con_num], NULL, work, &socks[con_num]) == -1)  error("pthread_create");
        if  (pthread_create(&senders[con_num], NULL, sender, &socks[con_num]) == -1)  error("pthread_create sender");
        if  (pthread_create(&recvers[con_num], NULL, recver, &socks[con_num]) == -1)  error("pthread_create recver");

        con_num++;

        if  (con_num >= MAX_CONS)  error("too many connections");

    }

    for (int i = 0; i < cons; i++)
    {
        //pthread_join(tids[i], NULL);
        pthread_join(senders[i], NULL);
        pthread_join(recvers[i], NULL);
    }

    printf("mistakes: %d\n", mistakes);

    return 0;
    
}

