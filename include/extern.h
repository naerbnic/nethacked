/*	SCCS Id: @(#)extern.h	3.4	2003/03/10	*/
/* Copyright (c) Steve Creps, 1988.				  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef EXTERN_H
#define EXTERN_H

#define E extern

/* ### alloc.c ### */

E char *fmt_ptr(const void*,char *);

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
E void o_unleash(struct Object *);
E void m_unleash(struct Monster *,bool);
E void unleash_all();
E bool next_to_u();
E struct Object *get_mleash(struct Monster *);
E void check_leash(xchar,xchar);
E bool um_dist(xchar,xchar,xchar);
E bool snuff_candle(struct Object *);
E bool snuff_lit(struct Object *);
E bool catch_lit(struct Object *);
E void use_unicorn_horn(struct Object *);
E bool tinnable(struct Object *);
E void reset_trapset();
E void fig_transform(genericptr_t, long);
E int unfixable_trouble_count(bool);

/* ### artifact.c ### */

E void init_artifacts();
E void save_artifacts(int);
E void restore_artifacts(int);
E const char *artiname(int);
E struct Object *mk_artifact(struct Object *,ALIGNTYP_P);
E const char *artifact_name(const char *,short *);
E bool exist_artifact(int,const char *);
E void artifact_exists(struct Object *,const char *,bool);
E int nartifact_exist();
E bool spec_ability(struct Object *,unsigned long);
E bool confers_luck(struct Object *);
E bool arti_reflects(struct Object *);
E bool restrict_name(struct Object *,const char *);
E bool defends(int,struct Object *);
E bool protects(int,struct Object *);
E void set_artifact_intrinsic(struct Object *,bool,long);
E int touch_artifact(struct Object *,struct Monster *);
E int spec_abon(struct Object *,struct Monster *);
E int spec_dbon(struct Object *,struct Monster *,int);
E void discover_artifact(xchar);
E bool undiscovered_artifact(xchar);
E int disp_artifact_discoveries(winid);
E bool artifact_hit(struct Monster *,struct Monster *,
				struct Object *,int *,int);
E int doinvoke();
E void arti_speak(struct Object *);
E bool artifact_light(struct Object *);
E long spec_m2(struct Object *);
E bool artifact_has_invprop(struct Object *,uchar);
E long arti_cost(struct Object *);

/* ### attrib.c ### */

E bool adjattrib(int,int,int);
E void change_luck(schar);
E int stone_luck(bool);
E void set_moreluck();
E void gainstr(struct Object *,int);
E void losestr(int);
E void restore_attrib();
E void exercise(int,bool);
E void exerchk();
E void reset_attribute_clock();
E void init_attr(int);
E void redist_attr();
E void adjabil(int,int);
E int newhp();
E schar acurr(int);
E schar acurrstr();
E void adjalign(int);

/* ### ball.c ### */

E void ballfall();
E void placebc();
E void unplacebc();
E void set_bc(int);
E void move_bc(int,int,xchar,xchar,xchar,xchar);
E bool drag_ball(xchar,xchar,
		int *,xchar *,xchar *,xchar *,xchar *, bool *,bool);
E void drop_ball(xchar,xchar);
E void drag_down();

/* ### bones.c ### */

E bool can_make_bones();
E void savebones(struct Object *);
E int getbones();

/* ### botl.c ### */

E int xlev_to_rank(int);
E int title_to_mon(const char *,int *,int *);
E void max_rank_sz();
#ifdef SCORE_ON_BOTL
E long botl_score();
#endif
E int describe_level(char *);
E const char *rank_of(int,short,bool);
E void bot();
#ifdef DUMP_LOG
E void bot1str(char *);
E void bot2str(char *);
#endif

/* ### cmd.c ### */

E void reset_occupations();
E void set_occupation(int (*)(void),const char *,int);
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
E int xytod(schar,schar);
E void dtoxy(coord *,int);
E int movecmd(char);
E int getdir(const char *);
E void confdir();
E int isok(int,int);
E int get_adjacent_loc(const char *, const char *, xchar, xchar, coord *);
E const char *click_to_cmd(int,int,int);
E char readchar();
#ifdef WIZARD
E void sanity_check();
#endif
E char yn_function(const char *, const char *, char);

/* ### dbridge.c ### */

E bool is_pool(int,int);
E bool is_lava(int,int);
E bool is_ice(int,int);
E int is_drawbridge_wall(int,int);
E bool is_db_wall(int,int);
E bool find_drawbridge(int *,int*);
E bool create_drawbridge(int,int,int,bool);
E void open_drawbridge(int,int);
E void close_drawbridge(int,int);
E void destroy_drawbridge(int,int);

/* ### decl.c ### */

E void decl_init();

/* ### detect.c ### */

E struct Object *o_in(struct Object*,char);
E struct Object *o_material(struct Object*,unsigned);
E int gold_detect(struct Object *);
E int food_detect(struct Object *);
E int object_detect(struct Object *,int);
E int monster_detect(struct Object *,int);
E int trap_detect(struct Object *);
E const char *level_distance(d_level *);
E void use_crystal_ball(struct Object *);
E void do_mapping();
E void do_vicinity_map();
E void cvt_sdoor_to_door(struct rm *);
E int findit();
E int openit();
E void find_trap(struct trap *);
E int dosearch0(int);
E int dosearch();
E void sokoban_detect();

/* ### dig.c ### */

E bool is_digging();
E int holetime();
E bool dig_check(struct Monster *, bool, int, int);
E void digactualhole(int,int,struct Monster *,int);
E bool dighole(bool);
E int use_pick_axe(struct Object *);
E int use_pick_axe2(struct Object *);
E bool mdig_tunnel(struct Monster *);
E void watch_dig(struct Monster *,xchar,xchar,bool);
E void zap_dig();
E struct Object *bury_an_obj(struct Object *);
E void bury_objs(int,int);
E void unearth_objs(int,int);
E void rot_organic(genericptr_t, long);
E void rot_corpse(genericptr_t, long);
#if 0
E void bury_monst(struct Monster *);
E void bury_you();
E void unearth_you();
E void escape_tomb();
E void bury_obj(struct Object *);
#endif

/* ### display.c ### */

#ifdef INVISIBLE_OBJECTS
E struct Object * vobj_at(xchar,xchar);
#endif /* INVISIBLE_OBJECTS */
E void magic_map_background(xchar,xchar,int);
E void map_background(xchar,xchar,int);
E void map_trap(struct trap *,int);
E void map_object(struct Object *,int);
E void map_invisible(xchar,xchar);
E void unmap_object(int,int);
E void map_location(int,int,int);
E void feel_location(xchar,xchar);
E void newsym(int,int);
E void shieldeff(xchar,xchar);
E void tmp_at(int,int);
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
E void show_glyph(int,int,int);
E void clear_glyph_buffer();
E void row_refresh(int,int,int);
E void cls();
E void flush_screen(int);
#ifdef DUMP_LOG
E void dump_screen();
#endif
E int back_to_glyph(xchar,xchar);
E int zapdir_to_glyph(int,int,int);
E int glyph_at(xchar,xchar);
E void set_wall_state();

/* ### do.c ### */

E int dodrop();
E bool boulder_hits_pool(struct Object *,int,int,bool);
E bool flooreffects(struct Object *,int,int,const char *);
E void doaltarobj(struct Object *);
E bool canletgo(struct Object *,const char *);
E void dropx(struct Object *);
E void dropy(struct Object *);
E void obj_no_longer_held(struct Object *);
E int doddrop();
E int dodown();
E int doup();
#ifdef INSURANCE
E void save_currentstate();
#endif
E void goto_level(d_level *,bool,bool,bool);
E void schedule_goto(d_level *,bool,bool,int,
			     const char *,const char *);
