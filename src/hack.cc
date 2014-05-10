/*	SCCS Id: @(#)hack.c	3.4	2003/04/30	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include <string.h>

#include "hack.h"
#include "teleport.h"
#include "steed.h"
#include "sounds.h"
#include "shk.h"
#include "role.h"
#include "rnd.h"
#include "priest.h"
#include "polyself.h"
#include "pline.h"
#include "pickup.h"
#include "objnam.h"
#include "o_init.h"
#include "monmove.h"
#include "mon.h"
#include "mkobj.h"
#include "mkmaze.h"
#include "mhitu.h"
#include "mhitm.h"
#include "invent.h"
#include "hacklib.h"
#include "end.h"
#include "eat.h"
#include "dog.h"
#include "do_wear.h"
#include "do_name.h"
#include "do.h"
#include "dig.h"
#include "dbridge.h"
#include "cmd.h"

#include "apply.h"
#include "ball.h"

#ifdef OVL1
STATIC_DCL void maybe_wail();
#endif /*OVL1*/
STATIC_DCL int moverock();
STATIC_DCL int still_chewing(xchar, xchar);
#ifdef SINKS
STATIC_DCL void dosinkfall();
#endif
STATIC_DCL bool findtravelpath(bool);
STATIC_DCL bool monstinroom(MonsterType *, int);

STATIC_DCL void move_update(bool);

#define IS_SHOP(x) (rooms[x].rtype >= SHOPBASE)

#ifdef OVL2

bool revive_nasty(int x, int y, const char *msg) {
  Object *otmp, *otmp2;
  Monster *mtmp;
  coord cc;
  bool revived = FALSE;

  for (otmp = level.objects[x][y]; otmp; otmp = otmp2) {
    otmp2 = otmp->nexthere;
    if (otmp->otyp == CORPSE && (is_rider(&mons[otmp->corpsenm]) ||
                                 otmp->corpsenm == PM_WIZARD_OF_YENDOR)) {
      /* move any living monster already at that location */
      if ((mtmp = m_at(x, y)) && enexto(&cc, x, y, mtmp->data))
        rloc_to(mtmp, cc.x, cc.y);
      if (msg)
        Norep("%s", msg);
      revived = revive_corpse(otmp);
    }
  }

  /* this location might not be safe, if not, move revived monster */
  if (revived) {
    mtmp = m_at(x, y);
    if (mtmp && !goodpos(x, y, mtmp, 0) && enexto(&cc, x, y, mtmp->data)) {
      rloc_to(mtmp, cc.x, cc.y);
    }
    /* else impossible? */
  }

  return (revived);
}

STATIC_OVL int moverock() {
  xchar rx, ry, sx, sy;
  Object *otmp;
  Trap *ttmp;
  Monster *mtmp;

  sx = player.ux + player.dx,
  sy = player.uy + player.dy; /* boulder starting position */
  while ((otmp = sobj_at(BOULDER, sx, sy)) != 0) {
    /* make sure that this boulder is visible as the top object */
    if (otmp != level.objects[sx][sy])
      movobj(otmp, sx, sy);

    rx = player.ux + 2 * player.dx; /* boulder destination position */
    ry = player.uy + 2 * player.dy;
    nomul(0, 0);
    if (Levitation || Is_airlevel(&player.uz)) {
      if (Blind)
        feel_location(sx, sy);
      You("don't have enough leverage to push %s.", the(xname(otmp)));
      /* Give them a chance to climb over it? */
      return -1;
    }
    if (verysmall(youmonst.data)
#ifdef STEED
        && !player.usteed
#endif
        ) {
      if (Blind)
        feel_location(sx, sy);
      pline("You're too small to push that %s.", xname(otmp));
      goto cannot_push;
    }
    if (isok(rx, ry) && !IS_ROCK(levl[rx][ry].typ) &&
        levl[rx][ry].typ != IRONBARS &&
        (!IS_DOOR(levl[rx][ry].typ) || !(player.dx && player.dy) ||
         (
#ifdef REINCARNATION
             !Is_rogue_level(&player.uz) &&
#endif
             (levl[rx][ry].doormask & ~D_BROKEN) == D_NODOOR)) &&
        !sobj_at(BOULDER, rx, ry)) {
      ttmp = t_at(rx, ry);
      mtmp = m_at(rx, ry);

      /* KMH -- Sokoban doesn't let you push boulders diagonally */
      if (In_sokoban(&player.uz) && player.dx && player.dy) {
        if (Blind)
          feel_location(sx, sy);
        pline("%s won't roll diagonally on this %s.", The(xname(otmp)),
              surface(sx, sy));
        goto cannot_push;
      }

      if (revive_nasty(rx, ry, "You sense movement on the other side."))
        return (-1);

      if (mtmp && !noncorporeal(mtmp->data) &&
          (!mtmp->mtrapped ||
           !(ttmp && ((ttmp->ttyp == PIT) || (ttmp->ttyp == SPIKED_PIT))))) {
        if (Blind)
          feel_location(sx, sy);
        if (canspotmon(mtmp))
          pline("There's %s on the other side.", a_monnam(mtmp));
        else {
          You_hear("a monster behind %s.", the(xname(otmp)));
          map_invisible(rx, ry);
        }
        if (flags.verbose)
          pline("Perhaps that's why %s cannot move it.",
#ifdef STEED
                player.usteed ? y_monnam(player.usteed) :
#endif
                              "you");
        goto cannot_push;
      }

      if (ttmp)
        switch (ttmp->ttyp) {
          case LANDMINE:
            if (rn2(10)) {
              RemoveObjectFromStorage(otmp);
              PlaceObject(otmp, rx, ry);
              unblock_point(sx, sy);
              newsym(sx, sy);
              pline("KAABLAMM!!!  %s %s land mine.", Tobjnam(otmp, "trigger"),
                    ttmp->madeby_u ? "your" : "a");
              blow_up_landmine(ttmp);
              /* if the boulder remains, it should fill the pit */
              fill_pit(player.ux, player.uy);
              if (cansee(rx, ry))
                newsym(rx, ry);
              continue;
            }
            break;
          case SPIKED_PIT:
          case PIT:
            RemoveObjectFromStorage(otmp);
            /* vision kludge to get messages right;
               the pit will temporarily be seen even
               if this is one among multiple boulders */
            if (!Blind)
              viz_array[ry][rx] |= IN_SIGHT;
            if (!flooreffects(otmp, rx, ry, "fall")) {
              PlaceObject(otmp, rx, ry);
            }
            if (mtmp && !Blind)
              newsym(rx, ry);
            continue;
          case HOLE:
          case TRAPDOOR:
            if (Blind)
              pline("Kerplunk!  You no longer feel %s.", the(xname(otmp)));
            else
              pline(
                  "%s%s and %s a %s in the %s!",
                  Tobjnam(otmp, (ttmp->ttyp == TRAPDOOR) ? "trigger" : "fall"),
                  (ttmp->ttyp == TRAPDOOR) ? nul : " into",
                  otense(otmp, "plug"),
                  (ttmp->ttyp == TRAPDOOR) ? "trap door" : "hole",
                  surface(rx, ry));
            deltrap(ttmp);
            delobj(otmp);
            bury_objs(rx, ry);
            if (cansee(rx, ry))
              newsym(rx, ry);
            continue;
          case LEVEL_TELEP:
          case TELEP_TRAP:
#ifdef STEED
            if (player.usteed)
              pline("%s pushes %s and suddenly it disappears!",
                    upstart(y_monnam(player.usteed)), the(xname(otmp)));
            else
#endif
              You("push %s and suddenly it disappears!", the(xname(otmp)));
            if (ttmp->ttyp == TELEP_TRAP)
              rloco(otmp);
            else {
              int newlev = random_teleport_level();
              d_level dest;

              if (newlev == depth(&player.uz) || In_endgame(&player.uz))
                continue;
              RemoveObjectFromStorage(otmp);
              AddObjectToMigrationList(otmp);
              get_level(&dest, newlev);
              otmp->ox = dest.dnum;
              otmp->oy = dest.dlevel;
              otmp->owornmask = (long)MIGR_RANDOM;
            }
            seetrap(ttmp);
            continue;
        }
      if (closed_door(rx, ry))
        goto nopushmsg;
      if (boulder_hits_pool(otmp, rx, ry, TRUE))
        continue;
      /*
       * Re-link at top of fobj chain so that pile order is preserved
       * when level is restored.
       */
      if (otmp != fobj) {
        RemoveObjectFromFloor(otmp);
        PlaceObject(otmp, otmp->ox, otmp->oy);
      }

      {
#ifdef LINT /* static long lastmovetime; */
        long lastmovetime;
        lastmovetime = 0;
#else
        /* note: reset to zero after save/restore cycle */
        static long lastmovetime;
#endif
#ifdef STEED
        if (!player.usteed) {
#endif
          if (moves > lastmovetime + 2 || moves < lastmovetime)
            pline("With %s effort you move %s.",
                  throws_rocks(youmonst.data) ? "little" : "great",
                  the(xname(otmp)));
          exercise(A_STR, TRUE);
#ifdef STEED
        } else
          pline("%s moves %s.", upstart(y_monnam(player.usteed)),
                the(xname(otmp)));
#endif
        lastmovetime = moves;
      }

      /* Move the boulder *after* the message. */
      if (glyph_is_invisible(levl[rx][ry].glyph))
        unmap_object(rx, ry);
      movobj(otmp, rx, ry); /* does newsym(rx,ry) */
      if (Blind) {
        feel_location(rx, ry);
        feel_location(sx, sy);
      } else {
        newsym(sx, sy);
      }
    } else {
    nopushmsg:
#ifdef STEED
      if (player.usteed)
        pline("%s tries to move %s, but cannot.",
              upstart(y_monnam(player.usteed)), the(xname(otmp)));
      else
#endif
        You("try to move %s, but in vain.", the(xname(otmp)));
      if (Blind)
        feel_location(sx, sy);
    cannot_push:
      if (throws_rocks(youmonst.data)) {
#ifdef STEED
        if (player.usteed && P_SKILL(P_RIDING) < P_BASIC) {
          You("aren't skilled enough to %s %s from %s.",
              (flags.pickup && !In_sokoban(&player.uz)) ? "pick up"
                                                        : "push aside",
              the(xname(otmp)), y_monnam(player.usteed));
        } else
#endif
        {
          pline("However, you can easily %s.",
                (flags.pickup && !In_sokoban(&player.uz)) ? "pick it up"
                                                          : "push it aside");
          if (In_sokoban(&player.uz))
            change_luck(-1); /* Sokoban guilt */
          break;
        }
        break;
      }

      if (
#ifdef STEED
              !player.usteed &&
#endif
              (((!invent || inv_weight() <= -850) &&
                (!player.dx || !player.dy ||
                 (IS_ROCK(levl[player.ux][sy].typ) &&
                  IS_ROCK(levl[sx][player.uy].typ)))) ||
               verysmall(youmonst.data))) {
        pline("However, you can squeeze yourself into a small opening.");
        if (In_sokoban(&player.uz))
          change_luck(-1); /* Sokoban guilt */
        break;
      } else
        return (-1);
    }
  }
  return (0);
}

