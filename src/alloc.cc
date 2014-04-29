/*	SCCS Id: @(#)alloc.c	3.4	1995/10/04	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/* to get the malloc() prototype from system.h */
#define ALLOC_C		/* comment line for pre-compiled headers */
/* since this file is also used in auxiliary programs, don't include all the
 * function declarations for all of nethack
 */
#define EXTERN_H	/* comment line for pre-compiled headers */
#include "config.h"

#if defined(WIZARD)
char *FDECL(fmt_ptr, (const genericptr,char *));
#endif

long *FDECL(alloc,(unsigned int));
extern void VDECL(panic, (const char *,...)) PRINTF_F(1,2);


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
char * fmt_ptr(const genericptr ptr, char *buf) {
	sprintf(buf, PTR_FMT, (PTR_TYP)ptr);
	return buf;
}

#endif


/*alloc.c*/
