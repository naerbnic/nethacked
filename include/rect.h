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

/* ## region.c ### */
void clear_regions();
void run_regions();
bool in_out_region(xchar, xchar);
bool m_in_out_region(Monster *, xchar, xchar);
void update_player_regions();
void update_monster_region(Monster *);
NhRegion *visible_region_at(xchar, xchar);
void show_region(NhRegion *, xchar, xchar);
void save_regions(int, int);
void rest_regions(int, bool);
NhRegion *create_gas_cloud(xchar, xchar, int, int);

#endif /* RECT_H */
