# Assembler in C

A two-pass assembler written in ANSI C for a made-up assembly language that targets a made-up machine.
Written as part of a university project assignment.

## Project Overview

### Target Machine

The target made-up machine has the following specifications:

- 8 general-purpose registers, named `r0`-`r7`. Each register is 12 bits wide.
- An internal register named PSW (**P**rogram **S**tatus **W**ord) holding different status flags
  (like the zero flag, updated by `cmp`).
- An internal register named PC (**P**rogram **C**ounter) holding the address of the word currently
  being executed (points to the first word of the current instruction).
- Memory size of 4096 memory cells/words (addresses 0-4095). Each cell/word is 12 bits wide.
- Integers are represented using 2's complement. No floating point support.

### Machine Instruction Structure

Every machine instruction is encoded as 1–3 consecutive memory words (cells).
The first word always has the layout:

|   Bits    |   8-11   |   4-7   |               2-3                |               0-1                |
|:---------:|:--------:|:-------:|:--------------------------------:|:--------------------------------:|
| **Field** | *opcode* | *funct* | *source operand addressing mode* | *target operand addressing mode* |

- If an instruction has no source operand, bits 2-3 are zeroed.
- If an instruction has no target operand, bits 0-1 are zeroed.
- `funct` is used to diffrentiate between different operations that share the same `opcode`.
If an opcode is unique to a single operation, `funct` is zeroed.
- If a source/target operand is used by the encoded instruction, an additional operand word will be appended
(if both operands are used, the encoded source operand word will come before the target's).

### Assembly language

### Assembler stages
     
#### Pre-assembler stage

#### First pass stage

#### Second pass stage


### Project Structure
<details>
     <summary><i>view</i></summary>
     
```
.
├── Makefile
├── README.md
├── assignment.pdf          Assignement instructions contatining the assembly sepcifications (in Hebrew)
├── build
│   └── release
│   └── debug
├── examples                Examples of valid and invalid (containing errors) input files,
│   ├── invalid_tests                                      with their corresponding output
│   │   ├── invalid_input1.as
│   │   └── ...
│   └── valid_tests
│       ├── valid_input1.am
│       ├── valid_input1.as
│       ├── valid_input1.ent
│       ├── valid_input1.ob
│       └── ...
├── include                     .h files
│   ├── assembler.h
│   ├── directives.h
│   ├── errors.h
│   ├── first_pass.h
│   ├── globals.h
│   ├── handle_file.h
│   ├── instructions.h
│   ├── keywords.h
│   ├── macros.h
│   ├── object_code.h
│   ├── pre_assembler.h
│   ├── second_pass.h
│   ├── symbols.h
│   └── utils.h
└── src                         .c files
    ├── assembler.c             Contains main program loop
    ├── directives.c            
    ├── errors.c                
    ├── first_pass.c
    ├── handle_file.c
    ├── instructions.c
    ├── keywords.c
    ├── macros.c
    ├── object_code.c
    ├── pre_assembler.c
    ├── second_pass.c
    ├── symbols.c
    └── utils.c
```
</details>

### Assignment Instructions
A `.pdf` file containing the assignment's instruction (written in Hebrew) can be found [here](./assignment.pdf).


## Bulding and Usage


## Notes
- The majority of git actions were performed *after* the project was finished, and 
therefore the git actions may seem artificial.
- The project contains an implementation of an *assembler* for the made-up assembly language.
Other parts needed for the execution of the assembly code (such as the linker and loader) are **not** implemented in the current project.
- Some instructions regarding the behavior of the assembler and the code design were given by the course's
instructors and are *not* written in the repository's `.pdf` file.




