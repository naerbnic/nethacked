/*	SCCS Id: @(#)timeout.h	3.4	1999/02/13	*/
/* Copyright 1994, Dean Luick					  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef TIMEOUT_H
#define TIMEOUT_H

/* generic timeout function */
typedef void (*timeout_proc)(genericptr_t, long);

/* kind of timer */
#define TIMER_LEVEL 0   /* event specific to level */
#define TIMER_GLOBAL 1  /* event follows current play */
#define TIMER_OBJECT 2  /* event follows a object */
#define TIMER_MONSTER 3 /* event follows a monster */

/* save/restore timer ranges */
#define RANGE_LEVEL 0  /* save/restore timers staying on level */
#define RANGE_GLOBAL 1 /* save/restore timers following global play */

/*
 * Timeout functions.  Add a define here, then put it in the table
 * in timeout.c.  "One more level of indirection will fix everything."
 */
#define ROT_ORGANIC 0 /* for buried organics */
#define ROT_CORPSE 1
#define REVIVE_MON 2
#define BURN_OBJECT 3
#define HATCH_EGG 4
#define FIG_TRANSFORM 5
#define NUM_TIME_FUNCS 6

/* used in timeout.c */
struct TimerElement {
  TimerElement *next;       /* next item in chain */
  long timeout;             /* when we time out */
  unsigned long tid;        /* timer ID */
  short kind;               /* kind of use */
  short func_index;         /* what to call when we time out */
  genericptr_t arg;         /* pointer to timeout argument */
  Bitfield(needs_fixup, 1); /* does arg need to be patched? */
};


void burn_away_slime();
void nh_timeout();
void fall_asleep(int, bool);
void attach_egg_hatch_timeout(Object *);
void attach_fig_transform_timeout(Object *);
void kill_egg(Object *);
void hatch_egg(genericptr_t, long);
void learn_egg_type(int);
void burn_object(genericptr_t, long);
void begin_burn(Object *, bool);
void end_burn(Object *, bool);
void do_storms();
bool start_timer(long, short, short, genericptr_t);
long stop_timer(short, genericptr_t);
void run_timers();
void obj_move_timers(Object *, Object *);
void obj_split_timers(Object *, Object *);
void obj_stop_timers(Object *);
bool obj_is_local(Object *);
void save_timers(int, int, int);
void restore_timers(int, int, bool, long);
void relink_timers(bool);
#ifdef WIZARD
int wiz_timeout_queue();
void timer_sanity_check();
#endif

#endif /* TIMEOUT_H */
