
// TODO(BNC): Add Docs
#ifndef WORN_H_
#define WORN_H_

void setworn(Object *, long);
void setnotworn(Object *);
void mon_set_minvis(Monster *);
void mon_adjust_speed(Monster *, int, Object *);
void update_mon_intrinsics(Monster *, Object *, bool, bool);
int find_mac(Monster *);
void m_dowear(Monster *, bool);
Object *which_armor(Monster *, long);
void mon_break_armor(Monster *, bool);
void bypass_obj(Object *);
void clear_bypasses();
int racial_exception(Monster *, Object *);


#endif  // WORN_H_
