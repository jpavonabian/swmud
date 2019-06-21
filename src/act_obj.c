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
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"
#include "bet.h"

/*double sqrt( double x );*/

/*
 * External functions
 */
void  write_corpses   args( ( CHAR_DATA *ch, char *name ) );
void    show_list_to_char  args( ( OBJ_DATA *list, CHAR_DATA *ch,
				bool fShort, bool fShowNothing ) );
/*
 * Local functions.
 */
void	get_obj		args( ( CHAR_DATA *ch, OBJ_DATA *obj,
			    OBJ_DATA *container ) );
bool	remove_obj	args( ( CHAR_DATA *ch, int iWear, bool fReplace ) );
void	wear_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace, sh_int wear_bit ) );


/*
 * how resistant an object is to damage				-Thoric
 */
sh_int get_obj_resistance( OBJ_DATA *obj )
{
    sh_int resist;

    resist = number_fuzzy(MAX_ITEM_IMPACT);

    /* magical items are more resistant */
    if ( IS_OBJ_STAT( obj, ITEM_MAGIC ) )
      resist += number_fuzzy(12);
    /* blessed objects should have a little bonus */
    if ( IS_OBJ_STAT( obj, ITEM_BLESS ) )
      resist += number_fuzzy(5);
    /* lets make store inventory pretty tough */
    if ( IS_OBJ_STAT( obj, ITEM_INVENTORY ) )
      resist += 20;

    /* okay... let's add some bonus/penalty for item level... */
    resist += (obj->level / 10);

    /* and lasty... take armor or weapon's condition into consideration */
    if (obj->item_type == ITEM_ARMOR || obj->item_type == ITEM_WEAPON)
      resist += (obj->value[0]);

    return URANGE(10, resist, 99);
}


void get_obj( CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container )
{
    CLAN_DATA *clan;
    int weight;

    if ( !CAN_WEAR(obj, ITEM_TAKE)
       && (ch->top_level < sysdata.level_getobjnotake )  )
    {
	send_to_char( "No puedes coger eso.\n\r", ch );
	return;
    }

    if ( IS_OBJ_STAT( obj, ITEM_PROTOTYPE )
    &&  !can_take_proto( ch ) )
    {
	send_to_char( "Algo te impide coger eso.\n\r", ch );
	return;
    }
   if(obj->item_type != ITEM_MONEY)
   {
    if ( (ch->carry_number + get_obj_number( obj ) > can_carry_n( ch )))
    {
	act( AT_PLAIN, "$d: No puedes cargar más objetos.",
		ch, NULL, obj->name, TO_CHAR );
	return;
    }
   }

    if ( IS_OBJ_STAT( obj, ITEM_COVERING ) )
      weight = obj->weight;
    else
      weight = get_obj_weight( obj );

    if ( ch->carry_weight + weight > can_carry_w( ch ) )
    {
	act( AT_PLAIN, "$d: no puedes cargar tanto peso.",
		ch, NULL, obj->name, TO_CHAR );
	return;
    }

    if ( container )
    {
	act( AT_ACTION, IS_OBJ_STAT(container, ITEM_COVERING) ? 
		"Coges $p de $P." : "Coges $p de $P",
		ch, obj, container, TO_CHAR );
	act( AT_ACTION, IS_OBJ_STAT(container, ITEM_COVERING) ?
		"$n coge $p de $P." : "$n coge $p de $P",
		ch, obj, container, TO_ROOM );
	obj_from_obj( obj );
    }
    else
    {
	act( AT_ACTION, "Coges $p.", ch, obj, container, TO_CHAR );
	act( AT_ACTION, "$n coge $p.", ch, obj, container, TO_ROOM );
	obj_from_room( obj );
    }

    /* Clan storeroom checks */
    if ( IS_SET(ch->in_room->room_flags, ROOM_CLANSTOREROOM) 
    && (!container || container->carried_by == NULL) )
	for ( clan = first_clan; clan; clan = clan->next )
	  if ( clan->storeroom == ch->in_room->vnum )
	    save_clan_storeroom(ch, clan);

    if ( obj->item_type != ITEM_CONTAINER )
      check_for_trap( ch, obj, TRAP_GET );
    if ( char_died(ch) )
      return;

    if ( obj->item_type == ITEM_MONEY )
    {
	ch->gold += obj->value[0];
	extract_obj( obj );
    }
    else
    {
	obj = obj_to_char( obj, ch );
    }

    if ( char_died(ch) || obj_extracted(obj) )
      return;
    oprog_get_trigger(ch, obj);
    return;
}


void do_get( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    OBJ_DATA *container;
    sh_int number;
    bool found;

    argument = one_argument( argument, arg1 );
    if ( is_number(arg1) )
    {
	number = atoi(arg1);
	if ( number < 1 )
	{
	    send_to_char( "Eso fue fácil...\n\r", ch );
	    return;
	}
	if ( (ch->carry_number + number) > can_carry_n(ch) )
	{
	    send_to_char( "No puedes cargar tantos objetos.\n\r", ch );
	    return;
	}
	argument = one_argument( argument, arg1 );
    }
    else
	number = 0;
    argument = one_argument( argument, arg2 );
    /* munch optional words */
    if ( !str_cmp( arg2, "de" ) && argument[0] != '\0' )
	argument = one_argument( argument, arg2 );

    /* Get type. */
    if ( arg1[0] == '\0' )
    {
	send_to_char( "¿Coger qué?\n\r", ch );
	return;
    }

    if ( ms_find_obj(ch) )
	return;

    if ( arg2[0] == '\0' )
    {
	if ( number <= 1 && str_cmp( arg1, "todo" ) && str_prefix( "todo.", arg1 ) )
	{
	    /* 'get obj' */
	    obj = get_obj_list( ch, arg1, ch->in_room->first_content );
	    if ( !obj )
	    {
		act( AT_PLAIN, "No veo $T aquí.", ch, NULL, arg1, TO_CHAR );
		return;
	    }
	    separate_obj(obj);
	    get_obj( ch, obj, NULL );
	    if ( char_died(ch) )
		return;
	    if ( IS_SET( sysdata.save_flags, SV_GET ) )
		save_char_obj( ch );
	}
	else
	{
	    sh_int cnt = 0;
	    bool fAll;
	    char *chk;

	    if ( IS_SET( ch->in_room->room_flags, ROOM_DONATION ) )
	    {
		send_to_char( "¡La codicia es mala!\n\r", ch );
		return;
	    }
	    if ( !str_cmp(arg1, "todo") )
		fAll = TRUE;
	    else
		fAll = FALSE;
	    if ( number > 1 )
		chk = arg1;
	    else
		chk = &arg1[4];
	    /* 'get all' or 'get all.obj' */
	    found = FALSE;
	    for ( obj = ch->in_room->first_content; obj; obj = obj_next )
	    {
		obj_next = obj->next_content;
		if ( ( fAll || nifty_is_name( chk, obj->name ) )
		&&   can_see_obj( ch, obj ) )
		{
		    found = TRUE;
		    if ( number && (cnt + obj->count) > number )
			split_obj( obj, number - cnt );
		    cnt += obj->count;
		    get_obj( ch, obj, NULL );
		    if ( char_died(ch)
		    ||   ch->carry_number >= can_carry_n( ch )
		    ||   ch->carry_weight >= can_carry_w( ch )
		    ||   (number && cnt >= number) )
		    {
			if ( IS_SET(sysdata.save_flags, SV_GET)
			&&  !char_died(ch) )
			    save_char_obj(ch);
			return;
		    }
		}
	    }

	    if ( !found )
	    {
		if ( fAll )
		  send_to_char( "No veo nada aquí.\n\r", ch );
		else
		  act( AT_PLAIN, "No veo $T aquí.", ch, NULL, chk, TO_CHAR );
	    }
	    else
	    if ( IS_SET( sysdata.save_flags, SV_GET ) )
		save_char_obj( ch );
	}
    }
    else
    {
	/* 'get ... container' */
	if ( !str_cmp( arg2, "todo" ) || !str_prefix( "todo.", arg2 ) )
	{
	    send_to_char( "No peudes hacer eso.\n\r", ch );
	    return;
	}

	if ( ( container = get_obj_here( ch, arg2 ) ) == NULL )
	{
	    act( AT_PLAIN, "No veo $T aquí.", ch, NULL, arg2, TO_CHAR );
	    return;
	}

	switch ( container->item_type )
	{
	default:
	    if ( !IS_OBJ_STAT( container, ITEM_COVERING ) )
	    {
		send_to_char( "Eso no es un contenedor.\n\r", ch );
		return;
	    }
	    if ( ch->carry_weight + container->weight > can_carry_w( ch ) )
	    {
		send_to_char( "Pesa demasiado.\n\r", ch );
		return;
	    }
	    break;

	case ITEM_CONTAINER:
	case ITEM_DROID_CORPSE:
	case ITEM_CORPSE_PC:
	case ITEM_CORPSE_NPC:
	    break;
	}

	if ( !IS_OBJ_STAT(container, ITEM_COVERING )
	&&    IS_SET(container->value[1], CONT_CLOSED) )
	{
	    act( AT_PLAIN, "Abre $d primero.", ch, NULL, container->name, TO_CHAR );
	    return;
	}

	if ( number <= 1 && str_cmp( arg1, "todo" ) && str_prefix( "todo.", arg1 ) )
	{
	    /* 'get obj container' */
	    obj = get_obj_list( ch, arg1, container->first_content );
	    if ( !obj )
	    {
		act( AT_PLAIN, IS_OBJ_STAT(container, ITEM_COVERING) ?
			"No veo nada como eso en $T." :
			"No veo nada como eso en $T.",
			ch, NULL, arg2, TO_CHAR );
		return;
	    }
	    separate_obj(obj);
	    get_obj( ch, obj, container );

	    check_for_trap( ch, container, TRAP_GET );
	    if ( char_died(ch) )
	      return;
	    if ( IS_SET( sysdata.save_flags, SV_GET ) )
		save_char_obj( ch );
	}
	else
	{
	    int cnt = 0;
	    bool fAll;
	    char *chk;

	    /* 'get all container' or 'get all.obj container' */
	    if ( IS_OBJ_STAT( container, ITEM_DONATION ) )
	    {
		send_to_char( "¡la codicia es mala!\n\r", ch );
		return;
	    }
	    if ( !str_cmp(arg1, "todo") )
		fAll = TRUE;
	    else
		fAll = FALSE;
	    if ( number > 1 )
		chk = arg1;
	    else
		chk = &arg1[4];
	    found = FALSE;
	    for ( obj = container->first_content; obj; obj = obj_next )
	    {
		obj_next = obj->next_content;
		if ( ( fAll || nifty_is_name( chk, obj->name ) )
		&&   can_see_obj( ch, obj ) )
		{
		    found = TRUE;
		    if ( number && (cnt + obj->count) > number )
			split_obj( obj, number - cnt );
		    cnt += obj->count;
		    get_obj( ch, obj, container );
		    if ( char_died(ch)
		    ||   ch->carry_number >= can_carry_n( ch )
		    ||   ch->carry_weight >= can_carry_w( ch )
		    ||   (number && cnt >= number) )
                    {
                       if( container->item_type == ITEM_CORPSE_PC ) 
                         write_corpses( NULL, container->short_descr + 14 ); 
                       if( found && IS_SET( sysdata.save_flags, SV_GET ) ) 
                         save_char_obj( ch ); 
		      return;
                    }
		}
	    }

	    if ( !found )
	    {
		if ( fAll )
		  act( AT_PLAIN, IS_OBJ_STAT(container, ITEM_COVERING) ?
			"No veo nada en $T." :
			"No veo nada en $T.",
			ch, NULL, arg2, TO_CHAR );
		else
		  act( AT_PLAIN, IS_OBJ_STAT(container, ITEM_COVERING) ?
			"No veo nada como eso en $T." :
			"No veo nada como eso en $T.",
			ch, NULL, arg2, TO_CHAR );
	    }
	    else
	      check_for_trap( ch, container, TRAP_GET );
	    if ( char_died(ch) )
		return;
	    if ( found && IS_SET( sysdata.save_flags, SV_GET ) )
		save_char_obj( ch );
	}
    }
    return;
}



