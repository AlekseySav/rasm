.ifndef square
	.define square x
		.word x * x
	.enddef
.endif

.const NULL 0

test:	| 0
	square 8
	square NULL
	.byte end + test, 9
end:	| 2+2+1
