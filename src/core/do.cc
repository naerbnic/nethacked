/*	SCCS Id: @(#)do.c	3.4	2003/12/02	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/* Contains code for 'd', 'D' (drop), '>', '<' (up, down) */

#include "core/do.h"

#include <string.h>

#include "core/hack.h"
#include "core/zap.h"
#include "core/worn.h"
#include "core/worm.h"
#include "core/wizard.h"
#include "core/wield.h"
#include "core/weapon.h"
#include "core/unixtty.h"
#include "core/uhitm.h"
#include "core/track.h"
#include "core/teleport.h"
#include "core/steed.h"
#include "core/steal.h"
#include "core/shk.h"
#include "core/save.h"
#include "core/rnd.h"
#include "core/restore.h"
#include "core/read.h"
#include "core/questpgr.h"
#include "core/priest.h"
#include "core/potion.h"
#include "core/polyself.h"
#include "core/pline.h"
#include "core/pickup.h"
#include "core/objnam.h"
#include "core/mplayer.h"
#include "core/mon.h"
#include "core/mkobj.h"
#include "core/mkmaze.h"
#include "core/mklev.h"
#include "core/makemon.h"
#include "core/invent.h"
#include "core/hacklib.h"
#include "core/fountain.h"
#include "core/files.h"
#include "core/end.h"
#include "core/dothrow.h"
#include "core/dokick.h"
#include "core/drawing.h"
#include "core/dog.h"
#include "core/do_name.h"
#include "core/dig.h"
#include "core/dbridge.h"
#include "core/cmd.h"

#include "core/apply.h"
#include "core/artifact.h"
#include "core/ball.h"
#include "core/lev.h"

#ifdef SINKS
#ifdef OVLB
STATIC_DCL void trycall(Object *);
#endif /* OVLB */
STATIC_DCL void dosinkring(Object *);
#endif /* SINKS */

STATIC_PTR int drop(Object *);
STATIC_PTR int wipeoff();

#ifdef OVL0
STATIC_DCL int menu_drop(int);
#endif
#ifdef OVL2
STATIC_DCL int currentlevel_rewrite();
STATIC_DCL void final_level();
/* static bool badspot(xchar,xchar); */
#endif

#ifdef OVLB

static const char drop_types[] = {ALLOW_COUNT, COIN_CLASS, ALL_CLASSES, 0};

/* 'd' command: drop one inventory item */
int dodrop() {
#ifndef GOLDOBJ
  int result, i = (invent || player.ugold) ? 0 : (SIZE(drop_types) - 1);
#else
  int result, i = (invent) ? 0 : (SIZE(drop_types) - 1);
#endif

  if (*player.ushops)
    sellobj_state(SELL_DELIBERATE);
  result = drop(getobj(&drop_types[i], "drop"));
  if (*player.ushops)
    sellobj_state(SELL_NORMAL);
  reset_occupations();

  return result;
}

#endif /* OVLB */
#ifdef OVL0

/* Called when a boulder is dropped, thrown, or pushed.  If it ends up
 * in a pool, it either fills the pool up or sinks away.  In either case,
 * it's gone for good...  If the destination is not a pool, returns FALSE.
 */
bool boulder_hits_pool(Object *otmp, int rx, int ry, bool pushing) {
  if (!otmp || otmp->otyp != BOULDER)
    impossible("Not a boulder?");
  else if (!Is_waterlevel(&player.uz) && (is_pool(rx, ry) || is_lava(rx, ry))) {
    bool lava = is_lava(rx, ry), fills_up;
    const char *what = waterbody_name(rx, ry);
    schar ltyp = levl[rx][ry].typ;
    int chance = rn2(10); /* water: 90%; lava: 10% */
    fills_up = lava ? chance == 0 : chance != 0;

    if (fills_up) {
      Trap *ttmp = t_at(rx, ry);

      if (ltyp == DRAWBRIDGE_UP) {
        levl[rx][ry].drawbridgemask &= ~DB_UNDER; /* clear lava */
        levl[rx][ry].drawbridgemask |= DB_FLOOR;
      } else
        levl[rx][ry].typ = ROOM;

      if (ttmp)
        (void)delfloortrap(ttmp);
      bury_objs(rx, ry);

      newsym(rx, ry);
      if (pushing) {
        You("push %s into the %s.", the(xname(otmp)), what);
        if (flags.verbose && !Blind)
          pline("Now you can cross it!");
        /* no splashing in this case */
      }
    }
    if (!fills_up || !pushing) { /* splashing occurs */
      if (!player.uinwater) {
        if (pushing ? !Blind : cansee(rx, ry)) {
          There("is a large splash as %s %s the %s.", the(xname(otmp)),
                fills_up ? "fills" : "falls into", what);
        } else if (flags.soundok)
          You_hear("a%s splash.", lava ? " sizzling" : "");
        wake_nearto(rx, ry, 40);
      }

      if (fills_up && player.uinwater && distu(rx, ry) == 0) {
        player.uinwater = 0;
        docrt();
        vision_full_recalc = 1;
        You("find yourself on dry land again!");
      } else if (lava && distu(rx, ry) <= 2) {
        You("are hit by molten lava%c", Fire_resistance ? '.' : '!');
        burn_away_slime();
        losehp(d((Fire_resistance ? 1 : 3), 6), "molten lava", KILLED_BY);
      } else if (!fills_up && flags.verbose &&
                 (pushing ? !Blind : cansee(rx, ry)))
        pline("It sinks without a trace!");
    }

    /* boulder is now gone */
    if (pushing)
      delobj(otmp);
    else
      obfree(otmp, nullptr);
    return TRUE;
  }
  return FALSE;
}

/* Used for objects which sometimes do special things when dropped; must be
 * called with the object not in any chain.  Returns TRUE if the object goes
 * away.
 */
