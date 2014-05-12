/*	SCCS Id: @(#)mkobj.c	3.4	2002/10/07	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include <string.h>

#include "core/hack.h"
#include "core/wield.h"
#include "core/topten.h"
#include "core/teleport.h"
#include "core/shk.h"
#include "core/rnd.h"
#include "core/pline.h"
#include "core/objnam.h"
#include "core/mon.h"
#include "core/mkobj.h"
#include "core/makemon.h"
#include "core/light.h"
#include "core/invent.h"
#include "core/end.h"
#include "core/eat.h"
#include "core/do_wear.h"
#include "core/do_name.h"
#include "core/do.h"
#include "core/dbridge.h"

#include "core/artifact.h"
#include "core/prop.h"

STATIC_DCL void AddRandomBoxContents(Object *);
STATIC_DCL void UpdateObjectTimerState(Object *, xchar, xchar, int);
STATIC_DCL void RefreshContainerWeight(Object *);
STATIC_DCL Object *SaveMonsterIntoObject(Object *, Monster *);
#ifdef WIZARD
STATIC_DCL const char *GetWhereName(int);
STATIC_DCL void CheckObjectIsCorrectlyContained(Object *, const char *);
#endif

extern Object *thrownobj; /* defined in dothrow.c */

/*#define DEBUG_EFFECTS*/ /* show some messages for debugging */

struct ItemClassProbability {
  int iprob;   /* probability of an item type */
  char iclass; /* item class */
};

const struct ItemClassProbability kMakeObjProbabilities[] = {{10, WEAPON_CLASS},
                                                             {10, ARMOR_CLASS},
                                                             {20, FOOD_CLASS},
                                                             {8, TOOL_CLASS},
                                                             {8, GEM_CLASS},
                                                             {16, POTION_CLASS},
                                                             {16, SCROLL_CLASS},
                                                             {4, SPBOOK_CLASS},
                                                             {4, WAND_CLASS},
                                                             {3, RING_CLASS},
                                                             {1, AMULET_CLASS}};

const struct ItemClassProbability kBoxProbabilities[] = {{18, GEM_CLASS},
                                                         {15, FOOD_CLASS},
                                                         {18, POTION_CLASS},
                                                         {18, SCROLL_CLASS},
                                                         {12, SPBOOK_CLASS},
                                                         {7, COIN_CLASS},
                                                         {6, WAND_CLASS},
                                                         {5, RING_CLASS},
                                                         {1, AMULET_CLASS}};

#ifdef REINCARNATION
const struct ItemClassProbability kRogueProbabilities[] = {{12, WEAPON_CLASS},
                                                           {12, ARMOR_CLASS},
                                                           {22, FOOD_CLASS},
                                                           {22, POTION_CLASS},
                                                           {22, SCROLL_CLASS},
                                                           {5, WAND_CLASS},
                                                           {5, RING_CLASS}};
#endif

const struct ItemClassProbability kHellProbabilities[] = {{20, WEAPON_CLASS},
                                                          {20, ARMOR_CLASS},
                                                          {16, FOOD_CLASS},
                                                          {12, TOOL_CLASS},
                                                          {10, GEM_CLASS},
                                                          {1, POTION_CLASS},
                                                          {1, SCROLL_CLASS},
                                                          {8, WAND_CLASS},
                                                          {8, RING_CLASS},
                                                          {4, AMULET_CLASS}};

Object *MakeRandomObjectAt(char let, int x, int y, bool artif) {
  Object *otmp = MakeRandomObject(let, artif);
  PlaceObject(otmp, x, y);
  return (otmp);
}

Object *MakeSpecificObjectAt(int otyp, int x, int y, bool init, bool artif) {
  Object *otmp = MakeSpecificObject(otyp, init, artif);
  PlaceObject(otmp, x, y);
  return (otmp);
}

char PickRandomClass() {
  const struct ItemClassProbability *iprobs =
#ifdef REINCARNATION
      (Is_rogue_level(&player.uz)) ? kRogueProbabilities :
#endif
                                   Inhell ? kHellProbabilities
                                          : kMakeObjProbabilities;

  for (int tprob = rnd(100); (tprob -= iprobs->iprob) > 0; iprobs++)
    ;
  return iprobs->iclass;
}

Object *MakeRandomObject(char oclass, bool artif) {
  if (oclass == RANDOM_CLASS) {
    oclass = PickRandomClass();
  }

  int i = bases[(int)oclass];
  int prob = rnd(1000);
  while ((prob -= objects[i].oc_prob) > 0)
    i++;

  if (objects[i].oc_class != oclass || !OBJ_NAME(objects[i]))
    panic("probtype error, oclass=%d i=%d", (int)oclass, i);

  return (MakeSpecificObject(i, TRUE, artif));
}

STATIC_OVL void AddRandomBoxContents(Object *box) {
  int n;

  box->cobj = nullptr;

  switch (box->otyp) {
    case ICE_BOX:
      n = 20;
      break;
    case CHEST:
      n = 5;
      break;
    case LARGE_BOX:
      n = 3;
      break;
    case SACK:
    case OILSKIN_SACK:
      /* initial inventory: sack starts out empty */
      if (moves <= 1 && !in_mklev) {
        n = 0;
      } else {
        n = 1;
      }
      break;
    /*else FALLTHRU*/
    case BAG_OF_HOLDING:
      n = 1;
      break;
    default:
      n = 0;
      break;
  }

  for (n = rn2(n + 1); n > 0; n--) {
    Object *otmp;
    if (box->otyp == ICE_BOX) {
      if (!(otmp = MakeSpecificObject(CORPSE, TRUE, TRUE)))
        continue;
      /* Note: setting age to 0 is correct.  Age has a different
       * from usual meaning for objects stored in ice boxes. -KAA
       */
      otmp->age = 0L;
      if (otmp->timed) {
        stop_timer(ROT_CORPSE, (genericptr_t)otmp);
        stop_timer(REVIVE_MON, (genericptr_t)otmp);
      }
    } else {
      int tprob;
      const struct ItemClassProbability *iprobs = kBoxProbabilities;

      for (tprob = rnd(100); (tprob -= iprobs->iprob) > 0; iprobs++)
        ;
      if (!(otmp = MakeRandomObject(iprobs->iclass, TRUE)))
        continue;

      /* handle a couple of special cases */
      if (otmp->oclass == COIN_CLASS) {
        /* 2.5 x level's usual amount; weight adjusted below */
        otmp->quan = (long)(rnd(level_difficulty() + 2) * rnd(75));
        otmp->owt = GetWeight(otmp);
      } else
        while (otmp->otyp == ROCK) {
          otmp->otyp = rnd_class(DILITHIUM_CRYSTAL, LOADSTONE);
          if (otmp->quan > 2L)
            otmp->quan = 1L;
          otmp->owt = GetWeight(otmp);
        }
      if (box->otyp == BAG_OF_HOLDING) {
        if (Is_mbag(otmp)) {
          otmp->otyp = SACK;
          otmp->spe = 0;
          otmp->owt = GetWeight(otmp);
        } else
          while (otmp->otyp == WAN_CANCELLATION)
            otmp->otyp = rnd_class(WAN_LIGHT, WAN_LIGHTNING);
      }
    }
    AddObjectToContainer(box, otmp);
  }
}

