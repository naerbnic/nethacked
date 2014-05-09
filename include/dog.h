/*
 * dog.h
 *
 *  Created on: May 9, 2014
 *      Author: brianchin
 */

#ifndef DOG_H_
#define DOG_H_

#include "monst.h"
#include "obj.h"
#include "global.h"

void initedog(Monster *);
Monster *make_familiar(Object *, xchar, xchar, bool);
Monster *makedog();
void update_mlstmv();
void losedogs();
void mon_arrive(Monster *, bool);
void mon_catchup_elapsed_time(Monster *, long);
void keepdogs(bool);
void migrate_to_level(Monster *, xchar, xchar, coord *);
int dogfood(Monster *, Object *);
Monster *tamedog(Monster *, Object *);
void abuse_dog(Monster *);
void wary_dog(Monster *, bool);

#endif /* DOG_H_ */
