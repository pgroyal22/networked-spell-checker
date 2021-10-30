#include <stdlib.h>
#include <pthread.h>
#include "ConnectionQueue.h"

struct ConnectionQueue
{
    int * socket_buffer;
    int fill_position;
    int use_position;
    int count;
    int max_size; 
};

void put(connectionqueue * queuePtr, int socket_descriptor, pthread_mutex_t lock){
    queuePtr -> socket_buffer[queuePtr -> fill_position] = socket_descriptor; 
    queuePtr -> fill_position = (queuePtr -> fill_position + 1) % (queuePtr -> max_size);
    queuePtr -> count++;
}

int get(connectionqueue * queuePtr, pthread_mutex_t lock){
    int tmp = queuePtr -> socket_buffer[queuePtr -> use_position]; 
    queuePtr -> use_position = (queuePtr -> use_position + 1) % (queuePtr -> max_size);
    queuePtr -> count--;
    return tmp;
}

connectionqueue * makeConnectionQueue(int max_size){
    connectionqueue * queuePtr = malloc(sizeof(connectionqueue));
    int * socket_buffer = malloc(max_size * sizeof(int)); 
    queuePtr -> socket_buffer = socket_buffer; 
    queuePtr -> fill_position = 0;
    queuePtr -> use_position = 0;
    queuePtr -> count = 0;
    queuePtr -> max_size = max_size; 

    return queuePtr;
}

void freeConnectionQueue(connectionqueue * queuePtr){
    free(queuePtr -> socket_buffer);
    free(queuePtr);
}
