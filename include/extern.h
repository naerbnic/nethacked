/*	SCCS Id: @(#)extern.h	3.4	2003/03/10	*/
/* Copyright (c) Steve Creps, 1988.				  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef EXTERN_H
#define EXTERN_H

#include <string>

#define E extern

/* ### alloc.c ### */

E char *fmt_ptr(const void *, char *);

/* This next pre-processor directive covers almost the entire file,
 * interrupted only occasionally to pick up specific functions as needed. */
#if !defined(MAKEDEFS_C) && !defined(LEV_LEX_C)

/* ### allmain.c ### */

E void moveloop();
E void stop_occupation();
E void display_gamewindows();
E void newgame();
E void welcome(bool);
#if defined(RECORD_REALTIME) || defined(REALTIME_ON_BOTL)
E time_t get_realtime();
#endif

/* ### apply.c ### */

E int doapply();
E int dorub();
E int dojump();
E int jump(int);
E int number_leashed();
E void o_unleash(Object *);
E void m_unleash(Monster *, bool);
E void unleash_all();
E bool next_to_u();
E Object *get_mleash(Monster *);
E void check_leash(xchar, xchar);
E bool um_dist(xchar, xchar, xchar);
E bool snuff_candle(Object *);
E bool snuff_lit(Object *);
E bool catch_lit(Object *);
E void use_unicorn_horn(Object *);
E bool tinnable(Object *);
E void reset_trapset();
E void fig_transform(genericptr_t, long);
E int unfixable_trouble_count(bool);

/* ### artifact.c ### */

E void init_artifacts();
E void save_artifacts(int);
E void restore_artifacts(int);
E const char *artiname(int);
E Object *mk_artifact(Object *, ALIGNTYP_P);
E const char *artifact_name(const char *, short *);
E bool exist_artifact(int, std::string const&);
E void artifact_exists(Object *, std::string const&, bool);
E int nartifact_exist();
E bool spec_ability(Object *, unsigned long);
E bool confers_luck(Object *);
E bool arti_reflects(Object *);
E bool restrict_name(Object *, std::string const&);
E bool defends(int, Object *);
E bool protects(int, Object *);
E void set_artifact_intrinsic(Object *, bool, long);
E int touch_artifact(Object *, Monster *);
E int spec_abon(Object *, Monster *);
E int spec_dbon(Object *, Monster *, int);
E void discover_artifact(xchar);
E bool undiscovered_artifact(xchar);
E int disp_artifact_discoveries(winid);
E bool artifact_hit(Monster *, Monster *, Object *, int *, int);
E int doinvoke();
E void arti_speak(Object *);
E bool artifact_light(Object *);
E long spec_m2(Object *);
E bool artifact_has_invprop(Object *, uchar);
E long arti_cost(Object *);

/* ### attrib.c ### */

E bool adjattrib(int, int, int);
E void change_luck(schar);
E int stone_luck(bool);
E void set_moreluck();
E void gainstr(Object *, int);
E void losestr(int);
E void restore_attrib();
E void exercise(int, bool);
E void exerchk();
E void reset_attribute_clock();
E void init_attr(int);
E void redist_attr();
E void adjabil(int, int);
E int newhp();
E schar acurr(int);
E schar acurrstr();
E void adjalign(int);

/* ### ball.c ### */

E void ballfall();
E void placebc();
E void unplacebc();
E void set_bc(int);
E void move_bc(int, int, xchar, xchar, xchar, xchar);
E bool drag_ball(xchar, xchar, int *, xchar *, xchar *, xchar *, xchar *,
                 bool *, bool);
E void drop_ball(xchar, xchar);
E void drag_down();

/* ### bones.c ### */

E bool can_make_bones();
E void savebones(Object *);
E int getbones();

/* ### botl.c ### */

E int xlev_to_rank(int);
E int title_to_mon(const char *, int *, int *);
E void max_rank_sz();
#ifdef SCORE_ON_BOTL
E long botl_score();
#endif
E int describe_level(char *);
E const char *rank_of(int, short, bool);
E void bot();
#ifdef DUMP_LOG
E void bot1str(char *);
E void bot2str(char *);
#endif

/* ### cmd.c ### */

E void reset_occupations();
E void set_occupation(int (*)(void), const char *, int);
#ifdef REDO
E char pgetchar();
E void pushch(char);
E void savech(char);
#endif
#ifdef WIZARD
E void add_debug_extended_commands();
#endif /* WIZARD */
E void rhack(char *);
E int doextlist();
E int extcmd_via_menu();
E void enlightenment(int);
E void show_conduct(int);
#ifdef DUMP_LOG
E void dump_enlightenment(int);
E void dump_conduct(int);
#endif
E int xytod(schar, schar);
E void dtoxy(coord *, int);
E int movecmd(char);
E int getdir(const char *);
E void confdir();
E int isok(int, int);
E int get_adjacent_loc(const char *, const char *, xchar, xchar, coord *);
E const char *click_to_cmd(int, int, int);
E char readchar();
#ifdef WIZARD
E void sanity_check();
#endif
E char yn_function(const char *, const char *, char);

/* ### dbridge.c ### */

E bool is_pool(int, int);
E bool is_lava(int, int);
E bool is_ice(int, int);
E int is_drawbridge_wall(int, int);
E bool is_db_wall(int, int);
E bool find_drawbridge(int *, int *);
E bool create_drawbridge(int, int, int, bool);
E void open_drawbridge(int, int);
E void close_drawbridge(int, int);
E void destroy_drawbridge(int, int);

/* ### decl.c ### */

E void decl_init();

/* ### detect.c ### */

E Object *o_in(Object *, char);
E Object *o_material(Object *, unsigned);
E int gold_detect(Object *);
E int food_detect(Object *);
E int object_detect(Object *, int);
E int monster_detect(Object *, int);
E int trap_detect(Object *);
E const char *level_distance(d_level *);
E void use_crystal_ball(Object *);
E void do_mapping();
E void do_vicinity_map();
E void cvt_sdoor_to_door(struct rm *);
E int findit();
E int openit();
E void find_trap(Trap *);
E int dosearch0(int);
E int dosearch();
E void sokoban_detect();

/* ### dig.c ### */

E bool is_digging();
E int holetime();
E bool dig_check(Monster *, bool, int, int);
E void digactualhole(int, int, Monster *, int);
E bool dighole(bool);
E int use_pick_axe(Object *);
E int use_pick_axe2(Object *);
E bool mdig_tunnel(Monster *);
E void watch_dig(Monster *, xchar, xchar, bool);
E void zap_dig();
E Object *bury_an_obj(Object *);
E void bury_objs(int, int);
E void unearth_objs(int, int);
E void rot_organic(genericptr_t, long);
E void rot_corpse(genericptr_t, long);
#if 0
E void bury_monst(Monster *);
E void bury_you();
E void unearth_you();
E void escape_tomb();
E void bury_obj(Object *);
#endif

/* ### display.c ### */

#ifdef INVISIBLE_OBJECTS
E Object *vobj_at(xchar, xchar);
#endif /* INVISIBLE_OBJECTS */
E void magic_map_background(xchar, xchar, int);
E void map_background(xchar, xchar, int);
E void map_trap(Trap *, int);
E void map_object(Object *, int);
E void map_invisible(xchar, xchar);
E void unmap_object(int, int);
E void map_location(int, int, int);
E void feel_location(xchar, xchar);
E void newsym(int, int);
E void shieldeff(xchar, xchar);
E void tmp_at(int, int);
E void swallowed(int);
E void under_ground(int);
E void under_water(int);
E void see_monsters();
E void set_mimic_blocking();
E void see_objects();
E void see_traps();
E void curs_on_u();
E int doredraw();
E void docrt();
E void show_glyph(int, int, int);
E void clear_glyph_buffer();
E void row_refresh(int, int, int);
E void cls();
E void flush_screen(int);
#ifdef DUMP_LOG
E void dump_screen();
#endif
E int back_to_glyph(xchar, xchar);
E int zapdir_to_glyph(int, int, int);
E int glyph_at(xchar, xchar);
E void set_wall_state();

