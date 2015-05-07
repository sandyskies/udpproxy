#include <sys/epoll.h>
#include <stdlib.h>
#include "common_lib.h"
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include "queue.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/timerfd.h>
#include "load_balance.h"

#define MAXEVENT 1024
#define FDSIZE 1024




void do_proxy(int listenfd);
