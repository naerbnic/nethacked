
// TODO(BNC): Add Docs
#ifndef STEED_H_
#define STEED_H_

#ifdef STEED
void rider_cant_reach();
bool can_saddle(Monster *);
int use_saddle(Object *);
bool can_ride(Monster *);
int doride();
bool mount_steed(Monster *, bool);
void exercise_steed();
void kick_steed();
void dismount_steed(int);
void place_monster(Monster *, int, int);
#endif


#endif  // STEED_H_
