/*	SCCS Id: @(#)detect.c	3.4	2003/08/13	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/*
 * Detection routines, including crystal ball, magic mapping, and search
 * command.
 */

#include "core/detect.h"

#include <string.h>

#include "core/hack.h"
#include "core/decl.h"
#include "core/youprop.h"
#include "core/mkroom.h"
#include "core/worm.h"
#include "core/vault.h"
#include "core/rnd.h"
#include "core/potion.h"
#include "core/polyself.h"
#include "core/pline.h"
#include "core/objnam.h"
#include "core/o_init.h"
#include "core/mon.h"
#include "core/mhitu.h"
#include "core/invent.h"
#include "core/hacklib.h"
#include "core/drawing.h"
#include "core/do_name.h"
#include "core/dbridge.h"
#include "core/cmd.h"
#include "core/artifact.h"

extern bool known; /* from read.c */

STATIC_DCL void do_dknown_of(Object *);
STATIC_DCL bool check_map_spot(int, int, char, unsigned);
STATIC_DCL bool clear_stale_map(char, unsigned);
STATIC_DCL void sense_trap(Trap *, xchar, xchar, int);
STATIC_DCL void show_map_spot(int, int);
STATIC_PTR void findone(int, int, genericptr_t);
STATIC_PTR void openone(int, int, genericptr_t);

/* Recursively search obj for an object in class_id oclass and return 1st found
 */
Object *o_in(Object *obj, char oclass) {
  Object *otmp;
  Object *temp;

  if (obj->oclass == oclass)
    return obj;

  if (Has_contents(obj)) {
    for (otmp = obj->cobj; otmp; otmp = otmp->nobj)
      if (otmp->oclass == oclass)
        return otmp;
      else if (Has_contents(otmp) && (temp = o_in(otmp, oclass)))
        return temp;
  }
  return nullptr;
}

/* Recursively search obj for an object made of specified material and return
 * 1st found */
Object *o_material(Object *obj, unsigned material) {
  Object *otmp;
  Object *temp;

  if (objects[obj->otyp].oc_material == material)
    return obj;

  if (Has_contents(obj)) {
    for (otmp = obj->cobj; otmp; otmp = otmp->nobj)
      if (objects[otmp->otyp].oc_material == material)
        return otmp;
      else if (Has_contents(otmp) && (temp = o_material(otmp, material)))
        return temp;
  }
  return nullptr;
}

STATIC_OVL void do_dknown_of(Object *obj) {
  Object *otmp;

  obj->dknown = 1;
  if (Has_contents(obj)) {
    for (otmp = obj->cobj; otmp; otmp = otmp->nobj)
      do_dknown_of(otmp);
  }
}

/* Check whether the location has an outdated object displayed on it. */
STATIC_OVL bool check_map_spot(int x, int y, char oclass, unsigned material) {
  int glyph;
  Object *otmp;
  Monster *mtmp;

  glyph = glyph_at(x, y);
  if (glyph_is_object(glyph)) {
    /* there's some object shown here */
    if (oclass == ALL_CLASSES) {
      return ((bool)(!(level.objects[x][y] || /* stale if nothing here */
                       ((mtmp = m_at(x, y)) != 0 && (
#ifndef GOLDOBJ
                                                        mtmp->mgold ||
#endif
                                                        mtmp->minvent)))));
    } else {
      if (material && objects[glyph_to_obj(glyph)].oc_material == material) {
        /* the object shown here is of interest because material matches */
        for (otmp = level.objects[x][y]; otmp; otmp = otmp->nexthere)
          if (o_material(otmp, GOLD))
            return FALSE;
        /* didn't find it; perhaps a monster is carrying it */
        if ((mtmp = m_at(x, y)) != 0) {
          for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj)
            if (o_material(otmp, GOLD))
              return FALSE;
        }
        /* detection indicates removal of this object from the map */
        return TRUE;
      }
      if (oclass && objects[glyph_to_obj(glyph)].oc_class == oclass) {
        /* the object shown here is of interest because its class_id matches */
        for (otmp = level.objects[x][y]; otmp; otmp = otmp->nexthere)
          if (o_in(otmp, oclass))
            return FALSE;
/* didn't find it; perhaps a monster is carrying it */
#ifndef GOLDOBJ
        if ((mtmp = m_at(x, y)) != 0) {
          if (oclass == COIN_CLASS && mtmp->mgold)
            return FALSE;
          else
            for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj)
              if (o_in(otmp, oclass))
                return FALSE;
        }
#else
        if ((mtmp = m_at(x, y)) != 0) {
          for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj)
            if (o_in(otmp, oclass))
              return FALSE;
        }
#endif
        /* detection indicates removal of this object from the map */
        return TRUE;
      }
    }
  }
  return FALSE;
}

/*
   When doing detection, remove stale data from the map display (corpses
   rotted away, objects carried away by monsters, etc) so that it won't
   reappear after the detection has completed.  Return true if noticeable
   change occurs.
 */
STATIC_OVL bool clear_stale_map(char oclass, unsigned material) {
  int zx, zy;
  bool change_made = FALSE;

  for (zx = 1; zx < COLNO; zx++)
    for (zy = 0; zy < ROWNO; zy++)
      if (check_map_spot(zx, zy, oclass, material)) {
        unmap_object(zx, zy);
        change_made = TRUE;
      }

  return change_made;
}

