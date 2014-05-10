
// TODO(BNC): Add Docs
#ifndef PICKUP_H_
#define PICKUP_H_

#ifdef GOLDOBJ
int collect_obj_classes(char *, Object *, bool, bool (*)(Object *), int *);
#else
int collect_obj_classes(char *, Object *, bool, bool, bool (*)(Object *),
                          int *);
#endif
void add_valid_menu_class(int);
bool allow_all(Object *);
bool allow_category(Object *);
bool is_worn_by_type(Object *);
int pickup(int);
int pickup_object(Object *, long, bool);
int query_category(const char *, Object *, int, menu_item **, int);
int query_objlist(const char *, Object *, int, menu_item **, int,
                    bool (*)(Object *));
Object *pick_obj(Object *);
int encumber_msg();
int doloot();
int use_container(Object *, int);
int loot_mon(Monster *, int *, bool *);
const char *safe_qbuf(const char *, unsigned, const char *, const char *,
                        const char *);
bool is_autopickup_exception(Object *, bool);


#endif  // PICKUP_H_
