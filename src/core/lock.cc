/*	SCCS Id: @(#)lock.c	3.4	2000/02/06	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "core/lock.h"

#include "core/hack.h"
#include "core/decl.h"
#include "core/youprop.h"
#include "core/mkroom.h"
#include "core/zap.h"
#include "core/worm.h"
#include "core/uhitm.h"
#include "core/shk.h"
#include "core/rnd.h"
#include "core/potion.h"
#include "core/pline.h"
#include "core/pickup.h"
#include "core/objnam.h"
#include "core/monmove.h"
#include "core/mon.h"
#include "core/mkobj.h"
#include "core/invent.h"
#include "core/hacklib.h"
#include "core/do_name.h"
#include "core/dbridge.h"
#include "core/cmd.h"

#include "core/allmain.h"

STATIC_PTR int picklock();
STATIC_PTR int forcelock();

/* at most one of `door' and `box' should be non-null at any given time */
STATIC_VAR struct xlock_s {
  struct rm *door;
  Object *box;
  int picktyp, chance, usedtime;
} xlock;


STATIC_DCL const char *lock_action();
STATIC_DCL bool obstructed(int, int);
STATIC_DCL void chest_shatter_msg(Object *);

bool picking_lock(int *x, int *y) {
  if (occupation == picklock) {
    *x = player.ux + player.dx;
    *y = player.uy + player.dy;
    return TRUE;
  } else {
    *x = *y = 0;
    return FALSE;
  }
}

bool picking_at(int x, int y) {
  return (bool)(occupation == picklock && xlock.door == &levl[x][y]);
}

/* produce an occupation string appropriate for the current activity */
STATIC_OVL const char *lock_action() {
  /* "unlocking"+2 == "locking" */
  static const char *actions[] = {/* [0] */ "unlocking the door",
                                  /* [1] */ "unlocking the chest",
                                  /* [2] */ "unlocking the box",
                                  /* [3] */ "picking the lock"};

  /* if the target is currently unlocked, we're trying to lock it now */
  if (xlock.door && !(xlock.door->doormask & D_LOCKED))
    return actions[0] + 2; /* "locking the door" */
  else if (xlock.box && !xlock.box->olocked)
    return xlock.box->otyp == CHEST ? actions[1] + 2 : actions[2] + 2;
  /* otherwise we're trying to unlock it */
  else if (xlock.picktyp == LOCK_PICK)
    return actions[3]; /* "picking the lock" */
#ifdef TOURIST
  else if (xlock.picktyp == CREDIT_CARD)
    return actions[3]; /* same as lock_pick */
#endif
  else if (xlock.door)
    return actions[0]; /* "unlocking the door" */
  else
    return xlock.box->otyp == CHEST ? actions[1] : actions[2];
}

STATIC_PTR
/* try to open/close a lock */
int picklock() {
  if (xlock.box) {
    if ((xlock.box->ox != player.ux) || (xlock.box->oy != player.uy)) {
      return ((xlock.usedtime = 0)); /* you or it moved */
    }
  } else { /* door */
    if (xlock.door != &(levl[player.ux + player.dx][player.uy + player.dy])) {
      return ((xlock.usedtime = 0)); /* you moved */
    }
    switch (xlock.door->doormask) {
      case D_NODOOR:
        pline("This doorway has no door.");
        return ((xlock.usedtime = 0));
      case D_ISOPEN:
        You("cannot lock an open door.");
        return ((xlock.usedtime = 0));
      case D_BROKEN:
        pline("This door is broken.");
        return ((xlock.usedtime = 0));
    }
  }

  if (xlock.usedtime++ >= 50 || nohands(youmonst.data)) {
    You("give up your attempt at %s.", lock_action());
    exercise(A_DEX, TRUE); /* even if you don't succeed */
    return ((xlock.usedtime = 0));
  }

  if (rn2(100) >= xlock.chance)
    return (1); /* still busy */

  You("succeed in %s.", lock_action());
  if (xlock.door) {
    if (xlock.door->doormask & D_TRAPPED) {
      b_trapped("door", FINGER);
      xlock.door->doormask = D_NODOOR;
      unblock_point(player.ux + player.dx, player.uy + player.dy);
      if (*in_rooms(player.ux + player.dx, player.uy + player.dy, SHOPBASE))
        add_damage(player.ux + player.dx, player.uy + player.dy, 0L);
      newsym(player.ux + player.dx, player.uy + player.dy);
    } else if (xlock.door->doormask & D_LOCKED)
      xlock.door->doormask = D_CLOSED;
    else
      xlock.door->doormask = D_LOCKED;
  } else {
    xlock.box->olocked = !xlock.box->olocked;
    if (xlock.box->otrapped)
      (void)chest_trap(xlock.box, FINGER, FALSE);
  }
  exercise(A_DEX, TRUE);
  return ((xlock.usedtime = 0));
}

