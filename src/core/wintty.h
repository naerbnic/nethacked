/*	SCCS Id: @(#)wintty.h	3.4	1996/02/18	*/
/* Copyright (c) David Cohrs, 1991,1992				  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef WINTTY_H
#define WINTTY_H

#define E extern

#ifndef WINDOW_STRUCTS
#define WINDOW_STRUCTS

/* menu structure */
typedef struct tty_mi {
  struct tty_mi *next;
  anything identifier; /* user identifier */
  long count;          /* user count */
  char *str;           /* description string (including accelerator) */
  int attr;            /* string attribute */
  bool selected;       /* TRUE if selected by user */
  char selector;       /* keyboard accelerator */
  char gselector;      /* group accelerator */
} tty_menu_item;

/* descriptor for tty-based windows */
struct WinDesc {
  int flags;            /* window flags */
  xchar type;           /* type of window */
  bool active;          /* true if window is active */
  uchar offx, offy;     /* offset from topleft of display */
  short rows, cols;     /* dimensions */
  short curx, cury;     /* current cursor position */
  short maxrow, maxcol; /* the maximum size used -- for MENU wins */
  /* maxcol is also used by WIN_MESSAGE for */
  /* tracking the ^P command */
  short *datlen;         /* allocation size for *data */
  char **data;           /* window data [row][column] */
  char *morestr;         /* string to display instead of default */
  tty_menu_item *mlist;  /* menu information (MENU) */
  tty_menu_item **plist; /* menu page pointers (MENU) */
  short plist_size;      /* size of allocated plist (MENU) */
  short npages;          /* number of pages in menu (MENU) */
  short nitems;          /* total number of items (MENU) */
  short how;             /* menu mode - pick 1 or N (MENU) */
  char menu_ch;          /* menu char (MENU) */
};

/* window flags */
#define WIN_CANCELLED 1
#define WIN_STOP 1 /* for NHW_MESSAGE; stops output */

/* descriptor for tty-based displays -- all the per-display data */
struct DisplayDesc {
  uchar rows, cols; /* width and height of tty display */
  uchar curx, cury; /* current cursor position on the screen */
#ifdef TEXTCOLOR
  int color; /* current color */
#endif
  int attrs;         /* attributes in effect */
  int toplin;        /* flag for topl stuff */
  int rawprint;      /* number of raw_printed lines since synch */
  int inmore;        /* non-zero if more() is active */
  int inread;        /* non-zero if reading a character */
  int intr;          /* non-zero if inread was interrupted */
  winid lastwin;     /* last window used for I/O */
  char dismiss_more; /* extra character accepted at --More-- */
};

#endif /* WINDOW_STRUCTS */

#define MAXWIN 20 /* maximum number of windows, cop-out */

/* tty dependent window types */
#ifdef NHW_BASE
#undef NHW_BASE
#endif
#define NHW_BASE 6

extern struct window_procs tty_procs;

/* port specific variable declarations */
extern winid BASE_WINDOW;

extern struct WinDesc *wins[MAXWIN];

extern struct DisplayDesc *ttyDisplay; /* the tty display descriptor */

extern char morc;         /* last character typed to xwaitforspace */
extern char defmorestr[]; /* default --more-- prompt */

/* port specific external function references */

/* ### getline.c ### */
E void xwaitforspace(const char *);

/* ### termcap.c, video.c ### */

E void tty_startup(int *, int *);
#ifndef NO_TERMS
E void tty_shutdown();
#endif
E int xputc(int);
E void xputs(const char *);
#if defined(SCREEN_VGA) || defined(SCREEN_8514)
E void xputg(int, int, unsigned);
#endif
E void cl_end();
E void clear_screen();
E void home();
E void standoutbeg();
E void standoutend();
#if 0
E void revbeg();
E void boldbeg();
E void blinkbeg();
E void dimbeg();
E void m_end();
#endif
E void backsp();
E void graph_on();
E void graph_off();
E void cl_eos();

/*
 * termcap.c (or facsimiles in other ports) is the right place for doing
 * strange and arcane things such as outputting escape sequences to select
 * a color or whatever.  wintty.c should concern itself with WHERE to put
 * stuff in a window.
 */
E void term_start_attr(int attr);
E void term_end_attr(int attr);
E void term_start_raw_bold();
E void term_end_raw_bold();

#ifdef TEXTCOLOR
E void term_end_color();
E void term_start_color(int color);
E int has_color(int color);
#endif /* TEXTCOLOR */

/* ### topl.c ### */

E void addtopl(const char *);
E void more();
E void update_topl(const char *);
E void putsyms(const char *);

/* ### wintty.c ### */
#ifdef CLIPPING
E void setclipped();
#endif
E void docorner(int, int);
E void end_glyphout();
E void g_putch(int);
E void win_tty_init();

/* external declarations */
E void tty_init_nhwindows(int *, char **);
E void tty_player_selection();
E void tty_askname();
E void tty_get_nh_event();
E void tty_exit_nhwindows(const char *);
E void tty_suspend_nhwindows(const char *);
E void tty_resume_nhwindows();
E winid tty_create_nhwindow(int);
E void tty_clear_nhwindow(winid);
E void tty_display_nhwindow(winid, bool);
E void tty_dismiss_nhwindow(winid);
E void tty_destroy_nhwindow(winid);
E void tty_curs(winid, int, int);
E void tty_putstr(winid, int, const char *);
E void tty_display_file(const char *, bool);
E void tty_start_menu(winid);
E void tty_add_menu(winid, int, const ANY_P *, char, char, int, const char *,
                    bool);
E void tty_end_menu(winid, const char *);
E int tty_select_menu(winid, int, MENU_ITEM_P **);
E char tty_message_menu(char, int, const char *);
E void tty_update_inventory();
E void tty_mark_synch();
E void tty_wait_synch();
#ifdef CLIPPING
E void tty_cliparound(int, int);
#endif
#ifdef POSITIONBAR
E void tty_update_positionbar(char *);
#endif
E void tty_print_glyph(winid, xchar, xchar, int);
E void tty_raw_print(const char *);
E void tty_raw_print_bold(const char *);
E int tty_nhgetch();
E int tty_nh_poskey(int *, int *, int *);
E void tty_nhbell();
E int tty_doprev_message();
E char TtyAskYesNoQuestion(const char *, const char *, char);
E void tty_getlin(const char *, char *);
E int tty_get_ext_cmd();
E void tty_number_pad(int);
E void tty_delay_output();
#ifdef CHANGE_COLOR
E void tty_change_color(int color, long rgb, int reverse);
E char *tty_get_color_string();
#endif

/* other defs that really should go away (they're tty specific) */
E void tty_start_screen();
E void tty_end_screen();

E void genl_outrip(winid, int);

#undef E

#endif /* WINTTY_H */
