/*	SCCS Id: @(#)bones.c	3.4	2003/09/06	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985,1993. */
/* NetHack may be freely redistributed.  See license for details. */

#include "core/bones.h"

#include <string.h>

#include "core/hack.h"
#include "core/decl.h"
#include "core/worn.h"
#include "core/steed.h"
#include "core/save.h"
#include "core/rnd.h"
#include "core/restore.h"
#include "core/questpgr.h"
#include "core/pline.h"
#include "core/objnam.h"
#include "core/mon.h"
#include "core/mkobj.h"
#include "core/makemon.h"
#include "core/light.h"
#include "core/invent.h"
#include "core/files.h"
#include "core/dog.h"
#include "core/do_name.h"
#include "core/do.h"
#include "core/cmd.h"

#include "core/apply.h"
#include "core/artifact.h"
#include "core/lev.h"

extern char bones[]; /* from files.c */

STATIC_DCL bool no_bones_level(d_level *);
STATIC_DCL void goodfruit(int);
STATIC_DCL void resetobjs(Object *, bool);
STATIC_DCL void drop_upon_death(Monster *, Object *);

STATIC_OVL bool no_bones_level(d_level *lev) {
  extern d_level save_dlevel; /* in do.c */
  s_level *sptr;

  if (ledger_no(&save_dlevel))
    assign_level(lev, &save_dlevel);

  return (bool)(((sptr = Is_special(lev)) != 0 && !sptr->boneid) ||
                !dungeons[lev->dnum].boneid
                 /* no bones on the last or multiway branch levels */
                 /* in any dungeon (level 1 isn't multiway).       */
                ||
                Is_botlevel(lev) || (Is_branchlev(lev) && lev->dlevel > 1)
                /* no bones in the invocation level               */
                ||
                (In_hell(lev) && lev->dlevel == dunlevs_in_dungeon(lev) - 1));
}

/* Call this function for each fruit object saved in the bones level: it marks
 * that particular type of fruit as existing (the marker is that that type's
 * ID is positive instead of negative).  This way, when we later save the
 * chain of fruit types, we know to only save the types that exist.
 */
STATIC_OVL void goodfruit(int id) {
  struct fruit *f;

  for (f = ffruit; f; f = f->nextf) {
    if (f->fid == -id) {
      f->fid = id;
      return;
    }
  }
}

STATIC_OVL void resetobjs(Object *ochain, bool restore) {
  Object *otmp;

  for (otmp = ochain; otmp; otmp = otmp->nobj) {
    if (otmp->cobj)
      resetobjs(otmp->cobj, restore);

    if (((otmp->otyp != CORPSE || otmp->corpsenm < SPECIAL_PM) &&
         otmp->otyp != STATUE) &&
        (!otmp->oartifact ||
         (restore && (exist_artifact(otmp->otyp, ONAME(otmp)) ||
                      is_quest_artifact(otmp))))) {
      otmp->oartifact = 0;
      otmp->objname = "";
    } else if (otmp->oartifact && restore)
      artifact_exists(otmp, ONAME(otmp), TRUE);
    if (!restore) {
      /* do not zero out o_ids for ghost levels anymore */

      if (objects[otmp->otyp].oc_uses_known)
        otmp->known = 0;
      otmp->dknown = otmp->bknown = 0;
      otmp->rknown = 0;
      otmp->invlet = 0;
      otmp->no_charge = 0;

      if (otmp->otyp == SLIME_MOLD)
        goodfruit(otmp->spe);
#ifdef MAIL
      else if (otmp->otyp == SCR_MAIL)
        otmp->spe = 1;
#endif
      else if (otmp->otyp == EGG)
        otmp->spe = 0;
      else if (otmp->otyp == TIN) {
        /* make tins of unique monster's meat be empty */
        if (otmp->corpsenm >= LOW_PM && (mons[otmp->corpsenm].geno & G_UNIQ))
          otmp->corpsenm = NON_PM;
      } else if (otmp->otyp == AMULET_OF_YENDOR) {
        /* no longer the real Amulet */
        otmp->otyp = FAKE_AMULET_OF_YENDOR;
        Curse(otmp);
      } else if (otmp->otyp == CANDELABRUM_OF_INVOCATION) {
        if (otmp->lamplit)
          end_burn(otmp, TRUE);
        otmp->otyp = WAX_CANDLE;
        otmp->age = 50L; /* assume used */
        if (otmp->spe > 0)
          otmp->quan = (long)otmp->spe;
        otmp->spe = 0;
        otmp->owt = GetWeight(otmp);
        Curse(otmp);
      } else if (otmp->otyp == BELL_OF_OPENING) {
        otmp->otyp = BELL;
        Curse(otmp);
      } else if (otmp->otyp == SPE_BOOK_OF_THE_DEAD) {
        otmp->otyp = SPE_BLANK_PAPER;
        Curse(otmp);
      }
    }
  }
}