/* ### do.c ### */

E int dodrop();
E bool boulder_hits_pool(Object *, int, int, bool);
E bool flooreffects(Object *, int, int, const char *);
E void doaltarobj(Object *);
E bool canletgo(Object *, const char *);
E void dropx(Object *);
E void dropy(Object *);
E void obj_no_longer_held(Object *);
E int doddrop();
E int dodown();
E int doup();
#ifdef INSURANCE
E void save_currentstate();
#endif
E void goto_level(d_level *, bool, bool, bool);
E void schedule_goto(d_level *, bool, bool, int, const char *, const char *);
E void deferred_goto();
E bool revive_corpse(Object *);
E void revive_mon(genericptr_t, long);
E int donull();
E int dowipe();
E void set_wounded_legs(long, int);
E void heal_legs();

/* ### do_name.c ### */

E int getpos(coord *, bool, const char *);
E Monster *christen_monst(Monster *, std::string const&);
E int do_mname();
E Object *oname(Object *, std::string const&);
E int ddocall();
E void docall(Object *);
E const char *rndghostname();
E char *x_monnam(Monster *, int, const char *, int, bool);
E char *l_monnam(Monster *);
E char *mon_nam(Monster *);
E char *noit_mon_nam(Monster *);
E char *Monnam(Monster *);
E char *noit_Monnam(Monster *);
E char *m_monnam(Monster *);
E char *y_monnam(Monster *);
E char *Adjmonnam(Monster *, const char *);
E char *Amonnam(Monster *);
E char *a_monnam(Monster *);
E char *distant_monnam(Monster *, int, char *);
E const char *rndmonnam();
E const char *hcolor(const char *);
E const char *rndcolor();
#ifdef REINCARNATION
E const char *roguename();
#endif
E Object *ReallocateExtraObjectSpace(Object *, int, genericptr_t, int,
                                     const char *);
E Object *ReallocateExtraObjectSpace(Object *obj, int oextra_size,
                                     genericptr_t oextra_src, std::string const& name);
E char *coyotename(Monster *, char *);

/* ### do_wear.c ### */

E void off_msg(Object *);
E void set_wear();
E bool donning(Object *);
E void cancel_don();
E int Armor_off();
E int Armor_gone();
E int Helmet_off();
E int Gloves_off();
E int Boots_off();
E int Cloak_off();
E int Shield_off();
#ifdef TOURIST
E int Shirt_off();
#endif
E void Amulet_off();
E void Ring_on(Object *);
E void Ring_off(Object *);
E void Ring_gone(Object *);
E void Blindf_on(Object *);
E void Blindf_off(Object *);
E int dotakeoff();
E int doremring();
E int cursed(Object *);
E int armoroff(Object *);
E int canwearobj(Object *, long *, bool);
E int dowear();
E int doputon();
E void find_ac();
E void glibr();
E Object *some_armor(Monster *);
E void erode_armor(Monster *, bool);
E Object *stuck_ring(Object *, int);
E Object *unchanger();
E void reset_remarm();
E int doddoremarm();
E int destroy_arm(Object *);
E void adj_abon(Object *, schar);

/* ### dog.c ### */

E void initedog(Monster *);
E Monster *make_familiar(Object *, xchar, xchar, bool);
E Monster *makedog();
E void update_mlstmv();
E void losedogs();
E void mon_arrive(Monster *, bool);
E void mon_catchup_elapsed_time(Monster *, long);
E void keepdogs(bool);
E void migrate_to_level(Monster *, xchar, xchar, coord *);
E int dogfood(Monster *, Object *);
E Monster *tamedog(Monster *, Object *);
E void abuse_dog(Monster *);
E void wary_dog(Monster *, bool);

/* ### dogmove.c ### */

E int dog_nutrition(Monster *, Object *);
E int dog_eat(Monster *, Object *, int, int, bool);
E int dog_move(Monster *, int);

/* ### dokick.c ### */

E bool ghitm(Monster *, Object *);
E void container_impact_dmg(Object *);
E int dokick();
E bool ship_object(Object *, xchar, xchar, bool);
E void obj_delivery();
E schar down_gate(xchar, xchar);
E void impact_drop(Object *, xchar, xchar, xchar);

/* ### dothrow.c ### */

E int dothrow();
E int dofire();
E void hitfloor(Object *);
E void hurtle(int, int, int, bool);
E void mhurtle(Monster *, int, int, int);
E void throwit(Object *, long, bool);
E int omon_adj(Monster *, Object *, bool);
E int thitmonst(Monster *, Object *);
E int hero_breaks(Object *, xchar, xchar, bool);
E int breaks(Object *, xchar, xchar);
E bool breaktest(Object *);
E bool walk_path(coord *, coord *, bool (*)(genericptr_t, int, int),
                 genericptr_t);
E bool hurtle_step(genericptr_t, int, int);
#endif /* !MAKEDEFS_C && !LEV_LEX_C */

/* ### drawing.c ### */
#include "drawing.h"

#if !defined(MAKEDEFS_C) && !defined(LEV_LEX_C)
/* ### dungeon.c ### */

E void save_dungeon(int, bool, bool);
E void restore_dungeon(int);
E void insert_branch(branch *, bool);
E void init_dungeons();
E s_level *find_level(const char *);
E s_level *Is_special(d_level *);
E branch *Is_branchlev(d_level *);
E xchar ledger_no(d_level *);
E xchar maxledgerno();
E schar depth(d_level *);
E xchar dunlev(d_level *);
E xchar dunlevs_in_dungeon(d_level *);
E xchar ledger_to_dnum(xchar);
E xchar ledger_to_dlev(xchar);
E xchar deepest_lev_reached(bool);
E bool on_level(d_level *, d_level *);
E void next_level(bool);
E void prev_level(bool);
E void u_on_newpos(int, int);
E void u_on_sstairs();
E void u_on_upstairs();
E void u_on_dnstairs();
E bool On_stairs(xchar, xchar);
E void get_level(d_level *, int);
E bool Is_botlevel(d_level *);
E bool Can_fall_thru(d_level *);
E bool Can_dig_down(d_level *);
E bool Can_rise_up(int, int, d_level *);
E bool In_quest(d_level *);
E bool In_mines(d_level *);
E branch *dungeon_branch(const char *);
E bool at_dgn_entrance(const char *);
E bool In_hell(d_level *);
E bool In_V_tower(d_level *);
E bool On_W_tower_level(d_level *);
E bool In_W_tower(int, int, d_level *);
E void find_hell(d_level *);
E void goto_hell(bool, bool);
E void assign_level(d_level *, d_level *);
E void assign_rnd_level(d_level *, d_level *, int);
E int induced_align(int);
E bool Invocation_lev(d_level *);
E xchar level_difficulty();
E schar lev_by_name(const char *);
#ifdef WIZARD
E schar print_dungeon(bool, schar *, xchar *);
#endif

/* ### eat.c ### */

E bool is_edible(Object *);
E void init_uhunger();
E int Hear_again();
E void reset_eat();
E int doeat();
E void gethungry();
E void morehungry(int);
E void lesshungry(int);
E bool is_fainted();
E void reset_faint();
E void violated_vegetarian();
#if 0
E void sync_hunger();
#endif
E void newuhs(bool);
E Object *floorfood(const char *, int);
E void vomit();
E int eaten_stat(int, Object *);
E void food_disappears(Object *);
E void food_substitution(Object *, Object *);
E void fix_petrification();
E void consume_oeaten(Object *, int);
E bool maybe_finished_meal(bool);

#endif /* !MAKEDEFS_C && !LEV_LEX_C */
/* ### end.c ### */

E void done1(int);
E int done2();
E void done_in_by(Monster *);
E void panic(const char *, ...) PRINTF_F(1, 2);
E void done(int);
E void container_contents(Object *, bool, bool);
#ifdef DUMP_LOG
E void dump(char const *, char const *);
E void do_containerconts(Object *, bool, bool, bool, bool);
#endif
E void terminate(int);
E int num_genocides();

