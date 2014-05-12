/*
 * botl.h
 *
 *  Created on: May 9, 2014
 *      Author: brianchin
 */

#ifndef BOTL_H_
#define BOTL_H_

#include "core/config.h"

extern int xlev_to_rank(int);
extern int title_to_mon(const char *, int *, int *);
extern void max_rank_sz();
#ifdef SCORE_ON_BOTL
extern long botl_score();
#endif
extern int describe_level(char *);
extern const char *rank_of(int, short, bool);
extern void bot();
#ifdef DUMP_LOG
extern void bot1str(char *);
extern void bot2str(char *);
#endif

#endif /* BOTL_H_ */
