/*	SCCS Id: @(#)config1.h	3.4	1999/12/05	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef CONFIG1_H
#define CONFIG1_H

/*
 * MS DOS - compilers
 *
 * Microsoft C auto-defines MSDOS,
 * Borland C   auto-defines __MSDOS__,
 * DJGPP       auto-defines MSDOS.
 */

#ifdef __TURBOC__
# error "Don't support!"
# define __MSC		/* increase Borland C compatibility in libraries */
#endif

/*
 * Mac Stuff.
 */
#ifdef macintosh	/*	Auto-defined symbol for MPW compilers (sc and mrc) */
# error "Don't support!"
# define MAC
#endif

#ifdef THINK_C		/* Think C auto-defined symbol */
# error "Don't support!"
# define MAC
# define NEED_VARARGS
#endif

#ifdef __MWERKS__	/* defined by Metrowerks' Codewarrior compiler */
# error "Don't support!"
# ifndef __BEOS__	/* BeOS */
#  define MAC
# endif
# define NEED_VARARGS
# define USE_STDARG
#endif

#if defined(MAC) || defined(__BEOS__)
# error "Don't support!"
# define DLB
# undef UNIX
#endif

#ifdef __BEOS__
# error "Don't support!"
# define NEED_VARARGS
#endif

/*
 * Windows NT Autodetection
 */
#ifdef _WIN32_WCE
# error "Don't support!"
#define WIN_CE
# ifndef WIN32
# define WIN32
# endif
#endif

#ifdef WIN32
# error "Don't support!"
# undef UNIX
# define NHSTDC
# define USE_STDARG
# define NEED_VARARGS

#ifndef WIN_CE
# define STRNCMPI
# define STRCMPI
#endif

#endif


#if defined(__linux__) && defined(__GNUC__) && !defined(_GNU_SOURCE)
# error "Don't support!"
/* ensure _GNU_SOURCE is defined before including any system headers */
# define _GNU_SOURCE
#endif

#ifdef vax
/* just in case someone thinks a DECstation is a vax. It's not, it's a mips */
# ifdef ULTRIX_PROTO
#  undef ULTRIX_PROTO
# endif
# ifdef ULTRIX_CC20
#  undef ULTRIX_CC20
# endif
#endif

#ifdef KR1ED		/* For compilers which cannot handle defined() */
#define defined(x) (-x-1 != -1)
/* Because:
 * #define FOO => FOO={} => defined( ) => (-1 != - - 1) => 1
 * #define FOO 1 or on command-line -DFOO
 *	=> defined(1) => (-1 != - 1 - 1) => 1
 * if FOO isn't defined, FOO=0. But some compilers default to 0 instead of 1
 * for -DFOO, oh well.
 *	=> defined(0) => (-1 != - 0 - 1) => 0
 *
 * But:
 * defined("") => (-1 != - "" - 1)
 *   [which is an unavoidable catastrophe.]
 */
#endif

#endif	/* CONFIG1_H */
