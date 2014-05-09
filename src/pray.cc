/*	SCCS Id: @(#)pray.c	3.4	2003/03/23	*/
/* Copyright (c) Benson I. Margulies, Mike Stephenson, Steve Linhart, 1989. */
/* NetHack may be freely redistributed.  See license for details. */

#include <string.h>

#include "hack.h"
#include "cmd.h"

#include "artifact.h"
#include "epri.h"

STATIC_PTR int prayer_done();
STATIC_DCL Object *worst_cursed_item();
STATIC_DCL int in_trouble();
STATIC_DCL void fix_worst_trouble(int);
STATIC_DCL void angrygods(ALIGNTYP_P);
STATIC_DCL void at_your_feet(const char *);
#ifdef ELBERETH
STATIC_DCL void gcrownu();
#endif /*ELBERETH*/
STATIC_DCL void pleased(ALIGNTYP_P);
STATIC_DCL void godvoice(ALIGNTYP_P, const char *);
STATIC_DCL void god_zaps_you(ALIGNTYP_P);
STATIC_DCL void fry_by_god(ALIGNTYP_P);
STATIC_DCL void gods_angry(ALIGNTYP_P);
STATIC_DCL void gods_upset(ALIGNTYP_P);
STATIC_DCL void consume_offering(Object *);
STATIC_DCL bool water_prayer(bool);
STATIC_DCL bool blocked_boulder(int, int);

/* simplify a few tests */
#define Cursed_obj(obj, typ) ((obj) && (obj)->otyp == (typ) && (obj)->cursed)

/*
 * Logic behind deities and altars and such:
 * + prayers are made to your god if not on an altar, and to the altar's god
 *   if you are on an altar
 * + If possible, your god answers all prayers, which is why bad things happen
 *   if you try to pray on another god's altar
 * + sacrifices work basically the same way, but the other god may decide to
 *   accept your allegiance, after which they are your god.  If rejected,
 *   your god takes over with your punishment.
 * + if you're in Gehennom, all messages come from Moloch
 */

/*
 *	Moloch, who dwells in Gehennom, is the "renegade" cruel god
 *	responsible for the theft of the Amulet from Marduk, the Creator.
 *	Moloch is unaligned.
 */
static const char *Moloch = "Moloch";

static const char *godvoices[] = {
    "booms out", "thunders", "rings out", "booms",
};

/* values calculated when prayer starts, and used when completed */
static aligntyp p_aligntyp;
static int p_trouble;
static int p_type; /* (-1)-3: (-1)=really naughty, 3=really good */

#define PIOUS 20
#define DEVOUT 14
#define FERVENT 9
#define STRIDENT 4

/*
 * The actual trouble priority is determined by the order of the
 * checks performed in in_trouble() rather than by these numeric
 * values, so keep that code and these values synchronized in
 * order to have the values be meaningful.
 */

#define TROUBLE_STONED 13
#define TROUBLE_SLIMED 12
#define TROUBLE_STRANGLED 11
#define TROUBLE_LAVA 10
#define TROUBLE_SICK 9
#define TROUBLE_STARVING 8
#define TROUBLE_HIT 7
#define TROUBLE_LYCANTHROPE 6
#define TROUBLE_COLLAPSING 5
#define TROUBLE_STUCK_IN_WALL 4
#define TROUBLE_CURSED_LEVITATION 3
#define TROUBLE_UNUSEABLE_HANDS 2
#define TROUBLE_CURSED_BLINDFOLD 1

#define TROUBLE_PUNISHED (-1)
#define TROUBLE_FUMBLING (-2)
#define TROUBLE_CURSED_ITEMS (-3)
#define TROUBLE_SADDLE (-4)
#define TROUBLE_BLIND (-5)
#define TROUBLE_POISONED (-6)
#define TROUBLE_WOUNDED_LEGS (-7)
#define TROUBLE_HUNGRY (-8)
#define TROUBLE_STUNNED (-9)
#define TROUBLE_CONFUSED (-10)
#define TROUBLE_HALLUCINATION (-11)

/* We could force rehumanize of polyselfed people, but we can't tell
   unintentional shape changes from the other kind. Oh well.
   3.4.2: make an exception if polymorphed into a form which lacks
   hands; that's a case where the ramifications override this doubt.
 */

/* Return 0 if nothing particular seems wrong, positive numbers for
   serious trouble, and negative numbers for comparative annoyances. This
   returns the worst problem. There may be others, and the gods may fix
   more than one.

This could get as bizarre as noting surrounding opponents, (or hostile dogs),
but that's really hard.
 */

#define ugod_is_angry() (player.ualign.record < 0)
#define on_altar() IS_ALTAR(levl[player.ux][player.uy].typ)
#define on_shrine() ((levl[player.ux][player.uy].altarmask & AM_SHRINE) != 0)
#define a_align(x, y) ((aligntyp)Amask2align(levl[x][y].altarmask & AM_MASK))

STATIC_OVL int in_trouble() {
  Object *otmp;
  int i, j, count = 0;

/* Borrowed from eat.c */

#define SATIATED 0
#define NOT_HUNGRY 1
#define HUNGRY 2
#define WEAK 3
#define FAINTING 4
#define FAINTED 5
#define STARVED 6

  /*
   * major troubles
   */
  if (Stoned)
    return (TROUBLE_STONED);
  if (Slimed)
    return (TROUBLE_SLIMED);
  if (Strangled)
    return (TROUBLE_STRANGLED);
  if (player.utrap && player.utraptype == TT_LAVA)
    return (TROUBLE_LAVA);
  if (Sick)
    return (TROUBLE_SICK);
  if (player.uhs >= WEAK)
    return (TROUBLE_STARVING);
  if (Upolyd ? (player.mh <= 5 || player.mh * 7 <= player.mhmax)
             : (player.uhp <= 5 || player.uhp * 7 <= player.uhpmax))
    return TROUBLE_HIT;
  if (player.ulycn >= LOW_PM)
    return (TROUBLE_LYCANTHROPE);
  if (near_capacity() >= EXT_ENCUMBER && AMAX(A_STR) - ABASE(A_STR) > 3)
    return (TROUBLE_COLLAPSING);

  for (i = -1; i <= 1; i++)
    for (j = -1; j <= 1; j++) {
      if (!i && !j)
        continue;
      if (!isok(player.ux + i, player.uy + j) ||
          IS_ROCK(levl[player.ux + i][player.uy + j].typ) ||
          (blocked_boulder(i, j) && !throws_rocks(youmonst.data)))
        count++;
    }
  if (count == 8 && !Passes_walls)
    return (TROUBLE_STUCK_IN_WALL);

  if (Cursed_obj(uarmf, LEVITATION_BOOTS) ||
      stuck_ring(uleft, RIN_LEVITATION) || stuck_ring(uright, RIN_LEVITATION))
    return (TROUBLE_CURSED_LEVITATION);
  if (nohands(youmonst.data) || !freehand()) {
    /* for bag/box access [cf use_container()]...
       make sure it's a case that we know how to handle;
       otherwise "fix all troubles" would get stuck in a loop */
    if (welded(uwep))
      return TROUBLE_UNUSEABLE_HANDS;
    if (Upolyd && nohands(youmonst.data) &&
        (!Unchanging || ((otmp = unchanger()) != 0 && otmp->cursed)))
      return TROUBLE_UNUSEABLE_HANDS;
  }
  if (Blindfolded && ublindf->cursed)
    return (TROUBLE_CURSED_BLINDFOLD);

  /*
   * minor troubles
   */
  if (Punished)
    return (TROUBLE_PUNISHED);
  if (Cursed_obj(uarmg, GAUNTLETS_OF_FUMBLING) ||
      Cursed_obj(uarmf, FUMBLE_BOOTS))
    return TROUBLE_FUMBLING;
  if (worst_cursed_item())
    return TROUBLE_CURSED_ITEMS;
#ifdef STEED
  if (player.usteed) { /* can't voluntarily dismount from a cursed saddle */
    otmp = which_armor(player.usteed, W_SADDLE);
    if (Cursed_obj(otmp, SADDLE))
      return TROUBLE_SADDLE;
  }
#endif

  if (Blinded > 1 && haseyes(youmonst.data))
    return (TROUBLE_BLIND);
  for (i = 0; i < A_MAX; i++)
    if (ABASE(i) < AMAX(i))
      return (TROUBLE_POISONED);
  if (Wounded_legs
#ifdef STEED
      && !player.usteed
#endif
      )
    return (TROUBLE_WOUNDED_LEGS);
  if (player.uhs >= HUNGRY)
    return (TROUBLE_HUNGRY);
  if (HStun)
    return (TROUBLE_STUNNED);
  if (HConfusion)
    return (TROUBLE_CONFUSED);
  if (Hallucination)
    return (TROUBLE_HALLUCINATION);
  return (0);
}

