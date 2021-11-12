#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <stdio.h>
#include <pthread.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include "Dictionary.h"
#include "ConnectionQueue.h"

#define DEFAULT_DICTIONARY "/usr/share/dict/words/american-english"
#define DEFAULT_PORT "2107"
#define DEFAULT_NUM_THREADS 4

dictionary * dictionaryPtr; 
connectionqueue * connectionqueuePtr;

struct controlParams
{
    char * DICTIONARY; 
    size_t CONNECTION_BUFFER_SIZE;
    size_t N_THREADS;
    char * PORT;
    enum priority_mode PRIORITY_MODE;
}controlParams;

// got from textbook for network initalization
int open_listenfd(char * port){
    struct addrinfo hints, *listp, *p;
    int listenfd, optval = 1; 

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;                              // socket accepts connections
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG | AI_NUMERICSERV; // on any address
    getaddrinfo(NULL, port, &hints, &listp);

    for(p = listp; p; p = p->ai_next){
        if((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol) < 0)){
            continue; // socket failed
        }
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *) &optval, sizeof(int));

        if(bind(listenfd, p->ai_addr, p->ai_addrlen) == 0){
            break; // socket bound to address
        }
        else{
            close(listenfd); // socket failed to bind
        }
    }

    freeaddrinfo(listp); 
    if(!p) // no address worked, no socket available and bound
        return -1;
    
    // makes it a listening  socket for n connection requests
    if(listen(listenfd, controlParams.CONNECTION_BUFFER_SIZE) < 0){
        close(listenfd);
        return -1;
    }

    return listenfd;
}


void * workerThread(void * arg){
    char word[256];
    char response[256];
    while(true){
        int socket_fd = get(connectionqueuePtr);
        memset(word, '\0', 256);
        printf("%d\n", socket_fd);
        while(read(socket_fd, word, 256) > 0){
            int i = 0;
            memset(response, '\0', 256);
            while(word[i] != '\000'){
                if(((word[i] == '\r') || (word[i]) == '\t' || (word[i]) == '\n')){
                    word[i] = '\0';
                }
                i++;
            }
            printf("read from open socket\n");
            if(searchDictionary(dictionaryPtr, word)){
                strcpy(response, word);
                strcat(response, " OK\n");
            }
            else{
                strcpy(response, word);
                strcat(response, " MISPELLED\n");
            }
            printf("writing to open socket\n");
            write(socket_fd, response, 256);
        }
        close(socket_fd);
    }
}

void loggerThread(){
    //TODO: make this actually do somethig 
}


void spawn_worker_threads(){
    // create an array that holds worker threads
    pthread_t threads[controlParams.N_THREADS];
    // create the worker threads, which begin execution at workerThread function
    for(size_t i = 0; i < controlParams.N_THREADS; i++){
        if(pthread_create(&threads[i], NULL, workerThread, NULL) != 0){
        printf("Error: Failed to create thread\n ");
        exit(EXIT_FAILURE);
        }
        else{
        printf("thread created\n");
        fflush(stdout);
        }
    }
}

int main(int argc, char const *argv[])
{
    // control param handling
    if (argc < 2){
        controlParams.DICTIONARY = DEFAULT_DICTIONARY;
        controlParams.PORT = DEFAULT_PORT;
        controlParams.N_THREADS = DEFAULT_NUM_THREADS;
        controlParams.CONNECTION_BUFFER_SIZE = 10;
        controlParams.PRIORITY_MODE = FIFO;
    }

    // load into dictionary data structure
    char * dictionary_path = controlParams.DICTIONARY;
    dictionaryPtr = read_in_dictionary(dictionary_path);

    // create a connection queue
    connectionqueuePtr = makeConnectionQueue(controlParams.CONNECTION_BUFFER_SIZE, controlParams.PRIORITY_MODE);

    // network initialization
    int listenfd, connectionfd;
    if((listenfd = open_listenfd(controlParams.PORT) < 0)){
        perror("not listening on any ports");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_storage clientaddr;
    socklen_t clientlen = sizeof(struct sockaddr_storage);
    
    // create our worker threads
    spawn_worker_threads();
    
    // start of main thread separation from worker threads
    while(true){
        connectionfd = accept(listenfd, (struct sockaddr *)&clientaddr, (socklen_t*) &clientlen);
        if(connectionfd < 0){
            continue;
        }
        else{
            printf("connection accepted with fd %d\n", connectionfd);
            connectionevent connection_event = {connectionfd , rand() % 11};
            put(connectionqueuePtr, connection_event);
        }
    }

    freeDictionary(dictionaryPtr);
    freeConnectionQueue(connectionqueuePtr);
    exit(EXIT_SUCCESS);
}