void do_put( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *container;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    CLAN_DATA *clan;
    sh_int	count;
    int		number;
    bool	save_char = FALSE;

    argument = one_argument( argument, arg1 );
    if ( is_number(arg1) )
    {
	number = atoi(arg1);
	if ( number < 1 )
	{
	    send_to_char( "Demasiado fácil...\n\r", ch );
	    return;
	}
	argument = one_argument( argument, arg1 );
    }
    else
	number = 0;
    argument = one_argument( argument, arg2 );
    /* munch optional words */
    if ( (!str_cmp(arg2, "dentro") || !str_cmp(arg2, "en") || !str_cmp(arg2, "sobre"))
    &&   argument[0] != '\0' )
	argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "¿Poner qué dónde?\n\r", ch );
	return;
    }

    if ( ms_find_obj(ch) )
	return;

    if ( !str_cmp( arg2, "todo" ) || !str_prefix( "todo.", arg2 ) )
    {
	send_to_char( "No puedes hacer eso.\n\r", ch );
	return;
    }

    if ( ( container = get_obj_here( ch, arg2 ) ) == NULL )
    {
	act( AT_PLAIN, "No ves $T aquí.", ch, NULL, arg2, TO_CHAR );
	return;
    }

    if ( !container->carried_by && IS_SET( sysdata.save_flags, SV_PUT ) )
	save_char = TRUE;

    if ( IS_OBJ_STAT(container, ITEM_COVERING) )
    {
	if ( ch->carry_weight + container->weight > can_carry_w( ch ) )
	{
	    send_to_char( "Pesa demasiado para moverlo.\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( container->item_type != ITEM_CONTAINER )
	{
	    send_to_char( "Eso no es un contenedor.\n\r", ch );
	    return;
	}

	if ( IS_SET(container->value[1], CONT_CLOSED) )
	{
	    act( AT_PLAIN, "abre primero $d.", ch, NULL, container->name, TO_CHAR );
	    return;
	}
    }

    if ( number > 0 )
    {
	/* 'put NNNN coins object' */

	if ( !str_cmp( arg1, "credito" ) || !str_cmp( arg1, "creditos" ) )
	{
	    if ( ch->gold < number )
	    {
		send_to_char( "No tienes tantos créditos.\n\r", ch );
		return;
	    }

	    if (!IS_NPC(ch) && ch->top_level < 11)
	    {
		send_to_char( "Para evitar trampas, los jugadores de tu nivel no pueden mover créditos.\n\r", ch);
		return;
	    }

	    ch->gold -= number;

	    for ( obj = container->first_content; obj; obj = obj_next )
	    {
		obj_next = obj->next_content;

		switch ( obj->pIndexData->vnum )
		{
		case OBJ_VNUM_MONEY_ONE:
		   number += 1;
		   extract_obj( obj );
		   break;

		case OBJ_VNUM_MONEY_SOME:
		   number += obj->value[0];
		   extract_obj( obj );
		   break;
		}
	    }

            act( AT_ACTION, "$n pone algunos créditos en $P.", ch, NULL, container, TO_ROOM );
	    obj_to_obj( create_money( number ), container );
	    send_to_char( "Bien.\n\r", ch );
	    if ( IS_SET( sysdata.save_flags, SV_DROP ) )
		save_char_obj( ch );
	    return;
	}
    }

    if ( number <= 1 && str_cmp( arg1, "todo" ) && str_prefix( "todo.", arg1 ) )
    {
	/* 'put obj container' */
	if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
	{
	    send_to_char( "No tienes ese objeto.\n\r", ch );
	    return;
	}

	if ( obj == container )
	{
	    send_to_char( "Sería un poco difícil.\n\r", ch );
	    return;
	}

	if ( !can_drop_obj( ch, obj ) )
	{
	    send_to_char( "No puedes dejarlo ir.\n\r", ch );
	    return;
	}

	if ( (IS_OBJ_STAT(container, ITEM_COVERING)
	&&   (get_obj_weight( obj ) / obj->count)
	  > ((get_obj_weight( container ) / container->count)
	  -   container->weight)) )
	{
	    send_to_char( "No cabrá.\n\r", ch );
	    return;
	}

	if ( (get_obj_weight( obj ) / obj->count)
	   + (get_obj_weight( container ) / container->count)
	   >  container->value[0] )
	{
	    send_to_char( "No cabrá.\n\r", ch );
	    return;
	}

	if(obj->item_type == ITEM_GRENADE && obj->timer > 0)
	{
	    send_to_char("¿Poner una granada armada en un contenedor? Eso es demasiado estÃºpido.\n\r", ch);
	    return;
	}

	separate_obj(obj);
	separate_obj(container);
	obj_from_char( obj );
	obj = obj_to_obj( obj, container );
	check_for_trap ( ch, container, TRAP_PUT );
	if ( char_died(ch) )
	  return;
	count = obj->count;
	obj->count = 1;
        if ( !oprog_use_trigger( ch, container, NULL, NULL, NULL ) )
        {
 	act( AT_ACTION, IS_OBJ_STAT( container, ITEM_COVERING )
 		? "$n oculta $p bajo $P." : "$n pone $p en $P.",
		ch, obj, container, TO_ROOM );
	act( AT_ACTION, IS_OBJ_STAT( container, ITEM_COVERING )
		? "Ocultas $p bajo $P." : "Pones $p en $P.",
		ch, obj, container, TO_CHAR );
	}
	obj->count = count;

	if ( save_char )
	  save_char_obj(ch);
	/* Clan storeroom check */
	if ( IS_SET(ch->in_room->room_flags, ROOM_CLANSTOREROOM) 
	&&   container->carried_by == NULL)
	   for ( clan = first_clan; clan; clan = clan->next )
	      if ( clan->storeroom == ch->in_room->vnum )
		save_clan_storeroom(ch, clan);
    }
    else
    {
	bool found = FALSE;
	int cnt = 0;
	bool fAll;
	char *chk;

	if ( !str_cmp(arg1, "todo") )
	    fAll = TRUE;
	else
	    fAll = FALSE;
	if ( number > 1 )
	    chk = arg1;
	else
	    chk = &arg1[4];

        if( container->pIndexData->vnum == 1097 && fAll)
	{
	    send_to_char("¡No puedes! ¡De uno en uno!\n\r", ch);
	    return;
	}

	separate_obj(container);
	/* 'put all container' or 'put all.obj container' */
	for ( obj = ch->first_carrying; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;

	    if ( ( fAll || nifty_is_name( chk, obj->name ) )
	    &&   can_see_obj( ch, obj )
	    &&   obj->wear_loc == WEAR_NONE
	    &&   obj != container
	    &&   can_drop_obj( ch, obj )
	    &&   get_obj_weight( obj ) + get_obj_weight( container )
		 <= container->value[0] )
	    {
		if ( number && (cnt + obj->count) > number )
		    split_obj( obj, number - cnt );
		cnt += obj->count;
		obj_from_char( obj );
        if ( !oprog_use_trigger( ch, container, NULL, NULL, NULL ) )
        {
		act( AT_ACTION, "$n pone $p en $P.", ch, obj, container, TO_ROOM );
		act( AT_ACTION, "Pones $p en $P.", ch, obj, container, TO_CHAR );
        }
		obj = obj_to_obj( obj, container );
		found = TRUE;

		check_for_trap( ch, container, TRAP_PUT );
		if ( char_died(ch) )
		  return;
		if ( number && cnt >= number )
		  break;
	    }
	}

	/*
	 * Don't bother to save anything if nothing was dropped   -Thoric
	 */
	if ( !found )
	{
	    if ( fAll )
	      act( AT_PLAIN, "No estás cargando nada.",
		    ch, NULL, NULL, TO_CHAR );
	    else
	      act( AT_PLAIN, "No estás cargando nada como $T.",
		    ch, NULL, chk, TO_CHAR );
	    return;
	}

	if ( save_char )
	    save_char_obj(ch);
	/* Clan storeroom check */
        if ( IS_SET(ch->in_room->room_flags, ROOM_CLANSTOREROOM) 
	&& container->carried_by == NULL )
	  for ( clan = first_clan; clan; clan = clan->next )
	     if ( clan->storeroom == ch->in_room->vnum )
        	save_clan_storeroom(ch, clan);
    }

    return;
}


void do_drop( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char logbuf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    bool found;
    CLAN_DATA *clan;
    int number;

    argument = one_argument( argument, arg );
    if ( is_number(arg) )
    {
	number = atoi(arg);
	if ( number < 1 )
	{
	    send_to_char( "Demasiado fácil...\n\r", ch );
	    return;
	}
	argument = one_argument( argument, arg );
    }
    else
	number = 0;

    if ( arg[0] == '\0' )
    {
	send_to_char( "¿dejar qué?\n\r", ch );
	return;
    }

    if ( ms_find_obj(ch) )
	return;

    if ( IS_SET( ch->in_room->room_flags, ROOM_NODROP )
    ||   ( !IS_NPC(ch) && IS_SET( ch->act, PLR_LITTERBUG )) )
    {
       set_char_color( AT_MAGIC, ch );
       send_to_char( "¡Algo te detiene!\n\r", ch );
       set_char_color( AT_TELL, ch );
       send_to_char( "¡nada de basura aquí!'\n\r", ch );
       return;
    }

    if ( number > 0 )
    {
	/* 'drop NNNN coins' */

	if ( !str_cmp( arg, "creditos" ) || !str_cmp( arg, "credito" ) )
	{
	    if ( ch->gold < number )
	    {
		send_to_char( "No tienes tantos créditos.\n\r", ch );
		return;
	    }

	    if (!IS_NPC(ch) && ch->top_level < 11)
	    {
		send_to_char( "Para evitar trampas, los personajes de tu nivel no pueden mover créditos.\n\r", ch);
		return;
	    }

	    ch->gold -= number;

	    for ( obj = ch->in_room->first_content; obj; obj = obj_next )
	    {
		obj_next = obj->next_content;

		switch ( obj->pIndexData->vnum )
		{
		case OBJ_VNUM_MONEY_ONE:
		   number += 1;
		   extract_obj( obj );
		   break;

		case OBJ_VNUM_MONEY_SOME:
		   number += obj->value[0];
		   extract_obj( obj );
		   break;
		}
	    }

	    act( AT_ACTION, "$n deja algunos créditos.", ch, NULL, NULL, TO_ROOM );
	    obj_to_room( create_money( number ), ch->in_room );
	    send_to_char( "bien.\n\r", ch );
	    if ( !IS_NPC(ch) && IS_IMMORTAL(ch))
	     {
		sprintf(logbuf, "%s dropped %d credits.", ch->name, number);
		log_string(logbuf);
	     }
	    if ( IS_SET( sysdata.save_flags, SV_DROP ) )
		save_char_obj( ch );
	    return;
	}
    }

    if ( number <= 1 && str_cmp( arg, "todo" ) && str_prefix( "todo.", arg ) )
    {
	/* 'drop obj' */
	if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
	{
	    send_to_char( "No tienes ese objeto.\n\r", ch );
	    return;
	}

	if ( !can_drop_obj( ch, obj ) )
	{
	    send_to_char( "No puedes dejarlo ir.\n\r", ch );
	    return;
	}

	separate_obj( obj );
	act( AT_ACTION, "$n deja $p.", ch, obj, NULL, TO_ROOM );
	act( AT_ACTION, "Dejas $p.", ch, obj, NULL, TO_CHAR );
	    if ( !IS_NPC(ch) && IS_IMMORTAL(ch))
	     {
		sprintf(logbuf, "%s dropped %s.", ch->name, obj->short_descr);
		log_string(logbuf);
	     }

	obj_from_char( obj );
	obj = obj_to_room( obj, ch->in_room );
	oprog_drop_trigger ( ch, obj );   /* mudprogs */

        if( char_died(ch) || obj_extracted(obj) )
          return;

	/* Clan storeroom saving */
	if ( IS_SET(ch->in_room->room_flags, ROOM_CLANSTOREROOM) )
	   for ( clan = first_clan; clan; clan = clan->next )
 	      if ( clan->storeroom == ch->in_room->vnum )
		save_clan_storeroom(ch, clan);
    }
    else
    {
	int cnt = 0;
	char *chk;
	bool fAll;

	if ( !str_cmp(arg, "todo") )
	    fAll = TRUE;
	else
	    fAll = FALSE;
	if ( number > 1 )
	    chk = arg;
	else
	    chk = &arg[4];
	/* 'drop all' or 'drop all.obj' */
	if ( IS_SET( ch->in_room->room_flags, ROOM_NODROPALL ) )
	{
	    send_to_char( "No puedes hacer eso aquí...\n\r", ch );
	    return;
	}
	found = FALSE;
	for ( obj = ch->first_carrying; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;

	    if ( (fAll || nifty_is_name( chk, obj->name ) )
	    &&   can_see_obj( ch, obj )
	    &&   obj->wear_loc == WEAR_NONE
	    &&   can_drop_obj( ch, obj ) )
	    {
		found = TRUE;
		if ( obj->pIndexData->progtypes & DROP_PROG && obj->count > 1 ) 
		{
		   ++cnt;
		   separate_obj( obj );
		   obj_from_char( obj );
		   if ( !obj_next )
		     obj_next = ch->first_carrying;
		}
		else
		{
		   if ( number && (cnt + obj->count) > number )
		     split_obj( obj, number - cnt );
		   cnt += obj->count;
		   obj_from_char( obj );
		}
		act( AT_ACTION, "$n deja $p.", ch, obj, NULL, TO_ROOM );
		act( AT_ACTION, "Dejas $p.", ch, obj, NULL, TO_CHAR );
		obj = obj_to_room( obj, ch->in_room );
		    if ( !IS_NPC(ch) && IS_IMMORTAL(ch))
		     {
			sprintf(logbuf, "%s dropped %s.", ch->name, obj->short_descr);
			log_string(logbuf);
		     }
		oprog_drop_trigger( ch, obj );		/* mudprogs */
                if ( char_died(ch) )
                    return;
		if ( number && cnt >= number )
		    break;
	    }
	}

        if ( IS_SET(ch->in_room->room_flags, ROOM_CLANSTOREROOM) )
	   for ( clan = first_clan; clan; clan = clan->next )
 	      if ( clan->storeroom == ch->in_room->vnum )
		save_clan_storeroom(ch, clan);
	
	if ( !found )
	{
	    if ( fAll )
	      act( AT_PLAIN, "No estás cargando nada.",
		    ch, NULL, NULL, TO_CHAR );
	    else
	      act( AT_PLAIN, "No estás cargando nada como $T.",
		    ch, NULL, chk, TO_CHAR );
	}
    }
    if ( IS_SET( sysdata.save_flags, SV_DROP ) )
	save_char_obj( ch );	/* duping protector */
    return;
}



void do_give( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char logbuf[MAX_STRING_LENGTH];
    char buf  [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA  *obj;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( !str_cmp( arg2, "a" ) && argument[0] != '\0' )
	argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "¿Dar qué a quién?\n\r", ch );
	return;
    }

    if ( ms_find_obj(ch) )
	return;

    if ( is_number( arg1 ) )
    {
	/* 'give NNNN coins victim' */
	int amount;

	amount   = atoi(arg1);
	if ( amount <= 0
	|| ( str_cmp( arg2, "creditos" ) && str_cmp( arg2, "credito" ) ) )
	{
	    send_to_char( "Lo siento, no puedes hacer eso.\n\r", ch );
	    return;
	}

	argument = one_argument( argument, arg2 );
	if ( !str_cmp( arg2, "a" ) && argument[0] != '\0' )
	  argument = one_argument( argument, arg2 );
	if ( arg2[0] == '\0' )
	{
	    send_to_char( "¿Dar qué a quién?\n\r", ch );
	    return;
	}

	if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "No está aquí.\n\r", ch );
	    return;
	}

	if ( ch->gold < amount )
	{
	    send_to_char( "Muy generoso por tu parte, pero no tienes tantos créditos.\n\r", ch );
	    return;
	}


	if (!IS_NPC(ch) && ch->top_level < 11)
	{
	    send_to_char( "Para evitar trampas, los jugadores de tu nivel no pueden mover créditos.\n\r", ch);
	    return;
	}

	ch->gold     -= amount;
	victim->gold += amount;
	sprintf(buf, "Recibes %d %s de $n.", amount, (amount > 1) ? "créditos" : "crédito");
