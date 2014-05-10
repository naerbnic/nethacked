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

int fightm(Monster *);
int mattackm(Monster *, Monster *);
int noattacks(MonsterType *);
int sleep_monst(Monster *, int, int);
void slept_monst(Monster *);
long attk_protection(int);

/* ### mhitu.c ### */

const char *mpoisons_subj(Monster *, struct Attack *);
void u_slow_down();
Monster *cloneu();
void expels(Monster *, MonsterType *, bool);
struct Attack *getmattk(MonsterType *, int, int *, struct Attack *);
int mattacku(Monster *);
int magic_negation(Monster *);
int gazemu(Monster *, struct Attack *);
void mdamageu(Monster *, int);
int could_seduce(Monster *, Monster *, struct Attack *);
#ifdef SEDUCE
int doseduce(Monster *);
#endif

/* ### minion.c ### */

void msummon(Monster *);
void summon_minion(ALIGNTYP_P, bool);
int demon_talk(Monster *);
long bribe(Monster *);
int dprince(ALIGNTYP_P);
int dlord(ALIGNTYP_P);
int llord();
int ndemon(ALIGNTYP_P);
int lminion();

/* ### mklev.c ### */

void sort_rooms();
void add_room(int, int, int, int, bool, schar, bool);
void add_subroom(struct mkroom *, int, int, int, int, bool, schar, bool);
void makecorridors();
void add_door(int, int, struct mkroom *);
void mklev();
#ifdef SPECIALIZATION
void topologize(struct mkroom *, bool);
#else
void topologize(struct mkroom *);
#endif
void place_branch(branch *, xchar, xchar);
bool occupied(xchar, xchar);
int okdoor(xchar, xchar);
void dodoor(int, int, struct mkroom *);
void mktrap(int, int, struct mkroom *, coord *);
void mkstairs(xchar, xchar, char, struct mkroom *);
void mkinvokearea();

/* ### mkmap.c ### */

void flood_fill_rm(int, int, int, bool, bool);
void remove_rooms(int, int, int, int);

/* ### mkmaze.c ### */

void wallification(int, int, int, int);
void walkfrom(int, int);
void makemaz(const char *);
void mazexy(coord *);
void bound_digging();
void mkportal(xchar, xchar, xchar, xchar);
bool bad_location(xchar, xchar, xchar, xchar, xchar, xchar);
void place_lregion(xchar, xchar, xchar, xchar, xchar, xchar, xchar, xchar,
                     xchar, d_level *);
void movebubbles();
void water_friction();
void save_waterlevel(int, int);
void restore_waterlevel(int);
const char *waterbody_name(xchar, xchar);

/* ### mkobj.c ### */

Object *MakeRandomObjectAt(char, int, int, bool);
Object *MakeSpecificObjectAt(int, int, int, bool, bool);
Object *MakeRandomObject(char, bool);
int PickRandomMonsterTypeIndex();
Object *SplitObject(Object *, long);
void ReplaceObject(Object *, Object *);
void CreateBillDummyObject(Object *);
Object *MakeSpecificObject(int, bool, bool);
int GetBUCSign(Object *);
int GetWeight(Object *);
Object *MakeGold(long, int, int);
Object *MakeCorpseOrStatue(int, Monster *, MonsterType *, int, int, bool);
Object *AttachMonsterIdToObject(Object *, unsigned);
Monster *NewMonsterFromObject(Object *, bool);
Object *NewTopTenObject(int, int, int);
Object *MakeNamedCorpseOrStatue(int, MonsterType *, int, int, const char *);
Object *MakeRandomTreefruitAt(int, int);
void StartCorpseTimeout(Object *);
void Bless(Object *);
void Unbless(Object *);
void Curse(Object *);
void Uncurse(Object *);
void BlessOrCurse(Object *, int);
bool IsFlammable(Object *);
bool IsRottable(Object *);
void PlaceObject(Object *, int, int);
void RemoveObjectFromFloor(Object *);
void DiscardMonsterInventory(Monster *);
void RemoveObjectFromStorage(Object *);
void ExtractObjectFromList(Object *, Object **);
void extract_nexthere(Object *, Object **);
int AddObjectToMonsterInventory(Monster *, Object *);
Object *AddObjectToContainer(Object *, Object *);
void AddObjectToMigrationList(Object *);
void AddToBuriedList(Object *);
void DeallocateObject(Object *);
void ApplyIceEffectsAt(int, int, bool);
long PeekAtIcedCorpseAge(Object *);
#ifdef WIZARD
void SanityCheckObjects();
#endif

