/*	SCCS Id: @(#)global.h	3.4	2003/08/31	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <cstdint>

#include "core/config.h"

/* #define BETA	*/ /* if a beta-test copy	[MRS] */

/*
 * Files expected to exist in the playground directory.
 */

#define RECORD "record"        /* file containing list of topscorers */
#define HELP "help"            /* file containing command descriptions */
#define SHELP "hh"             /* abbreviated form of the same */
#define DEBUGHELP "wizhelp"    /* file containing debug mode cmds */
#define RUMORFILE "rumors"     /* file with fortune cookies */
#define ORACLEFILE "oracles"   /* file with oracular information */
#define DATAFILE "data"        /* file giving the meaning of symbols used */
#define CMDHELPFILE "cmdhelp"  /* file telling what commands do */
#define HISTORY "history"      /* file giving nethack's history */
#define LICENSE "license"      /* file with license information */
#define OPTIONFILE "opthelp"   /* file explaining runtime options */
#define OPTIONS_USED "options" /* compile-time options, for #version */

#define LEV_EXT ".lev" /* extension for special level files */

/* Assorted definitions that may depend on selections in config.h. */

/*
 * type xchar: small integers in the range 0 - 127, usually coordinates
 * although they are nonnegative they must not be declared unsigned
 * since otherwise comparisons with signed quantities are done incorrectly
 */
typedef schar xchar;

#ifndef TRUE /* defined in some systems' native include files */
#define TRUE true
#define FALSE false
#endif

#ifndef STRNCMPI
#ifndef __SASC_60 /* SAS/C already shifts to stricmp */
#define strcmpi(a, b) strncmpi((a), (b), -1)
#endif
#endif

/* comment out to test effects of each #define -- these will probably
 * disappear eventually
 */
#ifdef INTERNAL_COMP
#define RLECOMP  /* run-length compression of levl array - JLee */
#define ZEROCOMP /* zero-run compression of everything - Olaf Seibert */
#endif

/* #define SPECIALIZATION */ /* do "specialized" version of new topology */

#ifdef BITFIELDS
#define Bitfield(x, n) unsigned x : n
#else
#define Bitfield(x, n) uchar x
#endif

#define ALIGNTYP_P aligntyp

#define SIZE(x) (int)(sizeof(x) / sizeof(x[0]))

/* A limit for some NetHack int variables.  It need not, and for comparable
 * scoring should not, depend on the actual limit on integers for a
 * particular machine, although it is set to the minimum required maximum
 * signed integer for C (2^15 -1).
 */
#define LARGEST_INT 32767

#ifdef REDO
#define Getchar pgetchar
#endif

#include "core/coord.h"
/*
 * Automatic inclusions for the subsidiary files.
 * Please don't change the order.  It does matter.
 */

#include "core/unixconf.h"

/* Displayable name of this port; don't redefine if defined in *conf.h */
#define PORT_ID "Unix"

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

/* Used for consistency checks of various data files; declare it here so
   that utility programs which include config.h but not hack.h can see it. */
struct version_info {
  unsigned long incarnation;  /* actual version number */
  unsigned long feature_set;  /* bitmask of config settings */
  unsigned long entity_count; /* # of monsters and objects */
  unsigned long struct_sizes; /* size of key structs */
};

/*
 * Configurable internal parameters.
 *
 * Please be very careful if you are going to change one of these.  Any
 * changes in these parameters, unless properly done, can render the
 * executable inoperative.
 */

/* size of terminal screen is (at least) (ROWNO+3) by COLNO */
constexpr int COLNO = 80;
constexpr int ROWNO = 21;

constexpr int MAXNROFROOMS = 40; /* max number of rooms per level */
constexpr int MAX_SUBROOMS = 24; /* max # of subrooms in a given room */
constexpr int DOORMAX = 120;     /* max number of doors per level */

constexpr int BUFSZ = 256;  /* for getlin buffers */
constexpr int QBUFSZ = 128; /* for building question text */
constexpr int TBUFSZ = 300; /* toplines[] buffer max msg: 3 81char names */
                            /* plus longest prefix plus a few extra words */

constexpr int PL_NSIZ = 32; /* name of player, ghost, shopkeeper */
constexpr int PL_CSIZ = 32; /* sizeof pl_character */
constexpr int PL_FSIZ = 32; /* fruit name */
constexpr int PL_PSIZ = 63; /* player-given names for pets, other
                             * monsters, objects */

constexpr int MAXDUNGEON = 16; /* current maximum number of dungeons */
constexpr int MAXLEVEL = 32;   /* max number of levels in one dungeon */
constexpr int MAXSTAIRS = 1;   /* max # of special stairways in a dungeon */
constexpr int ALIGNWEIGHT = 4; /* generation weight of alignment */

constexpr int MAXULEV = 30; /* max character experience level */

constexpr int MAXMONNO = 120; /* extinct monst after this number created */
constexpr int MHPMAX = 500;   /* maximum monster hp */

#endif /* GLOBAL_H */
