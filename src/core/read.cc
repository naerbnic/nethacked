/*	SCCS Id: @(#)read.c	3.4	2003/10/22	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include <string.h>

#include "core/hack.h"
#include "core/zap.h"
#include "core/worn.h"
#include "core/worm.h"
#include "core/wield.h"
#include "core/weapon.h"
#include "core/teleport.h"
#include "core/shk.h"
#include "core/rumors.h"
#include "core/rnd.h"
#include "core/read.h"
#include "core/questpgr.h"
#include "core/potion.h"
#include "core/polyself.h"
#include "core/pline.h"
#include "core/objnam.h"
#include "core/o_init.h"
#include "core/monmove.h"
#include "core/mon.h"
#include "core/mkobj.h"
#include "core/makemon.h"
#include "core/mail.h"
#include "core/light.h"
#include "core/invent.h"
#include "core/hacklib.h"
#include "core/explode.h"
#include "core/exper.h"
#include "core/end.h"
#include "core/drawing.h"
#include "core/dog.h"
#include "core/do_wear.h"
#include "core/do_name.h"
#include "core/do.h"
#include "core/detect.h"
#include "core/cmd.h"

#include "core/apply.h"
#include "core/artifact.h"
#include "core/ball.h"

/* KMH -- Copied from pray.c; this really belongs in a header file */
#define DEVOUT 14
#define STRIDENT 4

#define Your_Own_Role(mndx)   \
  ((mndx) == urole.malenum || \
   (urole.femalenum != NON_PM && (mndx) == urole.femalenum))
#define Your_Own_Race(mndx)   \
  ((mndx) == urace.malenum || \
   (urace.femalenum != NON_PM && (mndx) == urace.femalenum))

#ifdef OVLB

bool known;

static const char readable[] = {ALL_CLASSES, SCROLL_CLASS, SPBOOK_CLASS, 0};
static const char all_count[] = {ALLOW_COUNT, ALL_CLASSES, 0};

static void wand_explode(Object *);
static void do_class_genocide();
static void stripspe(Object *);
static void p_glow1(Object *);
static void p_glow2(Object *, const char *);
static void randomize(int *, int);
static void forget_single_object(int);
static void forget(int);
static void maybe_tame(Monster *, Object *);

STATIC_PTR void set_lit(int, int, genericptr_t);

int doread() {
  Object *scroll;
  bool confused;

  known = FALSE;
  if (check_capacity(nullptr))
    return (0);
  scroll = getobj(readable, "read");
  if (!scroll)
    return (0);

  /* outrumor has its own blindness check */
  if (scroll->otyp == FORTUNE_COOKIE) {
    if (flags.verbose)
      You("break up the cookie and throw away the pieces.");
    outrumor(GetBUCSign(scroll), BY_COOKIE);
    if (!Blind)
      player.uconduct.literate++;
    useup(scroll);
    return (1);
#ifdef TOURIST
  } else if (scroll->otyp == T_SHIRT) {
    static const char *shirt_msgs[] = {
        /* Scott Bigham */
        "I explored the Dungeons of Doom and all I got was this lousy T-shirt!",
        "Is that Mjollnir in your pocket or are you just happy to see me?",
        "It's not the size of your sword, it's how #enhance'd you are with it.",
        "Madame Elvira's House O' Succubi Lifetime Customer",
        "Madame Elvira's House O' Succubi Employee of the Month",
        "Ludios Vault Guards Do It In Small, Dark Rooms",
        "Yendor Military Soldiers Do It In Large Groups",
        "I survived Yendor Military Boot Camp",
        "Ludios Accounting School Intra-Mural Lacrosse Team",
        "Oracle(TM) Fountains 10th Annual Wet T-Shirt Contest",
        "Hey, black dragon!  Disintegrate THIS!",
        "I'm With Stupid -->",
        "Don't blame me, I voted for Izchak!",
        "Don't Panic",                         /* HHGTTG */
        "Furinkan High School Athletic Dept.", /* Ranma 1/2 */
        "Hel-LOOO, Nurse!",                    /* Animaniacs */
    };
    char buf[BUFSZ];
    int erosion;

    if (Blind) {
      You_cant("feel any Braille writing.");
      return 0;
    }
    player.uconduct.literate++;
    if (flags.verbose)
      pline("It reads:");
    strcpy(buf, shirt_msgs[scroll->o_id % SIZE(shirt_msgs)]);
    erosion = greatest_erosion(scroll);
    if (erosion)
      wipeout_text(buf, (int)(strlen(buf) * erosion / (2 * MAX_ERODE)),
                   scroll->o_id ^ (unsigned)player.ubirthday);
    pline("\"%s\"", buf);
    return 1;
#endif /* TOURIST */
  } else if (scroll->oclass != SCROLL_CLASS && scroll->oclass != SPBOOK_CLASS) {
    pline(silly_thing_to, "read");
    return (0);
  } else if (Blind) {
    const char *what = 0;
    if (scroll->oclass == SPBOOK_CLASS)
      what = "mystic runes";
    else if (!scroll->dknown)
      what = "formula on the scroll";
    if (what) {
      pline("Being blind, you cannot read the %s.", what);
      return (0);
    }
  }

  /* Actions required to win the game aren't counted towards conduct */
  if (scroll->otyp != SPE_BOOK_OF_THE_DEAD && scroll->otyp != SPE_BLANK_PAPER &&
      scroll->otyp != SCR_BLANK_PAPER)
    player.uconduct.literate++;

  confused = (Confusion != 0);
#ifdef MAIL
  if (scroll->otyp == SCR_MAIL)
    confused = FALSE;
#endif
  if (scroll->oclass == SPBOOK_CLASS) {
    return (study_book(scroll));
  }
  scroll->in_use = TRUE; /* scroll, not spellbook, now being read */
  if (scroll->otyp != SCR_BLANK_PAPER) {
    if (Blind)
      pline("As you %s the formula on it, the scroll disappears.",
            is_silent(youmonst.data) ? "cogitate" : "pronounce");
    else
      pline("As you read the scroll, it disappears.");
    if (confused) {
      if (Hallucination)
        pline("Being so trippy, you screw up...");
      else
        pline("Being confused, you mis%s the magic words...",
              is_silent(youmonst.data) ? "understand" : "pronounce");
    }
  }
  if (!seffects(scroll)) {
    if (!objects[scroll->otyp].oc_name_known) {
      if (known) {
        makeknown(scroll->otyp);
        more_experienced(0, 10);
      } else if (!objects[scroll->otyp].oc_uname)
        docall(scroll);
    }
    if (scroll->otyp != SCR_BLANK_PAPER)
      useup(scroll);
    else
      scroll->in_use = FALSE;
  }
  return (1);
}

static void stripspe(Object *obj) {
  if (obj->blessed)
    pline(nothing_happens);
  else {
    if (obj->spe > 0) {
      obj->spe = 0;
      if (obj->otyp == OIL_LAMP || obj->otyp == BRASS_LANTERN)
        obj->age = 0;
      Your("%s %s briefly.", xname(obj), otense(obj, "vibrate"));
    } else
      pline(nothing_happens);
  }
}

static void p_glow1(Object *otmp) {
  Your("%s %s briefly.", xname(otmp), otense(otmp, Blind ? "vibrate" : "glow"));
}

static void p_glow2(Object *otmp, const char *color) {
  Your("%s %s%s%s for a moment.", xname(otmp),
       otense(otmp, Blind ? "vibrate" : "glow"), Blind ? "" : " ",
       Blind ? nul : hcolor(color));
}

/* Is the object chargeable?  For purposes of inventory display; it is */
/* possible to be able to charge things for which this returns FALSE. */
bool is_chargeable(Object *obj) {
  if (obj->oclass == WAND_CLASS)
    return TRUE;
  /* known && !uname is possible after amnesia/mind flayer */
  if (obj->oclass == RING_CLASS)
    return (bool)(objects[obj->otyp].oc_charged &&
                  (obj->known || objects[obj->otyp].oc_uname));
  if (is_weptool(obj)) /* specific check before general tools */
    return FALSE;
  if (obj->oclass == TOOL_CLASS)
    return (bool)(objects[obj->otyp].oc_charged);
  return FALSE; /* why are weapons/armor considered charged anyway? */
}