/* select an item for TROUBLE_CURSED_ITEMS */
STATIC_OVL Object *worst_cursed_item() {
  Object *otmp;

  /* if strained or worse, check for loadstone first */
  if (near_capacity() >= HVY_ENCUMBER) {
    for (otmp = invent; otmp; otmp = otmp->nobj)
      if (Cursed_obj(otmp, LOADSTONE))
        return otmp;
  }
  /* weapon takes precedence if it is interfering
     with taking off a ring or putting on a shield */
  if (welded(uwep) && (uright || bimanual(uwep))) { /* weapon */
    otmp = uwep;
    /* gloves come next, due to rings */
  } else if (uarmg && uarmg->cursed) { /* gloves */
    otmp = uarmg;
    /* then shield due to two handed weapons and spells */
  } else if (uarms && uarms->cursed) { /* shield */
    otmp = uarms;
    /* then cloak due to body armor */
  } else if (uarmc && uarmc->cursed) { /* cloak */
    otmp = uarmc;
  } else if (uarm && uarm->cursed) { /* suit */
    otmp = uarm;
  } else if (uarmh && uarmh->cursed) { /* helmet */
    otmp = uarmh;
  } else if (uarmf && uarmf->cursed) { /* boots */
    otmp = uarmf;
#ifdef TOURIST
  } else if (uarmu && uarmu->cursed) { /* shirt */
    otmp = uarmu;
#endif
  } else if (uamul && uamul->cursed) { /* amulet */
    otmp = uamul;
  } else if (uleft && uleft->cursed) { /* left ring */
    otmp = uleft;
  } else if (uright && uright->cursed) { /* right ring */
    otmp = uright;
  } else if (ublindf && ublindf->cursed) { /* eyewear */
    otmp = ublindf;                        /* must be non-blinding lenses */
    /* if weapon wasn't handled above, do it now */
  } else if (welded(uwep)) { /* weapon */
    otmp = uwep;
    /* active secondary weapon even though it isn't welded */
  } else if (uswapwep && uswapwep->cursed && player.twoweap) {
    otmp = uswapwep;
    /* all worn items ought to be handled by now */
  } else {
    for (otmp = invent; otmp; otmp = otmp->nobj) {
      if (!otmp->cursed)
        continue;
      if (otmp->otyp == LOADSTONE || confers_luck(otmp))
        break;
    }
  }
  return otmp;
}

STATIC_OVL void fix_worst_trouble(int trouble) {
  int i;
  Object *otmp = 0;
  const char *what = (const char *)0;
  static const char leftglow[] = "left ring softly glows",
                    rightglow[] = "right ring softly glows";

  switch (trouble) {
    case TROUBLE_STONED:
      You_feel("more limber.");
      Stoned = 0;
      flags.botl = 1;
      delayed_killer = 0;
      break;
    case TROUBLE_SLIMED:
      pline_The("slime disappears.");
      Slimed = 0;
      flags.botl = 1;
      delayed_killer = 0;
      break;
    case TROUBLE_STRANGLED:
      if (uamul && uamul->otyp == AMULET_OF_STRANGULATION) {
        Your("amulet vanishes!");
        useup(uamul);
      }
      You("can breathe again.");
      Strangled = 0;
      flags.botl = 1;
      break;
    case TROUBLE_LAVA:
      You("are back on solid ground.");
      /* teleport should always succeed, but if not,
       * just untrap them.
       */
      if (!safe_teleds(FALSE))
        player.utrap = 0;
      break;
    case TROUBLE_STARVING:
      losestr(-1);
    /* fall into... */
    case TROUBLE_HUNGRY:
      Your("%s feels content.", body_part(STOMACH));
      init_uhunger();
      flags.botl = 1;
      break;
    case TROUBLE_SICK:
      You_feel("better.");
      make_sick(0L, nullptr, FALSE, SICK_ALL);
      break;
    case TROUBLE_HIT:
      /* "fix all troubles" will keep trying if hero has
         5 or less hit points, so make sure they're always
         boosted to be more than that */
      You_feel("much better.");
      if (Upolyd) {
        player.mhmax += rnd(5);
        if (player.mhmax <= 5)
          player.mhmax = 5 + 1;
        player.mh = player.mhmax;
      }
      if (player.uhpmax < player.ulevel * 5 + 11)
        player.uhpmax += rnd(5);
      if (player.uhpmax <= 5)
        player.uhpmax = 5 + 1;
      player.uhp = player.uhpmax;
      flags.botl = 1;
      break;
    case TROUBLE_COLLAPSING:
      ABASE(A_STR) = AMAX(A_STR);
      flags.botl = 1;
      break;
    case TROUBLE_STUCK_IN_WALL:
      Your("surroundings change.");
      /* no control, but works on no-teleport levels */
      (void)safe_teleds(FALSE);
      break;
    case TROUBLE_CURSED_LEVITATION:
      if (Cursed_obj(uarmf, LEVITATION_BOOTS)) {
        otmp = uarmf;
      } else if ((otmp = stuck_ring(uleft, RIN_LEVITATION)) != 0) {
        if (otmp == uleft)
          what = leftglow;
      } else if ((otmp = stuck_ring(uright, RIN_LEVITATION)) != 0) {
        if (otmp == uright)
          what = rightglow;
      }
      goto decurse;
    case TROUBLE_UNUSEABLE_HANDS:
      if (welded(uwep)) {
        otmp = uwep;
        goto decurse;
      }
      if (Upolyd && nohands(youmonst.data)) {
        if (!Unchanging) {
          Your("shape becomes uncertain.");
          rehumanize(); /* "You return to {normal} form." */
        } else if ((otmp = unchanger()) != 0 && otmp->cursed) {
          /* otmp is an amulet of unchanging */
          goto decurse;
        }
      }
      if (nohands(youmonst.data) || !freehand())
        impossible("fix_worst_trouble: couldn't cure hands.");
      break;
    case TROUBLE_CURSED_BLINDFOLD:
      otmp = ublindf;
      goto decurse;
    case TROUBLE_LYCANTHROPE:
      you_unwere(TRUE);
      break;
    /*
     */
    case TROUBLE_PUNISHED:
      Your("chain disappears.");
      unpunish();
      break;
    case TROUBLE_FUMBLING:
      if (Cursed_obj(uarmg, GAUNTLETS_OF_FUMBLING))
        otmp = uarmg;
      else if (Cursed_obj(uarmf, FUMBLE_BOOTS))
        otmp = uarmf;
      goto decurse;
      /*NOTREACHED*/
      break;
    case TROUBLE_CURSED_ITEMS:
      otmp = worst_cursed_item();
      if (otmp == uright)
        what = rightglow;
      else if (otmp == uleft)
        what = leftglow;
    decurse:
      if (!otmp) {
        impossible("fix_worst_trouble: nothing to uncurse.");
        return;
      }
      Uncurse(otmp);
      if (!Blind) {
        Your("%s %s.", what ? what : (const char *)aobjnam(otmp, "softly glow"),
             hcolor(NH_AMBER));
        otmp->bknown = TRUE;
      }
      update_inventory();
      break;
    case TROUBLE_POISONED:
      if (Hallucination)
        pline("There's a tiger in your tank.");
      else
        You_feel("in good health again.");
      for (i = 0; i < A_MAX; i++) {
        if (ABASE(i) < AMAX(i)) {
          ABASE(i) = AMAX(i);
          flags.botl = 1;
        }
      }
      (void)encumber_msg();
      break;
    case TROUBLE_BLIND: {
      int num_eyes = eyecount(youmonst.data);
      const char *eye = body_part(EYE);

      Your("%s feel%s better.", (num_eyes == 1) ? eye : makeplural(eye),
           (num_eyes == 1) ? "s" : "");
      player.ucreamed = 0;
      make_blinded(0L, FALSE);
      break;
    }
    case TROUBLE_WOUNDED_LEGS:
      heal_legs();
      break;
    case TROUBLE_STUNNED:
      make_stunned(0L, TRUE);
      break;
    case TROUBLE_CONFUSED:
      make_confused(0L, TRUE);
      break;
    case TROUBLE_HALLUCINATION:
      pline("Looks like you are back in Kansas.");
      (void)make_hallucinated(0L, FALSE, 0L);
      break;
#ifdef STEED
    case TROUBLE_SADDLE:
      otmp = which_armor(player.usteed, W_SADDLE);
      Uncurse(otmp);
      if (!Blind) {
        pline("%s %s %s.", s_suffix(upstart(y_monnam(player.usteed))),
              aobjnam(otmp, "softly glow"), hcolor(NH_AMBER));
        otmp->bknown = TRUE;
      }
      break;
#endif
  }
}

