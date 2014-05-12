
// TODO(BNC): Add Docs
#ifndef MHITU_H_
#define MHITU_H_

const char *mpoisons_subj(Monster *, struct Attack *);
void u_slow_down();
Monster *cloneu();
void expels(Monster *, MonsterType *, bool);
struct Attack *getmattk(MonsterType *, int, int *, struct Attack *);
int mattacku(Monster *);
int magic_negation(Monster *);
int gazemu(Monster *, struct Attack *);
void mdamageu(Monster *, int);
int could_seduce(Monster *, Monster *, struct Attack *);
#ifdef SEDUCE
int doseduce(Monster *);
#endif


#endif  // MHITU_H_
