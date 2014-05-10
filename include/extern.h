/*	SCCS Id: @(#)extern.h	3.4	2003/03/10	*/
/* Copyright (c) Steve Creps, 1988.				  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef EXTERN_H
#define EXTERN_H

#include <string>

/* ### alloc.c ### */

/* ### allmain.c ### */

/* ### apply.c ### */

/* ### artifact.c ### */

/* ### attrib.c ### */

/* ### ball.c ### */

/* ### bones.c ### */

/* ### botl.c ### */

/* ### cmd.c ### */

/* ### dbridge.c ### */

/* ### decl.c ### */

/* ### detect.c ### */

/* ### dig.c ### */

/* ### display.c ### */

/* ### do.c ### */

/* ### do_name.c ### */

/* ### do_wear.c ### */

/* ### dog.c ### */

/* ### dogmove.c ### */

/* ### dokick.c ### */

/* ### dothrow.c ### */

/* ### drawing.c ### */

/* ### dungeon.c ### */

/* ### eat.c ### */

/* ### end.c ### */

#if !defined(MAKEDEFS_C) && !defined(LEV_LEX_C)
/* ### engrave.c ### */

/* ### exper.c ### */

/* ### explode.c ### */

/* ### extralev.c ### */

/* ### files.c ### */

/* ### fountain.c ### */

/* ### hack.c ### */

/* ### hacklib.c ### */

/* ### invent.c ### */

/* ### ioctl.c ### */

/* ### light.c ### */

/* ### lock.c ### */

/* ### mail.c ### */

/* ### makemon.c ### */

/* ### mapglyph.c ### */

/* ### mcastu.c ### */

/* ### mhitm.c ### */

/* ### mhitu.c ### */

/* ### minion.c ### */

/* ### mklev.c ### */

/* ### mkmap.c ### */

/* ### mkmaze.c ### */

/* ### mkobj.c ### */

/* ### mkroom.c ### */

/* ### mon.c ### */

/* ### mondata.c ### */

/* ### monmove.c ### */

/* ### monst.c ### */

/* ### monstr.c ### */

/* ### mplayer.c ### */

/* ### mthrowu.c ### */

/* ### muse.c ### */

/* ### music.c ### */

/* ### nhlan.c ### */

/* ### o_init.c ### */

/* ### objects.c ### */

/* ### objnam.c ### */

/* ### options.c ### */

/* ### pager.c ### */

/* ### pickup.c ### */

/* ### pline.c ### */

/* ### polyself.c ### */

/* ### potion.c ### */

/* ### pray.c ### */

/* ### priest.c ### */

/* ### quest.c ### */

/* ### questpgr.c ### */

/* ### random.c ### */

/* ### read.c ### */

int doread();
bool is_chargeable(Object *);
void recharge(Object *, int);
void forget_objects(int);
void forget_levels(int);
void forget_traps();
void forget_map(int);
int seffects(Object *);
void litroom(bool, Object *);
void do_genocide(int);
void punish(Object *);
void unpunish();
bool cant_create(int *, bool);
#ifdef WIZARD
bool create_particular();
#endif

/* ### rect.c ### */

void init_rect();
NhRect *get_rect(NhRect *);
NhRect *rnd_rect();
void remove_rect(NhRect *);
void add_rect(NhRect *);
void split_rects(NhRect *, NhRect *);

/* ## region.c ### */
void clear_regions();
void run_regions();
bool in_out_region(xchar, xchar);
bool m_in_out_region(Monster *, xchar, xchar);
void update_player_regions();
void update_monster_region(Monster *);
NhRegion *visible_region_at(xchar, xchar);
void show_region(NhRegion *, xchar, xchar);
void save_regions(int, int);
void rest_regions(int, bool);
NhRegion *create_gas_cloud(xchar, xchar, int, int);

/* ### restore.c ### */

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

/* ### rip.c ### */

void genl_outrip(winid, int);

/* ### rnd.c ### */

void check_reseed();
int rn2(int);
int rnl(int);
int rnd(int);
int d(int, int);
int rne(int);
int rnz(int);

/* ### role.c ### */

