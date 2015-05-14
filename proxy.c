#include "proxy.h"


//static sem_t global_sem; 
pthread_mutex_t mutex;
int pipe_fd[2];


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

static void add_event(int epollfd, int fd, int state){
    int ret;
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
    if(ret == -1){
        log_error("epoll_ctl");
    }
}
static void del_event(int epollfd, int fd, int state){
    int ret;
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
    if(ret == -1){
        log_error("epoll_ctl");
    }
}

static void format_cnode(connection_t *p, char* buffer, int outgoing_fd, int timeout_fd, struct sockaddr_in incoming_addr, struct sockaddr_in outgoing_addr, int size){
   // p->buffer = buffer;
    bzero(buffer, SO_MAX_MSG_SIZE);
    memcpy(p->buffer, buffer ,size);
    p->outgoing_fd = outgoing_fd;
    p->timeout_fd = timeout_fd;
    p->incoming_addr = incoming_addr;
    p->outgoing_addr = outgoing_addr;
    p->retry_time = 0;
    p->next = NULL;
}

static connection_t* in_list(connection_t *head, connection_t *node){
    connection_t *tmp;
    if(head == NULL){
        head = node;
    }else{
        tmp = head->next;
        head->next = node;
        node->next = tmp;
    }
    return head;
}
static connection_t* del_list(connection_t *head, connection_t *node){
    connection_t *tmp = head;
    connection_t *tmp2 = NULL;
    server_t *p = global_conf.forward_servers;
    if(tmp == NULL){
        return NULL;
    }    
    while(tmp != NULL){
        if(tmp->outgoing_fd == node->outgoing_fd && tmp->timeout_fd == node->timeout_fd ){

            if(tmp == head){
                head = head->next;
                while(p){
                    if( p->server_addr.s_addr ==  tmp->outgoing_addr.sin_addr.s_addr && htons(p->server_port) == tmp->outgoing_addr.sin_port){
                        p->count--;
                    }
                    p = p->next;
                }
                free(tmp);
                return head;
            }else{
                while(p){
                    if( p->server_addr.s_addr ==  tmp->outgoing_addr.sin_addr.s_addr && htons(p->server_port) == tmp->outgoing_addr.sin_port){
                        p->count--;
                    }
                    p = p->next;
                }
                tmp2->next = tmp->next;
                free(tmp);
                return head;
            }
        }
        tmp2 = tmp;
        tmp = tmp->next;
    }
    return head;
}


static int  search_list(int fd, connection_t *head, connection_t **p){
    connection_t *q = head;
    while(q != NULL){
        if(q->timeout_fd == fd ){
            *p = q;
            return 1;
        }
        if(q->outgoing_fd == fd){
            *p = q;
            return 2;
        }
        q = q->next;
    }
    return 0;
}




