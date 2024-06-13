#define _GNU_SOURCE
#include "my_secmalloc.private.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <stdarg.h>

#define TAILLE_CANARY 16
#define TAILLE_POOL 1024*1024*1024

void *pool_data = NULL;
void *pool_meta = NULL;
metadata_t *liste_libre = NULL;
FILE *log_file = NULL;

void generer_canary(unsigned char *canary) {
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(1);
    }
    if (read(fd, canary, TAILLE_CANARY) != TAILLE_CANARY) {
        perror("read");
        exit(1);
    }
    close(fd);
}

void init_log_file(void) {
    const char *log_path = getenv("MSM_OUTPUT");
    if (log_path) {
        log_file = fopen(log_path, "a");
        if (!log_file) {
            perror("fopen");
            exit(1);
        }
    }
}

void ecrire_log(const char *format, ...) { // changer la fonction
    if (log_file) {
        va_list args;
        va_start(args, format);

        char buffer[512];
        vsprintf(buffer, format, args);
        fprintf(log_file, "%s\n", buffer);

        va_end(args);
        fflush(log_file);
    }
}

void init_pools(void) {
    pool_data = mmap(NULL, TAILLE_POOL, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (pool_data == MAP_FAILED) {
        perror("mmap pool_data");
        exit(1);
    }
    pool_meta = mmap(NULL, TAILLE_POOL, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (pool_meta == MAP_FAILED) {
        perror("mmap pool_meta");
        exit(1);
    }

    liste_libre = (metadata_t *)pool_meta;
    liste_libre->precedent = NULL;
    liste_libre->suivant = NULL;
    liste_libre->pointeur_data = pool_data;
    liste_libre->taille = TAILLE_POOL;
    liste_libre->est_alloue = false;
    generer_canary(liste_libre->canary);
    liste_libre->pointeur_canary = (void *)((char *)liste_libre->pointeur_data + liste_libre->taille - TAILLE_CANARY);
}

void *my_malloc(size_t taille) {
    if (taille == 0) {
        return NULL;
    }

    metadata_t *bloc_courant = liste_libre;
    while (bloc_courant) {
        if (bloc_courant->taille >= taille && !bloc_courant->est_alloue) {
            bloc_courant->est_alloue = true;
            ecrire_log("malloc: Taille: %zu, Adresse: %p", taille, bloc_courant->pointeur_data);
            return bloc_courant->pointeur_data;
        }
        bloc_courant = bloc_courant->suivant;
    }

    metadata_t *nouveau_bloc_meta = mmap(NULL, sizeof(metadata_t), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (nouveau_bloc_meta == MAP_FAILED) {
        perror("mmap metadata_t");
        exit(1);
    }
    nouveau_bloc_meta->precedent = NULL;
    nouveau_bloc_meta->suivant = NULL;
    nouveau_bloc_meta->pointeur_data = mmap(NULL, taille, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (nouveau_bloc_meta->pointeur_data == MAP_FAILED) {
        perror("mmap data");
        exit(1);
    }
    nouveau_bloc_meta->taille = taille;
    nouveau_bloc_meta->est_alloue = true;
    generer_canary(nouveau_bloc_meta->canary);
    nouveau_bloc_meta->pointeur_canary = (void *)((char *)nouveau_bloc_meta->pointeur_data + nouveau_bloc_meta->taille - TAILLE_CANARY);

    nouveau_bloc_meta->suivant = liste_libre;
    if (liste_libre) {
        liste_libre->precedent = nouveau_bloc_meta;
    }
    liste_libre = nouveau_bloc_meta;
    ecrire_log("malloc: Taille: %zu, Adresse: %p", taille, nouveau_bloc_meta->pointeur_data);
    return nouveau_bloc_meta->pointeur_data;
}

void my_free(void *ptr) {
    if (ptr == NULL) return;

    metadata_t *bloc_courant = liste_libre;
    while (bloc_courant) {
        if (bloc_courant->pointeur_data == ptr) {
            if (!bloc_courant->est_alloue) {
                return;
            }

            unsigned char *canary_ptr = (unsigned char *)bloc_courant->pointeur_canary;
            for (size_t i = 0; i < TAILLE_CANARY; ++i) {
                if (canary_ptr[i] != bloc_courant->canary[i]) {
                   fprintf(stderr, "Erreur: Canary altéré pour le bloc à l'adresse %p\n", ptr);
                    return;
                }
            }

            bloc_courant->est_alloue = false;
            ecrire_log("free: Taille: %zu, Adresse: %p", bloc_courant->taille, ptr);
            return;
        }
        bloc_courant = bloc_courant->suivant;
    }
}

void *my_calloc(size_t nmemb, size_t taille) {
    size_t taille_totale = nmemb * taille;
    ecrire_log("calloc: Taille: %zu", taille_totale);
    void *ptr = my_malloc(taille_totale);
    if (ptr) {
        memset(ptr, 0, taille_totale);
    }
    return ptr;
}

void *my_realloc(void *ptr, size_t nouvelle_taille) {
    if (ptr == NULL) {
        return my_malloc(nouvelle_taille);
    }

    if (nouvelle_taille == 0) {
        my_free(ptr);
        return NULL;
    }

    metadata_t *bloc_courant = liste_libre;
    while (bloc_courant) {
        if (bloc_courant->pointeur_data == ptr) {
            if (!bloc_courant->est_alloue) {
                return NULL;
            }

            if (bloc_courant->taille >= nouvelle_taille) {
                ecrire_log("realloc: Taille: %zu, Adresse: %p", nouvelle_taille, ptr);
                return ptr;
            }

            void *new_ptr = my_malloc(nouvelle_taille);
            if (new_ptr) {
                memcpy(new_ptr, ptr, bloc_courant->taille);
                my_free(ptr);
            }
            ecrire_log("realloc: Taille: %zu, Nouvelle Adresse: %p", nouvelle_taille, new_ptr);
            return new_ptr;
        }
        bloc_courant = bloc_courant->suivant;
    }
    return NULL;
}

#ifdef DYNAMIC
void *malloc(size_t size) {
    return my_malloc(size);
}
void free(void *ptr) {
    my_free(ptr);
}
void *calloc(size_t nmemb, size_t size) {
    return my_calloc(nmemb, size);
}
void *realloc(void *ptr, size_t size) {
    return my_realloc(ptr);
}
#endif

