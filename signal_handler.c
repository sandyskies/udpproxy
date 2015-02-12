#include "signal_handler.h"
#ifndef _COMMON_LIB_H_
#define _COMMON_LIB_H_
#endif



void clean_up(int signo){

}


void sighup_handler(int signo){
    reload_conf();
}