/* select a random, common monster type */
int PickRandomMonsterTypeIndex() {
  /* Plan A: get a level-appropriate common monster */
  MonsterType *ptr = rndmonst();
  if (ptr)
    return (monsndx(ptr));

  /* Plan B: get any common monster */
  int i;
  do {
    i = rn1(SPECIAL_PM - LOW_PM, LOW_PM);
    ptr = &mons[i];
  } while ((ptr->geno & G_NOGEN) || (!Inhell && (ptr->geno & G_HELL)));

  return (i);
}

/*
 * Split obj so that it gets size gets reduced by num. The quantity num is
 * put in the object structure delivered by this call.  The returned object
 * has its wornmask cleared and is positioned just following the original
 * in the nobj chain (and nexthere chain when on the floor).
 */
Object *SplitObject(Object *obj, long num) {
  Object *otmp;

  if (obj->cobj || num <= 0L || obj->quan <= num)
    panic("splitobj"); /* can't split containers */
  otmp = newobj(obj->oxlth);
  *otmp = *obj; /* copies whole structure */
  otmp->o_id = flags.ident++;
  if (!otmp->o_id)
    otmp->o_id = flags.ident++; /* ident overflowed */
  otmp->timed = 0;              /* not timed, yet */
  otmp->lamplit = 0;            /* ditto */
  otmp->owornmask = 0L;         /* new object isn't worn */
  obj->quan -= num;
  obj->owt = GetWeight(obj);
  otmp->quan = num;
  otmp->owt = GetWeight(otmp); /* -= obj->owt ? */
  obj->nobj = otmp;
  /* Only set nexthere when on the floor, nexthere is also used */
  /* as a back pointer to the container object when contained. */
  if (obj->where == OBJ_FLOOR)
    obj->nexthere = otmp;
  if (obj->oxlth)
    (void)memcpy((genericptr_t)otmp->oextra, (genericptr_t)obj->oextra,
                 obj->oxlth);
  otmp->objname = obj->objname;
  if (obj->unpaid)
    splitbill(obj, otmp);
  if (obj->timed)
    obj_split_timers(obj, otmp);
  if (obj_sheds_light(obj))
    obj_split_light_source(obj, otmp);
  return otmp;
}

/*
 * Insert otmp right after obj in whatever chain(s) it is on.  Then extract
 * obj from the chain(s).  This function does a literal swap.  It is up to
 * the caller to provide a valid context for the swap.  When done, obj will
 * still exist, but not on any chain.
 *
 * Note:  Don't use use RemoveObjectFromStorage() -- we are doing an in-place
 *swap,
 * not actually moving something.
 */
void ReplaceObject(Object *obj, Object *otmp) {
  otmp->where = obj->where;
  switch (obj->where) {
    case OBJ_FREE:
      /* do nothing */
      break;
    case OBJ_INVENT:
      otmp->nobj = obj->nobj;
      obj->nobj = otmp;
      ExtractObjectFromList(obj, &invent);
      break;
    case OBJ_CONTAINED:
      otmp->nobj = obj->nobj;
      otmp->ocontainer = obj->ocontainer;
      obj->nobj = otmp;
      ExtractObjectFromList(obj, &obj->ocontainer->cobj);
      break;
    case OBJ_MINVENT:
      otmp->nobj = obj->nobj;
      otmp->ocarry = obj->ocarry;
      obj->nobj = otmp;
      ExtractObjectFromList(obj, &obj->ocarry->minvent);
      break;
    case OBJ_FLOOR:
      otmp->nobj = obj->nobj;
      otmp->nexthere = obj->nexthere;
      otmp->ox = obj->ox;
      otmp->oy = obj->oy;
      obj->nobj = otmp;
      obj->nexthere = otmp;
      ExtractObjectFromList(obj, &fobj);
      extract_nexthere(obj, &level.objects[obj->ox][obj->oy]);
      break;
    default:
      panic("replace_object: obj position");
      break;
  }
}

/*
 * Create a dummy duplicate to put on shop bill.  The duplicate exists
 * only in the billobjs chain.  This function is used when a shop object
 * is being altered, and a copy of the original is needed for billing
 * purposes.  For example, when eating, where an interruption will yield
 * an object which is different from what it started out as; the "I x"
 * command needs to display the original object.
 *
 * The caller is responsible for checking otmp->unpaid and
 * costly_spot(player.ux, player.uy).  This function will make otmp no charge.
 *
 * Note that check_unpaid_usage() should be used instead for partial
 * usage of an object.
 */
void CreateBillDummyObject(Object *otmp) {
  Object *dummy;

  if (otmp->unpaid)
    subfrombill(otmp, shop_keeper(*player.ushops));
  dummy = newobj(otmp->oxlth);
  *dummy = *otmp;
  dummy->where = OBJ_FREE;
  dummy->o_id = flags.ident++;
  if (!dummy->o_id)
    dummy->o_id = flags.ident++; /* ident overflowed */
  dummy->timed = 0;
  if (otmp->oxlth)
    (void)memcpy((genericptr_t)dummy->oextra, (genericptr_t)otmp->oextra,
                 otmp->oxlth);
  dummy->objname = otmp->objname;
  if (Is_candle(dummy))
    dummy->lamplit = 0;
  addtobill(dummy, FALSE, TRUE, TRUE);
  otmp->no_charge = 1;
  otmp->unpaid = 0;
  return;
}

static const char dknowns[] = {WAND_CLASS,   RING_CLASS, POTION_CLASS,
                               SCROLL_CLASS, GEM_CLASS,  SPBOOK_CLASS,
                               WEAPON_CLASS, TOOL_CLASS, 0};

