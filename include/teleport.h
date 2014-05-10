
// TODO(BNC): Add Docs
#ifndef TELEPORT_H_
#define TELEPORT_H_

bool goodpos(int, int, Monster *, unsigned);
bool enexto(coord *, xchar, xchar, MonsterType *);
bool enexto_core(coord *, xchar, xchar, MonsterType *, unsigned);
void teleds(int, int, bool);
bool safe_teleds(bool);
bool teleport_pet(Monster *, bool);
void tele();
int dotele();
void level_tele();
void domagicportal(Trap *);
void tele_trap(Trap *);
void level_tele_trap(Trap *);
void rloc_to(Monster *, int, int);
bool rloc(Monster *, bool);
bool tele_restrict(Monster *);
void mtele_trap(Monster *, Trap *, int);
int mlevel_tele_trap(Monster *, Trap *, bool, int);
void rloco(Object *);
int random_teleport_level();
bool u_teleport_mon(Monster *, bool);


#endif  // TELEPORT_H_
