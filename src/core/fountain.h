/*
 * fountain.h
 *
 *  Created on: May 9, 2014
 *      Author: brianchin
 */

#ifndef FOUNTAIN_H_
#define FOUNTAIN_H_

#include "core/config.h"
#include "core/global.h"
#include "core/obj.h"

void floating_above(const char *);
void dogushforth(int);
void dryup(xchar, xchar, bool);
void drinkfountain();
void dipfountain(Object *);
#ifdef SINKS
void breaksink(int, int);
void drinksink();
#endif

#endif /* FOUNTAIN_H_ */
