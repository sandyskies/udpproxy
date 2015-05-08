#include "conf_parse.h"

const int default_listen_port = 1753;
const lbm_t default_lb_method = rr;
const int default_timeout = 1;
const int default_process_num = 8;





char*   getKeyValue(char *filename, char *section, char *key){
    char line[1024];
    char sectname[1024];
    char *skey, *value;
    char seps[] = "=";
    bool flag = false;
    FILE *fp = fopen(filename,"r");
    assert(fp != NULL);
    //process the section and line
    memset(line,0,1024);
    if (!strchr(section, '[')){
        strcpy(sectname,"[");
        strcat(sectname,section);
        strcat(sectname,"]");
    }else{
        strcpy(sectname,section);
    }
    while (fgets(line, 1024, fp) != NULL){
        //delete the  newline
        value = strchr(line,'\n');
        *value = '\0';
        if (flag){
            skey = strtok(line, seps);
            if (strcmp(skey , key) == 0){
                return strtok(NULL,seps);
            }
        }else{
            if (strcmp(sectname,line) == 0){
                flag = true;  //find the section 
            }
        }
    }
    return NULL;
}
static int count(char** a){
    int res = 0;
    while(*a++){
        res++;
    }
    return res;
}

void parse_conf(char* conf_dir, struct conf_s *cp){
    int i = 0, j = 0, c = 0;
    char* tmp_p = NULL;  //for get config.
    char* tmp_v = NULL;
    char* p[1024];
    char* buf;
    char* out_pointer = NULL; //for split servers
    char* inner_pointer = NULL; //for split ip  port weight 
    access_control_t *ap;
    //conf_t conf_result;
    server_t *sp; //server_t link list head
    if((tmp_p = getKeyValue(conf_dir, "global","listen")) == NULL){
       log_warning("listen derective undefined, use default address 0.0.0.0 "); 
       cp->listen_addr.s_addr = htonl(INADDR_ANY);
    }else{
       
       inet_aton(tmp_p, &global_conf.listen_addr); 
    }
    tmp_p = NULL;

    if((tmp_p = getKeyValue(conf_dir, "global","port")) == NULL){
       log_warning("port derective undefined, use default address 1753 "); 
       cp->listen_port = default_listen_port;
    }else{
       cp->listen_port = atoi(tmp_p);
    }
    tmp_p = NULL;

    if((tmp_p = getKeyValue(conf_dir, "global","backends")) == NULL){
        log_error("unkonw backends, use backends directive for configuration."); 
        exit(-1);
    }else{
        buf = tmp_p;
        while(p[i] = strtok_r(buf,",", &out_pointer )){
            buf = p[i];
            while((p[i] = strtok_r(buf, ":",&inner_pointer)) != NULL){
                i++;
                buf = NULL;
            }
            buf = NULL; 
        }
        c = count(p) ; //how many servers
        sp = (server_t *)malloc((c/3)*sizeof(server_t));
        for(i=0,j=0;j < c; j++,i++){
            inet_aton(p[j], &(sp[i].server_addr));
            sp[i].server_port = atoi(p[++j]);
            sp[i].weight = (unsigned int)atoi(p[++j]);
            if(i == c/3){
                sp[i].next = NULL;
            }else{
                sp[i].next = &sp[i+1]; 
                
            }
            sp[i].count = 0;
        }
      
    }
    tmp_p = NULL;

    if((tmp_p = getKeyValue(conf_dir, "global","lb_method")) == NULL){
       log_warning("Unkonw loadbalance method ,use default round robin."); 
       cp->loadbalance_method = (lbm_t)1;
    }else{
        i = atoi(tmp_p);
        switch(i){
            case 1:
                cp->loadbalance_method = (lbm_t)1;
                break;
            case 2:
                cp->loadbalance_method = (lbm_t)2; /*todo*/
                break;
            case 3:
                cp->loadbalance_method = (lbm_t)3;
                break;
            case 4:
                cp->loadbalance_method = (lbm_t)4;
                break;
            default:
                log_warning("unknow lb method, use default round robin");
                cp->loadbalance_method = (lbm_t)1;
        }
    }
    tmp_p = NULL;
    if((tmp_p = getKeyValue(conf_dir, "global","timeout")) == NULL){
       log_warning("unconfigured timeout ,use default 1s"); 
       cp->timeout = default_timeout;
    }else{
       cp->timeout = atoi(tmp_p);
    }   
    tmp_p = NULL;
    if((tmp_p = getKeyValue(conf_dir, "global","worker_num")) == NULL){
       log_warning("unconfigured worker_num ,use default 8"); 
       cp->process_num = default_process_num;
    }else{
       cp->process_num = atoi(tmp_p);
    }   
    tmp_p = NULL;
    if((tmp_p = getKeyValue(conf_dir, "global","white_lists")) == NULL){
       log_warning("unconfigured w whilte_lists, all is able"); 
     // bzero(conf_result.acl_rules,sizeof(conf_result.acl_rules));
        cp->acl_rules = NULL;
    }else{
        for(i=0; i<1024; i++){
            p[i] = NULL;
        }
        i = 0;
        out_pointer = NULL;
        inner_pointer = NULL;
        buf = tmp_p;
        while(p[i]=strtok_r(buf, ",", &out_pointer)){
            buf = p[i];
            while((p[i] = strtok_r(buf, "/", &inner_pointer)) != NULL){
                i++;
                buf = NULL;
            }
            buf = NULL;
        }
        c = count(p);
        ap =(access_control_t *) malloc((c/2)* sizeof(access_control_t));
        for(i=0,j=0; j <c; j++,i++){
            ap[i].addr = inet_addr(p[j]);
            ap[i].mask = inet_addr(p[++j]);
            if(i == c/2 ){
                ap[i].next = NULL;
            }else{
                ap[i].next = &ap[i+1];
            }
            ap[i].access = 1;
        }
    }   
    
}


