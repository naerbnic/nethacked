/*	SCCS Id: @(#)botl.c	3.4	1996/07/15	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "core/botl.h"

#include <string.h>

#include "core/hack.h"
#include "core/hacklib.h"
#if defined(HPMON) && defined(TEXTCOLOR)
#ifndef WINTTY_H
#include "core/wintty.h"
#endif
#endif

extern const char *hu_stat[]; /* defined in eat.c */

const char *const enc_stat[] = {"",         "Burdened",  "Stressed",
                                "Strained", "Overtaxed", "Overloaded"};

STATIC_DCL void bot1();
STATIC_DCL void bot2();

/* MAXCO must hold longest uncompressed status line, and must be larger
 * than COLNO
 *
 * longest practical second status line at the moment is
 *	Astral Plane $:12345 HP:700(700) Pw:111(111) AC:-127 Xp:30/123456789
 *	T:123456 Satiated Conf FoodPois Ill Blind Stun Hallu Overloaded
 * -- or somewhat over 130 characters
 */
#if COLNO <= 140
#define MAXCO 160
#else
#define MAXCO (COLNO + 20)
#endif

#ifndef OVLB
STATIC_DCL int mrank_sz;
#else  /* OVLB */
STATIC_OVL int mrank_sz = 0; /* loaded by max_rank_sz (from u_init) */
#endif /* OVLB */

STATIC_DCL const char *rank();

#ifdef OVL1

/* convert experience level (1..30) to rank index (0..8) */
int xlev_to_rank(int xlev) {
  return (xlev <= 2) ? 0 : (xlev <= 30) ? ((xlev + 2) / 4) : 8;
}

#if 0 /* not currently needed */
/* convert rank index (0..8) to experience level (1..30) */
int rank_to_xlev(int rank) {
	return (rank <= 0) ? 1 : (rank <= 8) ? ((rank * 4) - 2) : 30;
}
#endif

const char *rank_of(int lev, short monnum, bool female) {
  struct Role *role;
  int i;

  /* Find the role */
  for (role = (struct Role *)roles; role->name.m; role++)
    if (monnum == role->malenum || monnum == role->femalenum)
      break;
  if (!role->name.m)
    role = &urole;

  /* Find the rank */
  for (i = xlev_to_rank((int)lev); i >= 0; i--) {
    if (female && role->rank[i].f)
      return (role->rank[i].f);
    if (role->rank[i].m)
      return (role->rank[i].m);
  }

  /* Try the role name, instead */
  if (female && role->name.f)
    return (role->name.f);
  else if (role->name.m)
    return (role->name.m);
  return ("Player");
}

STATIC_OVL const char *rank() {
  return (rank_of(player.ulevel, Role_switch, flags.female));
}

int title_to_mon(const char *str, int *rank_indx, int *title_length) {
  int i, j;

  /* Loop through each of the roles */
  for (i = 0; roles[i].name.m; i++)
    for (j = 0; j < 9; j++) {
      if (roles[i].rank[j].m &&
          !strncmpi(str, roles[i].rank[j].m, strlen(roles[i].rank[j].m))) {
        if (rank_indx)
          *rank_indx = j;
        if (title_length)
          *title_length = strlen(roles[i].rank[j].m);
        return roles[i].malenum;
      }
      if (roles[i].rank[j].f &&
          !strncmpi(str, roles[i].rank[j].f, strlen(roles[i].rank[j].f))) {
        if (rank_indx)
          *rank_indx = j;
        if (title_length)
          *title_length = strlen(roles[i].rank[j].f);
        return ((roles[i].femalenum != NON_PM) ? roles[i].femalenum
                                               : roles[i].malenum);
      }
    }
  return NON_PM;
}

#endif /* OVL1 */
#ifdef OVLB

void max_rank_sz() {
  int i, r, maxr = 0;
  for (i = 0; i < 9; i++) {
    if (urole.rank[i].m && (r = strlen(urole.rank[i].m)) > maxr)
      maxr = r;
    if (urole.rank[i].f && (r = strlen(urole.rank[i].f)) > maxr)
      maxr = r;
  }
  mrank_sz = maxr;
  return;
}

#endif /* OVLB */

