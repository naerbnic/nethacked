/*	SCCS Id: @(#)save.c	3.4	2003/11/14	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "json_spirit.h"

#include "hack.h"
#include "mon.h"
#include "mkobj.h"
#include "mkmaze.h"
#include "light.h"
#include "invent.h"
#include "files.h"
#include "end.h"
#include "eat.h"
#include "cmd.h"

#include "allmain.h"
#include "artifact.h"
#include "lev.h"
#include "quest.h"

#ifndef NO_SIGNAL
#include <signal.h>
#endif
#if !defined(LSC) && !defined(O_WRONLY) && !defined(AZTEC_C)
#include <fcntl.h>
#endif

#ifdef ZEROCOMP
STATIC_DCL void bputc(int);
#endif
STATIC_DCL void savelevchn(int, int);
STATIC_DCL void savedamage(int, int);
STATIC_DCL void saveobjchn(int, Object *, int);
STATIC_DCL void savemonchn(int, Monster *, int);
STATIC_DCL void savetrapchn(int, Trap *, int);
STATIC_DCL void savegamestate(int, int);
#ifdef GCC_WARN
static long nulls[10];
#else
#define nulls nul
#endif

#define HUP if (!program_state.done_hup)

#ifdef MENU_COLOR
extern struct menucoloring *menu_colorings;
#endif

/* need to preserve these during save to avoid accessing freed memory */
static unsigned ustuck_id = 0, usteed_id = 0;

int dosave() {
  clear_nhwindow(WIN_MESSAGE);
  if (yn("Really save?") == 'n') {
    clear_nhwindow(WIN_MESSAGE);
    if (multi > 0)
      nomul(0, 0);
  } else {
    clear_nhwindow(WIN_MESSAGE);
    pline("Saving...");
    program_state.done_hup = 0;
    if (dosave0()) {
      program_state.something_worth_saving = 0;
      player.uhp = -1; /* universal game's over indicator */
      /* make sure they see the Saving message */
      display_nhwindow(WIN_MESSAGE, TRUE);
      exit_nhwindows("Be seeing you...");
      terminate(EXIT_SUCCESS);
    } else
      (void)doredraw();
  }
  return 0;
}

/*ARGSUSED*/
/* called as signal() handler, so sent at least one arg */
void hangup(int sig_unused) {
#ifdef NOSAVEONHANGUP
  (void)signal(SIGINT, SIG_IGN);
  clearlocks();
  terminate(EXIT_FAILURE);
#else /* SAVEONHANGUP */
  if (!program_state.done_hup++) {
    if (program_state.something_worth_saving)
      (void)dosave0();
    {
      clearlocks();
      terminate(EXIT_FAILURE);
    }
  }
#endif
  return;
}