/* look for gold, on the floor or in monsters' possession */
int gold_detect(Object *sobj) {
  Object *obj;
  Monster *mtmp;
  int uw = player.uinwater;
  Object *temp;
  bool stale;

  known = stale =
      clear_stale_map(COIN_CLASS, (unsigned)(sobj->blessed ? GOLD : 0));

  /* look for gold carried by monsters (might be in a container) */
  for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
    if (mtmp->dead())
      continue; /* probably not needed in this case but... */
#ifndef GOLDOBJ
    if (mtmp->mgold || monsndx(mtmp->data) == PM_GOLD_GOLEM) {
#else
    if (findgold(mtmp->minvent) || monsndx(mtmp->data) == PM_GOLD_GOLEM) {
#endif
      known = TRUE;
      goto outgoldmap; /* skip further searching */
    } else
      for (obj = mtmp->minvent; obj; obj = obj->nobj)
        if (sobj->blessed && o_material(obj, GOLD)) {
          known = TRUE;
          goto outgoldmap;
        } else if (o_in(obj, COIN_CLASS)) {
          known = TRUE;
          goto outgoldmap; /* skip further searching */
        }
  }

  /* look for gold objects */
  for (obj = fobj; obj; obj = obj->nobj) {
    if (sobj->blessed && o_material(obj, GOLD)) {
      known = TRUE;
      if (obj->ox != player.ux || obj->oy != player.uy)
        goto outgoldmap;
    } else if (o_in(obj, COIN_CLASS)) {
      known = TRUE;
      if (obj->ox != player.ux || obj->oy != player.uy)
        goto outgoldmap;
    }
  }

  if (!known) {
    /* no gold found on floor or monster's inventory.
       adjust message if you have gold in your inventory */
    if (sobj) {
      char buf[BUFSZ];
      if (youmonst.data == &mons[PM_GOLD_GOLEM]) {
        sprintf(buf, "You feel like a million %s!", currency(2L));
      } else if (hidden_gold() ||
#ifndef GOLDOBJ
                 player.ugold)
#else
                 money_cnt(invent))
#endif
        strcpy(buf, "You feel worried about your future financial situation.");
      else
        strcpy(buf, "You feel materially poor.");
      strange_feeling(sobj, buf);
    }
    return (1);
  }
  /* only under me - no separate display required */
  if (stale)
    docrt();
  You("notice some gold between your %s.", makeplural(body_part(FOOT)));
  return (0);

outgoldmap:
  cls();

  player.uinwater = 0;
  /* Discover gold locations. */
  for (obj = fobj; obj; obj = obj->nobj) {
    if (sobj->blessed && (temp = o_material(obj, GOLD))) {
      if (temp != obj) {
        temp->ox = obj->ox;
        temp->oy = obj->oy;
      }
      map_object(temp, 1);
    } else if ((temp = o_in(obj, COIN_CLASS))) {
      if (temp != obj) {
        temp->ox = obj->ox;
        temp->oy = obj->oy;
      }
      map_object(temp, 1);
    }
  }
  for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
    if (mtmp->dead())
      continue; /* probably overkill here */
#ifndef GOLDOBJ
    if (mtmp->mgold || monsndx(mtmp->data) == PM_GOLD_GOLEM) {
#else
    if (findgold(mtmp->minvent) || monsndx(mtmp->data) == PM_GOLD_GOLEM) {
#endif
      Object gold;

      gold.otyp = GOLD_PIECE;
      gold.ox = mtmp->mx;
      gold.oy = mtmp->my;
      map_object(&gold, 1);
    } else
      for (obj = mtmp->minvent; obj; obj = obj->nobj)
        if (sobj->blessed && (temp = o_material(obj, GOLD))) {
          temp->ox = mtmp->mx;
          temp->oy = mtmp->my;
          map_object(temp, 1);
          break;
        } else if ((temp = o_in(obj, COIN_CLASS))) {
          temp->ox = mtmp->mx;
          temp->oy = mtmp->my;
          map_object(temp, 1);
          break;
        }
  }

  newsym(player.ux, player.uy);
  You_feel("very greedy, and sense gold!");
  exercise(A_WIS, TRUE);
  display_nhwindow(WIN_MAP, TRUE);
  docrt();
  player.uinwater = uw;
  if (Underwater)
    under_water(2);
  if (player.uburied)
    under_ground(2);
  return (0);
}

