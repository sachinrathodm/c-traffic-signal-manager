//
//  signalManager.c
//  Signal
//
//  Created by Sachin Rathod on 03/05/21.
//  Copyright © 2021 Sachin Rathod. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>

#include "signalManager.h"

int connfd[2];

void* connectClient(void* arg)
{
    int connfd1 = *((int *)arg);//file descreapter passed through server send
    int client0 = connfd1==connfd[0]?0:1; //changes
    int client1 = client0==0?1:0;
    char buf[MAXLINE];
    int n;
    while((n=read(connfd[client0], buf, MAXLINE))!=0)//collect no of vehicles one client to server and server write to another client
    {
        // printf("%s - read from %d\n", buf,connfd1);//server
        if(connfd[client1]!=-1)//second signal not connected
            write(connfd[client1], buf, MAXLINE);//random number write  to sever to client
        else
        {
            char temp[10];
            sprintf(temp,"N-%d\n",rand()%10+1);
            // printf("%s send to %d\n", temp,connfd1);
            write(connfd1,temp,10);
        }
    }
    close(connfd[client0]);
    connfd[client0]=-1;   // sigpipe
}

void* writeClient(void* arg)//emergency wait input
{
    int connfd1 = *((int *)arg);
    int signal_num,side;
    while(1)
    {
        printf("Enter Signal Number(1/2): ");
        scanf("%d",&signal_num);
        printf("Enter Road Side(Left-1,Right-2,Up-3,Down-4): ");
        scanf("%d",&side);
        signal_num--;
        side--;
        if(connfd[signal_num]!=-1)
        {
            char temp[4];
            sprintf(temp,"E-%d",side);
            write(connfd[signal_num], temp, 5);
        }
    }
}

int main(int argc, char **argv)
{
    int listenfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    char client_hostname[MAXLINE], client_port[MAXLINE];
    listenfd = open_Serverfd(argv[1]);//setup connection for accept
    connfd[0]=-1;
    connfd[1]=-1;
    pthread_t thread1;
    for(int i=0;i<2;i++)//accept for connection to the client
    {
        printf("Waiting for a new Client to connect\n");
        clientlen = sizeof(struct sockaddr_storage);
        connfd[i] = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);
        getnameinfo((struct sockaddr *) &clientaddr, clientlen,client_hostname, MAXLINE, client_port, MAXLINE, 0);
        printf("Connected to (%s, %s)\n", client_hostname, client_port);
        printf("Start Communication with new Client\n");
        pthread_create (&thread1, NULL,connectClient,(void *)&connfd[i]);//for 2 client
        if(i==0)
            pthread_create (&thread1, NULL,writeClient,(void *)&connfd[i]);//sigle time run
    }
    pthread_exit(0);
}

