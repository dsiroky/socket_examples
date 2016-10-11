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
  char *hostname;
  struct hostent *hostinfo;
  struct sockaddr_in addr;
  char msg[FULL_MSG_SIZE];

  if (argc < 2)
  {
    printf("usage: cli hostaddr\n");
    exit(1);
  }

  hostname = argv[1];

  /* create socket */
  sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  /* get IP */
  hostinfo = gethostbyname(hostname);

  /* prepare inet address */
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(SRV_PORT);
  addr.sin_addr = *(struct in_addr *) hostinfo->h_addr;

  /* send 2 HALF packets at once */
  msg[0] = 'a';
  sendto(sock, msg, HALF_MSG_SIZE, 0, (struct sockaddr *)&addr, sizeof(addr));
  msg[0] = 'b';
  sendto(sock, msg, HALF_MSG_SIZE, 0, (struct sockaddr *)&addr, sizeof(addr));

  usleep(500000);

  /* send 1 FULL packet */
  msg[0] = 'a';
  msg[HALF_MSG_SIZE] = 'b';
  sendto(sock, msg, FULL_MSG_SIZE, 0, (struct sockaddr *)&addr, sizeof(addr));

  close(sock);
  return 0;
}

