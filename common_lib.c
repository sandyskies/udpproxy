#include "common_lib.h"


conf_t global_conf;
bool DEBUG = false;
char* PID_FILE = "/var/run/udpproxy.pid";


void log_debug(char *log_string){
    syslog(LOG_INFO, log_string);
}

void log_info(char *log_string){
    syslog(LOG_INFO, log_string);
}

void log_warning(char *log_string){
    syslog(LOG_WARNING, log_string);
}

void log_error(char *log_string){
    syslog(LOG_ERR, log_string);
}