/* returns 1 if save successful */
int dosave0() {
  const char *fq_save;
  int fd, ofd;
  xchar ltmp;
  d_level uz_save;
  char whynot[BUFSZ];

  if (!SAVEF[0])
    return 0;
  fq_save = fqname(SAVEF, SAVEPREFIX, 1); /* level files take 0 */

  (void)signal(SIGHUP, SIG_IGN);
#ifndef NO_SIGNAL
  (void)signal(SIGINT, SIG_IGN);
#endif

  HUP if (iflags.window_inited) {
    uncompress(fq_save);
    fd = open_savefile();
    if (fd > 0) {
      (void)close(fd);
      clear_nhwindow(WIN_MESSAGE);
      There("seems to be an old save file.");
      if (yn("Overwrite the old file?") == 'n') {
        compress(fq_save);
        return 0;
      }
    }
  }

  HUP mark_synch(); /* flush any buffered screen output */

  fd = create_savefile();
  if (fd < 0) {
    HUP pline("Cannot open save file.");
    (void)delete_savefile(); /* ab@unido */
    return (0);
  }

  vision_recalc(2); /* shut down vision to prevent problems
                       in the event of an impossible() call */

  /* undo date-dependent luck adjustments made at startup time */
  if (flags.moonphase == FULL_MOON) /* ut-sally!fletcher */
    change_luck(-1);                /* and unido!ab */
  if (flags.friday13)
    change_luck(1);
  if (iflags.window_inited)
    HUP clear_nhwindow(WIN_MESSAGE);

  store_version(fd);
#ifdef STORE_PLNAME_IN_FILE
  bwrite(fd, (genericptr_t)plname, PL_NSIZ);
#endif
  ustuck_id = (player.ustuck ? player.ustuck->m_id : 0);
#ifdef STEED
  usteed_id = (player.usteed ? player.usteed->m_id : 0);
#endif

  savelev(fd, ledger_no(&player.uz), WRITE_SAVE | FREE_SAVE);
  savegamestate(fd, WRITE_SAVE | FREE_SAVE);

  /* While copying level files around, zero out player.uz to keep
   * parts of the restore code from completely initializing all
   * in-core data structures, since all we're doing is copying.
   * This also avoids at least one nasty core dump.
   */
  uz_save = player.uz;
  player.uz.dnum = player.uz.dlevel = 0;
  /* these pointers are no longer valid, and at least player.usteed
   * may mislead place_monster() on other levels
   */
  player.ustuck = nullptr;
#ifdef STEED
  player.usteed = nullptr;
#endif

  for (ltmp = (xchar)1; ltmp <= maxledgerno(); ltmp++) {
    if (ltmp == ledger_no(&uz_save))
      continue;
    if (!(level_info[ltmp].flags & LFILE_EXISTS))
      continue;
    ofd = open_levelfile(ltmp, whynot);
    if (ofd < 0) {
      HUP pline("%s", whynot);
      (void)close(fd);
      (void)delete_savefile();
      HUP killer = whynot;
      HUP done(TRICKED);
      return (0);
    }
    minit(); /* ZEROCOMP */
    getlev(ofd, hackpid, ltmp, FALSE);
    (void)close(ofd);
    bwrite(fd, (genericptr_t) & ltmp, sizeof ltmp); /* level number*/
    savelev(fd, ltmp, WRITE_SAVE | FREE_SAVE);      /* actual level*/
    delete_levelfile(ltmp);
  }
  bclose(fd);

  player.uz = uz_save;

  /* get rid of current level --jgm */
  delete_levelfile(ledger_no(&player.uz));
  delete_levelfile(0);
  compress(fq_save);
  return (1);
}

STATIC_OVL void savegamestate(int fd, int mode) {
  int uid;
#if defined(RECORD_REALTIME) || defined(REALTIME_ON_BOTL)
  time_t realtime;
#endif

  uid = getuid();
  bwrite(fd, (genericptr_t) & uid, sizeof uid);
  bwrite(fd, (genericptr_t) & flags, sizeof(struct flag));
  bwrite(fd, (genericptr_t) & player, sizeof(struct Player));

  /* must come before migrating_objs and migrating_mons are freed */
  save_timers(fd, mode, RANGE_GLOBAL);
  save_light_sources(fd, mode, RANGE_GLOBAL);

  saveobjchn(fd, invent, mode);
  saveobjchn(fd, migrating_objs, mode);
  savemonchn(fd, migrating_mons, mode);
  if (release_data(mode)) {
    invent = 0;
    migrating_objs = 0;
    migrating_mons = 0;
  }
  bwrite(fd, (genericptr_t)mvitals, sizeof(mvitals));

  save_dungeon(fd, (bool)!!perform_bwrite(mode), (bool)!!release_data(mode));
  savelevchn(fd, mode);
  bwrite(fd, (genericptr_t) & moves, sizeof moves);
  bwrite(fd, (genericptr_t) & monstermoves, sizeof monstermoves);
  bwrite(fd, (genericptr_t) & quest_status, sizeof(struct q_score));
  bwrite(fd, (genericptr_t)spl_book, sizeof(struct spell) * (MAXSPELL + 1));
  save_artifacts(fd);
  save_oracles(fd, mode);
  if (ustuck_id)
    bwrite(fd, (genericptr_t) & ustuck_id, sizeof ustuck_id);
#ifdef STEED
  if (usteed_id)
    bwrite(fd, (genericptr_t) & usteed_id, sizeof usteed_id);
#endif
  bwrite(fd, (genericptr_t)pl_character, sizeof pl_character);
  bwrite(fd, (genericptr_t)pl_fruit, sizeof pl_fruit);
  bwrite(fd, (genericptr_t) & current_fruit, sizeof current_fruit);
  savefruitchn(fd, mode);
  savenames(fd, mode);
  save_waterlevel(fd, mode);

#ifdef RECORD_ACHIEVE
  bwrite(fd, (genericptr_t) & achieve, sizeof achieve);
#endif
#if defined(RECORD_REALTIME) || defined(REALTIME_ON_BOTL)
  realtime = get_realtime();
  bwrite(fd, (genericptr_t) & realtime, sizeof realtime);
#endif

  bflush(fd);
}