#if !defined(MAKEDEFS_C) && !defined(LEV_LEX_C)
/* ### engrave.c ### */

E char *random_engraving(char *);
E void wipeout_text(char *, int, unsigned);
E bool can_reach_floor();
E const char *surface(int, int);
E const char *ceiling(int, int);
E struct engr *engr_at(xchar, xchar);
#ifdef ELBERETH
E int sengr_at(const char *, xchar, xchar);
#endif
E void u_wipe_engr(int);
E void wipe_engr_at(xchar, xchar, xchar);
E void read_engr_at(int, int);
E void make_engr_at(int, int, const char *, long, xchar);
E void del_engr_at(int, int);
E int freehand();
E int doengrave();
E void save_engravings(int, int);
E void rest_engravings(int);
E void del_engr(struct engr *);
E void rloc_engr(struct engr *);
E void make_grave(int, int, const char *);

/* ### exper.c ### */

E int experience(Monster *, int);
E void more_experienced(int, int);
E void losexp(const char *);
E void newexplevel();
E void pluslvl(bool);
E long rndexp(bool);

/* ### explode.c ### */

E void explode(int, int, int, int, char, int);
E long scatter(int, int, int, unsigned int, Object *);
E void splatter_burning_oil(int, int);

/* ### extralev.c ### */

#ifdef REINCARNATION
E void makeroguerooms();
E void corr(int, int);
E void makerogueghost();
#endif

/* ### files.c ### */

E char *fname_encode(const char *, char, char *, char *, int);
E char *fname_decode(char, char *, char *, int);
E const char *fqname(const char *, int, int);
E FILE *fopen_datafile(const char *, const char *, int);
E bool uptodate(int, const char *);
E void store_version(int);
E void set_levelfile_name(char *, int);
E int create_levelfile(int, char *);
E int open_levelfile(int, char *);
E void delete_levelfile(int);
E void clearlocks();
E int create_bonesfile(d_level *, char **, char *);
E void commit_bonesfile(d_level *);
E int open_bonesfile(d_level *, char **);
E int delete_bonesfile(d_level *);
E void compress_bonesfile();
E void set_savefile_name();
#ifdef INSURANCE
E void save_savefile_name(int);
#endif
#if defined(WIZARD)
E void set_error_savefile();
#endif
E int create_savefile();
E int open_savefile();
E int delete_savefile();
E int restore_saved_game();
E void compress(const char *);
E void uncompress(const char *);
E bool lock_file(const char *, int, int);
E void unlock_file(const char *);
#ifdef USER_SOUNDS
E bool can_read_file(const char *);
#endif
E void read_config_file(const char *);
E void check_recordfile(const char *);
#if defined(WIZARD)
E void read_wizkit();
#endif
E void paniclog(const char *, const char *);
E int validate_prefix_locations(char *);
E char **get_saved_games();
E void free_saved_games(char **);
#ifdef SELF_RECOVER
E bool recover_savefile();
#endif
#ifdef HOLD_LOCKFILE_OPEN
E void really_close();
#endif

/* ### fountain.c ### */

E void floating_above(const char *);
E void dogushforth(int);
E void dryup(xchar, xchar, bool);
E void drinkfountain();
E void dipfountain(Object *);
#ifdef SINKS
E void breaksink(int, int);
E void drinksink();
#endif

/* ### hack.c ### */

E bool revive_nasty(int, int, const char *);
E void movobj(Object *, xchar, xchar);
E bool may_dig(xchar, xchar);
E bool may_passwall(xchar, xchar);
E bool bad_rock(MonsterType *, xchar, xchar);
E bool invocation_pos(xchar, xchar);
E bool test_move(int, int, int, int, int);
E void domove();
E void invocation_message();
E void spoteffects(bool);
E char *in_rooms(xchar, xchar, int);
E bool in_town(int, int);
E void check_special_room(bool);
E int dopickup();
E void lookaround();
E int monster_nearby();
E void nomul(int, const char *);
E void unmul(const char *);
E void losehp(int, const char *, bool);
E int weight_cap();
E int inv_weight();
E int near_capacity();
E int calc_capacity(int);
E int max_capacity();
E bool check_capacity(const char *);
E int inv_cnt();
#ifdef GOLDOBJ
E long money_cnt(Object *);
#endif

/* ### hacklib.c ### */

E bool digit(char);
E bool letter(char);
E char highc(char);
E char lowc(char);
E char *lcase(char *);
E char *upstart(char *);
E char *mungspaces(char *);
E char *eos(char *);
E char *strkitten(char *, char);
E char *s_suffix(const char *);
E char *xcrypt(const char *, char *);
E bool onlyspace(const char *);
E char *tabexpand(char *);
E char *visctrl(char);
E const char *ordin(int);
E char *sitoa(int);
E int sgn(int);
E int rounddiv(long, int);
E int dist2(int, int, int, int);
E int distmin(int, int, int, int);
E bool online2(int, int, int, int);
E bool pmatch(const char *, const char *);
#ifndef STRNCMPI
E int strncmpi(const char *, const char *, int);
#endif
#ifndef STRSTRI
E char *strstri(const char *, const char *);
#endif
E bool fuzzymatch(const char *, const char *, const char *, bool);
E void setrandom();
E int getyear();
#if 0
E char *yymmdd(time_t);
#endif
E long yyyymmdd(time_t);
E int phase_of_the_moon();
E bool friday_13th();
E int night();
E int midnight();

/* ### invent.c ### */

E void assigninvlet(Object *);
E Object *merge_choice(Object *, Object *);
E int merged(Object **, Object **);
E void addinv_core1(Object *);
E void addinv_core2(Object *);
E Object *addinv(Object *);
E Object *hold_another_object(Object *, const char *, const char *,
                              const char *);
E void useupall(Object *);
E void useup(Object *);
E void consume_obj_charge(Object *, bool);
E void freeinv_core(Object *);
E void freeinv(Object *);
E void delallobj(int, int);
E void delobj(Object *);
E Object *sobj_at(int, int, int);
E Object *carrying(int);
E bool have_lizard();
E Object *o_on(unsigned int, Object *);
E bool obj_here(Object *, int, int);
E bool wearing_armor();
E bool is_worn(Object *);
E Object *g_at(int, int);
E Object *mkgoldobj(long);
E Object *getobj(const char *, const char *);
E int ggetobj(const char *, int (*)(Object *), int, bool, unsigned *);
E void fully_identify_obj(Object *);
E int identify(Object *);
E void identify_pack(int);
E int askchain(Object **, const char *, int, int (*)(Object *),
               int (*)(Object *), int, const char *);
E void prinv(const char *, Object *, long);
E char *xprname(Object *, const char *, char, bool, long, long);
E int ddoinv();
E char display_inventory(const char *, bool);
#ifdef DUMP_LOG
E char dump_inventory(const char *, bool, bool);
#endif
E int display_binventory(int, int, bool);
E Object *display_cinventory(Object *);
E Object *display_minventory(Monster *, int, char *);
E int dotypeinv();
E const char *dfeature_at(int, int, char *);
E int look_here(int, bool);
E int dolook();
E bool will_feel_cockatrice(Object *, bool);
E void feel_cockatrice(Object *, bool);
E void stackobj(Object *);
E int doprgold();
E int doprwep();
E int doprarm();
E int doprring();
E int dopramulet();
E int doprtool();
E int doprinuse();
E void useupf(Object *, long);
E char *let_to_name(char, bool);
E void free_invbuf();
E void reassign();
E int doorganize();
E int count_unpaid(Object *);
E int count_buc(Object *, int);
E void carry_obj_effects(Object *);
E const char *currency(long);
E void silly_thing(const char *, Object *);

/* ### ioctl.c ### */

E void getwindowsz();
E void getioctls();
E void setioctls();
#ifdef SUSPEND
E int dosuspend();
#endif /* SUSPEND */

/* ### light.c ### */

