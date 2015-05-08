#include "common_lib.h"


conf_t global_conf;
bool DEBUG = false;
char* PID_FILE = "/var/run/udpproxy.pid";


void log_debug(char *log_string){
    syslog(LOG_INFO,"%s\n", log_string);
}

void log_info(char *log_string){
    syslog(LOG_INFO,"%s\n", log_string);
}

void log_warning(char *log_string){
    syslog(LOG_WARNING,"%s\n", log_string);
}

void log_error(char *log_string){
    syslog(LOG_ERR,"%s\n", log_string);
}
void debug_print(char* s1, char* s2){
    if(DEBUG){
        printf("%s %s\n",s1,s2);
    }
}
