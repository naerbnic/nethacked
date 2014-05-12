/*
 * end.h
 *
 *  Created on: May 9, 2014
 *      Author: brianchin
 */

#ifndef END_H_
#define END_H_

#include "core/monst.h"
#include "core/obj.h"
#include "core/tradstdc.h"

void done1(int);
int done2();
void done_in_by(Monster *);
void panic(const char *, ...) PRINTF_F(1, 2);
void done(int);
void container_contents(Object *, bool, bool);
#ifdef DUMP_LOG
void dump(char const *, char const *);
void do_containerconts(Object *, bool, bool, bool, bool);
#endif
void terminate(int);
int num_genocides();

#endif /* END_H_ */
