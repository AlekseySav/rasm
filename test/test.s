.macro example arg1
    .word arg1
.end example

.macro e2 8, 0
    8 = 0
.end e2

example 7
e2 0, 7