/*
 *  still_chewing()
 *
 *  Chew on a wall, door, or boulder.  Returns TRUE if still eating, FALSE
 *  when done.
 */
STATIC_OVL int still_chewing(xchar x, xchar y) {
  struct rm *lev = &levl[x][y];
  Object *boulder = sobj_at(BOULDER, x, y);
  const char *digtxt = nullptr, *dmgtxt = nullptr;

  if (digging.down) /* not continuing previous dig (w/ pick-axe) */
    (void)memset((genericptr_t) & digging, 0, sizeof digging);

  if (!boulder && IS_ROCK(lev->typ) && !may_dig(x, y)) {
    You("hurt your teeth on the %s.",
        IS_TREE(lev->typ) ? "tree" : "hard stone");
    nomul(0, 0);
    return 1;
  } else if (digging.pos.x != x || digging.pos.y != y ||
             !on_level(&digging.level, &player.uz)) {
    digging.down = FALSE;
    digging.chew = TRUE;
    digging.warned = FALSE;
    digging.pos.x = x;
    digging.pos.y = y;
    assign_level(&digging.level, &player.uz);
    /* solid rock takes more work & time to dig through */
    digging.effort =
        (IS_ROCK(lev->typ) && !IS_TREE(lev->typ) ? 30 : 60) + player.udaminc;
    You("start chewing %s %s.",
        (boulder || IS_TREE(lev->typ)) ? "on a" : "a hole in the",
        boulder ? "boulder" : IS_TREE(lev->typ) ? "tree" : IS_ROCK(lev->typ)
                                                               ? "rock"
                                                               : "door");
    watch_dig(nullptr, x, y, FALSE);
    return 1;
  } else if ((digging.effort += (30 + player.udaminc)) <= 100) {
    if (flags.verbose)
      You("%s chewing on the %s.", digging.chew ? "continue" : "begin",
          boulder ? "boulder" : IS_TREE(lev->typ) ? "tree" : IS_ROCK(lev->typ)
                                                                 ? "rock"
                                                                 : "door");
    digging.chew = TRUE;
    watch_dig(nullptr, x, y, FALSE);
    return 1;
  }

  /* Okay, you've chewed through something */
  player.uconduct.food++;
  player.uhunger += rnd(20);

  if (boulder) {
    delobj(boulder);         /* boulder goes bye-bye */
    You("eat the boulder."); /* yum */

    /*
     *  The location could still block because of
     *	1. More than one boulder
     *	2. Boulder stuck in a wall/stone/door.
     *
     *  [perhaps use does_block() below (from vision.c)]
     */
    if (IS_ROCK(lev->typ) || closed_door(x, y) || sobj_at(BOULDER, x, y)) {
      block_point(x, y); /* delobj will unblock the point */
      /* reset dig state */
      (void)memset((genericptr_t) & digging, 0, sizeof digging);
      return 1;
    }

  } else if (IS_WALL(lev->typ)) {
    if (*in_rooms(x, y, SHOPBASE)) {
      add_damage(x, y, 10L * ACURRSTR);
      dmgtxt = "damage";
    }
    digtxt = "chew a hole in the wall.";
    if (level.flags.is_maze_lev) {
      lev->typ = ROOM;
    } else if (level.flags.is_cavernous_lev && !in_town(x, y)) {
      lev->typ = CORR;
    } else {
      lev->typ = DOOR;
      lev->doormask = D_NODOOR;
    }
  } else if (IS_TREE(lev->typ)) {
    digtxt = "chew through the tree.";
    lev->typ = ROOM;
  } else if (lev->typ == SDOOR) {
    if (lev->doormask & D_TRAPPED) {
      lev->doormask = D_NODOOR;
      b_trapped("secret door", 0);
    } else {
      digtxt = "chew through the secret door.";
      lev->doormask = D_BROKEN;
    }
    lev->typ = DOOR;

  } else if (IS_DOOR(lev->typ)) {
    if (*in_rooms(x, y, SHOPBASE)) {
      add_damage(x, y, 400L);
      dmgtxt = "break";
    }
    if (lev->doormask & D_TRAPPED) {
      lev->doormask = D_NODOOR;
      b_trapped("door", 0);
    } else {
      digtxt = "chew through the door.";
      lev->doormask = D_BROKEN;
    }

  } else { /* STONE or SCORR */
    digtxt = "chew a passage through the rock.";
    lev->typ = CORR;
  }

  unblock_point(x, y); /* vision */
  newsym(x, y);
  if (digtxt)
    You(digtxt); /* after newsym */
  if (dmgtxt)
    pay_for_damage(dmgtxt, FALSE);
  (void)memset((genericptr_t) & digging, 0, sizeof digging);
  return 0;
}

#endif /* OVL2 */
#ifdef OVLB

void movobj(Object *obj, xchar ox, xchar oy) {
  /* optimize by leaving on the fobj chain? */
  RemoveObjectFromFloor(obj);
  newsym(obj->ox, obj->oy);
  PlaceObject(obj, ox, oy);
  newsym(ox, oy);
}

#ifdef SINKS
static const char fell_on_sink[] = "fell onto a sink";

STATIC_OVL void dosinkfall() {
  Object *obj;

  if (is_floater(youmonst.data) || (HLevitation & FROMOUTSIDE)) {
    You("wobble unsteadily for a moment.");
  } else {
    long save_ELev = ELevitation, save_HLev = HLevitation;

    /* fake removal of levitation in advance so that final
       disclosure will be right in case this turns out to
       be fatal; fortunately the fact that rings and boots
       are really still worn has no effect on bones data */
    ELevitation = HLevitation = 0L;
    You("crash to the floor!");
    losehp(rn1(8, 25 - (int)ACURR(A_CON)), fell_on_sink, NO_KILLER_PREFIX);
    exercise(A_DEX, FALSE);
    selftouch("Falling, you");
    for (obj = level.objects[player.ux][player.uy]; obj; obj = obj->nexthere)
      if (obj->oclass == WEAPON_CLASS || is_weptool(obj)) {
        You("fell on %s.", doname(obj));
        losehp(rnd(3), fell_on_sink, NO_KILLER_PREFIX);
        exercise(A_CON, FALSE);
      }
    ELevitation = save_ELev;
    HLevitation = save_HLev;
  }

  ELevitation &= ~W_ARTI;
  HLevitation &= ~(I_SPECIAL | TIMEOUT);
  HLevitation++;
  if (uleft && uleft->otyp == RIN_LEVITATION) {
    obj = uleft;
    Ring_off(obj);
    off_msg(obj);
  }
  if (uright && uright->otyp == RIN_LEVITATION) {
    obj = uright;
    Ring_off(obj);
    off_msg(obj);
  }
  if (uarmf && uarmf->otyp == LEVITATION_BOOTS) {
    obj = uarmf;
    (void)Boots_off();
    off_msg(obj);
  }
  HLevitation--;
}
#endif

/* intended to be called only on ROCKs */
bool may_dig(xchar x, xchar y) {
  return (bool)(!(IS_STWALL(levl[x][y].typ) &&
                  (levl[x][y].wall_info & W_NONDIGGABLE)));
}

bool may_passwall(xchar x, xchar y) {
  return (bool)(!(IS_STWALL(levl[x][y].typ) &&
                  (levl[x][y].wall_info & W_NONPASSWALL)));
}

#endif /* OVLB */
#ifdef OVL1

bool bad_rock(MonsterType *mdat, xchar x, xchar y) {
  return ((bool)((In_sokoban(&player.uz) && sobj_at(BOULDER, x, y)) ||
                 (IS_ROCK(levl[x][y].typ) &&
                  (!tunnels(mdat) || needspick(mdat) || !may_dig(x, y)) &&
                  !(passes_walls(mdat) && may_passwall(x, y)))));
}

bool invocation_pos(xchar x, xchar y) {
  return (
      (bool)(Invocation_lev(&player.uz) && x == inv_pos.x && y == inv_pos.y));
}

#endif /* OVL1 */
#ifdef OVL3

/* return TRUE if (dx,dy) is an OK place to move
 * mode is one of DO_MOVE, TEST_MOVE or TEST_TRAV
 */