E void deferred_goto();
E bool revive_corpse(struct Object *);
E void revive_mon(genericptr_t, long);
E int donull();
E int dowipe();
E void set_wounded_legs(long,int);
E void heal_legs();

/* ### do_name.c ### */

E int getpos(coord *,bool,const char *);
E struct Monster *christen_monst(struct Monster *,const char *);
E int do_mname();
E struct Object *oname(struct Object *,const char *);
E int ddocall();
E void docall(struct Object *);
E const char *rndghostname();
E char *x_monnam(struct Monster *,int,const char *,int,bool);
E char *l_monnam(struct Monster *);
E char *mon_nam(struct Monster *);
E char *noit_mon_nam(struct Monster *);
E char *Monnam(struct Monster *);
E char *noit_Monnam(struct Monster *);
E char *m_monnam(struct Monster *);
E char *y_monnam(struct Monster *);
E char *Adjmonnam(struct Monster *,const char *);
E char *Amonnam(struct Monster *);
E char *a_monnam(struct Monster *);
E char *distant_monnam(struct Monster *,int,char *);
E const char *rndmonnam();
E const char *hcolor(const char *);
E const char *rndcolor();
#ifdef REINCARNATION
E const char *roguename();
#endif
E struct Object *realloc_obj(
    struct Object *, int, genericptr_t, int, const char *);
E char *coyotename(struct Monster *,char *);

/* ### do_wear.c ### */

E void off_msg(struct Object *);
E void set_wear();
E bool donning(struct Object *);
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
E void Ring_on(struct Object *);
E void Ring_off(struct Object *);
E void Ring_gone(struct Object *);
E void Blindf_on(struct Object *);
E void Blindf_off(struct Object *);
E int dotakeoff();
E int doremring();
E int cursed(struct Object *);
E int armoroff(struct Object *);
E int canwearobj(struct Object *, long *, bool);
E int dowear();
E int doputon();
E void find_ac();
E void glibr();
E struct Object *some_armor(struct Monster *);
E void erode_armor(struct Monster *,bool);
E struct Object *stuck_ring(struct Object *,int);
E struct Object *unchanger();
E void reset_remarm();
E int doddoremarm();
E int destroy_arm(struct Object *);
E void adj_abon(struct Object *,schar);

/* ### dog.c ### */

E void initedog(struct Monster *);
E struct Monster *make_familiar(struct Object *,xchar,xchar,bool);
E struct Monster *makedog();
E void update_mlstmv();
E void losedogs();
E void mon_arrive(struct Monster *,bool);
E void mon_catchup_elapsed_time(struct Monster *,long);
E void keepdogs(bool);
E void migrate_to_level(struct Monster *,xchar,xchar,coord *);
E int dogfood(struct Monster *,struct Object *);
E struct Monster *tamedog(struct Monster *,struct Object *);
E void abuse_dog(struct Monster *);
E void wary_dog(struct Monster *, bool);

/* ### dogmove.c ### */

E int dog_nutrition(struct Monster *,struct Object *);
E int dog_eat(struct Monster *,struct Object *,int,int,bool);
E int dog_move(struct Monster *,int);

/* ### dokick.c ### */

E bool ghitm(struct Monster *,struct Object *);
E void container_impact_dmg(struct Object *);
E int dokick();
E bool ship_object(struct Object *,xchar,xchar,bool);
E void obj_delivery();
E schar down_gate(xchar,xchar);
E void impact_drop(struct Object *,xchar,xchar,xchar);

/* ### dothrow.c ### */

E int dothrow();
E int dofire();
E void hitfloor(struct Object *);
E void hurtle(int,int,int,bool);
E void mhurtle(struct Monster *,int,int,int);
E void throwit(struct Object *,long,bool);
E int omon_adj(struct Monster *,struct Object *,bool);
E int thitmonst(struct Monster *,struct Object *);
E int hero_breaks(struct Object *,xchar,xchar,bool);
E int breaks(struct Object *,xchar,xchar);
E bool breaktest(struct Object *);
E bool walk_path(coord *, coord *, bool (*)(genericptr_t,int,int), genericptr_t);
E bool hurtle_step(genericptr_t, int, int);
#endif /* !MAKEDEFS_C && !LEV_LEX_C */

/* ### drawing.c ### */
#include "drawing.h"

#if !defined(MAKEDEFS_C) && !defined(LEV_LEX_C)
/* ### dungeon.c ### */

E void save_dungeon(int,bool,bool);
E void restore_dungeon(int);
E void insert_branch(branch *,bool);
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
E bool on_level(d_level *,d_level *);
E void next_level(bool);
E void prev_level(bool);
E void u_on_newpos(int,int);
E void u_on_sstairs();
E void u_on_upstairs();
E void u_on_dnstairs();
E bool On_stairs(xchar,xchar);
E void get_level(d_level *,int);
E bool Is_botlevel(d_level *);
E bool Can_fall_thru(d_level *);
E bool Can_dig_down(d_level *);
E bool Can_rise_up(int,int,d_level *);
E bool In_quest(d_level *);
E bool In_mines(d_level *);
E branch *dungeon_branch(const char *);
E bool at_dgn_entrance(const char *);
E bool In_hell(d_level *);
E bool In_V_tower(d_level *);
E bool On_W_tower_level(d_level *);
E bool In_W_tower(int,int,d_level *);
E void find_hell(d_level *);
E void goto_hell(bool,bool);
E void assign_level(d_level *,d_level *);
E void assign_rnd_level(d_level *,d_level *,int);
E int induced_align(int);
E bool Invocation_lev(d_level *);
E xchar level_difficulty();
E schar lev_by_name(const char *);
#ifdef WIZARD
E schar print_dungeon(bool,schar *,xchar *);
#endif

/* ### eat.c ### */

E bool is_edible(struct Object *);
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
E struct Object *floorfood(const char *,int);
E void vomit();
E int eaten_stat(int,struct Object *);
E void food_disappears(struct Object *);
E void food_substitution(struct Object *,struct Object *);
E void fix_petrification();
E void consume_oeaten(struct Object *,int);
E bool maybe_finished_meal(bool);

#endif /* !MAKEDEFS_C && !LEV_LEX_C */
/* ### end.c ### */

E void done1(int);
E int done2();
E void done_in_by(struct Monster *);
E void panic(const char *,...) PRINTF_F(1,2);
E void done(int);
E void container_contents(struct Object *,bool,bool);
#ifdef DUMP_LOG
E void dump(char const*, char const*);
E void do_containerconts(struct Object *,bool,bool,bool,bool);
#endif
E void terminate(int);
E int num_genocides();

#if !defined(MAKEDEFS_C) && !defined(LEV_LEX_C)
/* ### engrave.c ### */

E char *random_engraving(char *);
E void wipeout_text(char *,int,unsigned);
E bool can_reach_floor();
E const char *surface(int,int);
E const char *ceiling(int,int);
E struct engr *engr_at(xchar,xchar);
#ifdef ELBERETH
E int sengr_at(const char *,xchar,xchar);
#endif
E void u_wipe_engr(int);
E void wipe_engr_at(xchar,xchar,xchar);
E void read_engr_at(int,int);
E void make_engr_at(int,int,const char *,long,xchar);
E void del_engr_at(int,int);
E int freehand();
E int doengrave();
E void save_engravings(int,int);
E void rest_engravings(int);
E void del_engr(struct engr *);
E void rloc_engr(struct engr *);
E void make_grave(int,int,const char *);

/* ### exper.c ### */

E int experience(struct Monster *,int);
E void more_experienced(int,int);
E void losexp(const char *);
E void newexplevel();
E void pluslvl(bool);
E long rndexp(bool);

