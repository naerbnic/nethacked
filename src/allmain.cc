/*	SCCS Id: @(#)allmain.c	3.4	2003/04/02	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/* various code that was replicated in *main.c */
#include "allmain.h"

#include <signal.h>
#include "hack.h"
#include "unixtty.h"
#include "u_init.h"
#include "track.h"
#include "teleport.h"
#include "sounds.h"
#include "role.h"
#include "rnd.h"
#include "questpgr.h"
#include "polyself.h"
#include "pline.h"
#include "pickup.h"
#include "objects.h"
#include "o_init.h"
#include "monstr.h"
#include "mon.h"
#include "mkmaze.h"
#include "mklev.h"
#include "makemon.h"
#include "mail.h"
#include "hacklib.h"
#include "end.h"
#include "eat.h"
#include "dog.h"
#include "do_wear.h"
#include "do.h"
#include "detect.h"
#include "dbridge.h"
#include "cmd.h"

#include "apply.h"
#include "artifact.h"
#include "botl.h"

#ifdef POSITIONBAR
STATIC_DCL void do_positionbar();
#endif

#ifdef OVL0

void moveloop() {
  int moveamt = 0, wtcap = 0, change = 0;
  bool didmove = FALSE, monscanmove = FALSE;

  flags.moonphase = phase_of_the_moon();
  if (flags.moonphase == FULL_MOON) {
    You("are lucky!  Full moon tonight.");
    change_luck(1);
  } else if (flags.moonphase == NEW_MOON) {
    pline("Be careful!  New moon tonight.");
  }
  flags.friday13 = friday_13th();
  if (flags.friday13) {
    pline("Watch out!  Bad things can happen on Friday the 13th.");
    change_luck(-1);
  }

  initrack();

  /* Note:  these initializers don't do anything except guarantee that
          we're linked properly.
  */
  decl_init();
  monst_init();
  monstr_init(); /* monster strengths */
  objects_init();

#ifdef WIZARD
  if (wizard)
    add_debug_extended_commands();
#endif

  (void)encumber_msg(); /* in case they auto-picked up something */

  player.uz0.dlevel = player.uz.dlevel;
  youmonst.movement = NORMAL_SPEED; /* give the hero some movement points */

  for (;;) {
    get_nh_event();
#ifdef POSITIONBAR
    do_positionbar();
#endif

    didmove = flags.move;
    if (didmove) {
      /* actual time passed */
      youmonst.movement -= NORMAL_SPEED;

      do { /* hero can't move this turn loop */
        wtcap = encumber_msg();

        flags.mon_moving = TRUE;
        do {
          monscanmove = movemon();
          if (youmonst.movement > NORMAL_SPEED)
            break; /* it's now your turn */
        } while (monscanmove);
        flags.mon_moving = FALSE;

        if (!monscanmove && youmonst.movement < NORMAL_SPEED) {
          /* both you and the monsters are out of steam this round */
          /* set up for a new turn */
          Monster *mtmp;
          mcalcdistress(); /* adjust monsters' trap, blind, etc */

          /* reallocate movement rations to monsters */
          for (mtmp = fmon; mtmp; mtmp = mtmp->nmon)
            mtmp->movement += mcalcmove(mtmp);

          if (!rn2(player.uevent.udemigod
                       ? 25
                       : (depth(&player.uz) > depth(&stronghold_level)) ? 50
                                                                        : 70))
            (void)makemon(nullptr, 0, 0, NO_MM_FLAGS);

/* calculate how much time passed. */
#ifdef STEED
          if (player.usteed && player.umoved) {
            /* your speed doesn't augment steed's speed */
            moveamt = mcalcmove(player.usteed);
          } else
#endif
          {
            moveamt = youmonst.data->mmove;

            if (Very_fast) { /* speed boots or potion */
              /* average movement is 1.67 times normal */
              moveamt += NORMAL_SPEED / 2;
              if (rn2(3) == 0)
                moveamt += NORMAL_SPEED / 2;
            } else if (Fast) {
              /* average movement is 1.33 times normal */
              if (rn2(3) != 0)
                moveamt += NORMAL_SPEED / 2;
            }
          }

          switch (wtcap) {
            case UNENCUMBERED:
              break;
            case SLT_ENCUMBER:
              moveamt -= (moveamt / 4);
              break;
            case MOD_ENCUMBER:
              moveamt -= (moveamt / 2);
              break;
            case HVY_ENCUMBER:
              moveamt -= ((moveamt * 3) / 4);
              break;
            case EXT_ENCUMBER:
              moveamt -= ((moveamt * 7) / 8);
              break;
            default:
              break;
          }

          youmonst.movement += moveamt;
          if (youmonst.movement < 0)
            youmonst.movement = 0;
          settrack();

          monstermoves++;
          moves++;

          /********************************/
          /* once-per-turn things go here */
          /********************************/

          if (flags.bypasses)
            clear_bypasses();
          if (Glib)
            glibr();
          nh_timeout();
          run_regions();

          if (player.ublesscnt)
            player.ublesscnt--;
          if (flags.time && !flags.run)
            flags.botl = 1;

          /* One possible result of prayer is healing.  Whether or
           * not you get healed depends on your current hit points.
           * If you are allowed to regenerate during the prayer, the
           * end-of-prayer calculation messes up on this.
           * Another possible result is rehumanization, which requires
           * that encumbrance and movement rate be recalculated.
           */
          if (player.uinvulnerable) {
            /* for the moment at least, you're in tiptop shape */
            wtcap = UNENCUMBERED;
          } else if (Upolyd && youmonst.data->mlet == S_EEL &&
                     !is_pool(player.ux, player.uy) &&
                     !Is_waterlevel(&player.uz)) {
            if (player.mh > 1) {
              player.mh--;
              flags.botl = 1;
            } else if (player.mh < 1)
              rehumanize();
          } else if (Upolyd && player.mh < player.mhmax) {
            if (player.mh < 1)
              rehumanize();
            else if (Regeneration || (wtcap < MOD_ENCUMBER && !(moves % 20))) {
              flags.botl = 1;
              player.mh++;
            }
          } else if (player.uhp < player.uhpmax &&
                     (wtcap < MOD_ENCUMBER || !player.umoved || Regeneration)) {
            if (player.ulevel > 9 && !(moves % 3)) {
              int heal, Con = (int)ACURR(A_CON);

              if (Con <= 12) {
                heal = 1;
              } else {
                heal = rnd(Con);
                if (heal > player.ulevel - 9)
                  heal = player.ulevel - 9;
              }
              flags.botl = 1;
              player.uhp += heal;
              if (player.uhp > player.uhpmax)
                player.uhp = player.uhpmax;
            } else if (Regeneration ||
                       (player.ulevel <= 9 &&
                        !(moves %
                          ((MAXULEV + 12) / (player.ulevel + 2) + 1)))) {
              flags.botl = 1;
              player.uhp++;
            }
          }

          /* moving around while encumbered is hard work */
          if (wtcap > MOD_ENCUMBER && player.umoved) {
            if (!(wtcap < EXT_ENCUMBER ? moves % 30 : moves % 10)) {
              if (Upolyd && player.mh > 1) {
                player.mh--;
              } else if (!Upolyd && player.uhp > 1) {
                player.uhp--;
              } else {
                You("pass out from exertion!");
                exercise(A_CON, FALSE);
                fall_asleep(-10, FALSE);
              }
            }
          }

          if ((player.uen < player.uenmax) &&
              ((wtcap < MOD_ENCUMBER &&
                (!(moves % ((MAXULEV + 8 - player.ulevel) *
                            (Role_if(PM_WIZARD) ? 3 : 4) / 6)))) ||
               Energy_regeneration)) {
            player.uen += rn1((int)(ACURR(A_WIS) + ACURR(A_INT)) / 15 + 1, 1);
            if (player.uen > player.uenmax)
              player.uen = player.uenmax;
            flags.botl = 1;
          }

          if (!player.uinvulnerable) {
            if (Teleportation && !rn2(85)) {
              xchar old_ux = player.ux, old_uy = player.uy;
              tele();
              if (player.ux != old_ux || player.uy != old_uy) {
                if (!next_to_u()) {
                  check_leash(old_ux, old_uy);
                }
#ifdef REDO
                /* clear doagain keystrokes */
                pushch(0);
                savech(0);
#endif
              }
            }
            /* delayed change may not be valid anymore */
            if ((change == 1 && !Polymorph) ||
                (change == 2 && player.ulycn == NON_PM))
              change = 0;
            if (Polymorph && !rn2(100))
              change = 1;
            else if (player.ulycn >= LOW_PM && !Upolyd &&
                     !rn2(80 - (20 * night())))
              change = 2;
            if (change && !Unchanging) {
              if (multi >= 0) {
                if (occupation)
                  stop_occupation();
                else
                  nomul(0, 0);
                if (change == 1)
                  polyself(FALSE);
                else
                  you_were();
                change = 0;
              }
            }
          }

          if (Searching && multi >= 0)
            (void)dosearch0(1);
          dosounds();
          do_storms();
          gethungry();
          age_spells();
          exerchk();
          invault();
          if (player.uhave.amulet)
            amulet();
          if (!rn2(40 + (int)(ACURR(A_DEX) * 3)))
            u_wipe_engr(rnd(3));
          if (player.uevent.udemigod && !player.uinvulnerable) {
            if (player.udg_cnt)
              player.udg_cnt--;
            if (!player.udg_cnt) {
              intervene();
              player.udg_cnt = rn1(200, 50);
            }
          }
          restore_attrib();
          /* underwater and waterlevel vision are done here */
          if (Is_waterlevel(&player.uz))
            movebubbles();
          else if (Underwater)
            under_water(0);
          /* vision while buried done here */
          else if (player.uburied)
            under_ground(0);

          /* when immobile, count is in turns */
          if (multi < 0) {
            if (++multi == 0) { /* finished yet? */
              unmul(nullptr);
              /* if unmul caused a level change, take it now */
              if (player.utotype)
                deferred_goto();
            }
          }
        }
      } while (youmonst.movement < NORMAL_SPEED); /* hero can't move loop */

      /******************************************/
      /* once-per-hero-took-time things go here */
      /******************************************/

    } /* actual time passed */

    /****************************************/
    /* once-per-player-input things go here */
    /****************************************/

    find_ac();
    if (!flags.mv || Blind) {
      /* redo monsters if hallu or wearing a helm of telepathy */
      if (Hallucination) { /* update screen randomly */
        see_monsters();
        see_objects();
        see_traps();
        if (player.uswallow)
          swallowed(0);
      } else if (Unblind_telepat) {
        see_monsters();
      } else if (Warning || Warn_of_mon)
        see_monsters();

      if (vision_full_recalc)
        vision_recalc(0); /* vision! */
    }

#ifdef REALTIME_ON_BOTL
    if (iflags.showrealtime) {
      /* Update the bottom line if the number of minutes has
       * changed */
      if (get_realtime() / 60 != realtime_data.last_displayed_time / 60)
        flags.botl = 1;
    }
#endif

    if (flags.botl || flags.botlx)
      bot();

    flags.move = 1;

    if (multi >= 0 && occupation) {
      if ((*occupation)() == 0)
        occupation = 0;
      if (monster_nearby()) {
        stop_occupation();
        reset_eat();
      }
      continue;
    }

    if ((player.uhave.amulet || Clairvoyant) && !In_endgame(&player.uz) &&
        !BClairvoyant && !(moves % 15) && !rn2(2))
      do_vicinity_map();

    if (player.utrap && player.utraptype == TT_LAVA) {
      if (!is_lava(player.ux, player.uy))
        player.utrap = 0;
      else if (!player.uinvulnerable) {
        player.utrap -= 1 << 8;
        if (player.utrap < 1 << 8) {
          killer_format = KILLED_BY;
          killer = "molten lava";
          You("sink below the surface and die.");
          done(DISSOLVED);
        } else if (didmove && !player.umoved) {
          Norep("You sink deeper into the lava.");
          player.utrap += rnd(4);
        }
      }
    }

#ifdef WIZARD
    if (iflags.sanity_check)
      sanity_check();
#endif

#ifdef CLIPPING
    /* just before rhack */
    cliparound(player.ux, player.uy);
#endif

    player.umoved = FALSE;

    if (multi > 0) {
      lookaround();
      if (!multi) {
        /* lookaround may clear multi */
        flags.move = 0;
        if (flags.time)
          flags.botl = 1;
        continue;
      }
      if (flags.mv) {
        if (multi < COLNO && !--multi)
          flags.travel = iflags.travel1 = flags.mv = flags.run = 0;
        domove();
      } else {
        --multi;
        rhack(save_cm);
      }
    } else if (multi == 0) {
#ifdef MAIL
      ckmailstatus();
#endif
      rhack(nullptr);
    }
    if (player.utotype) /* change dungeon level */
      deferred_goto();  /* after rhack() */
    /* !flags.move here: multiple movement command stopped */
    else if (flags.time && (!flags.move || !flags.mv))
      flags.botl = 1;

    if (vision_full_recalc)
      vision_recalc(0); /* vision! */
    /* when running in non-tport mode, this gets done through domove() */
    if ((!flags.run || iflags.runmode == RUN_TPORT) &&
        (multi && (!flags.travel ? !(multi % 7) : !(moves % 7L)))) {
      if (flags.time && flags.run)
        flags.botl = 1;
      display_nhwindow(WIN_MAP, FALSE);
    }
  }
}

