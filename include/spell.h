/*	SCCS Id: @(#)spell.h	3.4	1995/06/01	*/
/* Copyright 1986, M. Stephenson				  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef SPELL_H
#define SPELL_H

struct spell {
  short sp_id;  /* spell id (== object.otyp) */
  xchar sp_lev; /* power level */
  int sp_know;  /* knowlege of spell */
};

/* levels of memory destruction with a scroll of amnesia */
#define ALL_MAP 0x1
#define ALL_SPELLS 0x2

#define decrnknow(spell) spl_book[spell].sp_know--
#define spellid(spell) spl_book[spell].sp_id
#define spellknow(spell) spl_book[spell].sp_know


int study_book(Object *);
void book_disappears(Object *);
void book_substitution(Object *, Object *);
void age_spells();
int docast();
int spell_skilltype(int);
int spelleffects(int, bool);
void losespells();
int dovspell();
void initialspell(Object *);

#endif /* SPELL_H */
