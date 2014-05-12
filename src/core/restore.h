
// TODO(BNC): Add Docs
#ifndef RESTORE_H_
#define RESTORE_H_

void inven_inuse(bool);
int dorecover(int);
void trickery(char *);
void getlev(int, int, xchar, bool);
void minit();
bool lookup_id_mapping(unsigned long, unsigned long *);
#ifdef ZEROCOMP
int mread(int, genericptr_t, unsigned int);
#else
void mread(int, genericptr_t, unsigned int);
#endif


#endif  // RESTORE_H_