#endif /* OVL0 */
#ifdef OVL1

void stop_occupation() {
  if (occupation) {
    if (!maybe_finished_meal(TRUE))
      You("stop %s.", occtxt);
    occupation = 0;
    flags.botl = 1; /* in case player.uhs changed */
/* fainting stops your occupation, there's no reason to sync.
                sync_hunger();
*/
#ifdef REDO
    nomul(0, 0);
    pushch(0);
#endif
  }
}

#endif /* OVL1 */
#ifdef OVLB

void display_gamewindows() {
  WIN_MESSAGE = create_nhwindow(NHW_MESSAGE);
  WIN_STATUS = create_nhwindow(NHW_STATUS);
  WIN_MAP = create_nhwindow(NHW_MAP);
  WIN_INVEN = create_nhwindow(NHW_MENU);

  /*
   * The mac port is not DEPENDENT on the order of these
   * displays, but it looks a lot better this way...
   */
  display_nhwindow(WIN_STATUS, FALSE);
  display_nhwindow(WIN_MESSAGE, FALSE);
  clear_glyph_buffer();
  display_nhwindow(WIN_MAP, FALSE);
}

void newgame() {
  int i;

  flags.ident = 1;

  for (i = 0; i < NUMMONS; i++)
    mvitals[i].mvflags = mons[i].geno & G_NOCORPSE;

  init_objects(); /* must be before u_init() */

  flags.pantheon = -1; /* role_init() will reset this */
  role_init();         /* must be before init_dungeons(), u_init(),
                        * and init_artifacts() */

  init_dungeons();  /* must be before u_init() to avoid rndmonst()
                     * creating odd monsters for any tins and eggs
                     * in hero's initial inventory */
  init_artifacts(); /* before u_init() in case $WIZKIT specifies
                     * any artifacts */
  ErrMsg("Finished artifacts!");
  u_init();
  ErrMsg("Finished u_init!");

#ifndef NO_SIGNAL
  (void)signal(SIGINT, (SIG_RET_TYPE)done1);
#endif
#ifdef NEWS
  if (iflags.news)
    display_file(NEWS, FALSE);
#endif
  load_qtlist(); /* load up the quest text info */
  ErrMsg("loaded quest!");
  /*	quest_init();*/ /* Now part of role_init() */

  mklev();
  u_on_upstairs();
  vision_reset(); /* set up internals for level (after mklev) */
  check_special_room(FALSE);

  flags.botlx = 1;

  /* Move the monster from under you or else
   * makedog() will fail when it calls makemon().
   *			- ucsfcgl!kneller
   */
  if (MON_AT(player.ux, player.uy))
    mnexto(m_at(player.ux, player.uy));
  (void)makedog();
  docrt();

  if (flags.legacy) {
    flush_screen(1);
    com_pager(1);
  }

#ifdef INSURANCE
  save_currentstate();
#endif
  program_state.something_worth_saving++; /* useful data now exists */

#if defined(RECORD_REALTIME) || defined(REALTIME_ON_BOTL)

  /* Start the timer here */
  realtime_data.realtime = (time_t)0L;

  (void)time(&realtime_data.restoretime);

#endif /* RECORD_REALTIME || REALTIME_ON_BOTL */

  /* Success! */
  welcome(TRUE);
  return;
}

