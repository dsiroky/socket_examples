#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include "common.h"

/****************************************************************************/

void err(char *msg)
{
  printf("error (%i, %s): %s\n", errno, strerror(errno), msg);
  exit(1);
}

/****************************************************************************/

/** @return 1 if OK, 0 otherwise */
int send_txt(int sock, struct sockaddr_in *addr, char *txt)
{
  printf("sending: %s\n", txt);
  if (sendto(sock, txt, strlen(txt) + 1, 0, 
              (struct sockaddr*)addr, sizeof(*addr)) < 0)
    return 0;
  return 1;
}

/****************************************************************************/

/** @return 1 if OK, 0 otherwise */
int recv_txt(int sock, struct sockaddr_in *incoming_addr)
{
  char buf[MAX_TXT_LENGTH];
  unsigned int incoming_addr_len;

  incoming_addr_len = sizeof(*incoming_addr);
  if (recvfrom(sock, buf, MAX_TXT_LENGTH, 0, 
                (struct sockaddr*)incoming_addr, &incoming_addr_len) < 0) return 0;
  printf("received: %s\n", buf);
  return 1;
}