bool validrole(int);
bool validrace(int, int);
bool validgend(int, int, int);
bool validalign(int, int, int);
int randrole();
int randrace(int);
int randgend(int, int);
int randalign(int, int);
int str2role(char *);
int str2race(char *);
int str2gend(char *);
int str2align(char *);
bool ok_role(int, int, int, int);
int pick_role(int, int, int, int);
bool ok_race(int, int, int, int);
int pick_race(int, int, int, int);
bool ok_gend(int, int, int, int);
int pick_gend(int, int, int, int);
bool ok_align(int, int, int, int);
int pick_align(int, int, int, int);
void role_init();
void rigid_role_checks();
void plnamesuffix();
const char *Hello(Monster *);
const char *Goodbye();
char *build_plselection_prompt(char *, int, int, int, int, int);
char *root_plselection_prompt(char *, int, int, int, int, int);

/* ### rumors.c ### */

char *getrumor(int, char *, bool);
void outrumor(int, int);
void outoracle(bool, bool);
void save_oracles(int, int);
void restore_oracles(int);
int doconsult(Monster *);

/* ### save.c ### */

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

/* ### shk.c ### */

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

/* ### shknam.c ### */

void stock_room(int, struct mkroom *);
bool saleable(Monster *, Object *);
int get_shop_item(int);

/* ### sit.c ### */

void take_gold();
int dosit();
void rndcurse();
void attrcurse();

/* ### sounds.c ### */

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

/* ### sp_lev.c ### */

/* ### spell.c ### */

int study_book(Object *);
void book_disappears(Object *);
void book_substitution(Object *, Object *);
void age_spells();
int docast();
int spell_skilltype(int);
int spelleffects(int, bool);
void losespells();
int dovspell();
void initialspell(Object *);

/* ### steal.c ### */

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

/* ### steed.c ### */

#ifdef STEED
void rider_cant_reach();
bool can_saddle(Monster *);
int use_saddle(Object *);
bool can_ride(Monster *);
int doride();
bool mount_steed(Monster *, bool);
void exercise_steed();
void kick_steed();
void dismount_steed(int);
void place_monster(Monster *, int, int);
#endif

/* ### teleport.c ### */

bool goodpos(int, int, Monster *, unsigned);
bool enexto(coord *, xchar, xchar, MonsterType *);
bool enexto_core(coord *, xchar, xchar, MonsterType *, unsigned);
void teleds(int, int, bool);
bool safe_teleds(bool);
bool teleport_pet(Monster *, bool);
void tele();
int dotele();
void level_tele();
void domagicportal(Trap *);
void tele_trap(Trap *);
void level_tele_trap(Trap *);
void rloc_to(Monster *, int, int);
bool rloc(Monster *, bool);
bool tele_restrict(Monster *);
void mtele_trap(Monster *, Trap *, int);
int mlevel_tele_trap(Monster *, Trap *, bool, int);
void rloco(Object *);
int random_teleport_level();
bool u_teleport_mon(Monster *, bool);

/* ### tile.c ### */
#ifdef USE_TILES
void substitute_tiles(d_level *);
#endif

/* ### timeout.c ### */

void burn_away_slime();
void nh_timeout();
void fall_asleep(int, bool);
void attach_egg_hatch_timeout(Object *);
void attach_fig_transform_timeout(Object *);
void kill_egg(Object *);
void hatch_egg(genericptr_t, long);
void learn_egg_type(int);
void burn_object(genericptr_t, long);
void begin_burn(Object *, bool);
void end_burn(Object *, bool);
void do_storms();
bool start_timer(long, short, short, genericptr_t);
long stop_timer(short, genericptr_t);
void run_timers();
void obj_move_timers(Object *, Object *);
void obj_split_timers(Object *, Object *);
void obj_stop_timers(Object *);
bool obj_is_local(Object *);
void save_timers(int, int, int);
void restore_timers(int, int, bool, long);
void relink_timers(bool);
#ifdef WIZARD
int wiz_timeout_queue();
void timer_sanity_check();
#endif

/* ### topten.c ### */

void topten(int);
void prscore(int, char **);
Object *tt_oname(Object *);

/* ### track.c ### */

void initrack();
void settrack();
coord *gettrack(int, int);

/* ### trap.c ### */

