 /*********************************************************************************** 
 *                                                                                  *
 *          _______.____    __    ____       _______                  _______       *
 *         /       |\   \  /  \  /   /  _   |   ____|          __    |   ____|      *
 *        |   (----` \   \/    \/   /  (_)  |  |__    ____   _/  |_  |  |__         *
 *         \   \      \            /    _   |   __|  /  _ \  \   __\ |   __|        *
 *     .----)   |      \    /\    /    (_)  |  |    (  <_> )  |  |   |  |____       *
 *     |_______/        \__/  \__/          |__|     \____/   |__|   |_______|      *
 *                                                                                  *
 * SWFotE v2.0 (FotE v1.1 cleaned up and considerably modded)  by:                  *
 * Greg (Keberus) Mosley                                                            *
 * Roman (Trelar) Arnold                                                            *
 *                                                                                  *
 * SWFotE v1 & v1.1 copyright (c) 2002 was created by                               *
 * Chris 'Tawnos' Dary (cadary@uwm.edu),                                            *
 * Korey 'Eleven' King (no email),                                                  *
 * Matt 'Trillen' White (mwhite17@ureach.com),                                      *
 * Daniel 'Danimal' Berrill (danimal924@yahoo.com),                                 *
 * Richard 'Bambua' Berrill (email unknown),                                        *
 * Stuart 'Ackbar' Unknown (email unknown)                                          *
 *                                                                                  *
 * SWR 1.0 copyright (c) 1997, 1998 was created by Sean Cooper                      *
 * based on a concept and ideas from the original SWR immortals:                    *
 * Himself (Durga), Mark Matt (Merth), Jp Coldarone (Exar), Greg Baily (Thrawn),    *
 * Ackbar, Satin, Streen and Bib as well as much input from our other builders      *
 * and players.                                                                     *
 *                                                                                  *
 * Original SMAUG 1.4a written by Thoric (Derek Snider) with Altrag,                *
 * Blodkai, Haus, Narn, Scryn, Swordbearer, Tricops, Gorog, Rennard,                *
 * Grishnakh, Fireblade, and Nivek.                                                 *
 *                                                                                  *
 * Original MERC 2.1 code by Hatchet, Furey, and Kahn.                              *
 *                                                                                  *
 * Original DikuMUD code by: Hans Staerfeldt, Katja Nyboe, Tom Madsen,              *
 * Michael Seifert, and Sebastian Hammer.                                           *
 *                                                                                  *
 ***********************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>


#ifdef CYGWIN //CYGWIN support added by Keberus
  #include <sys/dirent.h> //Thanks Xie for the help
#else
  #include <sys/dir.h>
#endif
#include "mud.h"

extern char		lastplayercmd[MAX_INPUT_LENGTH];
extern CHAR_DATA *	gch_prev;

/* From Skills.c */
int ris_save( CHAR_DATA *ch, int chance, int ris );

/* From newarena.c */
void lost_arena(CHAR_DATA *ch);

/* From space.c */
void remship(SHIP_DATA *ship);

/* From comm.c */
void    name_log args( ( const char *str, ...) );

/*
 * Local functions.
 */
void	dam_message	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
			    int dt ) );
void	death_cry	args( ( CHAR_DATA *ch ) );
void	group_gain	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int	xp_compute	args( ( CHAR_DATA *gch, CHAR_DATA *victim ) );
int	align_compute	args( ( CHAR_DATA *gch, CHAR_DATA *victim ) );
ch_ret	one_hit		args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
int	obj_hitroll	args( ( OBJ_DATA *obj ) );
bool    get_cover( CHAR_DATA *ch );
bool	dual_flip = FALSE;
char *rand_death( CHAR_DATA * ch );


bool loot_coins_from_corpse( CHAR_DATA *ch, OBJ_DATA *corpse )
{
   OBJ_DATA *content, *content_next;
   int oldgold = ch->gold;

   for( content = corpse->first_content; content; content = content_next )
   {
      content_next = content->next_content;

      if( content->item_type != ITEM_MONEY )
         continue;
      if( !can_see_obj( ch, content ) )
         continue;
      if( !CAN_WEAR( content, ITEM_TAKE ) && ch->top_level < sysdata.level_getobjnotake )
         continue;
      if( IS_OBJ_STAT( content, ITEM_PROTOTYPE ) && !can_take_proto( ch ) )
         continue;

      act( AT_ACTION, "Coges $p de $P", ch, content, corpse, TO_CHAR );
      act( AT_ACTION, "$n coge $p de $P", ch, content, corpse, TO_ROOM );
      obj_from_obj( content );
      check_for_trap( ch, content, TRAP_GET );
      if( char_died( ch ) )
         return FALSE;

      oprog_get_trigger( ch, content );
      if( char_died( ch ) )
         return FALSE;

      ch->gold += content->value[0] * content->count;
      extract_obj( content );
   }

   if( ch->gold - oldgold > 1 && ch->position > POS_SLEEPING )
   {
      char buf[MAX_INPUT_LENGTH];

      snprintf( buf, MAX_INPUT_LENGTH, "%d", ch->gold - oldgold );
      do_split( ch, buf );
   }
   return TRUE;
}

/*
 * Check to see if weapon is poisoned.
 */
bool is_wielding_poisoned( CHAR_DATA *ch )
{
         OBJ_DATA *obj;

         if ( ( obj = get_eq_char( ch, WEAR_WIELD ) 	)
         &&   (IS_SET( obj->extra_flags, ITEM_POISONED) )	)
                  return TRUE;

         return FALSE;

}

/*
 * hunting, hating and fearing code				-Thoric
 */
bool is_hunting( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( !ch->hunting || ch->hunting->who != victim )
      return FALSE;
    
    return TRUE;    
}

bool is_hating( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( !ch->hating || ch->hating->who != victim )
      return FALSE;
    
    return TRUE;    
}

bool is_fearing( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( !ch->fearing || ch->fearing->who != victim )
      return FALSE;
    
    return TRUE;    
}

void stop_hunting( CHAR_DATA *ch )
{
    if ( ch->hunting )
    {
	STRFREE( ch->hunting->name );
	DISPOSE( ch->hunting );
	ch->hunting = NULL;
    }
    return;
}

void stop_hating( CHAR_DATA *ch )
{
    if ( ch->hating )
    {
	STRFREE( ch->hating->name );
	DISPOSE( ch->hating );
	ch->hating = NULL;
    }
    return;
}

void stop_fearing( CHAR_DATA *ch )
{
    if ( ch->fearing )
    {
	STRFREE( ch->fearing->name );
	DISPOSE( ch->fearing );
	ch->fearing = NULL;
    }
    return;
}

void start_hunting( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( ch->hunting )
      stop_hunting( ch );

    CREATE( ch->hunting, HHF_DATA, 1 );
    ch->hunting->name = QUICKLINK( victim->name );
    ch->hunting->who  = victim;
    return;
}

void start_hating( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( ch->hating )
      stop_hating( ch );

    CREATE( ch->hating, HHF_DATA, 1 );
    ch->hating->name = QUICKLINK( victim->name );
    ch->hating->who  = victim;
    return;
}

void start_fearing( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( ch->fearing )
      stop_fearing( ch );

    CREATE( ch->fearing, HHF_DATA, 1 );
    ch->fearing->name = QUICKLINK( victim->name );
    ch->fearing->who  = victim;
    return;
}


int max_fight( CHAR_DATA *ch )
{
    return 8;
}

/*
 * Control the fights going on.
 * Called periodically by update_handler.
 * Many hours spent fixing bugs in here by Thoric, as noted by residual
 * debugging checks.  If you never get any of these error messages again
 * in your logs... then you can comment out some of the checks without
 * worry.
 */
void violence_update( void )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *ch;
    CHAR_DATA *lst_ch;
    CHAR_DATA *victim;
    CHAR_DATA *rch, *rch_next;
    AFFECT_DATA *paf, *paf_next;
    TIMER	*timer, *timer_next;
    ch_ret     retcode;
    SKILLTYPE	*skill;

    lst_ch = NULL;
    for ( ch = last_char; ch; lst_ch = ch, ch = gch_prev )
    {
	set_cur_char( ch );

	if ( ch == first_char && ch->prev )
	{
	   bug( "ERROR: first_char->prev != NULL, fixing...", 0 );
	   ch->prev = NULL;
	}

	gch_prev	= ch->prev;

	if ( gch_prev && gch_prev->next != ch )
	{
	    sprintf( buf, "FATAL: violence_update: %s->prev->next doesn't point to ch.",
		ch->name );
	    bug( buf, 0 );	    
	    bug( "Short-cutting here", 0 );
	    ch->prev = NULL;
	    gch_prev = NULL;
	    do_shout( ch, "Thoric says, 'Prepare for the worst!'" );
	}

	/*
	 * See if we got a pointer to someone who recently died...
	 * if so, either the pointer is bad... or it's a player who
	 * "died", and is back at the healer...
	 * Since he/she's in the char_list, it's likely to be the later...
	 * and should not already be in another fight already
	 */
	if ( char_died(ch) )
	    continue;

	/*
	 * See if we got a pointer to some bad looking data...
	 */
	if ( !ch->in_room || !ch->name )
	{
	    log_string( "violence_update: bad ch record!  (Shortcutting.)" );
	    sprintf( buf, "ch: %ld  ch->in_room: %ld  ch->prev: %ld  ch->next: %ld",
	    	(long) ch, (long) ch->in_room, (long) ch->prev, (long) ch->next );
	    log_string( buf );
	    log_string( lastplayercmd );
	    if ( lst_ch )
	      sprintf( buf, "lst_ch: %ld  lst_ch->prev: %ld  lst_ch->next: %ld",
	      		(long) lst_ch, (long) lst_ch->prev, (long) lst_ch->next );
	    else
	      strcpy( buf, "lst_ch: NULL" );
	    log_string( buf );
	    gch_prev = NULL;
	    continue;
	}

        /*
         * Experience gained during battle deceases as battle drags on
         */
	if ( ch->fighting )
	  if ( (++ch->fighting->duration % 24) == 0 )
	    ch->fighting->xp = ((ch->fighting->xp * 9) / 10);


	for ( timer = ch->first_timer; timer; timer = timer_next )
	{
	    timer_next = timer->next;
	    if ( --timer->count <= 0 )
	    {
		if ( timer->type == TIMER_DO_FUN )
		{
		    int tempsub;

		    tempsub = ch->substate;
		    ch->substate = timer->value;
		    (timer->do_fun)( ch, "" );
		    if ( char_died(ch) )
		      break;
		    ch->substate = tempsub;
		}
		extract_timer( ch, timer );
	    }
	}

	if ( char_died(ch) )
	  continue;

	/*
	 * We need spells that have shorter durations than an hour.
	 * So a melee round sounds good to me... -Thoric
	 */
	for ( paf = ch->first_affect; paf; paf = paf_next )
	{
	      paf_next	= paf->next;
	      if ( paf->duration > 0 )
		paf->duration--;
	      else
	      if ( paf->duration < 0 )
		;
	      else
	      {
		  if ( !paf_next
		  ||    paf_next->type != paf->type
		  ||    paf_next->duration > 0 )
		  {
		      skill = get_skilltype(paf->type);
		      if ( paf->type > 0 && skill && skill->msg_off )
		      {
                          set_char_color( AT_WEAROFF, ch );
			  send_to_char( skill->msg_off, ch );
			  send_to_char( "\n\r", ch );
		      }
		  }
		  if (paf->type == gsn_possess && paf->type != gsn_blindness)
	          {
	            bug(ch->name, 0);
	            ch->desc->character       = ch->desc->original;
    	            ch->desc->original        = NULL;
    		    ch->desc->character->desc = ch->desc;
   	            ch->desc->character->switched = NULL;
   	            ch->switched = NULL;
    		    ch->desc                  = NULL;
		  }
		  affect_remove( ch, paf );
	      }
	}
	
	if ( ( victim = who_fighting( ch ) ) == NULL
	||   IS_AFFECTED( ch, AFF_PARALYSIS ) )
	    continue;

        retcode = rNONE;

	if ( IS_SET(ch->in_room->room_flags, ROOM_SAFE ) )
	{
	   sprintf( buf, "violence_update: %s fighting %s in a SAFE room.",
	   	ch->name, victim->name );
	   log_string( buf );
	   stop_fighting( ch, TRUE );
	}
	else
	if ( IS_AWAKE(ch) && ch->in_room == victim->in_room )
	{
	   if(!IS_NPC(ch))
	     ch->pcdata->lost_attacks = 0;
           retcode = multi_hit( ch, victim, TYPE_UNDEFINED );
	}
	else
	    stop_fighting( ch, FALSE );

	if ( char_died(ch) )
	    continue;

	if ( retcode == rCHAR_DIED
	|| ( victim = who_fighting( ch ) ) == NULL )
	    continue;

	/*
	 *  Mob triggers
	 */
	rprog_rfight_trigger( ch );
	if ( char_died(ch) )
	    continue;
	mprog_hitprcnt_trigger( ch, victim );
	if ( char_died(ch) )
	    continue;
	mprog_fight_trigger( ch, victim );
	if ( char_died(ch) )
	    continue;

	/*
	 * Fun for the whole family!
	 */
	for ( rch = ch->in_room->first_person; rch; rch = rch_next )
	{
	    rch_next = rch->next_in_room;

	    if ( IS_AWAKE(rch) && !rch->fighting )
	    {
		/*
		 * PC's auto-assist others in their group.
		 */
		if ( !IS_NPC(ch) || IS_AFFECTED(ch, AFF_CHARM) )
		{
		    if ( ( !IS_NPC(rch) || IS_AFFECTED(rch, AFF_CHARM) )
		    &&   is_same_group(ch, rch) )
			multi_hit( rch, victim, TYPE_UNDEFINED );
		    continue;
		}

		/*
		 * NPC's assist NPC's of same type or 12.5% chance regardless.
		 */
		if ( IS_NPC(rch) && !IS_AFFECTED(rch, AFF_CHARM)
		&&  !IS_SET(rch->act, ACT_NOASSIST) )
		{
		    if ( char_died(ch) )
			break;
		    if ( rch->pIndexData == ch->pIndexData
		    ||   number_bits( 3 ) == 0 )
		    {
			CHAR_DATA *vch;
			CHAR_DATA *target;
			int number;

			target = NULL;
			number = 0;			for ( vch = ch->in_room->first_person; vch; vch = vch->next )
			{
			    if ( can_see( rch, vch )
			    &&   is_same_group( vch, victim )
			    &&   number_range( 0, number ) == 0 )
			    {
				target = vch;
				number++;
			    }
			}

			if ( target )
			    multi_hit( rch, target, TYPE_UNDEFINED );
		    }
		}
	    }
	}
    }

    return;
}



