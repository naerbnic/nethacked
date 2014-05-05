/*	SCCS Id: @(#)windows.c	3.4	1996/05/19	*/
/* Copyright (c) D. Cohrs, 1993. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "wintty.h"

STATIC_DCL void def_raw_print(const char *s);

struct window_procs windowprocs;

static struct win_choices {
  struct window_procs *procs;
  void (*ini_routine)(); /* optional (can be 0) */
} winchoices[] = {
      {&tty_procs, win_tty_init}, {0, 0} /* must be last */
};

STATIC_OVL
void def_raw_print(const char *s) { puts(s); }

void choose_windows(const char *s) {
  int i;

  for (i = 0; winchoices[i].procs; i++)
    if (!strcmpi(s, winchoices[i].procs->name)) {
      windowprocs = *winchoices[i].procs;
      if (winchoices[i].ini_routine)
        (*winchoices[i].ini_routine)();
      return;
    }

  if (!windowprocs.win_raw_print)
    windowprocs.win_raw_print = def_raw_print;

  raw_printf("Window type %s not recognized.  Choices are:", s);
  for (i = 0; winchoices[i].procs; i++)
    raw_printf("        %s", winchoices[i].procs->name);

  if (windowprocs.win_raw_print == def_raw_print)
    terminate(EXIT_SUCCESS);
  wait_synch();
}

/*
 * tty_message_menu() provides a means to get feedback from the
 * --More-- prompt; other interfaces generally don't need that.
 */
/*ARGSUSED*/
char genl_message_menu(char let, int how, const char *mesg) {
  pline("%s", mesg);
  return 0;
}

/*ARGSUSED*/
void genl_preference_update(const char *pref) {
  /* window ports are expected to provide
     their own preference update routine
     for the preference capabilities that
     they support.
     Just return in this genl one. */
}
/*windows.c*/