/* "I am sometimes shocked by...  the nuns who never take a bath without
 * wearing a bathrobe all the time.  When asked why, since no man can see them,
 * they reply 'Oh, but you forget the good God'.  Apparently they conceive of
 * the Deity as a Peeping Tom, whose omnipotence enables Him to see through
 * bathroom walls, but who is foiled by bathrobes." --Bertrand Russell, 1943
 * Divine wrath, dungeon walls, and armor follow the same principle.
 */
STATIC_OVL void god_zaps_you(aligntyp resp_god) {
  if (player.uswallow) {
    pline("Suddenly a bolt of lightning comes down at you from the heavens!");
    pline("It strikes %s!", mon_nam(player.ustuck));
    if (!resists_elec(player.ustuck)) {
      pline("%s fries to a crisp!", Monnam(player.ustuck));
      /* Yup, you get experience.  It takes guts to successfully
       * pull off this trick on your god, anyway.
       */
      xkilled(player.ustuck, 0);
    } else
      pline("%s seems unaffected.", Monnam(player.ustuck));
  } else {
    pline("Suddenly, a bolt of lightning strikes you!");
    if (Reflecting) {
      shieldeff(player.ux, player.uy);
      if (Blind)
        pline("For some reason you're unaffected.");
      else
        (void)ureflects("%s reflects from your %s.", "It");
    } else if (Shock_resistance) {
      shieldeff(player.ux, player.uy);
      pline("It seems not to affect you.");
    } else
      fry_by_god(resp_god);
  }

  pline("%s is not deterred...", align_gname(resp_god));
  if (player.uswallow) {
    pline("A wide-angle disintegration beam aimed at you hits %s!",
          mon_nam(player.ustuck));
    if (!resists_disint(player.ustuck)) {
      pline("%s fries to a crisp!", Monnam(player.ustuck));
      xkilled(player.ustuck, 2); /* no corpse */
    } else
      pline("%s seems unaffected.", Monnam(player.ustuck));
  } else {
    pline("A wide-angle disintegration beam hits you!");

    /* disintegrate shield and body armor before disintegrating
     * the impudent mortal, like black dragon breath -3.
     */
    if (uarms && !(EReflecting & W_ARMS) && !(EDisint_resistance & W_ARMS))
      (void)destroy_arm(uarms);
    if (uarmc && !(EReflecting & W_ARMC) && !(EDisint_resistance & W_ARMC))
      (void)destroy_arm(uarmc);
    if (uarm && !(EReflecting & W_ARM) && !(EDisint_resistance & W_ARM) &&
        !uarmc)
      (void)destroy_arm(uarm);
#ifdef TOURIST
    if (uarmu && !uarm && !uarmc)
      (void)destroy_arm(uarmu);
#endif
    if (!Disint_resistance)
      fry_by_god(resp_god);
    else {
      You("bask in its %s glow for a minute...", NH_BLACK);
      godvoice(resp_god, "I believe it not!");
    }
    if (Is_astralevel(&player.uz) || Is_sanctum(&player.uz)) {
      /* one more try for high altars */
      verbalize("Thou cannot escape my wrath, mortal!");
      summon_minion(resp_god, FALSE);
      summon_minion(resp_god, FALSE);
      summon_minion(resp_god, FALSE);
      verbalize("Destroy %s, my servants!", uhim());
    }
  }
}

STATIC_OVL void fry_by_god(aligntyp resp_god) {
  char killerbuf[64];

  You("fry to a crisp.");
  killer_format = KILLED_BY;
  sprintf(killerbuf, "the wrath of %s", align_gname(resp_god));
  killer = killerbuf;
  done(DIED);
}

STATIC_OVL void angrygods(aligntyp resp_god) {
  int maxanger;

  if (Inhell)
    resp_god = A_NONE;
  player.ublessed = 0;

  /* changed from tmp = player.ugangr + abs (player.uluck) -- rph */
  /* added test for alignment diff -dlc */
  if (resp_god != player.ualign.type)
    maxanger = player.ualign.record / 2 + (Luck > 0 ? -Luck / 3 : -Luck);
  else
    maxanger =
        3 * player.ugangr +
        ((Luck > 0 || player.ualign.record >= STRIDENT) ? -Luck / 3 : -Luck);
  if (maxanger < 1)
    maxanger = 1; /* possible if bad align & good luck */
  else if (maxanger > 15)
    maxanger = 15; /* be reasonable */

  switch (rn2(maxanger)) {
    case 0:
    case 1:
      You_feel("that %s is %s.", align_gname(resp_god),
               Hallucination ? "bummed" : "displeased");
      break;
    case 2:
    case 3:
      godvoice(resp_god, nullptr);
      pline("\"Thou %s, %s.\"",
            (ugod_is_angry() && resp_god == player.ualign.type)
                ? "hast strayed from the path"
                : "art arrogant",
            youmonst.data->mlet == S_HUMAN ? "mortal" : "creature");
      verbalize("Thou must relearn thy lessons!");
      (void)adjattrib(A_WIS, -1, FALSE);
      losexp(nullptr);
      break;
    case 6:
      if (!Punished) {
        gods_angry(resp_god);
        punish(nullptr);
        break;
      } /* else fall thru */
    case 4:
    case 5:
      gods_angry(resp_god);
      if (!Blind && !Antimagic)
        pline("%s glow surrounds you.", An(hcolor(NH_BLACK)));
      rndcurse();
      break;
    case 7:
    case 8:
      godvoice(resp_god, nullptr);
      verbalize("Thou durst %s me?",
                (on_altar() && (a_align(player.ux, player.uy) != resp_god))
                    ? "scorn"
                    : "call upon");
      pline("\"Then die, %s!\"",
            youmonst.data->mlet == S_HUMAN ? "mortal" : "creature");
      summon_minion(resp_god, FALSE);
      break;

    default:
      gods_angry(resp_god);
      god_zaps_you(resp_god);
      break;
  }
  player.ublesscnt = rnz(300);
  return;
}

/* helper to print "str appears at your feet", or appropriate */
static void at_your_feet(const char *str) {
  if (Blind)
    str = Something;
  if (player.uswallow) {
    /* barrier between you and the floor */
    pline("%s %s into %s %s.", str, vtense(str, "drop"),
          s_suffix(mon_nam(player.ustuck)), mbodypart(player.ustuck, STOMACH));
  } else {
    pline("%s %s %s your %s!", str, Blind ? "lands" : vtense(str, "appear"),
          Levitation ? "beneath" : "at", makeplural(body_part(FOOT)));
  }
}

