#include "http.h"
#include <stdio.h>
#include <stdlib.h>

void AddHeader(HttpHeaders *headers, char *name, char *value) {
  HttpHeader *header = malloc(sizeof(HttpHeader));
  header->name = name;
  header->value = value;
  header->next = NULL;
  if (headers->tail == NULL) {
    headers->head = header;
    headers->tail = header;
  } else {
    headers->tail->next = header;
    headers->tail = header;
  }
}
void PrintHeaders(HttpHeaders *headers) {
  HttpHeader *header = headers->head;
  while (header != NULL) {
    printf("%s: %s\n", header->name, header->value);
    header = header->next;
  }
}
void CleanHeaders(HttpHeaders *headers) {
  HttpHeader *header = headers->head;
  while (header != NULL) {
    free(header->name);
    free(header->value);
    header = header->next;
  }
  headers->head = NULL;
  headers->tail = NULL;
}
