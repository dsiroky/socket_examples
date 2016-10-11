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
  int sock, recv_len;
  int optval;
  struct sockaddr_in addr;
  char msg[FULL_MSG_SIZE];

  /* create socket */
  sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  /* set reusable flag */
  optval = 1;
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

  /* prepare inet address */
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(SRV_PORT);
  addr.sin_addr.s_addr = htonl(INADDR_ANY); /* listen on all interfaces */
  bind(sock, (struct sockaddr*)&addr, sizeof(addr));
  
  /* get the first packet data - even if we try read FULL we will get only 
   * the first HALF packet */
  recv_len = recv(sock, msg, FULL_MSG_SIZE, 0);
  printf("recv full, len=%i (full=%i)\n", recv_len, FULL_MSG_SIZE);
  /* get the second HALF packet */
  recv(sock, msg, HALF_MSG_SIZE, 0); 

  /* read first half of the FULL packet */
  recv_len = recv(sock, msg, HALF_MSG_SIZE, 0);
  printf("recv half, len=%i\n", recv_len);
  /* try to read second half of the FULL packet, which is lost */
  recv_len = recv(sock, msg, HALF_MSG_SIZE, MSG_DONTWAIT);
  printf("recv half, len=%i\n", recv_len);

  return 0;
}