bool test_move(int ux, int uy, int dx, int dy, int mode) {
  int x = ux + dx;
  int y = uy + dy;
  struct rm *tmpr = &levl[x][y];
  struct rm *ust;

  /*
   *  Check for physical obstacles.  First, the place we are going.
   */
  if (IS_ROCK(tmpr->typ) || tmpr->typ == IRONBARS) {
    if (Blind && mode == DO_MOVE)
      feel_location(x, y);
    if (Passes_walls && may_passwall(x, y)) {
      ; /* do nothing */
    } else if (tmpr->typ == IRONBARS) {
      if (!(Passes_walls || passes_bars(youmonst.data)))
        return FALSE;
    } else if (tunnels(youmonst.data) && !needspick(youmonst.data)) {
      /* Eat the rock. */
      if (mode == DO_MOVE && still_chewing(x, y))
        return FALSE;
    } else if (flags.autodig && !flags.run && !flags.nopick && uwep &&
               is_pick(uwep)) {
      /* MRKR: Automatic digging when wielding the appropriate tool */
      if (mode == DO_MOVE)
        (void)use_pick_axe2(uwep);
      return FALSE;
    } else {
      if (mode == DO_MOVE) {
        if (Is_stronghold(&player.uz) && is_db_wall(x, y))
          pline_The("drawbridge is up!");
        if (Passes_walls && !may_passwall(x, y) && In_sokoban(&player.uz))
          pline_The("Sokoban walls resist your ability.");
      }
      return FALSE;
    }
  } else if (IS_DOOR(tmpr->typ)) {
    if (closed_door(x, y)) {
      if (Blind && mode == DO_MOVE)
        feel_location(x, y);
      if (Passes_walls)
        ; /* do nothing */
      else if (can_ooze(&youmonst)) {
        if (mode == DO_MOVE)
          You("ooze under the door.");
      } else if (tunnels(youmonst.data) && !needspick(youmonst.data)) {
        /* Eat the door. */
        if (mode == DO_MOVE && still_chewing(x, y))
          return FALSE;
      } else {
        if (mode == DO_MOVE) {
          if (amorphous(youmonst.data))
            You("try to ooze under the door, but can't squeeze your "
                "possessions through.");
          else if (x == ux || y == uy) {
            if (Blind || Stunned || ACURR(A_DEX) < 10 || Fumbling) {
#ifdef STEED
              if (player.usteed) {
                You_cant("lead %s through that closed door.",
                         y_monnam(player.usteed));
              } else
#endif
              {
                pline("Ouch!  You bump into a door.");
                exercise(A_DEX, FALSE);
              }
            } else
              pline("That door is closed.");
          }
        } else if (mode == TEST_TRAV)
          goto testdiag;
        return FALSE;
      }
    } else {
    testdiag:
      if (dx && dy && !Passes_walls && ((tmpr->doormask & ~D_BROKEN)
#ifdef REINCARNATION
                                        || Is_rogue_level(&player.uz)
#endif
                                        || block_door(x, y))) {
        /* Diagonal moves into a door are not allowed. */
        if (Blind && mode == DO_MOVE)
          feel_location(x, y);
        return FALSE;
      }
    }
  }
  if (dx && dy && bad_rock(youmonst.data, ux, y) &&
      bad_rock(youmonst.data, x, uy)) {
    /* Move at a diagonal. */
    if (In_sokoban(&player.uz)) {
      if (mode == DO_MOVE)
        You("cannot pass that way.");
      return FALSE;
    }
    if (bigmonst(youmonst.data)) {
      if (mode == DO_MOVE)
        Your("body is too large to fit through.");
      return FALSE;
    }
    if (invent && (inv_weight() + weight_cap() > 600)) {
      if (mode == DO_MOVE)
        You("are carrying too much to get through.");
      return FALSE;
    }
  }
  /* Pick travel path that does not require crossing a trap.
   * Avoid water and lava using the usual running rules.
   * (but not player.ux/player.uy because findtravelpath walks toward
   * player.ux/player.uy) */
  if (flags.run == 8 && mode != DO_MOVE && (x != player.ux || y != player.uy)) {
    Trap *t = t_at(x, y);

    if ((t && t->tseen) ||
        (!Levitation && !Flying && !is_clinger(youmonst.data) &&
         (is_pool(x, y) || is_lava(x, y)) && levl[x][y].seenv))
      return FALSE;
  }

  ust = &levl[ux][uy];

  /* Now see if other things block our way . . */
  if (dx && dy && !Passes_walls &&
      (IS_DOOR(ust->typ) && ((ust->doormask & ~D_BROKEN)
#ifdef REINCARNATION
                             || Is_rogue_level(&player.uz)
#endif
                             || block_entry(x, y)))) {
    /* Can't move at a diagonal out of a doorway with door. */
    return FALSE;
  }

  if (sobj_at(BOULDER, x, y) && (In_sokoban(&player.uz) || !Passes_walls)) {
    if (!(Blind || Hallucination) && (flags.run >= 2) && mode != TEST_TRAV)
      return FALSE;
    if (mode == DO_MOVE) {
      /* tunneling monsters will chew before pushing */
      if (tunnels(youmonst.data) && !needspick(youmonst.data) &&
          !In_sokoban(&player.uz)) {
        if (still_chewing(x, y))
          return FALSE;
      } else if (moverock() < 0)
        return FALSE;
    } else if (mode == TEST_TRAV) {
      Object *obj;

      /* don't pick two boulders in a row, unless there's a way thru */
      if (sobj_at(BOULDER, ux, uy) && !In_sokoban(&player.uz)) {
        if (!Passes_walls &&
            !(tunnels(youmonst.data) && !needspick(youmonst.data)) &&
            !carrying(PICK_AXE) && !carrying(DWARVISH_MATTOCK) &&
            !((obj = carrying(WAN_DIGGING)) &&
              !objects[obj->otyp].oc_name_known))
          return FALSE;
      }
    }
    /* assume you'll be able to push it when you get there... */
  }

  /* OK, it is a legal place to move. */
  return TRUE;
}

/*
 * Find a path from the destination (player.tx,player.ty) back to
 * (player.ux,player.uy).
 * A shortest path is returned.  If guess is TRUE, consider various
 * inaccessible locations as valid intermediate path points.
 * Returns TRUE if a path was found.
 */
static bool findtravelpath(bool guess) {
  /* if travel to adjacent, reachable location, use normal movement rules */
  if (!guess && iflags.travel1 &&
      distmin(player.ux, player.uy, player.tx, player.ty) == 1) {
    flags.run = 0;
    if (test_move(player.ux, player.uy, player.tx - player.ux,
                  player.ty - player.uy, TEST_MOVE)) {
      player.dx = player.tx - player.ux;
      player.dy = player.ty - player.uy;
      nomul(0, 0);
      iflags.travelcc.x = iflags.travelcc.y = -1;
      return TRUE;
    }
    flags.run = 8;
  }
  if (player.tx != player.ux || player.ty != player.uy) {
    xchar travel[COLNO][ROWNO];
    xchar travelstepx[2][COLNO * ROWNO];
    xchar travelstepy[2][COLNO * ROWNO];
    xchar tx, ty, ux, uy;
    int n = 1;      /* max offset in travelsteps */
    int set = 0;    /* two sets current and previous */
    int radius = 1; /* search radius */
    int i;

    /* If guessing, first find an "obvious" goal location.  The obvious
     * goal is the position the player knows of, or might figure out
     * (couldsee) that is closest to the target on a straight path.
     */
    if (guess) {
      tx = player.ux;
      ty = player.uy;
      ux = player.tx;
      uy = player.ty;
    } else {
      tx = player.tx;
      ty = player.ty;
      ux = player.ux;
      uy = player.uy;
    }

  noguess:
    (void)memset((genericptr_t)travel, 0, sizeof(travel));
    travelstepx[0][0] = tx;
    travelstepy[0][0] = ty;

    while (n != 0) {
      int nn = 0;

      for (i = 0; i < n; i++) {
        int dir;
        int x = travelstepx[set][i];
        int y = travelstepy[set][i];
        static int ordered[] = {0, 2, 4, 6, 1, 3, 5, 7};
        /* no diagonal movement for grid bugs */
        int dirmax = player.umonnum == PM_GRID_BUG ? 4 : 8;

        for (dir = 0; dir < dirmax; ++dir) {
          int nx = x + xdir[ordered[dir]];
          int ny = y + ydir[ordered[dir]];

          if (!isok(nx, ny))
            continue;
          if ((!Passes_walls && !can_ooze(&youmonst) && closed_door(x, y)) ||
              sobj_at(BOULDER, x, y)) {
            /* closed doors and boulders usually
             * cause a delay, so prefer another path */
            if (travel[x][y] > radius - 3) {
              travelstepx[1 - set][nn] = x;
              travelstepy[1 - set][nn] = y;
              /* don't change travel matrix! */
              nn++;
              continue;
            }
          }
          if (test_move(x, y, nx - x, ny - y, TEST_TRAV) &&
              (levl[nx][ny].seenv || (!Blind && couldsee(nx, ny)))) {
            if (nx == ux && ny == uy) {
              if (!guess) {
                player.dx = x - ux;
                player.dy = y - uy;
                if (x == player.tx && y == player.ty) {
                  nomul(0, 0);
                  /* reset run so domove run checks work */
                  flags.run = 8;
                  iflags.travelcc.x = iflags.travelcc.y = -1;
                }
                return TRUE;
              }
            } else if (!travel[nx][ny]) {
              travelstepx[1 - set][nn] = nx;
              travelstepy[1 - set][nn] = ny;
              travel[nx][ny] = radius;
              nn++;
            }
          }
        }
      }

      n = nn;
      set = 1 - set;
      radius++;
    }

    /* if guessing, find best location in travel matrix and go there */
    if (guess) {
      int px = tx, py = ty; /* pick location */
      int dist, nxtdist, d2, nd2;

      dist = distmin(ux, uy, tx, ty);
      d2 = dist2(ux, uy, tx, ty);
      for (tx = 1; tx < COLNO; ++tx)
        for (ty = 0; ty < ROWNO; ++ty)
          if (travel[tx][ty]) {
            nxtdist = distmin(ux, uy, tx, ty);
            if (nxtdist == dist && couldsee(tx, ty)) {
              nd2 = dist2(ux, uy, tx, ty);
              if (nd2 < d2) {
                /* prefer non-zigzag path */
                px = tx;
                py = ty;
                d2 = nd2;
              }
            } else if (nxtdist < dist && couldsee(tx, ty)) {
              px = tx;
              py = ty;
              dist = nxtdist;
              d2 = dist2(ux, uy, tx, ty);
            }
          }

      if (px == player.ux && py == player.uy) {
        /* no guesses, just go in the general direction */
        player.dx = sgn(player.tx - player.ux);
        player.dy = sgn(player.ty - player.uy);
        if (test_move(player.ux, player.uy, player.dx, player.dy, TEST_MOVE))
          return TRUE;
        goto found;
      }
      tx = px;
      ty = py;
      ux = player.ux;
      uy = player.uy;
      set = 0;
      n = radius = 1;
      guess = FALSE;
      goto noguess;
    }
    return FALSE;
  }

found:
  player.dx = 0;
  player.dy = 0;
  nomul(0, 0);
  return FALSE;
}

