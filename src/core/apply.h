/*
 * apply.h
 *
 *  Created on: May 9, 2014
 *      Author: brianchin
 */

#ifndef APPLY_H_
#define APPLY_H_

#include "core/obj.h"
#include "core/monst.h"
#include "core/tradstdc.h"

extern int doapply();
extern int dorub();
extern int dojump();
extern int jump(int);
extern int number_leashed();
extern void o_unleash(Object *);
extern void m_unleash(Monster *, bool);
extern void unleash_all();
extern bool next_to_u();
extern Object *get_mleash(Monster *);
extern void check_leash(xchar, xchar);
extern bool um_dist(xchar, xchar, xchar);
extern bool snuff_candle(Object *);
extern bool snuff_lit(Object *);
extern bool catch_lit(Object *);
extern void use_unicorn_horn(Object *);
extern bool tinnable(Object *);
extern void reset_trapset();
extern void fig_transform(genericptr_t, long);
extern int unfixable_trouble_count(bool);

#endif /* APPLY_H_ */
