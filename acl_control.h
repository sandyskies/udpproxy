#include <netinet/in.h>



typedef struct access_control_s{                                               
    in_addr_t addr;                                                            
    in_addr_t mask;                                                            
    unsigned int access; //deny or acesss?                                       
}access_control_t; 


extern access_contorl_t acl_whitelist;