#ifdef INSURANCE
void savestateinlock() {
  int fd, hpid;
  static bool havestate = TRUE;
  char whynot[BUFSZ];

  /* When checkpointing is on, the full state needs to be written
   * on each checkpoint.  When checkpointing is off, only the pid
   * needs to be in the level.0 file, so it does not need to be
   * constantly rewritten.  When checkpointing is turned off during
   * a game, however, the file has to be rewritten once to truncate
   * it and avoid restoring from outdated information.
   *
   * Restricting havestate to this routine means that an additional
   * noop pid rewriting will take place on the first "checkpoint" after
   * the game is started or restored, if checkpointing is off.
   */
  if (flags.ins_chkpt || havestate) {
    /* save the rest of the current game state in the lock file,
     * following the original int pid, the current level number,
     * and the current savefile name, which should not be subject
     * to any internal compression schemes since they must be
     * readable by an external utility
     */
    fd = open_levelfile(0, whynot);
    if (fd < 0) {
      pline("%s", whynot);
      pline("Probably someone removed it.");
      killer = whynot;
      done(TRICKED);
      return;
    }

    (void)read(fd, (genericptr_t) & hpid, sizeof(hpid));
    if (hackpid != hpid) {
      sprintf(whynot, "Level #0 pid (%d) doesn't match ours (%d)!", hpid,
              hackpid);
      pline("%s", whynot);
      killer = whynot;
      done(TRICKED);
    }
    (void)close(fd);

    fd = create_levelfile(0, whynot);
    if (fd < 0) {
      pline("%s", whynot);
      killer = whynot;
      done(TRICKED);
      return;
    }
    (void)write(fd, (genericptr_t) & hackpid, sizeof(hackpid));
    if (flags.ins_chkpt) {
      int currlev = ledger_no(&player.uz);

      (void)write(fd, (genericptr_t) & currlev, sizeof(currlev));
      save_savefile_name(fd);
      store_version(fd);
#ifdef STORE_PLNAME_IN_FILE
      bwrite(fd, (genericptr_t)plname, PL_NSIZ);
#endif
      ustuck_id = (player.ustuck ? player.ustuck->m_id : 0);
#ifdef STEED
      usteed_id = (player.usteed ? player.usteed->m_id : 0);
#endif
      savegamestate(fd, WRITE_SAVE);
    }
    bclose(fd);
  }
  havestate = flags.ins_chkpt;
}
#endif

