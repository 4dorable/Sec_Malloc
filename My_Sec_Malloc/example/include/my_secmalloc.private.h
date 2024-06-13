#ifndef _SECMALLOC_PRIVATE_H
#define _SECMALLOC_PRIVATE_H

#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>

#define TAILLE_CANARY 16
#define TAILLE_POOL 1024*1024*1024

typedef struct metadata {
    struct metadata *precedent;
    struct metadata *suivant;
    void *pointeur_data;
    size_t taille;
    bool est_alloue;
    unsigned char canary[TAILLE_CANARY];
    void *pointeur_canary;
} metadata_t;

extern void *pool_data;
extern void *pool_meta;
extern metadata_t *liste_libre;

void generer_canary(unsigned char *canary);
void init_log_file(void);
void ecrire_log(const char *type, ...);
void init_pools(void);

#endif // _SECMALLOC_PRIVATE_H
