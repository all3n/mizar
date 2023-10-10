#include "mz_sockets.h"
#include "utils.h"

int socket_create(Socket *s) {
  s->sockfd = socket(AF_INET, SOCK_STREAM, 0);
  return s->sockfd;
}
int socket_connect(Socket *socket, const char *server_ip, int server_port) {
  memset(&(socket->addr), 0, sizeof(socket->addr));
  socket->addr.sin_family = AF_INET;
  socket->addr.sin_port = htons(server_port);
  if (inet_pton(AF_INET, server_ip, &(socket->addr.sin_addr)) <= 0) {
    return -2;
  }
  return connect(socket->sockfd, (struct sockaddr *)&(socket->addr),
                 sizeof(socket->addr));
}
int socket_bind(Socket *socket, int port) {
  memset(&(socket->addr), 0, sizeof(socket->addr));
  socket->addr.sin_family = AF_INET;
  socket->addr.sin_port = htons(port);
  socket->addr.sin_addr.s_addr = htonl(INADDR_ANY);
  return bind(socket->sockfd, (struct sockaddr *)&(socket->addr),
              sizeof(socket->addr));
}

int socket_listen(Socket *socket, int backlog) {
  return listen(socket->sockfd, backlog);
}

int socket_accept(Socket *server_socket, Socket *client_socket) {
  socklen_t client_addr_len = sizeof(client_socket->addr);
  client_socket->sockfd =
      accept(server_socket->sockfd, (struct sockaddr *)&(client_socket->addr),
             &client_addr_len);
  return client_socket->sockfd;
}

ssize_t socket_send(Socket *socket, const char *data) {
  return send(socket->sockfd, data, strlen(data), 0);
}

ssize_t socket_receive(Socket *socket, char *buffer, size_t buffer_size) {
  return recv(socket->sockfd, buffer, buffer_size - 1, 0);
}

void socket_close(Socket *socket) { close(socket->sockfd); }

void socket_pool_init(SocketPool *pool) {
  pool->count = 0;
  pthread_mutex_init(&(pool->mutex), NULL);
}

void socket_pool_add(SocketPool *pool, int sockfd, struct sockaddr_in addr) {
  pthread_mutex_lock(&(pool->mutex));
  if (pool->count < MAX_CLIENTS) {
    Socket *socket = &(pool->sockets[pool->count]);
    socket->sockfd = sockfd;
    memcpy(&(socket->addr), &addr, sizeof(struct sockaddr_in));
    pool->count++;
  }
  pthread_mutex_unlock(&(pool->mutex));
}

void socket_pool_remove(SocketPool *pool, int index) {
  pthread_mutex_lock(&(pool->mutex));
  if (index >= 0 && index < pool->count) {
    Socket *socket = &(pool->sockets[index]);
    socket_close(socket);
    if (index < pool->count - 1) {
      memmove(&(pool->sockets[index]), &(pool->sockets[index + 1]),
              (pool->count - index - 1) * sizeof(Socket));
    }
    pool->count--;
  }
  pthread_mutex_unlock(&(pool->mutex));
}