/*
 * Do one group of attacks.
 */
ch_ret multi_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    int     chance;
    int	    dual_bonus;
    int	    extrahits;
    ch_ret  retcode;
    
    
    /* add timer if player is attacking another player */
    if ( !IS_NPC(ch) && !IS_NPC(victim) )
      add_timer( ch, TIMER_RECENTFIGHT, 20, NULL, 0 );

    if ( !IS_NPC(ch) && IS_SET( ch->act, PLR_NICE ) && !IS_NPC( victim ) )
      return rNONE;

    if ( (retcode = one_hit( ch, victim, dt )) != rNONE )
      return retcode;

    if ( who_fighting( ch ) != victim || dt == gsn_backstab || dt == gsn_circle || dt == gsn_dualstab)
	return rNONE;
	
    /* Very high chance of hitting compared to chance of going berserk */
    /* 40% or higher is always hit.. don't learn anything here though. */
    /* -- Altrag */
    chance = IS_NPC(ch) ? 100 : (ch->pcdata->learned[gsn_berserk]*5/2);
    if ( IS_AFFECTED(ch, AFF_BERSERK) && number_percent() < chance )
      if ( (retcode = one_hit( ch, victim, dt )) != rNONE ||
            who_fighting( ch ) != victim )
        return retcode;

    if ( get_eq_char( ch, WEAR_DUAL_WIELD ) )
    {
      dual_bonus = IS_NPC(ch) ? (ch->skill_level[COMBAT_ABILITY] / 10) : (ch->pcdata->learned[gsn_dual_wield] / 10);
      chance = IS_NPC(ch) ? ch->top_level : ch->pcdata->learned[gsn_dual_wield];
      if ( number_percent( ) < chance )
      {
	learn_from_success( ch, gsn_dual_wield );
	retcode = one_hit( ch, victim, dt );
	if ( retcode != rNONE || who_fighting( ch ) != victim )
	    return retcode;
      }
      else
	learn_from_failure( ch, gsn_dual_wield );
    }
    else
      dual_bonus = 0;
    
    if (dt == gsn_ambush)
      extrahits = number_range(1,3);
    else if( dt == gsn_dualstab)
      extrahits = 2;
    else
      extrahits = 0;
    
    if (dt == gsn_ambush)
     {
      if( IS_AFFECTED(ch, AFF_SNEAK))
       {
        affect_strip( ch, gsn_sneak );
        REMOVE_BIT   ( ch->affected_by, AFF_SNEAK );
       }

     }
   
    for(chance=0;chance <=extrahits;chance++){
    	retcode = one_hit( ch, victim, dt );
	if ( retcode != rNONE || who_fighting( ch ) != victim )
	  return retcode;
    }
    if ( ch->move < 10 )
      dual_bonus = -20;

    /*
     * NPC predetermined number of attacks			-Thoric
     */
    if ( IS_NPC(ch) && ch->numattacks > 0 )
    {
	for ( chance = 0; chance <= (ch->numattacks); chance++ )
	{
	   retcode = one_hit( ch, victim, dt );
	   if ( retcode != rNONE || who_fighting( ch ) != victim )
	     return retcode;
	}
	return retcode;
    }

    chance = IS_NPC(ch) ? ch->top_level
	   : (int) ((ch->pcdata->learned[gsn_second_attack]+dual_bonus)/1.5);
    if ( number_percent( ) < chance )
    {
	learn_from_success( ch, gsn_second_attack );
	retcode = one_hit( ch, victim, dt );
	if ( retcode != rNONE || who_fighting( ch ) != victim )
	    return retcode;
    }
    else
	learn_from_failure( ch, gsn_second_attack );

    chance = IS_NPC(ch) ? ch->top_level
	   : (int) ((ch->pcdata->learned[gsn_third_attack]+(dual_bonus*1.5))/2);
    if ( number_percent( ) < chance )
    {
	learn_from_success( ch, gsn_third_attack );
	retcode = one_hit( ch, victim, dt );
	if ( retcode != rNONE || who_fighting( ch ) != victim )
	    return retcode;
    }
    else
	learn_from_failure( ch, gsn_third_attack );

    retcode = rNONE;

    chance = IS_NPC(ch) ? (int) (ch->top_level / 4) : 0;
    if ( number_percent( ) < chance )
	retcode = one_hit( ch, victim, dt );

    if ( retcode == rNONE )
    {
	int move;

	if ( !IS_AFFECTED(ch, AFF_FLYING)
	&&   !IS_AFFECTED(ch, AFF_FLOATING) )
	  move = encumbrance( ch, movement_loss[UMIN(SECT_MAX-1, ch->in_room->sector_type)] );
	else
	  move = encumbrance( ch, 1 );
	if ( ch->move )
	  ch->move = UMAX( 0, ch->move - move );
    }
    return retcode;
}


/*
 * Weapon types, haus
 */
int weapon_prof_bonus_check( CHAR_DATA *ch, OBJ_DATA *wield, int *gsn_ptr )
{
    int bonus;

    bonus = 0;	*gsn_ptr = -1;
    if ( !IS_NPC(ch) && wield )   
    {
	switch(wield->value[3])
	{
	   default:	*gsn_ptr = -1;			break;
           case 3:      *gsn_ptr = gsn_lightsabers;     break;
           case 2:	*gsn_ptr = gsn_vibro_blades;	break;
           case 4:	*gsn_ptr = gsn_flexible_arms;	break;
           case 5:	*gsn_ptr = gsn_talonous_arms;	break;
           case 6:	*gsn_ptr = gsn_blasters;	break;
           case 8:	*gsn_ptr = gsn_bludgeons;	break;
           case 9:	*gsn_ptr = gsn_bowcasters;	break;
           case 11:	*gsn_ptr = gsn_force_pikes;	break;

	}
	if ( *gsn_ptr != -1 )
	  bonus = (int) ( ch->pcdata->learned[*gsn_ptr] );

    }
    if ( IS_NPC(ch) && wield )   
          bonus = get_trust(ch);
    return bonus;
}

/*
 * Calculate the tohit bonus on the object and return RIS values.
 * -- Altrag
 */
int obj_hitroll( OBJ_DATA *obj )
{
	int tohit = 0;
	AFFECT_DATA *paf;
	
	for ( paf = obj->pIndexData->first_affect; paf; paf = paf->next )
		if ( paf->location == APPLY_HITROLL )
			tohit += paf->modifier;
	for ( paf = obj->first_affect; paf; paf = paf->next )
		if ( paf->location == APPLY_HITROLL )
			tohit += paf->modifier;
	return tohit;
}

/*
 * Offensive shield level modifier
 */
sh_int off_shld_lvl( CHAR_DATA *ch, CHAR_DATA *victim )
{
    sh_int lvl;

    if ( !IS_NPC(ch) )		/* players get much less effect */
    {
	lvl = UMAX( 1, (ch->skill_level[FORCE_ABILITY])  );
	if ( number_percent() + (victim->skill_level[COMBAT_ABILITY] - lvl) < 35 )
	  return lvl;
	else
	  return 0;
    }
    else
    {
	lvl = ch->top_level;
	if ( number_percent() + (victim->skill_level[COMBAT_ABILITY] - lvl) < 70 )
	  return lvl;
	else
	  return 0;
    }
}

/*
 * Hit one guy once.
 */
ch_ret one_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    OBJ_DATA *wield;
    int victim_ac;
    int thac0;
    int thac0_00;
    int thac0_32;
    int plusris;
    int dam, x;
    int diceroll;
    int attacktype, cnt;
    int	prof_bonus;
    int	prof_gsn;
    ch_ret retcode;
    int chance;
    bool fail;
    AFFECT_DATA af;
                

    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if(!IS_NPC(ch))
    {
      if(ch->pcdata->lost_attacks < 0)
          ch->pcdata->lost_attacks = 0;
      if(ch->pcdata->lost_attacks != 0)
      {
          ch->pcdata->lost_attacks--;
          tail_chain();
        return rNONE;
      }
    }
    if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return rVICT_DIED;
                  

    /*
     * Figure out the weapon doing the damage			-Thoric
     */
    if ( (wield = get_eq_char( ch, WEAR_DUAL_WIELD )) != NULL )
    {
       if ( dual_flip == FALSE )
       {
	 dual_flip = TRUE;
	 wield = get_eq_char( ch, WEAR_WIELD );
       }
       else
	 dual_flip = FALSE;
    }
    else
      wield = get_eq_char( ch, WEAR_WIELD );

    prof_bonus = weapon_prof_bonus_check( ch, wield, &prof_gsn );

    if ( ch->fighting		/* make sure fight is already started */
    &&   dt == TYPE_UNDEFINED
    &&   IS_NPC(ch)
    &&   ch->attacks != 0 )
    {
	cnt = 0;
	for ( ;; )
	{
	   x = number_range( 0, 6 );
	   attacktype = 1 << x;
	   if ( IS_SET( ch->attacks, attacktype ) )
	     break;
	   if ( cnt++ > 16 )
	   {
	     attacktype = 0;
	     break;
	   }
	}
	if ( attacktype == ATCK_BACKSTAB )
	  attacktype = 0;
	if ( wield && number_percent( ) > 25 )
	  attacktype = 0;
	switch ( attacktype )
	{
	  default:
	    break;
	  case ATCK_BITE:
	    do_bite( ch, "" );
	    retcode = global_retcode;
	    break;
	  case ATCK_CLAWS:
	    do_claw( ch, "" );
	    retcode = global_retcode;
	    break;
	  case ATCK_TAIL:
	    do_tail( ch, "" );
	    retcode = global_retcode;
	    break;
	  case ATCK_STING:
	    do_sting( ch, "" );
	    retcode = global_retcode;
	    break;
	  case ATCK_PUNCH:
	    do_punch( ch, "" );
	    retcode = global_retcode;
	    break;
	  case ATCK_KICK:
	    do_kick( ch, "" );
	    retcode = global_retcode;
	    break;
	  case ATCK_TRIP:
	    retcode = 0;
	    break;
	}
	if ( attacktype )
	  return retcode;
    }

    if ( dt == TYPE_UNDEFINED )
    {
	dt = TYPE_HIT;
	if ( wield && wield->item_type == ITEM_WEAPON )
	    dt += wield->value[3];
	    
	if (wield && wield->item_type == ITEM_WEAPON && wield->value[3] == WEAPON_DUAL_LIGHTSABER)
	    dt = (TYPE_HIT + WEAPON_LIGHTSABER);
    }

    /*
     * Calculate to-hit-armor-class-0 versus armor.
     */
    thac0_00 = 20;
    thac0_32 = 10;
    thac0     = interpolate( ch->skill_level[COMBAT_ABILITY] , thac0_00, thac0_32 ) - GET_HITROLL(ch);
    victim_ac = (int) (GET_AC(victim) / 10);

    /* if you can't see what's coming... */
    if ( wield && !can_see_obj( victim, wield) )
	victim_ac += 1;
    if ( !can_see( ch, victim ) )
	victim_ac -= 4;

    if ( ch->race == RACE_DEFEL )
        victim_ac += 2; 

    if ( !IS_AWAKE ( victim ) )
        victim_ac += 5;
    
    /* Weapon proficiency bonus */
    victim_ac += prof_bonus/20;


    /* No more beating up stunned players */
    if ( IS_AFFECTED( victim, AFF_PARALYSIS ) ) 
       affect_strip( ch, gsn_stun );

    /*
     * The moment of excitement!
     */
    diceroll = number_range( 1,20 );

    if ( diceroll == 1
    || ( diceroll < 20 && diceroll < thac0 - victim_ac ) )
    {
	/* Miss. */
	if ( prof_gsn != -1 )
	  learn_from_failure( ch, prof_gsn );
	damage( ch, victim, 0, dt );
	tail_chain( );
	return rNONE;
    }

    /*
     * Hit.
     * Calc damage.
     */

    if ( !wield )       /* dice formula fixed by Thoric */
	dam = number_range( ch->barenumdie, ch->baresizedie * ch->barenumdie ) + ch->damplus;
    else
	dam = number_range( wield->value[1], wield->value[2] );
	
    /*
     * Bonuses.
     */
     
    dam += GET_DAMROLL(ch);
    
    if ( prof_bonus )
      dam *= ( 1 + prof_bonus / 100 );
    

    if ( !IS_NPC(ch) && ch->pcdata->learned[gsn_enhanced_damage] > 0 )
    {
	dam += (int) (dam * ch->pcdata->learned[gsn_enhanced_damage] / 120);
	learn_from_success( ch, gsn_enhanced_damage );
    }
    
    
    if ( !IS_AWAKE(victim) )
	dam *= 2;
    if ( dt == gsn_backstab || dt == gsn_dualstab)
	dam *= (2 + URANGE( 2, ch->skill_level[HUNTING_ABILITY] - (victim->skill_level[COMBAT_ABILITY]/4), 30 ) / 8);
 
    if ( dt == gsn_circle )
 	dam *= (2 + URANGE( 2, ch->skill_level[HUNTING_ABILITY] - (victim->skill_level[COMBAT_ABILITY]/4), 30 ) / 16); 
    
    plusris = 0;

    if ( wield )
    {
      if ( IS_SET( wield->extra_flags, ITEM_MAGIC ) )
        dam = ris_damage( victim, dam, RIS_MAGIC );
      else
        dam = ris_damage( victim, dam, RIS_NONMAGIC );
    
      /*
       * Handle PLUS1 - PLUS6 ris bits vs. weapon hitroll	-Thoric
       */
      plusris = obj_hitroll( wield );
    }
    else
      dam = ris_damage( victim, dam, RIS_NONMAGIC );

    /* check for RIS_PLUSx 					-Thoric */
    if ( dam )
    {
	int x, res, imm, sus, mod;

	if ( plusris )
	   plusris = RIS_PLUS1 << UMIN(plusris, 7);

	/* initialize values to handle a zero plusris */
	imm = res = -1;  sus = 1;

	/* find high ris */
	for ( x = RIS_PLUS1; x <= RIS_PLUS6; x <<= 1 )
	{
	   if ( IS_SET( victim->immune, x ) )
		imm = x;
	   if ( IS_SET( victim->resistant, x ) )
		res = x;
	   if ( IS_SET( victim->susceptible, x ) )
		sus = x;
	}
	mod = 10;
	if ( imm >= plusris )
	  mod -= 10;
	if ( res >= plusris )
	  mod -= 2;
	if ( sus <= plusris )
	  mod += 2;

	/* check if immune */
	if ( mod <= 0 )
	  dam = -1;
	if ( mod != 10 )
	  dam = (dam * mod) / 10;
    }
    
    /* race modifier */
    