/* ### explode.c ### */

E void explode(int,int,int,int,char,int);
E long scatter(int, int, int, unsigned int, struct Object *);
E void splatter_burning_oil(int, int);

/* ### extralev.c ### */

#ifdef REINCARNATION
E void makeroguerooms();
E void corr(int,int);
E void makerogueghost();
#endif

/* ### files.c ### */

E char *fname_encode(const char *, char, char *, char *, int);
E char *fname_decode(char, char *, char *, int);
E const char *fqname(const char *, int, int);
E FILE *fopen_datafile(const char *,const char *,int);
E bool uptodate(int,const char *);
E void store_version(int);
E void set_levelfile_name(char *,int);
E int create_levelfile(int,char *);
E int open_levelfile(int,char *);
E void delete_levelfile(int);
E void clearlocks();
E int create_bonesfile(d_level*,char **, char *);
E void commit_bonesfile(d_level *);
E int open_bonesfile(d_level*,char **);
E int delete_bonesfile(d_level*);
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
E bool lock_file(const char *,int,int);
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
E char** get_saved_games();
E void free_saved_games(char**);
#ifdef SELF_RECOVER
E bool recover_savefile();
#endif
#ifdef HOLD_LOCKFILE_OPEN
E void really_close();
#endif

/* ### fountain.c ### */

E void floating_above(const char *);
E void dogushforth(int);
E void dryup(xchar,xchar, bool);
E void drinkfountain();
E void dipfountain(struct Object *);
#ifdef SINKS
E void breaksink(int,int);
E void drinksink();
#endif

/* ### hack.c ### */

E bool revive_nasty(int,int,const char*);
E void movobj(struct Object *,xchar,xchar);
E bool may_dig(xchar,xchar);
E bool may_passwall(xchar,xchar);
E bool bad_rock(struct permonst *,xchar,xchar);
E bool invocation_pos(xchar,xchar);
E bool test_move(int, int, int, int, int);
E void domove();
E void invocation_message();
E void spoteffects(bool);
E char *in_rooms(xchar,xchar,int);
E bool in_town(int,int);
E void check_special_room(bool);
E int dopickup();
E void lookaround();
E int monster_nearby();
E void nomul(int, const char *);
E void unmul(const char *);
E void losehp(int,const char *,bool);
E int weight_cap();
E int inv_weight();
E int near_capacity();
E int calc_capacity(int);
E int max_capacity();
E bool check_capacity(const char *);
E int inv_cnt();
#ifdef GOLDOBJ
E long money_cnt(struct Object *);
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
E char *strkitten(char *,char);
E char *s_suffix(const char *);
E char *xcrypt(const char *,char *);
E bool onlyspace(const char *);
E char *tabexpand(char *);
E char *visctrl(char);
E const char *ordin(int);
E char *sitoa(int);
E int sgn(int);
E int rounddiv(long,int);
E int dist2(int,int,int,int);
E int distmin(int,int,int,int);
E bool online2(int,int,int,int);
E bool pmatch(const char *,const char *);
#ifndef STRNCMPI
E int strncmpi(const char *,const char *,int);
#endif
#ifndef STRSTRI
E char *strstri(const char *,const char *);
#endif
E bool fuzzymatch(const char *,const char *,const char *,bool);
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

E void assigninvlet(struct Object *);
E struct Object *merge_choice(struct Object *,struct Object *);
E int merged(struct Object **,struct Object **);
E void addinv_core1(struct Object *);
E void addinv_core2(struct Object *);
E struct Object *addinv(struct Object *);
E struct Object *hold_another_object(
    struct Object *,const char *,const char *,const char *);
E void useupall(struct Object *);
E void useup(struct Object *);
E void consume_obj_charge(struct Object *,bool);
E void freeinv_core(struct Object *);
E void freeinv(struct Object *);
E void delallobj(int,int);
E void delobj(struct Object *);
E struct Object *sobj_at(int,int,int);
E struct Object *carrying(int);
E bool have_lizard();
E struct Object *o_on(unsigned int,struct Object *);
E bool obj_here(struct Object *,int,int);
E bool wearing_armor();
E bool is_worn(struct Object *);
E struct Object *g_at(int,int);
E struct Object *mkgoldobj(long);
E struct Object *getobj(const char *,const char *);
E int ggetobj(const char *,int (*)(Object*),int,bool,unsigned *);
E void fully_identify_obj(struct Object *);
E int identify(struct Object *);
E void identify_pack(int);
E int askchain(struct Object **,const char *,int,int (*)(Object*),
			int (*)(Object*),int,const char *);
E void prinv(const char *,struct Object *,long);
E char *xprname(struct Object *,const char *,char,bool,long,long);
E int ddoinv();
E char display_inventory(const char *,bool);
#ifdef DUMP_LOG
E char dump_inventory(const char *,bool,bool);
#endif
E int display_binventory(int,int,bool);
E struct Object *display_cinventory(struct Object *);
E struct Object *display_minventory(struct Monster *,int,char *);
E int dotypeinv();
E const char *dfeature_at(int,int,char *);
E int look_here(int,bool);
E int dolook();
E bool will_feel_cockatrice(struct Object *,bool);
E void feel_cockatrice(struct Object *,bool);
E void stackobj(struct Object *);
E int doprgold();
E int doprwep();
E int doprarm();
E int doprring();
E int dopramulet();
E int doprtool();
E int doprinuse();
E void useupf(struct Object *,long);
E char *let_to_name(char,bool);
E void free_invbuf();
E void reassign();
E int doorganize();
E int count_unpaid(struct Object *);
E int count_buc(struct Object *,int);
E void carry_obj_effects(struct Object *);
E const char *currency(long);
E void silly_thing(const char *,struct Object *);

/* ### ioctl.c ### */

E void getwindowsz();
E void getioctls();
E void setioctls();
# ifdef SUSPEND
E int dosuspend();
# endif /* SUSPEND */

/* ### light.c ### */

E void new_light_source(xchar, xchar, int, int, genericptr_t);
E void del_light_source(int, genericptr_t);
E void do_light_sources(char **);
E struct Monster *find_mid(unsigned, unsigned);
E void save_light_sources(int, int, int);
E void restore_light_sources(int);
E void relink_light_sources(bool);
E void obj_move_light_source(struct Object *, struct Object *);
E bool any_light_source();
E void snuff_light_source(int, int);
E bool obj_sheds_light(struct Object *);
E bool obj_is_burning(struct Object *);
E void obj_split_light_source(struct Object *, struct Object *);
E void obj_merge_light_sources(struct Object *,struct Object *);
E int candle_light_range(struct Object *);
#ifdef WIZARD
E int wiz_light_sources();
#endif

/* ### lock.c ### */

E bool picking_lock(int *,int *);
E bool picking_at(int,int);
E void reset_pick();
E int pick_lock(struct Object *);
E int doforce();
E bool boxlock(struct Object *,struct Object *);
E bool doorlock(struct Object *,int,int);
E int doopen();
E int doclose();

/* ### mail.c ### */

#ifdef MAIL
E void getmailstatus();
E void ckmailstatus();
E void readmail(struct Object *);
#endif /* MAIL */

/* ### makemon.c ### */

