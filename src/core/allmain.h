#ifndef ALLMAIN_H_
#define ALLMAIN_H_

#include "core/config.h"

extern void moveloop();
extern void stop_occupation();
extern void display_gamewindows();
extern void newgame();
extern void welcome(bool);
#if defined(RECORD_REALTIME) || defined(REALTIME_ON_BOTL)
extern time_t get_realtime();
#endif

#endif
