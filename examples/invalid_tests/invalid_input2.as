; First pass errors

; Unknown line type
	blah	
LABEL1:		
LABEL2: blah

; Invalid symbol names
ThisSymbolIsExactly32Characterss: mov #0, r7
: mov #0, r0

r0: mov #0, r0
add: mov #0, r0
.string: mov #0, r0

4FirstChar: mov #0, r0
NotAlphaNumer!c: mov #0, r0

mcro M1
mcroend
M1: mov #0, r0

LABEL3: inc r0
LABEL3: dec r0

LABEL4: inc r0
.extern LABEL4

; Instruction errors
mov #0
mov #0, #0, #0
inc %LABEL4
mov %LOOP4, r0

; Operand errors
mov #+2048, r0
mov #-2049, r1
mov #blah, r0

jmp %

; Directive errors
.data
.data 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
.data blah
.data -2049
.data +2048

.string 
.string "blah
.string "¢"
.string "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
.string ""
.string "hi" bye

.extern
.extern LABEL5 blah

.entry			
Sym: inc r0
.entry Sym blah

; Parameter list errors
.data ,1,1
.data 1,,1
mov #0, r0, 
mov #0 r0
