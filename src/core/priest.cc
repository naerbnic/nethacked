/*	SCCS Id: @(#)priest.c	3.4	2002/11/06	*/
/* Copyright (c) Izchak Miller, Steve Linhart, 1989.		  */
/* NetHack may be freely redistributed.  See license for details. */

#include <string.h>

#include "core/hack.h"
#include "core/youprop.h"
#include "core/mkroom.h"
#include "core/zap.h"
#include "core/worn.h"
#include "core/worm.h"
#include "core/teleport.h"
#include "core/steed.h"
#include "core/steal.h"
#include "core/shk.h"
#include "core/rnd.h"
#include "core/priest.h"
#include "core/pray.h"
#include "core/potion.h"
#include "core/polyself.h"
#include "core/pline.h"
#include "core/objnam.h"
#include "core/mthrowu.h"
#include "core/mon.h"
#include "core/mkobj.h"
#include "core/minion.h"
#include "core/mhitu.h"
#include "core/makemon.h"
#include "core/hacklib.h"
#include "core/do_name.h"
#include "core/mfndpos.h"
#include "core/eshk.h"
#include "core/epri.h"
#include "core/emin.h"

/* this matches the categorizations shown by enlightenment */
#define ALGN_SINNED (-4) /* worse than strayed */


STATIC_DCL bool histemple_at(Monster *, xchar, xchar);
STATIC_DCL bool has_shrine(Monster *);

/*
 * Move for priests and shopkeepers.  Called from shk_move() and pri_move().
 * Valid returns are  1: moved  0: didn't  -1: let m_move do it  -2: died.
 */
int move_special(Monster *mtmp, bool in_his_shop, schar appr, bool uondoor,
                 bool avoid, xchar omx, xchar omy, xchar gx, xchar gy) {
  xchar nx, ny, nix, niy;
  schar i;
  schar chcnt, cnt;
  coord poss[9];
  long info[9];
  long allowflags;
  Object *ib = nullptr;

  if (omx == gx && omy == gy)
    return (0);
  if (mtmp->mconf) {
    avoid = FALSE;
    appr = 0;
  }

  nix = omx;
  niy = omy;
  if (mtmp->isshk)
    allowflags = ALLOW_SSM;
  else
    allowflags = ALLOW_SSM | ALLOW_SANCT;
  if (passes_walls(mtmp->data))
    allowflags |= (ALLOW_ROCK | ALLOW_WALL);
  if (throws_rocks(mtmp->data))
    allowflags |= ALLOW_ROCK;
  if (tunnels(mtmp->data))
    allowflags |= ALLOW_DIG;
  if (!nohands(mtmp->data) && !verysmall(mtmp->data)) {
    allowflags |= OPENDOOR;
    if (m_carrying(mtmp, SKELETON_KEY))
      allowflags |= BUSTDOOR;
  }
  if (is_giant(mtmp->data))
    allowflags |= BUSTDOOR;
  cnt = mfndpos(mtmp, poss, info, allowflags);

  if (mtmp->isshk && avoid && uondoor) { /* perhaps we cannot avoid him */
    for (i = 0; i < cnt; i++)
      if (!(info[i] & NOTONL))
        goto pick_move;
    avoid = FALSE;
  }

#define GDIST(x, y) (dist2(x, y, gx, gy))
pick_move:
  chcnt = 0;
  for (i = 0; i < cnt; i++) {
    nx = poss[i].x;
    ny = poss[i].y;
    if (levl[nx][ny].typ == ROOM ||
        (mtmp->ispriest && levl[nx][ny].typ == ALTAR) ||
        (mtmp->isshk && (!in_his_shop || ESHK(mtmp)->following))) {
      if (avoid && (info[i] & NOTONL))
        continue;
      if ((!appr && !rn2(++chcnt)) ||
          (appr && GDIST(nx, ny) < GDIST(nix, niy))) {
        nix = nx;
        niy = ny;
      }
    }
  }
  if (mtmp->ispriest && avoid && nix == omx && niy == omy &&
      onlineu(omx, omy)) {
    /* might as well move closer as long it's going to stay
     * lined up */
    avoid = FALSE;
    goto pick_move;
  }

  if (nix != omx || niy != omy) {
    remove_monster(omx, omy);
    place_monster(mtmp, nix, niy);
    newsym(nix, niy);
    if (mtmp->isshk && !in_his_shop && inhishop(mtmp))
      check_special_room(FALSE);
    if (ib) {
      if (cansee(mtmp->mx, mtmp->my))
        pline("%s picks up %s.", Monnam(mtmp), distant_name(ib, doname));
      RemoveObjectFromStorage(ib);
      (void)mpickobj(mtmp, ib);
    }
    return (1);
  }
  return (0);
}



