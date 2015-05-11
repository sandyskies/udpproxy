#include "load_balance.h"



struct sockaddr_in pick_up_one(){
    struct sockaddr_in server_addr;
    static server_t *now;
    static int first = 0;
    if(first == 0){
        now = global_conf.forward_servers;
        first = 1;
    }
    server_t *p=NULL,*q=NULL;
    int i,min;

    if(global_conf.loadbalance_method == rr){
        p = now;
        bzero(&server_addr, sizeof(server_addr));
        server_addr.sin_family = AF_INET;   
        server_addr.sin_port = htons(p->server_port); 
        server_addr.sin_addr = p->server_addr;
        p->count++;
        if(now->next == NULL){
            now = global_conf.forward_servers;
        }else{
            now = now->next;
        }
        return server_addr;
    }
    if(global_conf.loadbalance_method == lc){
        p = global_conf.forward_servers;
        min = p->count;
        while(p != NULL) {
            if( p->count < min  ){
                q = p;
            } 
            p = p->next;
        }
        bzero(&server_addr, sizeof(server_addr));
        server_addr.sin_family = AF_INET;   
        server_addr.sin_port = htons(q->server_port); 
        server_addr.sin_addr = q->server_addr;
        q->count++;
        return server_addr;
    }
    if(global_conf.loadbalance_method == wlc){
       p = global_conf.forward_servers;
       min = p->count / p->weight;
       while(p != NULL){
           if(p->count / p->weight < min){
               q = p;
           } 
           p = q->next;
       }
       bzero(&server_addr, sizeof(server_addr));
       server_addr.sin_family = AF_INET;   
       server_addr.sin_port = htons(q->server_port); 
       server_addr.sin_addr = q->server_addr;
       q->count++;
       return server_addr;
    }
} 

