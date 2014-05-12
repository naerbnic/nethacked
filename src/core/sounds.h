
// TODO(BNC): Add Docs
#ifndef SOUNDS_H_
#define SOUNDS_H_

void dosounds();
const char *growl_sound(Monster *);
void growl(Monster *);
void yelp(Monster *);
void whimper(Monster *);
void beg(Monster *);
int dotalk();
#ifdef USER_SOUNDS
int add_sound_mapping(const char *);
void play_sound_for_message(const char *);
#endif


#endif  // SOUNDS_H_
