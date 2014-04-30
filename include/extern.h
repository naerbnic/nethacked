/*	SCCS Id: @(#)extern.h	3.4	2003/03/10	*/
/* Copyright (c) Steve Creps, 1988.				  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef EXTERN_H
#define EXTERN_H

#define E extern

/* ### alloc.c ### */

E char *FDECL(fmt_ptr, (const void*,char *));

/* This next pre-processor directive covers almost the entire file,
 * interrupted only occasionally to pick up specific functions as needed. */
#if !defined(MAKEDEFS_C) && !defined(LEV_LEX_C)

/* ### allmain.c ### */

E void moveloop();
E void stop_occupation();
E void display_gamewindows();
E void newgame();
E void FDECL(welcome, (bool));
#if defined(RECORD_REALTIME) || defined(REALTIME_ON_BOTL)
E time_t get_realtime();
#endif

/* ### apply.c ### */

E int doapply();
E int dorub();
E int dojump();
E int FDECL(jump, (int));
E int number_leashed();
E void FDECL(o_unleash, (struct Object *));
E void FDECL(m_unleash, (struct Monster *,bool));
E void unleash_all();
E bool next_to_u();
E struct Object *FDECL(get_mleash, (struct Monster *));
E void FDECL(check_leash, (xchar,xchar));
E bool FDECL(um_dist, (xchar,xchar,xchar));
E bool FDECL(snuff_candle, (struct Object *));
E bool FDECL(snuff_lit, (struct Object *));
E bool FDECL(catch_lit, (struct Object *));
E void FDECL(use_unicorn_horn, (struct Object *));
E bool FDECL(tinnable, (struct Object *));
E void reset_trapset();
E void FDECL(fig_transform, (genericptr_t, long));
E int FDECL(unfixable_trouble_count,(bool));

/* ### artifact.c ### */

E void init_artifacts();
E void FDECL(save_artifacts, (int));
E void FDECL(restore_artifacts, (int));
E const char *FDECL(artiname, (int));
E struct Object *FDECL(mk_artifact, (struct Object *,ALIGNTYP_P));
E const char *FDECL(artifact_name, (const char *,short *));
E bool FDECL(exist_artifact, (int,const char *));
E void FDECL(artifact_exists, (struct Object *,const char *,bool));
E int nartifact_exist();
E bool FDECL(spec_ability, (struct Object *,unsigned long));
E bool FDECL(confers_luck, (struct Object *));
E bool FDECL(arti_reflects, (struct Object *));
E bool FDECL(restrict_name, (struct Object *,const char *));
E bool FDECL(defends, (int,struct Object *));
E bool FDECL(protects, (int,struct Object *));
E void FDECL(set_artifact_intrinsic, (struct Object *,bool,long));
E int FDECL(touch_artifact, (struct Object *,struct Monster *));
E int FDECL(spec_abon, (struct Object *,struct Monster *));
E int FDECL(spec_dbon, (struct Object *,struct Monster *,int));
E void FDECL(discover_artifact, (xchar));
E bool FDECL(undiscovered_artifact, (xchar));
E int FDECL(disp_artifact_discoveries, (winid));
E bool FDECL(artifact_hit, (struct Monster *,struct Monster *,
				struct Object *,int *,int));
E int doinvoke();
E void FDECL(arti_speak, (struct Object *));
E bool FDECL(artifact_light, (struct Object *));
E long FDECL(spec_m2, (struct Object *));
E bool FDECL(artifact_has_invprop, (struct Object *,uchar));
E long FDECL(arti_cost, (struct Object *));

/* ### attrib.c ### */

E bool FDECL(adjattrib, (int,int,int));
E void FDECL(change_luck, (schar));
E int FDECL(stone_luck, (bool));
E void set_moreluck();
E void FDECL(gainstr, (struct Object *,int));
E void FDECL(losestr, (int));
E void restore_attrib();
E void FDECL(exercise, (int,bool));
E void exerchk();
E void reset_attribute_clock();
E void FDECL(init_attr, (int));
E void redist_attr();
E void FDECL(adjabil, (int,int));
E int newhp();
E schar FDECL(acurr, (int));
E schar acurrstr();
E void FDECL(adjalign, (int));

/* ### ball.c ### */

E void ballfall();
E void placebc();
E void unplacebc();
E void FDECL(set_bc, (int));
E void FDECL(move_bc, (int,int,xchar,xchar,xchar,xchar));
E bool FDECL(drag_ball, (xchar,xchar,
		int *,xchar *,xchar *,xchar *,xchar *, bool *,bool));
E void FDECL(drop_ball, (xchar,xchar));
E void drag_down();

/* ### bones.c ### */

E bool can_make_bones();
E void FDECL(savebones, (struct Object *));
E int getbones();

/* ### botl.c ### */

E int FDECL(xlev_to_rank, (int));
E int FDECL(title_to_mon, (const char *,int *,int *));
E void max_rank_sz();
#ifdef SCORE_ON_BOTL
E long botl_score();
#endif
E int FDECL(describe_level, (char *));
E const char *FDECL(rank_of, (int,short,bool));
E void bot();
#ifdef DUMP_LOG
E void FDECL(bot1str, (char *));
E void FDECL(bot2str, (char *));
#endif

/* ### cmd.c ### */

#ifdef USE_TRAMPOLI
E int doextcmd();
E int domonability();
E int doprev_message();
E int timed_occupation();
E int wiz_attributes();
E int enter_explore_mode();
# ifdef WIZARD
E int wiz_detect();
E int wiz_genesis();
E int wiz_identify();
E int wiz_level_tele();
E int wiz_map();
E int wiz_where();
E int wiz_wish();
# endif /* WIZARD */
#endif /* USE_TRAMPOLI */
E void reset_occupations();
E void FDECL(set_occupation, (int (*)(void),const char *,int));
#ifdef REDO
E char pgetchar();
E void FDECL(pushch, (char));
E void FDECL(savech, (char));
#endif
#ifdef WIZARD
E void add_debug_extended_commands();
#endif /* WIZARD */
E void FDECL(rhack, (char *));
E int doextlist();
E int extcmd_via_menu();
E void FDECL(enlightenment, (int));
E void FDECL(show_conduct, (int));
#ifdef DUMP_LOG
E void FDECL(dump_enlightenment, (int));
E void FDECL(dump_conduct, (int));
#endif
E int FDECL(xytod, (schar,schar));
E void FDECL(dtoxy, (coord *,int));
E int FDECL(movecmd, (char));
E int FDECL(getdir, (const char *));
E void confdir();
E int FDECL(isok, (int,int));
E int FDECL(get_adjacent_loc, (const char *, const char *, xchar, xchar, coord *));
E const char *FDECL(click_to_cmd, (int,int,int));
E char readchar();
#ifdef WIZARD
E void sanity_check();
#endif
E char FDECL(yn_function, (const char *, const char *, char));

/* ### dbridge.c ### */

E bool FDECL(is_pool, (int,int));
E bool FDECL(is_lava, (int,int));
E bool FDECL(is_ice, (int,int));
E int FDECL(is_drawbridge_wall, (int,int));
E bool FDECL(is_db_wall, (int,int));
E bool FDECL(find_drawbridge, (int *,int*));
E bool FDECL(create_drawbridge, (int,int,int,bool));
E void FDECL(open_drawbridge, (int,int));
E void FDECL(close_drawbridge, (int,int));
E void FDECL(destroy_drawbridge, (int,int));

/* ### decl.c ### */

E void decl_init();

/* ### detect.c ### */

E struct Object *FDECL(o_in, (struct Object*,char));
E struct Object *FDECL(o_material, (struct Object*,unsigned));
E int FDECL(gold_detect, (struct Object *));
E int FDECL(food_detect, (struct Object *));
E int FDECL(object_detect, (struct Object *,int));
E int FDECL(monster_detect, (struct Object *,int));
E int FDECL(trap_detect, (struct Object *));
E const char *FDECL(level_distance, (d_level *));
E void FDECL(use_crystal_ball, (struct Object *));
E void do_mapping();
E void do_vicinity_map();
E void FDECL(cvt_sdoor_to_door, (struct rm *));
#ifdef USE_TRAMPOLI
E void FDECL(findone, (int,int,genericptr_t));
E void FDECL(openone, (int,int,genericptr_t));
#endif
E int findit();
E int openit();
E void FDECL(find_trap, (struct trap *));
E int FDECL(dosearch0, (int));
E int dosearch();
E void sokoban_detect();

/* ### dig.c ### */

E bool is_digging();
#ifdef USE_TRAMPOLI
E int dig();
#endif
E int holetime();
E bool FDECL(dig_check, (struct Monster *, bool, int, int));
E void FDECL(digactualhole, (int,int,struct Monster *,int));
E bool FDECL(dighole, (bool));
E int FDECL(use_pick_axe, (struct Object *));
E int FDECL(use_pick_axe2, (struct Object *));
E bool FDECL(mdig_tunnel, (struct Monster *));
E void FDECL(watch_dig, (struct Monster *,xchar,xchar,bool));
E void zap_dig();
E struct Object *FDECL(bury_an_obj, (struct Object *));
E void FDECL(bury_objs, (int,int));
E void FDECL(unearth_objs, (int,int));
E void FDECL(rot_organic, (genericptr_t, long));
E void FDECL(rot_corpse, (genericptr_t, long));
#if 0
E void FDECL(bury_monst, (struct Monster *));
E void bury_you();
E void unearth_you();
E void escape_tomb();
E void FDECL(bury_obj, (struct Object *));
#endif

/* ### display.c ### */

#ifdef INVISIBLE_OBJECTS
E struct Object * FDECL(vobj_at, (xchar,xchar));
#endif /* INVISIBLE_OBJECTS */
E void FDECL(magic_map_background, (xchar,xchar,int));
E void FDECL(map_background, (xchar,xchar,int));
E void FDECL(map_trap, (struct trap *,int));
E void FDECL(map_object, (struct Object *,int));
E void FDECL(map_invisible, (xchar,xchar));
E void FDECL(unmap_object, (int,int));
E void FDECL(map_location, (int,int,int));
E void FDECL(feel_location, (xchar,xchar));
E void FDECL(newsym, (int,int));
E void FDECL(shieldeff, (xchar,xchar));
E void FDECL(tmp_at, (int,int));
E void FDECL(swallowed, (int));
E void FDECL(under_ground, (int));
E void FDECL(under_water, (int));
E void see_monsters();
E void set_mimic_blocking();
E void see_objects();
E void see_traps();
E void curs_on_u();
E int doredraw();
E void docrt();
E void FDECL(show_glyph, (int,int,int));
E void clear_glyph_buffer();
E void FDECL(row_refresh, (int,int,int));
E void cls();
E void FDECL(flush_screen, (int));
#ifdef DUMP_LOG
E void dump_screen();
#endif
E int FDECL(back_to_glyph, (xchar,xchar));
E int FDECL(zapdir_to_glyph, (int,int,int));
E int FDECL(glyph_at, (xchar,xchar));
E void set_wall_state();

/* ### do.c ### */

#ifdef USE_TRAMPOLI
E int FDECL(drop, (struct Object *));
E int wipeoff();
#endif
E int dodrop();
E bool FDECL(boulder_hits_pool, (struct Object *,int,int,bool));
E bool FDECL(flooreffects, (struct Object *,int,int,const char *));
E void FDECL(doaltarobj, (struct Object *));
E bool FDECL(canletgo, (struct Object *,const char *));
E void FDECL(dropx, (struct Object *));
E void FDECL(dropy, (struct Object *));
E void FDECL(obj_no_longer_held, (struct Object *));
E int doddrop();
E int dodown();
E int doup();
#ifdef INSURANCE
E void save_currentstate();
#endif
E void FDECL(goto_level, (d_level *,bool,bool,bool));
E void FDECL(schedule_goto, (d_level *,bool,bool,int,
			     const char *,const char *));