void domove() {
  Monster *mtmp;
  struct rm *tmpr;
  xchar x, y;
  Trap *trap;
  int wtcap;
  bool on_ice;
  xchar chainx, chainy, ballx, bally; /* ball&chain new positions */
  int bc_control;                     /* control for ball&chain */
  bool cause_delay = FALSE;           /* dragging ball will skip a move */
  const char *predicament;

  u_wipe_engr(rnd(5));

  if (flags.travel) {
    if (!findtravelpath(FALSE))
      (void)findtravelpath(TRUE);
    iflags.travel1 = 0;
  }

  if (((wtcap = near_capacity()) >= OVERLOADED ||
       (wtcap > SLT_ENCUMBER &&
        (Upolyd ? (player.mh < 5 && player.mh != player.mhmax)
                : (player.uhp < 10 && player.uhp != player.uhpmax)))) &&
      !Is_airlevel(&player.uz)) {
    if (wtcap < OVERLOADED) {
      You("don't have enough stamina to move.");
      exercise(A_CON, FALSE);
    } else
      You("collapse under your load.");
    nomul(0, 0);
    return;
  }
  if (player.uswallow) {
    player.dx = player.dy = 0;
    player.ux = x = player.ustuck->mx;
    player.uy = y = player.ustuck->my;
    mtmp = player.ustuck;
  } else {
    if (Is_airlevel(&player.uz) && rn2(4) && !Levitation && !Flying) {
      switch (rn2(3)) {
        case 0:
          You("tumble in place.");
          exercise(A_DEX, FALSE);
          break;
        case 1:
          You_cant("control your movements very well.");
          break;
        case 2:
          pline("It's hard to walk in thin air.");
          exercise(A_DEX, TRUE);
          break;
      }
      return;
    }

    /* check slippery ice */
    on_ice = !Levitation && is_ice(player.ux, player.uy);
    if (on_ice) {
      static int skates = 0;
      if (!skates)
        skates = find_skates();
      if ((uarmf && uarmf->otyp == skates) || resists_cold(&youmonst) ||
          Flying || is_floater(youmonst.data) || is_clinger(youmonst.data) ||
          is_whirly(youmonst.data))
        on_ice = FALSE;
      else if (!rn2(Cold_resistance ? 3 : 2)) {
        HFumbling |= FROMOUTSIDE;
        HFumbling &= ~TIMEOUT;
        HFumbling += 1; /* slip on next move */
      }
    }
    if (!on_ice && (HFumbling & FROMOUTSIDE))
      HFumbling &= ~FROMOUTSIDE;

    x = player.ux + player.dx;
    y = player.uy + player.dy;
    if (Stunned || (Confusion && !rn2(5))) {
      int tries = 0;

      do {
        if (tries++ > 50) {
          nomul(0, 0);
          return;
        }
        confdir();
        x = player.ux + player.dx;
        y = player.uy + player.dy;
      } while (!isok(x, y) || bad_rock(youmonst.data, x, y));
    }
    /* turbulence might alter your actual destination */
    if (player.uinwater) {
      water_friction();
      if (!player.dx && !player.dy) {
        nomul(0, 0);
        return;
      }
      x = player.ux + player.dx;
      y = player.uy + player.dy;
    }
    if (!isok(x, y)) {
      nomul(0, 0);
      return;
    }
    if (((trap = t_at(x, y)) && trap->tseen) ||
        (Blind && !Levitation && !Flying && !is_clinger(youmonst.data) &&
         (is_pool(x, y) || is_lava(x, y)) && levl[x][y].seenv)) {
      if (flags.run >= 2) {
        nomul(0, 0);
        flags.move = 0;
        return;
      } else
        nomul(0, 0);
    }

    if (player.ustuck && (x != player.ustuck->mx || y != player.ustuck->my)) {
      if (distu(player.ustuck->mx, player.ustuck->my) > 2) {
        /* perhaps it fled (or was teleported or ... ) */
        player.ustuck = 0;
      } else if (sticks(youmonst.data)) {
        /* When polymorphed into a sticking monster,
         * player.ustuck means it's stuck to you, not you to it.
         */
        You("release %s.", mon_nam(player.ustuck));
        player.ustuck = 0;
      } else {
        /* If holder is asleep or paralyzed:
         *	37.5% chance of getting away,
         *	12.5% chance of waking/releasing it;
         * otherwise:
         *	 7.5% chance of getting away.
         * [strength ought to be a factor]
         * If holder is tame and there is no conflict,
         * guaranteed escape.
         */
        switch (rn2(!player.ustuck->mcanmove ? 8 : 40)) {
          case 0:
          case 1:
          case 2:
          pull_free:
            You("pull free from %s.", mon_nam(player.ustuck));
            player.ustuck = 0;
            break;
          case 3:
            if (!player.ustuck->mcanmove) {
              /* it's free to move on next turn */
              player.ustuck->mfrozen = 1;
              player.ustuck->msleeping = 0;
            }
          /*FALLTHRU*/
          default:
            if (player.ustuck->mtame && !Conflict && !player.ustuck->mconf)
              goto pull_free;
            You("cannot escape from %s!", mon_nam(player.ustuck));
            nomul(0, 0);
            return;
        }
      }
    }

    mtmp = m_at(x, y);
    if (mtmp) {
      /* Don't attack if you're running, and can see it */
      /* We should never get here if forcefight */
      if (flags.run && ((!Blind && mon_visible(mtmp) &&
                         ((mtmp->m_ap_type != M_AP_FURNITURE &&
                           mtmp->m_ap_type != M_AP_OBJECT) ||
                          Protection_from_shape_changers)) ||
                        sensemon(mtmp))) {
        nomul(0, 0);
        flags.move = 0;
        return;
      }
    }
  }

  player.ux0 = player.ux;
  player.uy0 = player.uy;
  bhitpos.x = x;
  bhitpos.y = y;
  tmpr = &levl[x][y];

  /* attack monster */
  if (mtmp) {
    nomul(0, 0);
    /* only attack if we know it's there */
    /* or if we used the 'F' command to fight blindly */
    /* or if it hides_under, in which case we call attack() to print
     * the Wait! message.
     * This is different from ceiling hiders, who aren't handled in
     * attack().
     */

    /* If they used a 'm' command, trying to move onto a monster
     * prints the below message and wastes a turn.  The exception is
     * if the monster is unseen and the player doesn't remember an
     * invisible monster--then, we fall through to attack() and
     * attack_check(), which still wastes a turn, but prints a
     * different message and makes the player remember the monster.
     */
    if (flags.nopick &&
        (canspotmon(mtmp) || glyph_is_invisible(levl[x][y].glyph))) {
      if (mtmp->m_ap_type && !Protection_from_shape_changers && !sensemon(mtmp))
        stumble_onto_mimic(mtmp);
      else if (mtmp->mpeaceful && !Hallucination)
        pline("Pardon me, %s.", m_monnam(mtmp));
      else
        You("move right into %s.", mon_nam(mtmp));
      return;
    }
    if (flags.forcefight || !mtmp->mundetected || sensemon(mtmp) ||
        ((hides_under(mtmp->data) || mtmp->data->mlet == S_EEL) &&
         !is_safepet(mtmp))) {
      gethungry();
      if (wtcap >= HVY_ENCUMBER && moves % 3) {
        if (Upolyd && player.mh > 1) {
          player.mh--;
        } else if (!Upolyd && player.uhp > 1) {
          player.uhp--;
        } else {
          You("pass out from exertion!");
          exercise(A_CON, FALSE);
          fall_asleep(-10, FALSE);
        }
      }
      if (multi < 0)
        return; /* we just fainted */

      /* try to attack; note that it might evade */
      /* also, we don't attack tame when _safepet_ */
      if (attack(mtmp))
        return;
    }
  }

  /* specifying 'F' with no monster wastes a turn */
  if (flags.forcefight ||
      /* remembered an 'I' && didn't use a move command */
      (glyph_is_invisible(levl[x][y].glyph) && !flags.nopick)) {
    bool expl = (Upolyd && attacktype(youmonst.data, AT_EXPL));
    char buf[BUFSZ];
    sprintf(buf, "a vacant spot on the %s", surface(x, y));
    You("%s %s.", expl ? "explode at" : "attack",
        !Underwater ? "thin air" : is_pool(x, y) ? "empty water" : buf);
    unmap_object(x, y); /* known empty -- remove 'I' if present */
    newsym(x, y);
    nomul(0, 0);
    if (expl) {
      player.mh = -1; /* dead in the current form */
      rehumanize();
    }
    return;
  }
  if (glyph_is_invisible(levl[x][y].glyph)) {
    unmap_object(x, y);
    newsym(x, y);
  }
/* not attacking an animal, so we try to move */
#ifdef STEED
  if (player.usteed && !player.usteed->mcanmove && (player.dx || player.dy)) {
    pline("%s won't move!", upstart(y_monnam(player.usteed)));
    nomul(0, 0);
    return;
  } else
#endif
      if (!youmonst.data->mmove) {
    You("are rooted %s.",
        Levitation || Is_airlevel(&player.uz) || Is_waterlevel(&player.uz)
            ? "in place"
            : "to the ground");
    nomul(0, 0);
    return;
  }
  if (player.utrap) {
    if (player.utraptype == TT_PIT) {
      if (!rn2(2) && sobj_at(BOULDER, player.ux, player.uy)) {
        Your("%s gets stuck in a crevice.", body_part(LEG));
        display_nhwindow(WIN_MESSAGE, FALSE);
        clear_nhwindow(WIN_MESSAGE);
        You("free your %s.", body_part(LEG));
      } else if (!(--player.utrap)) {
        You("%s to the edge of the pit.",
            (In_sokoban(&player.uz) && Levitation)
                ? "struggle against the air currents and float"
                :
#ifdef STEED
                player.usteed ? "ride" :
#endif
                              "crawl");
        fill_pit(player.ux, player.uy);
        vision_full_recalc = 1; /* vision limits change */
      } else if (flags.verbose) {
#ifdef STEED
        if (player.usteed)
          Norep("%s is still in a pit.", upstart(y_monnam(player.usteed)));
        else
#endif
          Norep((Hallucination && !rn2(5))
                    ? "You've fallen, and you can't get up."
                    : "You are still in a pit.");
      }
    } else if (player.utraptype == TT_LAVA) {
      if (flags.verbose) {
        predicament = "stuck in the lava";
#ifdef STEED
        if (player.usteed)
          Norep("%s is %s.", upstart(y_monnam(player.usteed)), predicament);
        else
#endif
          Norep("You are %s.", predicament);
      }
      if (!is_lava(x, y)) {
        player.utrap--;
        if ((player.utrap & 0xff) == 0) {
#ifdef STEED
          if (player.usteed)
            You("lead %s to the edge of the lava.", y_monnam(player.usteed));
          else
#endif
            You("pull yourself to the edge of the lava.");
          player.utrap = 0;
        }
      }
      player.umoved = TRUE;
    } else if (player.utraptype == TT_WEB) {
      if (uwep && uwep->oartifact == ART_STING) {
        player.utrap = 0;
        pline("Sting cuts through the web!");
        return;
      }
      if (--player.utrap) {
        if (flags.verbose) {
          predicament = "stuck to the web";
#ifdef STEED
          if (player.usteed)
            Norep("%s is %s.", upstart(y_monnam(player.usteed)), predicament);
          else
#endif
            Norep("You are %s.", predicament);
        }
      } else {
#ifdef STEED
        if (player.usteed)
          pline("%s breaks out of the web.", upstart(y_monnam(player.usteed)));
        else
#endif
          You("disentangle yourself.");
      }
    } else if (player.utraptype == TT_INFLOOR) {
      if (--player.utrap) {
        if (flags.verbose) {
          predicament = "stuck in the";
#ifdef STEED
          if (player.usteed)
            Norep("%s is %s %s.", upstart(y_monnam(player.usteed)), predicament,
                  surface(player.ux, player.uy));
          else
#endif
            Norep("You are %s %s.", predicament, surface(player.ux, player.uy));
        }
      } else {
#ifdef STEED
        if (player.usteed)
          pline("%s finally wiggles free.", upstart(y_monnam(player.usteed)));
        else
#endif
          You("finally wiggle free.");
      }
    } else {
      if (flags.verbose) {
        predicament = "caught in a bear trap";
#ifdef STEED
        if (player.usteed)
          Norep("%s is %s.", upstart(y_monnam(player.usteed)), predicament);
        else
#endif
          Norep("You are %s.", predicament);
      }
      if ((player.dx && player.dy) || !rn2(5))
        player.utrap--;
    }
    return;
  }

  if (!test_move(player.ux, player.uy, x - player.ux, y - player.uy, DO_MOVE)) {
    flags.move = 0;
    nomul(0, 0);
    return;
  }

  /* Move ball and chain.  */
  if (Punished)
    if (!drag_ball(x, y, &bc_control, &ballx, &bally, &chainx, &chainy,
                   &cause_delay, TRUE))
      return;

  /* Check regions entering/leaving */
  if (!in_out_region(x, y))
    return;

  /* now move the hero */
  mtmp = m_at(x, y);
  player.ux += player.dx;
  player.uy += player.dy;
#ifdef STEED
  /* Move your steed, too */
  if (player.usteed) {
    player.usteed->mx = player.ux;
    player.usteed->my = player.uy;
    exercise_steed();
  }
#endif

  /*
   * If safepet at destination then move the pet to the hero's
   * previous location using the same conditions as in attack().
   * there are special extenuating circumstances:
   * (1) if the pet dies then your god angers,
   * (2) if the pet gets trapped then your god may disapprove,
   * (3) if the pet was already trapped and you attempt to free it
   * not only do you encounter the trap but you may frighten your
   * pet causing it to go wild!  moral: don't abuse this privilege.
   *
   * Ceiling-hiding pets are skipped by this section of code, to
   * be caught by the normal falling-monster code.
   */
  if (is_safepet(mtmp) && !(is_hider(mtmp->data) && mtmp->mundetected)) {
    /* if trapped, there's a chance the pet goes wild */
    if (mtmp->mtrapped) {
      if (!rn2(mtmp->mtame)) {
        mtmp->mtame = mtmp->mpeaceful = mtmp->msleeping = 0;
        if (mtmp->mleashed)
          m_unleash(mtmp, TRUE);
        growl(mtmp);
      } else {
        yelp(mtmp);
      }
    }
    mtmp->mundetected = 0;
    if (mtmp->m_ap_type)
      seemimic(mtmp);
    else if (!mtmp->mtame)
      newsym(mtmp->mx, mtmp->my);

    if (mtmp->mtrapped && (trap = t_at(mtmp->mx, mtmp->my)) != 0 &&
        (trap->ttyp == PIT || trap->ttyp == SPIKED_PIT) &&
        sobj_at(BOULDER, trap->tx, trap->ty)) {
      /* can't swap places with pet pinned in a pit by a boulder */
      player.ux = player.ux0,
      player.uy = player.uy0; /* didn't move after all */
    } else if (player.ux0 != x && player.uy0 != y &&
               bad_rock(mtmp->data, x, player.uy0) &&
               bad_rock(mtmp->data, player.ux0, y) &&
               (bigmonst(mtmp->data) || (curr_mon_load(mtmp) > 600))) {
      /* can't swap places when pet won't fit thru the opening */
      player.ux = player.ux0,
      player.uy = player.uy0; /* didn't move after all */
      You("stop.  %s won't fit through.", upstart(y_monnam(mtmp)));
    } else {
      char pnambuf[BUFSZ];

      /* save its current description in case of polymorph */
      strcpy(pnambuf, y_monnam(mtmp));
      mtmp->mtrapped = 0;
      remove_monster(x, y);
      place_monster(mtmp, player.ux0, player.uy0);

      /* check for displacing it into pools and traps */
      switch (minliquid(mtmp) ? 2 : mintrap(mtmp)) {
        case 0:
          You("%s %s.", mtmp->mtame ? "displaced" : "frightened", pnambuf);
          break;
        case 1: /* trapped */
        case 3: /* changed levels */
          /* there's already been a trap message, reinforce it */
          abuse_dog(mtmp);
          adjalign(-3);
          break;
        case 2:
          /* it may have drowned or died.  that's no way to
           * treat a pet!  your god gets angry.
           */
          if (rn2(4)) {
            You_feel("guilty about losing your pet like this.");
            player.ugangr++;
            adjalign(-15);
          }

          /* you killed your pet by direct action.
           * minliquid and mintrap don't know to do this
           */
          player.uconduct.killer++;
          break;
        default:
          pline("that's strange, unknown mintrap result!");
          break;
      }
    }
  }

  reset_occupations();
  if (flags.run) {
    if (flags.run < 8)
      if (IS_DOOR(tmpr->typ) || IS_ROCK(tmpr->typ) || IS_FURNITURE(tmpr->typ))
        nomul(0, 0);
  }

  if (hides_under(youmonst.data))
    player.uundetected = OBJ_AT(player.ux, player.uy);
  else if (youmonst.data->mlet == S_EEL)
    player.uundetected =
        is_pool(player.ux, player.uy) && !Is_waterlevel(&player.uz);
  else if (player.dx || player.dy)
    player.uundetected = 0;

  /*
   * Mimics (or whatever) become noticeable if they move and are
   * imitating something that doesn't move.  We could extend this
   * to non-moving monsters...
   */
  if ((player.dx || player.dy) && (youmonst.m_ap_type == M_AP_OBJECT ||
                                   youmonst.m_ap_type == M_AP_FURNITURE))
    youmonst.m_ap_type = M_AP_NOTHING;

  check_leash(player.ux0, player.uy0);

  if (player.ux0 != player.ux || player.uy0 != player.uy) {
    player.umoved = TRUE;
    /* Clean old position -- vision_recalc() will print our new one. */
    newsym(player.ux0, player.uy0);
    /* Since the hero has moved, adjust what can be seen/unseen. */
    vision_recalc(1); /* Do the work now in the recover time. */
    invocation_message();
  }

  if (Punished) /* put back ball and chain */
    move_bc(0, bc_control, ballx, bally, chainx, chainy);

  spoteffects(TRUE);

  /* delay next move because of ball dragging */
  /* must come after we finished picking up, in spoteffects() */
  if (cause_delay) {
    nomul(-2, "dragging an iron ball");
    nomovemsg = "";
  }

  if (flags.run && iflags.runmode != RUN_TPORT) {
    /* display every step or every 7th step depending upon mode */
    if (iflags.runmode != RUN_LEAP || !(moves % 7L)) {
      if (flags.time)
        flags.botl = 1;
      curs_on_u();
      delay_output();
      if (iflags.runmode == RUN_CRAWL) {
        delay_output();
        delay_output();
        delay_output();
        delay_output();
      }
    }
  }
}

