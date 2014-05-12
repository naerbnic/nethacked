/*	SCCS Id: @(#)do_name.c	3.4	2003/01/14	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "core/do_name.h"

#include <string.h>
#include <string>

#include "core/hack.h"
#include "core/worn.h"
#include "core/worm.h"
#include "core/wield.h"
#include "core/shk.h"
#include "core/rnd.h"
#include "core/priest.h"
#include "core/polyself.h"
#include "core/pline.h"
#include "core/options.h"
#include "core/objnam.h"
#include "core/o_init.h"
#include "core/mon.h"
#include "core/mkobj.h"
#include "core/light.h"
#include "core/invent.h"
#include "core/hacklib.h"
#include "core/eat.h"
#include "core/cmd.h"

#include "core/artifact.h"
#include "core/botl.h"

using std::string;

#ifdef OVLB

STATIC_DCL void do_oname(Object *);
static void getpos_help(bool, const char *);

extern const char what_is_an_unknown_object[]; /* from pager.c */

/* the response for '?' help request in getpos() */
static void getpos_help(bool force, const char *goal) {
  char sbuf[BUFSZ];
  bool doing_what_is;
  winid tmpwin = create_nhwindow(NHW_MENU);

  sprintf(sbuf, "Use [%s] to move the cursor to %s.",
          iflags.num_pad ? "2468" : "hjkl", goal);
  putstr(tmpwin, 0, sbuf);
  putstr(tmpwin, 0, "Use [HJKL] to move the cursor 8 units at a time.");
  putstr(tmpwin, 0, "Or enter a background symbol (ex. <).");
  /* disgusting hack; the alternate selection characters work for any
     getpos call, but they only matter for dowhatis (and doquickwhatis) */
  doing_what_is = (goal == what_is_an_unknown_object);
  sprintf(sbuf, "Type a .%s when you are at the right place.",
          doing_what_is ? " or , or ; or :" : "");
  putstr(tmpwin, 0, sbuf);
  if (!force)
    putstr(tmpwin, 0, "Type Space or Escape when you're done.");
  putstr(tmpwin, 0, "");
  display_nhwindow(tmpwin, TRUE);
  destroy_nhwindow(tmpwin);
}