E void deferred_goto();
E bool FDECL(revive_corpse, (struct Object *));
E void FDECL(revive_mon, (genericptr_t, long));
E int donull();
E int dowipe();
E void FDECL(set_wounded_legs, (long,int));
E void heal_legs();

/* ### do_name.c ### */

E int FDECL(getpos, (coord *,bool,const char *));
E struct Monster *FDECL(christen_monst, (struct Monster *,const char *));
E int do_mname();
E struct Object *FDECL(oname, (struct Object *,const char *));
E int ddocall();
E void FDECL(docall, (struct Object *));
E const char *rndghostname();
E char *FDECL(x_monnam, (struct Monster *,int,const char *,int,bool));
E char *FDECL(l_monnam, (struct Monster *));
E char *FDECL(mon_nam, (struct Monster *));
E char *FDECL(noit_mon_nam, (struct Monster *));
E char *FDECL(Monnam, (struct Monster *));
E char *FDECL(noit_Monnam, (struct Monster *));
E char *FDECL(m_monnam, (struct Monster *));
E char *FDECL(y_monnam, (struct Monster *));
E char *FDECL(Adjmonnam, (struct Monster *,const char *));
E char *FDECL(Amonnam, (struct Monster *));
E char *FDECL(a_monnam, (struct Monster *));
E char *FDECL(distant_monnam, (struct Monster *,int,char *));
E const char *rndmonnam();
E const char *FDECL(hcolor, (const char *));
E const char *rndcolor();
#ifdef REINCARNATION
E const char *roguename();
#endif
E struct Object *FDECL(realloc_obj,
		(struct Object *, int, genericptr_t, int, const char *));
E char *FDECL(coyotename, (struct Monster *,char *));

/* ### do_wear.c ### */

#ifdef USE_TRAMPOLI
E int Armor_on();
E int Boots_on();
E int Gloves_on();
E int Helmet_on();
E int FDECL(select_off, (struct Object *));
E int take_off();
#endif
E void FDECL(off_msg, (struct Object *));
E void set_wear();
E bool FDECL(donning, (struct Object *));
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
E void FDECL(Ring_on, (struct Object *));
E void FDECL(Ring_off, (struct Object *));
E void FDECL(Ring_gone, (struct Object *));
E void FDECL(Blindf_on, (struct Object *));
E void FDECL(Blindf_off, (struct Object *));
E int dotakeoff();
E int doremring();
E int FDECL(cursed, (struct Object *));
E int FDECL(armoroff, (struct Object *));
E int FDECL(canwearobj, (struct Object *, long *, bool));
E int dowear();
E int doputon();
E void find_ac();
E void glibr();
E struct Object *FDECL(some_armor,(struct Monster *));
E void FDECL(erode_armor, (struct Monster *,bool));
E struct Object *FDECL(stuck_ring, (struct Object *,int));
E struct Object *unchanger();
E void reset_remarm();
E int doddoremarm();
E int FDECL(destroy_arm, (struct Object *));
E void FDECL(adj_abon, (struct Object *,schar));

/* ### dog.c ### */

E void FDECL(initedog, (struct Monster *));
E struct Monster *FDECL(make_familiar, (struct Object *,xchar,xchar,bool));
E struct Monster *makedog();
E void update_mlstmv();
E void losedogs();
E void FDECL(mon_arrive, (struct Monster *,bool));
E void FDECL(mon_catchup_elapsed_time, (struct Monster *,long));
E void FDECL(keepdogs, (bool));
E void FDECL(migrate_to_level, (struct Monster *,xchar,xchar,coord *));
E int FDECL(dogfood, (struct Monster *,struct Object *));
E struct Monster *FDECL(tamedog, (struct Monster *,struct Object *));
E void FDECL(abuse_dog, (struct Monster *));
E void FDECL(wary_dog, (struct Monster *, bool));

/* ### dogmove.c ### */

E int FDECL(dog_nutrition, (struct Monster *,struct Object *));
E int FDECL(dog_eat, (struct Monster *,struct Object *,int,int,bool));
E int FDECL(dog_move, (struct Monster *,int));
#ifdef USE_TRAMPOLI
E void FDECL(wantdoor, (int,int,genericptr_t));
#endif

/* ### dokick.c ### */

E bool FDECL(ghitm, (struct Monster *,struct Object *));
E void FDECL(container_impact_dmg, (struct Object *));
E int dokick();
E bool FDECL(ship_object, (struct Object *,xchar,xchar,bool));
E void obj_delivery();
E schar FDECL(down_gate, (xchar,xchar));
E void FDECL(impact_drop, (struct Object *,xchar,xchar,xchar));

/* ### dothrow.c ### */

E int dothrow();
E int dofire();
E void FDECL(hitfloor, (struct Object *));
E void FDECL(hurtle, (int,int,int,bool));
E void FDECL(mhurtle, (struct Monster *,int,int,int));
E void FDECL(throwit, (struct Object *,long,bool));
E int FDECL(omon_adj, (struct Monster *,struct Object *,bool));
E int FDECL(thitmonst, (struct Monster *,struct Object *));
E int FDECL(hero_breaks, (struct Object *,xchar,xchar,bool));
E int FDECL(breaks, (struct Object *,xchar,xchar));
E bool FDECL(breaktest, (struct Object *));
E bool FDECL(walk_path, (coord *, coord *, bool (*)(genericptr_t,int,int), genericptr_t));
E bool FDECL(hurtle_step, (genericptr_t, int, int));

/* ### drawing.c ### */
#endif /* !MAKEDEFS_C && !LEV_LEX_C */
E int FDECL(def_char_to_objclass, (char));
E int FDECL(def_char_to_monclass, (char));
#if !defined(MAKEDEFS_C) && !defined(LEV_LEX_C)
E void FDECL(assign_graphics, (uchar *,int,int,int));
E void FDECL(switch_graphics, (int));
#ifdef REINCARNATION
E void FDECL(assign_rogue_graphics, (bool));
#endif

/* ### dungeon.c ### */

E void FDECL(save_dungeon, (int,bool,bool));
E void FDECL(restore_dungeon, (int));
E void FDECL(insert_branch, (branch *,bool));
E void init_dungeons();
E s_level *FDECL(find_level, (const char *));
E s_level *FDECL(Is_special, (d_level *));
E branch *FDECL(Is_branchlev, (d_level *));
E xchar FDECL(ledger_no, (d_level *));
E xchar maxledgerno();
E schar FDECL(depth, (d_level *));
E xchar FDECL(dunlev, (d_level *));
E xchar FDECL(dunlevs_in_dungeon, (d_level *));
E xchar FDECL(ledger_to_dnum, (xchar));
E xchar FDECL(ledger_to_dlev, (xchar));
E xchar FDECL(deepest_lev_reached, (bool));
E bool FDECL(on_level, (d_level *,d_level *));
E void FDECL(next_level, (bool));
E void FDECL(prev_level, (bool));
E void FDECL(u_on_newpos, (int,int));
E void u_on_sstairs();
E void u_on_upstairs();
E void u_on_dnstairs();
E bool FDECL(On_stairs, (xchar,xchar));
E void FDECL(get_level, (d_level *,int));
E bool FDECL(Is_botlevel, (d_level *));
E bool FDECL(Can_fall_thru, (d_level *));
E bool FDECL(Can_dig_down, (d_level *));
E bool FDECL(Can_rise_up, (int,int,d_level *));
E bool FDECL(In_quest, (d_level *));
E bool FDECL(In_mines, (d_level *));
E branch *FDECL(dungeon_branch, (const char *));
E bool FDECL(at_dgn_entrance, (const char *));
E bool FDECL(In_hell, (d_level *));
E bool FDECL(In_V_tower, (d_level *));
E bool FDECL(On_W_tower_level, (d_level *));
E bool FDECL(In_W_tower, (int,int,d_level *));
E void FDECL(find_hell, (d_level *));
E void FDECL(goto_hell, (bool,bool));
E void FDECL(assign_level, (d_level *,d_level *));
E void FDECL(assign_rnd_level, (d_level *,d_level *,int));
E int FDECL(induced_align, (int));
E bool FDECL(Invocation_lev, (d_level *));
E xchar level_difficulty();
E schar FDECL(lev_by_name, (const char *));
#ifdef WIZARD
E schar FDECL(print_dungeon, (bool,schar *,xchar *));
#endif

/* ### eat.c ### */

#ifdef USE_TRAMPOLI
E int eatmdone();
E int eatfood();
E int opentin();
E int unfaint();
#endif
E bool FDECL(is_edible, (struct Object *));
E void init_uhunger();
E int Hear_again();
E void reset_eat();
E int doeat();
E void gethungry();
E void FDECL(morehungry, (int));
E void FDECL(lesshungry, (int));
E bool is_fainted();
E void reset_faint();
E void violated_vegetarian();
#if 0
E void sync_hunger();
#endif
E void FDECL(newuhs, (bool));
E struct Object *FDECL(floorfood, (const char *,int));
E void vomit();
E int FDECL(eaten_stat, (int,struct Object *));
E void FDECL(food_disappears, (struct Object *));
E void FDECL(food_substitution, (struct Object *,struct Object *));
E void fix_petrification();
E void FDECL(consume_oeaten, (struct Object *,int));
E bool FDECL(maybe_finished_meal, (bool));

/* ### end.c ### */

E void FDECL(done1, (int));
E int done2();
#ifdef USE_TRAMPOLI
E void FDECL(done_intr, (int));
#endif
E void FDECL(done_in_by, (struct Monster *));
#endif /* !MAKEDEFS_C && !LEV_LEX_C */
E void VDECL(panic, (const char *,...)) PRINTF_F(1,2);
#if !defined(MAKEDEFS_C) && !defined(LEV_LEX_C)
E void FDECL(done, (int));
E void FDECL(container_contents, (struct Object *,bool,bool));
#ifdef DUMP_LOG
E void FDECL(dump, (char const*, char const*));
E void FDECL(do_containerconts, (struct Object *,bool,bool,bool,bool));
#endif
E void FDECL(terminate, (int));
E int num_genocides();

/* ### engrave.c ### */

E char *FDECL(random_engraving, (char *));
E void FDECL(wipeout_text, (char *,int,unsigned));
E bool can_reach_floor();
E const char *FDECL(surface, (int,int));
E const char *FDECL(ceiling, (int,int));
E struct engr *FDECL(engr_at, (xchar,xchar));
#ifdef ELBERETH
E int FDECL(sengr_at, (const char *,xchar,xchar));
#endif
E void FDECL(u_wipe_engr, (int));
E void FDECL(wipe_engr_at, (xchar,xchar,xchar));
E void FDECL(read_engr_at, (int,int));
E void FDECL(make_engr_at, (int,int,const char *,long,xchar));
E void FDECL(del_engr_at, (int,int));
E int freehand();
E int doengrave();
E void FDECL(save_engravings, (int,int));
E void FDECL(rest_engravings, (int));
E void FDECL(del_engr, (struct engr *));
E void FDECL(rloc_engr, (struct engr *));
E void FDECL(make_grave, (int,int,const char *));

/* ### exper.c ### */

E int FDECL(experience, (struct Monster *,int));
E void FDECL(more_experienced, (int,int));
E void FDECL(losexp, (const char *));
E void newexplevel();
E void FDECL(pluslvl, (bool));
E long FDECL(rndexp, (bool));

/* ### explode.c ### */

E void FDECL(explode, (int,int,int,int,char,int));
E long FDECL(scatter, (int, int, int, unsigned int, struct Object *));
E void FDECL(splatter_burning_oil, (int, int));

