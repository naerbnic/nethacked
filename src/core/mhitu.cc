/*	SCCS Id: @(#)mhitu.c	3.4	2003/11/26	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "core/mhitu.h"

#include <string.h>

#include "core/hack.h"
#include "core/decl.h"
#include "core/youprop.h"
#include "core/zap.h"
#include "core/worn.h"
#include "core/worm.h"
#include "core/wield.h"
#include "core/were.h"
#include "core/weapon.h"
#include "core/teleport.h"
#include "core/steed.h"
#include "core/steal.h"
#include "core/sounds.h"
#include "core/sit.h"
#include "core/rnd.h"
#include "core/read.h"
#include "core/questpgr.h"
#include "core/potion.h"
#include "core/polyself.h"
#include "core/pline.h"
#include "core/pickup.h"
#include "core/objnam.h"
#include "core/o_init.h"
#include "core/muse.h"
#include "core/mthrowu.h"
#include "core/monmove.h"
#include "core/mon.h"
#include "core/minion.h"
#include "core/mhitm.h"
#include "core/mcastu.h"
#include "core/makemon.h"
#include "core/invent.h"
#include "core/hacklib.h"
#include "core/exper.h"
#include "core/end.h"
#include "core/eat.h"
#include "core/dog.h"
#include "core/do_wear.h"
#include "core/do_name.h"
#include "core/do.h"
#include "core/dbridge.h"
#include "core/cmd.h"

#include "core/allmain.h"
#include "core/apply.h"
#include "core/artifact.h"
#include "core/ball.h"
#include "core/botl.h"

STATIC_VAR Object *otmp;

STATIC_DCL void urustm(Monster *, Object *);
STATIC_DCL bool u_slip_free(Monster *, struct Attack *);
STATIC_DCL int passiveum(MonsterType *, Monster *, struct Attack *);

#ifdef SEDUCE
STATIC_DCL void mayberem(Object *, const char *);
#endif

STATIC_DCL bool diseasemu(MonsterType *);
STATIC_DCL int hitmu(Monster *, struct Attack *);
STATIC_DCL int gulpmu(Monster *, struct Attack *);
STATIC_DCL int explmu(Monster *, struct Attack *, bool);
STATIC_DCL void missmu(Monster *, bool, struct Attack *);
STATIC_DCL void mswings(Monster *, Object *);
STATIC_DCL void wildmiss(Monster *, struct Attack *);

STATIC_DCL void hurtarmor(int);
STATIC_DCL void hitmsg(Monster *, struct Attack *);

/* See comment in mhitm.c.  If we use this a lot it probably should be */
/* changed to a parameter to mhitu. */
static int dieroll;


STATIC_OVL void hitmsg(Monster *mtmp, struct Attack *mattk) {
  int compat;

  /* Note: if opposite gender, "seductively" */
  /* If same gender, "engagingly" for nymph, normal msg for others */
  if ((compat = could_seduce(mtmp, &youmonst, mattk)) && !mtmp->mcan &&
      !mtmp->mspec_used) {
    pline("%s %s you %s.", Monnam(mtmp), Blind ? "talks to" : "smiles at",
          compat == 2 ? "engagingly" : "seductively");
  } else
    switch (mattk->aatyp) {
      case AT_BITE:
        pline("%s bites!", Monnam(mtmp));
        break;
      case AT_KICK:
        pline("%s kicks%c", Monnam(mtmp),
              thick_skinned(youmonst.data) ? '.' : '!');
        break;
      case AT_STNG:
        pline("%s stings!", Monnam(mtmp));
        break;
      case AT_BUTT:
        pline("%s butts!", Monnam(mtmp));
        break;
      case AT_TUCH:
        pline("%s touches you!", Monnam(mtmp));
        break;
      case AT_TENT:
        pline("%s tentacles suck you!", s_suffix(Monnam(mtmp)));
        break;
      case AT_EXPL:
      case AT_BOOM:
        pline("%s explodes!", Monnam(mtmp));
        break;
      default:
        pline("%s hits!", Monnam(mtmp));
    }
}

/* monster missed you */
STATIC_OVL void missmu(Monster *mtmp, bool nearmiss, struct Attack *mattk) {
  if (!canspotmon(mtmp))
    map_invisible(mtmp->mx, mtmp->my);

  if (could_seduce(mtmp, &youmonst, mattk) && !mtmp->mcan)
    pline("%s pretends to be friendly.", Monnam(mtmp));
  else {
    if (!flags.verbose || !nearmiss)
      pline("%s misses.", Monnam(mtmp));
    else
      pline("%s just misses!", Monnam(mtmp));
  }
  stop_occupation();
}

/* monster swings obj */
STATIC_OVL void mswings(Monster *mtmp, Object *otemp) {
  if (!flags.verbose || Blind || !mon_visible(mtmp))
    return;
  pline("%s %s %s %s.", Monnam(mtmp),
        (objects[otemp->otyp].oc_dir & PIERCE) ? "thrusts" : "swings",
        mhis(mtmp), singular(otemp, xname));
}

/* return how a poison attack was delivered */
const char *mpoisons_subj(Monster *mtmp, struct Attack *mattk) {
  if (mattk->aatyp == AT_WEAP) {
    Object *mwep = (mtmp == &youmonst) ? uwep : mtmp->weapon();
    /* "Foo's attack was poisoned." is pretty lame, but at least
       it's better than "sting" when not a stinging attack... */
    return (!mwep || !mwep->opoisoned) ? "attack" : "weapon";
  } else {
    return (mattk->aatyp == AT_TUCH)
               ? "contact"
               : (mattk->aatyp == AT_GAZE) ? "gaze" : (mattk->aatyp == AT_BITE)
                                                          ? "bite"
                                                          : "sting";
  }
}

/* called when your intrinsic speed is taken away */
void u_slow_down() {
  HFast = 0L;
  if (!Fast)
    You("slow down.");
  else /* speed boots */
    Your("quickness feels less natural.");
  exercise(A_DEX, FALSE);
}


/* monster attacked your displaced image */
STATIC_OVL void wildmiss(Monster *mtmp, struct Attack *mattk) {
  int compat;

  /* no map_invisible() -- no way to tell where _this_ is coming from */

  if (!flags.verbose)
    return;
  if (!cansee(mtmp->mx, mtmp->my))
    return;
  /* maybe it's attacking an image around the corner? */

  compat = (mattk->adtyp == AD_SEDU || mattk->adtyp == AD_SSEX) &&
           could_seduce(mtmp, &youmonst, (struct Attack *)0);

  if (!mtmp->mcansee || (Invis && !perceives(mtmp->data))) {
    const char *swings =
        mattk->aatyp == AT_BITE ? "snaps" : mattk->aatyp == AT_KICK
                                                ? "kicks"
                                                : (mattk->aatyp == AT_STNG ||
                                                   mattk->aatyp == AT_BUTT ||
                                                   nolimbs(mtmp->data))
                                                      ? "lunges"
                                                      : "swings";

    if (compat)
      pline("%s tries to touch you and misses!", Monnam(mtmp));
    else
      switch (rn2(3)) {
        case 0:
          pline("%s %s wildly and misses!", Monnam(mtmp), swings);
          break;
        case 1:
          pline("%s attacks a spot beside you.", Monnam(mtmp));
          break;
        case 2:
          pline("%s strikes at %s!", Monnam(mtmp),
                levl[mtmp->mux][mtmp->muy].typ == WATER ? "empty water"
                                                        : "thin air");
          break;
        default:
          pline("%s %s wildly!", Monnam(mtmp), swings);
          break;
      }

  } else if (Displaced) {
    if (compat)
      pline("%s smiles %s at your %sdisplaced image...", Monnam(mtmp),
            compat == 2 ? "engagingly" : "seductively",
            Invis ? "invisible " : "");
    else
      pline("%s strikes at your %sdisplaced image and misses you!",
            /* Note: if you're both invisible and displaced,
             * only monsters which see invisible will attack your
             * displaced image, since the displaced image is also
             * invisible.
             */
            Monnam(mtmp), Invis ? "invisible " : "");

  } else if (Underwater) {
    /* monsters may miss especially on water level where
       bubbles shake the player here and there */
    if (compat)
      pline("%s reaches towards your distorted image.", Monnam(mtmp));
    else
      pline("%s is fooled by water reflections and misses!", Monnam(mtmp));

  } else
    impossible("%s attacks you without knowing your location?", Monnam(mtmp));
}

void expels(Monster *mtmp, MonsterType *mdat, bool message) {
  if (message) {
    if (is_animal(mdat))
      You("get regurgitated!");
    else {
      char blast[40];
      int i;

      blast[0] = '\0';
      for (i = 0; i < NATTK; i++)
        if (mdat->mattk[i].aatyp == AT_ENGL)
          break;
      if (mdat->mattk[i].aatyp != AT_ENGL)
        impossible("Swallower has no engulfing attack?");
      else {
        if (is_whirly(mdat)) {
          switch (mdat->mattk[i].adtyp) {
            case AD_ELEC:
              strcpy(blast, " in a shower of sparks");
              break;
            case AD_COLD:
              strcpy(blast, " in a blast of frost");
              break;
          }
        } else
          strcpy(blast, " with a squelch");
        You("get expelled from %s%s!", mon_nam(mtmp), blast);
      }
    }
  }
  unstuck(mtmp); /* ball&chain returned in unstuck() */
  mnexto(mtmp);
  newsym(player.ux, player.uy);
  spoteffects(TRUE);
  /* to cover for a case where mtmp is not in a next square */
  if (um_dist(mtmp->mx, mtmp->my, 1))
    pline("Brrooaa...  You land hard at some distance.");
}


/* select a monster's next attack, possibly substituting for its usual one */
struct Attack *getmattk(MonsterType *mptr, int indx, int prev_result[],
                        struct Attack *alt_attk_buf) {
  struct Attack *attk = &mptr->mattk[indx];

  /* prevent a monster with two consecutive disease or hunger attacks
     from hitting with both of them on the same turn; if the first has
     already hit, switch to a stun attack for the second */
  if (indx > 0 && prev_result[indx - 1] > 0 &&
      (attk->adtyp == AD_DISE || attk->adtyp == AD_PEST ||
       attk->adtyp == AD_FAMN) &&
      attk->adtyp == mptr->mattk[indx - 1].adtyp) {
    *alt_attk_buf = *attk;
    attk = alt_attk_buf;
    attk->adtyp = AD_STUN;
  }
  return attk;
}

/*
 * mattacku: monster attacks you
 *	returns 1 if monster dies (e.g. "yellow light"), 0 otherwise
 *	Note: if you're displaced or invisible the monster might attack the
 *		wrong position...
 *	Assumption: it's attacking you or an empty square; if there's another
 *		monster which it attacks by mistake, the caller had better
 *		take care of it...
 */