/*    if ( victim->race == RACE_DUINUOGWUIN )
       dam /= 5;    */
    

	// Jedi have a difficult time wielding a dualsaber

	if(ch->force_type == FORCE_JEDI && wield && wield->value[3] == WEAPON_DUAL_LIGHTSABER)
	{
		if(number_range(0, 5) == 4)
		{
			send_to_char("No est�s entrenado en el uso de sables de luz de doble hoja, pierdes tu tiempo.\n\r", ch);
			act(AT_YELLOW, "$n valanc�a torpemente su sable de luz de doble hoja.", ch, NULL, NULL, TO_ROOM);
			return rNONE;
		}
	}

    /* 
      * check to see if weapon is charged 
      */
     
     if ( dt == (TYPE_HIT + WEAPON_BLASTER ) && wield && wield->item_type == ITEM_WEAPON )
     {
     	if ( wield->value[4] < 1  )
     	{ 
            act( AT_YELLOW, "$n te dispara con su arma l�ser pero no ocurre nada.",  ch, NULL, victim, TO_VICT    );
            act( AT_YELLOW, "*CLICK*... �Tu arma l�ser necesita munici�n!", ch, NULL, victim, TO_CHAR    );            
            if ( IS_NPC(ch) )
     	    {
     	        do_remove( ch, wield->name );
     	    }
            return rNONE;    	    
     	}
     	else if ( wield->blaster_setting == BLASTER_FULL && wield->value[4] >=5 )
        {
     	    dam *=  1.5;
     	    wield->value[4] -= 5;
     	}
     	else if ( wield->blaster_setting == BLASTER_HIGH && wield->value[4] >=4 )
        {
     	    dam *=  1.25;
     	    wield->value[4] -= 4;
     	}
     	else if ( wield->blaster_setting == BLASTER_NORMAL && wield->value[4] >=3 )
     	{
     	    wield->value[4] -= 3;
     	}
     	else if ( wield->blaster_setting == BLASTER_STUN && wield->value[4] >=5 )    
        {
     	    dam /= 10;
     	    wield->value[4] -= 3;
     	    fail = FALSE;
            chance = ris_save( victim, ch->skill_level[COMBAT_ABILITY], RIS_PARALYSIS );
            if ( chance == 1000 )
               fail = TRUE;
            else
               fail = saves_para_petri( chance, victim );
            if ( victim->was_stunned > 0 )
            {
               fail = TRUE;
               victim->was_stunned--;
            }
     	    chance = 100 - get_curr_con(victim) - victim->skill_level[COMBAT_ABILITY]/2;
    	/* harder for player to stun another player */
    	    if ( !IS_NPC(ch) && !IS_NPC(victim) )
              	chance -= sysdata.stun_plr_vs_plr;
    	    else
      		chance -= sysdata.stun_regular;
            chance = URANGE( 5, chance, 95 );
            if ( !fail && number_percent( ) < chance )
    	    {
		WAIT_STATE( victim, PULSE_VIOLENCE );
		act( AT_BLUE, "�Anillos azules de energ�a del arma l�ser de $N te golpean y aturden!", victim, NULL, ch, TO_CHAR );
		act( AT_BLUE, "�Anillos azules de energ�a  de tu arma l�ser golpean a $N, aturdi�ndole!", ch, NULL, victim, TO_CHAR );
		act( AT_BLUE, "�Anillos azules de energ�a del arma l�ser de $n golpean a $N, aturdi�ndole!", ch, NULL, victim, TO_NOTVICT );
		stop_fighting( victim, TRUE );
		if ( !IS_AFFECTED( victim, AFF_PARALYSIS ) )
		{
	  	   af.type      = gsn_stun;
	  	   af.location  = APPLY_AC;
	  	   af.modifier  = 20;
	  	   af.duration  = 7;
	  	   af.bitvector = AFF_PARALYSIS;
	  	   affect_to_char( victim, &af );
	  	   update_pos( victim );
	  	   if ( IS_NPC(victim) )
	  	   {
	  	       start_hating( victim, ch );
	  	       start_hunting( victim, ch );
	  	       victim->was_stunned = 10;
	  	   }
		}
    	    }
    	    else
            {
               act( AT_BLUE, "�Anillos azules de energ�a del arma l�ser de $N te golpean pero logras resistir!", victim, NULL, ch, TO_CHAR );
		act( AT_BLUE, "�Anillos azules de energ�a  de tu arma l�ser golpean a $N, pero logra resistir!", ch, NULL, victim, TO_CHAR );
		act( AT_BLUE, "�Anillos azules de energ�a del arma l�ser de $ngolpean a $N, pero logra resistir!", ch, NULL, victim, TO_NOTVICT );
		
            }
     	}
        else if ( wield->blaster_setting == BLASTER_HALF && wield->value[4] >=2 )    
        {
     	    dam *=  0.75;
     	    wield->value[4] -= 2;
     	}
        else    
        {
     	    dam *= 0.5;
     	    wield->value[4] -= 1;
     	}
     
     }
    else if ( 
              dt == (TYPE_HIT + WEAPON_VIBRO_BLADE )
              && wield && wield->item_type == ITEM_WEAPON 
            )
     {
     	if ( wield->value[4] < 1  )
     	{ 
            act( AT_YELLOW, "Tu vibro-hoja necesita recargarse...", ch, NULL, victim, TO_CHAR    );            
            dam /= 3;    	    
     	}
     }
     else if ( 
              dt == (TYPE_HIT + WEAPON_FORCE_PIKE )
              && wield && wield->item_type == ITEM_WEAPON 
            )
     {
     	if ( wield->value[4] < 1  )
     	{ 
            act( AT_YELLOW, "Tu pica de fuerza necesita recargarse...", ch, NULL, victim, TO_CHAR    );            
            dam /= 2;    	    
     	}
     	else 
     	    wield->value[4]--;
     }
    else if ( 
              dt == (TYPE_HIT + WEAPON_LIGHTSABER )
              && wield && wield->item_type == ITEM_WEAPON 
            )
     {
     	if ( wield->value[4] < 1  )
     	{ 
            act( AT_YELLOW, "$n intenta atacar con un sable de luz que se apaaga.",  ch, NULL, victim, TO_VICT    );
            act( AT_YELLOW, "Necesitas recargar tu sable de luz. Ha perdido su hoja.", ch, NULL, victim, TO_CHAR    );            
     	    if ( IS_NPC(ch) )
     	    {
     	        do_remove( ch, wield->name );
     	    }  
     	    return rNONE;
     	}
     }
     else if ( dt == (TYPE_HIT + WEAPON_BOWCASTER ) && wield && wield->item_type == ITEM_WEAPON )
     {
     	if ( wield->value[4] < 1  )
     	{ 
            act( AT_YELLOW, "$n te dispara con su arco de energ�a pero no pasa nada.",  ch, NULL, victim, TO_VICT    );
            act( AT_YELLOW, "�*CLICK*... tu arco de energ�a requiere un nuevo cartucho!", ch, NULL, victim, TO_CHAR    );            
            if ( IS_NPC(ch) )
     	    {
     	        do_remove( ch, wield->name );
     	    }
            return rNONE;    	    
     	}
     	else
          wield->value[4]--;
     }
     
    if ( dam <= 0 )
	dam = 1;

    if ( prof_gsn != -1 )
    {
      if ( dam > 0 )
        learn_from_success( ch, prof_gsn );
      else
        learn_from_failure( ch, prof_gsn );
    }

    /* immune to damage */
    if ( dam == -1 )
    {
	if ( dt >= 0 && dt < top_sn )
	{
	    SKILLTYPE *skill = skill_table[dt];
	    bool found = FALSE;

	    if ( skill->imm_char && skill->imm_char[0] != '\0' )
	    {
		act( AT_HIT, skill->imm_char, ch, NULL, victim, TO_CHAR );
		found = TRUE;
	    }
	    if ( skill->imm_vict && skill->imm_vict[0] != '\0' )
	    {
		act( AT_HITME, skill->imm_vict, ch, NULL, victim, TO_VICT );
		found = TRUE;
	    }
	    if ( skill->imm_room && skill->imm_room[0] != '\0' )
	    {
		act( AT_ACTION, skill->imm_room, ch, NULL, victim, TO_NOTVICT );
		found = TRUE;
	    }
	    if ( found )
	      return rNONE;
	}
	dam = 0;
    }
    
    if (dt == gsn_ambush)
      dam *= 2;
    
    if ( (retcode = damage( ch, victim, dam, dt )) != rNONE )
      return retcode;
    if ( char_died(ch) )
      return rCHAR_DIED;
    if ( char_died(victim) )
      return rVICT_DIED;

    retcode = rNONE;
    if ( dam == 0 )
      return retcode;

/* weapon spells	-Thoric */
    if ( wield
    &&  !IS_SET(victim->immune, RIS_MAGIC)
    &&  !IS_SET(victim->in_room->room_flags, ROOM_NO_MAGIC) )
    {
	AFFECT_DATA *aff;
	
	for ( aff = wield->pIndexData->first_affect; aff; aff = aff->next )
	   if ( aff->location == APPLY_WEAPONSPELL
	   &&   IS_VALID_SN(aff->modifier)
	   &&   skill_table[aff->modifier]->spell_fun )
		retcode = (*skill_table[aff->modifier]->spell_fun) ( aff->modifier, (wield->level+3)/3, ch, victim );
	if ( retcode != rNONE || char_died(ch) || char_died(victim) )
		return retcode;
	for ( aff = wield->first_affect; aff; aff = aff->next )
	   if ( aff->location == APPLY_WEAPONSPELL
	   &&   IS_VALID_SN(aff->modifier)
	   &&   skill_table[aff->modifier]->spell_fun )
		retcode = (*skill_table[aff->modifier]->spell_fun) ( aff->modifier, (wield->level+3)/3, ch, victim );
	if ( retcode != rNONE || char_died(ch) || char_died(victim) )
		return retcode;
    }

    /*
     * magic shields that retaliate				-Thoric
     */
    if ( IS_AFFECTED( victim, AFF_FIRESHIELD )
    &&  !IS_AFFECTED( ch, AFF_FIRESHIELD ) )
	retcode = spell_fireball( gsn_fireball, off_shld_lvl(victim, ch), victim, ch );
    if ( retcode != rNONE || char_died(ch) || char_died(victim) )
      return retcode;

    if ( retcode != rNONE || char_died(ch) || char_died(victim) )
      return retcode;

    if ( IS_AFFECTED( victim, AFF_SHOCKSHIELD )
    &&  !IS_AFFECTED( ch, AFF_SHOCKSHIELD ) )
	retcode = spell_lightning_bolt( gsn_lightning_bolt, off_shld_lvl(victim, ch), victim, ch );
    if ( retcode != rNONE || char_died(ch) || char_died(victim) )
      return retcode;

    /*
     *   folks with blasters move and snipe instead of getting neatin up in one spot.
     */