/*
 * recharge an object; curse_bless is -1 if the recharging implement
 * was cursed, +1 if blessed, 0 otherwise.
 */
void recharge(Object *obj, int curse_bless) {
  int n;
  bool is_cursed, is_blessed;

  is_cursed = curse_bless < 0;
  is_blessed = curse_bless > 0;

  if (obj->oclass == WAND_CLASS) {
    /* undo any prior cancellation, even when is_cursed */
    if (obj->spe == -1)
      obj->spe = 0;

    /*
     * Recharging might cause wands to explode.
     *	v = number of previous recharges
     *	      v = percentage chance to explode on this attempt
     *		      v = cumulative odds for exploding
     *	0 :   0       0
     *	1 :   0.29    0.29
     *	2 :   2.33    2.62
     *	3 :   7.87   10.28
     *	4 :  18.66   27.02
     *	5 :  36.44   53.62
     *	6 :  62.97   82.83
     *	7 : 100     100
     */
    n = (int)obj->recharged;
    if (n > 0 && (obj->otyp == WAN_WISHING ||
                  (n * n * n > rn2(7 * 7 * 7)))) { /* recharge_limit */
      wand_explode(obj);
      return;
    }
    /* didn't explode, so increment the recharge count */
    obj->recharged = (unsigned)(n + 1);

    /* now handle the actual recharging */
    if (is_cursed) {
      stripspe(obj);
    } else {
      int lim = (obj->otyp == WAN_WISHING)
                    ? 3
                    : (objects[obj->otyp].oc_dir != NODIR) ? 8 : 15;

      n = (lim == 3) ? 3 : rn1(5, lim + 1 - 5);
      if (!is_blessed)
        n = rnd(n);

      if (obj->spe < n)
        obj->spe = n;
      else
        obj->spe++;
      if (obj->otyp == WAN_WISHING && obj->spe > 3) {
        wand_explode(obj);
        return;
      }
      if (obj->spe >= lim)
        p_glow2(obj, NH_BLUE);
      else
        p_glow1(obj);
    }

  } else if (obj->oclass == RING_CLASS && objects[obj->otyp].oc_charged) {
    /* charging does not affect ring's curse/bless status */
    int s = is_blessed ? rnd(3) : is_cursed ? -rnd(2) : 1;
    bool is_on = (obj == uleft || obj == uright);

    /* destruction depends on current state, not adjustment */
    if (obj->spe > rn2(7) || obj->spe <= -5) {
      Your("%s %s momentarily, then %s!", xname(obj), otense(obj, "pulsate"),
           otense(obj, "explode"));
      if (is_on)
        Ring_gone(obj);
      s = rnd(3 * abs(obj->spe)); /* amount of damage */
      useup(obj);
      losehp(s, "exploding ring", KILLED_BY_AN);
    } else {
      long mask = is_on ? (obj == uleft ? LEFT_RING : RIGHT_RING) : 0L;
      Your("%s spins %sclockwise for a moment.", xname(obj),
           s < 0 ? "counter" : "");
      /* cause attributes and/or properties to be updated */
      if (is_on)
        Ring_off(obj);
      obj->spe += s; /* update the ring while it's off */
      if (is_on)
        setworn(obj, mask), Ring_on(obj);
      /* oartifact: if a touch-sensitive artifact ring is
         ever created the above will need to be revised  */
    }

  } else if (obj->oclass == TOOL_CLASS) {
    int rechrg = (int)obj->recharged;

    if (objects[obj->otyp].oc_charged) {
      /* tools don't have a limit, but the counter used does */
      if (rechrg < 7) /* recharge_limit */
        obj->recharged++;
    }
    switch (obj->otyp) {
      case BELL_OF_OPENING:
        if (is_cursed)
          stripspe(obj);
        else if (is_blessed)
          obj->spe += rnd(3);
        else
          obj->spe += 1;
        if (obj->spe > 5)
          obj->spe = 5;
        break;
      case MAGIC_MARKER:
      case TINNING_KIT:
#ifdef TOURIST
      case EXPENSIVE_CAMERA:
#endif
        if (is_cursed)
          stripspe(obj);
        else if (rechrg &&
                 obj->otyp == MAGIC_MARKER) { /* previously recharged */
          obj->recharged = 1; /* override increment done above */
          if (obj->spe < 3)
            Your("marker seems permanently dried out.");
          else
            pline(nothing_happens);
        } else if (is_blessed) {
          n = rn1(16, 15); /* 15..30 */
          if (obj->spe + n <= 50)
            obj->spe = 50;
          else if (obj->spe + n <= 75)
            obj->spe = 75;
          else {
            int chrg = (int)obj->spe;
            if ((chrg + n) > 127)
              obj->spe = 127;
            else
              obj->spe += n;
          }
          p_glow2(obj, NH_BLUE);
        } else {
          n = rn1(11, 10); /* 10..20 */
          if (obj->spe + n <= 50)
            obj->spe = 50;
          else {
            int chrg = (int)obj->spe;
            if ((chrg + n) > 127)
              obj->spe = 127;
            else
              obj->spe += n;
          }
          p_glow2(obj, NH_WHITE);
        }
        break;
      case OIL_LAMP:
      case BRASS_LANTERN:
        if (is_cursed) {
          stripspe(obj);
          if (obj->lamplit) {
            if (!Blind)
              pline("%s out!", Tobjnam(obj, "go"));
            end_burn(obj, TRUE);
          }
        } else if (is_blessed) {
          obj->spe = 1;
          obj->age = 1500;
          p_glow2(obj, NH_BLUE);
        } else {
          obj->spe = 1;
          obj->age += 750;
          if (obj->age > 1500)
            obj->age = 1500;
          p_glow1(obj);
        }
        break;
      case CRYSTAL_BALL:
        if (is_cursed)
          stripspe(obj);
        else if (is_blessed) {
          obj->spe = 6;
          p_glow2(obj, NH_BLUE);
        } else {
          if (obj->spe < 5) {
            obj->spe++;
            p_glow1(obj);
          } else
            pline(nothing_happens);
        }
        break;
      case HORN_OF_PLENTY:
      case BAG_OF_TRICKS:
      case CAN_OF_GREASE:
        if (is_cursed)
          stripspe(obj);
        else if (is_blessed) {
          if (obj->spe <= 10)
            obj->spe += rn1(10, 6);
          else
            obj->spe += rn1(5, 6);
          if (obj->spe > 50)
            obj->spe = 50;
          p_glow2(obj, NH_BLUE);
        } else {
          obj->spe += rnd(5);
          if (obj->spe > 50)
            obj->spe = 50;
          p_glow1(obj);
        }
        break;
      case MAGIC_FLUTE:
      case MAGIC_HARP:
      case FROST_HORN:
      case FIRE_HORN:
      case DRUM_OF_EARTHQUAKE:
        if (is_cursed) {
          stripspe(obj);
        } else if (is_blessed) {
          obj->spe += d(2, 4);
          if (obj->spe > 20)
            obj->spe = 20;
          p_glow2(obj, NH_BLUE);
        } else {
          obj->spe += rnd(4);
          if (obj->spe > 20)
            obj->spe = 20;
          p_glow1(obj);
        }
        break;
      default:
        goto not_chargable;
        /*NOTREACHED*/
        break;
    } /* switch */

  } else {
  not_chargable:
    You("have a feeling of loss.");
  }
}

/* Forget known information about this object class. */
static void forget_single_object(int obj_id) {
  objects[obj_id].oc_name_known = 0;
  objects[obj_id].oc_pre_discovered = 0; /* a discovery when relearned */
  if (objects[obj_id].oc_uname) {
    free((genericptr_t)objects[obj_id].oc_uname);
    objects[obj_id].oc_uname = 0;
  }
  undiscover_object(obj_id); /* after clearing oc_name_known */

  /* clear & free object names from matching inventory items too? */
}

#if 0 /* here if anyone wants it.... */
/* Forget everything known about a particular object class. */
static void forget_objclass(int oclass) {
	int i;

	for (i=bases[oclass];
		i < NUM_OBJECTS && objects[i].oc_class==oclass; i++)
	    forget_single_object(i);
}
#endif