/* returns 1 if nothing was detected		*/
/* returns 0 if something was detected		*/
int food_detect(Object *sobj) {
  Object *obj;
  Monster *mtmp;
  int ct = 0, ctu = 0;
  bool confused = (Confusion || (sobj && sobj->cursed)), stale;
  char oclass = confused ? POTION_CLASS : FOOD_CLASS;
  const char *what = confused ? something : "food";
  int uw = player.uinwater;

  stale = clear_stale_map(oclass, 0);

  for (obj = fobj; obj; obj = obj->nobj)
    if (o_in(obj, oclass)) {
      if (obj->ox == player.ux && obj->oy == player.uy)
        ctu++;
      else
        ct++;
    }
  for (mtmp = fmon; mtmp && !ct; mtmp = mtmp->nmon) {
    /* no mtmp->dead() check needed since dmons never have inventory */
    for (obj = mtmp->minvent; obj; obj = obj->nobj)
      if (o_in(obj, oclass)) {
        ct++;
        break;
      }
  }

  if (!ct && !ctu) {
    known = stale && !confused;
    if (stale) {
      docrt();
      You("sense a lack of %s nearby.", what);
      if (sobj && sobj->blessed) {
        if (!player.uedibility)
          Your("%s starts to tingle.", body_part(NOSE));
        player.uedibility = 1;
      }
    } else if (sobj) {
      char buf[BUFSZ];
      sprintf(buf, "Your %s twitches%s.", body_part(NOSE),
              (sobj->blessed && !player.uedibility) ? " then starts to tingle"
                                                    : "");
      if (sobj->blessed && !player.uedibility) {
        bool savebeginner = flags.beginner; /* prevent non-delivery of */
        flags.beginner = FALSE;             /* 	message            */
        strange_feeling(sobj, buf);
        flags.beginner = savebeginner;
        player.uedibility = 1;
      } else
        strange_feeling(sobj, buf);
    }
    return !stale;
  } else if (!ct) {
    known = TRUE;
    You("%s %s nearby.", sobj ? "smell" : "sense", what);
    if (sobj && sobj->blessed) {
      if (!player.uedibility)
        pline("Your %s starts to tingle.", body_part(NOSE));
      player.uedibility = 1;
    }
  } else {
    Object *temp;
    known = TRUE;
    cls();
    player.uinwater = 0;
    for (obj = fobj; obj; obj = obj->nobj)
      if ((temp = o_in(obj, oclass)) != 0) {
        if (temp != obj) {
          temp->ox = obj->ox;
          temp->oy = obj->oy;
        }
        map_object(temp, 1);
      }
    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon)
      /* no mtmp->dead() check needed since dmons never have inventory */
      for (obj = mtmp->minvent; obj; obj = obj->nobj)
        if ((temp = o_in(obj, oclass)) != 0) {
          temp->ox = mtmp->mx;
          temp->oy = mtmp->my;
          map_object(temp, 1);
          break; /* skip rest of this monster's inventory */
        }
    newsym(player.ux, player.uy);
    if (sobj) {
      if (sobj->blessed) {
        Your("%s %s to tingle and you smell %s.", body_part(NOSE),
             player.uedibility ? "continues" : "starts", what);
        player.uedibility = 1;
      } else
        Your("%s tingles and you smell %s.", body_part(NOSE), what);
    } else
      You("sense %s.", what);
    display_nhwindow(WIN_MAP, TRUE);
    exercise(A_WIS, TRUE);
    docrt();
    player.uinwater = uw;
    if (Underwater)
      under_water(2);
    if (player.uburied)
      under_ground(2);
  }
  return (0);
}

/*
 * Used for scrolls, potions, spells, and crystal balls.  Returns:
 *
 *	1 - nothing was detected
 *	0 - something was detected
 */