/* ### extralev.c ### */

#ifdef REINCARNATION
E void makeroguerooms();
E void FDECL(corr, (int,int));
E void makerogueghost();
#endif

/* ### files.c ### */

E char *FDECL(fname_encode, (const char *, char, char *, char *, int));
E char *FDECL(fname_decode, (char, char *, char *, int));
E const char *FDECL(fqname, (const char *, int, int));
E FILE *FDECL(fopen_datafile, (const char *,const char *,int));
E bool FDECL(uptodate, (int,const char *));
E void FDECL(store_version, (int));
#ifdef MFLOPPY
E void set_lock_and_bones();
#endif
E void FDECL(set_levelfile_name, (char *,int));
E int FDECL(create_levelfile, (int,char *));
E int FDECL(open_levelfile, (int,char *));
E void FDECL(delete_levelfile, (int));
E void clearlocks();
E int FDECL(create_bonesfile, (d_level*,char **, char *));
#ifdef MFLOPPY
E void cancel_bonesfile();
#endif
E void FDECL(commit_bonesfile, (d_level *));
E int FDECL(open_bonesfile, (d_level*,char **));
E int FDECL(delete_bonesfile, (d_level*));
E void compress_bonesfile();
E void set_savefile_name();
#ifdef INSURANCE
E void FDECL(save_savefile_name, (int));
#endif
#if defined(WIZARD) && !defined(MICRO)
E void set_error_savefile();
#endif
E int create_savefile();
E int open_savefile();
E int delete_savefile();
E int restore_saved_game();
E void FDECL(compress, (const char *));
E void FDECL(uncompress, (const char *));
E bool FDECL(lock_file, (const char *,int,int));
E void FDECL(unlock_file, (const char *));
#ifdef USER_SOUNDS
E bool FDECL(can_read_file, (const char *));
#endif
E void FDECL(read_config_file, (const char *));
E void FDECL(check_recordfile, (const char *));
#if defined(WIZARD)
E void read_wizkit();
#endif
E void FDECL(paniclog, (const char *, const char *));
E int FDECL(validate_prefix_locations, (char *));
E char** get_saved_games();
E void FDECL(free_saved_games, (char**));
#ifdef SELF_RECOVER
E bool recover_savefile();
#endif
#ifdef HOLD_LOCKFILE_OPEN
E void really_close();
#endif

/* ### fountain.c ### */

E void FDECL(floating_above, (const char *));
E void FDECL(dogushforth, (int));
# ifdef USE_TRAMPOLI
E void FDECL(gush, (int,int,genericptr_t));
# endif
E void FDECL(dryup, (xchar,xchar, bool));
E void drinkfountain();
E void FDECL(dipfountain, (struct Object *));
#ifdef SINKS
E void FDECL(breaksink, (int,int));
E void drinksink();
#endif

/* ### hack.c ### */

E bool FDECL(revive_nasty, (int,int,const char*));
E void FDECL(movobj, (struct Object *,xchar,xchar));
E bool FDECL(may_dig, (xchar,xchar));
E bool FDECL(may_passwall, (xchar,xchar));
E bool FDECL(bad_rock, (struct permonst *,xchar,xchar));
E bool FDECL(invocation_pos, (xchar,xchar));
E bool FDECL(test_move, (int, int, int, int, int));
E void domove();
E void invocation_message();
E void FDECL(spoteffects, (bool));
E char *FDECL(in_rooms, (xchar,xchar,int));
E bool FDECL(in_town, (int,int));
E void FDECL(check_special_room, (bool));
E int dopickup();
E void lookaround();
E int monster_nearby();
E void FDECL(nomul, (int, const char *));
E void FDECL(unmul, (const char *));
E void FDECL(losehp, (int,const char *,bool));
E int weight_cap();
E int inv_weight();
E int near_capacity();
E int FDECL(calc_capacity, (int));
E int max_capacity();
E bool FDECL(check_capacity, (const char *));
E int inv_cnt();
#ifdef GOLDOBJ
E long FDECL(money_cnt, (struct Object *));
#endif

/* ### hacklib.c ### */

E bool FDECL(digit, (char));
E bool FDECL(letter, (char));
E char FDECL(highc, (char));
E char FDECL(lowc, (char));
E char *FDECL(lcase, (char *));
E char *FDECL(upstart, (char *));
E char *FDECL(mungspaces, (char *));
E char *FDECL(eos, (char *));
E char *FDECL(strkitten, (char *,char));
E char *FDECL(s_suffix, (const char *));
E char *FDECL(xcrypt, (const char *,char *));
E bool FDECL(onlyspace, (const char *));
E char *FDECL(tabexpand, (char *));
E char *FDECL(visctrl, (char));
E const char *FDECL(ordin, (int));
E char *FDECL(sitoa, (int));
E int FDECL(sgn, (int));
E int FDECL(rounddiv, (long,int));
E int FDECL(dist2, (int,int,int,int));
E int FDECL(distmin, (int,int,int,int));
E bool FDECL(online2, (int,int,int,int));
E bool FDECL(pmatch, (const char *,const char *));
#ifndef STRNCMPI
E int FDECL(strncmpi, (const char *,const char *,int));
#endif
#ifndef STRSTRI
E char *FDECL(strstri, (const char *,const char *));
#endif
E bool FDECL(fuzzymatch, (const char *,const char *,const char *,bool));
E void setrandom();
E int getyear();
#if 0
E char *FDECL(yymmdd, (time_t));
#endif
E long FDECL(yyyymmdd, (time_t));
E int phase_of_the_moon();
E bool friday_13th();
E int night();
E int midnight();

/* ### invent.c ### */

E void FDECL(assigninvlet, (struct Object *));
E struct Object *FDECL(merge_choice, (struct Object *,struct Object *));
E int FDECL(merged, (struct Object **,struct Object **));
#ifdef USE_TRAMPOLI
E int FDECL(ckunpaid, (struct Object *));
#endif
E void FDECL(addinv_core1, (struct Object *));
E void FDECL(addinv_core2, (struct Object *));
E struct Object *FDECL(addinv, (struct Object *));
E struct Object *FDECL(hold_another_object,
			(struct Object *,const char *,const char *,const char *));
E void FDECL(useupall, (struct Object *));
E void FDECL(useup, (struct Object *));
E void FDECL(consume_obj_charge, (struct Object *,bool));
E void FDECL(freeinv_core, (struct Object *));
E void FDECL(freeinv, (struct Object *));
E void FDECL(delallobj, (int,int));
E void FDECL(delobj, (struct Object *));
E struct Object *FDECL(sobj_at, (int,int,int));
E struct Object *FDECL(carrying, (int));
E bool have_lizard();
E struct Object *FDECL(o_on, (unsigned int,struct Object *));
E bool FDECL(obj_here, (struct Object *,int,int));
E bool wearing_armor();
E bool FDECL(is_worn, (struct Object *));
E struct Object *FDECL(g_at, (int,int));
E struct Object *FDECL(mkgoldobj, (long));
E struct Object *FDECL(getobj, (const char *,const char *));
E int FDECL(ggetobj, (const char *,int (*)(Object*),int,bool,unsigned *));
E void FDECL(fully_identify_obj, (struct Object *));
E int FDECL(identify, (struct Object *));
E void FDECL(identify_pack, (int));
E int FDECL(askchain, (struct Object **,const char *,int,int (*)(Object*),
			int (*)(Object*),int,const char *));
E void FDECL(prinv, (const char *,struct Object *,long));
E char *FDECL(xprname, (struct Object *,const char *,char,bool,long,long));
E int ddoinv();
E char FDECL(display_inventory, (const char *,bool));
#ifdef DUMP_LOG
E char FDECL(dump_inventory, (const char *,bool,bool));
#endif
E int FDECL(display_binventory, (int,int,bool));
E struct Object *FDECL(display_cinventory,(struct Object *));
E struct Object *FDECL(display_minventory,(struct Monster *,int,char *));
E int dotypeinv();
E const char *FDECL(dfeature_at, (int,int,char *));
E int FDECL(look_here, (int,bool));
E int dolook();
E bool FDECL(will_feel_cockatrice, (struct Object *,bool));
E void FDECL(feel_cockatrice, (struct Object *,bool));
E void FDECL(stackobj, (struct Object *));
E int doprgold();
E int doprwep();
E int doprarm();
E int doprring();
E int dopramulet();
E int doprtool();
E int doprinuse();
E void FDECL(useupf, (struct Object *,long));
E char *FDECL(let_to_name, (char,bool));
E void free_invbuf();
E void reassign();
E int doorganize();
E int FDECL(count_unpaid, (struct Object *));
E int FDECL(count_buc, (struct Object *,int));
E void FDECL(carry_obj_effects, (struct Object *));
E const char *FDECL(currency, (long));
E void FDECL(silly_thing, (const char *,struct Object *));

/* ### ioctl.c ### */

#if defined(UNIX) || defined(__BEOS__)
E void getwindowsz();
E void getioctls();
E void setioctls();
# ifdef SUSPEND
E int dosuspend();
# endif /* SUSPEND */
#endif /* UNIX || __BEOS__ */

/* ### light.c ### */

E void FDECL(new_light_source, (xchar, xchar, int, int, genericptr_t));
E void FDECL(del_light_source, (int, genericptr_t));
E void FDECL(do_light_sources, (char **));
E struct Monster *FDECL(find_mid, (unsigned, unsigned));
E void FDECL(save_light_sources, (int, int, int));
E void FDECL(restore_light_sources, (int));
E void FDECL(relink_light_sources, (bool));
E void FDECL(obj_move_light_source, (struct Object *, struct Object *));
E bool any_light_source();
E void FDECL(snuff_light_source, (int, int));
E bool FDECL(obj_sheds_light, (struct Object *));
E bool FDECL(obj_is_burning, (struct Object *));
E void FDECL(obj_split_light_source, (struct Object *, struct Object *));
E void FDECL(obj_merge_light_sources, (struct Object *,struct Object *));
E int FDECL(candle_light_range, (struct Object *));
#ifdef WIZARD
E int wiz_light_sources();
#endif

/* ### lock.c ### */

#ifdef USE_TRAMPOLI
E int forcelock();
E int picklock();
#endif
E bool FDECL(picking_lock, (int *,int *));
E bool FDECL(picking_at, (int,int));
E void reset_pick();
E int FDECL(pick_lock, (struct Object *));
E int doforce();
E bool FDECL(boxlock, (struct Object *,struct Object *));
E bool FDECL(doorlock, (struct Object *,int,int));
E int doopen();
E int doclose();

#ifdef MAC
/* These declarations are here because the main code calls them. */

/* ### macfile.c ### */

E int FDECL(maccreat, (const char *,long));
E int FDECL(macopen, (const char *,int,long));
E int FDECL(macclose, (int));
E int FDECL(macread, (int,void *,unsigned));
E int FDECL(macwrite, (int,void *,unsigned));
E long FDECL(macseek, (int,long,short));
E int FDECL(macunlink, (const char *));

/* ### macsnd.c ### */

E void FDECL(mac_speaker, (struct Object *,char *));

/* ### macunix.c ### */

E void FDECL(regularize, (char *));
E void getlock();

/* ### macwin.c ### */

E void FDECL(lock_mouse_cursor, (Boolean));
E int SanePositions();

/* ### mttymain.c ### */

E void FDECL(getreturn, (char *));
E void VDECL(msmsg, (const char *,...));
E void gettty();
E void setftty();
E void FDECL(settty, (const char *));
E int tgetch();
E void FDECL(cmov, (int x, int y));
E void FDECL(nocmov, (int x, int y));

#endif /* MAC */

/* ### mail.c ### */