int mattacku(Monster *mtmp) {
  struct Attack *mattk, alt_attk;
  int i, j, tmp, sum[NATTK];
  MonsterType *mdat = mtmp->data;
  bool ranged = (distu(mtmp->mx, mtmp->my) > 3);
  /* Is it near you?  Affects your actions */
  bool range2 = !monnear(mtmp, mtmp->mux, mtmp->muy);
  /* Does it think it's near you?  Affects its actions */
  bool foundyou = (mtmp->mux == player.ux && mtmp->muy == player.uy);
  /* Is it attacking you or your image? */
  bool youseeit = canseemon(mtmp);
  /* Might be attacking your image around the corner, or
   * invisible, or you might be blind....
   */

  if (!ranged)
    nomul(0, 0);
  if (mtmp->mhp <= 0 || (Underwater && !is_swimmer(mtmp->data)))
    return (0);

  /* If swallowed, can only be affected by player.ustuck */
  if (player.uswallow) {
    if (mtmp != player.ustuck)
      return (0);
    player.ustuck->mux = player.ux;
    player.ustuck->muy = player.uy;
    range2 = 0;
    foundyou = 1;
    if (player.uinvulnerable)
      return (0); /* stomachs can't hurt you! */
  }
#ifdef STEED
  else if (player.usteed) {
    if (mtmp == player.usteed) /* Your steed won't attack you */
      return (0);
    /* Orcs like to steal and eat horses and the like */
    if (!rn2(is_orc(mtmp->data) ? 2 : 4) && distu(mtmp->mx, mtmp->my) <= 2) {
      /* Attack your steed instead */
      i = mattackm(mtmp, player.usteed);
      if ((i & MM_AGR_DIED))
        return (1);
      if (i & MM_DEF_DIED || player.umoved)
        return (0);
      /* Let your steed retaliate */
      return (!!(mattackm(player.usteed, mtmp) & MM_DEF_DIED));
    }
  }
#endif

  if (player.uundetected && !range2 && foundyou && !player.uswallow) {
    player.uundetected = 0;
    if (is_hider(youmonst.data)) {
      coord cc; /* maybe we need a unexto() function? */
      Object *obj;

      You("fall from the %s!", ceiling(player.ux, player.uy));
      if (enexto(&cc, player.ux, player.uy, youmonst.data)) {
        remove_monster(mtmp->mx, mtmp->my);
        newsym(mtmp->mx, mtmp->my);
        place_monster(mtmp, player.ux, player.uy);
        if (mtmp->wormno)
          worm_move(mtmp);
        teleds(cc.x, cc.y, TRUE);
        set_apparxy(mtmp);
        newsym(player.ux, player.uy);
      } else {
        pline("%s is killed by a falling %s (you)!", Monnam(mtmp),
              youmonst.data->mname);
        killed(mtmp);
        newsym(player.ux, player.uy);
        if (mtmp->mhp > 0)
          return 0;
        else
          return 1;
      }
      if (youmonst.data->mlet != S_PIERCER)
        return (0); /* trappers don't attack */

      obj = which_armor(mtmp, WORN_HELMET);
      if (obj && is_metallic(obj)) {
        Your("blow glances off %s helmet.", s_suffix(mon_nam(mtmp)));
      } else {
        if (3 + find_mac(mtmp) <= rnd(20)) {
          pline("%s is hit by a falling piercer (you)!", Monnam(mtmp));
          if ((mtmp->mhp -= d(3, 6)) < 1)
            killed(mtmp);
        } else
          pline("%s is almost hit by a falling piercer (you)!", Monnam(mtmp));
      }
    } else {
      if (!youseeit)
        pline("It tries to move where you are hiding.");
      else {
        /* Ugly kludge for eggs.  The message is phrased so as
         * to be directed at the monster, not the player,
         * which makes "laid by you" wrong.  For the
         * parallelism to work, we can't rephrase it, so we
         * zap the "laid by you" momentarily instead.
         */
        Object *obj = level.objects[player.ux][player.uy];

        if (obj ||
            (youmonst.data->mlet == S_EEL && is_pool(player.ux, player.uy))) {
          int save_spe = 0; /* suppress warning */
          if (obj) {
            save_spe = obj->spe;
            if (obj->otyp == EGG)
              obj->spe = 0;
          }
          if (youmonst.data->mlet == S_EEL)
            pline("Wait, %s!  There's a hidden %s named %s there!",
                  m_monnam(mtmp), youmonst.data->mname, plname);
          else
            pline("Wait, %s!  There's a %s named %s hiding under %s!",
                  m_monnam(mtmp), youmonst.data->mname, plname,
                  doname(level.objects[player.ux][player.uy]));
          if (obj)
            obj->spe = save_spe;
        } else
          impossible("hiding under nothing?");
      }
      newsym(player.ux, player.uy);
    }
    return (0);
  }
  if (youmonst.data->mlet == S_MIMIC && youmonst.m_ap_type && !range2 &&
      foundyou && !player.uswallow) {
    if (!youseeit)
      pline("It gets stuck on you.");
    else
      pline("Wait, %s!  That's a %s named %s!", m_monnam(mtmp),
            youmonst.data->mname, plname);
    player.ustuck = mtmp;
    youmonst.m_ap_type = M_AP_NOTHING;
    youmonst.mappearance = 0;
    newsym(player.ux, player.uy);
    return (0);
  }

  /* player might be mimicking an object */
  if (youmonst.m_ap_type == M_AP_OBJECT && !range2 && foundyou &&
      !player.uswallow) {
    if (!youseeit)
      pline("%s %s!", Something,
            (likes_gold(mtmp->data) && youmonst.mappearance == GOLD_PIECE)
                ? "tries to pick you up"
                : "disturbs you");
    else
      pline("Wait, %s!  That %s is really %s named %s!", m_monnam(mtmp),
            mimic_obj_name(&youmonst), an(mons[player.umonnum].mname), plname);
    if (multi < 0) { /* this should always be the case */
      char buf[BUFSZ];
      sprintf(buf, "You appear to be %s again.",
              Upolyd ? (const char *)an(youmonst.data->mname)
                     : (const char *)"yourself");
      unmul(buf); /* immediately stop mimicking */
    }
    return 0;
  }

  /*	Work out the armor class differential	*/
  tmp = AC_VALUE(player.uac) + 10; /* tmp ~= 0 - 20 */
  tmp += mtmp->m_lev;
  if (multi < 0)
    tmp += 4;
  if ((Invis && !perceives(mdat)) || !mtmp->mcansee)
    tmp -= 2;
  if (mtmp->mtrapped)
    tmp -= 2;
  if (tmp <= 0)
    tmp = 1;

  /* make eels visible the moment they hit/miss us */
  if (mdat->mlet == S_EEL && mtmp->minvis && cansee(mtmp->mx, mtmp->my)) {
    mtmp->minvis = 0;
    newsym(mtmp->mx, mtmp->my);
  }

  /*	Special demon handling code */
  if (!mtmp->cham && is_demon(mdat) && !range2 &&
      mtmp->data != &mons[PM_BALROG] && mtmp->data != &mons[PM_SUCCUBUS] &&
      mtmp->data != &mons[PM_INCUBUS])
    if (!mtmp->mcan && !rn2(13))
      msummon(mtmp);

  /*	Special lycanthrope handling code */
  if (!mtmp->cham && is_were(mdat) && !range2) {
    if (is_human(mdat)) {
      if (!rn2(5 - (night() * 2)) && !mtmp->mcan)
        new_were(mtmp);
    } else if (!rn2(30) && !mtmp->mcan)
      new_were(mtmp);
    mdat = mtmp->data;

    if (!rn2(10) && !mtmp->mcan) {
      int numseen, numhelp;
      char buf[BUFSZ], genericwere[BUFSZ];

      strcpy(genericwere, "creature");
      numhelp = were_summon(mdat, FALSE, &numseen, genericwere);
      if (youseeit) {
        pline("%s summons help!", Monnam(mtmp));
        if (numhelp > 0) {
          if (numseen == 0)
            You_feel("hemmed in.");
        } else
          pline("But none comes.");
      } else {
        const char *from_nowhere;

        if (flags.soundok) {
          pline("%s %s!", Something, makeplural(growl_sound(mtmp)));
          from_nowhere = "";
        } else
          from_nowhere = " from nowhere";
        if (numhelp > 0) {
          if (numseen < 1)
            You_feel("hemmed in.");
          else {
            if (numseen == 1)
              sprintf(buf, "%s appears", an(genericwere));
            else
              sprintf(buf, "%s appear", makeplural(genericwere));
            pline("%s%s!", upstart(buf), from_nowhere);
          }
        } /* else no help came; but you didn't know it tried */
      }
    }
  }

  if (player.uinvulnerable) {
    /* monsters won't attack you */
    if (mtmp == player.ustuck)
      pline("%s loosens its grip slightly.", Monnam(mtmp));
    else if (!range2) {
      if (youseeit || sensemon(mtmp))
        pline("%s starts to attack you, but pulls back.", Monnam(mtmp));
      else
        You_feel("%s move nearby.", something);
    }
    return (0);
  }

  /* Unlike defensive stuff, don't let them use item _and_ attack. */
  if (find_offensive(mtmp)) {
    int foo = use_offensive(mtmp);

    if (foo != 0)
      return (foo == 1);
  }

  for (i = 0; i < NATTK; i++) {
    sum[i] = 0;
    mattk = getmattk(mdat, i, sum, &alt_attk);
    if (player.uswallow && (mattk->aatyp != AT_ENGL))
      continue;
    switch (mattk->aatyp) {
      case AT_CLAW: /* "hand to hand" attacks */
      case AT_KICK:
      case AT_BITE:
      case AT_STNG:
      case AT_TUCH:
      case AT_BUTT:
      case AT_TENT:
        if (!range2 && (!mtmp->weapon() || mtmp->mconf || Conflict ||
                        !touch_petrifies(youmonst.data))) {
          if (foundyou) {
            if (tmp > (j = rnd(20 + i))) {
              if (mattk->aatyp != AT_KICK || !thick_skinned(youmonst.data))
                sum[i] = hitmu(mtmp, mattk);
            } else
              missmu(mtmp, (tmp == j), mattk);
          } else
            wildmiss(mtmp, mattk);
        }
        break;

      case AT_HUGS: /* automatic if prev two attacks succeed */
        /* Note: if displaced, prev attacks never succeeded */
        if ((!range2 && i >= 2 && sum[i - 1] && sum[i - 2]) ||
            mtmp == player.ustuck)
          sum[i] = hitmu(mtmp, mattk);
        break;

      case AT_GAZE: /* can affect you either ranged or not */
                    /* Medusa gaze already operated through m_respond in
                     * dochug(); don't gaze more than once per round.
                     */
        if (mdat != &mons[PM_MEDUSA])
          sum[i] = gazemu(mtmp, mattk);
        break;

      case AT_EXPL: /* automatic hit if next to, and aimed at you */
        if (!range2)
          sum[i] = explmu(mtmp, mattk, foundyou);
        break;

      case AT_ENGL:
        if (!range2) {
          if (foundyou) {
            if (player.uswallow || tmp > (j = rnd(20 + i))) {
              /* Force swallowing monster to be
               * displayed even when player is
               * moving away */
              flush_screen(1);
              sum[i] = gulpmu(mtmp, mattk);
            } else {
              missmu(mtmp, (tmp == j), mattk);
            }
          } else if (is_animal(mtmp->data)) {
            pline("%s gulps some air!", Monnam(mtmp));
          } else {
            if (youseeit)
              pline("%s lunges forward and recoils!", Monnam(mtmp));
            else
              You_hear("a %s nearby.",
                       is_whirly(mtmp->data) ? "rushing noise" : "splat");
          }
        }
        break;
      case AT_BREA:
        if (range2)
          sum[i] = breamu(mtmp, mattk);
        /* Note: breamu takes care of displacement */
        break;
      case AT_SPIT:
        if (range2)
          sum[i] = spitmu(mtmp, mattk);
        /* Note: spitmu takes care of displacement */
        break;
      case AT_WEAP:
        if (range2) {
#ifdef REINCARNATION
          if (!Is_rogue_level(&player.uz))
#endif
            thrwmu(mtmp);
        } else {
          int hittmp = 0;

          /* Rare but not impossible.  Normally the monster
           * wields when 2 spaces away, but it can be
           * teleported or whatever....
           */
          if (mtmp->weapon_check == NEED_WEAPON || !mtmp->weapon()) {
            mtmp->weapon_check = NEED_HTH_WEAPON;
            /* mon_wield_item resets weapon_check as
             * appropriate */
            if (mon_wield_item(mtmp) != 0)
              break;
          }
          if (foundyou) {
            otmp = mtmp->weapon();
            if (otmp) {
              hittmp = hitval(otmp, &youmonst);
              tmp += hittmp;
              mswings(mtmp, otmp);
            }
            if (tmp > (j = dieroll = rnd(20 + i)))
              sum[i] = hitmu(mtmp, mattk);
            else
              missmu(mtmp, (tmp == j), mattk);
            /* KMH -- Don't accumulate to-hit bonuses */
            if (otmp)
              tmp -= hittmp;
          } else
            wildmiss(mtmp, mattk);
        }
        break;
      case AT_MAGC:
        if (range2)
          sum[i] = buzzmu(mtmp, mattk);
        else {
          if (foundyou)
            sum[i] = castmu(mtmp, mattk, TRUE, TRUE);
          else
            sum[i] = castmu(mtmp, mattk, TRUE, FALSE);
        }
        break;

      default: /* no attack */
        break;
    }
    if (flags.botl)
      bot();
    /* give player a chance of waking up before dying -kaa */
    if (sum[i] == 1) { /* successful attack */
      if (player.usleep && player.usleep < monstermoves && !rn2(10)) {
        multi = -1;
        nomovemsg = "The combat suddenly awakens you.";
      }
    }
    if (sum[i] == 2)
      return 1; /* attacker dead */
    if (sum[i] == 3)
      break; /* attacker teleported, no more attacks */
             /* sum[i] == 0: unsuccessful attack */
  }
  return (0);
}


