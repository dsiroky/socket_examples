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
  struct sockaddr_in mcast_addr;
  int group;

  if (argc < 2)
  {
    printf("usage: ./publisher group(0-255)\n");
    exit(1);
  }

  group = atoi(argv[1]);

  /* create socket */
  sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sock < 0) err("socket()");

  /* prepare address */
  memset(&mcast_addr, 0, sizeof(mcast_addr));
  mcast_addr.sin_family = AF_INET;
  mcast_addr.sin_port = htons(PORT);
  /* multicast prefix is 224.0.0.0/4, we will use 230.0.0.0/24 block */
  mcast_addr.sin_addr.s_addr = htonl(230 << 24 | group);

  /* go! */
  if (sendto(sock, "a", 1, 0, (struct sockaddr *)&mcast_addr, sizeof(mcast_addr)) < 0)
  {
    close(sock);
    err("send");
  }

  close(sock);
  return 0;
}