void invocation_message() {
  /* a special clue-msg when on the Invocation position */
  if (invocation_pos(player.ux, player.uy) &&
      !On_stairs(player.ux, player.uy)) {
    char buf[BUFSZ];
    Object *otmp = carrying(CANDELABRUM_OF_INVOCATION);

    nomul(0, 0); /* stop running or travelling */
#ifdef STEED
    if (player.usteed)
      sprintf(buf, "beneath %s", y_monnam(player.usteed));
    else
#endif
        if (Levitation || Flying)
      strcpy(buf, "beneath you");
    else
      sprintf(buf, "under your %s", makeplural(body_part(FOOT)));

    You_feel("a strange vibration %s.", buf);
    if (otmp && otmp->spe == 7 && otmp->lamplit)
      pline("%s %s!", The(xname(otmp)),
            Blind ? "throbs palpably" : "glows with a strange light");
  }
}

#endif /* OVL3 */
#ifdef OVL2

void spoteffects(bool pick) {
  Monster *mtmp;

  if (player.uinwater) {
    int was_underwater;

    if (!is_pool(player.ux, player.uy)) {
      if (Is_waterlevel(&player.uz))
        You("pop into an air bubble.");
      else if (is_lava(player.ux, player.uy))
        You("leave the water..."); /* oops! */
      else
        You("are on solid %s again.",
            is_ice(player.ux, player.uy) ? "ice" : "land");
    } else if (Is_waterlevel(&player.uz))
      goto stillinwater;
    else if (Levitation)
      You("pop out of the water like a cork!");
    else if (Flying)
      You("fly out of the water.");
    else if (Wwalking)
      You("slowly rise above the surface.");
    else
      goto stillinwater;
    was_underwater = Underwater && !Is_waterlevel(&player.uz);
    player.uinwater = 0;  /* leave the water */
    if (was_underwater) { /* restore vision */
      docrt();
      vision_full_recalc = 1;
    }
  }
stillinwater:
  ;
  if (!Levitation && !player.ustuck && !Flying) {
    /* limit recursive calls through teleds() */
    if (is_pool(player.ux, player.uy) || is_lava(player.ux, player.uy)) {
#ifdef STEED
      if (player.usteed && !is_flyer(player.usteed->data) &&
          !is_floater(player.usteed->data) &&
          !is_clinger(player.usteed->data)) {
        dismount_steed(Underwater ? DISMOUNT_FELL : DISMOUNT_GENERIC);
        /* dismount_steed() -> float_down() -> pickup() */
        if (!Is_airlevel(&player.uz) && !Is_waterlevel(&player.uz))
          pick = FALSE;
      } else
#endif
          if (is_lava(player.ux, player.uy)) {
        if (lava_effects())
          return;
      } else if (!Wwalking && drown())
        return;
    }
  }
  check_special_room(FALSE);
#ifdef SINKS
  if (IS_SINK(levl[player.ux][player.uy].typ) && Levitation)
    dosinkfall();
#endif
  if (!in_steed_dismounting) { /* if dismounting, we'll check again later */
    Trap *trap = t_at(player.ux, player.uy);
    bool pit;
    pit = (trap && (trap->ttyp == PIT || trap->ttyp == SPIKED_PIT));
    if (trap && pit)
      dotrap(trap, 0); /* fall into pit */
    if (pick)
      (void)pickup(1);
    if (trap && !pit)
      dotrap(trap, 0); /* fall into arrow trap, etc. */
  }
  if ((mtmp = m_at(player.ux, player.uy)) && !player.uswallow) {
    mtmp->mundetected = mtmp->msleeping = 0;
    switch (mtmp->data->mlet) {
      case S_PIERCER:
        pline("%s suddenly drops from the %s!", Amonnam(mtmp),
              ceiling(player.ux, player.uy));
        if (mtmp->mtame) /* jumps to greet you, not attack */
          ;
        else if (uarmh && is_metallic(uarmh))
          pline("Its blow glances off your helmet.");
        else if (player.uac + 3 <= rnd(20))
          You("are almost hit by %s!",
              x_monnam(mtmp, ARTICLE_A, "falling", 0, TRUE));
        else {
          int dmg;
          You("are hit by %s!", x_monnam(mtmp, ARTICLE_A, "falling", 0, TRUE));
          dmg = d(4, 6);
          if (Half_physical_damage)
            dmg = (dmg + 1) / 2;
          mdamageu(mtmp, dmg);
        }
        break;
      default: /* monster surprises you. */
        if (mtmp->mtame)
          pline("%s jumps near you from the %s.", Amonnam(mtmp),
                ceiling(player.ux, player.uy));
        else if (mtmp->mpeaceful) {
          You("surprise %s!",
              Blind && !sensemon(mtmp) ? something : a_monnam(mtmp));
          mtmp->mpeaceful = 0;
        } else
          pline("%s attacks you by surprise!", Amonnam(mtmp));
        break;
    }
    mnexto(mtmp); /* have to move the monster */
  }
}