int getpos(coord *cc, bool force, const char *goal) {
  int result = 0;
  int cx, cy, i, c;
  int sidx, tx, ty;
  bool msg_given = TRUE; /* clear message window by default */
  static const char pick_chars[] = ".,;:";
  const char *cp;
  const char *sdp;
  if (iflags.num_pad)
    sdp = ndir;
  else
    sdp = sdir; /* DICE workaround */

  if (flags.verbose) {
    pline("(For instructions type a ?)");
    msg_given = TRUE;
  }
  cx = cc->x;
  cy = cc->y;
#ifdef CLIPPING
  cliparound(cx, cy);
#endif
  curs(WIN_MAP, cx, cy);
  flush_screen(0);
  for (;;) {
    c = nh_poskey(&tx, &ty, &sidx);
    if (c == '\033') {
      cx = cy = -10;
      msg_given = TRUE; /* force clear */
      result = -1;
      break;
    }
    if (c == 0) {
      if (!isok(tx, ty))
        continue;
      /* a mouse click event, just assign and return */
      cx = tx;
      cy = ty;
      break;
    }
    if ((cp = index(pick_chars, c)) != 0) {
      /* '.' => 0, ',' => 1, ';' => 2, ':' => 3 */
      result = cp - pick_chars;
      break;
    }
    for (i = 0; i < 8; i++) {
      int dx, dy;

      if (sdp[i] == c) {
        /* a normal movement letter or digit */
        dx = xdir[i];
        dy = ydir[i];
      } else if (sdir[i] == lowc((char)c)) {
        /* a shifted movement letter */
        dx = 8 * xdir[i];
        dy = 8 * ydir[i];
      } else
        continue;

      /* truncate at map edge; diagonal moves complicate this... */
      if (cx + dx < 1) {
        dy -= sgn(dy) * (1 - (cx + dx));
        dx = 1 - cx; /* so that (cx+dx == 1) */
      } else if (cx + dx > COLNO - 1) {
        dy += sgn(dy) * ((COLNO - 1) - (cx + dx));
        dx = (COLNO - 1) - cx;
      }
      if (cy + dy < 0) {
        dx -= sgn(dx) * (0 - (cy + dy));
        dy = 0 - cy; /* so that (cy+dy == 0) */
      } else if (cy + dy > ROWNO - 1) {
        dx += sgn(dx) * ((ROWNO - 1) - (cy + dy));
        dy = (ROWNO - 1) - cy;
      }
      cx += dx;
      cy += dy;
      goto nxtc;
    }

    if (c == '?') {
      getpos_help(force, goal);
    } else {
      if (!index(quitchars, c)) {
        char matching[MAXPCHARS];
        int pass, lo_x, lo_y, hi_x, hi_y, k = 0;
        (void)memset((genericptr_t)matching, 0, sizeof matching);
        for (sidx = 1; sidx < MAXPCHARS; sidx++)
          if (c == defsyms[sidx].sym || c == (int)showsyms[sidx])
            matching[sidx] = (char)++k;
        if (k) {
          for (pass = 0; pass <= 1; pass++) {
            /* pass 0: just past current pos to lower right;
               pass 1: upper left corner to current pos */
            lo_y = (pass == 0) ? cy : 0;
            hi_y = (pass == 0) ? ROWNO - 1 : cy;
            for (ty = lo_y; ty <= hi_y; ty++) {
              lo_x = (pass == 0 && ty == lo_y) ? cx + 1 : 1;
              hi_x = (pass == 1 && ty == hi_y) ? cx : COLNO - 1;
              for (tx = lo_x; tx <= hi_x; tx++) {
                /* look at dungeon feature, not at user-visible glyph */
                k = back_to_glyph(tx, ty);
                /* uninteresting background glyph */
                if (glyph_is_cmap(k) &&
                    (IS_DOOR(
                         levl[tx][ty].typ) || /* monsters mimicking a door */
                     glyph_to_cmap(k) == S_room ||
                     glyph_to_cmap(k) == S_corr ||
                     glyph_to_cmap(k) == S_litcorr)) {
                  /* what the user remembers to be at tx,ty */
                  k = glyph_at(tx, ty);
                }
                /* TODO: - open doors are only matched with '-' */
                /* should remembered or seen items be matched? */
                if (glyph_is_cmap(k) && matching[glyph_to_cmap(k)] &&
                    levl[tx][ty].seenv && /* only if already seen */
                    (!IS_WALL(levl[tx][ty].typ) &&
                     (levl[tx][ty].typ != SDOOR) &&
                     glyph_to_cmap(k) != S_room && glyph_to_cmap(k) != S_corr &&
                     glyph_to_cmap(k) != S_litcorr)) {
                  cx = tx, cy = ty;
                  if (msg_given) {
                    clear_nhwindow(WIN_MESSAGE);
                    msg_given = FALSE;
                  }
                  goto nxtc;
                }
              } /* column */
            }   /* row */
          }     /* pass */
          pline("Can't find dungeon feature '%c'.", c);
          msg_given = TRUE;
          goto nxtc;
        } else {
          pline("Unknown direction: '%s' (%s).", visctrl((char)c),
                !force ? "aborted" : iflags.num_pad ? "use 2468 or ."
                                                    : "use hjkl or .");
          msg_given = TRUE;
        } /* k => matching */
      }   /* !quitchars */
      if (force)
        goto nxtc;
      pline("Done.");
      msg_given = FALSE; /* suppress clear */
      cx = -1;
      cy = 0;
      result = 0; /* not -1 */
      break;
    }
  nxtc:
    ;
#ifdef CLIPPING
    cliparound(cx, cy);
#endif
    curs(WIN_MAP, cx, cy);
    flush_screen(0);
  }
  if (msg_given)
    clear_nhwindow(WIN_MESSAGE);
  cc->x = cx;
  cc->y = cy;
  return result;
}

Monster *christen_monst(Monster *mtmp, string const& name) {
  int lth;
  Monster *mtmp2;
  char buf[PL_PSIZ];

  string new_name;
  if (name.size() >= PL_PSIZ) {
    new_name = name.substr(0, PL_PSIZ - 1);
  } else {
    new_name = name;
  }

  /* dogname & catname are PL_PSIZ arrays; object names have same limit */
  if (lth == mtmp->mnamelth) {
    /* don't need to allocate a new monst struct */
    if (lth)
      strcpy(mtmp->name(), new_name.c_str());
    return mtmp;
  }
  mtmp2 = newmonst(mtmp->mxlth + lth);
  *mtmp2 = *mtmp;
  (void)memcpy((genericptr_t)mtmp2->mextra, (genericptr_t)mtmp->mextra,
               mtmp->mxlth);
  mtmp2->mnamelth = lth;
  if (lth)
    strcpy(mtmp2->name(), new_name.c_str());
  replmon(mtmp, mtmp2);
  return (mtmp2);
}

