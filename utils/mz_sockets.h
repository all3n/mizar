#ifndef _MZ_SOCKETS_H
#define _MZ_SOCKETS_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

typedef struct {
  int sockfd;
  struct sockaddr_in addr;
} Socket;

typedef struct {
  Socket sockets[MAX_CLIENTS];
  int count;
  pthread_mutex_t mutex;
} SocketPool;

typedef struct {
  SocketPool *pool;
  int index;
} ThreadArg;

#define ERR_EXIT(func, msg)                                                    \
  do {                                                                         \
    if ((func) == -1) {                                                        \
      XLOG(ERROR, msg);                                                        \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
  } while (0)
#define ERR_BREAK(func, msg)                                                   \
  if ((func) == -1) {                                                          \
    XLOG(ERROR, msg);                                                          \
    break;                                                                     \
  }

// server socket
// bind -> listen -> accept
int socket_bind(Socket *socket, int port);
int socket_listen(Socket *socket, int backlog);
int socket_accept(Socket *server_socket, Socket *client_socket);
// client socket
// connect -> send -> recv
int socket_connect(Socket *socket, const char *server_ip, int server_port);

// common
int socket_create(Socket *s);
ssize_t socket_send(Socket *socket, const char *data);
ssize_t socket_receive(Socket *socket, char *buffer, size_t buffer_size);
void socket_close(Socket *socket);

// pool
void socket_pool_init(SocketPool *pool);
void socket_pool_add(SocketPool *pool, int sockfd, struct sockaddr_in addr);
void socket_pool_remove(SocketPool *pool, int index);

#endif