char temple_occupied(char *array) {
  char *ptr;

  for (ptr = array; *ptr; ptr++)
    if (rooms[*ptr - ROOMOFFSET].rtype == TEMPLE)
      return (*ptr);
  return ('\0');
}


STATIC_OVL bool histemple_at(Monster *priest, xchar x, xchar y) {
  return ((bool)((EPRI(priest)->shroom == *in_rooms(x, y, TEMPLE)) &&
                 on_level(&(EPRI(priest)->shrlevel), &player.uz)));
}

/*
 * pri_move: return 1: moved  0: didn't  -1: let m_move do it  -2: died
 */
int pri_move(Monster *priest) {
  xchar gx, gy, omx, omy;
  schar temple;
  bool avoid = TRUE;

  omx = priest->mx;
  omy = priest->my;

  if (!histemple_at(priest, omx, omy))
    return (-1);

  temple = EPRI(priest)->shroom;

  gx = EPRI(priest)->shrpos.x;
  gy = EPRI(priest)->shrpos.y;

  gx += rn1(3, -1); /* mill around the altar */
  gy += rn1(3, -1);

  if (!priest->mpeaceful || (Conflict && !resist(priest, RING_CLASS, 0, 0))) {
    if (monnear(priest, player.ux, player.uy)) {
      if (Displaced)
        Your("displaced image doesn't fool %s!", mon_nam(priest));
      (void)mattacku(priest);
      return (0);
    } else if (index(player.urooms, temple)) {
      /* chase player if inside temple & can see him */
      if (priest->mcansee && m_canseeu(priest)) {
        gx = player.ux;
        gy = player.uy;
      }
      avoid = FALSE;
    }
  } else if (Invis)
    avoid = FALSE;

  return (move_special(priest, FALSE, TRUE, FALSE, avoid, omx, omy, gx, gy));
}

/* exclusively for mktemple() */
void priestini(d_level *lvl, struct mkroom *sroom, int sx, int sy,
               bool sanctum) {
  Monster *priest;
  Object *otmp;
  int cnt;

  if (MON_AT(sx + 1, sy))
    (void)rloc(m_at(sx + 1, sy), FALSE); /* insurance */

  priest = makemon(&mons[sanctum ? PM_HIGH_PRIEST : PM_ALIGNED_PRIEST], sx + 1,
                   sy, NO_MM_FLAGS);
  if (priest) {
    EPRI(priest)->shroom = (sroom - rooms) + ROOMOFFSET;
    EPRI(priest)->shralign = Amask2align(levl[sx][sy].altarmask);
    EPRI(priest)->shrpos.x = sx;
    EPRI(priest)->shrpos.y = sy;
    assign_level(&(EPRI(priest)->shrlevel), lvl);
    priest->mtrapseen = ~0; /* traps are known */
    priest->mpeaceful = 1;
    priest->ispriest = 1;
    priest->msleeping = 0;
    set_malign(priest); /* mpeaceful may have changed */

    /* now his/her goodies... */
    if (sanctum && EPRI(priest)->shralign == A_NONE &&
        on_level(&sanctum_level, &player.uz)) {
      (void)mongets(priest, AMULET_OF_YENDOR);
    }
    /* 2 to 4 spellbooks */
    for (cnt = rn1(3, 2); cnt > 0; --cnt) {
      (void)mpickobj(priest, MakeRandomObject(SPBOOK_CLASS, FALSE));
    }
    /* robe [via makemon()] */
    if (rn2(2) && (otmp = which_armor(priest, W_ARMC)) != 0) {
      if (p_coaligned(priest))
        Uncurse(otmp);
      else
        Curse(otmp);
    }
  }
}