STATIC_OVL bool monstinroom(MonsterType *mdat, int roomno) {
  Monster *mtmp;

  for (mtmp = fmon; mtmp; mtmp = mtmp->nmon)
    if (!mtmp->dead() && mtmp->data == mdat &&
        index(in_rooms(mtmp->mx, mtmp->my, 0), roomno + ROOMOFFSET))
      return (TRUE);
  return (FALSE);
}

char *in_rooms(xchar x, xchar y, int typewanted) {
  static char buf[5];
  char rno, *ptr = &buf[4];
  int typefound, min_x, min_y, max_x, max_y_offset, step;
  struct rm *lev;

#define goodtype(rno)                                             \
  (!typewanted ||                                                 \
   ((typefound = rooms[rno - ROOMOFFSET].rtype) == typewanted) || \
   ((typewanted == SHOPBASE) && (typefound > SHOPBASE)))

  switch (rno = levl[x][y].roomno) {
    case NO_ROOM:
      return (ptr);
    case SHARED:
      step = 2;
      break;
    case SHARED_PLUS:
      step = 1;
      break;
    default: /* i.e. a regular room # */
      if (goodtype(rno))
        *(--ptr) = rno;
      return (ptr);
  }

  min_x = x - 1;
  max_x = x + 1;
  if (x < 1)
    min_x += step;
  else if (x >= COLNO)
    max_x -= step;

  min_y = y - 1;
  max_y_offset = 2;
  if (min_y < 0) {
    min_y += step;
    max_y_offset -= step;
  } else if ((min_y + max_y_offset) >= ROWNO)
    max_y_offset -= step;

  for (x = min_x; x <= max_x; x += step) {
    lev = &levl[x][min_y];
    y = 0;
    if (((rno = lev[y].roomno) >= ROOMOFFSET) && !index(ptr, rno) &&
        goodtype(rno))
      *(--ptr) = rno;
    y += step;
    if (y > max_y_offset)
      continue;
    if (((rno = lev[y].roomno) >= ROOMOFFSET) && !index(ptr, rno) &&
        goodtype(rno))
      *(--ptr) = rno;
    y += step;
    if (y > max_y_offset)
      continue;
    if (((rno = lev[y].roomno) >= ROOMOFFSET) && !index(ptr, rno) &&
        goodtype(rno))
      *(--ptr) = rno;
  }
  return (ptr);
}

/* is (x,y) in a town? */
bool in_town(int x, int y) {
  s_level *slev = Is_special(&player.uz);
  struct mkroom *sroom;
  bool has_subrooms = FALSE;

  if (!slev || !slev->flags.town)
    return FALSE;

  /*
   * See if (x,y) is in a room with subrooms, if so, assume it's the
   * town.  If there are no subrooms, the whole level is in town.
   */
  for (sroom = &rooms[0]; sroom->hx > 0; sroom++) {
    if (!sroom->subrooms.empty()) {
      has_subrooms = TRUE;
      if (inside_room(sroom, x, y))
        return TRUE;
    }
  }

  return !has_subrooms;
}

STATIC_OVL void move_update(bool newlev) {
  char *ptr1, *ptr2, *ptr3, *ptr4;

  strcpy(player.urooms0, player.urooms);
  strcpy(player.ushops0, player.ushops);
  if (newlev) {
    player.urooms[0] = '\0';
    player.uentered[0] = '\0';
    player.ushops[0] = '\0';
    player.ushops_entered[0] = '\0';
    strcpy(player.ushops_left, player.ushops0);
    return;
  }
  strcpy(player.urooms, in_rooms(player.ux, player.uy, 0));

  for (ptr1 = &player.urooms[0], ptr2 = &player.uentered[0],
      ptr3 = &player.ushops[0], ptr4 = &player.ushops_entered[0];
       *ptr1; ptr1++) {
    if (!index(player.urooms0, *ptr1))
      *(ptr2++) = *ptr1;
    if (IS_SHOP(*ptr1 - ROOMOFFSET)) {
      *(ptr3++) = *ptr1;
      if (!index(player.ushops0, *ptr1))
        *(ptr4++) = *ptr1;
    }
  }
  *ptr2 = '\0';
  *ptr3 = '\0';
  *ptr4 = '\0';

  /* filter player.ushops0 -> player.ushops_left */
  for (ptr1 = &player.ushops0[0], ptr2 = &player.ushops_left[0]; *ptr1; ptr1++)
    if (!index(player.ushops, *ptr1))
      *(ptr2++) = *ptr1;
  *ptr2 = '\0';
}

