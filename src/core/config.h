/*	SCCS Id: @(#)config.h	3.4	2003/12/06	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef CONFIG_H /* make sure the compiler does not see the typedefs twice */
#define CONFIG_H

#include <cstdint>

#define UNIX /* delete if no fork(), exec() available */

#define TTY_GRAPHICS /* good old tty based graphics */

#ifndef DEFAULT_WINDOW_SYS
#define DEFAULT_WINDOW_SYS "tty"
#endif

/*
 * Section 2:	Some global parameters and filenames.
 *		Commenting out WIZARD, LOGFILE, NEWS or PANICLOG removes that
 *		feature from the game; otherwise set the appropriate wizard
 *		name.  LOGFILE, NEWS and PANICLOG refer to files in the
 *		playground.
 */

#ifndef WIZARD /* allow for compile-time or Makefile changes */
#ifndef KR1ED
#define WIZARD "wizard" /* the person allowed to use the -D option */
#else
#define WIZARD
#define WIZARD_NAME "wizard"
#endif
#endif

#define LOGFILE "logfile"   /* larger file for debugging purposes */
#define XLOGFILE "xlogfile" /* even larger logfile */
#define NEWS "news"         /* the file containing the latest hack news */
#define PANICLOG "paniclog" /* log of panic and impossible events */

/*
 *	If COMPRESS is defined, it should contain the full path name of your
 *	'compress' program.  Defining INTERNAL_COMP causes NetHack to do
 *	simpler byte-stream compression internally.  Both COMPRESS and
 *	INTERNAL_COMP create smaller bones/level/save files, but require
 *	additional code and time.  Currently, only UNIX fully implements
 *	COMPRESS; other ports should be able to uncompress save files a
 *	la unixmain.c if so inclined.
 *	If you define COMPRESS, you must also define COMPRESS_EXTENSION
 *	as the extension your compressor appends to filenames after
 *	compression.
 */

/* path and file name extension for compression program */
#define COMPRESS "/usr/bin/compress" /* Lempel-Ziv compression */
#define COMPRESS_EXTENSION ".Z"      /* compress's extension */
/* An example of one alternative you might want to use: */
/* #define COMPRESS "/usr/local/bin/gzip" */ /* FSF gzip compression */
/* #define COMPRESS_EXTENSION ".gz" */       /* normal gzip extension */

/*
 *	Data librarian.  Defining DLB places most of the support files into
 *	a tar-like file, thus making a neater installation.  See *conf.h
 *	for detailed configuration.
 */
/* #define DLB */ /* not supported on all platforms */

/*
 *	Defining INSURANCE slows down level changes, but allows games that
 *	died due to program or system crashes to be resumed from the point
 *	of the last level change, after running a utility program.
 */
#define INSURANCE /* allow crashed game recovery */

/*
 * If you define HACKDIR, then this will be the default playground;
 * otherwise it will be the current directory.
 */
#ifndef HACKDIR
#define HACKDIR "/usr/games/lib/nethackdir"
#endif

/*
 * Some system administrators are stupid enough to make Hack suid root
 * or suid daemon, where daemon has other powers besides that of reading or
 * writing Hack files.	In such cases one should be careful with chdir's
 * since the user might create files in a directory of his choice.
 * Of course SECURE is meaningful only if HACKDIR is defined.
 */
/* #define SECURE */ /* do setuid(getuid()) after chdir() */

/*
 * If it is desirable to limit the number of people that can play Hack
 * simultaneously, define HACKDIR, SECURE and MAX_NR_OF_PLAYERS.
 * #define MAX_NR_OF_PLAYERS 6
 */

/*
 * Section 3:	Definitions that may vary with system type.
 *		For example, both schar and uchar should be short ints on
 *		the AT&T 3B2/3B5/etc. family.
 */

/*
 * Uncomment the following line if your compiler doesn't understand the
 * 'void' type (and thus would give all sorts of compile errors without
 * this definition).
 */
/* #define NOVOID */ /* define if no "void" data type. */

/*
 * Uncomment the following line if your compiler falsely claims to be
 * a standard C compiler (i.e., defines __STDC__ without cause).
 * Examples are Apollo's cc (in some versions) and possibly SCO UNIX's rcc.
 */
/* #define NOTSTDC */ /* define for lying compilers */

#include "core/tradstdc.h"

/*
 * type schar: small signed integers (8 bits suffice) (eg. TOS)
 *
 *	typedef char	schar;
 *
 *	will do when you have signed characters; otherwise use
 *
 *	typedef short int schar;
 */
typedef std::int8_t schar;

/*
 * type uchar: small unsigned integers (8 bits suffice - but 7 bits do not)
 *
 *	typedef unsigned char	uchar;
 *
 *	will be satisfactory if you have an "unsigned char" type;
 *	otherwise use
 *
 *	typedef unsigned short int uchar;
 */
#ifndef _AIX32 /* identical typedef in system file causes trouble */
typedef std::uint8_t uchar;
#endif

/*
 * Various structures have the option of using bitfields to save space.
 * If your C compiler handles bitfields well (e.g., it can initialize structs
 * containing bitfields), you can define BITFIELDS.  Otherwise, the game will
 * allocate a separate character for each bitfield.  (The bitfields used never
 * have more than 7 bits, and most are only 1 bit.)
 */