int object_detect(Object *detector, int class_id) {
  int x, y;
  char stuff[BUFSZ];
  int is_cursed = (detector && detector->cursed);
  int do_dknown = (detector && (detector->oclass == POTION_CLASS ||
                                detector->oclass == SPBOOK_CLASS) &&
                   detector->blessed);
  int ct = 0, ctu = 0;
  Object *obj, *otmp = nullptr;
  Monster *mtmp;
  int uw = player.uinwater;
  int sym, boulder = 0;

  if (class_id < 0 || class_id >= MAXOCLASSES) {
    impossible("object_detect:  illegal class_id %d", class_id);
    class_id = 0;
  }

  /* Special boulder symbol check - does the class_id symbol happen
   * to match iflags.bouldersym which is a user-defined?
   * If so, that means we aren't sure what they really wanted to
   * detect. Rather than trump anything, show both possibilities.
   * We can exclude checking the buried obj chain for boulders below.
   */
  sym = class_id ? def_oc_syms[class_id] : 0;
  if (sym && iflags.bouldersym && sym == iflags.bouldersym)
    boulder = ROCK_CLASS;

  if (Hallucination || (Confusion && class_id == SCROLL_CLASS))
    strcpy(stuff, something);
  else
    strcpy(stuff, class_id ? oclass_names[class_id] : "objects");
  if (boulder && class_id != ROCK_CLASS)
    strcat(stuff, " and/or large stones");

  if (do_dknown)
    for (obj = invent; obj; obj = obj->nobj)
      do_dknown_of(obj);

  for (obj = fobj; obj; obj = obj->nobj) {
    if ((!class_id && !boulder) || o_in(obj, class_id) || o_in(obj, boulder)) {
      if (obj->ox == player.ux && obj->oy == player.uy)
        ctu++;
      else
        ct++;
    }
    if (do_dknown)
      do_dknown_of(obj);
  }

  for (obj = level.buriedobjlist; obj; obj = obj->nobj) {
    if (!class_id || o_in(obj, class_id)) {
      if (obj->ox == player.ux && obj->oy == player.uy)
        ctu++;
      else
        ct++;
    }
    if (do_dknown)
      do_dknown_of(obj);
  }

  for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
    if (mtmp->dead())
      continue;
    for (obj = mtmp->minvent; obj; obj = obj->nobj) {
      if ((!class_id && !boulder) || o_in(obj, class_id) || o_in(obj, boulder))
        ct++;
      if (do_dknown)
        do_dknown_of(obj);
    }
    if ((is_cursed && mtmp->m_ap_type == M_AP_OBJECT &&
         (!class_id || class_id == objects[mtmp->mappearance].oc_class)) ||
#ifndef GOLDOBJ
        (mtmp->mgold && (!class_id || class_id == COIN_CLASS))) {
#else
        (findgold(mtmp->minvent) && (!class_id || class_id == COIN_CLASS))) {
#endif
      ct++;
      break;
    }
  }

  if (!clear_stale_map(!class_id ? ALL_CLASSES : class_id, 0) && !ct) {
    if (!ctu) {
      if (detector)
        strange_feeling(detector, "You feel a lack of something.");
      return 1;
    }

    You("sense %s nearby.", stuff);
    return 0;
  }

  cls();

  player.uinwater = 0;
  /*
   *	Map all buried objects first.
   */
  for (obj = level.buriedobjlist; obj; obj = obj->nobj)
    if (!class_id || (otmp = o_in(obj, class_id))) {
      if (class_id) {
        if (otmp != obj) {
          otmp->ox = obj->ox;
          otmp->oy = obj->oy;
        }
        map_object(otmp, 1);
      } else
        map_object(obj, 1);
    }
  /*
   * If we are mapping all objects, map only the top object of a pile or
   * the first object in a monster's inventory.  Otherwise, go looking
   * for a matching object class_id and display the first one encountered
   * at each location.
   *
   * Objects on the floor override buried objects.
   */
  for (x = 1; x < COLNO; x++)
    for (y = 0; y < ROWNO; y++)
      for (obj = level.objects[x][y]; obj; obj = obj->nexthere)
        if ((!class_id && !boulder) || (otmp = o_in(obj, class_id)) ||
            (otmp = o_in(obj, boulder))) {
          if (class_id || boulder) {
            if (otmp != obj) {
              otmp->ox = obj->ox;
              otmp->oy = obj->oy;
            }
            map_object(otmp, 1);
          } else
            map_object(obj, 1);
          break;
        }

  /* Objects in the monster's inventory override floor objects. */
  for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
    if (mtmp->dead())
      continue;
    for (obj = mtmp->minvent; obj; obj = obj->nobj)
      if ((!class_id && !boulder) || (otmp = o_in(obj, class_id)) ||
          (otmp = o_in(obj, boulder))) {
        if (!class_id && !boulder)
          otmp = obj;
        otmp->ox = mtmp->mx; /* at monster location */
        otmp->oy = mtmp->my;
        map_object(otmp, 1);
        break;
      }
    /* Allow a mimic to override the detected objects it is carrying. */
    if (is_cursed && mtmp->m_ap_type == M_AP_OBJECT &&
        (!class_id || class_id == objects[mtmp->mappearance].oc_class)) {
      Object temp;

      temp.otyp = mtmp->mappearance; /* needed for obj_to_glyph() */
      temp.ox = mtmp->mx;
      temp.oy = mtmp->my;
      temp.corpsenm = PM_TENGU; /* if mimicing a corpse */
      map_object(&temp, 1);
#ifndef GOLDOBJ
    } else if (mtmp->mgold && (!class_id || class_id == COIN_CLASS)) {
#else
    } else if (findgold(mtmp->minvent) &&
               (!class_id || class_id == COIN_CLASS)) {
#endif
      Object gold;

      gold.otyp = GOLD_PIECE;
      gold.ox = mtmp->mx;
      gold.oy = mtmp->my;
      map_object(&gold, 1);
    }
  }

  newsym(player.ux, player.uy);
  You("detect the %s of %s.", ct ? "presence" : "absence", stuff);
  display_nhwindow(WIN_MAP, TRUE);
  /*
   * What are we going to do when the hero does an object detect while blind
   * and the detected object covers a known pool?
   */
  docrt(); /* this will correctly reset vision */

  player.uinwater = uw;
  if (Underwater)
    under_water(2);
  if (player.uburied)
    under_ground(2);
  return 0;
}

/*
 * Used by: crystal balls, potions, fountains
 *
 * Returns 1 if nothing was detected.
 * Returns 0 if something was detected.
 */
int monster_detect(Object *otmp, int mclass) {
  Monster *mtmp;
  int mcnt = 0;

  /* Note: This used to just check fmon for a non-zero value
   * but in versions since 3.3.0 fmon can test TRUE due to the
   * presence of dmons, so we have to find at least one
   * with positive hit-points to know for sure.
   */
  for (mtmp = fmon; mtmp; mtmp = mtmp->nmon)
    if (!mtmp->dead()) {
      mcnt++;
      break;
    }

  if (!mcnt) {
    if (otmp)
      strange_feeling(otmp, Hallucination ? "You get the heebie jeebies."
                                          : "You feel threatened.");
    return 1;
  } else {
    bool woken = FALSE;

    cls();
    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
      if (mtmp->dead())
        continue;
      if (!mclass || mtmp->data->mlet == mclass ||
          (mtmp->data == &mons[PM_LONG_WORM] && mclass == S_WORM_TAIL))
        if (mtmp->mx > 0) {
          if (mclass && def_monsyms[mclass] == ' ')
            show_glyph(mtmp->mx, mtmp->my, detected_mon_to_glyph(mtmp));
          else
            show_glyph(mtmp->mx, mtmp->my, mon_to_glyph(mtmp));
          /* don't be stingy - display entire worm */
          if (mtmp->data == &mons[PM_LONG_WORM])
            detect_wsegs(mtmp, 0);
        }
      if (otmp && otmp->cursed && (mtmp->msleeping || !mtmp->mcanmove)) {
        mtmp->msleeping = mtmp->mfrozen = 0;
        mtmp->mcanmove = 1;
        woken = TRUE;
      }
    }
    display_self();
    You("sense the presence of monsters.");
    if (woken)
      pline("Monsters sense the presence of you.");
    display_nhwindow(WIN_MAP, TRUE);
    docrt();
    if (Underwater)
      under_water(2);
    if (player.uburied)
      under_ground(2);
  }
  return 0;
}