#ifdef ELBERETH
STATIC_OVL void gcrownu() {
  Object *obj;
  bool already_exists, in_hand;
  short class_gift;
  int sp_no;
#define ok_wep(o) ((o) && ((o)->oclass == WEAPON_CLASS || is_weptool(o)))

  HSee_invisible |= FROMOUTSIDE;
  HFire_resistance |= FROMOUTSIDE;
  HCold_resistance |= FROMOUTSIDE;
  HShock_resistance |= FROMOUTSIDE;
  HSleep_resistance |= FROMOUTSIDE;
  HPoison_resistance |= FROMOUTSIDE;
  godvoice(player.ualign.type, nullptr);

  obj = ok_wep(uwep) ? uwep : 0;
  already_exists = in_hand = FALSE; /* lint suppression */
  switch (player.ualign.type) {
    case A_LAWFUL:
      player.uevent.uhand_of_elbereth = 1;
      verbalize("I crown thee...  The Hand of Elbereth!");
      break;
    case A_NEUTRAL:
      player.uevent.uhand_of_elbereth = 2;
      in_hand = (uwep && uwep->oartifact == ART_VORPAL_BLADE);
      already_exists = exist_artifact(LONG_SWORD, artiname(ART_VORPAL_BLADE));
      verbalize("Thou shalt be my Envoy of Balance!");
      break;
    case A_CHAOTIC:
      player.uevent.uhand_of_elbereth = 3;
      in_hand = (uwep && uwep->oartifact == ART_STORMBRINGER);
      already_exists = exist_artifact(RUNESWORD, artiname(ART_STORMBRINGER));
      verbalize("Thou art chosen to %s for My Glory!",
                already_exists && !in_hand ? "take lives" : "steal souls");
      break;
  }

  class_gift = STRANGE_OBJECT;
  /* 3.3.[01] had this in the A_NEUTRAL case below,
     preventing chaotic wizards from receiving a spellbook */
  if (Role_if(PM_WIZARD) && (!uwep || (uwep->oartifact != ART_VORPAL_BLADE &&
                                       uwep->oartifact != ART_STORMBRINGER)) &&
      !carrying(SPE_FINGER_OF_DEATH)) {
    class_gift = SPE_FINGER_OF_DEATH;
  make_splbk:
    obj = MakeSpecificObject(class_gift, TRUE, FALSE);
    Bless(obj);
    obj->bknown = TRUE;
    at_your_feet("A spellbook");
    dropy(obj);
    player.ugifts++;
    /* when getting a new book for known spell, enhance
       currently wielded weapon rather than the book */
    for (sp_no = 0; sp_no < MAXSPELL; sp_no++)
      if (spl_book[sp_no].sp_id == class_gift) {
        if (ok_wep(uwep))
          obj = uwep; /* to be blessed,&c */
        break;
      }
  } else if (Role_if(PM_MONK) && (!uwep || !uwep->oartifact) &&
             !carrying(SPE_RESTORE_ABILITY)) {
    /* monks rarely wield a weapon */
    class_gift = SPE_RESTORE_ABILITY;
    goto make_splbk;
  }

  switch (player.ualign.type) {
    case A_LAWFUL:
      if (class_gift != STRANGE_OBJECT) {
        ; /* already got bonus above */
      } else if (obj && obj->otyp == LONG_SWORD && !obj->oartifact) {
        if (!Blind)
          Your("sword shines brightly for a moment.");
        obj = oname(obj, artiname(ART_EXCALIBUR));
        if (obj && obj->oartifact == ART_EXCALIBUR)
          player.ugifts++;
      }
      /* acquire Excalibur's skill regardless of weapon or gift */
      unrestrict_weapon_skill(P_LONG_SWORD);
      if (obj && obj->oartifact == ART_EXCALIBUR)
        discover_artifact(ART_EXCALIBUR);
      break;
    case A_NEUTRAL:
      if (class_gift != STRANGE_OBJECT) {
        ; /* already got bonus above */
      } else if (in_hand) {
        Your("%s goes snicker-snack!", xname(obj));
        obj->dknown = TRUE;
      } else if (!already_exists) {
        obj = MakeSpecificObject(LONG_SWORD, FALSE, FALSE);
        obj = oname(obj, artiname(ART_VORPAL_BLADE));
        obj->spe = 1;
        at_your_feet("A sword");
        dropy(obj);
        player.ugifts++;
      }
      /* acquire Vorpal Blade's skill regardless of weapon or gift */
      unrestrict_weapon_skill(P_LONG_SWORD);
      if (obj && obj->oartifact == ART_VORPAL_BLADE)
        discover_artifact(ART_VORPAL_BLADE);
      break;
    case A_CHAOTIC: {
      char swordbuf[BUFSZ];

      sprintf(swordbuf, "%s sword", hcolor(NH_BLACK));
      if (class_gift != STRANGE_OBJECT) {
        ; /* already got bonus above */
      } else if (in_hand) {
        Your("%s hums ominously!", swordbuf);
        obj->dknown = TRUE;
      } else if (!already_exists) {
        obj = MakeSpecificObject(RUNESWORD, FALSE, FALSE);
        obj = oname(obj, artiname(ART_STORMBRINGER));
        at_your_feet(An(swordbuf));
        obj->spe = 1;
        dropy(obj);
        player.ugifts++;
      }
      /* acquire Stormbringer's skill regardless of weapon or gift */
      unrestrict_weapon_skill(P_BROAD_SWORD);
      if (obj && obj->oartifact == ART_STORMBRINGER)
        discover_artifact(ART_STORMBRINGER);
      break;
    }
    default:
      obj = 0; /* lint */
      break;
  }

  /* enhance weapon regardless of alignment or artifact status */
  if (ok_wep(obj)) {
    Bless(obj);
    obj->oeroded = obj->oeroded2 = 0;
    obj->oerodeproof = TRUE;
    obj->bknown = obj->rknown = TRUE;
    if (obj->spe < 1)
      obj->spe = 1;
    /* acquire skill in this weapon */
    unrestrict_weapon_skill(weapon_type(obj));
  } else if (class_gift == STRANGE_OBJECT) {
    /* opportunity knocked, but there was nobody home... */
    You_feel("unworthy.");
  }
  update_inventory();
  return;
}
#endif /*ELBERETH*/

