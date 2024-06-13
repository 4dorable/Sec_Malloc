#include <stdio.h>
#include <stdlib.h>
#include <criterion/criterion.h>
#include "my_secmalloc.h"

Test(my_secmalloc, test_malloc) // simple allocation
{
   printf("\nTEST 01\n\n");
   void *ptr = my_malloc(13);
   cr_assert(ptr != NULL, "Failed to mmap.");
   my_free(ptr);
   printf("\nTEST 01 OK\n");
}

Test(Alloc, Alloc2) // allocation et alignement
{
    printf("\nTEST 02\n\n");
    void *ptr1 = my_malloc(10);
    cr_assert(ptr1 != NULL, "Failed to allocate");

    void *ptr2 = my_malloc(100);
    cr_assert(ptr2 != NULL, "Failed to allocate");

    void *ptr3 = my_malloc(200);
    cr_assert(ptr3 != NULL, "Failed to allocate");

    void *ptr4 = my_malloc(5);
    cr_assert(ptr4 != NULL, "Failed to allocate");

    void *ptr5 = my_malloc(500);
    cr_assert(ptr5 != NULL, "Failed to allocate");

    void *ptr6 = my_malloc(1200);
    cr_assert(ptr6 != NULL, "Failed to allocate");

    void *ptr7 = my_malloc(1000);
    cr_assert(ptr7 != NULL, "Failed to allocate");

    void *ptr8 = my_malloc(20);
    cr_assert(ptr8 != NULL, "Failed to allocate");

    my_free(ptr1);
    my_free(ptr2);
    my_free(ptr3);
    my_free(ptr4);
    my_free(ptr5);
    my_free(ptr6);
    my_free(ptr7);
    my_free(ptr8);
    
    printf("\nTEST 02 OK\n");
}

Test(Alloc, Alloc3) // remap des pools
{
    printf("\nTEST 03\n\n");
    void *ptr9 = my_malloc(50);
    cr_assert(ptr9 != NULL, "Failed to mmap.");

    void *ptr10 = my_malloc(5000);
    cr_assert(ptr10 != NULL, "Failed to allocate");

    void *ptr11 = my_malloc(100);
    cr_assert(ptr11 != NULL, "Failed to allocate");

    void *ptr12 = my_malloc(9000);
    cr_assert(ptr12 != NULL, "Failed to allocate");

    void *ptr13 = my_malloc(29000);
    cr_assert(ptr13 != NULL, "Failed to allocate");

    my_free(ptr9);
    my_free(ptr10);
    my_free(ptr11);
    my_free(ptr12);
    my_free(ptr13);
    printf("\nTEST 03 OK\n");
}

Test(Alloc, Alloc4) // libération de mémoire
{
    printf("\nTEST 04\n\n");
    void *ptr112 = my_malloc(12);
    void *ptr200 = my_malloc(20);
    void *ptr300 = my_malloc(400);
    void *ptr400 = my_malloc(500);
    void *ptr500 = my_malloc(1000);

    cr_assert(ptr112 != NULL, "Failed to mmap.");
    cr_assert(ptr200 != NULL, "Failed to mmap.");
    cr_assert(ptr300 != NULL, "Failed to mmap.");
    cr_assert(ptr400 != NULL, "Failed to mmap.");
    cr_assert(ptr500 != NULL, "Failed to mmap.");

    my_free(ptr200);
    my_free(ptr112);

    my_free(ptr300);
    my_free(ptr400);
    my_free(ptr500);
    printf("\nTEST 04 OK\n");
}

Test(Alloc, Alloc5) // Realloc pool
{
    printf("\nTEST 05\n\n");
    char *ptr113 = my_malloc(20);
    cr_assert(ptr113 != NULL, "Failed to mmap.");
    
    char test[] = "xxxxxxxxxxxxxxxxxxx";
    memcpy(ptr113, test, sizeof(test));
    printf("ptr113 created, value: %s\n", ptr113);

    char *ptr114 = my_malloc(10);
    cr_assert(ptr114 != NULL, "Failed to mmap.");
    char test2[] = "helloworld";
    memcpy(ptr114, test2, sizeof(test2));
    printf("ptr114 created, value: %s\n", ptr114);

    char *ptr115 = my_realloc(ptr113, 30);
    cr_assert(ptr115 != NULL, "Failed to realloc.");
    char test4[] = "aaaaaaaaaa";
    memcpy(ptr115 + 19, test4, sizeof(test4));
    printf("ptr115 created, value: %s\n", ptr115);

    char *ptr116 = my_realloc(ptr115, 20);
    cr_assert(ptr116 != NULL, "Failed to realloc.");
    printf("ptr116 created, value: %s\n", ptr116);

    // Adding logs to check pointer values
    printf("Before realloc: ptr116: %p, size: 20\n", (void*)ptr116);
    char *ptr117 = my_realloc(ptr116, 20);
    printf("After realloc: ptr117: %p, ptr116: %p\n", (void*)ptr117, (void*)ptr116);
    cr_assert(ptr117 == ptr116, "Failed to realloc. ptr117: %lu, ptr116: %lu, delta: %lu", (size_t)ptr117, (size_t)ptr116, (size_t)(ptr116-ptr117));
    printf("\nfail test5\n");

    void *ptr118 = my_malloc(20);
    cr_assert(ptr118 != NULL, "Failed to malloc.");

    my_free(ptr114);
    void *ptr119 = my_malloc(10);
    cr_assert(ptr119 != NULL, "Failed to malloc.");

    ptr117 = my_realloc(ptr116, 10000);
    cr_assert(ptr117 != NULL, "Failed to realloc.");

    void *ptr120 = my_malloc(500);
    cr_assert(ptr120 != NULL, "Failed to malloc.");

    my_free(ptr119);
    void *ptr121 = my_malloc(38);
    cr_assert(ptr121 != NULL, "Failed to malloc.");

    my_free(ptr117);
    my_free(ptr118);
    my_free(ptr120);
    my_free(ptr121);
    printf("\nTEST 05 OK\n");
}


Test(Alloc, Alloc6) // Calloc
{
    printf("\nTEST 06\n\n");
    size_t nmemb = 10;
    size_t size = sizeof(int);
    int *ptr = (int *)my_calloc(nmemb, size);
    cr_assert(ptr != NULL, "Failed to calloc.");
    for (size_t i = 0; i < nmemb; i++) {
        cr_assert(ptr[i] == 0, "Failed to calloc at position %lu\n", i);
    }
    my_free(ptr);
    printf("\nTEST 06 OK\n");
}