int do_mname() {
  char buf[BUFSZ];
  coord cc;
  int cx, cy;
  Monster *mtmp;
  char qbuf[QBUFSZ];

  if (Hallucination) {
    You("would never recognize it anyway.");
    return 0;
  }
  cc.x = player.ux;
  cc.y = player.uy;
  if (getpos(&cc, FALSE, "the monster you want to name") < 0 || (cx = cc.x) < 0)
    return 0;
  cy = cc.y;

  if (cx == player.ux && cy == player.uy) {
#ifdef STEED
    if (player.usteed && canspotmon(player.usteed))
      mtmp = player.usteed;
    else {
#endif
      pline("This %s creature is called %s and cannot be renamed.",
            ACURR(A_CHA) > 14 ? (flags.female ? "beautiful" : "handsome")
                              : "ugly",
            plname);
      return (0);
#ifdef STEED
    }
#endif
  } else
    mtmp = m_at(cx, cy);

  if (!mtmp ||
      (!sensemon(mtmp) &&
       (!(cansee(cx, cy) || see_with_infrared(mtmp)) || mtmp->mundetected ||
        mtmp->m_ap_type == M_AP_FURNITURE || mtmp->m_ap_type == M_AP_OBJECT ||
        (mtmp->minvis && !See_invisible)))) {
    pline("I see no monster there.");
    return (0);
  }
  /* special case similar to the one in lookat() */
  (void)distant_monnam(mtmp, ARTICLE_THE, buf);
  sprintf(qbuf, "What do you want to call %s?", buf);
  getlin(qbuf, buf);
  if (!*buf || *buf == '\033')
    return (0);
  /* strip leading and trailing spaces; unnames monster if all spaces */
  (void)mungspaces(buf);

  if (mtmp->data->geno & G_UNIQ) {
    if (mtmp->data == &mons[PM_HIGH_PRIEST] && Is_astralevel(&player.uz)) {
      pline_The("high priest%s doesn't like being called names!",
                mtmp->female ? "ess" : "");
    } else {
      pline("%s doesn't like being called names!", Monnam(mtmp));
    }
  } else
    (void)christen_monst(mtmp, buf);
  return (0);
}

/*
 * This routine changes the address of obj. Be careful not to call it
 * when there might be pointers around in unknown places. For now: only
 * when obj is in the inventory.
 */
STATIC_OVL
void do_oname(Object *obj) {
  char buf[BUFSZ], qbuf[QBUFSZ];
  const char *aname;
  short objtyp;

  sprintf(qbuf, "What do you want to name %s %s?",
          is_plural(obj) ? "these" : "this", xname(obj));
  getlin(qbuf, buf);
  if (!*buf || *buf == '\033')
    return;
  /* strip leading and trailing spaces; unnames item if all spaces */
  (void)mungspaces(buf);

  /* relax restrictions over proper capitalization for artifacts */
  if ((aname = artifact_name(buf, &objtyp)) != 0 && objtyp == obj->otyp)
    strcpy(buf, aname);

  if (obj->oartifact) {
    pline_The("artifact seems to resist the attempt.");
    return;
  } else if (restrict_name(obj, buf) || exist_artifact(obj->otyp, buf)) {
    int n = rn2((int)strlen(buf));
    char c1, c2;

    c1 = lowc(buf[n]);
    do
      c2 = 'a' + rn2('z' - 'a');
    while (c1 == c2);
    buf[n] = (buf[n] == c1) ? c2 : highc(c2); /* keep same case */
    pline("While engraving your %s slips.", body_part(HAND));
    display_nhwindow(WIN_MESSAGE, FALSE);
    You("engrave: \"%s\".", buf);
  }
  obj = oname(obj, buf);
}

/*
 * Allocate a new and possibly larger storage space for an obj.
 */
Object *ReallocateExtraObjectSpace(Object *obj, int oextra_size,
                                   genericptr_t oextra_src, int oname_size,
                                   const char *name) {
  return ReallocateExtraObjectSpace(obj, oextra_size, oextra_src,
      string(name, name + oname_size));
}

/*
 * Allocate a new and possibly larger storage space for an obj.
 */
