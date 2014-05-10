/*
 * hacklib.h
 *
 *  Created on: May 9, 2014
 *      Author: brianchin
 */

#ifndef HACKLIB_H_
#define HACKLIB_H_

#include "config.h"

bool digit(char);
bool letter(char);
char highc(char);
char lowc(char);
char *lcase(char *);
char *upstart(char *);
char *mungspaces(char *);
char *eos(char *);
char *strkitten(char *, char);
char *s_suffix(const char *);
char *xcrypt(const char *, char *);
bool onlyspace(const char *);
char *tabexpand(char *);
char *visctrl(char);
const char *ordin(int);
char *sitoa(int);
int sgn(int);
int rounddiv(long, int);
int dist2(int, int, int, int);
int distmin(int, int, int, int);
bool online2(int, int, int, int);
bool pmatch(const char *, const char *);
#ifndef STRNCMPI
int strncmpi(const char *, const char *, int);
#endif
#ifndef STRSTRI
char *strstri(const char *, const char *);
#endif
bool fuzzymatch(const char *, const char *, const char *, bool);
void setrandom();
int getyear();
#if 0
char *yymmdd(time_t);
#endif
long yyyymmdd(time_t);
int phase_of_the_moon();
bool friday_13th();
int night();
int midnight();

#endif /* HACKLIB_H_ */