#ifdef MAIL
# ifdef UNIX
E void getmailstatus();
# endif
E void ckmailstatus();
E void FDECL(readmail, (struct Object *));
#endif /* MAIL */

/* ### makemon.c ### */

E bool FDECL(is_home_elemental, (struct permonst *));
E struct Monster *FDECL(clone_mon, (struct Monster *,xchar,xchar));
E struct Monster *FDECL(makemon, (struct permonst *,int,int,int));
E bool FDECL(create_critters, (int,struct permonst *));
E struct permonst *rndmonst();
E void FDECL(reset_rndmonst, (int));
E struct permonst *FDECL(mkclass, (char,int));
E int FDECL(adj_lev, (struct permonst *));
E struct permonst *FDECL(grow_up, (struct Monster *,struct Monster *));
E int FDECL(mongets, (struct Monster *,int));
E int FDECL(golemhp, (int));
E bool FDECL(peace_minded, (struct permonst *));
E void FDECL(set_malign, (struct Monster *));
E void FDECL(set_mimic_sym, (struct Monster *));
E int FDECL(mbirth_limit, (int));
E void FDECL(mimic_hit_msg, (struct Monster *, short));
#ifdef GOLDOBJ
E void FDECL(mkmonmoney, (struct Monster *, long));
#endif
E void FDECL(bagotricks, (struct Object *));
E bool FDECL(propagate, (int, bool,bool));

/* ### mapglyph.c ### */

E void FDECL(mapglyph, (int, int *, int *, unsigned *, int, int));

/* ### mcastu.c ### */

E int FDECL(castmu, (struct Monster *,struct Attack *,bool,bool));
E int FDECL(buzzmu, (struct Monster *,struct Attack *));

/* ### mhitm.c ### */

E int FDECL(fightm, (struct Monster *));
E int FDECL(mattackm, (struct Monster *,struct Monster *));
E int FDECL(noattacks, (struct permonst *));
E int FDECL(sleep_monst, (struct Monster *,int,int));
E void FDECL(slept_monst, (struct Monster *));
E long FDECL(attk_protection, (int));

/* ### mhitu.c ### */

E const char *FDECL(mpoisons_subj, (struct Monster *,struct Attack *));
E void u_slow_down();
E struct Monster *cloneu();
E void FDECL(expels, (struct Monster *,struct permonst *,bool));
E struct Attack *FDECL(getmattk, (struct permonst *,int,int *,struct Attack *));
E int FDECL(mattacku, (struct Monster *));
E int FDECL(magic_negation, (struct Monster *));
E int FDECL(gazemu, (struct Monster *,struct Attack *));
E void FDECL(mdamageu, (struct Monster *,int));
E int FDECL(could_seduce, (struct Monster *,struct Monster *,struct Attack *));
#ifdef SEDUCE
E int FDECL(doseduce, (struct Monster *));
#endif

/* ### minion.c ### */

E void FDECL(msummon, (struct Monster *));
E void FDECL(summon_minion, (ALIGNTYP_P,bool));
E int FDECL(demon_talk, (struct Monster *));
E long FDECL(bribe, (struct Monster *));
E int FDECL(dprince, (ALIGNTYP_P));
E int FDECL(dlord, (ALIGNTYP_P));
E int llord();
E int FDECL(ndemon, (ALIGNTYP_P));
E int lminion();

/* ### mklev.c ### */

#ifdef USE_TRAMPOLI
E int FDECL(do_comp, (genericptr_t,genericptr_t));
#endif
E void sort_rooms();
E void FDECL(add_room, (int,int,int,int,bool,schar,bool));
E void FDECL(add_subroom, (struct mkroom *,int,int,int,int,
			   bool,schar,bool));
E void makecorridors();
E void FDECL(add_door, (int,int,struct mkroom *));
E void mklev();
#ifdef SPECIALIZATION
E void FDECL(topologize, (struct mkroom *,bool));
#else
E void FDECL(topologize, (struct mkroom *));
#endif
E void FDECL(place_branch, (branch *,xchar,xchar));
E bool FDECL(occupied, (xchar,xchar));
E int FDECL(okdoor, (xchar,xchar));
E void FDECL(dodoor, (int,int,struct mkroom *));
E void FDECL(mktrap, (int,int,struct mkroom *,coord*));
E void FDECL(mkstairs, (xchar,xchar,char,struct mkroom *));
E void mkinvokearea();

/* ### mkmap.c ### */

void FDECL(flood_fill_rm, (int,int,int,bool,bool));
void FDECL(remove_rooms, (int,int,int,int));

/* ### mkmaze.c ### */

E void FDECL(wallification, (int,int,int,int));
E void FDECL(walkfrom, (int,int));
E void FDECL(makemaz, (const char *));
E void FDECL(mazexy, (coord *));
E void bound_digging();
E void FDECL(mkportal, (xchar,xchar,xchar,xchar));
E bool FDECL(bad_location, (xchar,xchar,xchar,xchar,xchar,xchar));
E void FDECL(place_lregion, (xchar,xchar,xchar,xchar,
			     xchar,xchar,xchar,xchar,
			     xchar,d_level *));
E void movebubbles();
E void water_friction();
E void FDECL(save_waterlevel, (int,int));
E void FDECL(restore_waterlevel, (int));
E const char *FDECL(waterbody_name, (xchar,xchar));

/* ### mkobj.c ### */

E struct Object *FDECL(mkobj_at, (char,int,int,bool));
E struct Object *FDECL(mksobj_at, (int,int,int,bool,bool));
E struct Object *FDECL(mkobj, (char,bool));
E int rndmonnum();
E struct Object *FDECL(splitobj, (struct Object *,long));
E void FDECL(replace_object, (struct Object *,struct Object *));
E void FDECL(bill_dummy_object, (struct Object *));
E struct Object *FDECL(mksobj, (int,bool,bool));
E int FDECL(bcsign, (struct Object *));
E int FDECL(weight, (struct Object *));
E struct Object *FDECL(mkgold, (long,int,int));
E struct Object *FDECL(mkcorpstat,
		(int,struct Monster *,struct permonst *,int,int,bool));
E struct Object *FDECL(obj_attach_mid, (struct Object *, unsigned));
E struct Monster *FDECL(get_mtraits, (struct Object *, bool));
E struct Object *FDECL(mk_tt_object, (int,int,int));
E struct Object *FDECL(mk_named_object,
			(int,struct permonst *,int,int,const char *));
E struct Object *FDECL(rnd_treefruit_at, (int, int));
E void FDECL(start_corpse_timeout, (struct Object *));
E void FDECL(bless, (struct Object *));
E void FDECL(unbless, (struct Object *));
E void FDECL(curse, (struct Object *));
E void FDECL(uncurse, (struct Object *));
E void FDECL(blessorcurse, (struct Object *,int));
E bool FDECL(is_flammable, (struct Object *));
E bool FDECL(is_rottable, (struct Object *));
E void FDECL(place_object, (struct Object *,int,int));
E void FDECL(remove_object, (struct Object *));
E void FDECL(discard_minvent, (struct Monster *));
E void FDECL(obj_extract_self, (struct Object *));
E void FDECL(extract_nobj, (struct Object *, struct Object **));
E void FDECL(extract_nexthere, (struct Object *, struct Object **));
E int FDECL(add_to_minv, (struct Monster *, struct Object *));
E struct Object *FDECL(add_to_container, (struct Object *, struct Object *));
E void FDECL(add_to_migration, (struct Object *));
E void FDECL(add_to_buried, (struct Object *));
E void FDECL(dealloc_obj, (struct Object *));
E void FDECL(obj_ice_effects, (int, int, bool));
E long FDECL(peek_at_iced_corpse_age, (struct Object *));
#ifdef WIZARD
E void obj_sanity_check();
#endif

/* ### mkroom.c ### */

E void FDECL(mkroom, (int));
E void FDECL(fill_zoo, (struct mkroom *));
E bool FDECL(nexttodoor, (int,int));
E bool FDECL(has_dnstairs, (struct mkroom *));
E bool FDECL(has_upstairs, (struct mkroom *));
E int FDECL(somex, (struct mkroom *));
E int FDECL(somey, (struct mkroom *));
E bool FDECL(inside_room, (struct mkroom *,xchar,xchar));
E bool FDECL(somexy, (struct mkroom *,coord *));
E void FDECL(mkundead, (coord *,bool,int));
E struct permonst *courtmon();
E void FDECL(save_rooms, (int));
E void FDECL(rest_rooms, (int));
E struct mkroom *FDECL(search_special, (schar));

/* ### mon.c ### */

E int FDECL(undead_to_corpse, (int));
E int FDECL(genus, (int,int));
E int FDECL(pm_to_cham, (int));
E int FDECL(minliquid, (struct Monster *));
E int movemon();
E int FDECL(meatmetal, (struct Monster *));
E int FDECL(meatobj, (struct Monster *));
E void FDECL(mpickgold, (struct Monster *));
E bool FDECL(mpickstuff, (struct Monster *,const char *));
E int FDECL(curr_mon_load, (struct Monster *));
E int FDECL(max_mon_load, (struct Monster *));
E bool FDECL(can_carry, (struct Monster *,struct Object *));
E int FDECL(mfndpos, (struct Monster *,coord *,long *,long));
E bool FDECL(monnear, (struct Monster *,int,int));
E void dmonsfree();
E int FDECL(mcalcmove, (struct Monster*));
E void mcalcdistress();
E void FDECL(replmon, (struct Monster *,struct Monster *));
E void FDECL(relmon, (struct Monster *));
E struct Object *FDECL(mlifesaver, (struct Monster *));
E bool FDECL(corpse_chance,(struct Monster *,struct Monster *,bool));
E void FDECL(mondead, (struct Monster *));
E void FDECL(mondied, (struct Monster *));
E void FDECL(mongone, (struct Monster *));
E void FDECL(monstone, (struct Monster *));
E void FDECL(monkilled, (struct Monster *,const char *,int));
E void FDECL(unstuck, (struct Monster *));
E void FDECL(killed, (struct Monster *));
E void FDECL(xkilled, (struct Monster *,int));
E void FDECL(mon_to_stone, (struct Monster*));
E void FDECL(mnexto, (struct Monster *));
E bool FDECL(mnearto, (struct Monster *,xchar,xchar,bool));
E void FDECL(poisontell, (int));
E void FDECL(poisoned, (const char *,int,const char *,int));
E void FDECL(m_respond, (struct Monster *));
E void FDECL(setmangry, (struct Monster *));
E void FDECL(wakeup, (struct Monster *));
E void wake_nearby();
E void FDECL(wake_nearto, (int,int,int));
E void FDECL(seemimic, (struct Monster *));
E void rescham();
E void restartcham();
E void FDECL(restore_cham, (struct Monster *));
E void FDECL(mon_animal_list, (bool));
E int FDECL(newcham, (struct Monster *,struct permonst *,bool,bool));
E int FDECL(can_be_hatched, (int));
E int FDECL(egg_type_from_parent, (int,bool));
E bool FDECL(dead_species, (int,bool));
E void kill_genocided_monsters();
E void FDECL(golemeffects, (struct Monster *,int,int));
E bool FDECL(angry_guards, (bool));
E void pacify_guards();

/* ### mondata.c ### */