/*
        strcpy(buf, "$n te da ");
        strcat(buf, arg1);
        strcat(buf, (amount > 1) ? " créditos." : " crédito.");
*/
	act( AT_ACTION, buf, ch, NULL, victim, TO_VICT    );
	sprintf(buf, "Das a $N %d %s", amount, (amount > 1) ? "créditos." : "créditp.");
	act( AT_ACTION, "$n da a $N algunos créditos.",  ch, NULL, victim, TO_NOTVICT );
	act( AT_ACTION, buf,  ch, NULL, victim, TO_CHAR    );
	send_to_char( "bien.\n\r", ch );
	if(!IS_NPC(ch) && !IS_NPC(victim) && IS_IMMORTAL(ch))
	{
	 sprintf(logbuf, "%s gives %s %d credits.", ch->name, victim->name, amount);
	 log_string(logbuf);
	}
	mprog_bribe_trigger( victim, ch, amount );
	if ( IS_SET( sysdata.save_flags, SV_GIVE ) && !char_died(ch) )
	  save_char_obj(ch);
	if ( IS_SET( sysdata.save_flags, SV_RECEIVE ) && !char_died(victim) )
	  save_char_obj(victim);
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
    {
	send_to_char( "No tienes ese objeto.\n\r", ch );
	return;
    }

    if ( obj->wear_loc != WEAR_NONE )
    {
	send_to_char( "Deja de usar primero el objeto.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
    {
	send_to_char( "No está aquí.\n\r", ch );
	return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
	send_to_char( "No puedes dejarlo ir.\n\r", ch );
	return;
    }

    if ( victim->carry_number + (get_obj_number(obj)/obj->count) > can_carry_n( victim ) && !IS_NPC(victim))
    {
	act( AT_PLAIN, "$N tiene sus manos llenas.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( victim->carry_weight + (get_obj_weight(obj)/obj->count) > can_carry_w( victim ) )
    {
	act( AT_PLAIN, "$N no puede cargar tanto peso.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( !can_see_obj( victim, obj ) )
    {
	act( AT_PLAIN, "$N no puede ver el objeto.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if (IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) && !can_take_proto( victim ) )
    {
	act( AT_PLAIN, "¡No puedes darle eso a $N!", ch, NULL, victim, TO_CHAR );
	return;
    }

    separate_obj( obj );
    obj_from_char( obj );
    act( AT_ACTION, "$n da $p a $N.", ch, obj, victim, TO_NOTVICT );
    act( AT_ACTION, "$n te da $p.",   ch, obj, victim, TO_VICT    );
    act( AT_ACTION, "Das $p a $N.", ch, obj, victim, TO_CHAR    );
    obj = obj_to_char( obj, victim );
	if(!IS_NPC(ch) && !IS_NPC(victim) && IS_IMMORTAL(ch))
	{
	 sprintf(logbuf, "%s gives %s to %s.", ch->name, obj->short_descr, victim->name);
	 log_string(logbuf);
	}
                              
    mprog_give_trigger( victim, ch, obj );
    if ( IS_SET( sysdata.save_flags, SV_GIVE ) && !char_died(ch) )
	save_char_obj(ch);
    if ( IS_SET( sysdata.save_flags, SV_RECEIVE ) && !char_died(victim) )
	save_char_obj(victim);
    return;
}

/*
 * Damage an object.						-Thoric
 * Affect player's AC if necessary.
 * Make object into scraps if necessary.
 * Send message about damaged object.
 */
obj_ret damage_obj( OBJ_DATA *obj )
{
    CHAR_DATA *ch;
    obj_ret objcode;

    ch = obj->carried_by;
    objcode = rNONE;
  
    separate_obj( obj );
    if ( ch )
      act( AT_OBJECT, "($p ha recibido daños)", ch, obj, NULL, TO_CHAR );
    else
    if ( obj->in_room && ( ch = obj->in_room->first_person ) != NULL )
    {
	act( AT_OBJECT, "($p ha recibido daños)", ch, obj, NULL, TO_ROOM );
	act( AT_OBJECT, "($p ha recibido daños)", ch, obj, NULL, TO_CHAR );
	ch = NULL;
    }

    oprog_damage_trigger(ch, obj);
    if ( obj_extracted(obj) )
      return global_objcode;

    switch( obj->item_type )
    {
	default:
	  make_scraps( obj );
	  objcode = rOBJ_SCRAPPED;
	  break;
	case ITEM_CONTAINER:
	  if (--obj->value[3] <= 0) 
	  {
		make_scraps( obj );
		objcode = rOBJ_SCRAPPED;
	  }
	  break;
	case ITEM_ARMOR:
	  if ( ch && obj->value[0] >= 1 )
	    ch->armor += apply_ac( obj, obj->wear_loc );
	  if (--obj->value[0] <= 0)
	  {
		make_scraps( obj );
		objcode = rOBJ_SCRAPPED;
	  }
	  else
	  if ( ch && obj->value[0] >= 1 )
	    ch->armor -= apply_ac( obj, obj->wear_loc );
	  break;
	case ITEM_WEAPON:
	  if (--obj->value[0] <= 0)
	  {
		make_scraps( obj );
		objcode = rOBJ_SCRAPPED;
	  }
	  break;
    }
    if( ch != NULL )
      save_char_obj( ch ); /* Stop scrap duping - Samson 1-2-00 */

    return objcode;
}


/*
 * Remove an object.
 */
bool remove_obj( CHAR_DATA *ch, int iWear, bool fReplace )
{
    OBJ_DATA *obj, *tmpobj;

    if ( ( obj = get_eq_char( ch, iWear ) ) == NULL )
	return TRUE;

    if ( !fReplace
    &&   ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
    {
	act( AT_PLAIN, "$d: no puedes cargar más objetos.",
	    ch, NULL, obj->name, TO_CHAR );
	return FALSE;
    }

    if ( !fReplace )
	return FALSE;

    if ( IS_OBJ_STAT(obj, ITEM_NOREMOVE) )
    {
	act( AT_PLAIN, "No puedes dejar de usar $p.", ch, obj, NULL, TO_CHAR );
	return FALSE;
    }

    if ( obj == get_eq_char( ch, WEAR_WIELD )
    && ( tmpobj = get_eq_char( ch, WEAR_DUAL_WIELD)) != NULL )
       tmpobj->wear_loc = WEAR_WIELD;

    unequip_char( ch, obj );

    act( AT_ACTION, "$n deja de usar $p.", ch, obj, NULL, TO_ROOM );
    act( AT_ACTION, "Dejas de usar $p.", ch, obj, NULL, TO_CHAR );
    oprog_remove_trigger( ch, obj );
    return TRUE;
}

/*
 * See if char could be capable of dual-wielding		-Thoric
 */
bool could_dual( CHAR_DATA *ch )
{
  if ( IS_NPC(ch) )
    return TRUE;
  if ( ch->pcdata->learned[gsn_dual_wield] )
    return TRUE;

  return FALSE;
}

/*
* Check for the ability to dual-wield under all conditions.  -Orion
*
* Original version by Thoric.
*/
bool can_dual( CHAR_DATA *ch )
{
    /*
    * We must assume that when they come in, they are NOT wielding something. We
    * take care of the actual value later. -Orion
    */
    bool wielding[2], alreadyWielding = FALSE;
    wielding[0] = FALSE;
    wielding[1] = FALSE;

    /*
    * If they don't have the ability to dual-wield, why should we allow them to
    * do so? -Orion
    */
    if ( !could_dual(ch) )
        return FALSE;

    /*
    * Get that true wielding value I mentioned earlier. If they're wielding and
    * missile wielding, we can simply return FALSE. If not, set the values. -Orion
    */
    if ( get_eq_char( ch, WEAR_WIELD ) && get_eq_char( ch, WEAR_MISSILE_WIELD ) )
    {
        send_to_char( "¡Ya estás usando dos armas!\n\r", ch );
        return FALSE;
    }
    else
    {
        /*
        * Wield position. -Orion
        */
        wielding[0] = get_eq_char( ch, WEAR_WIELD ) ? TRUE : FALSE;
        /*
        * Missile wield position. -Orion
        */
        wielding[1] = get_eq_char( ch, WEAR_MISSILE_WIELD ) ? TRUE : FALSE;
    }

    /*
    * Save some extra typing farther down. -Orion
    */
    if ( wielding[0] || wielding[1] )
        alreadyWielding = TRUE;

    /*
    * If wielding and dual wielding, then they can't wear another weapon. Return
    * FALSE. We can assume that dual wield will not be full if there is no wield
    * slot already filled. -Orion
    */
    if ( wielding[0] && get_eq_char( ch, WEAR_DUAL_WIELD ) )
    {
        send_to_char( "¡Ya estás usando dos armas!\n\r", ch );
        return FALSE;
    }

    /*
    * If wielding or missile wielding and holding a shield, then we can return
    * FALSE. -Orion
    */
    if ( alreadyWielding && get_eq_char( ch, WEAR_SHIELD ) )
    {
        send_to_char( "¡Llevas un escudo y no tienes manos libres!\n\r", ch );
        return FALSE;
    }

    /*
    * If wielding or missile wielding and holding something, then we can return
    * FALSE. -Orion
    */
    if ( alreadyWielding && get_eq_char( ch, WEAR_HOLD ) )
    {
        send_to_char( "¡estás sosteniendo algo y no tienes manos libres!\n\r", ch );
        return FALSE;
    }

    return TRUE;
}

/*
 * Check to see if there is room to wear another object on this location
 * (Layered clothing support)
 */
bool can_layer( CHAR_DATA *ch, OBJ_DATA *obj, sh_int wear_loc )
{
    OBJ_DATA   *otmp;
    sh_int	bitlayers = 0;
    sh_int	objlayers = obj->pIndexData->layers;

    for ( otmp = ch->first_carrying; otmp; otmp = otmp->next_content )
	if ( otmp->wear_loc == wear_loc ){
	    if ( !otmp->pIndexData->layers )
		return FALSE;
	    else
		bitlayers |= otmp->pIndexData->layers;
        }
    if ( (bitlayers && !objlayers) || bitlayers > objlayers )
	return FALSE;
    if ( !bitlayers || ((bitlayers & ~objlayers) == bitlayers) )
	return TRUE;
    return FALSE;
}

/*
 * Wear one object.
 * Optional replacement of existing objects.
 * Big repetitive code, ick.
 * Restructured a bit to allow for specifying body location	-Thoric
 */
void wear_obj( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace, sh_int wear_bit )
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *tmpobj;
    sh_int bit, tmp;
    bool check_size;

    separate_obj( obj );
    
    if ( wear_bit > -1 )
    {
	bit = wear_bit;
	if ( !CAN_WEAR(obj, 1 << bit) )
	{
	    if ( fReplace )
	    {
		switch( 1 << bit )
		{
		    case ITEM_HOLD:
			send_to_char( "No puedes sostener eso.\n\r", ch );
			break;
		    case ITEM_WIELD:
			send_to_char( "No puedes empuñar eso.\n\r", ch );
			break;
		    default:
			sprintf( buf, "No puedes ponerte eso en tu %s.\n\r",
				w_flags[bit] );
			send_to_char( buf, ch );
		}
	    }
	    return;
	}
    }
    else
    {
	for ( bit = -1, tmp = 1; tmp < 31; tmp++ )
	{
	    if ( CAN_WEAR(obj, 1 << tmp) )
	    {
		bit = tmp;
		break;
	    }
	}
    }

    
    check_size = FALSE;
    
    if (  1 << bit == ITEM_WIELD ||   1 << bit == ITEM_HOLD 
    || obj->item_type == ITEM_LIGHT ||   1 << bit == ITEM_WEAR_SHIELD )
       check_size = FALSE;
    else if ( ch->race == RACE_DEFEL )
       check_size = TRUE; 
    else if ( !IS_NPC(ch) )
      switch (ch->race)
      {
          default:
          case RACE_TRANDOSHAN:
          case RACE_VERPINE:
          case RACE_HUMAN:
          case RACE_ADARIAN:
          case RACE_RODIAN:
          case RACE_TWI_LEK:
             
             if ( !IS_OBJ_STAT(obj, ITEM_HUMAN_SIZE) )
                 check_size = TRUE;
             break;
             
          case RACE_HUTT:
             
              if ( !IS_OBJ_STAT(obj, ITEM_HUTT_SIZE) )
                 check_size = TRUE;
             break;
          
          case RACE_GAMORREAN:
          case RACE_MON_CALAMARI:    
          case RACE_QUARREN:
          case RACE_WOOKIEE:
             
              if ( !IS_OBJ_STAT(obj, ITEM_LARGE_SIZE) )
                 check_size = TRUE;
             break;
          
          case RACE_EWOK:
          case RACE_NOGHRI:       
          case RACE_JAWA:
             
              if ( !IS_OBJ_STAT(obj, ITEM_SMALL_SIZE) )
                 check_size = TRUE;
             break;
             
    }
    
    /* 
       this seems redundant but it enables both multiple sized objects to be 
       used as well as objects with no size flags at all 
    */               
   
   if ( check_size )
   { 
    if ( ch->race == RACE_DEFEL )
    {
	act( AT_MAGIC, "Usar cualquier cosa que pueda hacerte visible va en contra de tu naturaleza.", ch, NULL, NULL, TO_CHAR );
	//act( AT_ACTION, "$n pretende usar $p, pero no puede.", ch, obj, NULL, TO_ROOM );
	return;
    }
     
    if ( IS_OBJ_STAT(obj, ITEM_HUTT_SIZE) )
    {
	act( AT_MAGIC, "Es demasiado grande para ti.", ch, NULL, NULL, TO_CHAR );
	act( AT_ACTION, "$n intenta usar $p, pero es demasiado grande.",
	    ch, obj, NULL, TO_ROOM );
	return;
    }
    
    if ( IS_OBJ_STAT(obj, ITEM_LARGE_SIZE) || IS_OBJ_STAT(obj, ITEM_HUMAN_SIZE) )
    {
	act( AT_MAGIC, "Este objeto es del tamaño equivocado para ti.", ch, NULL, NULL, TO_CHAR );
	act( AT_ACTION, "$n intenta usar $p, pero no puede.",
	    ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( IS_OBJ_STAT(obj, ITEM_SMALL_SIZE) )
    {
	act( AT_MAGIC, "Ese objeto es demasiado pequeño para ti.", ch, NULL, NULL, TO_CHAR );
	act( AT_ACTION, "$n intenta usar $p, pero es demasiado pequeño.",
	    ch, obj, NULL, TO_ROOM );
	return;
    }
   }
   
    /* currently cannot have a light in non-light position */
    if ( obj->item_type == ITEM_LIGHT )
    {
	if ( !remove_obj( ch, WEAR_LIGHT, fReplace ) )
	    return;  
	if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
        {
          if ( !obj->action_desc || obj->action_desc[0]=='\0' )
  	  {
  	    act( AT_ACTION, "$n sostiene $p como luz.", ch, obj, NULL, TO_ROOM );
	    act( AT_ACTION, "sostienes $p como luz.",  ch, obj, NULL, TO_CHAR );
           }
	   else
	    actiondesc( ch, obj, NULL );
        }
        equip_char( ch, obj, WEAR_LIGHT );
        oprog_wear_trigger( ch, obj );
	return;
    }

    if ( bit == -1 )
    {
	if ( fReplace )
	  send_to_char( "No puedes vestir, empuñar o sostener eso.\n\r", ch );
	return;
    }

    switch ( 1 << bit )
    {
	default:
	    bug( "wear_obj: uknown/unused item_wear bit %d", bit );
	    if ( fReplace )
	      send_to_char( "No puedes vestir, empuñar o sostener eso.\n\r", ch );
	    return;

	case ITEM_WEAR_FINGER:
	    if ( get_eq_char( ch, WEAR_FINGER_L )
	    &&   get_eq_char( ch, WEAR_FINGER_R )
	    &&   !remove_obj( ch, WEAR_FINGER_L, fReplace )
	    &&   !remove_obj( ch, WEAR_FINGER_R, fReplace ) )
	      return;

	    if ( !get_eq_char( ch, WEAR_FINGER_L ) )
	    {
                if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
                {
                 if ( !obj->action_desc || obj->action_desc[0]=='\0' )  
                 {  
                  act( AT_ACTION, "$n desliza $p en su dedo izquierdo.",    ch, obj, NULL, TO_ROOM );
		  act( AT_ACTION, "Deslizas $p en tu dedo izquierdo.",  ch, obj, NULL, TO_CHAR );
                 }
        	 else
	          actiondesc( ch, obj, NULL );
                }
		equip_char( ch, obj, WEAR_FINGER_L );
		oprog_wear_trigger( ch, obj );
		return;
	    }

	    if ( !get_eq_char( ch, WEAR_FINGER_R ) )
	    {
                if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
                {
                 if ( !obj->action_desc || obj->action_desc[0]=='\0' )  
                 {  
      		  act( AT_ACTION, "$n desliza $p en su dedo derecho.",    ch, obj, NULL, TO_ROOM );
		  act( AT_ACTION, "Deslizas $p en tu dedo derecho.",  ch, obj, NULL, TO_CHAR );
                 }
        	 else
	          actiondesc( ch, obj, NULL );
                }
		equip_char( ch, obj, WEAR_FINGER_R );
		oprog_wear_trigger( ch, obj );
		return;
	    }

	    bug( "Wear_obj: no free finger.", 0 );
	    send_to_char( "ya estás llevando algo en los dedos.\n\r", ch );
	    return;

	case ITEM_WEAR_NECK:
	    if ( get_eq_char( ch, WEAR_NECK_1 ) != NULL
	    &&   get_eq_char( ch, WEAR_NECK_2 ) != NULL
	    &&   !remove_obj( ch, WEAR_NECK_1, fReplace )
	    &&   !remove_obj( ch, WEAR_NECK_2, fReplace ) )
	      return;

	    if ( !get_eq_char( ch, WEAR_NECK_1 ) )
	    {
                if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
                {
                 if ( !obj->action_desc || obj->action_desc[0]=='\0' )  
                 {  
      		  act( AT_ACTION, "$n se pone $p alrededor del cuello.",   ch, obj, NULL, TO_ROOM );
		  act( AT_ACTION, "Te pones $p alrededor del cuello.", ch, obj, NULL, TO_CHAR );
                 }
        	 else
	          actiondesc( ch, obj, NULL );
                }
		equip_char( ch, obj, WEAR_NECK_1 );
		oprog_wear_trigger( ch, obj );
		return;
	    }

	    if ( !get_eq_char( ch, WEAR_NECK_2 ) )
	    {
                if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
                {
                 if ( !obj->action_desc || obj->action_desc[0]=='\0' )  
                 {  
      		  act( AT_ACTION, "$n se pone $p alrededor del cuello.",   ch, obj, NULL, TO_ROOM );
		  act( AT_ACTION, "Te pones $p alrededor del cuello.", ch, obj, NULL, TO_CHAR );
                 }
        	 else
	          actiondesc( ch, obj, NULL );
                }
		equip_char( ch, obj, WEAR_NECK_2 );
		oprog_wear_trigger( ch, obj );
		return;
	    }

	    bug( "Wear_obj: no free neck.", 0 );
	    send_to_char( "Ya llevas dos cosas en el cuello.\n\r", ch );
	    return;

	case ITEM_WEAR_BODY:
	/*
	    if ( !remove_obj( ch, WEAR_BODY, fReplace ) )
	      return;
	*/
	    if ( !can_layer( ch, obj, WEAR_BODY ) )
	    {
		send_to_char( "No se ajustará encima de lo que ya estás usando.\n\r", ch );
		return;
	    }
            if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
             if ( !obj->action_desc || obj->action_desc[0]=='\0' )  
             {  
      	      act( AT_ACTION, "$n se pone $p en el cuerpo.",   ch, obj, NULL, TO_ROOM );
	      act( AT_ACTION, "Te pones $p en el cuerpo.", ch, obj, NULL, TO_CHAR );
             }
             else
	      actiondesc( ch, obj, NULL );
            }
	    equip_char( ch, obj, WEAR_BODY );
	    oprog_wear_trigger( ch, obj );
	    return;

	case ITEM_WEAR_HEAD:
	    if ( ch->race == RACE_VERPINE || ch->race == RACE_TWI_LEK )
	    {
		send_to_char( "No puedes llevar nada en la cabeza.\n\r", ch );
		return;
	    }
	    if ( !remove_obj( ch, WEAR_HEAD, fReplace ) )
	      return;
            if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
             if ( !obj->action_desc || obj->action_desc[0]=='\0' )  
             {  
      	      act( AT_ACTION, "$n se pone $p sobre su cabeza.",   ch, obj, NULL, TO_ROOM );
	      act( AT_ACTION, "Te pones $p sobre la cabeza.", ch, obj, NULL, TO_CHAR );
             }
             else
	      actiondesc( ch, obj, NULL );
            }
	    equip_char( ch, obj, WEAR_HEAD );
	    oprog_wear_trigger( ch, obj );
	    return;

	case ITEM_WEAR_EYES:
	    if ( !remove_obj( ch, WEAR_EYES, fReplace ) )
	      return;
            if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
             if ( !obj->action_desc || obj->action_desc[0]=='\0' )  
             {  
      	      act( AT_ACTION, "$n se coloca $p en los ojos.",   ch, obj, NULL, TO_ROOM );
	      act( AT_ACTION, "Te colocas $p en los ojos.", ch, obj, NULL, TO_CHAR );
             }
             else
	      actiondesc( ch, obj, NULL );
           }
	    equip_char( ch, obj, WEAR_EYES );
	    oprog_wear_trigger( ch, obj );
	    return;
        case ITEM_WEAR_BACK:
	    if ( !can_layer( ch, obj, WEAR_BACK ) )
	    {
		send_to_char( "No se ajustará encima de lo que llevas puesto.\n\r", ch );
		return;
	    }
            if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
             if ( !obj->action_desc || obj->action_desc[0]=='\0' )  
             {  
      	      act( AT_ACTION, "$n se pone $p en la espalda.",   ch, obj, NULL, TO_ROOM );
	      act( AT_ACTION, "Te pones $p en la espalda.", ch, obj, NULL, TO_CHAR );
             }
             else
	      actiondesc( ch, obj, NULL );
            }
	    equip_char( ch, obj, WEAR_BACK );
	    oprog_wear_trigger( ch, obj );
	    return;        
	case ITEM_WEAR_EARS:
	    if ( ch->race == RACE_VERPINE )
	    {
		send_to_char( "¿Qué orejas?.\n\r", ch );
		return;
	    }
	    if ( !remove_obj( ch, WEAR_EARS, fReplace ) )
	      return;
            if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
             if ( !obj->action_desc || obj->action_desc[0]=='\0' )  
             {  
      	      act( AT_ACTION, "$n se pone $p en las orejas.",   ch, obj, NULL, TO_ROOM );
	      act( AT_ACTION, "Te pones $p en las orejas.", ch, obj, NULL, TO_CHAR );
             }
             else
             actiondesc( ch, obj, NULL );
            }
	    equip_char( ch, obj, WEAR_EARS );
	    oprog_wear_trigger( ch, obj );
	    return;

	case ITEM_WEAR_LEGS:
/*
	    if ( !remove_obj( ch, WEAR_LEGS, fReplace ) )
	      return;
*/
	    if ( ch->race == RACE_HUTT )
	    {
		send_to_char( "¡Los hutts no tienen piernas.\n\r", ch );
		return;
	    }
	    if ( !can_layer( ch, obj, WEAR_LEGS ) )
	    {
		send_to_char( "No se ajustará encima de lo que llevas puesto.\n\r", ch );
		return;
	    }
            if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
             if ( !obj->action_desc || obj->action_desc[0]=='\0' )  
             {  
      	      act( AT_ACTION, "$n se desliza en $p.",   ch, obj, NULL, TO_ROOM );
	      act( AT_ACTION, "Te deslizas en $p.", ch, obj, NULL, TO_CHAR );
             }
             else
             actiondesc( ch, obj, NULL );
            }
	    equip_char( ch, obj, WEAR_LEGS );
	    oprog_wear_trigger( ch, obj );
	    return;

	case ITEM_WEAR_FEET:
/*
	    if ( !remove_obj( ch, WEAR_FEET, fReplace ) )
	      return;
*/
	    if ( ch->race == RACE_HUTT )
	    {
		send_to_char( "¡Los hutts no tienen pies!\n\r", ch );
		return;
	    }
	    if ( !can_layer( ch, obj, WEAR_FEET ) )
	    {
		send_to_char( "No se ajustará encima de lo que llevas puesto.\n\r", ch );
		return;
	    }
            if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
             if ( !obj->action_desc || obj->action_desc[0]=='\0' )  
             {  
      	      act( AT_ACTION, "$n se pone $p en los pies.",   ch, obj, NULL, TO_ROOM );
	      act( AT_ACTION, "Te pones $p en los pies.", ch, obj, NULL, TO_CHAR );
             }
             else
             actiondesc( ch, obj, NULL );
            }
	    equip_char( ch, obj, WEAR_FEET );
	    oprog_wear_trigger( ch, obj );
	    return;

	case ITEM_WEAR_HOLSTER1:
/*
	    if ( !remove_obj( ch, WEAR_FEET, fReplace ) )
	      return;
*/
	    if ( !can_layer( ch, obj, WEAR_HOLSTER_L ) )
	    {
		send_to_char( "No se ajustará encima de lo que ya llevas puesto.\n\r", ch );
		return;
	    }
            if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
             if ( !obj->action_desc || obj->action_desc[0]=='\0' )  
             {  
      	      act( AT_ACTION, "$n ajusta $p sobre su cadera izquierda.",   ch, obj, NULL, TO_ROOM );
	      act( AT_ACTION, "Te pones $p en la cadera izquierda.", ch, obj, NULL, TO_CHAR );
             }
             else
             actiondesc( ch, obj, NULL );
            }
	    equip_char( ch, obj, WEAR_HOLSTER_L );
	    oprog_wear_trigger( ch, obj );
	    return;

	case ITEM_WEAR_HOLSTER2:
/*
	    if ( !remove_obj( ch, WEAR_FEET, fReplace ) )
	      return;
*/
	    if ( !can_layer( ch, obj, WEAR_HOLSTER_R ) )
	    {
		send_to_char( "No se ajustará encima de lo que llevas puesto.\n\r", ch );
		return;
	    }
            if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
             if ( !obj->action_desc || obj->action_desc[0]=='\0' )  
             {  
      	      act( AT_ACTION, "$n ajusta $p en su cadera derecha.",   ch, obj, NULL, TO_ROOM );
	      act( AT_ACTION, "Te pones $p en la cadera derecha.", ch, obj, NULL, TO_CHAR );
             }
             else
             actiondesc( ch, obj, NULL );
            }
	    equip_char( ch, obj, WEAR_HOLSTER_R );
	    oprog_wear_trigger( ch, obj );
	    return;

	case ITEM_WEAR_HANDS:
/*
	    if ( !remove_obj( ch, WEAR_HANDS, fReplace ) )
	      return;
*/
	    if ( !can_layer( ch, obj, WEAR_HANDS ) )
	    {
		send_to_char( "No se ajustará encima de lo que llevas puesto.\n\r", ch );
		return;
	    }
            if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
             if ( !obj->action_desc || obj->action_desc[0]=='\0' )  
             {  
      	      act( AT_ACTION, "$n se pone $p en las manos.",   ch, obj, NULL, TO_ROOM );
	      act( AT_ACTION, "Te pones $p en las manos.", ch, obj, NULL, TO_CHAR );
             }
             else
             actiondesc( ch, obj, NULL );
            }
	    equip_char( ch, obj, WEAR_HANDS );
	    oprog_wear_trigger( ch, obj );
	    return;

	case ITEM_WEAR_ARMS:
/*
	    if ( !remove_obj( ch, WEAR_ARMS, fReplace ) )
	      return;
*/
	    if ( !can_layer( ch, obj, WEAR_ARMS ) )
	    {
		send_to_char( "No se ajustará encima de lo que llevas puesto.\n\r", ch );
		return;
	    }
	    if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
             if ( !obj->action_desc || obj->action_desc[0]=='\0' )  
             {  
      	      act( AT_ACTION, "$n se pone $p en los brazos.",   ch, obj, NULL, TO_ROOM );
	      act( AT_ACTION, "Te pones $p en los brazos.", ch, obj, NULL, TO_CHAR );
             }
             else
             actiondesc( ch, obj, NULL );
            }
	    equip_char( ch, obj, WEAR_ARMS );
	    oprog_wear_trigger( ch, obj );
	    return;

	case ITEM_WEAR_ABOUT:
	/*
	    if ( !remove_obj( ch, WEAR_ABOUT, fReplace ) )
	      return;
	*/
	    if ( !can_layer( ch, obj, WEAR_ABOUT ) )
	    {
		send_to_char( "No se ajustará encima de lo que llevas puesto.\n\r", ch );
		return;
	    }
            if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
             if ( !obj->action_desc || obj->action_desc[0]=='\0' )  
             {  
      	      act( AT_ACTION, "$n se pone $p sobre el cuerpo.",   ch, obj, NULL, TO_ROOM );
	      act( AT_ACTION, "Te pones $p sobre el cuerpo.", ch, obj, NULL, TO_CHAR );
             }
             else
             actiondesc( ch, obj, NULL );
            }
	    equip_char( ch, obj, WEAR_ABOUT );
	    oprog_wear_trigger( ch, obj );
	    return;

	case ITEM_WEAR_WAIST:
/*
	    if ( !remove_obj( ch, WEAR_WAIST, fReplace ) )
	      return;
*/
	    if ( !can_layer( ch, obj, WEAR_WAIST ) )
	    {
		send_to_char( "No se ajustará encima de lo que llevas puesto.\n\r", ch );
		return;
	    }
            if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
             if ( !obj->action_desc || obj->action_desc[0]=='\0' )  
             {  
      	      act( AT_ACTION, "$n se pone $p alrededor de la cintura.",   ch, obj, NULL, TO_ROOM );
	      act( AT_ACTION, "Te pones $p alrededor de la cintura.", ch, obj, NULL, TO_CHAR );
             }
             else
             actiondesc( ch, obj, NULL );
            }
	    equip_char( ch, obj, WEAR_WAIST );
	    oprog_wear_trigger( ch, obj );
	    return;

	case ITEM_WEAR_WRIST:
	    if ( get_eq_char( ch, WEAR_WRIST_L )
	    &&   get_eq_char( ch, WEAR_WRIST_R )
	    &&   !remove_obj( ch, WEAR_WRIST_L, fReplace )
	    &&   !remove_obj( ch, WEAR_WRIST_R, fReplace ) )
	       return;

	    if ( !get_eq_char( ch, WEAR_WRIST_L ) )
	    {
		if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
		{
                 if ( !obj->action_desc || obj->action_desc[0]=='\0' )  
                 {  
      		   act( AT_ACTION, "$n se pone $p alrededor de la muñeca izquierda.",
			ch, obj, NULL, TO_ROOM );
		   act( AT_ACTION, "Te pones $p alrededor de la muñeca izquierda.",
			ch, obj, NULL, TO_CHAR );
                 }
                 else
                  actiondesc( ch, obj, NULL );
		}
		equip_char( ch, obj, WEAR_WRIST_L );
		oprog_wear_trigger( ch, obj );
		return;
	    }

	    if ( !get_eq_char( ch, WEAR_WRIST_R ) )
	    {
              if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
              {
               if ( !obj->action_desc || obj->action_desc[0]=='\0' )  
               {  
      		act( AT_ACTION, "$n se pone $p alrededor de la muñeca derecha.",
			ch, obj, NULL, TO_ROOM );
		act( AT_ACTION, "Te pones $p alrededor de la muñeca derecha.",
			ch, obj, NULL, TO_CHAR );
               }
               else
                actiondesc( ch, obj, NULL );
              }
		equip_char( ch, obj, WEAR_WRIST_R );
		oprog_wear_trigger( ch, obj );
		return;
	    }

	    bug( "Wear_obj: no free wrist.", 0 );
	    send_to_char( "Ya tienes las dos muñecas ocupadas.\n\r", ch );
	    return;

	case ITEM_WEAR_SHIELD:
	    if ( !remove_obj( ch, WEAR_SHIELD, fReplace ) )
	      return;
            if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
             if ( !obj->action_desc || obj->action_desc[0]=='\0' )  
             {  
      	      act( AT_ACTION, "$n usa $p como un escudo de energía.", ch, obj, NULL, TO_ROOM );
	      act( AT_ACTION, "usas $p como un escudo de energía.", ch, obj, NULL, TO_CHAR );
             }
             else
              actiondesc( ch, obj, NULL );
            }
	    equip_char( ch, obj, WEAR_SHIELD );
	    oprog_wear_trigger( ch, obj );
	    return;

	case ITEM_WIELD:
	    if ( (tmpobj  = get_eq_char( ch, WEAR_WIELD )) != NULL
	    &&   !could_dual(ch) )
	    {
		send_to_char( "ya empuñas algo.\n\r", ch );
		return;
	    }
	    
	    if (( tmpobj = get_eq_char(ch, WEAR_WIELD)) != NULL && could_dual(ch) &&  tmpobj->value[3] == WEAPON_DUAL_LIGHTSABER){
	    	send_to_char("Estás blandiendo un sable de luz de doble oja. Esto requiere ambas manos.\n\r", ch);
	    	return;
	    }

	    if (( tmpobj = get_eq_char(ch, WEAR_WIELD)) != NULL && could_dual(ch) &&  tmpobj->value[3] == WEAPON_FORCE_PIKE)
	    {
	    	send_to_char("Las picas de fuerza requieren las dos manos.\n\r", ch);
	    	return;
	    }

            if ( obj->value[3] == WEAPON_BOWCASTER && get_curr_str(ch) <=22)
            {
                send_to_char( "Eres demasiado débil para usar ese arma.\n\r", ch);
                return;
            }

	    if ( tmpobj )
	    {
		if ( can_dual(ch) )
		{
	            
	            if(obj->value[3] == WEAPON_DUAL_LIGHTSABER)
		    {
	            	send_to_char("Estás empuñando un arma, y necesitas las dos manos libres para blandir un sable láser de doble oja.\n\r", ch);
	            	return;
	            }

	            if(obj->value[3] == WEAPON_FORCE_PIKE)
		    {
	            	send_to_char("Estás empuñando un arma y necesitas las dos manos libres para usar una pica de fuerza.\n\r", ch);
	            	return;
	            }
	            
		    if ( get_obj_weight( obj ) + get_obj_weight( tmpobj ) > str_app[get_curr_str(ch)].wield )
		    {
			send_to_char( "Pesa demasiado para ti.\n\r", ch );
			return;
	      	    }
                    if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
                    {
                     if ( !obj->action_desc || obj->action_desc[0]=='\0' )  
                     {  
      	  	      act( AT_ACTION, "$n empuña con su mano libre $p.", ch, obj, NULL, TO_ROOM );
		      act( AT_ACTION, "Empuñas con tu mano libre $p.", ch, obj, NULL, TO_CHAR );
                     }
                     else
                      actiondesc( ch, obj, NULL );
                    }
		    equip_char( ch, obj, WEAR_DUAL_WIELD );
		    oprog_wear_trigger( ch, obj );
		}
	        return;
	    }

	    if ( get_obj_weight( obj ) > str_app[get_curr_str(ch)].wield )
	    {
		send_to_char( "Pesa demasiado para ti.\n\r", ch );
		return;
	    }

            if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
             if ( !obj->action_desc || obj->action_desc[0]=='\0' )  
             {  
      	      act( AT_ACTION, "$n empuña $p.", ch, obj, NULL, TO_ROOM );
	      act( AT_ACTION, "Empuñas $p.", ch, obj, NULL, TO_CHAR );
             }
             else
              actiondesc( ch, obj, NULL );
            }
	    equip_char( ch, obj, WEAR_WIELD );
	    oprog_wear_trigger( ch, obj );
	    return;

	case ITEM_HOLD:
	    if ( get_eq_char( ch, WEAR_DUAL_WIELD ) )
	    {
		send_to_char( "¡No puedes sostener algo y dos armas!\n\r", ch );
		return;
	    }
	    if ( !remove_obj( ch, WEAR_HOLD, fReplace ) )
	      return;
            if ( obj->item_type == ITEM_DEVICE
               || obj->item_type == ITEM_GRENADE
               || obj->item_type == ITEM_FOOD 
               || obj->item_type == ITEM_PILL
               || obj->item_type == ITEM_POTION
               || obj->item_type == ITEM_DRINK_CON 
               || obj->item_type == ITEM_SALVE
               || obj->item_type == ITEM_KEY
               || !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
	      act( AT_ACTION, "$n sostiene $p en su mano.",   ch, obj, NULL, TO_ROOM );
	      act( AT_ACTION, "sostienes $p en tu mano.", ch, obj, NULL, TO_CHAR );
            }
	    equip_char( ch, obj, WEAR_HOLD );
	    oprog_wear_trigger( ch, obj );
	    return;

	case ITEM_WEAR_BOTHWRISTS:
	    if ( !remove_obj( ch, WEAR_BOTH_WRISTS, fReplace ) )
	      return;
            if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
            {
             if ( !obj->action_desc || obj->action_desc[0]=='\0' )  
             {  
      	      act( AT_ACTION, "$n se pone $p en ambas muñecas.",   ch, obj, NULL, TO_ROOM );
	      act( AT_ACTION, "Te pones $p en ambas muñecas.", ch, obj, NULL, TO_CHAR );
             }
             else
             actiondesc( ch, obj, NULL );
            }
	    equip_char( ch, obj, WEAR_BOTH_WRISTS );
	    oprog_wear_trigger( ch, obj );
	    return;


    }
}


