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

### `.as` Source Files Structure

- The `.as` source files are made up of statements.
- Only one statement is allowed per line, where lines separated by `\n`.
- Maximum valid line length: **80 characters** (excluding the newline).
- Redundant white characters (white spaces and tabs) are ignored by the assembler (for example, `(tab)(space)clr(tab)(tab)r0(space)` will be
  treated as `clr r0`).

The four kinds of statements (lines in the `.as` file) are:

1. **Blank statement** - No characters at all, or only white characters (white spaces and tabs). These lines are ignored.
2. **Comment statement** - A comment in the source file. The first non-white character of the comment line should be `;` and the entire line
   is ignored.
3. **Directive statement** - A statement directing the assembler what to do when processing the current source file. Discussed later in the
   README.
4. **Instruction statement** - Represents an operation for the CPU to execute when running the output program. Discussed later in the
   README.

### Machine Code

The machine code is made up of two segments - the code image, and the data image.  
The code image contains encoded instructions, and the data image contains encoded data objects
(such as strings).
When constructing the code and data images, we use two counters, IC (**I**nstruction **C**ounter)
and DC (**D**ata **C**ounter),to keep track of the cell where the next
code/data word will be placed in the code/data image. Note that the initial IC is set to 100.  
In the final machine code, the code image is placed first, and the data image comes right after the code image.

Further explanations of the code and data image construction will be given later in the README

### Instruction Set

An instruction is an operation (such as addition) that is encoded by the assembler into a CPU instruction,
that can later be performed by the CPU on program execution (Theoretically).
An instruction has the following structure: `[optional label:] [operation name] operand1, operand2`

In the code image, each instruction is encoded as 1–3 consecutive memory words (cells),
and the first word always has the layout:

|   Bits    |   8-11   |   4-7   |               2-3                | 0-1                                   |
|:---------:|:--------:|:-------:|:--------------------------------:|---------------------------------------|
| **Field** | *opcode* | *funct* | *source operand addressing mode* | *destination operand addressing mode* |

- If an instruction has no source operand, bits 2-3 are zeroed.
- If an instruction has no target operand, bits 0-1 are zeroed.
- `funct` is used to differentiate between different operations that share the same `opcode`.
  If an opcode is unique to a single operation, `funct` is zeroed.
