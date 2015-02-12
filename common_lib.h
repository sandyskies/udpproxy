#include    <netinet/in.h>
#include    <sys/time.h>


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
    struct timeval timeout_tv;
    char* log_dir;
    int process_num; /*number of worker process*/
    
}conf_t;



extern conf_t global_conf;
extern char* PID_FILE;