/*
 * Specially aligned monsters are named specially.
 *	- aligned priests with ispriest and high priests have shrines
 *		they retain ispriest and epri when polymorphed
 *	- aligned priests without ispriest and Angels are roamers
 *		they retain isminion and access epri as emin when polymorphed
 *		(coaligned Angels are also created as minions, but they
 *		use the same naming convention)
 *	- minions do not have ispriest but have isminion and emin
 *	- caller needs to inhibit Hallucination if it wants to force
 *		the true name even when under that influence
 */
char *priestname(Monster *mon, char *pname) {
  const char *what = Hallucination ? rndmonnam() : mon->data->mname;

  strcpy(pname, "the ");
  if (mon->minvis)
    strcat(pname, "invisible ");
  if (mon->ispriest || mon->data == &mons[PM_ALIGNED_PRIEST] ||
      mon->data == &mons[PM_ANGEL]) {
    /* use epri */
    if (mon->mtame && mon->data == &mons[PM_ANGEL])
      strcat(pname, "guardian ");
    if (mon->data != &mons[PM_ALIGNED_PRIEST] &&
        mon->data != &mons[PM_HIGH_PRIEST]) {
      strcat(pname, what);
      strcat(pname, " ");
    }
    if (mon->data != &mons[PM_ANGEL]) {
      if (!mon->ispriest && EPRI(mon)->renegade)
        strcat(pname, "renegade ");
      if (mon->data == &mons[PM_HIGH_PRIEST])
        strcat(pname, "high ");
      if (Hallucination)
        strcat(pname, "poohbah ");
      else if (mon->female)
        strcat(pname, "priestess ");
      else
        strcat(pname, "priest ");
    }
    strcat(pname, "of ");
    strcat(pname, halu_gname((int)EPRI(mon)->shralign));
    return (pname);
  }
  /* use emin instead of epri */
  strcat(pname, what);
  strcat(pname, " of ");
  strcat(pname, halu_gname(EMIN(mon)->min_align));
  return (pname);
}

bool p_coaligned(Monster *priest) {
  return ((bool)(player.ualign.type == ((int)EPRI(priest)->shralign)));
}

STATIC_OVL bool has_shrine(Monster *pri) {
  struct rm *lev;

  if (!pri)
    return (FALSE);
  lev = &levl[EPRI(pri)->shrpos.x][EPRI(pri)->shrpos.y];
  if (!IS_ALTAR(lev->typ) || !(lev->altarmask & AM_SHRINE))
    return (FALSE);
  return (
      (bool)(EPRI(pri)->shralign == Amask2align(lev->altarmask & ~AM_SHRINE)));
}

Monster *findpriest(char roomno) {
  Monster *mtmp;

  for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
    if (mtmp->dead())
      continue;
    if (mtmp->ispriest && (EPRI(mtmp)->shroom == roomno) &&
        histemple_at(mtmp, mtmp->mx, mtmp->my))
      return (mtmp);
  }
  return nullptr;
}

