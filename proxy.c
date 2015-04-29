#include"proxy.h"
#include "common_lib.h"
#include <pthread.h>



void set_none_blocking(int sock){
    int opts;
    opts = fcntl(sock, F_GETFL);
    if(ops < 0){
        log_error("fcntl, GETFL");
        exit(1);
    }
}

void thread_main(){

}

void do_porxy(int listenfd){
    set_none_blocking(listenfd);
     
}