void check_special_room(bool newlev) {
  Monster *mtmp;
  char *ptr;

  move_update(newlev);

  if (*player.ushops0)
    u_left_shop(player.ushops_left, newlev);

  if (!*player.uentered && !*player.ushops_entered) /* implied by newlev */
    return; /* no entrance messages necessary */

  /* Did we just enter a shop? */
  if (*player.ushops_entered)
    u_entered_shop(player.ushops_entered);

  for (ptr = &player.uentered[0]; *ptr; ptr++) {
    int roomno = *ptr - ROOMOFFSET, rt = rooms[roomno].rtype;

    /* Did we just enter some other special room? */
    /* vault.c insists that a vault remain a VAULT,
     * and temples should remain TEMPLEs,
     * but everything else gives a message only the first time */
    switch (rt) {
      case ZOO:
        pline("Welcome to David's treasure zoo!");
        break;
      case SWAMP:
        pline("It %s rather %s down here.", Blind ? "feels" : "looks",
              Blind ? "humid" : "muddy");
        break;
      case COURT:
        You("enter an opulent throne room!");
        break;
      case LEPREHALL:
        You("enter a leprechaun hall!");
        break;
      case MORGUE:
        if (midnight()) {
          const char *run = locomotion(youmonst.data, "Run");
          pline("%s away!  %s away!", run, run);
        } else
          You("have an uncanny feeling...");
        break;
      case BEEHIVE:
        You("enter a giant beehive!");
        break;
      case COCKNEST:
        You("enter a disgusting nest!");
        break;
      case ANTHOLE:
        You("enter an anthole!");
        break;
      case BARRACKS:
        if (monstinroom(&mons[PM_SOLDIER], roomno) ||
            monstinroom(&mons[PM_SERGEANT], roomno) ||
            monstinroom(&mons[PM_LIEUTENANT], roomno) ||
            monstinroom(&mons[PM_CAPTAIN], roomno))
          You("enter a military barracks!");
        else
          You("enter an abandoned barracks.");
        break;
      case DELPHI:
        if (monstinroom(&mons[PM_ORACLE], roomno))
          verbalize("%s, %s, welcome to Delphi!", Hello(nullptr), plname);
        break;
      case TEMPLE:
        intemple(roomno + ROOMOFFSET);
      /* fall through */
      default:
        rt = 0;
    }

    if (rt != 0) {
      rooms[roomno].rtype = OROOM;
      if (!search_special(rt)) {
        /* No more room of that type */
        switch (rt) {
          case COURT:
            level.flags.has_court = 0;
            break;
          case SWAMP:
            level.flags.has_swamp = 0;
            break;
          case MORGUE:
            level.flags.has_morgue = 0;
            break;
          case ZOO:
            level.flags.has_zoo = 0;
            break;
          case BARRACKS:
            level.flags.has_barracks = 0;
            break;
          case TEMPLE:
            level.flags.has_temple = 0;
            break;
          case BEEHIVE:
            level.flags.has_beehive = 0;
            break;
        }
      }
      if (rt == COURT || rt == SWAMP || rt == MORGUE || rt == ZOO)
        for (mtmp = fmon; mtmp; mtmp = mtmp->nmon)
          if (!mtmp->dead() && !Stealth && !rn2(3))
            mtmp->msleeping = 0;
    }
  }

  return;
}

#endif /* OVL2 */
#ifdef OVLB

int dopickup() {
  int count;
  Trap *traphere = t_at(player.ux, player.uy);
  /* awful kludge to work around parse()'s pre-decrement */
  count = (multi || (save_cm && *save_cm == ',')) ? multi + 1 : 0;
  multi = 0; /* always reset */
  /* uswallow case added by GAN 01/29/87 */
  if (player.uswallow) {
    if (!player.ustuck->minvent) {
      if (is_animal(player.ustuck->data)) {
        You("pick up %s tongue.", s_suffix(mon_nam(player.ustuck)));
        pline("But it's kind of slimy, so you drop it.");
      } else
        You("don't %s anything in here to pick up.", Blind ? "feel" : "see");
      return (1);
    } else {
      int tmpcount = -count;
      return loot_mon(player.ustuck, &tmpcount, nullptr);
    }
  }
  if (is_pool(player.ux, player.uy)) {
    if (Wwalking || is_floater(youmonst.data) || is_clinger(youmonst.data) ||
        (Flying && !Breathless)) {
      You("cannot dive into the water to pick things up.");
      return (0);
    } else if (!Underwater) {
      You_cant("even see the bottom, let alone pick up %s.", something);
      return (0);
    }
  }
  if (is_lava(player.ux, player.uy)) {
    if (Wwalking || is_floater(youmonst.data) || is_clinger(youmonst.data) ||
        (Flying && !Breathless)) {
      You_cant("reach the bottom to pick things up.");
      return (0);
    } else if (!likes_lava(youmonst.data)) {
      You("would burn to a crisp trying to pick things up.");
      return (0);
    }
  }
  if (!OBJ_AT(player.ux, player.uy)) {
    There("is nothing here to pick up.");
    return (0);
  }
  if (!can_reach_floor()) {
#ifdef STEED
    if (player.usteed && P_SKILL(P_RIDING) < P_BASIC)
      You("aren't skilled enough to reach from %s.", y_monnam(player.usteed));
    else
#endif
      You("cannot reach the %s.", surface(player.ux, player.uy));
    return (0);
  }

  if (traphere && traphere->tseen) {
    /* Allow pickup from holes and trap doors that you escaped from
     * because that stuff is teetering on the edge just like you, but
     * not pits, because there is an elevation discrepancy with stuff
     * in pits.
     */
    if ((traphere->ttyp == PIT || traphere->ttyp == SPIKED_PIT) &&
        (!player.utrap || (player.utrap && player.utraptype != TT_PIT))) {
      You("cannot reach the bottom of the pit.");
      return (0);
    }
  }

  return (pickup(-count));
}

#endif /* OVLB */
#ifdef OVL2

/* stop running if we see something interesting */
/* turn around a corner if that is the only way we can proceed */
/* do not turn left or right twice */
void lookaround() {
  int x, y, i, x0 = 0, y0 = 0, m0 = 1, i0 = 9;
  int corrct = 0, noturn = 0;
  Monster *mtmp;
  Trap *trap;

  /* Grid bugs stop if trying to move diagonal, even if blind.  Maybe */
  /* they polymorphed while in the middle of a long move. */
  if (player.umonnum == PM_GRID_BUG && player.dx && player.dy) {
    nomul(0, 0);
    return;
  }

  if (Blind || flags.run == 0)
    return;
  for (x = player.ux - 1; x <= player.ux + 1; x++)
    for (y = player.uy - 1; y <= player.uy + 1; y++) {
      if (!isok(x, y))
        continue;

      if (player.umonnum == PM_GRID_BUG && x != player.ux && y != player.uy)
        continue;

      if (x == player.ux && y == player.uy)
        continue;

      if ((mtmp = m_at(x, y)) && mtmp->m_ap_type != M_AP_FURNITURE &&
          mtmp->m_ap_type != M_AP_OBJECT && (!mtmp->minvis || See_invisible) &&
          !mtmp->mundetected) {
        if ((flags.run != 1 && !mtmp->mtame) ||
            (x == player.ux + player.dx && y == player.uy + player.dy))
          goto stop;
      }

      if (levl[x][y].typ == STONE)
        continue;
      if (x == player.ux - player.dx && y == player.uy - player.dy)
        continue;

      if (IS_ROCK(levl[x][y].typ) || (levl[x][y].typ == ROOM) ||
          IS_AIR(levl[x][y].typ))
        continue;
      else if (closed_door(x, y) ||
               (mtmp && mtmp->m_ap_type == M_AP_FURNITURE &&
                (mtmp->mappearance == S_hcdoor ||
                 mtmp->mappearance == S_vcdoor))) {
        if (x != player.ux && y != player.uy)
          continue;
        if (flags.run != 1)
          goto stop;
        goto bcorr;
      } else if (levl[x][y].typ == CORR) {
      bcorr:
        if (levl[player.ux][player.uy].typ != ROOM) {
          if (flags.run == 1 || flags.run == 3 || flags.run == 8) {
            i = dist2(x, y, player.ux + player.dx, player.uy + player.dy);
            if (i > 2)
              continue;
            if (corrct == 1 && dist2(x, y, x0, y0) != 1)
              noturn = 1;
            if (i < i0) {
              i0 = i;
              x0 = x;
              y0 = y;
              m0 = mtmp ? 1 : 0;
            }
          }
          corrct++;
        }
        continue;
      } else if ((trap = t_at(x, y)) && trap->tseen) {
        if (flags.run == 1)
          goto bcorr; /* if you must */
        if (x == player.ux + player.dx && y == player.uy + player.dy)
          goto stop;
        continue;
      } else if (is_pool(x, y) || is_lava(x, y)) {
        /* water and lava only stop you if directly in front, and stop
         * you even if you are running
         */
        if (!Levitation && !Flying && !is_clinger(youmonst.data) &&
            x == player.ux + player.dx && y == player.uy + player.dy)
          /* No Wwalking check; otherwise they'd be able
           * to test boots by trying to SHIFT-direction
           * into a pool and seeing if the game allowed it
           */
          goto stop;
        continue;
      } else { /* e.g. objects or trap or stairs */
        if (flags.run == 1)
          goto bcorr;
        if (flags.run == 8)
          continue;
        if (mtmp)
          continue; /* d */
        if (((x == player.ux - player.dx) && (y != player.uy + player.dy)) ||
            ((y == player.uy - player.dy) && (x != player.ux + player.dx)))
          continue;
      }
    stop:
      nomul(0, 0);
      return;
    } /* end for loops */

  if (corrct > 1 && flags.run == 2)
    goto stop;
  if ((flags.run == 1 || flags.run == 3 || flags.run == 8) && !noturn && !m0 &&
      i0 && (corrct == 1 || (corrct == 2 && i0 == 1))) {
    /* make sure that we do not turn too far */
    if (i0 == 2) {
      if (player.dx == y0 - player.uy && player.dy == player.ux - x0)
        i = 2; /* straight turn right */
      else
        i = -2; /* straight turn left */
    } else if (player.dx && player.dy) {
      if ((player.dx == player.dy && y0 == player.uy) ||
          (player.dx != player.dy && y0 != player.uy))
        i = -1; /* half turn left */
      else
        i = 1; /* half turn right */
    } else {
      if ((x0 - player.ux == y0 - player.uy && !player.dy) ||
          (x0 - player.ux != y0 - player.uy && player.dy))
        i = 1; /* half turn right */
      else
        i = -1; /* half turn left */
    }

    i += player.last_str_turn;
    if (i <= 2 && i >= -2) {
      player.last_str_turn = i;
      player.dx = x0 - player.ux;
      player.dy = y0 - player.uy;
    }
  }
}

