#include "stdlib.h"
#define MAXLEN 1024
#define CAS __sync_bool_compare_and_swap
#define E_ISFULL -1
#define E_ISEMPTY -2
#include <sys/socket.h>

typedef struct
{
    struct sockaddr elem;
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
int enque(struct sockaddr elem);
int deque(struct sockaddr* pElem);


