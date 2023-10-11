#include "socks5.h"
#include "utils/mz_service.h"
#include "utils/mz_sockets.h"
#include "utils/utils.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
void BuildRequestData(Socks5Request *request, char *buffer, int *len) {
  int cnt = 0;
  buffer[cnt++] = request->version;
  buffer[cnt++] = request->command;
  buffer[cnt++] = request->rsv;
  buffer[cnt++] = request->atyp;
  switch (request->atyp) {
  case kAddrTypeIPv4:
    memcpy(buffer + cnt, &request->dst_addr.ipv4, 4);
    cnt += 4;
    break;
  case kAddrTypeDomain:
    memcpy(buffer + cnt, request->dst_addr.domain,
           strlen(request->dst_addr.domain));
    cnt += strlen(request->dst_addr.domain);
    break;
  case kAddrTypeIPv6:
    memcpy(buffer + cnt, &request->dst_addr.ipv6, 16);
    cnt += 16;
    break;
  }
  memcpy(buffer + cnt, &request->dst_port, 8);
  cnt += 8;
  *len = cnt;
}
void DebugRequest(Socks5Request *request) {
  printf("socks{");
  printf("version:%d,", request->version);
  printf("command:");
  switch (request->command) {
    case (uint8_t)kCmdConnect:
      printf("kCommandConnect");
      break;
    case (uint8_t)kCmdBind:
      printf("kCommandBind");
      break;
    case (uint8_t)kCmdUdpAssociate:
      printf("kCommandUDPAssociate");
      break;
    default:
      printf("unknown");
      break;
  }
  printf(",rsv:%d,", request->rsv);
  printf("atyp:%d, addr:", request->atyp);
  char buffer[256] = {0};
  switch (request->atyp) {
  case kAddrTypeIPv4:
    ipv4_to_string(request->dst_addr.ipv4, buffer, 256);
    printf("dst_addr.ipv4: %s,",  buffer);
    break;
  case kAddrTypeDomain:
    printf("dst_addr.domain: %s,", request->dst_addr.domain);
    break;
  case kAddrTypeIPv6:
    printf("dst_addr.ipv6: %s,", request->dst_addr.ipv6);
    break;
  }
  printf("dst_port: %d}\n", request->dst_port);
}
void DebugResponse(Socks5Response *response) {
  printf("sockets response:{");
  printf("version: %d,", response->version);
  printf("reply: %d,", response->reply);
  printf("rsv: %d,", response->rsv);
  printf("atyp: %d,", response->atyp);
  switch (response->atyp) {
  case kAddrTypeIPv4:
    printf("dst_addr.ipv4: %d,", response->dst_addr.ipv4);
    break;
  case kAddrTypeDomain:
    printf("dst_addr.domain: %s,", response->dst_addr.domain);
    break;
  case kAddrTypeIPv6:
    printf("dst_addr.ipv6: %s,", response->dst_addr.ipv6);
    break;
  }
  printf("dst_port: %d}\n", response->dst_port);
}
void DebugMethodSelect(Socks5MethodSelect *request) {
  printf("MethodRequest{version: %d,", request->version);
  printf("nmethods: %d,", request->nmethods);
  printf("methods:[");
  for (int i = 0; i < request->nmethods; i++) {
    printf("%d", request->methods[i]);
    if (i != request->nmethods - 1) {
      printf(",");
    }
  }
  printf("]}\n");
}
void ipv4_to_string(uint32_t ipv4, char *str, int len) {
  snprintf(str, len, "%d.%d.%d.%d", (ipv4 >> 24) & 0xff, (ipv4 >> 16) & 0xff,
          (ipv4 >> 8) & 0xff, ipv4 & 0xff);
}
static void ReadSocks5Request(Socks5Request *request, Socket *s) {
  socket_receive(s, (char *)&request->version, 1);
  socket_receive(s, (char *)&request->command, 1);
  socket_receive(s, (char *)&request->rsv, 1);
  socket_receive(s, (char *)&request->atyp, 1);
  switch (request->atyp) {
  case kAddrTypeIPv4:
    socket_receive(s, (char *)&request->dst_addr.ipv4, 4);
    break;
  case kAddrTypeDomain:
    socket_receive(s, (char *)request->dst_addr.domain, 256);
    break;
  case kAddrTypeIPv6:
    socket_receive(s, (char *)&request->dst_addr.ipv6, 16);
    break;
  }
  socket_receive(s, (char *)&request->dst_port, 2);
  // 2 bytes
  request->dst_port = ntohs(request->dst_port);
}
static void BuildSocks5Response(Socks5Response *response, char *buffer, size_t len) {
  int idx = 0;
  buffer[idx++] = response->version;
  buffer[idx++] = response->reply;
  buffer[idx++] = response->rsv;
  buffer[idx++] = response->atyp;

}
void *socks5_client_thread(void *arg) {
  ThreadArg *thread_arg = (ThreadArg *)arg;
  SocketPool *pool = thread_arg->pool;
  int index = thread_arg->index;
  Socket client_socket = pool->sockets[index];
  Socket remote_socket;
  char buffer[BUFFER_SIZE];
  memset(buffer, 0, sizeof(buffer));
  
  printf("Client connected: %s\n", inet_ntoa(client_socket.addr.sin_addr));
  ssize_t bytes_received = socket_receive(&client_socket, buffer, 2);
  if (bytes_received < 0) {
    printf("Client disconnected: %s\n", inet_ntoa(client_socket.addr.sin_addr));
    return NULL;
  }
  if (buffer[0] != 0x05) {
    XLOG(ERROR, "SOCKS5 Version: %d\n", buffer[0]);
    return NULL;
  }
  XLOG(INFO, "%d %d\n", buffer[0], buffer[1]);
  Socks5MethodSelect *method_select =
      malloc(sizeof(Socks5MethodSelect) + buffer[1]);
  method_select->version = buffer[0];
  method_select->nmethods = buffer[1];
  int len = socket_receive(&client_socket, (char *)method_select->methods,
                           method_select->nmethods);
  if (len != method_select->nmethods) {
    XLOG(ERROR, "SOCKS5 Method Select: %d", len);
    return NULL;
  }
  DebugMethodSelect(method_select);
  XLOG(INFO, "socks5 method select: %d", method_select->methods[0]);

  buffer[0] = method_select->version;
  buffer[1] = method_select->methods[0];
  socket_nsend(&client_socket, buffer, 2);

  Socks5Request request;
  Socks5Response response;
  bool running = true;
  while (running) {
    memset(&request, 0, sizeof(Socks5Request));
    memset(&response, 0, sizeof(Socks5Response));
    ReadSocks5Request(&request, &client_socket);
    DebugRequest(&request);
    
    int ret = socket_create(&remote_socket);
    if(ret == -1){
      XLOG(ERROR, "SOCKS5 Create Fail");
      response.reply = kReplyGeneralFailure;
      running = false;
    }else{
      memset(&remote_socket.addr, 0, sizeof(remote_socket.addr));
      remote_socket.addr.sin_addr.s_addr = request.dst_addr.ipv4;
      remote_socket.addr.sin_family = AF_INET;
      remote_socket.addr.sin_port = htons(request.dst_port);
      ret = socket_connect(&remote_socket, NULL, request.dst_port);
      if(ret == -1){
        response.reply = kReplyConnectionRefused;
        XLOG(ERROR, "SOCKS5 Connect Fail");
        running = false;
      }
    }
    response.version = request.version;
    response.reply = kReplySuccess;
    response.rsv = 0;
    response.atyp = request.atyp;
    memcpy(&response.dst_addr, &request.dst_addr, sizeof(request.dst_addr));
    response.dst_port = htons(request.dst_port);
    BuildSocks5Response(&response,buffer, BUFFER_SIZE);
    DebugResponse(&response);
    int bLen = 0;
    while(1){
      bLen = socket_receive(&client_socket, buffer, BUFFER_SIZE);
      socket_nsend(&remote_socket, buffer, bLen);
      // TODO
    }
  }
  free(method_select);
  XLOG(INFO, "Client disconnected: %s", inet_ntoa(client_socket.addr.sin_addr));
  socket_close(&client_socket);
  socket_pool_remove(pool, index);
  return NULL;
}
void run_socks5(MzOpts *opts) {
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
      pthread_create(&tid, NULL, socks5_client_thread, (void *)&thread_arg);
      pthread_detach(tid);
    }
  }
  socket_close(&server_socket);
}