Object *MakeSpecificObject(int otyp, bool init, bool artif) {
  int mndx, tryct;
  Object *otmp;
  char let = objects[otyp].oc_class;

  otmp = newobj(0);
  *otmp = zeroobj;
  otmp->age = monstermoves;
  otmp->o_id = flags.ident++;
  if (!otmp->o_id)
    otmp->o_id = flags.ident++; /* ident overflowed */
  otmp->quan = 1L;
  otmp->oclass = let;
  otmp->otyp = otyp;
  otmp->where = OBJ_FREE;
  otmp->dknown = index(dknowns, let) ? 0 : 1;
  if ((otmp->otyp >= ELVEN_SHIELD && otmp->otyp <= ORCISH_SHIELD) ||
      otmp->otyp == SHIELD_OF_REFLECTION)
    otmp->dknown = 0;
  if (!objects[otmp->otyp].oc_uses_known)
    otmp->known = 1;
#ifdef INVISIBLE_OBJECTS
  otmp->oinvis = !rn2(1250);
#endif
  if (init)
    switch (let) {
      case WEAPON_CLASS:
        otmp->quan = is_multigen(otmp) ? (long)rn1(6, 6) : 1L;
        if (!rn2(11)) {
          otmp->spe = rne(3);
          otmp->blessed = rn2(2);
        } else if (!rn2(10)) {
          Curse(otmp);
          otmp->spe = -rne(3);
        } else
          BlessOrCurse(otmp, 10);
        if (is_poisonable(otmp) && !rn2(100))
          otmp->opoisoned = 1;

        if (artif && !rn2(20))
          otmp = mk_artifact(otmp, (aligntyp)A_NONE);
        break;
      case FOOD_CLASS:
        otmp->oeaten = 0;
        switch (otmp->otyp) {
          case CORPSE:
            /* possibly overridden by MakeCorpseOrStatue() */
            tryct = 50;
            do
              otmp->corpsenm = undead_to_corpse(PickRandomMonsterTypeIndex());
            while ((mvitals[otmp->corpsenm].mvflags & G_NOCORPSE) &&
                   (--tryct > 0));
            if (tryct == 0) {
              /* perhaps rndmonnum() only wants to make G_NOCORPSE monsters on
                 this level; let's create an adventurer's corpse instead, then
                 */
              otmp->corpsenm = PM_HUMAN;
            }
            /* timer set below */
            break;
          case EGG:
            otmp->corpsenm = NON_PM; /* generic egg */
            if (!rn2(3))
              for (tryct = 200; tryct > 0; --tryct) {
                mndx = can_be_hatched(PickRandomMonsterTypeIndex());
                if (mndx != NON_PM && !dead_species(mndx, TRUE)) {
                  otmp->corpsenm = mndx; /* typed egg */
                  attach_egg_hatch_timeout(otmp);
                  break;
                }
              }
            break;
          case TIN:
            otmp->corpsenm = NON_PM; /* empty (so far) */
            if (!rn2(6))
              otmp->spe = 1; /* spinach */
            else
              for (tryct = 200; tryct > 0; --tryct) {
                mndx = undead_to_corpse(PickRandomMonsterTypeIndex());
                if (mons[mndx].cnutrit &&
                    !(mvitals[mndx].mvflags & G_NOCORPSE)) {
                  otmp->corpsenm = mndx;
                  break;
                }
              }
            BlessOrCurse(otmp, 10);
            break;
          case SLIME_MOLD:
            otmp->spe = current_fruit;
            break;
          case KELP_FROND:
            otmp->quan = (long)rnd(2);
            break;
        }
        if (otmp->otyp == CORPSE || otmp->otyp == MEAT_RING ||
            otmp->otyp == KELP_FROND)
          break;
      /* fall into next case */

      case GEM_CLASS:
        if (otmp->otyp == LOADSTONE)
          Curse(otmp);
        else if (otmp->otyp == ROCK)
          otmp->quan = (long)rn1(6, 6);
        else if (otmp->otyp != LUCKSTONE && !rn2(6))
          otmp->quan = 2L;
        else
          otmp->quan = 1L;
        break;
      case TOOL_CLASS:
        switch (otmp->otyp) {
          case TALLOW_CANDLE:
          case WAX_CANDLE:
            otmp->spe = 1;
            otmp->age = 20L * /* 400 or 200 */
                        (long)objects[otmp->otyp].oc_cost;
            otmp->lamplit = 0;
            otmp->quan = 1L + (long)(rn2(2) ? rn2(7) : 0);
            BlessOrCurse(otmp, 5);
            break;
          case BRASS_LANTERN:
          case OIL_LAMP:
            otmp->spe = 1;
            otmp->age = (long)rn1(500, 1000);
            otmp->lamplit = 0;
            BlessOrCurse(otmp, 5);
            break;
          case MAGIC_LAMP:
            otmp->spe = 1;
            otmp->lamplit = 0;
            BlessOrCurse(otmp, 2);
            break;
          case CHEST:
          case LARGE_BOX:
            otmp->olocked = !!(rn2(5));
            otmp->otrapped = !(rn2(10));
          case ICE_BOX:
          case SACK:
          case OILSKIN_SACK:
          case BAG_OF_HOLDING:
            AddRandomBoxContents(otmp);
            break;
#ifdef TOURIST
          case EXPENSIVE_CAMERA:
#endif
          case TINNING_KIT:
          case MAGIC_MARKER:
            otmp->spe = rn1(70, 30);
            break;
          case CAN_OF_GREASE:
            otmp->spe = rnd(25);
            BlessOrCurse(otmp, 10);
            break;
          case CRYSTAL_BALL:
            otmp->spe = rnd(5);
            BlessOrCurse(otmp, 2);
            break;
          case HORN_OF_PLENTY:
          case BAG_OF_TRICKS:
            otmp->spe = rnd(20);
            break;
          case FIGURINE: {
            int tryct2 = 0;
            do
              otmp->corpsenm = PickRandomMonsterTypeIndex();
            while (is_human(&mons[otmp->corpsenm]) && tryct2++ < 30);
            BlessOrCurse(otmp, 4);
            break;
          }
          case BELL_OF_OPENING:
            otmp->spe = 3;
            break;
          case MAGIC_FLUTE:
          case MAGIC_HARP:
          case FROST_HORN:
          case FIRE_HORN:
          case DRUM_OF_EARTHQUAKE:
            otmp->spe = rn1(5, 4);
            break;
        }
        break;
      case AMULET_CLASS:
        if (otmp->otyp == AMULET_OF_YENDOR)
          flags.made_amulet = TRUE;
        if (rn2(10) && (otmp->otyp == AMULET_OF_STRANGULATION ||
                        otmp->otyp == AMULET_OF_CHANGE ||
                        otmp->otyp == AMULET_OF_RESTFUL_SLEEP)) {
          Curse(otmp);
        } else
          BlessOrCurse(otmp, 10);
      case VENOM_CLASS:
      case CHAIN_CLASS:
      case BALL_CLASS:
        break;
      case POTION_CLASS:
        if (otmp->otyp == POT_OIL)
          otmp->age = MAX_OIL_IN_FLASK; /* amount of oil */
                                        /* fall through */
      case SCROLL_CLASS:
#ifdef MAIL
        if (otmp->otyp != SCR_MAIL)
#endif
          BlessOrCurse(otmp, 4);
        break;
      case SPBOOK_CLASS:
        BlessOrCurse(otmp, 17);
        break;
      case ARMOR_CLASS:
        if (rn2(10) &&
            (otmp->otyp == FUMBLE_BOOTS || otmp->otyp == LEVITATION_BOOTS ||
             otmp->otyp == HELM_OF_OPPOSITE_ALIGNMENT ||
             otmp->otyp == GAUNTLETS_OF_FUMBLING || !rn2(11))) {
          Curse(otmp);
          otmp->spe = -rne(3);
        } else if (!rn2(10)) {
          otmp->blessed = rn2(2);
          otmp->spe = rne(3);
        } else
          BlessOrCurse(otmp, 10);
        if (artif && !rn2(40))
          otmp = mk_artifact(otmp, (aligntyp)A_NONE);
        /* simulate lacquered armor for samurai */
        if (Role_if(PM_SAMURAI) && otmp->otyp == SPLINT_MAIL &&
            (moves <= 1 || In_quest(&player.uz))) {
#ifdef UNIXPC
          /* optimizer bitfield bug */
          otmp->oerodeproof = 1;
          otmp->rknown = 1;
#else
          otmp->oerodeproof = otmp->rknown = 1;
#endif
        }
        break;
      case WAND_CLASS:
        if (otmp->otyp == WAN_WISHING)
          otmp->spe = rnd(3);
        else
          otmp->spe = rn1(5, (objects[otmp->otyp].oc_dir == NODIR) ? 11 : 4);
        BlessOrCurse(otmp, 17);
        otmp->recharged = 0; /* used to control recharging */
        break;
      case RING_CLASS:
        if (objects[otmp->otyp].oc_charged) {
          BlessOrCurse(otmp, 3);
          if (rn2(10)) {
            if (rn2(10) && GetBUCSign(otmp))
              otmp->spe = GetBUCSign(otmp) * rne(3);
            else
              otmp->spe = rn2(2) ? rne(3) : -rne(3);
          }
          /* make useless +0 rings much less common */
          if (otmp->spe == 0)
            otmp->spe = rn2(4) - rn2(3);
          /* negative rings are usually cursed */
          if (otmp->spe < 0 && rn2(5))
            Curse(otmp);
        } else if (rn2(10) && (otmp->otyp == RIN_TELEPORTATION ||
                               otmp->otyp == RIN_POLYMORPH ||
                               otmp->otyp == RIN_AGGRAVATE_MONSTER ||
                               otmp->otyp == RIN_HUNGER || !rn2(9))) {
          Curse(otmp);
        }
        break;
      case ROCK_CLASS:
        switch (otmp->otyp) {
          case STATUE:
            /* possibly overridden by MakeCorpseOrStatue() */
            otmp->corpsenm = PickRandomMonsterTypeIndex();
            if (!verysmall(&mons[otmp->corpsenm]) &&
                rn2(level_difficulty() / 2 + 10) > 10)
              AddObjectToContainer(otmp, MakeRandomObject(SPBOOK_CLASS, FALSE));
        }
        break;
      case COIN_CLASS:
        break; /* do nothing */
      default:
        impossible("impossible mkobj %d, sym '%c'.", otmp->otyp,
                   objects[otmp->otyp].oc_class);
        return nullptr;
    }

  /* Some things must get done (timers) even if init = 0 */
  switch (otmp->otyp) {
    case CORPSE:
      StartCorpseTimeout(otmp);
      break;
  }

  /* unique objects may have an associated artifact entry */
  if (objects[otyp].oc_unique && !otmp->oartifact)
    otmp = mk_artifact(otmp, (aligntyp)A_NONE);
  otmp->owt = GetWeight(otmp);
  return (otmp);
}

