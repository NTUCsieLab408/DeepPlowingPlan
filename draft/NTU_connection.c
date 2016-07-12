#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include "NTU_connection.h"

/*
 * inplement socket function in unit-level
 * all function will return negtive value when error occurs
 * this file should be included by stockexchange.h
 */

static int my_listen_sockfd;
struct timeval send_timeout;
struct timeval recv_timeout;

/*
 * close, then setup connection between units
 * assume it won't setup a connection with a wrong unit
 */

int NTU_connect_unit(enum NTU_unit_id to) {
  int sockfd;
  int ack;
  struct timeval tmp = {1, 0};

  NTU_close_unit(to);
  DEBUG("Connect to %s ...", get_name(to));

  sockfd = my_connect(get_ip(to), get_port(to));
  if (sockfd < 0) return -1;

  if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *) &tmp, sizeof (struct timeval)) < 0) {
    my_close(sockfd);
    return -1;
  }

  /*
   * tell others who am I
   */

  if (my_send(sockfd, &me, sizeof (int)) < 0) {
    my_close(sockfd);
    return -1;
  }

  send_timeout.tv_sec = mysend_timeout[me][to];
  recv_timeout.tv_sec = myrecv_timeout[me][to];

  if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *) &send_timeout, sizeof (struct timeval)) < 0) {
    my_close(sockfd);
    return -1;
  }
  if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *) &recv_timeout, sizeof (struct timeval)) < 0) {
    my_close(sockfd);
    return -1;
  }

  INFO("%s was connected", get_name(to));
  set_sockfd(to, sockfd);
  set_status(to, LIVE);
  return to;
}

/*
 * this function will return sent bytes
 */

int NTU_send_unit(enum NTU_unit_id to, const void *buf, size_t len) {
  int n = -1;
  int sockfd;
  sockfd = get_sockfd(to);

  if (get_status(to) == LIVE) {
    if ((n = my_send(sockfd, buf, len)) < 0) {
      WARN("do_send %s", get_name(to));
      set_status(to, DEAD);
    }
  }
  return n;
}

/*
 * if there is no timeout, this function could be blocked forever after reconnted by other unit
 * this function will return received bytes
 */

int NTU_recv_unit(enum NTU_unit_id from, void *buf, size_t len) {
  int n = -1;
  int sockfd;
  sockfd = get_sockfd(from);

  if (get_status(from) == LIVE) {
    if ((n = my_recv(sockfd, buf, len)) < 0) {
      WARN("do_recv %s", get_name(from));
      set_status(from, DEAD);
    }
  }
  return n;
}

int NTU_listen_unit() {
  my_listen_sockfd = my_listen(get_port(me));
  if (my_listen_sockfd < 0) return -1;

  return my_listen_sockfd;
}

void NTU_close_unit(enum NTU_unit_id unit) {
  my_close(get_sockfd(unit));
  set_sockfd(unit, -1);
  set_status(unit, DEAD);
}

int NTU_accept_unit() {
  int sockfd;
  int from;
  struct timeval tmp = {1, 0};

  DEBUG("Accept ...");
  sockfd = my_accept(my_listen_sockfd);
  if (sockfd  < 0) return -1;

  if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *) &tmp, sizeof (struct timeval)) < 0) {
    my_close(sockfd);
    return -1;
  }

  if (my_recv(sockfd, &from, sizeof (int)) < 0) {
    my_close(sockfd);
    return -1;
  }

  if (from < 0) exit(1);

  if (my_send(sockfd, &me, sizeof (int)) < 0) {
    my_close(sockfd);
    return -1;
  }

  send_timeout.tv_sec = mysend_timeout[me][from];
  recv_timeout.tv_sec = myrecv_timeout[me][from];

  if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *) &send_timeout, sizeof (struct timeval)) < 0) {
    my_close(sockfd);
    return -1;
  }
  if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *) &recv_timeout, sizeof (struct timeval)) < 0) {
    my_close(sockfd);
    return -1;
  }

  set_sockfd(from, sockfd);
  set_status(from, LIVE);
  INFO("%s was accepted", get_name(from));
  return from;
}


/* Miscellaneous */

long DA_diff_time(struct timespec a, struct timespec b) {
  return (a.tv_nsec - b.tv_nsec) + (a.tv_sec - b.tv_sec) * 1000000000L;
}

void DA_data_analysis(int id, struct timespec start, struct timespec end, struct timespec begin) {
  printf("%d, %ld, %ld\n", id, DA_diff_time(end, start) / 1000, DA_diff_time(end, begin) / 1000);
  fflush(stdout);
}

