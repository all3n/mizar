#ifndef _MZ_HTTP_H
#define _MZ_HTTP_H
#include <stdint.h>
// Accept-Ranges: bytes
// HTTP/1.1 200 OK
// Cache-Control: private, no-cache, no-store, proxy-revalidate, no-transform
// Connection: keep-alive
// Content-Length: 2381
typedef struct HttpHeader {
  char *name;
  char *value;
  struct HttpHeader *next;
} HttpHeader;
typedef struct HttpHeaders {
  struct HttpHeader *head;
  struct HttpHeader *tail;
  uint32_t len;
} HttpHeaders;

typedef struct HttpResponse {
  uint8_t version;
  HttpHeaders headers;
  uint32_t body_len;
  char *body;
} HttpResponse;
void AddHeader(HttpHeaders *headers, char *name, char *value);
void CleanHeaders(HttpHeaders *headers);
void PrintHeaders(HttpHeaders *headers);
#define CRLF "\r\n"
#endif
