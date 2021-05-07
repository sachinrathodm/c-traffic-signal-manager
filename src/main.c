//
//  main.c
//  Signal
//
//  Created by Sachin Rathod on 03/05/21.
//  Copyright © 2021 Sachin Rathod. All rights reserved.
//

//using Concepts used
//signal
//thread
//process
//pipes
//Network

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <netdb.h>

#include "main.h"

unsigned int count[4];
pthread_t signal_thread[4];
int clientfd;


void* CreateSignal(void* p)
{
    //current thread accept
    unsigned int cur=(unsigned int)pthread_self();
    //  printf("Thread(%u) started\n",cur);
    int index=0;
    for (int i=0; i<3; ++i)
    {
        if(cur==(unsigned int)signal_thread[i])
        {
            index=i;
            break;
        }
    }
    count[index]=*((int *)p);
    while(1)
    {
        count[index]+=rand()%2+1;//random value add to down signal
        sleep(2);
    }
}

//Sigusr1
void Green_Signal()//current process take random and delete
{
    unsigned int curr=(unsigned int)pthread_self();
    //  printf("[(%u)] \n",curr);
    for (int i=0; i<4; ++i)
    {
        if(curr==(unsigned int)signal_thread[i])
        {
            int temp=(rand()%count[i])+1;
            count[i]-=temp;
            char t[10];
            sprintf(t,"N-%d",rand()%temp);
            write(clientfd,t,10);//send number of vehicle to server
        }
    }
    sleep(3);
}

//SigUsr2
void Red_Signal()
{
    sleep(0);
}

void* readFromController(void* arg)
{
    int clientfd = *((int*)arg);//
    char buf[MAXLINE];
    int n;
    while(1)
    {
        read(clientfd, buf, MAXLINE);//read from server
        char temp;
        int c;
        sscanf(buf,"%c-%d",&temp,&c);
        if(temp=='E') //emergncy
        {
            printf("Signal Received\n");
            switch(c+1)
            {
                case 1:
                    printf("Left");
                    break;
                case 2:
                    printf("Right");
                    break;
                case 3:
                    printf("Up");
                    break;
                case 4:
                    printf("Down");
                    break;
            }
            printf(" is ON\n");
            
            for (int i = 0; i < 4; ++i)
            {
                if(i==c)
                    pthread_kill(signal_thread[i],SIGUSR1);//green signal
                else
                    pthread_kill(signal_thread[i],SIGUSR2);//red signal
            }
        }
        else
            count[3]+=c;
        // sleep(1);
    }
    close(clientfd);
}

int main(int argc, const char * argv[])
{
    int randomnumber;
    signal(SIGUSR1, Green_Signal);
    signal(SIGUSR2, Red_Signal);
    signal(SIGPIPE,SIG_IGN); //signal pipe error handling
    //pthread_t printingThread;
    
    clientfd = open_Clientfd("localhost", argv[1]); //client fd for talking with server
    for (int i=0; i<3; ++i)//three threads for random
    {
        randomnumber=rand() % 10 + 1;
        count[i]=randomnumber;
        pthread_create(&signal_thread[i], NULL, CreateSignal,(void *)&randomnumber);
        sleep(0.5);
        //printf()
        // pthread_kill(signal_thread[i], SIGUSR2);
    }
    pthread_create(&signal_thread[3],NULL,readFromController,(void* )&clientfd);//server will send random number in this thread file descriptor
    
    // pthread_create(&printingThread,NULL,getCounts,NULL);
    int fd[2];// local
    pipe(fd);
    if(fork()==0)//child //no of vehicle in each signal
    {
        close(fd[1]);//write close
        char temp[20];
        char * VehicleSide[4];
        while(1)
        {
            int n = read(fd[0],temp,20);
            if(n==0)//when data not come
                break;
            char * sides = strtok(temp, "-");
            int i=0;
            while( sides != NULL ) {
                VehicleSide[i]=sides;
                ++i;
                sides = strtok(NULL, "-");
            }
            
            printf("\n----------------------\n");
            printf("Signal Sides\n");
            printf("----------------------\n");
            printf("Left side  - %s\n", VehicleSide[0]);
            printf("Right side - %s\n", VehicleSide[1]);
            printf("Up side - %s\n", VehicleSide[2]);
            printf("Down side - %s\n", VehicleSide[3]);
            printf("----------------------\n");
            printf("\n");
        }
        exit(0);
    }
    else //parent //line by line green signa to each signal and send data to child each signal
    {
        close(fd[0]);//read close
        int i=0;
        while(1)
        {
            pthread_kill(signal_thread[i],SIGUSR1);
            // printf("%u signal called from main\n", (unsigned int )signal_thread[i]);
            system("clear");
            switch(i+1)
            {
                case 1:
                    printf("Left");
                    break;
                case 2:
                    printf("Right");
                    break;
                case 3:
                    printf("Up");
                    break;
                case 4:
                    printf("Down");
                    break;
            }
            printf(" is ON");
            printf("\n\n");
            
            char temp[20];
            sprintf(temp,"%d-%d-%d-%d",count[0],count[1],count[2],count[3]);//
            // printf("%s\n", temp);
            // temp[strlen(temp)-1]='\0';
            write(fd[1],temp,20);//send count to the child
            sleep(3);
            pthread_kill(signal_thread[i],SIGUSR2);
            i++;
            if(i==4)//wrap around
                i=0;
        }
    }
    
    return 0;
}
