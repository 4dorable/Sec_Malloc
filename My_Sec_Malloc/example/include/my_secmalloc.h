#ifndef _MY_SECMALLOC_H
#define _MY_SECMALLOC_H

#include <stddef.h> // Pour size_t
#include <stdbool.h> // Pour bool

void *my_malloc(size_t taille);
void my_free(void *ptr);
void *my_calloc(size_t nmemb, size_t taille);
void *my_realloc(void *ptr, size_t taille);

#endif // _MY_SECMALLOC_H