void do_wear( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    sh_int wear_bit;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( (!str_cmp(arg2, "en")  || !str_cmp(arg2, "encima") || !str_cmp(arg2, "sobre"))
    &&   argument[0] != '\0' )
	argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "¿blandir, sostener o vestir qué?\n\r", ch );
	return;
    }

    if ( ms_find_obj(ch) )
	return;

    if ( !str_cmp( arg1, "todo" ) )
    {
	OBJ_DATA *obj_next;

	for ( obj = ch->first_carrying; obj; obj = obj_next )
	{
	    obj_next = obj->next_content;
            if ( obj->item_type == ITEM_BINDERS)
            {
	        send_to_char( "¿Temes algo?\n\r", ch);    
                return;
            }
	    if ( obj->wear_loc == WEAR_NONE && can_see_obj( ch, obj ) )
		wear_obj( ch, obj, FALSE, -1 );
	}
	return;
    }
    else
    {
	if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
	{
	    send_to_char( "No tienes ese objeto.\n\r", ch );
	    return;
	}
        if ( obj->item_type == ITEM_BINDERS)
        {
	    send_to_char( "¿Temes algo?\n\r", ch);    
    	    return;
        }
	if ( arg2[0] != '\0' )
	  wear_bit = get_wflag(arg2);
	else
	  wear_bit = -1;
	wear_obj( ch, obj, TRUE, wear_bit );
    }

    return;
}



