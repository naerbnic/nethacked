/*	SCCS Id: @(#)polyself.c	3.4	2003/01/08	*/
/*	Copyright (C) 1987, 1988, 1989 by Ken Arromdee */
/* NetHack may be freely redistributed.  See license for details. */

/*
 * Polymorph self routine.
 *
 * Note:  the light source handling code assumes that both youmonst.m_id
 * and youmonst.mx will always remain 0 when it handles the case of the
 * player polymorphed into a light-emitting monster.
 */

#include <string.h>

#include "hack.h"
#include "were.h"
#include "steed.h"
#include "shk.h"
#include "rnd.h"
#include "read.h"
#include "potion.h"
#include "polyself.h"
#include "pline.h"
#include "pickup.h"
#include "objnam.h"
#include "mon.h"
#include "mkobj.h"
#include "mhitu.h"
#include "makemon.h"
#include "light.h"
#include "invent.h"
#include "hacklib.h"
#include "exper.h"
#include "end.h"
#include "eat.h"
#include "dothrow.h"
#include "do_wear.h"
#include "do_name.h"
#include "do.h"
#include "dig.h"
#include "dbridge.h"
#include "cmd.h"

#include "artifact.h"
#include "botl.h"

#ifdef OVLB
STATIC_DCL void polyman(const char *, const char *);
STATIC_DCL void break_armor();
STATIC_DCL void drop_weapon(int);
STATIC_DCL void uunstick();
STATIC_DCL int armor_to_dragon(int);
STATIC_DCL void newman();

/* update the youmonst.data structure pointer */
void set_uasmon() { set_mon_data(&youmonst, &mons[player.umonnum], 0); }

/* make a (new) human out of the player */
STATIC_OVL void polyman(const char *fmt, const char *arg) {
  bool sticky = sticks(youmonst.data) && player.ustuck && !player.uswallow,
       was_mimicking = (youmonst.m_ap_type == M_AP_OBJECT);
  bool could_pass_walls = Passes_walls;
  bool was_blind = !!Blind;

  if (Upolyd) {
    player.acurr = player.macurr; /* restore old attribs */
    player.amax = player.mamax;
    player.umonnum = player.umonster;
    flags.female = player.mfemale;
  }
  set_uasmon();

  player.mh = player.mhmax = 0;
  player.mtimedone = 0;
  skinback(FALSE);
  player.uundetected = 0;

  if (sticky)
    uunstick();
  find_ac();
  if (was_mimicking) {
    if (multi < 0)
      unmul("");
    youmonst.m_ap_type = M_AP_NOTHING;
  }

  newsym(player.ux, player.uy);

  You(fmt, arg);
  /* check whether player foolishly genocided self while poly'd */
  if ((mvitals[urole.malenum].mvflags & G_GENOD) ||
      (urole.femalenum != NON_PM &&
       (mvitals[urole.femalenum].mvflags & G_GENOD)) ||
      (mvitals[urace.malenum].mvflags & G_GENOD) ||
      (urace.femalenum != NON_PM &&
       (mvitals[urace.femalenum].mvflags & G_GENOD))) {
    /* intervening activity might have clobbered genocide info */
    killer = delayed_killer;
    if (!killer || !strstri(killer, "genocid")) {
      killer_format = KILLED_BY;
      killer = "self-genocide";
    }
    done(GENOCIDED);
  }

  if (player.twoweap && !could_twoweap(youmonst.data))
    untwoweapon();

  if (player.utraptype == TT_PIT) {
    if (could_pass_walls) { /* player forms cannot pass walls */
      player.utrap = rn1(6, 2);
    }
  }
  if (was_blind && !Blind) { /* reverting from eyeless */
    Blinded = 1L;
    make_blinded(0L, TRUE); /* remove blindness */
  }

  if (!Levitation && !player.ustuck &&
      (is_pool(player.ux, player.uy) || is_lava(player.ux, player.uy)))
    spoteffects(TRUE);

  see_monsters();
}

void change_sex() {
  /* setting player.umonster for caveman/cavewoman or priest/priestess
     swap unintentionally makes `Upolyd' appear to be true */
  bool already_polyd = (bool)Upolyd;

  /* Some monsters are always of one sex and their sex can't be changed */
  /* succubi/incubi can change, but are handled below */
  /* !already_polyd check necessary because is_male() and is_female()
     are true if the player is a priest/priestess */
  if (!already_polyd || (!is_male(youmonst.data) && !is_female(youmonst.data) &&
                         !is_neuter(youmonst.data)))
    flags.female = !flags.female;
  if (already_polyd) /* poly'd: also change saved sex */
    player.mfemale = !player.mfemale;
  max_rank_sz(); /* [this appears to be superfluous] */
  if ((already_polyd ? player.mfemale : flags.female) && urole.name.f)
    strcpy(pl_character, urole.name.f);
  else
    strcpy(pl_character, urole.name.m);
  player.umonster = ((already_polyd ? player.mfemale : flags.female) &&
                     urole.femalenum != NON_PM)
                        ? urole.femalenum
                        : urole.malenum;
  if (!already_polyd) {
    player.umonnum = player.umonster;
  } else if (player.umonnum == PM_SUCCUBUS || player.umonnum == PM_INCUBUS) {
    flags.female = !flags.female;
    /* change monster type to match new sex */
    player.umonnum = (player.umonnum == PM_SUCCUBUS) ? PM_INCUBUS : PM_SUCCUBUS;
    set_uasmon();
  }
}