/*
 * helper function for some compilers that have trouble with hitmu
 */

STATIC_OVL void hurtarmor(int attk) {
  int hurt;

  switch (attk) {
    /* 0 is burning, which we should never be called with */
    case AD_RUST:
      hurt = 1;
      break;
    case AD_CORR:
      hurt = 3;
      break;
    default:
      hurt = 2;
      break;
  }

  /* What the following code does: it keeps looping until it
   * finds a target for the rust monster.
   * Head, feet, etc... not covered by metal, or covered by
   * rusty metal, are not targets.  However, your body always
   * is, no matter what covers it.
   */
  while (1) {
    switch (rn2(5)) {
      case 0:
        if (!uarmh || !rust_dmg(uarmh, xname(uarmh), hurt, FALSE, &youmonst))
          continue;
        break;
      case 1:
        if (uarmc) {
          (void)rust_dmg(uarmc, xname(uarmc), hurt, TRUE, &youmonst);
          break;
        }
        /* Note the difference between break and continue;
         * break means it was hit and didn't rust; continue
         * means it wasn't a target and though it didn't rust
         * something else did.
         */
        if (uarm)
          (void)rust_dmg(uarm, xname(uarm), hurt, TRUE, &youmonst);
#ifdef TOURIST
        else if (uarmu)
          (void)rust_dmg(uarmu, xname(uarmu), hurt, TRUE, &youmonst);
#endif
        break;
      case 2:
        if (!uarms || !rust_dmg(uarms, xname(uarms), hurt, FALSE, &youmonst))
          continue;
        break;
      case 3:
        if (!uarmg || !rust_dmg(uarmg, xname(uarmg), hurt, FALSE, &youmonst))
          continue;
        break;
      case 4:
        if (!uarmf || !rust_dmg(uarmf, xname(uarmf), hurt, FALSE, &youmonst))
          continue;
        break;
    }
    break; /* Out of while loop */
  }
}


STATIC_OVL bool diseasemu(MonsterType *mdat) {
  if (Sick_resistance) {
    You_feel("a slight illness.");
    return FALSE;
  } else {
    make_sick(Sick ? Sick / 3L + 1L : (long)rn1(ACURR(A_CON), 20), mdat->mname,
              TRUE, SICK_NONVOMITABLE);
    return TRUE;
  }
}

/* check whether slippery clothing protects from hug or wrap attack */
STATIC_OVL bool u_slip_free(Monster *mtmp, struct Attack *mattk) {
  Object *obj = (uarmc ? uarmc : uarm);

#ifdef TOURIST
  if (!obj)
    obj = uarmu;
#endif
  if (mattk->adtyp == AD_DRIN)
    obj = uarmh;

  /* if your cloak/armor is greased, monster slips off; this
     protection might fail (33% chance) when the armor is cursed */
  if (obj && (obj->greased || obj->otyp == OILSKIN_CLOAK) &&
      (!obj->cursed || rn2(3))) {
    pline("%s %s your %s %s!", Monnam(mtmp),
          (mattk->adtyp == AD_WRAP) ? "slips off of"
                                    : "grabs you, but cannot hold onto",
          obj->greased ? "greased" : "slippery",
          /* avoid "slippery slippery cloak"
             for undiscovered oilskin cloak */
          (obj->greased || objects[obj->otyp].oc_name_known)
              ? xname(obj)
              : cloak_simple_name(obj));

    if (obj->greased && !rn2(2)) {
      pline_The("grease wears off.");
      obj->greased = 0;
      update_inventory();
    }
    return TRUE;
  }
  return FALSE;
}

/* armor that sufficiently covers the body might be able to block magic */
int magic_negation(Monster *mon) {
  Object *armor;
  int armpro = 0;

  armor = (mon == &youmonst) ? uarm : which_armor(mon, W_ARM);
  if (armor && armpro < objects[armor->otyp].a_can)
    armpro = objects[armor->otyp].a_can;
  armor = (mon == &youmonst) ? uarmc : which_armor(mon, W_ARMC);
  if (armor && armpro < objects[armor->otyp].a_can)
    armpro = objects[armor->otyp].a_can;
  armor = (mon == &youmonst) ? uarmh : which_armor(mon, W_ARMH);
  if (armor && armpro < objects[armor->otyp].a_can)
    armpro = objects[armor->otyp].a_can;

/* armor types for shirt, gloves, shoes, and shield don't currently
   provide any magic cancellation but we might as well be complete */
#ifdef TOURIST
  armor = (mon == &youmonst) ? uarmu : which_armor(mon, W_ARMU);
  if (armor && armpro < objects[armor->otyp].a_can)
    armpro = objects[armor->otyp].a_can;
#endif
  armor = (mon == &youmonst) ? uarmg : which_armor(mon, W_ARMG);
  if (armor && armpro < objects[armor->otyp].a_can)
    armpro = objects[armor->otyp].a_can;
  armor = (mon == &youmonst) ? uarmf : which_armor(mon, W_ARMF);
  if (armor && armpro < objects[armor->otyp].a_can)
    armpro = objects[armor->otyp].a_can;
  armor = (mon == &youmonst) ? uarms : which_armor(mon, W_ARMS);
  if (armor && armpro < objects[armor->otyp].a_can)
    armpro = objects[armor->otyp].a_can;

#ifdef STEED
  /* this one is really a stretch... */
  armor = (mon == &youmonst) ? 0 : which_armor(mon, W_SADDLE);
  if (armor && armpro < objects[armor->otyp].a_can)
    armpro = objects[armor->otyp].a_can;
#endif

  return armpro;
}

/*
 * hitmu: monster hits you
 *	  returns 2 if monster dies (e.g. "yellow light"), 1 otherwise
 *	  3 if the monster lives but teleported/paralyzed, so it can't keep
 *	       attacking you
 */
