#include "acl_control.h"
#define UNFOUND -1
#define FOUND_WHITELIST 1



int access_find(in_addr_t addr, access_control_t* rules){
    access_control_t *p = rules;
    if( p == NULL ){
        return FOUND_WHITELIST; 
    }
    while(p != NULL){
        if((addr & p->mask) == p->addr){
            return p->access;
        }
        p = p->next;
    }
    return UNFOUND;
}


