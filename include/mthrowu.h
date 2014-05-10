
// TODO(BNC): Add Docs
#ifndef MTHROWU_H_
#define MTHROWU_H_

int thitu(int, int, Object *, const char *);
int ohitmon(Monster *, Object *, int, bool);
void thrwmu(Monster *);
int spitmu(Monster *, struct Attack *);
int breamu(Monster *, struct Attack *);
bool linedup(xchar, xchar, xchar, xchar);
bool lined_up(Monster *);
Object *m_carrying(Monster *, int);
void m_useup(Monster *, Object *);
void m_throw(Monster *, int, int, int, int, int, Object *);
bool hits_bars(Object **, int, int, int, int);


#endif  // MTHROWU_H_
