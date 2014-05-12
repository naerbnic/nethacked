/*
 * invent.h
 *
 *  Created on: May 9, 2014
 *      Author: brianchin
 */

#ifndef INVENT_H_
#define INVENT_H_

#include "core/obj.h"

void assigninvlet(Object *);
Object *merge_choice(Object *, Object *);
int merged(Object **, Object **);
void addinv_core1(Object *);
void addinv_core2(Object *);
Object *addinv(Object *);
Object *hold_another_object(Object *, const char *, const char *,
                              const char *);
void useupall(Object *);
void useup(Object *);
void consume_obj_charge(Object *, bool);
void freeinv_core(Object *);
void freeinv(Object *);
void delallobj(int, int);
void delobj(Object *);
Object *sobj_at(int, int, int);
Object *carrying(int);
bool have_lizard();
Object *o_on(unsigned int, Object *);
bool obj_here(Object *, int, int);
bool wearing_armor();
bool is_worn(Object *);
Object *g_at(int, int);
Object *mkgoldobj(long);
Object *getobj(const char *, const char *);
int ggetobj(const char *, int (*)(Object *), int, bool, unsigned *);
void fully_identify_obj(Object *);
int identify(Object *);
void identify_pack(int);
int askchain(Object **, const char *, int, int (*)(Object *),
               int (*)(Object *), int, const char *);
void prinv(const char *, Object *, long);
char *xprname(Object *, const char *, char, bool, long, long);
int ddoinv();
char display_inventory(const char *, bool);
#ifdef DUMP_LOG
char dump_inventory(const char *, bool, bool);
#endif
int display_binventory(int, int, bool);
Object *display_cinventory(Object *);
Object *display_minventory(Monster *, int, char *);
int dotypeinv();
const char *dfeature_at(int, int, char *);
int look_here(int, bool);
int dolook();
bool will_feel_cockatrice(Object *, bool);
void feel_cockatrice(Object *, bool);
void stackobj(Object *);
int doprgold();
int doprwep();
int doprarm();
int doprring();
int dopramulet();
int doprtool();
int doprinuse();
void useupf(Object *, long);
char *let_to_name(char, bool);
void free_invbuf();
void reassign();
int doorganize();
int count_unpaid(Object *);
int count_buc(Object *, int);
void carry_obj_effects(Object *);
const char *currency(long);
void silly_thing(const char *, Object *);

#endif /* INVENT_H_ */