/* called from check_special_room() when the player enters the temple room */
void intemple(int roomno) {
  Monster *priest = findpriest((char)roomno);
  bool tended = (priest != nullptr);
  bool shrined, sanctum, can_speak;
  const char *msg1, *msg2;
  char buf[BUFSZ];

  if (!temple_occupied(player.urooms0)) {
    if (tended) {
      shrined = has_shrine(priest);
      sanctum = (priest->data == &mons[PM_HIGH_PRIEST] &&
                 (Is_sanctum(&player.uz) || In_endgame(&player.uz)));
      can_speak = (priest->mcanmove && !priest->msleeping && flags.soundok);
      if (can_speak) {
        unsigned save_priest = priest->ispriest;
        /* don't reveal the altar's owner upon temple entry in
           the endgame; for the Sanctum, the next message names
           Moloch so suppress the "of Moloch" for him here too */
        if (sanctum && !Hallucination)
          priest->ispriest = 0;
        pline("%s intones:",
              canseemon(priest) ? Monnam(priest) : "A nearby voice");
        priest->ispriest = save_priest;
      }
      msg2 = 0;
      if (sanctum && Is_sanctum(&player.uz)) {
        if (priest->mpeaceful) {
          msg1 = "Infidel, you have entered Moloch's Sanctum!";
          msg2 = "Be gone!";
          priest->mpeaceful = 0;
          set_malign(priest);
        } else
          msg1 = "You desecrate this place by your presence!";
      } else {
        sprintf(buf, "Pilgrim, you enter a %s place!",
                !shrined ? "desecrated" : "sacred");
        msg1 = buf;
      }
      if (can_speak) {
        verbalize(msg1);
        if (msg2)
          verbalize(msg2);
      }
      if (!sanctum) {
        /* !tended -> !shrined */
        if (!shrined || !p_coaligned(priest) ||
            player.ualign.record <= ALGN_SINNED)
          You("have a%s forbidding feeling...", (!shrined) ? "" : " strange");
        else
          You("experience a strange sense of peace.");
      }
    } else {
      switch (rn2(3)) {
        case 0:
          You("have an eerie feeling...");
          break;
        case 1:
          You_feel("like you are being watched.");
          break;
        default:
          pline("A shiver runs down your %s.", body_part(SPINE));
          break;
      }
      if (!rn2(5)) {
        Monster *mtmp;

        if (!(mtmp =
                  makemon(&mons[PM_GHOST], player.ux, player.uy, NO_MM_FLAGS)))
          return;
        if (!Blind || sensemon(mtmp))
          pline("An enormous ghost appears next to you!");
        else
          You("sense a presence close by!");
        mtmp->mpeaceful = 0;
        set_malign(mtmp);
        if (flags.verbose)
          You("are frightened to death, and unable to move.");
        nomul(-3, "frightened to death");
        nomovemsg = "You regain your composure.";
      }
    }
  }
}

