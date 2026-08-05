/**
 * assembler.h
 * Header file of assembler.c
 */

#ifndef ASSEMBLER_H
#define ASSEMBLER_H

/**
 * Assembler's main function.
 * Performs assembling on given file(s) - pre-assembler, first pass, second pass.
 * Reports source assembly errors with context, and outputts info message regarding the process.
 * Input files should have the ".as" extension, and their paths should be passed to the assembler
 * program as arguments with no ".as" extension.
 *
 * For every input file, if no errors occurred and no assembly errors were detected,
 * output .am, .ent, .ext, .ob files will be created (with the same name as input file).
 * The .am file - Contains .as file content after macro expansion and empty/comment line filtering.
 * The .ent file - Contains the memory location of symbols declared as .entry (public visibility).
 * The .ext file - Contains the memory location of memory words that are associated with an external symbol.
 * The .ob file - Contains the final binary code (code image and data image) of the source .as file.
 *
 * If resulting .ent or .ext files are empty, they won't be produced.
 *
 * Note that for any program error (unexpected NULL pointer, allocation fail, ...)
 * program will be terminated immediately.
 *
 * @return 0 on success.
 *          Fail will only occur on program error, in which case exit(EXIT_FAILURE) will be called.
 */
int main(int argc, char *argv[]);

#endif