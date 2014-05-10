
// TODO(BNC): Add Docs
#ifndef READ_H_
#define READ_H_

int doread();
bool is_chargeable(Object *);
void recharge(Object *, int);
void forget_objects(int);
void forget_levels(int);
void forget_traps();
void forget_map(int);
int seffects(Object *);
void litroom(bool, Object *);
void do_genocide(int);
void punish(Object *);
void unpunish();
bool cant_create(int *, bool);
#ifdef WIZARD
bool create_particular();
#endif


#endif  // READ_H_
