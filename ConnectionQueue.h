#ifndef _CONNECTIONQUEUE_H
#define _CONNECTIONQUEUE_H


struct ConnectionEvent{
    int socket_descriptor;
    int priority;
};

enum priority_mode{
    FIFO = 1,
    HIGHEST = 2
};

typedef struct ConnectionEvent connectionevent;

typedef struct ConnectionQueue connectionqueue;

void put(connectionqueue * queuePtr, connectionevent connect_event);
int get(connectionqueue * quePtr);

connectionqueue * makeConnectionQueue(int max_size, enum priority_mode priority_mode);
void freeConnectionQueue(connectionqueue * queuePtr);

#endif