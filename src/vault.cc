/*	SCCS Id: @(#)vault.c	3.4	2003/01/15	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include <string.h>

#include "hack.h"
#include "vault.h"

STATIC_DCL Monster *findgd();

#define g_monnam(mtmp) \
	x_monnam(mtmp, ARTICLE_NONE, (char *)0, SUPPRESS_IT, FALSE)

#ifdef OVLB

STATIC_DCL bool clear_fcorr(Monster *,bool);
STATIC_DCL void restfakecorr(Monster *);
STATIC_DCL bool in_fcorridor(Monster *,int,int);
STATIC_DCL void move_gold(Object *,int);
STATIC_DCL void wallify_vault(Monster *);

STATIC_OVL bool clear_fcorr(Monster *grd, bool forceshow) {
	int fcx, fcy, fcbeg;
	Monster *mtmp;

	if (!on_level(&(EGD(grd)->gdlevel), &player.uz)) return TRUE;

	while((fcbeg = EGD(grd)->fcbeg) < EGD(grd)->fcend) {
		fcx = EGD(grd)->fakecorr[fcbeg].fx;
		fcy = EGD(grd)->fakecorr[fcbeg].fy;
		if((grd->mhp <= 0 || !in_fcorridor(grd, player.ux, player.uy)) &&
				   EGD(grd)->gddone)
			forceshow = TRUE;
		if((player.ux == fcx && player.uy == fcy && grd->mhp > 0)
			|| (!forceshow && couldsee(fcx,fcy))
			|| (Punished && !carried(uball)
				&& uball->ox == fcx && uball->oy == fcy))
			return FALSE;

		if ((mtmp = m_at(fcx,fcy)) != 0) {
			if(mtmp->isgd) return(FALSE);
			else if(!in_fcorridor(grd, player.ux, player.uy)) {
			    if(mtmp->mtame) yelp(mtmp);
			    (void) rloc(mtmp, FALSE);
			}
		}
		levl[fcx][fcy].typ = EGD(grd)->fakecorr[fcbeg].ftyp;
		map_location(fcx, fcy, 1);	/* bypass vision */
		if(!ACCESSIBLE(levl[fcx][fcy].typ)) block_point(fcx,fcy);
		EGD(grd)->fcbeg++;
	}
	if(grd->mhp <= 0) {
	    pline_The("corridor disappears.");
	    if(IS_ROCK(levl[player.ux][player.uy].typ)) You("are encased in rock.");
	}
	return(TRUE);
}

STATIC_OVL void restfakecorr(Monster *grd) {
	/* it seems you left the corridor - let the guard disappear */
	if(clear_fcorr(grd, FALSE)) mongone(grd);
}

/* called in mon.c */
bool grddead(Monster *grd) {
	bool dispose = clear_fcorr(grd, TRUE);

	if(!dispose) {
		/* see comment by newpos in gd_move() */
		remove_monster(grd->mx, grd->my);
		newsym(grd->mx, grd->my);
		place_monster(grd, 0, 0);
		EGD(grd)->ogx = grd->mx;
		EGD(grd)->ogy = grd->my;
		dispose = clear_fcorr(grd, TRUE);
	}
	return(dispose);
}

STATIC_OVL bool in_fcorridor(Monster *grd, int x, int y) {
	int fci;

	for(fci = EGD(grd)->fcbeg; fci < EGD(grd)->fcend; fci++)
		if(x == EGD(grd)->fakecorr[fci].fx &&
				y == EGD(grd)->fakecorr[fci].fy)
			return(TRUE);
	return(FALSE);
}

STATIC_OVL
Monster * findgd() {
	Monster *mtmp;

	for(mtmp = fmon; mtmp; mtmp = mtmp->nmon)
	    if(mtmp->isgd && !mtmp->dead() && on_level(&(EGD(mtmp)->gdlevel), &player.uz))
		return(mtmp);
	return((Monster *)0);
}

#endif /* OVLB */
#ifdef OVL0

char vault_occupied(char *array) {
	char *ptr;

	for (ptr = array; *ptr; ptr++)
		if (rooms[*ptr - ROOMOFFSET].rtype == VAULT)
			return(*ptr);
	return('\0');
}

