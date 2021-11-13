#ifndef _LOGQUEUE_H
#define _LOGQUEUE_H

#include <stdbool.h>
#include <stdbool.h>
typedef struct LogQueue LogQueue;

struct LogEvent{
    time_t arrival_time;
    time_t finish_time;
    char * result;
    int priority;
};

void logPut(LogQueue * queuePtr, char * response);
char * logGet(LogQueue * queuePtr);
LogQueue * makeLogQueue(int max_response_elements, int max_response_size);
void freeLogQueue(LogQueue * queuePtr);

#endif