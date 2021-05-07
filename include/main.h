//
//  main.h
//  Signal
//
//  Created by Sachin Rathod on 06/05/21.
//  Copyright © 2021 Sachin Rathod. All rights reserved.
//
#define MAXLINE 8192

//client connection side
int open_Clientfd(char *hostname, char *port)
{
    int clientfd;
    struct addrinfo hints, *listp, *p;
    char host[MAXLINE],service[MAXLINE];
    int flags;
    
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV;
    hints.ai_flags |= AI_ADDRCONFIG;
    getaddrinfo(hostname, port, &hints, &listp);
    
    
    for (p = listp; p; p = p->ai_next)
    {
        if ((clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
            continue;
        
        flags = NI_NUMERICHOST | NI_NUMERICSERV;
        getnameinfo(p->ai_addr, p->ai_addrlen, host, MAXLINE, service, MAXLINE, flags);
        printf("host:%s, service:%s\n", host, service);
        
        if (connect(clientfd, p->ai_addr, p->ai_addrlen) != -1)
        {
            printf("Connected to server %s at port %s\n", host, service);
            break;
        }
        close(clientfd);
    }
    
    
    freeaddrinfo(listp);
    if (!p)
        return -1;
    else
        return clientfd;
}