STATIC_OVL void sense_trap(Trap *trap, xchar x, xchar y, int src_cursed) {
  if (Hallucination || src_cursed) {
    Object obj; /* fake object */
    if (trap) {
      obj.ox = trap->tx;
      obj.oy = trap->ty;
    } else {
      obj.ox = x;
      obj.oy = y;
    }
    obj.otyp = (src_cursed) ? GOLD_PIECE : random_object();
    obj.corpsenm = random_monster(); /* if otyp == CORPSE */
    map_object(&obj, 1);
  } else if (trap) {
    map_trap(trap, 1);
    trap->tseen = 1;
  } else {
    Trap temp_trap; /* fake trap */
    temp_trap.tx = x;
    temp_trap.ty = y;
    temp_trap.ttyp = BEAR_TRAP; /* some kind of trap */
    map_trap(&temp_trap, 1);
  }
}

/* the detections are pulled out so they can	*/
/* also be used in the crystal ball routine	*/
/* returns 1 if nothing was detected		*/
/* returns 0 if something was detected		*/
/* sobj is null if crystal ball, *scroll if gold detection scroll */
int trap_detect(Object *sobj) {
  Trap *ttmp;
  Object *obj;
  int door;
  int uw = player.uinwater;
  bool found = FALSE;
  coord cc;

  for (ttmp = ftrap; ttmp; ttmp = ttmp->ntrap) {
    if (ttmp->tx != player.ux || ttmp->ty != player.uy)
      goto outtrapmap;
    else
      found = TRUE;
  }
  for (obj = fobj; obj; obj = obj->nobj) {
    if ((obj->otyp == LARGE_BOX || obj->otyp == CHEST) && obj->otrapped) {
      if (obj->ox != player.ux || obj->oy != player.uy)
        goto outtrapmap;
      else
        found = TRUE;
    }
  }
  for (door = 0; door < doorindex; door++) {
    cc = doors[door];
    if (levl[cc.x][cc.y].doormask & D_TRAPPED) {
      if (cc.x != player.ux || cc.y != player.uy)
        goto outtrapmap;
      else
        found = TRUE;
    }
  }
  if (!found) {
    char buf[42];
    sprintf(buf, "Your %s stop itching.", makeplural(body_part(TOE)));
    strange_feeling(sobj, buf);
    return (1);
  }
  /* traps exist, but only under me - no separate display required */
  Your("%s itch.", makeplural(body_part(TOE)));
  return (0);
outtrapmap:
  cls();

  player.uinwater = 0;
  for (ttmp = ftrap; ttmp; ttmp = ttmp->ntrap)
    sense_trap(ttmp, 0, 0, sobj && sobj->cursed);

  for (obj = fobj; obj; obj = obj->nobj)
    if ((obj->otyp == LARGE_BOX || obj->otyp == CHEST) && obj->otrapped)
      sense_trap(nullptr, obj->ox, obj->oy, sobj && sobj->cursed);

  for (door = 0; door < doorindex; door++) {
    cc = doors[door];
    if (levl[cc.x][cc.y].doormask & D_TRAPPED)
      sense_trap(nullptr, cc.x, cc.y, sobj && sobj->cursed);
  }

  newsym(player.ux, player.uy);
  You_feel("%s.", sobj && sobj->cursed ? "very greedy" : "entrapped");
  display_nhwindow(WIN_MAP, TRUE);
  docrt();
  player.uinwater = uw;
  if (Underwater)
    under_water(2);
  if (player.uburied)
    under_ground(2);
  return (0);
}

const char *level_distance(d_level *where) {
  schar ll = depth(&player.uz) - depth(where);
  bool indun = (player.uz.dnum == where->dnum);

  if (ll < 0) {
    if (ll < (-8 - rn2(3)))
      if (!indun)
        return "far away";
      else
        return "far below";
    else if (ll < -1)
      if (!indun)
        return "away below you";
      else
        return "below you";
    else if (!indun)
      return "in the distance";
    else
      return "just below";
  } else if (ll > 0) {
    if (ll > (8 + rn2(3)))
      if (!indun)
        return "far away";
      else
        return "far above";
    else if (ll > 1)
      if (!indun)
        return "away above you";
      else
        return "above you";
    else if (!indun)
      return "in the distance";
    else
      return "just above";
  } else if (!indun)
    return "in the distance";
  else
    return "near you";
}

static const struct {
  const char *what;
  d_level *where;
} level_detects[] = {
      {"Delphi", &oracle_level},
      {"Medusa's lair", &medusa_level},
      {"a castle", &stronghold_level},
      {"the Wizard of Yendor's tower", &wiz1_level},
  };

