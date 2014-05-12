/*	SCCS Id: @(#)fountain.c	3.4	2003/03/23	*/
/*	Copyright Scott R. Turner, srt@ucla, 10/27/86 */
/* NetHack may be freely redistributed.  See license for details. */

/* Code for drinking from fountains. */

#include "core/fountain.h"

#include "core/hack.h"
#include "core/decl.h"
#include "core/youprop.h"
#include "core/mkroom.h"
#include "core/zap.h"
#include "core/worm.h"
#include "core/steal.h"
#include "core/shk.h"
#include "core/rnd.h"
#include "core/potion.h"
#include "core/polyself.h"
#include "core/pline.h"
#include "core/objnam.h"
#include "core/monmove.h"
#include "core/mon.h"
#include "core/mkobj.h"
#include "core/makemon.h"
#include "core/invent.h"
#include "core/hacklib.h"
#include "core/exper.h"
#include "core/eat.h"
#include "core/do_name.h"
#include "core/detect.h"
#include "core/cmd.h"

#include "core/artifact.h"

STATIC_DCL void dowatersnakes();
STATIC_DCL void dowaterdemon();
STATIC_DCL void dowaternymph();
STATIC_PTR void gush(int, int, genericptr_t);
STATIC_DCL void dofindgem();

void floating_above(const char *what) {
  You("are floating high above the %s.", what);
}

/* Fountain of snakes! */
STATIC_OVL void dowatersnakes() {
  int num = rn1(5, 2);
  Monster *mtmp;

  if (!(mvitals[PM_WATER_MOCCASIN].mvflags & G_GONE)) {
    if (!Blind)
      pline("An endless stream of %s pours forth!",
            Hallucination ? makeplural(rndmonnam()) : "snakes");
    else
      You_hear("%s hissing!", something);
    while (num-- > 0)
      if ((mtmp = makemon(&mons[PM_WATER_MOCCASIN], player.ux, player.uy,
                          NO_MM_FLAGS)) &&
          t_at(mtmp->mx, mtmp->my))
        (void)mintrap(mtmp);
  } else
    pline_The("fountain bubbles furiously for a moment, then calms.");
}

STATIC_OVL
/* Water demon */
void dowaterdemon() {
  Monster *mtmp;

  if (!(mvitals[PM_WATER_DEMON].mvflags & G_GONE)) {
    if ((mtmp = makemon(&mons[PM_WATER_DEMON], player.ux, player.uy,
                        NO_MM_FLAGS))) {
      if (!Blind)
        You("unleash %s!", a_monnam(mtmp));
      else
        You_feel("the presence of evil.");

      /* Give those on low levels a (slightly) better chance of survival */
      if (rnd(100) > (80 + level_difficulty())) {
        pline("Grateful for %s release, %s grants you a wish!", mhis(mtmp),
              mhe(mtmp));
        makewish();
        mongone(mtmp);
      } else if (t_at(mtmp->mx, mtmp->my))
        (void)mintrap(mtmp);
    }
  } else
    pline_The("fountain bubbles furiously for a moment, then calms.");
}

/* Water Nymph */
STATIC_OVL void dowaternymph() {
  Monster *mtmp;

  if (!(mvitals[PM_WATER_NYMPH].mvflags & G_GONE) &&
      (mtmp =
           makemon(&mons[PM_WATER_NYMPH], player.ux, player.uy, NO_MM_FLAGS))) {
    if (!Blind)
      You("attract %s!", a_monnam(mtmp));
    else
      You_hear("a seductive voice.");
    mtmp->msleeping = 0;
    if (t_at(mtmp->mx, mtmp->my))
      (void)mintrap(mtmp);
  } else if (!Blind)
    pline("A large bubble rises to the surface and pops.");
  else
    You_hear("a loud pop.");
}

/* Gushing forth along LOS from (player.ux, player.uy) */
void dogushforth(int drinking) {
  int madepool = 0;

  do_clear_area(player.ux, player.uy, 7, gush, (genericptr_t) & madepool);
  if (!madepool) {
    if (drinking)
      Your("thirst is quenched.");
    else
      pline("Water sprays all over you.");
  }
}