E void new_light_source(xchar, xchar, int, int, genericptr_t);
E void del_light_source(int, genericptr_t);
E void do_light_sources(char **);
E Monster *find_mid(unsigned, unsigned);
E void save_light_sources(int, int, int);
E void restore_light_sources(int);
E void relink_light_sources(bool);
E void obj_move_light_source(Object *, Object *);
E bool any_light_source();
E void snuff_light_source(int, int);
E bool obj_sheds_light(Object *);
E bool obj_is_burning(Object *);
E void obj_split_light_source(Object *, Object *);
E void obj_merge_light_sources(Object *, Object *);
E int candle_light_range(Object *);
#ifdef WIZARD
E int wiz_light_sources();
#endif

/* ### lock.c ### */

E bool picking_lock(int *, int *);
E bool picking_at(int, int);
E void reset_pick();
E int pick_lock(Object *);
E int doforce();
E bool boxlock(Object *, Object *);
E bool doorlock(Object *, int, int);
E int doopen();
E int doclose();

/* ### mail.c ### */

#ifdef MAIL
E void getmailstatus();
E void ckmailstatus();
E void readmail(Object *);
#endif /* MAIL */

/* ### makemon.c ### */

E bool IsLevelHomeOfElementalType(MonsterType *);
E Monster *clone_mon(Monster *, xchar, xchar);
E Monster *makemon(MonsterType *, int, int, int);
E bool create_critters(int, MonsterType *);
E MonsterType *rndmonst();
E void reset_rndmonst(int);
E MonsterType *mkclass(char, int);
E int adj_lev(MonsterType *);
E MonsterType *grow_up(Monster *, Monster *);
E int mongets(Monster *, int);
E int golemhp(int);
E bool peace_minded(MonsterType *);
E void set_malign(Monster *);
E void set_mimic_sym(Monster *);
E int mbirth_limit(int);
E void mimic_hit_msg(Monster *, short);
#ifdef GOLDOBJ
E void mkmonmoney(Monster *, long);
#endif
E void bagotricks(Object *);
E bool propagate(int, bool, bool);

/* ### mapglyph.c ### */

E void mapglyph(int, int *, int *, unsigned *, int, int);

/* ### mcastu.c ### */

E int castmu(Monster *, struct Attack *, bool, bool);
E int buzzmu(Monster *, struct Attack *);

/* ### mhitm.c ### */

E int fightm(Monster *);
E int mattackm(Monster *, Monster *);
E int noattacks(MonsterType *);
E int sleep_monst(Monster *, int, int);
E void slept_monst(Monster *);
E long attk_protection(int);

/* ### mhitu.c ### */

E const char *mpoisons_subj(Monster *, struct Attack *);
E void u_slow_down();
E Monster *cloneu();
E void expels(Monster *, MonsterType *, bool);
E struct Attack *getmattk(MonsterType *, int, int *, struct Attack *);
E int mattacku(Monster *);
E int magic_negation(Monster *);
E int gazemu(Monster *, struct Attack *);
E void mdamageu(Monster *, int);
E int could_seduce(Monster *, Monster *, struct Attack *);
#ifdef SEDUCE
E int doseduce(Monster *);
#endif

/* ### minion.c ### */

E void msummon(Monster *);
E void summon_minion(ALIGNTYP_P, bool);
E int demon_talk(Monster *);
E long bribe(Monster *);
E int dprince(ALIGNTYP_P);
E int dlord(ALIGNTYP_P);
E int llord();
E int ndemon(ALIGNTYP_P);
E int lminion();

/* ### mklev.c ### */

E void sort_rooms();
E void add_room(int, int, int, int, bool, schar, bool);
E void add_subroom(struct mkroom *, int, int, int, int, bool, schar, bool);
E void makecorridors();
E void add_door(int, int, struct mkroom *);
E void mklev();
#ifdef SPECIALIZATION
E void topologize(struct mkroom *, bool);
#else
E void topologize(struct mkroom *);
#endif
E void place_branch(branch *, xchar, xchar);
E bool occupied(xchar, xchar);
E int okdoor(xchar, xchar);
E void dodoor(int, int, struct mkroom *);
E void mktrap(int, int, struct mkroom *, coord *);
E void mkstairs(xchar, xchar, char, struct mkroom *);
E void mkinvokearea();

/* ### mkmap.c ### */

void flood_fill_rm(int, int, int, bool, bool);
void remove_rooms(int, int, int, int);

/* ### mkmaze.c ### */

E void wallification(int, int, int, int);
E void walkfrom(int, int);
E void makemaz(const char *);
E void mazexy(coord *);
E void bound_digging();
E void mkportal(xchar, xchar, xchar, xchar);
E bool bad_location(xchar, xchar, xchar, xchar, xchar, xchar);
E void place_lregion(xchar, xchar, xchar, xchar, xchar, xchar, xchar, xchar,
                     xchar, d_level *);
E void movebubbles();
E void water_friction();
E void save_waterlevel(int, int);
E void restore_waterlevel(int);
E const char *waterbody_name(xchar, xchar);

/* ### mkobj.c ### */

E Object *MakeRandomObjectAt(char, int, int, bool);
E Object *MakeSpecificObjectAt(int, int, int, bool, bool);
E Object *MakeRandomObject(char, bool);
E int PickRandomMonsterTypeIndex();
E Object *SplitObject(Object *, long);
E void ReplaceObject(Object *, Object *);
E void CreateBillDummyObject(Object *);
E Object *MakeSpecificObject(int, bool, bool);
E int GetBUCSign(Object *);
E int GetWeight(Object *);
E Object *MakeGold(long, int, int);
E Object *MakeCorpseOrStatue(int, Monster *, MonsterType *, int, int, bool);
E Object *AttachMonsterIdToObject(Object *, unsigned);
E Monster *NewMonsterFromObject(Object *, bool);
E Object *NewTopTenObject(int, int, int);
E Object *MakeNamedCorpseOrStatue(int, MonsterType *, int, int, const char *);
E Object *MakeRandomTreefruitAt(int, int);
E void StartCorpseTimeout(Object *);
E void Bless(Object *);
E void Unbless(Object *);
E void Curse(Object *);
E void Uncurse(Object *);
E void BlessOrCurse(Object *, int);
E bool IsFlammable(Object *);
E bool IsRottable(Object *);
E void PlaceObject(Object *, int, int);
E void RemoveObjectFromFloor(Object *);
E void DiscardMonsterInventory(Monster *);
E void RemoveObjectFromStorage(Object *);
E void ExtractObjectFromList(Object *, Object **);
E void extract_nexthere(Object *, Object **);
E int AddObjectToMonsterInventory(Monster *, Object *);
E Object *AddObjectToContainer(Object *, Object *);
E void AddObjectToMigrationList(Object *);
E void AddToBuriedList(Object *);
E void DeallocateObject(Object *);
E void ApplyIceEffectsAt(int, int, bool);
E long PeekAtIcedCorpseAge(Object *);
#ifdef WIZARD
E void SanityCheckObjects();
#endif

/* ### mkroom.c ### */

E void mkroom(int);
E void fill_zoo(struct mkroom *);
E bool nexttodoor(int, int);
E bool has_dnstairs(struct mkroom *);
E bool has_upstairs(struct mkroom *);
E int somex(struct mkroom *);
E int somey(struct mkroom *);
E bool inside_room(struct mkroom *, xchar, xchar);
E bool somexy(struct mkroom *, coord *);
E void mkundead(coord *, bool, int);
E MonsterType *courtmon();
E void save_rooms(int);
E void rest_rooms(int);
E struct mkroom *search_special(schar);

/* ### mon.c ### */