/* randomize the given list of numbers  0 <= i < count */
static void randomize(int *indices, int count) {
  int i, iswap, temp;

  for (i = count - 1; i > 0; i--) {
    if ((iswap = rn2(i + 1)) == i)
      continue;
    temp = indices[i];
    indices[i] = indices[iswap];
    indices[iswap] = temp;
  }
}

/* Forget % of known objects. */
void forget_objects(int percent) {
  int i, count;
  int indices[NUM_OBJECTS];

  if (percent == 0)
    return;
  if (percent <= 0 || percent > 100) {
    impossible("forget_objects: bad percent %d", percent);
    return;
  }

  for (count = 0, i = 1; i < NUM_OBJECTS; i++)
    if (OBJ_DESCR(objects[i]) &&
        (objects[i].oc_name_known || objects[i].oc_uname))
      indices[count++] = i;

  randomize(indices, count);

  /* forget first % of randomized indices */
  count = ((count * percent) + 50) / 100;
  for (i = 0; i < count; i++)
    forget_single_object(indices[i]);
}

/* Forget some or all of map (depends on parameters). */
void forget_map(int howmuch) {
  int zx, zy;

  if (In_sokoban(&player.uz))
    return;

  known = TRUE;
  for (zx = 0; zx < COLNO; zx++)
    for (zy = 0; zy < ROWNO; zy++)
      if (howmuch & ALL_MAP || rn2(7)) {
        /* Zonk all memory of this location. */
        levl[zx][zy].seenv = 0;
        levl[zx][zy].waslit = 0;
        levl[zx][zy].glyph = cmap_to_glyph(S_stone);
      }
}

/* Forget all traps on the level. */
void forget_traps() {
  Trap *trap;

  /* forget all traps (except the one the hero is in :-) */
  for (trap = ftrap; trap; trap = trap->ntrap)
    if ((trap->tx != player.ux || trap->ty != player.uy) &&
        (trap->ttyp != HOLE))
      trap->tseen = 0;
}

/*
 * Forget given % of all levels that the hero has visited and not forgotten,
 * except this one.
 */
void forget_levels(int percent) {
  int i, count;
  xchar maxl, this_lev;
  int indices[MAXLINFO];

  if (percent == 0)
    return;

  if (percent <= 0 || percent > 100) {
    impossible("forget_levels: bad percent %d", percent);
    return;
  }

  this_lev = ledger_no(&player.uz);
  maxl = maxledgerno();

  /* count & save indices of non-forgotten visited levels */
  /* Sokoban levels are pre-mapped for the player, and should stay
   * so, or they become nearly impossible to solve.  But try to
   * shift the forgetting elsewhere by fiddling with percent
   * instead of forgetting fewer levels.
   */
  for (count = 0, i = 0; i <= maxl; i++)
    if ((level_info[i].flags & VISITED) && !(level_info[i].flags & FORGOTTEN) &&
        i != this_lev) {
      if (ledger_to_dnum(i) == sokoban_dnum)
        percent += 2;
      else
        indices[count++] = i;
    }

  if (percent > 100)
    percent = 100;

  randomize(indices, count);

  /* forget first % of randomized indices */
  count = ((count * percent) + 50) / 100;
  for (i = 0; i < count; i++) {
    level_info[indices[i]].flags |= FORGOTTEN;
  }
}

/*
 * Forget some things (e.g. after reading a scroll of amnesia).  When called,
 * the following are always forgotten:
 *
 *	- felt ball & chain
 *	- traps
 *	- part (6 out of 7) of the map
 *
 * Other things are subject to flags:
 *
 *	howmuch & ALL_MAP	= forget whole map
 *	howmuch & ALL_SPELLS	= forget all spells
 */
static void forget(int howmuch) {
  if (Punished)
    player.bc_felt = 0; /* forget felt ball&chain */

  forget_map(howmuch);
  forget_traps();

  /* 1 in 3 chance of forgetting some levels */
  if (!rn2(3))
    forget_levels(rn2(25));

  /* 1 in 3 chance of forgeting some objects */
  if (!rn2(3))
    forget_objects(rn2(25));

  if (howmuch & ALL_SPELLS)
    losespells();
  /*
   * Make sure that what was seen is restored correctly.  To do this,
   * we need to go blind for an instant --- turn off the display,
   * then restart it.  All this work is needed to correctly handle
   * walls which are stone on one side and wall on the other.  Turning
   * off the seen bits above will make the wall revert to stone,  but
   * there are cases where we don't want this to happen.  The easiest
   * thing to do is to run it through the vision system again, which
   * is always correct.
   */
  docrt(); /* this correctly will reset vision */
}

/* monster is hit by scroll of taming's effect */
static void maybe_tame(Monster *mtmp, Object *sobj) {
  if (sobj->cursed) {
    setmangry(mtmp);
  } else {
    if (mtmp->isshk)
      make_happy_shk(mtmp, FALSE);
    else if (!resist(mtmp, sobj->oclass, 0, NOTELL))
      (void)tamedog(mtmp, nullptr);
  }
}

