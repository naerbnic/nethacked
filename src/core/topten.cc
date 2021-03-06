/*	SCCS Id: @(#)topten.c	3.4	2000/01/21	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include <string.h>

#include "core/hack.h"
#include "core/save.h"
#include "core/role.h"
#include "core/rnd.h"
#include "core/pline.h"
#include "core/objnam.h"
#include "core/mkobj.h"
#include "core/hacklib.h"
#include "core/files.h"
#include "core/end.h"
#include "core/do_name.h"
#include "core/dlb.h"
#ifdef SHORT_FILENAMES
#include "patchlev.h"
#else
#include "core/patchlevel.h"
#endif

/*
 * Updating in place can leave junk at the end of the file in some
 * circumstances (if it shrinks and the O.S. doesn't have a straightforward
 * way to truncate it).  The trailing junk is harmless and the code
 * which reads the scores will ignore it.
 */
#ifdef UPDATE_RECORD_IN_PLACE
static long final_fpos;
#endif

#define done_stopprint program_state.stopprint

#define newttentry() (struct toptenentry *) alloc(sizeof(struct toptenentry))
#define dealloc_ttentry(ttent) free((genericptr_t)(ttent))
#define NAMSZ 10
#define DTHSZ 100
#define ROLESZ 3
#define PERSMAX 3    /* entries per name/uid per char. allowed */
#define POINTSMIN 1  /* must be > 0 */
#define ENTRYMAX 100 /* must be >= 10 */

#define PERS_IS_UID /* delete for PERSMAX per name; now per uid */
struct toptenentry {
  struct toptenentry *tt_next;
#ifdef UPDATE_RECORD_IN_PLACE
  long fpos;
#endif
  long points;
  int deathdnum, deathlev;
  int maxlvl, hp, maxhp, deaths;
  int ver_major, ver_minor, patchlevel;
  long deathdate, birthdate;
  int uid;
  char plrole[ROLESZ + 1];
  char plrace[ROLESZ + 1];
  char plgend[ROLESZ + 1];
  char plalign[ROLESZ + 1];
  char name[NAMSZ + 1];
  char death[DTHSZ + 1];
} *tt_head;

STATIC_DCL void topten_print(const char *);
STATIC_DCL void topten_print_bold(const char *);
STATIC_DCL xchar observable_depth(d_level *);
STATIC_DCL void outheader();
STATIC_DCL void outentry(int, struct toptenentry *, int);
STATIC_DCL void readentry(FILE *, struct toptenentry *);
STATIC_DCL void writeentry(FILE *, struct toptenentry *);
#ifdef XLOGFILE
STATIC_DCL void munge_xlstring(char *dest, char *src, int n);
STATIC_DCL void write_xlentry(FILE *, struct toptenentry *);
#endif
STATIC_DCL void free_ttlist(struct toptenentry *);
STATIC_DCL int classmon(char *, bool);
STATIC_DCL int score_wanted(bool, int, struct toptenentry *, int, const char **,
                            int);
#ifdef RECORD_CONDUCT
STATIC_DCL long encodeconduct(void);
#endif
#ifdef RECORD_ACHIEVE
STATIC_DCL long encodeachieve(void);
#endif
#ifdef NO_SCAN_BRACK
STATIC_DCL void nsb_mung_line(char *);
STATIC_DCL void nsb_unmung_line(char *);
#endif

extern const char *const killed_by_prefix[];

/* must fit with end.c; used in rip.c */
const char *const killed_by_prefix[] = {
    "killed by ",          "choked on ",           "poisoned by ",
    "died of ",            "drowned in ",          "burned by ",
    "dissolved in ",       "crushed to death by ", "petrified by ",
    "turned to slime by ", "killed by ",           "",
    "",                    "",                     "",
    ""};

static winid toptenwin = WIN_ERR;

#ifdef RECORD_START_END_TIME
static time_t deathtime = 0L;
#endif

STATIC_OVL void topten_print(const char *x) {
  if (toptenwin == WIN_ERR)
    raw_print(x);
  else
    putstr(toptenwin, ATR_NONE, x);
}

STATIC_OVL void topten_print_bold(const char *x) {
  if (toptenwin == WIN_ERR)
    raw_print_bold(x);
  else
    putstr(toptenwin, ATR_BOLD, x);
}

STATIC_OVL xchar observable_depth(d_level *lev) {
#if 0 /* if we ever randomize the order of the elemental planes, we \
         must use a constant external representation in the record file */
	if (In_endgame(lev)) {
	    if (Is_astralevel(lev))	 return -5;
	    else if (Is_waterlevel(lev)) return -4;
	    else if (Is_firelevel(lev))	 return -3;
	    else if (Is_airlevel(lev))	 return -2;
	    else if (Is_earthlevel(lev)) return -1;
	    else			 return 0;	/* ? */
	} else
#endif
  return depth(lev);
}