void* thread_main(void *argv){
    struct sockaddr_in  c_addr;  
    struct sockaddr_in server_addr;
    int size;
    char buffer[SO_MAX_MSG_SIZE];
    char recv_buffer[SO_MAX_MSG_SIZE];
    char log_s[100];
    char c[1];
    int proxy_event_fd;
    int send_fd;
    int ret, i, send_ret,j,recv_size;
    int sa_size = sizeof(struct sockaddr);
    struct sockaddr tmp_addr;
    int timeout_fd;
    int ret_fd;
    int type;
    int flag = 0;
    int listen_fd = *((int *)argv);
    struct epoll_event events[MAXEVENT];
    struct itimerspec new_timeout = {
        .it_interval = {
            .tv_sec = 0,
            .tv_nsec = 0,
        },
        .it_value = {
            .tv_sec = global_conf.timeout,
            .tv_nsec = 0,
        },
    };
    struct itimerspec old_timeout;
    connection_t *head = NULL;
    connection_t *tmp_cp;
    proxy_event_fd = epoll_create(FDSIZE);
    add_event(proxy_event_fd, pipe_fd[0], EPOLLIN );
    while(1){

        if((ret = epoll_wait(proxy_event_fd, events, MAXEVENT, -1)) < 0){
            sprintf(log_s, "error happen while epoll_wait(),%s\n",strerror(errno));
            log_error(log_s);
            continue;
        }else if(ret == 0){
            continue;
        }else{
            log_debug("events happens");
            for(i=0; i < ret; i++){
                ret_fd = events[i].data.fd;
                if(ret_fd == pipe_fd[0]){
                    if(read(pipe_fd[0], c, 1) == 1){
                        bzero(&c_addr,sizeof(c_addr));
                        bzero(buffer,SO_MAX_MSG_SIZE);
                        if(deque(&c_addr, buffer, &size)){
                            log_warning("queue is empty, wait for main thread notify.");
                            continue;  
                        }
                        if(access_find(c_addr.sin_addr.s_addr, global_conf.acl_rules) != 1){
                            sprintf(log_s , "access_deny for client: %s\n",inet_ntoa(c_addr.sin_addr) );
                            log_info(log_s);
                            continue;
                        }
                        bzero(&server_addr, sizeof(server_addr));
                        pthread_mutex_lock(&mutex);
                        server_addr = pick_up_one();
                        pthread_mutex_unlock(&mutex);
                        errno = 0;
                        send_fd = socket(AF_INET,SOCK_DGRAM,0);
                        if(errno != 0 ){
                            log_error("create socket");
                        }
                        while(1){
                            errno = 0 ;
                            if((ret = sendto(send_fd, buffer, size, 0 ,(struct sockaddr*)&server_addr,sizeof(struct sockaddr))) < 0){
                                if(errno == EINTR){
                                    continue;
                                }else{
                                    sprintf(log_s, "error happen while sendto(),%s\n",strerror(errno));
                                    log_error(log_s);
                                    break;
                                }
                            }else{
                                break;
                            }   
                        }        
                        log_debug("sendto sucess");
                        add_event(proxy_event_fd, send_fd, EPOLLIN );
                        timeout_fd = timerfd_create(CLOCK_MONOTONIC, 0);
                        if(timeout_fd < 0){
                            log_error("timerfd_create()");
                        }
                        ret = timerfd_settime(timeout_fd, 0, &new_timeout, &old_timeout);
                        if(ret == -1 ){
                            log_error("settime");
                        }
                        add_event(proxy_event_fd, timeout_fd, EPOLLIN );
                        tmp_cp = (connection_t*)malloc(sizeof(connection_t));
                        format_cnode(tmp_cp,buffer, send_fd, timeout_fd, c_addr, server_addr,size);
                        head = in_list(head, tmp_cp); 
                    }   
                    
                    
                }else{
                    type = search_list(ret_fd, head, &tmp_cp); 
                    if(type == 1){
                        /*timeout happens*/
                        log_debug("timeout happens");
                        flag = 0;
                        for(j=0; j<ret; j++){
                            if(events[j].data.fd == tmp_cp->outgoing_fd){
                                /*something to read */
                                log_debug("something happens.");

                                bzero(recv_buffer,SO_MAX_MSG_SIZE );
                                recv_size = recvfrom(tmp_cp->outgoing_fd, recv_buffer,SO_MAX_MSG_SIZE , 0, &tmp_addr, &sa_size);
                                sendto(listen_fd, recv_buffer, recv_size ,0,(struct sockaddr*)&tmp_cp->incoming_addr, sizeof(struct sockaddr));
                                del_event(proxy_event_fd, tmp_cp->timeout_fd, EPOLLIN);
                                del_event(proxy_event_fd, tmp_cp->outgoing_fd, EPOLLIN);
                                close(tmp_cp->outgoing_fd);
                                close(tmp_cp->timeout_fd);
                                head = del_list(head, tmp_cp); 
                                flag = 1;
                            }
                        }
                        if(tmp_cp->retry_time == 0 && flag == 0){
                            log_debug("retrans");
                            sendto(tmp_cp->outgoing_fd, tmp_cp->buffer, tmp_cp->size ,0,(struct sockaddr*)&tmp_cp->incoming_addr, sizeof(struct sockaddr));
                            timeout_fd = timerfd_create(CLOCK_MONOTONIC, 0);
                            timerfd_settime(timeout_fd, 0, &new_timeout, &old_timeout);
                            del_event(proxy_event_fd, tmp_cp->timeout_fd, EPOLLIN);
                            close(tmp_cp->timeout_fd);
                            add_event(proxy_event_fd, timeout_fd, EPOLLIN);
                            tmp_cp->timeout_fd = timeout_fd;
                            tmp_cp->retry_time += 1; 
                        }else{
                                log_debug("drop packet because of timeout");
                                del_event(proxy_event_fd, tmp_cp->timeout_fd, EPOLLIN);
                                del_event(proxy_event_fd, tmp_cp->outgoing_fd, EPOLLIN);
                                close(tmp_cp->timeout_fd);
                                close(tmp_cp->outgoing_fd);
                                head = del_list(head, tmp_cp); 
                        }



                    }else if(type==2){
                        /* data to read*/
                        log_debug("data to read");
                        while(1){
                            bzero(recv_buffer,SO_MAX_MSG_SIZE );
                            recv_size = recvfrom(tmp_cp->outgoing_fd, recv_buffer,SO_MAX_MSG_SIZE , 0, &tmp_addr, &sa_size);
                            errno = 0;
                            if((send_ret = sendto(listen_fd, recv_buffer, recv_size, 0 ,(struct sockaddr*)&tmp_cp->incoming_addr,sizeof(struct sockaddr))) < 0){
                                if(errno == EINTR){
                                    continue;
                                }else{
                                    sprintf(log_s, "error happen while sendto(),%s\n",strerror(errno));
                                    log_error(log_s);
                                    break;
                                }
                            }else{
                                break;
                            }   
                        }
                        del_event(proxy_event_fd, tmp_cp->timeout_fd, EPOLLIN);
                        del_event(proxy_event_fd, tmp_cp->outgoing_fd, EPOLLIN);
                        close(tmp_cp->outgoing_fd);
                        close(tmp_cp->timeout_fd);
                        head = del_list(head, tmp_cp); 

                    }else{
                        log_error("Not found.");
                    }
                }    
           } 
        }
    }    
}