STATIC_PTR
/* try to force a locked chest */
int forcelock() {
  Object *otmp;

  if ((xlock.box->ox != player.ux) || (xlock.box->oy != player.uy))
    return ((xlock.usedtime = 0)); /* you or it moved */

  if (xlock.usedtime++ >= 50 || !uwep || nohands(youmonst.data)) {
    You("give up your attempt to force the lock.");
    if (xlock.usedtime >= 50) /* you made the effort */
      exercise((xlock.picktyp) ? A_DEX : A_STR, TRUE);
    return ((xlock.usedtime = 0));
  }

  if (xlock.picktyp) { /* blade */

    if (rn2(1000 - (int)uwep->spe) > (992 - greatest_erosion(uwep) * 10) &&
        !uwep->cursed && !obj_resists(uwep, 0, 99)) {
      /* for a +0 weapon, probability that it survives an unsuccessful
       * attempt to force the lock is (.992)^50 = .67
       */
      pline("%sour %s broke!", (uwep->quan > 1L) ? "One of y" : "Y",
            xname(uwep));
      useup(uwep);
      You("give up your attempt to force the lock.");
      exercise(A_DEX, TRUE);
      return ((xlock.usedtime = 0));
    }
  } else           /* blunt */
    wake_nearby(); /* due to hammering on the container */

  if (rn2(100) >= xlock.chance)
    return (1); /* still busy */

  You("succeed in forcing the lock.");
  xlock.box->olocked = 0;
  xlock.box->obroken = 1;
  if (!xlock.picktyp && !rn2(3)) {
    Monster *shkp;
    bool costly;
    long loss = 0L;

    costly = (*player.ushops && costly_spot(player.ux, player.uy));
    shkp = costly ? shop_keeper(*player.ushops) : 0;

    pline("In fact, you've totally destroyed %s.", the(xname(xlock.box)));

    /* Put the contents on ground at the hero's feet. */
    while ((otmp = xlock.box->cobj) != 0) {
      RemoveObjectFromStorage(otmp);
      if (!rn2(3) || otmp->oclass == POTION_CLASS) {
        chest_shatter_msg(otmp);
        if (costly)
          loss += stolen_value(otmp, player.ux, player.uy,
                               (bool)shkp->mpeaceful, TRUE);
        if (otmp->quan == 1L) {
          obfree(otmp, nullptr);
          continue;
        }
        useup(otmp);
      }
      if (xlock.box->otyp == ICE_BOX && otmp->otyp == CORPSE) {
        otmp->age = monstermoves - otmp->age; /* actual age */
        StartCorpseTimeout(otmp);
      }
      PlaceObject(otmp, player.ux, player.uy);
      stackobj(otmp);
    }

    if (costly)
      loss += stolen_value(xlock.box, player.ux, player.uy,
                           (bool)shkp->mpeaceful, TRUE);
    if (loss)
      You("owe %ld %s for objects destroyed.", loss, currency(loss));
    delobj(xlock.box);
  }
  exercise((xlock.picktyp) ? A_DEX : A_STR, TRUE);
  return ((xlock.usedtime = 0));
}


void reset_pick() {
  xlock.usedtime = xlock.chance = xlock.picktyp = 0;
  xlock.door = 0;
  xlock.box = 0;
}


