/*	SCCS Id: @(#)youprop.h	3.4	1999/07/02	*/
/* Copyright (c) 1989 Mike Threepoint				  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef YOUPROP_H
#define YOUPROP_H

#include "prop.h"
#include "permonst.h"
#include "mondata.h"
#include "pm.h"


/* KMH, intrinsics patch.
 * Reorganized and rewritten for >32-bit properties.
 * HXxx refers to intrinsic bitfields while in human form.
 * EXxx refers to extrinsic bitfields from worn objects.
 * BXxx refers to the cause of the property being blocked.
 * Xxx refers to any source, including polymorph forms.
 */


#define maybe_polyd(if_so,if_not)	(Upolyd ? (if_so) : (if_not))


/*** Resistances to troubles ***/
/* With intrinsics and extrinsics */
#define HFire_resistance	player.uprops[FIRE_RES].intrinsic
#define EFire_resistance	player.uprops[FIRE_RES].extrinsic
#define Fire_resistance		(HFire_resistance || EFire_resistance || \
				 resists_fire(&youmonst))

#define HCold_resistance	player.uprops[COLD_RES].intrinsic
#define ECold_resistance	player.uprops[COLD_RES].extrinsic
#define Cold_resistance		(HCold_resistance || ECold_resistance || \
				 resists_cold(&youmonst))

#define HSleep_resistance	player.uprops[SLEEP_RES].intrinsic
#define ESleep_resistance	player.uprops[SLEEP_RES].extrinsic
#define Sleep_resistance	(HSleep_resistance || ESleep_resistance || \
				 resists_sleep(&youmonst))

#define HDisint_resistance	player.uprops[DISINT_RES].intrinsic
#define EDisint_resistance	player.uprops[DISINT_RES].extrinsic
#define Disint_resistance	(HDisint_resistance || EDisint_resistance || \
				 resists_disint(&youmonst))

#define HShock_resistance	player.uprops[SHOCK_RES].intrinsic
#define EShock_resistance	player.uprops[SHOCK_RES].extrinsic
#define Shock_resistance	(HShock_resistance || EShock_resistance || \
				 resists_elec(&youmonst))

#define HPoison_resistance	player.uprops[POISON_RES].intrinsic
#define EPoison_resistance	player.uprops[POISON_RES].extrinsic
#define Poison_resistance	(HPoison_resistance || EPoison_resistance || \
				 resists_poison(&youmonst))

#define HDrain_resistance	player.uprops[DRAIN_RES].intrinsic
#define EDrain_resistance	player.uprops[DRAIN_RES].extrinsic
#define Drain_resistance	(HDrain_resistance || EDrain_resistance || \
				 resists_drli(&youmonst))

/* Intrinsics only */
#define HSick_resistance	player.uprops[SICK_RES].intrinsic
#define Sick_resistance		(HSick_resistance || \
				 youmonst.data->mlet == S_FUNGUS || \
				 youmonst.data == &mons[PM_GHOUL] || \
				 defends(AD_DISE,uwep))
#define Invulnerable		player.uprops[INVULNERABLE].intrinsic    /* [Tom] */

/* Extrinsics only */
#define EAntimagic		player.uprops[ANTIMAGIC].extrinsic
#define Antimagic		(EAntimagic || \
				 (Upolyd && resists_magm(&youmonst)))

#define EAcid_resistance	player.uprops[ACID_RES].extrinsic
#define Acid_resistance		(EAcid_resistance || resists_acid(&youmonst))

#define EStone_resistance	player.uprops[STONE_RES].extrinsic
#define Stone_resistance	(EStone_resistance || resists_ston(&youmonst))


/*** Troubles ***/
/* Pseudo-property */
#define Punished		(uball)

/* Those implemented solely as timeouts (we use just intrinsic) */
#define HStun			player.uprops[STUNNED].intrinsic
#define Stunned			(HStun || player.umonnum == PM_STALKER || \
				 youmonst.data->mlet == S_BAT)
		/* Note: birds will also be stunned */

#define HConfusion		player.uprops[CONFUSION].intrinsic
#define Confusion		HConfusion