#ifdef SCORE_ON_BOTL
long botl_score() {
  int deepest = deepest_lev_reached(FALSE);
#ifndef GOLDOBJ
  long ugold = player.ugold + hidden_gold();

  if ((ugold -= player.ugold0) < 0L)
    ugold = 0L;
  return ugold + player.urexp + (long)(50 * (deepest - 1))
#else
  long umoney = money_cnt(invent) + hidden_gold();

  if ((umoney -= player.umoney0) < 0L)
    umoney = 0L;
  return umoney + player.urexp + (long)(50 * (deepest - 1))
#endif
         + (long)(deepest > 30 ? 10000 : deepest > 20 ? 1000 * (deepest - 20)
                                                      : 0);
}
#endif

#ifdef DUMP_LOG
void bot1str(char *newbot1)
#else
STATIC_OVL void bot1()
#endif
{
#ifndef DUMP_LOG
  char newbot1[MAXCO];
#endif
  char *nb;
  int i, j;

  strcpy(newbot1, plname);
  if ('a' <= newbot1[0] && newbot1[0] <= 'z')
    newbot1[0] += 'A' - 'a';
  newbot1[10] = 0;
  sprintf(nb = eos(newbot1), " the ");

  if (Upolyd) {
    char mbot[BUFSZ];
    int k = 0;

    strcpy(mbot, mons[player.umonnum].mname);
    while (mbot[k] != 0) {
      if ((k == 0 || (k > 0 && mbot[k - 1] == ' ')) && 'a' <= mbot[k] &&
          mbot[k] <= 'z')
        mbot[k] += 'A' - 'a';
      k++;
    }
    sprintf(nb = eos(nb), mbot);
  } else
    sprintf(nb = eos(nb), rank());

  sprintf(nb = eos(nb), "  ");
  i = mrank_sz + 15;
  j = (nb + 2) - newbot1; /* aka strlen(newbot1) but less computation */
  if ((i - j) > 0)
    sprintf(nb = eos(nb), "%*s", i - j, " "); /* pad with spaces */
  if (ACURR(A_STR) > 18) {
    if (ACURR(A_STR) > STR18(100))
      sprintf(nb = eos(nb), "St:%2d ", ACURR(A_STR) - 100);
    else if (ACURR(A_STR) < STR18(100))
      sprintf(nb = eos(nb), "St:18/%02d ", ACURR(A_STR) - 18);
    else
      sprintf(nb = eos(nb), "St:18/** ");
  } else
    sprintf(nb = eos(nb), "St:%-1d ", ACURR(A_STR));
  sprintf(nb = eos(nb), "Dx:%-1d Co:%-1d In:%-1d Wi:%-1d Ch:%-1d", ACURR(A_DEX),
          ACURR(A_CON), ACURR(A_INT), ACURR(A_WIS), ACURR(A_CHA));
  sprintf(nb = eos(nb),
          (player.ualign.type == A_CHAOTIC)
              ? "  Chaotic"
              : (player.ualign.type == A_NEUTRAL) ? "  Neutral" : "  Lawful");
#ifdef SCORE_ON_BOTL
  if (flags.showscore)
    sprintf(nb = eos(nb), " S:%ld", botl_score());
#endif
#ifdef DUMP_LOG
}
STATIC_OVL void bot1() {
  char newbot1[MAXCO];

  bot1str(newbot1);
#endif
  curs(WIN_STATUS, 1, 0);
  putstr(WIN_STATUS, 0, newbot1);
}

/* provide the name of the current level for display by various ports */
int describe_level(char *buf) {
  int ret = 1;

  /* TODO:	Add in dungeon name */
  if (Is_knox(&player.uz))
    sprintf(buf, "%s ", dungeons[player.uz.dnum].dname);
  else if (In_quest(&player.uz))
    sprintf(buf, "Home %d ", dunlev(&player.uz));
  else if (In_endgame(&player.uz))
    sprintf(buf, Is_astralevel(&player.uz) ? "Astral Plane " : "End Game ");
  else {
    /* ports with more room may expand this one */
    sprintf(buf, "Dlvl:%-2d ", depth(&player.uz));
    ret = 0;
  }
  return ret;
}

