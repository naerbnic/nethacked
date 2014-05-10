/*
 * makemon.h
 *
 *  Created on: May 9, 2014
 *      Author: brianchin
 */

#ifndef MAKEMON_H_
#define MAKEMON_H_

#include "config.h"

#include "monst.h"
#include "obj.h"
#include "global.h"

bool IsLevelHomeOfElementalType(MonsterType *);
Monster *clone_mon(Monster *, xchar, xchar);
Monster *makemon(MonsterType *, int, int, int);
bool create_critters(int, MonsterType *);
MonsterType *rndmonst();
void reset_rndmonst(int);
MonsterType *mkclass(char, int);
int adj_lev(MonsterType *);
MonsterType *grow_up(Monster *, Monster *);
int mongets(Monster *, int);
int golemhp(int);
bool peace_minded(MonsterType *);
void set_malign(Monster *);
void set_mimic_sym(Monster *);
int mbirth_limit(int);
void mimic_hit_msg(Monster *, short);
#ifdef GOLDOBJ
void mkmonmoney(Monster *, long);
#endif
void bagotricks(Object *);
bool propagate(int, bool, bool);

#endif /* MAKEMON_H_ */
