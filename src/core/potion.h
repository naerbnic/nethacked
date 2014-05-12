
// TODO(BNC): Add Docs
#ifndef POTION_H_
#define POTION_H_

void set_itimeout(long *, long);
void incr_itimeout(long *, int);
void make_confused(long, bool);
void make_stunned(long, bool);
void make_blinded(long, bool);
void make_sick(long, const char *, bool, int);
void make_vomiting(long, bool);
bool make_hallucinated(long, bool, long);
int dodrink();
int dopotion(Object *);
int peffects(Object *);
void healup(int, int, bool, bool);
void strange_feeling(Object *, const char *);
void potionhit(Monster *, Object *, bool);
void potionbreathe(Object *);
bool get_wet(Object *);
int dodip();
void djinni_from_bottle(Object *);
Monster *split_mon(Monster *, Monster *);
const char *bottlename();


#endif  // POTION_H_