void savelev(int fd, xchar lev, int mode) {

  /* if we're tearing down the current level without saving anything
     (which happens upon entrance to the endgame or after an aborted
     restore attempt) then we don't want to do any actual I/O */
  if (mode == FREE_SAVE)
    goto skip_lots;
  if (iflags.purge_monsters) {
    /* purge any dead monsters (necessary if we're starting
     * a panic save rather than a normal one, or sometimes
     * when changing levels without taking time -- e.g.
     * create statue trap then immediately level teleport) */
    dmonsfree();
  }

  if (fd < 0)
    panic("Save on bad file!"); /* impossible */
  if (lev >= 0 && lev <= maxledgerno())
    level_info[lev].flags |= VISITED;
  bwrite(fd, (genericptr_t) & hackpid, sizeof(hackpid));
  bwrite(fd, (genericptr_t) & lev, sizeof(lev));
#ifdef RLECOMP
  {
    /* perform run-length encoding of rm structs */
    struct rm *prm, *rgrm;
    int x, y;
    uchar match;

    rgrm = &levl[0][0]; /* start matching at first rm */
    match = 0;

    for (y = 0; y < ROWNO; y++) {
      for (x = 0; x < COLNO; x++) {
        prm = &levl[x][y];
        if (prm->glyph == rgrm->glyph && prm->typ == rgrm->typ &&
            prm->seenv == rgrm->seenv && prm->horizontal == rgrm->horizontal &&
            prm->flags == rgrm->flags && prm->lit == rgrm->lit &&
            prm->waslit == rgrm->waslit && prm->roomno == rgrm->roomno &&
            prm->edge == rgrm->edge) {
          match++;
          if (match > 254) {
            match = 254; /* undo this match */
            goto writeout;
          }
        } else {
        /* the run has been broken,
         * write out run-length encoding */
        writeout:
          bwrite(fd, (genericptr_t) & match, sizeof(uchar));
          bwrite(fd, (genericptr_t)rgrm, sizeof(struct rm));
          /* start encoding again. we have at least 1 rm
           * in the next run, viz. this one. */
          match = 1;
          rgrm = prm;
        }
      }
    }
    if (match > 0) {
      bwrite(fd, (genericptr_t) & match, sizeof(uchar));
      bwrite(fd, (genericptr_t)rgrm, sizeof(struct rm));
    }
  }
#else
  bwrite(fd, (genericptr_t)levl, sizeof(levl));
#endif /* RLECOMP */

  bwrite(fd, (genericptr_t) & monstermoves, sizeof(monstermoves));
  bwrite(fd, (genericptr_t) & upstair, sizeof(stairway));
  bwrite(fd, (genericptr_t) & dnstair, sizeof(stairway));
  bwrite(fd, (genericptr_t) & upladder, sizeof(stairway));
  bwrite(fd, (genericptr_t) & dnladder, sizeof(stairway));
  bwrite(fd, (genericptr_t) & sstairs, sizeof(stairway));
  bwrite(fd, (genericptr_t) & updest, sizeof(dest_area));
  bwrite(fd, (genericptr_t) & dndest, sizeof(dest_area));
  bwrite(fd, (genericptr_t) & level.flags, sizeof(level.flags));
  bwrite(fd, (genericptr_t)doors, sizeof(doors));
  save_rooms(fd); /* no dynamic memory to reclaim */

/* from here on out, saving also involves allocated memory cleanup */
skip_lots:
  /* must be saved before mons, objs, and buried objs */
  save_timers(fd, mode, RANGE_LEVEL);
  save_light_sources(fd, mode, RANGE_LEVEL);

  savemonchn(fd, fmon, mode);
  save_worm(fd, mode); /* save worm information */
  savetrapchn(fd, ftrap, mode);
  saveobjchn(fd, fobj, mode);
  saveobjchn(fd, level.buriedobjlist, mode);
  saveobjchn(fd, billobjs, mode);
  if (release_data(mode)) {
    fmon = 0;
    ftrap = 0;
    fobj = 0;
    level.buriedobjlist = 0;
    billobjs = 0;
  }
  save_engravings(fd, mode);
  savedamage(fd, mode);
  save_regions(fd, mode);
  if (mode != FREE_SAVE)
    bflush(fd);
}

#ifdef ZEROCOMP
/* The runs of zero-run compression are flushed after the game state or a
 * level is written out.  This adds a couple bytes to a save file, where
 * the runs could be mashed together, but it allows gluing together game
 * state and level files to form a save file, and it means the flushing
 * does not need to be specifically called for every other time a level
 * file is written out.
 */

#define RLESC '\0' /* Leading character for run of LRESC's */
#define flushoutrun(ln) (bputc(RLESC), bputc(ln), ln = -1)

#ifndef ZEROCOMP_BUFSIZ
#define ZEROCOMP_BUFSIZ BUFSZ
#endif
static unsigned char outbuf[ZEROCOMP_BUFSIZ];
static unsigned short outbufp = 0;
static short outrunlength = -1;
static int bwritefd;
static bool compressing = FALSE;

/*dbg()
{
    HUP printf("outbufp %d outrunlength %d\n", outbufp,outrunlength);
}*/

STATIC_OVL void bputc(int c) {
  if (outbufp >= sizeof outbuf) {
    (void)write(bwritefd, outbuf, sizeof outbuf);
    outbufp = 0;
  }
  outbuf[outbufp++] = (unsigned char)c;
}