int seffects(Object *sobj) {
  int cval;
  bool confused = (Confusion != 0);
  Object *otmp;

  if (objects[sobj->otyp].oc_magic)
    exercise(A_WIS, TRUE); /* just for trying */
  switch (sobj->otyp) {
#ifdef MAIL
    case SCR_MAIL:
      known = TRUE;
      if (sobj->spe)
        pline(
            "This seems to be junk mail addressed to the finder of the Eye of "
            "Larn.");
      /* note to the puzzled: the game Larn actually sends you junk
       * mail if you win!
       */
      else
        readmail(sobj);
      break;
#endif
    case SCR_ENCHANT_ARMOR: {
      schar s;
      bool special_armor;
      bool same_color;

      otmp = some_armor(&youmonst);
      if (!otmp) {
        strange_feeling(sobj, !Blind ? "Your skin glows then fades."
                                     : "Your skin feels warm for a moment.");
        exercise(A_CON, !sobj->cursed);
        exercise(A_STR, !sobj->cursed);
        return (1);
      }
      if (confused) {
        otmp->oerodeproof = !(sobj->cursed);
        if (Blind) {
          otmp->rknown = FALSE;
          Your("%s %s warm for a moment.", xname(otmp), otense(otmp, "feel"));
        } else {
          otmp->rknown = TRUE;
          Your("%s %s covered by a %s %s %s!", xname(otmp), otense(otmp, "are"),
               sobj->cursed ? "mottled" : "shimmering",
               hcolor(sobj->cursed ? NH_BLACK : NH_GOLDEN),
               sobj->cursed ? "glow" : (is_shield(otmp) ? "layer" : "shield"));
        }
        if (otmp->oerodeproof && (otmp->oeroded || otmp->oeroded2)) {
          otmp->oeroded = otmp->oeroded2 = 0;
          Your("%s %s as good as new!", xname(otmp),
               otense(otmp, Blind ? "feel" : "look"));
        }
        break;
      }
      /* elven armor vibrates warningly when enchanted beyond a limit */
      special_armor = is_elven_armor(otmp) ||
                      (Role_if(PM_WIZARD) && otmp->otyp == CORNUTHAUM);
      if (sobj->cursed)
        same_color = (otmp->otyp == BLACK_DRAGON_SCALE_MAIL ||
                      otmp->otyp == BLACK_DRAGON_SCALES);
      else
        same_color = (otmp->otyp == SILVER_DRAGON_SCALE_MAIL ||
                      otmp->otyp == SILVER_DRAGON_SCALES ||
                      otmp->otyp == SHIELD_OF_REFLECTION);
      if (Blind)
        same_color = FALSE;

      /* KMH -- catch underflow */
      s = sobj->cursed ? -otmp->spe : otmp->spe;
      if (s > (special_armor ? 5 : 3) && rn2(s)) {
        Your("%s violently %s%s%s for a while, then %s.", xname(otmp),
             otense(otmp, Blind ? "vibrate" : "glow"),
             (!Blind && !same_color) ? " " : nul,
             (Blind || same_color) ? nul : hcolor(sobj->cursed ? NH_BLACK
                                                               : NH_SILVER),
             otense(otmp, "evaporate"));
        if (is_cloak(otmp))
          (void)Cloak_off();
        if (is_boots(otmp))
          (void)Boots_off();
        if (is_helmet(otmp))
          (void)Helmet_off();
        if (is_gloves(otmp))
          (void)Gloves_off();
        if (is_shield(otmp))
          (void)Shield_off();
        if (otmp == uarm)
          (void)Armor_gone();
        useup(otmp);
        break;
      }
      s = sobj->cursed ? -1 : otmp->spe >= 9
                                  ? (rn2(otmp->spe) == 0)
                                  : sobj->blessed ? rnd(3 - otmp->spe / 3) : 1;
      if (s >= 0 && otmp->otyp >= GRAY_DRAGON_SCALES &&
          otmp->otyp <= YELLOW_DRAGON_SCALES) {
        /* dragon scales get turned into dragon scale mail */
        Your("%s merges and hardens!", xname(otmp));
        setworn(nullptr, W_ARM);
        /* assumes same order */
        otmp->otyp = GRAY_DRAGON_SCALE_MAIL + otmp->otyp - GRAY_DRAGON_SCALES;
        otmp->cursed = 0;
        if (sobj->blessed) {
          otmp->spe++;
          otmp->blessed = 1;
        }
        otmp->known = 1;
        setworn(otmp, W_ARM);
        break;
      }
      Your("%s %s%s%s%s for a %s.", xname(otmp), s == 0 ? "violently " : nul,
           otense(otmp, Blind ? "vibrate" : "glow"),
           (!Blind && !same_color) ? " " : nul,
           (Blind || same_color) ? nul
                                 : hcolor(sobj->cursed ? NH_BLACK : NH_SILVER),
           (s * s > 1) ? "while" : "moment");
      otmp->cursed = sobj->cursed;
      if (!otmp->blessed || sobj->cursed)
        otmp->blessed = sobj->blessed;
      if (s) {
        otmp->spe += s;
        adj_abon(otmp, s);
        known = otmp->known;
      }

      if ((otmp->spe > (special_armor ? 5 : 3)) && (special_armor || !rn2(7)))
        Your("%s suddenly %s %s.", xname(otmp), otense(otmp, "vibrate"),
             Blind ? "again" : "unexpectedly");
      break;
    }
    case SCR_DESTROY_ARMOR: {
      otmp = some_armor(&youmonst);
      if (confused) {
        if (!otmp) {
          strange_feeling(sobj, "Your bones itch.");
          exercise(A_STR, FALSE);
          exercise(A_CON, FALSE);
          return (1);
        }
        otmp->oerodeproof = sobj->cursed;
        p_glow2(otmp, NH_PURPLE);
        break;
      }
      if (!sobj->cursed || !otmp || !otmp->cursed) {
        if (!destroy_arm(otmp)) {
          strange_feeling(sobj, "Your skin itches.");
          exercise(A_STR, FALSE);
          exercise(A_CON, FALSE);
          return (1);
        } else
          known = TRUE;
      } else { /* armor and scroll both cursed */
        Your("%s %s.", xname(otmp), otense(otmp, "vibrate"));
        if (otmp->spe >= -6)
          otmp->spe--;
        make_stunned(HStun + rn1(10, 10), TRUE);
      }
    } break;
    case SCR_CONFUSE_MONSTER:
    case SPE_CONFUSE_MONSTER:
      if (youmonst.data->mlet != S_HUMAN || sobj->cursed) {
        if (!HConfusion)
          You_feel("confused.");
        make_confused(HConfusion + rnd(100), FALSE);
      } else if (confused) {
        if (!sobj->blessed) {
          Your("%s begin to %s%s.", makeplural(body_part(HAND)),
               Blind ? "tingle" : "glow ", Blind ? nul : hcolor(NH_PURPLE));
          make_confused(HConfusion + rnd(100), FALSE);
        } else {
          pline("A %s%s surrounds your %s.", Blind ? nul : hcolor(NH_RED),
                Blind ? "faint buzz" : " glow", body_part(HEAD));
          make_confused(0L, TRUE);
        }
      } else {
        if (!sobj->blessed) {
          Your("%s%s %s%s.", makeplural(body_part(HAND)),
               Blind ? "" : " begin to glow",
               Blind ? (const char *)"tingle" : hcolor(NH_RED),
               player.umconf ? " even more" : "");
          player.umconf++;
        } else {
          if (Blind)
            Your("%s tingle %s sharply.", makeplural(body_part(HAND)),
                 player.umconf ? "even more" : "very");
          else
            Your("%s glow a%s brilliant %s.", makeplural(body_part(HAND)),
                 player.umconf ? "n even more" : "", hcolor(NH_RED));
          /* after a while, repeated uses become less effective */
          if (player.umconf >= 40)
            player.umconf++;
          else
            player.umconf += rn1(8, 2);
        }
      }
      break;
    case SCR_SCARE_MONSTER:
    case SPE_CAUSE_FEAR: {
      int ct = 0;
      Monster *mtmp;

      for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
        if (mtmp->dead())
          continue;
        if (cansee(mtmp->mx, mtmp->my)) {
          if (confused || sobj->cursed) {
            mtmp->mflee = mtmp->mfrozen = mtmp->msleeping = 0;
            mtmp->mcanmove = 1;
          } else if (!resist(mtmp, sobj->oclass, 0, NOTELL))
            monflee(mtmp, 0, FALSE, FALSE);
          if (!mtmp->mtame)
            ct++; /* pets don't laugh at you */
        }
      }
      if (!ct)
        You_hear("%s in the distance.", (confused || sobj->cursed)
                                            ? "sad wailing"
                                            : "maniacal laughter");
      else if (sobj->otyp == SCR_SCARE_MONSTER)
        You_hear("%s close by.", (confused || sobj->cursed)
                                     ? "sad wailing"
                                     : "maniacal laughter");
      break;
    }
    case SCR_BLANK_PAPER:
      if (Blind)
        You("don't remember there being any magic words on this scroll.");
      else
        pline("This scroll seems to be blank.");
      known = TRUE;
      break;
    case SCR_REMOVE_CURSE:
    case SPE_REMOVE_CURSE: {
      Object *obj;
      if (confused)
        if (Hallucination)
          You_feel("the power of the Force against you!");
        else
          You_feel("like you need some help.");
      else if (Hallucination)
        You_feel("in touch with the Universal Oneness.");
      else
        You_feel("like someone is helping you.");

      if (sobj->cursed) {
        pline_The("scroll disintegrates.");
      } else {
        for (obj = invent; obj; obj = obj->nobj) {
          long wornmask;
#ifdef GOLDOBJ
          /* gold isn't subject to cursing and blessing */
          if (Object->oclass == COIN_CLASS)
            continue;
#endif
          wornmask = (obj->owornmask & ~(W_BALL | W_ART | W_ARTI));
          if (wornmask && !sobj->blessed) {
            /* handle a couple of special cases; we don't
               allow auxiliary weapon slots to be used to
               artificially increase number of worn items */
            if (obj == uswapwep) {
              if (!player.twoweap)
                wornmask = 0L;
            } else if (obj == uquiver) {
              if (obj->oclass == WEAPON_CLASS) {
                /* mergeable weapon test covers ammo,
                   missiles, spears, daggers & knives */
                if (!objects[obj->otyp].oc_merge)
                  wornmask = 0L;
              } else if (obj->oclass == GEM_CLASS) {
                /* possibly ought to check whether
                   alternate weapon is a sling... */
                if (!uslinging())
                  wornmask = 0L;
              } else {
                /* weptools don't merge and aren't
                   reasonable quivered weapons */
                wornmask = 0L;
              }
            }
          }
          if (sobj->blessed || wornmask || obj->otyp == LOADSTONE ||
              (obj->otyp == LEASH && obj->leashmon)) {
            if (confused)
              BlessOrCurse(obj, 2);
            else
              Uncurse(obj);
          }
        }
      }
      if (Punished && !confused)
        unpunish();
      update_inventory();
      break;
    }
    case SCR_CREATE_MONSTER:
    case SPE_CREATE_MONSTER:
      if (create_critters(1 + ((confused || sobj->cursed) ? 12 : 0) +
                              ((sobj->blessed || rn2(73)) ? 0 : rnd(4)),
                          confused ? &mons[PM_ACID_BLOB] : nullptr))
        known = TRUE;
      /* no need to flush monsters; we ask for identification only if the
       * monsters are not visible
       */
      break;
    case SCR_ENCHANT_WEAPON:
      if (uwep && (uwep->oclass == WEAPON_CLASS || is_weptool(uwep)) &&
          confused) {
        /* oclass check added 10/25/86 GAN */
        uwep->oerodeproof = !(sobj->cursed);
        if (Blind) {
          uwep->rknown = FALSE;
          Your("weapon feels warm for a moment.");
        } else {
          uwep->rknown = TRUE;
          Your("%s covered by a %s %s %s!", aobjnam(uwep, "are"),
               sobj->cursed ? "mottled" : "shimmering",
               hcolor(sobj->cursed ? NH_PURPLE : NH_GOLDEN),
               sobj->cursed ? "glow" : "shield");
        }
        if (uwep->oerodeproof && (uwep->oeroded || uwep->oeroded2)) {
          uwep->oeroded = uwep->oeroded2 = 0;
          Your("%s as good as new!", aobjnam(uwep, Blind ? "feel" : "look"));
        }
      } else
        return !chwepon(sobj,
                        sobj->cursed
                            ? -1
                            : !uwep ? 1 : uwep->spe >= 9
                                              ? (rn2(uwep->spe) == 0)
                                              : sobj->blessed
                                                    ? rnd(3 - uwep->spe / 3)
                                                    : 1);
      break;
    case SCR_TAMING:
    case SPE_CHARM_MONSTER:
      if (player.uswallow) {
        maybe_tame(player.ustuck, sobj);
      } else {
        int i, j, bd = confused ? 5 : 1;
        Monster *mtmp;

        for (i = -bd; i <= bd; i++)
          for (j = -bd; j <= bd; j++) {
            if (!isok(player.ux + i, player.uy + j))
              continue;
            if ((mtmp = m_at(player.ux + i, player.uy + j)) != 0)
              maybe_tame(mtmp, sobj);
          }
      }
      break;
    case SCR_GENOCIDE:
      You("have found a scroll of genocide!");
      known = TRUE;
      if (sobj->blessed)
        do_class_genocide();
      else
        do_genocide(!sobj->cursed | (2 * !!Confusion));
      break;
    case SCR_LIGHT:
      if (!Blind)
        known = TRUE;
      litroom(!confused && !sobj->cursed, sobj);
      break;
    case SCR_TELEPORTATION:
      if (confused || sobj->cursed)
        level_tele();
      else {
        if (sobj->blessed && !Teleport_control) {
          known = TRUE;
          if (yn("Do you wish to teleport?") == 'n')
            break;
        }
        tele();
        if (Teleport_control || !couldsee(player.ux0, player.uy0) ||
            (distu(player.ux0, player.uy0) >= 16))
          known = TRUE;
      }
      break;
    case SCR_GOLD_DETECTION:
      if (confused || sobj->cursed)
        return (trap_detect(sobj));
      else
        return (gold_detect(sobj));
    case SCR_FOOD_DETECTION:
    case SPE_DETECT_FOOD:
      if (food_detect(sobj))
        return (1); /* nothing detected */
      break;
    case SPE_IDENTIFY:
      cval = rn2(5);
      goto id;
    case SCR_IDENTIFY:
      /* known = TRUE; */
      if (confused)
        You("identify this as an identify scroll.");
      else
        pline("This is an identify scroll.");
      if (sobj->blessed || (!sobj->cursed && !rn2(5))) {
        cval = rn2(5);
        /* Note: if rn2(5)==0, identify all items */
        if (cval == 1 && sobj->blessed && Luck > 0)
          ++cval;
      } else
        cval = 1;
      if (!objects[sobj->otyp].oc_name_known)
        more_experienced(0, 10);
      useup(sobj);
      makeknown(SCR_IDENTIFY);
    id:
      if (invent && !confused) {
        identify_pack(cval);
      }
      return (1);
    case SCR_CHARGING:
      if (confused) {
        You_feel("charged up!");
        if (player.uen < player.uenmax)
          player.uen = player.uenmax;
        else
          player.uen = (player.uenmax += d(5, 4));
        flags.botl = 1;
        break;
      }
      known = TRUE;
      pline("This is a charging scroll.");
      otmp = getobj(all_count, "charge");
      if (!otmp)
        break;
      recharge(otmp, sobj->cursed ? -1 : (sobj->blessed ? 1 : 0));
      break;
    case SCR_MAGIC_MAPPING:
      if (level.flags.nommap) {
        Your("mind is filled with crazy lines!");
        if (Hallucination)
          pline("Wow!  Modern art.");
        else
          Your("%s spins in bewilderment.", body_part(HEAD));
        make_confused(HConfusion + rnd(30), FALSE);
        break;
      }
      if (sobj->blessed) {
        int x, y;

        for (x = 1; x < COLNO; x++)
          for (y = 0; y < ROWNO; y++)
            if (levl[x][y].typ == SDOOR)
              cvt_sdoor_to_door(&levl[x][y]);
        /* do_mapping() already reveals secret passages */
      }
      known = TRUE;
    case SPE_MAGIC_MAPPING:
      if (level.flags.nommap) {
        Your("%s spins as %s blocks the spell!", body_part(HEAD), something);
        make_confused(HConfusion + rnd(30), FALSE);
        break;
      }
      pline("A map coalesces in your mind!");
      cval = (sobj->cursed && !confused);
      if (cval)
        HConfusion = 1; /* to screw up map */
      do_mapping();
      if (cval) {
        HConfusion = 0; /* restore */
        pline("Unfortunately, you can't grasp the details.");
      }
      break;
    case SCR_AMNESIA:
      known = TRUE;
      forget((!sobj->blessed ? ALL_SPELLS : 0) |
             (!confused || sobj->cursed ? ALL_MAP : 0));
      if (Hallucination) /* Ommmmmm! */
        Your("mind releases itself from mundane concerns.");
      else if (!strncmpi(plname, "Maud", 4))
        pline(
            "As your mind turns inward on itself, you forget everything else.");
      else if (rn2(2))
        pline("Who was that Maud person anyway?");
      else
        pline("Thinking of Maud you forget everything else.");
      exercise(A_WIS, FALSE);
      break;
    case SCR_FIRE:
      /*
       * Note: Modifications have been made as of 3.0 to allow for
       * some damage under all potential cases.
       */
      cval = GetBUCSign(sobj);
      if (!objects[sobj->otyp].oc_name_known)
        more_experienced(0, 10);
      useup(sobj);
      makeknown(SCR_FIRE);
      if (confused) {
        if (Fire_resistance) {
          shieldeff(player.ux, player.uy);
          if (!Blind)
            pline("Oh, look, what a pretty fire in your %s.",
                  makeplural(body_part(HAND)));
          else
            You_feel("a pleasant warmth in your %s.",
                     makeplural(body_part(HAND)));
        } else {
          pline_The("scroll catches fire and you burn your %s.",
                    makeplural(body_part(HAND)));
          losehp(1, "scroll of fire", KILLED_BY_AN);
        }
        return (1);
      }
      if (Underwater)
        pline_The("water around you vaporizes violently!");
      else {
        pline_The("scroll erupts in a tower of flame!");
        burn_away_slime();
      }
      explode(player.ux, player.uy, 11, (2 * (rn1(3, 3) + 2 * cval) + 1) / 3,
              SCROLL_CLASS, EXPL_FIERY);
      return (1);
    case SCR_EARTH:
      /* TODO: handle steeds */
      if (
#ifdef REINCARNATION
              !Is_rogue_level(&player.uz) &&
#endif
              (!In_endgame(&player.uz) || Is_earthlevel(&player.uz))) {
        int x, y;

        /* Identify the scroll */
        pline_The("%s rumbles %s you!", ceiling(player.ux, player.uy),
                  sobj->blessed ? "around" : "above");
        known = 1;
        if (In_sokoban(&player.uz))
          change_luck(-1); /* Sokoban guilt */

        /* Loop through the surrounding squares */
        if (!sobj->cursed)
          for (x = player.ux - 1; x <= player.ux + 1; x++) {
            for (y = player.uy - 1; y <= player.uy + 1; y++) {
              /* Is this a suitable spot? */
              if (isok(x, y) && !closed_door(x, y) &&
                  !IS_ROCK(levl[x][y].typ) && !IS_AIR(levl[x][y].typ) &&
                  (x != player.ux || y != player.uy)) {
                Object *otmp2;
                Monster *mtmp;

                /* Make the object(s) */
                otmp2 =
                    MakeSpecificObject(confused ? ROCK : BOULDER, FALSE, FALSE);
                if (!otmp2)
                  continue; /* Shouldn't happen */
                otmp2->quan = confused ? rn1(5, 2) : 1;
                otmp2->owt = GetWeight(otmp2);

                /* Find the monster here (won't be player) */
                mtmp = m_at(x, y);
                if (mtmp && !amorphous(mtmp->data) &&
                    !passes_walls(mtmp->data) && !noncorporeal(mtmp->data) &&
                    !unsolid(mtmp->data)) {
                  Object *helmet = which_armor(mtmp, W_ARMH);
                  int mdmg;

                  if (cansee(mtmp->mx, mtmp->my)) {
                    pline("%s is hit by %s!", Monnam(mtmp), doname(otmp2));
                    if (mtmp->minvis && !canspotmon(mtmp))
                      map_invisible(mtmp->mx, mtmp->my);
                  }
                  mdmg = dmgval(otmp2, mtmp) * otmp2->quan;
                  if (helmet) {
                    if (is_metallic(helmet)) {
                      if (canspotmon(mtmp))
                        pline("Fortunately, %s is wearing a hard helmet.",
                              mon_nam(mtmp));
                      else if (flags.soundok)
                        You_hear("a clanging sound.");
                      if (mdmg > 2)
                        mdmg = 2;
                    } else {
                      if (canspotmon(mtmp))
                        pline("%s's %s does not protect %s.", Monnam(mtmp),
                              xname(helmet), mhim(mtmp));
                    }
                  }
                  mtmp->mhp -= mdmg;
                  if (mtmp->mhp <= 0)
                    xkilled(mtmp, 1);
                }
                /* Drop the rock/boulder to the floor */
                if (!flooreffects(otmp2, x, y, "fall")) {
                  PlaceObject(otmp2, x, y);
                  stackobj(otmp2);
                  newsym(x, y); /* map the rock */
                }
              }
            }
          }
        /* Attack the player */
        if (!sobj->blessed) {
          int dmg;
          Object *otmp2;

          /* Okay, _you_ write this without repeating the code */
          otmp2 = MakeSpecificObject(confused ? ROCK : BOULDER, FALSE, FALSE);
          if (!otmp2)
            break;
          otmp2->quan = confused ? rn1(5, 2) : 1;
          otmp2->owt = GetWeight(otmp2);
          if (!amorphous(youmonst.data) && !Passes_walls &&
              !noncorporeal(youmonst.data) && !unsolid(youmonst.data)) {
            You("are hit by %s!", doname(otmp2));
            dmg = dmgval(otmp2, &youmonst) * otmp2->quan;
            if (uarmh && !sobj->cursed) {
              if (is_metallic(uarmh)) {
                pline("Fortunately, you are wearing a hard helmet.");
                if (dmg > 2)
                  dmg = 2;
              } else if (flags.verbose) {
                Your("%s does not protect you.", xname(uarmh));
              }
            }
          } else
            dmg = 0;
          /* Must be before the losehp(), for bones files */
          if (!flooreffects(otmp2, player.ux, player.uy, "fall")) {
            PlaceObject(otmp2, player.ux, player.uy);
            stackobj(otmp2);
            newsym(player.ux, player.uy);
          }
          if (dmg)
            losehp(dmg, "scroll of earth", KILLED_BY_AN);
        }
      }
      break;
    case SCR_PUNISHMENT:
      known = TRUE;
      if (confused || sobj->blessed) {
        You_feel("guilty.");
        break;
      }
      punish(sobj);
      break;
    case SCR_STINKING_CLOUD: {
      coord cc;

      You("have found a scroll of stinking cloud!");
      known = TRUE;
      pline("Where do you want to center the cloud?");
      cc.x = player.ux;
      cc.y = player.uy;
      if (getpos(&cc, TRUE, "the desired position") < 0) {
        pline(Never_mind);
        return 0;
      }
      if (!cansee(cc.x, cc.y) || distu(cc.x, cc.y) >= 32) {
        You("smell rotten eggs.");
        return 0;
      }
      (void)create_gas_cloud(cc.x, cc.y, 3 + GetBUCSign(sobj),
                             8 + 4 * GetBUCSign(sobj));
      break;
    }
    default:
      impossible("What weird effect is this? (%u)", sobj->otyp);
  }
  return (0);
}

