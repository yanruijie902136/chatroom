#ifndef _UTILS_FDIO_H
#define _UTILS_FDIO_H

#include <unistd.h>

ssize_t readline(int, void *, size_t);
ssize_t writen(int, const void *, size_t);

#endif