STATIC_OVL void newman() {
  int tmp, oldlvl;

  tmp = player.uhpmax;
  oldlvl = player.ulevel;
  player.ulevel = player.ulevel + rn1(5, -2);
  if (player.ulevel > 127 || player.ulevel < 1) { /* level went below 0? */
    player.ulevel = oldlvl; /* restore old level in case they lifesave */
    goto dead;
  }
  if (player.ulevel > MAXULEV)
    player.ulevel = MAXULEV;
  /* If your level goes down, your peak level goes down by
     the same amount so that you can't simply use blessed
     full healing to undo the decrease.  But if your level
     goes up, your peak level does *not* undergo the same
     adjustment; you might end up losing out on the chance
     to regain some levels previously lost to other causes. */
  if (player.ulevel < oldlvl)
    player.ulevelmax -= (oldlvl - player.ulevel);
  if (player.ulevelmax < player.ulevel)
    player.ulevelmax = player.ulevel;

  if (!rn2(10))
    change_sex();

  adjabil(oldlvl, (int)player.ulevel);
  reset_rndmonst(NON_PM); /* new monster generation criteria */

  /* random experience points for the new experience level */
  player.uexp = rndexp(FALSE);

/* player.uhpmax * player.ulevel / oldlvl: proportionate hit points to new level
 * -10 and +10: don't apply proportionate HP to 10 of a starting
 *   character's hit points (since a starting character's hit points
 *   are not on the same scale with hit points obtained through level
 *   gain)
 * 9 - rn2(19): random change of -9 to +9 hit points
 */
#ifndef LINT
  player.uhpmax = ((player.uhpmax - 10) * (long)player.ulevel / oldlvl + 10) +
                  (9 - rn2(19));
#endif

#ifdef LINT
  player.uhp = player.uhp + tmp;
#else
  player.uhp = player.uhp * (long)player.uhpmax / tmp;
#endif

  tmp = player.uenmax;
#ifndef LINT
  player.uenmax = player.uenmax * (long)player.ulevel / oldlvl + 9 - rn2(19);
#endif
  if (player.uenmax < 0)
    player.uenmax = 0;
#ifndef LINT
  player.uen = (tmp ? player.uen * (long)player.uenmax / tmp : player.uenmax);
#endif

  redist_attr();
  player.uhunger = rn1(500, 500);
  if (Sick)
    make_sick(0L, nullptr, FALSE, SICK_ALL);
  Stoned = 0;
  delayed_killer = 0;
  if (player.uhp <= 0 || player.uhpmax <= 0) {
    if (Polymorph_control) {
      if (player.uhp <= 0)
        player.uhp = 1;
      if (player.uhpmax <= 0)
        player.uhpmax = 1;
    } else {
    dead: /* we come directly here if their experience level went to 0 or less
             */
      Your("new form doesn't seem healthy enough to survive.");
      killer_format = KILLED_BY_AN;
      killer = "unsuccessful polymorph";
      done(DIED);
      newuhs(FALSE);
      return; /* lifesaved */
    }
  }
  newuhs(FALSE);
  polyman("feel like a new %s!",
          (flags.female && urace.individual.f)
              ? urace.individual.f
              : (urace.individual.m) ? urace.individual.m : urace.noun);
  if (Slimed) {
    Your("body transforms, but there is still slime on you.");
    Slimed = 10L;
  }
  flags.botl = 1;
  see_monsters();
  (void)encumber_msg();
}

void polyself(bool forcecontrol) {
  char buf[BUFSZ];
  int old_light, new_light;
  int mntmp = NON_PM;
  int tries = 0;
  bool draconian = (uarm && uarm->otyp >= GRAY_DRAGON_SCALE_MAIL &&
                    uarm->otyp <= YELLOW_DRAGON_SCALES);
  bool iswere = (player.ulycn >= LOW_PM || is_were(youmonst.data));
  bool isvamp =
      (youmonst.data->mlet == S_VAMPIRE || player.umonnum == PM_VAMPIRE_BAT);
  bool was_floating = (Levitation || Flying);

  if (!Polymorph_control && !forcecontrol && !draconian && !iswere && !isvamp) {
    if (rn2(20) > ACURR(A_CON)) {
      You(shudder_for_moment);
      losehp(rnd(30), "system shock", KILLED_BY_AN);
      exercise(A_CON, FALSE);
      return;
    }
  }
  old_light = Upolyd ? emits_light(youmonst.data) : 0;

  if (Polymorph_control || forcecontrol) {
    do {
      getlin("Become what kind of monster? [type the name]", buf);
      mntmp = name_to_mon(buf);
      if (mntmp < LOW_PM)
        pline("I've never heard of such monsters.");
      /* Note:  humans are illegal as monsters, but an
       * illegal monster forces newman(), which is what we
       * want if they specified a human.... */
      else if (!polyok(&mons[mntmp]) && !your_race(&mons[mntmp]))
        You("cannot polymorph into that.");
      else
        break;
    } while (++tries < 5);
    if (tries == 5)
      pline(thats_enough_tries);
    /* allow skin merging, even when polymorph is controlled */
    if (draconian && (mntmp == armor_to_dragon(uarm->otyp) || tries == 5))
      goto do_merge;
  } else if (draconian || iswere || isvamp) {
    /* special changes that don't require polyok() */
    if (draconian) {
    do_merge:
      mntmp = armor_to_dragon(uarm->otyp);
      if (!(mvitals[mntmp].mvflags & G_GENOD)) {
        /* allow G_EXTINCT */
        You("merge with your scaly armor.");
        uskin = uarm;
        uarm = nullptr;
        /* save/restore hack */
        uskin->owornmask |= I_SPECIAL;
      }
    } else if (iswere) {
      if (is_were(youmonst.data))
        mntmp = PM_HUMAN; /* Illegal; force newman() */
      else
        mntmp = player.ulycn;
    } else {
      if (youmonst.data->mlet == S_VAMPIRE)
        mntmp = PM_VAMPIRE_BAT;
      else
        mntmp = PM_VAMPIRE;
    }
    /* if polymon fails, "you feel" message has been given
       so don't follow up with another polymon or newman */
    if (mntmp == PM_HUMAN)
      newman(); /* werecritter */
    else
      (void)polymon(mntmp);
    goto made_change; /* maybe not, but this is right anyway */
  }

  if (mntmp < LOW_PM) {
    tries = 0;
    do {
      /* randomly pick an "ordinary" monster */
      mntmp = rn1(SPECIAL_PM - LOW_PM, LOW_PM);
    } while ((!polyok(&mons[mntmp]) || is_placeholder(&mons[mntmp])) &&
             tries++ < 200);
  }

  /* The below polyok() fails either if everything is genocided, or if
   * we deliberately chose something illegal to force newman().
   */
  if (!polyok(&mons[mntmp]) || !rn2(5) || your_race(&mons[mntmp]))
    newman();
  else if (!polymon(mntmp))
    return;

  if (!uarmg)
    selftouch("No longer petrify-resistant, you");

made_change:
  new_light = Upolyd ? emits_light(youmonst.data) : 0;
  if (old_light != new_light) {
    if (old_light)
      del_light_source(LS_MONSTER, (genericptr_t) & youmonst);
    if (new_light == 1)
      ++new_light; /* otherwise it's undetectable */
    if (new_light)
      new_light_source(player.ux, player.uy, new_light, LS_MONSTER,
                       (genericptr_t) & youmonst);
  }
  if (is_pool(player.ux, player.uy) && was_floating &&
      !(Levitation || Flying) && !breathless(youmonst.data) &&
      !amphibious(youmonst.data) && !Swimming)
    drown();
}

