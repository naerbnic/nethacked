/*	SCCS Id: @(#)global.h	3.4	2003/08/31	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>


/* #define BETA	*/	/* if a beta-test copy	[MRS] */

/*
 * Files expected to exist in the playground directory.
 */

#define RECORD	      "record"	/* file containing list of topscorers */
#define HELP	      "help"	/* file containing command descriptions */
#define SHELP	      "hh"	/* abbreviated form of the same */
#define DEBUGHELP     "wizhelp" /* file containing debug mode cmds */
#define RUMORFILE     "rumors"	/* file with fortune cookies */
#define ORACLEFILE    "oracles" /* file with oracular information */
#define DATAFILE      "data"	/* file giving the meaning of symbols used */
#define CMDHELPFILE   "cmdhelp" /* file telling what commands do */
#define HISTORY       "history" /* file giving nethack's history */
#define LICENSE       "license" /* file with license information */
#define OPTIONFILE    "opthelp" /* file explaining runtime options */
#define OPTIONS_USED  "options" /* compile-time options, for #version */

#define LEV_EXT ".lev"		/* extension for special level files */


/* Assorted definitions that may depend on selections in config.h. */

/*
 * type xchar: small integers in the range 0 - 127, usually coordinates
 * although they are nonnegative they must not be declared unsigned
 * since otherwise comparisons with signed quantities are done incorrectly
 */
typedef schar	xchar;

#ifndef TRUE		/* defined in some systems' native include files */
#define TRUE	((bool)1)
#define FALSE	((bool)0)
#endif

#ifndef STRNCMPI
# ifndef __SASC_60		/* SAS/C already shifts to stricmp */
#  define strcmpi(a,b) strncmpi((a),(b),-1)
# endif
#endif

/* comment out to test effects of each #define -- these will probably
 * disappear eventually
 */
#ifdef INTERNAL_COMP
# define RLECOMP	/* run-length compression of levl array - JLee */
# define ZEROCOMP	/* zero-run compression of everything - Olaf Seibert */
#endif

/* #define SPECIALIZATION */	/* do "specialized" version of new topology */


#ifdef BITFIELDS
#define Bitfield(x,n)	unsigned x:n
#else
#define Bitfield(x,n)	uchar x
#endif

#define CHAR_P char
#define SCHAR_P schar
#define UCHAR_P uchar
#define XCHAR_P xchar
#define SHORT_P short
#define ALIGNTYP_P aligntyp

#define OBJ_P Object*
#define MONST_P Monster*

#define SIZE(x) (int)(sizeof(x) / sizeof(x[0]))


/* A limit for some NetHack int variables.  It need not, and for comparable
 * scoring should not, depend on the actual limit on integers for a
 * particular machine, although it is set to the minimum required maximum
 * signed integer for C (2^15 -1).
 */
#define LARGEST_INT	32767


#ifdef REDO
#define Getchar pgetchar
#endif


#include "coord.h"
/*
 * Automatic inclusions for the subsidiary files.
 * Please don't change the order.  It does matter.
 */

#ifdef UNIX
#include "unixconf.h"
#endif

/* Displayable name of this port; don't redefine if defined in *conf.h */
# ifdef UNIX
#  define PORT_ID	"Unix"
# endif

#ifndef EXIT_SUCCESS
# define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
# define EXIT_FAILURE 1
#endif

#ifdef NEED_VARARGS
#define Vprintf  (void) vprintf
#define Vfprintf (void) vfprintf
#define Vsprintf (void) vsprintf
#endif


/* primitive memory leak debugging; see alloc.c */
extern long *FDECL(alloc, (unsigned int));		/* alloc.c */

/* Used for consistency checks of various data files; declare it here so
   that utility programs which include config.h but not hack.h can see it. */
struct version_info {
	unsigned long	incarnation;	/* actual version number */
	unsigned long	feature_set;	/* bitmask of config settings */
	unsigned long	entity_count;	/* # of monsters and objects */
	unsigned long	struct_sizes;	/* size of key structs */
};


/*
 * Configurable internal parameters.
 *
 * Please be very careful if you are going to change one of these.  Any
 * changes in these parameters, unless properly done, can render the
 * executable inoperative.
 */

/* size of terminal screen is (at least) (ROWNO+3) by COLNO */
#define COLNO	80
#define ROWNO	21

#define MAXNROFROOMS	40	/* max number of rooms per level */
#define MAX_SUBROOMS	24	/* max # of subrooms in a given room */
#define DOORMAX		120	/* max number of doors per level */

#define BUFSZ		256	/* for getlin buffers */
#define QBUFSZ		128	/* for building question text */
#define TBUFSZ		300	/* toplines[] buffer max msg: 3 81char names */
				/* plus longest prefix plus a few extra words */

#define PL_NSIZ		32	/* name of player, ghost, shopkeeper */
#define PL_CSIZ		32	/* sizeof pl_character */
#define PL_FSIZ		32	/* fruit name */
#define PL_PSIZ		63	/* player-given names for pets, other
				 * monsters, objects */

#define MAXDUNGEON	16	/* current maximum number of dungeons */
#define MAXLEVEL	32	/* max number of levels in one dungeon */
#define MAXSTAIRS	1	/* max # of special stairways in a dungeon */
#define ALIGNWEIGHT	4	/* generation weight of alignment */

#define MAXULEV		30	/* max character experience level */

#define MAXMONNO	120	/* extinct monst after this number created */
#define MHPMAX		500	/* maximum monster hp */

#endif /* GLOBAL_H */