STATIC_OVL void drop_upon_death(Monster *mtmp, Object *cont) {
  Object *otmp;

  uswapwep = 0; /* ensure Curse() won't cause swapwep to drop twice */
  while ((otmp = invent) != 0) {
    RemoveObjectFromStorage(otmp);
    obj_no_longer_held(otmp);

    otmp->owornmask = 0;
    /* lamps don't go out when dropped */
    if ((cont || artifact_light(otmp)) && obj_is_burning(otmp))
      end_burn(otmp, TRUE); /* smother in statue */

    if (otmp->otyp == SLIME_MOLD)
      goodfruit(otmp->spe);

    if (rn2(5))
      Curse(otmp);
    if (mtmp)
      (void)AddObjectToMonsterInventory(mtmp, otmp);
    else if (cont)
      (void)AddObjectToContainer(cont, otmp);
    else
      PlaceObject(otmp, player.ux, player.uy);
  }
#ifndef GOLDOBJ
  if (player.ugold) {
    long ugold = player.ugold;
    if (mtmp)
      mtmp->mgold = ugold;
    else if (cont)
      (void)AddObjectToContainer(cont, mkgoldobj(ugold));
    else
      (void)MakeGold(ugold, player.ux, player.uy);
    player.ugold = ugold; /* undo mkgoldobj()'s removal */
  }
#endif
  if (cont)
    cont->owt = GetWeight(cont);
}

/* check whether bones are feasible */
bool can_make_bones() {
  Trap *ttmp;

  if (ledger_no(&player.uz) <= 0 || ledger_no(&player.uz) > maxledgerno())
    return FALSE;
  if (no_bones_level(&player.uz))
    return FALSE; /* no bones for specific levels */
  if (player.uswallow) {
    return FALSE; /* no bones when swallowed */
  }
  if (!Is_branchlev(&player.uz)) {
    /* no bones on non-branches with portals */
    for (ttmp = ftrap; ttmp; ttmp = ttmp->ntrap)
      if (ttmp->ttyp == MAGIC_PORTAL)
        return FALSE;
  }

  if (depth(&player.uz) <= 0 ||           /* bulletproofing for endgame */
      (!rn2(1 + (depth(&player.uz) >> 2)) /* fewer ghosts on low levels */
#ifdef WIZARD
       && !wizard
#endif
       ))
    return FALSE;
  /* don't let multiple restarts generate multiple copies of objects
   * in bones files */
  if (discover)
    return FALSE;
  return TRUE;
}