E void FDECL(set_mon_data, (struct Monster *,struct permonst *,int));
E struct Attack *FDECL(attacktype_fordmg, (struct permonst *,int,int));
E bool FDECL(attacktype, (struct permonst *,int));
E bool FDECL(poly_when_stoned, (struct permonst *));
E bool FDECL(resists_drli, (struct Monster *));
E bool FDECL(resists_magm, (struct Monster *));
E bool FDECL(resists_blnd, (struct Monster *));
E bool FDECL(can_blnd, (struct Monster *,struct Monster *,uchar,struct Object *));
E bool FDECL(ranged_attk, (struct permonst *));
E bool FDECL(hates_silver, (struct permonst *));
E bool FDECL(passes_bars, (struct permonst *));
E bool FDECL(can_track, (struct permonst *));
E bool FDECL(breakarm, (struct permonst *));
E bool FDECL(sliparm, (struct permonst *));
E bool FDECL(sticks, (struct permonst *));
E int FDECL(num_horns, (struct permonst *));
/* E bool FDECL(canseemon, (struct monst *)); */
E struct Attack *FDECL(dmgtype_fromattack, (struct permonst *,int,int));
E bool FDECL(dmgtype, (struct permonst *,int));
E int FDECL(max_passive_dmg, (struct Monster *,struct Monster *));
E int FDECL(monsndx, (struct permonst *));
E int FDECL(name_to_mon, (const char *));
E int FDECL(gender, (struct Monster *));
E int FDECL(pronoun_gender, (struct Monster *));
E bool FDECL(levl_follower, (struct Monster *));
E int FDECL(little_to_big, (int));
E int FDECL(big_to_little, (int));
E const char *FDECL(locomotion, (const struct permonst *,const char *));
E const char *FDECL(stagger, (const struct permonst *,const char *));
E const char *FDECL(on_fire, (struct permonst *,struct Attack *));
E const struct permonst *FDECL(raceptr, (struct Monster *));

/* ### monmove.c ### */

E bool FDECL(itsstuck, (struct Monster *));
E bool FDECL(mb_trapped, (struct Monster *));
E void FDECL(mon_regen, (struct Monster *,bool));
E int FDECL(dochugw, (struct Monster *));
E bool FDECL(onscary, (int,int,struct Monster *));
E void FDECL(monflee, (struct Monster *, int, bool, bool));
E int FDECL(dochug, (struct Monster *));
E int FDECL(m_move, (struct Monster *,int));
E bool FDECL(closed_door, (int,int));
E bool FDECL(accessible, (int,int));
E void FDECL(set_apparxy, (struct Monster *));
E bool FDECL(can_ooze, (struct Monster *));

/* ### monst.c ### */

E void monst_init();

/* ### monstr.c ### */

E void monstr_init();

/* ### mplayer.c ### */

E struct Monster *FDECL(mk_mplayer, (struct permonst *,xchar,
				   xchar,bool));
E void FDECL(create_mplayers, (int,bool));
E void FDECL(mplayer_talk, (struct Monster *));

#if defined(MICRO) || defined(WIN32)

/* ### msdos.c,os2.c,tos.c,winnt.c ### */

#  ifndef WIN32
E int tgetch();
#  endif
#  ifndef TOS
E char switchar();
#  endif
# ifndef __GO32__
E long FDECL(freediskspace, (char *));
E int FDECL(findfirst, (char *));
E int findnext();
E long FDECL(filesize, (char *));
E char *foundfile_buffer();
# endif /* __GO32__ */
E void FDECL(chdrive, (char *));
# ifndef TOS
E void disable_ctrlP();
E void enable_ctrlP();
# endif
# if defined(MICRO) && !defined(WINNT)
E void get_scr_size();
#  ifndef TOS
E void FDECL(gotoxy, (int,int));
#  endif
# endif
# ifdef TOS
E int FDECL(_copyfile, (char *,char *));
E int kbhit();
E void set_colors();
E void restore_colors();
#  ifdef SUSPEND
E int dosuspend();
#  endif
# endif /* TOS */
# ifdef WIN32
E char *FDECL(get_username, (int *));
E void FDECL(nt_regularize, (char *));
E int (*nt_kbhit)();
E void FDECL(Delay, (int));
# endif /* WIN32 */
#endif /* MICRO || WIN32 */

/* ### mthrowu.c ### */

E int FDECL(thitu, (int,int,struct Object *,const char *));
E int FDECL(ohitmon, (struct Monster *,struct Object *,int,bool));
E void FDECL(thrwmu, (struct Monster *));
E int FDECL(spitmu, (struct Monster *,struct Attack *));
E int FDECL(breamu, (struct Monster *,struct Attack *));
E bool FDECL(linedup, (xchar,xchar,xchar,xchar));
E bool FDECL(lined_up, (struct Monster *));
E struct Object *FDECL(m_carrying, (struct Monster *,int));
E void FDECL(m_useup, (struct Monster *,struct Object *));
E void FDECL(m_throw, (struct Monster *,int,int,int,int,int,struct Object *));
E bool FDECL(hits_bars, (struct Object **,int,int,int,int));

/* ### muse.c ### */

E bool FDECL(find_defensive, (struct Monster *));
E int FDECL(use_defensive, (struct Monster *));
E int FDECL(rnd_defensive_item, (struct Monster *));
E bool FDECL(find_offensive, (struct Monster *));
#ifdef USE_TRAMPOLI
E int FDECL(mbhitm, (struct Monster *,struct Object *));
#endif
E int FDECL(use_offensive, (struct Monster *));
E int FDECL(rnd_offensive_item, (struct Monster *));
E bool FDECL(find_misc, (struct Monster *));
E int FDECL(use_misc, (struct Monster *));
E int FDECL(rnd_misc_item, (struct Monster *));
E bool FDECL(searches_for_item, (struct Monster *,struct Object *));
E bool FDECL(mon_reflects, (struct Monster *,const char *));
E bool FDECL(ureflects, (const char *,const char *));
E bool FDECL(munstone, (struct Monster *,bool));

/* ### music.c ### */

E void awaken_soldiers();
E int FDECL(do_play_instrument, (struct Object *));

/* ### nhlan.c ### */
#ifdef LAN_FEATURES
E void init_lan_features();
E char *lan_username();
# ifdef LAN_MAIL
E bool lan_mail_check();
E void FDECL(lan_mail_read, (struct Object *));
E void lan_mail_init();
E void lan_mail_finish();
E void lan_mail_terminate();
# endif
#endif

/* ### nttty.c ### */

#ifdef WIN32CON
E void get_scr_size();
E int nttty_kbhit();
E void nttty_open();
E void nttty_rubout();
E int tgetch();
E int FDECL(ntposkey,(int *, int *, int *));
E void FDECL(set_output_mode, (int));
E void synch_cursor();
#endif

/* ### o_init.c ### */

E void init_objects();
E int find_skates();
E void oinit();
E void FDECL(savenames, (int,int));
E void FDECL(restnames, (int));
E void FDECL(discover_object, (int,bool,bool));
E void FDECL(undiscover_object, (int));
E int dodiscovered();

/* ### objects.c ### */

E void objects_init();

/* ### objnam.c ### */

E char *FDECL(obj_typename, (int));
E char *FDECL(simple_typename, (int));
E bool FDECL(obj_is_pname, (struct Object *));
E char *FDECL(distant_name, (struct Object *,char *(*)(Object*)));
E char *FDECL(fruitname, (bool));
E char *FDECL(xname, (struct Object *));
E char *FDECL(mshot_xname, (struct Object *));
E bool FDECL(the_unique_obj, (struct Object *obj));
E char *FDECL(doname, (struct Object *));
E bool FDECL(not_fully_identified, (struct Object *));
E char *FDECL(corpse_xname, (struct Object *,bool));
E char *FDECL(cxname, (struct Object *));
#ifdef SORTLOOT
E char *FDECL(cxname2, (struct Object *));
#endif
E char *FDECL(killer_xname, (struct Object *));
E const char *FDECL(singular, (struct Object *,char *(*)(Object*)));
E char *FDECL(an, (const char *));
E char *FDECL(An, (const char *));
E char *FDECL(The, (const char *));
E char *FDECL(the, (const char *));
E char *FDECL(aobjnam, (struct Object *,const char *));
E char *FDECL(Tobjnam, (struct Object *,const char *));
E char *FDECL(otense, (struct Object *,const char *));
E char *FDECL(vtense, (const char *,const char *));
E char *FDECL(Doname2, (struct Object *));
E char *FDECL(yname, (struct Object *));
E char *FDECL(Yname2, (struct Object *));
E char *FDECL(ysimple_name, (struct Object *));
E char *FDECL(Ysimple_name2, (struct Object *));
E char *FDECL(makeplural, (const char *));
E char *FDECL(makesingular, (const char *));
E struct Object *FDECL(readobjnam, (char *,struct Object *,bool));
E int FDECL(rnd_class, (int,int));
E const char *FDECL(cloak_simple_name, (struct Object *));
E const char *FDECL(mimic_obj_name, (struct Monster *));

/* ### options.c ### */

E bool FDECL(match_optname, (const char *,const char *,int,bool));
E void initoptions();
E void FDECL(parseoptions, (char *,bool,bool));
E int doset();
E int dotogglepickup();
E void option_help();
E void FDECL(next_opt, (winid,const char *));
E int FDECL(fruitadd, (char *));
E int FDECL(choose_classes_menu, (const char *,int,bool,char *,char *));
E void FDECL(add_menu_cmd_alias, (char, char));
E char FDECL(map_menu_cmd, (char));
E void FDECL(assign_warnings, (uchar *));
E char *FDECL(nh_getenv, (const char *));
E void FDECL(set_duplicate_opt_detection, (int));
E void FDECL(set_wc_option_mod_status, (unsigned long, int));
E void FDECL(set_wc2_option_mod_status, (unsigned long, int));
E void FDECL(set_option_mod_status, (const char *,int));
#ifdef AUTOPICKUP_EXCEPTIONS
E int FDECL(add_autopickup_exception, (const char *));
E void free_autopickup_exceptions();
#endif /* AUTOPICKUP_EXCEPTIONS */
#ifdef MENU_COLOR
E bool FDECL(add_menu_coloring, (char *));
#endif /* MENU_COLOR */

/* ### pager.c ### */

E int dowhatis();
E int doquickwhatis();
E int doidtrap();
E int dowhatdoes();
E char *FDECL(dowhatdoes_core,(char, char *));
E int dohelp();
E int dohistory();

/* ### pcmain.c ### */

#if defined(MICRO) || defined(WIN32)
# ifdef CHDIR
E void FDECL(chdirx, (char *,bool));
# endif /* CHDIR */
#endif /* MICRO || WIN32 */

/* ### pcsys.c ### */

#if defined(MICRO) || defined(WIN32)
E void flushout();
E int dosh();
# ifdef MFLOPPY
E void FDECL(eraseall, (const char *,const char *));
E void FDECL(copybones, (int));
E void playwoRAMdisk();
E int FDECL(saveDiskPrompt, (int));
E void gameDiskPrompt();
# endif
E void FDECL(append_slash, (char *));
E void FDECL(getreturn, (const char *));
# ifndef AMIGA
E void VDECL(msmsg, (const char *,...));
# endif
E FILE *FDECL(fopenp, (const char *,const char *));
#endif /* MICRO || WIN32 */

/* ### pctty.c ### */

#if defined(MICRO) || defined(WIN32)
E void gettty();
E void FDECL(settty, (const char *));
E void setftty();
E void VDECL(error, (const char *,...));
#if defined(TIMED_DELAY) && defined(_MSC_VER)
E void FDECL(msleep, (unsigned));
#endif
#endif /* MICRO || WIN32 */

/* ### pcunix.c ### */

#if defined(MICRO)
E void FDECL(regularize, (char *));
#endif /* MICRO */
#if defined(PC_LOCKING)
E void getlock();
#endif

/* ### pickup.c ### */

#ifdef GOLDOBJ
E int FDECL(collect_obj_classes,
	(char *,struct Object *,bool,bool FDECL((*),(Object*)), int *));
#else
E int FDECL(collect_obj_classes,
	(char *,struct Object *,bool,bool,bool FDECL((*),(Object*)), int *));