/*
 * Start a corpse decay or revive timer.
 * This takes the age of the corpse into consideration as of 3.4.0.
 */
void StartCorpseTimeout(Object *body) {
  long when;       /* rot away when this old */
  long corpse_age; /* age of corpse          */
  int rot_adjust;
  short action;

#define TAINT_AGE (50L)        /* age when corpses go bad */
#define TROLL_REVIVE_CHANCE 37 /* 1/37 chance for 50 turns ~ 75% chance */
#define ROT_AGE (250L)         /* age when corpses rot away */

  /* lizards and lichen don't rot or revive */
  if (body->corpsenm == PM_LIZARD || body->corpsenm == PM_LICHEN)
    return;

  action = ROT_CORPSE;             /* default action: rot away */
  rot_adjust = in_mklev ? 25 : 10; /* give some variation */
  corpse_age = monstermoves - body->age;
  if (corpse_age > ROT_AGE)
    when = rot_adjust;
  else
    when = ROT_AGE - corpse_age;
  when += (long)(rnz(rot_adjust) - rot_adjust);

  if (is_rider(&mons[body->corpsenm])) {
    /*
     * Riders always revive.  They have a 1/3 chance per turn
     * of reviving after 12 turns.  Always revive by 500.
     */
    action = REVIVE_MON;
    for (when = 12L; when < 500L; when++)
      if (!rn2(3))
        break;

  } else if (mons[body->corpsenm].mlet == S_TROLL && !body->norevive) {
    long age;
    for (age = 2; age <= TAINT_AGE; age++)
      if (!rn2(TROLL_REVIVE_CHANCE)) { /* troll revives */
        action = REVIVE_MON;
        when = age;
        break;
      }
  }

  if (body->norevive)
    body->norevive = 0;
  start_timer(when, TIMER_OBJECT, action, (genericptr_t)body);
}

void Bless(Object *otmp) {
#ifdef GOLDOBJ
  if (otmp->oclass == COIN_CLASS)
    return;
#endif
  otmp->cursed = 0;
  otmp->blessed = 1;
  if (carried(otmp) && confers_luck(otmp))
    set_moreluck();
  else if (otmp->otyp == BAG_OF_HOLDING)
    otmp->owt = GetWeight(otmp);
  else if (otmp->otyp == FIGURINE && otmp->timed)
    stop_timer(FIG_TRANSFORM, (genericptr_t)otmp);
  return;
}

void Unbless(Object *otmp) {
  otmp->blessed = 0;
  if (carried(otmp) && confers_luck(otmp))
    set_moreluck();
  else if (otmp->otyp == BAG_OF_HOLDING)
    otmp->owt = GetWeight(otmp);
}

void Curse(Object *otmp) {
#ifdef GOLDOBJ
  if (otmp->oclass == COIN_CLASS)
    return;
#endif
  otmp->blessed = 0;
  otmp->cursed = 1;
  /* welded two-handed weapon interferes with some armor removal */
  if (otmp == uwep && bimanual(uwep))
    reset_remarm();
  /* rules at top of wield.c state that twoweapon cannot be done
     with cursed alternate weapon */
  if (otmp == uswapwep && player.twoweap)
    drop_uswapwep();
  /* some cursed items need immediate updating */
  if (carried(otmp) && confers_luck(otmp))
    set_moreluck();
  else if (otmp->otyp == BAG_OF_HOLDING)
    otmp->owt = GetWeight(otmp);
  else if (otmp->otyp == FIGURINE) {
    if (otmp->corpsenm != NON_PM && !dead_species(otmp->corpsenm, TRUE) &&
        (carried(otmp) || mcarried(otmp)))
      attach_fig_transform_timeout(otmp);
  }
  return;
}

