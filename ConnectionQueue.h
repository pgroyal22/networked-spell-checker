#ifndef _CONNECTIONQUEUE_H
#define _CONNECTIONQUEUE_H

#include <stdlib.h>

struct ConnectionEvent{
    int socket_descriptor;
    int priority;
    time_t receipt_time;
};

enum priority_mode{
    FIFO = 1,
    HIGHEST = 2
};

typedef struct ConnectionEvent connectionevent;

typedef struct ConnectionQueue connectionqueue;

void put(connectionqueue * queuePtr, connectionevent connect_event);
struct ConnectionEvent get(connectionqueue * quePtr);

connectionqueue * makeConnectionQueue(int max_size, enum priority_mode priority_mode);
void freeConnectionQueue(connectionqueue * queuePtr);

#endif