bool flooreffects(Object *obj, int x, int y, const char *verb) {
  Trap *t;
  Monster *mtmp;

  if (obj->where != OBJ_FREE)
    panic("flooreffects: obj not free");

  /* make sure things like water_damage() have no pointers to follow */
  obj->nobj = obj->nexthere = nullptr;

  if (obj->otyp == BOULDER && boulder_hits_pool(obj, x, y, FALSE))
    return TRUE;
  else if (obj->otyp == BOULDER && (t = t_at(x, y)) != 0 &&
           (t->ttyp == PIT || t->ttyp == SPIKED_PIT || t->ttyp == TRAPDOOR ||
            t->ttyp == HOLE)) {
    if (((mtmp = m_at(x, y)) && mtmp->mtrapped) ||
        (player.utrap && player.ux == x && player.uy == y)) {
      if (*verb)
        pline_The("boulder %s into the pit%s.", vtense((const char *)0, verb),
                  (mtmp) ? "" : " with you");
      if (mtmp) {
        if (!passes_walls(mtmp->data) && !throws_rocks(mtmp->data)) {
          if (hmon(mtmp, obj, TRUE) && !is_whirly(mtmp->data))
            return FALSE; /* still alive */
        }
        mtmp->mtrapped = 0;
      } else {
        if (!Passes_walls && !throws_rocks(youmonst.data)) {
          losehp(rnd(15), "squished under a boulder", NO_KILLER_PREFIX);
          return FALSE; /* player remains trapped */
        } else
          player.utrap = 0;
      }
    }
    if (*verb) {
      if (Blind) {
        if ((x == player.ux) && (y == player.uy))
          You_hear("a CRASH! beneath you.");
        else
          You_hear("the boulder %s.", verb);
      } else if (cansee(x, y)) {
        pline_The("boulder %s%s.", t->tseen ? "" : "triggers and ",
                  t->ttyp == TRAPDOOR
                      ? "plugs a trap door"
                      : t->ttyp == HOLE ? "plugs a hole" : "fills a pit");
      }
    }
    deltrap(t);
    obfree(obj, nullptr);
    bury_objs(x, y);
    newsym(x, y);
    return TRUE;
  } else if (is_lava(x, y)) {
    return fire_damage(obj, FALSE, FALSE, x, y);
  } else if (is_pool(x, y)) {
    /* Reasonably bulky objects (arbitrary) splash when dropped.
     * If you're floating above the water even small things make noise.
     * Stuff dropped near fountains always misses */
    if ((Blind || (Levitation || Flying)) && flags.soundok &&
        ((x == player.ux) && (y == player.uy))) {
      if (!Underwater) {
        if (GetWeight(obj) > 9) {
          pline("Splash!");
        } else if (Levitation || Flying) {
          pline("Plop!");
        }
      }
      map_background(x, y, 0);
      newsym(x, y);
    }
    return water_damage(obj, FALSE, FALSE);
  } else if (player.ux == x && player.uy == y &&
             (!player.utrap || player.utraptype != TT_PIT) &&
             (t = t_at(x, y)) != 0 && t->tseen &&
             (t->ttyp == PIT || t->ttyp == SPIKED_PIT)) {
    /* you escaped a pit and are standing on the precipice */
    if (Blind && flags.soundok)
      You_hear("%s %s downwards.", The(xname(obj)), otense(obj, "tumble"));
    else
      pline("%s %s into %s pit.", The(xname(obj)), otense(obj, "tumble"),
            the_your[t->madeby_u]);
  }
  return FALSE;
}

#endif /* OVL0 */
#ifdef OVLB

/* obj is an object dropped on an altar */
void doaltarobj(Object *obj) {
  if (Blind)
    return;

  /* KMH, conduct */
  player.uconduct.gnostic++;

  if ((obj->blessed || obj->cursed) && obj->oclass != COIN_CLASS) {
    There("is %s flash as %s %s the altar.",
          an(hcolor(obj->blessed ? NH_AMBER : NH_BLACK)), doname(obj),
          otense(obj, "hit"));
    if (!Hallucination)
      obj->bknown = 1;
  } else {
    pline("%s %s on the altar.", Doname2(obj), otense(obj, "land"));
    obj->bknown = 1;
  }
}

#ifdef SINKS
STATIC_OVL
void trycall(Object *obj) {
  if (!objects[obj->otyp].oc_name_known && !objects[obj->otyp].oc_uname)
    docall(obj);
}

STATIC_OVL
/* obj is a ring being dropped over a kitchen sink */
void dosinkring(Object *obj) {
  Object *otmp, *otmp2;
  bool ideed = TRUE;

  You("drop %s down the drain.", doname(obj));
  obj->in_use = TRUE;  /* block free identification via interrupt */
  switch (obj->otyp) { /* effects that can be noticed without eyes */
    case RIN_SEARCHING:
      You("thought your %s got lost in the sink, but there it is!", xname(obj));
      goto giveback;
    case RIN_SLOW_DIGESTION:
      pline_The("ring is regurgitated!");
    giveback:
      obj->in_use = FALSE;
      dropx(obj);
      trycall(obj);
      return;
    case RIN_LEVITATION:
      pline_The("sink quivers upward for a moment.");
      break;
    case RIN_POISON_RESISTANCE:
      You("smell rotten %s.", makeplural(fruitname(FALSE)));
      break;
    case RIN_AGGRAVATE_MONSTER:
      pline("Several flies buzz angrily around the sink.");
      break;
    case RIN_SHOCK_RESISTANCE:
      pline("Static electricity surrounds the sink.");
      break;
    case RIN_CONFLICT:
      You_hear("loud noises coming from the drain.");
      break;
    case RIN_SUSTAIN_ABILITY: /* KMH */
      pline_The("water flow seems fixed.");
      break;
    case RIN_GAIN_STRENGTH:
      pline_The("water flow seems %ser now.",
                (obj->spe < 0) ? "weak" : "strong");
      break;
    case RIN_GAIN_CONSTITUTION:
      pline_The("water flow seems %ser now.",
                (obj->spe < 0) ? "less" : "great");
      break;
    case RIN_INCREASE_ACCURACY: /* KMH */
      pline_The("water flow %s the drain.", (obj->spe < 0) ? "misses" : "hits");
      break;
    case RIN_INCREASE_DAMAGE:
      pline_The("water's force seems %ser now.",
                (obj->spe < 0) ? "small" : "great");
      break;
    case RIN_HUNGER:
      ideed = FALSE;
      for (otmp = level.objects[player.ux][player.uy]; otmp; otmp = otmp2) {
        otmp2 = otmp->nexthere;
        if (otmp != uball && otmp != uchain && !obj_resists(otmp, 1, 99)) {
          if (!Blind) {
            pline("Suddenly, %s %s from the sink!", doname(otmp),
                  otense(otmp, "vanish"));
            ideed = TRUE;
          }
          delobj(otmp);
        }
      }
      break;
    case MEAT_RING:
      /* Not the same as aggravate monster; besides, it's obvious. */
      pline("Several flies buzz around the sink.");
      break;
    default:
      ideed = FALSE;
      break;
  }
  if (!Blind && !ideed && obj->otyp != RIN_HUNGER) {
    ideed = TRUE;
    switch (obj->otyp) { /* effects that need eyes */
      case RIN_ADORNMENT:
        pline_The("faucets flash brightly for a moment.");
        break;
      case RIN_REGENERATION:
        pline_The("sink looks as good as new.");
        break;
      case RIN_INVISIBILITY:
        You("don't see anything happen to the sink.");
        break;
      case RIN_FREE_ACTION:
        You("see the ring slide right down the drain!");
        break;
      case RIN_SEE_INVISIBLE:
        You("see some air in the sink.");
        break;
      case RIN_STEALTH:
        pline_The("sink seems to blend into the floor for a moment.");
        break;
      case RIN_FIRE_RESISTANCE:
        pline_The("hot water faucet flashes brightly for a moment.");
        break;
      case RIN_COLD_RESISTANCE:
        pline_The("cold water faucet flashes brightly for a moment.");
        break;
      case RIN_PROTECTION_FROM_SHAPE_CHAN:
        pline_The("sink looks nothing like a fountain.");
        break;
      case RIN_PROTECTION:
        pline_The("sink glows %s for a moment.",
                  hcolor((obj->spe < 0) ? NH_BLACK : NH_SILVER));
        break;
      case RIN_WARNING:
        pline_The("sink glows %s for a moment.", hcolor(NH_WHITE));
        break;
      case RIN_TELEPORTATION:
        pline_The("sink momentarily vanishes.");
        break;
      case RIN_TELEPORT_CONTROL:
        pline_The("sink looks like it is being beamed aboard somewhere.");
        break;
      case RIN_POLYMORPH:
        pline_The("sink momentarily looks like a fountain.");
        break;
      case RIN_POLYMORPH_CONTROL:
        pline_The("sink momentarily looks like a regularly erupting geyser.");
        break;
    }
  }
  if (ideed)
    trycall(obj);
  else
    You_hear("the ring bouncing down the drainpipe.");
  if (!rn2(20)) {
    pline_The("sink backs up, leaving %s.", doname(obj));
    obj->in_use = FALSE;
    dropx(obj);
  } else
    useup(obj);
}
#endif

