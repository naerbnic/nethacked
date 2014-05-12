/*
 * dokick.h
 *
 *  Created on: May 9, 2014
 *      Author: brianchin
 */

#ifndef DOKICK_H_
#define DOKICK_H_

#include "core/monst.h"
#include "core/obj.h"

bool ghitm(Monster *, Object *);
void container_impact_dmg(Object *);
int dokick();
bool ship_object(Object *, xchar, xchar, bool);
void obj_delivery();
schar down_gate(xchar, xchar);
void impact_drop(Object *, xchar, xchar, xchar);

#endif /* DOKICK_H_ */
