/*
 * eat.h
 *
 *  Created on: May 9, 2014
 *      Author: brianchin
 */

#ifndef EAT_H_
#define EAT_H_

#include "core/obj.h"

bool is_edible(Object *);
void init_uhunger();
int Hear_again();
void reset_eat();
int doeat();
void gethungry();
void morehungry(int);
void lesshungry(int);
bool is_fainted();
void reset_faint();
void violated_vegetarian();
#if 0
void sync_hunger();
#endif
void newuhs(bool);
Object *floorfood(const char *, int);
void vomit();
int eaten_stat(int, Object *);
void food_disappears(Object *);
void food_substitution(Object *, Object *);
void fix_petrification();
void consume_oeaten(Object *, int);
bool maybe_finished_meal(bool);

#endif /* EAT_H_ */