#endif /* OVLB */
#ifdef OVL0

/* some common tests when trying to drop or throw items */
bool canletgo(Object *obj, const char *word) {
  if (obj->owornmask & (W_ARMOR | W_RING | W_AMUL | W_TOOL)) {
    if (*word)
      Norep("You cannot %s %s you are wearing.", word, something);
    return (FALSE);
  }
  if (obj->otyp == LOADSTONE && obj->cursed) {
    /* getobj() kludge sets corpsenm to user's specified count
       when refusing to split a stack of cursed loadstones */
    if (*word) {
      /* getobj() ignores a count for throwing since that is
         implicitly forced to be 1; replicate its kludge... */
      if (!strcmp(word, "throw") && obj->quan > 1L)
        obj->corpsenm = 1;
      pline("For some reason, you cannot %s%s the stone%s!", word,
            obj->corpsenm ? " any of" : "", plur(obj->quan));
    }
    obj->corpsenm = 0; /* reset */
    obj->bknown = 1;
    return (FALSE);
  }
  if (obj->otyp == LEASH && obj->leashmon != 0) {
    if (*word)
      pline_The("leash is tied around your %s.", body_part(HAND));
    return (FALSE);
  }
#ifdef STEED
  if (obj->owornmask & W_SADDLE) {
    if (*word)
      You("cannot %s %s you are sitting on.", word, something);
    return (FALSE);
  }
#endif
  return (TRUE);
}

STATIC_PTR
int drop(Object *obj) {
  if (!obj)
    return (0);
  if (!canletgo(obj, "drop"))
    return (0);
  if (obj == uwep) {
    if (welded(uwep)) {
      weldmsg(obj);
      return (0);
    }
    setuwep(nullptr);
  }
  if (obj == uquiver) {
    setuqwep(nullptr);
  }
  if (obj == uswapwep) {
    setuswapwep(nullptr);
  }

  if (player.uswallow) {
    /* barrier between you and the floor */
    if (flags.verbose) {
      char buf[BUFSZ];

      /* doname can call s_suffix, reusing its buffer */
      strcpy(buf, s_suffix(mon_nam(player.ustuck)));
      You("drop %s into %s %s.", doname(obj), buf,
          mbodypart(player.ustuck, STOMACH));
    }
  } else {
#ifdef SINKS
    if ((obj->oclass == RING_CLASS || obj->otyp == MEAT_RING) &&
        IS_SINK(levl[player.ux][player.uy].typ)) {
      dosinkring(obj);
      return (1);
    }
#endif
    if (!can_reach_floor()) {
      if (flags.verbose)
        You("drop %s.", doname(obj));
#ifndef GOLDOBJ
      if (obj->oclass != COIN_CLASS || obj == invent)
        freeinv(obj);
#else
      /* Ensure update when we drop gold objects */
      if (Object->oclass == COIN_CLASS)
        flags.botl = 1;
      freeinv(Object);
#endif
      hitfloor(obj);
      return (1);
    }
    if (!IS_ALTAR(levl[player.ux][player.uy].typ) && flags.verbose)
      You("drop %s.", doname(obj));
  }
  dropx(obj);
  return (1);
}

/* Called in several places - may produce output */
/* eg ship_object() and dropy() -> sellobj() both produce output */
void dropx(Object *obj) {
#ifndef GOLDOBJ
  if (obj->oclass != COIN_CLASS || obj == invent)
    freeinv(obj);
#else
  /* Ensure update when we drop gold objects */
  if (Object->oclass == COIN_CLASS)
    flags.botl = 1;
  freeinv(Object);
#endif
  if (!player.uswallow) {
    if (ship_object(obj, player.ux, player.uy, FALSE))
      return;
    if (IS_ALTAR(levl[player.ux][player.uy].typ))
      doaltarobj(obj); /* set bknown */
  }
  dropy(obj);
}

void dropy(Object *obj) {
  if (obj == uwep)
    setuwep(nullptr);
  if (obj == uquiver)
    setuqwep(nullptr);
  if (obj == uswapwep)
    setuswapwep(nullptr);

  if (!player.uswallow && flooreffects(obj, player.ux, player.uy, "drop"))
    return;
  /* uswallow check done by GAN 01/29/87 */
  if (player.uswallow) {
    bool could_petrify = FALSE;
    bool could_poly = FALSE;
    bool could_slime = FALSE;
    bool could_grow = FALSE;
    bool could_heal = FALSE;

    if (obj != uball) { /* mon doesn't pick up ball */
      if (obj->otyp == CORPSE) {
        could_petrify = touch_petrifies(&mons[obj->corpsenm]);
        could_poly = polyfodder(obj);
        could_slime = (obj->corpsenm == PM_GREEN_SLIME);
        could_grow = (obj->corpsenm == PM_WRAITH);
        could_heal = (obj->corpsenm == PM_NURSE);
      }
      (void)mpickobj(player.ustuck, obj);
      if (is_animal(player.ustuck->data)) {
        if (could_poly || could_slime) {
          (void)newcham(player.ustuck,
                        could_poly ? nullptr : &mons[PM_GREEN_SLIME],
                        FALSE, could_slime);
          delobj(obj); /* corpse is digested */
        } else if (could_petrify) {
          minstapetrify(player.ustuck, TRUE);
          /* Don't leave a cockatrice corpse in a statue */
          if (!player.uswallow)
            delobj(obj);
        } else if (could_grow) {
          (void)grow_up(player.ustuck, nullptr);
          delobj(obj); /* corpse is digested */
        } else if (could_heal) {
          player.ustuck->mhp = player.ustuck->mhpmax;
          delobj(obj); /* corpse is digested */
        }
      }
    }
  } else {
    PlaceObject(obj, player.ux, player.uy);
    if (obj == uball)
      drop_ball(player.ux, player.uy);
    else
      sellobj(obj, player.ux, player.uy);
    stackobj(obj);
    if (Blind && Levitation)
      map_object(obj, 0);
    newsym(player.ux, player.uy); /* remap location under self */
  }
}

/* things that must change when not held; recurse into containers.
   Called for both player and monsters */
void obj_no_longer_held(Object *obj) {
  if (!obj) {
    return;
  } else if ((Is_container(obj) || obj->otyp == STATUE) && obj->cobj) {
    Object *contents;
    for (contents = obj->cobj; contents; contents = contents->nobj)
      obj_no_longer_held(contents);
  }
  switch (obj->otyp) {
    case CRYSKNIFE:
      /* KMH -- Fixed crysknives have only 10% chance of reverting */
      /* only changes when not held by player or monster */
      if (!obj->oerodeproof || !rn2(10)) {
        obj->otyp = WORM_TOOTH;
        obj->oerodeproof = 0;
      }
      break;
  }
}