void invault() {
#ifdef BSD_43_BUG
    int dummy;		/* hack to avoid schain botch */
#endif
    Monster *guard;
    int trycount, vaultroom = (int)vault_occupied(player.urooms);

    if(!vaultroom) {
	player.uinvault = 0;
	return;
    }

    vaultroom -= ROOMOFFSET;

    guard = findgd();
    if(++player.uinvault % 30 == 0 && !guard) { /* if time ok and no guard now. */
	char buf[BUFSZ];
	int x, y, dd, gx, gy;
	int lx = 0, ly = 0;
#ifdef GOLDOBJ
        long umoney;
#endif
	/* first find the goal for the guard */
	for(dd = 2; (dd < ROWNO || dd < COLNO); dd++) {
	  for(y = player.uy-dd; y <= player.uy+dd; ly = y, y++) {
	    if(y < 0 || y > ROWNO-1) continue;
	    for(x = player.ux-dd; x <= player.ux+dd; lx = x, x++) {
	      if(y != player.uy-dd && y != player.uy+dd && x != player.ux-dd)
		x = player.ux+dd;
	      if(x < 1 || x > COLNO-1) continue;
	      if(levl[x][y].typ == CORR) {
		  if(x < player.ux) lx = x + 1;
		  else if(x > player.ux) lx = x - 1;
		  else lx = x;
		  if(y < player.uy) ly = y + 1;
		  else if(y > player.uy) ly = y - 1;
		  else ly = y;
		  if(levl[lx][ly].typ != STONE && levl[lx][ly].typ != CORR)
		      goto incr_radius;
		  goto fnd;
	      }
	    }
	  }
incr_radius: ;
	}
	impossible("Not a single corridor on this level??");
	tele();
	return;
fnd:
	gx = x; gy = y;

	/* next find a good place for a door in the wall */
	x = player.ux; y = player.uy;
	if(levl[x][y].typ != ROOM) {  /* player dug a door and is in it */
		if(levl[x+1][y].typ == ROOM)  x = x + 1;
		else if(levl[x][y+1].typ == ROOM) y = y + 1;
		else if(levl[x-1][y].typ == ROOM) x = x - 1;
		else if(levl[x][y-1].typ == ROOM) y = y - 1;
		else if(levl[x+1][y+1].typ == ROOM) {
			x = x + 1;
			y = y + 1;
		} else if (levl[x-1][y-1].typ == ROOM) {
			x = x - 1;
			y = y - 1;
		} else if (levl[x+1][y-1].typ == ROOM) {
			x = x + 1;
			y = y - 1;
		} else if (levl[x-1][y+1].typ == ROOM) {
			x = x - 1;
			y = y + 1;
		}
	}
	while(levl[x][y].typ == ROOM) {
		int dx,dy;

		dx = (gx > x) ? 1 : (gx < x) ? -1 : 0;
		dy = (gy > y) ? 1 : (gy < y) ? -1 : 0;
		if(abs(gx-x) >= abs(gy-y))
			x += dx;
		else
			y += dy;
	}
	if(x == player.ux && y == player.uy) {
		if(levl[x+1][y].typ == HWALL || levl[x+1][y].typ == DOOR)
			x = x + 1;
		else if(levl[x-1][y].typ == HWALL || levl[x-1][y].typ == DOOR)
			x = x - 1;
		else if(levl[x][y+1].typ == VWALL || levl[x][y+1].typ == DOOR)
			y = y + 1;
		else if(levl[x][y-1].typ == VWALL || levl[x][y-1].typ == DOOR)
			y = y - 1;
		else return;
	}

	/* make something interesting happen */
	if(!(guard = makemon(&mons[PM_GUARD], x, y, NO_MM_FLAGS))) return;
	guard->isgd = 1;
	guard->mpeaceful = 1;
	set_malign(guard);
	EGD(guard)->gddone = 0;
	EGD(guard)->ogx = x;
	EGD(guard)->ogy = y;
	assign_level(&(EGD(guard)->gdlevel), &player.uz);
	EGD(guard)->vroom = vaultroom;
	EGD(guard)->warncnt = 0;

	reset_faint();			/* if fainted - wake up */
	if (canspotmon(guard))
	    pline("Suddenly one of the Vault's %s enters!",
		  makeplural(g_monnam(guard)));
	else
	    pline("Someone else has entered the Vault.");
	newsym(guard->mx,guard->my);
	if (youmonst.m_ap_type == M_AP_OBJECT || player.uundetected) {
	    if (youmonst.m_ap_type == M_AP_OBJECT &&
			youmonst.mappearance != GOLD_PIECE)
	    	verbalize("Hey! Who left that %s in here?", mimic_obj_name(&youmonst));
	    /* You're mimicking some object or you're hidden. */
	    pline("Puzzled, %s turns around and leaves.", mhe(guard));
	    mongone(guard);
	    return;
	}
	if (Strangled || is_silent(youmonst.data) || multi < 0) {
	    /* [we ought to record whether this this message has already
	       been given in order to vary it upon repeat visits, but
	       discarding the monster and its egd data renders that hard] */
	    verbalize("I'll be back when you're ready to speak to me!");
	    mongone(guard);
	    return;
	}

	stop_occupation();		/* if occupied, stop it *now* */
	if (multi > 0) { nomul(0, 0); unmul((char *)0); }
	trycount = 5;
	do {
	    getlin("\"Hello stranger, who are you?\" -", buf);
	    (void) mungspaces(buf);
	} while (!letter(buf[0]) && --trycount > 0);

	if (player.ualign.type == A_LAWFUL &&
	    /* ignore trailing text, in case player includes character's rank */
	    strncmpi(buf, plname, (int) strlen(plname)) != 0) {
		adjalign(-1);		/* Liar! */
	}

	if (!strcmpi(buf, "Croesus") || !strcmpi(buf, "Kroisos")
#ifdef TOURIST
		|| !strcmpi(buf, "Creosote")
#endif
	    ) {
	    if (!mvitals[PM_CROESUS].died) {
		verbalize("Oh, yes, of course.  Sorry to have disturbed you.");
		mongone(guard);
	    } else {
		setmangry(guard);
		verbalize("Back from the dead, are you?  I'll remedy that!");
		/* don't want guard to waste next turn wielding a weapon */
		if (!guard->weapon()) {
		    guard->weapon_check = NEED_HTH_WEAPON;
		    (void) mon_wield_item(guard);
		}
	    }
	    return;
	}
	verbalize("I don't know you.");
#ifndef GOLDOBJ
	if (!player.ugold && !hidden_gold())
	    verbalize("Please follow me.");
	else {
	    if (!player.ugold)
		verbalize("You have hidden gold.");
	    verbalize("Most likely all your gold was stolen from this vault.");
	    verbalize("Please drop that gold and follow me.");
	}
#else
        umoney = money_cnt(invent);
	if (!umoney && !hidden_gold())
	    verbalize("Please follow me.");
	else {
	    if (!umoney)
		verbalize("You have hidden money.");
	    verbalize("Most likely all your money was stolen from this vault.");
	    verbalize("Please drop that money and follow me.");
	}
#endif
	EGD(guard)->gdx = gx;
	EGD(guard)->gdy = gy;
	EGD(guard)->fcbeg = 0;
	EGD(guard)->fakecorr[0].fx = x;
	EGD(guard)->fakecorr[0].fy = y;
	if(IS_WALL(levl[x][y].typ))
	    EGD(guard)->fakecorr[0].ftyp = levl[x][y].typ;
	else { /* the initial guard location is a dug door */
	    int vlt = EGD(guard)->vroom;
	    xchar lowx = rooms[vlt].lx, hix = rooms[vlt].hx;
	    xchar lowy = rooms[vlt].ly, hiy = rooms[vlt].hy;

	    if(x == lowx-1 && y == lowy-1)
		EGD(guard)->fakecorr[0].ftyp = TLCORNER;
	    else if(x == hix+1 && y == lowy-1)
		EGD(guard)->fakecorr[0].ftyp = TRCORNER;
	    else if(x == lowx-1 && y == hiy+1)
		EGD(guard)->fakecorr[0].ftyp = BLCORNER;
	    else if(x == hix+1 && y == hiy+1)
		EGD(guard)->fakecorr[0].ftyp = BRCORNER;
	    else if(y == lowy-1 || y == hiy+1)
		EGD(guard)->fakecorr[0].ftyp = HWALL;
	    else if(x == lowx-1 || x == hix+1)
		EGD(guard)->fakecorr[0].ftyp = VWALL;
	}
	levl[x][y].typ = DOOR;
	levl[x][y].doormask = D_NODOOR;
	unblock_point(x, y);		/* doesn't block light */
	EGD(guard)->fcend = 1;
	EGD(guard)->warncnt = 1;
    }
}

