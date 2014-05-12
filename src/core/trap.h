/*	SCCS Id: @(#)trap.h	3.4	2000/08/30	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/* note for 3.1.0 and later: no longer manipulated by 'makedefs' */

#ifndef TRAP_H
#define TRAP_H

#include "core/dungeon.h"

union vlaunchinfo {
  short v_launch_otyp; /* type of object to be triggered */
  coord v_launch2;     /* secondary launch point (for boulders) */
};

struct Trap {
  Trap *ntrap;
  xchar tx, ty;
  d_level dst; /* destination for portals */
  coord launch;
  Bitfield(ttyp, 5);
  Bitfield(tseen, 1);
  Bitfield(once, 1);
  Bitfield(madeby_u, 1); /* So monsters may take offence when you trap
                            them.	Recognizing who made the trap isn't
                            completely unreasonable, everybody has
                            their own style.  This flag is also needed
                            when you untrap a monster.  It would be too
                            easy to make a monster peaceful if you could
                            set a trap for it and then untrap it. */
  union vlaunchinfo vl;
#define launch_otyp vl.v_launch_otyp
#define launch2 vl.v_launch2
};

extern Trap *ftrap;
#define newtrap() (Trap *) alloc(sizeof(Trap))
#define dealloc_trap(trap) free((genericptr_t)(trap))

/* reasons for statue animation */
#define ANIMATE_NORMAL 0
#define ANIMATE_SHATTER 1
#define ANIMATE_SPELL 2

/* reasons for animate_statue's failure */
#define AS_OK 0            /* didn't fail */
#define AS_NO_MON 1        /* makemon failed */
#define AS_MON_IS_UNIQUE 2 /* statue monster is unique */

/* Note: if adding/removing a trap, adjust trap_engravings[] in mklev.c */

/* unconditional traps */
#define NO_TRAP 0
#define ARROW_TRAP 1
#define DART_TRAP 2
#define ROCKTRAP 3
#define SQKY_BOARD 4
#define BEAR_TRAP 5
#define LANDMINE 6
#define ROLLING_BOULDER_TRAP 7
#define SLP_GAS_TRAP 8
#define RUST_TRAP 9
#define FIRE_TRAP 10
#define PIT 11
#define SPIKED_PIT 12
#define HOLE 13
#define TRAPDOOR 14
#define TELEP_TRAP 15
#define LEVEL_TELEP 16
#define MAGIC_PORTAL 17
#define WEB 18
#define STATUE_TRAP 19
#define MAGIC_TRAP 20
#define ANTI_MAGIC 21
#define POLY_TRAP 22
#define TRAPNUM 23


bool burnarmor(Monster *);
bool rust_dmg(Object *, const char *, int, bool, Monster *);
void grease_protect(Object *, const char *, Monster *);
Trap *maketrap(int, int, int);
void fall_through(bool);
Monster *animate_statue(Object *, xchar, xchar, int, int *);
Monster *activate_statue_trap(Trap *, xchar, xchar, bool);
void dotrap(Trap *, unsigned);
void seetrap(Trap *);
int mintrap(Monster *);
void instapetrify(const char *);
void minstapetrify(Monster *, bool);
void selftouch(const char *);
void mselftouch(Monster *, const char *, bool);
void float_up();
void fill_pit(int, int);
int float_down(long, long);
int fire_damage(Object *, bool, bool, xchar, xchar);
bool water_damage(Object *, bool, bool);
bool drown();
void drain_en(int);
int dountrap();
int untrap(bool);
bool chest_trap(Object *, int, bool);
void deltrap(Trap *);
bool delfloortrap(Trap *);
Trap *t_at(int, int);
void b_trapped(const char *, int);
bool unconscious();
bool lava_effects();
void blow_up_landmine(Trap *);
int launch_obj(short, int, int, int, int, int);

#endif /* TRAP_H */
