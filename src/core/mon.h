
// TODO(BNC): Add Docs
#ifndef MON_H_
#define MON_H_

int undead_to_corpse(int);
int genus(int, int);
int pm_to_cham(int);
int minliquid(Monster *);
int movemon();
int meatmetal(Monster *);
int meatobj(Monster *);
void mpickgold(Monster *);
bool mpickstuff(Monster *, const char *);
int curr_mon_load(Monster *);
int max_mon_load(Monster *);
bool can_carry(Monster *, Object *);
int mfndpos(Monster *, coord *, long *, long);
bool monnear(Monster *, int, int);
void dmonsfree();
int mcalcmove(Monster *);
void mcalcdistress();
void replmon(Monster *, Monster *);
void relmon(Monster *);
Object *mlifesaver(Monster *);
bool corpse_chance(Monster *, Monster *, bool);
void mondead(Monster *);
void mondied(Monster *);
void mongone(Monster *);
void monstone(Monster *);
void monkilled(Monster *, const char *, int);
void unstuck(Monster *);
void killed(Monster *);
void xkilled(Monster *, int);
void mon_to_stone(Monster *);
void mnexto(Monster *);
bool mnearto(Monster *, xchar, xchar, bool);
void poisontell(int);
void poisoned(const char *, int, const char *, int);
void m_respond(Monster *);
void setmangry(Monster *);
void wakeup(Monster *);
void wake_nearby();
void wake_nearto(int, int, int);
void seemimic(Monster *);
void rescham();
void restartcham();
void restore_cham(Monster *);
void mon_animal_list(bool);
int newcham(Monster *, MonsterType *, bool, bool);
int can_be_hatched(int);
int egg_type_from_parent(int, bool);
bool dead_species(int, bool);
void kill_genocided_monsters();
void golemeffects(Monster *, int, int);
bool angry_guards(bool);
void pacify_guards();


#endif  // MON_H_
