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

  if (argc < 2)
  {
    printf("usage: cli hostaddr\n");
    exit(1);
  }

  hostname = argv[1];

  /* create socket */
  sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock < 0) err("socket()");

  /* get IP */
  hostinfo = gethostbyname(hostname);
  if (hostinfo == NULL) err("gethostbyname");

  /* prepare inet address */
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(SRV_PORT);
  addr.sin_addr = *(struct in_addr *) hostinfo->h_addr;

  /* make TCP connection */
  printf("connecting to %s:%i\n", inet_ntoa(addr.sin_addr), SRV_PORT);
  if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) err("connect");

  if (!send_txt(sock, "hello!"))
  {
    close(sock);
    err("send hello");
  }

  if (!recv_txt(sock))
  {
    close(sock);
    err("recv greetings");
  }

  close(sock);
  return 0;
}

