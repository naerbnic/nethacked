
// TODO(BNC): Add Docs
#ifndef MHITM_H_
#define MHITM_H_

#include "core/monst.h"
#include "core/permonst.h"

int fightm(Monster *);
int mattackm(Monster *, Monster *);
int noattacks(MonsterType *);
int sleep_monst(Monster *, int, int);
void slept_monst(Monster *);
long attk_protection(int);


#endif  // MHITM_H_