/* (try to) make a mntmp monster out of the player */
/* returns 1 if polymorph successful */
int polymon(int mntmp) {
  bool sticky = sticks(youmonst.data) && player.ustuck && !player.uswallow,
       was_blind = !!Blind, dochange = FALSE;
  bool could_pass_walls = Passes_walls;
  int mlvl;

  if (mvitals[mntmp].mvflags & G_GENOD) { /* allow G_EXTINCT */
    You_feel("rather %s-ish.", mons[mntmp].mname);
    exercise(A_WIS, TRUE);
    return (0);
  }

  /* KMH, conduct */
  player.uconduct.polyselfs++;

  if (!Upolyd) {
    /* Human to monster; save human stats */
    player.macurr = player.acurr;
    player.mamax = player.amax;
    player.mfemale = flags.female;
  } else {
    /* Monster to monster; restore human stats, to be
     * immediately changed to provide stats for the new monster
     */
    player.acurr = player.macurr;
    player.amax = player.mamax;
    flags.female = player.mfemale;
  }

  if (youmonst.m_ap_type) {
    /* stop mimicking immediately */
    if (multi < 0)
      unmul("");
  } else if (mons[mntmp].mlet != S_MIMIC) {
    /* as in polyman() */
    youmonst.m_ap_type = M_AP_NOTHING;
  }
  if (is_male(&mons[mntmp])) {
    if (flags.female)
      dochange = TRUE;
  } else if (is_female(&mons[mntmp])) {
    if (!flags.female)
      dochange = TRUE;
  } else if (!is_neuter(&mons[mntmp]) && mntmp != player.ulycn) {
    if (!rn2(10))
      dochange = TRUE;
  }
  if (dochange) {
    flags.female = !flags.female;
    You("%s %s%s!",
        (player.umonnum != mntmp) ? "turn into a" : "feel like a new",
        (is_male(&mons[mntmp]) || is_female(&mons[mntmp]))
            ? ""
            : flags.female ? "female " : "male ",
        mons[mntmp].mname);
  } else {
    if (player.umonnum != mntmp)
      You("turn into %s!", an(mons[mntmp].mname));
    else
      You_feel("like a new %s!", mons[mntmp].mname);
  }
  if (Stoned && poly_when_stoned(&mons[mntmp])) {
    /* poly_when_stoned already checked stone golem genocide */
    You("turn to stone!");
    mntmp = PM_STONE_GOLEM;
    Stoned = 0;
    delayed_killer = 0;
  }

  player.mtimedone = rn1(500, 500);
  player.umonnum = mntmp;
  set_uasmon();

  /* New stats for monster, to last only as long as polymorphed.
   * Currently only strength gets changed.
   */
  if (strongmonst(&mons[mntmp]))
    ABASE(A_STR) = AMAX(A_STR) = STR18(100);

  if (Stone_resistance && Stoned) { /* parnes@eniac.seas.upenn.edu */
    Stoned = 0;
    delayed_killer = 0;
    You("no longer seem to be petrifying.");
  }
  if (Sick_resistance && Sick) {
    make_sick(0L, nullptr, FALSE, SICK_ALL);
    You("no longer feel sick.");
  }
  if (Slimed) {
    if (flaming(youmonst.data)) {
      pline_The("slime burns away!");
      Slimed = 0L;
      flags.botl = 1;
    } else if (mntmp == PM_GREEN_SLIME) {
      /* do it silently */
      Slimed = 0L;
      flags.botl = 1;
    }
  }
  if (nohands(youmonst.data))
    Glib = 0;

  /*
  mlvl = adj_lev(&mons[mntmp]);
   * We can't do the above, since there's no such thing as an
   * "experience level of you as a monster" for a polymorphed character.
   */
  mlvl = (int)mons[mntmp].mlevel;
  if (youmonst.data->mlet == S_DRAGON && mntmp >= PM_GRAY_DRAGON) {
    player.mhmax =
        In_endgame(&player.uz) ? (8 * mlvl) : (4 * mlvl + d(mlvl, 4));
  } else if (is_golem(youmonst.data)) {
    player.mhmax = golemhp(mntmp);
  } else {
    if (!mlvl)
      player.mhmax = rnd(4);
    else
      player.mhmax = d(mlvl, 8);
    if (IsLevelHomeOfElementalType(&mons[mntmp]))
      player.mhmax *= 3;
  }
  player.mh = player.mhmax;

  if (player.ulevel < mlvl) {
/* Low level characters can't become high level monsters for long */
#ifdef DUMB
    /* DRS/NS 2.2.6 messes up -- Peter Kendell */
    int mtd = player.mtimedone, ulv = player.ulevel;

    player.mtimedone = mtd * ulv / mlvl;
#else
    player.mtimedone = player.mtimedone * player.ulevel / mlvl;
#endif
  }

  if (uskin && mntmp != armor_to_dragon(uskin->otyp))
    skinback(FALSE);
  break_armor();
  drop_weapon(1);
  if (hides_under(youmonst.data))
    player.uundetected = OBJ_AT(player.ux, player.uy);
  else if (youmonst.data->mlet == S_EEL)
    player.uundetected = is_pool(player.ux, player.uy);
  else
    player.uundetected = 0;

  if (player.utraptype == TT_PIT) {
    if (could_pass_walls && !Passes_walls) {
      player.utrap = rn1(6, 2);
    } else if (!could_pass_walls && Passes_walls) {
      player.utrap = 0;
    }
  }
  if (was_blind && !Blind) { /* previous form was eyeless */
    Blinded = 1L;
    make_blinded(0L, TRUE); /* remove blindness */
  }
  newsym(player.ux, player.uy); /* Change symbol */

  if (!sticky && !player.uswallow && player.ustuck && sticks(youmonst.data))
    player.ustuck = 0;
  else if (sticky && !sticks(youmonst.data))
    uunstick();
#ifdef STEED
  if (player.usteed) {
    if (touch_petrifies(player.usteed->data) && !Stone_resistance && rnl(3)) {
      char buf[BUFSZ];

      pline("No longer petrifying-resistant, you touch %s.",
            mon_nam(player.usteed));
      sprintf(buf, "riding %s", an(player.usteed->data->mname));
      instapetrify(buf);
    }
    if (!can_ride(player.usteed))
      dismount_steed(DISMOUNT_POLY);
  }
#endif

  if (flags.verbose) {
    static const char use_thec[] = "Use the command #%s to %s.";
    static const char monsterc[] = "monster";
    if (can_breathe(youmonst.data))
      pline(use_thec, monsterc, "use your breath weapon");
    if (attacktype(youmonst.data, AT_SPIT))
      pline(use_thec, monsterc, "spit venom");
    if (youmonst.data->mlet == S_NYMPH)
      pline(use_thec, monsterc, "remove an iron ball");
    if (attacktype(youmonst.data, AT_GAZE))
      pline(use_thec, monsterc, "gaze at monsters");
    if (is_hider(youmonst.data))
      pline(use_thec, monsterc, "hide");
    if (is_were(youmonst.data))
      pline(use_thec, monsterc, "summon help");
    if (webmaker(youmonst.data))
      pline(use_thec, monsterc, "spin a web");
    if (player.umonnum == PM_GREMLIN)
      pline(use_thec, monsterc, "multiply in a fountain");
    if (is_unicorn(youmonst.data))
      pline(use_thec, monsterc, "use your horn");
    if (is_mind_flayer(youmonst.data))
      pline(use_thec, monsterc, "emit a mental blast");
    if (youmonst.data->msound == MS_SHRIEK) /* worthless, actually */
      pline(use_thec, monsterc, "shriek");
    if (lays_eggs(youmonst.data) && flags.female)
      pline(use_thec, "sit", "lay an egg");
  }
  /* you now know what an egg of your type looks like */
  if (lays_eggs(youmonst.data)) {
    learn_egg_type(player.umonnum);
    /* make queen bees recognize killer bee eggs */
    learn_egg_type(egg_type_from_parent(player.umonnum, TRUE));
  }
  find_ac();
  if ((!Levitation && !player.ustuck && !Flying &&
       (is_pool(player.ux, player.uy) || is_lava(player.ux, player.uy))) ||
      (Underwater && !Swimming))
    spoteffects(TRUE);
  if (Passes_walls && player.utrap && player.utraptype == TT_INFLOOR) {
    player.utrap = 0;
    pline_The("rock seems to no longer trap you.");
  } else if (likes_lava(youmonst.data) && player.utrap &&
             player.utraptype == TT_LAVA) {
    player.utrap = 0;
    pline_The("lava now feels soothing.");
  }
  if (amorphous(youmonst.data) || is_whirly(youmonst.data) ||
      unsolid(youmonst.data)) {
    if (Punished) {
      You("slip out of the iron chain.");
      unpunish();
    }
  }
  if (player.utrap &&
      (player.utraptype == TT_WEB || player.utraptype == TT_BEARTRAP) &&
      (amorphous(youmonst.data) || is_whirly(youmonst.data) ||
       unsolid(youmonst.data) ||
       (youmonst.data->msize <= MZ_SMALL && player.utraptype == TT_BEARTRAP))) {
    You("are no longer stuck in the %s.",
        player.utraptype == TT_WEB ? "web" : "bear trap");
    /* probably should burn webs too if PM_FIRE_ELEMENTAL */
    player.utrap = 0;
  }
  if (webmaker(youmonst.data) && player.utrap && player.utraptype == TT_WEB) {
    You("orient yourself on the web.");
    player.utrap = 0;
  }
  flags.botl = 1;
  vision_full_recalc = 1;
  see_monsters();
  exercise(A_CON, FALSE);
  exercise(A_WIS, TRUE);
  (void)encumber_msg();
  return (1);
}