static void wand_explode(Object *obj) {
  obj->in_use = TRUE; /* in case losehp() is fatal */
  Your("%s vibrates violently, and explodes!", xname(obj));
  nhbell();
  losehp(rnd(2 * (player.uhpmax + 1) / 3), "exploding wand", KILLED_BY_AN);
  useup(obj);
  exercise(A_STR, FALSE);
}

/*
 * Low-level lit-field update routine.
 */
STATIC_PTR void set_lit(int x, int y, genericptr_t val) {
  if (val)
    levl[x][y].lit = 1;
  else {
    levl[x][y].lit = 0;
    snuff_light_source(x, y);
  }
}

void litroom(bool on, Object *obj) {
  char is_lit; /* value is irrelevant; we use its address
                  as a `not null' flag for set_lit() */

  /* first produce the text (provided you're not blind) */
  if (!on) {
    Object *otmp;

    if (!Blind) {
      if (player.uswallow) {
        pline("It seems even darker in here than before.");
        return;
      }
      if (uwep && artifact_light(uwep) && uwep->lamplit)
        pline("Suddenly, the only light left comes from %s!", the(xname(uwep)));
      else
        You("are surrounded by darkness!");
    }

    /* the magic douses lamps, et al, too */
    for (otmp = invent; otmp; otmp = otmp->nobj)
      if (otmp->lamplit)
        (void)snuff_lit(otmp);
    if (Blind)
      goto do_it;
  } else {
    if (Blind)
      goto do_it;
    if (player.uswallow) {
      if (is_animal(player.ustuck->data))
        pline("%s %s is lit.", s_suffix(Monnam(player.ustuck)),
              mbodypart(player.ustuck, STOMACH));
      else if (is_whirly(player.ustuck->data))
        pline("%s shines briefly.", Monnam(player.ustuck));
      else
        pline("%s glistens.", Monnam(player.ustuck));
      return;
    }
    pline("A lit field surrounds you!");
  }

do_it:
  /* No-op in water - can only see the adjacent squares and that's it! */
  if (Underwater || Is_waterlevel(&player.uz))
    return;
  /*
   *  If we are darkening the room and the hero is punished but not
   *  blind, then we have to pick up and replace the ball and chain so
   *  that we don't remember them if they are out of sight.
   */
  if (Punished && !on && !Blind)
    move_bc(1, 0, uball->ox, uball->oy, uchain->ox, uchain->oy);

#ifdef REINCARNATION
  if (Is_rogue_level(&player.uz)) {
    /* Can't use do_clear_area because MAX_RADIUS is too small */
    /* rogue lighting must light the entire room */
    int rnum = levl[player.ux][player.uy].roomno - ROOMOFFSET;
    int rx, ry;
    if (rnum >= 0) {
      for (rx = rooms[rnum].lx - 1; rx <= rooms[rnum].hx + 1; rx++)
        for (ry = rooms[rnum].ly - 1; ry <= rooms[rnum].hy + 1; ry++)
          set_lit(rx, ry, (genericptr_t)(on ? &is_lit : nullptr));
      rooms[rnum].rlit = on;
    }
    /* hallways remain dark on the rogue level */
  } else
#endif
    do_clear_area(player.ux, player.uy,
                  (obj && obj->oclass == SCROLL_CLASS && obj->blessed) ? 9 : 5,
                  set_lit, (genericptr_t)(on ? &is_lit : nullptr));

  /*
   *  If we are not blind, then force a redraw on all positions in sight
   *  by temporarily blinding the hero.  The vision recalculation will
   *  correctly update all previously seen positions *and* correctly
   *  set the waslit bit [could be messed up from above].
   */
  if (!Blind) {
    vision_recalc(2);

    /* replace ball&chain */
    if (Punished && !on)
      move_bc(0, 0, uball->ox, uball->oy, uchain->ox, uchain->oy);
  }

  vision_full_recalc = 1; /* delayed vision recalculation */
}