void priest_talk(Monster *priest) {
  bool coaligned = p_coaligned(priest);
  bool strayed = (player.ualign.record < 0);

  /* KMH, conduct */
  player.uconduct.gnostic++;

  if (priest->mflee || (!priest->ispriest && coaligned && strayed)) {
    pline("%s doesn't want anything to do with you!", Monnam(priest));
    priest->mpeaceful = 0;
    return;
  }

  /* priests don't chat unless peaceful and in their own temple */
  if (!histemple_at(priest, priest->mx, priest->my) || !priest->mpeaceful ||
      !priest->mcanmove || priest->msleeping) {
    static const char *cranky_msg[3] = {
        "Thou wouldst have words, eh?  I'll give thee a word or two!",
        "Talk?  Here is what I have to say!",
        "Pilgrim, I would speak no longer with thee."};

    if (!priest->mcanmove || priest->msleeping) {
      pline("%s breaks out of %s reverie!", Monnam(priest), mhis(priest));
      priest->mfrozen = priest->msleeping = 0;
      priest->mcanmove = 1;
    }
    priest->mpeaceful = 0;
    verbalize(cranky_msg[rn2(3)]);
    return;
  }

  /* you desecrated the temple and now you want to chat? */
  if (priest->mpeaceful && *in_rooms(priest->mx, priest->my, TEMPLE) &&
      !has_shrine(priest)) {
    verbalize("Begone!  Thou desecratest this holy place with thy presence.");
    priest->mpeaceful = 0;
    return;
  }
#ifndef GOLDOBJ
  if (!player.ugold) {
    if (coaligned && !strayed) {
      if (priest->mgold > 0L) {
        /* Note: two bits is actually 25 cents.  Hmm. */
        pline("%s gives you %s for an ale.", Monnam(priest),
              (priest->mgold == 1L) ? "one bit" : "two bits");
        if (priest->mgold > 1L)
          player.ugold = 2L;
        else
          player.ugold = 1L;
        priest->mgold -= player.ugold;
        flags.botl = 1;
#else
  if (!money_cnt(invent)) {
    if (coaligned && !strayed) {
      long pmoney = money_cnt(priest->minvent);
      if (pmoney > 0L) {
        /* Note: two bits is actually 25 cents.  Hmm. */
        pline("%s gives you %s for an ale.", Monnam(priest),
              (pmoney == 1L) ? "one bit" : "two bits");
        money2u(priest, pmoney > 1L ? 2 : 1);
#endif
      } else
        pline("%s preaches the virtues of poverty.", Monnam(priest));
      exercise(A_WIS, TRUE);
    } else
      pline("%s is not interested.", Monnam(priest));
    return;
  } else {
    long offer;

    pline("%s asks you for a contribution for the temple.", Monnam(priest));
    if ((offer = bribe(priest)) == 0) {
      verbalize("Thou shalt regret thine action!");
      if (coaligned)
        adjalign(-1);
    } else if (offer < (player.ulevel * 200)) {
#ifndef GOLDOBJ
      if (player.ugold > (offer * 2L))
        verbalize("Cheapskate.");
#else
      if (money_cnt(invent) > (offer * 2L))
        verbalize("Cheapskate.");
#endif
      else {
        verbalize("I thank thee for thy contribution.");
        /*  give player some token  */
        exercise(A_WIS, TRUE);
      }
    } else if (offer < (player.ulevel * 400)) {
      verbalize("Thou art indeed a pious individual.");
#ifndef GOLDOBJ
      if (player.ugold < (offer * 2L)) {
#else
      if (money_cnt(invent) < (offer * 2L)) {
#endif
        if (coaligned && player.ualign.record <= ALGN_SINNED)
          adjalign(1);
        verbalize("I bestow upon thee a blessing.");
        incr_itimeout(&HClairvoyant, rn1(500, 500));
      }
    } else if (offer < (player.ulevel * 600) && player.ublessed < 20 &&
               (player.ublessed < 9 || !rn2(player.ublessed))) {
      verbalize("Thy devotion has been rewarded.");
      if (!(HProtection & INTRINSIC)) {
        HProtection |= FROMOUTSIDE;
        if (!player.ublessed)
          player.ublessed = rn1(3, 2);
      } else
        player.ublessed++;
    } else {
      verbalize("Thy selfless generosity is deeply appreciated.");
#ifndef GOLDOBJ
      if (player.ugold < (offer * 2L) && coaligned) {
#else
      if (money_cnt(invent) < (offer * 2L) && coaligned) {
#endif
        if (strayed && (moves - player.ucleansed) > 5000L) {
          player.ualign.record = 0; /* cleanse thee */
          player.ucleansed = moves;
        } else {
          adjalign(2);
        }
      }
    }
  }
}

Monster *mk_roamer(MonsterType *ptr, aligntyp alignment, xchar x, xchar y,
                   bool peaceful) {
  Monster *roamer;
  bool coaligned = (player.ualign.type == alignment);

  if (ptr != &mons[PM_ALIGNED_PRIEST] && ptr != &mons[PM_ANGEL])
    return (nullptr);

  if (MON_AT(x, y))
    (void)rloc(m_at(x, y), FALSE); /* insurance */

  if (!(roamer = makemon(ptr, x, y, NO_MM_FLAGS)))
    return (nullptr);

  EPRI(roamer)->shralign = alignment;
  if (coaligned && !peaceful)
    EPRI(roamer)->renegade = TRUE;
  /* roamer->ispriest == FALSE naturally */
  roamer->isminion = TRUE; /* borrowing this bit */
  roamer->mtrapseen = ~0;  /* traps are known */
  roamer->mpeaceful = peaceful;
  roamer->msleeping = 0;
  set_malign(roamer); /* peaceful may have changed */

  /* MORE TO COME */
  return (roamer);
}

void reset_hostility(Monster *roamer) {
  if (!(roamer->isminion && (roamer->data == &mons[PM_ALIGNED_PRIEST] ||
                             roamer->data == &mons[PM_ANGEL])))
    return;

  if (EPRI(roamer)->shralign != player.ualign.type) {
    roamer->mpeaceful = roamer->mtame = 0;
    set_malign(roamer);
  }
  newsym(roamer->mx, roamer->my);
}

bool in_your_sanctuary(Monster *mon, xchar x, xchar y) {
  char roomno;
  Monster *priest;

  if (mon) {
    if (is_minion(mon->data) || is_rider(mon->data))
      return FALSE;
    x = mon->mx, y = mon->my;
  }
  if (player.ualign.record <= ALGN_SINNED) /* sinned or worse */
    return FALSE;
  if ((roomno = temple_occupied(player.urooms)) == 0 ||
      roomno != *in_rooms(x, y, TEMPLE))
    return FALSE;
  if ((priest = findpriest(roomno)) == 0)
    return FALSE;
  return (bool)(has_shrine(priest) && p_coaligned(priest) && priest->mpeaceful);
}

/* when attacking "priest" in his temple */
void ghod_hitsu(Monster *priest) {
  int x, y, ax, ay, roomno = (int)temple_occupied(player.urooms);
  struct mkroom *troom;

  if (!roomno || !has_shrine(priest))
    return;

  ax = x = EPRI(priest)->shrpos.x;
  ay = y = EPRI(priest)->shrpos.y;
  troom = &rooms[roomno - ROOMOFFSET];

  if ((player.ux == x && player.uy == y) ||
      !linedup(player.ux, player.uy, x, y)) {
    if (IS_DOOR(levl[player.ux][player.uy].typ)) {
      if (player.ux == troom->lx - 1) {
        x = troom->hx;
        y = player.uy;
      } else if (player.ux == troom->hx + 1) {
        x = troom->lx;
        y = player.uy;
      } else if (player.uy == troom->ly - 1) {
        x = player.ux;
        y = troom->hy;
      } else if (player.uy == troom->hy + 1) {
        x = player.ux;
        y = troom->ly;
      }
    } else {
      switch (rn2(4)) {
        case 0:
          x = player.ux;
          y = troom->ly;
          break;
        case 1:
          x = player.ux;
          y = troom->hy;
          break;
        case 2:
          x = troom->lx;
          y = player.uy;
          break;
        default:
          x = troom->hx;
          y = player.uy;
          break;
      }
    }
    if (!linedup(player.ux, player.uy, x, y))
      return;
  }

  switch (rn2(3)) {
    case 0:
      pline("%s roars in anger:  \"Thou shalt suffer!\"", a_gname_at(ax, ay));
      break;
    case 1:
      pline("%s voice booms:  \"How darest thou harm my servant!\"",
            s_suffix(a_gname_at(ax, ay)));
      break;
    default:
      pline("%s roars:  \"Thou dost profane my shrine!\"", a_gname_at(ax, ay));
      break;
  }

  buzz(-10 - (AD_ELEC - 1), 6, x, y, sgn(tbx),
       sgn(tby)); /* bolt of lightning */
  exercise(A_WIS, FALSE);
}

void angry_priest() {
  Monster *priest;
  struct rm *lev;

  if ((priest = findpriest(temple_occupied(player.urooms))) != 0) {
    wakeup(priest);
    /*
     * If the altar has been destroyed or converted, let the
     * priest run loose.
     * (When it's just a conversion and there happens to be
     *	a fresh corpse nearby, the priest ought to have an
     *	opportunity to try converting it back; maybe someday...)
     */
    lev = &levl[EPRI(priest)->shrpos.x][EPRI(priest)->shrpos.y];
    if (!IS_ALTAR(lev->typ) ||
        ((aligntyp)Amask2align(lev->altarmask & AM_MASK) !=
         EPRI(priest)->shralign)) {
      priest->ispriest = 0; /* now a roamer */
      priest->isminion = 1; /* but still aligned */
      /* this overloads the `shroom' field, which is now clobbered */
      EPRI(priest)->renegade = 0;
    }
  }
}

/*
 * When saving bones, find priests that aren't on their shrine level,
 * and remove them.   This avoids big problems when restoring bones.
 */
void clearpriests() {
  Monster *mtmp, *mtmp2;

  for (mtmp = fmon; mtmp; mtmp = mtmp2) {
    mtmp2 = mtmp->nmon;
    if (!mtmp->dead() && mtmp->ispriest &&
        !on_level(&(EPRI(mtmp)->shrlevel), &player.uz))
      mongone(mtmp);
  }
}

/* munge priest-specific structure when restoring -dlc */
void restpriest(Monster *mtmp, bool ghostly) {
  if (player.uz.dlevel) {
    if (ghostly)
      assign_level(&(EPRI(mtmp)->shrlevel), &player.uz);
  }
}


/*priest.c*/