#define Blinded			player.uprops[BLINDED].intrinsic
#define Blindfolded		(ublindf && ublindf->otyp != LENSES)
		/* ...means blind because of a cover */
#define Blind	((Blinded || Blindfolded || !haseyes(youmonst.data)) && \
		 !(ublindf && ublindf->oartifact == ART_EYES_OF_THE_OVERWORLD))
		/* ...the Eyes operate even when you really are blind
		    or don't have any eyes */

#define Sick			player.uprops[SICK].intrinsic
#define Stoned			player.uprops[STONED].intrinsic
#define Strangled		player.uprops[STRANGLED].intrinsic
#define Vomiting		player.uprops[VOMITING].intrinsic
#define Glib			player.uprops[GLIB].intrinsic
#define Slimed			player.uprops[SLIMED].intrinsic	/* [Tom] */

/* Hallucination is solely a timeout; its resistance is extrinsic */
#define HHallucination		player.uprops[HALLUC].intrinsic
#define EHalluc_resistance	player.uprops[HALLUC_RES].extrinsic
#define Halluc_resistance	(EHalluc_resistance || \
				 (Upolyd && dmgtype(youmonst.data, AD_HALU)))
#define Hallucination		(HHallucination && !Halluc_resistance)

/* Timeout, plus a worn mask */
#define HFumbling		player.uprops[FUMBLING].intrinsic
#define EFumbling		player.uprops[FUMBLING].extrinsic
#define Fumbling		(HFumbling || EFumbling)

#define HWounded_legs		player.uprops[WOUNDED_LEGS].intrinsic
#define EWounded_legs		player.uprops[WOUNDED_LEGS].extrinsic
#define Wounded_legs		(HWounded_legs || EWounded_legs)

#define HSleeping		player.uprops[SLEEPING].intrinsic
#define ESleeping		player.uprops[SLEEPING].extrinsic
#define Sleeping		(HSleeping || ESleeping)

#define HHunger			player.uprops[HUNGER].intrinsic
#define EHunger			player.uprops[HUNGER].extrinsic
#define Hunger			(HHunger || EHunger)


/*** Vision and senses ***/
#define HSee_invisible		player.uprops[SEE_INVIS].intrinsic
#define ESee_invisible		player.uprops[SEE_INVIS].extrinsic
#define See_invisible		(HSee_invisible || ESee_invisible || \
				 perceives(youmonst.data))

#define HTelepat		player.uprops[TELEPAT].intrinsic
#define ETelepat		player.uprops[TELEPAT].extrinsic
#define Blind_telepat		(HTelepat || ETelepat || \
				 telepathic(youmonst.data))
#define Unblind_telepat		(ETelepat)

#define HWarning		player.uprops[WARNING].intrinsic
#define EWarning		player.uprops[WARNING].extrinsic
#define Warning			(HWarning || EWarning)

/* Warning for a specific type of monster */
#define HWarn_of_mon		player.uprops[WARN_OF_MON].intrinsic
#define EWarn_of_mon		player.uprops[WARN_OF_MON].extrinsic
#define Warn_of_mon		(HWarn_of_mon || EWarn_of_mon)

#define HUndead_warning		player.uprops[WARN_UNDEAD].intrinsic
#define Undead_warning		(HUndead_warning)

#define HSearching		player.uprops[SEARCHING].intrinsic
#define ESearching		player.uprops[SEARCHING].extrinsic
#define Searching		(HSearching || ESearching)

#define HClairvoyant		player.uprops[CLAIRVOYANT].intrinsic
#define EClairvoyant		player.uprops[CLAIRVOYANT].extrinsic
#define BClairvoyant		player.uprops[CLAIRVOYANT].blocked
#define Clairvoyant		((HClairvoyant || EClairvoyant) &&\
				 !BClairvoyant)

#define HInfravision		player.uprops[INFRAVISION].intrinsic
#define EInfravision		player.uprops[INFRAVISION].extrinsic
#define Infravision		(HInfravision || EInfravision || \
				  infravision(youmonst.data))

