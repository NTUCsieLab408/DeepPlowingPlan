#include "stockex.h"

/*
 * implement matching engine, have kernel version in XXX.c
 * one thread for receiving order
 * one thread for taking order and sending resault
 * two threads for receiving recover message
 */

/* protocol between two Matcher
 * using when initial state or one of the Matcher recovered
 * the fast one will give its order to another
 * Matcher does not record its internal state now, so it will give all order.
 * frequency to record internal state should be related to BUFFER_SIZE
 */

#define SEND_TASK   1
#define ASK_TO_SEND 2

/*
 * protocol between Matcher and Receiver
 * using when both Matcher miss their result which is sent to Receiver
 * or Receiver notice Forwarder is broken
 */

#define RESEND      1
#define F_BROKEN    2

/*
 * internal state could be something not recorded in order and result
 * it should be the same in two Matcher
 * recover by synchronizing internal state could be more efficient
 */

#define NUM_USER    10
struct DPW_internal_state {
  int id;
  int user[NUM_USER];
  int dependent_data;
};

static void DPW_recv_order(enum NTU_unit_id);
static int DPW_take_order(struct DPW_result*);
static int DPW_send_result(struct DPW_result*);

