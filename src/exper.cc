/*	SCCS Id: @(#)exper.c	3.4	2002/11/20	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include <string.h>

#include "hack.h"

STATIC_DCL long newuexp(int);
STATIC_DCL int enermod(int);

STATIC_OVL long newuexp(int lev) {
  if (lev < 10)
    return (10L * (1L << lev));
  if (lev < 20)
    return (10000L * (1L << (lev - 10)));
  return (10000000L * ((long)(lev - 19)));
}

STATIC_OVL int enermod(int en) {
  switch (Role_switch) {
    case PM_PRIEST:
    case PM_WIZARD:
      return (2 * en);
    case PM_HEALER:
    case PM_KNIGHT:
      return ((3 * en) / 2);
    case PM_BARBARIAN:
    case PM_VALKYRIE:
      return ((3 * en) / 4);
    default:
      return (en);
  }
}

/* return # of exp points for mtmp after nk killed */
int experience(Monster *mtmp, int nk) {
  MonsterType *ptr = mtmp->data;
  int i, tmp, tmp2;

  tmp = 1 + mtmp->m_lev * mtmp->m_lev;

  /*	For higher ac values, give extra experience */
  if ((i = find_mac(mtmp)) < 3)
    tmp += (7 - i) * ((i < 0) ? 2 : 1);

  /*	For very fast monsters, give extra experience */
  if (ptr->mmove > NORMAL_SPEED)
    tmp += (ptr->mmove > (3 * NORMAL_SPEED / 2)) ? 5 : 3;

  /*	For each "special" attack type give extra experience */
  for (i = 0; i < NATTK; i++) {
    tmp2 = ptr->mattk[i].aatyp;
    if (tmp2 > AT_BUTT) {
      if (tmp2 == AT_WEAP)
        tmp += 5;
      else if (tmp2 == AT_MAGC)
        tmp += 10;
      else
        tmp += 3;
    }
  }

  /*	For each "special" damage type give extra experience */
  for (i = 0; i < NATTK; i++) {
    tmp2 = ptr->mattk[i].adtyp;
    if (tmp2 > AD_PHYS && tmp2 < AD_BLND)
      tmp += 2 * mtmp->m_lev;
    else if ((tmp2 == AD_DRLI) || (tmp2 == AD_STON) || (tmp2 == AD_SLIM))
      tmp += 50;
    else if (tmp != AD_PHYS)
      tmp += mtmp->m_lev;
    /* extra heavy damage bonus */
    if ((int)(ptr->mattk[i].damd * ptr->mattk[i].damn) > 23)
      tmp += mtmp->m_lev;
    if (tmp2 == AD_WRAP && ptr->mlet == S_EEL && !Amphibious)
      tmp += 1000;
  }

  /*	For certain "extra nasty" monsters, give even more */
  if (extra_nasty(ptr))
    tmp += (7 * mtmp->m_lev);

  /*	For higher level monsters, an additional bonus is given */
  if (mtmp->m_lev > 8)
    tmp += 50;

#ifdef MAIL
  /* Mail daemons put up no fight. */
  if (mtmp->data == &mons[PM_MAIL_DAEMON])
    tmp = 1;
#endif

  return (tmp);
}

void more_experienced(int exp, int rexp) {
  player.uexp += exp;
  player.urexp += 4 * exp + rexp;
  if (exp
#ifdef SCORE_ON_BOTL
      || flags.showscore
#endif
      )
    flags.botl = 1;
  if (player.urexp >= (Role_if(PM_WIZARD) ? 1000 : 2000))
    flags.beginner = 0;
}