E bool is_home_elemental(struct permonst *);
E struct Monster *clone_mon(struct Monster *,xchar,xchar);
E struct Monster *makemon(struct permonst *,int,int,int);
E bool create_critters(int,struct permonst *);
E struct permonst *rndmonst();
E void reset_rndmonst(int);
E struct permonst *mkclass(char,int);
E int adj_lev(struct permonst *);
E struct permonst *grow_up(struct Monster *,struct Monster *);
E int mongets(struct Monster *,int);
E int golemhp(int);
E bool peace_minded(struct permonst *);
E void set_malign(struct Monster *);
E void set_mimic_sym(struct Monster *);
E int mbirth_limit(int);
E void mimic_hit_msg(struct Monster *, short);
#ifdef GOLDOBJ
E void mkmonmoney(struct Monster *, long);
#endif
E void bagotricks(struct Object *);
E bool propagate(int, bool,bool);

/* ### mapglyph.c ### */

E void mapglyph(int, int *, int *, unsigned *, int, int);

/* ### mcastu.c ### */

E int castmu(struct Monster *,struct Attack *,bool,bool);
E int buzzmu(struct Monster *,struct Attack *);

/* ### mhitm.c ### */

E int fightm(struct Monster *);
E int mattackm(struct Monster *,struct Monster *);
E int noattacks(struct permonst *);
E int sleep_monst(struct Monster *,int,int);
E void slept_monst(struct Monster *);
E long attk_protection(int);

/* ### mhitu.c ### */

E const char *mpoisons_subj(struct Monster *,struct Attack *);
E void u_slow_down();
E struct Monster *cloneu();
E void expels(struct Monster *,struct permonst *,bool);
E struct Attack *getmattk(struct permonst *,int,int *,struct Attack *);
E int mattacku(struct Monster *);
E int magic_negation(struct Monster *);
E int gazemu(struct Monster *,struct Attack *);
E void mdamageu(struct Monster *,int);
E int could_seduce(struct Monster *,struct Monster *,struct Attack *);
#ifdef SEDUCE
E int doseduce(struct Monster *);
#endif

/* ### minion.c ### */

E void msummon(struct Monster *);
E void summon_minion(ALIGNTYP_P,bool);
E int demon_talk(struct Monster *);
E long bribe(struct Monster *);
E int dprince(ALIGNTYP_P);
E int dlord(ALIGNTYP_P);
E int llord();
E int ndemon(ALIGNTYP_P);
E int lminion();

/* ### mklev.c ### */

E void sort_rooms();
E void add_room(int,int,int,int,bool,schar,bool);
E void add_subroom(struct mkroom *,int,int,int,int,
			   bool,schar,bool);
E void makecorridors();
E void add_door(int,int,struct mkroom *);
E void mklev();
#ifdef SPECIALIZATION
E void topologize(struct mkroom *,bool);
#else
E void topologize(struct mkroom *);
#endif
E void place_branch(branch *,xchar,xchar);
E bool occupied(xchar,xchar);
E int okdoor(xchar,xchar);
E void dodoor(int,int,struct mkroom *);
E void mktrap(int,int,struct mkroom *,coord*);
E void mkstairs(xchar,xchar,char,struct mkroom *);
E void mkinvokearea();

/* ### mkmap.c ### */

void flood_fill_rm(int,int,int,bool,bool);
void remove_rooms(int,int,int,int);

/* ### mkmaze.c ### */

E void wallification(int,int,int,int);
E void walkfrom(int,int);
E void makemaz(const char *);
E void mazexy(coord *);
E void bound_digging();
E void mkportal(xchar,xchar,xchar,xchar);
E bool bad_location(xchar,xchar,xchar,xchar,xchar,xchar);
E void place_lregion(xchar,xchar,xchar,xchar,
			     xchar,xchar,xchar,xchar,
			     xchar,d_level *);
E void movebubbles();
E void water_friction();
E void save_waterlevel(int,int);
E void restore_waterlevel(int);
E const char *waterbody_name(xchar,xchar);

/* ### mkobj.c ### */

E struct Object *mkobj_at(char,int,int,bool);
E struct Object *mksobj_at(int,int,int,bool,bool);
E struct Object *mkobj(char,bool);
E int rndmonnum();
E struct Object *splitobj(struct Object *,long);
E void replace_object(struct Object *,struct Object *);
E void bill_dummy_object(struct Object *);
E struct Object *mksobj(int,bool,bool);
E int bcsign(struct Object *);
E int weight(struct Object *);
E struct Object *mkgold(long,int,int);
E struct Object *mkcorpstat(
    int,struct Monster *,struct permonst *,int,int,bool);
E struct Object *obj_attach_mid(struct Object *, unsigned);
E struct Monster *get_mtraits(struct Object *, bool);
E struct Object *mk_tt_object(int,int,int);
E struct Object *mk_named_object(
    int,struct permonst *,int,int,const char *);
E struct Object *rnd_treefruit_at(int, int);
E void start_corpse_timeout(struct Object *);
E void bless(struct Object *);
E void unbless(struct Object *);
E void curse(struct Object *);
E void uncurse(struct Object *);
E void blessorcurse(struct Object *,int);
E bool is_flammable(struct Object *);
E bool is_rottable(struct Object *);
E void place_object(struct Object *,int,int);
E void remove_object(struct Object *);
E void discard_minvent(struct Monster *);
E void obj_extract_self(struct Object *);
E void extract_nobj(struct Object *, struct Object **);
E void extract_nexthere(struct Object *, struct Object **);
E int add_to_minv(struct Monster *, struct Object *);
E struct Object *add_to_container(struct Object *, struct Object *);
E void add_to_migration(struct Object *);
E void add_to_buried(struct Object *);
E void dealloc_obj(struct Object *);
E void obj_ice_effects(int, int, bool);
E long peek_at_iced_corpse_age(struct Object *);
#ifdef WIZARD
E void obj_sanity_check();
#endif

/* ### mkroom.c ### */

E void mkroom(int);
E void fill_zoo(struct mkroom *);
E bool nexttodoor(int,int);
E bool has_dnstairs(struct mkroom *);
E bool has_upstairs(struct mkroom *);
E int somex(struct mkroom *);
E int somey(struct mkroom *);
E bool inside_room(struct mkroom *,xchar,xchar);
E bool somexy(struct mkroom *,coord *);
E void mkundead(coord *,bool,int);
E struct permonst *courtmon();
E void save_rooms(int);
E void rest_rooms(int);
E struct mkroom *search_special(schar);

/* ### mon.c ### */

E int undead_to_corpse(int);
E int genus(int,int);
E int pm_to_cham(int);
E int minliquid(struct Monster *);
E int movemon();
E int meatmetal(struct Monster *);
E int meatobj(struct Monster *);
E void mpickgold(struct Monster *);
E bool mpickstuff(struct Monster *,const char *);
E int curr_mon_load(struct Monster *);
E int max_mon_load(struct Monster *);
E bool can_carry(struct Monster *,struct Object *);
E int mfndpos(struct Monster *,coord *,long *,long);
E bool monnear(struct Monster *,int,int);
E void dmonsfree();
E int mcalcmove(struct Monster*);
E void mcalcdistress();
E void replmon(struct Monster *,struct Monster *);
E void relmon(struct Monster *);
E struct Object *mlifesaver(struct Monster *);
E bool corpse_chance(struct Monster *,struct Monster *,bool);
E void mondead(struct Monster *);
E void mondied(struct Monster *);
E void mongone(struct Monster *);
E void monstone(struct Monster *);
E void monkilled(struct Monster *,const char *,int);
E void unstuck(struct Monster *);
E void killed(struct Monster *);
E void xkilled(struct Monster *,int);
E void mon_to_stone(struct Monster*);
E void mnexto(struct Monster *);
E bool mnearto(struct Monster *,xchar,xchar,bool);
E void poisontell(int);
E void poisoned(const char *,int,const char *,int);
E void m_respond(struct Monster *);
E void setmangry(struct Monster *);
E void wakeup(struct Monster *);
E void wake_nearby();
E void wake_nearto(int,int,int);
E void seemimic(struct Monster *);
E void rescham();
E void restartcham();
E void restore_cham(struct Monster *);
E void mon_animal_list(bool);
E int newcham(struct Monster *,struct permonst *,bool,bool);
E int can_be_hatched(int);
E int egg_type_from_parent(int,bool);
E bool dead_species(int,bool);
E void kill_genocided_monsters();
E void golemeffects(struct Monster *,int,int);
E bool angry_guards(bool);
E void pacify_guards();

