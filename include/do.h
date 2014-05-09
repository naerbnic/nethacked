/*
 * do.h
 *
 *  Created on: May 9, 2014
 *      Author: brianchin
 */

#ifndef DO_H_
#define DO_H_

#include "config.h"

#include "dungeon.h"
#include "obj.h"
#include "tradstdc.h"

int dodrop();
bool boulder_hits_pool(Object *, int, int, bool);
bool flooreffects(Object *, int, int, const char *);
void doaltarobj(Object *);
bool canletgo(Object *, const char *);
void dropx(Object *);
void dropy(Object *);
void obj_no_longer_held(Object *);
int doddrop();
int dodown();
int doup();
#ifdef INSURANCE
void save_currentstate();
#endif
void goto_level(d_level *, bool, bool, bool);
void schedule_goto(d_level *, bool, bool, int, const char *, const char *);
void deferred_goto();
bool revive_corpse(Object *);
void revive_mon(genericptr_t, long);
int donull();
int dowipe();
void set_wounded_legs(long, int);
void heal_legs();

#endif /* DO_H_ */
