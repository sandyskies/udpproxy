#include    <stdlib.h>
#include    <sys/time.h>
#include    <sys/resource.h>
#include    <syslog.h>
#include    <signal.h>
#include    <sys/types.h>
#include    <sys/stat.h>
#include    <fcntl.h>
#include    <unistd.h>
#include <stdio.h>

#ifndef _COMMON_LIB_H_
#define _COMMON_LIB_H_
#include "common_lib.h"
#endif




int daemon_init(const char *, int);
