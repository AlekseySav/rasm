.macro __define_const _Name = _Val
    .macro _Name
        _Val
    .end _Name
.end __define_const

__define_const const = ##__define_const

const null = 0
const x = null

x

it works :)