#endif /* OVL0 */
#ifdef OVLB

STATIC_OVL void move_gold(Object *gold, int vroom) {
	xchar nx, ny;

	RemoveObjectFromFloor(gold);
	newsym(gold->ox, gold->oy);
	nx = rooms[vroom].lx + rn2(2);
	ny = rooms[vroom].ly + rn2(2);
	PlaceObject(gold, nx, ny);
	stackobj(gold);
	newsym(nx,ny);
}

STATIC_OVL void wallify_vault(Monster *grd) {
	int x, y, typ;
	int vlt = EGD(grd)->vroom;
	char tmp_viz;
	xchar lox = rooms[vlt].lx - 1, hix = rooms[vlt].hx + 1,
	      loy = rooms[vlt].ly - 1, hiy = rooms[vlt].hy + 1;
	Monster *mon;
	Object *gold;
	Trap *trap;
	bool fixed = FALSE;
	bool movedgold = FALSE;

	for (x = lox; x <= hix; x++)
	    for (y = loy; y <= hiy; y++) {
		/* if not on the room boundary, skip ahead */
		if (x != lox && x != hix && y != loy && y != hiy) continue;

		if (!IS_WALL(levl[x][y].typ) && !in_fcorridor(grd, x, y)) {
		    if ((mon = m_at(x, y)) != 0 && mon != grd) {
			if (mon->mtame) yelp(mon);
			(void) rloc(mon, FALSE);
		    }
		    if ((gold = g_at(x, y)) != 0) {
			move_gold(gold, EGD(grd)->vroom);
			movedgold = TRUE;
		    }
		    if ((trap = t_at(x, y)) != 0)
			deltrap(trap);
		    if (x == lox)
			typ = (y == loy) ? TLCORNER :
			      (y == hiy) ? BLCORNER : VWALL;
		    else if (x == hix)
			typ = (y == loy) ? TRCORNER :
			      (y == hiy) ? BRCORNER : VWALL;
		    else  /* not left or right side, must be top or bottom */
			typ = HWALL;
		    levl[x][y].typ = typ;
		    levl[x][y].doormask = 0;
		    /*
		     * hack: player knows walls are restored because of the
		     * message, below, so show this on the screen.
		     */
		    tmp_viz = viz_array[y][x];
		    viz_array[y][x] = IN_SIGHT|COULD_SEE;
		    newsym(x,y);
		    viz_array[y][x] = tmp_viz;
		    block_point(x,y);
		    fixed = TRUE;
		}
	    }

	if(movedgold || fixed) {
	    if(in_fcorridor(grd, grd->mx, grd->my) || cansee(grd->mx, grd->my))
		pline_The("%s whispers an incantation.", g_monnam(grd));
	    else You_hear("a distant chant.");
	    if(movedgold)
		pline("A mysterious force moves the gold into the vault.");
	    if(fixed)
		pline_The("damaged vault's walls are magically restored!");
	}
}

