#ifndef STOCKEXH
#define STOCKEXH

/*
 * define an order and a result
 * and others for stockexchange
 * this file should be included by every unit
 */

#include "NTU_connection.h"

#define G0 0
#define F0 1
#define M0 2
#define M1 3
#define R  4
#define B  5
#define NUM_G 1
#define NUM_F 1
#define NUM_M 2
#define is_empty(task) (task.id == -1)

int DPW_initialize_cluster(char *);

/* an input by other company which is called order */

#define DPW_ORDER_SIZE 300
struct __attribute__ ((packed)) DPW_order {
  int id;                                          /* sequence number */
  char str[DPW_ORDER_SIZE];                        /* input information */
};

/* an output matched by Matcher is called result  */

#define DPW_RESULT_SIZE 300
struct __attribute__ ((packed)) DPW_result {
  int id;                                          /* sequence number */
  char str[DPW_RESULT_SIZE];                       /* output information */
  int count;                                       /* used for dependency between orders */
  int hash;                                        /* used for dependency between orders */
};

/* any single point failure should not lead the difference between units over this value */

#define BUFFER_SIZE 10000

/* define size */

extern const int DPW_order_size;
extern const int DPW_result_size;

#endif
