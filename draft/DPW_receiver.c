#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "DPW_receiver.h"

int count, commit;
pthread_t DPW_recv_f_t, DPW_recv_m_t[NUM_M], DPW_recv_b_t, DPW_send_b_t;
pthread_mutex_t save_mutex;
struct log_data *list;

static struct timespec DPW_DA_begin_time;

void *DPW_recv_f_task(void *arg);
void *DPW_recv_m_task(void *arg);
void *DPW_recv_b_task(void *arg);
void *DPW_send_b_task(void *arg);

int main(int argc, char **argv) {
  int i, tmp_m[NUM_M];
  int accepted = 0;

  /* Handle arguments */
  if (argc != 2) {
    printf("Usage: %s type\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  me = DPW_initialize_cluster(argv[1]);

  /* Initialization */
  count = 0;
  commit = 0;
  list = (struct log_data *) malloc(BUFFER_SIZE * sizeof (struct log_data));
  pthread_mutex_init(&save_mutex, NULL);

  if (NTU_listen_unit() < 0) ERROR("listen_unit");
  INFO("Start %s", get_name(me));

  for (i=F0; i<me; i++){
    NTU_connect_unit(i);
  }
  NTU_connect_unit(B);

  /* Create all necessary threads */
  while (pthread_create(&DPW_recv_b_t, NULL, DPW_recv_b_task, NULL) < 0) {
    WARN("pthread_create DPW_recv_b_t");
  }
  while (pthread_create(&DPW_recv_f_t, NULL, DPW_recv_f_task, NULL) < 0) {
    WARN("pthread_create DPW_recv_f_t");
  }
  for (i = 0; i < NUM_M; i++) {
    tmp_m[i] = i;
    while (pthread_create(&DPW_recv_m_t[i], NULL, DPW_recv_m_task, (void *) &tmp_m[i]) < 0) {
      WARN("pthread_create DPW_recv_m_t[%d]", i);
    }
  }
  while (pthread_create(&DPW_send_b_t, NULL, DPW_send_b_task, NULL) < 0) {
    WARN("pthread_create DPW_send_b_t");
  }

  /* Main loop */
  while (1) {
    accepted = NTU_accept_unit();
  }
  return 0;
}
void *DPW_recv_f_task(void *arg) {
  int protocol;

  /* Main loop */
  while (1) {
    if (get_status(F0) == DEAD) continue;

    if (NTU_recv_unit(F0, &protocol, sizeof (int)) < 0) {
      continue;
      //protocol = which+3;
      //if (send_unit(UNIT[B][0], &protocol, sizeof (int)) < 0) {
      //}
    }
    INFO("Recv F that M%d was broken", protocol - 3);
  }

  pthread_exit(NULL);
}

void *DPW_recv_m_task(void *arg) {
  int M = M0 + *((int *) arg);
  struct DPW_result rslt;
  int index, pre_index;//, i;
  struct timespec start_time, end_time;
  static int max_diff = 0;
  int tmp, start;

  /* Main loop */
  while (1) {
    if (get_status(M) == DEAD) continue;

    if (NTU_recv_unit(M, &rslt, DPW_result_size) < 0) continue;
    INFO("Recv %s id=[%d] count=[%d] hash=[%d] str=[%s]", get_name(M), rslt.id, rslt.count, rslt.hash, rslt.str);

    tmp = (int) count - (int) rslt.id * 2;
    if (tmp < 0) tmp = -tmp;
    if (tmp > max_diff) max_diff = tmp;
    //INFO("Diff=[%d] Max=[%d]", tmp, max_diff);

    /* Ignore recover outcome */
    if (rslt.id < commit) continue;

    if (rslt.id < 1) clock_gettime(CLOCK_REALTIME, &DPW_DA_begin_time);
    clock_gettime(CLOCK_REALTIME, &start_time);

    pthread_mutex_lock(&save_mutex);

    index = rslt.id % BUFFER_SIZE;
    pre_index = (rslt.id) - 1;
    while (rslt.id > 0 && list[pre_index % BUFFER_SIZE].flag == 0) {
      INFO("Miss data %d", pre_index);
      pre_index--;
      if (pre_index < 0) break;
    }
    if (pre_index != rslt.id - 1) {
      pre_index++;
      start = pre_index;
      INFO("Ask to resend");
      //for (i = 0; i < NUM_M; i++) {
      tmp = 1;
      if (NTU_send_unit(M, &tmp, sizeof (int)) < 0) {
      }
      if (NTU_send_unit(M, &start, sizeof (int)) < 0) {
      }
      pre_index = (rslt.id) - 1;
      if (NTU_send_unit(M, &pre_index, sizeof (int)) < 0) {
      }
      //}
      //pthread_mutex_unlock(&mutex);
      //continue;
    }

    if (list[index].flag == 1 && list[index].t.id != rslt.id) {
      list[index].flag = 0;
    }

    if (list[index].flag == 0 || list[index].flag == 2) {
      list[index].flag = 1;
      list[index].t = rslt;
      list[index].u = M;

      /* Send to B */
      if (NTU_send_unit(B, &rslt, DPW_result_size) < 0) {
      }
      else DEBUG("Send B id=[%d] count=[%d] hash=[%d] str=[%s]", rslt.id, rslt.count, rslt.hash, rslt.str);
    }
    else {
      list[index].flag = 2;

      if (strcmp(rslt.str, list[index].t.str) != 0 || rslt.hash != list[index].t.hash || rslt.count != list[index].t.count) {
        INFO("Task %d is different", rslt.id);
      }
      else {
        commit++;
        INFO("Task %d is commited, commit=[%d]", rslt.id, commit);
      }
    }

    clock_gettime(CLOCK_REALTIME, &end_time);
    DA_data_analysis(rslt.id, start_time, end_time, DPW_DA_begin_time);
    fflush(stdout);

    count++;

    pthread_mutex_unlock(&save_mutex);
  }

  pthread_exit(NULL);
}

void *DPW_recv_b_task(void *arg) {
  int protocol, i;

  /* Main loop */
  while (1) {
    if (get_status(B) == DEAD) continue;

    if (NTU_recv_unit(B, &protocol, sizeof (int)) < 0) {
      continue;
    }
    INFO("Recv B that F was broken");

    protocol = 2;
    for (i = 0; i < NUM_M; i++) {
      if (NTU_send_unit(M0 + i, &protocol, sizeof (int)) < 0) {
      }
    }
  }

  pthread_exit(NULL);
}

void *DPW_send_b_task(void *arg) {
  struct DPW_result rslt;

  while (1) {
    rslt.id = -1;
    if (NTU_send_unit(B, &rslt, DPW_result_size) < 0);
    sleep(1);
  }

  pthread_exit(NULL);
}