STATIC_OVL void break_armor() {
  Object *otmp;

  if (breakarm(youmonst.data)) {
    if ((otmp = uarm) != 0) {
      if (donning(otmp))
        cancel_don();
      You("break out of your armor!");
      exercise(A_STR, FALSE);
      (void)Armor_gone();
      useup(otmp);
    }
    if ((otmp = uarmc) != 0) {
      if (otmp->oartifact) {
        Your("%s falls off!", cloak_simple_name(otmp));
        (void)Cloak_off();
        dropx(otmp);
      } else {
        Your("%s tears apart!", cloak_simple_name(otmp));
        (void)Cloak_off();
        useup(otmp);
      }
    }
#ifdef TOURIST
    if (uarmu) {
      Your("shirt rips to shreds!");
      useup(uarmu);
    }
#endif
  } else if (sliparm(youmonst.data)) {
    if (((otmp = uarm) != 0) && (racial_exception(&youmonst, otmp) < 1)) {
      if (donning(otmp))
        cancel_don();
      Your("armor falls around you!");
      (void)Armor_gone();
      dropx(otmp);
    }
    if ((otmp = uarmc) != 0) {
      if (is_whirly(youmonst.data))
        Your("%s falls, unsupported!", cloak_simple_name(otmp));
      else
        You("shrink out of your %s!", cloak_simple_name(otmp));
      (void)Cloak_off();
      dropx(otmp);
    }
#ifdef TOURIST
    if ((otmp = uarmu) != 0) {
      if (is_whirly(youmonst.data))
        You("seep right through your shirt!");
      else
        You("become much too small for your shirt!");
      setworn(nullptr, otmp->owornmask & W_ARMU);
      dropx(otmp);
    }
#endif
  }
  if (has_horns(youmonst.data)) {
    if ((otmp = uarmh) != 0) {
      if (is_flimsy(otmp) && !donning(otmp)) {
        char hornbuf[BUFSZ], yourbuf[BUFSZ];

        /* Future possiblities: This could damage/destroy helmet */
        sprintf(hornbuf, "horn%s", plur(num_horns(youmonst.data)));
        Your("%s %s through %s %s.", hornbuf, vtense(hornbuf, "pierce"),
             shk_your(yourbuf, otmp), xname(otmp));
      } else {
        if (donning(otmp))
          cancel_don();
        Your("helmet falls to the %s!", surface(player.ux, player.uy));
        (void)Helmet_off();
        dropx(otmp);
      }
    }
  }
  if (nohands(youmonst.data) || verysmall(youmonst.data)) {
    if ((otmp = uarmg) != 0) {
      if (donning(otmp))
        cancel_don();
      /* Drop weapon along with gloves */
      You("drop your gloves%s!", uwep ? " and weapon" : "");
      drop_weapon(0);
      (void)Gloves_off();
      dropx(otmp);
    }
    if ((otmp = uarms) != 0) {
      You("can no longer hold your shield!");
      (void)Shield_off();
      dropx(otmp);
    }
    if ((otmp = uarmh) != 0) {
      if (donning(otmp))
        cancel_don();
      Your("helmet falls to the %s!", surface(player.ux, player.uy));
      (void)Helmet_off();
      dropx(otmp);
    }
  }
  if (nohands(youmonst.data) || verysmall(youmonst.data) ||
      slithy(youmonst.data) || youmonst.data->mlet == S_CENTAUR) {
    if ((otmp = uarmf) != 0) {
      if (donning(otmp))
        cancel_don();
      if (is_whirly(youmonst.data))
        Your("boots fall away!");
      else
        Your("boots %s off your feet!",
             verysmall(youmonst.data) ? "slide" : "are pushed");
      (void)Boots_off();
      dropx(otmp);
    }
  }
}

