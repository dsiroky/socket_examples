#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "common.h"

/****************************************************************************/

void err(char *msg)
{
  printf("error (%i, %s): %s\n", errno, strerror(errno), msg);
  exit(1);
}

