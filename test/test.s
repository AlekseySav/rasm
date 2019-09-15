.macro example (arg1), arg2=ds:arg3      ; ( ) , : \n
    .word 0
.end example

; example (eax), cs:10 + 8

; (eax) , cs : 10 + 8

; arg1=eax        ,
; arg2=cs         :
; arg3=10 + 8     \n

; example (eax), ax

; ( eax ) , ax

; (
; arg1=eax    ) ,
; arg2=ax     \n

; arg3=arg2
; arg2=ds
