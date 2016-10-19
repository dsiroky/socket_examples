#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//==========================================================================

#define SRV_PORT 22434

//--------------------------------------------------------------------------

void print_err(char* msg)
{
  printf("error (%i, %s): %s\n", errno, strerror(errno), msg);
}

//--------------------------------------------------------------------------

/// @return 1 if OK, 0 on error
int echo(int sock)
{
  char buffer[100];
  ssize_t recv_count = 0, send_count = 0;

  recv_count = recv(sock, buffer, sizeof(buffer), 0);
  if (recv_count <= 0)
  {
    print_err("recv");
    return 0;
  }

  // artificial delay to show GUI asynchonicity
  usleep(1000000);

  send_count = send(sock, buffer, recv_count, 0);
  if (send_count < 0)
  {
    print_err("send");
    return 0;
  }

  return 1;
}

//--------------------------------------------------------------------------

int main(int argc, char** argv)
{
  int sock = -1, incoming_sock = -1;
  int optval = -1;
  struct sockaddr_in addr, incoming_addr;
  unsigned int incoming_addr_len = 0;

  /* create socket */
  sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock < 0)
  {
    print_err("socket()");
    return 1;
  }

  /* set reusable flag */
  optval = 1;
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

  /* prepare inet address */
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(SRV_PORT);
  addr.sin_addr.s_addr = htonl(INADDR_ANY); /* listen on all interfaces */
  if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
  {
    print_err("bind");
    return 1;
  }

  if (listen(sock, 10) < 0)
  {
    print_err("listen");
    return 1;
  }

  for(;;)
  {
    incoming_addr_len = sizeof(incoming_addr);
    incoming_sock = accept(sock, (struct sockaddr*)&incoming_addr,
                            &incoming_addr_len);
    if (incoming_sock < 0)
    {
      print_err("accept");
      close(sock);
      continue;
    }

    printf("connection from %s:%i\n",
              inet_ntoa(incoming_addr.sin_addr),
              ntohs(incoming_addr.sin_port)
          );
    while (echo(incoming_sock)) {}
    close(incoming_sock);
  }

  return 0;
}

