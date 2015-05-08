#include    <sys/time.h>
#include    <stdbool.h>
#include    <syslog.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include "acl_control.h"
#include <stdio.h>


#ifndef SO_MAX_MSG_SIZE
#define SO_MAX_MSG_SIZE 65535
#endif

typedef enum loadbalance_algorithm{
    rr=1, 
    wrr, //todo 
    lc, 
    wlc
}lbm_t;

typedef struct server_s{
    struct in_addr server_addr;
    int server_port;
    unsigned int weight;
    struct server_s* next; 
    int count;
}server_t;

typedef struct conf_s{
    struct in_addr listen_addr;
    int listen_port;
    server_t* forward_servers;
    lbm_t loadbalance_method;
    int timeout;
    int process_num; /*number of worker process*/
    access_control_t *acl_rules;
    
}conf_t;


typedef struct connection_s{
    char buffer[SO_MAX_MSG_SIZE];
    int outgoing_fd;
    int timeout_fd;
    struct sockaddr_in incoming_addr;
    struct sockaddr_in outgoing_addr;
    unsigned int retry_time;
    struct connection_s *next;
}connection_t;



extern struct conf_s global_conf;
extern bool DEBUG;
extern char* PID_FILE;



/*functions*/
void log_debug(char *log_string);
void log_info(char *log_string);
void log_error(char *log_string);
void log_warning(char *log_string);
void debug_print(char * s1, char *s2);
