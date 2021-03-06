/*	SCCS Id: @(#)pline.c	3.4	1999/11/28	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include <string.h>
#include <stdarg.h>

#include "core/hack.h"
#include "core/youprop.h"
#include "core/worn.h"
#include "core/polyself.h"
#include "core/objnam.h"
#include "core/hacklib.h"
#include "core/files.h"
#include "core/end.h"
#include "core/do_name.h"
#include "core/epri.h"
#ifdef WIZARD
#include "core/edog.h"
#endif


static bool no_repeat = FALSE;

static char *You_buf(int);

#if defined(DUMP_LOG) && defined(DUMPMSGS)
char msgs[DUMPMSGS][BUFSZ];
int lastmsg = -1;
#endif

void msgpline_add(int typ, char *pattern) {
  struct _plinemsg *tmp = (struct _plinemsg *)alloc(sizeof(struct _plinemsg));
  if (!tmp)
    return;
  tmp->msgtype = typ;
  tmp->pattern = strdup(pattern);
  tmp->next = pline_msg;
  pline_msg = tmp;
}

void msgpline_free() {
  struct _plinemsg *tmp = pline_msg;
  struct _plinemsg *tmp2;
  while (tmp) {
    free(tmp->pattern);
    tmp2 = tmp;
    tmp = tmp->next;
    free(tmp2);
  }
  pline_msg = NULL;
}

int msgpline_type(char const *msg) {
  struct _plinemsg *tmp = pline_msg;
  while (tmp) {
    if (pmatch(tmp->pattern, msg))
      return tmp->msgtype;
    tmp = tmp->next;
  }
  return MSGTYP_NORMAL;
}

/*VARARGS1*/
/* Note that these declarations rely on knowledge of the internals
 * of the variable argument handling stuff in "tradstdc.h"
 */

static void vpline(const char *, va_list);

void pline(const char *line, ...) {
  va_list args;
  va_start(args, line);
  vpline(line, args);
  va_end(args);
}

char prevmsg[BUFSZ];

static void vpline(const char *line, va_list the_args) {
  char pbuf[BUFSZ];
  int typ;
  /* Do NOT use VA_START and VA_END in here... see above */

  if (!line || !*line)
    return;
  if (index(line, '%')) {
    vsprintf(pbuf, line, the_args);
    line = pbuf;
  }
#if defined(DUMP_LOG) && defined(DUMPMSGS)
  if (DUMPMSGS > 0 && !program_state.gameover) {
    lastmsg = (lastmsg + 1) % DUMPMSGS;
    strncpy(msgs[lastmsg], line, BUFSZ);
  }
#endif
  typ = msgpline_type(line);
  if (!iflags.window_inited) {
    raw_print(line);
    return;
  }
  if (no_repeat && !strcmp(line, toplines))
    return;
  if (vision_full_recalc)
    vision_recalc(0);
  if (player.ux)
    flush_screen(1); /* %% */
  if (typ == MSGTYP_NOSHOW)
    return;
  if (typ == MSGTYP_NOREP && !strcmp(line, prevmsg))
    return;
  putstr(WIN_MESSAGE, 0, line);
  strncpy(prevmsg, line, BUFSZ);
  if (typ == MSGTYP_STOP)
    display_nhwindow(WIN_MESSAGE, TRUE); /* --more-- */
}

/*VARARGS1*/
void Norep(const char *line, ...) {
  va_list args;
  va_start(args, line);
  no_repeat = TRUE;
  vpline(line, args);
  no_repeat = FALSE;
  va_end(args);
  return;
}

/* work buffer for You(), &c and verbalize() */
static char *you_buf = 0;
static int you_buf_siz = 0;

static char *You_buf(int siz) {
  if (siz > you_buf_siz) {
    if (you_buf)
      free((genericptr_t)you_buf);
    you_buf_siz = siz + 10;
    you_buf = (char *)alloc((unsigned)you_buf_siz);
  }
  return you_buf;
}

void free_youbuf() {
  if (you_buf)
    free((genericptr_t)you_buf), you_buf = nullptr;
  you_buf_siz = 0;
}

/* `prefix' must be a string literal, not a pointer */
#define YouPrefix(pointer, prefix, text) \
  strcpy((pointer = You_buf((int)(strlen(text) + sizeof prefix))), prefix)

#define YouMessage(pointer, prefix, text) \
  strcat((YouPrefix(pointer, prefix, text), pointer), text)

/*VARARGS1*/
void You(const char *line, ...) {
  va_list args;
  char *tmp;
  va_start(args, line);
  vpline(YouMessage(tmp, "You ", line), args);
  va_end(args);
}

/*VARARGS1*/
void Your(const char *line, ...) {
  char *tmp;
  va_list args;
  va_start(args, line);
  vpline(YouMessage(tmp, "Your ", line), args);
  va_end(args);
}

