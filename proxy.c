#include "proxy.h"
#include <stdlib.h>
#include "common_lib.h"
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include "queue.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>

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
    size_t size;
    initQue();
    struct epoll_event events[MAXEVENT];
    struct sockaddr_in src_buf;
    int addrlen = sizeof(struct sockaddr_in);

    char recv_buf[SO_MAX_MSG_SIZE];
    set_none_blocking(listenfd);
    for(i=0; i<global_conf.process_num; i++){
        pthread_create(&tid,NULL,thread_main, NULL);
    }
    listen_event_fd = epoll_create(FDSIZE);
    add_event(listen_event_fd, listenfd, EPOLLIN);
    while(1){
        ret = epoll_wait(listen_event_fd, events, MAXEVENT, -1);
        if(ret == -1){
            if(ret == EINTR){
                continue;
            }else{
                log_error("epoll_wait at listen fd");
                exit(-1);
            }
        }  
        while(1){
            bzero(recv_buf, SO_MAX_MSG_SIZE);
            bzero ( &src_buf, sizeof(src_buf));  
            size =  recvfrom(listen_event_fd, recv_buf, SO_MAX_MSG_SIZE, 0, (struct sockaddr *)&src_buf, &addrlen);
            if(size){
                if(size == EWOULDBLOCK  || size == EAGAIN){
                    break;
                }else if(size == EINTR){
                    continue;
                }
                
            }else{
            /* add to queue */
                recv_buf[size] = '\0';
                if (enque(src_buf, recv_buf)){
                  log_error("queue is full") ;
                  continue; 
                }
            }
        }   
    }
}