E int undead_to_corpse(int);
E int genus(int, int);
E int pm_to_cham(int);
E int minliquid(Monster *);
E int movemon();
E int meatmetal(Monster *);
E int meatobj(Monster *);
E void mpickgold(Monster *);
E bool mpickstuff(Monster *, const char *);
E int curr_mon_load(Monster *);
E int max_mon_load(Monster *);
E bool can_carry(Monster *, Object *);
E int mfndpos(Monster *, coord *, long *, long);
E bool monnear(Monster *, int, int);
E void dmonsfree();
E int mcalcmove(Monster *);
E void mcalcdistress();
E void replmon(Monster *, Monster *);
E void relmon(Monster *);
E Object *mlifesaver(Monster *);
E bool corpse_chance(Monster *, Monster *, bool);
E void mondead(Monster *);
E void mondied(Monster *);
E void mongone(Monster *);
E void monstone(Monster *);
E void monkilled(Monster *, const char *, int);
E void unstuck(Monster *);
E void killed(Monster *);
E void xkilled(Monster *, int);
E void mon_to_stone(Monster *);
E void mnexto(Monster *);
E bool mnearto(Monster *, xchar, xchar, bool);
E void poisontell(int);
E void poisoned(const char *, int, const char *, int);
E void m_respond(Monster *);
E void setmangry(Monster *);
E void wakeup(Monster *);
E void wake_nearby();
E void wake_nearto(int, int, int);
E void seemimic(Monster *);
E void rescham();
E void restartcham();
E void restore_cham(Monster *);
E void mon_animal_list(bool);
E int newcham(Monster *, MonsterType *, bool, bool);
E int can_be_hatched(int);
E int egg_type_from_parent(int, bool);
E bool dead_species(int, bool);
E void kill_genocided_monsters();
E void golemeffects(Monster *, int, int);
E bool angry_guards(bool);
E void pacify_guards();

/* ### mondata.c ### */

E void set_mon_data(Monster *, MonsterType *, int);
E struct Attack *attacktype_fordmg(MonsterType *, int, int);
E bool attacktype(MonsterType *, int);
E bool poly_when_stoned(MonsterType *);
E bool resists_drli(Monster *);
E bool resists_magm(Monster *);
E bool resists_blnd(Monster *);
E bool can_blnd(Monster *, Monster *, uchar, Object *);
E bool ranged_attk(MonsterType *);
E bool hates_silver(MonsterType *);
E bool passes_bars(MonsterType *);
E bool can_track(MonsterType *);
E bool breakarm(MonsterType *);
E bool sliparm(MonsterType *);
E bool sticks(MonsterType *);
E int num_horns(MonsterType *);
/* E bool canseemon(struct monst *); */
E struct Attack *dmgtype_fromattack(MonsterType *, int, int);
E bool dmgtype(MonsterType *, int);
E int max_passive_dmg(Monster *, Monster *);
E int monsndx(MonsterType *);
E int name_to_mon(const char *);
E int gender(Monster *);
E int pronoun_gender(Monster *);
E bool levl_follower(Monster *);
E int little_to_big(int);
E int big_to_little(int);
E const char *locomotion(const MonsterType *, const char *);
E const char *stagger(const MonsterType *, const char *);
E const char *on_fire(MonsterType *, struct Attack *);
E const MonsterType *raceptr(Monster *);

/* ### monmove.c ### */

E bool itsstuck(Monster *);
E bool mb_trapped(Monster *);
E void mon_regen(Monster *, bool);
E int dochugw(Monster *);
E bool onscary(int, int, Monster *);
E void monflee(Monster *, int, bool, bool);
E int dochug(Monster *);
E int m_move(Monster *, int);
E bool closed_door(int, int);
E bool accessible(int, int);
E void set_apparxy(Monster *);
E bool can_ooze(Monster *);

/* ### monst.c ### */

E void monst_init();

/* ### monstr.c ### */

E void monstr_init();

/* ### mplayer.c ### */

E Monster *mk_mplayer(MonsterType *, xchar, xchar, bool);
E void create_mplayers(int, bool);
E void mplayer_talk(Monster *);

/* ### mthrowu.c ### */

E int thitu(int, int, Object *, const char *);
E int ohitmon(Monster *, Object *, int, bool);
E void thrwmu(Monster *);
E int spitmu(Monster *, struct Attack *);
E int breamu(Monster *, struct Attack *);
E bool linedup(xchar, xchar, xchar, xchar);
E bool lined_up(Monster *);
E Object *m_carrying(Monster *, int);
E void m_useup(Monster *, Object *);
E void m_throw(Monster *, int, int, int, int, int, Object *);
E bool hits_bars(Object **, int, int, int, int);

/* ### muse.c ### */

E bool find_defensive(Monster *);
E int use_defensive(Monster *);
E int rnd_defensive_item(Monster *);
E bool find_offensive(Monster *);
E int use_offensive(Monster *);
E int rnd_offensive_item(Monster *);
E bool find_misc(Monster *);
E int use_misc(Monster *);
E int rnd_misc_item(Monster *);
E bool searches_for_item(Monster *, Object *);
E bool mon_reflects(Monster *, const char *);
E bool ureflects(const char *, const char *);
E bool munstone(Monster *, bool);

/* ### music.c ### */

E void awaken_soldiers();
E int do_play_instrument(Object *);

/* ### nhlan.c ### */
#ifdef LAN_FEATURES
E void init_lan_features();
E char *lan_username();
#ifdef LAN_MAIL
E bool lan_mail_check();
E void lan_mail_read(Object *);
E void lan_mail_init();
E void lan_mail_finish();
E void lan_mail_terminate();
#endif
#endif

/* ### o_init.c ### */

E void init_objects();
E int find_skates();
E void oinit();
E void savenames(int, int);
E void restnames(int);
E void discover_object(int, bool, bool);
E void undiscover_object(int);
E int dodiscovered();

/* ### objects.c ### */

E void objects_init();

/* ### objnam.c ### */

E char *obj_typename(int);
E char *simple_typename(int);
E bool obj_is_pname(Object *);
E char *distant_name(Object *, char *(*)(Object *));
E char *fruitname(bool);
E char *xname(Object *);
E char *mshot_xname(Object *);
E bool the_unique_obj(Object *obj);
E char *doname(Object *);
E bool not_fully_identified(Object *);
E char *corpse_xname(Object *, bool);
E char *cxname(Object *);
#ifdef SORTLOOT
E char *cxname2(Object *);
#endif
E char *killer_xname(Object *);
E const char *singular(Object *, char *(*)(Object *));
E char *an(const char *);
E char *An(const char *);
E char *The(const char *);
E char *the(const char *);
E char *aobjnam(Object *, const char *);
E char *Tobjnam(Object *, const char *);
E char *otense(Object *, const char *);
E char *vtense(const char *, const char *);
E char *Doname2(Object *);
E char *yname(Object *);
E char *Yname2(Object *);
E char *ysimple_name(Object *);
E char *Ysimple_name2(Object *);
E char *makeplural(const char *);
E char *makesingular(const char *);
E Object *readobjnam(char *, Object *, bool);
E int rnd_class(int, int);
E const char *cloak_simple_name(Object *);
E const char *mimic_obj_name(Monster *);

/* ### options.c ### */

E bool match_optname(const char *, const char *, int, bool);
E void initoptions();
E void parseoptions(char *, bool, bool);
E int doset();
E int dotogglepickup();
E void option_help();
E void next_opt(winid, const char *);
E int fruitadd(char *);
E int choose_classes_menu(const char *, int, bool, char *, char *);
E void add_menu_cmd_alias(char, char);
E char map_menu_cmd(char);
E void assign_warnings(uchar *);
E char *nh_getenv(const char *);
E void set_duplicate_opt_detection(int);
E void set_wc_option_mod_status(unsigned long, int);
E void set_wc2_option_mod_status(unsigned long, int);
E void set_option_mod_status(const char *, int);
#ifdef AUTOPICKUP_EXCEPTIONS
E int add_autopickup_exception(const char *);
E void free_autopickup_exceptions();
#endif /* AUTOPICKUP_EXCEPTIONS */
#ifdef MENU_COLOR
E bool add_menu_coloring(char *);
#endif /* MENU_COLOR */

/* ### pager.c ### */

E int dowhatis();
E int doquickwhatis();
E int doidtrap();
E int dowhatdoes();
E char *dowhatdoes_core(char, char *);
E int dohelp();
E int dohistory();

/* ### pickup.c ### */

#ifdef GOLDOBJ
E int collect_obj_classes(char *, Object *, bool, bool (*)(Object *), int *);
#else
E int collect_obj_classes(char *, Object *, bool, bool, bool (*)(Object *),
                          int *);
