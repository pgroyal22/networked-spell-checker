#ifndef _DICTIONARY_H
#define _DICTIONARY_H

#include <stdbool.h>

typedef struct Dictionary dictionary;

dictionary * read_in_dictionary(char * path);
bool searchDictionary(dictionary *, char * word);
void freeDictionary(dictionary * );

#endif