/* ### mondata.c ### */

E void set_mon_data(struct Monster *,struct permonst *,int);
E struct Attack *attacktype_fordmg(struct permonst *,int,int);
E bool attacktype(struct permonst *,int);
E bool poly_when_stoned(struct permonst *);
E bool resists_drli(struct Monster *);
E bool resists_magm(struct Monster *);
E bool resists_blnd(struct Monster *);
E bool can_blnd(struct Monster *,struct Monster *,uchar,struct Object *);
E bool ranged_attk(struct permonst *);
E bool hates_silver(struct permonst *);
E bool passes_bars(struct permonst *);
E bool can_track(struct permonst *);
E bool breakarm(struct permonst *);
E bool sliparm(struct permonst *);
E bool sticks(struct permonst *);
E int num_horns(struct permonst *);
/* E bool canseemon(struct monst *); */
E struct Attack *dmgtype_fromattack(struct permonst *,int,int);
E bool dmgtype(struct permonst *,int);
E int max_passive_dmg(struct Monster *,struct Monster *);
E int monsndx(struct permonst *);
E int name_to_mon(const char *);
E int gender(struct Monster *);
E int pronoun_gender(struct Monster *);
E bool levl_follower(struct Monster *);
E int little_to_big(int);
E int big_to_little(int);
E const char *locomotion(const struct permonst *,const char *);
E const char *stagger(const struct permonst *,const char *);
E const char *on_fire(struct permonst *,struct Attack *);
E const struct permonst *raceptr(struct Monster *);

/* ### monmove.c ### */

E bool itsstuck(struct Monster *);
E bool mb_trapped(struct Monster *);
E void mon_regen(struct Monster *,bool);
E int dochugw(struct Monster *);
E bool onscary(int,int,struct Monster *);
E void monflee(struct Monster *, int, bool, bool);
E int dochug(struct Monster *);
E int m_move(struct Monster *,int);
E bool closed_door(int,int);
E bool accessible(int,int);
E void set_apparxy(struct Monster *);
E bool can_ooze(struct Monster *);

/* ### monst.c ### */

E void monst_init();

/* ### monstr.c ### */

E void monstr_init();

/* ### mplayer.c ### */

E struct Monster *mk_mplayer(struct permonst *,xchar,
				   xchar,bool);
E void create_mplayers(int,bool);
E void mplayer_talk(struct Monster *);

/* ### mthrowu.c ### */

E int thitu(int,int,struct Object *,const char *);
E int ohitmon(struct Monster *,struct Object *,int,bool);
E void thrwmu(struct Monster *);
E int spitmu(struct Monster *,struct Attack *);
E int breamu(struct Monster *,struct Attack *);
E bool linedup(xchar,xchar,xchar,xchar);
E bool lined_up(struct Monster *);
E struct Object *m_carrying(struct Monster *,int);
E void m_useup(struct Monster *,struct Object *);
E void m_throw(struct Monster *,int,int,int,int,int,struct Object *);
E bool hits_bars(struct Object **,int,int,int,int);

/* ### muse.c ### */

E bool find_defensive(struct Monster *);
E int use_defensive(struct Monster *);
E int rnd_defensive_item(struct Monster *);
E bool find_offensive(struct Monster *);
E int use_offensive(struct Monster *);
E int rnd_offensive_item(struct Monster *);
E bool find_misc(struct Monster *);
E int use_misc(struct Monster *);
E int rnd_misc_item(struct Monster *);
E bool searches_for_item(struct Monster *,struct Object *);
E bool mon_reflects(struct Monster *,const char *);
E bool ureflects(const char *,const char *);
E bool munstone(struct Monster *,bool);

/* ### music.c ### */

E void awaken_soldiers();
E int do_play_instrument(struct Object *);

/* ### nhlan.c ### */
#ifdef LAN_FEATURES
E void init_lan_features();
E char *lan_username();
# ifdef LAN_MAIL
E bool lan_mail_check();
E void lan_mail_read(struct Object *);
E void lan_mail_init();
E void lan_mail_finish();
E void lan_mail_terminate();
# endif
#endif

/* ### o_init.c ### */

E void init_objects();
E int find_skates();
E void oinit();
E void savenames(int,int);
E void restnames(int);
E void discover_object(int,bool,bool);
E void undiscover_object(int);
E int dodiscovered();

/* ### objects.c ### */

E void objects_init();

/* ### objnam.c ### */

E char *obj_typename(int);
E char *simple_typename(int);
E bool obj_is_pname(struct Object *);
E char *distant_name(struct Object *,char *(*)(Object*));
E char *fruitname(bool);
E char *xname(struct Object *);
E char *mshot_xname(struct Object *);
E bool the_unique_obj(struct Object *obj);
E char *doname(struct Object *);
E bool not_fully_identified(struct Object *);
E char *corpse_xname(struct Object *,bool);
E char *cxname(struct Object *);
#ifdef SORTLOOT
E char *cxname2(struct Object *);
#endif
E char *killer_xname(struct Object *);
E const char *singular(struct Object *,char *(*)(Object*));
E char *an(const char *);
E char *An(const char *);
E char *The(const char *);
E char *the(const char *);
E char *aobjnam(struct Object *,const char *);
E char *Tobjnam(struct Object *,const char *);
E char *otense(struct Object *,const char *);
E char *vtense(const char *,const char *);
E char *Doname2(struct Object *);
E char *yname(struct Object *);
E char *Yname2(struct Object *);
E char *ysimple_name(struct Object *);
E char *Ysimple_name2(struct Object *);
E char *makeplural(const char *);
E char *makesingular(const char *);
E struct Object *readobjnam(char *,struct Object *,bool);
E int rnd_class(int,int);
E const char *cloak_simple_name(struct Object *);
E const char *mimic_obj_name(struct Monster *);

/* ### options.c ### */

E bool match_optname(const char *,const char *,int,bool);
E void initoptions();
E void parseoptions(char *,bool,bool);
E int doset();
E int dotogglepickup();
E void option_help();
E void next_opt(winid,const char *);
E int fruitadd(char *);
E int choose_classes_menu(const char *,int,bool,char *,char *);
E void add_menu_cmd_alias(char, char);
E char map_menu_cmd(char);
E void assign_warnings(uchar *);
E char *nh_getenv(const char *);
E void set_duplicate_opt_detection(int);
E void set_wc_option_mod_status(unsigned long, int);
E void set_wc2_option_mod_status(unsigned long, int);
E void set_option_mod_status(const char *,int);
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
E int collect_obj_classes(char *,struct Object *,bool,bool (*)(Object*), int *);
#else
E int collect_obj_classes(
    char *,struct Object *,bool,bool,bool (*)(Object*), int *);
#endif
E void add_valid_menu_class(int);
E bool allow_all(struct Object *);
E bool allow_category(struct Object *);
E bool is_worn_by_type(struct Object *);
E int pickup(int);
E int pickup_object(struct Object *, long, bool);
E int query_category(const char *, struct Object *, int,
				menu_item **, int);
E int query_objlist(const char *, struct Object *, int,
				menu_item **, int, bool (*)(Object*));
