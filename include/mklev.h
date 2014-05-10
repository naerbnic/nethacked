
// TODO(BNC): Add Docs
#ifndef MKLEV_H_
#define MKLEV_H_

void sort_rooms();
void add_room(int, int, int, int, bool, schar, bool);
void add_subroom(struct mkroom *, int, int, int, int, bool, schar, bool);
void makecorridors();
void add_door(int, int, struct mkroom *);
void mklev();
#ifdef SPECIALIZATION
void topologize(struct mkroom *, bool);
#else
void topologize(struct mkroom *);
#endif
void place_branch(branch *, xchar, xchar);
bool occupied(xchar, xchar);
int okdoor(xchar, xchar);
void dodoor(int, int, struct mkroom *);
void mktrap(int, int, struct mkroom *, coord *);
void mkstairs(xchar, xchar, char, struct mkroom *);
void mkinvokearea();


#endif  // MKLEV_H_
