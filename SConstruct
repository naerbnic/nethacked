env = Environment(CPPPATH = ['#include'])

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
])

lev_comp, dgn_comp, makedefs = SConscript([
    'util/SConscript',
])

Export(['lev_comp', 'dgn_comp', 'makedefs'])
SConscript([
    'dat/SConscript',
])