STATIC_OVL int hitmu(Monster *mtmp, struct Attack *mattk) {
  MonsterType *mdat = mtmp->data;
  int uncancelled, ptmp;
  int dmg, armpro, permdmg;
  char buf[BUFSZ];
  MonsterType *olduasmon = youmonst.data;
  int res;

  if (!canspotmon(mtmp))
    map_invisible(mtmp->mx, mtmp->my);

  /*	If the monster is undetected & hits you, you should know where
   *	the attack came from.
   */
  if (mtmp->mundetected && (hides_under(mdat) || mdat->mlet == S_EEL)) {
    mtmp->mundetected = 0;
    if (!(Blind ? Blind_telepat : Unblind_telepat)) {
      Object *obj;
      const char *what;

      if ((obj = level.objects[mtmp->mx][mtmp->my]) != 0) {
        if (Blind && !obj->dknown)
          what = something;
        else if (is_pool(mtmp->mx, mtmp->my) && !Underwater)
          what = "the water";
        else
          what = doname(obj);

        pline("%s was hidden under %s!", Amonnam(mtmp), what);
      }
      newsym(mtmp->mx, mtmp->my);
    }
  }

  /*	First determine the base damage done */
  dmg = d((int)mattk->damn, (int)mattk->damd);
  if (is_undead(mdat) && midnight())
    dmg += d((int)mattk->damn, (int)mattk->damd); /* extra damage */

  /*	Next a cancellation factor	*/
  /*	Use uncancelled when the cancellation factor takes into account certain
   *	armor's special magic protection.  Otherwise just use !mtmp->mcan.
   */
  armpro = magic_negation(&youmonst);
  uncancelled = !mtmp->mcan && ((rn2(3) >= armpro) || !rn2(50));

  permdmg = 0;
  /*	Now, adjust damages via resistances or specific attacks */
  switch (mattk->adtyp) {
    case AD_PHYS:
      if (mattk->aatyp == AT_HUGS && !sticks(youmonst.data)) {
        if (!player.ustuck && rn2(2)) {
          if (u_slip_free(mtmp, mattk)) {
            dmg = 0;
          } else {
            player.ustuck = mtmp;
            pline("%s grabs you!", Monnam(mtmp));
          }
        } else if (player.ustuck == mtmp) {
          exercise(A_STR, FALSE);
          You("are being %s.",
              (mtmp->data == &mons[PM_ROPE_GOLEM]) ? "choked" : "crushed");
        }
      } else { /* hand to hand weapon */
        if (mattk->aatyp == AT_WEAP && otmp) {
          if (otmp->otyp == CORPSE && touch_petrifies(&mons[otmp->corpsenm])) {
            dmg = 1;
            pline("%s hits you with the %s corpse.", Monnam(mtmp),
                  mons[otmp->corpsenm].mname);
            if (!Stoned)
              goto do_stone;
          }
          dmg += dmgval(otmp, &youmonst);
          if (dmg <= 0)
            dmg = 1;
          if (!(otmp->oartifact &&
                artifact_hit(mtmp, &youmonst, otmp, &dmg, dieroll)))
            hitmsg(mtmp, mattk);
          if (!dmg)
            break;
          if (player.mh > 1 &&
              player.mh > ((player.uac > 0) ? dmg : dmg + player.uac) &&
              objects[otmp->otyp].oc_material == IRON &&
              (player.umonnum == PM_BLACK_PUDDING ||
               player.umonnum == PM_BROWN_PUDDING)) {
            /* This redundancy necessary because you have to
             * take the damage _before_ being cloned.
             */
            if (player.uac < 0)
              dmg += player.uac;
            if (dmg < 1)
              dmg = 1;
            if (dmg > 1)
              exercise(A_STR, FALSE);
            player.mh -= dmg;
            flags.botl = 1;
            dmg = 0;
            if (cloneu())
              You("divide as %s hits you!", mon_nam(mtmp));
          }
          urustm(mtmp, otmp);
        } else if (mattk->aatyp != AT_TUCH || dmg != 0 || mtmp != player.ustuck)
          hitmsg(mtmp, mattk);
      }
      break;
    case AD_DISE:
      hitmsg(mtmp, mattk);
      if (!diseasemu(mdat))
        dmg = 0;
      break;
    case AD_FIRE:
      hitmsg(mtmp, mattk);
      if (uncancelled) {
        pline("You're %s!", on_fire(youmonst.data, mattk));
        if (youmonst.data == &mons[PM_STRAW_GOLEM] ||
            youmonst.data == &mons[PM_PAPER_GOLEM]) {
          You("roast!");
          /* KMH -- this is okay with unchanging */
          rehumanize();
          break;
        } else if (Fire_resistance) {
          pline_The("fire doesn't feel hot!");
          dmg = 0;
        }
        if ((int)mtmp->m_lev > rn2(20))
          destroy_item(SCROLL_CLASS, AD_FIRE);
        if ((int)mtmp->m_lev > rn2(20))
          destroy_item(POTION_CLASS, AD_FIRE);
        if ((int)mtmp->m_lev > rn2(25))
          destroy_item(SPBOOK_CLASS, AD_FIRE);
        burn_away_slime();
      } else
        dmg = 0;
      break;
    case AD_COLD:
      hitmsg(mtmp, mattk);
      if (uncancelled) {
        pline("You're covered in frost!");
        if (Cold_resistance) {
          pline_The("frost doesn't seem cold!");
          dmg = 0;
        }
        if ((int)mtmp->m_lev > rn2(20))
          destroy_item(POTION_CLASS, AD_COLD);
      } else
        dmg = 0;
      break;
    case AD_ELEC:
      hitmsg(mtmp, mattk);
      if (uncancelled) {
        You("get zapped!");
        if (Shock_resistance) {
          pline_The("zap doesn't shock you!");
          dmg = 0;
        }
        if ((int)mtmp->m_lev > rn2(20))
          destroy_item(WAND_CLASS, AD_ELEC);
        if ((int)mtmp->m_lev > rn2(20))
          destroy_item(RING_CLASS, AD_ELEC);
      } else
        dmg = 0;
      break;
    case AD_SLEE:
      hitmsg(mtmp, mattk);
      if (uncancelled && multi >= 0 && !rn2(5)) {
        if (Sleep_resistance)
          break;
        fall_asleep(-rnd(10), TRUE);
        if (Blind)
          You("are put to sleep!");
        else
          You("are put to sleep by %s!", mon_nam(mtmp));
      }
      break;
    case AD_BLND:
      if (can_blnd(mtmp, &youmonst, mattk->aatyp, nullptr)) {
        if (!Blind)
          pline("%s blinds you!", Monnam(mtmp));
        make_blinded(Blinded + (long)dmg, FALSE);
        if (!Blind)
          Your(vision_clears);
      }
      dmg = 0;
      break;
    case AD_DRST:
      ptmp = A_STR;
      goto dopois;
    case AD_DRDX:
      ptmp = A_DEX;
      goto dopois;
    case AD_DRCO:
      ptmp = A_CON;
    dopois:
      hitmsg(mtmp, mattk);
      if (uncancelled && !rn2(8)) {
        sprintf(buf, "%s %s", s_suffix(Monnam(mtmp)),
                mpoisons_subj(mtmp, mattk));
        poisoned(buf, ptmp, mdat->mname, 30);
      }
      break;
    case AD_DRIN:
      hitmsg(mtmp, mattk);
      if (defends(AD_DRIN, uwep) || !has_head(youmonst.data)) {
        You("don't seem harmed.");
        /* Not clear what to do for green slimes */
        break;
      }
      if (u_slip_free(mtmp, mattk))
        break;

      if (uarmh && rn2(8)) {
        /* not body_part(HEAD) */
        Your("helmet blocks the attack to your head.");
        break;
      }
      if (Half_physical_damage)
        dmg = (dmg + 1) / 2;
      mdamageu(mtmp, dmg);

      if (!uarmh || uarmh->otyp != DUNCE_CAP) {
        Your("brain is eaten!");
        /* No such thing as mindless players... */
        if (ABASE(A_INT) <= ATTRMIN(A_INT)) {
          int lifesaved = 0;
          Object *wore_amulet = uamul;

          while (1) {
            /* avoid looping on "die(y/n)?" */
            if (lifesaved && (discover || wizard)) {
              if (wore_amulet && !uamul) {
                /* used up AMULET_OF_LIFE_SAVING; still
                   subject to dying from brainlessness */
                wore_amulet = 0;
              } else {
                /* explicitly chose not to die;
                   arbitrarily boost intelligence */
                ABASE(A_INT) = ATTRMIN(A_INT) + 2;
                You_feel("like a scarecrow.");
                break;
              }
            }

            if (lifesaved)
              pline("Unfortunately your brain is still gone.");
            else
              Your("last thought fades away.");
            killer = "brainlessness";
            killer_format = KILLED_BY;
            done(DIED);
            lifesaved++;
          }
        }
      }
      /* adjattrib gives dunce cap message when appropriate */
      (void)adjattrib(A_INT, -rnd(2), FALSE);
      forget_levels(25);  /* lose memory of 25% of levels */
      forget_objects(25); /* lose memory of 25% of objects */
      exercise(A_WIS, FALSE);
      break;
    case AD_PLYS:
      hitmsg(mtmp, mattk);
      if (uncancelled && multi >= 0 && !rn2(3)) {
        if (Free_action) {
          You("momentarily stiffen.");
        } else {
          if (Blind)
            You("are frozen!");
          else
            You("are frozen by %s!", mon_nam(mtmp));
          nomovemsg = 0; /* default: "you can move again" */
          nomul(-rnd(10), "paralyzed by a monster");
          exercise(A_DEX, FALSE);
        }
      }
      break;
    case AD_DRLI:
      hitmsg(mtmp, mattk);
      if (uncancelled && !rn2(3) && !Drain_resistance) {
        losexp("life drainage");
      }
      break;
    case AD_LEGS: {
      long side = rn2(2) ? RIGHT_SIDE : LEFT_SIDE;
      const char *sidestr = (side == RIGHT_SIDE) ? "right" : "left";

      /* This case is too obvious to ignore, but Nethack is not in
       * general very good at considering height--most short monsters
       * still _can_ attack you when you're flying or mounted.
       * [FIXME: why can't a flying attacker overcome this?]
       */
      if (
#ifdef STEED
              player.usteed ||
#endif
              Levitation || Flying) {
        pline("%s tries to reach your %s %s!", Monnam(mtmp), sidestr,
              body_part(LEG));
        dmg = 0;
      } else if (mtmp->mcan) {
        pline("%s nuzzles against your %s %s!", Monnam(mtmp), sidestr,
              body_part(LEG));
        dmg = 0;
      } else {
        if (uarmf) {
          if (rn2(2) && (uarmf->otyp == LOW_BOOTS || uarmf->otyp == IRON_SHOES))
            pline("%s pricks the exposed part of your %s %s!", Monnam(mtmp),
                  sidestr, body_part(LEG));
          else if (!rn2(5))
            pline("%s pricks through your %s boot!", Monnam(mtmp), sidestr);
          else {
            pline("%s scratches your %s boot!", Monnam(mtmp), sidestr);
            dmg = 0;
            break;
          }
        } else
          pline("%s pricks your %s %s!", Monnam(mtmp), sidestr, body_part(LEG));
        set_wounded_legs(side, rnd(60 - ACURR(A_DEX)));
        exercise(A_STR, FALSE);
        exercise(A_DEX, FALSE);
      }
      break;
    }
    case AD_STON: /* cockatrice */
      hitmsg(mtmp, mattk);
      if (!rn2(3)) {
        if (mtmp->mcan) {
          if (flags.soundok)
            You_hear("a cough from %s!", mon_nam(mtmp));
        } else {
          if (flags.soundok)
            You_hear("%s hissing!", s_suffix(mon_nam(mtmp)));
          if (!rn2(10) || (flags.moonphase == NEW_MOON && !have_lizard())) {
          do_stone:
            if (!Stoned && !Stone_resistance &&
                !(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))) {
              Stoned = 5;
              delayed_killer = mtmp->data->mname;
              if (mtmp->data->geno & G_UNIQ) {
                if (!type_is_pname(mtmp->data)) {
                  static char kbuf[BUFSZ];

                  /* "the" buffer may be reallocated */
                  strcpy(kbuf, the(delayed_killer));
                  delayed_killer = kbuf;
                }
                killer_format = KILLED_BY;
              } else
                killer_format = KILLED_BY_AN;
              return (1);
              /* You("turn to stone..."); */
              /* done_in_by(mtmp); */
            }
          }
        }
      }
      break;
    case AD_STCK:
      hitmsg(mtmp, mattk);
      if (uncancelled && !player.ustuck && !sticks(youmonst.data))
        player.ustuck = mtmp;
      break;
    case AD_WRAP:
      if ((!mtmp->mcan || player.ustuck == mtmp) && !sticks(youmonst.data)) {
        if (!player.ustuck && !rn2(10)) {
          if (u_slip_free(mtmp, mattk)) {
            dmg = 0;
          } else {
            pline("%s swings itself around you!", Monnam(mtmp));
            player.ustuck = mtmp;
          }
        } else if (player.ustuck == mtmp) {
          if (is_pool(mtmp->mx, mtmp->my) && !Swimming && !Amphibious) {
            bool moat = (levl[mtmp->mx][mtmp->my].typ != POOL) &&
                        (levl[mtmp->mx][mtmp->my].typ != WATER) &&
                        !Is_medusa_level(&player.uz) &&
                        !Is_waterlevel(&player.uz);

            pline("%s drowns you...", Monnam(mtmp));
            killer_format = KILLED_BY_AN;
            sprintf(buf, "%s by %s", moat ? "moat" : "pool of water",
                    an(mtmp->data->mname));
            killer = buf;
            done(DROWNING);
          } else if (mattk->aatyp == AT_HUGS)
            You("are being crushed.");
        } else {
          dmg = 0;
          if (flags.verbose)
            pline("%s brushes against your %s.", Monnam(mtmp), body_part(LEG));
        }
      } else
        dmg = 0;
      break;
    case AD_WERE:
      hitmsg(mtmp, mattk);
      if (uncancelled && !rn2(4) && player.ulycn == NON_PM &&
          !Protection_from_shape_changers && !defends(AD_WERE, uwep)) {
        You_feel("feverish.");
        exercise(A_CON, FALSE);
        player.ulycn = monsndx(mdat);
      }
      break;
    case AD_SGLD:
      hitmsg(mtmp, mattk);
      if (youmonst.data->mlet == mdat->mlet)
        break;
      if (!mtmp->mcan)
        stealgold(mtmp);
      break;

    case AD_SITM: /* for now these are the same */
    case AD_SEDU:
      if (is_animal(mtmp->data)) {
        hitmsg(mtmp, mattk);
        if (mtmp->mcan)
          break;
        /* Continue below */
      } else if (dmgtype(youmonst.data, AD_SEDU)
#ifdef SEDUCE
                 || dmgtype(youmonst.data, AD_SSEX)
#endif
                 ) {
        pline("%s %s.", Monnam(mtmp),
              mtmp->minvent
                  ? "brags about the goods some dungeon explorer provided"
                  : "makes some remarks about how difficult theft is lately");
        if (!tele_restrict(mtmp))
          (void)rloc(mtmp, FALSE);
        return 3;
      } else if (mtmp->mcan) {
        if (!Blind) {
          pline("%s tries to %s you, but you seem %s.",
                Adjmonnam(mtmp, "plain"), flags.female ? "charm" : "seduce",
                flags.female ? "unaffected" : "uninterested");
        }
        if (rn2(3)) {
          if (!tele_restrict(mtmp))
            (void)rloc(mtmp, FALSE);
          return 3;
        }
        break;
      }
      buf[0] = '\0';
      switch (steal(mtmp, buf)) {
        case -1:
          return 2;
        case 0:
          break;
        default:
          if (!is_animal(mtmp->data) && !tele_restrict(mtmp))
            (void)rloc(mtmp, FALSE);
          if (is_animal(mtmp->data) && *buf) {
            if (canseemon(mtmp))
              pline("%s tries to %s away with %s.", Monnam(mtmp),
                    locomotion(mtmp->data, "run"), buf);
          }
          monflee(mtmp, 0, FALSE, FALSE);
          return 3;
      }
      break;
#ifdef SEDUCE
    case AD_SSEX:
      if (could_seduce(mtmp, &youmonst, mattk) == 1 && !mtmp->mcan)
        if (doseduce(mtmp))
          return 3;
      break;
#endif
    case AD_SAMU:
      hitmsg(mtmp, mattk);
      /* when the Wiz hits, 1/20 steals the amulet */
      if (player.uhave.amulet || player.uhave.bell || player.uhave.book ||
          player.uhave.menorah ||
          player.uhave.questart) /* carrying the Quest Artifact */
        if (!rn2(20))
          stealamulet(mtmp);
      break;

    case AD_TLPT:
      hitmsg(mtmp, mattk);
      if (uncancelled) {
        if (flags.verbose)
          Your("position suddenly seems very uncertain!");
        tele();
      }
      break;
    case AD_RUST:
      hitmsg(mtmp, mattk);
      if (mtmp->mcan)
        break;
      if (player.umonnum == PM_IRON_GOLEM) {
        You("rust!");
        /* KMH -- this is okay with unchanging */
        rehumanize();
        break;
      }
      hurtarmor(AD_RUST);
      break;
    case AD_CORR:
      hitmsg(mtmp, mattk);
      if (mtmp->mcan)
        break;
      hurtarmor(AD_CORR);
      break;
    case AD_DCAY:
      hitmsg(mtmp, mattk);
      if (mtmp->mcan)
        break;
      if (player.umonnum == PM_WOOD_GOLEM ||
          player.umonnum == PM_LEATHER_GOLEM) {
        You("rot!");
        /* KMH -- this is okay with unchanging */
        rehumanize();
        break;
      }
      hurtarmor(AD_DCAY);
      break;
    case AD_HEAL:
      /* a cancelled nurse is just an ordinary monster */
      if (mtmp->mcan) {
        hitmsg(mtmp, mattk);
        break;
      }
      if (!uwep
#ifdef TOURIST
          && !uarmu
#endif
          && !uarm && !uarmh && !uarms && !uarmg && !uarmc && !uarmf) {
        bool goaway = FALSE;
        pline("%s hits!  (I hope you don't mind.)", Monnam(mtmp));
        if (Upolyd) {
          player.mh += rnd(7);
          if (!rn2(7)) {
            /* no upper limit necessary; effect is temporary */
            player.mhmax++;
            if (!rn2(13))
              goaway = TRUE;
          }
          if (player.mh > player.mhmax)
            player.mh = player.mhmax;
        } else {
          player.uhp += rnd(7);
          if (!rn2(7)) {
            /* hard upper limit via nurse care: 25 * ulevel */
            if (player.uhpmax < 5 * player.ulevel + d(2 * player.ulevel, 10))
              player.uhpmax++;
            if (!rn2(13))
              goaway = TRUE;
          }
          if (player.uhp > player.uhpmax)
            player.uhp = player.uhpmax;
        }
        if (!rn2(3))
          exercise(A_STR, TRUE);
        if (!rn2(3))
          exercise(A_CON, TRUE);
        if (Sick)
          make_sick(0L, nullptr, FALSE, SICK_ALL);
        flags.botl = 1;
        if (goaway) {
          mongone(mtmp);
          return 2;
        } else if (!rn2(33)) {
          if (!tele_restrict(mtmp))
            (void)rloc(mtmp, FALSE);
          monflee(mtmp, d(3, 6), TRUE, FALSE);
          return 3;
        }
        dmg = 0;
      } else {
        if (Role_if(PM_HEALER)) {
          if (flags.soundok && !(moves % 5))
            verbalize("Doc, I can't help you unless you cooperate.");
          dmg = 0;
        } else
          hitmsg(mtmp, mattk);
      }
      break;
    case AD_CURS:
      hitmsg(mtmp, mattk);
      if (!night() && mdat == &mons[PM_GREMLIN])
        break;
      if (!mtmp->mcan && !rn2(10)) {
        if (flags.soundok) {
          if (Blind)
            You_hear("laughter.");
          else
            pline("%s chuckles.", Monnam(mtmp));
        }
        if (player.umonnum == PM_CLAY_GOLEM) {
          pline("Some writing vanishes from your head!");
          /* KMH -- this is okay with unchanging */
          rehumanize();
          break;
        }
        attrcurse();
      }
      break;
    case AD_STUN:
      hitmsg(mtmp, mattk);
      if (!mtmp->mcan && !rn2(4)) {
        make_stunned(HStun + dmg, TRUE);
        dmg /= 2;
      }
      break;
    case AD_ACID:
      hitmsg(mtmp, mattk);
      if (!mtmp->mcan && !rn2(3))
        if (Acid_resistance) {
          pline("You're covered in acid, but it seems harmless.");
          dmg = 0;
        } else {
          pline("You're covered in acid!	It burns!");
          exercise(A_STR, FALSE);
        }
      else
        dmg = 0;
      break;
    case AD_SLOW:
      hitmsg(mtmp, mattk);
      if (uncancelled && HFast && !defends(AD_SLOW, uwep) && !rn2(4))
        u_slow_down();
      break;
    case AD_DREN:
      hitmsg(mtmp, mattk);
      if (uncancelled && !rn2(4))
        drain_en(dmg);
      dmg = 0;
      break;
    case AD_CONF:
      hitmsg(mtmp, mattk);
      if (!mtmp->mcan && !rn2(4) && !mtmp->mspec_used) {
        mtmp->mspec_used = mtmp->mspec_used + (dmg + rn2(6));
        if (Confusion)
          You("are getting even more confused.");
        else
          You("are getting confused.");
        make_confused(HConfusion + dmg, FALSE);
      }
      dmg = 0;
      break;
    case AD_DETH:
      pline("%s reaches out with its deadly touch.", Monnam(mtmp));
      if (is_undead(youmonst.data)) {
        /* Still does normal damage */
        pline("Was that the touch of death?");
        break;
      }
      switch (rn2(20)) {
        case 19:
        case 18:
        case 17:
          if (!Antimagic) {
            killer_format = KILLED_BY_AN;
            killer = "touch of death";
            done(DIED);
            dmg = 0;
            break;
          }      /* else FALLTHRU */
        default: /* case 16: ... case 5: */
          You_feel("your life force draining away...");
          permdmg = 1; /* actual damage done below */
          break;
        case 4:
        case 3:
        case 2:
        case 1:
        case 0:
          if (Antimagic)
            shieldeff(player.ux, player.uy);
          pline("Lucky for you, it didn't work!");
          dmg = 0;
          break;
      }
      break;
    case AD_PEST:
      pline("%s reaches out, and you feel fever and chills.", Monnam(mtmp));
      (void)diseasemu(mdat); /* plus the normal damage */
      break;
    case AD_FAMN:
      pline("%s reaches out, and your body shrivels.", Monnam(mtmp));
      exercise(A_CON, FALSE);
      if (!is_fainted())
        morehungry(rn1(40, 40));
      /* plus the normal damage */
      break;
    case AD_SLIM:
      hitmsg(mtmp, mattk);
      if (!uncancelled)
        break;
      if (flaming(youmonst.data)) {
        pline_The("slime burns away!");
        dmg = 0;
      } else if (Unchanging || youmonst.data == &mons[PM_GREEN_SLIME]) {
        You("are unaffected.");
        dmg = 0;
      } else if (!Slimed) {
        You("don't feel very well.");
        Slimed = 10L;
        flags.botl = 1;
        killer_format = KILLED_BY_AN;
        delayed_killer = mtmp->data->mname;
      } else
        pline("Yuck!");
      break;
    case AD_ENCH: /* KMH -- remove enchantment (disenchanter) */
      hitmsg(mtmp, mattk);
      /* uncancelled is sufficient enough; please
         don't make this attack less frequent */
      if (uncancelled) {
        Object *obj = some_armor(&youmonst);

        if (drain_item(obj)) {
          Your("%s less effective.", aobjnam(obj, "seem"));
        }
      }
      break;
    default:
      dmg = 0;
      break;
  }
  if (player.uhp < 1)
    done_in_by(mtmp);

  /*	Negative armor class reduces damage done instead of fully protecting
   *	against hits.
   */
  if (dmg && player.uac < 0) {
    dmg -= rnd(-player.uac);
    if (dmg < 1)
      dmg = 1;
  }

  if (dmg) {
    if (Half_physical_damage
        /* Mitre of Holiness */
        ||
        (Role_if(PM_PRIEST) && uarmh && is_quest_artifact(uarmh) &&
         (is_undead(mtmp->data) || is_demon(mtmp->data))))
      dmg = (dmg + 1) / 2;

    if (permdmg) { /* Death's life force drain */
      int lowerlimit, *hpmax_p;
      /*
       * Apply some of the damage to permanent hit points:
       *	polymorphed	    100% against poly'd hpmax
       *	hpmax > 25*lvl	    100% against normal hpmax
       *	hpmax > 10*lvl	50..100%
       *	hpmax >  5*lvl	25..75%
       *	otherwise	 0..50%
       * Never reduces hpmax below 1 hit point per level.
       */
      permdmg = rn2(dmg / 2 + 1);
      if (Upolyd || player.uhpmax > 25 * player.ulevel)
        permdmg = dmg;
      else if (player.uhpmax > 10 * player.ulevel)
        permdmg += dmg / 2;
      else if (player.uhpmax > 5 * player.ulevel)
        permdmg += dmg / 4;

      if (Upolyd) {
        hpmax_p = &player.mhmax;
        /* [can't use youmonst.m_lev] */
        lowerlimit = min((int)youmonst.data->mlevel, player.ulevel);
      } else {
        hpmax_p = &player.uhpmax;
        lowerlimit = player.ulevel;
      }
      if (*hpmax_p - permdmg > lowerlimit)
        *hpmax_p -= permdmg;
      else if (*hpmax_p > lowerlimit)
        *hpmax_p = lowerlimit;
      else /* unlikely... */
        ;  /* already at or below minimum threshold; do nothing */
      flags.botl = 1;
    }

    mdamageu(mtmp, dmg);
  }

  if (dmg)
    res = passiveum(olduasmon, mtmp, mattk);
  else
    res = 1;
  stop_occupation();
  return res;
}


