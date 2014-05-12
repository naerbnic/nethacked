
// TODO(BNC): Add Docs
#ifndef RANDOM_H_
#define RANDOM_H_

#if defined(RANDOM) && !defined(__GO32__) /* djgpp has its own random */
void srandom(unsigned);
char *initstate(unsigned, char *, int);
char *setstate(char *);
long random();
#endif /* RANDOM */


#endif  // RANDOM_H_