/*
 * return  1: guard moved,  0: guard didn't,  -1: let m_move do it,  -2: died
 */
int gd_move(Monster *grd) {
	int x, y, nx, ny, m, n;
	int dx, dy, gx, gy, fci;
	uchar typ;
	struct fakecorridor *fcp;
	struct egd *egrd = EGD(grd);
	struct rm *crm;
	bool goldincorridor = FALSE,
			 u_in_vault = vault_occupied(player.urooms)? TRUE : FALSE,
			 grd_in_vault = *in_rooms(grd->mx, grd->my, VAULT)?
					TRUE : FALSE;
	bool disappear_msg_seen = FALSE, semi_dead = (grd->mhp <= 0);
#ifndef GOLDOBJ
	bool u_carry_gold = ((player.ugold + hidden_gold()) > 0L);
#else
        long umoney = money_cnt(invent);
	bool u_carry_gold = ((umoney + hidden_gold()) > 0L);
#endif
	bool see_guard;

	if(!on_level(&(egrd->gdlevel), &player.uz)) return(-1);
	nx = ny = m = n = 0;
	if(!u_in_vault && !grd_in_vault)
	    wallify_vault(grd);
	if(!grd->mpeaceful) {
	    if(semi_dead) {
		egrd->gddone =1;
		goto newpos;
	    }
	    if(!u_in_vault &&
	       (grd_in_vault ||
		(in_fcorridor(grd, grd->mx, grd->my) &&
		 !in_fcorridor(grd, player.ux, player.uy)))) {
		(void) rloc(grd, FALSE);
		wallify_vault(grd);
		(void) clear_fcorr(grd, TRUE);
		goto letknow;
	    }
	    if(!in_fcorridor(grd, grd->mx, grd->my))
		(void) clear_fcorr(grd, TRUE);
	    return(-1);
	}
	if(abs(egrd->ogx - grd->mx) > 1 ||
			abs(egrd->ogy - grd->my) > 1)
		return(-1);	/* teleported guard - treat as monster */
	if(egrd->fcend == 1) {
	    if(u_in_vault &&
			(u_carry_gold || um_dist(grd->mx, grd->my, 1))) {
		if(egrd->warncnt == 3)
			verbalize("I repeat, %sfollow me!",
				u_carry_gold ? (
#ifndef GOLDOBJ
					  !player.ugold ?
					  "drop that hidden gold and " :
					  "drop that gold and ") : "");
#else
					  !umoney ?
					  "drop that hidden money and " :
					  "drop that money and ") : "");
#endif
		if(egrd->warncnt == 7) {
			m = grd->mx;
			n = grd->my;
			verbalize("You've been warned, knave!");
			mnexto(grd);
			levl[m][n].typ = egrd->fakecorr[0].ftyp;
			newsym(m,n);
			grd->mpeaceful = 0;
			return(-1);
		}
		/* not fair to get mad when (s)he's fainted or paralyzed */
		if(!is_fainted() && multi >= 0) egrd->warncnt++;
		return(0);
	    }

	    if (!u_in_vault) {
		if (u_carry_gold) {	/* player teleported */
		    m = grd->mx;
		    n = grd->my;
		    (void) rloc(grd, FALSE);
		    levl[m][n].typ = egrd->fakecorr[0].ftyp;
		    newsym(m,n);
		    grd->mpeaceful = 0;
letknow:
		    if (!cansee(grd->mx, grd->my) || !mon_visible(grd))
			You_hear("the shrill sound of a guard's whistle.");
		    else
			You(um_dist(grd->mx, grd->my, 2) ?
			    "see an angry %s approaching." :
			    "are confronted by an angry %s.",
			    g_monnam(grd));
		    return(-1);
		} else {
		    verbalize("Well, begone.");
		    wallify_vault(grd);
		    egrd->gddone = 1;
		    goto cleanup;
		}
	    }
	}

	if(egrd->fcend > 1) {
	    if(egrd->fcend > 2 && in_fcorridor(grd, grd->mx, grd->my) &&
		  !egrd->gddone && !in_fcorridor(grd, player.ux, player.uy) &&
		  levl[egrd->fakecorr[0].fx][egrd->fakecorr[0].fy].typ
				 == egrd->fakecorr[0].ftyp) {
		pline_The("%s, confused, disappears.", g_monnam(grd));
		disappear_msg_seen = TRUE;
		goto cleanup;
	    }
	    if(u_carry_gold &&
		    (in_fcorridor(grd, player.ux, player.uy) ||
		    /* cover a 'blind' spot */
		    (egrd->fcend > 1 && u_in_vault))) {
		if(!grd->mx) {
			restfakecorr(grd);
			return(-2);
		}
		if(egrd->warncnt < 6) {
			egrd->warncnt = 6;
			verbalize("Drop all your gold, scoundrel!");
			return(0);
		} else {
			verbalize("So be it, rogue!");
			grd->mpeaceful = 0;
			return(-1);
		}
	    }
	}
	for(fci = egrd->fcbeg; fci < egrd->fcend; fci++)
	    if(g_at(egrd->fakecorr[fci].fx, egrd->fakecorr[fci].fy)){
		m = egrd->fakecorr[fci].fx;
		n = egrd->fakecorr[fci].fy;
		goldincorridor = TRUE;
	    }
	if(goldincorridor && !egrd->gddone) {
		x = grd->mx;
		y = grd->my;
		if (m == player.ux && n == player.uy) {
		    Object *gold = g_at(m,n);
		    /* Grab the gold from between the hero's feet.  */
#ifndef GOLDOBJ
		    grd->mgold += gold->quan;
		    delobj(gold);
#else
		    RemoveObjectFromStorage(gold);
		    add_to_minv(grd, gold);
#endif
		    newsym(m,n);
		} else if (m == x && n == y) {
		    mpickgold(grd);	/* does a newsym */
		} else {
		    /* just for insurance... */
		    if (MON_AT(m, n) && m != grd->mx && n != grd->my) {
			verbalize("Out of my way, scum!");
			(void) rloc(m_at(m, n), FALSE);
		    }
		    remove_monster(grd->mx, grd->my);
		    newsym(grd->mx, grd->my);
		    place_monster(grd, m, n);
		    mpickgold(grd);	/* does a newsym */
		}
		if(cansee(m,n))
		    pline("%s%s picks up the gold.", Monnam(grd),
				grd->mpeaceful ? " calms down and" : "");
		if(x != grd->mx || y != grd->my) {
		    remove_monster(grd->mx, grd->my);
		    newsym(grd->mx, grd->my);
		    place_monster(grd, x, y);
		    newsym(x, y);
		}
		if(!grd->mpeaceful) return(-1);
		else {
		    egrd->warncnt = 5;
		    return(0);
		}
	}
	if(um_dist(grd->mx, grd->my, 1) || egrd->gddone) {
		if(!egrd->gddone && !rn2(10)) verbalize("Move along!");
		restfakecorr(grd);
		return(0);	/* didn't move */
	}
	x = grd->mx;
	y = grd->my;

	if(u_in_vault) goto nextpos;

	/* look around (hor & vert only) for accessible places */
	for(nx = x-1; nx <= x+1; nx++) for(ny = y-1; ny <= y+1; ny++) {
	  if((nx == x || ny == y) && (nx != x || ny != y) && isok(nx, ny)) {

	    typ = (crm = &levl[nx][ny])->typ;
	    if(!IS_STWALL(typ) && !IS_POOL(typ)) {

		if(in_fcorridor(grd, nx, ny))
			goto nextnxy;

		if(*in_rooms(nx,ny,VAULT))
			continue;

		/* seems we found a good place to leave him alone */
		egrd->gddone = 1;
		if(ACCESSIBLE(typ)) goto newpos;
#ifdef STUPID
		if (typ == SCORR)
		    crm->typ = CORR;
		else
		    crm->typ = DOOR;
#else
		crm->typ = (typ == SCORR) ? CORR : DOOR;
#endif
		if(crm->typ == DOOR) crm->doormask = D_NODOOR;
		goto proceed;
	    }
	  }
nextnxy:	;
	}