/* monster swallows you, or damage if player.uswallow */
STATIC_OVL int gulpmu(Monster *mtmp, struct Attack *mattk) {
  Trap *t = t_at(player.ux, player.uy);
  int tmp = d((int)mattk->damn, (int)mattk->damd);
  int tim_tmp;
  Object *otmp2;
  int i;

  if (!player.uswallow) { /* swallows you */
    if (youmonst.data->msize >= MZ_HUGE)
      return (0);
    if ((t && ((t->ttyp == PIT) || (t->ttyp == SPIKED_PIT))) &&
        sobj_at(BOULDER, player.ux, player.uy))
      return (0);

    if (Punished)
      unplacebc(); /* ball&chain go away */
    remove_monster(mtmp->mx, mtmp->my);
    mtmp->mtrapped = 0; /* no longer on old trap */
    place_monster(mtmp, player.ux, player.uy);
    player.ustuck = mtmp;
    newsym(mtmp->mx, mtmp->my);
#ifdef STEED
    if (is_animal(mtmp->data) && player.usteed) {
      char buf[BUFSZ];
      /* Too many quirks presently if hero and steed
       * are swallowed. Pretend purple worms don't
       * like horses for now :-)
       */
      strcpy(buf, mon_nam(player.usteed));
      pline("%s lunges forward and plucks you off %s!", Monnam(mtmp), buf);
      dismount_steed(DISMOUNT_ENGULFED);
    } else
#endif
      pline("%s engulfs you!", Monnam(mtmp));
    stop_occupation();
    reset_occupations(); /* behave as if you had moved */

    if (player.utrap) {
      You("are released from the %s!",
          player.utraptype == TT_WEB ? "web" : "trap");
      player.utrap = 0;
    }

    i = number_leashed();
    if (i > 0) {
      const char *s = (i > 1) ? "leashes" : "leash";
      pline_The("%s %s loose.", s, vtense(s, "snap"));
      unleash_all();
    }

    if (touch_petrifies(youmonst.data) && !resists_ston(mtmp)) {
      minstapetrify(mtmp, TRUE);
      if (mtmp->mhp > 0)
        return 0;
      else
        return 2;
    }

    display_nhwindow(WIN_MESSAGE, FALSE);
    vision_recalc(2); /* hero can't see anything */
    player.uswallow = 1;
    /* player.uswldtim always set > 1 */
    tim_tmp = 25 - (int)mtmp->m_lev;
    if (tim_tmp > 0)
      tim_tmp = rnd(tim_tmp) / 2;
    else if (tim_tmp < 0)
      tim_tmp = -(rnd(-tim_tmp) / 2);
    tim_tmp += -player.uac + 10;
    player.uswldtim = (unsigned)((tim_tmp < 2) ? 2 : tim_tmp);
    swallowed(1);
    for (otmp2 = invent; otmp2; otmp2 = otmp2->nobj)
      (void)snuff_lit(otmp2);
  }

  if (mtmp != player.ustuck)
    return (0);
  if (player.uswldtim > 0)
    player.uswldtim -= 1;

  switch (mattk->adtyp) {
    case AD_DGST:
      if (Slow_digestion) {
        /* Messages are handled below */
        player.uswldtim = 0;
        tmp = 0;
      } else if (player.uswldtim == 0) {
        pline("%s totally digests you!", Monnam(mtmp));
        tmp = player.uhp;
        if (Half_physical_damage)
          tmp *= 2; /* sorry */
      } else {
        pline("%s%s digests you!", Monnam(mtmp),
              (player.uswldtim == 2) ? " thoroughly" : (player.uswldtim == 1)
                                                           ? " utterly"
                                                           : "");
        exercise(A_STR, FALSE);
      }
      break;
    case AD_PHYS:
      if (mtmp->data == &mons[PM_FOG_CLOUD]) {
        You("are laden with moisture and %s",
            flaming(youmonst.data)
                ? "are smoldering out!"
                : Breathless
                      ? "find it mildly uncomfortable."
                      : amphibious(youmonst.data) ? "feel comforted."
                                                  : "can barely breathe!");
        /* NB: Amphibious includes Breathless */
        if (Amphibious && !flaming(youmonst.data))
          tmp = 0;
      } else {
        You("are pummeled with debris!");
        exercise(A_STR, FALSE);
      }
      break;
    case AD_ACID:
      if (Acid_resistance) {
        You("are covered with a seemingly harmless goo.");
        tmp = 0;
      } else {
        if (Hallucination)
          pline("Ouch!  You've been slimed!");
        else
          You("are covered in slime!  It burns!");
        exercise(A_STR, FALSE);
      }
      break;
    case AD_BLND:
      if (can_blnd(mtmp, &youmonst, mattk->aatyp, nullptr)) {
        if (!Blind) {
          You_cant("see in here!");
          make_blinded((long)tmp, FALSE);
          if (!Blind)
            Your(vision_clears);
        } else
          /* keep him blind until disgorged */
          make_blinded(Blinded + 1, FALSE);
      }
      tmp = 0;
      break;
    case AD_ELEC:
      if (!mtmp->mcan && rn2(2)) {
        pline_The("air around you crackles with electricity.");
        if (Shock_resistance) {
          shieldeff(player.ux, player.uy);
          You("seem unhurt.");
          ugolemeffects(AD_ELEC, tmp);
          tmp = 0;
        }
      } else
        tmp = 0;
      break;
    case AD_COLD:
      if (!mtmp->mcan && rn2(2)) {
        if (Cold_resistance) {
          shieldeff(player.ux, player.uy);
          You_feel("mildly chilly.");
          ugolemeffects(AD_COLD, tmp);
          tmp = 0;
        } else
          You("are freezing to death!");
      } else
        tmp = 0;
      break;
    case AD_FIRE:
      if (!mtmp->mcan && rn2(2)) {
        if (Fire_resistance) {
          shieldeff(player.ux, player.uy);
          You_feel("mildly hot.");
          ugolemeffects(AD_FIRE, tmp);
          tmp = 0;
        } else
          You("are burning to a crisp!");
        burn_away_slime();
      } else
        tmp = 0;
      break;
    case AD_DISE:
      if (!diseasemu(mtmp->data))
        tmp = 0;
      break;
    default:
      tmp = 0;
      break;
  }

  if (Half_physical_damage)
    tmp = (tmp + 1) / 2;

  mdamageu(mtmp, tmp);
  if (tmp)
    stop_occupation();

  if (touch_petrifies(youmonst.data) && !resists_ston(mtmp)) {
    pline("%s very hurriedly %s you!", Monnam(mtmp),
          is_animal(mtmp->data) ? "regurgitates" : "expels");
    expels(mtmp, mtmp->data, FALSE);
  } else if (!player.uswldtim || youmonst.data->msize >= MZ_HUGE) {
    You("get %s!", is_animal(mtmp->data) ? "regurgitated" : "expelled");
    if (flags.verbose && (is_animal(mtmp->data) ||
                          (dmgtype(mtmp->data, AD_DGST) && Slow_digestion)))
      pline("Obviously %s doesn't like your taste.", mon_nam(mtmp));
    expels(mtmp, mtmp->data, FALSE);
  }
  return (1);
}