static void do_class_genocide() {
  int i, j, immunecnt, gonecnt, goodcnt, class_id, feel_dead = 0;
  char buf[BUFSZ];
  bool gameover = FALSE; /* true iff killed self */

  for (j = 0;; j++) {
    if (j >= 5) {
      pline(thats_enough_tries);
      return;
    }
    do {
      getlin("What class_id of monsters do you wish to genocide?", buf);
      (void)mungspaces(buf);
    } while (buf[0] == '\033' || !buf[0]);
    /* choosing "none" preserves genocideless conduct */
    if (!strcmpi(buf, "none") || !strcmpi(buf, "nothing"))
      return;

    if (strlen(buf) == 1) {
      if (buf[0] == ILLOBJ_SYM)
        buf[0] = def_monsyms[S_MIMIC];
      class_id = def_char_to_monclass(buf[0]);
    } else {
      char buf2[BUFSZ];

      class_id = 0;
      strcpy(buf2, makesingular(buf));
      strcpy(buf, buf2);
    }
    immunecnt = gonecnt = goodcnt = 0;
    for (i = LOW_PM; i < NUMMONS; i++) {
      if (class_id == 0 && strstri(monexplain[(int)mons[i].mlet], buf) != 0)
        class_id = mons[i].mlet;
      if (mons[i].mlet == class_id) {
        if (!(mons[i].geno & G_GENO))
          immunecnt++;
        else if (mvitals[i].mvflags & G_GENOD)
          gonecnt++;
        else
          goodcnt++;
      }
    }
    /*
     * TODO[?]: If user's input doesn't match any class_id
     *	    description, check individual species names.
     */
    if (!goodcnt && class_id != mons[urole.malenum].mlet &&
        class_id != mons[urace.malenum].mlet) {
      if (gonecnt)
        pline("All such monsters are already nonexistent.");
      else if (immunecnt || (buf[0] == DEF_INVISIBLE && buf[1] == '\0'))
        You("aren't permitted to genocide such monsters.");
      else
#ifdef WIZARD /* to aid in topology testing; remove pesky monsters */
          if (wizard && buf[0] == '*') {
        Monster *mtmp, *mtmp2;

        gonecnt = 0;
        for (mtmp = fmon; mtmp; mtmp = mtmp2) {
          mtmp2 = mtmp->nmon;
          if (mtmp->dead())
            continue;
          mongone(mtmp);
          gonecnt++;
        }
        pline("Eliminated %d monster%s.", gonecnt, plur(gonecnt));
        return;
      } else
#endif
        pline("That symbol does not represent any monster.");
      continue;
    }

    for (i = LOW_PM; i < NUMMONS; i++) {
      if (mons[i].mlet == class_id) {
        char nam[BUFSZ];

        strcpy(nam, makeplural(mons[i].mname));
        /* Although "genus" is Latin for race, the hero benefits
         * from both race and role; thus genocide affects either.
         */
        if (Your_Own_Role(i) || Your_Own_Race(i) ||
            ((mons[i].geno & G_GENO) && !(mvitals[i].mvflags & G_GENOD))) {
          /* This check must be first since player monsters might
           * have G_GENOD or !G_GENO.
           */
          mvitals[i].mvflags |= (G_GENOD | G_NOCORPSE);
          reset_rndmonst(i);
          kill_genocided_monsters();
          update_inventory(); /* eggs & tins */
          pline("Wiped out all %s.", nam);
          if (Upolyd && i == player.umonnum) {
            player.mh = -1;
            if (Unchanging) {
              if (!feel_dead++)
                You("die.");
              /* finish genociding this class_id of
                 monsters before ultimately dying */
              gameover = TRUE;
            } else
              rehumanize();
          }
          /* Self-genocide if it matches either your race
             or role.  Assumption:  male and female forms
             share same monster class_id. */
          if (i == urole.malenum || i == urace.malenum) {
            player.uhp = -1;
            if (Upolyd) {
              if (!feel_dead++)
                You_feel("dead inside.");
            } else {
              if (!feel_dead++)
                You("die.");
              gameover = TRUE;
            }
          }
        } else if (mvitals[i].mvflags & G_GENOD) {
          if (!gameover)
            pline("All %s are already nonexistent.", nam);
        } else if (!gameover) {
          /* suppress feedback about quest beings except
             for those applicable to our own role */
          if ((mons[i].msound != MS_LEADER || quest_info(MS_LEADER) == i) &&
              (mons[i].msound != MS_NEMESIS || quest_info(MS_NEMESIS) == i) &&
              (mons[i].msound != MS_GUARDIAN || quest_info(MS_GUARDIAN) == i)
              /* non-leader/nemesis/guardian role-specific monster */
              &&
              (i != PM_NINJA || /* nuisance */
               Role_if(PM_SAMURAI))) {
            bool named, uniq;

            named = type_is_pname(&mons[i]) ? TRUE : FALSE;
            uniq = (mons[i].geno & G_UNIQ) ? TRUE : FALSE;
            /* one special case */
            if (i == PM_HIGH_PRIEST)
              uniq = FALSE;

            You("aren't permitted to genocide %s%s.",
                (uniq && !named) ? "the " : "",
                (uniq || named) ? mons[i].mname : nam);
          }
        }
      }
    }
    if (gameover || player.uhp == -1) {
      killer_format = KILLED_BY_AN;
      killer = "scroll of genocide";
      if (gameover)
        done(GENOCIDED);
    }
    return;
  }
}

