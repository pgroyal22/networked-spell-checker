#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include "ConnectionQueue.h"

struct ConnectionQueue
{
    int * * socket_buffer;
    int fill_position;
    int use_position;
    int count;
    int max_size;
};

pthread_cond_t sockets_available;
pthread_cond_t space_available; 

void put(connectionqueue * queuePtr, int socket_descriptor, pthread_mutex_t * lockPtr){
    printf("trying lock in put\n");
    fflush(stdout);
    pthread_mutex_lock(lockPtr);
    
    while(queuePtr -> count == queuePtr -> max_size){
        pthread_cond_wait(&space_available, lockPtr);
    }
    // start of critical section
    *queuePtr -> socket_buffer[queuePtr -> fill_position] = socket_descriptor; 
    queuePtr -> fill_position = (queuePtr -> fill_position + 1) % (queuePtr -> max_size);
    queuePtr -> count++;

    // signals that there is now data available
    pthread_cond_signal(&sockets_available);
    // frees mutex
    pthread_mutex_unlock(lockPtr);

    printf("connection put to queue");  
    fflush(stdout);
}

int get(connectionqueue * queuePtr, pthread_mutex_t * lockPtr){
    pthread_mutex_lock(lockPtr);
    while (queuePtr -> count == 0){
        pthread_cond_wait(&sockets_available, lockPtr);
    }
    // start of critical section
    int * tmp = queuePtr -> socket_buffer[queuePtr -> use_position]; 
    queuePtr -> use_position = (queuePtr -> use_position + 1) % (queuePtr -> max_size);
    queuePtr -> count--;

    // signal that queue can accept more sockets, release mutex
    pthread_cond_signal(&space_available);

    // frees mutex
    pthread_mutex_unlock(lockPtr);
    
    printf("connection pulled from queue");
    return *tmp;
}

connectionqueue * makeConnectionQueue(int max_size){
    connectionqueue * queuePtr = malloc(sizeof(connectionqueue));
    int * * _socket_buffer = malloc(max_size * sizeof(int)); 
    queuePtr -> socket_buffer = _socket_buffer; 
    queuePtr -> fill_position = 0;
    queuePtr -> use_position = 0;
    queuePtr -> count = 0;
    queuePtr -> max_size = max_size;

    // condition variable initalization 
    pthread_cond_init(&space_available, NULL);
    pthread_cond_init(&sockets_available, NULL);

    return queuePtr;
}

void freeConnectionQueue(connectionqueue * queuePtr){
    free(queuePtr -> socket_buffer);
    free(queuePtr);
    pthread_cond_destroy(&space_available);
    pthread_cond_destroy(&sockets_available);
}
