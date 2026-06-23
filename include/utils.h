/**
 * utils.h
 * Header file of utils.c
 */

#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

/**
 * Helper wrapper function for malloc.
 * Behavior is the same as malloc, but on allocation fail, calls handle_error which terminates program.
 * @param allocation_size Allocation size in bytes
 * @return On allocation success, returns pointer to newly allocated memory chunk.
 *          Otherwise, execution is terminated.
 */
void *malloc_helper(size_t allocation_size);

/**
 * Helper wrapper function for realloc.
 * Behavior is the same as realloc, but on reallocation fail, calls handle_error which terminates program.
 * @param curr_ptr Pointer to current memory chunk.
 * @param new_size New size in bytes
 * @return On reallocation success, returns pointer to reallocated memory chunk.
 *          Otherwise, execution is terminated.
 */
void *realloc_helper(void *curr_ptr, size_t new_size);

#endif