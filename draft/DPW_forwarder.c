#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "DPW_forwarder.h"

int DPW_countack, num_ack[NUM_M];
int DPW_infly_high, DPW_infly_low;
int *DPW_table4g;

pthread_t DPW_recv_g_send_m_t[NUM_G], DPW_recv_m_send_g_t[NUM_M], DPW_FT_send_b_t;
pthread_mutex_t ordered_mutex, ack_mutex;

struct DPW_order *DPW_order_list;
struct timespec DPW_DA_begin_time;
struct timespec *DPW_DA_end_list, *DPW_DA_start_list;

void *DPW_recv_g_send_m_task(void *arg);
void *DPW_recv_m_send_g_task(void *arg);
void *DPW_FT_send_b_task(void *arg);

int main(int argc, char **argv) {
  int i, tmp_g[NUM_G], tmp_m[NUM_M], ack;
  int accepted;

  /* Handle arguments */
  if (argc != 3) {
    printf("Usage: %s type infly_high\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  me = DPW_initialize_cluster(argv[1]);
  printf("%d\n", me);
  DPW_infly_low = DPW_infly_high = atoi(argv[2]);  // count diff between F and M

  /* Initialization */
  DPW_countack = 0;
  for (i = 0; i < NUM_M; i++) num_ack[i] = -1;
  DPW_order_list = (struct DPW_order *) malloc(BUFFER_SIZE * sizeof (struct DPW_order));
  DPW_table4g = (int *) malloc(BUFFER_SIZE * sizeof (int));
  DPW_DA_end_list = (struct timespec *) malloc(BUFFER_SIZE * sizeof (struct timespec));
  DPW_DA_start_list = (struct timespec *) malloc(BUFFER_SIZE * sizeof (struct timespec));

  pthread_mutex_init(&ordered_mutex, NULL);
  pthread_mutex_init(&ack_mutex, NULL);
  if (NTU_listen_unit() < 0) ERROR("listen_unit");

  INFO("Start %s", get_name(me));

  /* Connect to all Ms, R, B */
  NTU_connect_unit(M0);
  NTU_connect_unit(M1);
  NTU_connect_unit(R);

  if (NTU_connect_unit(B) == B) {
    if (NTU_recv_unit(B, &DPW_countack, sizeof (int)) < 0) {
    }
    else INFO("Recover DPW_countack=[%d]", DPW_countack);
    if (NTU_recv_unit(B, &ack, sizeof (int)) < 0) {
    }
    else {
      INFO("Recover ack=[%d]", ack);
      num_ack[0] = ack;
      num_ack[1] = ack;
    }
  }

  /* Create all necessary threads */
  for (i = 0; i < NUM_G; i++) {
    tmp_g[i] = i;
    while (pthread_create(&DPW_recv_g_send_m_t[i], NULL, DPW_recv_g_send_m_task, (void *) &tmp_g[i]) < 0) {
      WARN("pthread_create m_main_t[%d]", i);
    }
  }
  for (i = 0; i < NUM_M; i++) {
    tmp_m[i] = i;
    while (pthread_create(&DPW_recv_m_send_g_t[i], NULL, DPW_recv_m_send_g_task, (void *) &tmp_m[i]) < 0) {
      WARN("pthread_create m_main_t[%d]", i);
    }
  }
  while (pthread_create(&DPW_FT_send_b_t, NULL, DPW_FT_send_b_task, NULL) < 0) {
    WARN("pthread_create DPW_FT_send_b_t");
  }

  /* Main loop */

  while (1) {
    accepted = NTU_accept_unit();

    if (accepted >= F0 + NUM_F) {  // M R B
      INFO("Reconnect to %s", get_name(accepted));
      while (NTU_connect_unit(accepted) < 0) {
        sleep(1);
      }
    }
    if(accepted < F0);
  }
  return 0;
}

void *DPW_recv_m_send_g_task(void *arg){
  int w = *((int *) arg);
  int M = M0 + w;
  int ack, ok = 0, protocol, gw;

  /* Main loop */
  while (1) {
    if (get_status(M) == DEAD) continue;
    if (num_ack[w] >= DPW_countack - 1) continue;

    if (NTU_recv_unit(M, &ack, sizeof (int)) < 0) {
      protocol = w + 3;
      if (NTU_send_unit(R, &protocol, sizeof (int)) < 0) {
      }
      INFO("Notify R that %s die", get_name(M));
      continue;
    }
    DEBUG("Recv %s ack=[%d]", get_name(M), ack);

    /* Set ack for G */

    pthread_mutex_lock(&ack_mutex);
    if (ack > num_ack[0] && ack > num_ack[1]) {  // critical section!!
      num_ack[w] = ack;
      ok = 1;
    }
    pthread_mutex_unlock(&ack_mutex);

    if (ok == 1) {
      ok = 0;
      gw = DPW_table4g[ack % BUFFER_SIZE];
      if (NTU_send_unit(gw, &ack, sizeof (int)) < 0) continue;
      DEBUG("Send G%d ack=[%d]", gw, ack);

      clock_gettime(CLOCK_REALTIME, &DPW_DA_end_list[ack % BUFFER_SIZE]);
      DA_data_analysis(ack, DPW_DA_start_list[ack%BUFFER_SIZE], DPW_DA_end_list[ack%BUFFER_SIZE], DPW_DA_begin_time);
      fflush(stdout);
    }

    if (ack > num_ack[w]) num_ack[w] = ack;
  }

  pthread_exit(NULL);
}

void *DPW_recv_g_send_m_task(void *arg) {
  int w = *((int *) arg);
  int G = G0 + w;
  struct DPW_order odr;
  int i, infly = 0;

  /* Main loop */
  while (1) {
    if (get_status(G) == DEAD) continue;

    if (DPW_countack - (DPW_infly_high - infly) > num_ack[0] && DPW_countack - (DPW_infly_high - infly) > num_ack[1]) {
      infly = DPW_infly_high - DPW_infly_low;
      continue;
    }
    INFO("%d %d",DPW_countack - num_ack[0], DPW_countack - num_ack[1]);
    infly = 0;
    if (NTU_recv_unit(G, odr.str, DPW_order_size) < 0) continue;
    DEBUG("Recv %s str=[%s]", get_name(G), odr.str);

    if (DPW_countack < 1) clock_gettime(CLOCK_REALTIME, &DPW_DA_begin_time);
    clock_gettime(CLOCK_REALTIME, &DPW_DA_start_list[DPW_countack % BUFFER_SIZE]);

    pthread_mutex_lock(&ack_mutex);

    /* Generate an id */
    odr.id = DPW_countack;
    DPW_table4g[DPW_countack % BUFFER_SIZE] = w;
    DPW_order_list[DPW_countack % BUFFER_SIZE] = odr;

    /* Send to all Ms */
    for (i = 0; i < NUM_M; i++) {
      if (NTU_send_unit(M0 + i, &odr, DPW_order_size) < 0) {
      }
      else DEBUG("Send M%d id=[%d] str=[%s]", i, odr.id, odr.str);
    }

    pthread_mutex_unlock(&ack_mutex);

    /* Send to B */
    if (NTU_send_unit(B, &odr, DPW_order_size) < 0) {
    }
    else DEBUG("Send B id=[%d] str=[%s]", odr.id, odr.str);

    DPW_countack++;
  }

  pthread_exit(NULL);
}

void *DPW_FT_send_b_task(void *arg) {
  struct DPW_order odr;

  while (1) {
    odr.id = -1;
    if (NTU_send_unit(B, &odr, DPW_order_size) < 0) {
    }
    sleep(1);
  }

  pthread_exit(NULL);
}