STATIC_OVL void pleased(aligntyp g_align) {
  /* don't use p_trouble, worst trouble may get fixed while praying */
  int trouble = in_trouble(); /* what's your worst difficulty? */
  int pat_on_head = 0, kick_on_butt;

  You_feel("that %s is %s.", align_gname(g_align),
           player.ualign.record >= DEVOUT ? Hallucination ? "pleased as punch"
           : "well-pleased"
           : player.ualign.record >= STRIDENT ? Hallucination ? "ticklish"
           : "pleased"
           : Hallucination ? "full" : "satisfied");

  /* not your deity */
  if (on_altar() && p_aligntyp != player.ualign.type) {
    adjalign(-1);
    return;
  } else if (player.ualign.record < 2 && trouble <= 0)
    adjalign(1);

  /* depending on your luck & align level, the god you prayed to will:
     - fix your worst problem if it's major.
     - fix all your major problems.
     - fix your worst problem if it's minor.
     - fix all of your problems.
     - do you a gratuitous favor.

     if you make it to the the last category, you roll randomly again
     to see what they do for you.

     If your luck is at least 0, then you are guaranteed rescued
     from your worst major problem. */

  if (!trouble && player.ualign.record >= DEVOUT) {
    /* if hero was in trouble, but got better, no special favor */
    if (p_trouble == 0)
      pat_on_head = 1;
  } else {
    int action = rn1(Luck + (on_altar() ? 3 + on_shrine() : 2), 1);

    if (!on_altar())
      action = min(action, 3);
    if (player.ualign.record < STRIDENT)
      action = (player.ualign.record > 0 || !rnl(2)) ? 1 : 0;

    switch (min(action, 5)) {
      case 5:
        pat_on_head = 1;
      case 4:
        do
          fix_worst_trouble(trouble);
        while ((trouble = in_trouble()) != 0);
        break;

      case 3:
        fix_worst_trouble(trouble);
      case 2:
        while ((trouble = in_trouble()) > 0)
          fix_worst_trouble(trouble);
        break;

      case 1:
        if (trouble > 0)
          fix_worst_trouble(trouble);
      case 0:
        break; /* your god blows you off, too bad */
    }
  }

  /* note: can't get pat_on_head unless all troubles have just been
     fixed or there were no troubles to begin with; hallucination
     won't be in effect so special handling for it is superfluous */
  if (pat_on_head)
    switch (rn2((Luck + 6) >> 1)) {
      case 0:
        break;
      case 1:
        if (uwep && (welded(uwep) || uwep->oclass == WEAPON_CLASS ||
                     is_weptool(uwep))) {
          char repair_buf[BUFSZ];

          *repair_buf = '\0';
          if (uwep->oeroded || uwep->oeroded2)
            sprintf(repair_buf, " and %s now as good as new",
                    otense(uwep, "are"));

          if (uwep->cursed) {
            Uncurse(uwep);
            uwep->bknown = TRUE;
            if (!Blind)
              Your("%s %s%s.", aobjnam(uwep, "softly glow"), hcolor(NH_AMBER),
                   repair_buf);
            else
              You_feel("the power of %s over your %s.", u_gname(), xname(uwep));
            *repair_buf = '\0';
          } else if (!uwep->blessed) {
            Bless(uwep);
            uwep->bknown = TRUE;
            if (!Blind)
              Your("%s with %s aura%s.", aobjnam(uwep, "softly glow"),
                   an(hcolor(NH_LIGHT_BLUE)), repair_buf);
            else
              You_feel("the blessing of %s over your %s.", u_gname(),
                       xname(uwep));
            *repair_buf = '\0';
          }

          /* fix any rust/burn/rot damage, but don't protect
             against future damage */
          if (uwep->oeroded || uwep->oeroded2) {
            uwep->oeroded = uwep->oeroded2 = 0;
            /* only give this message if we didn't just bless
               or uncurse (which has already given a message) */
            if (*repair_buf)
              Your("%s as good as new!",
                   aobjnam(uwep, Blind ? "feel" : "look"));
          }
          update_inventory();
        }
        break;
      case 3:
        /* takes 2 hints to get the music to enter the stronghold */
        if (!player.uevent.uopened_dbridge) {
          if (player.uevent.uheard_tune < 1) {
            godvoice(g_align, nullptr);
            verbalize("Hark, %s!",
                      youmonst.data->mlet == S_HUMAN ? "mortal" : "creature");
            verbalize("To enter the castle, thou must play the right tune!");
            player.uevent.uheard_tune++;
            break;
          } else if (player.uevent.uheard_tune < 2) {
            You_hear("a divine music...");
            pline("It sounds like:  \"%s\".", tune);
            player.uevent.uheard_tune++;
            break;
          }
        }
      /* Otherwise, falls into next case */
      case 2:
        if (!Blind)
          You("are surrounded by %s glow.", an(hcolor(NH_GOLDEN)));
        /* if any levels have been lost (and not yet regained),
           treat this effect like blessed full healing */
        if (player.ulevel < player.ulevelmax) {
          player.ulevelmax -= 1; /* see potion.c */
          pluslvl(FALSE);
        } else {
          player.uhpmax += 5;
          if (Upolyd)
            player.mhmax += 5;
        }
        player.uhp = player.uhpmax;
        if (Upolyd)
          player.mh = player.mhmax;
        ABASE(A_STR) = AMAX(A_STR);
        if (player.uhunger < 900)
          init_uhunger();
        if (player.uluck < 0)
          player.uluck = 0;
        make_blinded(0L, TRUE);
        flags.botl = 1;
        break;
      case 4: {
        Object *otmp;
        int any = 0;

        if (Blind)
          You_feel("the power of %s.", u_gname());
        else
          You("are surrounded by %s aura.", an(hcolor(NH_LIGHT_BLUE)));
        for (otmp = invent; otmp; otmp = otmp->nobj) {
          if (otmp->cursed) {
            Uncurse(otmp);
            if (!Blind) {
              Your("%s %s.", aobjnam(otmp, "softly glow"), hcolor(NH_AMBER));
              otmp->bknown = TRUE;
              ++any;
            }
          }
        }
        if (any)
          update_inventory();
        break;
      }
      case 5: {
        const char *msg = "\"and thus I grant thee the gift of %s!\"";
        godvoice(player.ualign.type, "Thou hast pleased me with thy progress,");
        if (!(HTelepat & INTRINSIC)) {
          HTelepat |= FROMOUTSIDE;
          pline(msg, "Telepathy");
          if (Blind)
            see_monsters();
        } else if (!(HFast & INTRINSIC)) {
          HFast |= FROMOUTSIDE;
          pline(msg, "Speed");
        } else if (!(HStealth & INTRINSIC)) {
          HStealth |= FROMOUTSIDE;
          pline(msg, "Stealth");
        } else {
          if (!(HProtection & INTRINSIC)) {
            HProtection |= FROMOUTSIDE;
            if (!player.ublessed)
              player.ublessed = rn1(3, 2);
          } else
            player.ublessed++;
          pline(msg, "my protection");
        }
        verbalize("Use it wisely in my name!");
        break;
      }
      case 7:
      case 8:
      case 9: /* KMH -- can occur during full moons */
#ifdef ELBERETH
        if (player.ualign.record >= PIOUS && !player.uevent.uhand_of_elbereth) {
          gcrownu();
          break;
        } /* else FALLTHRU */
#endif    /*ELBERETH*/
      case 6: {
        Object *otmp;
        int sp_no, trycnt = player.ulevel + 1;

        at_your_feet("An object");
        /* not yet known spells given preference over already known ones */
        /* Also, try to grant a spell for which there is a skill slot */
        otmp = MakeRandomObject(SPBOOK_CLASS, TRUE);
        while (--trycnt > 0) {
          if (otmp->otyp != SPE_BLANK_PAPER) {
            for (sp_no = 0; sp_no < MAXSPELL; sp_no++)
              if (spl_book[sp_no].sp_id == otmp->otyp)
                break;
            if (sp_no == MAXSPELL && !P_RESTRICTED(spell_skilltype(otmp->otyp)))
              break; /* usable, but not yet known */
          } else {
            if (!objects[SPE_BLANK_PAPER].oc_name_known ||
                carrying(MAGIC_MARKER))
              break;
          }
          otmp->otyp = rnd_class(bases[SPBOOK_CLASS], SPE_BLANK_PAPER);
        }
        Bless(otmp);
        PlaceObject(otmp, player.ux, player.uy);
        break;
      }
      default:
        impossible("Confused deity!");
        break;
    }

  player.ublesscnt = rnz(350);
  kick_on_butt = player.uevent.udemigod ? 1 : 0;
#ifdef ELBERETH
  if (player.uevent.uhand_of_elbereth)
    kick_on_butt++;
#endif
  if (kick_on_butt)
    player.ublesscnt += kick_on_butt * rnz(1000);

  return;
}

/* either blesses or curses water on the altar,
 * returns true if it found any water here.
 */
STATIC_OVL bool water_prayer(bool bless_water) {
  Object *otmp;
  long changed = 0;
  bool other = FALSE, bc_known = !(Blind || Hallucination);

  for (otmp = level.objects[player.ux][player.uy]; otmp;
       otmp = otmp->nexthere) {
    /* turn water into (un)holy water */
    if (otmp->otyp == POT_WATER &&
        (bless_water ? !otmp->blessed : !otmp->cursed)) {
      otmp->blessed = bless_water;
      otmp->cursed = !bless_water;
      otmp->bknown = bc_known;
      changed += otmp->quan;
    } else if (otmp->oclass == POTION_CLASS)
      other = TRUE;
  }
  if (!Blind && changed) {
    pline("%s potion%s on the altar glow%s %s for a moment.",
          ((other && changed > 1L) ? "Some of the"
                                   : (other ? "One of the" : "The")),
          ((other || changed > 1L) ? "s" : ""), (changed > 1L ? "" : "s"),
          (bless_water ? hcolor(NH_LIGHT_BLUE) : hcolor(NH_BLACK)));
  }
  return ((bool)(changed > 0L));
}

STATIC_OVL void godvoice(aligntyp g_align, const char *words) {
  const char *quot = "";
  if (words)
    quot = "\"";
  else
    words = "";

  pline_The("voice of %s %s: %s%s%s", align_gname(g_align),
            godvoices[rn2(SIZE(godvoices))], quot, words, quot);
}

STATIC_OVL void gods_angry(aligntyp g_align) {
  godvoice(g_align, "Thou hast angered me.");
}

/* The g_align god is upset with you. */
STATIC_OVL void gods_upset(aligntyp g_align) {
  if (g_align == player.ualign.type)
    player.ugangr++;
  else if (player.ugangr)
    player.ugangr--;
  angrygods(g_align);
}

static const char sacrifice_types[] = {FOOD_CLASS, AMULET_CLASS, 0};

STATIC_OVL void consume_offering(Object *otmp) {
  if (Hallucination)
    switch (rn2(3)) {
      case 0:
        Your("sacrifice sprouts wings and a propeller and roars away!");
        break;
      case 1:
        Your("sacrifice puffs up, swelling bigger and bigger, and pops!");
        break;
      case 2:
        Your(
            "sacrifice collapses into a cloud of dancing particles and fades "
            "away!");
        break;
    }
  else if (Blind && player.ualign.type == A_LAWFUL)
    Your("sacrifice disappears!");
  else
    Your("sacrifice is consumed in a %s!",
         player.ualign.type == A_LAWFUL ? "flash of light" : "burst of flame");
  if (carried(otmp))
    useup(otmp);
  else
    useupf(otmp, 1L);
  exercise(A_WIS, TRUE);
}

