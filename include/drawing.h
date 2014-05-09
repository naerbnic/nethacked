#ifndef DRAWING_H
#define DRAWING_H

#include "global.h"

int def_char_to_objclass(char);
int def_char_to_monclass(char);
void assign_graphics(uchar *, int, int, int);
void switch_graphics(int);
#ifdef REINCARNATION
void assign_rogue_graphics(bool);
#endif

#endif