/* 'D' command: drop several things */
int doddrop() {
  int result = 0;

  add_valid_menu_class(0); /* clear any classes already there */
  if (*player.ushops)
    sellobj_state(SELL_DELIBERATE);
  if (flags.menu_style != MENU_TRADITIONAL ||
      (result = ggetobj("drop", drop, 0, FALSE, nullptr)) < -1)
    result = menu_drop(result);
  if (*player.ushops)
    sellobj_state(SELL_NORMAL);
  reset_occupations();

  return result;
}

/* Drop things from the hero's inventory, using a menu. */
STATIC_OVL int menu_drop(int retry) {
  int n, i, n_dropped = 0;
  long cnt;
  Object *otmp, *otmp2;
#ifndef GOLDOBJ
  Object *u_gold = 0;
#endif
  menu_item *pick_list;
  bool all_categories = TRUE;
  bool drop_everything = FALSE;

#ifndef GOLDOBJ
  if (player.ugold) {
    /* Hack: gold is not in the inventory, so make a gold object
       and put it at the head of the inventory list. */
    u_gold = mkgoldobj(player.ugold); /* removes from player.ugold */
    u_gold->in_use = TRUE;
    player.ugold = u_gold->quan; /* put the gold back */
    assigninvlet(u_gold);        /* might end up as NOINVSYM */
    u_gold->nobj = invent;
    invent = u_gold;
  }
#endif
  if (retry) {
    all_categories = (retry == -2);
  } else if (flags.menu_style == MENU_FULL) {
    all_categories = FALSE;
    n = query_category("Drop what type of items?", invent,
                       UNPAID_TYPES | ALL_TYPES | CHOOSE_ALL | BUC_BLESSED |
                           BUC_CURSED | BUC_UNCURSED | BUC_UNKNOWN,
                       &pick_list, PICK_ANY);
    if (!n)
      goto drop_done;
    for (i = 0; i < n; i++) {
      if (pick_list[i].item.a_int == ALL_TYPES_SELECTED)
        all_categories = TRUE;
      else if (pick_list[i].item.a_int == 'A')
        drop_everything = TRUE;
      else
        add_valid_menu_class(pick_list[i].item.a_int);
    }
    free((genericptr_t)pick_list);
  } else if (flags.menu_style == MENU_COMBINATION) {
    unsigned ggoresults = 0;
    all_categories = FALSE;
    /* Gather valid classes via traditional NetHack method */
    i = ggetobj("drop", drop, 0, TRUE, &ggoresults);
    if (i == -2)
      all_categories = TRUE;
    if (ggoresults & ALL_FINISHED) {
      n_dropped = i;
      goto drop_done;
    }
  }

  if (drop_everything) {
    for (otmp = invent; otmp; otmp = otmp2) {
      otmp2 = otmp->nobj;
      n_dropped += drop(otmp);
    }
  } else {
    /* should coordinate with perm invent, maybe not show worn items */
    n = query_objlist("What would you like to drop?", invent,
                      USE_INVLET | INVORDER_SORT, &pick_list, PICK_ANY,
                      all_categories ? allow_all : allow_category);
    if (n > 0) {
      for (i = 0; i < n; i++) {
        otmp = pick_list[i].item.a_obj;
        cnt = pick_list[i].count;
        if (cnt < otmp->quan) {
          if (welded(otmp)) {
            ; /* don't split */
          } else if (otmp->otyp == LOADSTONE && otmp->cursed) {
            /* same kludge as getobj(), for canletgo()'s use */
            otmp->corpsenm = (int)cnt; /* don't split */
          } else {
#ifndef GOLDOBJ
            if (otmp->oclass == COIN_CLASS)
              (void)SplitObject(otmp, otmp->quan - cnt);
            else
#endif
              otmp = SplitObject(otmp, cnt);
          }
        }
        n_dropped += drop(otmp);
      }
      free((genericptr_t)pick_list);
    }
  }

drop_done:
#ifndef GOLDOBJ
  if (u_gold && invent && invent->oclass == COIN_CLASS) {
    /* didn't drop [all of] it */
    u_gold = invent;
    invent = u_gold->nobj;
    u_gold->in_use = FALSE;
    DeallocateObject(u_gold);
    update_inventory();
  }
#endif
  return n_dropped;
}

#endif /* OVL0 */
#ifdef OVL2

/* on a ladder, used in goto_level */
static bool at_ladder = FALSE;

int dodown() {
  Trap *trap = 0;
  bool stairs_down = ((player.ux == xdnstair && player.uy == ydnstair) ||
                      (player.ux == sstairs.sx && player.uy == sstairs.sy &&
                       !sstairs.up)),
       ladder_down = (player.ux == xdnladder && player.uy == ydnladder);

#ifdef STEED
  if (player.usteed && !player.usteed->mcanmove) {
    pline("%s won't move!", Monnam(player.usteed));
    return (0);
  } else if (player.usteed && player.usteed->meating) {
    pline("%s is still eating.", Monnam(player.usteed));
    return (0);
  } else
#endif
      if (Levitation) {
    if ((HLevitation & I_SPECIAL) || (ELevitation & W_ARTI)) {
      /* end controlled levitation */
      if (ELevitation & W_ARTI) {
        Object *obj;

        for (obj = invent; obj; obj = obj->nobj) {
          if (obj->oartifact && artifact_has_invprop(obj, LEVITATION)) {
            if (obj->age < monstermoves)
              obj->age = monstermoves + rnz(100);
            else
              obj->age += rnz(100);
          }
        }
      }
      if (float_down(I_SPECIAL | TIMEOUT, W_ARTI))
        return (1); /* came down, so moved */
    }
    floating_above(stairs_down ? "stairs"
                               : ladder_down ? "ladder"
                                             : surface(player.ux, player.uy));
    return (0); /* didn't move */
  }
  if (!stairs_down && !ladder_down) {
    if (!(trap = t_at(player.ux, player.uy)) ||
        (trap->ttyp != TRAPDOOR && trap->ttyp != HOLE) ||
        !Can_fall_thru(&player.uz) || !trap->tseen) {
      if (flags.autodig && !flags.nopick && uwep && is_pick(uwep)) {
        return use_pick_axe2(uwep);
      } else {
        You_cant("go down here.");
        return (0);
      }
    }
  }
  if (player.ustuck) {
    You("are %s, and cannot go down.",
        !player.uswallow ? "being held" : is_animal(player.ustuck->data)
                                              ? "swallowed"
                                              : "engulfed");
    return (1);
  }
  if (on_level(&valley_level, &player.uz) && !player.uevent.gehennom_entered) {
    You("are standing at the gate to Gehennom.");
    pline("Unspeakable cruelty and harm lurk down there.");
    if (yn("Are you sure you want to enter?") != 'y')
      return (0);
    else
      pline("So be it.");
    player.uevent.gehennom_entered = 1; /* don't ask again */
  }

  if (!next_to_u()) {
    You("are held back by your pet!");
    return (0);
  }

  if (trap)
    You("%s %s.", locomotion(youmonst.data, "jump"),
        trap->ttyp == HOLE ? "down the hole" : "through the trap door");

  if (trap && Is_stronghold(&player.uz)) {
    goto_hell(FALSE, TRUE);
  } else {
    at_ladder = (bool)(levl[player.ux][player.uy].typ == LADDER);
    next_level(!trap);
    at_ladder = FALSE;
  }
  return (1);
}