nextpos:
	nx = x;
	ny = y;
	gx = egrd->gdx;
	gy = egrd->gdy;
	dx = (gx > x) ? 1 : (gx < x) ? -1 : 0;
	dy = (gy > y) ? 1 : (gy < y) ? -1 : 0;
	if(abs(gx-x) >= abs(gy-y)) nx += dx; else ny += dy;

	while((typ = (crm = &levl[nx][ny])->typ) != 0) {
	/* in view of the above we must have IS_WALL(typ) or typ == POOL */
	/* must be a wall here */
		if(isok(nx+nx-x,ny+ny-y) && !IS_POOL(typ) &&
		    IS_ROOM(levl[nx+nx-x][ny+ny-y].typ)){
			crm->typ = DOOR;
			crm->doormask = D_NODOOR;
			goto proceed;
		}
		if(dy && nx != x) {
			nx = x; ny = y+dy;
			continue;
		}
		if(dx && ny != y) {
			ny = y; nx = x+dx; dy = 0;
			continue;
		}
		/* I don't like this, but ... */
		if(IS_ROOM(typ)) {
			crm->typ = DOOR;
			crm->doormask = D_NODOOR;
			goto proceed;
		}
		break;
	}
	crm->typ = CORR;
proceed:
	unblock_point(nx, ny);	/* doesn't block light */
	if (cansee(nx,ny))
	    newsym(nx,ny);

	fcp = &(egrd->fakecorr[egrd->fcend]);
	if(egrd->fcend++ == FCSIZ) panic("fakecorr overflow");
	fcp->fx = nx;
	fcp->fy = ny;
	fcp->ftyp = typ;