/* save bones and possessions of a deceased adventurer */
void savebones(Object *corpse) {
  int fd, x, y;
  Trap *ttmp;
  Monster *mtmp;
  MonsterType *mptr;
  struct fruit *f;
  char c, *bonesid;
  char whynot[BUFSZ];

  /* caller has already checked `can_make_bones()' */

  clear_bypasses();
  fd = open_bonesfile(&player.uz, &bonesid);
  if (fd >= 0) {
    (void)close(fd);
    compress_bonesfile();
#ifdef WIZARD
    if (wizard) {
      if (yn("Bones file already exists.  Replace it?") == 'y') {
        if (delete_bonesfile(&player.uz))
          goto make_bones;
        else
          pline("Cannot unlink old bones.");
      }
    }
#endif
    return;
  }

#ifdef WIZARD
make_bones:
#endif
  unleash_all();
  /* in case these characters are not in their home bases */
  for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
    if (mtmp->dead())
      continue;
    mptr = mtmp->data;
    if (mtmp->iswiz || mptr == &mons[PM_MEDUSA] || mptr->msound == MS_NEMESIS ||
        mptr->msound == MS_LEADER || mptr == &mons[PM_VLAD_THE_IMPALER])
      mongone(mtmp);
  }
#ifdef STEED
  if (player.usteed)
    dismount_steed(DISMOUNT_BONES);
#endif
  dmonsfree(); /* discard dead or gone monsters */

  /* mark all fruits as nonexistent; when we come to them we'll mark
   * them as existing (using goodfruit())
   */
  for (f = ffruit; f; f = f->nextf)
    f->fid = -f->fid;

  /* check iron balls separately--maybe they're not carrying it */
  if (uball)
    uball->owornmask = uchain->owornmask = 0;

  /* dispose of your possessions, usually cursed */
  if (player.ugrave_arise == (NON_PM - 1)) {
    Object *otmp;

    /* embed your possessions in your statue */
    otmp = MakeNamedCorpseOrStatue(STATUE, &mons[player.umonnum], player.ux,
                                   player.uy, plname);

    drop_upon_death(nullptr, otmp);
    if (!otmp)
      return; /* couldn't make statue */
    mtmp = nullptr;
  } else if (player.ugrave_arise < LOW_PM) {
    /* drop everything */
    drop_upon_death(nullptr, nullptr);
    /* trick makemon() into allowing monster creation
     * on your location
     */
    in_mklev = TRUE;
    mtmp = makemon(&mons[PM_GHOST], player.ux, player.uy, MM_NONAME);
    in_mklev = FALSE;
    if (!mtmp)
      return;
    mtmp = christen_monst(mtmp, plname);
    if (corpse)
      (void)AttachMonsterIdToObject(corpse, mtmp->m_id);
  } else {
    /* give your possessions to the monster you become */
    in_mklev = TRUE;
    mtmp =
        makemon(&mons[player.ugrave_arise], player.ux, player.uy, NO_MM_FLAGS);
    in_mklev = FALSE;
    if (!mtmp) {
      drop_upon_death(nullptr, nullptr);
      return;
    }
    mtmp = christen_monst(mtmp, plname);
    newsym(player.ux, player.uy);
    Your("body rises from the dead as %s...",
         an(mons[player.ugrave_arise].mname));
    display_nhwindow(WIN_MESSAGE, FALSE);
    drop_upon_death(mtmp, nullptr);
    m_dowear(mtmp, TRUE);
  }
  if (mtmp) {
    mtmp->m_lev = (player.ulevel ? player.ulevel : 1);
    mtmp->mhp = mtmp->mhpmax = player.uhpmax;
    mtmp->female = flags.female;
    mtmp->msleeping = 1;
  }
  for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
    resetobjs(mtmp->minvent, FALSE);
    /* do not zero out m_ids for bones levels any more */
    mtmp->mlstmv = 0L;
    if (mtmp->mtame)
      mtmp->mtame = mtmp->mpeaceful = 0;
  }
  for (ttmp = ftrap; ttmp; ttmp = ttmp->ntrap) {
    ttmp->madeby_u = 0;
    ttmp->tseen = (ttmp->ttyp == HOLE);
  }
  resetobjs(fobj, FALSE);
  resetobjs(level.buriedobjlist, FALSE);

  /* Hero is no longer on the map. */
  player.ux = player.uy = 0;

  /* Clear all memory from the level. */
  for (x = 0; x < COLNO; x++)
    for (y = 0; y < ROWNO; y++) {
      levl[x][y].seenv = 0;
      levl[x][y].waslit = 0;
      levl[x][y].glyph = cmap_to_glyph(S_stone);
    }

  fd = create_bonesfile(&player.uz, &bonesid, whynot);
  if (fd < 0) {
#ifdef WIZARD
    if (wizard)
      pline("%s", whynot);
#endif
    /* bones file creation problems are silent to the player.
     * Keep it that way, but place a clue into the paniclog.
     */
    paniclog("savebones", whynot);
    return;
  }
  c = (char)(strlen(bonesid) + 1);

  store_version(fd);
  bwrite(fd, (genericptr_t) & c, sizeof c);
  bwrite(fd, (genericptr_t)bonesid, (unsigned)c); /* DD.nnn */
  savefruitchn(fd, WRITE_SAVE | FREE_SAVE);
  update_mlstmv(); /* update monsters for eventual restoration */
  savelev(fd, ledger_no(&player.uz), WRITE_SAVE | FREE_SAVE);
  bclose(fd);
  commit_bonesfile(&player.uz);
  compress_bonesfile();
}