void bot2str(char *newbot2) {
  char *nb;
  int hp, hpmax;
#ifdef HPMON
  int hpcolor, hpattr;
#endif
  int cap = near_capacity();

  hp = Upolyd ? player.mh : player.uhp;
  hpmax = Upolyd ? player.mhmax : player.uhpmax;

  if (hp < 0)
    hp = 0;
  (void)describe_level(newbot2);
  sprintf(nb = eos(newbot2),
#ifdef HPMON
          "%c:%-2ld HP:", oc_syms[COIN_CLASS],
#ifndef GOLDOBJ
          player.ugold
#else
          money_cnt(invent)
#endif
          );
#else /* HPMON */
          "%c:%-2ld HP:%d(%d) Pw:%d(%d) AC:%-2d", oc_syms[COIN_CLASS],
#ifndef GOLDOBJ
          player.ugold,
#else
          money_cnt(invent),
#endif
          hp, hpmax, player.uen, player.uenmax, player.uac);
#endif /* HPMON */
#ifdef HPMON
  curs(WIN_STATUS, 1, 1);
  putstr(WIN_STATUS, 0, newbot2);

  sprintf(nb = eos(newbot2), "%d(%d)", hp, hpmax);
#ifdef TEXTCOLOR
  if (iflags.use_color) {
    curs(WIN_STATUS, 1, 1);
    hpattr = ATR_NONE;
    if (hp == hpmax) {
      hpcolor = NO_COLOR;
    } else if (hp > (hpmax * 2 / 3)) {
      hpcolor = CLR_GREEN;
    } else if (hp <= (hpmax / 3)) {
      hpcolor = CLR_RED;
      if (hp <= (hpmax / 10))
        hpattr = ATR_BLINK;
    } else {
      hpcolor = CLR_YELLOW;
    }
    if (hpcolor != NO_COLOR)
      term_start_color(hpcolor);
    if (hpattr != ATR_NONE)
      term_start_attr(hpattr);
    putstr(WIN_STATUS, hpattr, newbot2);
    if (hpattr != ATR_NONE)
      term_end_attr(hpattr);
    if (hpcolor != NO_COLOR)
      term_end_color();
  }
#endif /* TEXTCOLOR */
  sprintf(nb = eos(newbot2), " Pw:%d(%d) AC:%-2d", player.uen, player.uenmax,
          player.uac);
#endif /* HPMON */

  if (Upolyd)
    sprintf(nb = eos(nb), " HD:%d", mons[player.umonnum].mlevel);
#ifdef EXP_ON_BOTL
  else if (flags.showexp)
    sprintf(nb = eos(nb), " Xp:%u/%-1ld", player.ulevel, player.uexp);
#endif
  else
    sprintf(nb = eos(nb), " Exp:%u", player.ulevel);

  if (flags.time)
    sprintf(nb = eos(nb), " T:%ld", moves);

#ifdef REALTIME_ON_BOTL
  if (iflags.showrealtime) {
    time_t currenttime = get_realtime();
    sprintf(nb = eos(nb), " %d:%2.2d", currenttime / 3600,
            (currenttime % 3600) / 60);
  }
#endif

  if (strcmp(hu_stat[player.uhs], "        ")) {
    sprintf(nb = eos(nb), " ");
    strcat(newbot2, hu_stat[player.uhs]);
  }
  if (Confusion)
    sprintf(nb = eos(nb), " Conf");
  if (Sick) {
    if (player.usick_type & SICK_VOMITABLE)
      sprintf(nb = eos(nb), " FoodPois");
    if (player.usick_type & SICK_NONVOMITABLE)
      sprintf(nb = eos(nb), " Ill");
  }
  if (Blind)
    sprintf(nb = eos(nb), " Blind");
  if (Stunned)
    sprintf(nb = eos(nb), " Stun");
  if (Hallucination)
    sprintf(nb = eos(nb), " Hallu");
  if (Slimed)
    sprintf(nb = eos(nb), " Slime");
  if (cap > UNENCUMBERED)
    sprintf(nb = eos(nb), " %s", enc_stat[cap]);
}
STATIC_OVL void bot2() {
  char newbot2[MAXCO];
  bot2str(newbot2);
  curs(WIN_STATUS, 1, 1);
  putstr(WIN_STATUS, 0, newbot2);
}

void bot() {
  bot1();
  bot2();
  flags.botl = flags.botlx = 0;
}


/*botl.c*/