/* pick a lock with a given object */
int pick_lock(Object *pick) {
  int picktyp, c, ch;
  coord cc;
  struct rm *door;
  Object *otmp;
  char qbuf[QBUFSZ];

  picktyp = pick->otyp;

  /* check whether we're resuming an interrupted previous attempt */
  if (xlock.usedtime && picktyp == xlock.picktyp) {
    static char no_longer[] = "Unfortunately, you can no longer %s %s.";

    if (nohands(youmonst.data)) {
      const char *what = (picktyp == LOCK_PICK) ? "pick" : "key";
#ifdef TOURIST
      if (picktyp == CREDIT_CARD)
        what = "card";
#endif
      pline(no_longer, "hold the", what);
      reset_pick();
      return 0;
    } else if (xlock.box && !can_reach_floor()) {
      pline(no_longer, "reach the", "lock");
      reset_pick();
      return 0;
    } else {
      const char *action = lock_action();
      You("resume your attempt at %s.", action);
      set_occupation(picklock, action, 0);
      return (1);
    }
  }

  if (nohands(youmonst.data)) {
    You_cant("hold %s -- you have no hands!", doname(pick));
    return (0);
  }

  if ((picktyp != LOCK_PICK &&
#ifdef TOURIST
       picktyp != CREDIT_CARD &&
#endif
       picktyp != SKELETON_KEY)) {
    impossible("picking lock with object %d?", picktyp);
    return (0);
  }
  ch = 0; /* lint suppression */

  if (!get_adjacent_loc(nullptr, "Invalid location!", player.ux, player.uy,
                        &cc))
    return 0;
  if (cc.x == player.ux && cc.y == player.uy) { /* pick lock on a container */
    const char *verb;
    bool it;
    int count;

    if (player.dz < 0) {
      There("isn't any sort of lock up %s.", Levitation ? "here" : "there");
      return 0;
    } else if (is_lava(player.ux, player.uy)) {
      pline("Doing that would probably melt your %s.", xname(pick));
      return 0;
    } else if (is_pool(player.ux, player.uy) && !Underwater) {
      pline_The("water has no lock.");
      return 0;
    }

    count = 0;
    c = 'n'; /* in case there are no boxes here */

#ifdef CONFIRM_LOCKING
    for (otmp = level.objects[cc.x][cc.y]; otmp; otmp = otmp->nexthere)
      if (Is_box(otmp))
        ++count;
    if ((count == 1) && !iflags.confirm_locking)
      c = 'y';
#endif

    for (otmp = level.objects[cc.x][cc.y]; otmp; otmp = otmp->nexthere)
      if (Is_box(otmp)) {
#ifndef CONFIRM_LOCKING
        ++count;
#endif

        if (!can_reach_floor()) {
          You_cant("reach %s from up here.", the(xname(otmp)));
          return 0;
        }

#ifdef CONFIRM_LOCKING
        if ((count > 1) || iflags.confirm_locking) {
#endif

          it = 0;
          if (otmp->obroken)
            verb = "fix";
          else if (!otmp->olocked)
            verb = "lock", it = 1;
          else if (picktyp != LOCK_PICK)
            verb = "unlock", it = 1;
          else
            verb = "pick";
          sprintf(
              qbuf, "There is %s here, %s %s?",
              safe_qbuf("", sizeof("There is  here, unlock its lock?"),
                        doname(otmp), an(simple_typename(otmp->otyp)), "a box"),
              verb, it ? "it" : "its lock");

          c = ynq(qbuf);
          if (c == 'q')
            return (0);
          if (c == 'n')
            continue;

#ifdef CONFIRM_LOCKING
        }
#endif

        if (otmp->obroken) {
          You_cant("fix its broken lock with %s.", doname(pick));
          return 0;
        }
#ifdef TOURIST
        else if (picktyp == CREDIT_CARD && !otmp->olocked) {
          /* credit cards are only good for unlocking */
          You_cant("do that with %s.", doname(pick));
          return 0;
        }
#endif
        switch (picktyp) {
#ifdef TOURIST
          case CREDIT_CARD:
            ch = ACURR(A_DEX) + 20 * Role_if(PM_ROGUE);
            break;
#endif
          case LOCK_PICK:
            ch = 4 * ACURR(A_DEX) + 25 * Role_if(PM_ROGUE);
            break;
          case SKELETON_KEY:
            ch = 75 + ACURR(A_DEX);
            break;
          default:
            ch = 0;
        }
        if (otmp->cursed)
          ch /= 2;

        xlock.picktyp = picktyp;
        xlock.box = otmp;
        xlock.door = 0;
        break;
      }
    if (c != 'y') {
      if (!count)
        There("doesn't seem to be any sort of lock here.");
      return (0); /* decided against all boxes */
    }
  } else { /* pick the lock in a door */
    Monster *mtmp;

    if (player.utrap && player.utraptype == TT_PIT) {
      You_cant("reach over the edge of the pit.");
      return (0);
    }

    door = &levl[cc.x][cc.y];
    if ((mtmp = m_at(cc.x, cc.y)) && canseemon(mtmp) &&
        mtmp->m_ap_type != M_AP_FURNITURE && mtmp->m_ap_type != M_AP_OBJECT) {
#ifdef TOURIST
      if (picktyp == CREDIT_CARD &&
          (mtmp->isshk || mtmp->data == &mons[PM_ORACLE]))
        verbalize("No checks, no credit, no problem.");
      else
#endif
        pline("I don't think %s would appreciate that.", mon_nam(mtmp));
      return (0);
    }
    if (!IS_DOOR(door->typ)) {
      if (is_drawbridge_wall(cc.x, cc.y) >= 0)
        You("%s no lock on the drawbridge.", Blind ? "feel" : "see");
      else
        You("%s no door there.", Blind ? "feel" : "see");
      return (0);
    }
    switch (door->doormask) {
      case D_NODOOR:
        pline("This doorway has no door.");
        return (0);
      case D_ISOPEN:
        You("cannot lock an open door.");
        return (0);
      case D_BROKEN:
        pline("This door is broken.");
        return (0);
      default:
#ifdef TOURIST
        /* credit cards are only good for unlocking */
        if (picktyp == CREDIT_CARD && !(door->doormask & D_LOCKED)) {
          You_cant("lock a door with a credit card.");
          return (0);
        }
#endif

#ifdef CONFIRM_LOCKING
        if (iflags.confirm_locking) {
#endif

          sprintf(qbuf, "%sock it?", (door->doormask & D_LOCKED) ? "Unl" : "L");

          c = yn(qbuf);
          if (c == 'n')
            return (0);

#ifdef CONFIRM_LOCKING
        }
#endif

        switch (picktyp) {
#ifdef TOURIST
          case CREDIT_CARD:
            ch = 2 * ACURR(A_DEX) + 20 * Role_if(PM_ROGUE);
            break;
#endif
          case LOCK_PICK:
            ch = 3 * ACURR(A_DEX) + 30 * Role_if(PM_ROGUE);
            break;
          case SKELETON_KEY:
            ch = 70 + ACURR(A_DEX);
            break;
          default:
            ch = 0;
        }
        xlock.door = door;
        xlock.box = 0;
    }
  }
  flags.move = 0;
  xlock.chance = ch;
  xlock.picktyp = picktyp;
  xlock.usedtime = 0;
  set_occupation(picklock, lock_action(), 0);
  return (1);
}