#define HDetect_monsters	player.uprops[DETECT_MONSTERS].intrinsic
#define EDetect_monsters	player.uprops[DETECT_MONSTERS].extrinsic
#define Detect_monsters		(HDetect_monsters || EDetect_monsters)


/*** Appearance and behavior ***/
#define Adornment		player.uprops[ADORNED].extrinsic

#define HInvis			player.uprops[INVIS].intrinsic
#define EInvis			player.uprops[INVIS].extrinsic
#define BInvis			player.uprops[INVIS].blocked
#define Invis			((HInvis || EInvis || \
				 pm_invisible(youmonst.data)) && !BInvis)
#define Invisible		(Invis && !See_invisible)
		/* Note: invisibility also hides inventory and steed */

#define EDisplaced		player.uprops[DISPLACED].extrinsic
#define Displaced		EDisplaced

#define HStealth		player.uprops[STEALTH].intrinsic
#define EStealth		player.uprops[STEALTH].extrinsic
#define BStealth		player.uprops[STEALTH].blocked
#define Stealth			((HStealth || EStealth) && !BStealth)

#define HAggravate_monster	player.uprops[AGGRAVATE_MONSTER].intrinsic
#define EAggravate_monster	player.uprops[AGGRAVATE_MONSTER].extrinsic
#define Aggravate_monster	(HAggravate_monster || EAggravate_monster)

#define HConflict		player.uprops[CONFLICT].intrinsic
#define EConflict		player.uprops[CONFLICT].extrinsic
#define Conflict		(HConflict || EConflict)


/*** Transportation ***/
#define HJumping		player.uprops[JUMPING].intrinsic
#define EJumping		player.uprops[JUMPING].extrinsic
#define Jumping			(HJumping || EJumping)

#define HTeleportation		player.uprops[TELEPORT].intrinsic
#define ETeleportation		player.uprops[TELEPORT].extrinsic
#define Teleportation		(HTeleportation || ETeleportation || \
				 can_teleport(youmonst.data))

#define HTeleport_control	player.uprops[TELEPORT_CONTROL].intrinsic
#define ETeleport_control	player.uprops[TELEPORT_CONTROL].extrinsic
#define Teleport_control	(HTeleport_control || ETeleport_control || \
				 control_teleport(youmonst.data))

#define HLevitation		player.uprops[LEVITATION].intrinsic
#define ELevitation		player.uprops[LEVITATION].extrinsic
#define Levitation		(HLevitation || ELevitation || \
				 is_floater(youmonst.data))
	/* Can't touch surface, can't go under water; overrides all others */
#define Lev_at_will		(((HLevitation & I_SPECIAL) != 0L || \
				 (ELevitation & W_ARTI) != 0L) && \
				 (HLevitation & ~(I_SPECIAL|TIMEOUT)) == 0L && \
				 (ELevitation & ~W_ARTI) == 0L && \
				 !is_floater(youmonst.data))

#define EFlying			player.uprops[FLYING].extrinsic
#ifdef STEED
# define Flying			(EFlying || is_flyer(youmonst.data) || \
				 (player.usteed && is_flyer(player.usteed->data)))
#else
# define Flying			(EFlying || is_flyer(youmonst.data))
#endif
	/* May touch surface; does not override any others */

#define Wwalking		(player.uprops[WWALKING].extrinsic && \
				 !Is_waterlevel(&player.uz))
	/* Don't get wet, can't go under water; overrides others except levitation */
	/* Wwalking is meaningless on water level */

#define HSwimming		player.uprops[SWIMMING].intrinsic
#define ESwimming		player.uprops[SWIMMING].extrinsic	/* [Tom] */
#ifdef STEED
# define Swimming		(HSwimming || ESwimming || \
				 is_swimmer(youmonst.data) || \
				 (player.usteed && is_swimmer(player.usteed->data)))
#else
# define Swimming		(HSwimming || ESwimming || \
				 is_swimmer(youmonst.data))
#endif
	/* Get wet, don't go under water unless if amphibious */

