
// TODO(BNC): Add Docs
#ifndef MONMOVE_H_
#define MONMOVE_H_

bool itsstuck(Monster *);
bool mb_trapped(Monster *);
void mon_regen(Monster *, bool);
int dochugw(Monster *);
bool onscary(int, int, Monster *);
void monflee(Monster *, int, bool, bool);
int dochug(Monster *);
int m_move(Monster *, int);
bool closed_door(int, int);
bool accessible(int, int);
void set_apparxy(Monster *);
bool can_ooze(Monster *);


#endif  // MONMOVE_H_
