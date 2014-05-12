
// TODO(BNC): Add Docs
#ifndef ROLE_H_
#define ROLE_H_

bool validrole(int);
bool validrace(int, int);
bool validgend(int, int, int);
bool validalign(int, int, int);
int randrole();
int randrace(int);
int randgend(int, int);
int randalign(int, int);
int str2role(char *);
int str2race(char *);
int str2gend(char *);
int str2align(char *);
bool ok_role(int, int, int, int);
int pick_role(int, int, int, int);
bool ok_race(int, int, int, int);
int pick_race(int, int, int, int);
bool ok_gend(int, int, int, int);
int pick_gend(int, int, int, int);
bool ok_align(int, int, int, int);
int pick_align(int, int, int, int);
void role_init();
void rigid_role_checks();
void plnamesuffix();
const char *Hello(Monster *);
const char *Goodbye();
char *build_plselection_prompt(char *, int, int, int, int, int);
char *root_plselection_prompt(char *, int, int, int, int, int);


#endif  // ROLE_H_
