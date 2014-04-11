env = Environment
Export(['env'])

baselib = SConscript([
    'base/SConscript',
])
Export(['baselib'])

entitylib = SConscript([
    'entitylib/SConscript',
])
Export(['entitylib'])

SConscript([
    'src/SConscript',
    'util/SConscript',
])

