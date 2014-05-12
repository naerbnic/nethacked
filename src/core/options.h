
// TODO(BNC): Add Docs
#ifndef OPTIONS_H_
#define OPTIONS_H_

bool match_optname(const char *, const char *, int, bool);
void initoptions();
void parseoptions(char *, bool, bool);
int doset();
int dotogglepickup();
void option_help();
void next_opt(winid, const char *);
int fruitadd(char *);
int choose_classes_menu(const char *, int, bool, char *, char *);
void add_menu_cmd_alias(char, char);
char map_menu_cmd(char);
void assign_warnings(uchar *);
char *nh_getenv(const char *);
void set_duplicate_opt_detection(int);
void set_wc_option_mod_status(unsigned long, int);
void set_wc2_option_mod_status(unsigned long, int);
void set_option_mod_status(const char *, int);
#ifdef AUTOPICKUP_EXCEPTIONS
int add_autopickup_exception(const char *);
void free_autopickup_exceptions();
#endif /* AUTOPICKUP_EXCEPTIONS */
#ifdef MENU_COLOR
bool add_menu_coloring(char *);
#endif /* MENU_COLOR */


#endif  // OPTIONS_H_