/* monster explodes in your face */
STATIC_OVL int explmu(Monster *mtmp, struct Attack *mattk, bool ufound) {
  if (mtmp->mcan)
    return (0);

  if (!ufound)
    pline("%s explodes at a spot in %s!", canseemon(mtmp) ? Monnam(mtmp) : "It",
          levl[mtmp->mux][mtmp->muy].typ == WATER ? "empty water" : "thin air");
  else {
    int tmp = d((int)mattk->damn, (int)mattk->damd);
    bool not_affected = defends((int)mattk->adtyp, uwep);

    hitmsg(mtmp, mattk);

    switch (mattk->adtyp) {
      case AD_COLD:
        not_affected |= Cold_resistance;
        goto common;
      case AD_FIRE:
        not_affected |= Fire_resistance;
        goto common;
      case AD_ELEC:
        not_affected |= Shock_resistance;
      common:

        if (!not_affected) {
          if (ACURR(A_DEX) > rnd(20)) {
            You("duck some of the blast.");
            tmp = (tmp + 1) / 2;
          } else {
            if (flags.verbose)
              You("get blasted!");
          }
          if (mattk->adtyp == AD_FIRE)
            burn_away_slime();
          if (Half_physical_damage)
            tmp = (tmp + 1) / 2;
          mdamageu(mtmp, tmp);
        }
        break;

      case AD_BLND:
        not_affected = resists_blnd(&youmonst);
        if (!not_affected) {
          /* sometimes you're affected even if it's invisible */
          if (mon_visible(mtmp) || (rnd(tmp /= 2) > player.ulevel)) {
            You("are blinded by a blast of light!");
            make_blinded((long)tmp, FALSE);
            if (!Blind)
              Your(vision_clears);
          } else if (flags.verbose)
            You("get the impression it was not terribly bright.");
        }
        break;

      case AD_HALU:
        not_affected |= Blind || (player.umonnum == PM_BLACK_LIGHT ||
                                  player.umonnum == PM_VIOLET_FUNGUS ||
                                  dmgtype(youmonst.data, AD_STUN));
        if (!not_affected) {
          bool chg;
          if (!Hallucination)
            You("are caught in a blast of kaleidoscopic light!");
          chg = make_hallucinated(HHallucination + (long)tmp, FALSE, 0L);
          You("%s.", chg ? "are freaked out" : "seem unaffected");
        }
        break;

      default:
        break;
    }
    if (not_affected) {
      You("seem unaffected by it.");
      ugolemeffects((int)mattk->adtyp, tmp);
    }
  }
  mondead(mtmp);
  wake_nearto(mtmp->mx, mtmp->my, 7 * 7);
  if (mtmp->mhp > 0)
    return (0);
  return (2); /* it dies */
}