int doup() {
  if ((player.ux != xupstair || player.uy != yupstair) &&
      (!xupladder || player.ux != xupladder || player.uy != yupladder) &&
      (!sstairs.sx || player.ux != sstairs.sx || player.uy != sstairs.sy ||
       !sstairs.up)) {
    You_cant("go up here.");
    return (0);
  }
#ifdef STEED
  if (player.usteed && !player.usteed->mcanmove) {
    pline("%s won't move!", Monnam(player.usteed));
    return (0);
  } else if (player.usteed && player.usteed->meating) {
    pline("%s is still eating.", Monnam(player.usteed));
    return (0);
  } else
#endif
      if (player.ustuck) {
    You("are %s, and cannot go up.",
        !player.uswallow ? "being held" : is_animal(player.ustuck->data)
                                              ? "swallowed"
                                              : "engulfed");
    return (1);
  }
  if (near_capacity() > SLT_ENCUMBER) {
    /* No levitation check; inv_weight() already allows for it */
    Your("load is too heavy to climb the %s.",
         levl[player.ux][player.uy].typ == STAIRS ? "stairs" : "ladder");
    return (1);
  }
  if (ledger_no(&player.uz) == 1) {
    if (yn("Beware, there will be no return! Still climb?") != 'y')
      return (0);
  }
  if (!next_to_u()) {
    You("are held back by your pet!");
    return (0);
  }
  at_ladder = (bool)(levl[player.ux][player.uy].typ == LADDER);
  prev_level(TRUE);
  at_ladder = FALSE;
  return (1);
}

d_level save_dlevel = {0, 0};

/* check that we can write out the current level */
STATIC_OVL int currentlevel_rewrite() {
  int fd;
  char whynot[BUFSZ];

  /* since level change might be a bit slow, flush any buffered screen
   *  output (like "you fall through a trap door") */
  mark_synch();

  fd = create_levelfile(ledger_no(&player.uz), whynot);
  if (fd < 0) {
    /*
     * This is not quite impossible: e.g., we may have
     * exceeded our quota. If that is the case then we
     * cannot leave this level, and cannot save either.
     * Another possibility is that the directory was not
     * writable.
     */
    pline("%s", whynot);
    return -1;
  }

  return fd;
}

#ifdef INSURANCE
void save_currentstate() {
  if (flags.ins_chkpt) {
    /* write out just-attained level, with pets and everything */
    int fd = currentlevel_rewrite();
    if (fd < 0)
      return;
    bufon(fd);
    savelev(fd, ledger_no(&player.uz), WRITE_SAVE);
    bclose(fd);
  }

  /* write out non-level state */
  savestateinlock();
}
#endif

/*
static bool badspot(xchar x, xchar y) {
        return((levl[x][y].typ != ROOM && levl[x][y].typ != AIR &&
                         levl[x][y].typ != CORR) || MON_AT(x, y));
}
*/

