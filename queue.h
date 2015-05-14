#include "stdlib.h"
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#ifndef _COMMON_LIB_H_
#define _COMMON_LIB_H_
#include "common_lib.h"
#endif


#define MAXLEN 1024
#define CAS __sync_bool_compare_and_swap
#define E_ISFULL -1
#define E_ISEMPTY -2
typedef struct
{
    struct sockaddr_in elem;
    char buffer[SO_MAX_MSG_SIZE];
    int size;
    int status;

}node;

typedef struct
{
    node elePool[MAXLEN];
    int front;
    int rear;

}queue;

enum
{
    EMPTY =1,
    FULL,

};

extern queue g_que;


void initQue();
int enque(struct sockaddr_in elem, char* buffer, int size);
int deque(struct sockaddr_in* pElem, char* buffer, int *size);