bool burnarmor(Monster *);
bool rust_dmg(Object *, const char *, int, bool, Monster *);
void grease_protect(Object *, const char *, Monster *);
Trap *maketrap(int, int, int);
void fall_through(bool);
Monster *animate_statue(Object *, xchar, xchar, int, int *);
Monster *activate_statue_trap(Trap *, xchar, xchar, bool);
void dotrap(Trap *, unsigned);
void seetrap(Trap *);
int mintrap(Monster *);
void instapetrify(const char *);
void minstapetrify(Monster *, bool);
void selftouch(const char *);
void mselftouch(Monster *, const char *, bool);
void float_up();
void fill_pit(int, int);
int float_down(long, long);
int fire_damage(Object *, bool, bool, xchar, xchar);
bool water_damage(Object *, bool, bool);
bool drown();
void drain_en(int);
int dountrap();
int untrap(bool);
bool chest_trap(Object *, int, bool);
void deltrap(Trap *);
bool delfloortrap(Trap *);
Trap *t_at(int, int);
void b_trapped(const char *, int);
bool unconscious();
bool lava_effects();
void blow_up_landmine(Trap *);
int launch_obj(short, int, int, int, int, int);

/* ### u_init.c ### */

void u_init();

/* ### uhitm.c ### */

void hurtmarmor(Monster *, int);
bool attack_checks(Monster *, Object *);
void check_caitiff(Monster *);
schar find_roll_to_hit(Monster *);
bool attack(Monster *);
bool hmon(Monster *, Object *, int);
int damageum(Monster *, struct Attack *);
void missum(Monster *, struct Attack *);
int passive(Monster *, bool, int, uchar);
void passive_obj(Monster *, Object *, struct Attack *);
void stumble_onto_mimic(Monster *);
int flash_hits_mon(Monster *, Object *);

/* ### unixmain.c ### */

#ifdef PORT_HELP
void port_help();
#endif

/* ### unixtty.c ### */

void gettty();
void settty(const char *);
void setftty();
void intron();
void introff();
void ErrMsg(const char *, ...) PRINTF_F(1, 2);
void VErrMsg(const char *, va_list);
void error(const char *, ...) PRINTF_F(1, 2);

/* ### unixunix.c ### */

void getlock();
void regularize(char *);
#if defined(TIMED_DELAY) && !defined(msleep) && defined(SYSV)
void msleep(unsigned);
#endif
#ifdef SHELL
int dosh();
#endif /* SHELL */
#if defined(SHELL) || defined(DEF_PAGER) || defined(DEF_MAILREADER)
int child(int);
#endif

/* ### vault.c ### */

bool grddead(Monster *);
char vault_occupied(char *);
void invault();
int gd_move(Monster *);
void paygd();
long hidden_gold();
bool gd_sound();

/* ### version.c ### */

char *version_string(char *);
char *getversionstring(char *);
int doversion();
int doextversion();
bool check_version(struct version_info *, const char *, bool);
unsigned long get_feature_notice_ver(char *);
unsigned long get_current_feature_ver();
#ifdef RUNTIME_PORT_ID
void append_port_id(char *);
#endif

/* ### video.c ### */

#ifdef VIDEOSHADES
int assign_videoshades(char *);
int assign_videocolors(char *);
#endif

/* ### vis_tab.c ### */

#ifdef VISION_TABLES
void vis_tab_init();
#endif

/* ### vision.c ### */

void vision_init();
int does_block(int, int, struct rm *);
void vision_reset();
void vision_recalc(int);
void block_point(int, int);
void unblock_point(int, int);
bool clear_path(int, int, int, int);
void do_clear_area(int, int, int, void (*)(int, int, genericptr_t),
                     genericptr_t);

/* ### weapon.c ### */

int hitval(Object *, Monster *);
int dmgval(Object *, Monster *);
Object *select_rwep(Monster *);
Object *select_hwep(Monster *);
void possibly_unwield(Monster *, bool);
int mon_wield_item(Monster *);
int abon();
int dbon();
int enhance_weapon_skill();
#ifdef DUMP_LOG
void dump_weapon_skill();
#endif
void unrestrict_weapon_skill(int);
void use_skill(int, int);
void add_weapon_skill(int);
void lose_weapon_skill(int);
int weapon_type(Object *);
int uwep_skill_type();
int weapon_hit_bonus(Object *);
int weapon_dam_bonus(Object *);
void skill_init(const struct def_skill *);