STATIC_PTR void gush(int x, int y, genericptr_t poolcnt) {
  Monster *mtmp;
  Trap *ttmp;

  if (((x + y) % 2) || (x == player.ux && y == player.uy) ||
      (rn2(1 + distmin(player.ux, player.uy, x, y))) ||
      (levl[x][y].typ != ROOM) || (sobj_at(BOULDER, x, y)) || nexttodoor(x, y))
    return;

  if ((ttmp = t_at(x, y)) != 0 && !delfloortrap(ttmp))
    return;

  if (!((*(int *)poolcnt)++))
    pline("Water gushes forth from the overflowing fountain!");

  /* Put a pool at x, y */
  levl[x][y].typ = POOL;
  /* No kelp! */
  del_engr_at(x, y);
  water_damage(level.objects[x][y], FALSE, TRUE);

  if ((mtmp = m_at(x, y)) != 0)
    (void)minliquid(mtmp);
  else
    newsym(x, y);
}

/* Find a gem in the sparkling waters. */
STATIC_OVL void dofindgem() {
  if (!Blind)
    You("spot a gem in the sparkling waters!");
  else
    You_feel("a gem here!");
  (void)MakeSpecificObjectAt(rnd_class(DILITHIUM_CRYSTAL, LUCKSTONE - 1),
                             player.ux, player.uy, FALSE, FALSE);
  SET_FOUNTAIN_LOOTED(player.ux, player.uy);
  newsym(player.ux, player.uy);
  exercise(A_WIS, TRUE); /* a discovery! */
}

void dryup(xchar x, xchar y, bool isyou) {
  if (IS_FOUNTAIN(levl[x][y].typ) && (!rn2(3) || FOUNTAIN_IS_WARNED(x, y))) {
    if (isyou && in_town(x, y) && !FOUNTAIN_IS_WARNED(x, y)) {
      Monster *mtmp;
      SET_FOUNTAIN_WARNED(x, y);
      /* Warn about future fountain use. */
      for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
        if (mtmp->dead())
          continue;
        if ((mtmp->data == &mons[PM_WATCHMAN] ||
             mtmp->data == &mons[PM_WATCH_CAPTAIN]) &&
            couldsee(mtmp->mx, mtmp->my) && mtmp->mpeaceful) {
          pline("%s yells:", Amonnam(mtmp));
          verbalize("Hey, stop using that fountain!");
          break;
        }
      }
      /* You can see or hear this effect */
      if (!mtmp)
        pline_The("flow reduces to a trickle.");
      return;
    }
#ifdef WIZARD
    if (isyou && wizard) {
      if (yn("Dry up fountain?") == 'n')
        return;
    }
#endif
    /* replace the fountain with ordinary floor */
    levl[x][y].typ = ROOM;
    levl[x][y].looted = 0;
    levl[x][y].blessedftn = 0;
    if (cansee(x, y))
      pline_The("fountain dries up!");
    /* The location is seen if the hero/monster is invisible */
    /* or felt if the hero is blind.			 */
    newsym(x, y);
    level.flags.nfountains--;
    if (isyou && in_town(x, y))
      (void)angry_guards(FALSE);
  }
}

