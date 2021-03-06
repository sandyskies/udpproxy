#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>




int main(int argc, char* argv[]){
    int listen_sock;
    struct sockaddr_in server_addr;
    int ret;
    char recv_buffer[65535];
    struct sockaddr_in client_addr;
    int addrlen = sizeof(struct sockaddr_in);
    bzero(&server_addr, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    server_addr.sin_port = htons(atoi(argv[1]));
    listen_sock = socket(AF_INET,SOCK_DGRAM,0);
    bind(listen_sock,(struct sockaddr *)&server_addr, sizeof(server_addr));
    while(1){
        bzero(recv_buffer,65535);
        ret = recvfrom(listen_sock,recv_buffer,65535,0,(struct sockaddr*)&client_addr,&addrlen);
            if(ret <0){
                if (errno == EINTR){
                    continue;
                }else{
                    exit(2);
                }
            }
        //recv_buffer[ret] = '\0';
//        sleep(1);
        strcat(recv_buffer, argv[1]);
        sendto(listen_sock, recv_buffer, ret + strlen(argv[1]), 0,(struct sockaddr*)&client_addr, addrlen);
    }

}