Object *ReallocateExtraObjectSpace(Object *obj, int oextra_size,
                                   genericptr_t oextra_src, string const& name) {
  Object *otmp;

  otmp = newobj(oextra_size);
  *otmp = *obj; /* the cobj pointer is copied to otmp */
  if (oextra_size) {
    if (oextra_src)
      (void)memcpy((genericptr_t)otmp->oextra, oextra_src, oextra_size);
  } else {
    otmp->oattached = OATTACHED_NOTHING;
  }
  otmp->oxlth = oextra_size;

  otmp->objname = name;
  otmp->timed = 0;   /* not timed, yet */
  otmp->lamplit = 0; /* ditto */

  if (obj->owornmask) {
    bool save_twoweap = player.twoweap;
    /* unwearing the old instance will clear dual-wield mode
       if this object is either of the two weapons */
    setworn(nullptr, obj->owornmask);
    setworn(otmp, otmp->owornmask);
    player.twoweap = save_twoweap;
  }

  /* replace obj with otmp */
  ReplaceObject(obj, otmp);

  /* fix ocontainer pointers */
  if (Has_contents(obj)) {
    Object *inside;

    for (inside = obj->cobj; inside; inside = inside->nobj)
      inside->ocontainer = otmp;
  }

  /* move timers and light sources from obj to otmp */
  if (obj->timed)
    obj_move_timers(obj, otmp);
  if (obj->lamplit)
    obj_move_light_source(obj, otmp);

  /* objects possibly being manipulated by multi-turn occupations
     which have been interrupted but might be subsequently resumed */
  if (obj->oclass == FOOD_CLASS)
    food_substitution(obj, otmp); /* eat food or open tin */
  else if (obj->oclass == SPBOOK_CLASS)
    book_substitution(obj, otmp); /* read spellbook */

  /* obfree(obj, otmp);	now unnecessary: no pointers on bill */
  DeallocateObject(obj); /* let us hope nobody else saved a pointer */
  return otmp;
}

Object *oname(Object *obj, string const& name) {
  int lth;
  char buf[PL_PSIZ];

  string new_name;
  if (name.size() >= PL_PSIZ) {
    new_name = name.substr(0, PL_PSIZ - 1);
  } else {
    new_name = name;
  }

  /* If named artifact exists in the game, do not create another.
   * Also trying to create an artifact shouldn't de-artifact
   * it (e.g. Excalibur from prayer). In this case the object
   * will retain its current name. */
  if (obj->oartifact || (!new_name.empty() && exist_artifact(obj->otyp, new_name)))
    return obj;

  obj->objname = new_name;
  if (lth)
    artifact_exists(obj, name, TRUE);
  if (obj->oartifact) {
    /* can't dual-wield with artifact as secondary weapon */
    if (obj == uswapwep)
      untwoweapon();
    /* activate warning if you've just named your weapon "Sting" */
    if (obj == uwep)
      set_artifact_intrinsic(obj, TRUE, W_WEP);
  }
  if (carried(obj))
    update_inventory();
  return obj;
}

static const char callable[] = {
    SCROLL_CLASS, POTION_CLASS, WAND_CLASS,  RING_CLASS, AMULET_CLASS,
    GEM_CLASS,    SPBOOK_CLASS, ARMOR_CLASS, TOOL_CLASS, 0};

int ddocall() {
  Object *obj;
#ifdef REDO
  char ch;
#endif
  char allowall[2];

  switch (
#ifdef REDO
      ch =
#endif
          ynq("Name an individual object?")) {
    case 'q':
      break;
    case 'y':
#ifdef REDO
      savech(ch);
#endif
      allowall[0] = ALL_CLASSES;
      allowall[1] = '\0';
      obj = getobj(allowall, "name");
      if (obj)
        do_oname(obj);
      break;
    default:
#ifdef REDO
      savech(ch);
#endif
      obj = getobj(callable, "call");
      if (obj) {
        /* behave as if examining it in inventory;
           this might set dknown if it was picked up
           while blind and the hero can now see */
        (void)xname(obj);

        if (!obj->dknown) {
          You("would never recognize another one.");
          return 0;
        }
        docall(obj);
      }
      break;
  }
  return 0;
}

void docall(Object *obj) {
  char buf[BUFSZ], qbuf[QBUFSZ];
  Object otemp;
  char **str1;

  if (!obj->dknown)
    return; /* probably blind */
  otemp = *obj;
  otemp.quan = 1L;
  otemp.objname = "";
  otemp.oxlth = 0;
  if (objects[otemp.otyp].oc_class == POTION_CLASS && otemp.fromsink)
    /* kludge, meaning it's sink water */
    sprintf(qbuf, "Call a stream of %s fluid:", OBJ_DESCR(objects[otemp.otyp]));
  else
    sprintf(qbuf, "Call %s:", an(xname(&otemp)));
  getlin(qbuf, buf);
  if (!*buf || *buf == '\033')
    return;

  /* clear old name */
  str1 = &(objects[obj->otyp].oc_uname);
  if (*str1)
    free((genericptr_t) * str1);

  /* strip leading and trailing spaces; uncalls item if all spaces */
  (void)mungspaces(buf);
  if (!*buf) {
    if (*str1) { /* had name, so possibly remove from disco[] */
                 /* strip name first, for the update_inventory() call
                    from undiscover_object() */
      *str1 = nullptr;
      undiscover_object(obj->otyp);
    }
  } else {
    *str1 = strcpy((char *)alloc((unsigned)strlen(buf) + 1), buf);
    discover_object(obj->otyp, FALSE, TRUE); /* possibly add to disco[] */
  }
}

