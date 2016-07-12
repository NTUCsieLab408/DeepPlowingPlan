/*
 * implement basic function of socket
 * for general purpose
 */

#include <string.h>
#include <errno.h>
#include "NTU_socket.h"

int my_listen(unsigned short port) {
  int sockfd, optval = 1;
  struct sockaddr_in addr;

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) return -1;

  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, (const void *) &optval, sizeof (int)) < 0) {
    close(sockfd);
    return -1;
  }

  memset(&addr, 0, sizeof (struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);

  if (bind(sockfd, (struct sockaddr *) &addr, sizeof (struct sockaddr_in)) < 0) {
    close(sockfd);
    return -1;
  }

  if (listen(sockfd, NUM_LISTEN) < 0) {
    close(sockfd);
    return -1;
  }

  return sockfd;
}

int my_connect(const char *host, unsigned short port) {
  int sockfd;
  struct sockaddr_in addr;

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) return -1;

  memset(&addr, 0, sizeof (struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(host);
  addr.sin_port = htons(port);

  if (connect(sockfd, (struct sockaddr *) &addr, sizeof (struct sockaddr_in)) < 0) {
    close(sockfd);
    return -1;
  }

  return sockfd;
}

int my_accept(int listen_sockfd) {
  int sockfd;
  socklen_t addr_len;
  struct sockaddr_in addr;

  addr_len = sizeof (struct sockaddr_in);
  if ((sockfd = accept(listen_sockfd, (struct sockaddr *) &addr, &addr_len)) < 0) return -1;

  return sockfd;
}

size_t my_send(int sockfd, const void *buf, size_t len) {
  int n, m = 0;

  while (m < len) {
    n = send(sockfd, buf + m, len - m, MSG_NOSIGNAL);
    if (errno == EINTR) continue;
    if (n < 0) return -1;
    m += n;
  }

  return m;
}

size_t my_recv(int sockfd, void *buf, size_t len) {
  int n, m = 0;

  while (m < len) {
    n = recv(sockfd, buf + m, len - m, 0);
    if (errno == EINTR) continue;
    if (n <= 0) return -1;
    m += n;
  }

  return m;
}

void my_close(int sockfd) {
  close(sockfd);
}