/*
     if ( IS_NPC(victim) )
     {
         OBJ_DATA *wield;
         
         wield = get_eq_char( victim, WEAR_WIELD );                 
         if ( wield != NULL && wield->value[3] == WEAPON_BLASTER && get_cover( victim ) == TRUE ) 
         {
               start_hating( victim, ch );
	       start_hunting( victim, ch );
         }
     }
  */   
    tail_chain( );
    return retcode;
}

/*
 * Calculate damage based on resistances, immunities and suceptibilities
 *					-Thoric
 */
sh_int ris_damage( CHAR_DATA *ch, sh_int dam, int ris )
{
   sh_int modifier;

   modifier = 10;
   if ( IS_SET(ch->immune, ris ) )
     modifier -= 10;
   if ( IS_SET(ch->resistant, ris ) )
     modifier -= 2;
   if ( IS_SET(ch->susceptible, ris ) )
     modifier += 2;
   if ( modifier <= 0 )
     return -1;
   if ( modifier == 10 )
     return dam;
   return (dam * modifier) / 10;
}


/*
 * Inflict damage from a hit.
 */
ch_ret damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt )
{
    CHAR_DATA *gch;
    sh_int dameq;
    int room;
    bool npcvict;
    bool loot;
    int  xp_gain;
    OBJ_DATA *damobj, *wield, *victwield;
    ch_ret retcode;
    sh_int dampmod;

    int nocorpse = 0;
    retcode = rNONE;

    if ( !ch )
    {
	bug( "Damage: null ch!", 0 );
	return rERROR;
    }
    if ( !victim )
    {
	bug( "Damage: null victim!", 0 );
	return rVICT_DIED;
    }

    if ( victim->position == POS_DEAD )
	return rVICT_DIED;

    npcvict = IS_NPC(victim);

    /*
     * Check damage types for RIS				-Thoric
     */
    if ( dam && dt != TYPE_UNDEFINED )
    {
	if ( IS_FIRE(dt) )
	  dam = ris_damage(victim, dam, RIS_FIRE);
	else
	if ( IS_COLD(dt) )
	  dam = ris_damage(victim, dam, RIS_COLD);
	else
	if ( IS_ACID(dt) )
	  dam = ris_damage(victim, dam, RIS_ACID);
	else
	if ( IS_ELECTRICITY(dt) )
	  dam = ris_damage(victim, dam, RIS_ELECTRICITY);
	else
	if ( IS_ENERGY(dt) )
	  dam = ris_damage(victim, dam, RIS_ENERGY);
	else
	if ( IS_DRAIN(dt) )
	  dam = ris_damage(victim, dam, RIS_DRAIN);
	else
	if ( dt == gsn_poison || IS_POISON(dt) )
	  dam = ris_damage(victim, dam, RIS_POISON);
	else
	if ( dt == (TYPE_HIT + 7) || dt == (TYPE_HIT + 8) )
	  dam = ris_damage(victim, dam, RIS_BLUNT);
	else
	if ( dt == (TYPE_HIT + 2) || dt == (TYPE_HIT + 11)
	||   dt == (TYPE_HIT + 10) )
	  dam = ris_damage(victim, dam, RIS_PIERCE);
	else
	if ( dt == (TYPE_HIT + 1) || dt == (TYPE_HIT + 3)
	||   dt == (TYPE_HIT + 4) || dt == (TYPE_HIT + 5) )
	  dam = ris_damage(victim, dam, RIS_SLASH);

	if ( dam == -1 )
	{
	    if ( dt >= 0 && dt < top_sn )
	    {
		bool found = FALSE;
		SKILLTYPE *skill = skill_table[dt];

		if ( skill->imm_char && skill->imm_char[0] != '\0' )
		{
		   act( AT_HIT, skill->imm_char, ch, NULL, victim, TO_CHAR );
		   found = TRUE;
		}
		if ( skill->imm_vict && skill->imm_vict[0] != '\0' )
		{
		   act( AT_HITME, skill->imm_vict, ch, NULL, victim, TO_VICT );
		   found = TRUE;
		}
		if ( skill->imm_room && skill->imm_room[0] != '\0' )
		{
		   act( AT_ACTION, skill->imm_room, ch, NULL, victim, TO_NOTVICT );
		   found = TRUE;
		}
		if ( found )
		   return rNONE;
	    }
	    dam = 0;
	}
    }

    if ( dam && npcvict && ch != victim )
    {
	if ( !IS_SET( victim->act, ACT_SENTINEL ) )
 	{
	   if ( victim->hunting )
	   {
	     if ( victim->hunting->who != ch )
	     {
		STRFREE( victim->hunting->name );
		victim->hunting->name = QUICKLINK( ch->name );
		victim->hunting->who  = ch;
	     }
           }
	   else
	     start_hunting( victim, ch );
	}

      if ( victim->hating )
      {
	if ( victim->hating->who != ch )
	{
	   STRFREE( victim->hating->name );
	   victim->hating->name = QUICKLINK( ch->name );
	   victim->hating->who  = ch;
	}
      }
      else
	start_hating( victim, ch );
    }

    if ( victim != ch )
    {
	/*
	 * Certain attacks are forbidden.
	 * Most other attacks are returned.
	 */
	if ( is_safe( ch, victim ) )
	    return rNONE;


	if ( victim->position > POS_STUNNED && dt != TYPE_MISSILE)
	{
	    if ( !victim->fighting )
		set_fighting( victim, ch );
	    if ( victim->fighting )
		victim->position = POS_FIGHTING;
	}

	if ( victim->position > POS_STUNNED && dt != TYPE_MISSILE)
	{
	    if ( !ch->fighting )
		set_fighting( ch, victim );

	    /*
	     * If victim is charmed, ch might attack victim's master.
	     */
	    if ( IS_NPC(ch)
	    &&   npcvict
	    &&   IS_AFFECTED(victim, AFF_CHARM)
	    &&   victim->master
	    &&   victim->master->in_room == ch->in_room
	    &&   number_bits( 3 ) == 0 )
	    {
		stop_fighting( ch, FALSE );
		retcode = multi_hit( ch, victim->master, TYPE_UNDEFINED );
		return retcode;
	    }
	}


	/*
	 * More charm stuff.
	 */
	if ( victim->master == ch )
	    stop_follower( victim );

       
	/*
	 * Inviso attacks ... not.
	 */
	if ( IS_AFFECTED(ch, AFF_INVISIBLE) && ch->race != RACE_DEFEL )
	{
	    affect_strip( ch, gsn_invis );
	    affect_strip( ch, gsn_mass_invis );
	    REMOVE_BIT( ch->affected_by, AFF_INVISIBLE );
	act( AT_MAGIC, "$n aparece lentamente.", ch, NULL, NULL, TO_ROOM );
	}

	/* Take away Hide */
	if ( IS_AFFECTED(ch, AFF_HIDE) )
	     REMOVE_BIT(ch->affected_by, AFF_HIDE);
	/*
	 * Damage modifiers.
	 */
	if ( IS_AFFECTED(victim, AFF_SANCTUARY) )
	    dam /= 2;

	if ( IS_AFFECTED(victim, AFF_PROTECT) && IS_EVIL(ch) )
	    dam -= (int) (dam / 4);

	if ( dam < 0 )
	    dam = 0;

	/*
	 * Check for disarm, trip, parry, and dodge.
	 */
	if ( dt >= TYPE_HIT )
	{
	    if ( IS_NPC(ch)
	    &&   IS_SET( ch->attacks, DFND_DISARM )
	    &&   number_percent( ) < ch->skill_level[COMBAT_ABILITY] / 2 )
		disarm( ch, victim );

	    if ( IS_NPC(ch)
	    &&   IS_SET( ch->attacks, ATCK_TRIP )
	    &&   number_percent( ) < ch->skill_level[COMBAT_ABILITY] )
		trip( ch, victim );

	    if ( check_parry( ch, victim ) )
	     {
	      if(!IS_NPC(ch) && ch->pcdata->learned[gsn_reflect] && (ch->pcdata->learned[gsn_reflect]/2) > number_percent())
	       {
               if((wield = get_eq_char( ch, WEAR_WIELD )) != NULL)
	        {
	        if((victwield = get_eq_char( victim, WEAR_WIELD)) != NULL)
		 {
	         if((victwield->value[3] == WEAPON_LIGHTSABER || victwield->value[3] == WEAPON_DUAL_LIGHTSABER) && wield->value[3] == WEAPON_BLASTER)
                  {
                   act( AT_WHITE, "�Predices mediante la fuerza el disparo de arma l�ser de $n y colocas la hoja de tu sable de luz en la posici�n adecuada, interceptando y reflejando su rayo!", ch, NULL, victim, TO_VICT );
                   act( AT_WHITE, "�$N intercepta el disparo de tu arma l�ser con la hoja de su sable de luz y te lo refleja!", ch, NULL, victim, TO_CHAR );
		   act( AT_WHITE, "�$N intercepta con la hoja de su sable de luz el disparo de arma l�ser de $N y se lo refleja!", ch, NULL, victim, TO_NOTVICT);
		   ch->hit -= wield->value[1];
		   dam_message(victim, ch, wield->value[1], (TYPE_HIT + WEAPON_BLASTER ));
		   learn_from_success(victim, gsn_reflect);
		  }
		 }
		}
               }
		else
		return rNONE;
	     }
	    if ( check_dodge( ch, victim ) )
		return rNONE;
	    if ( check_reflect(ch, victim, dam) )
		return rNONE;
	}
     
    if ( dam > 0 && dt > TYPE_HIT
    && !IS_AFFECTED( victim, AFF_POISON )
    &&  is_wielding_poisoned( ch )
    && !IS_SET( victim->immune, RIS_POISON )
    && !saves_poison_death( ch->skill_level[COMBAT_ABILITY], victim ) )
    {
	AFFECT_DATA af;

	af.type      = gsn_poison;
	af.duration  = 20;
	af.location  = APPLY_STR;
	af.modifier  = -2;
	af.bitvector = AFF_POISON;
	affect_join( victim, &af );
	victim->mental_state = URANGE( 20, victim->mental_state + 2, 100 );
	dam = dam + (dam/2);
    }

     
     
    /*
     * Check control panel settings and modify damage
     */
    if ( IS_NPC(ch) )
    {
	if ( npcvict )
	  dampmod = sysdata.dam_mob_vs_mob;
	else
	  dampmod = sysdata.dam_mob_vs_plr;
    }
    else
    {
	if ( npcvict )
	  dampmod = sysdata.dam_plr_vs_mob;
	else
	  dampmod = sysdata.dam_plr_vs_plr;
    }
    if ( dampmod > 0 )
      dam = ( dam * dampmod ) / 100;
    }

    dam_message( ch, victim, dam, dt );


    /*
     * Code to handle equipment getting damaged, and also support  -Thoric
     * bonuses/penalties for having or not having equipment where hit
     */
    if (dam > 10 && dt != TYPE_UNDEFINED && !IS_SET( ch->in_room->room_flags2, ROOM_ARENA))
    {
	/* get a random body eq part */
	dameq  = number_range(WEAR_LIGHT, WEAR_EYES);
	damobj = get_eq_char(victim, dameq);
	if ( damobj )
	{
	  if ( dam > get_obj_resistance(damobj) )
	  {
	     set_cur_obj(damobj);
	     damage_obj(damobj);
	  }
	  dam -= 5;  /* add a bonus for having something to block the blow */
	}
	else
	  dam += 5;  /* add penalty for bare skin! */
    }

    if( ch != victim )
       dam_message( ch, victim, dam, dt );

   /*
    * Hurt the victim.
    * Inform the victim of his new state.
    */
    victim->hit -= dam;

    /*
     * Get experience based on % of damage done			-Thoric
     */
    if ( dam && ch != victim
    &&  !IS_NPC(ch) && ch->fighting && ch->fighting->xp )
    {
	xp_gain = (int) (xp_compute( ch, victim ) * 0.1 * dam) / victim->max_hit;
	gain_exp( ch, xp_gain, COMBAT_ABILITY );
    }

    if ( !IS_NPC(victim)
    &&   victim->top_level >= LEVEL_IMMORTAL
    &&   victim->hit < 1 )
       victim->hit = 1;

    /* Make sure newbies dont die */

    if (!IS_NPC(victim) && NOT_AUTHED(victim) && victim->hit < 1)
	victim->hit = 1;

    if ( victim->hit < 1 && !IS_NPC(victim) && IS_SET(victim->in_room->room_flags2, ROOM_ARENA))
     {
        char_from_room(victim);
        char_to_room(victim,victim->pcdata->roomarena);
        victim->hit = victim->max_hit;
        victim->mana = victim->max_mana;
        if( num_in_arena() == 1)
          find_game_winner();
        do_look(victim, "auto");
        stop_fighting( victim, TRUE );
        lost_arena(victim);
        return rNONE;
     }

    if ( !npcvict
    &&   get_trust(victim) >= LEVEL_IMMORTAL
    &&	 get_trust(ch)	   >= LEVEL_IMMORTAL
    &&   victim->hit < 1 )
	victim->hit = 1;
    update_pos( victim );

    switch( victim->position )
    {
    case POS_MORTAL:
	act( AT_DYING, "$n tiene heridas de muerte y morir� lentamente si nadie le ayuda.",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char( "&RTienes heridas mortales y morir�s lentamente si nadie te ayuda.",victim);
	break;

    case POS_INCAP:
	act( AT_DYING, "$n es incapaz de reaccionar y morir� lentamente si nadie le ayuda.",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char( "&RNo eres capaz de reaccionar y morir�s lentamente si nadie te ayuda.",victim);
	break;

    case POS_STUNNED:
        if ( !IS_AFFECTED( victim, AFF_PARALYSIS ) )
        {
	  act( AT_ACTION, "A $n le han aturdido, pero se recuperar�.",
	    victim, NULL, NULL, TO_ROOM );
	  send_to_char( "&RTe han aturdido, pero te recuperar�s.",victim);
	}
	break;

    case POS_DEAD:
	if ( dt >= 0 && dt < top_sn )
	{
	    SKILLTYPE *skill = skill_table[dt];

	    if ( skill->die_char && skill->die_char[0] != '\0' )
	      act( AT_DEAD, skill->die_char, ch, NULL, victim, TO_CHAR );
	    if ( skill->die_vict && skill->die_vict[0] != '\0' )
	      act( AT_DEAD, skill->die_vict, ch, NULL, victim, TO_VICT );
	    if ( skill->die_room && skill->die_room[0] != '\0' )
	      act( AT_DEAD, skill->die_room, ch, NULL, victim, TO_NOTVICT );
	}
	if ( IS_NPC(victim) && IS_SET( victim->act, ACT_NOKILL )  )
	   act( AT_YELLOW, "$n corre por su vida... �Escapando de una muerte segura!", victim, 0, 0, TO_ROOM );
         else
            act( AT_DEAD, rand_death( victim ), victim, 0, 0, TO_ROOM );
	break;

    default:
	if ( dam > victim->max_hit / 4 )
	{
	   act( AT_HURT, "�Eso ha dolido!", victim, 0, 0, TO_CHAR );
	   if ( number_bits(3) == 0 )
		worsen_mental_state( ch, 1 );
	}
	if ( victim->hit < victim->max_hit / 4 )

	{
	   act( AT_DANGER, "�Desear�as que tus heridas no sangraran tanto!",
		victim, 0, 0, TO_CHAR );
	   if ( number_bits(2) == 0 )
		worsen_mental_state( ch, 1 );
	}
	break;
    }

    /*
     * Sleep spells and extremely wounded folks.
     */
    if ( !IS_AWAKE(victim)		/* lets make NPC's not slaughter PC's */
    &&   !IS_AFFECTED( victim, AFF_PARALYSIS ) )
    {
	if ( victim->fighting
	&&   victim->fighting->who->hunting
	&&   victim->fighting->who->hunting->who == victim )
	   stop_hunting( victim->fighting->who );

	if ( victim->fighting
	&&   victim->fighting->who->hating
	&&   victim->fighting->who->hating->who == victim )
	   stop_hating( victim->fighting->who );

	stop_fighting( victim, TRUE );
    }
    
    if ( victim->hit <=0 && !IS_NPC(victim))
    {
       OBJ_DATA *obj;
       OBJ_DATA *obj_next;
       int cnt=0;
    
       REMOVE_BIT( victim->act, PLR_ATTACKER );
       
        stop_fighting( victim, TRUE );
       
       if ( ( obj = get_eq_char( victim, WEAR_DUAL_WIELD ) ) != NULL )
          unequip_char( victim, obj );
       if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) != NULL )
          unequip_char( victim, obj );
       if ( ( obj = get_eq_char( victim, WEAR_HOLD ) ) != NULL )
          unequip_char( victim, obj );
       if ( ( obj = get_eq_char( victim, WEAR_MISSILE_WIELD ) ) != NULL )
          unequip_char( victim, obj );
       if ( ( obj = get_eq_char( victim, WEAR_LIGHT ) ) != NULL )
          unequip_char( victim, obj );
       
        for ( obj = victim->first_carrying; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;

	    if ( obj->wear_loc == WEAR_NONE )
	    {
		if ( obj->pIndexData->progtypes & DROP_PROG && obj->count > 1 ) 
		{
		   ++cnt;
		   separate_obj( obj );
		   obj_from_char( obj );
		   if ( !obj_next )
		     obj_next = victim->first_carrying;
		}
		else
		{
		   cnt += obj->count;
		   obj_from_char( obj );
		}
		act( AT_ACTION, "$n deja caer $p.", victim, obj, NULL, TO_ROOM );
		act( AT_ACTION, "Dejas caer $p.", victim, obj, NULL, TO_CHAR );
		obj = obj_to_room( obj, victim->in_room );
	    }
	}
         
       if ( IS_NPC( ch ) && !IS_NPC( victim ) )
       {   
           long lose_exp;    
           lose_exp = UMAX( ( victim->experience[COMBAT_ABILITY] - exp_level( victim->skill_level[COMBAT_ABILITY] ) ) , 0 );
           ch_printf( victim, "Pierdes %ld experiencia.\n\r", lose_exp ); 
           victim->experience[COMBAT_ABILITY] -= lose_exp; 
       }

      add_timer( victim, TIMER_RECENTFIGHT, 100, NULL, 0 );

    }
       
    /*
     * Payoff for killing things.
     */
    if ( victim->position == POS_DEAD )
    {
        OBJ_DATA *new_corpse;
  	group_gain( ch, victim );
        
	if ( !npcvict )
	{
	    sprintf( log_buf, "%s killed by %s at %d",
		victim->name,
		(IS_NPC(ch) ? ch->short_descr : ch->name),
		victim->in_room->vnum );
	    log_string( log_buf );
	    to_channel( log_buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL );

	}
	else if ( !IS_NPC(ch) && IS_NPC(victim) )		/* keep track of mob vnum killed */
        {
	    add_kill( ch, victim );

            /*
             * Add to kill tracker for grouped chars, as well. -Halcyon
             */
             for( gch = ch->in_room->first_person; gch; gch = gch->next_in_room )
                 if( is_same_group( gch, ch ) && !IS_NPC(gch) && gch != ch )
                    add_kill( gch, victim );
        }

	check_killer( ch, victim );

	if ( !IS_NPC(victim) || !IS_SET( victim->act, ACT_NOKILL )  )
	   loot = legal_loot( ch, victim );
        else
           loot = FALSE;
	if (IS_SET(victim->act, ACT_NOCORPSE))
	  nocorpse=1;        
	//Make sure snipe doesnt loot.
	room = victim->in_room->vnum;
	set_cur_char(victim);
	new_corpse = raw_kill( ch, victim );
	victim = NULL;

      if( !IS_NPC( ch ) && loot && new_corpse && new_corpse->item_type == ITEM_CORPSE_NPC
       && new_corpse->in_room == ch->in_room && can_see_obj( ch, new_corpse ) && ch->position > POS_SLEEPING )
      {
         /*
          * Autogold by Scryn 8/12 
          */
         if( IS_SET( ch->act, PLR_AUTOGOLD ) && !loot_coins_from_corpse( ch, new_corpse ) )
            return rBOTH_DIED;

         if( new_corpse && !obj_extracted(new_corpse) && new_corpse->in_room == ch->in_room
          && ch->position > POS_SLEEPING && can_see_obj( ch, new_corpse ) )
         {
            if( IS_SET( ch->act, PLR_AUTOLOOT ) )
               do_get( ch, "todo cuerpo" );
            else
               do_look( ch, "en cuerpo" );
            if( !char_died(ch) && IS_SET( ch->act, PLR_AUTOSAC ) && !obj_extracted(new_corpse)
             && new_corpse->in_room == ch->in_room && ch->position > POS_SLEEPING
             && can_see_obj( ch, new_corpse ) )
               do_sacrifice( ch, "corpse" );
         }
      }

	if ( IS_SET( sysdata.save_flags, SV_KILL ) )
	   save_char_obj( ch );
	return rVICT_DIED;
    }

    if ( victim == ch )
	return rNONE;

    /*
     * Take care of link dead people.
     */
    if ( !npcvict && !victim->desc && !victim->switched )
    {
	if ( number_range( 0, victim->wait ) == 0)
	{
	    do_flee( victim, "" );
	    do_flee( victim, "" );
	    do_flee( victim, "" );
	    do_flee( victim, "" );
	    do_flee( victim, "" );
	    do_hail( victim, "" );
	    do_quit( victim, "" );
	    return rNONE;
	}
    }

    /*
     * Wimp out?
     */
    if ( npcvict && dam > 0 )
    {
	if ( ( IS_SET(victim->act, ACT_WIMPY) && number_bits( 1 ) == 0
	&&   victim->hit < victim->max_hit / 2 )
	||   ( IS_AFFECTED(victim, AFF_CHARM) && victim->master
	&&     victim->master->in_room != victim->in_room ) )
	{
	    start_fearing( victim, ch );
	    stop_hunting( victim );
	    //do_flee( victim, "" );
	}
    }

/* player wimpy deprecated in FotE.

    if ( !npcvict
    &&   victim->hit > 0
    &&   victim->hit <= victim->wimpy
    &&   victim->wait == 0 )
	do_flee( victim, "" );
*/

    tail_chain( );
    return rNONE;
}

