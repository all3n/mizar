#include "mz_service.h"
#include "utils.h"
#include <string.h>
#include "../socks5.h"
void run_client(MzOpts *opts) {
  Socket client_socket;
  bool socket_connected = false;
  int ret = 0;
  int flag = 1;
  while (flag) {
    ERR_EXIT(socket_create(&client_socket), "socket create fail");
    if (socket_connected == false) {
      ret = socket_connect(&client_socket, opts->host, opts->port);
    }
    if (ret == 0) {
      socket_connected = true;
    } else {
      XLOG(WARNING, "connect to %s:%d fail:%d", opts->host, opts->port, ret);
      usleep(1000 * 1000);
      continue;
    }

    XLOG(INFO, "connect to %s:%d success", opts->host, opts->port);
    char buf[1024];
    while (socket_connected) {
      memset(buf, 0, sizeof(buf));
      printf("%s:%d> ", opts->host, opts->port);
      fgets(buf, sizeof(buf), stdin);
      char *l = strrchr(buf, '\n');
      if (l) {
        *l = '\0';
      }
      if (strcmp(buf, "exit") == 0) {
        flag = 0;
        break;
      }
      int sret = socket_send(&client_socket, buf);
      if (sret < 0) {
        socket_connected = false;
        break;
      }
      while (1) {
        memset(buf, 0, sizeof(buf));
        int rb = socket_receive(&client_socket, buf, sizeof(buf));
        if (rb < 0) {
          XLOG(ERROR, "socket_receive fail");
          socket_connected = false;
          break;
        }
        buf[rb] = '\0';
        if (buf[0] == '\1' && buf[1] == '\2') {
          break;
        }
        printf("%s", buf);
      }
      if (!socket_connected) {
        break;
      }
    }
    socket_close(&client_socket);
  }
}
void run_server(MzOpts *opts) {
  Socket server_socket;
  int server_port = opts->port;
  ERR_EXIT(socket_create(&server_socket), "Failed to create socket.");
  ERR_EXIT(socket_bind(&server_socket, server_port), "Failed to bind socket.");
  ERR_EXIT(socket_listen(&server_socket, 5), "Failed to listen on socket.");
  XLOG(INFO, "Listening on port %d", server_port);
  SocketPool socket_pool;
  socket_pool_init(&socket_pool);
  while (1) {
    Socket client_socket;
    socket_accept(&server_socket, &client_socket);
    if (socket_pool.count >= MAX_CLIENTS) {
      printf("Connection limit reached. Closing client socket.\n");
      socket_close(&client_socket);
    } else {
      socket_pool_add(&socket_pool, client_socket.sockfd, client_socket.addr);
      ThreadArg thread_arg;
      thread_arg.pool = &socket_pool;
      thread_arg.index = socket_pool.count - 1;
      pthread_t tid;
      pthread_create(&tid, NULL, client_thread, (void *)&thread_arg);
      pthread_detach(tid);
    }
  }
  socket_close(&server_socket);
}
void *client_thread(void *arg) {
  ThreadArg *thread_arg = (ThreadArg *)arg;
  SocketPool *pool = thread_arg->pool;
  int index = thread_arg->index;
  Socket client_socket = pool->sockets[index];
  char buffer[BUFFER_SIZE];
  printf("Client connected: %s\n", inet_ntoa(client_socket.addr.sin_addr));
  while (1) {
    ssize_t bytes_received =
        socket_receive(&client_socket, buffer, BUFFER_SIZE);
    if (bytes_received == 0) {
      break;
    }
    ERR_BREAK(bytes_received, "Failed to recv data to client.");
    buffer[bytes_received] = '\0';
    if (strcmp(buffer, "quit") == 0) {
      printf("Client disconnected: %s\n",
             inet_ntoa(client_socket.addr.sin_addr));
      break;
    }
    // char *cmd = strstr(buffer, "cmd:");
    char *cmd = buffer;
    if (cmd) {
      // printf("exec:%s\n", cmd);
      FILE *f = popen(cmd, "r");
      if (!f) {
        XLOG(ERROR, "Failed to execute cmd:%s", cmd);
        sprintf(buffer, "Failed to execute cmd:%s", cmd);
        ssize_t bytes_sent = socket_send(&client_socket, buffer);
        if (bytes_sent < 0) {
          break;
        }
      } else {
        // fputs(cmd, f);
        while (fgets(buffer, BUFFER_SIZE, f) != NULL) {
          ssize_t bytes_sent = socket_send(&client_socket, buffer);
          if (bytes_sent < 0) {
            break;
          }
        }
      }
      pclose(f);
      buffer[0] = '\1';
      buffer[1] = '\2';
      ssize_t bytes_sent = socket_send(&client_socket, buffer);
      if (bytes_sent < 0) {
        continue;
      }
    } else {
      printf("Received data from client: %s:%zd\n", buffer, bytes_received);
      ssize_t bytes_sent = socket_send(&client_socket, buffer);
      ERR_BREAK(bytes_sent, "Failed to send data to client.");
    }
  }
  XLOG(INFO, "Client disconnected: %s", inet_ntoa(client_socket.addr.sin_addr));
  socket_close(&client_socket);
  socket_pool_remove(pool, index);
  return NULL;
}

// extern SvcFunc run_socks5;
void run_svc(const char *name, MzOpts *opts) {
  MzSvc mzSvcs[] = {{"client", run_client},
                    {"server", run_server},
                    {"socks5", run_socks5},
                    {NULL, NULL}};
  if (name == NULL) {
    return;
  }
  for (int i = 0; mzSvcs[i].name != NULL; i++) {
    if (!strcasecmp(mzSvcs[i].name, name)) {
      XLOG(INFO, "Running service:%s", mzSvcs[i].name);
      mzSvcs[i].func(opts);
    }
  }
}