/* show "welcome [back] to nethack" message at program startup */
void welcome(bool new_game) {
  char buf[BUFSZ];
  bool currentgend = Upolyd ? player.mfemale : flags.female;

  /*
   * The "welcome back" message always describes your innate form
   * even when polymorphed or wearing a helm of opposite alignment.
   * Alignment is shown unconditionally for new games; for restores
   * it's only shown if it has changed from its original value.
   * Sex is shown for new games except when it is redundant; for
   * restores it's only shown if different from its original value.
   */
  *buf = '\0';
  if (new_game || player.ualignbase[A_ORIGINAL] != player.ualignbase[A_CURRENT])
    sprintf(eos(buf), " %s", align_str(player.ualignbase[A_ORIGINAL]));
  if (!urole.name.f &&
      (new_game ? (urole.allow & ROLE_GENDMASK) == (ROLE_MALE | ROLE_FEMALE)
                : currentgend != flags.initgend))
    sprintf(eos(buf), " %s", genders[currentgend].adj);

  pline(new_game ? "%s %s, welcome to NetHack!  You are a%s %s %s."
                 : "%s %s, the%s %s %s, welcome back to NetHack!",
        Hello(nullptr), plname, buf, urace.adj,
        (currentgend && urole.name.f) ? urole.name.f : urole.name.m);
}

