/**
 * utils.c
 * Utility functions for ease of use
 */

#include "utils.h"

#include "errors.h"

#include <stdlib.h>


void *malloc_helper(size_t allocation_size) {
    void *ptr;

    ptr = malloc(allocation_size);

    if (!ptr)
        handle_error(ERR_ALLOC_FAILED, NULL, 0);

    return ptr;
}


void *realloc_helper(void *curr_ptr, size_t new_size) {
    void *new_ptr = realloc(curr_ptr, new_size);

    if (!new_ptr)
        handle_error(ERR_REALLOC_FAILED, NULL, 0);

    return new_ptr;
}
