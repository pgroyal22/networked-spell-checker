#ifndef _LOGQUEUE_H
#define _LOGQUEUE_H

#include <stdbool.h>

typedef struct LogQueue LogQueue;

void logPut(LogQueue * queuePtr, char * response);
char * logGet(LogQueue * queuePtr);
LogQueue * makeLogQueue(int max_response_elements, int max_response_size);
void freeLogQueue(LogQueue * queuePtr);

#endif