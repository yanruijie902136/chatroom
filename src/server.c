// server.c
#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <err.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <utils/fdio.h>

struct user {
    int fd;
    char *name;
    struct sockaddr_in addr;
};

static void *threadfunc(void *);

int main(int argc, char *argv[]) {
    int listenfd, connfd, opt, error;
    pthread_t tid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    struct user *userp;

    if (argc != 2) {
        (void)fprintf(stderr, "usage: ./server <port>\n");
        exit(EXIT_FAILURE);
    }

    if ( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err(EXIT_FAILURE, "socket");

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons((uint16_t)atoi(argv[1]));

    opt = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (void *)&opt,
            sizeof(opt)) < 0)
        err(EXIT_FAILURE, "setsockopt");

    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        err(EXIT_FAILURE, "bind");

    if (listen(listenfd, 1024) < 0)
        err(EXIT_FAILURE, "listen");
    printf("start listening...\n");

    for ( ; ; ) {
        clilen = sizeof(cliaddr);
        if ( (connfd = accept(listenfd, (struct sockaddr *)&cliaddr,
                &clilen)) < 0) {
            if (errno == EINTR || errno == ECONNABORTED || errno == EPROTO)
                continue;
            err(EXIT_FAILURE, "accept");
        }

        if ( (userp = malloc(sizeof(struct user))) == NULL)
            err(EXIT_FAILURE, "malloc");
        userp->fd = connfd;
        memcpy(&userp->addr, &cliaddr, sizeof(cliaddr));

        if ( (error = pthread_create(&tid, NULL, threadfunc,
                (void *)userp)) != 0)
            errc(EXIT_FAILURE, error, "pthread_create");
        if ( (error = pthread_detach(tid)) != 0)
            errc(EXIT_FAILURE, error, "pthread_detach");
    }
}

static void *threadfunc(void *arg) {
    ssize_t n;
    char buff[8192];
    struct user *userp = (struct user *)arg;

    if ( (n = readline(userp->fd, buff, sizeof(buff))) < 0)
        err(EXIT_FAILURE, "readline");
    else if (n == 0) {
        if (close(userp->fd) < 0)
            err(EXIT_FAILURE, "close");
        free(userp);
        return ((void *)0);
    }
    
    if ( (userp->name = malloc(strlen(buff) + 1)) == NULL)
        err(EXIT_FAILURE, "malloc");
    strncpy(userp->name, buff, strlen(buff));
    printf("User %s has entered the chatroom.\n"
           "(socket fd = %d, IP = %s, port = %hu)\n",
            userp->name, userp->fd, inet_ntoa(userp->addr.sin_addr),
            ntohs(userp->addr.sin_port));

    while ( (n = readline(userp->fd, buff, sizeof(buff))) > 0)
        printf("%s: %s\n", userp->name, buff);
    if (n < 0)
        err(EXIT_FAILURE, "readline");

    printf("User %s has left the chatroom.\n", userp->name);
    if (close(userp->fd) < 0)
        err(EXIT_FAILURE, "close");
    free(userp->name);
    free(userp);
    return ((void *)0);
}