#define HMagical_breathing	player.uprops[MAGICAL_BREATHING].intrinsic
#define EMagical_breathing	player.uprops[MAGICAL_BREATHING].extrinsic
#define Amphibious		(HMagical_breathing || EMagical_breathing || \
				 amphibious(youmonst.data))
	/* Get wet, may go under surface */

#define Breathless		(HMagical_breathing || EMagical_breathing || \
				 breathless(youmonst.data))

#define Underwater		(player.uinwater)
/* Note that Underwater and player.uinwater are both used in code.
   The latter form is for later implementation of other in-water
   states, like swimming, wading, etc. */

#define HPasses_walls		player.uprops[PASSES_WALLS].intrinsic
#define EPasses_walls		player.uprops[PASSES_WALLS].extrinsic
#define Passes_walls		(HPasses_walls || EPasses_walls || \
				 passes_walls(youmonst.data))


/*** Physical attributes ***/
#define HSlow_digestion		player.uprops[SLOW_DIGESTION].intrinsic
#define ESlow_digestion		player.uprops[SLOW_DIGESTION].extrinsic
#define Slow_digestion		(HSlow_digestion || ESlow_digestion)  /* KMH */

#define HHalf_spell_damage	player.uprops[HALF_SPDAM].intrinsic
#define EHalf_spell_damage	player.uprops[HALF_SPDAM].extrinsic
#define Half_spell_damage	(HHalf_spell_damage || EHalf_spell_damage)

#define HHalf_physical_damage	player.uprops[HALF_PHDAM].intrinsic
#define EHalf_physical_damage	player.uprops[HALF_PHDAM].extrinsic
#define Half_physical_damage	(HHalf_physical_damage || EHalf_physical_damage)

#define HRegeneration		player.uprops[REGENERATION].intrinsic
#define ERegeneration		player.uprops[REGENERATION].extrinsic
#define Regeneration		(HRegeneration || ERegeneration || \
				 regenerates(youmonst.data))

#define HEnergy_regeneration	player.uprops[ENERGY_REGENERATION].intrinsic
#define EEnergy_regeneration	player.uprops[ENERGY_REGENERATION].extrinsic
#define Energy_regeneration	(HEnergy_regeneration || EEnergy_regeneration)

#define HProtection		player.uprops[PROTECTION].intrinsic
#define EProtection		player.uprops[PROTECTION].extrinsic
#define Protection		(HProtection || EProtection)

#define HProtection_from_shape_changers \
				player.uprops[PROT_FROM_SHAPE_CHANGERS].intrinsic
#define EProtection_from_shape_changers \
				player.uprops[PROT_FROM_SHAPE_CHANGERS].extrinsic
#define Protection_from_shape_changers \
				(HProtection_from_shape_changers || \
				 EProtection_from_shape_changers)

#define HPolymorph		player.uprops[POLYMORPH].intrinsic
#define EPolymorph		player.uprops[POLYMORPH].extrinsic
#define Polymorph		(HPolymorph || EPolymorph)

#define HPolymorph_control	player.uprops[POLYMORPH_CONTROL].intrinsic
#define EPolymorph_control	player.uprops[POLYMORPH_CONTROL].extrinsic
#define Polymorph_control	(HPolymorph_control || EPolymorph_control)

#define HUnchanging		player.uprops[UNCHANGING].intrinsic
#define EUnchanging		player.uprops[UNCHANGING].extrinsic
#define Unchanging		(HUnchanging || EUnchanging)	/* KMH */

#define HFast			player.uprops[FAST].intrinsic
#define EFast			player.uprops[FAST].extrinsic
#define Fast			(HFast || EFast)
#define Very_fast		((HFast & ~INTRINSIC) || EFast)

#define EReflecting		player.uprops[REFLECTING].extrinsic
#define Reflecting		(EReflecting || \
				 (youmonst.data == &mons[PM_SILVER_DRAGON]))

#define Free_action		player.uprops[FREE_ACTION].extrinsic /* [Tom] */

#define Fixed_abil		player.uprops[FIXED_ABIL].extrinsic	/* KMH */

#define Lifesaved		player.uprops[LIFESAVED].extrinsic


#endif /* YOUPROP_H */
