.const null 0

.if null != 0
    .error "expected null value"
.end if

.const _const
.release _const

.if declared _const
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
