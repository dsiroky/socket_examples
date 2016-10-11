#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "common.h"

/****************************************************************************/

int main(int argc, char **argv)
{
  int sock;
  char *sometext;
  struct sockaddr_in listen_addr, bcast_addr, incoming_addr;
  int optval;
  struct timeval tv;

  if (argc < 2)
  {
    printf("usage: cli sometext\n");
    exit(1);
  }

  sometext = argv[1];

  /* create socket */
  sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sock < 0) err("socket()");

  /* socket timeout */
  tv.tv_sec = 3;
  tv.tv_usec = 0;
  setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

  /* set broadcast flag */
  optval = 1;
  setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval));

  /* prepare receiving address */
  memset(&listen_addr, 0, sizeof(listen_addr));
  listen_addr.sin_family = AF_INET;
  listen_addr.sin_port = htons(CLI_PORT);
  listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(sock, (struct sockaddr*)&listen_addr, sizeof(listen_addr)) < 0) err("bind");

  /* prepare broadcast address */
  memset(&bcast_addr, 0, sizeof(bcast_addr));
  bcast_addr.sin_family = AF_INET;
  bcast_addr.sin_port = htons(SRV_PORT);
  bcast_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
  /* bcast_addr.sin_addr.s_addr = inet_addr("255.255.255.255"); */

  if (!send_txt(sock, &bcast_addr, sometext))
  {
    close(sock);
    err("send");
  }

  if (!recv_txt(sock, &incoming_addr))
  {
    close(sock);
    err("recv");
  }
  printf("received from %s\n", inet_ntoa(incoming_addr.sin_addr));

  close(sock);
  return 0;
}