#endif
E void add_valid_menu_class(int);
E bool allow_all(Object *);
E bool allow_category(Object *);
E bool is_worn_by_type(Object *);
E int pickup(int);
E int pickup_object(Object *, long, bool);
E int query_category(const char *, Object *, int, menu_item **, int);
E int query_objlist(const char *, Object *, int, menu_item **, int,
                    bool (*)(Object *));
E Object *pick_obj(Object *);
E int encumber_msg();
E int doloot();
E int use_container(Object *, int);
E int loot_mon(Monster *, int *, bool *);
E const char *safe_qbuf(const char *, unsigned, const char *, const char *,
                        const char *);
E bool is_autopickup_exception(Object *, bool);

/* ### pline.c ### */

E void msgpline_add(int, char *);
E void msgpline_free();
E void pline(const char *, ...) PRINTF_F(1, 2);
E void Norep(const char *, ...) PRINTF_F(1, 2);
E void free_youbuf();
E void You(const char *, ...) PRINTF_F(1, 2);
E void Your(const char *, ...) PRINTF_F(1, 2);
E void You_feel(const char *, ...) PRINTF_F(1, 2);
E void You_cant(const char *, ...) PRINTF_F(1, 2);
E void You_hear(const char *, ...) PRINTF_F(1, 2);
E void pline_The(const char *, ...) PRINTF_F(1, 2);
E void There(const char *, ...) PRINTF_F(1, 2);
E void verbalize(const char *, ...) PRINTF_F(1, 2);
E void raw_printf(const char *, ...) PRINTF_F(1, 2);
E void impossible(const char *, ...) PRINTF_F(1, 2);
E const char *align_str(ALIGNTYP_P);
E void mstatusline(Monster *);
E void ustatusline();
E void self_invis_message();

/* ### polyself.c ### */

E void set_uasmon();
E void change_sex();
E void polyself(bool);
E int polymon(int);
E void rehumanize();
E int dobreathe();
E int dospit();
E int doremove();
E int dospinweb();
E int dosummon();
E int dogaze();
E int dohide();
E int domindblast();
E void skinback(bool);
E const char *mbodypart(Monster *, int);
E const char *body_part(int);
E int poly_gender();
E void ugolemeffects(int, int);

/* ### potion.c ### */

E void set_itimeout(long *, long);
E void incr_itimeout(long *, int);
E void make_confused(long, bool);
E void make_stunned(long, bool);
E void make_blinded(long, bool);
E void make_sick(long, const char *, bool, int);
E void make_vomiting(long, bool);
E bool make_hallucinated(long, bool, long);
E int dodrink();
E int dopotion(Object *);
E int peffects(Object *);
E void healup(int, int, bool, bool);
E void strange_feeling(Object *, const char *);
E void potionhit(Monster *, Object *, bool);
E void potionbreathe(Object *);
E bool get_wet(Object *);
E int dodip();
E void djinni_from_bottle(Object *);
E Monster *split_mon(Monster *, Monster *);
E const char *bottlename();

/* ### pray.c ### */

E int dosacrifice();
E bool can_pray(bool);
E int dopray();
E const char *u_gname();
E int doturn();
E const char *a_gname();
E const char *a_gname_at(xchar x, xchar y);
E const char *align_gname(ALIGNTYP_P);
E const char *halu_gname(ALIGNTYP_P);
E const char *align_gtitle(ALIGNTYP_P);
E void altar_wrath(int, int);

/* ### priest.c ### */

E int move_special(Monster *, bool, schar, bool, bool, xchar, xchar, xchar,
                   xchar);
E char temple_occupied(char *);
E int pri_move(Monster *);
E void priestini(d_level *, struct mkroom *, int, int, bool);
E char *priestname(Monster *, char *);
E bool p_coaligned(Monster *);
E Monster *findpriest(char);
E void intemple(int);
E void priest_talk(Monster *);
E Monster *mk_roamer(MonsterType *, ALIGNTYP_P, xchar, xchar, bool);
E void reset_hostility(Monster *);
E bool in_your_sanctuary(Monster *, xchar, xchar);
E void ghod_hitsu(Monster *);
E void angry_priest();
E void clearpriests();
E void restpriest(Monster *, bool);

/* ### quest.c ### */

E void onquest();
E void nemdead();
E void artitouch();
E bool ok_to_quest();
E void leader_speaks(Monster *);
E void nemesis_speaks();
E void quest_chat(Monster *);
E void quest_talk(Monster *);
E void quest_stat_check(Monster *);
E void finish_quest(Object *);

/* ### questpgr.c ### */

E void load_qtlist();
E void unload_qtlist();
E short quest_info(int);
E const char *ldrname();
E bool is_quest_artifact(Object *);
E void com_pager(int);
E void qt_pager(int);
E MonsterType *qt_montype();

/* ### random.c ### */

#if defined(RANDOM) && !defined(__GO32__) /* djgpp has its own random */
E void srandom(unsigned);
E char *initstate(unsigned, char *, int);
E char *setstate(char *);
E long random();
#endif /* RANDOM */

/* ### read.c ### */

E int doread();
E bool is_chargeable(Object *);
E void recharge(Object *, int);
E void forget_objects(int);
E void forget_levels(int);
E void forget_traps();
E void forget_map(int);
E int seffects(Object *);
E void litroom(bool, Object *);
E void do_genocide(int);
E void punish(Object *);
E void unpunish();
E bool cant_create(int *, bool);
#ifdef WIZARD
E bool create_particular();
#endif

/* ### rect.c ### */

E void init_rect();
E NhRect *get_rect(NhRect *);
E NhRect *rnd_rect();
E void remove_rect(NhRect *);
E void add_rect(NhRect *);
E void split_rects(NhRect *, NhRect *);

/* ## region.c ### */
E void clear_regions();
E void run_regions();
E bool in_out_region(xchar, xchar);
E bool m_in_out_region(Monster *, xchar, xchar);
E void update_player_regions();
E void update_monster_region(Monster *);
E NhRegion *visible_region_at(xchar, xchar);
E void show_region(NhRegion *, xchar, xchar);
E void save_regions(int, int);
E void rest_regions(int, bool);
E NhRegion *create_gas_cloud(xchar, xchar, int, int);

/* ### restore.c ### */

E void inven_inuse(bool);
E int dorecover(int);
E void trickery(char *);
E void getlev(int, int, xchar, bool);
E void minit();
E bool lookup_id_mapping(unsigned long, unsigned long *);
#ifdef ZEROCOMP
E int mread(int, genericptr_t, unsigned int);
#else
E void mread(int, genericptr_t, unsigned int);
#endif

/* ### rip.c ### */

E void genl_outrip(winid, int);

/* ### rnd.c ### */

E void check_reseed();
E int rn2(int);
E int rnl(int);
E int rnd(int);
E int d(int, int);
E int rne(int);
E int rnz(int);

/* ### role.c ### */

E bool validrole(int);
E bool validrace(int, int);
E bool validgend(int, int, int);
E bool validalign(int, int, int);
E int randrole();
E int randrace(int);
E int randgend(int, int);
E int randalign(int, int);
E int str2role(char *);
E int str2race(char *);
E int str2gend(char *);
E int str2align(char *);
E bool ok_role(int, int, int, int);
E int pick_role(int, int, int, int);
E bool ok_race(int, int, int, int);
E int pick_race(int, int, int, int);
E bool ok_gend(int, int, int, int);
E int pick_gend(int, int, int, int);
E bool ok_align(int, int, int, int);
E int pick_align(int, int, int, int);
E void role_init();
E void rigid_role_checks();
E void plnamesuffix();
E const char *Hello(Monster *);
E const char *Goodbye();
E char *build_plselection_prompt(char *, int, int, int, int, int);
E char *root_plselection_prompt(char *, int, int, int, int, int);

/* ### rumors.c ### */

E char *getrumor(int, char *, bool);
E void outrumor(int, int);
E void outoracle(bool, bool);
E void save_oracles(int, int);
E void restore_oracles(int);
E int doconsult(Monster *);