newpos:
	if(egrd->gddone) {
		/* The following is a kludge.  We need to keep    */
		/* the guard around in order to be able to make   */
		/* the fake corridor disappear as the player      */
		/* moves out of it, but we also need the guard    */
		/* out of the way.  We send the guard to never-   */
		/* never land.  We set ogx ogy to mx my in order  */
		/* to avoid a check at the top of this function.  */
		/* At the end of the process, the guard is killed */
		/* in restfakecorr().				  */
cleanup:
		x = grd->mx; y = grd->my;

		see_guard = canspotmon(grd);
		wallify_vault(grd);
		remove_monster(grd->mx, grd->my);
		newsym(grd->mx,grd->my);
		place_monster(grd, 0, 0);
		egrd->ogx = grd->mx;
		egrd->ogy = grd->my;
		restfakecorr(grd);
		if(!semi_dead && (in_fcorridor(grd, player.ux, player.uy) ||
				     cansee(x, y))) {
		    if (!disappear_msg_seen && see_guard)
			pline("Suddenly, the %s disappears.", g_monnam(grd));
		    return(1);
		}
		return(-2);
	}
	egrd->ogx = grd->mx;	/* update old positions */
	egrd->ogy = grd->my;
	remove_monster(grd->mx, grd->my);
	place_monster(grd, nx, ny);
	newsym(grd->mx,grd->my);
	restfakecorr(grd);
	return(1);
}

