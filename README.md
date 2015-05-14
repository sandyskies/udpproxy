# udpproxy 

Introduction
---
     This is proxy server for udp backend servers, with access control, load balance ,timeout , and retransmission.

Compile
---
    make clean; make 

Configuration
---
    [global]
    ;listen directive is used for the address to listen on , 0.0.0.0 stands for all address in the machine.
    listen=127.0.0.1
    ; port directive is used for the listen port.
    port=8891
    ;backends directive:
    ;format: ip:port:weight,ip:port:weight...
    ;weight is used for load balance.
    backends=127.0.0.1:8893:2,127.0.0.1:8892:4
    ;lb_method: the algorithm use for load balance,
    ;1 stands for round robin.
    ;2 weight round robin, uncompleted.
    ;3 least connection
    ;4 weight least connection
    lb_method=1
    ;timeout: in seconds, and if timeout happends and backend didn't response, we retrans another time.
    ;both time didn't response ,we give up
    timeout=3
    ;worker_num: threads to work. 
    worker_num=4
    ;white_list:
    ;format:ip/mask,ip/mask
    ;only the clients in thoses subnet are able to send request, if not, the data will be ignored. 
    white_lists=10.0.0.0/255.0.0.0,172.16.0.0/255.240.0.0,127.0.0.1/255.255.255.255
    
    Note: We didn't use library to parse, Please to keep the exact format in the conf.ini. Do not add comment line
    or new line in the configuration.
    
usage
---
    usage: ./udpproxy -c path_to_configure_file -d -h
    -d if to daemonize.
    -c identify the path of configure file
    -h to print this help
    -D if Debug or not
    
    ./udpproxy -c conf.ini -d
    
log
---
    we use syslog to log. U need to add this line to the syslog configuration and restart the rsyslog or syslog service. 
    user.* /var/log/udpproxy.log

test:
---
    cd test 
    ./bcs 8892 &
    ./bcs 8893 &
    ./cli 8891