void Uncurse(Object *otmp) {
  otmp->cursed = 0;
  if (carried(otmp) && confers_luck(otmp))
    set_moreluck();
  else if (otmp->otyp == BAG_OF_HOLDING)
    otmp->owt = GetWeight(otmp);
  else if (otmp->otyp == FIGURINE && otmp->timed)
    stop_timer(FIG_TRANSFORM, (genericptr_t)otmp);
  return;
}

void BlessOrCurse(Object *otmp, int chance) {
  if (otmp->blessed || otmp->cursed)
    return;

  if (!rn2(chance)) {
    if (!rn2(2)) {
      Curse(otmp);
    } else {
      Bless(otmp);
    }
  }
  return;
}

int GetBUCSign(Object *otmp) { return (!!otmp->blessed - !!otmp->cursed); }

/*
 *  Calculate the weight of the given object.  This will recursively follow
 *  and calculate the weight of any containers.
 *
 *  Note:  It is possible to end up with an incorrect weight if some part
 *	   of the code messes with a contained object and doesn't update the
 *	   container's weight.
 */
int GetWeight(Object *obj) {
  int wt = objects[obj->otyp].oc_weight;

  if (obj->otyp == LARGE_BOX && obj->spe == 1) /* Schroedinger's Cat */
    wt += mons[PM_HOUSECAT].cwt;
  if (Is_container(obj) || obj->otyp == STATUE) {
    Object *contents;
    int cwt = 0;

    if (obj->otyp == STATUE && obj->corpsenm >= LOW_PM)
      wt = (int)obj->quan * ((int)mons[obj->corpsenm].cwt * 3 / 2);

    for (contents = obj->cobj; contents; contents = contents->nobj)
      cwt += GetWeight(contents);
    /*
     *  The weight of bags of holding is calculated as the weight
     *  of the bag plus the weight of the bag's contents modified
     *  as follows:
     *
     *	Bag status	Weight of contents
     *	----------	------------------
     *	cursed			2x
     *	blessed			x/4 + 1
     *	otherwise		x/2 + 1
     *
     *  The macro DELTA_CWT in pickup.c also implements these
     *  weight equations.
     *
     *  Note:  The above checks are performed in the given order.
     *	   this means that if an object is both blessed and
     *	   cursed (not supposed to happen), it will be treated
     *	   as cursed.
     */
    if (obj->otyp == BAG_OF_HOLDING)
      cwt = obj->cursed ? (cwt * 2) : (1 + (cwt / (obj->blessed ? 4 : 2)));

    return wt + cwt;
  }
  if (obj->otyp == CORPSE && obj->corpsenm >= LOW_PM) {
    long long_wt = obj->quan * (long)mons[obj->corpsenm].cwt;

    wt = (long_wt > LARGEST_INT) ? LARGEST_INT : (int)long_wt;
    if (obj->oeaten)
      wt = eaten_stat(wt, obj);
    return wt;
  } else if (obj->oclass == FOOD_CLASS && obj->oeaten) {
    return eaten_stat((int)obj->quan * wt, obj);
  } else if (obj->oclass == COIN_CLASS)
    return (int)((obj->quan + 50L) / 100L);
  else if (obj->otyp == HEAVY_IRON_BALL && obj->owt != 0)
    return ((int)(obj->owt)); /* kludge for "very" heavy iron ball */
  return (wt ? wt * (int)obj->quan : ((int)obj->quan + 1) >> 1);
}

static int treefruits[] = {APPLE, ORANGE, PEAR, BANANA, EUCALYPTUS_LEAF};

Object *MakeRandomTreefruitAt(int x, int y) {
  return MakeSpecificObjectAt(treefruits[rn2(SIZE(treefruits))], x, y, TRUE,
                              FALSE);
}

Object *MakeGold(long amount, int x, int y) {
  Object *gold = g_at(x, y);

  if (amount <= 0L)
    amount = (long)(1 + rnd(level_difficulty() + 2) * rnd(30));
  if (gold) {
    gold->quan += amount;
  } else {
    gold = MakeSpecificObjectAt(GOLD_PIECE, x, y, TRUE, FALSE);
    gold->quan = amount;
  }
  gold->owt = GetWeight(gold);
  return (gold);
}

/* return TRUE if the corpse has special timing */
#define special_corpse(num)                                                  \
  (((num) == PM_LIZARD) || ((num) == PM_LICHEN) || (is_rider(&mons[num])) || \
   (mons[num].mlet == S_TROLL))

/*
 * OEXTRA note: Passing mtmp causes mtraits to be saved
 * even if ptr passed as well, but ptr is always used for
 * the corpse type (corpsenm). That allows the corpse type
 * to be different from the original monster,
 *	i.e.  vampire -> human corpse
 * yet still allow restoration of the original monster upon
 * resurrection.
 */
Object *MakeCorpseOrStatue(int objtype, Monster *mtmp, MonsterType *ptr, int x,
                           int y, bool init) {
  Object *otmp;

  if (objtype != CORPSE && objtype != STATUE)
    impossible("making corpstat type %d", objtype);
  if (x == 0 && y == 0) { /* special case - random placement */
    otmp = MakeSpecificObject(objtype, init, FALSE);
    if (otmp)
      rloco(otmp);
  } else
    otmp = MakeSpecificObjectAt(objtype, x, y, init, FALSE);
  if (otmp) {
    if (mtmp) {
      Object *otmp2;

      if (!ptr)
        ptr = mtmp->data;
      /* save_mtraits frees original data pointed to by otmp */
      otmp2 = SaveMonsterIntoObject(otmp, mtmp);
      if (otmp2)
        otmp = otmp2;
    }
    /* use the corpse or statue produced by MakeSpecificObject() as-is
     unless `ptr' is non-null */
    if (ptr) {
      int old_corpsenm = otmp->corpsenm;

      otmp->corpsenm = monsndx(ptr);
      otmp->owt = GetWeight(otmp);
      if (otmp->otyp == CORPSE &&
          (special_corpse(old_corpsenm) || special_corpse(otmp->corpsenm))) {
        obj_stop_timers(otmp);
        StartCorpseTimeout(otmp);
      }
    }
  }
  return (otmp);
}

/*
 * Attach a monster id to an object, to provide
 * a lasting association between the two.
 */
Object *AttachMonsterIdToObject(Object *obj, unsigned mid) {
  Object *otmp;
  int lth, namelth;

  if (!mid || !obj)
    return nullptr;
  lth = sizeof(mid);
  namelth = obj->has_name() ? obj->objname.size(): 0;
  if (namelth)
    otmp = ReallocateExtraObjectSpace(obj, lth, (genericptr_t) & mid, ONAME(obj));
  else {
    otmp = obj;
    otmp->oxlth = sizeof(mid);
    memcpy((genericptr_t)otmp->oextra, (genericptr_t) & mid, sizeof(mid));
  }
  if (otmp && otmp->oxlth)
    otmp->oattached = OATTACHED_M_ID; /* mark it */
  return otmp;
}

