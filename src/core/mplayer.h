
// TODO(BNC): Add Docs
#ifndef MPLAYER_H_
#define MPLAYER_H_

#include "core/global.h"
#include "core/monst.h"
#include "core/permonst.h"

Monster *mk_mplayer(MonsterType *, xchar, xchar, bool);
void create_mplayers(int, bool);
void mplayer_talk(Monster *);


#endif  // MPLAYER_H_
