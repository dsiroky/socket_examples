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
    recv_txt(sock, &incoming_addr);
    if (fork() == 0) 
    {
      printf("data from %s:%i\n",
              inet_ntoa(incoming_addr.sin_addr),
              ntohs(incoming_addr.sin_port)
        );
      send_txt(sock, &incoming_addr, "welcome!");
      break;
    }
  }

  return 0;
}

