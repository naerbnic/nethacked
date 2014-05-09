/*	SCCS Id: @(#)attrib.h	3.4	1990/22/02	*/
/* Copyright 1988, Mike Stephenson				  */
/* NetHack may be freely redistributed.  See license for details. */

/*	attrib.h - Header file for character class processing. */

#ifndef ATTRIB_H
#define ATTRIB_H

#include "config.h"
#include "obj.h"

#define A_STR 0
#define A_INT 1
#define A_WIS 2
#define A_DEX 3
#define A_CON 4
#define A_CHA 5

#define A_MAX 6 /* used in rn2() selection of attrib */

#define ABASE(x) (player.acurr.a[x])
#define ABON(x) (player.abon.a[x])
#define AEXE(x) (player.aexe.a[x])
#define ACURR(x) (acurr(x))
#define ACURRSTR (acurrstr())
/* should be: */
/* #define ACURR(x) (ABON(x) + ATEMP(x) + (Upolyd  ? MBASE(x) : ABASE(x)) */
#define MCURR(x) (player.macurr.a[x])
#define AMAX(x) (player.amax.a[x])
#define MMAX(x) (player.mamax.a[x])

#define ATEMP(x) (player.atemp.a[x])
#define ATIME(x) (player.atime.a[x])

/* KMH -- Conveniences when dealing with strength constants */
#define STR18(x) (18 + (x))  /* 18/xx */
#define STR19(x) (100 + (x)) /* For 19 and above */

struct Attributes {
  schar a[A_MAX];
};

#define ATTRMAX(x)                                                   \
  ((x == A_STR && Upolyd && strongmonst(youmonst.data)) ? STR18(100) \
                                                        : urace.attrmax[x])
#define ATTRMIN(x) (urace.attrmin[x])

extern bool adjattrib(int, int, int);
extern void change_luck(schar);
extern int stone_luck(bool);
extern void set_moreluck();
extern void gainstr(Object *, int);
extern void losestr(int);
extern void restore_attrib();
extern void exercise(int, bool);
extern void exerchk();
extern void reset_attribute_clock();
extern void init_attr(int);
extern void redist_attr();
extern void adjabil(int, int);
extern int newhp();
extern schar acurr(int);
extern schar acurrstr();
extern void adjalign(int);

#endif /* ATTRIB_H */
