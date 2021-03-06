/*	SCCS Id: @(#)decl.c	3.2	2001/12/10	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "core/decl.h"

#include "core/hack.h"
#include "core/mkroom.h"

int (*afternmv)();
int (*occupation)();

/* from xxxmain.c */
const char *hname = 0; /* name of the game (argv[0] of main) */
int hackpid = 0;       /* current process id */
int locknum = 0;       /* max num of simultaneous users */
#ifdef DEF_PAGER
char *catmore = 0; /* default pager */
#endif

// Keeps the base indexes of the first item of that class in
// the objects[] array.
int bases[MAXOCLASSES] = DUMMY;

int multi = 0;
char multi_txt[BUFSZ] = DUMMY;
#if 0
int warnlevel = 0;		/* used by movemon and dochugw */
#endif
int nroom = 0;
int nsubroom = 0;
int occtime = 0;

int x_maze_max, y_maze_max; /* initialized in main, used in mkmaze.c */
int otg_temp;               /* used by object_to_glyph() [otg] */

#ifdef REDO
int in_doagain = 0;
#endif

/*
 *	The following structure will be initialized at startup time with
 *	the level numbers of some "important" things in the game.
 */
struct dgn_topology dungeon_topology = {DUMMY};

#include "core/quest.h"
struct q_score quest_status = DUMMY;

int smeq[MAXNROFROOMS + 1] = DUMMY;
int doorindex = 0;

char *save_cm = 0;
int killer_format = 0;
const char *killer = 0;
const char *delayed_killer = 0;
#ifdef GOLDOBJ
long done_money = 0;
#endif
char killer_buf[BUFSZ] = DUMMY;
const char *nomovemsg = 0;
const char nul[40] = DUMMY;   /* contains zeros */
char plname[PL_NSIZ] = DUMMY; /* player name */
char pl_character[PL_CSIZ] = DUMMY;
char pl_race = '\0';

char pl_fruit[PL_FSIZ] = DUMMY;
int current_fruit = 0;
struct fruit *ffruit = (struct fruit *)0;

char tune[6] = DUMMY;

const char *occtxt = DUMMY;
const char quitchars[] = " \r\n\033";
const char vowels[] = "aeiouAEIOU";
const char ynchars[] = "yn";
const char ynqchars[] = "ynq";
const char ynaqchars[] = "ynaq";
const char ynNaqchars[] = "yn#aq";
long yn_number = 0L;

const char disclosure_options[] = "iavgc";

struct linfo level_info[MAXLINFO];

struct sinfo program_state;

/* 'rogue'-like direction commands (cmd.c) */
const char sdir[] = "hykulnjb><";
const char ndir[] = "47896321><"; /* number pad mode */
const schar xdir[10] = {-1, -1, 0, 1, 1, 1, 0, -1, 0, 0};
const schar ydir[10] = {0, -1, -1, -1, 0, 1, 1, 1, 0, 0};
const schar zdir[10] = {0, 0, 0, 0, 0, 0, 0, 0, 1, -1};

schar tbx = 0, tby = 0; /* mthrowu: target */

/* for xname handling of multiple shot missile volleys:
   number of shots, index of current one, validity check, shoot vs throw */
struct multishot m_shot = {0, 0, STRANGE_OBJECT, FALSE};

struct dig_info digging;

dungeon dungeons[MAXDUNGEON]; /* ini'ed by init_dungeon() */
s_level *sp_levchn;
stairway upstair = {0, 0}, dnstair = {0, 0};
stairway upladder = {0, 0}, dnladder = {0, 0};
stairway sstairs = {0, 0};
dest_area updest = {0, 0, 0, 0, 0, 0, 0, 0};
dest_area dndest = {0, 0, 0, 0, 0, 0, 0, 0};
coord inv_pos = {0, 0};

bool in_mklev = FALSE;
bool stoned = FALSE; /* done to monsters hit by 'c' */
bool unweapon = FALSE;
bool mrg_to_wielded = FALSE;
/* weapon picked is merged with wielded one */
Object *current_wand = 0; /* wand currently zapped/applied */

bool in_steed_dismounting = FALSE;

coord bhitpos = DUMMY;
coord doors[DOORMAX] = {DUMMY};

struct mkroom rooms[(MAXNROFROOMS + 1) * 2] = {DUMMY};
std::vector<struct mkroom> subrooms;
struct mkroom *upstairs_room, *dnstairs_room, *sstairs_room;

dlevel_t level; /* level map */
Trap *ftrap = nullptr;
Monster youmonst = DUMMY;
struct flag flags = DUMMY;
struct instance_flags iflags = DUMMY;
struct Player player = DUMMY;

Object *invent = nullptr, *uwep = nullptr, *uarm = nullptr, *uswapwep = nullptr,
       *uquiver = nullptr, /* quiver */
#ifdef TOURIST
    *uarmu = nullptr, /* under-wear, so to speak */
#endif
    *uskin = nullptr, /* dragon armor, if a dragon */
    *uarmc = nullptr, *uarmh = nullptr, *uarms = nullptr, *uarmg = nullptr,
       *uarmf = nullptr, *uamul = nullptr, *uright = nullptr, *uleft = nullptr,
       *ublindf = nullptr, *uchain = nullptr, *uball = nullptr;

#ifdef TEXTCOLOR
/*
 *  This must be the same order as used for buzz() in zap.c.
 */