STATIC_OVL void drop_weapon(int alone) {
  Object *otmp;
  Object *otmp2;

  if ((otmp = uwep) != 0) {
    /* !alone check below is currently superfluous but in the
     * future it might not be so if there are monsters which cannot
     * wear gloves but can wield weapons
     */
    if (!alone || cantwield(youmonst.data)) {
      Object *wep = uwep;

      if (alone)
        You("find you must drop your weapon%s!", player.twoweap ? "s" : "");
      otmp2 = player.twoweap ? uswapwep : 0;
      uwepgone();
      if (!wep->cursed || wep->otyp != LOADSTONE)
        dropx(otmp);
      if (otmp2 != 0) {
        uswapwepgone();
        if (!otmp2->cursed || otmp2->otyp != LOADSTONE)
          dropx(otmp2);
      }
      untwoweapon();
    } else if (!could_twoweap(youmonst.data)) {
      untwoweapon();
    }
  }
}

void rehumanize() {
  /* You can't revert back while unchanging */
  if (Unchanging && (player.mh < 1)) {
    killer_format = NO_KILLER_PREFIX;
    killer = "killed while stuck in creature form";
    done(DIED);
  }

  if (emits_light(youmonst.data))
    del_light_source(LS_MONSTER, (genericptr_t) & youmonst);
  polyman("return to %s form!", urace.adj);

  if (player.uhp < 1) {
    char kbuf[256];

    sprintf(kbuf, "reverting to unhealthy %s form", urace.adj);
    killer_format = KILLED_BY;
    killer = kbuf;
    done(DIED);
  }
  if (!uarmg)
    selftouch("No longer petrify-resistant, you");
  nomul(0, 0);

  flags.botl = 1;
  vision_full_recalc = 1;
  (void)encumber_msg();
}

int dobreathe() {
  struct Attack *mattk;

  if (Strangled) {
    You_cant("breathe.  Sorry.");
    return (0);
  }
  if (player.uen < 15) {
    You("don't have enough energy to breathe!");
    return (0);
  }
  player.uen -= 15;
  flags.botl = 1;

  if (!getdir(nullptr))
    return (0);

  mattk = attacktype_fordmg(youmonst.data, AT_BREA, AD_ANY);
  if (!mattk)
    impossible("bad breath attack?"); /* mouthwash needed... */
  else
    buzz((int)(20 + mattk->adtyp - 1), (int)mattk->damn, player.ux, player.uy,
         player.dx, player.dy);
  return (1);
}

int dospit() {
  Object *otmp;

  if (!getdir(nullptr))
    return (0);
  otmp = MakeSpecificObject(
      player.umonnum == PM_COBRA ? BLINDING_VENOM : ACID_VENOM, TRUE, FALSE);
  otmp->spe = 1; /* to indicate it's yours */
  throwit(otmp, 0L, FALSE);
  return (1);
}

int doremove() {
  if (!Punished) {
    You("are not chained to anything!");
    return (0);
  }
  unpunish();
  return (1);
}