#endif /*OVLB*/

static const char *const ghostnames[] = {
    /* these names should have length < PL_NSIZ */
    /* Capitalize the names for aesthetics -dgk */
    "Adri",    "Andries",       "Andreas",     "Bert",    "David",  "Dirk",
    "Emile",   "Frans",         "Fred",        "Greg",    "Hether", "Jay",
    "John",    "Jon",           "Karnov",      "Kay",     "Kenny",  "Kevin",
    "Maud",    "Michiel",       "Mike",        "Peter",   "Robert", "Ron",
    "Tom",     "Wilmar",        "Nick Danger", "Phoenix", "Jiro",   "Mizue",
    "Stephan", "Lance Braccus", "Shadowhawk"};

/* ghost names formerly set by x_monnam(), now by makemon() instead */
const char *rndghostname() {
  return rn2(7) ? ghostnames[rn2(SIZE(ghostnames))] : (const char *)plname;
}

/* Monster naming functions:
 * x_monnam is the generic monster-naming function.
 *		  seen	      unseen	   detected		  named
 * mon_nam:	the newt	it	the invisible orc	Fido
 * noit_mon_nam:the newt (as if detected) the invisible orc	Fido
 * l_monnam:	newt		it	invisible orc		dog called fido
 * Monnam:	The newt	It	The invisible orc	Fido
 * noit_Monnam: The newt (as if detected) The invisible orc	Fido
 * Adjmonnam:	The poor newt	It	The poor invisible orc	The poor Fido
 * Amonnam:	A newt		It	An invisible orc	Fido
 * a_monnam:	a newt		it	an invisible orc	Fido
 * m_monnam:	newt		xan	orc			Fido
 * y_monnam:	your newt     your xan	your invisible orc	Fido
 */

/* Bug: if the monster is a priest or shopkeeper, not every one of these
 * options works, since those are special cases.
 */
