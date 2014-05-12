
// TODO(BNC): Add Docs
#ifndef OBJNAM_H_
#define OBJNAM_H_

char *obj_typename(int);
char *simple_typename(int);
bool obj_is_pname(Object *);
char *distant_name(Object *, char *(*)(Object *));
char *fruitname(bool);
char *xname(Object *);
char *mshot_xname(Object *);
bool the_unique_obj(Object *obj);
char *doname(Object *);
bool not_fully_identified(Object *);
char *corpse_xname(Object *, bool);
char *cxname(Object *);
#ifdef SORTLOOT
char *cxname2(Object *);
#endif
char *killer_xname(Object *);
const char *singular(Object *, char *(*)(Object *));
char *an(const char *);
char *An(const char *);
char *The(const char *);
char *the(const char *);
char *aobjnam(Object *, const char *);
char *Tobjnam(Object *, const char *);
char *otense(Object *, const char *);
char *vtense(const char *, const char *);
char *Doname2(Object *);
char *yname(Object *);
char *Yname2(Object *);
char *ysimple_name(Object *);
char *Ysimple_name2(Object *);
char *makeplural(const char *);
char *makesingular(const char *);
Object *readobjnam(char *, Object *, bool);
int rnd_class(int, int);
const char *cloak_simple_name(Object *);
const char *mimic_obj_name(Monster *);


#endif  // OBJNAM_H_