void drinkfountain() {
  /* What happens when you drink from a fountain? */
  bool mgkftn = (levl[player.ux][player.uy].blessedftn == 1);
  int fate = rnd(30);

  if (Levitation) {
    floating_above("fountain");
    return;
  }

  if (mgkftn && player.uluck >= 0 && fate >= 10) {
    int i, ii, littleluck = (player.uluck < 4);

    pline("Wow!  This makes you feel great!");
    /* blessed restore ability */
    for (ii = 0; ii < A_MAX; ii++)
      if (ABASE(ii) < AMAX(ii)) {
        ABASE(ii) = AMAX(ii);
        flags.botl = 1;
      }
    /* gain ability, blessed if "natural" luck is high */
    i = rn2(A_MAX); /* start at a random attribute */
    for (ii = 0; ii < A_MAX; ii++) {
      if (adjattrib(i, 1, littleluck ? -1 : 0) && littleluck)
        break;
      if (++i >= A_MAX)
        i = 0;
    }
    display_nhwindow(WIN_MESSAGE, FALSE);
    pline("A wisp of vapor escapes the fountain...");
    exercise(A_WIS, TRUE);
    levl[player.ux][player.uy].blessedftn = 0;
    return;
  }

  if (fate < 10) {
    pline_The("cool draught refreshes you.");
    player.uhunger += rnd(10); /* don't choke on water */
    newuhs(FALSE);
    if (mgkftn)
      return;
  } else {
    switch (fate) {
      case 19: /* Self-knowledge */

        You_feel("self-knowledgeable...");
        display_nhwindow(WIN_MESSAGE, FALSE);
        enlightenment(0);
        exercise(A_WIS, TRUE);
        pline_The("feeling subsides.");
        break;

      case 20: /* Foul water */

        pline_The("water is foul!  You gag and vomit.");
        morehungry(rn1(20, 11));
        vomit();
        break;

      case 21: /* Poisonous */

        pline_The("water is contaminated!");
        if (Poison_resistance) {
          pline("Perhaps it is runoff from the nearby %s farm.",
                fruitname(FALSE));
          losehp(rnd(4), "unrefrigerated sip of juice", KILLED_BY_AN);
          break;
        }
        losestr(rn1(4, 3));
        losehp(rnd(10), "contaminated water", KILLED_BY);
        exercise(A_CON, FALSE);
        break;

      case 22: /* Fountain of snakes! */

        dowatersnakes();
        break;

      case 23: /* Water demon */
        dowaterdemon();
        break;

      case 24: /* Curse an item */ {
        Object *obj;

        pline("This water's no good!");
        morehungry(rn1(20, 11));
        exercise(A_CON, FALSE);
        for (obj = invent; obj; obj = obj->nobj)
          if (!rn2(5))
            Curse(obj);
        break;
      }

      case 25: /* See invisible */

        if (Blind) {
          if (Invisible) {
            You("feel transparent.");
          } else {
            You("feel very self-conscious.");
            pline("Then it passes.");
          }
        } else {
          You("see an image of someone stalking you.");
          pline("But it disappears.");
        }
        HSee_invisible |= FROMOUTSIDE;
        newsym(player.ux, player.uy);
        exercise(A_WIS, TRUE);
        break;

      case 26: /* See Monsters */

        (void)monster_detect(nullptr, 0);
        exercise(A_WIS, TRUE);
        break;

      case 27: /* Find a gem in the sparkling waters. */

        if (!FOUNTAIN_IS_LOOTED(player.ux, player.uy)) {
          dofindgem();
          break;
        }

      case 28: /* Water Nymph */

        dowaternymph();
        break;

      case 29: /* Scare */ {
        Monster *mtmp;

        pline("This water gives you bad breath!");
        for (mtmp = fmon; mtmp; mtmp = mtmp->nmon)
          if (!mtmp->dead())
            monflee(mtmp, 0, FALSE, FALSE);
      } break;

      case 30: /* Gushing forth in this room */

        dogushforth(TRUE);
        break;

      default:

        pline("This tepid water is tasteless.");
        break;
    }
  }
  dryup(player.ux, player.uy, TRUE);
}

