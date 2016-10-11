#ifndef __COMMON_H
#define __COMMON_H

#define SRV_PORT          23456
#define MAX_TXT_LENGTH    1000

void err(char *msg);
int send_txt(int sock, char *txt);
int recv_txt(int sock);
void send_greetings(int sock);

#endif
