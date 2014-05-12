
// TODO(BNC): Add Docs
#ifndef RUMORS_H_
#define RUMORS_H_

char *getrumor(int, char *, bool);
void outrumor(int, int);
void outoracle(bool, bool);
void save_oracles(int, int);
void restore_oracles(int);
int doconsult(Monster *);


#endif  // RUMORS_H_
