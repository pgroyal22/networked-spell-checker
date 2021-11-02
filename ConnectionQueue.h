#ifndef _CONNECTIONQUEUE_H
#define _CONNECTIONQUEUE_H

typedef struct ConnectionQueue connectionqueue;

void put(connectionqueue * queuePtr, int socket_descriptor, pthread_mutex_t * lock);
int get(connectionqueue * quePtr, pthread_mutex_t * lock);

connectionqueue * makeConnectionQueue(int max_size);
void freeConnectionQueue(connectionqueue * queuePtr);

#endif