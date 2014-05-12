/*
 * dothrow.h
 *
 *  Created on: May 9, 2014
 *      Author: brianchin
 */

#ifndef DOTHROW_H_
#define DOTHROW_H_

#include "core/coord.h"
#include "core/monst.h"
#include "core/obj.h"
#include "core/tradstdc.h"

int dothrow();
int dofire();
void hitfloor(Object *);
void hurtle(int, int, int, bool);
void mhurtle(Monster *, int, int, int);
void throwit(Object *, long, bool);
int omon_adj(Monster *, Object *, bool);
int thitmonst(Monster *, Object *);
int hero_breaks(Object *, xchar, xchar, bool);
int breaks(Object *, xchar, xchar);
bool breaktest(Object *);
bool walk_path(coord *, coord *, bool (*)(genericptr_t, int, int),
                 genericptr_t);
bool hurtle_step(genericptr_t, int, int);

#endif /* DOTHROW_H_ */
