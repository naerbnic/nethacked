/*
 * do_wear.h
 *
 *  Created on: May 9, 2014
 *      Author: brianchin
 */

#ifndef DO_WEAR_H_
#define DO_WEAR_H_

#include "core/monst.h"
#include "core/obj.h"

void off_msg(Object *);
void set_wear();
bool donning(Object *);
void cancel_don();
int Armor_off();
int Armor_gone();
int Helmet_off();
int Gloves_off();
int Boots_off();
int Cloak_off();
int Shield_off();
#ifdef TOURIST
int Shirt_off();
#endif
void Amulet_off();
void Ring_on(Object *);
void Ring_off(Object *);
void Ring_gone(Object *);
void Blindf_on(Object *);
void Blindf_off(Object *);
int dotakeoff();
int doremring();
int cursed(Object *);
int armoroff(Object *);
int canwearobj(Object *, long *, bool);
int dowear();
int doputon();
void find_ac();
void glibr();
Object *some_armor(Monster *);
void erode_armor(Monster *, bool);
Object *stuck_ring(Object *, int);
Object *unchanger();
void reset_remarm();
int doddoremarm();
int destroy_arm(Object *);
void adj_abon(Object *, schar);

#endif /* DO_WEAR_H_ */