/* monster gazes at you */
int gazemu(Monster *mtmp, struct Attack *mattk) {
  switch (mattk->adtyp) {
    case AD_STON:
      if (mtmp->mcan || !mtmp->mcansee) {
        if (!canseemon(mtmp))
          break; /* silently */
        pline("%s %s.", Monnam(mtmp),
              (mtmp->data == &mons[PM_MEDUSA] && mtmp->mcan)
                  ? "doesn't look all that ugly"
                  : "gazes ineffectually");
        break;
      }
      if (Reflecting && couldsee(mtmp->mx, mtmp->my) &&
          mtmp->data == &mons[PM_MEDUSA]) {
        /* hero has line of sight to Medusa and she's not blind */
        bool useeit = canseemon(mtmp);

        if (useeit)
          (void)ureflects("%s gaze is reflected by your %s.",
                          s_suffix(Monnam(mtmp)));
        if (mon_reflects(mtmp, !useeit
                                   ? nullptr
                                   : "The gaze is reflected away by %s %s!"))
          break;
        if (!m_canseeu(mtmp)) { /* probably you're invisible */
          if (useeit)
            pline("%s doesn't seem to notice that %s gaze was reflected.",
                  Monnam(mtmp), mhis(mtmp));
          break;
        }
        if (useeit)
          pline("%s is turned to stone!", Monnam(mtmp));
        stoned = TRUE;
        killed(mtmp);

        if (mtmp->mhp > 0)
          break;
        return 2;
      }
      if (canseemon(mtmp) && couldsee(mtmp->mx, mtmp->my) &&
          !Stone_resistance) {
        You("meet %s gaze.", s_suffix(mon_nam(mtmp)));
        stop_occupation();
        if (poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))
          break;
        You("turn to stone...");
        killer_format = KILLED_BY;
        killer = mtmp->data->mname;
        done(STONING);
      }
      break;
    case AD_CONF:
      if (!mtmp->mcan && canseemon(mtmp) && couldsee(mtmp->mx, mtmp->my) &&
          mtmp->mcansee && !mtmp->mspec_used && rn2(5)) {
        int conf = d(3, 4);

        mtmp->mspec_used = mtmp->mspec_used + (conf + rn2(6));
        if (!Confusion)
          pline("%s gaze confuses you!", s_suffix(Monnam(mtmp)));
        else
          You("are getting more and more confused.");
        make_confused(HConfusion + conf, FALSE);
        stop_occupation();
      }
      break;
    case AD_STUN:
      if (!mtmp->mcan && canseemon(mtmp) && couldsee(mtmp->mx, mtmp->my) &&
          mtmp->mcansee && !mtmp->mspec_used && rn2(5)) {
        int stun = d(2, 6);

        mtmp->mspec_used = mtmp->mspec_used + (stun + rn2(6));
        pline("%s stares piercingly at you!", Monnam(mtmp));
        make_stunned(HStun + stun, TRUE);
        stop_occupation();
      }
      break;
    case AD_BLND:
      if (!mtmp->mcan && canseemon(mtmp) && !resists_blnd(&youmonst) &&
          distu(mtmp->mx, mtmp->my) <= BOLT_LIM * BOLT_LIM) {
        int blnd = d((int)mattk->damn, (int)mattk->damd);

        You("are blinded by %s radiance!", s_suffix(mon_nam(mtmp)));
        make_blinded((long)blnd, FALSE);
        stop_occupation();
        /* not blind at this point implies you're wearing
           the Eyes of the Overworld; make them block this
           particular stun attack too */
        if (!Blind)
          Your(vision_clears);
        else
          make_stunned((long)d(1, 3), TRUE);
      }
      break;
    case AD_FIRE:
      if (!mtmp->mcan && canseemon(mtmp) && couldsee(mtmp->mx, mtmp->my) &&
          mtmp->mcansee && !mtmp->mspec_used && rn2(5)) {
        int dmg = d(2, 6);

        pline("%s attacks you with a fiery gaze!", Monnam(mtmp));
        stop_occupation();
        if (Fire_resistance) {
          pline_The("fire doesn't feel hot!");
          dmg = 0;
        }
        burn_away_slime();
        if ((int)mtmp->m_lev > rn2(20))
          destroy_item(SCROLL_CLASS, AD_FIRE);
        if ((int)mtmp->m_lev > rn2(20))
          destroy_item(POTION_CLASS, AD_FIRE);
        if ((int)mtmp->m_lev > rn2(25))
          destroy_item(SPBOOK_CLASS, AD_FIRE);
        if (dmg)
          mdamageu(mtmp, dmg);
      }
      break;
#ifdef PM_BEHOLDER /* work in progress */
    case AD_SLEE:
      if (!mtmp->mcan && canseemon(mtmp) && couldsee(mtmp->mx, mtmp->my) &&
          mtmp->mcansee && multi >= 0 && !rn2(5) && !Sleep_resistance) {
        fall_asleep(-rnd(10), TRUE);
        pline("%s gaze makes you very sleepy...", s_suffix(Monnam(mtmp)));
      }
      break;
    case AD_SLOW:
      if (!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee &&
          (HFast & (INTRINSIC | TIMEOUT)) && !defends(AD_SLOW, uwep) && !rn2(4))

        u_slow_down();
      stop_occupation();
      break;
#endif
    default:
      impossible("Gaze attack %d?", mattk->adtyp);
      break;
  }
  return (0);
}


/* mtmp hits you for n points damage */
void mdamageu(Monster *mtmp, int n) {
  flags.botl = 1;
  if (Upolyd) {
    player.mh -= n;
    if (player.mh < 1)
      rehumanize();
  } else {
    player.uhp -= n;
    if (player.uhp < 1)
      done_in_by(mtmp);
  }
}


STATIC_OVL void urustm(Monster *mon, Object *obj) {
  bool vis;
  bool is_acid;

  if (!mon || !obj)
    return; /* just in case */
  if (dmgtype(youmonst.data, AD_CORR))
    is_acid = TRUE;
  else if (dmgtype(youmonst.data, AD_RUST))
    is_acid = FALSE;
  else
    return;

  vis = cansee(mon->mx, mon->my);

  if ((is_acid ? is_corrodeable(obj) : is_rustprone(obj)) &&
      (is_acid ? obj->oeroded2 : obj->oeroded) < MAX_ERODE) {
    if (obj->greased || obj->oerodeproof || (obj->blessed && rn2(3))) {
      if (vis)
        pline("Somehow, %s weapon is not affected.", s_suffix(mon_nam(mon)));
      if (obj->greased && !rn2(2))
        obj->greased = 0;
    } else {
      if (vis)
        pline("%s %s%s!", s_suffix(Monnam(mon)),
              aobjnam(obj, (is_acid ? "corrode" : "rust")),
              (is_acid ? obj->oeroded2 : obj->oeroded) ? " further" : "");
      if (is_acid)
        obj->oeroded2++;
      else
        obj->oeroded++;
    }
  }
}


/* returns 0 if seduction impossible,
 *	   1 if fine,
 *	   2 if wrong gender for nymph */
int could_seduce(Monster *magr, Monster *mdef, struct Attack *mattk) {
  MonsterType *pagr;
  bool agrinvis, defperc;
  xchar genagr, gendef;

  if (is_animal(magr->data))
    return (0);
  if (magr == &youmonst) {
    pagr = youmonst.data;
    agrinvis = (Invis != 0);
    genagr = poly_gender();
  } else {
    pagr = magr->data;
    agrinvis = magr->minvis;
    genagr = gender(magr);
  }
  if (mdef == &youmonst) {
    defperc = (See_invisible != 0);
    gendef = poly_gender();
  } else {
    defperc = perceives(mdef->data);
    gendef = gender(mdef);
  }

  if (agrinvis && !defperc
#ifdef SEDUCE
      && mattk && mattk->adtyp != AD_SSEX
#endif
      )
    return 0;

  if (pagr->mlet != S_NYMPH &&
      ((pagr != &mons[PM_INCUBUS] && pagr != &mons[PM_SUCCUBUS])
#ifdef SEDUCE
       || (mattk && mattk->adtyp != AD_SSEX)
#endif
       ))
    return 0;

  if (genagr == 1 - gendef)
    return 1;
  else
    return (pagr->mlet == S_NYMPH) ? 2 : 0;
}


#ifdef SEDUCE
/* Returns 1 if monster teleported */
int doseduce(Monster *mon) {
  Object *ring, *nring;
  bool fem = (mon->data == &mons[PM_SUCCUBUS]); /* otherwise incubus */
  char qbuf[QBUFSZ];

  if (mon->mcan || mon->mspec_used) {
    pline("%s acts as though %s has got a %sheadache.", Monnam(mon), mhe(mon),
          mon->mcan ? "severe " : "");
    return 0;
  }

  if (unconscious()) {
    pline("%s seems dismayed at your lack of response.", Monnam(mon));
    return 0;
  }

  if (Blind)
    pline("It caresses you...");
  else
    You_feel("very attracted to %s.", mon_nam(mon));

  for (ring = invent; ring; ring = nring) {
    nring = ring->nobj;
    if (ring->otyp != RIN_ADORNMENT)
      continue;
    if (fem) {
      if (rn2(20) < ACURR(A_CHA)) {
        sprintf(qbuf, "\"That %s looks pretty.  May I have it?\"",
                safe_qbuf("", sizeof("\"That  looks pretty.  May I have it?\""),
                          xname(ring), simple_typename(ring->otyp), "ring"));
        makeknown(RIN_ADORNMENT);
        if (yn(qbuf) == 'n')
          continue;
      } else
        pline("%s decides she'd like your %s, and takes it.",
              Blind ? "She" : Monnam(mon), xname(ring));
      makeknown(RIN_ADORNMENT);
      if (ring == uleft || ring == uright)
        Ring_gone(ring);
      if (ring == uwep)
        setuwep(nullptr);
      if (ring == uswapwep)
        setuswapwep(nullptr);
      if (ring == uquiver)
        setuqwep(nullptr);
      freeinv(ring);
      (void)mpickobj(mon, ring);
    } else {
      char buf[BUFSZ];

      if (uleft && uright && uleft->otyp == RIN_ADORNMENT &&
          uright->otyp == RIN_ADORNMENT)
        break;
      if (ring == uleft || ring == uright)
        continue;
      if (rn2(20) < ACURR(A_CHA)) {
        sprintf(
            qbuf, "\"That %s looks pretty.  Would you wear it for me?\"",
            safe_qbuf(
                "",
                sizeof("\"That  looks pretty.  Would you wear it for me?\""),
                xname(ring), simple_typename(ring->otyp), "ring"));
        makeknown(RIN_ADORNMENT);
        if (yn(qbuf) == 'n')
          continue;
      } else {
        pline("%s decides you'd look prettier wearing your %s,",
              Blind ? "He" : Monnam(mon), xname(ring));
        pline("and puts it on your finger.");
      }
      makeknown(RIN_ADORNMENT);
      if (!uright) {
        pline("%s puts %s on your right %s.", Blind ? "He" : Monnam(mon),
              the(xname(ring)), body_part(HAND));
        setworn(ring, RIGHT_RING);
      } else if (!uleft) {
        pline("%s puts %s on your left %s.", Blind ? "He" : Monnam(mon),
              the(xname(ring)), body_part(HAND));
        setworn(ring, LEFT_RING);
      } else if (uright && uright->otyp != RIN_ADORNMENT) {
        strcpy(buf, xname(uright));
        pline("%s replaces your %s with your %s.", Blind ? "He" : Monnam(mon),
              buf, xname(ring));
        Ring_gone(uright);
        setworn(ring, RIGHT_RING);
      } else if (uleft && uleft->otyp != RIN_ADORNMENT) {
        strcpy(buf, xname(uleft));
        pline("%s replaces your %s with your %s.", Blind ? "He" : Monnam(mon),
              buf, xname(ring));
        Ring_gone(uleft);
        setworn(ring, LEFT_RING);
      } else
        impossible("ring replacement");
      Ring_on(ring);
      prinv(nullptr, ring, 0L);
    }
  }

  if (!uarmc && !uarmf && !uarmg && !uarms && !uarmh
#ifdef TOURIST
      && !uarmu
#endif
      )
    pline("%s murmurs sweet nothings into your ear.",
          Blind ? (fem ? "She" : "He") : Monnam(mon));
  else
    pline("%s murmurs in your ear, while helping you undress.",
          Blind ? (fem ? "She" : "He") : Monnam(mon));
  mayberem(uarmc, cloak_simple_name(uarmc));
  if (!uarmc)
    mayberem(uarm, "suit");
  mayberem(uarmf, "boots");
  if (!uwep || !welded(uwep))
    mayberem(uarmg, "gloves");
  mayberem(uarms, "shield");
  mayberem(uarmh, "helmet");
#ifdef TOURIST
  if (!uarmc && !uarm)
    mayberem(uarmu, "shirt");
#endif

  if (uarm || uarmc) {
    verbalize("You're such a %s; I wish...",
              flags.female ? "sweet lady" : "nice guy");
    if (!tele_restrict(mon))
      (void)rloc(mon, FALSE);
    return 1;
  }
  if (player.ualign.type == A_CHAOTIC)
    adjalign(1);

  /* by this point you have discovered mon's identity, blind or not... */
  pline("Time stands still while you and %s lie in each other's arms...",
        noit_mon_nam(mon));
  if (rn2(35) > ACURR(A_CHA) + ACURR(A_INT)) {
    /* Don't bother with mspec_used here... it didn't get tired! */
    pline("%s seems to have enjoyed it more than you...", noit_Monnam(mon));
    switch (rn2(5)) {
      case 0:
        You_feel("drained of energy.");
        player.uen = 0;
        player.uenmax -= rnd(Half_physical_damage ? 5 : 10);
        exercise(A_CON, FALSE);
        if (player.uenmax < 0)
          player.uenmax = 0;
        break;
      case 1:
        You("are down in the dumps.");
        (void)adjattrib(A_CON, -1, TRUE);
        exercise(A_CON, FALSE);
        flags.botl = 1;
        break;
      case 2:
        Your("senses are dulled.");
        (void)adjattrib(A_WIS, -1, TRUE);
        exercise(A_WIS, FALSE);
        flags.botl = 1;
        break;
      case 3:
        if (!resists_drli(&youmonst)) {
          You_feel("out of shape.");
          losexp("overexertion");
        } else {
          You("have a curious feeling...");
        }
        break;
      case 4: {
        int tmp;
        You_feel("exhausted.");
        exercise(A_STR, FALSE);
        tmp = rn1(10, 6);
        if (Half_physical_damage)
          tmp = (tmp + 1) / 2;
        losehp(tmp, "exhaustion", KILLED_BY);
        break;
      }
    }
  } else {
    mon->mspec_used = rnd(100); /* monster is worn out */
    You("seem to have enjoyed it more than %s...", noit_mon_nam(mon));
    switch (rn2(5)) {
      case 0:
        You_feel("raised to your full potential.");
        exercise(A_CON, TRUE);
        player.uen = (player.uenmax += rnd(5));
        break;
      case 1:
        You_feel("good enough to do it again.");
        (void)adjattrib(A_CON, 1, TRUE);
        exercise(A_CON, TRUE);
        flags.botl = 1;
        break;
      case 2:
        You("will always remember %s...", noit_mon_nam(mon));
        (void)adjattrib(A_WIS, 1, TRUE);
        exercise(A_WIS, TRUE);
        flags.botl = 1;
        break;
      case 3:
        pline("That was a very educational experience.");
        pluslvl(FALSE);
        exercise(A_WIS, TRUE);
        break;
      case 4:
        You_feel("restored to health!");
        player.uhp = player.uhpmax;
        if (Upolyd)
          player.mh = player.mhmax;
        exercise(A_STR, TRUE);
        flags.botl = 1;
        break;
    }
  }

  if (mon->mtame) /* don't charge */
    ;
  else if (rn2(20) < ACURR(A_CHA)) {
    pline("%s demands that you pay %s, but you refuse...", noit_Monnam(mon),
          Blind ? (fem ? "her" : "him") : mhim(mon));
  } else if (player.umonnum == PM_LEPRECHAUN)
    pline("%s tries to take your money, but fails...", noit_Monnam(mon));
  else {
#ifndef GOLDOBJ
    long cost;

    if (player.ugold > (long)LARGEST_INT - 10L)
      cost = (long)rnd(LARGEST_INT) + 500L;
    else
      cost = (long)rnd((int)player.ugold + 10) + 500L;
    if (mon->mpeaceful) {
      cost /= 5L;
      if (!cost)
        cost = 1L;
    }
    if (cost > player.ugold)
      cost = player.ugold;
    if (!cost)
      verbalize("It's on the house!");
    else {
      pline("%s takes %ld %s for services rendered!", noit_Monnam(mon), cost,
            currency(cost));
      player.ugold -= cost;
      mon->mgold += cost;
      flags.botl = 1;
    }
#else
    long cost;
    long umoney = money_cnt(invent);

    if (umoney > (long)LARGEST_INT - 10L)
      cost = (long)rnd(LARGEST_INT) + 500L;
    else
      cost = (long)rnd((int)umoney + 10) + 500L;
    if (mon->mpeaceful) {
      cost /= 5L;
      if (!cost)
        cost = 1L;
    }
    if (cost > umoney)
      cost = umoney;
    if (!cost)
      verbalize("It's on the house!");
    else {
      pline("%s takes %ld %s for services rendered!", noit_Monnam(mon), cost,
            currency(cost));
      money2mon(mon, cost);
      flags.botl = 1;
    }
#endif
  }
  if (!rn2(25))
    mon->mcan = 1; /* monster is worn out */
  if (!tele_restrict(mon))
    (void)rloc(mon, FALSE);
  return 1;
}

