/*	SCCS Id: @(#)alloc.c	3.4	1995/10/04	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "alloc.h"

/* since this file is also used in auxiliary programs, don't include all the
 * function declarations for all of nethack
 */
#include "config.h"

#if defined(WIZARD)
char *fmt_ptr(const void*,char *);
#endif

long *alloc(unsigned int);


long* alloc(unsigned int lth)
{
	genericptr_t ptr;

	ptr = malloc(lth);
	return((long *) ptr);
}


#if defined(WIZARD)

# define PTR_FMT "%p"
# define PTR_TYP genericptr_t

/* format a pointer for display purposes; caller supplies the result buffer */
char * fmt_ptr(const void* ptr, char *buf) {
	sprintf(buf, PTR_FMT, (PTR_TYP)ptr);
	return buf;
}

#endif


/*alloc.c*/