/* ### mkroom.c ### */

void mkroom(int);
void fill_zoo(struct mkroom *);
bool nexttodoor(int, int);
bool has_dnstairs(struct mkroom *);
bool has_upstairs(struct mkroom *);
int somex(struct mkroom *);
int somey(struct mkroom *);
bool inside_room(struct mkroom *, xchar, xchar);
bool somexy(struct mkroom *, coord *);
void mkundead(coord *, bool, int);
MonsterType *courtmon();
void save_rooms(int);
void rest_rooms(int);
struct mkroom *search_special(schar);

/* ### mon.c ### */

int undead_to_corpse(int);
int genus(int, int);
int pm_to_cham(int);
int minliquid(Monster *);
int movemon();
int meatmetal(Monster *);
int meatobj(Monster *);
void mpickgold(Monster *);
bool mpickstuff(Monster *, const char *);
int curr_mon_load(Monster *);
int max_mon_load(Monster *);
bool can_carry(Monster *, Object *);
int mfndpos(Monster *, coord *, long *, long);
bool monnear(Monster *, int, int);
void dmonsfree();
int mcalcmove(Monster *);
void mcalcdistress();
void replmon(Monster *, Monster *);
void relmon(Monster *);
Object *mlifesaver(Monster *);
bool corpse_chance(Monster *, Monster *, bool);
void mondead(Monster *);
void mondied(Monster *);
void mongone(Monster *);
void monstone(Monster *);
void monkilled(Monster *, const char *, int);
void unstuck(Monster *);
void killed(Monster *);
void xkilled(Monster *, int);
void mon_to_stone(Monster *);
void mnexto(Monster *);
bool mnearto(Monster *, xchar, xchar, bool);
void poisontell(int);
void poisoned(const char *, int, const char *, int);
void m_respond(Monster *);
void setmangry(Monster *);
void wakeup(Monster *);
void wake_nearby();
void wake_nearto(int, int, int);
void seemimic(Monster *);
void rescham();
void restartcham();
void restore_cham(Monster *);
void mon_animal_list(bool);
int newcham(Monster *, MonsterType *, bool, bool);
int can_be_hatched(int);
int egg_type_from_parent(int, bool);
bool dead_species(int, bool);
void kill_genocided_monsters();
void golemeffects(Monster *, int, int);
bool angry_guards(bool);
void pacify_guards();

/* ### mondata.c ### */

void set_mon_data(Monster *, MonsterType *, int);
struct Attack *attacktype_fordmg(MonsterType *, int, int);
bool attacktype(MonsterType *, int);
bool poly_when_stoned(MonsterType *);
bool resists_drli(Monster *);
bool resists_magm(Monster *);
bool resists_blnd(Monster *);
bool can_blnd(Monster *, Monster *, uchar, Object *);
bool ranged_attk(MonsterType *);
bool hates_silver(MonsterType *);
bool passes_bars(MonsterType *);
bool can_track(MonsterType *);
bool breakarm(MonsterType *);
bool sliparm(MonsterType *);
bool sticks(MonsterType *);
int num_horns(MonsterType *);
/* E bool canseemon(struct monst *); */
struct Attack *dmgtype_fromattack(MonsterType *, int, int);
bool dmgtype(MonsterType *, int);
int max_passive_dmg(Monster *, Monster *);
int monsndx(MonsterType *);
int name_to_mon(const char *);
int gender(Monster *);
int pronoun_gender(Monster *);
bool levl_follower(Monster *);
int little_to_big(int);
int big_to_little(int);
const char *locomotion(const MonsterType *, const char *);
const char *stagger(const MonsterType *, const char *);
const char *on_fire(MonsterType *, struct Attack *);
const MonsterType *raceptr(Monster *);