bool is_safe( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( !victim )
        return FALSE;

    /* Thx Josh! */
    if ( who_fighting( ch ) == ch )
	return FALSE;
                 
    if ( IS_SET( victim->in_room->room_flags, ROOM_SAFE ) )
    {
	set_char_color( AT_MAGIC, ch );
	send_to_char( "Aqu� no.\n\r", ch );
	return TRUE;
    }

    if ( get_trust(ch) > LEVEL_HERO )
            return FALSE;
 
    if ( IS_NPC(ch) || IS_NPC(victim) )
	return FALSE;


return FALSE;

}

/* checks is_safe but without the output 
   cuts out imms and safe rooms as well 
   for info only */

bool is_safe_nm( CHAR_DATA *ch, CHAR_DATA *victim )
{
    return FALSE;
}


/*
 * just verify that a corpse looting is legal
 */
bool legal_loot( CHAR_DATA *ch, CHAR_DATA *victim )
{
  /* pc's can now loot .. why not .. death is pretty final */
  if ( !IS_NPC(ch) )
     return TRUE;
  /* non-charmed mobs can loot anything */
  if ( IS_NPC(ch) && !ch->master )
    return TRUE;
 
  return FALSE;
 }

/*
see if an attack justifies a KILLER flag --- edited so that none do but can't 
murder a no pk person. --- edited again for planetary wanted flags -- well will be soon :p
 */
 
void check_killer( CHAR_DATA *ch, CHAR_DATA *victim )
{
    
//    int x;
    
    /*
    * Charm-o-rama.
    */
    if ( IS_SET(ch->affected_by, AFF_CHARM) )
    {
	if ( !ch->master )
	{
	    char buf[MAX_STRING_LENGTH];

	    sprintf( buf, "Check_killer: %s bad AFF_CHARM",
		IS_NPC(ch) ? ch->short_descr : ch->name );
	    bug( buf, 0 );
	    affect_strip( ch, gsn_charm_person );
	    REMOVE_BIT( ch->affected_by, AFF_CHARM );
	    return;
	}

	/* stop_follower( ch ); */
	if ( ch->master )
	  check_killer( ch->master, victim );
    }
    
    if ( IS_NPC(victim) )
    {
	if ( !IS_NPC( ch ) )
	{

/* Commented out for the outlaw command.

	  for ( x = 0; x < 32; x++ )
	  {
	      if ( IS_SET(victim->vip_flags , 1 << x ) )
	      {
	         SET_BIT(ch->pcdata->wanted_flags, 1 << x );
	         ch_printf( ch, "&YAhora te buscan en %s.&w\n\r", planet_flags[x] , victim->short_descr );
	      }
	  }      

*/
	  if ( ch->pcdata->clan )
	    ch->pcdata->clan->mkills++;
	  ch->pcdata->mkills++;
	  if (ch->in_room->area)
	    ch->in_room->area->mkills++;
	}
	return;
    }
   
    if ( !IS_NPC(ch) && !IS_NPC(victim) )
    {
	if ( ch->pcdata->clan ) ch->pcdata->clan->pkills++;
	  ch->pcdata->pkills++;
	update_pos(victim);
	if ( victim->pcdata->clan )
	  victim->pcdata->clan->pdeaths++;	   
    }


    if ( IS_NPC(ch) )
      if ( !IS_NPC(victim) && victim->in_room && victim->in_room->area )
	victim->in_room->area->mdeaths++;

    return;
}



/*
 * Set position of a victim.
 */
