
// TODO(BNC): Add Docs
#ifndef STEAL_H_
#define STEAL_H_

#ifdef GOLDOBJ
long somegold(long);
#else
long somegold();
#endif
void stealgold(Monster *);
void remove_worn_item(Object *, bool);
int steal(Monster *, char *);
int mpickobj(Monster *, Object *);
void stealamulet(Monster *);
void mdrop_special_objs(Monster *);
void relobj(Monster *, int, bool);
#ifdef GOLDOBJ
Object *findgold(Object *);
#endif


#endif  // STEAL_H_
