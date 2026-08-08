; Pre-assembler errors
; This line is a long line that contains exactly 81 characters (excluding '\n'  )

; Macro name duplicate 
mcro M1
mov #0, r0
mcroend

mcro M1
; No mcroend because of input assumptions

; Reserved keywords
mcro r0
mcro add
mcro .entry

; Non-white characters after macro reference
M1 	b

; Non-white characters after macro definition start/end
mcro M2 blah blah
mcro M3
mcroend 		blah