char *x_monnam(
    Monster *mtmp, int article,
    /* ARTICLE_NONE, ARTICLE_THE, ARTICLE_A: obvious
     * ARTICLE_YOUR: "your" on pets, "the" on everything else
     *
     * If the monster would be referred to as "it" or if the monster has a name
     * _and_ there is no adjective, "invisible", "saddled", etc., override this
     * and always use no article.
     */
    char const *adjective, int suppress,
    /* SUPPRESS_IT, SUPPRESS_INVISIBLE, SUPPRESS_HALLUCINATION, SUPPRESS_SADDLE.
     * EXACT_NAME: combination of all the above
     */
    bool called) {
#ifdef LINT /* static char buf[BUFSZ]; */
  char buf[BUFSZ];
#else
  static char buf[BUFSZ];
#endif
  MonsterType *mdat = mtmp->data;
  bool do_hallu, do_invis, do_it, do_saddle;
  bool name_at_start, has_adjectives;
  char *bp;

  if (program_state.gameover)
    suppress |= SUPPRESS_HALLUCINATION;
  if (article == ARTICLE_YOUR && !mtmp->mtame)
    article = ARTICLE_THE;

  do_hallu = Hallucination && !(suppress & SUPPRESS_HALLUCINATION);
  do_invis = mtmp->minvis && !(suppress & SUPPRESS_INVISIBLE);
  do_it =
      !canspotmon(mtmp) && article != ARTICLE_YOUR && !program_state.gameover &&
#ifdef STEED
      mtmp != player.usteed &&
#endif
      !(player.uswallow && mtmp == player.ustuck) && !(suppress & SUPPRESS_IT);
  do_saddle = !(suppress & SUPPRESS_SADDLE);

  buf[0] = 0;

  /* unseen monsters, etc.  Use "it" */
  if (do_it) {
    strcpy(buf, "it");
    return buf;
  }

  /* priests and minions: don't even use this function */
  if (mtmp->ispriest || mtmp->isminion) {
    char priestnambuf[BUFSZ];
    char *name;
    long save_prop = EHalluc_resistance;
    unsigned save_invis = mtmp->minvis;

    /* when true name is wanted, explicitly block Hallucination */
    if (!do_hallu)
      EHalluc_resistance = 1L;
    if (!do_invis)
      mtmp->minvis = 0;
    name = priestname(mtmp, priestnambuf);
    EHalluc_resistance = save_prop;
    mtmp->minvis = save_invis;
    if (article == ARTICLE_NONE && !strncmp(name, "the ", 4))
      name += 4;
    return strcpy(buf, name);
  }

  /* Shopkeepers: use shopkeeper name.  For normal shopkeepers, just
   * "Asidonhopo"; for unusual ones, "Asidonhopo the invisible
   * shopkeeper" or "Asidonhopo the blue dragon".  If hallucinating,
   * none of this applies.
   */
  if (mtmp->isshk && !do_hallu) {
    if (adjective && article == ARTICLE_THE) {
      /* pathological case: "the angry Asidonhopo the blue dragon"
         sounds silly */
      strcpy(buf, "the ");
      strcat(strcat(buf, adjective), " ");
      strcat(buf, shkname(mtmp));
      return buf;
    }
    strcat(buf, shkname(mtmp));
    if (mdat == &mons[PM_SHOPKEEPER] && !do_invis)
      return buf;
    strcat(buf, " the ");
    if (do_invis)
      strcat(buf, "invisible ");
    strcat(buf, mdat->mname);
    return buf;
  }

  /* Put the adjectives in the buffer */
  if (adjective)
    strcat(strcat(buf, adjective), " ");
  if (do_invis)
    strcat(buf, "invisible ");
#ifdef STEED
  if (do_saddle && (mtmp->misc_worn_check & W_SADDLE) && !Blind &&
      !Hallucination)
    strcat(buf, "saddled ");
#endif
  if (buf[0] != 0)
    has_adjectives = TRUE;
  else
    has_adjectives = FALSE;

  /* Put the actual monster name or type into the buffer now */
  /* Be sure to remember whether the buffer starts with a name */
  if (do_hallu) {
    strcat(buf, rndmonnam());
    name_at_start = FALSE;
  } else if (mtmp->mnamelth) {
    char *name = mtmp->name();

    if (mdat == &mons[PM_GHOST]) {
      sprintf(eos(buf), "%s ghost", s_suffix(name));
      name_at_start = TRUE;
    } else if (called) {
      sprintf(eos(buf), "%s called %s", mdat->mname, name);
      name_at_start = (bool)type_is_pname(mdat);
    } else if (is_mplayer(mdat) && (bp = strstri(name, " the ")) != 0) {
      /* <name> the <adjective> <invisible> <saddled> <rank> */
      char pbuf[BUFSZ];

      strcpy(pbuf, name);
      pbuf[bp - name + 5] = '\0'; /* adjectives right after " the " */
      if (has_adjectives)
        strcat(pbuf, buf);
      strcat(pbuf, bp + 5); /* append the rest of the name */
      strcpy(buf, pbuf);
      article = ARTICLE_NONE;
      name_at_start = TRUE;
    } else {
      strcat(buf, name);
      name_at_start = TRUE;
    }
  } else if (is_mplayer(mdat) && !In_endgame(&player.uz)) {
    char pbuf[BUFSZ];
    strcpy(pbuf, rank_of((int)mtmp->m_lev, monsndx(mdat), (bool)mtmp->female));
    strcat(buf, lcase(pbuf));
    name_at_start = FALSE;
  } else {
    strcat(buf, mdat->mname);
    name_at_start = (bool)type_is_pname(mdat);
  }

  if (name_at_start && (article == ARTICLE_YOUR || !has_adjectives)) {
    if (mdat == &mons[PM_WIZARD_OF_YENDOR])
      article = ARTICLE_THE;
    else
      article = ARTICLE_NONE;
  } else if ((mdat->geno & G_UNIQ) && article == ARTICLE_A) {
    article = ARTICLE_THE;
  }

  {
    char buf2[BUFSZ];

    switch (article) {
      case ARTICLE_YOUR:
        strcpy(buf2, "your ");
        strcat(buf2, buf);
        strcpy(buf, buf2);
        return buf;
      case ARTICLE_THE:
        strcpy(buf2, "the ");
        strcat(buf2, buf);
        strcpy(buf, buf2);
        return buf;
      case ARTICLE_A:
        return (an(buf));
      case ARTICLE_NONE:
      default:
        return buf;
    }
  }
}

#ifdef OVLB

char *l_monnam(Monster *mtmp) {
  return (x_monnam(mtmp, ARTICLE_NONE, nullptr,
                   mtmp->mnamelth ? SUPPRESS_SADDLE : 0, TRUE));
}

#endif /* OVLB */

char *mon_nam(Monster *mtmp) {
  return (x_monnam(mtmp, ARTICLE_THE, nullptr,
                   mtmp->mnamelth ? SUPPRESS_SADDLE : 0, FALSE));
}

/* print the name as if mon_nam() was called, but assume that the player
 * can always see the monster--used for probing and for monsters aggravating
 * the player with a cursed potion of invisibility
 */
