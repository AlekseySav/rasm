.ifndef mov

.define mov x, y
    .word 0x6100 + x * 10 + y
.enddef

.endif