void do_remove( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj, *obj_next;


    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "¿Dejar de usar qué?\n\r", ch );
	return;
    }

    if ( ms_find_obj(ch) )
	return;

   if ( !str_cmp( arg, "todo" ) )  /* SB Remove all */
    {
      for ( obj = ch->first_carrying; obj != NULL ; obj = obj_next )
      {
        obj_next = obj->next_content;
        if ( obj->wear_loc != WEAR_NONE && can_see_obj ( ch, obj ) && obj->item_type != ITEM_BINDERS)
          remove_obj ( ch, obj->wear_loc, TRUE );
      }
      return;
    }

    if ( ( obj = get_obj_wear( ch, arg ) ) == NULL )
    {
	send_to_char( "No estás usando ese objeto.\n\r", ch );
	return;
    }
    if ( (obj_next=get_eq_char(ch, obj->wear_loc)) != obj )
    {
	act( AT_PLAIN, "Debes dejar de usar $p primero.", ch, obj_next, NULL, TO_CHAR );
	return;
    }
    if ( obj->item_type == ITEM_BINDERS)
    {
	send_to_char( "No puedes quitarte las esposas.\n\r", ch);
	return;
    }

    remove_obj( ch, obj->wear_loc, TRUE );
    return;
}


void do_bury( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    bool shovel;
    sh_int move;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {    
        send_to_char( "¿Qué deseas enterrar?\n\r", ch );
        return;
    }
    
    if ( ms_find_obj(ch) )
        return;
 
    shovel = FALSE;
    for ( obj = ch->first_carrying; obj; obj = obj->next_content )
      if ( obj->item_type == ITEM_SHOVEL )
      {
	  shovel = TRUE;
	  break;
      }

    obj = get_obj_list_rev( ch, arg, ch->in_room->last_content );
    if ( !obj )
    {
        send_to_char( "No encuentras eso.\n\r", ch );
        return;
    }

    separate_obj(obj);
    if ( !CAN_WEAR(obj, ITEM_TAKE) )
    {
		act( AT_PLAIN, "No puedes enterrar $p.", ch, obj, 0, TO_CHAR );
        	return;
    }
  
    switch( ch->in_room->sector_type )
    {
	case SECT_CITY:
	case SECT_INSIDE:
	    send_to_char( "El suelo es demasiado duro para cavar en él.\n\r", ch );
	    return;
	case SECT_WATER_SWIM:
	case SECT_WATER_NOSWIM:
	case SECT_UNDERWATER:
	    send_to_char( "No puedes enterrar nada aquí.\n\r", ch );
	    return;
	case SECT_AIR:
	    send_to_char( "¿En el aire?!\n\r", ch );
	    return;
    }

    if ( obj->weight > (UMAX(5, (can_carry_w(ch) / 10)))
    &&  !shovel )
    {
	send_to_char( "Necesitas una pala para eso.\n\r", ch );
	return;
    }
    
    move = (obj->weight * 50 * (shovel ? 1 : 5)) / UMAX(1, can_carry_w(ch));
    move = URANGE( 2, move, 1000 );
    if ( move > ch->move )
    {
	send_to_char( "El cansancio te impide cavar tanto.\n\r", ch );
	return;
    }
    ch->move -= move;
    
    act( AT_ACTION, "Entierras $p...", ch, obj, NULL, TO_CHAR );
    act( AT_ACTION, "$n entierra $p...", ch, obj, NULL, TO_ROOM );
    SET_BIT( obj->extra_flags, ITEM_BURRIED );
    WAIT_STATE( ch, URANGE( 10, move / 2, 100 ) );
    return;
}

