
// TODO(BNC): Add Docs
#ifndef MINION_H_
#define MINION_H_

#include "core/global.h"
#include "core/monst.h"

void msummon(Monster *);
void summon_minion(ALIGNTYP_P, bool);
int demon_talk(Monster *);
long bribe(Monster *);
int dprince(ALIGNTYP_P);
int dlord(ALIGNTYP_P);
int llord();
int ndemon(ALIGNTYP_P);
int lminion();


#endif  // MINION_H_