#define BITFIELDS /* Good bitfield handling */

/* #define STRNCMPI */ /* compiler/library has the strncmpi function */

/*
 * There are various choices for the NetHack vision system.  There is a
 * choice of two algorithms with the same behavior.  Defining VISION_TABLES
 * creates huge (60K) tables at compile time, drastically increasing data
 * size, but runs slightly faster than the alternate algorithm.  (MSDOS in
 * particular cannot tolerate the increase in data size; other systems can
 * flip a coin weighted to local conditions.)
 *
 * If VISION_TABLES is not defined, things will be faster if you can use
 * MACRO_CPATH.  Some cpps, however, cannot deal with the size of the
 * functions that have been macroized.
 */

/* #define VISION_TABLES */ /* use vision tables generated at compile time */
#ifndef VISION_TABLES
#ifndef NO_MACRO_CPATH
#define MACRO_CPATH /* use clear_path macros instead of functions */
#endif
#endif

/*
 * Section 4:  THE FUN STUFF!!!
 *
 * Conditional compilation of special options are controlled here.
 * If you define the following flags, you will add not only to the
 * complexity of the game but also to the size of the load module.
 */

/* dungeon features */
#define SINKS /* Kitchen sinks - Janet Walz */
/* dungeon levels */
#define WALLIFIED_MAZE /* Fancy mazes - Jean-Christophe Collet */
#define REINCARNATION  /* Special Rogue-like levels */
/* monsters & objects */
#define KOPS    /* Keystone Kops by Scott R. Turner */
#define SEDUCE  /* Succubi/incubi seduction, by KAA, suggested by IM */
#define STEED   /* Riding steeds */
#define TOURIST /* Tourist players with cameras and Hawaiian shirts */
/* difficulty */
#define ELBERETH /* Engraving the E-word repels monsters */
/* I/O */
#define REDO     /* support for redoing last command - DGK */
#define CLIPPING /* allow smaller screens -- ERS */

#ifdef REDO
#define DOAGAIN '\001' /* ^A, the "redo" key used in cmd.c and getline.c */
#endif

#define EXP_ON_BOTL         /* Show experience on bottom line */
/* #define SCORE_ON_BOTL */ /* added by Gary Erickson (erickson@ucivax) */

/* #define REALTIME_ON_BOTL */ /* Show elapsed time on bottom line.  Note:
                                * this breaks savefile compatibility. */

/* The options in this section require the extended logfile support */
#ifdef XLOGFILE
#define RECORD_CONDUCT        /* Record conducts kept in logfile */
#define RECORD_TURNS          /* Record turns elapsed in logfile */
#define RECORD_ACHIEVE        /* Record certain notable achievements in the         \
                               * logfile.  Note: this breaks savefile compatibility \
                               * due to the addition of the u_achieve struct. */
#define RECORD_REALTIME       /* Record the amount of actual playing time (in    \
                               * seconds) in the record file.  Note: this breaks \
                               * savefile compatibility. */
#define RECORD_START_END_TIME /* Record to-the-second starting and ending \
                               * times; stored as 32-bit values obtained  \
                               * from time(2) (seconds since the Epoch.) */
#define RECORD_GENDER0        /* Record initial gender in logfile */
#define RECORD_ALIGN0         /* Record initial alignment in logfile */
#endif

/*
 * Section 5:  EXPERIMENTAL STUFF
 *
 * Conditional compilation of new or experimental options are controlled here.
 * Enable any of these at your own risk -- there are almost certainly
 * bugs left here.
 */

#define MENU_COLOR
#define MENU_COLOR_REGEX
#define MENU_COLOR_REGEX_POSIX
/* if MENU_COLOR_REGEX is defined, use regular expressions (regex.h,
 * GNU specific functions by default, POSIX functions with
 * MENU_COLOR_REGEX_POSIX).
 * otherwise use pmatch() to match menu color lines.
 * pmatch() provides basic globbing: '*' and '?' wildcards.
 */

/*#define GOLDOBJ */ /* Gold is kept on obj chains - Helge Hafting */
/*#define AUTOPICKUP_EXCEPTIONS */ /* exceptions to autopickup */
#define DUMP_LOG                   /* Dump game end information to a file */
/* #define DUMP_FN "/tmp/%n.nh" */ /* Fixed dumpfile name, if you want
                                    * to prevent definition by users */
#define DUMPMSGS 20 /* Number of latest messages in the dump file  */

/* show{born, extinct} patch */
#define SHOW_BORN
#define SHOW_EXTINCT

/* Sortloot */
#define SORTLOOT

/* Paranoid {hit, quit, remove} */
#define PARANOID

/* HPmon, colored HP monitor */
#define HPMON

#define CONFIRM_LOCKING /* Confirmation messages for (un)locking doors or      \
                                                 * containers if only one      \
                         * container in the location                           \
                                                 * if confirm_locking is set   \
                         * in the options.                                     \
                                                 * Default is on.              \
                                                 * Can be set during the game. \
                                                 */

#define X_TWOWEAPON /* X command is changed to twoweapon; meta-x and \
                                     * #explore is for entering the  \
                     * explore-mode.                                 \
                                     */

/* End of Section 5 */

#include "core/global.h" /* Define everything else according to choices above */

#endif /* CONFIG_H */
