
// TODO(BNC): Add Docs
#ifndef UHITM_H_
#define UHITM_H_

void hurtmarmor(Monster *, int);
bool attack_checks(Monster *, Object *);
void check_caitiff(Monster *);
schar find_roll_to_hit(Monster *);
bool attack(Monster *);
bool hmon(Monster *, Object *, int);
int damageum(Monster *, struct Attack *);
void missum(Monster *, struct Attack *);
int passive(Monster *, bool, int, uchar);
void passive_obj(Monster *, Object *, struct Attack *);
void stumble_onto_mimic(Monster *);
int flash_hits_mon(Monster *, Object *);


#endif  // UHITM_H_