int dospinweb() {
  Trap *ttmp = t_at(player.ux, player.uy);

  if (Levitation || Is_airlevel(&player.uz) || Underwater ||
      Is_waterlevel(&player.uz)) {
    You("must be on the ground to spin a web.");
    return (0);
  }
  if (player.uswallow) {
    You("release web fluid inside %s.", mon_nam(player.ustuck));
    if (is_animal(player.ustuck->data)) {
      expels(player.ustuck, player.ustuck->data, TRUE);
      return (0);
    }
    if (is_whirly(player.ustuck->data)) {
      int i;

      for (i = 0; i < NATTK; i++)
        if (player.ustuck->data->mattk[i].aatyp == AT_ENGL)
          break;
      if (i == NATTK)
        impossible("Swallower has no engulfing attack?");
      else {
        char sweep[30];

        sweep[0] = '\0';
        switch (player.ustuck->data->mattk[i].adtyp) {
          case AD_FIRE:
            strcpy(sweep, "ignites and ");
            break;
          case AD_ELEC:
            strcpy(sweep, "fries and ");
            break;
          case AD_COLD:
            strcpy(sweep, "freezes, shatters and ");
            break;
        }
        pline_The("web %sis swept away!", sweep);
      }
      return (0);
    } /* default: a nasty jelly-like creature */
    pline_The("web dissolves into %s.", mon_nam(player.ustuck));
    return (0);
  }
  if (player.utrap) {
    You("cannot spin webs while stuck in a trap.");
    return (0);
  }
  exercise(A_DEX, TRUE);
  if (ttmp)
    switch (ttmp->ttyp) {
      case PIT:
      case SPIKED_PIT:
        You("spin a web, covering up the pit.");
        deltrap(ttmp);
        bury_objs(player.ux, player.uy);
        newsym(player.ux, player.uy);
        return (1);
      case SQKY_BOARD:
        pline_The("squeaky board is muffled.");
        deltrap(ttmp);
        newsym(player.ux, player.uy);
        return (1);
      case TELEP_TRAP:
      case LEVEL_TELEP:
      case MAGIC_PORTAL:
        Your("webbing vanishes!");
        return (0);
      case WEB:
        You("make the web thicker.");
        return (1);
      case HOLE:
      case TRAPDOOR:
        You("web over the %s.",
            (ttmp->ttyp == TRAPDOOR) ? "trap door" : "hole");
        deltrap(ttmp);
        newsym(player.ux, player.uy);
        return 1;
      case ROLLING_BOULDER_TRAP:
        You("spin a web, jamming the trigger.");
        deltrap(ttmp);
        newsym(player.ux, player.uy);
        return (1);
      case ARROW_TRAP:
      case DART_TRAP:
      case BEAR_TRAP:
      case ROCKTRAP:
      case FIRE_TRAP:
      case LANDMINE:
      case SLP_GAS_TRAP:
      case RUST_TRAP:
      case MAGIC_TRAP:
      case ANTI_MAGIC:
      case POLY_TRAP:
        You("have triggered a trap!");
        dotrap(ttmp, 0);
        return (1);
      default:
        impossible("Webbing over trap type %d?", ttmp->ttyp);
        return (0);
    }
  else if (On_stairs(player.ux, player.uy)) {
    /* cop out: don't let them hide the stairs */
    Your("web fails to impede access to the %s.",
         (levl[player.ux][player.uy].typ == STAIRS) ? "stairs" : "ladder");
    return (1);
  }
  ttmp = maketrap(player.ux, player.uy, WEB);
  if (ttmp) {
    ttmp->tseen = 1;
    ttmp->madeby_u = 1;
  }
  newsym(player.ux, player.uy);
  return (1);
}

int dosummon() {
  int placeholder;
  if (player.uen < 10) {
    You("lack the energy to send forth a call for help!");
    return (0);
  }
  player.uen -= 10;
  flags.botl = 1;

  You("call upon your brethren for help!");
  exercise(A_WIS, TRUE);
  if (!were_summon(youmonst.data, TRUE, &placeholder, nullptr))
    pline("But none arrive.");
  return (1);
}

int dogaze() {
  Monster *mtmp;
  int looked = 0;
  char qbuf[QBUFSZ];
  int i;
  uchar adtyp = 0;

  for (i = 0; i < NATTK; i++) {
    if (youmonst.data->mattk[i].aatyp == AT_GAZE) {
      adtyp = youmonst.data->mattk[i].adtyp;
      break;
    }
  }
  if (adtyp != AD_CONF && adtyp != AD_FIRE) {
    impossible("gaze attack %d?", adtyp);
    return 0;
  }

  if (Blind) {
    You_cant("see anything to gaze at.");
    return 0;
  }
  if (player.uen < 15) {
    You("lack the energy to use your special gaze!");
    return (0);
  }
  player.uen -= 15;
  flags.botl = 1;

  for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
    if (mtmp->dead())
      continue;
    if (canseemon(mtmp) && couldsee(mtmp->mx, mtmp->my)) {
      looked++;
      if (Invis && !perceives(mtmp->data))
        pline("%s seems not to notice your gaze.", Monnam(mtmp));
      else if (mtmp->minvis && !See_invisible)
        You_cant("see where to gaze at %s.", Monnam(mtmp));
      else if (mtmp->m_ap_type == M_AP_FURNITURE ||
               mtmp->m_ap_type == M_AP_OBJECT) {
        looked--;
        continue;
      } else if (flags.safe_dog && !Confusion && !Hallucination &&
                 mtmp->mtame) {
        You("avoid gazing at %s.", y_monnam(mtmp));
      } else {
        if (flags.confirm && mtmp->mpeaceful && !Confusion && !Hallucination) {
          sprintf(qbuf, "Really %s %s?",
                  (adtyp == AD_CONF) ? "confuse" : "attack", mon_nam(mtmp));
          if (yn(qbuf) != 'y')
            continue;
          setmangry(mtmp);
        }
        if (!mtmp->mcanmove || mtmp->mstun || mtmp->msleeping ||
            !mtmp->mcansee || !haseyes(mtmp->data)) {
          looked--;
          continue;
        }
        /* No reflection check for consistency with when a monster
         * gazes at *you*--only medusa gaze gets reflected then.
         */
        if (adtyp == AD_CONF) {
          if (!mtmp->mconf)
            Your("gaze confuses %s!", mon_nam(mtmp));
          else
            pline("%s is getting more and more confused.", Monnam(mtmp));
          mtmp->mconf = 1;
        } else if (adtyp == AD_FIRE) {
          int dmg = d(2, 6);
          You("attack %s with a fiery gaze!", mon_nam(mtmp));
          if (resists_fire(mtmp)) {
            pline_The("fire doesn't burn %s!", mon_nam(mtmp));
            dmg = 0;
          }
          if ((int)player.ulevel > rn2(20))
            (void)destroy_mitem(mtmp, SCROLL_CLASS, AD_FIRE);
          if ((int)player.ulevel > rn2(20))
            (void)destroy_mitem(mtmp, POTION_CLASS, AD_FIRE);
          if ((int)player.ulevel > rn2(25))
            (void)destroy_mitem(mtmp, SPBOOK_CLASS, AD_FIRE);
          if (dmg && !mtmp->dead())
            mtmp->mhp -= dmg;
          if (mtmp->mhp <= 0)
            killed(mtmp);
        }
        /* For consistency with passive() in uhitm.c, this only
         * affects you if the monster is still alive.
         */
        if (!mtmp->dead() && (mtmp->data == &mons[PM_FLOATING_EYE]) &&
            !mtmp->mcan) {
          if (!Free_action) {
            You("are frozen by %s gaze!", s_suffix(mon_nam(mtmp)));
            nomul((player.ulevel > 6 || rn2(4))
                      ? -d((int)mtmp->m_lev + 1, (int)mtmp->data->mattk[0].damd)
                      : -200,
                  "frozen by a monster's gaze");
            return 1;
          } else
            You("stiffen momentarily under %s gaze.", s_suffix(mon_nam(mtmp)));
        }
        /* Technically this one shouldn't affect you at all because
         * the Medusa gaze is an active monster attack that only
         * works on the monster's turn, but for it to *not* have an
         * effect would be too weird.
         */
        if (!mtmp->dead() && (mtmp->data == &mons[PM_MEDUSA]) && !mtmp->mcan) {
          pline("Gazing at the awake %s is not a very good idea.",
                l_monnam(mtmp));
          /* as if gazing at a sleeping anything is fruitful... */
          You("turn to stone...");
          killer_format = KILLED_BY;
          killer = "deliberately meeting Medusa's gaze";
          done(STONING);
        }
      }
    }
  }
  if (!looked)
    You("gaze at no place in particular.");
  return 1;
}