void update_pos( CHAR_DATA *victim )
{
    if ( !victim )
    {
      bug( "update_pos: null victim", 0 );
      return;
    }

    if ( victim->hit > 0 )
    {
	if ( victim->position <= POS_STUNNED )
	  victim->position = POS_STANDING;
	if ( IS_AFFECTED( victim, AFF_PARALYSIS ) )
	  victim->position = POS_STUNNED;
	return;
    }

    if ( IS_NPC(victim) || victim->hit <= -500 )
    {
	if ( victim->mount )
	{
	  act( AT_ACTION, "$n cae de $N.",
		victim, NULL, victim->mount, TO_ROOM );
	  REMOVE_BIT( victim->mount->act, ACT_MOUNTED );
	  victim->mount = NULL;
	}
	victim->position = POS_DEAD;
	return;
    }

	 if ( victim->hit <= -400 ) victim->position = POS_MORTAL;
    else if ( victim->hit <= -200 ) victim->position = POS_INCAP;
    else          victim->position = POS_STUNNED;

    if ( victim->position > POS_STUNNED
    &&   IS_AFFECTED( victim, AFF_PARALYSIS ) )
      victim->position = POS_STUNNED;

    if ( victim->mount )
    {
	act( AT_ACTION, "$n cae inconsciente de $N.",
		victim, NULL, victim->mount, TO_ROOM );
	REMOVE_BIT( victim->mount->act, ACT_MOUNTED );
	victim->mount = NULL;
    }
    return;
}


/*
 * Start fights.
 */
void set_fighting( CHAR_DATA *ch, CHAR_DATA *victim )
{
    FIGHT_DATA *fight;

    if ( ch->fighting )
    {
	char buf[MAX_STRING_LENGTH];

	sprintf( buf, "Set_fighting: %s -> %s (already fighting %s)",
		ch->name, victim->name, ch->fighting->who->name );
	bug( buf, 0 );
	return;
    }

    if ( IS_AFFECTED(ch, AFF_SLEEP) )
      affect_strip( ch, gsn_sleep );

    /* Limit attackers -Thoric */
    if ( victim->num_fighting > max_fight(victim) )
    {
	send_to_char( "Hay mucha gente luchando, no puedes entrar al combate.\n\r", ch );
	return;
    }

    CREATE( fight, FIGHT_DATA, 1 );
    fight->who	 = victim;
    fight->xp	 = (int) xp_compute( ch, victim );
    fight->align = align_compute( ch, victim );
    if ( !IS_NPC(ch) && IS_NPC(victim) )
      fight->timeskilled = times_killed(ch, victim);
    ch->num_fighting = 1;
    ch->fighting = fight;
    ch->position = POS_FIGHTING;
    victim->num_fighting++;
    if ( victim->switched && IS_AFFECTED(victim->switched, AFF_POSSESS) )
    {
	send_to_char( "�Te han perturvado!\n\r", victim->switched );
	do_return( victim->switched, "" );
    }
    return;
}


CHAR_DATA *who_fighting( CHAR_DATA *ch )
{
    if ( !ch )
    {
	bug( "who_fighting: null ch", 0 );
	return NULL;
    }
    if ( !ch->fighting )
      return NULL;
    return ch->fighting->who;
}

void free_fight( CHAR_DATA *ch )
{
   if ( !ch )
   {
	bug( "Free_fight: null ch!", 0 );
	return;
   }
   if ( ch->fighting )
   {
     if ( !char_died(ch->fighting->who) )
       --ch->fighting->who->num_fighting;
     DISPOSE( ch->fighting );
   }
   ch->fighting = NULL;
   if ( ch->mount )
     ch->position = POS_MOUNTED;
   else
     ch->position = POS_STANDING;
   /* Berserk wears off after combat. -- Altrag */
   if ( IS_AFFECTED(ch, AFF_BERSERK) )
   {
     affect_strip(ch, gsn_berserk);
     set_char_color(AT_WEAROFF, ch);
     send_to_char(skill_table[gsn_berserk]->msg_off, ch);
     send_to_char("\n\r", ch);
   }
   return;
}


/*
 * Stop fights.
 */
void stop_fighting( CHAR_DATA *ch, bool fBoth )
{
    CHAR_DATA *fch;

    free_fight( ch );
    update_pos( ch );

    if ( !fBoth )   /* major short cut here by Thoric */
      return;

    for ( fch = first_char; fch; fch = fch->next )
    {
	if ( who_fighting( fch ) == ch )
	{
	    free_fight( fch );
	    update_pos( fch );
	}
    }
    return;
}



void death_cry( CHAR_DATA *ch )
{

    return;
}



OBJ_DATA *raw_kill( CHAR_DATA * ch, CHAR_DATA * victim )
{
   CHAR_DATA *victmp;
   OBJ_DATA *corpse_to_return = NULL;
   OBJ_DATA *obj, *obj_next;
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   char arg[MAX_STRING_LENGTH];
   long kexp;

   if( !victim )
   {
      bug( "%s: null victim!", __FUNCTION__ );
      return NULL;
   }

   strcpy( arg, victim->name );

   stop_fighting( victim, TRUE );

   if( ch && !IS_NPC( ch ) && !IS_NPC( victim ) )
   {
      CONTRACT_DATA *contract;
      CONTRACT_DATA *scontract = NULL;
      claim_disintegration( ch, victim );

      for( contract = ch->first_contract; contract; contract = contract->next_in_contract )
      {
         if( !str_cmp( contract->target, victim->name ) )
         {
            scontract = contract;
            break;
         }
      }

      if( scontract != NULL )
      {
         ch_printf( ch, "&w&RYou have claimed your contract on %s, and collect your reward of %d credits.\r\n",
                    scontract->target, scontract->amount );
         ch->gold += scontract->amount;
         kexp = ( exp_level( ch->skill_level[ASSASSIN_ABILITY] + 1 ) - exp_level( ch->skill_level[ASSASSIN_ABILITY] ) );
         gain_exp( ch, kexp, ASSASSIN_ABILITY );

         STRFREE( scontract->target );
         UNLINK( scontract, ch->first_contract, ch->last_contract, next_in_contract, prev_in_contract );
         DISPOSE( scontract );
      }
   }

   /* Take care of polymorphed chars */
   if( IS_NPC( victim ) && IS_SET( victim->act, ACT_POLYMORPHED ) )
   {
      char_from_room( victim->desc->original );
      char_to_room( victim->desc->original, victim->in_room );
      victmp = victim->desc->original;
      do_revert( victim, "" );
      return raw_kill( ch, victmp );
   }

   if( victim->in_room && IS_NPC( victim ) && victim->vip_flags != 0 && victim->in_room->area
       && victim->in_room->area->planet )
   {
      victim->in_room->area->planet->population--;
      victim->in_room->area->planet->population = UMAX( victim->in_room->area->planet->population, 0 );
      victim->in_room->area->planet->pop_support -= ( float )( 1 + 1 / ( victim->in_room->area->planet->population + 1 ) );
      if( victim->in_room->area->planet->pop_support < -100 )
         victim->in_room->area->planet->pop_support = -100;
   }

   if( !IS_NPC( victim ) || !IS_SET( victim->act, ACT_NOKILL ) )
      mprog_death_trigger( ch, victim );
   if( char_died( victim ) )
      return NULL;

   if( !IS_NPC( victim ) || !IS_SET( victim->act, ACT_NOKILL ) )
      rprog_death_trigger( ch, victim );
   if( char_died( victim ) )
      return NULL;

   if( !IS_NPC( victim ) || ( !IS_SET( victim->act, ACT_NOKILL ) && !IS_SET( victim->act, ACT_NOCORPSE ) ) )
      corpse_to_return = make_corpse( victim, IS_NPC( ch ) ? ch->short_descr : ch->name );
   else
   {
      for( obj = victim->last_carrying; obj; obj = obj_next )
      {
         obj_next = obj->prev_content;
         obj_from_char( obj );
         extract_obj( obj );
      }
   }

   if( IS_NPC( victim ) )
   {
      victim->pIndexData->killed++;
      extract_char( victim, TRUE );
      victim = NULL;
      return corpse_to_return;
   }

   set_char_color( AT_DIEMSG, victim );
   do_help( victim, "_DIEMSG_" );

/* swreality chnages begin here */

   if( victim->plr_home )
   {
      ROOM_INDEX_DATA *room = victim->plr_home;

      STRFREE( room->name );
      room->name = STRALLOC( "An Empty Apartment" );

      REMOVE_BIT( room->room_flags, ROOM_PLR_HOME );
      SET_BIT( room->room_flags, ROOM_EMPTY_HOME );

      fold_area( room->area, room->area->filename, FALSE );
   }

   if( victim->pcdata && victim->pcdata->clan )
   {
      if( victim->pcdata->clan->shortname && victim->pcdata->clan->shortname[0] != '\0' )
         remove_member( victim->name, victim->pcdata->clan->shortname );

      if( !str_cmp( victim->name, victim->pcdata->clan->leader ) )
      {
         STRFREE( victim->pcdata->clan->leader );
         if( victim->pcdata->clan->number1 )
         {
            victim->pcdata->clan->leader = STRALLOC( victim->pcdata->clan->number1 );
            STRFREE( victim->pcdata->clan->number1 );
            victim->pcdata->clan->number1 = STRALLOC( "" );
         }
         else if( victim->pcdata->clan->number2 )
         {
            victim->pcdata->clan->leader = STRALLOC( victim->pcdata->clan->number2 );
            STRFREE( victim->pcdata->clan->number2 );
            victim->pcdata->clan->number2 = STRALLOC( "" );
         }
         else
            victim->pcdata->clan->leader = STRALLOC( "" );
      }

      if( !str_cmp( victim->name, victim->pcdata->clan->number1 ) )
      {
         STRFREE( victim->pcdata->clan->number1 );
         if( victim->pcdata->clan->number2 )
         {
            victim->pcdata->clan->number1 = STRALLOC( victim->pcdata->clan->number2 );
            STRFREE( victim->pcdata->clan->number2 );
            victim->pcdata->clan->number2 = STRALLOC( "" );
         }
         else
            victim->pcdata->clan->number1 = STRALLOC( "" );
      }

      if( !str_cmp( victim->name, victim->pcdata->clan->number2 ) )
      {
         STRFREE( victim->pcdata->clan->number2 );
         victim->pcdata->clan->number1 = STRALLOC( "" );
      }

      victim->pcdata->clan->members--;
   }

   if( !victim )
   {
      DESCRIPTOR_DATA *d;

      /*
       * Make sure they aren't halfway logged in. 
       */
      for( d = first_descriptor; d; d = d->next )
         if( ( victim = d->character ) && !IS_NPC( victim ) )
            break;
      if( d )
         close_socket( d, TRUE );
   }
   else
   {
      int x, y;

      quitting_char = victim;
      save_char_obj( victim );
      saving_char = NULL;
      extract_char( victim, TRUE );
      for( x = 0; x < MAX_WEAR; x++ )
         for( y = 0; y < MAX_LAYERS; y++ )
            save_equipment[x][y] = NULL;
   }

   sprintf( buf, "%s%c/%s", PLAYER_DIR, tolower( arg[0] ), capitalize( arg ) );
   sprintf( buf2, "%s%c/%s", BACKUP_DIR, tolower( arg[0] ), capitalize( arg ) );

   rename( buf, buf2 );

   sprintf( buf, "%s%c/%s.clone", PLAYER_DIR, tolower( arg[0] ), capitalize( arg ) );
   sprintf( buf2, "%s%c/%s", PLAYER_DIR, tolower( arg[0] ), capitalize( arg ) );

   rename( buf, buf2 );

   /* Profile Deletion. */
   sprintf( buf, "%s%s.htm", PROFILE_DIR, capitalize( arg ) );
   remove( buf );

   return corpse_to_return;
}


void group_gain( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *gch, *gch_next;
    CHAR_DATA *lch;
    int xp;
    int members;

    /*
     * Monsters don't get kill xp's or alignment changes.
     * Dying of mortal wounds or poison doesn't give xp to anyone!
     */
    if ( IS_NPC(ch) || victim == ch )
	return;

    members = 0;
    
    for ( gch = ch->in_room->first_person; gch; gch = gch->next_in_room )
    {
	if ( is_same_group( gch, ch ) )
	    members++;
    }

    if ( members == 0 )
    {
	bug( "Group_gain: members.", members );
	members = 1;
    }

    lch = ch->leader ? ch->leader : ch;

    for ( gch = ch->in_room->first_person; gch; gch = gch_next )
    {
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;

        gch_next = gch->next_in_room;

	if ( !is_same_group( gch, ch ) )
	    continue;

	xp = (int) (xp_compute( gch, victim ) / members);
	 
	gch->alignment = align_compute( gch, victim );
	
	if ( !IS_NPC(gch) && IS_NPC(victim) && gch->pcdata && gch->pcdata->clan
	&& !str_cmp ( gch->pcdata->clan->name , victim->mob_clan ) )
	{
	     xp = 0;
	     sprintf( buf, "No recibes experiencia por matar tropas de tu organizaci�n.\n\r");
	     send_to_char( buf, gch );
	}
	else
	{
	   sprintf( buf, "Recibes %d experiencia en combate.\n\r", xp );
	   send_to_char( buf, gch );
	}
        
        gain_exp( gch, xp , COMBAT_ABILITY );
        
        if ( lch == gch && members > 1 )
        {
           xp = URANGE( members, xp*members, (exp_level( gch->skill_level[POLITICIAN_ABILITY]+1) - exp_level(gch->skill_level[POLITICIAN_ABILITY] )/10) );  
           sprintf( buf, "Consigues %d experiencia en diplomacia por llevar a tu grupo a la victoria.\n\r", xp ); 
	   send_to_char( buf, gch );
           gain_exp( gch, xp , POLITICIAN_ABILITY );
        }   
        
        
	for ( obj = gch->first_carrying; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( obj->wear_loc == WEAR_NONE )
		continue;

	    if ( ( IS_OBJ_STAT(obj, ITEM_ANTI_EVIL)    && IS_EVIL(gch)    )
	    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_GOOD)    && IS_GOOD(gch)    )
	    ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL(gch) ) )
	    {
		act( AT_MAGIC, "Eres agitado por $p.", gch, obj, NULL, TO_CHAR );
		act( AT_MAGIC, "$n es agitado por $p.",   gch, obj, NULL, TO_ROOM );
	        
		obj_from_char( obj );
		obj = obj_to_room( obj, gch->in_room );
//		oprog_zap_trigger(ch, obj);  /* mudprogs */
		if ( char_died(gch) )
		  return;
	    }
	}
    }

    return;
}


