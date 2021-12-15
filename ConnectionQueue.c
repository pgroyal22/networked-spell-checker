#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include "ConnectionQueue.h"

struct ConnectionQueue
{
    connectionevent * socket_buffer;
    int fill_position;
    int use_position;
    int count;
    int max_size;
    enum priority_mode priority_mode;
};

pthread_cond_t sockets_available;
pthread_cond_t space_available; 
pthread_mutex_t lock;

void put(connectionqueue * queuePtr, connectionevent connection_event){
    pthread_mutex_lock(&lock);
    
    printf("got mutex for putting to queue\n");

    while(queuePtr -> count == queuePtr -> max_size){
        printf("gave up mutex for getting from queue\n");
        pthread_cond_wait(&space_available, &lock);
    }
    // start of critical section
    printf("putting connection to connection queue\n");
    queuePtr -> socket_buffer[queuePtr -> fill_position] = connection_event; 
    queuePtr -> fill_position = (queuePtr -> fill_position + 1) % (queuePtr -> max_size);
    queuePtr -> count++;

    // signals that there is now data available
    pthread_cond_signal(&sockets_available);
    // frees mutex
    pthread_mutex_unlock(&lock);
}

connectionevent get(connectionqueue * queuePtr){
    pthread_mutex_lock(&lock);

    printf("got mutex for getting from queue\n");

    while (queuePtr -> count == 0){
        printf("gave up mutex for getting from queue\n");
        pthread_cond_wait(&sockets_available, &lock);
    }
    
    // start of critical section
    printf("pulling from connection queue\n");
    connectionevent tmp = queuePtr -> socket_buffer[queuePtr -> use_position]; 
    queuePtr -> use_position = (queuePtr -> use_position + 1) % (queuePtr -> max_size);
    queuePtr -> count--;

    

    // signal that queue can accept more sockets, release mutex
    pthread_cond_signal(&space_available);

    // frees mutex
    pthread_mutex_unlock(&lock);
    
    return tmp;
}

connectionqueue * makeConnectionQueue(int max_size){
    connectionqueue * queuePtr = malloc(sizeof(connectionqueue));
    connectionevent * _socket_buffer = malloc(max_size * sizeof(connectionevent)); 
    queuePtr -> socket_buffer = _socket_buffer; 
    queuePtr -> fill_position = 0;
    queuePtr -> use_position = 0;
    queuePtr -> count = 0;
    queuePtr -> max_size = max_size;

    // condition variable initalization 
    pthread_cond_init(&space_available, NULL);
    pthread_cond_init(&sockets_available, NULL);

    // &lock initialization
    pthread_mutex_init(&lock, NULL);
    return queuePtr;
}

void freeConnectionQueue(connectionqueue * queuePtr){
    free(queuePtr -> socket_buffer);
    free(queuePtr);
    pthread_cond_destroy(&space_available);
    pthread_cond_destroy(&sockets_available);
    pthread_mutex_destroy(&lock);
}
