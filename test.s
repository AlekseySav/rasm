.include "test.s"
.const NULL 0

test:	| 0
	square 8
	square NULL
	.byte end + test, 0xff
end:	| 2+2+1
