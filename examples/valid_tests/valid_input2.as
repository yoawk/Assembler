; Create macros
mcro M1
dec r0
dec r1
dec r2
mcroend

mcro M2
inc r3
inc r4
inc r5
inc r6
inc r7
mcroend

; Macro expansion
M1
M1
M2
M2
M1
M2
M1

mcro M3
; comment

mcroend

M3
M3
stop