void do_sacrifice( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' || !str_cmp( arg, ch->name ) )
    {
	act( AT_ACTION, "$n offers $mself to $s deity, who graciously declines.",
	    ch, NULL, NULL, TO_ROOM );
	send_to_char( "Your deity appreciates your offer and may accept it later.", ch );
	return;
    }

    if ( ms_find_obj(ch) )
	return;

    obj = get_obj_list_rev( ch, arg, ch->in_room->last_content );
    if ( !obj )
    {
	send_to_char( "You can't find it.\n\r", ch );
	return;
    }

    separate_obj(obj);
    if ( !CAN_WEAR(obj, ITEM_TAKE) )
    {
	act( AT_PLAIN, "$p is not an acceptable sacrifice.", ch, obj, 0, TO_CHAR );
	return;
    }

    oprog_sac_trigger( ch, obj );
    if ( obj_extracted(obj) )
      return;
    if ( cur_obj == obj->serial )
      global_objcode = rOBJ_SACCED;
    separate_obj( obj );
    extract_obj( obj );
    return;
}

void do_brandish( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    OBJ_DATA *staff;
    ch_ret retcode;
    int sn;

    if ( ( staff = get_eq_char( ch, WEAR_HOLD ) ) == NULL )
    {
	send_to_char( "You hold nothing in your hand.\n\r", ch );
	return;
    }

    if ( staff->item_type != ITEM_STAFF )
    {
	send_to_char( "You can brandish only with a staff.\n\r", ch );
	return;
    }

    if ( ( sn = staff->value[3] ) < 0
    ||   sn >= top_sn
    ||   skill_table[sn]->spell_fun == NULL )
    {
	bug( "Do_brandish: bad sn %d.", sn );
	return;
    }

    WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

    if ( staff->value[2] > 0 )
    {
      if ( !oprog_use_trigger( ch, staff, NULL, NULL, NULL ) )
      {
        act( AT_MAGIC, "$n brandishes $p.", ch, staff, NULL, TO_ROOM );
        act( AT_MAGIC, "You brandish $p.",  ch, staff, NULL, TO_CHAR );
      }
	for ( vch = ch->in_room->first_person; vch; vch = vch_next )
	{
	    vch_next	= vch->next_in_room;
            if ( !IS_NPC( vch ) && IS_SET( vch->act, PLR_WIZINVIS ) 
                  && vch->pcdata->wizinvis >= LEVEL_IMMORTAL )
                continue;
            else
	    switch ( skill_table[sn]->target )
	    {
	    default:
		bug( "Do_brandish: bad target for sn %d.", sn );
		return;

	    case TAR_IGNORE:
		if ( vch != ch )
		    continue;
		break;

	    case TAR_CHAR_OFFENSIVE:
		if ( IS_NPC(ch) ? IS_NPC(vch) : !IS_NPC(vch) )
		    continue;
		break;
		
	    case TAR_CHAR_DEFENSIVE:
		if ( IS_NPC(ch) ? !IS_NPC(vch) : IS_NPC(vch) )
		    continue;
		break;

	    case TAR_CHAR_SELF:
		if ( vch != ch )
		    continue;
		break;
	    }

	    retcode = obj_cast_spell( staff->value[3], staff->value[0], ch, vch, NULL );
	    if ( retcode == rCHAR_DIED || retcode == rBOTH_DIED )
	    {
		bug( "do_brandish: char died", 0 );
		return;
	    }  
	}
    }

    if ( --staff->value[2] <= 0 )
    {
	act( AT_MAGIC, "$p blazes bright and vanishes from $n's hands!", ch, staff, NULL, TO_ROOM );
	act( AT_MAGIC, "$p blazes bright and is gone!", ch, staff, NULL, TO_CHAR );
	if ( staff->serial == cur_obj )
	  global_objcode = rOBJ_USED;
	extract_obj( staff );
    }

    return;
}



void do_zap( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *wand;
    OBJ_DATA *obj;
    ch_ret retcode;

    one_argument( argument, arg );
    if ( arg[0] == '\0' && !ch->fighting )
    {
	send_to_char( "Zap whom or what?\n\r", ch );
	return;
    }

    if ( ( wand = get_eq_char( ch, WEAR_HOLD ) ) == NULL )
    {
	send_to_char( "You hold nothing in your hand.\n\r", ch );
	return;
    }

    if ( wand->item_type != ITEM_WAND )
    {
	send_to_char( "You can zap only with a wand.\n\r", ch );
	return;
    }

    obj = NULL;
    if ( arg[0] == '\0' )
    {
	if ( ch->fighting )
	{
	    victim = who_fighting( ch );
	}
	else
	{
	    send_to_char( "Zap whom or what?\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( ( victim = get_char_room ( ch, arg ) ) == NULL
	&&   ( obj    = get_obj_here  ( ch, arg ) ) == NULL )
	{
	    send_to_char( "You can't find it.\n\r", ch );
	    return;
	}
    }

    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );

    if ( wand->value[2] > 0 )
    {
	if ( victim )
	{
          if ( !oprog_use_trigger( ch, wand, victim, NULL, NULL ) )
          {
	    act( AT_MAGIC, "$n aims $p at $N.", ch, wand, victim, TO_ROOM );
	    act( AT_MAGIC, "You aim $p at $N.", ch, wand, victim, TO_CHAR );
          }
	}
	else
	{
          if ( !oprog_use_trigger( ch, wand, NULL, obj, NULL ) )
          {
	    act( AT_MAGIC, "$n aims $p at $P.", ch, wand, obj, TO_ROOM );
	    act( AT_MAGIC, "You aim $p at $P.", ch, wand, obj, TO_CHAR );
          }
	}

	retcode = obj_cast_spell( wand->value[3], wand->value[0], ch, victim, obj );
	if ( retcode == rCHAR_DIED || retcode == rBOTH_DIED )
	{
	   bug( "do_zap: char died", 0 );
	   return;
	}
    }

    if ( --wand->value[2] <= 0 )
    {
      act( AT_MAGIC, "$p explodes into fragments.", ch, wand, NULL, TO_ROOM );
      act( AT_MAGIC, "$p explodes into fragments.", ch, wand, NULL, TO_CHAR );
      if ( wand->serial == cur_obj )
        global_objcode = rOBJ_USED;
      extract_obj( wand );
    }

    return;
}

/*
 * Save items in a clan storage room			-Scryn & Thoric
 */
void save_clan_storeroom( CHAR_DATA *ch, CLAN_DATA *clan )
{
    FILE *fp;
    char filename[256];
    sh_int templvl;
    OBJ_DATA *contents;

    if ( !clan )
    {
	bug( "save_clan_storeroom: Null clan pointer!", 0 );
	return;
    }

    if ( !ch )
    {
	bug ("save_clan_storeroom: Null ch pointer!", 0);
	return;
    }

    sprintf( filename, "%s%s.vault", CLAN_DIR, clan->filename );
    if ( ( fp = fopen( filename, "w" ) ) == NULL )
    {
	bug( "save_clan_storeroom: fopen", 0 );
	perror( filename );
    }
    else
    {
	templvl = ch->top_level;
	ch->top_level = LEVEL_HERO;		/* make sure EQ doesn't get lost */
        contents = ch->in_room->last_content;
        if (contents)
	  fwrite_obj(ch, contents, fp, 0, OS_CARRY, FALSE );
	fprintf( fp, "#END\n" );
	ch->top_level = templvl;
	fclose( fp );
	return;
    }
    return;
}

