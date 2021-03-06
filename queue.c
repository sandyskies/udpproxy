#include "queue.h"

queue g_que;

void initQue()
{
    int i = 0;
    g_que.front = 0;
    g_que.rear  = 0;
    
    for(i=0;i<MAXLEN;i++)
    {
        g_que.elePool[i].status = EMPTY;
    }
    return;
}

int enque(struct sockaddr_in elem, char* buffer, int size)
{
    do
    {
        if((g_que.rear+1)%MAXLEN == g_que.front)
        {
            return E_ISFULL;
        }
    }while(!CAS(&(g_que.elePool[g_que.rear].status),EMPTY,FULL));
    g_que.elePool[g_que.rear].elem = elem;
    g_que.elePool[g_que.rear].size = size;
    memcpy(g_que.elePool[g_que.rear].buffer, buffer, size);
    CAS(&(g_que.rear),g_que.rear,(g_que.rear+1)%MAXLEN);
    
    return 0;
}

int deque(struct sockaddr_in* pElem, char* buffer, int *size)
{
    do
    {
        if(g_que.rear == g_que.front)
        {
            return E_ISEMPTY;
        }
    }while(!CAS(&(g_que.elePool[g_que.front].status),FULL,EMPTY));
    //*pElem = g_que.elePool[g_que.front].elem;
    memcpy(pElem, &g_que.elePool[g_que.front].elem ,sizeof(struct sockaddr_in));
    memcpy(buffer, g_que.elePool[g_que.front].buffer,g_que.elePool[g_que.front].size);
    *size = g_que.elePool[g_que.front].size;
    bzero(&g_que.elePool[g_que.front].buffer,SO_MAX_MSG_SIZE);
    bzero(&g_que.elePool[g_que.front].elem,sizeof(struct sockaddr_in));
    CAS(&(g_que.front),g_que.front,(g_que.front+1)%MAXLEN);
    return 0;
}