/* try to force a chest with your weapon */
int doforce() {
  Object *otmp;
  int c, picktyp;
  char qbuf[QBUFSZ];

  if (!uwep || /* proper type test */
      (uwep->oclass != WEAPON_CLASS && !is_weptool(uwep) &&
       uwep->oclass != ROCK_CLASS) ||
      (objects[uwep->otyp].oc_skill < P_DAGGER) ||
      (objects[uwep->otyp].oc_skill > P_LANCE) || uwep->otyp == FLAIL ||
      uwep->otyp == AKLYS
#ifdef KOPS
      || uwep->otyp == RUBBER_HOSE
#endif
      ) {
    You_cant("force anything without a %sweapon.", (uwep) ? "proper " : "");
    return (0);
  }

  picktyp = is_blade(uwep);
  if (xlock.usedtime && xlock.box && picktyp == xlock.picktyp) {
    You("resume your attempt to force the lock.");
    set_occupation(forcelock, "forcing the lock", 0);
    return (1);
  }

  /* A lock is made only for the honest man, the thief will break it. */
  xlock.box = nullptr;
  for (otmp = level.objects[player.ux][player.uy]; otmp; otmp = otmp->nexthere)
    if (Is_box(otmp)) {
      if (otmp->obroken || !otmp->olocked) {
        There("is %s here, but its lock is already %s.", doname(otmp),
              otmp->obroken ? "broken" : "unlocked");
        continue;
      }
      sprintf(
          qbuf, "There is %s here, force its lock?",
          safe_qbuf("", sizeof("There is  here, force its lock?"), doname(otmp),
                    an(simple_typename(otmp->otyp)), "a box"));

      c = ynq(qbuf);
      if (c == 'q')
        return (0);
      if (c == 'n')
        continue;

      if (picktyp)
        You("force your %s into a crack and pry.", xname(uwep));
      else
        You("start bashing it with your %s.", xname(uwep));
      xlock.box = otmp;
      xlock.chance = objects[uwep->otyp].oc_wldam * 2;
      xlock.picktyp = picktyp;
      xlock.usedtime = 0;
      break;
    }

  if (xlock.box)
    set_occupation(forcelock, "forcing the lock", 0);
  else
    You("decide not to force the issue.");
  return (1);
}

