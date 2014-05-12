/*	SCCS Id: @(#)vault.h	3.4	1997/05/01	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef VAULT_H
#define VAULT_H

#include "core/monst.h"

#define FCSIZ (ROWNO + COLNO)
struct fakecorridor {
  xchar fx, fy, ftyp;
};

struct egd {
  int fcbeg, fcend;    /* fcend: first unused pos */
  int vroom;           /* room number of the vault */
  xchar gdx, gdy;      /* goal of guard's walk */
  xchar ogx, ogy;      /* guard's last position */
  d_level gdlevel;     /* level (& dungeon) guard was created in */
  xchar warncnt;       /* number of warnings to follow */
  Bitfield(gddone, 1); /* true iff guard has released player */
  Bitfield(unused, 7);
  struct fakecorridor fakecorr[FCSIZ];
};

#define EGD(mon) ((struct egd *)&(mon)->mextra[0])


bool grddead(Monster *);
char vault_occupied(char *);
void invault();
int gd_move(Monster *);
void paygd();
long hidden_gold();
bool gd_sound();

#endif /* VAULT_H */