int align_compute( CHAR_DATA *gch, CHAR_DATA *victim )
{

/* never cared much for this system

    int align, newalign;

    align = gch->alignment - victim->alignment;

    if ( align >  500 )
	newalign  = UMIN( gch->alignment + (align-500)/4,  1000 );
    else
    if ( align < -500 )
	newalign  = UMAX( gch->alignment + (align+500)/4, -1000 );
    else
	newalign  = gch->alignment - (int) (gch->alignment / 4);
    
    return newalign;

make it simple instead */

      if(IS_SET( gch->in_room->room_flags2, ROOM_ARENA ))
        return 0;
    return URANGE ( -1000, 
                     (int) ( gch->alignment - victim->alignment/5 ),
                     1000 );

}


/*
 * Calculate how much XP gch should gain for killing victim
 * Lots of redesigning for new exp system by Thoric
 */
int xp_compute( CHAR_DATA *gch, CHAR_DATA *victim )
{
    int align;
    int xp;

    xp	  = (get_exp_worth( victim )
    	  *  URANGE( 1, (victim->skill_level[COMBAT_ABILITY] - gch->skill_level[COMBAT_ABILITY]) + 10, 20 )) / 10;
    align = gch->alignment - victim->alignment;

    /* bonus for attacking opposite alignment */
    if ( align >  990 || align < -990 )
	xp = (xp*5) >> 2;
    else
    /* penalty for good attacking same alignment */
    if ( gch->alignment > 300 && align < 250 )
	xp = (xp*3) >> 2;

    xp = number_range( (xp*3) >> 2, (xp*5) >> 2 );

    /* reduce exp for killing the same mob repeatedly		-Thoric */
    if ( !IS_NPC( gch ) && IS_NPC( victim ) )
    {
	int times = times_killed( gch, victim );

	if ( times >= 5 )
	   xp = 0;
	else
	if ( times )
	   xp = (xp * (5-times)) / 5;
    }

    /* new xp cap for swreality */
    
    return URANGE(1, xp, ( exp_level(  gch->skill_level[COMBAT_ABILITY]+1 ) - exp_level( gch->skill_level[COMBAT_ABILITY]) )   );
}

// Funci�n para puentear bug. hay que externalizarla o cambiarla por la original.

void __bug( const char *archivo, const char* funcion, int linea, const char *fmt, ... )
{
    char buf[MSL * 2], buf2[MSL * 2];
    va_list args;

    va_start( args, fmt );
    vsprintf( buf, fmt, args );
    va_end( args );
    
//	SCOPY( buf2, "%s: %s (%d) %s: %s", archivo, linea, funcion, buf );
//	bug( buf2, 0 );
    return;
}

/*
 * Funci�n String Copy, para copiar un texto dentro de un buffer.
 * Si el buffer conten�a algo, se sustituye por el nuevo contenido.
 */
void __scopy( const char *fun, int line, char *buf, size_t size, char *fmt, ... )
{
	int num;
    va_list args;

	if( !buf )
	{
		BUG( "Sin buffer de destino, llamado en %s (%d)", fun, line );
		return;
	}
	if( !fmt )
	{
		BUG( "Sin texto, llamado en %s (%d)", fun, line );
		return;
	}
	if( strlen( fmt ) >= size )
	{
		BUG( "Texto demasiado largo llamado en %s (%d)", fun, line );
		return;
	}

	va_start( args, fmt );
	num = vsnprintf( buf, size, fmt, args );
	va_end( args );

	if( num < 0 )
	{
		BUG( "Copia de texto erroneoa, llamada en %s (%d)", fun, line );
		return;
	}
	if( num >= size )
	{
		BUG( "Texto demasiado largo(%d/%d), llamado en %s (%d)", num, size, fun, line );
		return;
	}
    return;
}

/*
 * Funci�n String Concatenate, para a�adir un texto a la cola de un buffer.
 * Si el buffer conten�a algo, el nuevo contenido contenido se agrega detras.
 * Un buffer debe estar ***SIEMPRE*** inicializado para usar SCAT() en el.
 */
void __scat( const char *fun, int line, char *buf, size_t size, char *fmt, ... )
{
	char buf2[( size - strlen( buf ) )];
	int num;
    va_list args;

	if( !buf )
	{
		BUG( "Sin buffer de destino, llamado en %s (%d)", fun, line );
		return;
	}
	if( !fmt )
	{
		BUG( "Sin texto, llamado en %s (%d)", fun, line );
		return;
	}
	
	if( ( strlen( fmt ) + strlen( buf ) ) >= size )
	{
		BUG( "Texto demasiado largo(%d/%d) llamado en %s (%d)", strlen( fmt ) + strlen( buf ), size, fun, line );
		return;
	}

	va_start( args, fmt );
	num = vsnprintf( buf2, sizeof( buf2 ), fmt, args );
	va_end( args );
	
	if( num < 0 )
	{
		BUG( "Copia de texto erronea, llamada en %s (%d)", fun, line );
		return;
	}
	if( num >= sizeof( buf2 ) )
	{
		BUG( "Texto demasiado largo(%d/%d), llamado en %s (%d)", num, sizeof( buf2 ), fun, line );
		return;
	}
	
	register char *b = buf;
	register char *b2 = buf2;
	register size_t s = size;
	size_t b_len;

	// Buscamos el final de buf sin sobrepasarlo
	while( s-- != 0 && *b != '\0' )
		b++;
	b_len = b - buf;
	s = size - b_len;
	
	if( s == 0 )
	{
		BUG( "Texto demasiado largo, llamado en %s (%d)", fun, line );
		return;
	}
	
	while( *b2 != '\0' )
	{
		if( s != 1 )
		{
			*b++ = *b2;
			s--;
		}
		b2++;
		if( s == 1 && *b2 != '\0' )
		{
			*b = '\0';
			BUG( "Texto demasiado largo, llamado en %s (%d)", fun, line );
			return;
		}
	}
	*b = '\0';
	return;
}

/*
 * Revamped by Thoric to be more realistic
 */
void dam_message( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt )
{
    char buf1[256], buf2[256], buf3[256];
    char vp[256];
	char vs[256];
	char attack[256];
	char attack2[256];
    char punct;
    sh_int dampc;
    struct skill_type *skill = NULL;
    bool gcflag = FALSE;
    bool gvflag = FALSE;

    if ( ! dam )
      dampc = 0;
    else
      dampc = ( (dam * 1000) / victim->max_hit) + ( 50 - ((victim->hit * 50) / victim->max_hit) );

   /*		     10 * percent					*/
	 if ( dam ==      0 ) { SCOPY( vs, "falla" ); SCOPY( vp, "falla" ); }
    else if ( dampc <=    5 ) { SCOPY( vs, "roza levemente" );SCOPY( vp, "roza levemente" );}
    else if ( dampc <=   10 ) { SCOPY( vs, "roza" ); SCOPY( vp, "roza" ); }
    else if ( dampc <=   20 ) { SCOPY( vs, "ara�a" ); SCOPY( vp, "ara�a" ); }
    else if ( dampc <=   30 ) { SCOPY( vs, "da�a" ); SCOPY( vp, "da�a" ); }
    else if ( dampc <=   40 ) { SCOPY( vs, "causa moratones" );	SCOPY( vp, "causa moratones" ); }
    else if ( dampc <=   50 ) { SCOPY( vs, "golpea" ); SCOPY( vp, "golpea" ); }
    else if ( dampc <=   60 ) { SCOPY( vs, "hiere levemente" );	SCOPY( vp, "hiere levemente" ); }
    else if ( dampc <=   75 ) { SCOPY( vs, "hiere" ); SCOPY( vp, "hiere" ); }
    else if ( dampc <=   80 ) { SCOPY( vs, "hace sangrar" ); SCOPY( vp, "hace sangrar" ); }
    else if ( dampc <=   90 ) { SCOPY( vs, "destripa" ); SCOPY( vp, "destripa" ); }
    else if ( dampc <=  125 ) { SCOPY( vs, "diezma" ); SCOPY( vp, "diezma" ); }
    else if ( dampc <=  150 ) { SCOPY( vs, "devasta" ); SCOPY( vp, "devasta" ); }
    else if ( dampc <=  200 ) { SCOPY( vs, "mutila" ); SCOPY( vp, "mutila" ); }
    else if ( dampc <=  300 ) { SCOPY( vs, "MUTILA" ); SCOPY( vp, "MUTILA" ); }
    else if ( dampc <=  400 ) { SCOPY( vs, "DESCUARTIZA" ); SCOPY( vp, "DESCUARTIZA" );	}
    else if ( dampc <=  500 ) { SCOPY( vs, "MASACRA" ); SCOPY( vp, "MASACRA" );	}
    else if ( dampc <=  600 ) { SCOPY( vs, "PULVERIZA" ); SCOPY( vp, "PULVERIZA" );	}
    else if ( dampc <=  750 ) { SCOPY( vs, "EVISCERA" ); SCOPY( vp, "EVISCERA" ); }
    else if ( dampc <=  990 ) { SCOPY( vs, "* DESCOMPONE *" ); SCOPY( vp, "* DESCOMPONE *" ); }
    else                      { SCOPY( vs, "*** ANIQUILA ***" ); SCOPY( vp, "*** ANIQUILA ***" ); }

    punct   = (dampc <= 30) ? '.' : '!';
//    if ( dam == 0 && (!IS_NPC(ch) && (IS_SET(ch->pcdata->flags, PCFLAG_GAG))))
//		gcflag = TRUE;

//    if ( dam == 0 && (!IS_NPC(victim) && (IS_SET(victim->pcdata->flags, PCFLAG_GAG))))
//		gvflag = TRUE;

    if ( dt >=0 && dt < top_sn )
		skill = skill_table[dt];
	
    if ( dt == (TYPE_HIT + WEAPON_BLASTER ) )	
    {    
         char sound[MAX_STRING_LENGTH];
         int vol = number_range( 20 , 80 );
         
         sprintf( sound , "!!SOUND(blaster V=%d)" , vol );
         sound_to_room(ch->in_room, sound);
    }
    
    if ( dt == TYPE_HIT || dam==0 )
    {
		SCOPY( buf1, "El ataque de $n %s a $N%c", vp, punct );
		SCOPY( buf2, "Tu ataque %s a $N%c", vs, punct );
		SCOPY( buf3, "El ataque de $n te %s%c", vp, punct );
    }
    else if ( dt > TYPE_HIT && is_wielding_poisoned( ch ) )
    {
		if ( dt < TYPE_HIT + sizeof(attack_table)/sizeof(attack_table[0]) )
		{
			SCOPY( attack2, attack_table2[dt - TYPE_HIT] );
			SCOPY( attack, attack_table[dt - TYPE_HIT] );
		}
		else
		{
			bug( "Dam_message: bad dt %d.", dt );
			dt  = TYPE_HIT;
			SCOPY( attack, "golpe" );
			SCOPY( attack2, "El golpe" );
        }

		SCOPY( buf1, "%s con veneno de $n %s a $N%c", attack2, vp, punct );
		SCOPY( buf2, "Tu %s con veneno %s a $N%c", attack, vp, punct );
		SCOPY( buf3, "%s con veneno de $n te %s%c", attack2, vp, punct ); 
    }
    else
    {
		if ( skill )
		{
			SCOPY( attack, skill->noun_damage );
			SCOPY( attack2, "El %s", skill->noun_damage );
			if ( dam == 0 )
			{
				bool found = FALSE;

				if ( skill->miss_char && skill->miss_char[0] != '\0' )
				{
					act( AT_HIT, skill->miss_char, ch, NULL, victim, TO_CHAR );
					found = TRUE;
				}
				if ( skill->miss_vict && skill->miss_vict[0] != '\0' )
				{
					act( AT_HITME, skill->miss_vict, ch, NULL, victim, TO_VICT );
					found = TRUE;
				}
				if ( skill->miss_room && skill->miss_room[0] != '\0' )
				{
					act( AT_ACTION, skill->miss_room, ch, NULL, victim, TO_NOTVICT );
					found = TRUE;
				}
				if ( found )	/* miss message already sent */
					return;
			}
			else
			{
				if ( skill->hit_char && skill->hit_char[0] != '\0' )
					act( AT_HIT, skill->hit_char, ch, NULL, victim, TO_CHAR );
				if ( skill->hit_vict && skill->hit_vict[0] != '\0' )
					act( AT_HITME, skill->hit_vict, ch, NULL, victim, TO_VICT );
				if ( skill->hit_room && skill->hit_room[0] != '\0' )
					act( AT_ACTION, skill->hit_room, ch, NULL, victim, TO_NOTVICT );
			}
		}
		else if ( dt >= TYPE_HIT && dt < TYPE_HIT + sizeof(attack_table)/sizeof(attack_table[0]) )
		{
			SCOPY( attack, attack_table[dt - TYPE_HIT] );
			SCOPY( attack2, attack_table2[dt - TYPE_HIT] );
		}
		else
		{
			bug( "Dam_message: bad dt %d.", dt );
			dt  = TYPE_HIT;
			SCOPY( attack, "golpe" );
			SCOPY( attack2, "El golpe" );
		}

		SCOPY( buf1, "%s de $n %s a $N%c",  attack2, vp, punct );
		SCOPY( buf2, "Tu %s %s a $N%c",  attack, vp, punct );
		SCOPY( buf3, "%s de $n te %s%c", attack2, vp, punct );
    }

    if ( ch->skill_level[COMBAT_ABILITY] >= 15 )
       SCAT( buf2, " haces %d puntos de da�o.", dam );
	if( dt != TYPE_MISSILE )
	{
		act( AT_ACTION, buf1, ch, NULL, victim, TO_NOTVICT );
		if (!gcflag)
			act( AT_HIT, buf2, ch, NULL, victim, TO_CHAR );
		if (!gvflag)
			act( AT_HITME, buf3, ch, NULL, victim, TO_VICT );
    }
    return;
}