#endif
E void FDECL(add_valid_menu_class, (int));
E bool FDECL(allow_all, (struct Object *));
E bool FDECL(allow_category, (struct Object *));
E bool FDECL(is_worn_by_type, (struct Object *));
#ifdef USE_TRAMPOLI
E int FDECL(ck_bag, (struct Object *));
E int FDECL(in_container, (struct Object *));
E int FDECL(out_container, (struct Object *));
#endif
E int FDECL(pickup, (int));
E int FDECL(pickup_object, (struct Object *, long, bool));
E int FDECL(query_category, (const char *, struct Object *, int,
				menu_item **, int));
E int FDECL(query_objlist, (const char *, struct Object *, int,
				menu_item **, int, bool (*)(Object*)));
E struct Object *FDECL(pick_obj, (struct Object *));
E int encumber_msg();
E int doloot();
E int FDECL(use_container, (struct Object *,int));
E int FDECL(loot_mon, (struct Monster *,int *,bool *));
E const char *FDECL(safe_qbuf, (const char *,unsigned,
				const char *,const char *,const char *));
E bool FDECL(is_autopickup_exception, (struct Object *, bool));

/* ### pline.c ### */

E void FDECL(msgpline_add, (int, char *));
E void msgpline_free();
E void VDECL(pline, (const char *,...)) PRINTF_F(1,2);
E void VDECL(Norep, (const char *,...)) PRINTF_F(1,2);
E void free_youbuf();
E void VDECL(You, (const char *,...)) PRINTF_F(1,2);
E void VDECL(Your, (const char *,...)) PRINTF_F(1,2);
E void VDECL(You_feel, (const char *,...)) PRINTF_F(1,2);
E void VDECL(You_cant, (const char *,...)) PRINTF_F(1,2);
E void VDECL(You_hear, (const char *,...)) PRINTF_F(1,2);
E void VDECL(pline_The, (const char *,...)) PRINTF_F(1,2);
E void VDECL(There, (const char *,...)) PRINTF_F(1,2);
E void VDECL(verbalize, (const char *,...)) PRINTF_F(1,2);
E void VDECL(raw_printf, (const char *,...)) PRINTF_F(1,2);
E void VDECL(impossible, (const char *,...)) PRINTF_F(1,2);
E const char *FDECL(align_str, (ALIGNTYP_P));
E void FDECL(mstatusline, (struct Monster *));
E void ustatusline();
E void self_invis_message();

/* ### polyself.c ### */

E void set_uasmon();
E void change_sex();
E void FDECL(polyself, (bool));
E int FDECL(polymon, (int));
E void rehumanize();
E int dobreathe();
E int dospit();
E int doremove();
E int dospinweb();
E int dosummon();
E int dogaze();
E int dohide();
E int domindblast();
E void FDECL(skinback, (bool));
E const char *FDECL(mbodypart, (struct Monster *,int));
E const char *FDECL(body_part, (int));
E int poly_gender();
E void FDECL(ugolemeffects, (int,int));

/* ### potion.c ### */

E void FDECL(set_itimeout, (long *,long));
E void FDECL(incr_itimeout, (long *,int));
E void FDECL(make_confused, (long,bool));
E void FDECL(make_stunned, (long,bool));
E void FDECL(make_blinded, (long,bool));
E void FDECL(make_sick, (long, const char *, bool,int));
E void FDECL(make_vomiting, (long,bool));
E bool FDECL(make_hallucinated, (long,bool,long));
E int dodrink();
E int FDECL(dopotion, (struct Object *));
E int FDECL(peffects, (struct Object *));
E void FDECL(healup, (int,int,bool,bool));
E void FDECL(strange_feeling, (struct Object *,const char *));
E void FDECL(potionhit, (struct Monster *,struct Object *,bool));
E void FDECL(potionbreathe, (struct Object *));
E bool FDECL(get_wet, (struct Object *));
E int dodip();
E void FDECL(djinni_from_bottle, (struct Object *));
E struct Monster *FDECL(split_mon, (struct Monster *,struct Monster *));
E const char *bottlename();

/* ### pray.c ### */

#ifdef USE_TRAMPOLI
E int prayer_done();
#endif
E int dosacrifice();
E bool FDECL(can_pray, (bool));
E int dopray();
E const char *u_gname();
E int doturn();
E const char *a_gname();
E const char *FDECL(a_gname_at, (xchar x,xchar y));
E const char *FDECL(align_gname, (ALIGNTYP_P));
E const char *FDECL(halu_gname, (ALIGNTYP_P));
E const char *FDECL(align_gtitle, (ALIGNTYP_P));
E void FDECL(altar_wrath, (int,int));


/* ### priest.c ### */

E int FDECL(move_special, (struct Monster *,bool,schar,bool,bool,
			   xchar,xchar,xchar,xchar));
E char FDECL(temple_occupied, (char *));
E int FDECL(pri_move, (struct Monster *));
E void FDECL(priestini, (d_level *,struct mkroom *,int,int,bool));
E char *FDECL(priestname, (struct Monster *,char *));
E bool FDECL(p_coaligned, (struct Monster *));
E struct Monster *FDECL(findpriest, (char));
E void FDECL(intemple, (int));
E void FDECL(priest_talk, (struct Monster *));
E struct Monster *FDECL(mk_roamer, (struct permonst *,ALIGNTYP_P,
				  xchar,xchar,bool));
E void FDECL(reset_hostility, (struct Monster *));
E bool FDECL(in_your_sanctuary, (struct Monster *,xchar,xchar));
E void FDECL(ghod_hitsu, (struct Monster *));
E void angry_priest();
E void clearpriests();
E void FDECL(restpriest, (struct Monster *,bool));

/* ### quest.c ### */

E void onquest();
E void nemdead();
E void artitouch();
E bool ok_to_quest();
E void FDECL(leader_speaks, (struct Monster *));
E void nemesis_speaks();
E void FDECL(quest_chat, (struct Monster *));
E void FDECL(quest_talk, (struct Monster *));
E void FDECL(quest_stat_check, (struct Monster *));
E void FDECL(finish_quest, (struct Object *));

/* ### questpgr.c ### */

E void load_qtlist();
E void unload_qtlist();
E short FDECL(quest_info, (int));
E const char *ldrname();
E bool FDECL(is_quest_artifact, (struct Object*));
E void FDECL(com_pager, (int));
E void FDECL(qt_pager, (int));
E struct permonst *qt_montype();

/* ### random.c ### */

#if defined(RANDOM) && !defined(__GO32__) /* djgpp has its own random */
E void FDECL(srandom, (unsigned));
E char *FDECL(initstate, (unsigned,char *,int));
E char *FDECL(setstate, (char *));
E long random();
#endif /* RANDOM */

/* ### read.c ### */

E int doread();
E bool FDECL(is_chargeable, (struct Object *));
E void FDECL(recharge, (struct Object *,int));
E void FDECL(forget_objects, (int));
E void FDECL(forget_levels, (int));
E void forget_traps();
E void FDECL(forget_map, (int));
E int FDECL(seffects, (struct Object *));
#ifdef USE_TRAMPOLI
E void FDECL(set_lit, (int,int,genericptr_t));
#endif
E void FDECL(litroom, (bool,struct Object *));
E void FDECL(do_genocide, (int));
E void FDECL(punish, (struct Object *));
E void unpunish();
E bool FDECL(cant_create, (int *, bool));
#ifdef WIZARD
E bool create_particular();
#endif

/* ### rect.c ### */

E void init_rect();
E NhRect *FDECL(get_rect, (NhRect *));
E NhRect *rnd_rect();
E void FDECL(remove_rect, (NhRect *));
E void FDECL(add_rect, (NhRect *));
E void FDECL(split_rects, (NhRect *,NhRect *));

/* ## region.c ### */
E void clear_regions();
E void run_regions();
E bool FDECL(in_out_region, (xchar,xchar));
E bool FDECL(m_in_out_region, (struct Monster *,xchar,xchar));
E void update_player_regions();
E void FDECL(update_monster_region, (struct Monster *));
E NhRegion *FDECL(visible_region_at, (xchar,xchar));
E void FDECL(show_region, (NhRegion*, xchar, xchar));
E void FDECL(save_regions, (int,int));
E void FDECL(rest_regions, (int,bool));
E NhRegion* FDECL(create_gas_cloud, (xchar, xchar, int, int));

/* ### restore.c ### */

E void FDECL(inven_inuse, (bool));
E int FDECL(dorecover, (int));
E void FDECL(trickery, (char *));
E void FDECL(getlev, (int,int,xchar,bool));
E void minit();
E bool FDECL(lookup_id_mapping, (unsigned long, unsigned long*));
#ifdef ZEROCOMP
E int FDECL(mread, (int,genericptr_t,unsigned int));
#else
E void FDECL(mread, (int,genericptr_t,unsigned int));
#endif

/* ### rip.c ### */

E void FDECL(genl_outrip, (winid,int));

/* ### rnd.c ### */

E void check_reseed();
E int FDECL(rn2, (int));
E int FDECL(rnl, (int));
E int FDECL(rnd, (int));
E int FDECL(d, (int,int));
E int FDECL(rne, (int));
E int FDECL(rnz, (int));

/* ### role.c ### */

E bool FDECL(validrole, (int));
E bool FDECL(validrace, (int, int));
E bool FDECL(validgend, (int, int, int));
E bool FDECL(validalign, (int, int, int));
E int randrole();
E int FDECL(randrace, (int));
E int FDECL(randgend, (int, int));
E int FDECL(randalign, (int, int));
E int FDECL(str2role, (char *));
E int FDECL(str2race, (char *));
E int FDECL(str2gend, (char *));
E int FDECL(str2align, (char *));
E bool FDECL(ok_role, (int, int, int, int));
E int FDECL(pick_role, (int, int, int, int));
E bool FDECL(ok_race, (int, int, int, int));
E int FDECL(pick_race, (int, int, int, int));
E bool FDECL(ok_gend, (int, int, int, int));
E int FDECL(pick_gend, (int, int, int, int));
E bool FDECL(ok_align, (int, int, int, int));
E int FDECL(pick_align, (int, int, int, int));
E void role_init();
E void rigid_role_checks();
E void plnamesuffix();
E const char *FDECL(Hello, (struct Monster *));
E const char *Goodbye();
E char *FDECL(build_plselection_prompt, (char *, int, int, int, int, int));
E char *FDECL(root_plselection_prompt, (char *, int, int, int, int, int));

/* ### rumors.c ### */

E char *FDECL(getrumor, (int,char *, bool));
E void FDECL(outrumor, (int,int));
E void FDECL(outoracle, (bool, bool));
E void FDECL(save_oracles, (int,int));
E void FDECL(restore_oracles, (int));
E int FDECL(doconsult, (struct Monster *));

/* ### save.c ### */

E int dosave();
#if defined(UNIX) || defined(VMS) || defined(__EMX__) || defined(WIN32)
E void FDECL(hangup, (int));
#endif
E int dosave0();
#ifdef INSURANCE
E void savestateinlock();
#endif
#ifdef MFLOPPY
E bool FDECL(savelev, (int,xchar,int));
E bool FDECL(swapin_file, (int));
E void co_false();
#else
E void FDECL(savelev, (int,xchar,int));
#endif
E void FDECL(bufon, (int));
E void FDECL(bufoff, (int));
E void FDECL(bflush, (int));
E void FDECL(bwrite, (int,genericptr_t,unsigned int));
E void FDECL(bclose, (int));
E void FDECL(savefruitchn, (int,int));
E void free_dungeons();
E void freedynamicdata();

/* ### shk.c ### */