/* try to open a door */
int doopen() {
  coord cc;
  struct rm *door;
  Monster *mtmp;

  if (nohands(youmonst.data)) {
    You_cant("open anything -- you have no hands!");
    return 0;
  }

  if (player.utrap && player.utraptype == TT_PIT) {
    You_cant("reach over the edge of the pit.");
    return 0;
  }

  if (!get_adjacent_loc(nullptr, nullptr, player.ux, player.uy, &cc))
    return (0);

  if ((cc.x == player.ux) && (cc.y == player.uy))
    return (0);

  if ((mtmp = m_at(cc.x, cc.y)) && mtmp->m_ap_type == M_AP_FURNITURE &&
      (mtmp->mappearance == S_hcdoor || mtmp->mappearance == S_vcdoor) &&
      !Protection_from_shape_changers) {
    stumble_onto_mimic(mtmp);
    return (1);
  }

  door = &levl[cc.x][cc.y];

  if (!IS_DOOR(door->typ)) {
    if (is_db_wall(cc.x, cc.y)) {
      There("is no obvious way to open the drawbridge.");
      return (0);
    }
    You("%s no door there.", Blind ? "feel" : "see");
    return (0);
  }

  if (!(door->doormask & D_CLOSED)) {
    const char *mesg;

    switch (door->doormask) {
      case D_BROKEN:
        mesg = " is broken";
        break;
      case D_NODOOR:
        mesg = "way has no door";
        break;
      case D_ISOPEN:
        mesg = " is already open";
        break;
      default:
        mesg = " is locked";
        break;
    }
    pline("This door%s.", mesg);
    if (Blind)
      feel_location(cc.x, cc.y);
    return (0);
  }

  if (verysmall(youmonst.data)) {
    pline("You're too small to pull the door open.");
    return (0);
  }

  /* door is known to be CLOSED */
  if (rnl(20) < (ACURRSTR + ACURR(A_DEX) + ACURR(A_CON)) / 3) {
    pline_The("door opens.");
    if (door->doormask & D_TRAPPED) {
      b_trapped("door", FINGER);
      door->doormask = D_NODOOR;
      if (*in_rooms(cc.x, cc.y, SHOPBASE))
        add_damage(cc.x, cc.y, 0L);
    } else
      door->doormask = D_ISOPEN;
    if (Blind)
      feel_location(cc.x, cc.y); /* the hero knows she opened it  */
    else
      newsym(cc.x, cc.y);
    unblock_point(cc.x, cc.y); /* vision: new see through there */
  } else {
    exercise(A_STR, TRUE);
    pline_The("door resists!");
  }

  return (1);
}

STATIC_OVL
bool obstructed(int x, int y) {
  Monster *mtmp = m_at(x, y);

  if (mtmp && mtmp->m_ap_type != M_AP_FURNITURE) {
    if (mtmp->m_ap_type == M_AP_OBJECT)
      goto objhere;
    pline("%s stands in the way!",
          !canspotmon(mtmp) ? "Some creature" : Monnam(mtmp));
    if (!canspotmon(mtmp))
      map_invisible(mtmp->mx, mtmp->my);
    return (TRUE);
  }
  if (OBJ_AT(x, y)) {
  objhere:
    pline("%s's in the way.", Something);
    return (TRUE);
  }
  return (FALSE);
}

