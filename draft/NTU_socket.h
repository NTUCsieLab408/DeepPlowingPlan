/*
 * implement basic function of socket
 * for general purpose
 */

#ifndef NTUSOCKETH
#define NTUSOCKETH

#include <arpa/inet.h>
#define NUM_LISTEN 30   /* limit the number of accepted client */

int my_listen(unsigned short port);
int my_accept(int listen_sockfd);
int my_connect(const char *ip, unsigned short port);
void my_close(int sockfd);

size_t my_send(int sockfd, const void *buf, size_t len);
size_t my_recv(int sockfd, void *buf, size_t len);

#endif
