
// TODO(BNC): Add Docs
#ifndef WORM_H_
#define WORM_H_

int get_wormno();
void initworm(Monster *, int);
void worm_move(Monster *);
void worm_nomove(Monster *);
void wormgone(Monster *);
void wormhitu(Monster *);
void cutworm(Monster *, xchar, xchar, Object *);
void see_wsegs(Monster *);
void detect_wsegs(Monster *, bool);
void save_worm(int, int);
void rest_worm(int);
void place_wsegs(Monster *);
void remove_worm(Monster *);
void place_worm_tail_randomly(Monster *, xchar, xchar);
int count_wsegs(Monster *);
bool worm_known(Monster *);


#endif  // WORM_H_
