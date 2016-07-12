#ifndef NTU_CLUSTERH
#define NTU_CLUSTERH

/*
 * define basic structure of unit in fault-tolerance system
 * each unit should belong to difference hosts, normally
 * this file should be include by NTU_connection.h
 */

#define UNIT_NUM 6                                /* total number of unit */

/*
 * unit_id to represent an unit in the project
 */
enum NTU_unit_id { UNIT1, UNIT2, UNIT3, UNIT4, UNIT5, UNIT6};

struct sock_info {
  const char* ip;
  int sockfd;
  unsigned short port;
};

#define LIVE 1
#define DEAD 0

typedef struct unit {
  int id;                                         /* for communication between machine */
  char* name;                                     /* for debug messages  */
  int status;                                     /* only will be LIVE or DEAD */
  struct sock_info info;                          /* record where other units is  */
}NTU_unit;

/*
 * get and set value of unit, first input should be unit_id
 */

#define get_ip(unit_id)                     UNIT[unit_id].info.ip
#define get_port(unit_id)                   UNIT[unit_id].info.port
#define get_sockfd(unit_id)                 UNIT[unit_id].info.sockfd
#define set_ip(unit_id, value)             (UNIT[unit_id].info.ip = value)
#define set_port(unit_id, value)           (UNIT[unit_id].info.port = value)
#define set_sockfd(unit_id, value)         (UNIT[unit_id].info.sockfd = value)

#define set_name(unit_id, string)          (UNIT[unit_id].name = string)
#define get_name(unit_id)                   UNIT[unit_id].name
#define set_id(unit_id)                    (UNIT[unit_id].id = unit_id)
#define set_status(unit_id, value)          UNIT[unit_id].status = value
#define get_status(unit_id)                 UNIT[unit_id].status

#define set_recv_timeout(me, other, value)  my_recv_timeout[me][other] = value
#define get_recv_timeout(me, other)         my_recv_timeout[me][other]
#define set_send_timeout(me, other, value)  my_send_timeout[me][other] = value
#define get_send_timeout(me, other)         my_send_timeout[me][other]

NTU_unit UNIT[UNIT_NUM];
int mysend_timeout[UNIT_NUM][UNIT_NUM];
int myrecv_timeout[UNIT_NUM][UNIT_NUM];
int me;                                           /* will be assigned by the input of main  */

#endif