/* something like lookaround, but we are not running */
/* react only to monsters that might hit us */
int monster_nearby() {
  int x, y;
  Monster *mtmp;

  /* Also see the similar check in dochugw() in monmove.c */
  for (x = player.ux - 1; x <= player.ux + 1; x++)
    for (y = player.uy - 1; y <= player.uy + 1; y++) {
      if (!isok(x, y))
        continue;
      if (x == player.ux && y == player.uy)
        continue;
      if ((mtmp = m_at(x, y)) && mtmp->m_ap_type != M_AP_FURNITURE &&
          mtmp->m_ap_type != M_AP_OBJECT &&
          (!mtmp->mpeaceful || Hallucination) &&
          (!is_hider(mtmp->data) || !mtmp->mundetected) &&
          !noattacks(mtmp->data) && mtmp->mcanmove &&
          !mtmp->msleeping && /* aplvax!jcn */
          !onscary(player.ux, player.uy, mtmp) && canspotmon(mtmp))
        return (1);
    }
  return (0);
}

void nomul(int nval, const char *txt) {
  if (multi < nval)
    return;                     /* This is a bug fix by ab@unido */
  player.uinvulnerable = FALSE; /* Kludge to avoid ctrl-C bug -dlc */
  player.usleep = 0;
  multi = nval;
  if (txt && txt[0])
    (void)strncpy(multi_txt, txt, BUFSZ);
  else
    (void)memset(multi_txt, 0, BUFSZ);
  flags.travel = iflags.travel1 = flags.mv = flags.run = 0;
}

/* called when a non-movement, multi-turn action has completed */
void unmul(const char *msg_override) {
  multi = 0; /* caller will usually have done this already */
  (void)memset(multi_txt, 0, BUFSZ);
  if (msg_override)
    nomovemsg = msg_override;
  else if (!nomovemsg)
    nomovemsg = You_can_move_again;
  if (*nomovemsg)
    pline(nomovemsg);
  nomovemsg = 0;
  player.usleep = 0;
  if (afternmv)
    (*afternmv)();
  afternmv = 0;
}

#endif /* OVL2 */
#ifdef OVL1

STATIC_OVL void maybe_wail() {
  static short powers[] = {TELEPORT,         SEE_INVIS, POISON_RES, COLD_RES,
                           SHOCK_RES,        FIRE_RES,  SLEEP_RES,  DISINT_RES,
                           TELEPORT_CONTROL, STEALTH,   FAST,       INVIS};

  if (moves <= wailmsg + 50)
    return;

  wailmsg = moves;
  if (Role_if(PM_WIZARD) || Race_if(PM_ELF) || Role_if(PM_VALKYRIE)) {
    const char *who;
    int i, powercnt;

    who = (Role_if(PM_WIZARD) || Role_if(PM_VALKYRIE)) ? urole.name.m : "Elf";
    if (player.uhp == 1) {
      pline("%s is about to die.", who);
    } else {
      for (i = 0, powercnt = 0; i < SIZE(powers); ++i)
        if (player.uprops[powers[i]].intrinsic & INTRINSIC)
          ++powercnt;

      pline(powercnt >= 4 ? "%s, all your powers will be lost..."
                          : "%s, your life force is running out.",
            who);
    }
  } else {
    You_hear(player.uhp == 1 ? "the wailing of the Banshee..."
                             : "the howling of the CwnAnnwn...");
  }
}

void losehp(int n, const char *knam, bool k_format) {
  if (Upolyd) {
    player.mh -= n;
    if (player.mhmax < player.mh)
      player.mhmax = player.mh;
    flags.botl = 1;
    if (player.mh < 1)
      rehumanize();
    else if (n > 0 && player.mh * 10 < player.mhmax && Unchanging)
      maybe_wail();
    return;
  }

  player.uhp -= n;
  if (player.uhp > player.uhpmax)
    player.uhpmax = player.uhp; /* perhaps n was negative */
  flags.botl = 1;
  if (player.uhp < 1) {
    killer_format = k_format;
    killer = knam; /* the thing that killed you */
    You("die...");
    done(DIED);
  } else if (n > 0 && player.uhp * 10 < player.uhpmax) {
    maybe_wail();
  }
}

int weight_cap() {
  long carrcap;

  carrcap = 25 * (ACURRSTR + ACURR(A_CON)) + 50;
  if (Upolyd) {
    /* consistent with can_carry() in mon.c */
    if (youmonst.data->mlet == S_NYMPH)
      carrcap = MAX_CARR_CAP;
    else if (!youmonst.data->cwt)
      carrcap = (carrcap * (long)youmonst.data->msize) / MZ_HUMAN;
    else if (!strongmonst(youmonst.data) ||
             (strongmonst(youmonst.data) && (youmonst.data->cwt > WT_HUMAN)))
      carrcap = (carrcap * (long)youmonst.data->cwt / WT_HUMAN);
  }

  if (Levitation || Is_airlevel(&player.uz) /* pugh@cornell */
#ifdef STEED
      || (player.usteed && strongmonst(player.usteed->data))
#endif
      )
    carrcap = MAX_CARR_CAP;
  else {
    if (carrcap > MAX_CARR_CAP)
      carrcap = MAX_CARR_CAP;
    if (!Flying) {
      if (EWounded_legs & LEFT_SIDE)
        carrcap -= 100;
      if (EWounded_legs & RIGHT_SIDE)
        carrcap -= 100;
    }
    if (carrcap < 0)
      carrcap = 0;
  }
  return ((int)carrcap);
}

static int wc; /* current weight_cap(); valid after call to inv_weight() */

/* returns how far beyond the normal capacity the player is currently. */
/* inv_weight() is negative if the player is below normal capacity. */
int inv_weight() {
  Object *otmp = invent;
  int wt = 0;

#ifndef GOLDOBJ
  /* when putting stuff into containers, gold is inserted at the head
     of invent for easier manipulation by askchain & co, but it's also
     retained in player.ugold in order to keep the status line accurate; we
     mustn't add its weight in twice under that circumstance */
  wt = (otmp && otmp->oclass == COIN_CLASS)
           ? 0
           : (int)((player.ugold + 50L) / 100L);
#endif
  while (otmp) {
#ifndef GOLDOBJ
    if (otmp->otyp != BOULDER || !throws_rocks(youmonst.data))
#else
    if (otmp->oclass == COIN_CLASS)
      wt += (int)(((long)otmp->quan + 50L) / 100L);
    else if (otmp->otyp != BOULDER || !throws_rocks(youmonst.data))
#endif
      wt += otmp->owt;
    otmp = otmp->nobj;
  }
  wc = weight_cap();
  return (wt - wc);
}

/*
 * Returns 0 if below normal capacity, or the number of "capacity units"
 * over the normal capacity the player is loaded.  Max is 5.
 */
int calc_capacity(int xtra_wt) {
  int cap, wt = inv_weight() + xtra_wt;

  if (wt <= 0)
    return UNENCUMBERED;
  if (wc <= 1)
    return OVERLOADED;
  cap = (wt * 2 / wc) + 1;
  return min(cap, OVERLOADED);
}

int near_capacity() { return calc_capacity(0); }

int max_capacity() {
  int wt = inv_weight();

  return (wt - (2 * wc));
}

bool check_capacity(const char *str) {
  if (near_capacity() >= EXT_ENCUMBER) {
    if (str)
      pline(str);
    else
      You_cant("do that while carrying so much stuff.");
    return 1;
  }
  return 0;
}

#endif /* OVL1 */
#ifdef OVLB

int inv_cnt() {
  Object *otmp = invent;
  int ct = 0;

  while (otmp) {
    ct++;
    otmp = otmp->nobj;
  }
  return (ct);
}

#ifdef GOLDOBJ
/* Counts the money in an object chain. */
/* Intended use is for your or some monsters inventory, */
/* now that player.gold/m.gold is gone.*/
/* Counting money in a container might be possible too. */
long money_cnt(Object *otmp) {
  while (otmp) {
    /* Must change when silver & copper is implemented: */
    if (otmp->oclass == COIN_CLASS)
      return otmp->quan;
    otmp = otmp->nobj;
  }
  return 0;
}
#endif
#endif /* OVLB */

/*hack.c*/