void dipfountain(Object *obj) {
  if (Levitation) {
    floating_above("fountain");
    return;
  }

  /* Don't grant Excalibur when there's more than one object.  */
  /* (quantity could be > 1 if merged daggers got polymorphed) */
  if (obj->otyp == LONG_SWORD && obj->quan == 1L && player.ulevel >= 5 &&
      !rn2(6) && !obj->oartifact &&
      !exist_artifact(LONG_SWORD, artiname(ART_EXCALIBUR))) {
    if (player.ualign.type != A_LAWFUL) {
      /* Ha!  Trying to cheat her. */
      pline("A freezing mist rises from the water and envelopes the sword.");
      pline_The("fountain disappears!");
      Curse(obj);
      if (obj->spe > -6 && !rn2(3))
        obj->spe--;
      obj->oerodeproof = FALSE;
      exercise(A_WIS, FALSE);
    } else {
      /* The lady of the lake acts! - Eric Backus */
      /* Be *REAL* nice */
      pline("From the murky depths, a hand reaches up to bless the sword.");
      pline("As the hand retreats, the fountain disappears!");
      obj = oname(obj, artiname(ART_EXCALIBUR));
      discover_artifact(ART_EXCALIBUR);
      Bless(obj);
      obj->oeroded = obj->oeroded2 = 0;
      obj->oerodeproof = TRUE;
      exercise(A_WIS, TRUE);
    }
    update_inventory();
    levl[player.ux][player.uy].typ = ROOM;
    levl[player.ux][player.uy].looted = 0;
    newsym(player.ux, player.uy);
    level.flags.nfountains--;
    if (in_town(player.ux, player.uy))
      (void)angry_guards(FALSE);
    return;
  } else if (get_wet(obj) && !rn2(2))
    return;

  /* Acid and water don't mix */
  if (obj->otyp == POT_ACID) {
    useup(obj);
    return;
  }

  switch (rnd(30)) {
    case 16: /* Curse the item */
      Curse(obj);
      break;
    case 17:
    case 18:
    case 19:
    case 20: /* Uncurse the item */
      if (obj->cursed) {
        if (!Blind)
          pline_The("water glows for a moment.");
        Uncurse(obj);
      } else {
        pline("A feeling of loss comes over you.");
      }
      break;
    case 21: /* Water Demon */
      dowaterdemon();
      break;
    case 22: /* Water Nymph */
      dowaternymph();
      break;
    case 23: /* an Endless Stream of Snakes */
      dowatersnakes();
      break;
    case 24: /* Find a gem */
      if (!FOUNTAIN_IS_LOOTED(player.ux, player.uy)) {
        dofindgem();
        break;
      }
    case 25: /* Water gushes forth */
      dogushforth(FALSE);
      break;
    case 26: /* Strange feeling */
      pline("A strange tingling runs up your %s.", body_part(ARM));
      break;
    case 27: /* Strange feeling */
      You_feel("a sudden chill.");
      break;
    case 28: /* Strange feeling */
      pline("An urge to take a bath overwhelms you.");
#ifndef GOLDOBJ
      if (player.ugold > 10) {
        player.ugold -= somegold() / 10;
        You("lost some of your gold in the fountain!");
        CLEAR_FOUNTAIN_LOOTED(player.ux, player.uy);
        exercise(A_WIS, FALSE);
      }
#else
      {
        long money = money_cnt(invent);
        Object *otmp;
        if (money > 10) {
          /* Amount to loose.  Might get rounded up as fountains don't pay
           * change... */
          money = somegold(money) / 10;
          for (otmp = invent; otmp && money > 0; otmp = otmp->nobj)
            if (otmp->oclass == COIN_CLASS) {
              int denomination = objects[otmp->otyp].oc_cost;
              long coin_loss = (money + denomination - 1) / denomination;
              coin_loss = min(coin_loss, otmp->quan);
              otmp->quan -= coin_loss;
              money -= coin_loss * denomination;
              if (!otmp->quan)
                delobj(otmp);
            }
          You("lost some of your money in the fountain!");
          CLEAR_FOUNTAIN_LOOTED(player.ux, player.uy);
          exercise(A_WIS, FALSE);
        }
      }
#endif
      break;
    case 29: /* You see coins */

      /* We make fountains have more coins the closer you are to the
       * surface.  After all, there will have been more people going
       * by.	Just like a shopping mall!  Chris Woodbury  */

      if (FOUNTAIN_IS_LOOTED(player.ux, player.uy))
        break;
      SET_FOUNTAIN_LOOTED(player.ux, player.uy);
      (void)MakeGold(
          (long)(rnd((dunlevs_in_dungeon(&player.uz) - dunlev(&player.uz) + 1) *
                     2) +
                 5),
          player.ux, player.uy);
      if (!Blind)
        pline("Far below you, you see coins glistening in the water.");
      exercise(A_WIS, TRUE);
      newsym(player.ux, player.uy);
      break;
  }
  update_inventory();
  dryup(player.ux, player.uy, TRUE);
}

