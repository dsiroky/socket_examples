#ifndef __COMMON_H
#define __COMMON_H

#define SRV_PORT 23456
#define LARGE_MESSAGE_SIZE 300000
#define RECV_BUFFER_SIZE (32 * 1024)
#define SEND_SIZE (64 * 1024)

void err(char *msg);

#endif
