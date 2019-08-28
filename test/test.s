.const null 0

.if null != 0
    .error "expected null value"
.end if

.macro sum a, b
    a + b
.end sum

.if 4 != sum 2, 2
    .error "expected 4 (2+2 sum)"
.end if

.const _const
.release _const

.if !declared _const
    .error "released const used"
.end if

.include "test/inc.s"

.if !declared __included
    .error "uncorrect 'include' work"
.end if

.if 1
.else
    .error "'.else' directive runned while if get good value"
.end if

.warning "check warning & EOF"
