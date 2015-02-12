#include "conf_parse.h" 
#ifndef _COMMON_LIB_H_
#define _COMMON_LIB_H_
#endif
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>




void print_help(void){
    printf("usage: ./%s -c path_to_configure_file -d -h\n");
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
    struct sockaddr_in server_addr;
    listen_sock = socket(AF_INET,SOCK_DGRAM,0);
    pid_t pid;
    check_error("Error happens while create socket");
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    bzero(server_addr, sizeof(sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr = global_conf.server_addr;
    server_addr.sin_port = htons(global_conf.server_port)
    bind(listen_sock, (sockaddr *) &server_addr, sizeof(server_addr));
    check_error("Error happends while binding socket.");
    for(int i=0; i<global_conf.process_num; i++){
        pid = fork();
        if(pid>0){
            /*child process*/

        }else if(pid <0){
            /*error */
            printf("Error while forking child process.\n");
            exit(1);
           
        }else{
            /*parrent wait for child process. */

        }
        
    }
    




    
}




int main(int argc, char*[] argv){
    int ch;
    char* conf_dir;
    bool daemonize = false;
    const char* pname = "udpproxy"
    
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
        global_conf = parse_conf(conf_dir);
    }
    if(daemonize){
        daemonize_init(pname,LOG_USER); 
    }
    do_main_loop();
    return 0;
}
