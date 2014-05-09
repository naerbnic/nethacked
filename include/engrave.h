/*	SCCS Id: @(#)engrave.h	3.4	1991/07/31	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef ENGRAVE_H
#define ENGRAVE_H

#include "global.h"

struct engr {
  struct engr *nxt_engr;
  char *engr_txt;
  xchar engr_x, engr_y;
  unsigned engr_lth; /* for save & restore; not length of text */
  long engr_time;    /* moment engraving was (will be) finished */
  xchar engr_type;
#define DUST 1
#define ENGRAVE 2
#define BURN 3
#define MARK 4
#define ENGR_BLOOD 5
#define HEADSTONE 6
#define N_ENGRAVE 6
};

#define newengr(lth) \
  (struct engr *) alloc((unsigned)(lth) + sizeof(struct engr))
#define dealloc_engr(engr) free((genericptr_t)(engr))

char *random_engraving(char *);
void wipeout_text(char *, int, unsigned);
bool can_reach_floor();
const char *surface(int, int);
const char *ceiling(int, int);
struct engr *engr_at(xchar, xchar);
#ifdef ELBERETH
int sengr_at(const char *, xchar, xchar);
#endif
void u_wipe_engr(int);
void wipe_engr_at(xchar, xchar, xchar);
void read_engr_at(int, int);
void make_engr_at(int, int, const char *, long, xchar);
void del_engr_at(int, int);
int freehand();
int doengrave();
void save_engravings(int, int);
void rest_engravings(int);
void del_engr(struct engr *);
void rloc_engr(struct engr *);
void make_grave(int, int, const char *);

#endif /* ENGRAVE_H */