/* Routine when dying or quitting with a vault guard around */
void paygd() {
	Monster *grd = findgd();
#ifndef GOLDOBJ
	Object *gold;
#else
        long umoney = money_cnt(invent);
	Object *coins, *nextcoins;
#endif
	int gx,gy;
	char buf[BUFSZ];

#ifndef GOLDOBJ
	if (!player.ugold || !grd) return;
#else
	if (!umoney || !grd) return;
#endif

	if (player.uinvault) {
	    Your("%ld %s goes into the Magic Memory Vault.",
#ifndef GOLDOBJ
		player.ugold,
		currency(player.ugold));
#else
		umoney,
		currency(umoney));
#endif
	    gx = player.ux;
	    gy = player.uy;
	} else {
	    if(grd->mpeaceful) { /* guard has no "right" to your gold */
		mongone(grd);
		return;
	    }
	    mnexto(grd);
	    pline("%s remits your gold to the vault.", Monnam(grd));
	    gx = rooms[EGD(grd)->vroom].lx + rn2(2);
	    gy = rooms[EGD(grd)->vroom].ly + rn2(2);
	    sprintf(buf,
		"To Croesus: here's the gold recovered from %s the %s.",
		plname, mons[player.umonster].mname);
	    make_grave(gx, gy, buf);
	}
#ifndef GOLDOBJ
	PlaceObject(gold = mkgoldobj(player.ugold), gx, gy);
	stackobj(gold);
#else
        for (coins = invent; coins; coins = nextcoins) {
            nextcoins = coins->nobj;
	    if (objects[coins->otyp].oc_class == COIN_CLASS) {
	        freeinv(coins);
                PlaceObject(coins, gx, gy);
		stackobj(coins);
	    }
        }
#endif
	mongone(grd);
}

long hidden_gold() {
	long value = 0L;
	Object *obj;

	for (obj = invent; obj; obj = obj->nobj)
	    if (Has_contents(obj))
		value += contained_gold(obj);
	/* unknown gold stuck inside statues may cause some consternation... */

	return(value);
}

/* prevent "You hear footsteps.." when inappropriate */
bool gd_sound() {
	Monster *grd = findgd();

	if (vault_occupied(player.urooms)) return(FALSE);
	else return((bool)(grd == (Monster *)0));
}

#endif /* OVLB */

/*vault.c*/