#define REALLY 1
#define PLAYER 2
#define ONTHRONE 4
/* 0 = no genocide; create monsters (cursed scroll) */
/* 1 = normal genocide */
/* 3 = forced genocide of player */
/* 5 (4 | 1) = normal genocide from throne */
void do_genocide(int how) {
  char buf[BUFSZ];
  int i, killplayer = 0;
  int mndx;
  MonsterType *ptr;
  const char *which;

  if (how & PLAYER) {
    mndx = player.umonster; /* non-polymorphed mon num */
    ptr = &mons[mndx];
    strcpy(buf, ptr->mname);
    killplayer++;
  } else {
    for (i = 0;; i++) {
      if (i >= 5) {
        pline(thats_enough_tries);
        return;
      }
      getlin("What monster do you want to genocide? [type the name]", buf);
      (void)mungspaces(buf);
      /* choosing "none" preserves genocideless conduct */
      if (!strcmpi(buf, "none") || !strcmpi(buf, "nothing")) {
        /* ... but no free pass if cursed */
        if (!(how & REALLY)) {
          ptr = rndmonst();
          if (!ptr)
            return; /* no message, like normal case */
          mndx = monsndx(ptr);
          break; /* remaining checks don't apply */
        } else
          return;
      }

      mndx = name_to_mon(buf);
      if (mndx == NON_PM || (mvitals[mndx].mvflags & G_GENOD)) {
        pline("Such creatures %s exist in this world.",
              (mndx == NON_PM) ? "do not" : "no longer");
        continue;
      }
      ptr = &mons[mndx];
      /* Although "genus" is Latin for race, the hero benefits
       * from both race and role; thus genocide affects either.
       */
      if (Your_Own_Role(mndx) || Your_Own_Race(mndx)) {
        killplayer++;
        break;
      }
      if (is_human(ptr))
        adjalign(-sgn(player.ualign.type));
      if (is_demon(ptr))
        adjalign(sgn(player.ualign.type));

      if (!(ptr->geno & G_GENO)) {
        if (flags.soundok) {
          /* fixme: unconditional "caverns" will be silly in some circumstances
           */
          if (flags.verbose)
            pline("A thunderous voice booms through the caverns:");
          verbalize("No, mortal!  That will not be done.");
        }
        continue;
      }
      /* KMH -- Unchanging prevents rehumanization */
      if (Unchanging && ptr == youmonst.data)
        killplayer++;
      break;
    }
  }

  which = "all ";
  if (Hallucination) {
    if (Upolyd)
      strcpy(buf, youmonst.data->mname);
    else {
      strcpy(buf, (flags.female && urole.name.f) ? urole.name.f : urole.name.m);
      buf[0] = lowc(buf[0]);
    }
  } else {
    strcpy(buf, ptr->mname); /* make sure we have standard singular */
    if ((ptr->geno & G_UNIQ) && ptr != &mons[PM_HIGH_PRIEST])
      which = !type_is_pname(ptr) ? "the " : "";
  }
  if (how & REALLY) {
    /* setting no-corpse affects wishing and random tin generation */
    mvitals[mndx].mvflags |= (G_GENOD | G_NOCORPSE);
    pline("Wiped out %s%s.", which, (*which != 'a') ? buf : makeplural(buf));

    if (killplayer) {
      /* might need to wipe out dual role */
      if (urole.femalenum != NON_PM && mndx == urole.malenum)
        mvitals[urole.femalenum].mvflags |= (G_GENOD | G_NOCORPSE);
      if (urole.femalenum != NON_PM && mndx == urole.femalenum)
        mvitals[urole.malenum].mvflags |= (G_GENOD | G_NOCORPSE);
      if (urace.femalenum != NON_PM && mndx == urace.malenum)
        mvitals[urace.femalenum].mvflags |= (G_GENOD | G_NOCORPSE);
      if (urace.femalenum != NON_PM && mndx == urace.femalenum)
        mvitals[urace.malenum].mvflags |= (G_GENOD | G_NOCORPSE);

      player.uhp = -1;
      if (how & PLAYER) {
        killer_format = KILLED_BY;
        killer = "genocidal confusion";
      } else if (how & ONTHRONE) {
        /* player selected while on a throne */
        killer_format = KILLED_BY_AN;
        killer = "imperious order";
      } else { /* selected player deliberately, not confused */
        killer_format = KILLED_BY_AN;
        killer = "scroll of genocide";
      }

      /* Polymorphed characters will die as soon as they're rehumanized. */
      /* KMH -- Unchanging prevents rehumanization */
      if (Upolyd && ptr != youmonst.data) {
        delayed_killer = killer;
        killer = 0;
        You_feel("dead inside.");
      } else
        done(GENOCIDED);
    } else if (ptr == youmonst.data) {
      rehumanize();
    }
    reset_rndmonst(mndx);
    kill_genocided_monsters();
    update_inventory(); /* in case identified eggs were affected */
  } else {
    int cnt = 0;

    if (!(mons[mndx].geno & G_UNIQ) &&
        !(mvitals[mndx].mvflags & (G_GENOD | G_EXTINCT)))
      for (i = rn1(3, 4); i > 0; i--) {
        if (!makemon(ptr, player.ux, player.uy, NO_MINVENT))
          break; /* couldn't make one */
        ++cnt;
        if (mvitals[mndx].mvflags & G_EXTINCT)
          break; /* just made last one */
      }
    if (cnt)
      pline("Sent in some %s.", makeplural(buf));
    else
      pline(nothing_happens);
  }
}