- If a source/destination operand is used by the encoded instruction, an additional operand word will be appended
  (if both operands are used, the encoded source operand word will come before the destination's).

The instruction set contains 16 operations that are grouped into three categories:

#### Operations with 2 operands (source and destination)

| Instruction    | Opcode | Funct | Effect                                                                                               |
|----------------|--------|-------|------------------------------------------------------------------------------------------------------|
| `mov src, dst` | 0      | –     | Copies the value of the source operand into the destination operand                                  |
| `cmp src, dst` | 1      | –     | Compares the two operands, and updates the zero flag of PSW if they are equal (result is not stored) |
| `add src, dst` | 2      | 10    | `dst = dst + src`                                                                                    |
| `sub src, dst` | 2      | 11    | `dst = dst − src`                                                                                    |
| `lea src, dst` | 4      | –     | Loads the effective address represented by label `src` into `dst`                                    |

#### Operations with 1 operand (destination)

For these, the source operand field (bits 2-3) is unused/zeroed.

| Instruction | Opcode | Funct | Effect                                                                                  |
|-------------|--------|-------|-----------------------------------------------------------------------------------------|
| `clr dst`   | 5      | 10    | Sets operand to 0                                                                       |
| `not dst`   | 5      | 11    | Bitwise inversion of the operand                                                        |
| `inc dst`   | 5      | 12    | Increments the operand by 1                                                             |
| `dec dst`   | 5      | 13    | Decrements the operand by 1                                                             |
| `jmp label` | 9      | 10    | Unconditional jump: `PC = address(label)`                                               |
| `bne label` | 9      | 11    | Branch to `label` if not equal to 0 (determined by the zero flag)                       |
| `jsr label` | 9      | 12    | Subroutine call. Pushes `PC+2` (return address) to the stack, before jumping to `label` |
| `red dst`   | 12     | –     | Reads one character from standard input and stores its ASCII code into the operand      |
| `prn dst`   | 13     | –     | Prints the character stored in the operand                                              |

#### Operations with no operands

Both operand mode fields (bits 0-3) are unused/zeroed.

| Instruction | Opcode | Effect                                                                      |
|-------------|--------|-----------------------------------------------------------------------------|
| `rts`       | 14     | Returns from a subroutine. Pops the return address from the stack into `PC` |
| `stop`      | 15     | Halts program execution immediately                                         |

#### Addressing modes

There are 4 addressing modes that can be used by the instruction:

| mode number | name                | operand syntax                | encoded word contents                                                                                                                  |
|-------------|---------------------|-------------------------------|----------------------------------------------------------------------------------------------------------------------------------------|
| 0           | **Immediate**       | `#<number>` (e.g. `#-1`)      | The integer value itself represented in 2's complement, 12 bits wide                                                                   |
| 1           | **Direct**          | `<label>` (e.g. `x`, `LIST`)  | The memory address the label represents, encoded as a 12-bit unsigned number                                                           |
| 2           | **Relative**        | `%<label>` (e.g. `%LOOP`)     | Encodes the signed 2's-complement distance (in words) from the address of the current data word to the target instruction's first word |
| 3           | **Register direct** | `<register name>` (e.g. `r1`) | All bits of the the word zeroed out except the bit corresponding to the register's number (0-7)                                        |

#### Legal addressing modes per instruction

| Instruction | Legal source modes | Legal target modes |
|:-----------:|:------------------:|:------------------:|
|    `mov`    |      0, 1, 3       |        1, 3        |
|    `cmp`    |      0, 1, 3       |      0, 1, 3       |
|    `add`    |      0, 1, 3       |        1, 3        |
|    `sub`    |      0, 1, 3       |        1, 3        |
|    `lea`    |         1          |        1, 3        |
|    `clr`    |         -          |        1, 3        |
|    `not`    |         -          |        1, 3        |
|    `inc`    |         -          |        1, 3        |
|    `dec`    |         -          |        1, 3        |
|    `jmp`    |         -          |        1, 2        |
|    `bne`    |         -          |        1, 2        |
|    `jsr`    |         -          |        1, 2        |
|    `red`    |         -          |        1, 3        |
|    `prn`    |         -          |      0, 1, 3       |
|    `rts`    |         -          |         -          |
|   `stop`    |         -          |         -          |

### Macros

- Defined with `mcro <name>` ... `mcroend`.
- A macro body is a block of lines, that can be invoked later in the assembly code to "paste" the macro's block of lines.
- Nested macro definitions are not allowed.
- Macro names cannot collide with instruction/directive names, or be reused as labels.
- Macro definitions always appear before their first use.

Example:

```asm
mcro MC_GEN
    inc r2
    mov A, r1
mcroend
...
MC_GEN
clr r0
MC_GEN
```

expands to:

```asm
...	
inc r2
mov A, r1
clr r0
inc r2
mov A, r1
```

### Directives

Directives are statements used to provide additional guidance to the assembler when it processes the source file.
(e.g. Treat certain symbols differently, or allocate space for data).
Directive keywords start with a `.` followed by their name in lowercase letters and have the following structure:  
`[optional label:] .[directive name] param1, param2, ...`

There are 4 directives:

#### `.data`

```
LIST: .data 6, -9, 100
```

Declares one or more comma-separated signed decimal integers in the valid range of the 2's complement of 12 bits.
Each value is stored as one word in the data image, advancing the data counter (DC) by one per value.
If a label is defined on this line, it receives the initial value of the DC (before advancing) as its
address, with the `data` attribute.

#### `.string`

```
STR: .string "abcd"
```

Declares a string literal. Each character is encoded as its ASCII value and stored as a word in the data image, in order.
A `000000000000` word (used for string termination) is appended after the characters in the data image.
DC advances by `(string length + 1)`. Label handling is identical to `.data`.

#### `.entry`

```
.entry HELLO
```

Marks a label, **defined in the current `.as` file**, as visible to other source files (for the non-implemented linking stage).
The label is recorded in the symbol table with the `entry` attribute and written to the `.ent` output file.
A label written at the start of a `.entry` line itself is meaningless and is ignored.

#### `.extern`

```
.extern val1
```

Declares a symbol/label that is **not** defined in the current `.as` file but can be used throughout the code.
The symbol resolution is done by the (non-implemented) linking stage.
The symbol is entered into the symbol table with value/address `0` (since it's not defined in the current `.as` file),
and with the `external` attribute.
A label **can't** be declared as both `entry` and `extern` in the same file.

### Labels/Symbols

Symbols are used to mark a certain code or data word. To define such symbols, we use labels.

- A valid label starts with an alphabetic character, followed by any sequence of letters and/or digits.
- Maximum length of 31 characters.
- Definitions end with `:` (immediately after the label name, no space), at the start of an instruction line or a `.data`/`.string` line.
- Labels are case-sensitive, meaning that `test` is different from `Test`.
- A label may not be defined more than once.
- Reserved words (instruction names, directive names, register names) and macro names cannot be used as label names.
- The value that corresponds to a label/symbol depends on context. If the label is defined on a `.data`/`.string` line,
  then its value is the current data counter (DC).
  If the label is defined on an instruction line, then its value is the current instruction counter (IC).
- A label may be *referenced* (used as an operand) before it is defined later in the file. It may also remain undefined in the current file
  as long as it is declared with `.extern`.

### ARE Marks

Each code image word has one of three relocation markers,
(needed for the, non-implemented, linking/loading stages):

- A (**A**bsolute) - The word's content does not depend on where the program is loaded in memory (e.g. an immediate value).
- R (**R**elocatable) - The word's content depends on the load address (e.g. a word holding the address of a label defined in this file).
- E (**E**xternal) - The word's content depends on the value of an external symbol resolved at link time.

Data image words are always Absolute.

### Output Files

For an input `.as` file, if no errors occur, the assembler produces:

- The `.am` file that contains the assembly code of the `.as` file after comment/white line removal and macro expansions.
- The `.ob` file, containing the machine code (instruction image and data image).
  The file's first line has the format `[length of code image] [length of data image]`,
  and is used to differentiate between the two concatenated sections.
  The rest of the lines are of the format `[4-digit decimal address] [3-digit hexadecimal word content] [ARE character]`.
- The `.ent` file is generated if the source file contains at least one entry symbol (`.entry` line).
  The file contains the entry symbols' names and addresses, in the following format:
  `[symbol name] [4-digit decimal address]`.
- The `.ext` file is generated if the source file contains at least one reference to an external symbol.
  The file contains the referenced external symbols' name, and location of reference (in the code image)
  in the following format: `[symbol name] [4-digit decimal address of reference]`.

### Assembler stages

<!-- This section might need more details explaining how the stages achieve their "goals". -->

The assembler processes each input source file independently, producing its own set of output files.  
The assembler's work is divided into three main stages:

#### Pre-assembler stage

Operates on the given `.as` file and performs:

- Macro expansion (together with the generation of the macro table containing the macros)
- Comment/White lines filtering

The stage

- Assumes there are no nested macro definitions.
- Assumes each macro definition start has a corresponding macro ending.
- Assumes macro calls appear only after corresponding macro definitions.

On success, writes the resulted assembly code into a new `.am` file with the same name as the `.as` file.
When errors are found in the assembly code, reports them, and, if possible, coninues processing the `.as` file.
The output `.am` file will not be created if any error is found, and the next stages will be skipped.

#### First pass stage

Operates on the generated `.am` file together with the generated macro table and performs:

- Partial encoding of the `.am` file's instruction and directives into the code and data images
  (symbol dependent encodings happen in the second pass)
- Parsing and validation of label definitions
- Parsing and validation of `.extern` directive lines, marking external symbols in the symbol table
- Generation of the symbol table
- Determines the final sizes of the code and data images

When errors are found in the assembly code, reports them, and, if possible, coninues processing the `.am` file.
The second pass stage will not be executed if errors were found.

#### Second pass stage

Operates on the generated `.am` file together with the generated macro and symbol tables and performs:

- Full encoding of the partially encoded code and data images
- Parsing and validation of `.entry` directives, marking entry symbol in the symbol table

When errors are found in the assembly code, reports them, and, if possible, coninues processing.
If no errors were found, generates the `.ob`, `.ent` (if not empty),`.ext` (if not empty) output files.

### Project Structure

<details>
     <summary><i>view</i></summary>

```
.
├── Makefile
├── README.md
├── assignment.pdf          Assignment instructions containing the assembly specifications (in Hebrew)
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

## Building and Usage

Note that building and execution of the project was only tested on an
Ubuntu machine using `gcc` (specific flags in the Makefile)!

Clone the repository

```bash
git clone https://github.com/yoawk/Assembler.git
cd Assembler
```

Use the Makefile to compile the program (`make` needs to be installed).

```bash
make
```

The assmebler executable will be placed in `./build/release/` (if `make` or `make release` were used).

In order to use the assembler, you must create (or use avaliable [examples](./examples)) a `.as` file (valid or invalid).  
`.as` source files are passed to the assembler as program arguments **without** the `.as` extension.
For example, the following command will make the assembler proccess the files `file1.as`, `file2.as`, `file3.as` in order.

```
./build/release/assembler file1 file2 file3
```

Each file is processed separatly, meaning that an **assembly** source file error (such as syntax error) of one file,
will not affect the processing of the other files (and the generation of the output files).

## Notes

- The majority of git actions were performed *after* the project was finished, and
  therefore some of the git actions may seem artificial.
- The project contains an implementation of an *assembler* for the made-up assembly language.
  Other parts needed for the execution of the assembly code (such as the linker and loader) are **not** implemented in the current project.
- Some instructions regarding the behavior of the assembler and the code design were given by the course's
  instructors and are *not* written in the repository's `.pdf` file.
