.ifndef jmp
	.define jmp x
		.word x x
	.enddef
.endif

jmp 0xfeeb
.word 0