void use_crystal_ball(Object *obj) {
  char ch;
  int oops;

  if (Blind) {
    pline("Too bad you can't see %s.", the(xname(obj)));
    return;
  }
  oops = (rnd(20) > ACURR(A_INT) || obj->cursed);
  if (oops && (obj->spe > 0)) {
    switch (rnd(obj->oartifact ? 4 : 5)) {
      case 1:
        pline("%s too much to comprehend!", Tobjnam(obj, "are"));
        break;
      case 2:
        pline("%s you!", Tobjnam(obj, "confuse"));
        make_confused(HConfusion + rnd(100), FALSE);
        break;
      case 3:
        if (!resists_blnd(&youmonst)) {
          pline("%s your vision!", Tobjnam(obj, "damage"));
          make_blinded(Blinded + rnd(100), FALSE);
          if (!Blind)
            Your(vision_clears);
        } else {
          pline("%s your vision.", Tobjnam(obj, "assault"));
          You("are unaffected!");
        }
        break;
      case 4:
        pline("%s your mind!", Tobjnam(obj, "zap"));
        (void)make_hallucinated(HHallucination + rnd(100), FALSE, 0L);
        break;
      case 5:
        pline("%s!", Tobjnam(obj, "explode"));
        useup(obj);
        obj = 0; /* it's gone */
        losehp(rnd(30), "exploding crystal ball", KILLED_BY_AN);
        break;
    }
    if (obj)
      consume_obj_charge(obj, TRUE);
    return;
  }

  if (Hallucination) {
    if (!obj->spe) {
      pline("All you see is funky %s haze.", hcolor(nullptr));
    } else {
      switch (rnd(6)) {
        case 1:
          You("grok some groovy globs of incandescent lava.");
          break;
        case 2:
          pline("Whoa!  Psychedelic colors, %s!",
                poly_gender() == 1 ? "babe" : "dude");
          break;
        case 3:
          pline_The("crystal pulses with sinister %s light!",
                    hcolor(nullptr));
          break;
        case 4:
          You("see goldfish swimming above fluorescent rocks.");
          break;
        case 5:
          You("see tiny snowflakes spinning around a miniature farmhouse.");
          break;
        default:
          pline("Oh wow... like a kaleidoscope!");
          break;
      }
      consume_obj_charge(obj, TRUE);
    }
    return;
  }

  /* read a single character */
  if (flags.verbose)
    You("may look for an object or monster symbol.");
  ch = yn_function("What do you look for?", nullptr, '\0');
  /* Don't filter out ' ' here; it has a use */
  if ((ch != def_monsyms[S_GHOST]) && index(quitchars, ch)) {
    if (flags.verbose)
      pline(Never_mind);
    return;
  }
  You("peer into %s...", the(xname(obj)));
  nomul(-rnd(10), "gazing into a crystal ball");
  nomovemsg = "";
  if (obj->spe <= 0)
    pline_The("vision is unclear.");
  else {
    int class_id;
    int ret = 0;

    makeknown(CRYSTAL_BALL);
    consume_obj_charge(obj, TRUE);

    /* special case: accept ']' as synonym for mimic
     * we have to do this before the def_char_to_objclass check
     */
    if (ch == DEF_MIMIC_DEF)
      ch = DEF_MIMIC;

    if ((class_id = def_char_to_objclass(ch)) != MAXOCLASSES)
      ret = object_detect(nullptr, class_id);
    else if ((class_id = def_char_to_monclass(ch)) != MAXMCLASSES)
      ret = monster_detect(nullptr, class_id);
    else if (iflags.bouldersym && (ch == iflags.bouldersym))
      ret = object_detect(nullptr, ROCK_CLASS);
    else
      switch (ch) {
        case '^':
          ret = trap_detect(nullptr);
          break;
        default: {
          int i = rn2(SIZE(level_detects));
          You("see %s, %s.", level_detects[i].what,
              level_distance(level_detects[i].where));
        }
          ret = 0;
          break;
      }

    if (ret) {
      if (!rn2(100)) /* make them nervous */
        You("see the Wizard of Yendor gazing out at you.");
      else
        pline_The("vision is unclear.");
    }
  }
  return;
}

STATIC_OVL void show_map_spot(int x, int y) {
  struct rm *lev;

  if (Confusion && rn2(7))
    return;
  lev = &levl[x][y];

  lev->seenv = SVALL;

  /* Secret corridors are found, but not secret doors. */
  if (lev->typ == SCORR) {
    lev->typ = CORR;
    unblock_point(x, y);
  }

  /* if we don't remember an object or trap there, map it */
  if (lev->typ == ROOM
          ? (glyph_is_cmap(lev->glyph) && !glyph_is_trap(lev->glyph) &&
             glyph_to_cmap(lev->glyph) != ROOM)
          : (!glyph_is_object(lev->glyph) && !glyph_is_trap(lev->glyph))) {
    if (level.flags.hero_memory) {
      magic_map_background(x, y, 0);
      newsym(x, y); /* show it, if not blocked */
    } else {
      magic_map_background(x, y, 1); /* display it */
    }
  }
}

void do_mapping() {
  int zx, zy;
  int uw = player.uinwater;

  player.uinwater = 0;
  for (zx = 1; zx < COLNO; zx++)
    for (zy = 0; zy < ROWNO; zy++)
      show_map_spot(zx, zy);
  exercise(A_WIS, TRUE);
  player.uinwater = uw;
  if (!level.flags.hero_memory || Underwater) {
    flush_screen(1);                 /* flush temp screen */
    display_nhwindow(WIN_MAP, TRUE); /* wait */
    docrt();
  }
}

