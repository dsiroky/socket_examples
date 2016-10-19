#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/select.h>
#include <sys/ioctl.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "common.h"

/****************************************************************************/

#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

#define SOCKET_MAX 1024

#define NOT_CONNECTED (-1)

/****************************************************************************/

char *generate_large_message()
{
  char *buf;

  buf = malloc(LARGE_MESSAGE_SIZE);
  if (buf == NULL) err("generate malloc");

  memset(buf, 'a', LARGE_MESSAGE_SIZE);
  return buf;
}

/**************************************************************************/

void process_accept(int listen_sock, fd_set *master_fds, int *fdmax,
                    int *sock_to_offset)
{
  int incoming_sock;
  struct sockaddr_in incoming_addr;
  unsigned int incoming_addr_len;

  /* accept all new connections */
  for (;;) {
    incoming_addr_len = sizeof(incoming_addr);
    incoming_sock = accept(listen_sock, (struct sockaddr*)&incoming_addr,
                      &incoming_addr_len);
    if (incoming_sock < 0)
    {
      /* if errno == EWOULDBLOCK ... not waiting incoming connections
       *                             and accept would block*/
      //                            see a note near select()
      if (errno != EWOULDBLOCK)
      {
        close(listen_sock);
        err("accept");
      }
      break;
    }
    printf("connection from %s:%i\n",
              inet_ntoa(incoming_addr.sin_addr),
              ntohs(incoming_addr.sin_port)
        );

    sock_to_offset[incoming_sock] = 0;

    FD_SET(incoming_sock, master_fds);
    if (incoming_sock > *fdmax) *fdmax = incoming_sock;
  };
}

/**************************************************************************/

void process_send(int sock, fd_set *master_fds, int *fdmax, char *reply,
                        int *sock_to_offset)
{
  int len;
  int send_size;

  send_size = MIN(SEND_SIZE, LARGE_MESSAGE_SIZE - sock_to_offset[sock]);
  len = send(sock, reply + sock_to_offset[sock], send_size, 0);
  if (len < 0)
  {
    // == EWOULDBLOCK: system might be still full, see a note near select()
    if (errno != EWOULDBLOCK)
    {
      printf("recv error (%i, %s)\n", errno, strerror(errno));
      // TODO close and release the socket
      return;
    }
  }
  sock_to_offset[sock] += len;
  printf("socket %i, %i bytes sent, %i to go\n", sock, len,
        LARGE_MESSAGE_SIZE - sock_to_offset[sock]);

  /* all data sent */
  if (sock_to_offset[sock] == LARGE_MESSAGE_SIZE)
  {
    close(sock);

    /* remove the socket from the master set */
    FD_CLR(sock, master_fds);

    sock_to_offset[sock] = NOT_CONNECTED;

    /* clear the tail of the master set */
    if (sock == *fdmax)
      while (!FD_ISSET(*fdmax, master_fds)) *fdmax -= 1;
  }
}

/**************************************************************************/

void process_recv(int sock, fd_set *master_fds, int *fdmax, char *reply,
                        int *sock_to_offset)
{
  char buf[RECV_BUFFER_SIZE];
  int recv_len;

  recv_len = recv(sock, buf, RECV_BUFFER_SIZE, 0);
  printf("socket %i, %i bytes received\n", sock, recv_len);
  if (recv_len == 0)
  {
    printf("closed connection\n");
    return;
  } else if (recv_len < 0)
  {
    // == EWOULDBLOCK: there might not be data, see a note near select()
    if (errno != EWOULDBLOCK)
    {
      printf("recv error (%i, %s)\n", errno, strerror(errno));
      return;
    }
  }

  /* initiate reply */
  process_send(sock, master_fds, fdmax, reply, sock_to_offset);
}

/**************************************************************************/

int main(int argc, char **argv)
{
  fd_set master_fds; /* master file descriptor list */
  fd_set read_fds; /* temp file descriptor list for read events */
  fd_set write_fds; /* temp file descriptor list for write events */

  int listen_sock, fdmax;
  int optval;
  int i;
  struct sockaddr_in addr;
  char *reply;

  /* this should be implemented as a generic map socket:offset,
    -1 ... not connected, >=0 ... amount of sent bytes */
  int sock_to_offset[SOCKET_MAX];

  /**********/

  for (i = 0; i < SOCKET_MAX; i++) sock_to_offset[i] = NOT_CONNECTED;

  reply = generate_large_message();

  /* clear the master and temp sets */
  FD_ZERO(&master_fds);
  FD_ZERO(&read_fds);
  FD_ZERO(&write_fds);

  /* create socket */
  listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (listen_sock < 0) err("socket()");

  /* non-blocking operation */
  optval = 1;
  if (ioctl(listen_sock, FIONBIO, (char *)&optval) < 0)
  {
    close(listen_sock);
    err("ioctl nonblock");
  }

  /* set reusable flag */
  optval = 1;
  setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

  /* prepare inet address */
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(SRV_PORT);
  addr.sin_addr.s_addr = htonl(INADDR_ANY); /* listen on all interfaces */
  if (bind(listen_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) err("bind");

  if (listen(listen_sock, 10) < 0) err("listen");

  /* add the listener to the master set */
  FD_SET(listen_sock, &master_fds);
  /* keep track of the biggest file descriptor */
  fdmax = listen_sock; /* so far, it's this one*/

  for(;;)
  {
    /* copy it */
    memcpy(&read_fds, &master_fds, sizeof(master_fds));
    memcpy(&write_fds, &master_fds, sizeof(master_fds));

    /* wait for any action */
    if (select(fdmax + 1, &read_fds, &write_fds, NULL, NULL) == -1)
    {
      close(listen_sock);
      err("select");
    }
    // NOTE: select might return with non-zero bits even if there is no event
    // pending (accept/recv/send might block)
    printf("select is OK\n");

    for (i = 0; i <= fdmax; i++)
    {
      if (FD_ISSET(i, &read_fds))
      {
        /* the socket is the listening socket */
        if (i == listen_sock) process_accept(listen_sock, &master_fds, &fdmax,
                                            sock_to_offset);
        /* otherwise it is a connection socket ready for reading */
        else process_recv(i, &master_fds, &fdmax, reply, sock_to_offset);
      }
      if (FD_ISSET(i, &write_fds)) {
        /* socket is ready to send data */
        process_send(i, &master_fds, &fdmax, reply, sock_to_offset);
      }
    }
  }

  return 0;
}