/* try to close a door */
int doclose() {
  int x, y;
  struct rm *door;
  Monster *mtmp;

  if (nohands(youmonst.data)) {
    You_cant("close anything -- you have no hands!");
    return 0;
  }

  if (player.utrap && player.utraptype == TT_PIT) {
    You_cant("reach over the edge of the pit.");
    return 0;
  }

  if (!getdir(nullptr))
    return (0);

  x = player.ux + player.dx;
  y = player.uy + player.dy;
  if ((x == player.ux) && (y == player.uy)) {
    You("are in the way!");
    return (1);
  }

  if ((mtmp = m_at(x, y)) && mtmp->m_ap_type == M_AP_FURNITURE &&
      (mtmp->mappearance == S_hcdoor || mtmp->mappearance == S_vcdoor) &&
      !Protection_from_shape_changers) {
    stumble_onto_mimic(mtmp);
    return (1);
  }

  door = &levl[x][y];

  if (!IS_DOOR(door->typ)) {
    if (door->typ == DRAWBRIDGE_DOWN)
      There("is no obvious way to close the drawbridge.");
    else
      You("%s no door there.", Blind ? "feel" : "see");
    return (0);
  }

  if (door->doormask == D_NODOOR) {
    pline("This doorway has no door.");
    return (0);
  }

  if (obstructed(x, y))
    return (0);

  if (door->doormask == D_BROKEN) {
    pline("This door is broken.");
    return (0);
  }

  if (door->doormask & (D_CLOSED | D_LOCKED)) {
    pline("This door is already closed.");
    return (0);
  }

  if (door->doormask == D_ISOPEN) {
    if (verysmall(youmonst.data)
#ifdef STEED
        && !player.usteed
#endif
        ) {
      pline("You're too small to push the door closed.");
      return (0);
    }
    if (
#ifdef STEED
            player.usteed ||
#endif
            rn2(25) < (ACURRSTR + ACURR(A_DEX) + ACURR(A_CON)) / 3) {
      pline_The("door closes.");
      door->doormask = D_CLOSED;
      if (Blind)
        feel_location(x, y); /* the hero knows she closed it */
      else
        newsym(x, y);
      block_point(x, y); /* vision:  no longer see there */
    } else {
      exercise(A_STR, TRUE);
      pline_The("door resists!");
    }
  }

  return (1);
}

/* returns true if something happened */
bool /* box obj was hit with spell effect otmp */ boxlock(Object *obj,
                                                          Object *otmp) {
  bool res = 0;

  switch (otmp->otyp) {
    case WAN_LOCKING:
    case SPE_WIZARD_LOCK:
      if (!obj->olocked) { /* lock it; fix if broken */
        pline("Klunk!");
        obj->olocked = 1;
        obj->obroken = 0;
        res = 1;
      } /* else already closed and locked */
      break;
    case WAN_OPENING:
    case SPE_KNOCK:
      if (obj->olocked) { /* unlock; couldn't be broken */
        pline("Klick!");
        obj->olocked = 0;
        res = 1;
      } else /* silently fix if broken */
        obj->obroken = 0;
      break;
    case WAN_POLYMORPH:
    case SPE_POLYMORPH:
      /* maybe start unlocking chest, get interrupted, then zap it;
         we must avoid any attempt to resume unlocking it */
      if (xlock.box == obj)
        reset_pick();
      break;
  }
  return res;
}