/* ### monmove.c ### */

bool itsstuck(Monster *);
bool mb_trapped(Monster *);
void mon_regen(Monster *, bool);
int dochugw(Monster *);
bool onscary(int, int, Monster *);
void monflee(Monster *, int, bool, bool);
int dochug(Monster *);
int m_move(Monster *, int);
bool closed_door(int, int);
bool accessible(int, int);
void set_apparxy(Monster *);
bool can_ooze(Monster *);

/* ### monst.c ### */

void monst_init();

/* ### monstr.c ### */

void monstr_init();

/* ### mplayer.c ### */

Monster *mk_mplayer(MonsterType *, xchar, xchar, bool);
void create_mplayers(int, bool);
void mplayer_talk(Monster *);

/* ### mthrowu.c ### */

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

/* ### muse.c ### */

bool find_defensive(Monster *);
int use_defensive(Monster *);
int rnd_defensive_item(Monster *);
bool find_offensive(Monster *);
int use_offensive(Monster *);
int rnd_offensive_item(Monster *);
bool find_misc(Monster *);
int use_misc(Monster *);
int rnd_misc_item(Monster *);
bool searches_for_item(Monster *, Object *);
bool mon_reflects(Monster *, const char *);
bool ureflects(const char *, const char *);
bool munstone(Monster *, bool);

/* ### music.c ### */

void awaken_soldiers();
int do_play_instrument(Object *);

/* ### nhlan.c ### */
#ifdef LAN_FEATURES
void init_lan_features();
char *lan_username();
#ifdef LAN_MAIL
bool lan_mail_check();
void lan_mail_read(Object *);
void lan_mail_init();
void lan_mail_finish();
void lan_mail_terminate();
#endif
#endif

/* ### o_init.c ### */

void init_objects();
int find_skates();
void oinit();
void savenames(int, int);
void restnames(int);
void discover_object(int, bool, bool);
void undiscover_object(int);
int dodiscovered();

/* ### objects.c ### */

void objects_init();

/* ### objnam.c ### */

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

/* ### options.c ### */

bool match_optname(const char *, const char *, int, bool);
void initoptions();
void parseoptions(char *, bool, bool);
int doset();
int dotogglepickup();
void option_help();
void next_opt(winid, const char *);
int fruitadd(char *);
int choose_classes_menu(const char *, int, bool, char *, char *);
void add_menu_cmd_alias(char, char);
char map_menu_cmd(char);
void assign_warnings(uchar *);
char *nh_getenv(const char *);
void set_duplicate_opt_detection(int);
void set_wc_option_mod_status(unsigned long, int);
void set_wc2_option_mod_status(unsigned long, int);
void set_option_mod_status(const char *, int);
#ifdef AUTOPICKUP_EXCEPTIONS
int add_autopickup_exception(const char *);
void free_autopickup_exceptions();
#endif /* AUTOPICKUP_EXCEPTIONS */
#ifdef MENU_COLOR
bool add_menu_coloring(char *);
#endif /* MENU_COLOR */

/* ### pager.c ### */

int dowhatis();
int doquickwhatis();
int doidtrap();
int dowhatdoes();
char *dowhatdoes_core(char, char *);
int dohelp();
int dohistory();

/* ### pickup.c ### */

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

/* ### pline.c ### */

