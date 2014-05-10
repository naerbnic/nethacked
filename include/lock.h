/*
 * lock.h
 *
 *  Created on: May 9, 2014
 *      Author: brianchin
 */

#ifndef LOCK_H_
#define LOCK_H_

#include "obj.h"

bool picking_lock(int *, int *);
bool picking_at(int, int);
void reset_pick();
int pick_lock(Object *);
int doforce();
bool boxlock(Object *, Object *);
bool doorlock(Object *, int, int);
int doopen();
int doclose();

#endif /* LOCK_H_ */