void punish(Object *sobj) {
  /* KMH -- Punishment is still okay when you are riding */
  You("are being punished for your misbehavior!");
  if (Punished) {
    Your("iron ball gets heavier.");
    uball->owt += 160 * (1 + sobj->cursed);
    return;
  }
  if (amorphous(youmonst.data) || is_whirly(youmonst.data) ||
      unsolid(youmonst.data)) {
    pline("A ball and chain appears, then falls away.");
    dropy(MakeRandomObject(BALL_CLASS, TRUE));
    return;
  }
  setworn(MakeRandomObject(CHAIN_CLASS, TRUE), W_CHAIN);
  setworn(MakeRandomObject(BALL_CLASS, TRUE), W_BALL);
  uball->spe = 1; /* special ball (see save) */

  /*
   *  Place ball & chain if not swallowed.  If swallowed, the ball &
   *  chain variables will be set at the next call to placebc().
   */
  if (!player.uswallow) {
    placebc();
    if (Blind)
      set_bc(1);                  /* set up ball and chain variables */
    newsym(player.ux, player.uy); /* see ball&chain if can't see self */
  }
}

void unpunish() {
  Object *savechain = uchain;

  RemoveObjectFromStorage(uchain);
  newsym(uchain->ox, uchain->oy);
  setworn(nullptr, W_CHAIN);
  DeallocateObject(savechain);
  uball->spe = 0;
  setworn(nullptr, W_BALL);
}

/* some creatures have special data structures that only make sense in their
 * normal locations -- if the player tries to create one elsewhere, or to revive
 * one, the disoriented creature becomes a zombie
 */
bool cant_create(int *mtype, bool revival) {
  /* SHOPKEEPERS can be revived now */
  if (*mtype == PM_GUARD || (*mtype == PM_SHOPKEEPER && !revival) ||
      *mtype == PM_ALIGNED_PRIEST || *mtype == PM_ANGEL) {
    *mtype = PM_HUMAN_ZOMBIE;
    return TRUE;
  } else if (*mtype == PM_LONG_WORM_TAIL) { /* for create_particular() */
    *mtype = PM_LONG_WORM;
    return TRUE;
  }
  return FALSE;
}

#ifdef WIZARD
/*
 * Make a new monster with the type controlled by the user.
 *
 * Note:  when creating a monster by class letter, specifying the
 * "strange object" (']') symbol produces a random monster rather
 * than a mimic; this behavior quirk is useful so don't "fix" it...
 */
bool create_particular() {
  char buf[BUFSZ], *bufp, monclass = MAXMCLASSES;
  int which, tries, i;
  MonsterType *whichpm;
  Monster *mtmp;
  bool madeany = FALSE;
  bool maketame, makepeaceful, makehostile;

  tries = 0;
  do {
    which = urole.malenum; /* an arbitrary index into mons[] */
    maketame = makepeaceful = makehostile = FALSE;
    getlin("Create what kind of monster? [type the name or symbol]", buf);
    bufp = mungspaces(buf);
    if (*bufp == '\033')
      return FALSE;
    /* allow the initial disposition to be specified */
    if (!strncmpi(bufp, "tame ", 5)) {
      bufp += 5;
      maketame = TRUE;
    } else if (!strncmpi(bufp, "peaceful ", 9)) {
      bufp += 9;
      makepeaceful = TRUE;
    } else if (!strncmpi(bufp, "hostile ", 8)) {
      bufp += 8;
      makehostile = TRUE;
    }
    /* decide whether a valid monster was chosen */
    if (strlen(bufp) == 1) {
      monclass = def_char_to_monclass(*bufp);
      if (monclass != MAXMCLASSES)
        break; /* got one */
    } else {
      which = name_to_mon(bufp);
      if (which >= LOW_PM)
        break; /* got one */
    }
    /* no good; try again... */
    pline("I've never heard of such monsters.");
  } while (++tries < 5);

  if (tries == 5) {
    pline(thats_enough_tries);
  } else {
    (void)cant_create(&which, FALSE);
    whichpm = &mons[which];
    for (i = 0; i <= multi; i++) {
      if (monclass != MAXMCLASSES)
        whichpm = mkclass(monclass, 0);
      if (maketame) {
        mtmp = makemon(whichpm, player.ux, player.uy, MM_EDOG);
        if (mtmp) {
          initedog(mtmp);
          set_malign(mtmp);
        }
      } else {
        mtmp = makemon(whichpm, player.ux, player.uy, NO_MM_FLAGS);
        if ((makepeaceful || makehostile) && mtmp) {
          mtmp->mtame = 0; /* sanity precaution */
          mtmp->mpeaceful = makepeaceful ? 1 : 0;
          set_malign(mtmp);
        }
      }
      if (mtmp)
        madeany = TRUE;
    }
  }
  return madeany;
}
#endif /* WIZARD */

#endif /* OVLB */

/*read.c*/