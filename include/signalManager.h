//
//  signalManager.h
//  Signal
//
//  Created by Sachin Rathod on 06/05/21.
//  Copyright © 2021 Sachin Rathod. All rights reserved.
//

#define    MAXLINE     8192  /* Max text line length */
#define LISTENQ  2  /* Second argument to listen() */

int open_Serverfd(char *);
void* connectClient(void*);
void* writeClient(void*);

//server connection side
int open_Serverfd(char *port)
{
    struct addrinfo hints, *listp, *p;
    int listenfd, optval=1;
    char host[MAXLINE],service[MAXLINE];
    int flags;
    
    
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
    hints.ai_flags |= AI_NUMERICSERV;
    getaddrinfo(NULL, port, &hints, &listp);
    
    
    for (p = listp; p; p = p->ai_next)
    {
        if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
            continue;
        
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
                   (const void *)&optval , sizeof(int));
        
        flags = NI_NUMERICHOST | NI_NUMERICSERV;
        getnameinfo(p->ai_addr, p->ai_addrlen, host, MAXLINE, service, MAXLINE, flags);
        printf("host:%s, service:%s\n", host, service);
        
        
        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
            break;
        
        close(listenfd);
    }
    
    freeaddrinfo(listp);
    if (!p) /* No address worked */
        return -1;
    
    if (listen(listenfd, LISTENQ) < 0) {
        close(listenfd);
        return -1;
    }
    return listenfd;
}