static Object *SaveMonsterIntoObject(Object *obj, Monster *mtmp) {
  // Add the monster structure into the object's extra space
  int lth = sizeof(Monster) + mtmp->mxlth + mtmp->mnamelth;
  Object *otmp = ReallocateExtraObjectSpace(obj, lth, (genericptr_t)mtmp,
                                            ONAME(obj));

  // TODO(BNC): Good place for using a buffer?
  if (otmp && otmp->oxlth) {
    Monster *mtmp2 = (Monster *)otmp->oextra;
    if (mtmp->data)
      mtmp2->mnum = monsndx(mtmp->data);
    /* invalidate pointers */
    /* m_id is needed to know if this is a revived quest leader */
    /* but m_id must be cleared when loading bones */
    mtmp2->nmon = nullptr;
    mtmp2->data = nullptr;
    mtmp2->minvent = nullptr;
    otmp->oattached = OATTACHED_MONST; /* mark it */
  }
  return otmp;
}

/* returns a pointer to a new monst structure based on
 * the one contained within the obj.
 */
Monster *NewMonsterFromObject(Object *obj, bool copyof) {
  Monster *mtmp = nullptr;
  Monster *mnew = nullptr;

  if (obj->oxlth && obj->oattached == OATTACHED_MONST)
    mtmp = (Monster *)obj->oextra;
  if (mtmp) {
    if (copyof) {
      int lth = mtmp->mxlth + mtmp->mnamelth;
      mnew = newmonst(lth);
      lth += sizeof(Monster);
      memcpy((genericptr_t)mnew, (genericptr_t)mtmp, lth);
    } else {
      /* Never insert this returned pointer into mon chains! */
      mnew = mtmp;
    }
  }
  return mnew;
}

/* make an object named after someone listed in the scoreboard file */
Object *NewTopTenObject(int objtype, int x, int y) {
  /* player statues never contain books */
  bool initialize_it = (objtype != STATUE);
  Object *otmp = MakeSpecificObjectAt(objtype, x, y, initialize_it, FALSE);
  if (otmp) {
    /* tt_oname will return null if the scoreboard is empty */
    Object *otmp2 = tt_oname(otmp);
    if (otmp2)
      otmp = otmp2;
  }
  return otmp;
}

/* make a new corpse or statue, uninitialized if a statue (i.e. no books) */
Object *MakeNamedCorpseOrStatue(int objtype, MonsterType *ptr, int x, int y,
                                const char *nm) {
  Object *otmp;

  otmp = MakeCorpseOrStatue(objtype, nullptr, ptr, x, y,
                            (bool)(objtype != STATUE));
  if (nm)
    otmp = oname(otmp, nm);
  return (otmp);
}

bool IsFlammable(Object *otmp) {
  int otyp = otmp->otyp;
  int omat = objects[otyp].oc_material;

  if (objects[otyp].oc_oprop == FIRE_RES || otyp == WAN_FIRE)
    return FALSE;

  return ((bool)((omat <= WOOD && omat != LIQUID) || omat == PLASTIC));
}

bool IsRottable(Object *otmp) {
  int otyp = otmp->otyp;

  return ((bool)(objects[otyp].oc_material <= WOOD &&
                 objects[otyp].oc_material != LIQUID));
}

/*
 * These routines maintain the single-linked lists headed in level.objects[][]
 * and threaded through the nexthere fields in the object-instance structure.
 */

/* put the object at the given location */
void PlaceObject(Object *otmp, int x, int y) {
  Object *otmp2 = level.objects[x][y];

  if (otmp->where != OBJ_FREE)
    panic("place_object: obj not free");

  obj_no_longer_held(otmp);
  if (otmp->otyp == BOULDER)
    block_point(x, y); /* vision */

  /* obj goes under boulders */
  if (otmp2 && (otmp2->otyp == BOULDER)) {
    otmp->nexthere = otmp2->nexthere;
    otmp2->nexthere = otmp;
  } else {
    otmp->nexthere = otmp2;
    level.objects[x][y] = otmp;
  }

  /* set the new object's location */
  otmp->ox = x;
  otmp->oy = y;

  otmp->where = OBJ_FLOOR;

  /* add to floor chain */
  otmp->nobj = fobj;
  fobj = otmp;
  if (otmp->timed)
    UpdateObjectTimerState(otmp, x, y, 0);
}

#define ON_ICE(a) ((a)->recharged)
#define ROT_ICE_ADJUSTMENT 2 /* rotting on ice takes 2 times as long */

/* If ice was affecting any objects correct that now
 * Also used for starting ice effects too. [zap.c]
 */
void ApplyIceEffectsAt(int x, int y, bool do_buried) {
  Object *otmp;

  for (otmp = level.objects[x][y]; otmp; otmp = otmp->nexthere) {
    if (otmp->timed)
      UpdateObjectTimerState(otmp, x, y, 0);
  }
  if (do_buried) {
    for (otmp = level.buriedobjlist; otmp; otmp = otmp->nobj) {
      if (otmp->ox == x && otmp->oy == y) {
        if (otmp->timed) {
          UpdateObjectTimerState(otmp, x, y, 0);
        }
      }
    }
  }
}

/*
 * Returns an obj->age for a corpse object on ice, that would be the
 * actual obj->age if the corpse had just been lifted from the ice.
 * This is useful when just using obj->age in a check or calculation because
 * rot timers pertaining to the object don't have to be stopped and
 * restarted etc.
 */
long PeekAtIcedCorpseAge(Object *otmp) {
  long retval = otmp->age;

  if (otmp->otyp == CORPSE && ON_ICE(otmp)) {
    /* Adjust the age; must be same as UpdateObjectTimerState() for off ice*/
    long age = monstermoves - otmp->age;
    retval = otmp->age + (age / ROT_ICE_ADJUSTMENT);
#ifdef DEBUG_EFFECTS
    pline_The("%s age has ice modifications:otmp->age = %ld, returning %ld.",
              s_suffix(doname(otmp)), otmp->age, retval);
    pline("Effective age of corpse: %ld.", monstermoves - retval);
#endif
  }
  return retval;
}

