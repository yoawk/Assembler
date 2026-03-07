/**
 * utils.c
 * 
 */

#include "../include/utils.h"

/* Might be unnecessary */
void *malloc_helper(size_t size) {
    void *p = malloc(size);
    if (!p) {
        print_error(ERR_ALLOC_FAILED);
        return NULL;
    }

    return p;
}