E struct Object *pick_obj(struct Object *);
E int encumber_msg();
E int doloot();
E int use_container(struct Object *,int);
E int loot_mon(struct Monster *,int *,bool *);
E const char *safe_qbuf(const char *,unsigned,
				const char *,const char *,const char *);
E bool is_autopickup_exception(struct Object *, bool);

/* ### pline.c ### */

E void msgpline_add(int, char *);
E void msgpline_free();
E void pline(const char *,...) PRINTF_F(1,2);
E void Norep(const char *,...) PRINTF_F(1,2);
E void free_youbuf();
E void You(const char *,...) PRINTF_F(1,2);
E void Your(const char *,...) PRINTF_F(1,2);
E void You_feel(const char *,...) PRINTF_F(1,2);
E void You_cant(const char *,...) PRINTF_F(1,2);
E void You_hear(const char *,...) PRINTF_F(1,2);
E void pline_The(const char *,...) PRINTF_F(1,2);
E void There(const char *,...) PRINTF_F(1,2);
E void verbalize(const char *,...) PRINTF_F(1,2);
E void raw_printf(const char *,...) PRINTF_F(1,2);
E void impossible(const char *,...) PRINTF_F(1,2);
E const char *align_str(ALIGNTYP_P);
E void mstatusline(struct Monster *);
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
E const char *mbodypart(struct Monster *,int);
E const char *body_part(int);
E int poly_gender();
E void ugolemeffects(int,int);

/* ### potion.c ### */

E void set_itimeout(long *,long);
E void incr_itimeout(long *,int);
E void make_confused(long,bool);
E void make_stunned(long,bool);
E void make_blinded(long,bool);
E void make_sick(long, const char *, bool,int);
E void make_vomiting(long,bool);
E bool make_hallucinated(long,bool,long);
E int dodrink();
E int dopotion(struct Object *);
E int peffects(struct Object *);
E void healup(int,int,bool,bool);
E void strange_feeling(struct Object *,const char *);
E void potionhit(struct Monster *,struct Object *,bool);
E void potionbreathe(struct Object *);
E bool get_wet(struct Object *);
E int dodip();
E void djinni_from_bottle(struct Object *);
E struct Monster *split_mon(struct Monster *,struct Monster *);
E const char *bottlename();

/* ### pray.c ### */

E int dosacrifice();
E bool can_pray(bool);
E int dopray();
E const char *u_gname();
E int doturn();
E const char *a_gname();
E const char *a_gname_at(xchar x,xchar y);
E const char *align_gname(ALIGNTYP_P);
E const char *halu_gname(ALIGNTYP_P);
E const char *align_gtitle(ALIGNTYP_P);
E void altar_wrath(int,int);


/* ### priest.c ### */

E int move_special(struct Monster *,bool,schar,bool,bool,
			   xchar,xchar,xchar,xchar);
E char temple_occupied(char *);
E int pri_move(struct Monster *);
E void priestini(d_level *,struct mkroom *,int,int,bool);
E char *priestname(struct Monster *,char *);
E bool p_coaligned(struct Monster *);
E struct Monster *findpriest(char);
E void intemple(int);
E void priest_talk(struct Monster *);
E struct Monster *mk_roamer(struct permonst *,ALIGNTYP_P,
				  xchar,xchar,bool);
E void reset_hostility(struct Monster *);
E bool in_your_sanctuary(struct Monster *,xchar,xchar);
E void ghod_hitsu(struct Monster *);
E void angry_priest();
E void clearpriests();
E void restpriest(struct Monster *,bool);

/* ### quest.c ### */

E void onquest();
E void nemdead();
E void artitouch();
E bool ok_to_quest();
E void leader_speaks(struct Monster *);
E void nemesis_speaks();
E void quest_chat(struct Monster *);
E void quest_talk(struct Monster *);
E void quest_stat_check(struct Monster *);
E void finish_quest(struct Object *);

/* ### questpgr.c ### */

E void load_qtlist();
E void unload_qtlist();
E short quest_info(int);
E const char *ldrname();
E bool is_quest_artifact(struct Object*);
E void com_pager(int);
E void qt_pager(int);
E struct permonst *qt_montype();

/* ### random.c ### */

#if defined(RANDOM) && !defined(__GO32__) /* djgpp has its own random */
E void srandom(unsigned);
E char *initstate(unsigned,char *,int);
E char *setstate(char *);
E long random();
#endif /* RANDOM */

/* ### read.c ### */

E int doread();
E bool is_chargeable(struct Object *);
E void recharge(struct Object *,int);
E void forget_objects(int);
E void forget_levels(int);
E void forget_traps();
E void forget_map(int);
E int seffects(struct Object *);
E void litroom(bool,struct Object *);
E void do_genocide(int);
E void punish(struct Object *);
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
E void split_rects(NhRect *,NhRect *);

/* ## region.c ### */
E void clear_regions();
E void run_regions();
E bool in_out_region(xchar,xchar);
E bool m_in_out_region(struct Monster *,xchar,xchar);
E void update_player_regions();
E void update_monster_region(struct Monster *);
E NhRegion *visible_region_at(xchar,xchar);
E void show_region(NhRegion*, xchar, xchar);
E void save_regions(int,int);
E void rest_regions(int,bool);
E NhRegion* create_gas_cloud(xchar, xchar, int, int);

/* ### restore.c ### */

E void inven_inuse(bool);
E int dorecover(int);
E void trickery(char *);
E void getlev(int,int,xchar,bool);
E void minit();
E bool lookup_id_mapping(unsigned long, unsigned long*);
#ifdef ZEROCOMP
E int mread(int,genericptr_t,unsigned int);
#else
E void mread(int,genericptr_t,unsigned int);
#endif

/* ### rip.c ### */

E void genl_outrip(winid,int);

/* ### rnd.c ### */

E void check_reseed();
E int rn2(int);
E int rnl(int);
E int rnd(int);
E int d(int,int);
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
E const char *Hello(struct Monster *);
E const char *Goodbye();
E char *build_plselection_prompt(char *, int, int, int, int, int);
E char *root_plselection_prompt(char *, int, int, int, int, int);

/* ### rumors.c ### */

E char *getrumor(int,char *, bool);
E void outrumor(int,int);
E void outoracle(bool, bool);
E void save_oracles(int,int);
E void restore_oracles(int);
E int doconsult(struct Monster *);

/* ### save.c ### */

E int dosave();
E void hangup(int);
E int dosave0();
#ifdef INSURANCE
E void savestateinlock();
#endif
E void savelev(int,xchar,int);
E void bufon(int);
E void bufoff(int);
E void bflush(int);
E void bwrite(int,genericptr_t,unsigned int);
E void bclose(int);
E void savefruitchn(int,int);
E void free_dungeons();
E void freedynamicdata();

/* ### shk.c ### */