void msgpline_add(int, char *);
void msgpline_free();
void pline(const char *, ...) PRINTF_F(1, 2);
void Norep(const char *, ...) PRINTF_F(1, 2);
void free_youbuf();
void You(const char *, ...) PRINTF_F(1, 2);
void Your(const char *, ...) PRINTF_F(1, 2);
void You_feel(const char *, ...) PRINTF_F(1, 2);
void You_cant(const char *, ...) PRINTF_F(1, 2);
void You_hear(const char *, ...) PRINTF_F(1, 2);
void pline_The(const char *, ...) PRINTF_F(1, 2);
void There(const char *, ...) PRINTF_F(1, 2);
void verbalize(const char *, ...) PRINTF_F(1, 2);
void raw_printf(const char *, ...) PRINTF_F(1, 2);
void impossible(const char *, ...) PRINTF_F(1, 2);
const char *align_str(ALIGNTYP_P);
void mstatusline(Monster *);
void ustatusline();
void self_invis_message();

/* ### polyself.c ### */

void set_uasmon();
void change_sex();
void polyself(bool);
int polymon(int);
void rehumanize();
int dobreathe();
int dospit();
int doremove();
int dospinweb();
int dosummon();
int dogaze();
int dohide();
int domindblast();
void skinback(bool);
const char *mbodypart(Monster *, int);
const char *body_part(int);
int poly_gender();
void ugolemeffects(int, int);

/* ### potion.c ### */

void set_itimeout(long *, long);
void incr_itimeout(long *, int);
void make_confused(long, bool);
void make_stunned(long, bool);
void make_blinded(long, bool);
void make_sick(long, const char *, bool, int);
void make_vomiting(long, bool);
bool make_hallucinated(long, bool, long);
int dodrink();
int dopotion(Object *);
int peffects(Object *);
void healup(int, int, bool, bool);
void strange_feeling(Object *, const char *);
void potionhit(Monster *, Object *, bool);
void potionbreathe(Object *);
bool get_wet(Object *);
int dodip();
void djinni_from_bottle(Object *);
Monster *split_mon(Monster *, Monster *);
const char *bottlename();

/* ### pray.c ### */

int dosacrifice();
bool can_pray(bool);
int dopray();
const char *u_gname();
int doturn();
const char *a_gname();
const char *a_gname_at(xchar x, xchar y);
const char *align_gname(ALIGNTYP_P);
const char *halu_gname(ALIGNTYP_P);
const char *align_gtitle(ALIGNTYP_P);
void altar_wrath(int, int);

/* ### priest.c ### */

int move_special(Monster *, bool, schar, bool, bool, xchar, xchar, xchar,
                   xchar);
char temple_occupied(char *);
int pri_move(Monster *);
void priestini(d_level *, struct mkroom *, int, int, bool);
char *priestname(Monster *, char *);
bool p_coaligned(Monster *);
Monster *findpriest(char);
void intemple(int);
void priest_talk(Monster *);
Monster *mk_roamer(MonsterType *, ALIGNTYP_P, xchar, xchar, bool);
void reset_hostility(Monster *);
bool in_your_sanctuary(Monster *, xchar, xchar);
void ghod_hitsu(Monster *);
void angry_priest();
void clearpriests();
void restpriest(Monster *, bool);

/* ### quest.c ### */

void onquest();
void nemdead();
void artitouch();
bool ok_to_quest();
void leader_speaks(Monster *);
void nemesis_speaks();
void quest_chat(Monster *);
void quest_talk(Monster *);
void quest_stat_check(Monster *);
void finish_quest(Object *);

/* ### questpgr.c ### */

void load_qtlist();
void unload_qtlist();
short quest_info(int);
const char *ldrname();
bool is_quest_artifact(Object *);
void com_pager(int);
void qt_pager(int);
MonsterType *qt_montype();

/* ### random.c ### */

#if defined(RANDOM) && !defined(__GO32__) /* djgpp has its own random */
void srandom(unsigned);
char *initstate(unsigned, char *, int);
char *setstate(char *);
long random();
#endif /* RANDOM */

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