int dosacrifice() {
  Object *otmp;
  int value = 0;
  int pm;
  aligntyp altaralign = a_align(player.ux, player.uy);

  if (!on_altar() || player.uswallow) {
    You("are not standing on an altar.");
    return 0;
  }

  if (In_endgame(&player.uz)) {
    if (!(otmp = getobj(sacrifice_types, "sacrifice")))
      return 0;
  } else {
    if (!(otmp = floorfood("sacrifice", 1)))
      return 0;
  }
/*
  Was based on nutritional value and aging behavior (< 50 moves).
  Sacrificing a food ration got you max luck instantly, making the
  gods as easy to please as an angry dog!

  Now only accepts corpses, based on the game's evaluation of their
  toughness.  Human and pet sacrifice, as well as sacrificing unicorns
  of your alignment, is strongly discouraged.
 */

#define MAXVALUE 24 /* Highest corpse value (besides Wiz) */

  if (otmp->otyp == CORPSE) {
    MonsterType *ptr = &mons[otmp->corpsenm];
    Monster *mtmp;
    extern const int monstr[];

    /* KMH, conduct */
    player.uconduct.gnostic++;

    /* you're handling this corpse, even if it was killed upon the altar */
    feel_cockatrice(otmp, TRUE);

    if (otmp->corpsenm == PM_ACID_BLOB ||
        (monstermoves <= PeekAtIcedCorpseAge(otmp) + 50)) {
      value = monstr[otmp->corpsenm] + 1;
      if (otmp->oeaten)
        value = eaten_stat(value, otmp);
    }

    if (your_race(ptr)) {
      if (is_demon(youmonst.data)) {
        You("find the idea very satisfying.");
        exercise(A_WIS, TRUE);
      } else if (player.ualign.type != A_CHAOTIC) {
        pline("You'll regret this infamous offense!");
        exercise(A_WIS, FALSE);
      }

      if (altaralign != A_CHAOTIC && altaralign != A_NONE) {
        /* curse the lawful/neutral altar */
        pline_The("altar is stained with %s blood.", urace.adj);
        if (!Is_astralevel(&player.uz))
          levl[player.ux][player.uy].altarmask = AM_CHAOTIC;
        angry_priest();
      } else {
        Monster *dmon;
        const char *demonless_msg;

        /* Human sacrifice on a chaotic or unaligned altar */
        /* is equivalent to demon summoning */
        if (altaralign == A_CHAOTIC && player.ualign.type != A_CHAOTIC) {
          pline("The blood floods the altar, which vanishes in %s cloud!",
                an(hcolor(NH_BLACK)));
          levl[player.ux][player.uy].typ = ROOM;
          levl[player.ux][player.uy].altarmask = 0;
          newsym(player.ux, player.uy);
          angry_priest();
          demonless_msg = "cloud dissipates";
        } else {
          /* either you're chaotic or altar is Moloch's or both */
          pline_The("blood covers the altar!");
          change_luck(altaralign == A_NONE ? -2 : 2);
          demonless_msg = "blood coagulates";
        }
        if ((pm = dlord(altaralign)) != NON_PM &&
            (dmon = makemon(&mons[pm], player.ux, player.uy, NO_MM_FLAGS))) {
          You("have summoned %s!", a_monnam(dmon));
          if (sgn(player.ualign.type) == sgn(dmon->data->maligntyp))
            dmon->mpeaceful = TRUE;
          You("are terrified, and unable to move.");
          nomul(-3, "being terrified of a demon");
        } else
          pline_The("%s.", demonless_msg);
      }

      if (player.ualign.type != A_CHAOTIC) {
        adjalign(-5);
        player.ugangr += 3;
        (void)adjattrib(A_WIS, -1, TRUE);
        if (!Inhell)
          angrygods(player.ualign.type);
        change_luck(-5);
      } else
        adjalign(5);
      if (carried(otmp))
        useup(otmp);
      else
        useupf(otmp, 1L);
      return (1);
    } else if (otmp->oxlth && otmp->oattached == OATTACHED_MONST &&
               ((mtmp = NewMonsterFromObject(otmp, FALSE)) != nullptr) &&
               mtmp->mtame) {
      /* mtmp is a temporary pointer to a tame monster's attributes,
       * not a real monster */
      pline("So this is how you repay loyalty?");
      adjalign(-3);
      value = -1;
      HAggravate_monster |= FROMOUTSIDE;
    } else if (is_undead(ptr)) { /* Not demons--no demon corpses */
      if (player.ualign.type != A_CHAOTIC)
        value += 1;
    } else if (is_unicorn(ptr)) {
      int unicalign = sgn(ptr->maligntyp);

      /* If same as altar, always a very bad action. */
      if (unicalign == altaralign) {
        pline(
            "Such an action is an insult to %s!",
            (unicalign == A_CHAOTIC) ? "chaos" : unicalign ? "law" : "balance");
        (void)adjattrib(A_WIS, -1, TRUE);
        value = -5;
      } else if (player.ualign.type == altaralign) {
        /* If different from altar, and altar is same as yours, */
        /* it's a very good action */
        if (player.ualign.record < ALIGNLIM)
          You_feel("appropriately %s.", align_str(player.ualign.type));
        else
          You_feel("you are thoroughly on the right path.");
        adjalign(5);
        value += 3;
      } else
          /* If sacrificing unicorn of your alignment to altar not of */
          /* your alignment, your god gets angry and it's a conversion */
          if (unicalign == player.ualign.type) {
        player.ualign.record = -1;
        value = 1;
      } else
        value += 3;
    }
  } /* corpse */

  if (otmp->otyp == AMULET_OF_YENDOR) {
    if (!Is_astralevel(&player.uz)) {
      if (Hallucination)
        You_feel("homesick.");
      else
        You_feel("an urge to return to the surface.");
      return 1;
    } else {
      /* The final Test.	Did you win? */
      if (uamul == otmp)
        Amulet_off();
      player.uevent.ascended = 1;
      if (carried(otmp))
        useup(otmp); /* well, it's gone now */
      else
        useupf(otmp, 1L);
      You("offer the Amulet of Yendor to %s...", a_gname());
      if (player.ualign.type != altaralign) {
        /* And the opposing team picks you up and
           carries you off on their shoulders */
        adjalign(-99);
        pline("%s accepts your gift, and gains dominion over %s...", a_gname(),
              u_gname());
        pline("%s is enraged...", u_gname());
        pline("Fortunately, %s permits you to live...", a_gname());
        pline("A cloud of %s smoke surrounds you...",
              hcolor((const char *)"orange"));
        done(ESCAPED);
      } else { /* super big win */
        adjalign(10);

#ifdef RECORD_ACHIEVE
        achieve.ascended = 1;
#endif

        pline("An invisible choir sings, and you are bathed in radiance...");
        godvoice(altaralign, "Congratulations, mortal!");
        display_nhwindow(WIN_MESSAGE, FALSE);
        verbalize(
            "In return for thy service, I grant thee the gift of Immortality!");
        You("ascend to the status of Demigod%s...", flags.female ? "dess" : "");
        done(ASCENDED);
      }
    }
  } /* real Amulet */

  if (otmp->otyp == FAKE_AMULET_OF_YENDOR) {
    if (flags.soundok)
      You_hear("a nearby thunderclap.");
    if (!otmp->known) {
      You("realize you have made a %s.", Hallucination ? "boo-boo" : "mistake");
      otmp->known = TRUE;
      change_luck(-1);
      return 1;
    } else {
      /* don't you dare try to fool the gods */
      change_luck(-3);
      adjalign(-1);
      player.ugangr += 3;
      value = -3;
    }
  } /* fake Amulet */

  if (value == 0) {
    pline(nothing_happens);
    return (1);
  }

  if (altaralign != player.ualign.type &&
      (Is_astralevel(&player.uz) || Is_sanctum(&player.uz))) {
    /*
     * REAL BAD NEWS!!! High altars cannot be converted.  Even an attempt
     * gets the god who owns it truely pissed off.
     */
    You_feel("the air around you grow charged...");
    pline("Suddenly, you realize that %s has noticed you...", a_gname());
    godvoice(altaralign, "So, mortal!  You dare desecrate my High Temple!");
    /* Throw everything we have at the player */
    god_zaps_you(altaralign);
  } else if (value < 0) { /* I don't think the gods are gonna like this... */
    gods_upset(altaralign);
  } else {
    int saved_anger = player.ugangr;
    int saved_cnt = player.ublesscnt;
    int saved_luck = player.uluck;

    /* Sacrificing at an altar of a different alignment */
    if (player.ualign.type != altaralign) {
      /* Is this a conversion ? */
      /* An unaligned altar in Gehennom will always elicit rejection. */
      if (ugod_is_angry() || (altaralign == A_NONE && Inhell)) {
        if (player.ualignbase[A_CURRENT] == player.ualignbase[A_ORIGINAL] &&
            altaralign != A_NONE) {
          You("have a strong feeling that %s is angry...", u_gname());
          consume_offering(otmp);
          pline("%s accepts your allegiance.", a_gname());

          /* The player wears a helm of opposite alignment? */
          if (uarmh && uarmh->otyp == HELM_OF_OPPOSITE_ALIGNMENT)
            player.ualignbase[A_CURRENT] = altaralign;
          else
            player.ualign.type = player.ualignbase[A_CURRENT] = altaralign;
          player.ublessed = 0;
          flags.botl = 1;

          You("have a sudden sense of a new direction.");
          /* Beware, Conversion is costly */
          change_luck(-3);
          player.ublesscnt += 300;
          adjalign((int)(player.ualignbase[A_ORIGINAL] * (ALIGNLIM / 2)));
        } else {
          player.ugangr += 3;
          adjalign(-5);
          pline("%s rejects your sacrifice!", a_gname());
          godvoice(altaralign, "Suffer, infidel!");
          change_luck(-5);
          (void)adjattrib(A_WIS, -2, TRUE);
          if (!Inhell)
            angrygods(player.ualign.type);
        }
        return (1);
      } else {
        consume_offering(otmp);
        You("sense a conflict between %s and %s.", u_gname(), a_gname());
        if (rn2(8 + player.ulevel) > 5) {
          Monster *pri;
          You_feel("the power of %s increase.", u_gname());
          exercise(A_WIS, TRUE);
          change_luck(1);
          /* Yes, this is supposed to be &=, not |= */
          levl[player.ux][player.uy].altarmask &= AM_SHRINE;
          /* the following accommodates stupid compilers */
          levl[player.ux][player.uy].altarmask =
              levl[player.ux][player.uy].altarmask |
              (Align2amask(player.ualign.type));
          if (!Blind)
            pline_The("altar glows %s.",
                      hcolor(player.ualign.type == A_LAWFUL
                                 ? NH_WHITE
                                 : player.ualign.type ? NH_BLACK
                                                      : (const char *)"gray"));

          if (rnl(player.ulevel) > 6 && player.ualign.record > 0 &&
              rnd(player.ualign.record) > (3 * ALIGNLIM) / 4)
            summon_minion(altaralign, TRUE);
          /* anger priest; test handles bones files */
          if ((pri = findpriest(temple_occupied(player.urooms))) &&
              !p_coaligned(pri))
            angry_priest();
        } else {
          pline("Unluckily, you feel the power of %s decrease.", u_gname());
          change_luck(-1);
          exercise(A_WIS, FALSE);
          if (rnl(player.ulevel) > 6 && player.ualign.record > 0 &&
              rnd(player.ualign.record) > (7 * ALIGNLIM) / 8)
            summon_minion(altaralign, TRUE);
        }
        return (1);
      }
    }

    consume_offering(otmp);
    /* OK, you get brownie points. */
    if (player.ugangr) {
      player.ugangr -=
          ((value * (player.ualign.type == A_CHAOTIC ? 2 : 3)) / MAXVALUE);
      if (player.ugangr < 0)
        player.ugangr = 0;
      if (player.ugangr != saved_anger) {
        if (player.ugangr) {
          pline("%s seems %s.", u_gname(),
                Hallucination ? "groovy" : "slightly mollified");

          if ((int)player.uluck < 0)
            change_luck(1);
        } else {
          pline("%s seems %s.", u_gname(),
                Hallucination ? "cosmic (not a new fact)" : "mollified");

          if ((int)player.uluck < 0)
            player.uluck = 0;
        }
      } else { /* not satisfied yet */
        if (Hallucination)
          pline_The("gods seem tall.");
        else
          You("have a feeling of inadequacy.");
      }
    } else if (ugod_is_angry()) {
      if (value > MAXVALUE)
        value = MAXVALUE;
      if (value > -player.ualign.record)
        value = -player.ualign.record;
      adjalign(value);
      You_feel("partially absolved.");
    } else if (player.ublesscnt > 0) {
      player.ublesscnt -=
          ((value * (player.ualign.type == A_CHAOTIC ? 500 : 300)) / MAXVALUE);
      if (player.ublesscnt < 0)
        player.ublesscnt = 0;
      if (player.ublesscnt != saved_cnt) {
        if (player.ublesscnt) {
          if (Hallucination)
            You("realize that the gods are not like you and I.");
          else
            You("have a hopeful feeling.");
          if ((int)player.uluck < 0)
            change_luck(1);
        } else {
          if (Hallucination)
            pline("Overall, there is a smell of fried onions.");
          else
            You("have a feeling of reconciliation.");
          if ((int)player.uluck < 0)
            player.uluck = 0;
        }
      }
    } else {
      int nartifacts = nartifact_exist();

      /* you were already in pretty good standing */
      /* The player can gain an artifact */
      /* The chance goes down as the number of artifacts goes up */
      if (player.ulevel > 2 && player.uluck >= 0 &&
          !rn2(10 + (2 * player.ugifts * nartifacts))) {
        otmp = mk_artifact(nullptr, a_align(player.ux, player.uy));
        if (otmp) {
          if (otmp->spe < 0)
            otmp->spe = 0;
          if (otmp->cursed)
            Uncurse(otmp);
          otmp->oerodeproof = TRUE;
          dropy(otmp);
          at_your_feet("An object");
          godvoice(player.ualign.type, "Use my gift wisely!");
          player.ugifts++;
          player.ublesscnt = rnz(300 + (50 * nartifacts));
          exercise(A_WIS, TRUE);
          /* make sure we can use this weapon */
          unrestrict_weapon_skill(weapon_type(otmp));
          discover_artifact(otmp->oartifact);
          return (1);
        }
      }
      change_luck((value * LUCKMAX) / (MAXVALUE * 2));
      if ((int)player.uluck < 0)
        player.uluck = 0;
      if (player.uluck != saved_luck) {
        if (Blind)
          You("think %s brushed your %s.", something, body_part(FOOT));
        else
          You(Hallucination
                  ? "see crabgrass at your %s.  A funny thing in a dungeon."
                  : "glimpse a four-leaf clover at your %s.",
              makeplural(body_part(FOOT)));
      }
    }
  }
  return (1);
}