STATIC_OVL void readentry(FILE *rfile, struct toptenentry *tt) {
#ifdef NO_SCAN_BRACK /* Version_ Pts DgnLevs_ Hp___ Died__Born id */
  static const char fmt[] = "%d %d %d %ld %d %d %d %d %d %d %ld %ld %d%*c";
  static const char fmt32[] = "%c%c %s %s%*c";
  static const char fmt33[] = "%s %s %s %s %s %s%*c";
#else
  static const char fmt[] = "%d.%d.%d %ld %d %d %d %d %d %d %ld %ld %d ";
  static const char fmt32[] = "%c%c %[^,],%[^\n]%*c";
  static const char fmt33[] = "%s %s %s %s %[^,],%[^\n]%*c";
#endif

#ifdef UPDATE_RECORD_IN_PLACE
  /* note: fscanf() below must read the record's terminating newline */
  final_fpos = tt->fpos = ftell(rfile);
#endif
#define TTFIELDS 13
  if (fscanf(rfile, fmt, &tt->ver_major, &tt->ver_minor, &tt->patchlevel,
             &tt->points, &tt->deathdnum, &tt->deathlev, &tt->maxlvl, &tt->hp,
             &tt->maxhp, &tt->deaths, &tt->deathdate, &tt->birthdate,
             &tt->uid) != TTFIELDS)
#undef TTFIELDS
    tt->points = 0;
  else {
    /* Check for backwards compatibility */
    if (tt->ver_major < 3 || (tt->ver_major == 3 && tt->ver_minor < 3)) {
      int i;

      if (fscanf(rfile, fmt32, tt->plrole, tt->plgend, tt->name, tt->death) !=
          4)
        tt->points = 0;
      tt->plrole[1] = '\0';
      if ((i = str2role(tt->plrole)) >= 0)
        strcpy(tt->plrole, roles[i].filecode);
      strcpy(tt->plrace, "?");
      strcpy(tt->plgend, (tt->plgend[0] == 'M') ? "Mal" : "Fem");
      strcpy(tt->plalign, "?");
    } else if (fscanf(rfile, fmt33, tt->plrole, tt->plrace, tt->plgend,
                      tt->plalign, tt->name, tt->death) != 6)
      tt->points = 0;
#ifdef NO_SCAN_BRACK
    if (tt->points > 0) {
      nsb_unmung_line(tt->name);
      nsb_unmung_line(tt->death);
    }
#endif
  }

  /* check old score entries for Y2K problem and fix whenever found */
  if (tt->points > 0) {
    if (tt->birthdate < 19000000L)
      tt->birthdate += 19000000L;
    if (tt->deathdate < 19000000L)
      tt->deathdate += 19000000L;
  }
}

