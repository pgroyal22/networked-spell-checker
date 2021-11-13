#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>

#include "LogQueue.h"
struct LogQueue
{
    char * * response_buffer;
    int fill_position;
    int use_position;
    int count;
    int max_response_size;
    int max_response_elements;
};

pthread_cond_t response_available;
pthread_cond_t response_space_available; 
pthread_mutex_t log_lock;

void logPut(LogQueue * queuePtr, char * response){
    pthread_mutex_lock(&log_lock);
    
    while(queuePtr -> count == queuePtr -> max_response_size){
        pthread_cond_wait(&response_space_available, &log_lock);
    }
    // start of critical section
    printf("putting to log queue\n");
    queuePtr -> response_buffer[queuePtr -> fill_position] = response; 
    queuePtr -> fill_position = (queuePtr -> fill_position + 1) % (queuePtr -> max_response_size);
    queuePtr -> count++;
    
    // signals that there is now data available
    pthread_cond_signal(&response_available);
    // frees mutex
    pthread_mutex_unlock(&log_lock);
}

char * logGet(LogQueue * queuePtr){
    pthread_mutex_lock(&log_lock);

    while (queuePtr -> count == 0){
        pthread_cond_wait(&response_available, &log_lock);
    }
    
    // start of critical section
    printf("getting from log queue\n");
    char * tmp = queuePtr -> response_buffer[queuePtr -> use_position]; 
    queuePtr -> use_position = (queuePtr -> use_position + 1) % (queuePtr -> max_response_size);
    queuePtr -> count--;

    // signal that queue can accept more sockets, release mutex
    pthread_cond_signal(&response_space_available);

    // frees mutex
    pthread_mutex_unlock(&log_lock);
    
    return tmp;
}

LogQueue * makeLogQueue(int max_response_elements, int max_response_size){
    LogQueue * queuePtr = malloc(sizeof(LogQueue));
    queuePtr -> fill_position = 0;
    queuePtr -> use_position = 0;
    queuePtr -> count = 0;
    queuePtr -> max_response_elements = max_response_elements;
    queuePtr -> max_response_size = max_response_size;
    char * *_response_buffer = malloc(max_response_elements * max_response_size * sizeof(char *)); 
    queuePtr -> response_buffer = _response_buffer; 

    // condition variable initalization 
    pthread_cond_init(&response_space_available, NULL);
    pthread_cond_init(&response_available, NULL);

    // &log_lock initialization
    pthread_mutex_init(&log_lock, NULL);
    return queuePtr;
}

void freeLogQueue(LogQueue * queuePtr){
    free(queuePtr -> response_buffer);
    free(queuePtr);
    pthread_cond_destroy(&response_space_available);
    pthread_cond_destroy(&response_available);
    pthread_mutex_destroy(&log_lock);
}
