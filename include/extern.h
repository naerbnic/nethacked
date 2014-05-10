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

/* ### rect.c ### */

/* ## region.c ### */

/* ### restore.c ### */

/* ### rip.c ### */

/* ### rnd.c ### */

/* ### role.c ### */

/* ### rumors.c ### */

/* ### save.c ### */

/* ### shk.c ### */

/* ### shknam.c ### */

/* ### sit.c ### */

/* ### sounds.c ### */

/* ### sp_lev.c ### */

/* ### spell.c ### */

/* ### steal.c ### */

/* ### steed.c ### */

/* ### teleport.c ### */

/* ### tile.c ### */

/* ### timeout.c ### */

/* ### topten.c ### */

/* ### track.c ### */

/* ### trap.c ### */

/* ### u_init.c ### */

/* ### uhitm.c ### */

/* ### unixmain.c ### */

/* ### unixtty.c ### */

/* ### unixunix.c ### */

/* ### vault.c ### */

/* ### version.c ### */

/* ### video.c ### */

/* ### vis_tab.c ### */

/* ### vision.c ### */

/* ### weapon.c ### */

/* ### were.c ### */

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