/* ### save.c ### */

E int dosave();
E void hangup(int);
E int dosave0();
#ifdef INSURANCE
E void savestateinlock();
#endif
E void savelev(int, xchar, int);
E void bufon(int);
E void bufoff(int);
E void bflush(int);
E void bwrite(int, void const*, unsigned int);
E void bclose(int);
E void savefruitchn(int, int);
E void free_dungeons();
E void freedynamicdata();

/* ### shk.c ### */

#ifdef GOLDOBJ
E long money2mon(Monster *, long);
E void money2u(Monster *, long);
#endif
E char *shkname(Monster *);
E void shkgone(Monster *);
E void set_residency(Monster *, bool);
E void replshk(Monster *, Monster *);
E void restshk(Monster *, bool);
E char inside_shop(xchar, xchar);
E void u_left_shop(char *, bool);
E void remote_burglary(xchar, xchar);
E void u_entered_shop(char *);
E bool same_price(Object *, Object *);
E void shopper_financial_report();
E int inhishop(Monster *);
E Monster *shop_keeper(char);
E bool tended_shop(struct mkroom *);
E void delete_contents(Object *);
E void obfree(Object *, Object *);
E void home_shk(Monster *, bool);
E void make_happy_shk(Monster *, bool);
E void hot_pursuit(Monster *);
E void make_angry_shk(Monster *, xchar, xchar);
E int dopay();
E bool paybill(int);
E void finish_paybill();
E Object *find_oid(unsigned);
E long contained_cost(Object *, Monster *, long, bool, bool);
E long contained_gold(Object *);
E void picked_container(Object *);
E long unpaid_cost(Object *);
E void addtobill(Object *, bool, bool, bool);
E void splitbill(Object *, Object *);
E void subfrombill(Object *, Monster *);
E long stolen_value(Object *, xchar, xchar, bool, bool);
E void sellobj_state(int);
E void sellobj(Object *, xchar, xchar);
E int doinvbill(int);
E Monster *shkcatch(Object *, xchar, xchar);
E void add_damage(xchar, xchar, long);
E int repair_damage(Monster *, struct damage *, bool);
E int shk_move(Monster *);
E void after_shk_move(Monster *);
E bool is_fshk(Monster *);
E void shopdig(int);
E void pay_for_damage(const char *, bool);
E bool costly_spot(xchar, xchar);
E Object *shop_object(xchar, xchar);
E void price_quote(Object *);
E void shk_chat(Monster *);
E void check_unpaid_usage(Object *, bool);
E void check_unpaid(Object *);
E void costly_gold(xchar, xchar, long);
E bool block_door(xchar, xchar);
E bool block_entry(xchar, xchar);
E char *shk_your(char *, Object *);
E char *Shk_Your(char *, Object *);

/* ### shknam.c ### */

E void stock_room(int, struct mkroom *);
E bool saleable(Monster *, Object *);
E int get_shop_item(int);

/* ### sit.c ### */

E void take_gold();
E int dosit();
E void rndcurse();
E void attrcurse();

/* ### sounds.c ### */

E void dosounds();
E const char *growl_sound(Monster *);
E void growl(Monster *);
E void yelp(Monster *);
E void whimper(Monster *);
E void beg(Monster *);
E int dotalk();
#ifdef USER_SOUNDS
E int add_sound_mapping(const char *);
E void play_sound_for_message(const char *);
#endif

/* ### sp_lev.c ### */

E bool check_room(xchar *, xchar *, xchar *, xchar *, bool);
E bool create_room(xchar, xchar, xchar, xchar, xchar, xchar, xchar, xchar);
E void create_secret_door(struct mkroom *, xchar);
E bool dig_corridor(coord *, coord *, bool, schar, schar);
E void fill_room(struct mkroom *, bool);
E bool load_special(const char *);

/* ### spell.c ### */

E int study_book(Object *);
E void book_disappears(Object *);
E void book_substitution(Object *, Object *);
E void age_spells();
E int docast();
E int spell_skilltype(int);
E int spelleffects(int, bool);
E void losespells();
E int dovspell();
E void initialspell(Object *);

/* ### steal.c ### */

#ifdef GOLDOBJ
E long somegold(long);
#else
E long somegold();
#endif
E void stealgold(Monster *);
E void remove_worn_item(Object *, bool);
E int steal(Monster *, char *);
E int mpickobj(Monster *, Object *);
E void stealamulet(Monster *);
E void mdrop_special_objs(Monster *);
E void relobj(Monster *, int, bool);
#ifdef GOLDOBJ
E Object *findgold(Object *);
#endif

/* ### steed.c ### */

#ifdef STEED
E void rider_cant_reach();
E bool can_saddle(Monster *);
E int use_saddle(Object *);
E bool can_ride(Monster *);
E int doride();
E bool mount_steed(Monster *, bool);
E void exercise_steed();
E void kick_steed();
E void dismount_steed(int);
E void place_monster(Monster *, int, int);
#endif

/* ### teleport.c ### */

E bool goodpos(int, int, Monster *, unsigned);
E bool enexto(coord *, xchar, xchar, MonsterType *);
E bool enexto_core(coord *, xchar, xchar, MonsterType *, unsigned);
E void teleds(int, int, bool);
E bool safe_teleds(bool);
E bool teleport_pet(Monster *, bool);
E void tele();
E int dotele();
E void level_tele();
E void domagicportal(Trap *);
E void tele_trap(Trap *);
E void level_tele_trap(Trap *);
E void rloc_to(Monster *, int, int);
E bool rloc(Monster *, bool);
E bool tele_restrict(Monster *);
E void mtele_trap(Monster *, Trap *, int);
E int mlevel_tele_trap(Monster *, Trap *, bool, int);
E void rloco(Object *);
E int random_teleport_level();
E bool u_teleport_mon(Monster *, bool);

/* ### tile.c ### */
#ifdef USE_TILES
E void substitute_tiles(d_level *);
#endif

/* ### timeout.c ### */

E void burn_away_slime();
E void nh_timeout();
E void fall_asleep(int, bool);
E void attach_egg_hatch_timeout(Object *);
E void attach_fig_transform_timeout(Object *);
E void kill_egg(Object *);
E void hatch_egg(genericptr_t, long);
E void learn_egg_type(int);
E void burn_object(genericptr_t, long);
E void begin_burn(Object *, bool);
E void end_burn(Object *, bool);
E void do_storms();
E bool start_timer(long, short, short, genericptr_t);
E long stop_timer(short, genericptr_t);
E void run_timers();
E void obj_move_timers(Object *, Object *);
E void obj_split_timers(Object *, Object *);
E void obj_stop_timers(Object *);
E bool obj_is_local(Object *);
E void save_timers(int, int, int);
E void restore_timers(int, int, bool, long);
E void relink_timers(bool);
#ifdef WIZARD
E int wiz_timeout_queue();
E void timer_sanity_check();
#endif

/* ### topten.c ### */

E void topten(int);
E void prscore(int, char **);
E Object *tt_oname(Object *);

/* ### track.c ### */

E void initrack();
E void settrack();
E coord *gettrack(int, int);

/* ### trap.c ### */

E bool burnarmor(Monster *);
E bool rust_dmg(Object *, const char *, int, bool, Monster *);
E void grease_protect(Object *, const char *, Monster *);
E Trap *maketrap(int, int, int);
E void fall_through(bool);
E Monster *animate_statue(Object *, xchar, xchar, int, int *);
E Monster *activate_statue_trap(Trap *, xchar, xchar, bool);
E void dotrap(Trap *, unsigned);
E void seetrap(Trap *);
E int mintrap(Monster *);
E void instapetrify(const char *);
E void minstapetrify(Monster *, bool);
E void selftouch(const char *);
E void mselftouch(Monster *, const char *, bool);
E void float_up();
E void fill_pit(int, int);
E int float_down(long, long);
E int fire_damage(Object *, bool, bool, xchar, xchar);
E bool water_damage(Object *, bool, bool);
E bool drown();
E void drain_en(int);
E int dountrap();
E int untrap(bool);
E bool chest_trap(Object *, int, bool);
E void deltrap(Trap *);
E bool delfloortrap(Trap *);
E Trap *t_at(int, int);
E void b_trapped(const char *, int);
E bool unconscious();
E bool lava_effects();
E void blow_up_landmine(Trap *);
E int launch_obj(short, int, int, int, int, int);

