/*
 * ball.h
 *
 *  Created on: May 9, 2014
 *      Author: brianchin
 */

#ifndef BALL_H_
#define BALL_H_

#include "global.h"

extern void ballfall();
extern void placebc();
extern void unplacebc();
extern void set_bc(int);
extern void move_bc(int, int, xchar, xchar, xchar, xchar);
extern bool drag_ball(xchar, xchar, int *, xchar *, xchar *, xchar *, xchar *,
                 bool *, bool);
extern void drop_ball(xchar, xchar);
extern void drag_down();

#endif /* BALL_H_ */
