
// TODO(BNC): Add Docs
#ifndef WEAPON_H_
#define WEAPON_H_

int hitval(Object *, Monster *);
int dmgval(Object *, Monster *);
Object *select_rwep(Monster *);
Object *select_hwep(Monster *);
void possibly_unwield(Monster *, bool);
int mon_wield_item(Monster *);
int abon();
int dbon();
int enhance_weapon_skill();
#ifdef DUMP_LOG
void dump_weapon_skill();
#endif
void unrestrict_weapon_skill(int);
void use_skill(int, int);
void add_weapon_skill(int);
void lose_weapon_skill(int);
int weapon_type(Object *);
int uwep_skill_type();
int weapon_hit_bonus(Object *);
int weapon_dam_bonus(Object *);
void skill_init(const struct def_skill *);


#endif  // WEAPON_H_
