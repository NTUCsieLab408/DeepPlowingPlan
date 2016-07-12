#include "NTU_cluster.h"
#include "NTU_socket.h"

/*
 * connection between units, socket function in unit-level
 */

int NTU_connect_unit(enum NTU_unit_id to);
int NTU_send_unit(enum NTU_unit_id to, const void *buf, size_t len);
int NTU_recv_unit(enum NTU_unit_id from, void *buf, size_t len);
int NTU_accept_unit();
void NTU_close_unit(enum NTU_unit_id who);
int NTU_listen_unit();

/* debug message */

#define WARN(FORMAT, ...) fprintf(stderr, "[W] %d %s: "FORMAT"\n",\
  errno, strerror(errno), ##__VA_ARGS__)

#define INFO(FORMAT, ...) fprintf(stderr, "[I] "FORMAT"\n", ##__VA_ARGS__)

#define ERROR(FORMAT, ...) do { fprintf(stderr, "[E] %d %s: "FORMAT"\n",\
  errno, strerror(errno), ##__VA_ARGS__); exit(EXIT_FAILURE); } while (0)

#define DEVELOP
#ifdef DEVELOP
  #define DEBUG(FORMAT, ...) fprintf(stderr, FORMAT"\n", ##__VA_ARGS__)
#else
  #define DEBUG(FORMAT, ...)
#endif

long DA_diff_time(struct timespec a, struct timespec b);
void DA_data_analysis(int id, struct timespec start, struct timespec end, struct timespec begin);