char *noit_mon_nam(Monster *mtmp) {
  return (x_monnam(
      mtmp, ARTICLE_THE, nullptr,
      mtmp->mnamelth ? (SUPPRESS_SADDLE | SUPPRESS_IT) : SUPPRESS_IT, FALSE));
}

char *Monnam(Monster *mtmp) {
  char *bp = mon_nam(mtmp);

  *bp = highc(*bp);
  return (bp);
}

char *noit_Monnam(Monster *mtmp) {
  char *bp = noit_mon_nam(mtmp);

  *bp = highc(*bp);
  return (bp);
}

/* monster's own name */
char *m_monnam(Monster *mtmp) {
  return x_monnam(mtmp, ARTICLE_NONE, nullptr, EXACT_NAME, FALSE);
}

/* pet name: "your little dog" */
char *y_monnam(Monster *mtmp) {
  int prefix, suppression_flag;

  prefix = mtmp->mtame ? ARTICLE_YOUR : ARTICLE_THE;
  suppression_flag = (mtmp->mnamelth
#ifdef STEED
                      /* "saddled" is redundant when mounted */
                      ||
                      mtmp == player.usteed
#endif
                      )
                         ? SUPPRESS_SADDLE
                         : 0;

  return x_monnam(mtmp, prefix, nullptr, suppression_flag, FALSE);
}

#ifdef OVLB

char *Adjmonnam(Monster *mtmp, const char *adj) {
  char *bp = x_monnam(mtmp, ARTICLE_THE, adj,
                      mtmp->mnamelth ? SUPPRESS_SADDLE : 0, FALSE);

  *bp = highc(*bp);
  return (bp);
}

char *a_monnam(Monster *mtmp) {
  return x_monnam(mtmp, ARTICLE_A, nullptr,
                  mtmp->mnamelth ? SUPPRESS_SADDLE : 0, FALSE);
}

char *Amonnam(Monster *mtmp) {
  char *bp = a_monnam(mtmp);

  *bp = highc(*bp);
  return (bp);
}

/* used for monster ID by the '/', ';', and 'C' commands to block remote
   identification of the endgame altars via their attending priests */
char *distant_monnam(Monster *mon, int article, char *outbuf) {
  /* high priest(ess)'s identity is concealed on the Astral Plane,
     unless you're adjacent (overridden for hallucination which does
     its own obfuscation) */
  if (mon->data == &mons[PM_HIGH_PRIEST] && !Hallucination &&
      Is_astralevel(&player.uz) && distu(mon->mx, mon->my) > 2) {
    strcpy(outbuf, article == ARTICLE_THE ? "the " : "");
    strcat(outbuf, mon->female ? "high priestess" : "high priest");
  } else {
    strcpy(outbuf, x_monnam(mon, article, nullptr, 0, TRUE));
  }
  return outbuf;
}

static const char *const bogusmons[] = {
    "jumbo shrimp", "giant pigmy", "gnu", "killer penguin", "giant cockroach",
    "giant slug", "maggot", "pterodactyl", "tyrannosaurus rex", "basilisk",
    "beholder", "nightmare", "efreeti", "marid", "rot grub", "bookworm",
    "master lichen", "shadow", "hologram", "jester", "attorney", "sleazoid",
    "killer tomato", "amazon", "robot", "battlemech", "rhinovirus", "harpy",
    "lion-dog", "rat-ant", "Y2K bug",
    /* misc. */
    "grue", "Christmas-tree monster", "luck sucker", "paskald", "brogmoid",
    "dornbeast", /* Quendor (Zork, &c.) */
    "Ancient Multi-Hued Dragon", "Evil Iggy",
    /* Moria */
    "emu", "kestrel", "xeroc", "venus flytrap",
    /* Rogue */
    "creeping coins",         /* Wizardry */
    "hydra", "siren",         /* Greek legend */
    "killer bunny",           /* Monty Python */
    "rodent of unusual size", /* The Princess Bride */
    "Smokey the bear",        /* "Only you can prevent forest fires!" */
    "Luggage",                /* Discworld */
    "Ent",                    /* Lord of the Rings */
    "tangle tree", "nickelpede", "wiggle", /* Xanth */
    "white rabbit", "snark",               /* Lewis Carroll */
    "pushmi-pullyu",                       /* Dr. Doolittle */
    "smurf",                               /* The Smurfs */
    "tribble", "Klingon", "Borg",          /* Star Trek */
    "Ewok",                                /* Star Wars */
    "Totoro",                              /* Tonari no Totoro */
    "ohmu",                                /* Nausicaa */
    "youma",                               /* Sailor Moon */
    "nyaasu",                              /* Pokemon (Meowth) */
    "Godzilla", "King Kong",               /* monster movies */
    "earthquake beast",                    /* old L of SH */
    "Invid",                               /* Robotech */
    "Terminator",                          /* The Terminator */
    "boomer",                              /* Bubblegum Crisis */
    "Dalek",                               /* Dr. Who ("Exterminate!") */
    "microscopic space fleet", "Ravenous Bugblatter Beast of Traal",
    /* HGttG */
    "teenage mutant ninja turtle", /* TMNT */
    "samurai rabbit",              /* Usagi Yojimbo */
    "aardvark",                    /* Cerebus */
    "Audrey II",                   /* Little Shop of Horrors */
    "witch doctor", "one-eyed one-horned flying purple people eater",
    /* 50's rock 'n' roll */
    "Barney the dinosaur", /* saccharine kiddy TV */
    "Morgoth",             /* Angband */
    "Vorlon",              /* Babylon 5 */
    "questing beast",      /* King Arthur */
    "Predator",            /* Movie */
    "mother-in-law"        /* common pest */
};