#ifdef SINKS
void breaksink(int x, int y) {
  if (cansee(x, y) || (x == player.ux && y == player.uy))
    pline_The("pipes break!  Water spurts out!");
  level.flags.nsinks--;
  levl[x][y].doormask = 0;
  levl[x][y].typ = FOUNTAIN;
  level.flags.nfountains++;
  newsym(x, y);
}

void drinksink() {
  Object *otmp;
  Monster *mtmp;

  if (Levitation) {
    floating_above("sink");
    return;
  }
  switch (rn2(20)) {
    case 0:
      You("take a sip of very cold water.");
      break;
    case 1:
      You("take a sip of very warm water.");
      break;
    case 2:
      You("take a sip of scalding hot water.");
      if (Fire_resistance)
        pline("It seems quite tasty.");
      else
        losehp(rnd(6), "sipping boiling water", KILLED_BY);
      break;
    case 3:
      if (mvitals[PM_SEWER_RAT].mvflags & G_GONE)
        pline_The("sink seems quite dirty.");
      else {
        mtmp = makemon(&mons[PM_SEWER_RAT], player.ux, player.uy, NO_MM_FLAGS);
        if (mtmp)
          pline("Eek!  There's %s in the sink!", (Blind || !canspotmon(mtmp))
                                                     ? "something squirmy"
                                                     : a_monnam(mtmp));
      }
      break;
    case 4:
      do {
        otmp = MakeRandomObject(POTION_CLASS, FALSE);
        if (otmp->otyp == POT_WATER) {
          obfree(otmp, nullptr);
          otmp = nullptr;
        }
      } while (!otmp);
      otmp->cursed = otmp->blessed = 0;
      pline("Some %s liquid flows from the faucet.",
            Blind ? "odd" : hcolor(OBJ_DESCR(objects[otmp->otyp])));
      otmp->dknown = !(Blind || Hallucination);
      otmp->quan++;       /* Avoid panic upon useup() */
      otmp->fromsink = 1; /* kludge for docall() */
      (void)dopotion(otmp);
      obfree(otmp, nullptr);
      break;
    case 5:
      if (!(levl[player.ux][player.uy].looted & S_LRING)) {
        You("find a ring in the sink!");
        (void)MakeRandomObjectAt(RING_CLASS, player.ux, player.uy, TRUE);
        levl[player.ux][player.uy].looted |= S_LRING;
        exercise(A_WIS, TRUE);
        newsym(player.ux, player.uy);
      } else
        pline("Some dirty water backs up in the drain.");
      break;
    case 6:
      breaksink(player.ux, player.uy);
      break;
    case 7:
      pline_The("water moves as though of its own will!");
      if ((mvitals[PM_WATER_ELEMENTAL].mvflags & G_GONE) ||
          !makemon(&mons[PM_WATER_ELEMENTAL], player.ux, player.uy,
                   NO_MM_FLAGS))
        pline("But it quiets down.");
      break;
    case 8:
      pline("Yuk, this water tastes awful.");
      more_experienced(1, 0);
      newexplevel();
      break;
    case 9:
      pline("Gaggg... this tastes like sewage!  You vomit.");
      morehungry(rn1(30 - ACURR(A_CON), 11));
      vomit();
      break;
    case 10:
      pline("This water contains toxic wastes!");
      if (!Unchanging) {
        You("undergo a freakish metamorphosis!");
        polyself(FALSE);
      }
      break;
    /* more odd messages --JJB */
    case 11:
      You_hear("clanking from the pipes...");
      break;
    case 12:
      You_hear("snatches of song from among the sewers...");
      break;
    case 19:
      if (Hallucination) {
        pline("From the murky drain, a hand reaches up... --oops--");
        break;
      }
    default:
      You("take a sip of %s water.",
          rn2(3) ? (rn2(2) ? "cold" : "warm") : "hot");
  }
}
#endif /* SINKS */

/*fountain.c*/