void goto_level(d_level *newlevel, bool at_stairs, bool falling, bool portal) {
  int fd, l_idx;
  xchar new_ledger;
  bool cant_go_back,
      up = (depth(newlevel) < depth(&player.uz)),
      newdungeon = (player.uz.dnum != newlevel->dnum),
      was_in_W_tower = In_W_tower(player.ux, player.uy, &player.uz),
      familiar = FALSE;
  bool is_new = FALSE; /* made a is_new level? */
  Monster *mtmp;
  char whynot[BUFSZ];

  if (dunlev(newlevel) > dunlevs_in_dungeon(newlevel))
    newlevel->dlevel = dunlevs_in_dungeon(newlevel);
  if (newdungeon && In_endgame(newlevel)) { /* 1st Endgame Level !!! */
    if (player.uhave.amulet)
      assign_level(newlevel, &earth_level);
    else
      return;
  }
  new_ledger = ledger_no(newlevel);
  if (new_ledger <= 0)
    done(ESCAPED); /* in fact < 0 is impossible */

  /* If you have the amulet and are trying to get out of Gehennom, going
   * up a set of stairs sometimes does some very strange things!
   * Biased against law and towards chaos, but not nearly as strongly
   * as it used to be (prior to 3.2.0).
   * Odds:	    old				    is_new
   *	"up"    L      N      C		"up"    L      N      C
   *	 +1   75.0   75.0   75.0	 +1   75.0   75.0   75.0
   *	  0    0.0   12.5   25.0	  0    6.25   8.33  12.5
   *	 -1    8.33   4.17   0.0	 -1    6.25   8.33  12.5
   *	 -2    8.33   4.17   0.0	 -2    6.25   8.33   0.0
   *	 -3    8.33   4.17   0.0	 -3    6.25   0.0    0.0
   */
  if (Inhell && up && player.uhave.amulet && !newdungeon && !portal &&
      (dunlev(&player.uz) < dunlevs_in_dungeon(&player.uz) - 3)) {
    if (!rn2(4)) {
      int odds = 3 + (int)player.ualign.type, /* 2..4 */
          diff = odds <= 1 ? 0 : rn2(odds);   /* paranoia */

      if (diff != 0) {
        assign_rnd_level(newlevel, &player.uz, diff);
        /* if inside the tower, stay inside */
        if (was_in_W_tower && !On_W_tower_level(newlevel))
          diff = 0;
      }
      if (diff == 0)
        assign_level(newlevel, &player.uz);

      new_ledger = ledger_no(newlevel);

      pline("A mysterious force momentarily surrounds you...");
      if (on_level(newlevel, &player.uz)) {
        (void)safe_teleds(FALSE);
        (void)next_to_u();
        return;
      } else
        at_stairs = at_ladder = FALSE;
    }
  }

  /* Prevent the player from going past the first quest level unless
   * (s)he has been given the go-ahead by the leader.
   */
  if (on_level(&player.uz, &qstart_level) && !newdungeon && !ok_to_quest()) {
    pline("A mysterious force prevents you from descending.");
    return;
  }

  if (on_level(newlevel, &player.uz))
    return; /* this can happen */

  fd = currentlevel_rewrite();
  if (fd < 0)
    return;

  if (falling) /* assuming this is only trap door or hole */
    impact_drop(nullptr, player.ux, player.uy, newlevel->dlevel);

  check_special_room(TRUE); /* probably was a trap door */
  if (Punished)
    unplacebc();
  player.utrap = 0; /* needed in level_tele */
  fill_pit(player.ux, player.uy);
  player.ustuck = 0; /* idem */
  player.uinwater = 0;
  player.uundetected = 0; /* not hidden, even if means are available */
  keepdogs(FALSE);
  if (player.uswallow) /* idem */
    player.uswldtim = player.uswallow = 0;
  /*
   *  We no longer see anything on the level.  Make sure that this
   *  follows player.uswallow set to null since uswallow overrides all
   *  normal vision.
   */
  vision_recalc(2);

  /*
   * Save the level we're leaving.  If we're entering the endgame,
   * we can get rid of all existing levels because they cannot be
   * reached any more.  We still need to use savelev()'s cleanup
   * for the level being left, to recover dynamic memory in use and
   * to avoid dangling timers and light sources.
   */
  cant_go_back = (newdungeon && In_endgame(newlevel));
  if (!cant_go_back) {
    update_mlstmv(); /* current monsters are becoming inactive */
    bufon(fd);       /* use buffered output */
  }
  savelev(fd, ledger_no(&player.uz),
          cant_go_back ? FREE_SAVE : (WRITE_SAVE | FREE_SAVE));
  bclose(fd);
  if (cant_go_back) {
    /* discard unreachable levels; keep #0 */
    for (l_idx = maxledgerno(); l_idx > 0; --l_idx)
      delete_levelfile(l_idx);
  }

#ifdef REINCARNATION
  if (Is_rogue_level(newlevel) || Is_rogue_level(&player.uz))
    assign_rogue_graphics(Is_rogue_level(newlevel));
#endif
#ifdef USE_TILES
  substitute_tiles(newlevel);
#endif
  assign_level(&player.uz0, &player.uz);
  assign_level(&player.uz, newlevel);
  assign_level(&player.utolev, newlevel);
  player.utotype = 0;
  if (dunlev_reached(&player.uz) < dunlev(&player.uz))
    dunlev_reached(&player.uz) = dunlev(&player.uz);
  reset_rndmonst(NON_PM); /* player.uz change affects monster generation */

  /* set default level change destination areas */
  /* the special level code may override these */
  (void)memset((genericptr_t) & updest, 0, sizeof updest);
  (void)memset((genericptr_t) & dndest, 0, sizeof dndest);

  if (!(level_info[new_ledger].flags & LFILE_EXISTS)) {
    /* entering this level for first time; make it now */
    if (level_info[new_ledger].flags & (FORGOTTEN | VISITED)) {
      impossible("goto_level: returning to discarded level?");
      level_info[new_ledger].flags &= ~(FORGOTTEN | VISITED);
    }
    mklev();
    is_new = TRUE; /* made the level */
  } else {
    /* returning to previously visited level; reload it */
    fd = open_levelfile(new_ledger, whynot);
    if (fd < 0) {
      pline("%s", whynot);
      pline("Probably someone removed it.");
      killer = whynot;
      done(TRICKED);
      /* we'll reach here if running in wizard mode */
      error("Cannot continue this game.");
    }
    minit(); /* ZEROCOMP */
    getlev(fd, hackpid, new_ledger, FALSE);
    (void)close(fd);
  }
  /* do this prior to level-change pline messages */
  vision_reset();         /* clear old level's line-of-sight */
  vision_full_recalc = 0; /* don't let that reenable vision yet */
  flush_screen(-1);       /* ensure all map flushes are postponed */

  if (portal && !In_endgame(&player.uz)) {
    /* find the portal on the is_new level */
    Trap *ttrap;

    for (ttrap = ftrap; ttrap; ttrap = ttrap->ntrap)
      if (ttrap->ttyp == MAGIC_PORTAL)
        break;

    if (!ttrap)
      panic("goto_level: no corresponding portal!");
    seetrap(ttrap);
    u_on_newpos(ttrap->tx, ttrap->ty);
  } else if (at_stairs && !In_endgame(&player.uz)) {
    if (up) {
      if (at_ladder) {
        u_on_newpos(xdnladder, ydnladder);
      } else {
        if (newdungeon) {
          if (Is_stronghold(&player.uz)) {
            xchar x, y;

            do {
              x = (COLNO - 2 - rnd(5));
              y = rn1(ROWNO - 4, 3);
            } while (occupied(x, y) || IS_WALL(levl[x][y].typ));
            u_on_newpos(x, y);
          } else
            u_on_sstairs();
        } else
          u_on_dnstairs();
      }
      /* Remove bug which crashes with levitation/punishment  KAA */
      if (Punished && !Levitation) {
        pline("With great effort you climb the %s.",
              at_ladder ? "ladder" : "stairs");
      } else if (at_ladder)
        You("climb up the ladder.");
    } else { /* down */
      if (at_ladder) {
        u_on_newpos(xupladder, yupladder);
      } else {
        if (newdungeon)
          u_on_sstairs();
        else
          u_on_upstairs();
      }
      if (player.dz && Flying)
        You("fly down along the %s.", at_ladder ? "ladder" : "stairs");
      else if (player.dz &&
               (near_capacity() > UNENCUMBERED || Punished || Fumbling)) {
        You("fall down the %s.", at_ladder ? "ladder" : "stairs");
        if (Punished) {
          drag_down();
          if (carried(uball)) {
            if (uwep == uball)
              setuwep(nullptr);
            if (uswapwep == uball)
              setuswapwep(nullptr);
            if (uquiver == uball)
              setuqwep(nullptr);
            freeinv(uball);
          }
        }
#ifdef STEED
        /* falling off steed has its own losehp() call */
        if (player.usteed)
          dismount_steed(DISMOUNT_FELL);
        else
#endif
          losehp(rnd(3), "falling downstairs", KILLED_BY);
        selftouch("Falling, you");
      } else if (player.dz && at_ladder)
        You("climb down the ladder.");
    }
  } else { /* trap door or level_tele or In_endgame */
    if (was_in_W_tower && On_W_tower_level(&player.uz))
      /* Stay inside the Wizard's tower when feasible.	*/
      /* Note: up vs down doesn't really matter in this case. */
      place_lregion(dndest.nlx, dndest.nly, dndest.nhx, dndest.nhy, 0, 0, 0, 0,
                    LR_DOWNTELE, nullptr);
    else if (up)
      place_lregion(updest.lx, updest.ly, updest.hx, updest.hy, updest.nlx,
                    updest.nly, updest.nhx, updest.nhy, LR_UPTELE,
                    nullptr);
    else
      place_lregion(dndest.lx, dndest.ly, dndest.hx, dndest.hy, dndest.nlx,
                    dndest.nly, dndest.nhx, dndest.nhy, LR_DOWNTELE,
                    nullptr);
    if (falling) {
      if (Punished)
        ballfall();
      selftouch("Falling, you");
    }
  }

  if (Punished)
    placebc();
  obj_delivery(); /* before killing geno'd monsters' eggs */
  losedogs();
  kill_genocided_monsters(); /* for those wiped out while in limbo */
                             /*
                              * Expire all timers that have gone off while away.  Must be
                              * after migrating monsters and objects are delivered
                              * (losedogs and obj_delivery).
                              */
  run_timers();

  initrack();

  if ((mtmp = m_at(player.ux, player.uy)) != 0
#ifdef STEED
      && mtmp != player.usteed
#endif
      ) {
    /* There's a monster at your target destination; it might be one
       which accompanied you--see mon_arrive(dogmove.c)--or perhaps
       it was already here.  Randomly move you to an adjacent spot
       or else the monster to any nearby location.  Prior to 3.3.0
       the latter was done unconditionally. */
    coord cc;

    if (!rn2(2) && enexto(&cc, player.ux, player.uy, youmonst.data) &&
        distu(cc.x, cc.y) <= 2)
      u_on_newpos(cc.x, cc.y); /*[maybe give message here?]*/
    else
      mnexto(mtmp);

    if ((mtmp = m_at(player.ux, player.uy)) != 0) {
      impossible("mnexto failed (do.c)?");
      (void)rloc(mtmp, FALSE);
    }
  }

  /* initial movement of bubbles just before vision_recalc */
  if (Is_waterlevel(&player.uz))
    movebubbles();

  if (level_info[new_ledger].flags & FORGOTTEN) {
    forget_map(ALL_MAP); /* forget the map */
    forget_traps();      /* forget all traps too */
    familiar = TRUE;
    level_info[new_ledger].flags &= ~FORGOTTEN;
  }

  /* Reset the screen. */
  vision_reset(); /* reset the blockages */
  docrt();        /* does a full vision recalc */
  flush_screen(-1);

  /*
   *  Move all plines beyond the screen reset.
   */

  /* give room entrance message, if any */
  check_special_room(FALSE);

  /* Check whether we just entered Gehennom. */
  if (!In_hell(&player.uz0) && Inhell) {
    if (Is_valley(&player.uz)) {
      You("arrive at the Valley of the Dead...");
      pline_The("odor of burnt flesh and decay pervades the air.");
      You_hear("groans and moans everywhere.");
    } else
      pline("It is hot here.  You smell smoke...");

#ifdef RECORD_ACHIEVE
    achieve.enter_gehennom = 1;
#endif
  }

  if (familiar) {
    static const char *const fam_msgs[4] = {
        "You have a sense of deja vu.",
        "You feel like you've been here before.", "This place %s familiar...",
        0 /* no message */
    };
    static const char *const halu_fam_msgs[4] = {
        "Whoa!  Everything %s different.",
        "You are surrounded by twisty little passages, all alike.",
        "Gee, this %s like uncle Conan's place...", 0 /* no message */
    };
    const char *mesg;
    char buf[BUFSZ];
    int which = rn2(4);

    if (Hallucination)
      mesg = halu_fam_msgs[which];
    else
      mesg = fam_msgs[which];
    if (mesg && index(mesg, '%')) {
      sprintf(buf, mesg, !Blind ? "looks" : "seems");
      mesg = buf;
    }
    if (mesg)
      pline(mesg);
  }

#ifdef REINCARNATION
  if (is_new && Is_rogue_level(&player.uz))
    You("enter what seems to be an older, more primitive world.");
#endif
  /* Final confrontation */
  if (In_endgame(&player.uz) && newdungeon && player.uhave.amulet)
    resurrect();
  if (newdungeon && In_V_tower(&player.uz) && In_hell(&player.uz0))
    pline_The("heat and smoke are gone.");

  /* the message from your quest leader */
  if (!In_quest(&player.uz0) && at_dgn_entrance("The Quest") &&
      !(player.uevent.qexpelled || player.uevent.qcompleted ||
        quest_status.leader_is_dead)) {
    if (player.uevent.qcalled) {
      com_pager(Role_if(PM_ROGUE) ? 4 : 3);
    } else {
      com_pager(2);
      player.uevent.qcalled = TRUE;
    }
  }

  /* once Croesus is dead, his alarm doesn't work any more */
  if (Is_knox(&player.uz) && (is_new || !mvitals[PM_CROESUS].died)) {
    You("penetrated a high security area!");
    pline("An alarm sounds!");
    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon)
      if (!mtmp->dead() && mtmp->msleeping)
        mtmp->msleeping = 0;
  }

  if (on_level(&player.uz, &astral_level))
    final_level();
  else
    onquest();
  assign_level(&player.uz0, &player.uz); /* reset player.uz0 */

