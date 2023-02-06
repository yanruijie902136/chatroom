#include "fdio.h"
#include <errno.h>

ssize_t readline(int fd, void *vbuf, size_t maxlen) {
    ssize_t n = 0, rc;
    char c, *ptr = vbuf;

    while (n < maxlen) {
        if ( (rc = read(fd, &c, 1)) == 1) {
            n++;
            if (c == '\n')
                break;
            *ptr++ = c;
        } else if (rc == 0)
            break;
        else
            return (-1);
    }
    *ptr = 0;
    return (n);
}

ssize_t writen(int fd, const void *vbuf, size_t n) {
    size_t nleft = n;
    ssize_t nwritten;
    const char *ptr = vbuf;

    while (nleft > 0) {
        if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
            if (nwritten < 0 && errno == EINTR)
                nwritten = 0;
            else
                return (-1);
        }
        ptr += nwritten;
        nleft -= nwritten;
    }
    return (n);
}
