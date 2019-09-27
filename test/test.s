[]
.macro nil
11
.end nil

.if !declared nil
    .error 0 != 0
.else
    .warning hehe
.end if
