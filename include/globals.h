/**
 * globals.h
 * Contains widely used macros and declarations of global variables
 */

#ifndef GLOBALS_H
#define GLOBALS_H

/* Max line size in an input file excluding '\n' character and '\0' character */
#define MAX_LINE_SIZE 80

/* Assembly-language architecture word size in bits */
#define WORD_SIZE 12
/* Word mask of the lower 12 bits (size of word). Sometimes used just for readability */
#define WORD_MASK (0x0FFF)
/* Memory size in words */
#define MEMORY_SIZE 4096

/* Input/Output file extensions (file name will always be the same) */
#define AS_FILE_EXTENSION ".as"
#define AM_FILE_EXTENSION ".am"
#define ENT_FILE_EXTENSION ".ent"
#define EXT_FILE_EXTENSION ".ext"
#define OBJECT_FILE_EXTENSION ".ob"




#endif