#include "stockex.h"
/* define size */

const int DPW_order_size =  sizeof (struct DPW_order);
const int DPW_result_size = sizeof (struct DPW_result);

int DPW_initialize_cluster(char* me) {
  int my_id;

  set_id(G0);
  set_name(G0,"G");
  //set_ip(G0,"192.168.1.11");
  set_ip(G0,"192.168.48.82");
  set_port(G0,50100);
  set_sockfd(G0,-1);
  set_status(G0,DEAD);

  set_id(F0);
  set_name(F0,"F");
  //set_ip(F0,"192.168.1.21");
  set_ip(F0,"192.168.48.83");
  set_port(F0,50120);
  set_sockfd(F0,-1);
  set_status(F0,DEAD);

  set_id(M0);
  set_name(M0,"M0");
  //set_ip(M0,"192.168.1.31");
  set_ip(M0,"192.168.48.80");
  set_port(M0,50140);
  set_sockfd(M0,-1);
  set_status(M0,DEAD);

  set_id(M1);
  set_name(M1,"M1");
  //set_ip(M1,"192.168.1.41");
  set_ip(M1,"192.168.48.81");
  set_port(M1,50160);
  set_sockfd(M1,-1);
  set_status(M1,DEAD);

  set_id(R);
  set_name(R,"R");
  //set_ip(R,"192.168.1.51");
  set_ip(R,"192.168.48.78");
  set_port(R,50180);
  set_sockfd(R,-1);
  set_status(R,DEAD);

  set_id(B);
  set_name(B,"B");
  //set_ip(B,"192.168.1.61");
  set_ip(B,"192.168.48.84");
  //set_ip(B,"192.168.49.167");
  set_port(B,50200);
  set_sockfd(B,-1);
  set_status(B,DEAD);

  /* set timeout between units
   * first index is the one who will send/receive
   * second index is the one who will be sent/received
   */

  int i, j;
  for (i=0; i<UNIT_NUM; ++i) {
    if (strcmp(me, get_name(i)) == 0)  my_id = i;
    for (j=0; j<UNIT_NUM; ++j) {
      mysend_timeout[i][j] = 1;
      myrecv_timeout[i][j] = 0;
    }
  }

  for (j=0; j<UNIT_NUM; ++j) {
    mysend_timeout[G0][j] = 10;
    myrecv_timeout[G0][j] = 10;
    myrecv_timeout[F0][j] = 5;
    myrecv_timeout[B][j] = 2;
  }
  mysend_timeout[M0][R] = 2;
  mysend_timeout[M1][R] = 2;
  return my_id;
}