void do_vicinity_map() {
  int zx, zy;
  int lo_y = (player.uy - 5 < 0 ? 0 : player.uy - 5),
      hi_y = (player.uy + 6 > ROWNO ? ROWNO : player.uy + 6),
      lo_x = (player.ux - 9 < 1 ? 1 : player.ux - 9), /* avoid column 0 */
      hi_x = (player.ux + 10 > COLNO ? COLNO : player.ux + 10);

  for (zx = lo_x; zx < hi_x; zx++)
    for (zy = lo_y; zy < hi_y; zy++)
      show_map_spot(zx, zy);

  if (!level.flags.hero_memory || Underwater) {
    flush_screen(1);                 /* flush temp screen */
    display_nhwindow(WIN_MAP, TRUE); /* wait */
    docrt();
  }
}

/* convert a secret door into a normal door */
void cvt_sdoor_to_door(struct rm *lev) {
  int newmask = lev->doormask & ~WM_MASK;

#ifdef REINCARNATION
  if (Is_rogue_level(&player.uz)) /* rogue didn't have doors, only doorways */
    newmask = D_NODOOR;
  else
#endif
      /* newly exposed door is closed */
      if (!(newmask & D_LOCKED))
    newmask |= D_CLOSED;

  lev->typ = DOOR;
  lev->doormask = newmask;
}

STATIC_PTR void findone(int zx, int zy, genericptr_t num) {
  Trap *ttmp;
  Monster *mtmp;

  if (levl[zx][zy].typ == SDOOR) {
    cvt_sdoor_to_door(&levl[zx][zy]); /* .typ = DOOR */
    magic_map_background(zx, zy, 0);
    newsym(zx, zy);
    (*(int *)num)++;
  } else if (levl[zx][zy].typ == SCORR) {
    levl[zx][zy].typ = CORR;
    unblock_point(zx, zy);
    magic_map_background(zx, zy, 0);
    newsym(zx, zy);
    (*(int *)num)++;
  } else if ((ttmp = t_at(zx, zy)) != 0) {
    if (!ttmp->tseen && ttmp->ttyp != STATUE_TRAP) {
      ttmp->tseen = 1;
      newsym(zx, zy);
      (*(int *)num)++;
    }
  } else if ((mtmp = m_at(zx, zy)) != 0) {
    if (mtmp->m_ap_type) {
      seemimic(mtmp);
      (*(int *)num)++;
    }
    if (mtmp->mundetected &&
        (is_hider(mtmp->data) || mtmp->data->mlet == S_EEL)) {
      mtmp->mundetected = 0;
      newsym(zx, zy);
      (*(int *)num)++;
    }
    if (!canspotmon(mtmp) && !glyph_is_invisible(levl[zx][zy].glyph))
      map_invisible(zx, zy);
  } else if (glyph_is_invisible(levl[zx][zy].glyph)) {
    unmap_object(zx, zy);
    newsym(zx, zy);
    (*(int *)num)++;
  }
}

STATIC_PTR void openone(int zx, int zy, genericptr_t num) {
  Trap *ttmp;
  Object *otmp;

  if (OBJ_AT(zx, zy)) {
    for (otmp = level.objects[zx][zy]; otmp; otmp = otmp->nexthere) {
      if (Is_box(otmp) && otmp->olocked) {
        otmp->olocked = 0;
        (*(int *)num)++;
      }
    }
    /* let it fall to the next cases. could be on trap. */
  }
  if (levl[zx][zy].typ == SDOOR ||
      (levl[zx][zy].typ == DOOR &&
       (levl[zx][zy].doormask & (D_CLOSED | D_LOCKED)))) {
    if (levl[zx][zy].typ == SDOOR)
      cvt_sdoor_to_door(&levl[zx][zy]); /* .typ = DOOR */
    if (levl[zx][zy].doormask & D_TRAPPED) {
      if (distu(zx, zy) < 3)
        b_trapped("door", 0);
      else
        Norep("You %s an explosion!",
              cansee(zx, zy) ? "see"
                             : (flags.soundok ? "hear" : "feel the shock of"));
      wake_nearto(zx, zy, 11 * 11);
      levl[zx][zy].doormask = D_NODOOR;
    } else
      levl[zx][zy].doormask = D_ISOPEN;
    unblock_point(zx, zy);
    newsym(zx, zy);
    (*(int *)num)++;
  } else if (levl[zx][zy].typ == SCORR) {
    levl[zx][zy].typ = CORR;
    unblock_point(zx, zy);
    newsym(zx, zy);
    (*(int *)num)++;
  } else if ((ttmp = t_at(zx, zy)) != 0) {
    if (!ttmp->tseen && ttmp->ttyp != STATUE_TRAP) {
      ttmp->tseen = 1;
      newsym(zx, zy);
      (*(int *)num)++;
    }
  } else if (find_drawbridge(&zx, &zy)) {
    /* make sure it isn't an open drawbridge */
    open_drawbridge(zx, zy);
    (*(int *)num)++;
  }
}

/* returns number of things found */
int findit() {
  int num = 0;

  if (player.uswallow)
    return (0);
  do_clear_area(player.ux, player.uy, BOLT_LIM, findone, (genericptr_t) & num);
  return (num);
}

