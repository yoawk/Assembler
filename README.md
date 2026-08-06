# Assembler in C
An assembler written in ANSI C for a made-up assembly language.
Written as part of a university project assignment.

## Project Overview


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




