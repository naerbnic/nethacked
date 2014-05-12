/*
 * detect.h
 *
 *  Created on: May 9, 2014
 *      Author: brianchin
 */

#ifndef DETECT_H_
#define DETECT_H_

#include "core/dungeon.h"
#include "core/obj.h"
#include "core/trap.h"

extern Object *o_in(Object *, char);
extern Object *o_material(Object *, unsigned);
extern int gold_detect(Object *);
extern int food_detect(Object *);
extern int object_detect(Object *, int);
extern int monster_detect(Object *, int);
extern int trap_detect(Object *);
extern const char *level_distance(d_level *);
extern void use_crystal_ball(Object *);
extern void do_mapping();
extern void do_vicinity_map();
extern void cvt_sdoor_to_door(struct rm *);
extern int findit();
extern int openit();
extern void find_trap(Trap *);
extern int dosearch0(int);
extern int dosearch();
extern void sokoban_detect();

#endif /* DETECT_H_ */