int dohide() {
  bool ismimic = youmonst.data->mlet == S_MIMIC;

  if (player.uundetected || (ismimic && youmonst.m_ap_type != M_AP_NOTHING)) {
    You("are already hiding.");
    return (0);
  }
  if (ismimic) {
    /* should bring up a dialog "what would you like to imitate?" */
    youmonst.m_ap_type = M_AP_OBJECT;
    youmonst.mappearance = STRANGE_OBJECT;
  } else
    player.uundetected = 1;
  newsym(player.ux, player.uy);
  return (1);
}

int domindblast() {
  Monster *mtmp, *nmon;

  if (player.uen < 10) {
    You("concentrate but lack the energy to maintain doing so.");
    return (0);
  }
  player.uen -= 10;
  flags.botl = 1;

  You("concentrate.");
  pline("A wave of psychic energy pours out.");
  for (mtmp = fmon; mtmp; mtmp = nmon) {
    int u_sen;

    nmon = mtmp->nmon;
    if (mtmp->dead())
      continue;
    if (distu(mtmp->mx, mtmp->my) > BOLT_LIM * BOLT_LIM)
      continue;
    if (mtmp->mpeaceful)
      continue;
    u_sen = telepathic(mtmp->data) && !mtmp->mcansee;
    if (u_sen || (telepathic(mtmp->data) && rn2(2)) || !rn2(10)) {
      You("lock in on %s %s.", s_suffix(mon_nam(mtmp)),
          u_sen ? "telepathy" : telepathic(mtmp->data) ? "latent telepathy"
                                                       : "mind");
      mtmp->mhp -= rnd(15);
      if (mtmp->mhp <= 0)
        killed(mtmp);
    }
  }
  return 1;
}

STATIC_OVL void uunstick() {
  pline("%s is no longer in your clutches.", Monnam(player.ustuck));
  player.ustuck = 0;
}

void skinback(bool silently) {
  if (uskin) {
    if (!silently)
      Your("skin returns to its original form.");
    uarm = uskin;
    uskin = nullptr;
    /* undo save/restore hack */
    uarm->owornmask &= ~I_SPECIAL;
  }
}

#endif /* OVLB */
#ifdef OVL1

const char *mbodypart(Monster *mon, int part) {
  static const char *
      humanoid_parts[] = {"arm",       "eye",  "face",         "finger",
                          "fingertip", "foot", "hand",         "handed",
                          "head",      "leg",  "light headed", "neck",
                          "spine",     "toe",  "hair",         "blood",
                          "lung",      "nose", "stomach"},
     *jelly_parts[] = {
         "pseudopod",           "dark spot",           "front",
         "pseudopod extension", "pseudopod extremity", "pseudopod root",
         "grasp",               "grasped",             "cerebral area",
         "lower pseudopod",     "viscous",             "middle",
         "surface",             "pseudopod extremity", "ripples",
         "juices",              "surface",             "sensor",
         "stomach"},
     *animal_parts[] = {"forelimb", "eye",           "face",         "foreclaw",
                        "claw tip", "rear claw",     "foreclaw",     "clawed",
                        "head",     "rear limb",     "light headed", "neck",
                        "spine",    "rear claw tip", "fur",          "blood",
                        "lung",     "nose",          "stomach"},
     *bird_parts[] = {"wing",         "eye",  "face",   "wing",   "wing tip",
                      "foot",         "wing", "winged", "head",   "leg",
                      "light headed", "neck", "spine",  "toe",    "feathers",
                      "blood",        "lung", "bill",   "stomach"},
     *horse_parts[] = {"foreleg",  "eye",           "face",         "forehoof",
                       "hoof tip", "rear hoof",     "foreclaw",     "hooved",
                       "head",     "rear leg",      "light headed", "neck",
                       "backbone", "rear hoof tip", "mane",         "blood",
                       "lung",     "nose",          "stomach"},
     *sphere_parts[] = {
         "appendage",    "optic nerve",        "body",       "tentacle",
         "tentacle tip", "lower appendage",    "tentacle",   "tentacled",
         "body",         "lower tentacle",     "rotational", "equator",
         "body",         "lower tentacle tip", "cilia",      "life force",
         "retina",       "olfactory nerve",    "interior"},
     *fungus_parts[] = {"mycelium", "visual area", "front",      "hypha",
                        "hypha",    "root",        "strand",     "stranded",
                        "cap area", "rhizome",     "sporulated", "stalk",
                        "root",     "rhizome tip", "spores",     "juices",
                        "gill",     "gill",        "interior"},
     *vortex_parts[] = {
         "region",        "eye",           "front",    "minor current",
         "minor current", "lower current", "swirl",    "swirled",
         "central core",  "lower current", "addled",   "center",
         "currents",      "edge",          "currents", "life force",
         "center",        "leading edge",  "interior"},
     *snake_parts[] = {
         "vestigial limb",  "eye",           "face",         "large scale",
         "large scale tip", "rear region",   "scale gap",    "scale gapped",
         "head",            "rear region",   "light headed", "neck",
         "length",          "rear scale",    "scales",       "blood",
         "lung",            "forked tongue", "stomach"},
     *fish_parts[] = {
         "fin",        "eye",        "premaxillary", "pelvic axillary",
         "pelvic fin", "anal fin",   "pectoral fin", "finned",
         "head",       "peduncle",   "played out",   "gills",
         "dorsal fin", "caudal fin", "scales",       "blood",
         "gill",       "nostril",    "stomach"};
  /* claw attacks are overloaded in mons[]; most humanoids with
     such attacks should still reference hands rather than claws */
  static const char not_claws[] = {
      S_HUMAN,     S_MUMMY,   S_ZOMBIE, S_ANGEL, S_NYMPH, S_LEPRECHAUN,
      S_QUANTMECH, S_VAMPIRE, S_ORC,    S_GIANT, /* quest nemeses */
      '\0' /* string terminator; assert( S_xxx != 0 ); */
  };
  MonsterType *mptr = mon->data;

  if (part == HAND || part == HANDED) { /* some special cases */
    if (mptr->mlet == S_DOG || mptr->mlet == S_FELINE || mptr->mlet == S_YETI)
      return part == HAND ? "paw" : "pawed";
    if (humanoid(mptr) && attacktype(mptr, AT_CLAW) &&
        !index(not_claws, mptr->mlet) && mptr != &mons[PM_STONE_GOLEM] &&
        mptr != &mons[PM_INCUBUS] && mptr != &mons[PM_SUCCUBUS])
      return part == HAND ? "claw" : "clawed";
  }
  if ((mptr == &mons[PM_MUMAK] || mptr == &mons[PM_MASTODON]) && part == NOSE)
    return "trunk";
  if (mptr == &mons[PM_SHARK] && part == HAIR)
    return "skin"; /* sharks don't have scales */
  if (mptr == &mons[PM_JELLYFISH] &&
      (part == ARM || part == FINGER || part == HAND || part == FOOT ||
       part == TOE))
    return "tentacle";
  if (mptr == &mons[PM_FLOATING_EYE] && part == EYE)
    return "cornea";
  if (humanoid(mptr) && (part == ARM || part == FINGER || part == FINGERTIP ||
                         part == HAND || part == HANDED))
    return humanoid_parts[part];
  if (mptr == &mons[PM_RAVEN])
    return bird_parts[part];
  if (mptr->mlet == S_CENTAUR || mptr->mlet == S_UNICORN ||
      (mptr == &mons[PM_ROTHE] && part != HAIR))
    return horse_parts[part];
  if (mptr->mlet == S_LIGHT) {
    if (part == HANDED)
      return "rayed";
    else if (part == ARM || part == FINGER || part == FINGERTIP || part == HAND)
      return "ray";
    else
      return "beam";
  }
  if (mptr->mlet == S_EEL && mptr != &mons[PM_JELLYFISH])
    return fish_parts[part];
  if (slithy(mptr) || (mptr->mlet == S_DRAGON && part == HAIR))
    return snake_parts[part];
  if (mptr->mlet == S_EYE)
    return sphere_parts[part];
  if (mptr->mlet == S_JELLY || mptr->mlet == S_PUDDING ||
      mptr->mlet == S_BLOB || mptr == &mons[PM_JELLYFISH])
    return jelly_parts[part];
  if (mptr->mlet == S_VORTEX || mptr->mlet == S_ELEMENTAL)
    return vortex_parts[part];
  if (mptr->mlet == S_FUNGUS)
    return fungus_parts[part];
  if (humanoid(mptr))
    return humanoid_parts[part];
  return animal_parts[part];
}