/*VARARGS1*/
void You_feel(const char *line, ...) {
  char *tmp;
  va_list args;
  va_start(args, line);
  vpline(YouMessage(tmp, "You feel ", line), args);
  va_end(args);
}

/*VARARGS1*/
void You_cant(const char *line, ...) {
  char *tmp;
  va_list args;
  va_start(args, line);
  vpline(YouMessage(tmp, "You can't ", line), args);
  va_end(args);
}

/*VARARGS1*/
void pline_The(const char *line, ...) {
  char *tmp;
  va_list args;
  va_start(args, line);
  vpline(YouMessage(tmp, "The ", line), args);
  va_end(args);
}

/*VARARGS1*/
void There(const char *line, ...) {
  char *tmp;
  va_list args;
  va_start(args, line);
  vpline(YouMessage(tmp, "There ", line), args);
  va_end(args);
}

/*VARARGS1*/
void You_hear(const char *line, ...) {
  char *tmp;
  va_list args;
  va_start(args, line);
  if (Underwater)
    YouPrefix(tmp, "You barely hear ", line);
  else if (player.usleep)
    YouPrefix(tmp, "You dream that you hear ", line);
  else
    YouPrefix(tmp, "You hear ", line);
  vpline(strcat(tmp, line), args);
  va_end(args);
}

/*VARARGS1*/
void verbalize(const char *line, ...) {
  char *tmp;
  if (!flags.soundok)
    return;
  va_list args;
  va_start(args, line);
  tmp = You_buf((int)strlen(line) + sizeof "\"\"");
  strcpy(tmp, "\"");
  strcat(tmp, line);
  strcat(tmp, "\"");
  vpline(tmp, args);
  va_end(args);
}

/*VARARGS1*/
/* Note that these declarations rely on knowledge of the internals
 * of the variable argument handling stuff in "tradstdc.h"
 */

static void vraw_printf(const char *, va_list);

void raw_printf(const char *line, ...) {
  va_list args;
  va_start(args, line);
  vraw_printf(line, args);
  va_end(args);
}

static void vraw_printf(const char *line, va_list the_args) {
  /* Do NOT use VA_START and VA_END in here... see above */

  if (!index(line, '%'))
    raw_print(line);
  else {
    char pbuf[BUFSZ];
    vsprintf(pbuf, line, the_args);
    raw_print(pbuf);
  }
}

/*VARARGS1*/
void impossible(const char *s, ...) {
  va_list args;
  va_start(args, s);
  if (program_state.in_impossible)
    panic("impossible called impossible");
  program_state.in_impossible = 1;
  {
    char pbuf[BUFSZ];
    vsprintf(pbuf, s, args);
    paniclog("impossible", pbuf);
  }
  vpline(s, args);
  pline("Program in disorder - perhaps you'd better #quit.");
  program_state.in_impossible = 0;
  va_end(args);
}

const char *align_str(aligntyp alignment) {
  switch ((int)alignment) {
    case A_CHAOTIC:
      return "chaotic";
    case A_NEUTRAL:
      return "neutral";
    case A_LAWFUL:
      return "lawful";
    case A_NONE:
      return "unaligned";
  }
  return "unknown";
}

