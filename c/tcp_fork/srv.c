#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "common.h"

/**************************************************************************/

int main(int argc, char **argv)
{
  // avoid SIGPIPE signals on writing to a closed connection
  signal(SIGPIPE, SIG_IGN);

  int sock, incoming_sock;
  int optval;
  struct sockaddr_in addr, incoming_addr;
  unsigned int incoming_addr_len;

  /* create socket */
  sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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

  if (listen(sock, 10) < 0) err("listen");

  for(;;)
  {
    incoming_addr_len = sizeof(incoming_addr);
    incoming_sock = accept(sock, (struct sockaddr*)&incoming_addr, 
                      &incoming_addr_len);
    if (incoming_sock < 0)
    {
      close_socket(sock);
      err("accept");
    }
    printf("connection from %s:%i\n",
              inet_ntoa(incoming_addr.sin_addr),
              ntohs(incoming_addr.sin_port)
        );
    if (fork() == 0) 
    {
      send_greetings(incoming_sock);
      close_socket(incoming_sock);
      break;
    }
  }

  return 0;
}

