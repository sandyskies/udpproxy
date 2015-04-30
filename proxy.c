#include "proxy.h"
#include <stdlib.h>
#include "common_lib.h"
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include "queue.h"

#define MAXEVENT 1024
#define FDSIZE 1024



static void set_none_blocking(int sock){
    int opts;
    opts = fcntl(sock, F_GETFL);
    if(opts < 0){
        log_error("fcntl, GETFL");
        exit(1);
    }
    opts = opts|O_NONBLOCK;
    if(fcntl(sock,F_SETFL,opts)<0)
    {
        log_error("fcntl(sock,SETFL,opts)");
        exit(1);
                        
    }  
}

void* thread_main(){

}
static void add_event(int epollfd, int fd, int state){
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}

void do_porxy(int listenfd){
    int i;
    pthread_t tid;
    int listen_event_fd;
    int ret;
    struct epoll_event events[MAXEVENT];
    //set_none_blocking(listenfd);
    for(i=0; i<global_conf.process_num; i++){
        pthread_create(&tid,NULL,thread_main, NULL);
    }
    listen_event_fd = epoll_create(FDSIZE);
    add_event(listen_event_fd, listenfd, EPOLLIN);
    while(1){
        
        ret = epoll_wait(listen_event_fd, events, MAXEVENT, -2);

    }
    
}



