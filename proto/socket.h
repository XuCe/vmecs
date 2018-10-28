#ifndef _PROTO_SOCKET_H_
#define _PROTO_SOCKET_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>

#include "pub/type.h"

typedef int socket_t;

// read/write with retry
INLINE ssize_t read_r(int fd, byte_t *buf, size_t size)
{
    ssize_t ret;
    while ((ret = read(fd, buf, size)) == -1 && errno == EINTR)
        perror("read");
    return ret;
}

INLINE ssize_t write_r(int fd, const byte_t *buf, size_t size)
{
    ssize_t ret;
    while ((ret = write(fd, buf, size)) == -1 && errno == EINTR)
        perror("write");
    return ret;
}

/** Returns true on success, or false if there was an error */
INLINE int socket_set_block(int fd, bool blocking)
{
   int flags = fcntl(fd, F_GETFL, 0);
   if (flags == -1) return -1;
   flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
   return fcntl(fd, F_SETFL, flags);
}

#endif