STATIC_OVL void mayberem(Object *obj, const char *str) {
  char qbuf[QBUFSZ];

  if (!obj || !obj->owornmask)
    return;

  if (rn2(20) < ACURR(A_CHA)) {
    sprintf(qbuf, "\"Shall I remove your %s, %s?\"", str,
            (!rn2(2) ? "lover" : !rn2(2) ? "dear" : "sweetheart"));
    if (yn(qbuf) == 'n')
      return;
  } else {
    char hairbuf[BUFSZ];

    sprintf(hairbuf, "let me run my fingers through your %s", body_part(HAIR));
    verbalize(
        "Take off your %s; %s.", str,
        (obj == uarm)
            ? "let's get a little closer"
            : (obj == uarmc || obj == uarms)
                  ? "it's in the way"
                  : (obj == uarmf)
                        ? "let me rub your feet"
                        : (obj == uarmg) ? "they're too clumsy" :
#ifdef TOURIST
                                         (obj == uarmu) ? "let me massage you" :
#endif
                                                        /* obj == uarmh */
                                             hairbuf);
  }
  remove_worn_item(obj, TRUE);
}
#endif /* SEDUCE */



STATIC_OVL int passiveum(MonsterType *olduasmon, Monster *mtmp,
                         struct Attack *mattk) {
  int i, tmp;

  for (i = 0;; i++) {
    if (i >= NATTK)
      return 1;
    if (olduasmon->mattk[i].aatyp == AT_NONE ||
        olduasmon->mattk[i].aatyp == AT_BOOM)
      break;
  }
  if (olduasmon->mattk[i].damn)
    tmp = d((int)olduasmon->mattk[i].damn, (int)olduasmon->mattk[i].damd);
  else if (olduasmon->mattk[i].damd)
    tmp = d((int)olduasmon->mlevel + 1, (int)olduasmon->mattk[i].damd);
  else
    tmp = 0;

  /* These affect the enemy even if you were "killed" (rehumanized) */
  switch (olduasmon->mattk[i].adtyp) {
    case AD_ACID:
      if (!rn2(2)) {
        pline("%s is splashed by your acid!", Monnam(mtmp));
        if (resists_acid(mtmp)) {
          pline("%s is not affected.", Monnam(mtmp));
          tmp = 0;
        }
      } else
        tmp = 0;
      if (!rn2(30))
        erode_armor(mtmp, TRUE);
      if (!rn2(6))
        erode_obj(mtmp->weapon(), TRUE, TRUE);
      goto assess_dmg;
    case AD_STON: /* cockatrice */
    {
      long protector = attk_protection((int)mattk->aatyp),
           wornitems = mtmp->misc_worn_check;

      /* wielded weapon gives same protection as gloves here */
      if (mtmp->weapon() != 0)
        wornitems |= W_ARMG;

      if (!resists_ston(mtmp) &&
          (protector == 0L ||
           (protector != ~0L && (wornitems & protector) != protector))) {
        if (poly_when_stoned(mtmp->data)) {
          mon_to_stone(mtmp);
          return (1);
        }
        pline("%s turns to stone!", Monnam(mtmp));
        stoned = 1;
        xkilled(mtmp, 0);
        if (mtmp->mhp > 0)
          return 1;
        return 2;
      }
      return 1;
    }
    case AD_ENCH: /* KMH -- remove enchantment (disenchanter) */
      if (otmp) {
        (void)drain_item(otmp);
        /* No message */
      }
      return (1);
    default:
      break;
  }
  if (!Upolyd)
    return 1;

  /* These affect the enemy only if you are still a monster */
  if (rn2(3))
    switch (youmonst.data->mattk[i].adtyp) {
      case AD_PHYS:
        if (youmonst.data->mattk[i].aatyp == AT_BOOM) {
          You("explode!");
          /* KMH, balance patch -- this is okay with unchanging */
          rehumanize();
          goto assess_dmg;
        }
        break;
      case AD_PLYS: /* Floating eye */
        if (tmp > 127)
          tmp = 127;
        if (player.umonnum == PM_FLOATING_EYE) {
          if (!rn2(4))
            tmp = 127;
          if (mtmp->mcansee && haseyes(mtmp->data) && rn2(3) &&
              (perceives(mtmp->data) || !Invis)) {
            if (Blind)
              pline("As a blind %s, you cannot defend yourself.",
                    youmonst.data->mname);
            else {
              if (mon_reflects(mtmp, "Your gaze is reflected by %s %s."))
                return 1;
              pline("%s is frozen by your gaze!", Monnam(mtmp));
              mtmp->mcanmove = 0;
              mtmp->mfrozen = tmp;
              return 3;
            }
          }
        } else { /* gelatinous cube */
          pline("%s is frozen by you.", Monnam(mtmp));
          mtmp->mcanmove = 0;
          mtmp->mfrozen = tmp;
          return 3;
        }
        return 1;
      case AD_COLD: /* Brown mold or blue jelly */
        if (resists_cold(mtmp)) {
          shieldeff(mtmp->mx, mtmp->my);
          pline("%s is mildly chilly.", Monnam(mtmp));
          golemeffects(mtmp, AD_COLD, tmp);
          tmp = 0;
          break;
        }
        pline("%s is suddenly very cold!", Monnam(mtmp));
        player.mh += tmp / 2;
        if (player.mhmax < player.mh)
          player.mhmax = player.mh;
        if (player.mhmax > ((youmonst.data->mlevel + 1) * 8))
          (void)split_mon(&youmonst, mtmp);
        break;
      case AD_STUN: /* Yellow mold */
        if (!mtmp->mstun) {
          mtmp->mstun = 1;
          pline("%s %s.", Monnam(mtmp),
                makeplural(stagger(mtmp->data, "stagger")));
        }
        tmp = 0;
        break;
      case AD_FIRE: /* Red mold */
        if (resists_fire(mtmp)) {
          shieldeff(mtmp->mx, mtmp->my);
          pline("%s is mildly warm.", Monnam(mtmp));
          golemeffects(mtmp, AD_FIRE, tmp);
          tmp = 0;
          break;
        }
        pline("%s is suddenly very hot!", Monnam(mtmp));
        break;
      case AD_ELEC:
        if (resists_elec(mtmp)) {
          shieldeff(mtmp->mx, mtmp->my);
          pline("%s is slightly tingled.", Monnam(mtmp));
          golemeffects(mtmp, AD_ELEC, tmp);
          tmp = 0;
          break;
        }
        pline("%s is jolted with your electricity!", Monnam(mtmp));
        break;
      default:
        tmp = 0;
        break;
    }
  else
    tmp = 0;

assess_dmg:
  if ((mtmp->mhp -= tmp) <= 0) {
    pline("%s dies!", Monnam(mtmp));
    xkilled(mtmp, 0);
    if (mtmp->mhp > 0)
      return 1;
    return 2;
  }
  return 1;
}


#include "core/edog.h"
Monster *cloneu() {
  Monster *mon;
  int mndx = monsndx(youmonst.data);

  if (player.mh <= 1)
    return nullptr;
  if (mvitals[mndx].mvflags & G_EXTINCT)
    return nullptr;
  mon = makemon(youmonst.data, player.ux, player.uy, NO_MINVENT | MM_EDOG);
  mon = christen_monst(mon, plname);
  initedog(mon);
  mon->m_lev = youmonst.data->mlevel;
  mon->mhpmax = player.mhmax;
  mon->mhp = player.mh / 2;
  player.mh -= mon->mhp;
  flags.botl = 1;
  return (mon);
}


/*mhitu.c*/