/*ARGSUSED*/
void bufon(int fd) {
  compressing = TRUE;
  return;
}

/*ARGSUSED*/
void bufoff(int fd) {
  if (outbufp) {
    outbufp = 0;
    panic("closing file with buffered data still unwritten");
  }
  outrunlength = -1;
  compressing = FALSE;
  return;
}

/* flush run and buffer */
void bflush(int fd) {
  bwritefd = fd;
  if (outrunlength >= 0) { /* flush run */
    flushoutrun(outrunlength);
  }

  if (outbufp) {
    if (write(fd, outbuf, outbufp) != outbufp) {
      if (program_state.done_hup)
        terminate(EXIT_FAILURE);
      else
        bclose(fd); /* panic (outbufp != 0) */
    }
    outbufp = 0;
  }
}

void bwrite(int fd, genericptr_t loc, unsigned num) {
  unsigned char *bp = (unsigned char *)loc;

  if (!compressing) {
    if ((unsigned)write(fd, loc, num) != num) {
      if (program_state.done_hup)
        terminate(EXIT_FAILURE);
      else
        panic("cannot write %u bytes to file #%d", num, fd);
    }
  } else {
    bwritefd = fd;
    for (; num; num--, bp++) {
      if (*bp == RLESC) { /* One more char in run */
        if (++outrunlength == 0xFF) {
          flushoutrun(outrunlength);
        }
      } else {                   /* end of run */
        if (outrunlength >= 0) { /* flush run */
          flushoutrun(outrunlength);
        }
        bputc(*bp);
      }
    }
  }
}

void bclose(int fd) {
  bufoff(fd);
  (void)close(fd);
  return;
}

#else /* ZEROCOMP */

static int bw_fd = -1;
static FILE *bw_FILE = 0;
static bool buffering = FALSE;

void bufon(int fd) {
  if (bw_fd >= 0)
    panic("double buffering unexpected");
  bw_fd = fd;
  if ((bw_FILE = fdopen(fd, "w")) == 0)
    panic("buffering of file %d failed", fd);
  buffering = TRUE;
}

void bufoff(int fd) {
  bflush(fd);
  buffering = FALSE;
}

void bflush(int fd) {
  if (fd == bw_fd) {
    if (fflush(bw_FILE) == EOF)
      panic("flush of savefile failed!");
  }
  return;
}

void bwrite(int fd, void const* loc, unsigned num) {
  bool failed;

  if (num == 0) {
    return;
  }

  if (buffering) {
    if (fd != bw_fd)
      panic("unbuffered write to fd %d (!= %d)", fd, bw_fd);

    failed = (fwrite(loc, (int)num, 1, bw_FILE) != 1);
  } else {
/* lint wants the 3rd arg of write to be an int; lint -p an unsigned */
#if defined(BSD) || defined(ULTRIX)
    failed = (write(fd, loc, (int)num) != (int)num);
#else /* e.g. SYSV, __TURBOC__ */
    failed = (write(fd, loc, num) != num);
#endif
  }

  if (failed) {
    if (program_state.done_hup)
      terminate(EXIT_FAILURE);
    else
      panic("cannot write %u bytes to file #%d", num, fd);
  }
}

void bclose(int fd) {
  bufoff(fd);
  if (fd == bw_fd) {
    (void)fclose(bw_FILE);
    bw_fd = -1;
    bw_FILE = 0;
  } else
    (void)close(fd);
  return;
}
#endif /* ZEROCOMP */

STATIC_OVL void savelevchn(int fd, int mode) {
  s_level *tmplev, *tmplev2;
  int cnt = 0;

  for (tmplev = sp_levchn; tmplev; tmplev = tmplev->next)
    cnt++;
  if (perform_bwrite(mode))
    bwrite(fd, (genericptr_t) & cnt, sizeof(int));

  for (tmplev = sp_levchn; tmplev; tmplev = tmplev2) {
    tmplev2 = tmplev->next;
    if (perform_bwrite(mode))
      bwrite(fd, (genericptr_t)tmplev, sizeof(s_level));
    if (release_data(mode))
      free((genericptr_t)tmplev);
  }
  if (release_data(mode))
    sp_levchn = 0;
}

