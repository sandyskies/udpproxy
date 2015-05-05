#include "acl_control.h"
#define UNFOUND -1
#define FOUND_WHITELIST 1


/*int access_find(in_addr_t addr, access_control_t* rules, size_t rules_size){
    for(int i=0; i < rules_size, i++){
        if((addr & rules[i].mask) == rules[i].addr){
            return rules[i].access; //found in white or blacklist
        }
    }
    return -1;//unfound. 
} */
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

//we use nginx acl contorl agrithm

/*static ngx_int_t
ngx_http_access_inet(ngx_http_request_t *r, ngx_http_access_loc_conf_t *alcf,
            in_addr_t addr)
{
        ngx_uint_t               i;
            ngx_http_access_rule_t  *rule;

                rule = alcf->rules->elts;
                for (i = 0; i < alcf->rules->nelts; i++) {

                            ngx_log_debug3(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                                                           "access: %08XD %08XD %08XD",
                                                                                  addr, rule[i].mask, rule[i].addr);

                            if ((addr & rule[i].mask) == rule[i].addr) {
                                            return ngx_http_access_found(r, rule[i].deny);
                                                    
                            }
                                
                }

                    return NGX_DECLINED;

}*/

