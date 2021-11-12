#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Dictionary.h"
#define MAX_WORDS 100000
#define MAX_WORD_LENGTH 128

struct Dictionary
{
    size_t size;
    char * * words;
};

dictionary * read_in_dictionary(char * path){
    FILE * dictionary_file = fopen(path, "r");
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

    size_t max_word_size = MAX_WORD_LENGTH * sizeof(char);
    char * * words = calloc(num_lines, max_word_size);
    for(int i = 0; i < num_lines; i++){
        getline(&words[i], &max_word_size, dictionary_file);
        words[i][strlen(words[i])-1] = '\0';
    }

    dictionary * dictionaryPtr = malloc(sizeof(dictionary));
    dictionaryPtr -> size = num_lines;
    dictionaryPtr -> words = words;

    return(dictionaryPtr);
}

bool binarySearch(dictionary * dictionaryPtr, int l, int r, char * word){
    if (r>=l){
        int mid = l + (r-l)/2;
        if (strcasecmp(dictionaryPtr -> words[mid], word) == 0){
            return(true);
        }
        else if (strcasecmp(dictionaryPtr -> words[mid], word) > 0){
            return binarySearch(dictionaryPtr, l, mid-1, word);
        }
        else{
            return(binarySearch(dictionaryPtr, mid+1, r, word));
        }
    }
    return false;
}

bool searchDictionary(dictionary * dictionaryPtr, char * word){
    printf("checking dictionary");
    int i = 0;
    int k = dictionaryPtr -> size - 1;
    
    return binarySearch(dictionaryPtr, i, k, word);
}

void freeDictionary(dictionary * dictionaryPtr){
    free(dictionaryPtr->words);
    free(dictionaryPtr);
    return;
}