#ifdef GOLDOBJ
E long FDECL(money2mon, (struct Monster *, long));
E void FDECL(money2u, (struct Monster *, long));
#endif
E char *FDECL(shkname, (struct Monster *));
E void FDECL(shkgone, (struct Monster *));
E void FDECL(set_residency, (struct Monster *,bool));
E void FDECL(replshk, (struct Monster *,struct Monster *));
E void FDECL(restshk, (struct Monster *,bool));
E char FDECL(inside_shop, (xchar,xchar));
E void FDECL(u_left_shop, (char *,bool));
E void FDECL(remote_burglary, (xchar,xchar));
E void FDECL(u_entered_shop, (char *));
E bool FDECL(same_price, (struct Object *,struct Object *));
E void shopper_financial_report();
E int FDECL(inhishop, (struct Monster *));
E struct Monster *FDECL(shop_keeper, (char));
E bool FDECL(tended_shop, (struct mkroom *));
E void FDECL(delete_contents, (struct Object *));
E void FDECL(obfree, (struct Object *,struct Object *));
E void FDECL(home_shk, (struct Monster *,bool));
E void FDECL(make_happy_shk, (struct Monster *,bool));
E void FDECL(hot_pursuit, (struct Monster *));
E void FDECL(make_angry_shk, (struct Monster *,xchar,xchar));
E int dopay();
E bool FDECL(paybill, (int));
E void finish_paybill();
E struct Object *FDECL(find_oid, (unsigned));
E long FDECL(contained_cost, (struct Object *,struct Monster *,long,bool, bool));
E long FDECL(contained_gold, (struct Object *));
E void FDECL(picked_container, (struct Object *));
E long FDECL(unpaid_cost, (struct Object *));
E void FDECL(addtobill, (struct Object *,bool,bool,bool));
E void FDECL(splitbill, (struct Object *,struct Object *));
E void FDECL(subfrombill, (struct Object *,struct Monster *));
E long FDECL(stolen_value, (struct Object *,xchar,xchar,bool,bool));
E void FDECL(sellobj_state, (int));
E void FDECL(sellobj, (struct Object *,xchar,xchar));
E int FDECL(doinvbill, (int));
E struct Monster *FDECL(shkcatch, (struct Object *,xchar,xchar));
E void FDECL(add_damage, (xchar,xchar,long));
E int FDECL(repair_damage, (struct Monster *,struct damage *,bool));
E int FDECL(shk_move, (struct Monster *));
E void FDECL(after_shk_move, (struct Monster *));
E bool FDECL(is_fshk, (struct Monster *));
E void FDECL(shopdig, (int));
E void FDECL(pay_for_damage, (const char *,bool));
E bool FDECL(costly_spot, (xchar,xchar));
E struct Object *FDECL(shop_object, (xchar,xchar));
E void FDECL(price_quote, (struct Object *));
E void FDECL(shk_chat, (struct Monster *));
E void FDECL(check_unpaid_usage, (struct Object *,bool));
E void FDECL(check_unpaid, (struct Object *));
E void FDECL(costly_gold, (xchar,xchar,long));
E bool FDECL(block_door, (xchar,xchar));
E bool FDECL(block_entry, (xchar,xchar));
E char *FDECL(shk_your, (char *,struct Object *));
E char *FDECL(Shk_Your, (char *,struct Object *));

/* ### shknam.c ### */

E void FDECL(stock_room, (int,struct mkroom *));
E bool FDECL(saleable, (struct Monster *,struct Object *));
E int FDECL(get_shop_item, (int));

/* ### sit.c ### */

E void take_gold();
E int dosit();
E void rndcurse();
E void attrcurse();

/* ### sounds.c ### */

E void dosounds();
E const char *FDECL(growl_sound, (struct Monster *));
E void FDECL(growl, (struct Monster *));
E void FDECL(yelp, (struct Monster *));
E void FDECL(whimper, (struct Monster *));
E void FDECL(beg, (struct Monster *));
E int dotalk();
#ifdef USER_SOUNDS
E int FDECL(add_sound_mapping, (const char *));
E void FDECL(play_sound_for_message, (const char *));
#endif

/* ### sp_lev.c ### */

E bool FDECL(check_room, (xchar *,xchar *,xchar *,xchar *,bool));
E bool FDECL(create_room, (xchar,xchar,xchar,xchar,
			      xchar,xchar,xchar,xchar));
E void FDECL(create_secret_door, (struct mkroom *,xchar));
E bool FDECL(dig_corridor, (coord *,coord *,bool,schar,schar));
E void FDECL(fill_room, (struct mkroom *,bool));
E bool FDECL(load_special, (const char *));

/* ### spell.c ### */

#ifdef USE_TRAMPOLI
E int learn();
#endif
E int FDECL(study_book, (struct Object *));
E void FDECL(book_disappears, (struct Object *));
E void FDECL(book_substitution, (struct Object *,struct Object *));
E void age_spells();
E int docast();
E int FDECL(spell_skilltype, (int));
E int FDECL(spelleffects, (int,bool));
E void losespells();
E int dovspell();
E void FDECL(initialspell, (struct Object *));

/* ### steal.c ### */

#ifdef USE_TRAMPOLI
E int stealarm();
#endif
#ifdef GOLDOBJ
E long FDECL(somegold, (long));
#else
E long somegold();
#endif
E void FDECL(stealgold, (struct Monster *));
E void FDECL(remove_worn_item, (struct Object *,bool));
E int FDECL(steal, (struct Monster *, char *));
E int FDECL(mpickobj, (struct Monster *,struct Object *));
E void FDECL(stealamulet, (struct Monster *));
E void FDECL(mdrop_special_objs, (struct Monster *));
E void FDECL(relobj, (struct Monster *,int,bool));
#ifdef GOLDOBJ
E struct Object *FDECL(findgold, (struct Object *));
#endif

/* ### steed.c ### */

#ifdef STEED
E void rider_cant_reach();
E bool FDECL(can_saddle, (struct Monster *));
E int FDECL(use_saddle, (struct Object *));
E bool FDECL(can_ride, (struct Monster *));
E int doride();
E bool FDECL(mount_steed, (struct Monster *, bool));
E void exercise_steed();
E void kick_steed();
E void FDECL(dismount_steed, (int));
E void FDECL(place_monster, (struct Monster *,int,int));
#endif

/* ### teleport.c ### */

E bool FDECL(goodpos, (int,int,struct Monster *,unsigned));
E bool FDECL(enexto, (coord *,xchar,xchar,struct permonst *));
E bool FDECL(enexto_core, (coord *,xchar,xchar,struct permonst *,unsigned));
E void FDECL(teleds, (int,int,bool));
E bool FDECL(safe_teleds, (bool));
E bool FDECL(teleport_pet, (struct Monster *,bool));
E void tele();
E int dotele();
E void level_tele();
E void FDECL(domagicportal, (struct trap *));
E void FDECL(tele_trap, (struct trap *));
E void FDECL(level_tele_trap, (struct trap *));
E void FDECL(rloc_to, (struct Monster *,int,int));
E bool FDECL(rloc, (struct Monster *, bool));
E bool FDECL(tele_restrict, (struct Monster *));
E void FDECL(mtele_trap, (struct Monster *, struct trap *,int));
E int FDECL(mlevel_tele_trap, (struct Monster *, struct trap *,bool,int));
E void FDECL(rloco, (struct Object *));
E int random_teleport_level();
E bool FDECL(u_teleport_mon, (struct Monster *,bool));

/* ### tile.c ### */
#ifdef USE_TILES
E void FDECL(substitute_tiles, (d_level *));
#endif

/* ### timeout.c ### */

E void burn_away_slime();
E void nh_timeout();
E void FDECL(fall_asleep, (int, bool));
E void FDECL(attach_egg_hatch_timeout, (struct Object *));
E void FDECL(attach_fig_transform_timeout, (struct Object *));
E void FDECL(kill_egg, (struct Object *));
E void FDECL(hatch_egg, (genericptr_t, long));
E void FDECL(learn_egg_type, (int));
E void FDECL(burn_object, (genericptr_t, long));
E void FDECL(begin_burn, (struct Object *, bool));
E void FDECL(end_burn, (struct Object *, bool));
E void do_storms();
E bool FDECL(start_timer, (long, short, short, genericptr_t));
E long FDECL(stop_timer, (short, genericptr_t));
E void run_timers();
E void FDECL(obj_move_timers, (struct Object *, struct Object *));
E void FDECL(obj_split_timers, (struct Object *, struct Object *));
E void FDECL(obj_stop_timers, (struct Object *));
E bool FDECL(obj_is_local, (struct Object *));
E void FDECL(save_timers, (int,int,int));
E void FDECL(restore_timers, (int,int,bool,long));
E void FDECL(relink_timers, (bool));
#ifdef WIZARD
E int wiz_timeout_queue();
E void timer_sanity_check();
#endif

/* ### topten.c ### */

E void FDECL(topten, (int));
E void FDECL(prscore, (int,char **));
E struct Object *FDECL(tt_oname, (struct Object *));

/* ### track.c ### */

E void initrack();
E void settrack();
E coord *FDECL(gettrack, (int,int));

/* ### trap.c ### */

E bool FDECL(burnarmor,(struct Monster *));
E bool FDECL(rust_dmg, (struct Object *,const char *,int,bool,struct Monster *));
E void FDECL(grease_protect, (struct Object *,const char *,struct Monster *));
E struct trap *FDECL(maketrap, (int,int,int));
E void FDECL(fall_through, (bool));
E struct Monster *FDECL(animate_statue, (struct Object *,xchar,xchar,int,int *));
E struct Monster *FDECL(activate_statue_trap,
			(struct trap *,xchar,xchar,bool));
E void FDECL(dotrap, (struct trap *, unsigned));
E void FDECL(seetrap, (struct trap *));
E int FDECL(mintrap, (struct Monster *));
E void FDECL(instapetrify, (const char *));
E void FDECL(minstapetrify, (struct Monster *,bool));
E void FDECL(selftouch, (const char *));
E void FDECL(mselftouch, (struct Monster *,const char *,bool));
E void float_up();
E void FDECL(fill_pit, (int,int));
E int FDECL(float_down, (long, long));
E int FDECL(fire_damage, (struct Object *,bool,bool,xchar,xchar));
E bool FDECL(water_damage, (struct Object *,bool,bool));
E bool drown();
E void FDECL(drain_en, (int));
E int dountrap();
E int FDECL(untrap, (bool));
E bool FDECL(chest_trap, (struct Object *,int,bool));
E void FDECL(deltrap, (struct trap *));
E bool FDECL(delfloortrap, (struct trap *));
E struct trap *FDECL(t_at, (int,int));
E void FDECL(b_trapped, (const char *,int));
E bool unconscious();
E bool lava_effects();
E void FDECL(blow_up_landmine, (struct trap *));
E int FDECL(launch_obj,(short,int,int,int,int,int));

/* ### u_init.c ### */

E void u_init();

/* ### uhitm.c ### */

E void FDECL(hurtmarmor,(struct Monster *,int));
E bool FDECL(attack_checks, (struct Monster *,struct Object *));
E void FDECL(check_caitiff, (struct Monster *));
E schar FDECL(find_roll_to_hit, (struct Monster *));
E bool FDECL(attack, (struct Monster *));
E bool FDECL(hmon, (struct Monster *,struct Object *,int));
E int FDECL(damageum, (struct Monster *,struct Attack *));
E void FDECL(missum, (struct Monster *,struct Attack *));
E int FDECL(passive, (struct Monster *,bool,int,uchar));
E void FDECL(passive_obj, (struct Monster *,struct Object *,struct Attack *));
E void FDECL(stumble_onto_mimic, (struct Monster *));
E int FDECL(flash_hits_mon, (struct Monster *,struct Object *));

