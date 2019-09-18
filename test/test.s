.macro .const name = value
    .macro name
        value
    .end name
.end .const

.const BOOTSEG = 0x07c0

BOOTSEG