void do_kill( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "�matar a qui�n?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "No est� aqu�.\n\r", ch );
	return;
    }

    if ( !IS_NPC(victim) )
    {
	    send_to_char( "Debes asesinar a jugadores.\n\r", ch );
	    return;
    }

   /*
    *
    else
    {
	if ( IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL )
	{
	    send_to_char( "Debes asesinar a una criatura encantada.\n\r", ch );
	    return;
	}
    }
    *
    */

    if ( victim == ch )
    {
	send_to_char( "Te golpeas. �Ouch!\n\r", ch );
	multi_hit( ch, ch, TYPE_UNDEFINED );
	return;
    }

    if ( is_safe( ch, victim ) )
	return;

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
    act( AT_PLAIN, "$N es tu querido maestro.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "�Lo haces lo mejor que puedes!\n\r", ch );
	return;
    }

    if(!IS_NPC(ch))
      ch->pcdata->lost_attacks = 0;
    if ( victim->vip_flags != 0 )
       ch->alignment -= 10;

    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}



void do_murde( CHAR_DATA *ch, char *argument )
{
    send_to_char( "Si quieres asesinar escr�belo entero.\n\r", ch );
    return;
}



void do_murder( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char logbuf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "�Asesinar a qui�n?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "No est� aqu�.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Suicidarse est� mal visto.\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
       return;
        
    if ( IS_AFFECTED(ch, AFF_CHARM) ) 
    {
      if ( ch->master == victim )
      {
        act( AT_PLAIN, "No eres capaz de pensar siquiera en asesinar a $N.", ch, NULL, victim, TO_CHAR );
	return;
      }
    }

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "�Lo haces lo mejor que puedes!\n\r", ch );
	return;
    }
    if(!IS_NPC(ch))
      ch->pcdata->lost_attacks = 0;
    if ( !IS_NPC( victim ) && IS_SET( ch->act, PLR_NICE ) )
    {
      send_to_char( "�Te sientes demasiado bien para hacer esto!\n\r", ch );
      return;
    }

    if (!IS_NPC(ch) && IS_SET(ch->pcdata->act2, ACT_BOUND))
    {
	send_to_char("Eso es algo dif�cil ahora mismo...\n\r", ch);
	return;
    }

    ch->alignment = URANGE( -1000, ch->alignment - 10, 1000 );
    
    if((!ch->in_room->area || !in_arena(ch)) || !IS_NPC(ch) || !IS_NPC(victim))
    {
      sprintf( logbuf, "%s: murder %s", ch->name, argument);
      log_string(logbuf);
    }
    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}

bool in_arena( CHAR_DATA *ch )
{

if ( !str_cmp( ch->in_room->area->filename, "arena.are" ) )
  return TRUE;

if ( ch->in_room->vnum < 29 || ch->in_room->vnum > 43 )
  return FALSE;

return TRUE;
}


void do_flee( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *was_in;
    ROOM_INDEX_DATA *now_in;
    char buf[MAX_STRING_LENGTH];
    int attempt;
    sh_int door;
    EXIT_DATA *pexit;

    if ( !who_fighting( ch ) )
    {
	if ( ch->position == POS_FIGHTING )
	{
	  if ( ch->mount )
	    ch->position = POS_MOUNTED;
	  else
	    ch->position = POS_STANDING;
	}
	send_to_char( "No est�s luchando.\n\r", ch );
	return;
    }

    if ( ch->move <= 0 )
    {
	send_to_char( "�El cansancio te lo impide!\n\r", ch );
	return;
    }

    if ( IS_NPC(ch) && IS_SET(ch->act, ACT_NOFLEE) && !IS_AFFECTED(ch,AFF_CHARM))
    {
	send_to_char( "�Ahora mismo no!\n\r", ch);
	return;
    }

    if (!IS_NPC(ch) && IS_SET(ch->pcdata->act2, ACT_BOUND))
    {
        send_to_char("Eso es un poco complicado ahora mismo...\n\r", ch);
        return;
    }

    /* No fleeing while stunned. - Narn */
    if ( ch->position < POS_FIGHTING )
	return; 

    was_in = ch->in_room;
    for ( attempt = 0; attempt < 8; attempt++ )
    {

	door = number_door( );
	if ( ( pexit = get_exit(was_in, door) ) == NULL
	||   !pexit->to_room
	|| ( IS_SET(pexit->exit_info, EX_CLOSED)
	&&   !IS_AFFECTED( ch, AFF_PASS_DOOR ) )
	|| ( IS_NPC(ch)
	&&   IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB) ) )
	    continue;
 
        affect_strip ( ch, gsn_sneak );
        REMOVE_BIT   ( ch->affected_by, AFF_SNEAK );
	if ( ch->mount && ch->mount->fighting )
	    stop_fighting( ch->mount, TRUE );
	move_char( ch, pexit, 0 );
	if ( ( now_in = ch->in_room ) == was_in )
	    continue;

	ch->in_room = was_in;
	act( AT_FLEE, "�$n corre buscando protecci�n!", ch, NULL, NULL, TO_ROOM );
	ch->in_room = now_in;
	act( AT_FLEE, "$n mira a su alrededor, comprobando si alguien le persigue.", ch, NULL, NULL, TO_ROOM );
        sprintf(buf, "�Corres buscando protecci�n!");
        send_to_char( buf, ch );
    
	stop_fighting( ch, TRUE );
	return;
    }

    sprintf(buf, "�Intentas correr buscando protecci�n!");
    send_to_char( buf, ch );
    return;
}

bool get_cover( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA *was_in;
    ROOM_INDEX_DATA *now_in;
    int attempt;
    sh_int door;
    EXIT_DATA *pexit;

    if ( !who_fighting( ch ) )
	return FALSE;

    if ( ch->position < POS_FIGHTING )
	return FALSE; 

    if ( IS_NPC(ch) && IS_SET(ch->act, ACT_NOFLEE) && !IS_AFFECTED(ch,AFF_CHARM))
	return FALSE;

    was_in = ch->in_room;

    for ( attempt = 0; attempt < 10; attempt++ )
    {

	door = number_door( );
	if ( ( pexit = get_exit(was_in, door) ) == NULL
	||   !pexit->to_room
	|| ( IS_SET(pexit->exit_info, EX_CLOSED)
	&&   !IS_AFFECTED( ch, AFF_PASS_DOOR ) )
	|| ( IS_NPC(ch)
	&&   IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB) ) )
	    continue;
 
        affect_strip ( ch, gsn_sneak );
        REMOVE_BIT   ( ch->affected_by, AFF_SNEAK );
	if ( ch->mount && ch->mount->fighting )
	    stop_fighting( ch->mount, TRUE );
	move_char( ch, pexit, 0 );
	if ( ( now_in = ch->in_room ) == was_in )
	    continue;

	ch->in_room = was_in;
	act( AT_FLEE, "�$n corre buscando protecci�n!", ch, NULL, NULL, TO_ROOM );
	ch->in_room = now_in;
	act( AT_FLEE, "$n corre en c�rculos.", ch, NULL, NULL, TO_ROOM );

	stop_fighting( ch, TRUE );

	return TRUE;
    }

    return FALSE;
}



void do_sla( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to SLAY, spell it out.\n\r", ch );
    return;
}



//Assassin skill - retreat. Fairly shoddily thrown together.
void do_retreat( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *was_in;
    ROOM_INDEX_DATA *now_in;
    char buf[MAX_STRING_LENGTH];
    int edir, chance;
    EXIT_DATA *pexit;
    EXIT_DATA *xit;

    if ( !who_fighting( ch ) )
    {
	if ( ch->position == POS_FIGHTING )
	{
	  if ( ch->mount )
	    ch->position = POS_MOUNTED;
	  else
	    ch->position = POS_STANDING;
	}
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    if ( ch->move <= 0 )
    {
	send_to_char( "You're too exhausted to retreat!\n\r", ch );
	return;
    }

    if ( IS_NPC(ch) && IS_SET(ch->act, ACT_NOFLEE) && !IS_AFFECTED(ch,AFF_CHARM))
    {
	send_to_char( "You're too brave to retreat!\n\r", ch);
	return;
    }

    if ( ch->position < POS_FIGHTING )
	return; 

    if (!IS_NPC(ch) && IS_SET(ch->pcdata->act2, ACT_BOUND))
    {
        send_to_char("Thats a bit hard to do right now...\n\r", ch);
        return;
    }

    if(argument[0] == '\0')
    {
	send_to_char("Retreat in what direction?\n\r", ch);
	return;
    }

    if ( ( edir = get_door( argument ) ) == -1 )
    {
    	send_to_char( "Retreat where??\n\r", ch );
	return;
    }

    chance = IS_NPC(ch) ? ch->top_level : (int)ch->pcdata->learned[gsn_retreat];

    if(chance < number_percent())
    {
      send_to_char("&RYou fail to find the correct exit in the heat of battle!\n\r", ch);   
      learn_from_failure(ch, gsn_retreat);
      return;
    }

    xit = get_exit( ch->in_room, edir );
    was_in = ch->in_room;

    if ( ( pexit = get_exit( ch->in_room, edir ) ) == NULL || !pexit->to_room)
    {
      send_to_char("Theres no exit there!\n\r", ch);
      return;
    }

    if ( IS_SET(pexit->exit_info, EX_CLOSED) && !IS_AFFECTED( ch, AFF_PASS_DOOR ) )
     {
       send_to_char("You slam into the door!\n\r", ch);
       return;
     }
 
    affect_strip ( ch, gsn_sneak );
    REMOVE_BIT   ( ch->affected_by, AFF_SNEAK );
    if ( ch->mount && ch->mount->fighting )
      stop_fighting( ch->mount, TRUE );
    move_char( ch, pexit, 0 );
    if ( ( now_in = ch->in_room ) == was_in )
      return;

    ch->in_room = was_in;
    act( AT_FLEE, "&Y$n makes a hasty retreat.", ch, NULL, NULL, TO_ROOM );
    ch->in_room = now_in;
    act( AT_FLEE, "$n glances around for signs of pursuit.", ch, NULL, NULL, TO_ROOM );
    sprintf(buf, "&YYou make a hasty retreat!");
    send_to_char( buf, ch );
    learn_from_success(ch, gsn_retreat);    
    stop_fighting( ch, TRUE );
    return;
}

#define MAX_HDEATHS 7
#define MAX_DDEATHS 4

struct death_type
{
   char *text;
};

const struct death_type hdeath_table[MAX_HDEATHS] = {
   {"�$n ha muerto!"},
   {"�$n cae al suelo sin vida!"},
   {"$n cae al suelo con un golpe sordo... �HA MUERTO!"},
   {"Lentamente $n cae al suelo. Su vida se ha extinguido."},
   {"La sangre sale a chorros del cuerpo de $n, �cubri�ndote de pies a cabeza!"},
   {"Una mezcla de sangre y �cido estomacal salen del cuerpo de $n."},
   {"�Las entra�as de $n se desparraman por el suelo llen�ndolo todo de sangre y v�sceras!"}
};

const struct death_type ddeath_table[MAX_DDEATHS] = {
   {"�$n explota en pedacitos!"},
   {"�$n comienza a arder y explota!"},
   {"�$n se apaga y cae al suelo!"},
   {"�$n explota en un mont�n de piezas!"}
};

char *rand_death( CHAR_DATA * ch )
{
   static char buf[MAX_STRING_LENGTH];
   int number;

   if( !IS_DROID( ch ) )
   {
      number = number_range( 1, MAX_HDEATHS ) - 1;
      sprintf( buf, "%s\n\r", hdeath_table[number].text );
   }

   else if( IS_DROID( ch ) )
   {
      number = number_range( 1, MAX_DDEATHS ) - 1;
      sprintf( buf, "%s\n\r", ddeath_table[number].text );
   }
   else
      log_string( "rand_death unknown race...it's not droid...and not humanoid" );

   return buf;
} 
