/*
 * do_name.h
 *
 *  Created on: May 9, 2014
 *      Author: brianchin
 */

#ifndef DO_NAME_H_
#define DO_NAME_H_

#include <string>

#include "core/monst.h"
#include "core/obj.h"
#include "core/tradstdc.h"

int getpos(coord *, bool, const char *);
Monster *christen_monst(Monster *, std::string const&);
int do_mname();
Object *oname(Object *, std::string const&);
int ddocall();
void docall(Object *);
const char *rndghostname();
char *x_monnam(Monster *, int, const char *, int, bool);
char *l_monnam(Monster *);
char *mon_nam(Monster *);
char *noit_mon_nam(Monster *);
char *Monnam(Monster *);
char *noit_Monnam(Monster *);
char *m_monnam(Monster *);
char *y_monnam(Monster *);
char *Adjmonnam(Monster *, const char *);
char *Amonnam(Monster *);
char *a_monnam(Monster *);
char *distant_monnam(Monster *, int, char *);
const char *rndmonnam();
const char *hcolor(const char *);
const char *rndcolor();
#ifdef REINCARNATION
const char *roguename();
#endif
Object *ReallocateExtraObjectSpace(Object *, int, genericptr_t, int,
                                     const char *);
Object *ReallocateExtraObjectSpace(Object *obj, int oextra_size,
                                     genericptr_t oextra_src, std::string const& name);
char *coyotename(Monster *, char *);

#endif /* DO_NAME_H_ */