/* returns true if something happened */
bool /* Door/secret door was hit with spell effect otmp */ doorlock(
    Object *otmp, int x, int y) {
  struct rm *door = &levl[x][y];
  bool res = TRUE;
  int loudness = 0;
  const char *msg = (const char *)0;
  const char *dustcloud = "A cloud of dust";
  const char *quickly_dissipates = "quickly dissipates";

  if (door->typ == SDOOR) {
    switch (otmp->otyp) {
      case WAN_OPENING:
      case SPE_KNOCK:
      case WAN_STRIKING:
      case SPE_FORCE_BOLT:
        door->typ = DOOR;
        door->doormask = D_CLOSED | (door->doormask & D_TRAPPED);
        newsym(x, y);
        if (cansee(x, y))
          pline("A door appears in the wall!");
        if (otmp->otyp == WAN_OPENING || otmp->otyp == SPE_KNOCK)
          return TRUE;
        break; /* striking: continue door handling below */
      case WAN_LOCKING:
      case SPE_WIZARD_LOCK:
      default:
        return FALSE;
    }
  }

  switch (otmp->otyp) {
    case WAN_LOCKING:
    case SPE_WIZARD_LOCK:
#ifdef REINCARNATION
      if (Is_rogue_level(&player.uz)) {
        bool vis = cansee(x, y);
        /* Can't have real locking in Rogue, so just hide doorway */
        if (vis)
          pline("%s springs up in the older, more primitive doorway.",
                dustcloud);
        else
          You_hear("a swoosh.");
        if (obstructed(x, y)) {
          if (vis)
            pline_The("cloud %s.", quickly_dissipates);
          return FALSE;
        }
        block_point(x, y);
        door->typ = SDOOR;
        if (vis)
          pline_The("doorway vanishes!");
        newsym(x, y);
        return TRUE;
      }
#endif
      if (obstructed(x, y))
        return FALSE;
      /* Don't allow doors to close over traps.  This is for pits */
      /* & trap doors, but is it ever OK for anything else? */
      if (t_at(x, y)) {
        /* maketrap() clears doormask, so it should be NODOOR */
        pline("%s springs up in the doorway, but %s.", dustcloud,
              quickly_dissipates);
        return FALSE;
      }

      switch (door->doormask & ~D_TRAPPED) {
        case D_CLOSED:
          msg = "The door locks!";
          break;
        case D_ISOPEN:
          msg = "The door swings shut, and locks!";
          break;
        case D_BROKEN:
          msg = "The broken door reassembles and locks!";
          break;
        case D_NODOOR:
          msg = "A cloud of dust springs up and assembles itself into a door!";
          break;
        default:
          res = FALSE;
          break;
      }
      block_point(x, y);
      door->doormask = D_LOCKED | (door->doormask & D_TRAPPED);
      newsym(x, y);
      break;
    case WAN_OPENING:
    case SPE_KNOCK:
      if (door->doormask & D_LOCKED) {
        msg = "The door unlocks!";
        door->doormask = D_CLOSED | (door->doormask & D_TRAPPED);
      } else
        res = FALSE;
      break;
    case WAN_STRIKING:
    case SPE_FORCE_BOLT:
      if (door->doormask & (D_LOCKED | D_CLOSED)) {
        if (door->doormask & D_TRAPPED) {
          if (MON_AT(x, y))
            (void)mb_trapped(m_at(x, y));
          else if (flags.verbose) {
            if (cansee(x, y))
              pline("KABOOM!!  You see a door explode.");
            else if (flags.soundok)
              You_hear("a distant explosion.");
          }
          door->doormask = D_NODOOR;
          unblock_point(x, y);
          newsym(x, y);
          loudness = 40;
          break;
        }
        door->doormask = D_BROKEN;
        if (flags.verbose) {
          if (cansee(x, y))
            pline_The("door crashes open!");
          else if (flags.soundok)
            You_hear("a crashing sound.");
        }
        unblock_point(x, y);
        newsym(x, y);
        /* force vision recalc before printing more messages */
        if (vision_full_recalc)
          vision_recalc(0);
        loudness = 20;
      } else
        res = FALSE;
      break;
    default:
      impossible("magic (%d) attempted on door.", otmp->otyp);
      break;
  }
  if (msg && cansee(x, y))
    pline(msg);
  if (loudness > 0) {
    /* door was destroyed */
    wake_nearto(x, y, loudness);
    if (*in_rooms(x, y, SHOPBASE))
      add_damage(x, y, 0L);
  }

  if (res && picking_at(x, y)) {
    /* maybe unseen monster zaps door you're unlocking */
    stop_occupation();
    reset_pick();
  }
  return res;
}

STATIC_OVL void chest_shatter_msg(Object *otmp) {
  const char *disposition;
  const char *thing;
  long save_Blinded;

  if (otmp->oclass == POTION_CLASS) {
    You("%s %s shatter!", Blind ? "hear" : "see", an(bottlename()));
    if (!breathless(youmonst.data) || haseyes(youmonst.data))
      potionbreathe(otmp);
    return;
  }
  /* We have functions for distant and singular names, but not one */
  /* which does _both_... */
  save_Blinded = Blinded;
  Blinded = 1;
  thing = singular(otmp, xname);
  Blinded = save_Blinded;
  switch (objects[otmp->otyp].oc_material) {
    case PAPER:
      disposition = "is torn to shreds";
      break;
    case WAX:
      disposition = "is crushed";
      break;
    case VEGGY:
      disposition = "is pulped";
      break;
    case FLESH:
      disposition = "is mashed";
      break;
    case GLASS:
      disposition = "shatters";
      break;
    case WOOD:
      disposition = "splinters to fragments";
      break;
    default:
      disposition = "is destroyed";
      break;
  }
  pline("%s %s!", An(thing), disposition);
}


/*lock.c*/