STATIC_OVL void writeentry(FILE *rfile, struct toptenentry *tt) {
#ifdef NO_SCAN_BRACK
  nsb_mung_line(tt->name);
  nsb_mung_line(tt->death);
  /* Version_ Pts DgnLevs_ Hp___ Died__Born id */
  (void)fprintf(rfile, "%d %d %d %ld %d %d %d %d %d %d %ld %ld %d ",
#else
  (void)fprintf(rfile, "%d.%d.%d %ld %d %d %d %d %d %d %ld %ld %d ",
#endif
                tt->ver_major, tt->ver_minor, tt->patchlevel, tt->points,
                tt->deathdnum, tt->deathlev, tt->maxlvl, tt->hp, tt->maxhp,
                tt->deaths, tt->deathdate, tt->birthdate, tt->uid);
  if (tt->ver_major < 3 || (tt->ver_major == 3 && tt->ver_minor < 3))
#ifdef NO_SCAN_BRACK
    (void)fprintf(rfile, "%c%c %s %s\n",
#else
    (void)fprintf(rfile, "%c%c %s,%s\n",
#endif
                  tt->plrole[0], tt->plgend[0],
                  onlyspace(tt->name) ? "_" : tt->name, tt->death);
  else
#ifdef NO_SCAN_BRACK
    (void)fprintf(rfile, "%s %s %s %s %s %s\n",
#else
    (void)fprintf(rfile, "%s %s %s %s %s,%s\n",
#endif
                  tt->plrole, tt->plrace, tt->plgend, tt->plalign,
                  onlyspace(tt->name) ? "_" : tt->name, tt->death);

#ifdef NO_SCAN_BRACK
  nsb_unmung_line(tt->name);
  nsb_unmung_line(tt->death);
#endif
}

#ifdef XLOGFILE
#define SEP ":"
#define SEPC ':'

/* copy a maximum of n-1 characters from src to dest, changing ':' and '\n'
 * to '_'; always null-terminate. */
STATIC_OVL void munge_xlstring(char *dest, char *src, int n) {
  int i;

  for (i = 0; i < (n - 1) && src[i] != '\0'; i++) {
    if (src[i] == SEPC || src[i] == '\n')
      dest[i] = '_';
    else
      dest[i] = src[i];
  }

  dest[i] = '\0';

  return;
}

STATIC_OVL void write_xlentry(FILE *rfile, struct toptenentry *tt) {
  char buf[DTHSZ + 1];

  /* Log all of the data found in the regular logfile */
  (void)fprintf(rfile, "version=%d.%d.%d" SEP "points=%ld" SEP
                       "deathdnum=%d" SEP "deathlev=%d" SEP "maxlvl=%d" SEP
                       "hp=%d" SEP "maxhp=%d" SEP "deaths=%d" SEP
                       "deathdate=%ld" SEP "birthdate=%ld" SEP "uid=%d",
                tt->ver_major, tt->ver_minor, tt->patchlevel, tt->points,
                tt->deathdnum, tt->deathlev, tt->maxlvl, tt->hp, tt->maxhp,
                tt->deaths, tt->deathdate, tt->birthdate, tt->uid);

  (void)fprintf(rfile,
                SEP "role=%s" SEP "race=%s" SEP "gender=%s" SEP "align=%s",
                tt->plrole, tt->plrace, tt->plgend, tt->plalign);

  munge_xlstring(buf, plname, DTHSZ + 1);
  (void)fprintf(rfile, SEP "name=%s", buf);

  munge_xlstring(buf, tt->death, DTHSZ + 1);
  (void)fprintf(rfile, SEP "death=%s", buf);

#ifdef RECORD_CONDUCT
  (void)fprintf(rfile, SEP "conduct=0x%lx", encodeconduct());
#endif

#ifdef RECORD_TURNS
  (void)fprintf(rfile, SEP "turns=%ld", moves);
#endif

#ifdef RECORD_ACHIEVE
  (void)fprintf(rfile, SEP "achieve=0x%lx", encodeachieve());
#endif

#ifdef RECORD_REALTIME
  (void)fprintf(rfile, SEP "realtime=%ld", (long)realtime_data.realtime);
#endif

#ifdef RECORD_START_END_TIME
  (void)fprintf(rfile, SEP "starttime=%ld", (long)player.ubirthday);
  (void)fprintf(rfile, SEP "endtime=%ld", (long)deathtime);
#endif

#ifdef RECORD_GENDER0
  (void)fprintf(rfile, SEP "gender0=%s", genders[flags.initgend].filecode);
#endif

#ifdef RECORD_ALIGN0
  (void)fprintf(rfile, SEP "align0=%s",
                aligns[1 - player.ualignbase[A_ORIGINAL]].filecode);
#endif

  (void)fprintf(rfile, "\n");
}

#undef SEP
#undef SEPC
#endif /* XLOGFILE */

STATIC_OVL void free_ttlist(struct toptenentry *tt) {
  struct toptenentry *ttnext;

  while (tt->points > 0) {
    ttnext = tt->tt_next;
    dealloc_ttentry(tt);
    tt = ttnext;
  }
  dealloc_ttentry(tt);
}

void topten(int how) {
  int uid = getuid();
  int rank, rank0 = -1, rank1 = 0;
  int occ_cnt = PERSMAX;
  struct toptenentry *t0, *tprev;
  struct toptenentry *t1;
  FILE *rfile;
  int flg = 0;
  bool t0_used;
  char buf[15];
#ifdef LOGFILE
  FILE *lfile;
#endif /* LOGFILE */
#ifdef XLOGFILE
  FILE *xlfile;
#endif /* XLOGFILE */

/* Under DICE 3.0, this crashes the system consistently, apparently due to
 * corruption of *rfile somewhere.  Until I figure this out, just cut out
 * topten support entirely - at least then the game exits cleanly.  --AC
 */
#ifdef _DCC
  return;
#endif

  /* If we are in the midst of a panic, cut out topten entirely.
   * topten uses alloc() several times, which will lead to
   * problems if the panic was the result of an alloc() failure.
   */
  if (program_state.panicking)
    return;

  if (flags.toptenwin) {
    toptenwin = create_nhwindow(NHW_TEXT);
  }

#define HUP if (!program_state.done_hup)

  /* create a new 'topten' entry */
  t0_used = FALSE;
  t0 = newttentry();
  /* deepest_lev_reached() is in terms of depth(), and reporting the
   * deepest level reached in the dungeon death occurred in doesn't
   * seem right, so we have to report the death level in depth() terms
   * as well (which also seems reasonable since that's all the player
   * sees on the screen anyway)
   */
  t0->ver_major = VERSION_MAJOR;
  t0->ver_minor = VERSION_MINOR;
  t0->patchlevel = PATCHLEVEL;
  t0->points = player.urexp;
  t0->deathdnum = player.uz.dnum;
  t0->deathlev = observable_depth(&player.uz);
  t0->maxlvl = deepest_lev_reached(TRUE);
  t0->hp = player.uhp;
  t0->maxhp = player.uhpmax;
  t0->deaths = player.umortality;
  t0->uid = uid;
  (void)strncpy(t0->plrole, urole.filecode, ROLESZ);
  t0->plrole[ROLESZ] = '\0';
  (void)strncpy(t0->plrace, urace.filecode, ROLESZ);
  t0->plrace[ROLESZ] = '\0';
  (void)strncpy(t0->plgend, genders[flags.female].filecode, ROLESZ);
  t0->plgend[ROLESZ] = '\0';
  (void)strncpy(t0->plalign, aligns[1 - player.ualign.type].filecode, ROLESZ);
  t0->plalign[ROLESZ] = '\0';
  (void)strncpy(t0->name, plname, NAMSZ);
  t0->name[NAMSZ] = '\0';
  t0->death[0] = '\0';
  switch (killer_format) {
    default:
      impossible("bad killer format?");
    case KILLED_BY_AN:
      strcat(t0->death, killed_by_prefix[how]);
      (void)strncat(t0->death, an(killer), DTHSZ - strlen(t0->death));
      break;
    case KILLED_BY:
      strcat(t0->death, killed_by_prefix[how]);
      (void)strncat(t0->death, killer, DTHSZ - strlen(t0->death));
      break;
    case NO_KILLER_PREFIX:
      (void)strncat(t0->death, killer, DTHSZ);
      break;
  }
#ifdef LOG_MOVES
  /* jl 08.2000 - 09.2003 */
  sprintf(buf, " {%ld}", moves);
  if (strlen(t0->death) + strlen(buf) < DTHSZ)
    strcat(t0->death, buf);
  if (wizard && strlen(t0->death) < DTHSZ - 6)
    strcat(t0->death, " {wiz}");
  if (discover && strlen(t0->death) < DTHSZ - 6)
    strcat(t0->death, " {exp}");
#endif
  t0->birthdate = yyyymmdd(player.ubirthday);

#ifdef RECORD_START_END_TIME
/* Make sure that deathdate and deathtime refer to the same time; it
 * wouldn't be good to have deathtime refer to the day after deathdate. */

#if defined(BSD) && !defined(POSIX_TYPES)
  (void)time((long *)&deathtime);
#else
  (void)time(&deathtime);
#endif

  t0->deathdate = yyyymmdd(deathtime);
#else
  t0->deathdate = yyyymmdd((time_t)0L);
#endif /* RECORD_START_END_TIME */

  t0->tt_next = 0;
#ifdef UPDATE_RECORD_IN_PLACE
  t0->fpos = -1L;
#endif

#ifdef LOGFILE /* used for debugging (who dies of what, where) */
  if (lock_file(LOGFILE, SCOREPREFIX, 10)) {
    if (!(lfile = fopen_datafile(LOGFILE, "a", SCOREPREFIX))) {
      HUP raw_print("Cannot open log file!");
    } else {
      writeentry(lfile, t0);
      (void)fclose(lfile);
    }
    unlock_file(LOGFILE);
  }
#endif /* LOGFILE */

#ifdef XLOGFILE
  if (lock_file(XLOGFILE, SCOREPREFIX, 10)) {
    if (!(xlfile = fopen_datafile(XLOGFILE, "a", SCOREPREFIX))) {
      HUP raw_print("Cannot open extended log file!");
    } else {
      write_xlentry(xlfile, t0);
      (void)fclose(xlfile);
    }
    unlock_file(XLOGFILE);
  }
#endif /* XLOGFILE */

  if (wizard || discover) {
    if (how != PANICKED)
      HUP {
        char pbuf[BUFSZ];
        topten_print("");
        sprintf(
            pbuf,
            "Since you were in %s mode, the score list will not be checked.",
            wizard ? "wizard" : "discover");
        topten_print(pbuf);
#ifdef DUMP_LOG
        if (dump_fn[0]) {
          dump("", pbuf);
          dump("", "");
        }
#endif
      }
    goto showwin;
  }

  if (!lock_file(RECORD, SCOREPREFIX, 60))
    goto destroywin;

#ifdef UPDATE_RECORD_IN_PLACE
  rfile = fopen_datafile(RECORD, "r+", SCOREPREFIX);
#else
  rfile = fopen_datafile(RECORD, "r", SCOREPREFIX);
#endif

  if (!rfile) {
    HUP raw_print("Cannot open record file!");
    unlock_file(RECORD);
    goto destroywin;
  }

  HUP topten_print("");
#ifdef DUMP_LOG
  dump("", "");
#endif

  /* assure minimum number of points */
  if (t0->points < POINTSMIN)
    t0->points = 0;

  t1 = tt_head = newttentry();
  tprev = 0;
  /* rank0: -1 undefined, 0 not_on_list, n n_th on list */
  for (rank = 1;;) {
    readentry(rfile, t1);
    if (t1->points < POINTSMIN)
      t1->points = 0;
    if (rank0 < 0 && t1->points < t0->points) {
      rank0 = rank++;
      if (tprev == 0)
        tt_head = t0;
      else
        tprev->tt_next = t0;
      t0->tt_next = t1;
#ifdef UPDATE_RECORD_IN_PLACE
      t0->fpos = t1->fpos; /* insert here */
#endif
      t0_used = TRUE;
      occ_cnt--;
      flg++; /* ask for a rewrite */
    } else
      tprev = t1;

    if (t1->points == 0)
      break;
    if (
#ifdef PERS_IS_UID
            t1->uid == t0->uid &&
#else
            strncmp(t1->name, t0->name, NAMSZ) == 0 &&
#endif
            !strncmp(t1->plrole, t0->plrole, ROLESZ) && --occ_cnt <= 0) {
      if (rank0 < 0) {
        rank0 = 0;
        rank1 = rank;
        HUP {
          char pbuf[BUFSZ];
          sprintf(pbuf, "You didn't beat your previous score of %ld points.",
                  t1->points);
          topten_print(pbuf);
          topten_print("");
#ifdef DUMP_LOG
          dump("", pbuf);
          dump("", "");
#endif
        }
      }
      if (occ_cnt < 0) {
        flg++;
        continue;
      }
    }
    if (rank <= ENTRYMAX) {
      t1->tt_next = newttentry();
      t1 = t1->tt_next;
      rank++;
    }
    if (rank > ENTRYMAX) {
      t1->points = 0;
      break;
    }
  }
  if (flg) { /* rewrite record file */
#ifdef UPDATE_RECORD_IN_PLACE
    (void)fseek(rfile, (t0->fpos >= 0 ? t0->fpos : final_fpos), SEEK_SET);
#else
    (void)fclose(rfile);
    if (!(rfile = fopen_datafile(RECORD, "w", SCOREPREFIX))) {
      HUP raw_print("Cannot write record file");
      unlock_file(RECORD);
      free_ttlist(tt_head);
      goto destroywin;
    }
#endif /* UPDATE_RECORD_IN_PLACE */
    if (rank0 > 0) {
      if (rank0 <= 10) {
        if (!done_stopprint)
          topten_print("You made the top ten list!");
#ifdef DUMP_LOG
        dump("", "You made the top ten list!");
#endif
      } else {
        char pbuf[BUFSZ];
        sprintf(pbuf, "You reached the %d%s place on the top %d list.", rank0,
                ordin(rank0), ENTRYMAX);
        if (!done_stopprint)
          topten_print(pbuf);
#ifdef DUMP_LOG
        dump("", pbuf);
#endif
      }
      if (!done_stopprint)
        topten_print("");
#ifdef DUMP_LOG
      dump("", "");
#endif
    }
  }
  if (rank0 == 0)
    rank0 = rank1;
  if (rank0 <= 0)
    rank0 = rank;
  if (!done_stopprint)
    outheader();
  t1 = tt_head;
  for (rank = 1; t1->points != 0; rank++, t1 = t1->tt_next) {
    if (flg
#ifdef UPDATE_RECORD_IN_PLACE
        && rank >= rank0
#endif
        )
      writeentry(rfile, t1);
    /* if (done_stopprint) continue; */
    if (rank > flags.end_top &&
        (rank < rank0 - flags.end_around || rank > rank0 + flags.end_around) &&
        (!flags.end_own ||
#ifdef PERS_IS_UID
         t1->uid != t0->uid
#else
         strncmp(t1->name, t0->name, NAMSZ)
#endif
         ))
      continue;
    if (rank == rank0 - flags.end_around &&
        rank0 > flags.end_top + flags.end_around + 1 && !flags.end_own) {
      if (!done_stopprint)
        topten_print("");
#ifdef DUMP_LOG
      dump("", "");
#endif
    }
    if (rank != rank0)
      outentry(rank, t1, FALSE);
    else if (!rank1)
      outentry(rank, t1, TRUE);
    else {
      outentry(rank, t1, TRUE);
      outentry(0, t0, TRUE);
    }
  }
  if (rank0 >= rank)
    if (!done_stopprint)
      outentry(0, t0, TRUE);
#ifdef UPDATE_RECORD_IN_PLACE
  if (flg) {
#ifdef TRUNCATE_FILE
    /* if a reasonable way to truncate a file exists, use it */
    truncate_file(rfile);
#else
    /* use sentinel record rather than relying on truncation */
    t1->points = 0L; /* terminates file when read back in */
    t1->ver_major = t1->ver_minor = t1->patchlevel = 0;
    t1->uid = t1->deathdnum = t1->deathlev = 0;
    t1->maxlvl = t1->hp = t1->maxhp = t1->deaths = 0;
    t1->plrole[0] = t1->plrace[0] = t1->plgend[0] = t1->plalign[0] = '-';
    t1->plrole[1] = t1->plrace[1] = t1->plgend[1] = t1->plalign[1] = 0;
    t1->birthdate = t1->deathdate = yyyymmdd((time_t)0L);
    strcpy(t1->name, "@");
    strcpy(t1->death, "<eod>\n");
    writeentry(rfile, t1);
    (void)fflush(rfile);
#endif /* TRUNCATE_FILE */
  }
#endif /* UPDATE_RECORD_IN_PLACE */
  (void)fclose(rfile);
  unlock_file(RECORD);
  free_ttlist(tt_head);

showwin:
  if (flags.toptenwin && !done_stopprint)
    display_nhwindow(toptenwin, 1);
destroywin:
  if (!t0_used)
    dealloc_ttentry(t0);
  if (flags.toptenwin) {
    destroy_nhwindow(toptenwin);
    toptenwin = WIN_ERR;
  }
}

STATIC_OVL void outheader() {
  char linebuf[BUFSZ];
  char *bp;

  strcpy(linebuf, " No  Points     Name");
  bp = eos(linebuf);
  while (bp < linebuf + COLNO - 9)
    *bp++ = ' ';
  strcpy(bp, "Hp [max]");
  if (!done_stopprint)
    topten_print(linebuf);
#ifdef DUMP_LOG
  dump("", linebuf);
#endif
}

/* so>0: standout line; so=0: ordinary line */
STATIC_OVL void outentry(int rank, struct toptenentry *t1, int so) {
  bool second_line = TRUE;
  char linebuf[BUFSZ];
  char *bp, hpbuf[24], linebuf3[BUFSZ];
  int hppos, lngr;

  linebuf[0] = '\0';
  if (rank)
    sprintf(eos(linebuf), "%3d", rank);
  else
    strcat(linebuf, "   ");

  sprintf(eos(linebuf), " %10ld  %.10s", t1->points, t1->name);
  sprintf(eos(linebuf), "-%s", t1->plrole);
  if (t1->plrace[0] != '?')
    sprintf(eos(linebuf), "-%s", t1->plrace);
  /* Printing of gender and alignment is intentional.  It has been
   * part of the NetHack Geek Code, and illustrates a proper way to
   * specify a character from the command line.
   */
  sprintf(eos(linebuf), "-%s", t1->plgend);
  if (t1->plalign[0] != '?')
    sprintf(eos(linebuf), "-%s ", t1->plalign);
  else
    strcat(linebuf, " ");
  if (!strncmp("escaped", t1->death, 7)) {
    sprintf(eos(linebuf), "escaped the dungeon %s[max level %d]",
            !strncmp(" (", t1->death + 7, 2) ? t1->death + 7 + 2 : "",
            t1->maxlvl);
    /* fixup for closing paren in "escaped... with...Amulet)[max..." */
    if ((bp = index(linebuf, ')')) != 0)
      *bp = (t1->deathdnum == astral_level.dnum) ? '\0' : ' ';
    second_line = FALSE;
  } else if (!strncmp("ascended", t1->death, 8)) {
    sprintf(eos(linebuf), "ascended to demigod%s-hood",
            (t1->plgend[0] == 'F') ? "dess" : "");
    second_line = FALSE;
  } else {
    if (!strncmp(t1->death, "quit", 4)) {
      strcat(linebuf, "quit");
      second_line = FALSE;
    } else if (!strncmp(t1->death, "died of st", 10)) {
      strcat(linebuf, "starved to death");
      second_line = FALSE;
    } else if (!strncmp(t1->death, "choked", 6)) {
      sprintf(eos(linebuf), "choked on h%s food",
              (t1->plgend[0] == 'F') ? "er" : "is");
    } else if (!strncmp(t1->death, "poisoned", 8)) {
      strcat(linebuf, "was poisoned");
    } else if (!strncmp(t1->death, "crushed", 7)) {
      strcat(linebuf, "was crushed to death");
    } else if (!strncmp(t1->death, "petrified by ", 13)) {
      strcat(linebuf, "turned to stone");
    } else
      strcat(linebuf, "died");

    if (t1->deathdnum == astral_level.dnum) {
      const char *arg, *fmt = " on the Plane of %s";

      switch (t1->deathlev) {
        case -5:
          fmt = " on the %s Plane";
          arg = "Astral";
          break;
        case -4:
          arg = "Water";
          break;
        case -3:
          arg = "Fire";
          break;
        case -2:
          arg = "Air";
          break;
        case -1:
          arg = "Earth";
          break;
        default:
          arg = "Void";
          break;
      }
      sprintf(eos(linebuf), fmt, arg);
    } else {
      sprintf(eos(linebuf), " in %s", dungeons[t1->deathdnum].dname);
      if (t1->deathdnum != knox_level.dnum)
        sprintf(eos(linebuf), " on level %d", t1->deathlev);
      if (t1->deathlev != t1->maxlvl)
        sprintf(eos(linebuf), " [max %d]", t1->maxlvl);
    }

    /* kludge for "quit while already on Charon's boat" */
    if (!strncmp(t1->death, "quit ", 5))
      strcat(linebuf, t1->death + 4);
  }
  strcat(linebuf, ".");

  /* Quit, starved, ascended, and escaped contain no second line */
  if (second_line)
    sprintf(eos(linebuf), "  %c%s.", highc(*(t1->death)), t1->death + 1);

  lngr = (int)strlen(linebuf);
  if (t1->hp <= 0)
    hpbuf[0] = '-', hpbuf[1] = '\0';
  else
    sprintf(hpbuf, "%d", t1->hp);
  /* beginning of hp column after padding (not actually padded yet) */
  hppos = COLNO - (sizeof("  Hp [max]") - 1); /* sizeof(str) includes \0 */
  while (lngr >= hppos) {
    for (bp = eos(linebuf); !(*bp == ' ' && (bp - linebuf < hppos)); bp--)
      ;
    /* special case: if about to wrap in the middle of maximum
       dungeon depth reached, wrap in front of it instead */
    if (bp > linebuf + 5 && !strncmp(bp - 5, " [max", 5))
      bp -= 5;
    strcpy(linebuf3, bp + 1);
    *bp = 0;
    if (so) {
      while (bp < linebuf + (COLNO - 1))
        *bp++ = ' ';
      *bp = 0;
      if (!done_stopprint)
        topten_print_bold(linebuf);
#ifdef DUMP_LOG
      dump("*", linebuf[0] == ' ' ? linebuf + 1 : linebuf);
#endif
    } else {
      if (!done_stopprint)
        topten_print(linebuf);
#ifdef DUMP_LOG
      dump(" ", linebuf[0] == ' ' ? linebuf + 1 : linebuf);
#endif
    }
    sprintf(linebuf, "%15s %s", "", linebuf3);
    lngr = strlen(linebuf);
  }
  /* beginning of hp column not including padding */
  hppos = COLNO - 7 - (int)strlen(hpbuf);
  bp = eos(linebuf);

  if (bp <= linebuf + hppos) {
    /* pad any necessary blanks to the hit point entry */
    while (bp < linebuf + hppos)
      *bp++ = ' ';
    strcpy(bp, hpbuf);
    sprintf(eos(bp), " %s[%d]",
            (t1->maxhp < 10) ? "  " : (t1->maxhp < 100) ? " " : "", t1->maxhp);
  }

  if (so) {
    bp = eos(linebuf);
    if (so >= COLNO)
      so = COLNO - 1;
    while (bp < linebuf + so)
      *bp++ = ' ';
    *bp = 0;
    if (!done_stopprint)
      topten_print_bold(linebuf);
  } else if (!done_stopprint)
    topten_print(linebuf);
#ifdef DUMP_LOG
  dump(" ", linebuf[0] == ' ' ? linebuf + 1 : linebuf);
#endif
}

STATIC_OVL int score_wanted(bool current_ver, int rank, struct toptenentry *t1,
                            int playerct, const char **players, int uid) {
  int i;

  if (current_ver &&
      (t1->ver_major != VERSION_MAJOR || t1->ver_minor != VERSION_MINOR ||
       t1->patchlevel != PATCHLEVEL))
    return 0;

#ifdef PERS_IS_UID
  if (!playerct && t1->uid == uid)
    return 1;
#endif

  for (i = 0; i < playerct; i++) {
    if (players[i][0] == '-' && index("pr", players[i][1]) &&
        players[i][2] == 0 && i + 1 < playerct) {
      char *arg = (char *)players[i + 1];
      if ((players[i][1] == 'p' && str2role(arg) == str2role(t1->plrole)) ||
          (players[i][1] == 'r' && str2race(arg) == str2race(t1->plrace)))
        return 1;
      i++;
    } else if (strcmp(players[i], "all") == 0 ||
               strncmp(t1->name, players[i], NAMSZ) == 0 ||
               (players[i][0] == '-' && players[i][1] == t1->plrole[0] &&
                players[i][2] == 0) ||
               (digit(players[i][0]) && rank <= atoi(players[i])))
      return 1;
  }
  return 0;
}

#ifdef RECORD_CONDUCT
long encodeconduct(void) {
  long e = 0L;

  if (!player.uconduct.food)
    e |= 0x001L;
  if (!player.uconduct.unvegan)
    e |= 0x002L;
  if (!player.uconduct.unvegetarian)
    e |= 0x004L;
  if (!player.uconduct.gnostic)
    e |= 0x008L;
  if (!player.uconduct.weaphit)
    e |= 0x010L;
  if (!player.uconduct.killer)
    e |= 0x020L;
  if (!player.uconduct.literate)
    e |= 0x040L;
  if (!player.uconduct.polypiles)
    e |= 0x080L;
  if (!player.uconduct.polyselfs)
    e |= 0x100L;
  if (!player.uconduct.wishes)
    e |= 0x200L;
  if (!player.uconduct.wisharti)
    e |= 0x400L;
  if (!num_genocides())
    e |= 0x800L;

  return e;
}
#endif

#ifdef RECORD_ACHIEVE
long encodeachieve(void) {
  /* Achievement bitfield:
   * bit  meaning
   *  0   obtained the Bell of Opening
   *  1   entered gehennom (by any means)
   *  2   obtained the Candelabrum of Invocation
   *  3   obtained the Book of the Dead
   *  4   performed the invocation ritual
   *  5   obtained the amulet
   *  6   entered elemental planes
   *  7   entered astral plane
   *  8   ascended (not "escaped in celestial disgrace!")
   *  9   obtained the luckstone from the Mines
   *  10  obtained the sokoban prize
   *  11  killed medusa
   */

  long r;

  r = 0;

  if (achieve.get_bell)
    r |= 1L << 0;
  if (achieve.enter_gehennom)
    r |= 1L << 1;
  if (achieve.get_candelabrum)
    r |= 1L << 2;
  if (achieve.get_book)
    r |= 1L << 3;
  if (achieve.perform_invocation)
    r |= 1L << 4;
  if (achieve.get_amulet)
    r |= 1L << 5;
  if (In_endgame(&player.uz))
    r |= 1L << 6;
  if (Is_astralevel(&player.uz))
    r |= 1L << 7;
  if (achieve.ascended)
    r |= 1L << 8;
  if (achieve.get_luckstone)
    r |= 1L << 9;
  if (achieve.finish_sokoban)
    r |= 1L << 10;
  if (achieve.killed_medusa)
    r |= 1L << 11;

  return r;
}
#endif

/*
 * print selected parts of score list.
 * argc >= 2, with argv[0] untrustworthy (directory names, et al.),
 * and argv[1] starting with "-s".
 */
void prscore(int argc, char **argv) {
  const char **players;
  int playerct, rank;
  bool current_ver = TRUE, init_done = FALSE;
  struct toptenentry *t1;
  FILE *rfile;
  bool match_found = FALSE;
  int i;
  char pbuf[BUFSZ];
  int uid = -1;
#ifndef PERS_IS_UID
  const char *player0;
#endif

  if (argc < 2 || strncmp(argv[1], "-s", 2)) {
    raw_printf("prscore: bad arguments (%d)", argc);
    return;
  }

  rfile = fopen_datafile(RECORD, "r", SCOREPREFIX);
  if (!rfile) {
    raw_print("Cannot open record file!");
    return;
  }

  /* If the score list isn't after a game, we never went through
   * initialization. */
  if (wiz1_level.dlevel == 0) {
    dlb_init();
    init_dungeons();
    init_done = TRUE;
  }

  if (!argv[1][2]) { /* plain "-s" */
    argc--;
    argv++;
  } else
    argv[1] += 2;

  if (argc > 1 && !strcmp(argv[1], "-v")) {
    current_ver = FALSE;
    argc--;
    argv++;
  }

  if (argc <= 1) {
#ifdef PERS_IS_UID
    uid = getuid();
    playerct = 0;
    players = (const char **)0;
#else
    player0 = plname;
    if (!*player0)
      player0 = "hackplayer";
    playerct = 1;
    players = &player0;
#endif
  } else {
    playerct = --argc;
    players = (const char **)++argv;
  }
  raw_print("");

  t1 = tt_head = newttentry();
  for (rank = 1;; rank++) {
    readentry(rfile, t1);
    if (t1->points == 0)
      break;
    if (!match_found &&
        score_wanted(current_ver, rank, t1, playerct, players, uid))
      match_found = TRUE;
    t1->tt_next = newttentry();
    t1 = t1->tt_next;
  }

  (void)fclose(rfile);
  if (init_done) {
    free_dungeons();
    dlb_cleanup();
  }

  if (match_found) {
    outheader();
    t1 = tt_head;
    for (rank = 1; t1->points != 0; rank++, t1 = t1->tt_next) {
      if (score_wanted(current_ver, rank, t1, playerct, players, uid))
        (void)outentry(rank, t1, 0);
    }
  } else {
    sprintf(pbuf, "Cannot find any %sentries for ",
            current_ver ? "current " : "");
    if (playerct < 1)
      strcat(pbuf, "you.");
    else {
      if (playerct > 1)
        strcat(pbuf, "any of ");
      for (i = 0; i < playerct; i++) {
        /* stop printing players if there are too many to fit */
        if (strlen(pbuf) + strlen(players[i]) + 2 >= BUFSZ) {
          if (strlen(pbuf) < BUFSZ - 4)
            strcat(pbuf, "...");
          else
            strcpy(pbuf + strlen(pbuf) - 4, "...");
          break;
        }
        strcat(pbuf, players[i]);
        if (i < playerct - 1) {
          if (players[i][0] == '-' && index("pr", players[i][1]) &&
              players[i][2] == 0)
            strcat(pbuf, " ");
          else
            strcat(pbuf, ":");
        }
      }
    }
    raw_print(pbuf);
    raw_printf("Usage: %s -s [-v] <playertypes> [maxrank] [playernames]",
               hname);
    raw_printf("Player types are: [-p role] [-r race]");
  }
  free_ttlist(tt_head);
}

STATIC_OVL int classmon(char *plch, bool fem) {
  int i;

  /* Look for this role in the role table */
  for (i = 0; roles[i].name.m; i++)
    if (!strncmp(plch, roles[i].filecode, ROLESZ)) {
      if (fem && roles[i].femalenum != NON_PM)
        return roles[i].femalenum;
      else if (roles[i].malenum != NON_PM)
        return roles[i].malenum;
      else
        return PM_HUMAN;
    }
  /* this might be from a 3.2.x score for former Elf class */
  if (!strcmp(plch, "E"))
    return PM_RANGER;

  impossible("What weird role is this? (%s)", plch);
  return (PM_HUMAN_MUMMY);
}

/*
 * Get a random player name and class from the high score list,
 * and attach them to an object (for statues or morgue corpses).
 */
Object *tt_oname(Object *otmp) {
  int rank;
  int i;
  struct toptenentry *tt;
  FILE *rfile;
  struct toptenentry tt_buf;

  if (!otmp)
    return (nullptr);

  rfile = fopen_datafile(RECORD, "r", SCOREPREFIX);
  if (!rfile) {
    impossible("Cannot open record file!");
    return nullptr;
  }

  tt = &tt_buf;
  rank = rnd(10);
pickentry:
  for (i = rank; i; i--) {
    readentry(rfile, tt);
    if (tt->points == 0)
      break;
  }

  if (tt->points == 0) {
    if (rank > 1) {
      rank = 1;
      rewind(rfile);
      goto pickentry;
    }
    otmp = nullptr;
  } else {
    /* reset timer in case corpse started out as lizard or troll */
    if (otmp->otyp == CORPSE)
      obj_stop_timers(otmp);
    otmp->corpsenm = classmon(tt->plrole, (tt->plgend[0] == 'F'));
    otmp->owt = GetWeight(otmp);
    otmp = oname(otmp, tt->name);
    if (otmp->otyp == CORPSE)
      StartCorpseTimeout(otmp);
  }

  (void)fclose(rfile);
  return otmp;
}

#ifdef NO_SCAN_BRACK
/* Lattice scanf isn't up to reading the scorefile.  What */
/* follows deals with that; I admit it's ugly. (KL) */
/* Now generally available (KL) */
STATIC_OVL void nsb_mung_line(char *p) {
  while ((p = index(p, ' ')) != 0)
    *p = '|';
}

STATIC_OVL void nsb_unmung_line(char *p) {
  while ((p = index(p, '|')) != 0)
    *p = ' ';
}
#endif /* NO_SCAN_BRACK */

/*topten.c*/