/* ### unixmain.c ### */

#ifdef UNIX
# ifdef PORT_HELP
E void port_help();
# endif
#endif /* UNIX */


/* ### unixtty.c ### */

#if defined(UNIX) || defined(__BEOS__)
E void gettty();
E void FDECL(settty, (const char *));
E void setftty();
E void intron();
E void introff();
E void VDECL(error, (const char *,...)) PRINTF_F(1,2);
#endif /* UNIX || __BEOS__ */

/* ### unixunix.c ### */

#ifdef UNIX
E void getlock();
E void FDECL(regularize, (char *));
# if defined(TIMED_DELAY) && !defined(msleep) && defined(SYSV)
E void FDECL(msleep, (unsigned));
# endif
# ifdef SHELL
E int dosh();
# endif /* SHELL */
# if defined(SHELL) || defined(DEF_PAGER) || defined(DEF_MAILREADER)
E int FDECL(child, (int));
# endif
#endif /* UNIX */

/* ### unixres.c ### */

#ifdef UNIX
# ifdef GNOME_GRAPHICS 
E int FDECL(hide_privileges, (bool));
# endif
#endif /* UNIX */

/* ### vault.c ### */

E bool FDECL(grddead, (struct Monster *));
E char FDECL(vault_occupied, (char *));
E void invault();
E int FDECL(gd_move, (struct Monster *));
E void paygd();
E long hidden_gold();
E bool gd_sound();

/* ### version.c ### */

E char *FDECL(version_string, (char *));
E char *FDECL(getversionstring, (char *));
E int doversion();
E int doextversion();
#ifdef MICRO
E bool FDECL(comp_times, (long));
#endif
E bool FDECL(check_version, (struct version_info *,
				const char *,bool));
E unsigned long FDECL(get_feature_notice_ver, (char *));
E unsigned long get_current_feature_ver();
#ifdef RUNTIME_PORT_ID
E void FDECL(append_port_id, (char *));
#endif

/* ### video.c ### */

#ifdef VIDEOSHADES
E int FDECL(assign_videoshades, (char *));
E int FDECL(assign_videocolors, (char *));
#endif

/* ### vis_tab.c ### */

#ifdef VISION_TABLES
E void vis_tab_init();
#endif

/* ### vision.c ### */

E void vision_init();
E int FDECL(does_block, (int,int,struct rm*));
E void vision_reset();
E void FDECL(vision_recalc, (int));
E void FDECL(block_point, (int,int));
E void FDECL(unblock_point, (int,int));
E bool FDECL(clear_path, (int,int,int,int));
E void FDECL(do_clear_area, (int,int,int,
			     void (*)(int,int,genericptr_t),genericptr_t));

#ifdef VMS

/* ### vmsfiles.c ### */

E int FDECL(vms_link, (const char *,const char *));
E int FDECL(vms_unlink, (const char *));
E int FDECL(vms_creat, (const char *,unsigned int));
E int FDECL(vms_open, (const char *,int,unsigned int));
E bool FDECL(same_dir, (const char *,const char *));
E int FDECL(c__translate, (int));
E char *FDECL(vms_basename, (const char *));

/* ### vmsmail.c ### */

E unsigned long init_broadcast_trapping();
E unsigned long enable_broadcast_trapping();
E unsigned long disable_broadcast_trapping();
# if 0
E struct mail_info *parse_next_broadcast();
# endif /*0*/

/* ### vmsmain.c ### */

E int FDECL(main, (int, char **));
# ifdef CHDIR
E void FDECL(chdirx, (const char *,bool));
# endif /* CHDIR */

/* ### vmsmisc.c ### */

E void vms_abort();
E void FDECL(vms_exit, (int));

/* ### vmstty.c ### */

E int vms_getchar();
E void gettty();
E void FDECL(settty, (const char *));
E void FDECL(shuttty, (const char *));
E void setftty();
E void intron();
E void introff();
E void VDECL(error, (const char *,...)) PRINTF_F(1,2);
#ifdef TIMED_DELAY
E void FDECL(msleep, (unsigned));
#endif

/* ### vmsunix.c ### */

E void getlock();
E void FDECL(regularize, (char *));
E int vms_getuid();
E bool FDECL(file_is_stmlf, (int));
E int FDECL(vms_define, (const char *,const char *,int));
E int FDECL(vms_putenv, (const char *));
E char *verify_termcap();
# if defined(CHDIR) || defined(SHELL) || defined(SECURE)
E void privoff();
E void privon();
# endif
# ifdef SHELL
E int dosh();
# endif
# if defined(SHELL) || defined(MAIL)
E int FDECL(vms_doshell, (const char *,bool));
# endif
# ifdef SUSPEND
E int dosuspend();
# endif

#endif /* VMS */

/* ### weapon.c ### */

E int FDECL(hitval, (struct Object *,struct Monster *));
E int FDECL(dmgval, (struct Object *,struct Monster *));
E struct Object *FDECL(select_rwep, (struct Monster *));
E struct Object *FDECL(select_hwep, (struct Monster *));
E void FDECL(possibly_unwield, (struct Monster *,bool));
E int FDECL(mon_wield_item, (struct Monster *));
E int abon();
E int dbon();
E int enhance_weapon_skill();
#ifdef DUMP_LOG
E void dump_weapon_skill();
#endif
E void FDECL(unrestrict_weapon_skill, (int));
E void FDECL(use_skill, (int,int));
E void FDECL(add_weapon_skill, (int));
E void FDECL(lose_weapon_skill, (int));
E int FDECL(weapon_type, (struct Object *));
E int uwep_skill_type();
E int FDECL(weapon_hit_bonus, (struct Object *));
E int FDECL(weapon_dam_bonus, (struct Object *));
E void FDECL(skill_init, (const struct def_skill *));

/* ### were.c ### */

E void FDECL(were_change, (struct Monster *));
E void FDECL(new_were, (struct Monster *));
E int FDECL(were_summon, (struct permonst *,bool,int *,char *));
E void you_were();
E void FDECL(you_unwere, (bool));

/* ### wield.c ### */

E void FDECL(setuwep, (struct Object *));
E void FDECL(setuqwep, (struct Object *));
E void FDECL(setuswapwep, (struct Object *));
E int dowield();
E int doswapweapon();
E int dowieldquiver();
E bool FDECL(wield_tool, (struct Object *,const char *));
E int can_twoweapon();
E void drop_uswapwep();
E int dotwoweapon();
E void uwepgone();
E void uswapwepgone();
E void uqwepgone();
E void untwoweapon();
E void FDECL(erode_obj, (struct Object *,bool,bool));
E int FDECL(chwepon, (struct Object *,int));
E int FDECL(welded, (struct Object *));
E void FDECL(weldmsg, (struct Object *));
E void FDECL(setmnotwielded, (struct Monster *,struct Object *));

/* ### windows.c ### */

E void FDECL(choose_windows, (const char *));
E char FDECL(genl_message_menu, (char,int,const char *));
E void FDECL(genl_preference_update, (const char *));

/* ### wizard.c ### */

E void amulet();
E int FDECL(mon_has_amulet, (struct Monster *));
E int FDECL(mon_has_special, (struct Monster *));
E int FDECL(tactics, (struct Monster *));
E void aggravate();
E void clonewiz();
E int pick_nasty();
E int FDECL(nasty, (struct Monster*));
E void resurrect();
E void intervene();
E void wizdead();
E void FDECL(cuss, (struct Monster *));

/* ### worm.c ### */

E int get_wormno();
E void FDECL(initworm, (struct Monster *,int));
E void FDECL(worm_move, (struct Monster *));
E void FDECL(worm_nomove, (struct Monster *));
E void FDECL(wormgone, (struct Monster *));
E void FDECL(wormhitu, (struct Monster *));
E void FDECL(cutworm, (struct Monster *,xchar,xchar,struct Object *));
E void FDECL(see_wsegs, (struct Monster *));
E void FDECL(detect_wsegs, (struct Monster *,bool));
E void FDECL(save_worm, (int,int));
E void FDECL(rest_worm, (int));
E void FDECL(place_wsegs, (struct Monster *));
E void FDECL(remove_worm, (struct Monster *));
E void FDECL(place_worm_tail_randomly, (struct Monster *,xchar,xchar));
E int FDECL(count_wsegs, (struct Monster *));
E bool FDECL(worm_known, (struct Monster *));

/* ### worn.c ### */

E void FDECL(setworn, (struct Object *,long));
E void FDECL(setnotworn, (struct Object *));
E void FDECL(mon_set_minvis, (struct Monster *));
E void FDECL(mon_adjust_speed, (struct Monster *,int,struct Object *));
E void FDECL(update_mon_intrinsics,
		(struct Monster *,struct Object *,bool,bool));
E int FDECL(find_mac, (struct Monster *));
E void FDECL(m_dowear, (struct Monster *,bool));
E struct Object *FDECL(which_armor, (struct Monster *,long));
E void FDECL(mon_break_armor, (struct Monster *,bool));
E void FDECL(bypass_obj, (struct Object *));
E void clear_bypasses();
E int FDECL(racial_exception, (struct Monster *, struct Object *));

/* ### write.c ### */

E int FDECL(dowrite, (struct Object *));

/* ### zap.c ### */

E int FDECL(bhitm, (struct Monster *,struct Object *));
E void FDECL(probe_monster, (struct Monster *));
E bool FDECL(get_obj_location, (struct Object *,xchar *,xchar *,int));
E bool FDECL(get_mon_location, (struct Monster *,xchar *,xchar *,int));
E struct Monster *FDECL(get_container_location, (struct Object *obj, int *, int *));
E struct Monster *FDECL(montraits, (struct Object *,coord *));
E struct Monster *FDECL(revive, (struct Object *));
E int FDECL(unturn_dead, (struct Monster *));
E void FDECL(cancel_item, (struct Object *));
E bool FDECL(drain_item, (struct Object *));
E struct Object *FDECL(poly_obj, (struct Object *, int));
E bool FDECL(obj_resists, (struct Object *,int,int));
E bool FDECL(obj_shudders, (struct Object *));
E void FDECL(do_osshock, (struct Object *));
E int FDECL(bhito, (struct Object *,struct Object *));
E int FDECL(bhitpile, (struct Object *,int (*)(Object*,Object*),int,int));
E int FDECL(zappable, (struct Object *));
E void FDECL(zapnodir, (struct Object *));
E int dozap();
E int FDECL(zapyourself, (struct Object *,bool));
E bool FDECL(cancel_monst, (struct Monster *,struct Object *,
			       bool,bool,bool));
E void FDECL(weffects, (struct Object *));
E int spell_damage_bonus();
E const char *FDECL(exclam, (int force));
E void FDECL(hit, (const char *,struct Monster *,const char *));
E void FDECL(miss, (const char *,struct Monster *));
E struct Monster *FDECL(bhit, (int,int,int,int,int (*)(Monster*,Object*),
			     int (*)(Object*,Object*),struct Object *, bool *));
E struct Monster *FDECL(boomhit, (int,int));
E int FDECL(burn_floor_paper, (int,int,bool,bool));
E void FDECL(buzz, (int,int,xchar,xchar,int,int));
E void FDECL(melt_ice, (xchar,xchar));
E int FDECL(zap_over_floor, (xchar,xchar,int,bool *));
E void FDECL(fracture_rock, (struct Object *));
E bool FDECL(break_statue, (struct Object *));
E void FDECL(destroy_item, (int,int));
E int FDECL(destroy_mitem, (struct Monster *,int,int));
E int FDECL(resist, (struct Monster *,char,int,int));
E void makewish();

#endif /* !MAKEDEFS_C && !LEV_LEX_C */

#undef E

#endif /* EXTERN_H */