#ifdef POSITIONBAR
STATIC_DCL void do_positionbar() {
  static char pbar[COLNO];
  char *p;

  p = pbar;
  /* up stairway */
  if (upstair.sx &&
      (glyph_to_cmap(level.locations[upstair.sx][upstair.sy].glyph) ==
           S_upstair ||
       glyph_to_cmap(level.locations[upstair.sx][upstair.sy].glyph) ==
           S_upladder)) {
    *p++ = '<';
    *p++ = upstair.sx;
  }
  if (sstairs.sx &&
      (glyph_to_cmap(level.locations[sstairs.sx][sstairs.sy].glyph) ==
           S_upstair ||
       glyph_to_cmap(level.locations[sstairs.sx][sstairs.sy].glyph) ==
           S_upladder)) {
    *p++ = '<';
    *p++ = sstairs.sx;
  }

  /* down stairway */
  if (dnstair.sx &&
      (glyph_to_cmap(level.locations[dnstair.sx][dnstair.sy].glyph) ==
           S_dnstair ||
       glyph_to_cmap(level.locations[dnstair.sx][dnstair.sy].glyph) ==
           S_dnladder)) {
    *p++ = '>';
    *p++ = dnstair.sx;
  }
  if (sstairs.sx &&
      (glyph_to_cmap(level.locations[sstairs.sx][sstairs.sy].glyph) ==
           S_dnstair ||
       glyph_to_cmap(level.locations[sstairs.sx][sstairs.sy].glyph) ==
           S_dnladder)) {
    *p++ = '>';
    *p++ = sstairs.sx;
  }

  /* hero location */
  if (player.ux) {
    *p++ = '@';
    *p++ = player.ux;
  }
  /* fence post */
  *p = 0;

  update_positionbar(pbar);
}
#endif

#if defined(REALTIME_ON_BOTL) || defined(RECORD_REALTIME)
time_t get_realtime(void) {
  time_t curtime;

/* Get current time */
#if defined(BSD) && !defined(POSIX_TYPES)
  (void)time((long *)&curtime);
#else
  (void)time(&curtime);
#endif

  /* Since the timer isn't set until the game starts, this prevents us
   * from displaying nonsense on the bottom line before it does. */
  if (realtime_data.restoretime == 0) {
    curtime = realtime_data.realtime;
  } else {
    curtime -= realtime_data.restoretime;
    curtime += realtime_data.realtime;
  }

  return curtime;
}
#endif /* REALTIME_ON_BOTL || RECORD_REALTIME */

#endif /* OVLB */

/*allmain.c*/
