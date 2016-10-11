#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "common.h"

/**************************************************************************/

int main(int argc, char **argv)
{
  int sock;
  int optval;
  struct sockaddr_in addr, incoming_addr;
  socklen_t addr_len;
  struct ip_mreq mreq;
  char buf;
  int group;

  if (argc < 2)
  {
    printf("usage: ./subscriber group(0-255)\n");
    exit(1);
  }

  group = atoi(argv[1]);

  /* create socket */
  sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sock < 0) err("socket()");

  /* set reusable flag */
  optval = 1;
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

  /* prepare inet address */
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = htonl(INADDR_ANY); /* listen on all interfaces */
  if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) err("bind");

  /* join the multicast group */
  mreq.imr_multiaddr.s_addr = htonl(230 << 24 | group);
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);
  if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
    err("setsockopt membership");

  for(;;)
  {
    addr_len = sizeof(incoming_addr);
    recvfrom(sock, &buf, 1, 0, (struct sockaddr *)&incoming_addr, &addr_len);
    if (fork() == 0) 
    {
      printf("data from %s:%i\n",
              inet_ntoa(incoming_addr.sin_addr),
              ntohs(incoming_addr.sin_port)
        );
      break;
    }
  }

  return 0;
}

