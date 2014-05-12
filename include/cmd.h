/*
 * cmd.h
 *
 *  Created on: May 9, 2014
 *      Author: brianchin
 */

#ifndef CMD_H_
#define CMD_H_

#include "core/config.h"

extern void reset_occupations();
extern void set_occupation(int (*)(void), const char *, int);
#ifdef REDO
extern char pgetchar();
extern void pushch(char);
extern void savech(char);
#endif
#ifdef WIZARD
extern void add_debug_extended_commands();
#endif /* WIZARD */
extern void rhack(char *);
extern int doextlist();
extern int extcmd_via_menu();
extern void enlightenment(int);
extern void show_conduct(int);
#ifdef DUMP_LOG
extern void dump_enlightenment(int);
extern void dump_conduct(int);
#endif
extern int xytod(schar, schar);
extern void dtoxy(coord *, int);
extern int movecmd(char);
extern int getdir(const char *);
extern void confdir();
extern int isok(int, int);
extern int get_adjacent_loc(const char *, const char *, xchar, xchar, coord *);
extern const char *click_to_cmd(int, int, int);
extern char readchar();
#ifdef WIZARD
extern void sanity_check();
#endif
extern char yn_function(const char *, const char *, char);

#endif /* CMD_H_ */
