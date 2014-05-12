/*
 * dig.h
 *
 *  Created on: May 9, 2014
 *      Author: brianchin
 */

#ifndef DIG_H_
#define DIG_H_

#include "core/monst.h"
#include "core/obj.h"
#include "core/tradstdc.h"

bool is_digging();
int holetime();
bool dig_check(Monster *, bool, int, int);
void digactualhole(int, int, Monster *, int);
bool dighole(bool);
int use_pick_axe(Object *);
int use_pick_axe2(Object *);
bool mdig_tunnel(Monster *);
void watch_dig(Monster *, xchar, xchar, bool);
void zap_dig();
Object *bury_an_obj(Object *);
void bury_objs(int, int);
void unearth_objs(int, int);
void rot_organic(genericptr_t, long);
void rot_corpse(genericptr_t, long);
#if 0
void bury_monst(Monster *);
void bury_you();
void unearth_you();
void escape_tomb();
void bury_obj(Object *);
#endif

#endif /* DIG_H_ */