const char *body_part(int part) { return mbodypart(&youmonst, part); }

#endif /* OVL1 */
#ifdef OVL0

int poly_gender() {
  /* Returns gender of polymorphed player; 0/1=same meaning as flags.female,
   * 2=none.
   */
  if (is_neuter(youmonst.data) || !humanoid(youmonst.data))
    return 2;
  return flags.female;
}

#endif /* OVL0 */
#ifdef OVLB

void ugolemeffects(int damtype, int dam) {
  int heal = 0;
  /* We won't bother with "slow"/"haste" since players do not
   * have a monster-specific slow/haste so there is no way to
   * restore the old velocity once they are back to human.
   */
  if (player.umonnum != PM_FLESH_GOLEM && player.umonnum != PM_IRON_GOLEM)
    return;
  switch (damtype) {
    case AD_ELEC:
      if (player.umonnum == PM_FLESH_GOLEM)
        heal = dam / 6; /* Approx 1 per die */
      break;
    case AD_FIRE:
      if (player.umonnum == PM_IRON_GOLEM)
        heal = dam;
      break;
  }
  if (heal && (player.mh < player.mhmax)) {
    player.mh += heal;
    if (player.mh > player.mhmax)
      player.mh = player.mhmax;
    flags.botl = 1;
    pline("Strangely, you feel better than before.");
    exercise(A_STR, TRUE);
  }
}

STATIC_OVL int armor_to_dragon(int atyp) {
  switch (atyp) {
    case GRAY_DRAGON_SCALE_MAIL:
    case GRAY_DRAGON_SCALES:
      return PM_GRAY_DRAGON;
    case SILVER_DRAGON_SCALE_MAIL:
    case SILVER_DRAGON_SCALES:
      return PM_SILVER_DRAGON;
#if 0 /* DEFERRED */
	    case SHIMMERING_DRAGON_SCALE_MAIL:
	    case SHIMMERING_DRAGON_SCALES:
		return PM_SHIMMERING_DRAGON;
#endif
    case RED_DRAGON_SCALE_MAIL:
    case RED_DRAGON_SCALES:
      return PM_RED_DRAGON;
    case ORANGE_DRAGON_SCALE_MAIL:
    case ORANGE_DRAGON_SCALES:
      return PM_ORANGE_DRAGON;
    case WHITE_DRAGON_SCALE_MAIL:
    case WHITE_DRAGON_SCALES:
      return PM_WHITE_DRAGON;
    case BLACK_DRAGON_SCALE_MAIL:
    case BLACK_DRAGON_SCALES:
      return PM_BLACK_DRAGON;
    case BLUE_DRAGON_SCALE_MAIL:
    case BLUE_DRAGON_SCALES:
      return PM_BLUE_DRAGON;
    case GREEN_DRAGON_SCALE_MAIL:
    case GREEN_DRAGON_SCALES:
      return PM_GREEN_DRAGON;
    case YELLOW_DRAGON_SCALE_MAIL:
    case YELLOW_DRAGON_SCALES:
      return PM_YELLOW_DRAGON;
    default:
      return -1;
  }
}

#endif /* OVLB */

/*polyself.c*/
