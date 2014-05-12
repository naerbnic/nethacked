/*
 * light.h
 *
 *  Created on: May 9, 2014
 *      Author: brianchin
 */

#ifndef LIGHT_H_
#define LIGHT_H_

#include "core/global.h"
#include "core/obj.h"
#include "core/tradstdc.h"

void new_light_source(xchar, xchar, int, int, genericptr_t);
void del_light_source(int, genericptr_t);
void do_light_sources(char **);
Monster *find_mid(unsigned, unsigned);
void save_light_sources(int, int, int);
void restore_light_sources(int);
void relink_light_sources(bool);
void obj_move_light_source(Object *, Object *);
bool any_light_source();
void snuff_light_source(int, int);
bool obj_sheds_light(Object *);
bool obj_is_burning(Object *);
void obj_split_light_source(Object *, Object *);
void obj_merge_light_sources(Object *, Object *);
int candle_light_range(Object *);
#ifdef WIZARD
int wiz_light_sources();
#endif

#endif /* LIGHT_H_ */