const int zapcolors[NUM_ZAP] = {
    HI_ZAP,     /* 0 - missile */
    CLR_ORANGE, /* 1 - fire */
    CLR_WHITE,  /* 2 - frost */
    HI_ZAP,     /* 3 - sleep */
    CLR_BLACK,  /* 4 - death */
    CLR_WHITE,  /* 5 - lightning */
    CLR_YELLOW, /* 6 - poison gas */
    CLR_GREEN,  /* 7 - acid */
};
#endif /* text color */

const int shield_static[SHIELD_COUNT] = {
    S_ss1, S_ss2, S_ss3, S_ss2, S_ss1, S_ss2, S_ss4, /* 7 per row */
    S_ss1, S_ss2, S_ss3, S_ss2, S_ss1, S_ss2, S_ss4,
    S_ss1, S_ss2, S_ss3, S_ss2, S_ss1, S_ss2, S_ss4,
};

struct spell spl_book[MAXSPELL + 1] = {DUMMY};

long moves = 1L, monstermoves = 1L;
/* These diverge when player is Fast */
long wailmsg = 0L;

/* objects that are moving to another dungeon level */
Object *migrating_objs = nullptr;
/* objects not yet paid for */
Object *billobjs = nullptr;

/* used to zero all elements of a struct obj */
Object zeroobj = DUMMY;

/* originally from dog.c */
char dogname[PL_PSIZ] = DUMMY;
char catname[PL_PSIZ] = DUMMY;
char horsename[PL_PSIZ] = DUMMY;
char preferred_pet; /* '\0', 'c', 'd', 'n' (none) */
/* monsters that went down/up together with @ */
Monster *mydogs = nullptr;
/* monsters that are moving to another dungeon level */
Monster *migrating_mons = nullptr;

struct mvitals mvitals[NUMMONS];

/* originally from end.c */
#ifdef DUMP_LOG
#ifdef DUMP_FN
char dump_fn[] = DUMP_FN;
#else
char dump_fn[PL_PSIZ] = DUMMY;
#endif
#endif /* DUMP_LOG */

struct c_color_names c_color_names = {"black",  "amber", "golden", "light blue",
                                      "red",    "green", "silver", "blue",
                                      "purple", "white"};

const char *c_obj_colors[] = {
    "black",          /* CLR_BLACK */
    "red",            /* CLR_RED */
    "green",          /* CLR_GREEN */
    "brown",          /* CLR_BROWN */
    "blue",           /* CLR_BLUE */
    "magenta",        /* CLR_MAGENTA */
    "cyan",           /* CLR_CYAN */
    "gray",           /* CLR_GRAY */
    "transparent",    /* no_color */
    "orange",         /* CLR_ORANGE */
    "bright green",   /* CLR_BRIGHT_GREEN */
    "yellow",         /* CLR_YELLOW */
    "bright blue",    /* CLR_BRIGHT_BLUE */
    "bright magenta", /* CLR_BRIGHT_MAGENTA */
    "bright cyan",    /* CLR_BRIGHT_CYAN */
    "white",          /* CLR_WHITE */
};

#ifdef MENU_COLOR
struct menucoloring *menu_colorings = 0;
#endif

struct c_common_strings c_common_strings = {"Nothing happens.",
                                            "That's enough tries!",
                                            "That is a silly thing to %s.",
                                            "shudder for a moment.",
                                            "something",
                                            "Something",
                                            "You can move again.",
                                            "Never mind.",
                                            "vision quickly clears.",
                                            {"the", "your"}};

/* NOTE: the order of these words exactly corresponds to the
   order of oc_material values #define'd in objclass.h. */
const char *materialnm[] = {
    "mysterious", "liquid",  "wax",      "organic", "flesh",      "paper",
    "cloth",      "leather", "wooden",   "bone",    "dragonhide", "iron",
    "metal",      "copper",  "silver",   "gold",    "platinum",   "mithril",
    "plastic",    "glass",   "gemstone", "stone"};

/* Vision */
bool vision_full_recalc = 0;
char **viz_array = 0; /* used in cansee() and couldsee() macros */

/* Global windowing data, defined here for multi-window-system support */
winid WIN_MESSAGE = WIN_ERR, WIN_STATUS = WIN_ERR;
winid WIN_MAP = WIN_ERR, WIN_INVEN = WIN_ERR;
char toplines[TBUFSZ];
/* Windowing stuff that's really tty oriented, but present for all ports */
struct tc_gbl_data tc_gbl_data = {0, 0, 0, 0}; /* AS,AE, LI,CO */

char *fqn_prefix[PREFIX_COUNT] = {nullptr, nullptr, nullptr,
                                  nullptr, nullptr, nullptr,
                                  nullptr, nullptr, nullptr};

#ifdef PREFIXES_IN_USE
char *fqn_prefix_names[PREFIX_COUNT] = {"hackdir",  "leveldir",  "savedir",
                                        "bonesdir", "datadir",   "scoredir",
                                        "lockdir",  "configdir", "troubledir"};
#endif

struct _plinemsg *pline_msg = NULL;

#ifdef RECORD_ACHIEVE
struct u_achieve achieve = DUMMY;
#endif

#if defined(RECORD_REALTIME) || defined(REALTIME_ON_BOTL)
struct realtime_data realtime_data = {0, 0, 0};
#endif

/* dummy routine used to force linkage */
void decl_init() { return; }

/*decl.c*/