int getbones() {
  int fd;
  int ok;
  char c, *bonesid, oldbonesid[10];

  if (discover) /* save bones files for real games */
    return (0);

  /* wizard check added by GAN 02/05/87 */
  if (rn2(3) /* only once in three times do we find bones */
#ifdef WIZARD
      && !wizard
#endif
      )
    return (0);
  if (no_bones_level(&player.uz))
    return (0);
  fd = open_bonesfile(&player.uz, &bonesid);
  if (fd < 0)
    return (0);

  if ((ok = uptodate(fd, bones)) == 0) {
#ifdef WIZARD
    if (!wizard)
#endif
      pline("Discarding unuseable bones; no need to panic...");
  } else {
#ifdef WIZARD
    if (wizard) {
      if (yn("Get bones?") == 'n') {
        (void)close(fd);
        compress_bonesfile();
        return (0);
      }
    }
#endif
    mread(fd, (genericptr_t) & c, sizeof c);          /* length incl. '\0' */
    mread(fd, (genericptr_t)oldbonesid, (unsigned)c); /* DD.nnn */
    if (strcmp(bonesid, oldbonesid) != 0) {
      char errbuf[BUFSZ];

      sprintf(errbuf, "This is bones level '%s', not '%s'!", oldbonesid,
              bonesid);
#ifdef WIZARD
      if (wizard) {
        pline("%s", errbuf);
        ok = FALSE; /* won't die of trickery */
      }
#endif
      trickery(errbuf);
    } else {
      Monster *mtmp;

      getlev(fd, 0, 0, TRUE);

      /* Note that getlev() now keeps tabs on unique
       * monsters such as demon lords, and tracks the
       * birth counts of all species just as makemon()
       * does.  If a bones monster is extinct or has been
       * subject to genocide, their mhpmax will be
       * set to the magic DEFUNCT_MONSTER cookie value.
       */
      for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
        if (mtmp->mhpmax == DEFUNCT_MONSTER) {
#if defined(DEBUG) && defined(WIZARD)
          if (wizard)
            pline("Removing defunct monster %s from bones.", mtmp->data->mname);
#endif
          mongone(mtmp);
        } else
          /* to correctly reset named artifacts on the level */
          resetobjs(mtmp->minvent, TRUE);
      }
      resetobjs(fobj, TRUE);
      resetobjs(level.buriedobjlist, TRUE);
    }
  }
  (void)close(fd);

#ifdef WIZARD
  if (wizard) {
    if (yn("Unlink bones?") == 'n') {
      compress_bonesfile();
      return (ok);
    }
  }
#endif
  if (!delete_bonesfile(&player.uz)) {
    /* When N games try to simultaneously restore the same
     * bones file, N-1 of them will fail to delete it
     * (the first N-1 under AmigaDOS, the last N-1 under UNIX).
     * So no point in a mysterious message for a normal event
     * -- just generate a new level for those N-1 games.
     */
    /* pline("Cannot unlink bones."); */
    return (0);
  }
  return (ok);
}

/*bones.c*/
