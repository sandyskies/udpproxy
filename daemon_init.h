#include    <stdlib.h>
#include    <sys/time.h>
#include    <sys/resource.h>
#include    <syslog.h>
#include    <signal.h>
#include    <sys/types.h>
#include    <sys/stat.h>
#include    <fcntl.h>
#include "sys/types.h"
#ifndef _COMMON_LIB_H
#define _COMMON_LIB_H
#endif



int daemon_init(const char *, int);
