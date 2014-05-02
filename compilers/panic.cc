/*	SCCS Id: @(#)panic.c	3.4	1994/03/02	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/*
 *	This code was adapted from the code in end.c to run in a standalone
 *	mode for the makedefs / drg code.
 */

#include "config.h"
#include <stdarg.h>

/*VARARGS1*/
bool panicking;
void raw_panic(char const*,...);

void raw_panic(char const* str, ...) {
  va_list args;
  va_start(args, str);
	if(panicking)
		abort();    /* avoid loops - this should never happen*/
	panicking = true;
	fputs(" ERROR:  ", stderr);
	vfprintf(stderr, str, args);
	fflush(stderr);
	abort();	/* generate core dump */
	va_end(args);
	exit(EXIT_FAILURE);		/* redundant */
	return;
}

#ifdef ALLOCA_HACK
/*
 * In case bison-generated foo_yacc.c tries to use alloca(); if we don't
 * have it then just use malloc() instead.  This may not work on some
 * systems, but they should either use yacc or get a real alloca routine.
 */
long *alloca(cnt)
unsigned cnt;
{
	return cnt ? alloc(cnt) : (long *)0;
}
#endif

/*panic.c*/
