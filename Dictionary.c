#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Dictionary.h"
#define MAX_WORDS 100000

struct Dictionary
{
    size_t size;
    char * * words;
};

dictionary * read_in_dictionary(char * path){
    FILE * dictionary_file = fopen("dictionary_path", "r");
    if(dictionary_file == NULL){
        perror("dictionary");
        exit(EXIT_FAILURE);
    }
    // finding out how long word array will be
    size_t num_lines = 0;
    while(!feof(dictionary_file))
    {
        char ch = fgetc(dictionary_file);
        if(ch == '\n')
        {
            num_lines++;
        }
    }
    rewind(dictionary_file);

    size_t buffer_size = 1024;
    char * buffer = malloc(sizeof(char) * buffer_size);
    char * * words = malloc(num_lines*sizeof(char*));
    for(int i = 0; i < num_lines; i++){
        words[i] = getline(&buffer, &buffer_size, dictionary_file);
    }

    dictionary * dictionaryPtr = malloc(sizeof(dictionary *));
    dictionaryPtr -> size = num_lines;
    dictionaryPtr -> words = words;

    free(buffer);
    return(dictionaryPtr);
}

bool searchDictionary(dictionary * dictionaryPtr, char * word){

    int i = 0;
    int k = dictionaryPtr -> size - 1;
    
    return binarySearch(dictionaryPtr, i, k, word);
}

bool binarySearch(dictionary * dictionaryPtr, int l, int r, char * word){
    if (r>=l){
        int mid = l + (r-l)/2;
        if (strcmp(dictionaryPtr -> words[mid], word) == 0){
            return(true);
        }
        else if (strcmp(dictionaryPtr -> words[mid], word) > 0){
            return binarySearch(dictionaryPtr, l, mid-1, word);
        }
        else{
            return(binarySearch(dictionaryPtr, mid+1, r, word));
        }
    }
    return false;
}

void freeDictionary(dictionary * dictionaryPtr){
    free(dictionaryPtr->words);
    free(dictionaryPtr);
    return;
}