STATIC_OVL void UpdateObjectTimerState(Object *otmp, xchar x, xchar y,
                                       int force) {
  long tleft = 0L;
  short action = ROT_CORPSE;
  bool restart_timer = FALSE;
  bool on_floor = (otmp->where == OBJ_FLOOR);
  bool buried = (otmp->where == OBJ_BURIED);

  /* Check for corpses just placed on or in ice */
  if (otmp->otyp == CORPSE && (on_floor || buried) && is_ice(x, y)) {
    tleft = stop_timer(action, (genericptr_t)otmp);
    if (tleft == 0L) {
      action = REVIVE_MON;
      tleft = stop_timer(action, (genericptr_t)otmp);
    }
    if (tleft != 0L) {
      long age;

      tleft = tleft - monstermoves;
      /* mark the corpse as being on ice */
      ON_ICE(otmp) = 1;
#ifdef DEBUG_EFFECTS
      pline("%s is now on ice at %d,%d.", The(xname(otmp)), x, y);
#endif
      /* Adjust the time remaining */
      tleft *= ROT_ICE_ADJUSTMENT;
      restart_timer = TRUE;
      /* Adjust the age; must be same as in obj_ice_age() */
      age = monstermoves - otmp->age;
      otmp->age = monstermoves - (age * ROT_ICE_ADJUSTMENT);
    }
  }
  /* Check for corpses coming off ice */
  else if ((force < 0) || (otmp->otyp == CORPSE && ON_ICE(otmp) &&
                           ((on_floor && !is_ice(x, y)) || !on_floor))) {
    tleft = stop_timer(action, (genericptr_t)otmp);
    if (tleft == 0L) {
      action = REVIVE_MON;
      tleft = stop_timer(action, (genericptr_t)otmp);
    }
    if (tleft != 0L) {
      long age;

      tleft = tleft - monstermoves;
      ON_ICE(otmp) = 0;
#ifdef DEBUG_EFFECTS
      pline("%s is no longer on ice at %d,%d.", The(xname(otmp)), x, y);
#endif
      /* Adjust the remaining time */
      tleft /= ROT_ICE_ADJUSTMENT;
      restart_timer = TRUE;
      /* Adjust the age */
      age = monstermoves - otmp->age;
      otmp->age = otmp->age + (age / ROT_ICE_ADJUSTMENT);
    }
  }
  /* now re-start the timer with the appropriate modifications */
  if (restart_timer)
    start_timer(tleft, TIMER_OBJECT, action, (genericptr_t)otmp);
}

#undef ON_ICE
#undef ROT_ICE_ADJUSTMENT

void RemoveObjectFromFloor(Object *otmp) {
  xchar x = otmp->ox;
  xchar y = otmp->oy;

  if (otmp->where != OBJ_FLOOR)
    panic("remove_object: obj not on floor");
  if (otmp->otyp == BOULDER)
    unblock_point(x, y); /* vision */
  extract_nexthere(otmp, &level.objects[x][y]);
  ExtractObjectFromList(otmp, &fobj);
  if (otmp->timed)
    UpdateObjectTimerState(otmp, x, y, 0);
}

/* throw away all of a monster's inventory */
void DiscardMonsterInventory(Monster *mtmp) {
  Object *otmp;

  while ((otmp = mtmp->minvent) != 0) {
    RemoveObjectFromStorage(otmp);
    obfree(otmp, nullptr); /* DeallocateObject() isn't sufficient */
  }
}

/*
 * Free obj from whatever list it is on in preperation of deleting it or
 * moving it elsewhere.  This will perform all high-level consequences
 * involved with removing the item.  E.g. if the object is in the hero's
 * inventory and confers heat resistance, the hero will lose it.
 *
 * Object positions:
 *	OBJ_FREE	not on any list
 *	OBJ_FLOOR	fobj, level.locations[][] chains (use remove_object)
 *	OBJ_CONTAINED	cobj chain of container object
 *	OBJ_INVENT	hero's invent chain (use freeinv)
 *	OBJ_MINVENT	monster's invent chain
 *	OBJ_MIGRATING	migrating chain
 *	OBJ_BURIED	level.buriedobjs chain
 *	OBJ_ONBILL	on billobjs chain
 */
void RemoveObjectFromStorage(Object *obj) {
  switch (obj->where) {
    case OBJ_FREE:
      break;
    case OBJ_FLOOR:
      RemoveObjectFromFloor(obj);
      break;
    case OBJ_CONTAINED:
      ExtractObjectFromList(obj, &obj->ocontainer->cobj);
      RefreshContainerWeight(obj->ocontainer);
      break;
    case OBJ_INVENT:
      freeinv(obj);
      break;
    case OBJ_MINVENT:
      ExtractObjectFromList(obj, &obj->ocarry->minvent);
      break;
    case OBJ_MIGRATING:
      ExtractObjectFromList(obj, &migrating_objs);
      break;
    case OBJ_BURIED:
      ExtractObjectFromList(obj, &level.buriedobjlist);
      break;
    case OBJ_ONBILL:
      ExtractObjectFromList(obj, &billobjs);
      break;
    default:
      panic("obj_extract_self");
      break;
  }
}

/* Extract the given object from the chain, following nobj chain. */
void ExtractObjectFromList(Object *obj, Object **head_ptr) {
  Object *prev;

  Object *curr = *head_ptr;
  for (prev = nullptr; curr; prev = curr, curr = curr->nobj) {
    if (curr == obj) {
      if (prev)
        prev->nobj = curr->nobj;
      else
        *head_ptr = curr->nobj;
      break;
    }
  }
  if (!curr)
    panic("extract_nobj: object lost");
  obj->where = OBJ_FREE;
}

/*
 * Extract the given object from the chain, following nexthere chain.
 *
 * This does not set obj->where, this function is expected to be called
 * in tandem with extract_nobj, which does set it.
 */
void extract_nexthere(Object *obj, Object **head_ptr) {
  Object *curr, *prev;

  curr = *head_ptr;
  for (prev = nullptr; curr; prev = curr, curr = curr->nexthere) {
    if (curr == obj) {
      if (prev)
        prev->nexthere = curr->nexthere;
      else
        *head_ptr = curr->nexthere;
      break;
    }
  }
  if (!curr)
    panic("extract_nexthere: object lost");
}

/*
 * Add obj to mon's inventory.  If obj is able to merge with something already
 * in the inventory, then the passed obj is deleted and 1 is returned.
 * Otherwise 0 is returned.
 */
int AddObjectToMonsterInventory(Monster *mon, Object *obj) {
  Object *otmp;

  if (obj->where != OBJ_FREE)
    panic("add_to_minv: obj not free");

  /* merge if possible */
  for (otmp = mon->minvent; otmp; otmp = otmp->nobj)
    if (merged(&otmp, &obj))
      return 1; /* obj merged and then free'd */
  /* else insert; don't bother forcing it to end of chain */
  obj->where = OBJ_MINVENT;
  obj->ocarry = mon;
  obj->nobj = mon->minvent;
  mon->minvent = obj;
  return 0; /* obj on mon's inventory chain */
}

/*
 * Add obj to container, make sure obj is "free".  Returns (merged) obj.
 * The input obj may be deleted in the process.
 */