/* ### were.c ### */

void were_change(Monster *);
void new_were(Monster *);
int were_summon(MonsterType *, bool, int *, char *);
void you_were();
void you_unwere(bool);

/* ### wield.c ### */

void setuwep(Object *);
void setuqwep(Object *);
void setuswapwep(Object *);
int dowield();
int doswapweapon();
int dowieldquiver();
bool wield_tool(Object *, const char *);
int can_twoweapon();
void drop_uswapwep();
int dotwoweapon();
void uwepgone();
void uswapwepgone();
void uqwepgone();
void untwoweapon();
void erode_obj(Object *, bool, bool);
int chwepon(Object *, int);
int welded(Object *);
void weldmsg(Object *);
void setmnotwielded(Monster *, Object *);

/* ### windows.c ### */

void choose_windows(const char *);
char genl_message_menu(char, int, const char *);
void genl_preference_update(const char *);

/* ### wizard.c ### */

void amulet();
int mon_has_amulet(Monster *);
int mon_has_special(Monster *);
int tactics(Monster *);
void aggravate();
void clonewiz();
int pick_nasty();
int nasty(Monster *);
void resurrect();
void intervene();
void wizdead();
void cuss(Monster *);

/* ### worm.c ### */

int get_wormno();
void initworm(Monster *, int);
void worm_move(Monster *);
void worm_nomove(Monster *);
void wormgone(Monster *);
void wormhitu(Monster *);
void cutworm(Monster *, xchar, xchar, Object *);
void see_wsegs(Monster *);
void detect_wsegs(Monster *, bool);
void save_worm(int, int);
void rest_worm(int);
void place_wsegs(Monster *);
void remove_worm(Monster *);
void place_worm_tail_randomly(Monster *, xchar, xchar);
int count_wsegs(Monster *);
bool worm_known(Monster *);

/* ### worn.c ### */

void setworn(Object *, long);
void setnotworn(Object *);
void mon_set_minvis(Monster *);
void mon_adjust_speed(Monster *, int, Object *);
void update_mon_intrinsics(Monster *, Object *, bool, bool);
int find_mac(Monster *);
void m_dowear(Monster *, bool);
Object *which_armor(Monster *, long);
void mon_break_armor(Monster *, bool);
void bypass_obj(Object *);
void clear_bypasses();
int racial_exception(Monster *, Object *);

/* ### write.c ### */

int dowrite(Object *);

/* ### zap.c ### */

int bhitm(Monster *, Object *);
void probe_monster(Monster *);
bool get_obj_location(Object *, xchar *, xchar *, int);
bool get_mon_location(Monster *, xchar *, xchar *, int);
Monster *get_container_location(Object *obj, int *, int *);
Monster *montraits(Object *, coord *);
Monster *revive(Object *);
int unturn_dead(Monster *);
void cancel_item(Object *);
bool drain_item(Object *);
Object *poly_obj(Object *, int);
bool obj_resists(Object *, int, int);
bool obj_shudders(Object *);
void do_osshock(Object *);
int bhito(Object *, Object *);
int bhitpile(Object *, int (*)(Object *, Object *), int, int);
int zappable(Object *);
void zapnodir(Object *);
int dozap();
int zapyourself(Object *, bool);
bool cancel_monst(Monster *, Object *, bool, bool, bool);
void weffects(Object *);
int spell_damage_bonus();
const char *exclam(int force);
void hit(const char *, Monster *, const char *);
void miss(const char *, Monster *);
Monster *bhit(int, int, int, int, int (*)(Monster *, Object *),
                int (*)(Object *, Object *), Object *, bool *);
Monster *boomhit(int, int);
int burn_floor_paper(int, int, bool, bool);
void buzz(int, int, xchar, xchar, int, int);
void melt_ice(xchar, xchar);
int zap_over_floor(xchar, xchar, int, bool *);
void fracture_rock(Object *);
bool break_statue(Object *);
void destroy_item(int, int);
int destroy_mitem(Monster *, int, int);
int resist(Monster *, char, int, int);
void makewish();

#endif /* !MAKEDEFS_C && !LEV_LEX_C */

#endif /* EXTERN_H */