/* e.g., hit by drain life attack */
void losexp(const char *drainer) {
  int num;

#ifdef WIZARD
  /* override life-drain resistance when handling an explicit
     wizard mode request to reduce level; never fatal though */
  if (drainer && !strcmp(drainer, "#levelchange"))
    drainer = 0;
  else
#endif
      if (resists_drli(&youmonst))
    return;

  if (player.ulevel > 1) {
    pline("%s level %d.", Goodbye(), player.ulevel--);
    /* remove intrinsic abilities */
    adjabil(player.ulevel + 1, player.ulevel);
    reset_rndmonst(NON_PM); /* new monster selection */
  } else {
    if (drainer) {
      killer_format = KILLED_BY;
      killer = drainer;
      done(DIED);
    }
    /* no drainer or lifesaved */
    player.uexp = 0;
  }
  num = newhp();
  player.uhpmax -= num;
  if (player.uhpmax < 1)
    player.uhpmax = 1;
  player.uhp -= num;
  if (player.uhp < 1)
    player.uhp = 1;
  else if (player.uhp > player.uhpmax)
    player.uhp = player.uhpmax;

  if (player.ulevel < urole.xlev)
    num = rn1((int)ACURR(A_WIS) / 2 + urole.enadv.lornd + urace.enadv.lornd,
              urole.enadv.lofix + urace.enadv.lofix);
  else
    num = rn1((int)ACURR(A_WIS) / 2 + urole.enadv.hirnd + urace.enadv.hirnd,
              urole.enadv.hifix + urace.enadv.hifix);
  num = enermod(num); /* M. Stephenson */
  player.uenmax -= num;
  if (player.uenmax < 0)
    player.uenmax = 0;
  player.uen -= num;
  if (player.uen < 0)
    player.uen = 0;
  else if (player.uen > player.uenmax)
    player.uen = player.uenmax;

  if (player.uexp > 0)
    player.uexp = newuexp(player.ulevel) - 1;
  flags.botl = 1;
}

/*
 * Make experience gaining similar to AD&D(tm), whereby you can at most go
 * up by one level at a time, extra expr possibly helping you along.
 * After all, how much real experience does one get shooting a wand of death
 * at a dragon created with a wand of polymorph??
 */
void newexplevel() {
  if (player.ulevel < MAXULEV && player.uexp >= newuexp(player.ulevel))
    pluslvl(TRUE);
}

void pluslvl(bool incr) {
  int num;

  if (!incr)
    You_feel("more experienced.");
  num = newhp();
  player.uhpmax += num;
  player.uhp += num;
  if (Upolyd) {
    num = rnd(8);
    player.mhmax += num;
    player.mh += num;
  }
  if (player.ulevel < urole.xlev)
    num = rn1((int)ACURR(A_WIS) / 2 + urole.enadv.lornd + urace.enadv.lornd,
              urole.enadv.lofix + urace.enadv.lofix);
  else
    num = rn1((int)ACURR(A_WIS) / 2 + urole.enadv.hirnd + urace.enadv.hirnd,
              urole.enadv.hifix + urace.enadv.hifix);
  num = enermod(num); /* M. Stephenson */
  player.uenmax += num;
  player.uen += num;
  if (player.ulevel < MAXULEV) {
    if (incr) {
      long tmp = newuexp(player.ulevel + 1);
      if (player.uexp >= tmp)
        player.uexp = tmp - 1;
    } else {
      player.uexp = newuexp(player.ulevel);
    }
    ++player.ulevel;
    if (player.ulevelmax < player.ulevel)
      player.ulevelmax = player.ulevel;
    pline("Welcome to experience level %d.", player.ulevel);
    adjabil(player.ulevel - 1, player.ulevel); /* give new intrinsics */
    reset_rndmonst(NON_PM);                    /* new monster selection */
  }
  flags.botl = 1;
}

/* compute a random amount of experience points suitable for the hero's
   experience level:  base number of points needed to reach the current
   level plus a random portion of what it takes to get to the next level */
long rndexp(bool gaining) {
  long minexp, maxexp, diff, factor, result;

  minexp = (player.ulevel == 1) ? 0L : newuexp(player.ulevel - 1);
  maxexp = newuexp(player.ulevel);
  diff = maxexp - minexp, factor = 1L;
  /* make sure that `diff' is an argument which rn2() can handle */
  while (diff >= (long)LARGEST_INT)
    diff /= 2L, factor *= 2L;
  result = minexp + factor * (long)rn2((int)diff);
  /* 3.4.1:  if already at level 30, add to current experience
     points rather than to threshold needed to reach the current
     level; otherwise blessed potions of gain level can result
     in lowering the experience points instead of raising them */
  if (player.ulevel == MAXULEV && gaining) {
    result += (player.uexp - minexp);
    /* avoid wrapping (over 400 blessed potions needed for that...) */
    if (result < player.uexp)
      result = player.uexp;
  }
  return result;
}

/*exper.c*/