void do_proxy(int listenfd){
    int i;
    pthread_t tid;
    int listen_event_fd;
    int ret;
    size_t size;
    initQue();
    struct epoll_event events[MAXEVENT];
    struct sockaddr_in src_buf;
    int addrlen = sizeof(struct sockaddr_in);
    //sem_init(&global_sem, 0, MAXLEN);
    pipe(pipe_fd);
    pthread_mutex_init(&mutex, NULL);

    char recv_buf[SO_MAX_MSG_SIZE];
    set_none_blocking(listenfd);
    for(i=0; i<global_conf.process_num; i++){
        pthread_create(&tid,NULL,thread_main, &listenfd);
    }
    listen_event_fd = epoll_create(FDSIZE);
    add_event(listen_event_fd, listenfd, EPOLLIN);
    while(1){
        errno = 0;
        ret = epoll_wait(listen_event_fd, events, MAXEVENT, -1);
        if(ret == -1){
            if(errno == EINTR){
                continue;
            }else{
                log_error("epoll_wait at listen fd");
                exit(-1);
            }
        }  
        while(1){
            bzero(recv_buf, SO_MAX_MSG_SIZE);
            bzero ( &src_buf, sizeof(src_buf));  
            errno = 0;
            size =  recvfrom(listenfd, recv_buf, SO_MAX_MSG_SIZE, 0, (struct sockaddr *)&src_buf, &addrlen);
            if(size == -1){
                if(errno  == EWOULDBLOCK  || errno == EAGAIN){
                    break;
                }else if(errno == EINTR){
                    continue;
                }
                
            }else{
            /* add to queue */
                //recv_buf[size] = '\0';
                //We can't treat it as a c string.
                if (enque(src_buf, recv_buf,size)){
                  log_error("queue is full") ;
                  continue; 
                }
  /*              if(sem_post(&global_sem)<0){
                  log_debug("post fail"); */
                write(pipe_fd[1], "1", 1);
            }
        }   
    }
}



