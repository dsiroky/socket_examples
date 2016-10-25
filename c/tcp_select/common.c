#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include "common.h"

/****************************************************************************/

void err(char *msg)
{
  printf("error (%i, %s): %s\n", errno, strerror(errno), msg);
  exit(1);
}

//--------------------------------------------------------------------------

void close_socket(int sock)
{
  // send TCP finalization
  shutdown(sock, SHUT_RDWR);
  close(sock);
}
