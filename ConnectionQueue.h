#ifndef _CONNECTIONQUEUE_H
#define _CONNECTIONQUEUE_H

typedef struct ConnectionQueue connectionqueue;

void put(connectionqueue * queuePtr, int socket_descriptor);
int get(connectionqueue * quePtr);

connectionqueue * makeConnectionQueue(int max_size);
void freeConnectionQueue(connectionqueue * queuePtr);

#endif