STATIC_OVL void savedamage(int fd, int mode) {
  struct damage *damageptr, *tmp_dam;
  unsigned int xl = 0;

  damageptr = level.damagelist;
  for (tmp_dam = damageptr; tmp_dam; tmp_dam = tmp_dam->next)
    xl++;
  if (perform_bwrite(mode))
    bwrite(fd, (genericptr_t) & xl, sizeof(xl));

  while (xl--) {
    if (perform_bwrite(mode))
      bwrite(fd, (genericptr_t)damageptr, sizeof(*damageptr));
    tmp_dam = damageptr;
    damageptr = damageptr->next;
    if (release_data(mode))
      free((genericptr_t)tmp_dam);
  }
  if (release_data(mode))
    level.damagelist = 0;
}

STATIC_OVL void saveobjchn(int fd, Object *otmp, int mode) {
  Object *otmp2;
  unsigned int xl;
  int minusone = -1;

  while (otmp) {
    otmp2 = otmp->nobj;
    if (perform_bwrite(mode)) {
      // Since we're using C++ strings in Objects, we will do the following
      // instead.
      // Write the extra length
      // Write the object contents with the extra length (the objname field
      // will be garbage data)
      // Write the objname length
      // Write the contents of objname.
      //FIXME: This will break horribly
      xl = otmp->oxlth;
      bwrite(fd, (genericptr_t) & xl, sizeof(int));
      bwrite(fd, (genericptr_t)otmp, xl + sizeof(Object));
      int namelen = otmp->objname.size();
      bwrite(fd, (genericptr_t)&namelen, sizeof(int));
      bwrite(fd, (genericptr_t)otmp->objname.data(), namelen);
    }
    if (Has_contents(otmp))
      saveobjchn(fd, otmp->cobj, mode);
    if (release_data(mode)) {
      if (otmp->oclass == FOOD_CLASS)
        food_disappears(otmp);
      if (otmp->oclass == SPBOOK_CLASS)
        book_disappears(otmp);
      otmp->where = OBJ_FREE; /* set to free so dealloc will work */
      otmp->timed = 0;        /* not timed any more */
      otmp->lamplit = 0;      /* caller handled lights */
      DeallocateObject(otmp);
    }
    otmp = otmp2;
  }
  if (perform_bwrite(mode))
    bwrite(fd, (genericptr_t) & minusone, sizeof(int));
}

STATIC_OVL void savemonchn(int fd, Monster *mtmp, int mode) {
  Monster *mtmp2;
  unsigned int xl;
  int minusone = -1;
  MonsterType *monbegin = &mons[0];

  if (perform_bwrite(mode))
    bwrite(fd, (genericptr_t) & monbegin, sizeof(monbegin));

  while (mtmp) {
    mtmp2 = mtmp->nmon;
    if (perform_bwrite(mode)) {
      xl = mtmp->mxlth + mtmp->mnamelth;
      bwrite(fd, (genericptr_t) & xl, sizeof(int));
      bwrite(fd, (genericptr_t)mtmp, xl + sizeof(Monster));
    }
    if (mtmp->minvent)
      saveobjchn(fd, mtmp->minvent, mode);
    if (release_data(mode))
      dealloc_monst(mtmp);
    mtmp = mtmp2;
  }
  if (perform_bwrite(mode))
    bwrite(fd, (genericptr_t) & minusone, sizeof(int));
}

STATIC_OVL void savetrapchn(int fd, Trap *trap, int mode) {
  Trap *trap2;

  while (trap) {
    trap2 = trap->ntrap;
    if (perform_bwrite(mode))
      bwrite(fd, (genericptr_t)trap, sizeof(Trap));
    if (release_data(mode))
      dealloc_trap(trap);
    trap = trap2;
  }
  if (perform_bwrite(mode))
    bwrite(fd, (genericptr_t)nulls, sizeof(Trap));
}

/* save all the fruit names and ID's; this is used only in saving whole games
 * (not levels) and in saving bones levels.  When saving a bones level,
 * we only want to save the fruits which exist on the bones level; the bones
 * level routine marks nonexistent fruits by making the fid negative.
 */
