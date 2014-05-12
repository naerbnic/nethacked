/*	SCCS Id: @(#)rect.h	3.4	1990/02/22	*/
/* Copyright (c) 1990 by Jean-Christophe Collet			  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef RECT_H
#define RECT_H

struct NhRegion;

typedef struct nhrect {
  xchar lx, ly;
  xchar hx, hy;
} NhRect;

void init_rect();
NhRect *get_rect(NhRect *);
NhRect *rnd_rect();
void remove_rect(NhRect *);
void add_rect(NhRect *);
void split_rects(NhRect *, NhRect *);

#endif /* RECT_H */
