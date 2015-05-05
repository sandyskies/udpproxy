#include <netinet/in.h>
#include<stdlib.h>



typedef struct access_control_s{                                               
    in_addr_t addr;                                                            
    in_addr_t mask;                                                            
    unsigned int access; //deny or acesss?                                       
    struct access_control_s *next;
}access_control_t; 


extern int access_find(in_addr_t addr, access_control_t* rules);