void savefruitchn(int fd, int mode) {
  struct fruit *f2, *f1;

  f1 = ffruit;
  while (f1) {
    f2 = f1->nextf;
    if (f1->fid >= 0 && perform_bwrite(mode))
      bwrite(fd, (genericptr_t)f1, sizeof(struct fruit));
    if (release_data(mode))
      dealloc_fruit(f1);
    f1 = f2;
  }
  if (perform_bwrite(mode))
    bwrite(fd, (genericptr_t)nulls, sizeof(struct fruit));
  if (release_data(mode))
    ffruit = 0;
}

/* also called by prscore(); this probably belongs in dungeon.c... */
void free_dungeons() {
#ifdef FREE_ALL_MEMORY
  savelevchn(0, FREE_SAVE);
  save_dungeon(0, FALSE, TRUE);
#endif
  return;
}

#ifdef MENU_COLOR
void free_menu_coloring() {
  struct menucoloring *tmp = menu_colorings;

  while (tmp) {
    struct menucoloring *tmp2 = tmp->next;
#ifdef MENU_COLOR_REGEX
    (void)regfree(&tmp->match);
#else
    free(tmp->match);
#endif
    free(tmp);
    tmp = tmp2;
  }
}
#endif /* MENU_COLOR */

void freedynamicdata() {
  unload_qtlist();
  free_invbuf(); /* let_to_name (invent.c) */
  free_youbuf(); /* You_buf,&c (pline.c) */
#ifdef MENU_COLOR
  free_menu_coloring();
#endif
  msgpline_free();
  tmp_at(DISP_FREEMEM, 0); /* temporary display effects */
#ifdef FREE_ALL_MEMORY
#define freeobjchn(X) (saveobjchn(0, X, FREE_SAVE), X = 0)
#define freemonchn(X) (savemonchn(0, X, FREE_SAVE), X = 0)
#define freetrapchn(X) (savetrapchn(0, X, FREE_SAVE), X = 0)
#define freefruitchn() savefruitchn(0, FREE_SAVE)
#define freenames() savenames(0, FREE_SAVE)
#define free_oracles() save_oracles(0, FREE_SAVE)
#define free_waterlevel() save_waterlevel(0, FREE_SAVE)
#define free_worm() save_worm(0, FREE_SAVE)
#define free_timers(R) save_timers(0, FREE_SAVE, R)
#define free_light_sources(R) save_light_sources(0, FREE_SAVE, R);
#define free_engravings() save_engravings(0, FREE_SAVE)
#define freedamage() savedamage(0, FREE_SAVE)
#define free_animals() mon_animal_list(FALSE)

  /* move-specific data */
  dmonsfree(); /* release dead monsters */

  /* level-specific data */
  free_timers(RANGE_LEVEL);
  free_light_sources(RANGE_LEVEL);
  freemonchn(fmon);
  free_worm(); /* release worm segment information */
  freetrapchn(ftrap);
  freeobjchn(fobj);
  freeobjchn(level.buriedobjlist);
  freeobjchn(billobjs);
  free_engravings();
  freedamage();

  /* game-state data */
  free_timers(RANGE_GLOBAL);
  free_light_sources(RANGE_GLOBAL);
  freeobjchn(invent);
  freeobjchn(migrating_objs);
  freemonchn(migrating_mons);
  freemonchn(mydogs); /* ascension or dungeon escape */
                      /* freelevchn();	[folded into free_dungeons()] */
  free_animals();
  free_oracles();
  freefruitchn();
  freenames();
  free_waterlevel();
  free_dungeons();

  /* some pointers in iflags */
  if (iflags.wc_font_map)
    free(iflags.wc_font_map);
  if (iflags.wc_font_message)
    free(iflags.wc_font_message);
  if (iflags.wc_font_text)
    free(iflags.wc_font_text);
  if (iflags.wc_font_menu)
    free(iflags.wc_font_menu);
  if (iflags.wc_font_status)
    free(iflags.wc_font_status);
  if (iflags.wc_tile_file)
    free(iflags.wc_tile_file);
#ifdef AUTOPICKUP_EXCEPTIONS
  free_autopickup_exceptions();
#endif

#endif /* FREE_ALL_MEMORY */
  return;
}

/*save.c*/