void mstatusline(Monster *mtmp) {
  aligntyp alignment;
  char info[BUFSZ], monnambuf[BUFSZ];

  if (mtmp->ispriest || mtmp->data == &mons[PM_ALIGNED_PRIEST] ||
      mtmp->data == &mons[PM_ANGEL])
    alignment = EPRI(mtmp)->shralign;
  else
    alignment = mtmp->data->maligntyp;
  alignment =
      (alignment > 0) ? A_LAWFUL : (alignment < 0) ? A_CHAOTIC : A_NEUTRAL;

  info[0] = 0;
  if (mtmp->mtame) {
    strcat(info, ", tame");
#ifdef WIZARD
    if (wizard) {
      sprintf(eos(info), " (%d", mtmp->mtame);
      if (!mtmp->isminion)
        sprintf(eos(info), "; hungry %ld; apport %d", EDOG(mtmp)->hungrytime,
                EDOG(mtmp)->apport);
      strcat(info, ")");
    }
#endif
  } else if (mtmp->mpeaceful)
    strcat(info, ", peaceful");
  if (mtmp->meating)
    strcat(info, ", eating");
  if (mtmp->mcan)
    strcat(info, ", cancelled");
  if (mtmp->mconf)
    strcat(info, ", confused");
  if (mtmp->mblinded || !mtmp->mcansee)
    strcat(info, ", blind");
  if (mtmp->mstun)
    strcat(info, ", stunned");
  if (mtmp->msleeping)
    strcat(info, ", asleep");
#if 0 /* unfortunately mfrozen covers temporary sleep and being busy \
         (donning armor, for instance) as well as paralysis */
	else if (mtmp->mfrozen)	  strcat(info, ", paralyzed");
#else
  else if (mtmp->mfrozen || !mtmp->mcanmove)
    strcat(info, ", can't move");
#endif
  /* [arbitrary reason why it isn't moving] */
  else if (mtmp->mstrategy & STRAT_WAITMASK)
    strcat(info, ", meditating");
  else if (mtmp->mflee)
    strcat(info, ", scared");
  if (mtmp->mtrapped)
    strcat(info, ", trapped");
  if (mtmp->mspeed)
    strcat(info, mtmp->mspeed == MFAST ? ", fast" : mtmp->mspeed == MSLOW
                                                        ? ", slow"
                                                        : ", ???? speed");
  if (mtmp->mundetected)
    strcat(info, ", concealed");
  if (mtmp->minvis)
    strcat(info, ", invisible");
  if (mtmp == player.ustuck)
    strcat(info, (sticks(youmonst.data))
                     ? ", held by you"
                     : player.uswallow
                           ? (is_animal(player.ustuck->data) ? ", swallowed you"
                                                             : ", engulfed you")
                           : ", holding you");
#ifdef STEED
  if (mtmp == player.usteed)
    strcat(info, ", carrying you");
#endif

  /* avoid "Status of the invisible newt ..., invisible" */
  /* and unlike a normal mon_nam, use "saddled" even if it has a name */
  strcpy(monnambuf, x_monnam(mtmp, ARTICLE_THE, nullptr,
                             (SUPPRESS_IT | SUPPRESS_INVISIBLE), FALSE));

  pline("Status of %s (%s):  Level %d  HP %d(%d)  AC %d%s.", monnambuf,
        align_str(alignment), mtmp->m_lev, mtmp->mhp, mtmp->mhpmax,
        find_mac(mtmp), info);
}

void ustatusline() {
  char info[BUFSZ];

  info[0] = '\0';
  if (Sick) {
    strcat(info, ", dying from");
    if (player.usick_type & SICK_VOMITABLE)
      strcat(info, " food poisoning");
    if (player.usick_type & SICK_NONVOMITABLE) {
      if (player.usick_type & SICK_VOMITABLE)
        strcat(info, " and");
      strcat(info, " illness");
    }
  }
  if (Stoned)
    strcat(info, ", solidifying");
  if (Slimed)
    strcat(info, ", becoming slimy");
  if (Strangled)
    strcat(info, ", being strangled");
  if (Vomiting)
    strcat(info, ", nauseated"); /* !"nauseous" */
  if (Confusion)
    strcat(info, ", confused");
  if (Blind) {
    strcat(info, ", blind");
    if (player.ucreamed) {
      if ((long)player.ucreamed < Blinded || Blindfolded ||
          !haseyes(youmonst.data))
        strcat(info, ", cover");
      strcat(info, "ed by sticky goop");
    } /* note: "goop" == "glop"; variation is intentional */
  }
  if (Stunned)
    strcat(info, ", stunned");
#ifdef STEED
  if (!player.usteed)
#endif
    if (Wounded_legs) {
      const char *what = body_part(LEG);
      if ((Wounded_legs & BOTH_SIDES) == BOTH_SIDES)
        what = makeplural(what);
      sprintf(eos(info), ", injured %s", what);
    }
  if (Glib)
    sprintf(eos(info), ", slippery %s", makeplural(body_part(HAND)));
  if (player.utrap)
    strcat(info, ", trapped");
  if (Fast)
    strcat(info, Very_fast ? ", very fast" : ", fast");
  if (player.uundetected)
    strcat(info, ", concealed");
  if (Invis)
    strcat(info, ", invisible");
  if (player.ustuck) {
    if (sticks(youmonst.data))
      strcat(info, ", holding ");
    else
      strcat(info, ", held by ");
    strcat(info, mon_nam(player.ustuck));
  }

  pline("Status of %s (%s%s):  Level %d  HP %d(%d)  AC %d%s.", plname,
        (player.ualign.record >= 20)
            ? "piously "
            : (player.ualign.record > 13)
                  ? "devoutly "
                  : (player.ualign.record > 8)
                        ? "fervently "
                        : (player.ualign.record > 3)
                              ? "stridently "
                              : (player.ualign.record == 3)
                                    ? ""
                                    : (player.ualign.record >= 1)
                                          ? "haltingly "
                                          : (player.ualign.record == 0)
                                                ? "nominally "
                                                : "insufficiently ",
        align_str(player.ualign.type),
        Upolyd ? mons[player.umonnum].mlevel : player.ulevel,
        Upolyd ? player.mh : player.uhp, Upolyd ? player.mhmax : player.uhpmax,
        player.uac, info);
}

void self_invis_message() {
  pline(
      "%s %s.",
      Hallucination ? "Far out, man!  You" : "Gee!  All of a sudden, you",
      See_invisible ? "can see right through yourself" : "can't see yourself");
}

/*pline.c*/
