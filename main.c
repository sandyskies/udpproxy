#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h> //for exit(0)
#include <errno.h> //for perror
#include <sys/socket.h>
#include <pthread.h>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h> //for getopt
#include "daemon_init.h"
#include "proxy.h"
#include "conf_parse.h"
#ifndef _COMMON_LIB_H_
#define _COMMON_LIB_H_
#include "common_lib.h"
#endif




/* 
 *Author: ChenMingjie  
 *Mail: chenmingjie0828@163.com
 */

void print_help(void){
    printf("usage: ./udpproxy -c path_to_configure_file -d -h\n");
    printf("-d if to daemonize.\n");
    printf("-c identify the path of configure file\n");
    printf("-h to print this help\n");
    printf("-D if Debug or not\n");


}

void check_error(const char *reason){
    if(errno != 0 ){
        perror(reason);
        exit(errno);
    }
    
}

void do_main_loop(){
    int on = 1;  
    int listen_sock;
    pid_t pid;
    struct sockaddr_in server_addr;

    listen_sock = socket(AF_INET,SOCK_DGRAM,0);
    check_error("Error happens while create socket");
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    bzero(&server_addr, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr = global_conf.listen_addr;
    server_addr.sin_port = htons(global_conf.listen_port);
    bind(listen_sock, (struct sockaddr *) &server_addr, sizeof(server_addr));
    check_error("Error happends while binding socket.");
    do_proxy(listen_sock); 
}




int main(int argc, char* argv[]){
    int ch;
    char* conf_dir;
    bool daemonize = false;
    const char* pname = "udpproxy";
    
    while((ch=getopt(argc,argv,"c:dhD"))!=-1){
        switch(ch){
            case 'c':
                conf_dir = optarg;
                break;
            case 'd':
                daemonize = true;
                break;
            case 'D':
                DEBUG = true;
                break;
            case 'h':
                print_help();
                exit(0);
            default:
                print_help();
                exit(0);
        }
    }     
    if (access(conf_dir, 0) == 0){
        parse_conf(conf_dir, &global_conf); 
    }
    if(daemonize){
        daemon_init(pname,LOG_USER); 
    }
    do_main_loop();
    return 0;
}