/* determine prayer results in advance; also used for enlightenment */
bool can_pray(bool praying) {
  int alignment;

  p_aligntyp = on_altar() ? a_align(player.ux, player.uy) : player.ualign.type;
  p_trouble = in_trouble();

  if (is_demon(youmonst.data) && (p_aligntyp != A_CHAOTIC)) {
    if (praying)
      pline_The("very idea of praying to a %s god is repugnant to you.",
                p_aligntyp ? "lawful" : "neutral");
    return FALSE;
  }

  if (praying)
    You("begin praying to %s.", align_gname(p_aligntyp));

  if (player.ualign.type && player.ualign.type == -p_aligntyp)
    alignment = -player.ualign.record; /* Opposite alignment altar */
  else if (player.ualign.type != p_aligntyp)
    alignment = player.ualign.record / 2; /* Different alignment altar */
  else
    alignment = player.ualign.record;

  if ((p_trouble > 0) ? (player.ublesscnt > 200) :     /* big trouble */
          (p_trouble < 0) ? (player.ublesscnt > 100) : /* minor difficulties */
              (player.ublesscnt > 0))                  /* not in trouble */
    p_type = 0;                                        /* too soon... */
  else if ((int)Luck < 0 || player.ugangr || alignment < 0)
    p_type = 1; /* too naughty... */
  else /* alignment >= 0 */ {
    if (on_altar() && player.ualign.type != p_aligntyp)
      p_type = 2;
    else
      p_type = 3;
  }

  if (is_undead(youmonst.data) && !Inhell &&
      (p_aligntyp == A_LAWFUL || (p_aligntyp == A_NEUTRAL && !rn2(10))))
    p_type = -1;
  /* Note:  when !praying, the random factor for neutrals makes the
     return value a non-deterministic approximation for enlightenment.
     This case should be uncommon enough to live with... */

  return !praying ? (bool)(p_type == 3 && !Inhell) : TRUE;
}

int dopray() {
  /* Confirm accidental slips of Alt-P */
  if (flags.prayconfirm)
    if (yn("Are you sure you want to pray?") == 'n')
      return 0;

  player.uconduct.gnostic++;
  /* Praying implies that the hero is conscious and since we have
     no deafness attribute this implies that all verbalized messages
     can be heard.  So, in case the player has used the 'O' command
     to toggle this accessible flag off, force it to be on. */
  flags.soundok = 1;

  /* set up p_type and p_alignment */
  if (!can_pray(TRUE))
    return 0;

#ifdef WIZARD
  if (wizard && p_type >= 0) {
    if (yn("Force the gods to be pleased?") == 'y') {
      player.ublesscnt = 0;
      if (player.uluck < 0)
        player.uluck = 0;
      if (player.ualign.record <= 0)
        player.ualign.record = 1;
      player.ugangr = 0;
      if (p_type < 2)
        p_type = 3;
    }
  }
#endif
  nomul(-3, "praying");
  nomovemsg = "You finish your prayer.";
  afternmv = prayer_done;

  if (p_type == 3 && !Inhell) {
    /* if you've been true to your god you can't die while you pray */
    if (!Blind)
      You("are surrounded by a shimmering light.");
    player.uinvulnerable = TRUE;
  }

  return (1);
}

