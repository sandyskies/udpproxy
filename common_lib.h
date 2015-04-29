#include    <sys/time.h>
#include    <stdbool.h>
#include    <syslog.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#ifndef SO_MAX_MSG_SIZE
#define SO_MAX_MSG_SIZE 65535
#endif

typedef enum loadbalance_argirthm{
    rr=1, 
    wrr, 
    lc, 
    wlc
}lbm_t;

typedef struct server_s{
    struct in_addr server_addr;
    int server_port;
    unsigned int weight;
    struct server_s* next; 
}server_t;

typedef struct conf_s{
    struct in_addr listen_addr;
    int listen_port;
    server_t* forward_servers;
    lbm_t loadbalance_method;
    int timeout;
    int process_num; /*number of worker process*/
    
}conf_t;

typedef struct msg_buffer_s{
    char buffer[SO_MAX_MSG_SIZE];
    size_t size;
    struct msg_buffer_s *next;
}msg_buffer_t;

typedef struct connection_s{
    msg_buffer_t* read_buffer;
    int incoming_fd;
    int outgoing_fd;
    struct sockaddr_in incoming_addr;
    struct sockaddr_in outgoing_addr;
    struct connection_s *next;
}connection_t;



extern conf_t global_conf;
extern int DEBUG;
extern char* PID_FILE;



/*functions*/
void log_debug(char *log_string);
void log_info(char *log_string);
void log_error(char *log_string);
void log_warning(char *log_string);