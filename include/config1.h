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