Object *AddObjectToContainer(Object *container, Object *obj) {
  Object *otmp;

  if (obj->where != OBJ_FREE)
    panic("add_to_container: obj not free");
  if (container->where != OBJ_INVENT && container->where != OBJ_MINVENT)
    obj_no_longer_held(obj);

  /* merge if possible */
  for (otmp = container->cobj; otmp; otmp = otmp->nobj)
    if (merged(&otmp, &obj))
      return (otmp);

  obj->where = OBJ_CONTAINED;
  obj->ocontainer = container;
  obj->nobj = container->cobj;
  container->cobj = obj;
  return (obj);
}

void AddObjectToMigrationList(Object *obj) {
  if (obj->where != OBJ_FREE)
    panic("add_to_migration: obj not free");

  obj->where = OBJ_MIGRATING;
  obj->nobj = migrating_objs;
  migrating_objs = obj;
}

void AddToBuriedList(Object *obj) {
  if (obj->where != OBJ_FREE)
    panic("add_to_buried: obj not free");

  obj->where = OBJ_BURIED;
  obj->nobj = level.buriedobjlist;
  level.buriedobjlist = obj;
}

/* Recalculate the weight of this container and all of _its_ containers. */
STATIC_OVL void RefreshContainerWeight(Object *container) {
  container->owt = GetWeight(container);
  if (container->where == OBJ_CONTAINED)
    RefreshContainerWeight(container->ocontainer);
  /*
   else if (container->where == OBJ_INVENT)
   recalculate load delay here ???
   */
}

/*
 * Deallocate the object.  _All_ objects should be run through here for
 * them to be deallocated.
 */
void DeallocateObject(Object *obj) {
  if (obj->where != OBJ_FREE)
    panic("dealloc_obj: obj not free");

  /* free up any timers attached to the object */
  if (obj->timed)
    obj_stop_timers(obj);

  /*
   * Free up any light sources attached to the object.
   *
   * We may want to just call del_light_source() without any
   * checks (requires a code change there).  Otherwise this
   * list must track all objects that can have a light source
   * attached to it (and also requires lamplit to be set).
   */
  if (obj_sheds_light(obj))
    del_light_source(LS_OBJECT, (genericptr_t)obj);

  if (obj == thrownobj)
    thrownobj = nullptr;

  free((genericptr_t)obj);
}

#ifdef WIZARD
/* Check all object lists for consistency. */
void SanityCheckObjects() {
  int x, y;
  Object *obj;
  Monster *mon;
  const char *mesg;
  char obj_address[20], mon_address[20]; /* room for formatted pointers */

  mesg = "fobj sanity";
  for (obj = fobj; obj; obj = obj->nobj) {
    if (obj->where != OBJ_FLOOR) {
      pline("%s obj %s %s@(%d,%d): %s\n", mesg,
            fmt_ptr((genericptr_t)obj, obj_address), GetWhereName(obj->where),
            obj->ox, obj->oy, doname(obj));
    }
    CheckObjectIsCorrectlyContained(obj, mesg);
  }

  mesg = "location sanity";
  for (int x = 0; x < COLNO; x++)
    for (int y = 0; y < ROWNO; y++)
      for (obj = level.objects[x][y]; obj; obj = obj->nexthere)
        if (obj->where != OBJ_FLOOR) {
          pline("%s obj %s %s@(%d,%d): %s\n", mesg,
                fmt_ptr((genericptr_t)obj, obj_address),
                GetWhereName(obj->where), obj->ox, obj->oy, doname(obj));
        }

  mesg = "invent sanity";
  for (obj = invent; obj; obj = obj->nobj) {
    if (obj->where != OBJ_INVENT) {
      pline("%s obj %s %s: %s\n", mesg, fmt_ptr((genericptr_t)obj, obj_address),
            GetWhereName(obj->where), doname(obj));
    }
    CheckObjectIsCorrectlyContained(obj, mesg);
  }

  mesg = "migrating sanity";
  for (obj = migrating_objs; obj; obj = obj->nobj) {
    if (obj->where != OBJ_MIGRATING) {
      pline("%s obj %s %s: %s\n", mesg, fmt_ptr((genericptr_t)obj, obj_address),
            GetWhereName(obj->where), doname(obj));
    }
    CheckObjectIsCorrectlyContained(obj, mesg);
  }

  mesg = "buried sanity";
  for (obj = level.buriedobjlist; obj; obj = obj->nobj) {
    if (obj->where != OBJ_BURIED) {
      pline("%s obj %s %s: %s\n", mesg, fmt_ptr((genericptr_t)obj, obj_address),
            GetWhereName(obj->where), doname(obj));
    }
    CheckObjectIsCorrectlyContained(obj, mesg);
  }

  mesg = "bill sanity";
  for (obj = billobjs; obj; obj = obj->nobj) {
    if (obj->where != OBJ_ONBILL) {
      pline("%s obj %s %s: %s\n", mesg, fmt_ptr((genericptr_t)obj, obj_address),
            GetWhereName(obj->where), doname(obj));
    }
    /* shouldn't be a full container on the bill */
    if (obj->cobj) {
      pline("%s obj %s contains %s! %s\n", mesg,
            fmt_ptr((genericptr_t)obj, obj_address), something, doname(obj));
    }
  }

  mesg = "minvent sanity";
  for (mon = fmon; mon; mon = mon->nmon)
    for (obj = mon->minvent; obj; obj = obj->nobj) {
      if (obj->where != OBJ_MINVENT) {
        pline("%s obj %s %s: %s\n", mesg,
              fmt_ptr((genericptr_t)obj, obj_address), GetWhereName(obj->where),
              doname(obj));
      }
      if (obj->ocarry != mon) {
        pline("%s obj %s (%s) not held by mon %s (%s)\n", mesg,
              fmt_ptr((genericptr_t)obj, obj_address), doname(obj),
              fmt_ptr((genericptr_t)mon, mon_address), mon_nam(mon));
      }
      CheckObjectIsCorrectlyContained(obj, mesg);
    }
}

/* This must stay consistent with the defines in obj.h. */
static const char *obj_state_names[NOBJ_STATES] = {
    "free",    "floor",     "contained", "invent",
    "minvent", "migrating", "buried",    "onbill"};

STATIC_OVL const char *GetWhereName(int where) {
  return (where < 0 || where >= NOBJ_STATES) ? "unknown"
                                             : obj_state_names[where];
}

/* obj sanity check: check objs contained by container */
STATIC_OVL void CheckObjectIsCorrectlyContained(Object *container,
                                                const char *mesg) {
  Object *obj;
  char obj1_address[20], obj2_address[20];

  for (obj = container->cobj; obj; obj = obj->nobj) {
    if (obj->where != OBJ_CONTAINED)
      pline("contained %s obj %s: %s\n", mesg,
            fmt_ptr((genericptr_t)obj, obj1_address), GetWhereName(obj->where));
    else if (obj->ocontainer != container)
      pline("%s obj %s not in container %s\n", mesg,
            fmt_ptr((genericptr_t)obj, obj1_address),
            fmt_ptr((genericptr_t)container, obj2_address));
  }
}
#endif /* WIZARD */

/*mkobj.c*/
