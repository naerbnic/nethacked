
// TODO(BNC): Add Docs
#ifndef MUSE_H_
#define MUSE_H_

bool find_defensive(Monster *);
int use_defensive(Monster *);
int rnd_defensive_item(Monster *);
bool find_offensive(Monster *);
int use_offensive(Monster *);
int rnd_offensive_item(Monster *);
bool find_misc(Monster *);
int use_misc(Monster *);
int rnd_misc_item(Monster *);
bool searches_for_item(Monster *, Object *);
bool mon_reflects(Monster *, const char *);
bool ureflects(const char *, const char *);
bool munstone(Monster *, bool);


#endif  // MUSE_H_
