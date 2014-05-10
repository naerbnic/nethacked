
// TODO(BNC): Add Docs
#ifndef SHK_H_
#define SHK_H_

#ifdef GOLDOBJ
long money2mon(Monster *, long);
void money2u(Monster *, long);
#endif
char *shkname(Monster *);
void shkgone(Monster *);
void set_residency(Monster *, bool);
void replshk(Monster *, Monster *);
void restshk(Monster *, bool);
char inside_shop(xchar, xchar);
void u_left_shop(char *, bool);
void remote_burglary(xchar, xchar);
void u_entered_shop(char *);
bool same_price(Object *, Object *);
void shopper_financial_report();
int inhishop(Monster *);
Monster *shop_keeper(char);
bool tended_shop(struct mkroom *);
void delete_contents(Object *);
void obfree(Object *, Object *);
void home_shk(Monster *, bool);
void make_happy_shk(Monster *, bool);
void hot_pursuit(Monster *);
void make_angry_shk(Monster *, xchar, xchar);
int dopay();
bool paybill(int);
void finish_paybill();
Object *find_oid(unsigned);
long contained_cost(Object *, Monster *, long, bool, bool);
long contained_gold(Object *);
void picked_container(Object *);
long unpaid_cost(Object *);
void addtobill(Object *, bool, bool, bool);
void splitbill(Object *, Object *);
void subfrombill(Object *, Monster *);
long stolen_value(Object *, xchar, xchar, bool, bool);
void sellobj_state(int);
void sellobj(Object *, xchar, xchar);
int doinvbill(int);
Monster *shkcatch(Object *, xchar, xchar);
void add_damage(xchar, xchar, long);
int repair_damage(Monster *, struct damage *, bool);
int shk_move(Monster *);
void after_shk_move(Monster *);
bool is_fshk(Monster *);
void shopdig(int);
void pay_for_damage(const char *, bool);
bool costly_spot(xchar, xchar);
Object *shop_object(xchar, xchar);
void price_quote(Object *);
void shk_chat(Monster *);
void check_unpaid_usage(Object *, bool);
void check_unpaid(Object *);
void costly_gold(xchar, xchar, long);
bool block_door(xchar, xchar);
bool block_entry(xchar, xchar);
char *shk_your(char *, Object *);
char *Shk_Your(char *, Object *);


#endif  // SHK_H_