#ifdef GOLDOBJ
E long money2mon(struct Monster *, long);
E void money2u(struct Monster *, long);
#endif
E char *shkname(struct Monster *);
E void shkgone(struct Monster *);
E void set_residency(struct Monster *,bool);
E void replshk(struct Monster *,struct Monster *);
E void restshk(struct Monster *,bool);
E char inside_shop(xchar,xchar);
E void u_left_shop(char *,bool);
E void remote_burglary(xchar,xchar);
E void u_entered_shop(char *);
E bool same_price(struct Object *,struct Object *);
E void shopper_financial_report();
E int inhishop(struct Monster *);
E struct Monster *shop_keeper(char);
E bool tended_shop(struct mkroom *);
E void delete_contents(struct Object *);
E void obfree(struct Object *,struct Object *);
E void home_shk(struct Monster *,bool);
E void make_happy_shk(struct Monster *,bool);
E void hot_pursuit(struct Monster *);
E void make_angry_shk(struct Monster *,xchar,xchar);
E int dopay();
E bool paybill(int);
E void finish_paybill();
E struct Object *find_oid(unsigned);
E long contained_cost(struct Object *,struct Monster *,long,bool, bool);
E long contained_gold(struct Object *);
E void picked_container(struct Object *);
E long unpaid_cost(struct Object *);
E void addtobill(struct Object *,bool,bool,bool);
E void splitbill(struct Object *,struct Object *);
E void subfrombill(struct Object *,struct Monster *);
E long stolen_value(struct Object *,xchar,xchar,bool,bool);
E void sellobj_state(int);
E void sellobj(struct Object *,xchar,xchar);
E int doinvbill(int);
E struct Monster *shkcatch(struct Object *,xchar,xchar);
E void add_damage(xchar,xchar,long);
E int repair_damage(struct Monster *,struct damage *,bool);
E int shk_move(struct Monster *);
E void after_shk_move(struct Monster *);
E bool is_fshk(struct Monster *);
E void shopdig(int);
E void pay_for_damage(const char *,bool);
E bool costly_spot(xchar,xchar);
E struct Object *shop_object(xchar,xchar);
E void price_quote(struct Object *);
E void shk_chat(struct Monster *);
E void check_unpaid_usage(struct Object *,bool);
E void check_unpaid(struct Object *);
E void costly_gold(xchar,xchar,long);
E bool block_door(xchar,xchar);
E bool block_entry(xchar,xchar);
E char *shk_your(char *,struct Object *);
E char *Shk_Your(char *,struct Object *);

/* ### shknam.c ### */

E void stock_room(int,struct mkroom *);
E bool saleable(struct Monster *,struct Object *);
E int get_shop_item(int);

/* ### sit.c ### */

E void take_gold();
E int dosit();
E void rndcurse();
E void attrcurse();

/* ### sounds.c ### */

E void dosounds();
E const char *growl_sound(struct Monster *);
E void growl(struct Monster *);
E void yelp(struct Monster *);
E void whimper(struct Monster *);
E void beg(struct Monster *);
E int dotalk();
#ifdef USER_SOUNDS
E int add_sound_mapping(const char *);
E void play_sound_for_message(const char *);
#endif

/* ### sp_lev.c ### */

E bool check_room(xchar *,xchar *,xchar *,xchar *,bool);
E bool create_room(xchar,xchar,xchar,xchar,
			      xchar,xchar,xchar,xchar);
E void create_secret_door(struct mkroom *,xchar);
E bool dig_corridor(coord *,coord *,bool,schar,schar);
E void fill_room(struct mkroom *,bool);
E bool load_special(const char *);

/* ### spell.c ### */

E int study_book(struct Object *);
E void book_disappears(struct Object *);
E void book_substitution(struct Object *,struct Object *);
E void age_spells();
E int docast();
E int spell_skilltype(int);
E int spelleffects(int,bool);
E void losespells();
E int dovspell();
E void initialspell(struct Object *);

/* ### steal.c ### */

#ifdef GOLDOBJ
E long somegold(long);
#else
E long somegold();
#endif
E void stealgold(struct Monster *);
E void remove_worn_item(struct Object *,bool);
E int steal(struct Monster *, char *);
E int mpickobj(struct Monster *,struct Object *);
E void stealamulet(struct Monster *);
E void mdrop_special_objs(struct Monster *);
E void relobj(struct Monster *,int,bool);
#ifdef GOLDOBJ
E struct Object *findgold(struct Object *);
#endif

/* ### steed.c ### */

#ifdef STEED
E void rider_cant_reach();
E bool can_saddle(struct Monster *);
E int use_saddle(struct Object *);
E bool can_ride(struct Monster *);
E int doride();
E bool mount_steed(struct Monster *, bool);
E void exercise_steed();
E void kick_steed();
E void dismount_steed(int);
E void place_monster(struct Monster *,int,int);
#endif

/* ### teleport.c ### */

E bool goodpos(int,int,struct Monster *,unsigned);
E bool enexto(coord *,xchar,xchar,struct permonst *);
E bool enexto_core(coord *,xchar,xchar,struct permonst *,unsigned);
E void teleds(int,int,bool);
E bool safe_teleds(bool);
E bool teleport_pet(struct Monster *,bool);
E void tele();
E int dotele();
E void level_tele();
E void domagicportal(struct trap *);
E void tele_trap(struct trap *);
E void level_tele_trap(struct trap *);
E void rloc_to(struct Monster *,int,int);
E bool rloc(struct Monster *, bool);
E bool tele_restrict(struct Monster *);
E void mtele_trap(struct Monster *, struct trap *,int);
E int mlevel_tele_trap(struct Monster *, struct trap *,bool,int);
E void rloco(struct Object *);
E int random_teleport_level();
E bool u_teleport_mon(struct Monster *,bool);

/* ### tile.c ### */
#ifdef USE_TILES
E void substitute_tiles(d_level *);
#endif

/* ### timeout.c ### */

E void burn_away_slime();
E void nh_timeout();
E void fall_asleep(int, bool);
E void attach_egg_hatch_timeout(struct Object *);
E void attach_fig_transform_timeout(struct Object *);
E void kill_egg(struct Object *);
E void hatch_egg(genericptr_t, long);
E void learn_egg_type(int);
E void burn_object(genericptr_t, long);
E void begin_burn(struct Object *, bool);
E void end_burn(struct Object *, bool);
E void do_storms();
E bool start_timer(long, short, short, genericptr_t);
E long stop_timer(short, genericptr_t);
E void run_timers();
E void obj_move_timers(struct Object *, struct Object *);
E void obj_split_timers(struct Object *, struct Object *);
E void obj_stop_timers(struct Object *);
E bool obj_is_local(struct Object *);
E void save_timers(int,int,int);
E void restore_timers(int,int,bool,long);
E void relink_timers(bool);
#ifdef WIZARD
E int wiz_timeout_queue();
E void timer_sanity_check();
#endif

/* ### topten.c ### */

E void topten(int);
E void prscore(int,char **);
E struct Object *tt_oname(struct Object *);

/* ### track.c ### */

E void initrack();
E void settrack();
E coord *gettrack(int,int);

/* ### trap.c ### */

E bool burnarmor(struct Monster *);
E bool rust_dmg(struct Object *,const char *,int,bool,struct Monster *);
E void grease_protect(struct Object *,const char *,struct Monster *);
E struct trap *maketrap(int,int,int);
E void fall_through(bool);
E struct Monster *animate_statue(struct Object *,xchar,xchar,int,int *);
E struct Monster *activate_statue_trap(struct trap *,xchar,xchar,bool);
E void dotrap(struct trap *, unsigned);
E void seetrap(struct trap *);
E int mintrap(struct Monster *);
E void instapetrify(const char *);
E void minstapetrify(struct Monster *,bool);
E void selftouch(const char *);
E void mselftouch(struct Monster *,const char *,bool);
E void float_up();
E void fill_pit(int,int);
E int float_down(long, long);
E int fire_damage(struct Object *,bool,bool,xchar,xchar);
E bool water_damage(struct Object *,bool,bool);
E bool drown();
E void drain_en(int);
E int dountrap();
E int untrap(bool);
E bool chest_trap(struct Object *,int,bool);
E void deltrap(struct trap *);
E bool delfloortrap(struct trap *);
E struct trap *t_at(int,int);
E void b_trapped(const char *,int);
E bool unconscious();
E bool lava_effects();
E void blow_up_landmine(struct trap *);
E int launch_obj(short,int,int,int,int,int);

/* ### u_init.c ### */

E void u_init();

/* ### uhitm.c ### */