/* Return a random monster name, for hallucination.
 * KNOWN BUG: May be a proper name (Godzilla, Barney), may not
 * (the Terminator, a Dalek).  There's no elegant way to deal
 * with this without radically modifying the calling functions.
 */
const char *rndmonnam() {
  int name;

  do {
    name = rn1(SPECIAL_PM + SIZE(bogusmons) - LOW_PM, LOW_PM);
  } while (name < SPECIAL_PM &&
           (type_is_pname(&mons[name]) || (mons[name].geno & G_NOGEN)));

  if (name >= SPECIAL_PM)
    return bogusmons[name - SPECIAL_PM];
  return mons[name].mname;
}

#ifdef REINCARNATION
/* Name of a Rogue player */
const char *roguename() {
  char *i, *opts;

  if ((opts = nh_getenv("ROGUEOPTS")) != 0) {
    for (i = opts; *i; i++)
      if (!strncmp("name=", i, 5)) {
        char *j;
        if ((j = index(i + 5, ',')) != 0)
          *j = (char)0;
        return i + 5;
      }
  }
  return rn2(3) ? (rn2(2) ? "Michael Toy" : "Kenneth Arnold") : "Glenn Wichman";
}
#endif /* REINCARNATION */
#endif /* OVLB */


static const char *const hcolors[] = {
    "ultraviolet",  "infrared",          "bluish-orange", "reddish-green",
    "dark white",   "light black",       "sky blue-pink", "salty",
    "sweet",        "sour",              "bitter",        "striped",
    "spiral",       "swirly",            "plaid",         "checkered",
    "argyle",       "paisley",           "blotchy",       "guernsey-spotted",
    "polka-dotted", "square",            "round",         "triangular",
    "cabernet",     "sangria",           "fuchsia",       "wisteria",
    "lemon-lime",   "strawberry-banana", "peppermint",    "romantic",
    "incandescent"};

const char *hcolor(const char *colorpref) {
  return (Hallucination || !colorpref) ? hcolors[rn2(SIZE(hcolors))]
                                       : colorpref;
}

/* return a random real color unless hallucinating */
const char *rndcolor() {
  int k = rn2(CLR_MAX);
  return Hallucination ? hcolor(nullptr) : (k == NO_COLOR) ? "colorless"
                                                             : c_obj_colors[k];
}

/* Aliases for road-runner nemesis
 */
static const char *const coynames[] = {
    "Carnivorous Vulgaris",      "Road-Runnerus Digestus",
    "Eatibus Anythingus",        "Famishus-Famishus",
    "Eatibus Almost Anythingus", "Eatius Birdius",
    "Famishius Fantasticus",     "Eternalii Famishiis",
    "Famishus Vulgarus",         "Famishius Vulgaris Ingeniusi",
    "Eatius-Slobbius",           "Hardheadipus Oedipus",
    "Carnivorous Slobbius",      "Hard-Headipus Ravenus",
    "Evereadii Eatibus",         "Apetitius Giganticus",
    "Hungrii Flea-Bagius",       "Overconfidentii Vulgaris",
    "Caninus Nervous Rex",       "Grotesques Appetitus",
    "Nemesis Riduclii",          "Canis latrans"};

char *coyotename(Monster *mtmp, char *buf) {
  if (mtmp && buf) {
    sprintf(buf, "%s - %s", x_monnam(mtmp, ARTICLE_NONE, nullptr, 0, TRUE),
            mtmp->mcan ? coynames[SIZE(coynames) - 1]
                       : coynames[rn2(SIZE(coynames) - 1)]);
  }
  return buf;
}

/*do_name.c*/
