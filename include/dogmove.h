/*
 * dogmove.h
 *
 *  Created on: May 9, 2014
 *      Author: brianchin
 */

#ifndef DOGMOVE_H_
#define DOGMOVE_H_

#include "core/monst.h"
#include "core/obj.h"

int dog_nutrition(Monster *, Object *);
int dog_eat(Monster *, Object *, int, int, bool);
int dog_move(Monster *, int);

#endif /* DOGMOVE_H_ */