#ifdef INSURANCE
  save_currentstate();
#endif

  /* assume this will always return TRUE when changing level */
  (void)in_out_region(player.ux, player.uy);
  (void)pickup(1);
}

STATIC_OVL void final_level() {
  Monster *mtmp;
  Object *otmp;
  coord mm;
  int i;

  /* reset monster hostility relative to player */
  for (mtmp = fmon; mtmp; mtmp = mtmp->nmon)
    if (!mtmp->dead())
      reset_hostility(mtmp);

  /* create some player-monsters */
  create_mplayers(rn1(4, 3), TRUE);

  /* create a guardian angel next to player, if worthy */
  if (Conflict) {
    pline("A voice booms: \"Thy desire for conflict shall be fulfilled!\"");
    for (i = rnd(4); i > 0; --i) {
      mm.x = player.ux;
      mm.y = player.uy;
      if (enexto(&mm, mm.x, mm.y, &mons[PM_ANGEL]))
        (void)mk_roamer(&mons[PM_ANGEL], player.ualign.type, mm.x, mm.y, FALSE);
    }

  } else if (player.ualign.record > 8) { /* fervent */
    pline("A voice whispers: \"Thou hast been worthy of me!\"");
    mm.x = player.ux;
    mm.y = player.uy;
    if (enexto(&mm, mm.x, mm.y, &mons[PM_ANGEL])) {
      if ((mtmp = mk_roamer(&mons[PM_ANGEL], player.ualign.type, mm.x, mm.y,
                            TRUE)) != 0) {
        if (!Blind)
          pline("An angel appears near you.");
        else
          You_feel("the presence of a friendly angel near you.");
        /* guardian angel -- the one case mtame doesn't
         * imply an edog structure, so we don't want to
         * call tamedog().
         */
        mtmp->mtame = 10;
        /* make him strong enough vs. endgame foes */
        mtmp->m_lev = rn1(8, 15);
        mtmp->mhp = mtmp->mhpmax = d((int)mtmp->m_lev, 10) + 30 + rnd(30);
        if ((otmp = select_hwep(mtmp)) == 0) {
          otmp = MakeSpecificObject(SILVER_SABER, FALSE, FALSE);
          if (mpickobj(mtmp, otmp))
            panic("merged weapon?");
        }
        Bless(otmp);
        if (otmp->spe < 4)
          otmp->spe += rnd(4);
        if ((otmp = which_armor(mtmp, W_ARMS)) == 0 ||
            otmp->otyp != SHIELD_OF_REFLECTION) {
          (void)mongets(mtmp, AMULET_OF_REFLECTION);
          m_dowear(mtmp, TRUE);
        }
      }
    }
  }
}

static char *dfr_pre_msg = 0, /* pline() before level change */
    *dfr_post_msg = 0;        /* pline() after level change */

/* change levels at the end of this turn, after monsters finish moving */
void schedule_goto(d_level *tolev, bool at_stairs, bool falling,
                   int portal_flag, const char *pre_msg, const char *post_msg) {
  int typmask = 0100; /* non-zero triggers `deferred_goto' */

  /* destination flags (`goto_level' args) */
  if (at_stairs)
    typmask |= 1;
  if (falling)
    typmask |= 2;
  if (portal_flag)
    typmask |= 4;
  if (portal_flag < 0)
    typmask |= 0200; /* flag for portal removal */
  player.utotype = typmask;
  /* destination level */
  assign_level(&player.utolev, tolev);

  if (pre_msg)
    dfr_pre_msg = strcpy((char *)alloc(strlen(pre_msg) + 1), pre_msg);
  if (post_msg)
    dfr_post_msg = strcpy((char *)alloc(strlen(post_msg) + 1), post_msg);
}