/* put an item on auction, or see the stats on the current item or bet */
void do_auction (CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *obj;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    AFFECT_DATA *paf;

    argument = one_argument (argument, arg1);

    if (IS_NPC(ch)) /* NPC can be extracted at any time and thus can't auction! */
	return;
        
/*    if ( ( time_info.hour > 18 || time_info.hour < 9 ) && auction->item == NULL )
    {
        set_char_color ( AT_LBLUE, ch );
        send_to_char ( "\n\rThe auctioneer has retired for the evening...\n\r", ch );
        return;
    }
*/
    if (arg1[0] == '\0')
    {
        if (auction->item != NULL)
        {
	    if(ch == auction->seller && !IS_IMMORTAL(ch))
	    {
		send_to_char("¡No puedes comprobar las características de tu propio objeto!\n\r", ch);
		return;
	    }

  	    obj = auction->item;
 
            /* show item data here */
            if (auction->bet > 0)
                sprintf (buf, "%d",auction->bet);
            else
                sprintf (buf, "zero");
	ch_printf(ch, "\n\r&W++\n\r&z||&w Objeto:&G %s  &wTipo:&G %s\n\r", obj->short_descr, item_type_spanish_name(obj));
	ch_printf(ch, "&z|| &wla puja actual está en &Y%s&w credits.\n\r", buf); 
    ch_printf(ch, "&z||&w Precio:&G %d  &wPeso:&G %d  &wSe viste en:&G %s\n\r",
															obj->cost, obj->weight,
															flag_string(obj->wear_flags-1, w_flags));
	ch_printf(ch, "&W++\n\r");

	    switch ( obj->item_type )
	    {
		
		case ITEM_RLAUNCHER:
			if(obj->value[5] == 0)
			 ch_printf(ch, "&z|| &wNo está cargado con nada.\n\r");
		    else
			 ch_printf(ch, "&z|| &wEstá cargado con un misil &R%s&w.\n\r", obj->value[1] == 1 ? "incendiario" : "explosivo");
			if(obj->value[2] == 1)
			 ch_printf(ch, "&z|| &wEstá equipado con un sistema de guía.\n\r");
			ch_printf(ch, "&W++\n\r");

			break;
		case ITEM_ARMOR:
		  ch_printf( ch, "&z|| &wArmadura actual: &G%d&w  Máxima: &G%d\n\r", obj->value[0], obj->value[1] );
		  ch_printf(ch, "&W++\n\r");
		  break;
		case ITEM_WEAPON:
		  ch_printf(ch, "&z|| &wEs &G%s&w.  Media de daño: &G%d&w\n\r",
							obj->value[3] == WEAPON_VIBRO_BLADE ? "un bibrocuchillo" :
							obj->value[3] == WEAPON_BOWCASTER ? "un bowcaster" :
							obj->value[3] == WEAPON_FORCE_PIKE ? "una pica de fuerza" :
							obj->value[3] == WEAPON_BLASTER ? "arma láser" :
							obj->value[3] == WEAPON_LIGHTSABER ? "sable láser" :
							"weapon", (obj->value[1]+obj->value[2])/2);
		 if(obj->value[3] == WEAPON_BLASTER || obj->value[3] == WEAPON_VIBRO_BLADE || obj->value[3] == WEAPON_LIGHTSABER || obj->value[3] == WEAPON_FORCE_PIKE)
	      ch_printf(ch, "&z|| &wEnergía: &G%d\n\r", obj->value[5]);
		 ch_printf(ch, "&W++\n\r");
		 break;
		 
	    }
        set_char_color(AT_WHITE, ch);
	    for ( paf = obj->pIndexData->first_affect; paf; paf = paf->next )
		showaffect( ch, paf );
        
	    for ( paf = obj->first_affect; paf; paf = paf->next )
		showaffect( ch, paf );
	    if ( ( obj->item_type == ITEM_CONTAINER ) && ( obj->first_content ) )
	    {
		ch_printf(ch, "&z||&w Contenido de &G%s&w:\n\r\n\r", obj->short_descr );
		set_char_color(AT_LBLUE, ch);
		show_list_to_char( obj->first_content, ch, TRUE, FALSE );
		set_char_color(AT_WHITE, ch);
		ch_printf(ch, "\n\r&z||&w\n\r&W++\n\r", ch);
	    }

	    if (IS_IMMORTAL(ch))
	    {
		sprintf(buf, "Vendedor: %s.  Pujador: %s.  Ronda: %d.\n\r",
                        auction->seller->name, auction->buyer->name,
                        (auction->going + 1));
		send_to_char(buf, ch);
		sprintf(buf, "Tiempo restante de ronda: %d segundos\n\r", auction->pulse/4);
		send_to_char(buf, ch);
	    }
            return;
	}
	else
	{
	    set_char_color ( AT_AUCTION, ch );
	    send_to_char ( "\n\rNo hay nada subastándose ahora mismo. ¿Quieres subastar algo?\n\r", ch );
	    return;
	}
    }

    if ( IS_IMMORTAL(ch) && !str_cmp(arg1,"parar")){
    if (auction->item == NULL)
    {
        send_to_char ("No hay subastas que parar.\n\r",ch);
        return;
    }
    else /* stop the auction */
    {
	set_char_color ( AT_AUCTION, ch );
        sprintf (buf,"La subasta de %s ha sido cancelada por un inmortal.",
                        auction->item->short_descr);
        talk_auction (buf);
        obj_to_char (auction->item, auction->seller);
	if ( IS_SET( sysdata.save_flags, SV_AUCTION ) )
	    save_char_obj(auction->seller);
        auction->item = NULL;
        if (auction->buyer != NULL && auction->buyer != auction->seller) /* return money to the buyer */
        {
            auction->buyer->gold += auction->bet;
            send_to_char ("Se te han devuelto tus créditos.\n\r",auction->buyer);
        }
        return;
    }
    }
    if (!str_cmp(arg1,"pujar") ){
        if (auction->item != NULL)
        {
            int newbet;

	    if ( ch == auction->seller)
	    {
		send_to_char("¡No puedes pujar por tu propio objeto!\n\r", ch);
		return;
	    }

            /* make - perhaps - a bet now */
            if (argument[0] == '\0')
            {
                send_to_char ("¿Pujar cuánto?\n\r",ch);
                return;
            }

            newbet = parsebet (auction->bet, argument);
/*	    ch_printf( ch, "Bid: %d\n\r",newbet);	*/

	    if (newbet < auction->starting)
	    {
		send_to_char("Debes pujar  más alto que la oferta inicial.\n\r", ch);
		return;
	    }

	    /* to avoid slow auction, use a bigger amount than 100 if the bet
 	       is higher up - changed to 10000 for our high economy
            */

            if (newbet < (auction->bet + 100))
            {
                send_to_char ("Debes pujar 100 créditos más que la puja actual.\n\r",ch);
                return;
            }

            if (newbet > ch->gold)
            {
                send_to_char ("¡No tienes tantos créditos!\n\r",ch);
                return;
            }

	    if (newbet > 2000000000)
	    {
		send_to_char("No puedes pujar más de 2000000000 de créditos.\n\r", ch);
		return;
	    }

            /* the actual bet is OK! */

            /* return the gold to the last buyer, if one exists */
            if (auction->buyer != NULL && auction->buyer != auction->seller)
                auction->buyer->gold += auction->bet;

            ch->gold -= newbet; /* substract the gold - important :) */
	    if ( IS_SET( sysdata.save_flags, SV_AUCTION ) )
		save_char_obj(ch);
            auction->buyer = ch;
            auction->bet   = newbet;
            auction->going = 0;
            auction->pulse = PULSE_AUCTION; /* start the auction over again */

            sprintf (buf,"&WNueva puja: &Y%d &Wcréditos por %s.\n\r",newbet,auction->item->short_descr);
            talk_auction (buf);
            return;


        }
        else
        {
            send_to_char ("No hay nada subastándose ahora mismo.\n\r",ch);
            return;
        }
    }
/* finally... */
    if ( ms_find_obj(ch) )
	return;

    obj = get_obj_carry (ch, arg1); /* does char have the item ? */

    if (obj == NULL)
    {
        send_to_char ("No estás cargando eso.\n\r",ch);
        return;
    }

    if (obj->timer > 0)
    {
	send_to_char ("No puedes subastar eso.\n\r", ch);
	return;
    }

    argument = one_argument (argument, arg2);

    if (arg2[0] == '\0')
    {
      auction->starting = 0;
      strcpy(arg2, "0");
    }

    if ( !is_number(arg2) )
    {
	send_to_char("Debes indicar un precio inicial para la subasta.\n\r", ch);
	return;
    }

    if ( atoi(arg2) < 0 )
    {
	send_to_char("¡al menos pide 0 créditos!\n\r", ch);
 	return;
    }

    if (auction->item == NULL)
    switch (obj->item_type)
    {

    default:
        act (AT_TELL, "No puedes subastar $T.",ch, NULL, item_type_name (obj), TO_CHAR);
        return;

/* insert any more item types here... items with a timer MAY NOT BE 
   AUCTIONED! 
*/
    case ITEM_LIGHT:
    case ITEM_TREASURE:    
    case ITEM_RARE_METAL:
    case ITEM_CRYSTAL:
    case ITEM_BOOK:
    case ITEM_FABRIC:
    case ITEM_PAPER:
    case ITEM_ARMOR:
    case ITEM_COMLINK:
    case ITEM_WEAPON:
	case ITEM_GLAUNCHER:
    case ITEM_RLAUNCHER:
	case ITEM_GRENADE:
	case ITEM_SHIPBOMB:
		
    case ITEM_MISSILE:
    case ITEM_CONTAINER:
    case ITEM_GOGGLES:
	separate_obj(obj);
	obj_from_char (obj);
	if ( IS_SET( sysdata.save_flags, SV_AUCTION ) )
	    save_char_obj(ch);
	auction->item = obj;
	auction->bet = 0;
	auction->buyer = ch;
	auction->seller = ch;
	auction->pulse = PULSE_AUCTION;
	auction->going = 0;
	auction->starting = atoi(arg2);

	if (auction->starting > 0)
	  auction->bet = auction->starting;

	sprintf (buf, "&WNuevo objeto: %s&W por &Y%d&W créditos.", obj->short_descr, auction->starting);
	talk_auction (buf);

	return;

    } /* switch */
    else
    {
        act (AT_TELL, "¡Prueba más tarde! Ahora se está subastando $p.",ch,auction->item,NULL,TO_CHAR);
	WAIT_STATE( ch, 1.5 * PULSE_VIOLENCE );
        return;
    }
}



/* Make objects in rooms that are nofloor fall - Scryn 1/23/96 */

void obj_fall( OBJ_DATA *obj, bool through )
{
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *to_room;
    static int fall_count;
    char buf[MAX_STRING_LENGTH];
    static bool is_falling; /* Stop loops from the call to obj_to_room()  -- Altrag */
	
    if ( !obj->in_room || is_falling )
    	return;

    if (fall_count > 30)
    {
    	bug( "object falling in loop more than 30 times", 0 );
	extract_obj(obj);
    	fall_count = 0;
	return;
     }

     if ( IS_SET( obj->in_room->room_flags, ROOM_NOFLOOR )
     &&   CAN_GO( obj, DIR_DOWN )
     &&   !IS_OBJ_STAT( obj, ITEM_MAGIC ) )
     {

	pexit = get_exit( obj->in_room, DIR_DOWN );
    	to_room = pexit->to_room;

    	if (through)
	  fall_count++;
	else
	  fall_count = 0;

	if (obj->in_room == to_room)
	{
	    sprintf(buf, "Object falling into same room, room %d",
		to_room->vnum);
	    bug( buf, 0 );
	    extract_obj( obj );
            return;
	}

	if (obj->in_room->first_person)
	{
	  	act( AT_PLAIN, "$p cae hacia abajo...",
			obj->in_room->first_person, obj, NULL, TO_ROOM );
		act( AT_PLAIN, "$p cae hacia abajo...",
			obj->in_room->first_person, obj, NULL, TO_CHAR );
	}
	obj_from_room( obj );
	is_falling = TRUE;
	obj = obj_to_room( obj, to_room );
	is_falling = FALSE;

	if (obj->in_room->first_person)
	{
	  	act( AT_PLAIN, "$p cae desde arriba...", 
			obj->in_room->first_person, obj, NULL, TO_ROOM );
		act( AT_PLAIN, "$p cae desde arriba...",
			obj->in_room->first_person, obj, NULL, TO_CHAR );
	}

 	if (!IS_SET( obj->in_room->room_flags, ROOM_NOFLOOR ) && through )
	{
/*		int dam = (int)9.81*sqrt(fall_count*2/9.81)*obj->weight/2;
*/		int dam = fall_count*obj->weight/2;
		/* Damage players */
		if ( obj->in_room->first_person && number_percent() > 15 )
		{
			CHAR_DATA *rch;
			CHAR_DATA *vch = NULL;
			int chcnt = 0;
			
			for ( rch = obj->in_room->first_person; rch;
				rch = rch->next_in_room, chcnt++ )
				if ( number_range( 0, chcnt ) == 0 )
					vch = rch;
			act( AT_WHITE, "¡$p le cae encima a $n!", vch, obj, NULL, TO_ROOM );
			act( AT_WHITE, "¡$p te cae encima!", vch, obj, NULL, TO_CHAR );
			damage( vch, vch, dam*vch->top_level, TYPE_UNDEFINED );
		}
    	/* Damage objects */
	    switch( obj->item_type )
     	    {
	     	case ITEM_WEAPON:
		case ITEM_ARMOR:
		    if ( (obj->value[0] - dam) <= 0 )
 		    {
   			if (obj->in_room->first_person)
			{
			act( AT_PLAIN, "¡$p se destruye al caer!", 
				obj->in_room->first_person, obj, NULL, TO_ROOM );
			act( AT_PLAIN, "¡$p se destruye al caer!",
				obj->in_room->first_person, obj, NULL, TO_CHAR );
			}
			make_scraps(obj);
	 	    }
		    else
	           	obj->value[0] -= dam;
		    break;
		default:
		    if ( (dam*15) > get_obj_resistance(obj) )
		    {
	              if (obj->in_room->first_person)
		      {
 			    act( AT_PLAIN, "¡$p se destruye al caer!",
			    	obj->in_room->first_person, obj, NULL, TO_ROOM );
			    act( AT_PLAIN, "¡$p se destruye al caer!",
		    		obj->in_room->first_person, obj, NULL, TO_CHAR );
		      }
		      make_scraps(obj);
		    }
		    break;
	    }
     	}
     	obj_fall( obj, TRUE );
    }
    return;
}

