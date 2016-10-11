#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "common.h"

/**************************************************************************/

typedef struct {
    char buf[MAX_TXT_LENGTH];
    int data_len;
    int sock;
    struct sockaddr_in incoming_addr;
  } data_t;

/**************************************************************************/

void *handler(void *param)
{
  data_t *data = param;
  printf("received: %s\n", data->buf);
  send_txt(data->sock, &(data->incoming_addr), "welcome!");
  free(data);
  return NULL;
}

/**************************************************************************/

int main(int argc, char **argv)
{
  int sock;
  int optval;
  unsigned int incoming_addr_len;
  struct sockaddr_in addr;
  data_t *data;
  pthread_t thread;

  /* create socket */
  sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sock < 0) err("socket()");

  /* set reusable flag */
  optval = 1;
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

  /* prepare inet address */
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(SRV_PORT);
  addr.sin_addr.s_addr = htonl(INADDR_ANY); /* listen on all interfaces */
  if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) err("bind");

  for(;;)
  {
    data = malloc(sizeof(data_t));
    if (data == NULL) err("malloc");
    data->sock = sock;
    incoming_addr_len = sizeof(struct sockaddr_in);
    data->data_len = recvfrom(sock, data->buf, MAX_TXT_LENGTH, 0, 
                (struct sockaddr*)&(data->incoming_addr), &incoming_addr_len);
    if (data->data_len < 0) err("recv");

    if (pthread_create(&thread, NULL, handler, data) != 0) err("pthread_create");
  }

  return 0;
}

