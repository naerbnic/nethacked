
// TODO(BNC): Add Docs
#ifndef SAVE_H_
#define SAVE_H_

int dosave();
void hangup(int);
int dosave0();
#ifdef INSURANCE
void savestateinlock();
#endif
void savelev(int, xchar, int);
void bufon(int);
void bufoff(int);
void bflush(int);
void bwrite(int, void const*, unsigned int);
void bclose(int);
void savefruitchn(int, int);
void free_dungeons();
void freedynamicdata();


#endif  // SAVE_H_