/* handle something like portal ejection */
void deferred_goto() {
  if (!on_level(&player.uz, &player.utolev)) {
    d_level dest;
    int typmask =
        player.utotype; /* save it; goto_level zeroes player.utotype */

    assign_level(&dest, &player.utolev);
    if (dfr_pre_msg)
      pline(dfr_pre_msg);
    goto_level(&dest, !!(typmask & 1), !!(typmask & 2), !!(typmask & 4));
    if (typmask & 0200) { /* remove portal */
      Trap *t = t_at(player.ux, player.uy);

      if (t) {
        deltrap(t);
        newsym(player.ux, player.uy);
      }
    }
    if (dfr_post_msg)
      pline(dfr_post_msg);
  }
  player.utotype = 0; /* our caller keys off of this */
  if (dfr_pre_msg)
    free((genericptr_t)dfr_pre_msg), dfr_pre_msg = 0;
  if (dfr_post_msg)
    free((genericptr_t)dfr_post_msg), dfr_post_msg = 0;
}

#endif /* OVL2 */
#ifdef OVL3

/*
 * Return TRUE if we created a monster for the corpse.  If successful, the
 * corpse is gone.
 */
bool revive_corpse(Object *corpse) {
  Monster *mtmp, *mcarry;
  bool is_uwep, chewed;
  xchar where;
  char *cname, cname_buf[BUFSZ];
  Object *container = nullptr;
  int container_where = 0;

  where = corpse->where;
  is_uwep = corpse == uwep;
  cname = eos(strcpy(cname_buf, "bite-covered "));
  strcpy(cname, corpse_xname(corpse, TRUE));
  mcarry = (where == OBJ_MINVENT) ? corpse->ocarry : 0;

  if (where == OBJ_CONTAINED) {
    Monster *mtmp2 = nullptr;
    container = corpse->ocontainer;
    mtmp2 = get_container_location(container, &container_where, nullptr);
    /* container_where is the outermost container's location even if nested */
    if (container_where == OBJ_MINVENT && mtmp2)
      mcarry = mtmp2;
  }
  mtmp = revive(corpse); /* corpse is gone if successful */

  if (mtmp) {
    chewed = (mtmp->mhp < mtmp->mhpmax);
    if (chewed)
      cname = cname_buf; /* include "bite-covered" prefix */
    switch (where) {
      case OBJ_INVENT:
        if (is_uwep)
          pline_The("%s writhes out of your grasp!", cname);
        else
          You_feel("squirming in your backpack!");
        break;

      case OBJ_FLOOR:
        if (cansee(mtmp->mx, mtmp->my))
          pline("%s rises from the dead!",
                chewed ? Adjmonnam(mtmp, "bite-covered") : Monnam(mtmp));
        break;

      case OBJ_MINVENT: /* probably a nymph's */
        if (cansee(mtmp->mx, mtmp->my)) {
          if (canseemon(mcarry))
            pline("Startled, %s drops %s as it revives!", mon_nam(mcarry),
                  an(cname));
          else
            pline("%s suddenly appears!",
                  chewed ? Adjmonnam(mtmp, "bite-covered") : Monnam(mtmp));
        }
        break;
      case OBJ_CONTAINED:
        if (container_where == OBJ_MINVENT && cansee(mtmp->mx, mtmp->my) &&
            mcarry && canseemon(mcarry) && container) {
          char sackname[BUFSZ];
          sprintf(sackname, "%s %s", s_suffix(mon_nam(mcarry)),
                  xname(container));
          pline("%s writhes out of %s!", Amonnam(mtmp), sackname);
        } else if (container_where == OBJ_INVENT && container) {
          char sackname[BUFSZ];
          strcpy(sackname, an(xname(container)));
          pline("%s %s out of %s in your pack!",
                Blind ? Something : Amonnam(mtmp),
                locomotion(mtmp->data, "writhes"), sackname);
        } else if (container_where == OBJ_FLOOR && container &&
                   cansee(mtmp->mx, mtmp->my)) {
          char sackname[BUFSZ];
          strcpy(sackname, an(xname(container)));
          pline("%s escapes from %s!", Amonnam(mtmp), sackname);
        }
        break;
      default:
        /* we should be able to handle the other cases... */
        impossible("revive_corpse: lost corpse @ %d", where);
        break;
    }
    return TRUE;
  }
  return FALSE;
}

/* Revive the corpse via a timeout. */
/*ARGSUSED*/
void revive_mon(genericptr_t arg, long timeout) {
  Object *body = (Object *)arg;

  /* if we succeed, the corpse is gone, otherwise, rot it away */
  if (!revive_corpse(body)) {
    if (is_rider(&mons[body->corpsenm]))
      You_feel("less hassled.");
    (void)start_timer(250L - (monstermoves - body->age), TIMER_OBJECT,
                      ROT_CORPSE, arg);
  }
}

int donull() { return (1); /* Do nothing, but let other things happen */ }

#endif /* OVL3 */
#ifdef OVLB

STATIC_PTR int wipeoff() {
  if (player.ucreamed < 4)
    player.ucreamed = 0;
  else
    player.ucreamed -= 4;
  if (Blinded < 4)
    Blinded = 0;
  else
    Blinded -= 4;
  if (!Blinded) {
    pline("You've got the glop off.");
    player.ucreamed = 0;
    Blinded = 1;
    make_blinded(0L, TRUE);
    return (0);
  } else if (!player.ucreamed) {
    Your("%s feels clean now.", body_part(FACE));
    return (0);
  }
  return (1); /* still busy */
}

int dowipe() {
  if (player.ucreamed) {
    static char buf[39];

    sprintf(buf, "wiping off your %s", body_part(FACE));
    set_occupation(wipeoff, buf, 0);
    /* Not totally correct; what if they change back after now
     * but before they're finished wiping?
     */
    return (1);
  }
  Your("%s is already clean.", body_part(FACE));
  return (1);
}

void set_wounded_legs(long side, int timex) {
  /* KMH -- STEED
   * If you are riding, your steed gets the wounded legs instead.
   * You still call this function, but don't lose hp.
   * Caller is also responsible for adjusting messages.
   */

  if (!Wounded_legs) {
    ATEMP(A_DEX)--;
    flags.botl = 1;
  }

  if (!Wounded_legs || (HWounded_legs & TIMEOUT))
    HWounded_legs = timex;
  EWounded_legs = side;
  (void)encumber_msg();
}

void heal_legs() {
  if (Wounded_legs) {
    if (ATEMP(A_DEX) < 0) {
      ATEMP(A_DEX)++;
      flags.botl = 1;
    }

#ifdef STEED
    if (!player.usteed)
#endif
    {
      /* KMH, intrinsics patch */
      if ((EWounded_legs & BOTH_SIDES) == BOTH_SIDES) {
        Your("%s feel somewhat better.", makeplural(body_part(LEG)));
      } else {
        Your("%s feels somewhat better.", body_part(LEG));
      }
    }
    HWounded_legs = EWounded_legs = 0;
  }
  (void)encumber_msg();
}

#endif /* OVLB */

/*do.c*/