STATIC_PTR int prayer_done() /* M. Stephenson (1.0.3b) */
{
  aligntyp alignment = p_aligntyp;

  player.uinvulnerable = FALSE;
  if (p_type == -1) {
    godvoice(alignment, alignment == A_LAWFUL
                            ? "Vile creature, thou durst call upon me?"
                            : "Walk no more, perversion of nature!");
    You_feel("like you are falling apart.");
    /* KMH -- Gods have mastery over unchanging */
    rehumanize();
    losehp(rnd(20), "residual undead turning effect", KILLED_BY_AN);
    exercise(A_CON, FALSE);
    return (1);
  }
  if (Inhell) {
    pline("Since you are in Gehennom, %s won't help you.",
          align_gname(alignment));
    /* haltingly aligned is least likely to anger */
    if (player.ualign.record <= 0 || rnl(player.ualign.record))
      angrygods(player.ualign.type);
    return (0);
  }

  if (p_type == 0) {
    if (on_altar() && player.ualign.type != alignment)
      (void)water_prayer(FALSE);
    player.ublesscnt += rnz(250);
    change_luck(-3);
    gods_upset(player.ualign.type);
  } else if (p_type == 1) {
    if (on_altar() && player.ualign.type != alignment)
      (void)water_prayer(FALSE);
    angrygods(player.ualign.type); /* naughty */
  } else if (p_type == 2) {
    if (water_prayer(FALSE)) {
      /* attempted water prayer on a non-coaligned altar */
      player.ublesscnt += rnz(250);
      change_luck(-3);
      gods_upset(player.ualign.type);
    } else
      pleased(alignment);
  } else {
    /* coaligned */
    if (on_altar())
      (void)water_prayer(TRUE);
    pleased(alignment); /* nice */
  }
  return (1);
}

int doturn() {
  Monster *mtmp, *mtmp2;
  int once, range, xlev;

  if (!Role_if(PM_PRIEST) && !Role_if(PM_KNIGHT)) {
    /* Try to use turn undead spell. */
    if (objects[SPE_TURN_UNDEAD].oc_name_known) {
      int sp_no;
      for (sp_no = 0; sp_no < MAXSPELL && spl_book[sp_no].sp_id != NO_SPELL &&
                          spl_book[sp_no].sp_id != SPE_TURN_UNDEAD;
           sp_no++)
        ;

      if (sp_no < MAXSPELL && spl_book[sp_no].sp_id == SPE_TURN_UNDEAD)
        return spelleffects(sp_no, TRUE);
    }

    You("don't know how to turn undead!");
    return (0);
  }
  player.uconduct.gnostic++;

  if ((player.ualign.type != A_CHAOTIC &&
       (is_demon(youmonst.data) || is_undead(youmonst.data))) ||
      player.ugangr > 6 /* "Die, mortal!" */) {
    pline("For some reason, %s seems to ignore you.", u_gname());
    aggravate();
    exercise(A_WIS, FALSE);
    return (0);
  }

  if (Inhell) {
    pline("Since you are in Gehennom, %s won't help you.", u_gname());
    aggravate();
    return (0);
  }
  pline("Calling upon %s, you chant an arcane formula.", u_gname());
  exercise(A_WIS, TRUE);

  /* note: does not perform unturn_dead() on victims' inventories */
  range = BOLT_LIM + (player.ulevel / 5); /* 5 to 11 */
  range *= range;
  once = 0;
  for (mtmp = fmon; mtmp; mtmp = mtmp2) {
    mtmp2 = mtmp->nmon;

    if (mtmp->dead())
      continue;
    if (!cansee(mtmp->mx, mtmp->my) || distu(mtmp->mx, mtmp->my) > range)
      continue;

    if (!mtmp->mpeaceful &&
        (is_undead(mtmp->data) ||
         (is_demon(mtmp->data) && (player.ulevel > (MAXULEV / 2))))) {
      mtmp->msleeping = 0;
      if (Confusion) {
        if (!once++)
          pline("Unfortunately, your voice falters.");
        mtmp->mflee = 0;
        mtmp->mfrozen = 0;
        mtmp->mcanmove = 1;
      } else if (!resist(mtmp, '\0', 0, TELL)) {
        xlev = 6;
        switch (mtmp->data->mlet) {
          /* this is intentional, lichs are tougher
             than zombies. */
          case S_LICH:
            xlev += 2; /*FALLTHRU*/
          case S_GHOST:
            xlev += 2; /*FALLTHRU*/
          case S_VAMPIRE:
            xlev += 2; /*FALLTHRU*/
          case S_WRAITH:
            xlev += 2; /*FALLTHRU*/
          case S_MUMMY:
            xlev += 2; /*FALLTHRU*/
          case S_ZOMBIE:
            if (player.ulevel >= xlev && !resist(mtmp, '\0', 0, NOTELL)) {
              if (player.ualign.type == A_CHAOTIC) {
                mtmp->mpeaceful = 1;
                set_malign(mtmp);
              } else { /* damn them */
                killed(mtmp);
              }
              break;
            } /* else flee */
              /*FALLTHRU*/
          default:
            monflee(mtmp, 0, FALSE, TRUE);
            break;
        }
      }
    }
  }
  nomul(-5, "trying to turn the monsters");
  return (1);
}

const char *a_gname() { return (a_gname_at(player.ux, player.uy)); }

/* returns the name of an altar's deity */
const char *a_gname_at(xchar x, xchar y) {
  if (!IS_ALTAR(levl[x][y].typ))
    return (nullptr);

  return align_gname(a_align(x, y));
}

/* returns the name of the player's deity */
const char *u_gname() { return align_gname(player.ualign.type); }

const char *align_gname(aligntyp alignment) {
  const char *gnam;

  switch (alignment) {
    case A_NONE:
      gnam = Moloch;
      break;
    case A_LAWFUL:
      gnam = urole.lgod;
      break;
    case A_NEUTRAL:
      gnam = urole.ngod;
      break;
    case A_CHAOTIC:
      gnam = urole.cgod;
      break;
    default:
      impossible("unknown alignment.");
      gnam = "someone";
      break;
  }
  if (*gnam == '_')
    ++gnam;
  return gnam;
}

/* hallucination handling for priest/minion names: select a random god
   iff character is hallucinating */
const char *halu_gname(aligntyp alignment) {
  const char *gnam;
  int which;

  if (!Hallucination)
    return align_gname(alignment);

  which = randrole();
  switch (rn2(3)) {
    case 0:
      gnam = roles[which].lgod;
      break;
    case 1:
      gnam = roles[which].ngod;
      break;
    case 2:
      gnam = roles[which].cgod;
      break;
    default:
      gnam = 0;
      break; /* lint suppression */
  }
  if (!gnam)
    gnam = Moloch;
  if (*gnam == '_')
    ++gnam;
  return gnam;
}

/* deity's title */
const char *align_gtitle(aligntyp alignment) {
  const char *gnam, *result = "god";

  switch (alignment) {
    case A_LAWFUL:
      gnam = urole.lgod;
      break;
    case A_NEUTRAL:
      gnam = urole.ngod;
      break;
    case A_CHAOTIC:
      gnam = urole.cgod;
      break;
    default:
      gnam = 0;
      break;
  }
  if (gnam && *gnam == '_')
    result = "goddess";
  return result;
}

void altar_wrath(int x, int y) {
  aligntyp altaralign = a_align(x, y);

  if (!strcmp(align_gname(altaralign), u_gname())) {
    godvoice(altaralign, "How darest thou desecrate my altar!");
    (void)adjattrib(A_WIS, -1, FALSE);
  } else {
    pline("A voice (could it be %s?) whispers:", align_gname(altaralign));
    verbalize("Thou shalt pay, infidel!");
    change_luck(-1);
  }
}

/* assumes isok() at one space away, but not necessarily at two */
STATIC_OVL bool blocked_boulder(int dx, int dy) {
  Object *otmp;
  long count = 0L;

  for (otmp = level.objects[player.ux + dx][player.uy + dy]; otmp;
       otmp = otmp->nexthere) {
    if (otmp->otyp == BOULDER)
      count += otmp->quan;
  }

  switch (count) {
    case 0:
      return FALSE; /* no boulders--not blocked */
    case 1:
      break; /* possibly blocked depending on if it's pushable */
    default:
      return TRUE; /* >1 boulder--blocked after they push the top
  one; don't force them to push it first to find out */
  }

  if (!isok(player.ux + 2 * dx, player.uy + 2 * dy))
    return TRUE;
  if (IS_ROCK(levl[player.ux + 2 * dx][player.uy + 2 * dy].typ))
    return TRUE;
  if (sobj_at(BOULDER, player.ux + 2 * dx, player.uy + 2 * dy))
    return TRUE;

  return FALSE;
}

/*pray.c*/
