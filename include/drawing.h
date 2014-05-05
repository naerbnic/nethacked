#ifndef DRAWING_H
#define DRAWING_H

#include "global.h"

E int def_char_to_objclass(char);
E int def_char_to_monclass(char);
E void assign_graphics(uchar *, int, int, int);
E void switch_graphics(int);
#ifdef REINCARNATION
E void assign_rogue_graphics(bool);
#endif

#endif