E void hurtmarmor(struct Monster *,int);
E bool attack_checks(struct Monster *,struct Object *);
E void check_caitiff(struct Monster *);
E schar find_roll_to_hit(struct Monster *);
E bool attack(struct Monster *);
E bool hmon(struct Monster *,struct Object *,int);
E int damageum(struct Monster *,struct Attack *);
E void missum(struct Monster *,struct Attack *);
E int passive(struct Monster *,bool,int,uchar);
E void passive_obj(struct Monster *,struct Object *,struct Attack *);
E void stumble_onto_mimic(struct Monster *);
E int flash_hits_mon(struct Monster *,struct Object *);

/* ### unixmain.c ### */

# ifdef PORT_HELP
E void port_help();
# endif


/* ### unixtty.c ### */

E void gettty();
E void settty(const char *);
E void setftty();
E void intron();
E void introff();
E void ErrMsg(const char *,...) PRINTF_F(1,2);
E void VErrMsg(const char *, va_list);
E void error(const char *,...) PRINTF_F(1,2);

/* ### unixunix.c ### */

E void getlock();
E void regularize(char *);
# if defined(TIMED_DELAY) && !defined(msleep) && defined(SYSV)
E void msleep(unsigned);
# endif
# ifdef SHELL
E int dosh();
# endif /* SHELL */
# if defined(SHELL) || defined(DEF_PAGER) || defined(DEF_MAILREADER)
E int child(int);
# endif

/* ### vault.c ### */

E bool grddead(struct Monster *);
E char vault_occupied(char *);
E void invault();
E int gd_move(struct Monster *);
E void paygd();
E long hidden_gold();
E bool gd_sound();

/* ### version.c ### */

E char *version_string(char *);
E char *getversionstring(char *);
E int doversion();
E int doextversion();
E bool check_version(struct version_info *, const char *,bool);
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
E int does_block(int,int,struct rm*);
E void vision_reset();
E void vision_recalc(int);
E void block_point(int,int);
E void unblock_point(int,int);
E bool clear_path(int,int,int,int);
E void do_clear_area(int,int,int, void (*)(int,int,genericptr_t),genericptr_t);

/* ### weapon.c ### */

E int hitval(struct Object *,struct Monster *);
E int dmgval(struct Object *,struct Monster *);
E struct Object *select_rwep(struct Monster *);
E struct Object *select_hwep(struct Monster *);
E void possibly_unwield(struct Monster *,bool);
E int mon_wield_item(struct Monster *);
E int abon();
E int dbon();
E int enhance_weapon_skill();
#ifdef DUMP_LOG
E void dump_weapon_skill();
#endif
E void unrestrict_weapon_skill(int);
E void use_skill(int,int);
E void add_weapon_skill(int);
E void lose_weapon_skill(int);
E int weapon_type(struct Object *);
E int uwep_skill_type();
E int weapon_hit_bonus(struct Object *);
E int weapon_dam_bonus(struct Object *);
E void skill_init(const struct def_skill *);

/* ### were.c ### */

E void were_change(struct Monster *);
E void new_were(struct Monster *);
E int were_summon(struct permonst *,bool,int *,char *);
E void you_were();
E void you_unwere(bool);

/* ### wield.c ### */

E void setuwep(struct Object *);
E void setuqwep(struct Object *);
E void setuswapwep(struct Object *);
E int dowield();
E int doswapweapon();
E int dowieldquiver();
E bool wield_tool(struct Object *,const char *);
E int can_twoweapon();
E void drop_uswapwep();
E int dotwoweapon();
E void uwepgone();
E void uswapwepgone();
E void uqwepgone();
E void untwoweapon();
E void erode_obj(struct Object *,bool,bool);
E int chwepon(struct Object *,int);
E int welded(struct Object *);
E void weldmsg(struct Object *);
E void setmnotwielded(struct Monster *,struct Object *);

/* ### windows.c ### */

E void choose_windows(const char *);
E char genl_message_menu(char,int,const char *);
E void genl_preference_update(const char *);

/* ### wizard.c ### */

E void amulet();
E int mon_has_amulet(struct Monster *);
E int mon_has_special(struct Monster *);
E int tactics(struct Monster *);
E void aggravate();
E void clonewiz();
E int pick_nasty();
E int nasty(struct Monster*);
E void resurrect();
E void intervene();
E void wizdead();
E void cuss(struct Monster *);

/* ### worm.c ### */

E int get_wormno();
E void initworm(struct Monster *,int);
E void worm_move(struct Monster *);
E void worm_nomove(struct Monster *);
E void wormgone(struct Monster *);
E void wormhitu(struct Monster *);
E void cutworm(struct Monster *,xchar,xchar,struct Object *);
E void see_wsegs(struct Monster *);
E void detect_wsegs(struct Monster *,bool);
E void save_worm(int,int);
E void rest_worm(int);
E void place_wsegs(struct Monster *);
E void remove_worm(struct Monster *);
E void place_worm_tail_randomly(struct Monster *,xchar,xchar);
E int count_wsegs(struct Monster *);
E bool worm_known(struct Monster *);

/* ### worn.c ### */

E void setworn(struct Object *,long);
E void setnotworn(struct Object *);
E void mon_set_minvis(struct Monster *);
E void mon_adjust_speed(struct Monster *,int,struct Object *);
E void update_mon_intrinsics(struct Monster *,struct Object *,bool,bool);
E int find_mac(struct Monster *);
E void m_dowear(struct Monster *,bool);
E struct Object *which_armor(struct Monster *,long);
E void mon_break_armor(struct Monster *,bool);
E void bypass_obj(struct Object *);
E void clear_bypasses();
E int racial_exception(struct Monster *, struct Object *);

/* ### write.c ### */

E int dowrite(struct Object *);

/* ### zap.c ### */

E int bhitm(struct Monster *,struct Object *);
E void probe_monster(struct Monster *);
E bool get_obj_location(struct Object *,xchar *,xchar *,int);
E bool get_mon_location(struct Monster *,xchar *,xchar *,int);
E struct Monster *get_container_location(struct Object *obj, int *, int *);
E struct Monster *montraits(struct Object *,coord *);
E struct Monster *revive(struct Object *);
E int unturn_dead(struct Monster *);
E void cancel_item(struct Object *);
E bool drain_item(struct Object *);
E struct Object *poly_obj(struct Object *, int);
E bool obj_resists(struct Object *,int,int);
E bool obj_shudders(struct Object *);
E void do_osshock(struct Object *);
E int bhito(struct Object *,struct Object *);
E int bhitpile(struct Object *,int (*)(Object*,Object*),int,int);
E int zappable(struct Object *);
E void zapnodir(struct Object *);
E int dozap();
E int zapyourself(struct Object *,bool);
E bool cancel_monst(struct Monster *,struct Object *, bool,bool,bool);
E void weffects(struct Object *);
E int spell_damage_bonus();
E const char *exclam(int force);
E void hit(const char *,struct Monster *,const char *);
E void miss(const char *,struct Monster *);
E struct Monster *bhit(int,int,int,int,int (*)(Monster*,Object*),
			     int (*)(Object*,Object*),struct Object *, bool *);
E struct Monster *boomhit(int,int);
E int burn_floor_paper(int,int,bool,bool);
E void buzz(int,int,xchar,xchar,int,int);
E void melt_ice(xchar,xchar);
E int zap_over_floor(xchar,xchar,int,bool *);
E void fracture_rock(struct Object *);
E bool break_statue(struct Object *);
E void destroy_item(int,int);
E int destroy_mitem(struct Monster *,int,int);
E int resist(struct Monster *,char,int,int);
E void makewish();

#endif /* !MAKEDEFS_C && !LEV_LEX_C */

#undef E

#endif /* EXTERN_H */