/* returns number of things found and opened */
int openit() {
  int num = 0;

  if (player.uswallow) {
    if (is_animal(player.ustuck->data)) {
      if (Blind)
        pline("Its mouth opens!");
      else
        pline("%s opens its mouth!", Monnam(player.ustuck));
    }
    expels(player.ustuck, player.ustuck->data, TRUE);
    return (-1);
  }

  do_clear_area(player.ux, player.uy, BOLT_LIM, openone, (genericptr_t) & num);
  return (num);
}

void find_trap(Trap *trap) {
  int tt = what_trap(trap->ttyp);
  bool cleared = FALSE;

  trap->tseen = 1;
  exercise(A_WIS, TRUE);
  if (Blind)
    feel_location(trap->tx, trap->ty);
  else
    newsym(trap->tx, trap->ty);

  if (levl[trap->tx][trap->ty].glyph != trap_to_glyph(trap)) {
    /* There's too much clutter to see your find otherwise */
    cls();
    map_trap(trap, 1);
    display_self();
    cleared = TRUE;
  }

  You("find %s.", an(defsyms[trap_to_defsym(tt)].explanation));

  if (cleared) {
    display_nhwindow(WIN_MAP, TRUE); /* wait */
    docrt();
  }
}

int dosearch0(int aflag) {
#ifdef GCC_BUG
  /* some versions of gcc seriously muck up nested loops. if you get strange
     crashes while searching in a version compiled with gcc, try putting
     #define GCC_BUG in *conf.h (or adding -DGCC_BUG to CFLAGS in the
     makefile).
   */
  volatile xchar x, y;
#else
  xchar x, y;
#endif
  Trap *trap;
  Monster *mtmp;

  if (player.uswallow) {
    if (!aflag)
      pline("What are you looking for?  The exit?");
  } else {
    int fund = (uwep && uwep->oartifact && spec_ability(uwep, SPFX_SEARCH))
                   ? uwep->spe
                   : 0;
    if (ublindf && ublindf->otyp == LENSES && !Blind)
      fund += 2; /* JDS: lenses help searching */
    if (fund > 5)
      fund = 5;
    for (x = player.ux - 1; x < player.ux + 2; x++)
      for (y = player.uy - 1; y < player.uy + 2; y++) {
        if (!isok(x, y))
          continue;
        if (x != player.ux || y != player.uy) {
          if (Blind && !aflag)
            feel_location(x, y);
          if (levl[x][y].typ == SDOOR) {
            if (rnl(7 - fund))
              continue;
            cvt_sdoor_to_door(&levl[x][y]); /* .typ = DOOR */
            exercise(A_WIS, TRUE);
            nomul(0, 0);
            if (Blind && !aflag)
              feel_location(x, y); /* make sure it shows up */
            else
              newsym(x, y);
          } else if (levl[x][y].typ == SCORR) {
            if (rnl(7 - fund))
              continue;
            levl[x][y].typ = CORR;
            unblock_point(x, y); /* vision */
            exercise(A_WIS, TRUE);
            nomul(0, 0);
            newsym(x, y);
          } else {
            /* Be careful not to find anything in an SCORR or SDOOR */
            if ((mtmp = m_at(x, y)) && !aflag) {
              if (mtmp->m_ap_type) {
                seemimic(mtmp);
              find:
                exercise(A_WIS, TRUE);
                if (!canspotmon(mtmp)) {
                  if (glyph_is_invisible(levl[x][y].glyph)) {
                    /* found invisible monster in a square
                     * which already has an 'I' in it.
                     * Logically, this should still take
                     * time and lead to a return(1), but if
                     * we did that the player would keep
                     * finding the same monster every turn.
                     */
                    continue;
                  } else {
                    You_feel("an unseen monster!");
                    map_invisible(x, y);
                  }
                } else if (!sensemon(mtmp))
                  You("find %s.", a_monnam(mtmp));
                return (1);
              }
              if (!canspotmon(mtmp)) {
                if (mtmp->mundetected &&
                    (is_hider(mtmp->data) || mtmp->data->mlet == S_EEL))
                  mtmp->mundetected = 0;
                newsym(x, y);
                goto find;
              }
            }

            /* see if an invisible monster has moved--if Blind,
             * feel_location() already did it
             */
            if (!aflag && !mtmp && !Blind &&
                glyph_is_invisible(levl[x][y].glyph)) {
              unmap_object(x, y);
              newsym(x, y);
            }

            if ((trap = t_at(x, y)) && !trap->tseen && !rnl(8)) {
              nomul(0, 0);

              if (trap->ttyp == STATUE_TRAP) {
                if (activate_statue_trap(trap, x, y, FALSE))
                  exercise(A_WIS, TRUE);
                return (1);
              } else {
                find_trap(trap);
              }
            }
          }
        }
      }
  }
  return (1);
}

int dosearch() { return (dosearch0(0)); }

/* Pre-map the sokoban levels */
void sokoban_detect() {
  int x, y;
  Trap *ttmp;
  Object *obj;

  /* Map the background and boulders */
  for (x = 1; x < COLNO; x++)
    for (y = 0; y < ROWNO; y++) {
      levl[x][y].seenv = SVALL;
      levl[x][y].waslit = TRUE;
      map_background(x, y, 1);
      for (obj = level.objects[x][y]; obj; obj = obj->nexthere)
        if (obj->otyp == BOULDER)
          map_object(obj, 1);
    }

  /* Map the traps */
  for (ttmp = ftrap; ttmp; ttmp = ttmp->ntrap) {
    ttmp->tseen = 1;
    map_trap(ttmp, 1);
  }
}

/*detect.c*/
