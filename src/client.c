// client.c
#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <utils/fdio.h>

int main(int argc, char *argv[]) {
    int sockfd;
    char *p, buff[8192];
    ssize_t n;
    struct sockaddr_in servaddr;

    if (argc != 2 || (p = strchr(argv[1], ':')) == NULL) {
        (void)fprintf(stderr, "usage: ./client <server IP>:<server port>\n");
        exit(EXIT_FAILURE);
    }
    *p++ = 0;
    setbuf(stdout, NULL);

    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err(EXIT_FAILURE, "socket");

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons((uint16_t)atoi(p));
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
        errx(EXIT_FAILURE, "inet_pton error for %s", argv[1]);

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        err(EXIT_FAILURE, "connect");

    printf("Please enter your username: ");
    if (fgets(buff, sizeof(buff), stdin) == NULL) {
        if (ferror(stdin))
            err(EXIT_FAILURE, "fgets");
        exit(EXIT_SUCCESS);
    }
    if (writen(sockfd, buff, strlen(buff)) < 0)
        err(EXIT_FAILURE, "writen");

    for ( ; ; ) {
        printf("> ");
        if (fgets(buff, sizeof(buff), stdin) == NULL)
            break;
        if (writen(sockfd, buff, strlen(buff)) < 0)
            err(EXIT_FAILURE, "writen");
    }
    if (ferror(stdin))
        err(EXIT_FAILURE, "fgets");

    exit(EXIT_SUCCESS);
}
