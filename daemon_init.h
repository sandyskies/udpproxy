#include    <stdlib.h>
#include    <sys/time.h>
#include    <sys/resource.h>
#include    <syslog.h>
#include    <signal.h>
#include    <sys/types.h>
#include    <sys/stat.h>
#include    <fcntl.h>



int daemon_init(const char *, int);