/* ### u_init.c ### */

E void u_init();

/* ### uhitm.c ### */

E void hurtmarmor(Monster *, int);
E bool attack_checks(Monster *, Object *);
E void check_caitiff(Monster *);
E schar find_roll_to_hit(Monster *);
E bool attack(Monster *);
E bool hmon(Monster *, Object *, int);
E int damageum(Monster *, struct Attack *);
E void missum(Monster *, struct Attack *);
E int passive(Monster *, bool, int, uchar);
E void passive_obj(Monster *, Object *, struct Attack *);
E void stumble_onto_mimic(Monster *);
E int flash_hits_mon(Monster *, Object *);

/* ### unixmain.c ### */

#ifdef PORT_HELP
E void port_help();
#endif

/* ### unixtty.c ### */

E void gettty();
E void settty(const char *);
E void setftty();
E void intron();
E void introff();
E void ErrMsg(const char *, ...) PRINTF_F(1, 2);
E void VErrMsg(const char *, va_list);
E void error(const char *, ...) PRINTF_F(1, 2);

/* ### unixunix.c ### */

E void getlock();
E void regularize(char *);
#if defined(TIMED_DELAY) && !defined(msleep) && defined(SYSV)
E void msleep(unsigned);
#endif
#ifdef SHELL
E int dosh();
#endif /* SHELL */
#if defined(SHELL) || defined(DEF_PAGER) || defined(DEF_MAILREADER)
E int child(int);
#endif

/* ### vault.c ### */

E bool grddead(Monster *);
E char vault_occupied(char *);
E void invault();
E int gd_move(Monster *);
E void paygd();
E long hidden_gold();
E bool gd_sound();

/* ### version.c ### */

E char *version_string(char *);
E char *getversionstring(char *);
E int doversion();
E int doextversion();
E bool check_version(struct version_info *, const char *, bool);
E unsigned long get_feature_notice_ver(char *);
E unsigned long get_current_feature_ver();
#ifdef RUNTIME_PORT_ID
E void append_port_id(char *);
#endif

/* ### video.c ### */

#ifdef VIDEOSHADES
E int assign_videoshades(char *);
E int assign_videocolors(char *);
#endif

/* ### vis_tab.c ### */

#ifdef VISION_TABLES
E void vis_tab_init();
#endif

/* ### vision.c ### */

E void vision_init();
E int does_block(int, int, struct rm *);
E void vision_reset();
E void vision_recalc(int);
E void block_point(int, int);
E void unblock_point(int, int);
E bool clear_path(int, int, int, int);
E void do_clear_area(int, int, int, void (*)(int, int, genericptr_t),
                     genericptr_t);

/* ### weapon.c ### */

E int hitval(Object *, Monster *);
E int dmgval(Object *, Monster *);
E Object *select_rwep(Monster *);
E Object *select_hwep(Monster *);
E void possibly_unwield(Monster *, bool);
E int mon_wield_item(Monster *);
E int abon();
E int dbon();
E int enhance_weapon_skill();
#ifdef DUMP_LOG
E void dump_weapon_skill();
#endif
E void unrestrict_weapon_skill(int);
E void use_skill(int, int);
E void add_weapon_skill(int);
E void lose_weapon_skill(int);
E int weapon_type(Object *);
E int uwep_skill_type();
E int weapon_hit_bonus(Object *);
E int weapon_dam_bonus(Object *);
E void skill_init(const struct def_skill *);

/* ### were.c ### */

E void were_change(Monster *);
E void new_were(Monster *);
E int were_summon(MonsterType *, bool, int *, char *);
E void you_were();
E void you_unwere(bool);

/* ### wield.c ### */

E void setuwep(Object *);
E void setuqwep(Object *);
E void setuswapwep(Object *);
E int dowield();
E int doswapweapon();
E int dowieldquiver();
E bool wield_tool(Object *, const char *);
E int can_twoweapon();
E void drop_uswapwep();
E int dotwoweapon();
E void uwepgone();
E void uswapwepgone();
E void uqwepgone();
E void untwoweapon();
E void erode_obj(Object *, bool, bool);
E int chwepon(Object *, int);
E int welded(Object *);
E void weldmsg(Object *);
E void setmnotwielded(Monster *, Object *);

/* ### windows.c ### */

E void choose_windows(const char *);
E char genl_message_menu(char, int, const char *);
E void genl_preference_update(const char *);

/* ### wizard.c ### */

E void amulet();
E int mon_has_amulet(Monster *);
E int mon_has_special(Monster *);
E int tactics(Monster *);
E void aggravate();
E void clonewiz();
E int pick_nasty();
E int nasty(Monster *);
E void resurrect();
E void intervene();
E void wizdead();
E void cuss(Monster *);

/* ### worm.c ### */

E int get_wormno();
E void initworm(Monster *, int);
E void worm_move(Monster *);
E void worm_nomove(Monster *);
E void wormgone(Monster *);
E void wormhitu(Monster *);
E void cutworm(Monster *, xchar, xchar, Object *);
E void see_wsegs(Monster *);
E void detect_wsegs(Monster *, bool);
E void save_worm(int, int);
E void rest_worm(int);
E void place_wsegs(Monster *);
E void remove_worm(Monster *);
E void place_worm_tail_randomly(Monster *, xchar, xchar);
E int count_wsegs(Monster *);
E bool worm_known(Monster *);

/* ### worn.c ### */

E void setworn(Object *, long);
E void setnotworn(Object *);
E void mon_set_minvis(Monster *);
E void mon_adjust_speed(Monster *, int, Object *);
E void update_mon_intrinsics(Monster *, Object *, bool, bool);
E int find_mac(Monster *);
E void m_dowear(Monster *, bool);
E Object *which_armor(Monster *, long);
E void mon_break_armor(Monster *, bool);
E void bypass_obj(Object *);
E void clear_bypasses();
E int racial_exception(Monster *, Object *);

/* ### write.c ### */

E int dowrite(Object *);

/* ### zap.c ### */

E int bhitm(Monster *, Object *);
E void probe_monster(Monster *);
E bool get_obj_location(Object *, xchar *, xchar *, int);
E bool get_mon_location(Monster *, xchar *, xchar *, int);
E Monster *get_container_location(Object *obj, int *, int *);
E Monster *montraits(Object *, coord *);
E Monster *revive(Object *);
E int unturn_dead(Monster *);
E void cancel_item(Object *);
E bool drain_item(Object *);
E Object *poly_obj(Object *, int);
E bool obj_resists(Object *, int, int);
E bool obj_shudders(Object *);
E void do_osshock(Object *);
E int bhito(Object *, Object *);
E int bhitpile(Object *, int (*)(Object *, Object *), int, int);
E int zappable(Object *);
E void zapnodir(Object *);
E int dozap();
E int zapyourself(Object *, bool);
E bool cancel_monst(Monster *, Object *, bool, bool, bool);
E void weffects(Object *);
E int spell_damage_bonus();
E const char *exclam(int force);
E void hit(const char *, Monster *, const char *);
E void miss(const char *, Monster *);
E Monster *bhit(int, int, int, int, int (*)(Monster *, Object *),
                int (*)(Object *, Object *), Object *, bool *);
E Monster *boomhit(int, int);
E int burn_floor_paper(int, int, bool, bool);
E void buzz(int, int, xchar, xchar, int, int);
E void melt_ice(xchar, xchar);
E int zap_over_floor(xchar, xchar, int, bool *);
E void fracture_rock(Object *);
E bool break_statue(Object *);
E void destroy_item(int, int);
E int destroy_mitem(Monster *, int, int);
E int resist(Monster *, char, int, int);
E void makewish();

#endif /* !MAKEDEFS_C && !LEV_LEX_C */

#undef E

#endif /* EXTERN_H */
