; Test different limits
; This line is a long line that contains exactly 80 characters (excluding  '\n')

; Minimum and maximum symbol size:
ThisSymbolIsA31CharactersLong1:	add r0,	r1
s:	add r0,	r1

.extern ThisSymbolIsA31CharactersLong2
.extern ThisSymbolIsA31CharactersLong2
.extern t
.extern t

.entry ThisSymbolIsA31CharactersLong1
.entry ThisSymbolIsA31CharactersLong1
.entry s
.entry s

; Minimum and maximum immediate values
mov #-2048, r0
mov #+2047, r0

; Minimum and maximum data list values
BigData:	.data +2047
SmallData:	.data -2048

; Maximum data list length
LongData:	.data 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1

; Minimum and maximum string sizes
SmallString:	.string "a"
BigSt: .string "This string is 63 characters long (excluding the ' ' and '\0' )"

stop
