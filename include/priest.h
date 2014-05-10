
// TODO(BNC): Add Docs
#ifndef PRIEST_H_
#define PRIEST_H_

int move_special(Monster *, bool, schar, bool, bool, xchar, xchar, xchar,
                   xchar);
char temple_occupied(char *);
int pri_move(Monster *);
void priestini(d_level *, struct mkroom *, int, int, bool);
char *priestname(Monster *, char *);
bool p_coaligned(Monster *);
Monster *findpriest(char);
void intemple(int);
void priest_talk(Monster *);
Monster *mk_roamer(MonsterType *, ALIGNTYP_P, xchar, xchar, bool);
void reset_hostility(Monster *);
bool in_your_sanctuary(Monster *, xchar, xchar);
void ghod_hitsu(Monster *);
void angry_priest();
void clearpriests();
void restpriest(Monster *, bool);


#endif  // PRIEST_H_
