/*
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
#include <time.h>
#include <unistd.h>
#include "mud.h"


ROOM_INDEX_DATA *generate_exit( ROOM_INDEX_DATA *in_room, EXIT_DATA **pexit );

char *	const	where_name	[] =
{
    "&G&b[&wused as light    &b]&G&w ",
    "&G&b[&wen un dedo   &b]&G&w ",
    "&G&b[&wen un dedo   &b]&G&w ",
    "&G&b[&alrededor del cuello &b]&G&w ",
    "&G&b[&walrededor del cuello &b]&G&w ",
    "&G&b[&wen el cuerpo     &b]&G&w ",
    "&G&b[&wen la cabeza     &b]&G&w ",
    "&G&b[&wen las piernas     &b]&G&w ",
    "&G&b[&wen los pies     &b]&G&w ",
    "&G&b[&wen las manos    &b]&G&w ",
    "&G&b[&wen los brazos     &b]&G&w ",
    "&G&b[&wescudo de energía    &b]&G&w ",
    "&G&b[&wsobre el cuerpo  &b]&G&w ",
    "&G&b[&wen la cintura &b]&G&w ",
    "&G&b[&wen la muñeca&b]&G&w ",
    "&G&b[&wen la muñeca&b]&G&w ",
    "&G&b[&wempuñando          &b]&G&w ",
    "&G&b[&wsostenido             &b]&G&w ",
    "&G&b[&wempuñando     &b]&G&w ",
    "&G&b[&wen las orejas     &b]&G&w ",
    "&G&b[&wen los ojos     &b]&G&w ",
    "&G&b[&wlanzador  &b]&G&w ",
    "&G&b[&wen la espalda     &b]&G&w ",
    "&G&b[&wen la cadera izquierda     &b]&G&w ",   
    "&G&b[&wen la cadera derecha    &b]&G&w ",
    "&G&b[&wen ambas muñecas &b]&G&w "    
};


/*
 * Local functions.
 */
void	show_char_to_char_0	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void	show_char_to_char_1	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void	show_char_to_char	args( ( CHAR_DATA *list, CHAR_DATA *ch ) );
void	show_char_to_char	args( ( CHAR_DATA *list, CHAR_DATA *ch ) );
void	show_ships_to_char	args( ( SHIP_DATA *ship, CHAR_DATA *ch ) );
bool	check_blind		args( ( CHAR_DATA *ch ) );
void    show_condition          args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );


char *format_obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch, bool fShort )
{
    static char buf[MAX_STRING_LENGTH];

    buf[0] = '\0';
    if ( IS_OBJ_STAT(obj, ITEM_INVIS)     )   strcat( buf, "(Invis) "     );
    if ( ( IS_AFFECTED(ch, AFF_DETECT_MAGIC) || IS_IMMORTAL(ch) )
	 && IS_OBJ_STAT(obj, ITEM_MAGIC)  )   strcat( buf, "&B(Aura Azul)&w "   );
    if ( IS_OBJ_STAT(obj, ITEM_GLOW)      )   strcat( buf, "&G&W(&OB&Yr&wi&Wll&wa&Yn&Ote&G&W) "   );
    if ( IS_OBJ_STAT(obj, ITEM_HUM)       )   strcat( buf, "&G&W(&gH&Gu&wm&We&wa&Gn&gte&G&W) "   );
    if ( IS_OBJ_STAT(obj, ITEM_HIDDEN)	  )   strcat( buf, "(Oculto) "	  );
    if ( IS_OBJ_STAT(obj, ITEM_BURRIED)	  )   strcat( buf, "(Burried) "	  );
    if ( IS_IMMORTAL(ch)
	 && IS_OBJ_STAT(obj, ITEM_PROTOTYPE) ) strcat( buf, "(PROTO) "	  );
    if ( IS_AFFECTED(ch, AFF_DETECTTRAPS)
	 && is_trapped(obj)   )   strcat( buf, "(Trampa) "  );

    if ( fShort )
    {
	if ( obj->short_descr )
	    strcat( buf, obj->short_descr );
    }
    else
    {
	if ( obj->description )
	    strcat( buf, obj->description );
    }

    return buf;
}


/*
 * Some increasingly freaky halucinated objects		-Thoric
 */
char *halucinated_object( int ms, bool fShort )
{
    int sms = URANGE( 1, (ms+10)/5, 20 );

    if ( fShort )
    switch( number_range( 6-URANGE(1,sms/2,5), sms ) )
    {
	case  1: return "un sable láser";
	case  2: return "un palo";
	case  3: return "algo siniestro";
	case  4: return "algo";
	case  5: return "algo interesante";
	case  6: return "algo colorido";
	case  7: return "algo que parece guay";
	case  8: return "una cosa ingeniosa";
	case  9: return "una capa de colores brillantes";
	case 10: return "una espada de llamas";
	case 11: return "un enjambre de insectos";
	case 12: return "un esqueleto";
	case 13: return "un fragmento de tu imaginación";
	case 14: return "tu alma";
	case 15: return "la fuerza";
	case 16: return "el lado oscuro de la fuerza";
	case 17: return "un secreto antiguamente guardado";
	case 18: return "el significado de la vida";
	case 19: return "la respuesta";
	case 20: return "la pregunta";
    }
    switch( number_range( 6-URANGE(1,sms/2,5), sms ) )
    {
	case  1: return "Un sable láser ilumina el lugar.";
	case  2: return "El suelo está cubierto de pequeños palos.";
	case  3: return "Algo siniestro llama tu atención.";
	case  4: return "Algo llama tu atención.";
	case  5: return "Algo interesante llama tu atención.";
	case  6: return "Algo colorido brilla por aquí.";
	case  7: return "algo interesante te llama.";
	case  8: return "Una cosa ingeniosa de gran importancia está aquí.";
	case  9: return "Un manto de colores brillantes pide que lo uses.";
	case 10: return "Una espada de llamas arde en el suelo.";
	case 11: return "¡Un enjambre de insectos zumba en tu cara!";
	case 12: return "Algo muy raro descansa ante tus pies.";
	case 13: return "Un fragmento de tu imaginación está a tus órdenes.";
	case 14: return "el lado oscuro de la fuerza está aquí tirado.";
	case 15: return "El lado luminoso de la fuerza está aquí tirado.";
	case 16: return "Algo muy antiguo está aquí acumulando polvo.";
	case 17: return "Un gran secreto está claro para tí.";
	case 18: return "Lo que siempre quisiste decir está escrito ante tus ojos.";
	case 19: return "La respuesta está aquí.";
	case 20: return "el secreto del universo brilla por aquí.";
    }
    return "Whoa!!!";
}


/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */
void show_list_to_char( OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing )
{
    char **prgpstrShow;
    int *prgnShow;
    int *pitShow;
    char *pstrShow;
    OBJ_DATA *obj;
    int nShow;
    int iShow;
    int count, offcount, tmp, ms, cnt;
    bool fCombine;

    if ( !ch->desc )
	return;

    /*
     * if there's no list... then don't do all this crap!  -Thoric
     */
    if ( !list )
    {
    	if ( fShowNothing )
    	{
	   if ( IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE) )
	      send_to_char( "     ", ch );
	   send_to_char( "nada.\n\r", ch );
	}
	return;
    }
    /*
     * Alloc space for output lines.
     */
    count = 0;
    for ( obj = list; obj; obj = obj->next_content )
	count++;

    ms  = (ch->mental_state ? ch->mental_state : 1)
	* (IS_NPC(ch) ? 1 : (ch->pcdata->condition[COND_DRUNK] ? (ch->pcdata->condition[COND_DRUNK]/12) : 1));

    /*
     * If not mentally stable...
     */
    if ( abs(ms) > 40 )
    {
	offcount = URANGE( -(count), (count * ms) / 100, count*2 );
	if ( offcount < 0 )
	  offcount += number_range(0, abs(offcount));
	else
	if ( offcount > 0 )
	  offcount -= number_range(0, offcount);
    }
    else
	offcount = 0;

    if ( count + offcount <= 0 )
    {
    	if ( fShowNothing )
    	{
	   if ( IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE) )
	      send_to_char( "     ", ch );
	   send_to_char( "nada.\n\r", ch );
	}
	return;
    }

    CREATE( prgpstrShow,	char*,	count + ((offcount > 0) ? offcount : 0) );
    CREATE( prgnShow,		int,	count + ((offcount > 0) ? offcount : 0) );
    CREATE( pitShow,		int,	count + ((offcount > 0) ? offcount : 0) );
    nShow	= 0;
    tmp		= (offcount > 0) ? offcount : 0;
    cnt		= 0;

    /*
     * Format the list of objects.
     */
    for ( obj = list; obj; obj = obj->next_content )
    {
	if ( offcount < 0 && ++cnt > (count + offcount) )
	    break;
	if ( tmp > 0 && number_bits(1) == 0 )
	{
	    prgpstrShow [nShow] = str_dup( halucinated_object(ms, fShort) );
	    prgnShow	[nShow] = 1;
	    pitShow	[nShow] = number_range( ITEM_LIGHT, ITEM_BOOK );
	    nShow++;
	    --tmp;
	}
	if ( obj->wear_loc == WEAR_NONE
	&& can_see_obj( ch, obj )
	&& (obj->item_type != ITEM_TRAP || IS_AFFECTED(ch, AFF_DETECTTRAPS) ) )
	{
	    pstrShow = format_obj_to_char( obj, ch, fShort );
	    fCombine = FALSE;

	    if ( IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE) )
	    {
		/*
		 * Look for duplicates, case sensitive.
		 * Matches tend to be near end so run loop backwords.
		 */
		for ( iShow = nShow - 1; iShow >= 0; iShow-- )
		{
		    if ( !strcmp( prgpstrShow[iShow], pstrShow ) )
		    {
			prgnShow[iShow] += obj->count;
			fCombine = TRUE;
			break;
		    }
		}
	    }

	    pitShow[nShow] = obj->item_type;
	    /*
	     * Couldn't combine, or didn't want to.
	     */
	    if ( !fCombine )
	    {
		prgpstrShow [nShow] = str_dup( pstrShow );
		prgnShow    [nShow] = obj->count;
		nShow++;
	    }
	}
    }
    if ( tmp > 0 )
    {
	int x;
	for ( x = 0; x < tmp; x++ )
	{
	    prgpstrShow [nShow] = str_dup( halucinated_object(ms, fShort) );
	    prgnShow	[nShow] = 1;
	    pitShow	[nShow] = number_range( ITEM_LIGHT, ITEM_BOOK );
	    nShow++;
	}
    }

    /*
     * Output the formatted list.		-Color support by Thoric
     */
    for ( iShow = 0; iShow < nShow; iShow++ )
    {
	switch(pitShow[iShow]) {
	default:
	  set_char_color( AT_OBJECT, ch );
	  break;
	case ITEM_BLOOD:
	  set_char_color( AT_BLOOD, ch );
	  break;
	case ITEM_MONEY:
	case ITEM_TREASURE:
	  set_char_color( AT_YELLOW, ch );
	  break;
	case ITEM_FOOD:
	  set_char_color( AT_HUNGRY, ch );
	  break;
	case ITEM_DRINK_CON:
	case ITEM_FOUNTAIN:
	  set_char_color( AT_THIRSTY, ch );
	  break;
	case ITEM_FIRE:
	  set_char_color( AT_FIRE, ch );
	  break;
	case ITEM_SCROLL:
	case ITEM_WAND:
	case ITEM_STAFF:
	  set_char_color( AT_MAGIC, ch );
	  break;
	}
	if ( fShowNothing )
	    send_to_char( "     ", ch );
	send_to_char( prgpstrShow[iShow], ch );
/*	if ( IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE) ) */
	{
	    if ( prgnShow[iShow] != 1 )
		ch_printf( ch, " (%d)", prgnShow[iShow] );
	}

	send_to_char( "\n\r", ch );
	DISPOSE( prgpstrShow[iShow] );
    }

    if ( fShowNothing && nShow == 0 )
    {
	if ( IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE) )
	    send_to_char( "     ", ch );
	send_to_char( "Nada.\n\r", ch );
    }

    /*
     * Clean up.
     */
    DISPOSE( prgpstrShow );
    DISPOSE( prgnShow	 );
    DISPOSE( pitShow	 );
    return;
}


/*
 * Show fancy descriptions for certain spell affects		-Thoric
 */
void show_visible_affects_to_char( CHAR_DATA *victim, CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
/*    if ( IS_AFFECTED(victim, AFF_SANCTUARY) )
    {
        if ( IS_GOOD(victim) )
        {
            set_char_color( AT_WHITE, ch );
            ch_printf( ch, "%s brilla con un aura de resplandor divino.\n\r",
		IS_NPC( victim ) ? capitalize(victim->short_descr) : (victim->name) );
        }
        else if ( IS_EVIL(victim) )
        {
            set_char_color( AT_WHITE, ch );
            ch_printf( ch, "%s brilla bajo un aura de energía oscura.\n\r",
		IS_NPC( victim ) ? capitalize(victim->short_descr) : (victim->name) );
        }
        else
        {
            set_char_color( AT_WHITE, ch );
            ch_printf( ch, "%s está envuelto en una corriente de luz y sombras.\n\r",
		IS_NPC( victim ) ? capitalize(victim->short_descr) : (victim->name) );
        }
    }*/
    if ( IS_AFFECTED(victim, AFF_FIRESHIELD) )
    {
        set_char_color( AT_FIRE, ch );
        ch_printf( ch, "%s está envuelto en un resplandor de llamas místicas.\n\r",
	    IS_NPC( victim ) ? capitalize(victim->short_descr) : (victim->name) );
    }
    if ( IS_AFFECTED(victim, AFF_SHOCKSHIELD) )
    {
        set_char_color( AT_BLUE, ch );
	ch_printf( ch, "%s está rodeado por una cascada de energía.\n\r",
	    IS_NPC( victim ) ? capitalize(victim->short_descr) : (victim->name) );
    }
/*Scryn 8/13*/
    if ( IS_AFFECTED(victim, AFF_ICESHIELD) )
    {
        set_char_color( AT_LBLUE, ch );
        ch_printf( ch, "%s es rodeado por fragmentos brillantes de hielo.\n\r",
	    IS_NPC( victim ) ? capitalize(victim->short_descr) : (victim->name) );
    }
    if ( IS_AFFECTED(victim, AFF_CHARM)       )
    {
	set_char_color( AT_MAGIC, ch );
	ch_printf( ch, "%s tiene el rostro libre de expresión.\n\r",
	    IS_NPC( victim ) ? capitalize(victim->short_descr) : (victim->name) );
    }
    if ( !IS_NPC(victim) && !victim->desc
    &&    victim->switched && IS_AFFECTED(victim->switched, AFF_POSSESS) )
    {
	set_char_color( AT_MAGIC, ch );
	strcpy( buf, PERS( victim, ch ) );
	strcat( buf, " parece estar en un trance profundo...\n\r" );
    }
}

void show_char_to_char_0( CHAR_DATA *victim, CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char message[MSL];
    char colorbuf[4];

    buf[0] = '\0';

    if ( IS_NPC(victim) )
      strcat( buf, " "  );
      
    if ( !IS_NPC(victim) && !victim->desc )
    {
	if ( !victim->switched )		strcat( buf, "(Link Dead) "  );
	else
	if ( !IS_AFFECTED(victim->switched, AFF_POSSESS) )
						strcat( buf, "(Poseyendo) " );
    }
    if ( !IS_NPC(victim)
    && IS_SET(victim->act, PLR_AFK) )		strcat( buf, "[AFK] ");        
    if ( (!IS_NPC(victim) && IS_SET(victim->act, PLR_WIZINVIS))
      || (IS_NPC(victim) && IS_SET(victim->act, ACT_MOBINVIS)) ) 
    {
        if (!IS_NPC(victim))
	sprintf( buf1,"(Invis %d) ", victim->pcdata->wizinvis );
        else sprintf( buf1,"(Mobinvis %d) ", victim->mobinvis);
	strcat( buf, buf1 );
    }
    if ( IS_AFFECTED(victim, AFF_INVISIBLE)   ) strcat( buf, "(Invisible) "      );
    if ( IS_AFFECTED(victim, AFF_HIDE)        ) strcat( buf, "(Ocu) "       );
    if ( IS_AFFECTED(victim, AFF_PASS_DOOR)   ) strcat( buf, "(Translúcido) ");
    if ( IS_AFFECTED(victim, AFF_FAERIE_FIRE) ) strcat( buf, "&P(Aura rosa)&w "  );
    if ( !IS_NPC(victim) && IS_SET(victim->pcdata->act2, ACT_GAGGED)) strcat( buf, "&w&W(&rG&Ra&rg&Rg&re&Rd&w&W)&w ");
    if ( !IS_NPC(victim) && IS_SET(victim->pcdata->act2, ACT_BOUND)) strcat( buf, "&G&W(&gB&Go&wu&Gn&gd&G&W)&w ");
    if ( IS_EVIL(victim)
    &&   IS_AFFECTED(ch, AFF_DETECT_EVIL)     ) strcat( buf, "&R(aura roja)&w "   );
    if ( ( victim->mana > 10 )
    &&   ( IS_AFFECTED( ch , AFF_DETECT_MAGIC ) || IS_IMMORTAL( ch ) ) )
                                                 strcat( buf, "&B(Aura azul)&w "  );   
    if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_LITTERBUG  ) )
						strcat( buf, "(LITTERBUG) "  );
    if ( IS_NPC(victim) && IS_IMMORTAL(ch)
	 && IS_SET(victim->act, ACT_PROTOTYPE) ) strcat( buf, "(PROTO) " );
    if ( victim->desc && victim->desc->connected == CON_EDITING )
						strcat( buf, "(Escribiendo) " );

    set_char_color( AT_PERSON, ch );
    if ( victim && victim->position == victim->defposition && victim->long_descr[0] != '\0' )
    {
	    if(IS_AFFECTED(ch, AFF_INFRARED))
		{ // \033[1;31m
		
			sprintf(buf2, "\033[1;31m");
			strcat( buf, victim->long_descr );
			strcat(buf2, buf);
			send_to_char_noand(buf2, ch);
			show_visible_affects_to_char( victim, ch );
		}
		else
		{
			strcat( buf, victim->long_descr );
			send_to_pager( buf, ch );
			show_visible_affects_to_char( victim, ch );
		}
	return;
    }

   /*   strcat( buf, PERS( victim, ch ) );       old system of titles
    *    removed to prevent prepending of name to title     -Kuran  
    *
    *    But added back bellow so that you can see mobs too :P   -Durga 
    */
      if( IS_IMMORTAL(victim) )
        sprintf(colorbuf, "&w&W%s", color_str( AT_IMMORT, ch) );
      else
        sprintf(colorbuf, "&w&W%s", color_str( AT_PERSON, ch) );

      strcat( buf, colorbuf);
      strcat( buf, PERS( victim, ch ) );  

    switch ( victim->position )
    {
    case POS_DEAD:     strcat( buf, " está muerto." );			break;
    case POS_MORTAL:   strcat( buf, " está al borde de la muerte." );		break;
    case POS_INCAP:    strcat( buf, " está inconsciente." );		break;
    case POS_STUNNED:  strcat( buf, " está aquí aturdido." );	break;

/* Furniture ideas taken from ROT
   Furniture 1.01 is provided by Xerves
   Info rewrite for sleeping/resting/standing/sitting on Objects -- Xerves */
   case POS_SLEEPING:
    if (victim->on != NULL)
        {
            if (IS_SET(victim->on->value[2],SLEEP_AT))
            {
                sprintf(message," está durmiendo en %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
            else if (IS_SET(victim->on->value[2],SLEEP_ON))
            {
                sprintf(message," está durmiendo sobre %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
            else
            {
                sprintf(message, " está durmiendo en %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
        }
        else
        {
          if (ch->position == POS_SITTING
          ||  ch->position == POS_RESTING )
              strcat( buf, " está durmiendo cerca de aquí.&G" );

	  else
              strcat( buf, " está durmiendo tranquilamente aquí.&G" );
        }
          break;
    case POS_RESTING:
        if (victim->on != NULL)
        {
            if (IS_SET(victim->on->value[2],REST_AT))
            {
                sprintf(message," está descansando en %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
            else if (IS_SET(victim->on->value[2],REST_ON))
            {
                sprintf(message," está descansando en %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
            else
            {
                sprintf(message, " está descansando en %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
        }
        else
        {
          if (ch->position == POS_RESTING)
              strcat ( buf, " está descansando a tu lado.&G" );
          else
	  if (ch->position == POS_MOUNTED)
	      strcat ( buf, " está descansando a los pies de tu montura.&G" );
	  else
              strcat (buf, " está descansando aquí.&G" );
        }
          break;
    case POS_SITTING:
        if (victim->on != NULL)
        {
            if (IS_SET(victim->on->value[2],SIT_AT))
            {
                sprintf(message," está sentado en %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
            else if (IS_SET(victim->on->value[2],SIT_ON))
            {
                sprintf(message," está sentado en %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
            else
            {
                sprintf(message, " está sentado en %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
        }
        else
	    strcat(buf, " está sentado aquí.");
	break;
    case POS_STANDING:
     if (victim->on != NULL)
        {
            if (IS_SET(victim->on->value[2],STAND_AT))
            {
                sprintf(message," está de pie en %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
            else if (IS_SET(victim->on->value[2],STAND_ON))
            {
                sprintf(message," está de pie en %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
            else
            {
                sprintf(message, " está de pie en %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
        }
      else
        if ( IS_IMMORTAL(victim) )
        {   sprintf( colorbuf, "&w&W%s", color_str(AT_IMMORT, ch) );
            strcat( buf, colorbuf);
            strcat( buf, " está delante tuyo.&G" );
        }
	else
        if ( ( victim->in_room->sector_type == SECT_UNDERWATER )
        && !IS_AFFECTED(victim, AFF_AQUA_BREATH) && !IS_NPC(victim) )
            strcat( buf, " se está ahogando aquí.&G" );
	else
	if ( victim->in_room->sector_type == SECT_UNDERWATER )
            strcat( buf, " está aquí en el agua.&G" );
	else
	if ( ( victim->in_room->sector_type == SECT_OCEANFLOOR )
	&& !IS_AFFECTED(victim, AFF_AQUA_BREATH) && !IS_NPC(victim) )
	    strcat( buf, " se está ahogando aquí.&G" );
	else
	if ( victim->in_room->sector_type == SECT_OCEANFLOOR )
	    strcat( buf, " está aquí de pie en el agua.&G" );
	else
	if ( IS_AFFECTED(victim, AFF_FLOATING)
        || IS_AFFECTED(victim, AFF_FLYING) )
          strcat( buf, " se mantiene aquí.&G" );
        else
          strcat( buf, " está aquí de pie.&G" );
          break;

    case POS_SHOVE:    strcat( buf, " is being shoved around." );	break;
    case POS_DRAG:     strcat( buf, " is being dragged around." );	break;
    case POS_MOUNTED:
	strcat( buf, " está aquí sobre " );
	if ( !victim->mount )
	    strcat( buf, "el aire???" );
	else
	if ( victim->mount == ch )
	    strcat( buf, "tu espalda." );
	else
	if ( victim->in_room == victim->mount->in_room )
	{
	    strcat( buf, PERS( victim->mount, ch ) );
	    strcat( buf, "." );
	}
	else
	    strcat( buf, "alguien que se ha ido??" );
	break;
    case POS_FIGHTING:
	strcat( buf, " está aquí luchando " );
	if ( !victim->fighting )
	    strcat( buf, "con el aire???" );
	else if ( who_fighting( victim ) == ch )
	    strcat( buf, "¡contigo!" );
	else if ( victim->in_room == victim->fighting->who->in_room )
	{
	    strcat( buf, PERS( victim->fighting->who, ch ) );
	    strcat( buf, "." );
	}
	else
	    strcat( buf, "con alguien que se ha ido??" );
	break;
    }

    strcat( buf, "\n\r" );
    buf[0] = UPPER(buf[0]);
   if(IS_AFFECTED(ch, AFF_INFRARED))
   { // \033[1;31m
	sprintf(buf2, "\033[1;31m");
    strcat(buf2, buf);
    send_to_char_noand(buf2, ch);
    show_visible_affects_to_char( victim, ch );
   }
   else
   {
    send_to_char( buf, ch ); //Nonseq
    show_visible_affects_to_char( victim, ch );
   }
    return;
}



void show_char_to_char_1( CHAR_DATA *victim, CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    int iWear;
    bool found;

    if ( can_see( victim, ch ) )
    {
    act( AT_ACTION, "$n te mira.", ch, NULL, victim, TO_VICT    );
    if(ch != victim)
     act( AT_ACTION, "$n mira a $N.",  ch, NULL, victim, TO_NOTVICT );
    else
     act( AT_ACTION, "$n se mira.", ch, NULL, victim, TO_NOTVICT );
    }

    if(victim == ch && !IS_NPC(ch))
    {
     if(!IS_DROID(ch))
	ch_printf(ch, "Eres de complexión %s y tu altura es %s.\n\r", build_name[ch->build], height_name[ch->pheight]);
     else
	ch_printf(ch, "Tu aspecto es %s y %s.\n\r", droid_name[ch->build], height_name[ch->pheight]);

     if(ch->pcdata->disguise && ch->pcdata->disguise[0] != '\0')
	ch_printf(ch, "Te has disfrazado de %s\n\r", ch->pcdata->disguise);
    }
 
    if ( victim->description[0] != '\0' )
    {
	send_to_char( victim->description, ch );
    }
    else
    {
    act( AT_PLAIN, "No le ves nada especial.", ch, NULL, victim, TO_CHAR );
    }

    show_condition( ch, victim );

    found = FALSE;
    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
	if ( ( obj = get_eq_char( victim, iWear ) ) != NULL
	&&   can_see_obj( ch, obj ) )
	{
	    if ( !found )
	    {
		send_to_char( "\n\r", ch );
		act( AT_PLAIN, "$N está usando:", ch, NULL, victim, TO_CHAR );
		found = TRUE;
	    }
	    send_to_char( where_name[iWear], ch );
	    send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
	    send_to_char( "\n\r", ch );
	}
    }

    /*
     * Crash fix here by Thoric
     */
    if ( IS_NPC(ch) || victim == ch )
      return;

    if ( number_percent( ) < ch->pcdata->learned[gsn_peek] )
    {
	send_to_char( "\n\rFisgas en su inventario:\n\r", ch );
	show_list_to_char( victim->first_carrying, ch, TRUE, TRUE );
	learn_from_success( ch, gsn_peek );
    }
    else
      if ( ch->pcdata->learned[gsn_peek] )
        learn_from_failure( ch, gsn_peek );

    return;
}


void show_char_to_char( CHAR_DATA *list, CHAR_DATA *ch )
{
    CHAR_DATA *rch;

    for ( rch = list; rch; rch = rch->next_in_room )
    {
	if ( rch == ch )
	    continue;

	if ( can_see( ch, rch ) )
	{
	    show_char_to_char_0( rch, ch );
	}
	else if ( rch->race == RACE_DEFEL )
	{
	    set_char_color( AT_BLOOD, ch );
	    send_to_char( "Ves un par de ojos rojos mirando en tu dirección.\n\r", ch );	
	}
	else if ( room_is_dark( ch->in_room )
	&&        IS_AFFECTED(rch, AFF_INFRARED ) )
	{
	    set_char_color( AT_BLOOD, ch );
	    send_to_char( "La forma roja de una criatura viviente está aquí.\n\r", ch );
	}
    }

    return;
}

void show_ships_to_char( SHIP_DATA *ship, CHAR_DATA *ch )
{
    SHIP_DATA *rship;
    SHIP_DATA *nship=NULL;

    for ( rship = ship; rship; rship = nship )
    {
        ch_printf( ch , "&W%s%-35s     ",color_str(AT_SHIP, ch) ,rship->name );  
        if ( ( nship = rship->next_in_room ) !=NULL )
        {
            ch_printf( ch , "%-35s", nship->name );  
            nship = nship->next_in_room;
        }   
        ch_printf( ch, "\n\r&w");
    }

    return;
}



bool check_blind( CHAR_DATA *ch )
{
    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT) )
	return TRUE;
	
    if ( IS_AFFECTED(ch, AFF_TRUESIGHT) )
      return TRUE;

    if ( IS_AFFECTED(ch, AFF_BLIND) )
    {
	send_to_char( "¡No puedes ver nada!\n\r", ch );
	return FALSE;
    }

    return TRUE;
}

/*
 * Returns classical DIKU door direction based on text in arg	-Thoric
 */
int get_door( char *arg )
{
    int door;

	 if ( !str_cmp( arg, "n"  ) || !str_cmp( arg, "north"	  ) || !str_cmp( arg, "norte"	  ) ) door = 0;
    else if ( !str_cmp( arg, "e"  ) || !str_cmp( arg, "east"	  ) || !str_cmp( arg, "este"	  ) ) door = 1;
    else if ( !str_cmp( arg, "s"  ) || !str_cmp( arg, "south"	  ) || !str_cmp( arg, "sur"	  ) ) door = 2;
    else if ( !str_cmp( arg, "w"  ) || !str_cmp( arg, "west"	  ) || !str_cmp( arg, "o"	  ) || !str_cmp( arg, "oeste"	  ) ) door = 3;
    else if ( !str_cmp( arg, "u"  ) || !str_cmp( arg, "up"	  ) || !str_cmp( arg, "ar"	  ) || !str_cmp( arg, "arriba"	  ) ) door = 4;
    else if ( !str_cmp( arg, "d"  ) || !str_cmp( arg, "down"	  ) || !str_cmp( arg, "ab"	  ) || !str_cmp( arg, "abajo"	  ) ) door = 5;
    else if ( !str_cmp( arg, "ne" ) || !str_cmp( arg, "northeast" ) || !str_cmp( arg, "noreste"	  ) ) door = 6;
    else if ( !str_cmp( arg, "nw" ) || !str_cmp( arg, "northwest" ) || !str_cmp( arg, "no"	  ) || !str_cmp( arg, "noroeste"	  ) ) door = 7;
    else if ( !str_cmp( arg, "se" ) || !str_cmp( arg, "southeast" ) || !str_cmp( arg, "sureste"	  ) ) door = 8;
    else if ( !str_cmp( arg, "sw" ) || !str_cmp( arg, "southwest" ) || !str_cmp( arg, "so"	  ) || !str_cmp( arg, "suroeste"	  ) ) door = 9;
    else door = -1;
    return door;
}

void do_look( CHAR_DATA *ch, char *argument )
{
    char arg  [MAX_INPUT_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    EXIT_DATA *pexit;
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    ROOM_INDEX_DATA *original;
    char *pdesc;
    bool doexaprog; 
    sh_int door;
    int number, cnt;

    if ( !ch->desc )
	return;

    if ( ch->position < POS_SLEEPING )
    {
	send_to_char( "¡Solo ves estrellas!\n\r", ch );
	return;
    }

    if ( ch->position == POS_SLEEPING )
    {
	send_to_char( "¡Estás durmiendo!\n\r", ch );
	return;
    }

    if ( !check_blind( ch ) )
	return;

    if ( !IS_NPC(ch)
    &&   !IS_SET(ch->act, PLR_HOLYLIGHT)
    &&   !IS_AFFECTED(ch, AFF_TRUESIGHT)
    &&   !IS_AFFECTED(ch, AFF_INFRARED)
    &&   room_is_dark( ch->in_room ) )
    {
	set_char_color( AT_DGREY, ch );
	send_to_char( "Está demasiado oscuro.\n\r", ch );
	show_char_to_char( ch->in_room->first_person, ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    doexaprog = str_cmp( "noprog", arg2 ) && str_cmp( "noprog", arg3 );

    if ( arg1[0] == '\0' || !str_cmp( arg1, "auto" ) )
    {
       SHIP_DATA * ship;
    
	/* 'look' or 'look auto' */
	set_char_color( AT_RMNAME, ch);
	ch_printf(ch, "&R-=&r( &G&W%s%s &G&r)&R=-&C&w ", color_str( AT_RMNAME, ch), ch->in_room->name);
	//send_to_char( ch->in_room->name, ch);
	//send_to_char(" ", ch);

        if ( !ch->desc->original )
        {
  	  if ((get_trust(ch) >= LEVEL_IMMORTAL) && (IS_SET(ch->pcdata->flags, PCFLAG_ROOM)))
	  {
             if( get_trust( ch ) >= LEVEL_IMMORTAL )
             {
                 if( IS_SET( ch->act, PLR_ROOMVNUM ) )
                 {
                     set_char_color( AT_BLUE, ch );   /* Added 10/17 by Kuran of */
                     send_to_char( "{", ch );   /* SWReality */
                     ch_printf( ch, "&G&W%s%d", color_str(AT_RVNUM, ch), ch->in_room->vnum );
                     set_char_color( AT_BLUE, ch );   /* Added 10/17 by Kuran of */
                     send_to_char( "}", ch );
                 }
                 if( IS_SET( ch->pcdata->flags, PCFLAG_ROOM ) )
                 {
                     set_char_color( AT_CYAN, ch );
                     send_to_char( "[", ch );
                     send_to_char( " ", ch );
                     set_char_color( AT_RFLAGS1, ch );
                     send_to_char( flag_string( ch->in_room->room_flags, r_flags ), ch );
                     send_to_char( " ", ch );
                     if( ch->in_room->room_flags2 != 0 )
                     {
                        set_char_color( AT_RFLAGS2, ch );
                        send_to_char( flag_string( ch->in_room->room_flags2, r_flags2 ), ch );
                        send_to_char( " ", ch );
                     }
                     set_char_color( AT_CYAN, ch );
                     send_to_char( "]", ch );
                 }
             }
          }
        }

	send_to_char( "\n\r", ch );
	set_char_color( AT_RMDESC, ch ); 	
	
	if ( arg1[0] == '\0'
	|| ( !IS_NPC(ch) && !IS_SET(ch->act, PLR_BRIEF) ) )
	    send_to_char( ch->in_room->description, ch );
 

        if( !IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOEXIT) )
        {
         if ( IS_SET(ch->pcdata->flags, PCFLAG_MAP))
           do_newexits(ch, "" );
	 else
	   do_exits(ch, "" );
	}
	

	show_ships_to_char( ch->in_room->first_ship, ch );
        show_list_to_char( ch->in_room->first_content, ch, FALSE, FALSE );
        show_char_to_char( ch->in_room->first_person, ch );
        
	if ( str_cmp( arg1, "auto" ) )
	if (   (ship = ship_from_cockpit(ch->in_room->vnum))  != NULL )
	{
	      set_char_color(  AT_WHITE, ch );
    	                 ch_printf( ch , "\n\rA través de la ventana ves:\n\r" );
    	           
	      if (ship->starsystem)
	      {
	           MISSILE_DATA *missile;
	           SHIP_DATA *target;
                   PLANET_DATA *planet;
                   	           
    	           set_char_color(  AT_GREEN, ch );
    	           if ( ship->starsystem->star1 && str_cmp(ship->starsystem->star1,"") ) 
    	                 ch_printf(ch, "&YLa estrella %s.\n\r" , 
    	                        ship->starsystem->star1);
    	           if ( ship->starsystem->star2 && str_cmp(ship->starsystem->star2,"")  ) 
    	                 ch_printf(ch, "&YLa estrella %s.\n\r" , 
    	                        ship->starsystem->star2 );
    	           for( planet = ship->starsystem->first_planet; planet; planet = planet->next_in_system ) 
		    if(planet->controls == 0)
    	                 ch_printf(ch, "&Gel planeta %s.\n\r" , planet->name );
		    else
			 ch_printf(ch, "&OAsteroide designado %s.\n\r", planet->name);
    	           for ( target = ship->starsystem->first_ship; target; target = target->next_in_starsystem )
                   {       
                        if ( target != ship ) 
			{
			 if(ship->class != 11)
                           ch_printf(ch, "&C%s\n\r", 
                           	target->name);
			 else
			  ch_printf(ch, "&O%s\n\r", target->name); }
                   }
    	           for ( missile = ship->starsystem->first_missile; missile; missile = missile->next_in_starsystem )
                   {        
                           ch_printf(ch, "&R%s\n\r", 
                           	missile->missiletype == CONCUSSION_MISSILE ? "A Concusion Missile" : 
    			        ( missile->missiletype ==  PROTON_TORPEDO ? "A Torpedo" : 
    			        ( missile->missiletype ==  HEAVY_ROCKET ? "A Heavy Rocket" : "A Heavy Bomb" ) ) );
                   }
                     
	      }
	      else if ( ship->location == ship->lastdoc )
	      {
	          ROOM_INDEX_DATA *to_room;
	          to_room = ship->in_room;
	          if (to_room) //get_room_index( ship->location ) ) != NULL )
	          {
	            ch_printf( ch, "\n\r" );
	            original = ch->in_room;
                    char_from_room( ch );
                    char_to_room( ch, to_room );
                    do_glance( ch, "" );	
                    char_from_room( ch );
                    char_to_room( ch, original );
                  }    
	      }
	      
	      
	}
	
	return;
    }

    if ( !str_cmp( arg1, "bajo" ) )
    {
	int count;

	/* 'look under' */
	if ( arg2[0] == '\0' )
	{
	    send_to_char( "¿mirar bajo qué?\n\r", ch );
	    return;
	}

	if ( ( obj = get_obj_here( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "No ves eso aquí.\n\r", ch );
	    return;
	}
	if ( ch->carry_weight + obj->weight > can_carry_w( ch ) )
	{
	    send_to_char( "Pesa mucho para que puedas mirar debajo.\n\r", ch );
	    return;
	}
	count = obj->count;
	obj->count = 1;
	act( AT_PLAIN, "Levantas $p y miras debajo:", ch, obj, NULL, TO_CHAR );
	act( AT_PLAIN, "$n levanta $p y mira debajo.", ch, obj, NULL, TO_ROOM );
	obj->count = count;
	if ( IS_OBJ_STAT( obj, ITEM_COVERING ) )
	   show_list_to_char( obj->first_content, ch, TRUE, TRUE );
	else
	   send_to_char( "nada.\n\r", ch );
	if ( doexaprog ) oprog_examine_trigger( ch, obj );
	return;
    }

    if ( !str_cmp( arg1, "dentro" ) || !str_cmp( arg1, "en" ) )
    {
	int count;

	/* 'look in' */
	if ( arg2[0] == '\0' )
	{
	    send_to_char( "¿Mirar dentro de qué?\n\r", ch );
	    return;
	}

	if ( ( obj = get_obj_here( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "No ves eso aquí.\n\r", ch );
	    return;
	}

	switch ( obj->item_type )
	{
	default:
	    send_to_char( "Eso no es un contenedor.\n\r", ch );
	    break;

	case ITEM_DRINK_CON:
	    if ( obj->value[1] <= 0 )
	    {
		send_to_char( "No contiene nada.\n\r", ch );
	        if ( doexaprog ) oprog_examine_trigger( ch, obj );
		break;
	    }

	    ch_printf( ch, "Está %s de un líquido %s.\n\r",
		obj->value[1] <     obj->value[0] / 4
		    ? "más o menos lleno" :
		obj->value[1] < 3 * obj->value[0] / 4
		    ? "casi lleno"     : "más o menos lleno",
		liq_table[obj->value[2]].liq_color
		);

	    if ( doexaprog ) oprog_examine_trigger( ch, obj );
	    break;

	case ITEM_PORTAL:
	    for ( pexit = ch->in_room->first_exit; pexit; pexit = pexit->next )
	    {
		if ( pexit->vdir == DIR_PORTAL
		&&   IS_SET(pexit->exit_info, EX_PORTAL) )
		{
		    if ( room_is_private( ch, pexit->to_room )
		    &&   get_trust(ch) < sysdata.level_override_private )
		    {
			set_char_color( AT_WHITE, ch );
			send_to_char( "¡Esa habitación es pribada!\n\r", ch );
			return;
		    }
		    original = ch->in_room;
		    char_from_room( ch );
		    char_to_room( ch, pexit->to_room );
		    do_look( ch, "auto" );
		    char_from_room( ch );
		    char_to_room( ch, original );
		    return;
		}
	    }
	    send_to_char( "Ves el caos...\n\r", ch );
	    break;
	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	case ITEM_DROID_CORPSE:
	    if ( IS_SET(obj->value[1], CONT_CLOSED) )
	    {
		send_to_char( "No mientras no se abra.\n\r", ch );
		break;
	    }

	    count = obj->count;
	    obj->count = 1;
	    act( AT_PLAIN, "$p contiene:", ch, obj, NULL, TO_CHAR );
	    obj->count = count;
	    show_list_to_char( obj->first_content, ch, TRUE, TRUE );
	    if ( doexaprog ) oprog_examine_trigger( ch, obj );
	    break;
	}
	return;
    }

    if ( (pdesc=get_extra_descr(arg1, ch->in_room->first_extradesc)) != NULL )
    {
	send_to_char( pdesc, ch );
	return;
    }

    door = get_door( arg1 );
    if ( ( pexit = find_door( ch, arg1, TRUE ) ) != NULL )
    {
      if ( pexit->keyword )
      {
	  if ( IS_SET(pexit->exit_info, EX_CLOSED)
	  &&  !IS_SET(pexit->exit_info, EX_WINDOW) )
	  {
	      if ( IS_SET(pexit->exit_info, EX_SECRET)
	      &&   door != -1 )
		send_to_char( "Nada especial allí.\n\r", ch );
	      else
		act( AT_PLAIN, "$d está cerrado.", ch, NULL, pexit->keyword, TO_CHAR );
	      return;
	  }
	  if ( IS_SET( pexit->exit_info, EX_BASHED ) )
	      act(AT_RED, "¡$d ha sido sacada de sus visagras!",ch, NULL, pexit->keyword, TO_CHAR);
      }

      if ( pexit->description && pexit->description[0] != '\0' )
  	send_to_char( pexit->description, ch );
      else
	send_to_char( "Nada especial allí.\n\r", ch );

      /*
       * Ability to look into the next room			-Thoric
       */
      if ( pexit->to_room
      && ( IS_AFFECTED( ch, AFF_SCRYING )
      ||   IS_SET( pexit->exit_info, EX_xLOOK )
      ||   get_trust(ch) >= LEVEL_IMMORTAL ) )
      {
        if ( !IS_SET( pexit->exit_info, EX_xLOOK )
        &&    get_trust( ch ) < LEVEL_IMMORTAL )
        {
  	  set_char_color( AT_MAGIC, ch );
	  send_to_char( "Intentas espiar...\n\r", ch );
          /* Change by Narn, Sept 96 to allow characters who don't have the
             scry spell to benefit from objects that are affected by scry.
          */
	  if (!IS_NPC(ch) )
          {
            int percent = ch->pcdata->learned[ skill_lookup("scry") ];
            if ( !percent )
              percent = 99;
 
	    if(  number_percent( ) > percent ) 
	    {
	      send_to_char( "Fallas.\n\r", ch );
	      return;
	    }
          }
        }
        if ( room_is_private( ch, pexit->to_room )
        &&   get_trust(ch) < sysdata.level_override_private )
        {
	  set_char_color( AT_WHITE, ch );
	  send_to_char( "¡Esa habitación es pribada!\n\r", ch );
	  return;
        }
        original = ch->in_room;
        if ( pexit->distance > 1 )
        {  
           ROOM_INDEX_DATA * to_room;
           if ( (to_room=generate_exit(ch->in_room, &pexit)) != NULL )
           {
              char_from_room( ch );
              char_to_room( ch, to_room );
           }
           else
           {
              char_from_room( ch );
              char_to_room( ch, pexit->to_room );
           }
        }
        else
        {
           char_from_room( ch );
           char_to_room( ch, pexit->to_room );
        }
        do_look( ch, "auto" );
        char_from_room( ch );
        char_to_room( ch, original );
      }
      return;
    }
    else
    if ( door != -1 )
    {
	send_to_char( "nada especial allí.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) != NULL )
    {
	show_char_to_char_1( victim, ch );
	return;
    }


    /* finally fixed the annoying look 2.obj desc bug	-Thoric */
    number = number_argument( arg1, arg );
    for ( cnt = 0, obj = ch->last_carrying; obj; obj = obj->prev_content )
    {
	if ( can_see_obj( ch, obj ) )
	{
	    if ( (pdesc=get_extra_descr(arg, obj->first_extradesc)) != NULL )
	    {
		if ( (cnt += obj->count) < number )
		  continue;
		send_to_char( pdesc, ch );
	        if ( doexaprog ) oprog_examine_trigger( ch, obj );
		return;
	    }

	    if ( (pdesc=get_extra_descr(arg, obj->pIndexData->first_extradesc)) != NULL )
	    {
		if ( (cnt += obj->count) < number )
		  continue;
		send_to_char( pdesc, ch );
	        if ( doexaprog ) oprog_examine_trigger( ch, obj );
		return;
	    }
	    
	    if ( nifty_is_name_prefix( arg, obj->name ) )
	    {
		if ( (cnt += obj->count) < number )
		  continue;
		pdesc = get_extra_descr( obj->name, obj->pIndexData->first_extradesc );
		if ( !pdesc )
		  pdesc = get_extra_descr( obj->name, obj->first_extradesc );
		if ( !pdesc )
		  send_to_char( "No ves nada especial.\r\n", ch );
		else
		  send_to_char( pdesc, ch );
		if ( doexaprog ) oprog_examine_trigger( ch, obj );
		  return;
	    }
	}
    }

    for ( obj = ch->in_room->last_content; obj; obj = obj->prev_content )
    {
	if ( can_see_obj( ch, obj ) )
	{
	    if ( (pdesc=get_extra_descr(arg, obj->first_extradesc)) != NULL )
	    {
		if ( (cnt += obj->count) < number )
		  continue;
		send_to_char( pdesc, ch );
	        if ( doexaprog ) oprog_examine_trigger( ch, obj );
		return;
	    }

	    if ( (pdesc=get_extra_descr(arg, obj->pIndexData->first_extradesc)) != NULL )
	    {
		if ( (cnt += obj->count) < number )
		  continue;
		send_to_char( pdesc, ch );
	        if ( doexaprog ) oprog_examine_trigger( ch, obj );
		return;
	    }
	    if ( nifty_is_name_prefix( arg, obj->name ) )
	    {
		if ( (cnt += obj->count) < number )
		  continue;
		pdesc = get_extra_descr( obj->name, obj->pIndexData->first_extradesc );
		if ( !pdesc )
		  pdesc = get_extra_descr( obj->name, obj->first_extradesc );
		if ( !pdesc )
		  send_to_char( "No ves nada especial.\r\n", ch );
		else
		  send_to_char( pdesc, ch );
		if ( doexaprog ) oprog_examine_trigger( ch, obj );
		  return;
	    }
	}
    }

    send_to_char( "No ves eso aquí.\n\r", ch );
    return;
}

void show_condition( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf[MAX_STRING_LENGTH];
    int percent;

    if ( victim->max_hit > 0 )
        percent = ( 100 * victim->hit ) / victim->max_hit;
    else
        percent = -1;


    strcpy( buf, PERS(victim, ch) );

    if (( IS_NPC ( victim ) && IS_SET( victim->act , ACT_DROID )) || IS_DROID(victim))
    {
    
         if ( percent >= 100 ) strcat( buf, " está en perfecto estado.\n\r"  );
    else if ( percent >=  90 ) strcat( buf, " tiene algún que otro leve arañazo.\n\r" );
    else if ( percent >=  80 ) strcat( buf, " tiene un par de rasguños.\n\r"     );
    else if ( percent >=  70 ) strcat( buf, " tiene algunas abolladuras.\n\r"         );
    else if ( percent >=  60 ) strcat( buf, " tiene un par de agujeros en la carcasa.\n\r"    );
    else if ( percent >=  50 ) strcat( buf, " tiene algunas piezas rotas.\n\r" );
    else if ( percent >=  40 ) strcat( buf, " tiene algunos circuitos expuestos.\n\r"    );
    else if ( percent >=  30 ) strcat( buf, " pierde aceite.\n\r"   );
    else if ( percent >=  20 ) strcat( buf, " está echando humo.\n\r"       );
    else if ( percent >=  10 ) strcat( buf, " está casi roto.\n\r"        );
    else                       strcat( buf, " está a punto de explotar.\n\r"              );
    
    }
    else  
    { 
    
         if ( percent >= 100 ) strcat( buf, " está en perfecto estado de salud.\n\r"  );
    else if ( percent >=  90 ) strcat( buf, " tiene un par de arañazos.\n\r" );
    else if ( percent >=  80 ) strcat( buf, " tiene un par de moretones.\n\r"     );
    else if ( percent >=  70 ) strcat( buf, " tiene algunos cortes.\n\r"         );
    else if ( percent >=  60 ) strcat( buf, " tiene algunas heridas.\n\r"    );
    else if ( percent >=  50 ) strcat( buf, " tiene muchas heridas desagradables.\n\r" );
    else if ( percent >=  40 ) strcat( buf, " sangra abundantemente.\n\r"    );
    else if ( percent >=  30 ) strcat( buf, " es un río de sangre.\n\r"   );
    else if ( percent >=  20 ) strcat( buf, " tiene los intestinos al aire.\n\r"       );
    else if ( percent >=  10 ) strcat( buf, " está al borde de la muerte.\n\r"        );
    else                       strcat( buf, " está muriendo.\n\r"              );
    
    }
    buf[0] = UPPER(buf[0]);
    send_to_char( buf, ch );
    return;
}

/* A much simpler version of look, this function will show you only
the condition of a mob or pc, or if used without an argument, the
same you would see if you enter the room and have config +brief.
-- Narn, winter '96
*/
void do_glance( CHAR_DATA *ch, char *argument )
{
  char arg1 [MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  int save_act;

  if ( !ch->desc )
    return;

  if ( ch->position < POS_SLEEPING )
  {
    send_to_char( "¡Solo ves estrellas!\n\r", ch );
    return;
  }

  if ( ch->position == POS_SLEEPING )
  {
    send_to_char( "¡Estás durmiendo!\n\r", ch );
    return;
  }

  if ( !check_blind( ch ) )
    return;

  argument = one_argument( argument, arg1 );

  if ( arg1[0] == '\0' )
  {
    save_act = ch->act;
    SET_BIT( ch->act, PLR_BRIEF );
    do_look( ch, "auto" );
    ch->act = save_act;
    return;
  }

  if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
  {
    send_to_char( "No está aquí.", ch );
    return;
  }
  else
  {
    if ( can_see( victim, ch ) )
    {
      act( AT_ACTION, "$n te echa una ojeada.", ch, NULL, victim, TO_VICT    );
      act( AT_ACTION, "$n echa una ojeada a $N.",  ch, NULL, victim, TO_NOTVICT );
    }

    show_condition( ch, victim );
    return;
  }

  return;
}


void do_examine( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    BOARD_DATA *board;
    sh_int dam;

    if ( !argument )
    {
	bug( "do_examine: null argument.", 0);
	return;
    }

    if ( !ch )
    {
	bug( "do_examine: null ch.", 0);
	return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "¿Examinar qué?\n\r", ch );
	return;
    }

    sprintf( buf, "%s noprog", arg );
    do_look( ch, buf );

    /*
     * Support for looking at boards, checking equipment conditions,
     * and support for trigger positions by Thoric
     */
    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	if ( (board = get_board( obj )) != NULL )
	{
	   if ( board->num_posts )
	     ch_printf( ch, "Hay %d notas publicadas aquí. Teclea 'nota listar' para listarlas.\n\r", board->num_posts );
	   else
	     send_to_char( "No hay notas publicadas aquí.\n\r", ch );
	}

	switch ( obj->item_type )
	{
	default:
	    break;

    case ITEM_RLAUNCHER:
	    if(obj->value[5] == 0)
		 ch_printf(ch, "&wNo está cargado con nada.\n\r");
	    else
		 ch_printf(ch, "Está cargado con un misil %s.\n\r", obj->value[1] == 1 ? "incendiario" : "explosivo");
		break;

	case ITEM_ARMOR:
	    if ( obj->value[1] == 0 )
	      obj->value[1] = obj->value[0];
	    if ( obj->value[1] == 0 )
	      obj->value[1] = 1;
	    dam = (sh_int) ((obj->value[0] * 10) / obj->value[1]);
	    strcpy( buf, "Al mirar de cerca, te das cuenta que está ");
	    if (dam >= 10) strcat( buf, "En perfecto estado.");
       else if (dam ==  9) strcat( buf, "en muy buen estado.");
       else if (dam ==  8) strcat( buf, "en buen estado.");
       else if (dam ==  7) strcat( buf, "presentando síntomas de desgaste.");
       else if (dam ==  6) strcat( buf, "mostrando síntomas de descuido.");
       else if (dam ==  5) strcat( buf, "pidiendo una reparación.");
       else if (dam ==  4) strcat( buf, "necesitando una buena reparación.");
       else if (dam ==  3) strcat( buf, "en extrema necesidad de una reparación.");
       else if (dam ==  2) strcat( buf, "casi inutilizable.");
       else if (dam ==  1) strcat( buf, "A punto de romperse por completo.");
       else if (dam <=  0) strcat( buf, "roto.");
	    strcat( buf, "\n\r" );
	    send_to_char( buf, ch );
	    break;

	case ITEM_WEAPON:
	    dam = INIT_WEAPON_CONDITION - obj->value[0];
	    strcpy( buf, "Al mirar de cerca, te das cuenta de que está ");
	    if (dam ==  0) strcat( buf, "en perfecto estado.");
       else if (dam ==  1) strcat( buf, "en escelente estado.");
       else if (dam ==  2) strcat( buf, "en muy buen estado.");
       else if (dam ==  3) strcat( buf, "en buen estado.");
       else if (dam ==  4) strcat( buf, "presentando síntomas de desgaste.");
       else if (dam ==  5) strcat( buf, "mostrando síntomas de descuido.");
       else if (dam ==  6) strcat( buf, "necesitando repararse.");
       else if (dam ==  7) strcat( buf, "necesitando una buena reparación.");
       else if (dam ==  8) strcat( buf, "Necesitando una reparación extrema.");
       else if (dam ==  9) strcat( buf, "mostrando síntomas de un evidente mal uso.");
       else if (dam == 10) strcat( buf, "a punto de romperse.");
       else if (dam == 11) strcat( buf, "en las útlimas.");
       else if (dam == 12) strcat( buf, "roto.");
	    strcat( buf, "\n\r" );
	    send_to_char( buf, ch );
	    if (obj->value[3] == WEAPON_BLASTER )
	    {
		  if (obj->blaster_setting == BLASTER_FULL)
	    		ch_printf( ch, "Está a máxima potencia.\n\r");
	  	  else if (obj->blaster_setting == BLASTER_HIGH)
	    		ch_printf( ch, "está a alta potencia.\n\r");
	  	  else if (obj->blaster_setting == BLASTER_NORMAL)
	    		ch_printf( ch, "Está a potencia normal.\n\r");
	  	  else if (obj->blaster_setting == BLASTER_HALF)
	    		ch_printf( ch, "Está a potencia media.\n\r");
	  	  else if (obj->blaster_setting == BLASTER_LOW)
	    		ch_printf( ch, "Está a baja potencia.\n\r");
	  	  else if (obj->blaster_setting == BLASTER_STUN)
	    		ch_printf( ch, "Está en modo aturdir.\n\r");
	  	  ch_printf( ch, "Tiene entre %d y %d disparos.\n\r", obj->value[4]/5 , obj->value[4] );
	    }
	    else if (     ( obj->value[3] == WEAPON_LIGHTSABER || obj->value[3] == WEAPON_DUAL_LIGHTSABER ||
		            obj->value[3] == WEAPON_VIBRO_BLADE ||
		            obj->value[3] == WEAPON_FORCE_PIKE ) )
	    {
		   ch_printf( ch, "Tiene %d/%d unidades de carga.\n\r", obj->value[4] , obj->value[5] );
	    }        
	    break;

	case ITEM_FOOD:
	    if ( obj->timer > 0 && obj->value[1] > 0 )
	      dam = (obj->timer * 10) / obj->value[1];
	    else
	      dam = 10;
	    strcpy( buf, "Al mirar de cerca te das cuenta de que " );
	    if (dam >= 10) strcat( buf, "es un alimento fresco.");
       else if (dam ==  9) strcat( buf, "es un alimento casi fresco.");
       else if (dam ==  8) strcat( buf, "es un alimento que está en buenas condiciones.");
       else if (dam ==  7) strcat( buf, "es un alimento que parece estar bueno.");
       else if (dam ==  6) strcat( buf, "es un alimento que no tiene mala pinta.");
       else if (dam ==  5) strcat( buf, "es un alimento que está un poco rancio.");
       else if (dam ==  4) strcat( buf, "es un alimento que está algo rancio.");
       else if (dam ==  3) strcat( buf, "es un alimento que huele algo mal.");
       else if (dam ==  2) strcat( buf, "es un alimento que huele mal.");
       else if (dam ==  1) strcat( buf, "es un alimento que te revuelve las tripas.");
       else if (dam <=  0) strcat( buf, "es un alimento que está lleno de gusanos.");
	    strcat( buf, "\n\r" );
	    send_to_char( buf, ch );
	    break;

	case ITEM_SWITCH:
	case ITEM_LEVER:
	case ITEM_PULLCHAIN:
	    if ( IS_SET( obj->value[0], TRIG_UP ) )
		send_to_char( "Te das cuenta que está en la posición de arriba.\n\r", ch );
	    else
		send_to_char( "Te das cuenta que está en la posición de abajo.\n\r", ch );
	    break;
	case ITEM_BUTTON:
	    if ( IS_SET( obj->value[0], TRIG_UP ) )
		send_to_char( "Te das cuenta que alguien lo ha pulsado.\n\r", ch );
	    else
		send_to_char( "Te das cuenta que puede pulsarse.\n\r", ch );
	    break;

/* Not needed due to check in do_look already
	case ITEM_PORTAL:
	    sprintf( buf, "in %s noprog", arg );
	    do_look( ch, buf );
	    break;
*/

        case ITEM_CORPSE_PC:
	case ITEM_CORPSE_NPC:
            {
		sh_int timerfrac = obj->timer;
		if ( obj->item_type == ITEM_CORPSE_PC )
		timerfrac = (int)obj->timer / 8 + 1; 

		switch (timerfrac)
		{
		    default:
			send_to_char( "Este cadáver ha sido asesinado recientemente.\n\r", ch );
			break;
		    case 4:
			send_to_char( "Este cadáver fue asesinado hace un rato.\n\r", ch );
			break;
		    case 3:
			send_to_char( "Un olor nauseabundo sale del cadáver y está cubierto de moscas.\n\r", ch );
			break;
		    case 2:
			send_to_char( "Una masa de gusanos rodea el cadáver.\n\r", ch );
			break;
		    case 1:
		    case 0:
			send_to_char( "De este cadáver no quedan más que huesos.\n\r", ch );
			break;
		}
            }
	    if ( IS_OBJ_STAT( obj, ITEM_COVERING ) )
	      break;
	    send_to_char( "Cuando miras dentro, ves:\n\r", ch );
	    sprintf( buf, "en %s noprog", arg );
	    do_look( ch, buf );
	    break;
	
	case ITEM_DROID_CORPSE:
            {
		sh_int timerfrac = obj->timer;
		
		switch (timerfrac)
		{
		    default:
			send_to_char( "Estos restos echan humo.\n\r", ch );
			break;
		    case 4:
			send_to_char( "Los restos de este droide se han enfriado por completo.\n\r", ch );
			break;
		    case 3:
			send_to_char( "Los restos de este droide se están comenzando a oxidar.\n\r", ch );
			break;
		    case 2:
			send_to_char( "Los restos de este droide están completamente cubiertas de óxido.\n\r", ch );
			break;
		    case 1:
		    case 0:
			send_to_char( "Todo lo que queda de los restos es una pila de metal oxidado que se desmorona.\n\r", ch );
			break;
		}
            }
		
	case ITEM_CONTAINER:
	    if ( IS_OBJ_STAT( obj, ITEM_COVERING ) )
	      break;

	case ITEM_DRINK_CON:
	    send_to_char( "cuando miras dentro ves:\n\r", ch );
	    sprintf( buf, "en %s noprog", arg );
	    do_look( ch, buf );
	}
	if ( IS_OBJ_STAT( obj, ITEM_COVERING ) )
	{
	    sprintf( buf, "bajo %s noprog", arg );
	    do_look( ch, buf );
	}
	oprog_examine_trigger( ch, obj );
	if( char_died(ch) || obj_extracted(obj) )
	  return;

	check_for_trap( ch, obj, TRAP_EXAMINE );
    }
    return;
}


void do_exits( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    EXIT_DATA *pexit;
    bool found;
    bool fAuto;

    set_char_color( AT_EXITS, ch );
    buf[0] = '\0';
    fAuto  = !str_cmp( argument, "auto" );

    if ( !check_blind( ch ) )
	return;

   if( !IS_NPC( ch )
       && !IS_SET( ch->act, PLR_HOLYLIGHT )
       && !IS_AFFECTED( ch, AFF_TRUESIGHT ) && !IS_AFFECTED( ch, AFF_INFRARED ) && room_is_dark( ch->in_room ) )
   {
      set_char_color( AT_DGREY, ch );
      send_to_char( "Está todo oscuro...\r\n", ch );
      return;
   }

    strcpy( buf, fAuto ? "Salidas:" : "\n\rSalidas obvias:\n\r&R--------------&W\n\r" );

    set_char_color( AT_EXITS, ch);
    found = FALSE;
    for ( pexit = ch->in_room->first_exit; pexit; pexit = pexit->next )
    {
	if ( pexit->to_room
	&&  !IS_SET(pexit->exit_info, EX_HIDDEN) )
	{
	    found = TRUE;
	    if ( !fAuto )
	    {	
		if ( IS_SET(pexit->exit_info, EX_CLOSED) )
		{
		    sprintf( buf + strlen(buf), "%s%-5s - (cerrada)&w&W\n\r",
		    color_str(AT_EXITS, ch), capitalize( dir_name2[pexit->vdir] ) );
		}
		else if ( IS_SET(pexit->exit_info, EX_WINDOW) )
		{
		    sprintf( buf + strlen(buf), "%s%-5s - (ventana)&w&W\n\r",
		    color_str(AT_EXITS, ch), capitalize( dir_name2[pexit->vdir] ) );
		}
		else if ( IS_SET(pexit->exit_info, EX_xAUTO) )
		{
		   sprintf( buf + strlen(buf), "%s%-5s - %s%s&w&W\n\r",
		    color_str(AT_EXITS, ch), capitalize( pexit->keyword ), color_str( AT_RMNAME, ch),
		    room_is_dark( pexit->to_room )
			?  "Demasiado oscuro"
			: pexit->to_room->name );
		}
		else
		    sprintf( buf + strlen(buf), "%s%-5s - %s%s&w&W\n\r",
		    color_str(AT_EXITS, ch), capitalize( dir_name2[pexit->vdir] ),color_str( AT_RMNAME, ch),
		    room_is_dark( pexit->to_room )
			?  "Demasiado oscuro"
			: pexit->to_room->name );
	    }
	    else
	    {
	        sprintf( buf + strlen(buf), " %s&w&W",
		    capitalize( dir_name2[pexit->vdir] ) );
	    }
	}
    }

    if ( !found )
	strcat( buf, fAuto ? " ninguna.\n\r" : "Ninguna.\n\r" );
    else
      if ( fAuto )
	strcat( buf, ".\n\r" );
    send_to_char( buf, ch );
    return;
}

char *	const	day_name	[] =
{
    "la Redención", "la Cautela", "la Decepción", "la Rebelión", "la Lucha",
    "el Renacimiento", "la Belleza"
};

char *	const	month_name	[] =
{
    "el emperador", "el Imperio", "el Contrabandista", "la Lágrima",
    "los Soles Gemelos", "el Jedi", "Honoghr", "del Sith", "la Fuerza",
    "el asesino", "el Cazarecompensas", "el Clon", "las Sombras Oscuras",
    "la Luz", "el Massassi", "los Antiguos", "Kashyyyk"
};

void do_time( CHAR_DATA *ch, char *argument )
{
    extern char str_boot_time[];
    extern char reboot_time[];
        int day;

    day     = time_info.day + 1;

    set_char_color( AT_YELLOW, ch );
    ch_printf( ch,
	"%d %s, del día de %s, %d del mes de %s.\n\r"  
        "El mud se inició el     %s\r"
        "La hora del sistema es %s\r"
        "El próximo reinicio será el %s\r",

	(time_info.hour % 12 == 0) ? 12 : time_info.hour % 12,
	time_info.hour >= 12 ? "pm" : "am",
	day_name[day % 7],
	day,
	month_name[time_info.month],
	str_boot_time,
	(char *) ctime( &current_time ),
	reboot_time
	);
    if( sysdata.CLEANPFILES )
	ch_printf( ch, "La siguiente purga de jugadores se hará el %s\n\r", (char *)ctime( &new_pfile_time_t ) );	
	

    return;
}



void do_weather( CHAR_DATA *ch, char *argument )
{
    static char * const sky_look[4] =
    {
	"despejado",
	"nublado",
	"lluvioso",
	"iluminado por rayos"
    };

    if ( !IS_OUTSIDE(ch) )
    {
	send_to_char( "No puedes ver el cielo desde aquí.\n\r", ch );
	return;
    }

    set_char_color( AT_BLUE, ch );
    ch_printf( ch, "El cielo está %s y %s.\n\r",
	sky_look[weather_info.sky],
	weather_info.change >= 0
	? "sopla una brisa cálida del sur"
	: "sopla un viento frío ddesde el norte"
	);
    return;
}


/*
 * Moved into a separate function so it can be used for other things
 * ie: online help editing				-Thoric
 */
HELP_DATA *get_help( CHAR_DATA *ch, char *argument )
{
    char argall[MAX_INPUT_LENGTH];
    char argone[MAX_INPUT_LENGTH];
    char argnew[MAX_INPUT_LENGTH];
    HELP_DATA *pHelp;
    int lev;

    if ( argument[0] == '\0' )
	argument = "summary";

    if ( isdigit(argument[0]) )
    {
	lev = number_argument( argument, argnew );
	argument = argnew;
    }
    else
	lev = -2;
    /*
     * Tricky argument handling so 'help a b' doesn't match a.
     */
    argall[0] = '\0';
    while ( argument[0] != '\0' )
    {
	argument = one_argument( argument, argone );
	if ( argall[0] != '\0' )
	    strcat( argall, " " );
	strcat( argall, argone );
    }

    for ( pHelp = first_help; pHelp; pHelp = pHelp->next )
    {
	if ( pHelp->level > get_trust( ch ) )
	    continue;
	if ( lev != -2 && pHelp->level != lev )
	    continue;

	if ( is_name( argall, pHelp->keyword ) )
	    return pHelp;
    }

    return NULL;
}

sh_int str_similarity( const char *astr, const char *bstr )
{
   sh_int matches=0;

    if (!astr || !bstr)
       return matches;

    for ( ; *astr; astr++)
    {
        if ( LOWER(*astr) == LOWER(*bstr) )
           matches++;
        
        if (++bstr == '\0')
           return matches;                
    }
    
    return matches;
}

void similar_help_files(CHAR_DATA *ch, char *argument)
{
   HELP_DATA *pHelp=NULL;
   char buf[MAX_STRING_LENGTH];
   char *extension;
   sh_int lvl=0;
   bool single=FALSE;
// char *argnew;    

/*
    if ( isdigit(argument[0]) && (index(argument, '.')))
    {
	number_argument( argument, argnew );
	argument = argnew;
    }

*/        
    send_to_pager_color( "&C&BAyudas similares:\n\r", ch);
        
    for ( pHelp = first_help; pHelp; pHelp=pHelp->next)
    {
        buf[0]='\0';      
        extension=pHelp->keyword;
        
        if (pHelp->level > get_trust(ch))
           continue;

        while ( extension[0] != '\0' )
        {
              extension= one_argument(extension, buf);
              
              if ( str_similarity(argument, buf) > lvl)
              {
                 lvl=str_similarity(argument, buf);
                 single=TRUE;
              }        
              else if ( str_similarity(argument, buf) == lvl && lvl > 0)
              {
                 single=FALSE;
              }
        }
    }
        
    if (lvl==0)
    {
       send_to_pager_color( "&C&GNo se encontraron ayudas similares.\n\r", ch);   
       return;
    }

    for ( pHelp = first_help; pHelp; pHelp=pHelp->next)
    {
        buf[0]='\0';      
        extension=pHelp->keyword;

        while ( extension[0] != '\0' )
        {
              extension=one_argument(extension, buf);

              if ( str_similarity(argument, buf) >= lvl
                   && pHelp->level <= get_trust(ch))
              {
                 if (single)
                 {
                    send_to_pager_color( "&C&GMostrando la única ayuda similar.&C\n\r", ch);
                    do_help( ch, buf);
                    return;
                 }

                 pager_printf(ch, "&C&G   %s\n\r", pHelp->keyword);
                 break;

              }

        }
    }
    return;
}


/*
 * Now this is cleaner
 */
void do_help( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *pHelp;
    char nohelp[MSL];
    char buf[MAX_STRING_LENGTH];

    strcpy(nohelp, argument);

    if ( (pHelp = get_help( ch, argument )) == NULL )
    {
	send_to_char( "No hay ayuda sobre esa palabra.\n\r", ch );
	similar_help_files(ch, argument);
        append_file( ch, HELP_FILE, nohelp );
	return;
    }

    if ( pHelp->level >= 0 && str_cmp( argument, "imotd" ) )
    {
	//send_to_pager( pHelp->keyword, ch );
	//send_to_pager( "\n\r", ch );
	sprintf(buf, "&G=-=-=-=-=-=- &R%s &G-=-=-=-=-=-=&W\n\r", pHelp->keyword);
	send_to_pager( buf, ch );
    }

    if ( !IS_NPC(ch) && IS_SET( ch->act , PLR_SOUND ) )
        send_to_pager( "!!SOUND(help)", ch );

    /*
     * Strip leading '.' to allow initial blanks.
     */
     set_char_color( AT_HELP, ch);
    if ( pHelp->text[0] == '.' )
	send_to_pager_color( pHelp->text+1, ch );
    else
	send_to_pager_color( pHelp->text  , ch );
    return;
}

/*
 * Help editor							-Thoric
 */
void do_hedit( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *pHelp;

    if ( !ch->desc )
    {
	send_to_char( "You have no descriptor.\n\r", ch );
	return;
    }

    switch( ch->substate )
    {
	default:
	  break;
	case SUB_HELP_EDIT:
	  if ( (pHelp = ch->dest_buf) == NULL )
	  {
		bug( "hedit: sub_help_edit: NULL ch->dest_buf", 0 );
		stop_editing( ch );
		return;
	  }
	  STRFREE( pHelp->text );
	  pHelp->text = copy_buffer( ch );
	  stop_editing( ch );
	  return;
    }
   if( ( pHelp = get_help( ch, argument ) ) == NULL ) /* new help */
   {
      HELP_DATA *tHelp;
      char argnew[MAX_INPUT_LENGTH];
      int lev;
      bool new_help = TRUE;

      for( tHelp = first_help; tHelp; tHelp = tHelp->next )
         if( !str_cmp( argument, tHelp->keyword ) )
         {
            pHelp = tHelp;
            new_help = FALSE;
            break;
         }
      if( new_help )
      {
         if( isdigit( argument[0] ) )
         {
            lev = number_argument( argument, argnew );
            argument = argnew;
         }
         else
            lev = get_trust( ch );
         CREATE( pHelp, HELP_DATA, 1 );
         pHelp->keyword = STRALLOC( strupper( argument ) );
         pHelp->text = STRALLOC( "" );
         pHelp->level = lev;
         add_help( pHelp );
      }
   }
    ch->substate = SUB_HELP_EDIT;
    ch->dest_buf = pHelp;
    start_editing( ch, pHelp->text );
    editor_desc_printf( ch, "Help topic, keyword '%s', level %d.", pHelp->keyword, pHelp->level );
}

/*
 * Stupid leading space muncher fix				-Thoric
 */
char *help_fix( char *text )
{
    char *fixed;

    if ( !text )
      return "";
    fixed = strip_cr(text);
    if ( fixed[0] == ' ' )
      fixed[0] = '.';
    return fixed;
}

void do_hset( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *pHelp;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Syntax: hset <field> [value] [help page]\n\r",	ch );
	send_to_char( "\n\r",						ch );
	send_to_char( "Field being one of:\n\r",			ch );
	send_to_char( "  level keyword remove save\n\r",		ch );
	return;
    }

    if ( !str_cmp( arg1, "save" ) )
    {
	FILE *fpout;

	log_string_plus( "Saving help.are...", LOG_NORMAL, LEVEL_GREATER );

	rename( "help.are", "help.are.bak" );
	fclose( fpReserve );
	if ( ( fpout = fopen( "help.are", "w" ) ) == NULL )
	{
	   bug( "hset save: fopen", 0 );
	   perror( "help.are" );
	   fpReserve = fopen( NULL_FILE, "r" );
	   return;
	}
    
	fprintf( fpout, "#HELPS\n\n" );
	for ( pHelp = first_help; pHelp; pHelp = pHelp->next )
	    fprintf( fpout, "%d %s~\n%s~\n\n",
			pHelp->level, pHelp->keyword, help_fix(pHelp->text) );

	fprintf( fpout, "0 $~\n\n\n#$\n" );
	fclose( fpout );
	fpReserve = fopen( NULL_FILE, "r" );
	send_to_char( "Saved.\n\r", ch );
	return;
    }
    if ( str_cmp( arg1, "remove" ) )
	argument = one_argument( argument, arg2 );

    if ( (pHelp = get_help( ch, argument )) == NULL )
    {
	send_to_char( "Cannot find help on that subject.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg1, "remove" ) )
    {
	UNLINK( pHelp, first_help, last_help, next, prev );
	STRFREE( pHelp->text );
	STRFREE( pHelp->keyword );
	DISPOSE( pHelp );
	send_to_char( "Removed.\n\r", ch );
	return;
    }
    if( !str_cmp( arg1, "level" ) )
    {
       int lev; 

       if( !is_number(arg2) )
       {
          send_to_char( "Level field must be numeric.\r\n", ch );
          return;
       }

       lev = atoi(arg2);
       if( lev < -1 || lev > get_trust(ch) )
       {
          send_to_char( "You can't set the level to that.\r\n", ch );
          return;
       }
       pHelp->level = lev;
       send_to_char( "Done.\r\n", ch );
       return;
    }
    if ( !str_cmp( arg1, "keyword" ) )
    {
	STRFREE( pHelp->keyword );
	pHelp->keyword = STRALLOC( strupper(arg2) );
	send_to_char( "Done.\n\r", ch );
	return;
    }

    do_hset( ch, "" );
}

/*
 * Show help topics in a level range				-Thoric
 * Idea suggested by Gorog
 * prefix keyword indexing added by Fireblade
 */
void do_hlist( CHAR_DATA *ch, char *argument )
{
    int min, max, minlimit, maxlimit, cnt;
    char arg[MAX_INPUT_LENGTH];
    HELP_DATA *help;
    bool minfound, maxfound;
    char *idx;

    maxlimit = get_trust(ch);
    minlimit = maxlimit >= LEVEL_GREATER ? -1 : 0;
    
    min = minlimit;
    max  = maxlimit;
    
    idx = NULL;
    minfound = FALSE;
    maxfound = FALSE;
    
    for ( argument = one_argument(argument, arg); arg[0] != '\0';
	  argument = one_argument(argument, arg))
    {
	if( !isdigit(arg[0]) )
	{
 	    if ( idx )
	    {
		set_char_color(AT_GREEN, ch);
		ch_printf(ch, "You may only use a single keyword to index the list.\n\r");
		return;
	    }
	    idx = STRALLOC(arg);
    	}
	else
	{
	    if ( !minfound )
	    {
		min = URANGE(minlimit, atoi(arg), maxlimit);
		minfound = TRUE;
	    }
	    else
	    if ( !maxfound )
	    {
		max = URANGE(minlimit, atoi(arg), maxlimit);
		maxfound = TRUE;
	    }
	    else
	    {
		set_char_color(AT_GREEN, ch);
		ch_printf(ch, "Debes usar dos niveles como límite.\n\r");
		return;
	    }
	}
    }
    
    if ( min > max )
    {
	int temp = min;

	min = max;
	max = temp;
    }
    
    set_pager_color( AT_GREEN, ch );
    pager_printf( ch, "Ayudas entre el rango de niveles %d y %d:\n\r\n\r", min, max );
    for ( cnt = 0, help = first_help; help; help = help->next )
	if ( help->level >= min && help->level <= max
	&&  (!idx || nifty_is_name_prefix(idx, help->keyword)) )
	{
	    pager_printf( ch, "  %3d %s\n\r", help->level, help->keyword );
	    ++cnt;
	}
    if ( cnt )
	pager_printf( ch, "\n\r%d encontradas.\n\r", cnt );
    else
	send_to_char( "No se ha encontrado ninguna.\n\r", ch );

    if ( idx )
    	STRFREE(idx);
    
    return;
}


/* 
 * New do_who with WHO REQUEST, clan, race and homepage support.  -Thoric
 *
 * Latest version of do_who eliminates redundant code by using linked lists.
 * Shows imms separately, indicates guest and retired immortals.
 * Narn, Oct/96
 *
 * Uh, new do_who with no clan or race. Heh. -||
 */
void do_who( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char clan_name[MAX_INPUT_LENGTH];
    char invis_str[MAX_INPUT_LENGTH];
    char char_name[MAX_INPUT_LENGTH];
    char extra_title[MAX_STRING_LENGTH];
    char race_text[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    int iRace;
    int iLevelLower;
    int iLevelUpper;
    int nNumber;
    int nMatch;
    int sMatch;
    bool rgfRace[MAX_RACE];
    bool fRaceRestrict;
    bool fImmortalOnly;
    bool fShowHomepage;
    FILE *whoout;
    char mudnamebuffer[MSL];

    /*
    #define WT_IMM    0;
    #define WT_MORTAL 1;
    */

    WHO_DATA *cur_who = NULL;
    WHO_DATA *next_who = NULL;
    WHO_DATA *first_mortal = NULL;
    WHO_DATA *first_newbie = NULL;
    WHO_DATA *first_imm = NULL;

    /*
     * Set default arguments.
     */
    iLevelLower    = 0;
    iLevelUpper    = MAX_LEVEL;
    fRaceRestrict  = FALSE;
    fImmortalOnly  = FALSE;
    fShowHomepage  = FALSE;
    for ( iRace = 0; iRace < MAX_RACE; iRace++ )
	rgfRace[iRace] = FALSE;

    /*
     * Parse arguments.
     */
    nNumber = 0;
    for ( ;; )
    {
	char arg[MAX_STRING_LENGTH];

	argument = one_argument( argument, arg );
	if ( arg[0] == '\0' )
	    break;

	if ( is_number( arg ) )
	{
	    switch ( ++nNumber )
	    {
	    case 1: iLevelLower = atoi( arg ); break;
	    case 2: iLevelUpper = atoi( arg ); break;
	    default:
		return;
		send_to_char( "Solo se permiten dos números.\n\r", ch );
	    }
	}
	else
	{
	    if ( strlen(arg) < 3 )
	    {
		send_to_char( "Especifica un poco más, por favor.\n\r", ch );
		return;
	    }

	    /*
	     * Look for classes to turn on.
	     */
            
	    if ( !str_cmp( arg, "dioses" ) || !str_cmp( arg, "inmortales" ) )
		fImmortalOnly = TRUE;
	    else
	    if ( !str_cmp( arg, "www" ) )
		fShowHomepage = TRUE;

	}
    }

    /*
     * Now find matching chars.
     */
    nMatch = 0;
    sMatch = 0;
    buf[0] = '\0';
 /*   if ( ch )
	set_pager_color( AT_GREEN, ch );
    else */

    if ( fShowHomepage )
      whoout = fopen( WEBWHO_FILE, "w" );
    else
      whoout = fopen( WHO_FILE, "w" );

/* start from last to first to get it in the proper order */
    for ( d = last_descriptor; d; d = d->prev )
    {
	CHAR_DATA *wch;
	char const *race;

	if ( (d->connected != CON_PLAYING && d->connected != CON_EDITING)
	|| ( !can_see( ch, d->character ) && IS_IMMORTAL( d->character ) )
	|| d->original )
	    continue;
	wch   = d->original ? d->original : d->character;
	if ( wch->top_level < iLevelLower
	||   wch->top_level > iLevelUpper 
	|| ( fImmortalOnly  && wch->top_level < LEVEL_IMMORTAL )
	|| ( fRaceRestrict && !rgfRace[wch->race] ))
	    continue;

	nMatch++;

	sMatch++;

/*
   No need.

	if ( fShowHomepage
	&&   wch->pcdata->homepage
	&&   wch->pcdata->homepage[0] != '\0' )
	  sprintf( char_name, %s,"<A HREF=\"%s\">%s</A>",
		show_tilde( wch->pcdata->homepage ), wch->name );
	else
*/
	  strcpy( char_name, "" );
    if((wch->rank == NULL || !str_cmp(wch->rank, "(null)")) && !IS_IMMORTAL(wch)) 
                wch->rank = str_dup("   "); 

	sprintf( race_text, "&G&w%s&G&c &G&w", wch->rank);
	race = race_text;


      if ( !wch->rank || (wch->rank[0] == '\0')  ) 	//If the imm has set thier rank show it instead --Odis ->KeB
        {
			switch ( wch->top_level )
			{
				default: break;
				case 200: race = "el fantasma en la máquina";	break;
				case MAX_LEVEL -  0: race = "&G&z(&G&wAdmin        &G&z)&G&w";	break;
				case MAX_LEVEL -  1: race = "&G&z(&G&wJefe Inmortal&G&z)&G&w";	break;
				case MAX_LEVEL -  2: race = "&G&z(&G&wCoordinador &G&z)&G&w";	break;
				case MAX_LEVEL -  3: race = "&G&z(&G&wConstructor  &G&z)&G&w";	break;
				case MAX_LEVEL -  4: race = "&G&z(&G&wAprendiz     &G&z)&G&w";	break;
				case MAX_LEVEL -  5: race = "&G&z(&G&wRetirado     &G&z)&G&w";	break;
			}
		}

  
        if ( ch && !nifty_is_name(wch->name, remand(wch->pcdata->title)) && ch->top_level > wch->top_level )
          sprintf( extra_title , " [%s]" , wch->name );
        else
          strcpy(extra_title, "");
        
        /*if ( IS_RETIRED( wch ) )
          race = "Retired"; 
        else if ( IS_GUEST( wch ) )
          race = "Guest"; 
	else if ( wch->rank && wch->rank[0] != '\0' )
	  race = wch->rank;
        */

if (ch && ch->pcdata && ch->pcdata->clan && wch->pcdata->clan 
    && (ch->pcdata->clan == wch->pcdata->clan 
    || ch->top_level >= LEVEL_IMMORTAL))  

	{
          CLAN_DATA *pclan = wch->pcdata->clan;
	    
	  strcpy( clan_name, " (" );

	    if ( !str_cmp( wch->name, pclan->leader ) )
              strcat( clan_name, "Líder, " );
            if ( !str_cmp( wch->name, pclan->number1 ) )
              strcat( clan_name, "Primero, " );
            if ( !str_cmp( wch->name, pclan->number2 ) )
              strcat( clan_name, "Segundo, " );

	  strcat( clan_name, pclan->name );
	  strcat( clan_name, ")" );
	}
	else
	  clan_name[0] = '\0';

	
	if ( IS_SET(wch->act, PLR_WIZINVIS) )
	  sprintf( invis_str, "(%d)", wch->pcdata->wizinvis );
	else
	  invis_str[0] = '\0';
	sprintf( buf, "%s   %s%s%s%s %s%s\n%s",
	    race,
	    invis_str,
	    IS_SET(wch->act, PLR_AFK) ? "[AFK]" : "",
	    char_name,
	    wch->pcdata->title,
            extra_title,
	    clan_name,
	    ch ? "\r" : "");

          /*  
           * This is where the old code would display the found player to the ch.
           * What we do instead is put the found data into a linked list
           */ 

          /* First make the structure. */
          CREATE( cur_who, WHO_DATA, 1 );
          cur_who->text = str_dup( buf );
          if ( IS_IMMORTAL( wch ) )
            cur_who->type = WT_IMM;
          else
            cur_who->type = WT_MORTAL;
          
          /* Then put it into the appropriate list. */
          switch ( cur_who->type )
          {
            case WT_MORTAL:
              cur_who->next = first_mortal;
              first_mortal = cur_who;
              break;
            case WT_IMM:
              cur_who->next = first_imm;
              first_imm = cur_who;
              break;
            case WT_NEWBIE:
              cur_who->next = first_newbie;
              first_newbie = cur_who;
              break;
            
          }

    }


    /* Ok, now we have three separate linked lists and what remains is to 
     * display the information and clean up.
     */

     /* Deadly list removed for swr ... now only 2 lists */

    if ( first_newbie )
    {
      if ( !ch )
       {
          sprintf( mudnamebuffer, "&G===&B[ &P%s &B]&G===&B[ &WMortal  &B]&G=====================================================&B[]&W\n\n", sysdata.mud_acronym );
          fprintf( whoout,"%s",htmlcolor(mudnamebuffer) );
       }
          
      else
      {
         sprintf(mudnamebuffer, "\n\r&G===&B[ &P%s &B]&G===&B[ &WMortal  &B]&G=====================================================&B[]&W\n\r\n\r", sysdata.mud_acronym );
         send_to_pager(mudnamebuffer, ch);
      }
   }

    for ( cur_who = first_newbie; cur_who; cur_who = next_who )
    {
      if ( !ch )
        fprintf( whoout, "%s", htmlcolor(cur_who->text) );
      else
        send_to_pager( cur_who->text, ch );
      next_who = cur_who->next;
      DISPOSE( cur_who->text );
      DISPOSE( cur_who ); 
    } 


    if ( first_mortal )
    {
      if ( !ch )
      {
        sprintf(mudnamebuffer, "&z(  &rjugador&z  )&R-_-&r^^-_-^^-&R_-&r^^-_-^^-_-^^-&R_-^^-&r_-^^-_-&R^^&z(=====+ &r%s &z+====&z=)&w\n\n", sysdata.mudname );
        fprintf( whoout,"%s", htmlcolor(mudnamebuffer) );
      }
 
      else
      {
          sprintf(mudnamebuffer,  "\n\r&z(  &rJugador&z  )&R-_-&r^^-_-^^-&R_-&r^^-_-^^-_-^^-&R_-^^-&r_-^^-_-&R^^&z(=====+ &r%s &z+====&z=)&w\n\r\n\r", sysdata.mudname );
         send_to_pager( mudnamebuffer, ch );
      }
    }

    for ( cur_who = first_mortal; cur_who; cur_who = next_who )
    {
      if ( !ch )
        fprintf( whoout, "%s", htmlcolor(cur_who->text) );
      else
        send_to_pager( cur_who->text, ch );
      next_who = cur_who->next;
      DISPOSE( cur_who->text );
      DISPOSE( cur_who ); 
    } 

    if ( first_imm )
    {
      if ( !ch )
	  {
          sprintf(mudnamebuffer, "&z( &rInmortal &z )&R-_&r-^^-_-&R^&r-_-^^-_-^^&R-_-^^&r-_-^^-_-^^-&R_-^^&z(=====+ &r%s &z+====&z=)&w\n\n", sysdata.mudname);
		  fprintf( whoout, "%s", htmlcolor(mudnamebuffer) );
	  }

	  else
	  {
	      sprintf(mudnamebuffer,"\n\r&z( &rInmortal &z )&R-_&r-^^-_-&R^^&r-_-^^-_-^^&R-_-^^&r-_-^^-_-^^-&R_-^^&z(=====+ &r%s &z+====&z=)&w\n\r\n\r", sysdata.mudname);
		  send_to_pager( mudnamebuffer , ch );
	  }
	}

    for ( cur_who = first_imm; cur_who; cur_who = next_who )
    {
      if ( !ch )
        fprintf( whoout,"%s",  htmlcolor(cur_who->text) );
      else
        send_to_pager( cur_who->text, ch );
      next_who = cur_who->next;
      DISPOSE( cur_who->text );
      DISPOSE( cur_who ); 
    } 

    if ( !ch )
    {
//	fprintf( whoout, "%s", htmlcolor("&z( &R%d &rvisible &zplayer%s. &R%d &rtotal&z player%s. )&R-_&r-^^-_-&R^^&r-_-^^-_-^^&R-_-^^&r-_-^^-_-^^-&R^^-\n"),
//		sMatch, sMatch == 1 ? "" : "s", nMatch, nMatch == 1 ? "" : "s" );
	fprintf( whoout, "<div style='position: absolute; color: white; top: 5px; right: 5px;'>%s</div></font>", (char *) ctime( &current_time ));
	fclose( whoout );
	return;
    }
      fclose( whoout );
	ch_printf( ch, "\n\r&z( &R%d &rjugadores &zvisible%s. &R%d &rjugadores&z totale%s. )&R-_&r-^^-_-&R^^&r-_-^^&R-_-^^&r-_-^^-_-^^-&R_-^^&r-_-^^-_-&R^^-\n\r",
		sMatch, sMatch == 1 ? "s" : "s", nMatch, nMatch == 1 ? "s" : "s" );
    return;
}

void do_setrank( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA *vict; 
	char arg1[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];
	bool isleader=FALSE;

	argument = one_argument(argument, arg1);

	if(IS_NPC(ch))
	  return;

	if(argument[0] == '\0' || arg1[0] == '\0')
	{
		send_to_char("&Ruso: setrank <player> <string/none>\n\r", ch);
		return;
	}
        
        if( !ch->pcdata->clan)
        {
                send_to_char("&RYou don't have a clan!\n\r", ch);
                return;
        }

	if(!ch->pcdata->clan && str_cmp(arg1, "self"))
	{
		send_to_char("&RYou cannot set others ranks when you are not in a clan.\n\r",ch);
		return;
	}

	if(str_cmp(ch->pcdata->clan->leader, ch->name) && str_cmp(ch->pcdata->clan->number1, ch->name) && !IS_IMMORTAL(ch))
	{
		send_to_char("&RYou are not in a position to set a rank.\n\r", ch);
		return;
	}
	else
		isleader = TRUE;

	if(( vict = get_char_room(ch, arg1)) == NULL)
	{
		ch_printf(ch, "&RThere isn't %s '%s' here.\n\r", aoran(arg1), arg1);
		return;
	}
	if(IS_NPC(vict))
	{
		send_to_char("&RThat lowly peasant is not worthy of rank.\n\r", ch);
		return;
	}
	if(ch->pcdata->clan != vict->pcdata->clan && !IS_IMMORTAL(ch))
	{
		send_to_char("&RThey aren't in your clan.\n\r", ch);
		return;
	}

	if(ch->pcdata->clan && !isleader && !str_cmp(arg1, "self"))
	{
	    send_to_char("&RYou may only set your own rank if you are a leader or not in a clan.\n\r", ch);
	    return;
	}

	if((ch->pcdata->clan && isleader && !str_cmp(arg1,"self")) || (!ch->pcdata->clan && !str_cmp(arg1, "self")))
	{
	    do_rank(ch, argument);
	    return;
	}

	if(!str_cmp(argument, "none"))
	{
		if(vict->rank) 
                   DISPOSE(vict->rank);
		vict->rank = str_dup("                  ");
		ch_printf(ch, "You have removed %s's rank.\n\r", PERS(vict, ch));
		ch_printf(vict, "%s has removed your rank.\n\r", PERS(ch, vict));
		return;
	}

	if(strlen(argument) > 40 || strlen(remand(argument)) > 19)
	{
	   send_to_char("&RThat rank is too long. Choose one under 40 characters with color codes and under 20 without.\n\r", ch);
	   return;
	}

  smash_tilde( argument );
//  argument = rembg(argument);
  sprintf(buf, "%s", argument);
  vict->rank = str_dup(buf);

  ch_printf(ch, "&wYou have set %s's rank to &w%s&w.\n\r", PERS(vict, ch), argument);
  ch_printf(vict, "&w%s has assigned the rank of '%s&w' to you.\n\r", PERS(ch, vict), argument);

  return;
}

void do_compare( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj1;
    OBJ_DATA *obj2;
    int value1;
    int value2;
    char *msg;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' )
    {
	send_to_char( "¿Comparar qué con qué?\n\r", ch );
	return;
    }

    if ( ( obj1 = get_obj_carry( ch, arg1 ) ) == NULL )
    {
	send_to_char( "No tienes ese objeto.\n\r", ch );
	return;
    }

    if ( arg2[0] == '\0' )
    {
	for ( obj2 = ch->first_carrying; obj2; obj2 = obj2->next_content )
	{
	    if ( obj2->wear_loc != WEAR_NONE
	    &&   can_see_obj( ch, obj2 )
	    &&   obj1->item_type == obj2->item_type
	    && ( obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE) != 0 )
		break;
	}

	if ( !obj2 )
	{
	    send_to_char( "No vistes nada comparable.\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( ( obj2 = get_obj_carry( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "No tienes ese objeto.\n\r", ch );
	    return;
	}
    }

    msg		= NULL;
    value1	= 0;
    value2	= 0;

    if ( obj1 == obj2 )
    {
	msg = "Comparas $p consigo mismo.  parecen iguales.";
    }
    else if ( obj1->item_type != obj2->item_type )
    {
	msg = "No puedes comparar $p y $P.";
    }
    else
    {
	switch ( obj1->item_type )
	{
	default:
	    msg = "No puedes comparar $p y $P.";
	    break;

	case ITEM_ARMOR:
	    value1 = obj1->value[0];
	    value2 = obj2->value[0];
	    break;

	case ITEM_WEAPON:
	    value1 = obj1->value[1] + obj1->value[2];
	    value2 = obj2->value[1] + obj2->value[2];
	    break;
	}
    }

    if ( !msg )
    {
	     if ( value1 == value2 ) msg = "$p y $P parecen iguales.";
	else if ( value1  > value2 ) msg = "$p parece mejor que $P.";
	else                         msg = "$p parece peor que $P.";
    }

    act( AT_PLAIN, msg, ch, obj1, obj2, TO_CHAR );
    return;
}



void do_where( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    DESCRIPTOR_DATA *d;
    bool found;

    if (get_trust(ch) < LEVEL_IMMORTAL)
    {
       send_to_char( "If only life were really that simple...\n\r" , ch);
       return;
    }    

    one_argument( argument, arg );

    set_pager_color( AT_PERSON, ch );
    if ( arg[0] == '\0' )
    {
        if (get_trust(ch) >= LEVEL_IMMORTAL)
           send_to_pager( "Players logged in:\n\r", ch );
        else
           pager_printf( ch, "Players near you in %s:\n\r", ch->in_room->area->name );
	found = FALSE;
	for ( d = first_descriptor; d; d = d->next )
	    if ( (d->connected == CON_PLAYING || d->connected == CON_EDITING )
	    && ( victim = d->character ) != NULL
	    &&   !IS_NPC(victim)
	    &&   victim->in_room
	    &&   (victim->in_room->area == ch->in_room->area || get_trust(ch) >= LEVEL_IMMORTAL )
	    &&   can_see( ch, victim ) )
	    {
		found = TRUE;
		pager_printf( ch, "%-28s %-30s \n\r",
		    victim->name, victim->in_room->name );
	    }
	if ( !found )
	    send_to_char( "None\n\r", ch );
    }
    else
    {
	found = FALSE;
	for ( victim = first_char; victim; victim = victim->next )
	    if ( victim->in_room
	    &&   victim->in_room->area == ch->in_room->area
	    &&   !IS_AFFECTED(victim, AFF_HIDE)
	    &&   !IS_AFFECTED(victim, AFF_SNEAK)
	    &&   can_see( ch, victim )
	    &&   is_name( arg, victim->name ) )
	    {
		found = TRUE;
		pager_printf( ch, "%-28s %s\n\r",
		    PERS(victim, ch), victim->in_room->name );
		break;
	    }
	if ( !found )
	    act( AT_PLAIN, "You didn't find any $T.", ch, NULL, arg, TO_CHAR );
    }

    return;
}




void do_consider( CHAR_DATA *ch, char *argument )
{
  /* New Concider code by Ackbar counts in a little bit more than
     just their hp compared to yours :P Messages from godwars so leave this
     in they deserve some credit :) - Ackbar
  */
  int diff;
  int con_hp;  
  char *msg;
  int overall;
  CHAR_DATA *victim;
  /* It counts your difference's in combat, ac, and hp and then uses
   * a simple enough formula to determine who's most likely to win :).
  */
  
  if((victim = get_char_room(ch, argument)) == NULL)
   {
   	send_to_char("No está aquí.\n\r", ch);
   	return;
   }
  
//  act(AT_WHITE, "$n examines $N closely looking for any weaknesses.", ch, NULL, victim, TO_NOTVICT);
  act(AT_CONSIDER, "Examinas a $N buscando sus debilidades.", ch, NULL, victim, TO_CHAR);
//  act(AT_WHITE, "$n examines you closely looking for weaknesses.", ch, NULL, victim, TO_VICT);
  
  overall = 0;
  con_hp = victim->hit;

  
    diff = ch->hit * 100 / con_hp;
         if ( diff <=  10 ) {msg = "parece mucho más vital que tú."; overall = overall - 3;}
    else if ( diff <=  50 ) {msg = "Parece más vital que tú."; overall = overall - 2;}
    else if ( diff <=  75 ) {msg = "Parece algo más vital que tú."; overall = overall - 1;}
    else if ( diff <= 125 ) {msg = "Teneis más o menos la misma vitalidad.";}
    else if ( diff <= 200 ) {msg = "Pareces ser más vital."; overall = overall + 1;}
    else if ( diff <= 500 ) {msg = "Pareces tener mucha más vitalidad."; overall = overall + 2;}
    else                    {msg = "Tienes mucha más vitalidad."; overall = overall + 3;}
    act(AT_CONSIDER, msg, ch, NULL, victim, TO_CHAR);
    
    diff = victim->armor - ch->armor;
         if ( diff <= -100) {msg = "Su defensa es mucho mejor que la tuya."; overall = overall - 3;}
    else if ( diff <= -50 ) {msg = "Su defensa parece ser mejor que la tuya."; overall = overall - 2;}
    else if ( diff <= -25 ) {msg = "Su defensa parece ser algo mejor que la tuya."; overall = overall - 1;}
    else if ( diff <=  25 ) {msg = "Vuestras defensas parecen iguales.";}
    else if ( diff <=  50 ) {msg = "Pareces tener mejor defensa."; overall = overall + 1;}
    else if ( diff <=  100) {msg = "pareces tener mucha mejor defensa."; overall = overall + 2;}
    else                    {msg = "tienes mucha mejor defensa."; overall = overall + 3;}
    act(AT_CONSIDER, msg, ch, NULL, victim, TO_CHAR);
    
    diff = victim->top_level - ch->top_level + GET_HITROLL(victim) - GET_HITROLL(ch);
         if ( diff <= -35 ) {msg = "Eres mucho más hábil."; overall = overall + 3;}
    else if ( diff <= -15 ) {msg = "No es tan hábil como tú."; overall = overall + 2;}
    else if ( diff <=  -5 ) {msg = "No parece ser tan hábil como tú."; overall = overall + 1;}
    else if ( diff <=   5 ) {msg = "Soys más o menos igual de hábiles.";}
    else if ( diff <=  15 ) {msg = "Parece ser un poco más habil que tú."; overall = overall - 1;}
    else if ( diff <=  35 ) {msg = "Es más hábil que tú."; overall = overall -2;}
    else                    {msg = "Es mucho más habil que tú."; overall = overall - 3;}
    act(AT_CONSIDER, msg, ch, NULL, victim, TO_CHAR);
    
    diff = victim->top_level - ch->top_level + GET_DAMROLL(victim) - GET_DAMROLL(ch);
         if ( diff <= -35 ) {msg = "Eres más fuerte."; overall = overall + 3;}
    else if ( diff <= -15 ) {msg = "No es tan fuerte como tú."; overall = overall + 2;}
    else if ( diff <=  -5 ) {msg = "No parece ser tan fuerte como tú."; overall = overall + 1;}
    else if ( diff <=   5 ) {msg = "Vuestras fuerzas parecen iguales.";}
    else if ( diff <=  15 ) {msg = "Parece ser algo más fuerte que tú."; overall = overall - 1;}
    else if ( diff <=  35 ) {msg = "Es más fuerte que tú."; overall = overall -2;}
    else                    {msg = "Es mucho más fuerte que tú."; overall = overall - 3;}
    act(AT_CONSIDER, msg, ch, NULL, victim, TO_CHAR);
    
    diff = overall;
         if ( diff <= -11 ) {msg = "Conclusión: Te mataría en segundos.";}
    else if ( diff <=  -7 ) {msg = "Conclusión: Necesitas mucha suerte para derrotarle.";}
    else if ( diff <=  -3 ) {msg = "Conclusión: Necesitas algo de suerte para derrotarle.";}
    else if ( diff <=   2 ) {msg = "Conclusión: Tiene pinta de ser una lucha igualada.";}
    else if ( diff <=   6 ) {msg = "Conclusión: No deberías tener muchos problemas para derrotarle.";}
    else if ( diff <=  10 ) {msg = "Conclusión: Eres muchísimo más fuerte";}
    else                    {msg = "Conclusión: No te duraría más de unos segundos.";}
    act(AT_CONSIDER, msg, ch, NULL, victim, TO_CHAR);

    return;
    
}



/*
 * Place any skill types you don't want them to be able to practice
 * normally in this list.  Separate each with a space.
 * (Uses an is_name check). -- Altrag
 */
#define CANT_PRAC "Tongue"

void do_practice( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int sn;

    if ( IS_NPC(ch) )
	return;

    if ( argument[0] == '\0' )
    {
	int	col;
	sh_int	lasttype, cnt;

	col = cnt = 0;	lasttype = SKILL_SPELL;
	
//	set_pager_color( AT_MAGIC, ch );

//		      send_to_pager("&G------------------------------------&B[&WSpells&B]&G------------------------------------\n\r", ch);

	for ( sn = 0; sn < top_sn; sn++ )
	{
	    if ( !skill_table[sn]->name )
		break;

            if ( skill_table[sn]->guild < 0 || skill_table[sn]->guild >= MAX_ABILITY )
                continue;
			if(skill_table[sn]->guild == FORCE_ABILITY)  continue;

	    if ( strcmp(skill_table[sn]->name, "reserved") == 0
	    && ( IS_IMMORTAL(ch) ) )
	    {
		if ( col % 3 != 0 )
		    send_to_pager( "\n\r", ch );
		col = 0;
	    }
	    if ( skill_table[sn]->type != lasttype )
	    {
		if ( !cnt )
		    send_to_pager( "", ch );
		else
		if ( col % 3 != 0 )
		    send_to_pager( "\n\r", ch );
		    if (!str_cmp(skill_tname[skill_table[sn]->type], "weapon"))
		      pager_printf( ch,"&G-----------------------------------&B[&W%ss&B]&G------------------------------------\n\r", skill_tname[skill_table[sn]->type]);
		    else
		      pager_printf( ch,"&G------------------------------------&B[&W%s&B]&G-------------------------------------\n\r",
			 skill_tname[skill_table[sn]->type]);
		col = cnt = 0;
	    }
	    lasttype = skill_table[sn]->type;
            
            if ( skill_table[sn]->guild < 0 || skill_table[sn]->guild >= MAX_ABILITY )
                continue;
                
	    if (  ch->pcdata->learned[sn] <= 0  &&  ch->skill_level[skill_table[sn]->guild] < skill_table[sn]->min_level )
 	        continue;

	    if ( ch->pcdata->learned[sn] == 0
	    &&   SPELL_FLAG(skill_table[sn], SF_SECRETSKILL))
		continue;

	    ++cnt;
	    pager_printf( ch, "&B[&W%3d%%&B] %s%-18.18s ",
		 ch->pcdata->learned[sn],skill_table[sn]->alignment == 0 ? "&G" : skill_table[sn]->alignment == -1001 ? "&R" : "&B",skill_table[sn]->name );
	    if ( ++col % 3 == 0 )
		send_to_pager( "\n\r", ch );
	}

	if ( col % 3 != 0 )
	    send_to_pager( "\n\r", ch );

    }
    else
    {
	CHAR_DATA *mob;
	int adept;
	bool can_prac = TRUE;

	if ( !IS_AWAKE(ch) )
	{
	    send_to_char( "¿En tus sueños?\n\r", ch );
	    return;
	}

	for ( mob = ch->in_room->first_person; mob; mob = mob->next_in_room )
	    if ( IS_NPC(mob) && IS_SET(mob->act, ACT_PRACTICE) )
		break;

	if ( !mob )
	{
	    send_to_char( "No puedes hacer eso aquí.\n\r", ch );
	    return;
	}

	
	sn = skill_lookup( argument );
   
        if ( sn == -1 )
        {
            act( AT_TELL, "&R&C$n dice:&W nunca he oído hablar de eso.",
		mob, NULL, ch, TO_VICT );
	    return;
        }
	
	if ( skill_table[sn]->guild < 0  || skill_table[sn]->guild >= MAX_ABILITY )
        {
	    act( AT_TELL, "&R&C$n dice:&W No puedo enseñarte eso.",
		mob, NULL, ch, TO_VICT );
	    return;
	}
	
	if ( can_prac &&  !IS_NPC(ch)
	&&   ch->skill_level[skill_table[sn]->guild] < skill_table[sn]->min_level )
	{
	    act( AT_TELL, "&R&C$n dice:&W Pienso que te falta para aprender eso todavía.",
		mob, NULL, ch, TO_VICT );
	    return;
	}

	if ( is_name( skill_tname[skill_table[sn]->type], CANT_PRAC ) )
	{
	    act( AT_TELL, "&R&C$n dice:&W No sabría enseñarte eso.",  
		  mob, NULL, ch, TO_VICT );
	    return;
	}

	/*
	 * Skill requires a special teacher
	 */
	if ( skill_table[sn]->teachers && skill_table[sn]->teachers[0] != '\0' )
	{
	    sprintf( buf, "%d", mob->pIndexData->vnum );
	    if ( !is_name( buf, skill_table[sn]->teachers ) )
	    {
		act( AT_TELL, "&R&C$n dice:&W No sabría como enseñarte eso.",
		    mob, NULL, ch, TO_VICT );
		return;
	    }
	}
        else
        {
                act( AT_TELL, "&R&C$n dice:&W No sé como enseñar eso.",
		    mob, NULL, ch, TO_VICT );
		return;
        }
	
	adept = 20;

	if ( ch->pcdata->learned[sn] >= adept )
	{
	    sprintf( buf, "$n te dice, 'Ya sabes más que yo sobre %s.'",
		skill_table[sn]->name );
	    act( AT_TELL, buf, mob, NULL, ch, TO_VICT );
	    act( AT_TELL, "&R&C$n dice:&W Ahora deberás practicar por tu cuenta.",
		mob, NULL, ch, TO_VICT );
	}
	else
	{
	    ch->pcdata->learned[sn] += int_app[get_curr_int(ch)].learn;
	    act( AT_ACTION, "Practicas $T.",
		    ch, NULL, skill_table[sn]->name, TO_CHAR );
	    act( AT_ACTION, "$n practica $T.",
		    ch, NULL, skill_table[sn]->name, TO_ROOM );
	    if ( ch->pcdata->learned[sn] >= adept )
	    {
		ch->pcdata->learned[sn] = adept;
		act( AT_TELL,
		 "&R&C$n dice:&W Ahora deberás practicar por tu cuenta.",
		 mob, NULL, ch, TO_VICT );
	    }
	}
    }
    return;
}

void do_viewskills( CHAR_DATA *ch, char *argument )
{
    int sn;
    CHAR_DATA *victim;
    int	col;
    sh_int lasttype, cnt;

    if ( IS_NPC(ch) )
	return;

    if ( argument[0] == '\0')
    {
	send_to_char("Syntax: viewskills <player>\n\r", ch);
	return;
    }

    if( (victim = get_char_world(ch, argument)) == NULL )
    {
	send_to_char("No such player online.\n\r", ch);
    	return;
    }

    if( IS_NPC(victim))
    {
	send_to_char("You may only view the skills of a player.\n\r", ch);
	return;
    }

	col = cnt = 0;	lasttype = SKILL_SPELL;
	
	set_pager_color( AT_MAGIC, ch );
		      send_to_pager("&G------------------------------------&B[&WSpells&B]&G------------------------------------\n\r", ch);

	for ( sn = 0; sn < top_sn; sn++ )
	{
	    if ( !skill_table[sn]->name )
		break;

            if ( skill_table[sn]->guild < 0 || skill_table[sn]->guild >= MAX_ABILITY )
                continue;

	    if ( strcmp(skill_table[sn]->name, "reserved") == 0
	    && ( IS_IMMORTAL(ch) ) )
	    {
		if ( col % 3 != 0 )
		    send_to_pager( "\n\r", ch );
		col = 0;
	    }
	    if ( skill_table[sn]->type != lasttype )
	    {
		if ( !cnt )
		    send_to_pager( "                                (none)\n\r", ch );
		else
		if ( col % 3 != 0 )
		    send_to_pager( "\n\r", ch );
		    if (!str_cmp(skill_tname[skill_table[sn]->type], "weapon"))
		      pager_printf( ch,"&G-----------------------------------&B[&W%ss&B]&G------------------------------------\n\r", skill_tname[skill_table[sn]->type]);
		    else
		      pager_printf( ch,"&G------------------------------------&B[&W%s&B]&G-------------------------------------\n\r",
			 skill_tname[skill_table[sn]->type]);
		col = cnt = 0;
	    }
	    lasttype = skill_table[sn]->type;
            
            if ( skill_table[sn]->guild < 0 || skill_table[sn]->guild >= MAX_ABILITY )
                continue;
                
	    if (  victim->pcdata->learned[sn] <= 0  &&  victim->skill_level[skill_table[sn]->guild] < skill_table[sn]->min_level )
 	        continue;

	    if ( victim->pcdata->learned[sn] == 0
	    &&   SPELL_FLAG(skill_table[sn], SF_SECRETSKILL) )
		continue;

	    ++cnt;
	    pager_printf( ch, "&R[&W%3d%%&R] %s%-18.18s ",
		 victim->pcdata->learned[sn],skill_table[sn]->alignment == 0 ? "&G" : skill_table[sn]->alignment == -1001 ? "&R" : "&B",skill_table[sn]->name );
	    if ( ++col % 3 == 0 )
		send_to_pager( "\n\r", ch );
	}

	if ( col % 3 != 0 )
	    send_to_pager( "\n\r", ch );

  return;
}

void do_teach( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int sn;
    char arg[MAX_INPUT_LENGTH];
    
    if ( IS_NPC(ch) )
	return;

    argument = one_argument(argument, arg);

    if ( argument[0] == '\0' )
    {
        send_to_char( "¿Enseñar a quién qué?\n\r", ch );
	return;
    }
    else
    {
	CHAR_DATA *victim;
	int adept;

	if ( !IS_AWAKE(ch) )
	{
	    send_to_char( "¿En tus sueños o qué?\n\r", ch );
	    return;
	}
        
        if ( ( victim = get_char_room( ch, arg ) ) == NULL )  
        {
	    send_to_char( "No parece estar aquí...\n\r", ch );
	    return;
	}
                
        if (IS_NPC(victim))
        {
	    send_to_char( "No puedes enseñarle eso!\n\r", ch );
	    return;
	}
        
	sn = skill_lookup( argument );
   
        if ( sn == -1 )
        {
            act( AT_TELL, "No tienes ni idea de eso.",
		victim, NULL, ch, TO_VICT );
	    return;
        }
	
	if ( skill_table[sn]->guild < 0  || skill_table[sn]->guild >= MAX_ABILITY )
        {
	    act( AT_TELL, "Eso no va a suceder.",
		victim, NULL, ch, TO_VICT );
	    return;
	}
	
	if ( victim->skill_level[skill_table[sn]->guild] < skill_table[sn]->min_level )
	{
	    act( AT_TELL, "$n debe aprender algo más antes de que le puedas enseñar eso.",
		victim, NULL, ch, TO_VICT );
	    return;
	}

	if ( is_name( skill_tname[skill_table[sn]->type], CANT_PRAC ) )
	{
	    act( AT_TELL, "No eres capaz de enseñar eso.",  
		  victim, NULL, ch, TO_VICT );
	    return;
	}

	adept = 20;

	if ( victim->pcdata->learned[sn] >= adept )
	{
	    act( AT_TELL, "$n debe practicar ahora por su cuenta.", victim, NULL, ch, TO_VICT );
	    return;
	}
	if ( ch->pcdata->learned[sn] < 100 )
	{
	    act( AT_TELL, "Debes perfeccionar antes lo que quieres enseñar.", victim, NULL, ch, TO_VICT );
	    return;
	}
	else
	{
	    victim->pcdata->learned[sn] += int_app[get_curr_int(ch)].learn;
	    sprintf( buf, "Enseñas a %s $T.", PERS(victim, ch) );
	    act( AT_ACTION, buf,
		    ch, NULL, skill_table[sn]->name, TO_CHAR );
	    sprintf( buf, "%s te eseña $T.", PERS(ch, victim) );
	    act( AT_ACTION, buf,
		    victim, NULL, skill_table[sn]->name, TO_CHAR );
	}
    }
    return;
}

//Deprecated in FotE, but left in in case we decide to use it again.
void do_wimpy( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int wimpy;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
	wimpy = (int) ch->max_hit / 5;
    else
	wimpy = atoi( arg );

    if ( wimpy < 0 )
    {
	send_to_char( "Your courage exceeds your wisdom.\n\r", ch );
	return;
    }

    if ( wimpy > ch->max_hit )
    {
	send_to_char( "Such cowardice ill becomes you.\n\r", ch );
	return;
    }

    ch->wimpy	= wimpy;
    ch_printf( ch, "Wimpy set to %d hit points.\n\r", wimpy );
    return;
}



void do_password( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char *pArg;
    char *pwdnew;
    char *p;
    char cEnd;

    if ( IS_NPC(ch) )
	return;

    /*
     * Can't use one_argument here because it smashes case.
     * So we just steal all its code.  Bleagh.
     */
    pArg = arg1;
    while ( isspace(*argument) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*pArg++ = *argument++;
    }
    *pArg = '\0';

    pArg = arg2;
    while ( isspace(*argument) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*pArg++ = *argument++;
    }
    *pArg = '\0';

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Sintaxis: password <Vieja> <nueva>.\n\r", ch );
	return;
    }

    if ( strcmp( smaug_crypt( arg1 ), ch->pcdata->pwd ) )
    {
	WAIT_STATE( ch, 40 );
	send_to_char( "Contraseña incorrecta. Espera 10 segundos.\n\r", ch );
        log_string( ch->pcdata->pwd );
        log_string( smaug_crypt(arg1) );
	return;
    }

    if ( strlen(arg2) < 5 )
    {
	send_to_char("La nueva contraseña debe tener al menos cinco carácteres.\n\r", ch );
	return;
    }

    /*
     * No tilde allowed because of player file format.
     */
    pwdnew = smaug_crypt( arg2 );
    for ( p = pwdnew; *p != '\0'; p++ )
    {
	if ( *p == '~' )
	{
	    send_to_char("La nueva contraseña no es aceptable, prueba de nuevo.\n\r", ch );
	    return;
	}
    }

    DISPOSE( ch->pcdata->pwd );
    ch->pcdata->pwd = str_dup( pwdnew );
    if ( IS_SET(sysdata.save_flags, SV_PASSCHG) )
	save_char_obj( ch );
    send_to_char( "Contraseña cambiada con éxito.\n\r", ch );
    return;
}



void do_socials( CHAR_DATA *ch, char *argument )
{
    int iHash;
    int col = 0;
    SOCIALTYPE *social;

    set_pager_color( AT_PLAIN, ch );
    for ( iHash = 0; iHash < 27; iHash++ )
	for ( social = social_index[iHash]; social; social = social->next )
	{
	    pager_printf( ch, "%-12s", social->name );
	    if ( ++col % 6 == 0 )
		send_to_pager( "\n\r", ch );
	}

    if ( col % 6 != 0 )
	send_to_pager( "\n\r", ch );
    return;
}


void do_commands( CHAR_DATA *ch, char *argument )
{
    int col;
    bool found;
    int hash;
    CMDTYPE *command;

    col = 0;
    set_pager_color( AT_PLAIN, ch );
    if ( argument[0] == '\0' )
    {
	for ( hash = 0; hash < 126; hash++ )
	    for ( command = command_hash[hash]; command; command = command->next )
		if ( command->level <  LEVEL_HERO
		&&   command->level <= get_trust( ch )
		&&  (command->name[0] != 'm'
		&&   command->name[1] != 'p') )
		{
		    if(IS_IMMORTAL(ch))
		      pager_printf( ch, "[%-2d] %s%-12s&W", command->level, get_help(ch, command->name) ? "" : "&R", command->name );
                    else
                      pager_printf( ch, "[%-2d] %-12s&W", command->level, command->name );
                    
		    if ( ++col % 4 == 0 )
			send_to_pager( "\n\r", ch );
		}
	if ( col % 4 != 0 )
	    send_to_pager( "\n\r", ch );
    }
    else
    {
	found = FALSE;
	for ( hash = 0; hash < 126; hash++ )
	    for ( command = command_hash[hash]; command; command = command->next )
		if ( command->level <  LEVEL_HERO
		&&   command->level <= get_trust( ch )
		&&  !str_prefix(argument, command->name)
		&&  (command->name[0] != 'm'
		&&   command->name[1] != 'p') )
		{
		    if( IS_IMMORTAL(ch))
		      pager_printf( ch, "%s%-12s&W", get_help(ch, command->name)? "" : "&R", command->name );
                    else
                      pager_printf( ch, "%-12s", command->name );

		    found = TRUE;
		    if ( ++col % 4 == 0 )
			send_to_pager( "\n\r", ch );
		}

	if ( col % 4 != 0 )
	    send_to_pager( "\n\r", ch );
	if ( !found )
	    ch_printf( ch, "No command found under %s.\n\r", argument);
    }
    return;
}


void do_channels( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE) )
	{
	    send_to_char( "Te han silenciado.\n\r", ch );
	    return;
	}

	send_to_char( "Canales:", ch );

        if ( get_trust( ch ) > 2 && !NOT_AUTHED( ch ) )
        {
	 send_to_char( !IS_SET(ch->deaf, CHANNEL_AUCTION)
	    ? " +SUBASTAS"
	    : " -subastas",
	    ch );
        }

	send_to_char( !IS_SET(ch->deaf, CHANNEL_CHAT)
	    ? " +CHARLAR"
	    : " -charlar",
	    ch );
        
        send_to_char( !IS_SET(ch->deaf, CHANNEL_OOC)
	    ? " +OOC"
	    : " -ooc",
	    ch );

	if ( !IS_NPC( ch ) && ch->pcdata->clan )
	{
 	    send_to_char( !IS_SET(ch->deaf, CHANNEL_CLAN)
	        ? " +CLAN"
	        : " -clan",
	        ch );
	}
	

	send_to_char( !IS_SET(ch->deaf, CHANNEL_QUEST)
	    ? " +QUEST"
	    : " -quest",
	    ch );

	send_to_char( !IS_SET( ch->deaf, CHANNEL_TELLS )
	    ? " +CONTAR"
	    : " -contar",
	    ch );

        send_to_char( !IS_SET( ch->deaf, CHANNEL_WARTALK )
            ? " +GUERRACHARLAR"
            : " -guerracharlar",
            ch );

	if ( IS_HERO(ch) )
	{
	    send_to_char( !IS_SET(ch->deaf, CHANNEL_AVTALK)
		? " +AVATAR"
		: " -avatar",
		ch );
	}

	if ( IS_IMMORTAL(ch) )
	{
	    send_to_char( !IS_SET(ch->deaf, CHANNEL_IMMTALK)
		? " +IMMTALK"
		: " -immtalk",
		ch );

	    send_to_char( !IS_SET(ch->deaf, CHANNEL_PRAY)
		? " +REZAR"
		: " -rezar",
		ch );
	}

	send_to_char( !IS_SET(ch->deaf, CHANNEL_MUSIC)
	    ? " +CANTAR"
	    : " -cantar",
	    ch );

	send_to_char( !IS_SET(ch->deaf, CHANNEL_ASK)
	    ? " +PREGUNTAR"
	    : " -preguntar",
	    ch );

	send_to_char( !IS_SET(ch->deaf, CHANNEL_SHOUT)
	    ? " +GRITAR"
	    : " -gritar",
	    ch );

	send_to_char( !IS_SET(ch->deaf, CHANNEL_YELL)
	    ? " +CHILLAR"
	    : " -chillar",
	    ch );

	if ( IS_IMMORTAL(ch) )
	{
	    send_to_char( !IS_SET(ch->deaf, CHANNEL_MONITOR)
		? " +MONITOR"
		: " -monitor",
		ch );
	}

	send_to_char( !IS_SET(ch->deaf, CHANNEL_NEWBIE)
	      	? " +NOVATO"
		: " -novato",
		ch );

	send_to_char( !IS_SET(ch->deaf, CHANNEL_SPORTS)
	      	? " +DEPORTES"
		: " -deportes",
		ch );

	send_to_char( !IS_SET(ch->deaf, CHANNEL_HOLONET)
	      	? " +HOLONET"
		: " -HOLONET",
		ch );

	if ( get_trust(ch) >= sysdata.log_level )
	{
	    send_to_char( !IS_SET(ch->deaf, CHANNEL_LOG)
		? " +LOG"
		: " -log",
		ch );

	    send_to_char( !IS_SET(ch->deaf, CHANNEL_BUILD)
		? " +BUILD"
		: " -build",
		ch );

            send_to_char( !IS_SET(ch->deaf, CHANNEL_COMM)
	        ? " +COMM"
	        : " -comm",
	        ch );
	}
	send_to_char( ".\n\r", ch );
    }
    else
    {
	bool fClear;
	bool ClearAll;
	int bit;

        bit=0;
        ClearAll = FALSE;

	     if ( arg[0] == '+' ) fClear = TRUE;
	else if ( arg[0] == '-' ) fClear = FALSE;
	else
	{
	    send_to_char( "Canales -canal o +canal?\n\r", ch );
	    return;
	}

	     if ( !str_cmp( arg+1, "subastas"  ) ) bit = CHANNEL_AUCTION;
	else if ( !str_cmp( arg+1, "charlar"     ) ) bit = CHANNEL_CHAT;
	else if ( !str_cmp( arg+1, "ooc"      ) ) bit = CHANNEL_OOC;
	else if ( !str_cmp( arg+1, "clan"     ) ) bit = CHANNEL_CLAN;
        else if ( !str_cmp( arg+1, "guild"    ) ) bit = CHANNEL_GUILD;  
	else if ( !str_cmp( arg+1, "quest"    ) ) bit = CHANNEL_QUEST;
	else if ( !str_cmp( arg+1, "contar"    ) ) bit = CHANNEL_TELLS;
	else if ( !str_cmp( arg+1, "immtalk"  ) ) bit = CHANNEL_IMMTALK;
	else if ( !str_cmp( arg+1, "log"      ) ) bit = CHANNEL_LOG;
	else if ( !str_cmp( arg+1, "build"    ) ) bit = CHANNEL_BUILD;
	else if ( !str_cmp( arg+1, "rezar"     ) ) bit = CHANNEL_PRAY;
	else if ( !str_cmp( arg+1, "avatar"   ) ) bit = CHANNEL_AVTALK;
	else if ( !str_cmp( arg+1, "monitor"  ) ) bit = CHANNEL_MONITOR;
	else if ( !str_cmp( arg+1, "novato"   ) ) bit = CHANNEL_NEWBIE;
	else if ( !str_cmp( arg+1, "cantar"    ) ) bit = CHANNEL_MUSIC;
	else if ( !str_cmp( arg+1, "preguntar"      ) ) bit = CHANNEL_ASK;
	else if ( !str_cmp( arg+1, "gritar"    ) ) bit = CHANNEL_SHOUT;
	else if ( !str_cmp( arg+1, "chillar"     ) ) bit = CHANNEL_YELL;
	else if ( !str_cmp( arg+1, "comm"     ) ) bit = CHANNEL_COMM;
	else if ( !str_cmp( arg+1, "order"    ) ) bit = CHANNEL_ORDER;
        else if ( !str_cmp( arg+1, "guerracharlar"  ) ) bit = CHANNEL_WARTALK;
        else if ( !str_cmp( arg+1, "deportes"   ) ) bit = CHANNEL_SPORTS;
        else if ( !str_cmp( arg+1, "holonet"   ) ) bit = CHANNEL_HOLONET;
	else if ( !str_cmp( arg+1, "limpiar"      ) ) ClearAll = TRUE;
	else
	{
	    send_to_char( "¿Ajustar o limpiar qué canal?\n\r", ch );
	    return;
	}

	if (( fClear ) && ( ClearAll ))
	{
            REMOVE_BIT (ch->deaf, CHANNEL_AUCTION);
            REMOVE_BIT (ch->deaf, CHANNEL_CHAT);
            REMOVE_BIT (ch->deaf, CHANNEL_QUEST);
       /*     REMOVE_BIT (ch->deaf, CHANNEL_IMMTALK); */
            REMOVE_BIT (ch->deaf, CHANNEL_PRAY);
            REMOVE_BIT (ch->deaf, CHANNEL_MUSIC);
            REMOVE_BIT (ch->deaf, CHANNEL_ASK);
            REMOVE_BIT (ch->deaf, CHANNEL_SHOUT);
            REMOVE_BIT (ch->deaf, CHANNEL_YELL);
            REMOVE_BIT (ch->deaf, CHANNEL_SPORTS);
            REMOVE_BIT (ch->deaf, CHANNEL_HOLONET);

       /*     if (ch->pcdata->clan)
              REMOVE_BIT (ch->deaf, CHANNEL_CLAN);


            if (ch->pcdata->guild)
              REMOVE_BIT (ch->deaf, CHANNEL_GUILD);
       */
            if (ch->top_level >= LEVEL_IMMORTAL)
              REMOVE_BIT (ch->deaf, CHANNEL_AVTALK);
 
	    if (ch->top_level >= sysdata.log_level )
	      REMOVE_BIT (ch->deaf, CHANNEL_COMM);

        } else if ((!fClear) && (ClearAll))
        {
            SET_BIT (ch->deaf, CHANNEL_AUCTION);
            SET_BIT (ch->deaf, CHANNEL_CHAT);
            SET_BIT (ch->deaf, CHANNEL_QUEST);
       /*     SET_BIT (ch->deaf, CHANNEL_IMMTALK); */
            SET_BIT (ch->deaf, CHANNEL_PRAY);
            SET_BIT (ch->deaf, CHANNEL_MUSIC);
            SET_BIT (ch->deaf, CHANNEL_ASK);
            SET_BIT (ch->deaf, CHANNEL_SHOUT);
            SET_BIT (ch->deaf, CHANNEL_YELL);
          
            if (ch->top_level >= LEVEL_IMMORTAL)
              SET_BIT (ch->deaf, CHANNEL_AVTALK);

	    if (ch->top_level >= sysdata.log_level)
	      SET_BIT (ch->deaf, CHANNEL_COMM);

         } else if (fClear)
         {
	    REMOVE_BIT (ch->deaf, bit);
         } else
         {
	    SET_BIT    (ch->deaf, bit);
         }

	  send_to_char( "Canal modificado.\n\r", ch );
    }

    return;
}


/*
 * display WIZLIST file						-Thoric
 */
void do_wizlist( CHAR_DATA *ch, char *argument )
{
    set_pager_color( AT_IMMORT, ch );
    show_file( ch, WIZLIST_FILE );
}

/*
 * Contributed by Grodyn.
 */
void do_config( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    if ( IS_NPC(ch) )
	return;

    one_argument( argument, arg );

    set_char_color( AT_WHITE, ch );
    if ( arg[0] == '\0' )
    {
	send_to_char( "[ Palabra  ] Opción\n\r", ch );

	/*send_to_char(  IS_SET(ch->pcdata->flags, PCFLAG_NORECALL)
	    ? "[+NORECALL ] You fight to the death, link-dead or not.\n\r"
	    : "[-norecall ] You try to recall if fighting link-dead.\n\r"
	    , ch );*/

	send_to_char(  IS_SET(ch->act, PLR_AUTOEXIT)
	    ? "[+AUTOSALIDAS ] Ves las salidas automáticamente.\n\r"
	    : "[-autosalidas ] No ves las salidas de forma automática.\n\r"
	    , ch );

        send_to_char( IS_SET(ch->pcdata->flags, PCFLAG_MAP)
	    ? "[+BRUJULA] Ves la brújula en tus salidas.\n\r"
	    : "[-brujula ] No ves la brújula en las salidas.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_AUTOLOOT)
	    ? "[+AUTOSAQUEO ] Saqueas automáticamente los cuerpos.\n\r"
	    : "[-autosaqueo ] No saqueas los cuerpos automáticamente.\n\r"
	    , ch );

/*	send_to_char(  IS_SET(ch->act, PLR_AUTOSAC)
	    ? "[+AUTOSAC  ] You automatically sacrifice corpses.\n\r"
	    : "[-autosac  ] You don't automatically sacrifice corpses.\n\r"
	    , ch );*/

	send_to_char(  IS_SET(ch->act, PLR_AUTOGOLD)
	    ? "[+AUTOCREDITO ] Repartes los créditos con el grupo automáticamente.\n\r"
	    : "[-autocredito ] No repartes los créditos con los del grupo.\n\r"
	    , ch );

        send_to_char(  IS_SET(ch->pcdata->flags, PCFLAG_GAG)       
            ? "[+GAG      ] Solo ves lo necesario en los combates.\n\r"
            : "[-gag      ] Ves el combate completo.\n\r"
            , ch );

        send_to_char(  IS_SET(ch->pcdata->flags, PCFLAG_PAGERON)
            ? "[+PAGINADOR    ] Tienes paginador.\n\r"
            : "[-paginador    ] No tienes paginador.\n\r"
            , ch );
	
	send_to_char(  IS_SET(ch->act, PLR_BLANK)
	    ? "[+BLANCO    ] Ves una línea blanca después del prompt.\n\r"
	    : "[-blanco    ] No ves una línea en blanco después del prompt.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_BRIEF)
	    ? "[+BREVE    ] Ves descripciones breves.\n\r"
	    : "[-breve    ] Ves descripciones completas.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_COMBINE)
	    ? "[+COMBINADO  ] Ves los objetos en formato combinado.\n\r"
	    : "[-combinado  ] Ves los objetos en formato simple.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->pcdata->flags, PCFLAG_NOINTRO)
	    ? "[+NOINTRO  ] No ves la introducción al entrar al mud.\n\r"
	    : "[-nointro  ] Ves la introducción al entrar al mud.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_PROMPT)
	    ? "[+PROMPT   ] Tienes prompt.\n\r"
	    : "[-prompt   ] No tienes prompt.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_TELNET_GA)
	    ? "[+TELNETGA ] Recibes secuencia de telnet GA.\n\r"
	    : "[-telnetga ] No recibes secuencia de telnet GA.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_ANSI)
	    ? "[+ANSI     ] Juegas con ANSI.\n\r"
	    : "[-ansi     ] No juegas con ANSI.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_SOUND)
	    ? "[+SONIDO     ] Juegas con sonido.\n\r"
	    : "[-sonido     ] No juegas con sonido.\n\r"
	    , ch );


	send_to_char(  IS_SET(ch->act, PLR_SHOVEDRAG)
	      ? "[+EMPUJAR] Permites que te empujen o arrastren.\n\r"
	      : "[-empujar] no permites que te empujen o arrastren.\n\r"
	      , ch );

	if ( IS_IMMORTAL( ch ) )
	  send_to_char(  IS_SET(ch->act, PLR_ROOMVNUM)
	      ? "[+VNUM     ] You can see the VNUM of a room.\n\r"
	      : "[-vnum     ] You do not see the VNUM of a room.\n\r"
	      , ch );

	if ( IS_IMMORTAL( ch) )             /* Added 10/16 by Kuran of SWR */
	  send_to_char( IS_SET(ch->pcdata->flags, PCFLAG_ROOM)
	      ? "[+ROOMFLAGS] You will see room flags.\n\r"
	      : "[-roomflags] You will not see room flags.\n\r"
	      , ch );
	
	send_to_char(  IS_SET(ch->act, PLR_SILENCE)
	    ? "[+SILENCIO  ] Te han silenciado.\n\r"
	    : ""
	    , ch );

	send_to_char( !IS_SET(ch->act, PLR_NO_EMOTE)
	    ? ""
	    : "[-emocion    ] No puedes hacer emociones.\n\r"
	    , ch );

	send_to_char( !IS_SET(ch->act, PLR_NO_TELL)
	    ? ""
	    : "[-contar     ] No puedes usar el canal contar.\n\r"
	    , ch );

	send_to_char( !IS_SET(ch->act, PLR_LITTERBUG)
	    ? ""
	    : "[-condenado  ] Te han condenado. No puedes dejar nada.\n\r"
	    , ch );

    }
    else
    {
	bool fSet;
	int bit = 0;

	     if ( arg[0] == '+' ) fSet = TRUE;
	else if ( arg[0] == '-' ) fSet = FALSE;
	else
	{
	    send_to_char( "Configurar -opción o +opción?\n\r", ch );
	    return;
	}

	     if ( !str_prefix( arg+1, "autosalidas" ) ) bit = PLR_AUTOEXIT;
	else if ( !str_prefix( arg+1, "autosaqueo" ) ) bit = PLR_AUTOLOOT;
	else if ( !str_prefix( arg+1, "autosac"  ) ) bit = PLR_AUTOSAC;
	else if ( !str_prefix( arg+1, "autocredito" ) ) bit = PLR_AUTOGOLD;
	else if ( !str_prefix( arg+1, "blanco"    ) ) bit = PLR_BLANK;
	else if ( !str_prefix( arg+1, "breve"    ) ) bit = PLR_BRIEF;
	else if ( !str_prefix( arg+1, "combinado"  ) ) bit = PLR_COMBINE;
	else if ( !str_prefix( arg+1, "prompt"   ) ) bit = PLR_PROMPT;
	else if ( !str_prefix( arg+1, "telnetga" ) ) bit = PLR_TELNET_GA;
	else if ( !str_prefix( arg+1, "ansi"     ) ) bit = PLR_ANSI;
	else if ( !str_prefix( arg+1, "sonido"      ) ) bit = PLR_SOUND;
	else if ( !str_prefix( arg+1, "nice"     ) ) bit = PLR_NICE;
	else if ( !str_prefix( arg+1, "empujar") ) bit = PLR_SHOVEDRAG;
	else if ( IS_IMMORTAL( ch )
	     &&   !str_prefix( arg+1, "vnum"     ) ) bit = PLR_ROOMVNUM;

	if (bit)
        {
          
	  if ( fSet )
	    SET_BIT    (ch->act, bit);
	  else
	    REMOVE_BIT (ch->act, bit);
	  send_to_char( "Configuración cambiada.\n\r", ch );
          return;
        }
        else
        {
	       if ( !str_prefix( arg+1, "norecall" ) ) bit = PCFLAG_NORECALL;
	  else if ( !str_prefix( arg+1, "nointro"  ) ) bit = PCFLAG_NOINTRO;
          else if ( !str_prefix( arg+1, "gag"      ) ) bit = PCFLAG_GAG; 
          else if ( !str_prefix( arg+1, "compass" ) ) bit = PCFLAG_MAP;
          else if ( !str_prefix( arg+1, "pager"    ) ) bit = PCFLAG_PAGERON;
          else if ( !str_prefix( arg+1, "roomflags") 
                    && (IS_IMMORTAL(ch))) bit = PCFLAG_ROOM;
          else
	  {
	    send_to_char( "¿Configurar qué opción?\n\r", ch );
	    return;
    	  }

          if ( fSet )
	    SET_BIT    (ch->pcdata->flags, bit);
	  else
	    REMOVE_BIT (ch->pcdata->flags, bit);

	  send_to_char( "Configuración cambiada.\n\r", ch );
          return;
        }
    }

    return;
}


void do_credits( CHAR_DATA *ch, char *argument )
{
  do_help( ch, "credits" );
}


extern int top_area;

/*
void do_areas( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea1;
    AREA_DATA *pArea2;
    int iArea;
    int iAreaHalf;

    iAreaHalf = (top_area + 1) / 2;
    pArea1    = first_area;
    pArea2    = first_area;
    for ( iArea = 0; iArea < iAreaHalf; iArea++ )
	pArea2 = pArea2->next;

    for ( iArea = 0; iArea < iAreaHalf; iArea++ )
    {
	ch_printf( ch, "%-39s%-39s\n\r",
	    pArea1->name, pArea2 ? pArea2->name : "" );
	pArea1 = pArea1->next;
	if ( pArea2 )
	    pArea2 = pArea2->next;
    }

    return;
}
*/

/* 
 * New do_areas with soft/hard level ranges 
 */

void do_areas( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;

    set_pager_color( AT_PLAIN, ch );
    send_to_pager("\n\r   Autor     |             Area                     | Recomendado |  Permitido\n\r", ch);
    send_to_pager("-------------+--------------------------------------+-------------+-----------\n\r", ch);

    for ( pArea = first_area; pArea; pArea = pArea->next )
	pager_printf(ch, "%-12s | %-36s | %4d - %-4d | %3d - %-3d \n\r", 
	 	pArea->author, pArea->name, pArea->low_soft_range, 
		pArea->hi_soft_range, pArea->low_hard_range, 
		pArea->hi_hard_range);
    return;
}

void do_afk( CHAR_DATA *ch, char *argument )
{
     if ( IS_NPC(ch) )
     return;
     
     if IS_SET(ch->act, PLR_AFK)
     {
    	REMOVE_BIT(ch->act, PLR_AFK);
	send_to_char( "Ya no estás afk.\n\r", ch );
	act(AT_GREY,"$n ya no está afk.", ch, NULL, NULL, TO_ROOM);
     }
     else
     {
	SET_BIT(ch->act, PLR_AFK);
	send_to_char( "Estás afk.\n\r", ch );
	act(AT_GREY,"$n está afk.", ch, NULL, NULL, TO_ROOM);
	return;
     }
         
}

void do_slist( CHAR_DATA *ch, char *argument )
{
   int sn, i, lFound;
   char skn[MAX_INPUT_LENGTH];
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH]; 
   int lowlev, hilev;
   int col = 0;
   int ability;
     
   if ( IS_NPC(ch) )
     return;

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   lowlev=1;
   hilev=100;

   if (arg1[0]!='\0')
      lowlev=atoi(arg1);

   if ((lowlev<1) || (lowlev>LEVEL_IMMORTAL))
      lowlev=1;

   if (arg2[0]!='\0')
      hilev=atoi(arg2);

/*   if ((hilev<0) || (hilev>=LEVEL_IMMORTAL))
      hilev=LEVEL_HERO;
*/
   if( hilev > 100 )
      hilev=100;

   if(lowlev>hilev)
      lowlev=hilev;

   set_pager_color( AT_MAGIC, ch );
   send_to_pager("&G&WLista de habilidades\n\r",ch);
   send_to_pager("&W&B------------------\n\r",ch);

  for ( ability = 0 ; ability < MAX_ABILITY ; ability++ )
  {
   if ( ability == FORCE_ABILITY )
      continue;
  
   if ( ability >= 0 )
      sprintf(skn, "\n\r&G&W%s%s\n\r", color_str( AT_SLIST, ch), ability_name[ability] );
   else
      sprintf(skn, "\n\r&G&W%sGeneral Skills\n\r", color_str(AT_SLIST, ch) );
      
   send_to_pager(skn,ch);
   for (i=lowlev; i <= hilev; i++)
   {
	lFound= 0;
	for ( sn = 0; sn < top_sn; sn++ )
	{
	    if ( !skill_table[sn]->name )
		break;
            
            if ( skill_table[sn]->guild != ability)
		continue;

	    if ( ch->pcdata->learned[sn] == 0
	    &&   SPELL_FLAG(skill_table[sn], SF_SECRETSKILL) )
		continue;
	
	    if(i==skill_table[sn]->min_level  )
	    {
	        pager_printf(ch, "&B(&W%2d&B)%s %s%-18.18s&W  ",
                       i, skill_table[sn]->alignment >= 1001 ? "&B" :
		         skill_table[sn]->alignment <= -1001 ? "&R" : 
			 "&W", color_str(AT_SLIST, ch),skill_table[sn]->name );

                if ( ++col == 3 )
                {
                  pager_printf(ch, "\n\r");
	          col = 0;
	        }
	    }
	}
   }
   if ( col != 0 )
   {
                  pager_printf(ch, "\n\r");
	          col = 0;   
   }
  }
  return;
}

void do_whois( CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];

  buf[0] = '\0';

  if(IS_NPC(ch))
    return;

  if(argument[0] == '\0')
  {
    send_to_char("Debes introducir el nombre de un jugador conectado.\n\r", ch);
    return;
  }

  strcat(buf, "0.");
  strcat(buf, argument);
  if( ( ( victim = get_char_world_ooc(ch, buf) ) == NULL ))
  {
    send_to_char("No está en línea. Recuerda, debes introducir el nombre verdadero de un jugador.\n\r", ch);
    return;
  }

  if(IS_NPC(victim))
  {
    send_to_char("¡Eso no es un jugador!\n\r", ch);
    return;
  }

  if(strlen(argument) != strlen(victim->name))
  {
    send_to_char("No está en línea. Recuerda, debes introducir el nombre verdadero de un jugador.\n\r", ch);
    return;
  }

  if(!IS_IMMORTAL(ch) && IS_SET(victim->act, PLR_WHOINVIS) )
  {
    send_to_char("No está en línea. Recuerda, debes introducir el nombre verdadero de un jugador.\n\r", ch);
    return;
  }

  ch_printf(ch, "%s es un %s %s",
	victim->name,
	victim->sex == SEX_MALE ? "varón" : 
	victim->sex == SEX_FEMALE ? "Hembra" : "neutral",
	npc_race[victim->race]);
  if (IS_IMMORTAL(ch))
    ch_printf(ch, " in room %d.\n\r",
	victim->in_room->vnum);
  else
    ch_printf(ch, ".\n\r");

/*
  if ( victim->pcdata->clan )
  {
	if ( victim->pcdata->clan->clan_type == CLAN_CRIME )
	   send_to_char( ", and belongs to the crime family ", ch );
	else if ( victim->pcdata->clan->clan_type == CLAN_GUILD )
	   send_to_char( ", and belongs to the guild ", ch );
	else
	   send_to_char( ", and belongs to organization ", ch );
	send_to_char( victim->pcdata->clan->name, ch );
  }
  send_to_char( ".\n\r", ch );
*/

  if(victim->pcdata->homepage && victim->pcdata->homepage[0] != '\0')
    ch_printf(ch, "%s tiene una página web: %s.\n\r", 
	victim->name,
	victim->pcdata->homepage);

  if(victim->pcdata->bio && victim->pcdata->bio[0] != '\0')
    ch_printf(ch, "Historia de %s:\n\r%s",
	victim->name,
	victim->pcdata->bio);

  if(IS_IMMORTAL(ch))
  {  
    send_to_char("----------------------------------------------------\n\r", ch);

    send_to_char("Info for immortals:\n\r", ch);

    if ( victim->pcdata->authed_by && victim->pcdata->authed_by[0] != '\0' )
	ch_printf(ch, "%s was authorized by %s.\n\r",
		victim->name, victim->pcdata->authed_by);

    ch_printf(ch, "%s has killed %d mobiles, and been killed by a mobile %d times.\n\r",
		victim->name, victim->pcdata->mkills, victim->pcdata->mdeaths );
    if ( victim->pcdata->pkills || victim->pcdata->pdeaths )
	ch_printf(ch, "%s has killed %d players, and been killed by a player %d times.\n\r",
		victim->name, victim->pcdata->pkills, victim->pcdata->pdeaths );
    if ( victim->pcdata->illegal_pk )
	ch_printf(ch, "%s has committed %d illegal player kills.\n\r",
		victim->name, victim->pcdata->illegal_pk );

    ch_printf(ch, "%s is %shelled at the moment.\n\r",
	victim->name, 
	(victim->pcdata->release_date == 0) ? "not " : "");

    if(victim->pcdata->release_date != 0)
      ch_printf(ch, "%s was helled by %s, and will be released on %24.24s.\n\r",
	victim->sex == SEX_MALE ? "He" :
	victim->sex == SEX_FEMALE ? "She" : "It",
        victim->pcdata->helled_by,
	ctime(&victim->pcdata->release_date));

    if(get_trust(victim) < get_trust(ch))
    {
      sprintf(buf2, "list %s", buf);
      do_comment(ch, buf2);
    }

    if(IS_SET(victim->act, PLR_SILENCE) || IS_SET(victim->act, PLR_NO_EMOTE) 
    || IS_SET(victim->act, PLR_NO_TELL) )
    {
      SCOPY(buf2, "This player has the following flags set:");
      if(IS_SET(victim->act, PLR_SILENCE)) 
        SCAT(buf2, " silence");
      if(IS_SET(victim->act, PLR_NO_EMOTE)) 
        SCAT(buf2, " noemote");
      if(IS_SET(victim->act, PLR_NO_TELL) )
        SCAT(buf2, " notell");
      SCAT(buf2, ".\n\r");
      send_to_char(buf2, ch);
    }
    if ( victim->desc && victim->desc->host[0]!='\0' )   /* added by Gorog */
    {
      sprintf (buf2, "%s's IP info: %s ", victim->name, victim->desc->hostip);
      if (get_trust(ch) >= LEVEL_GOD)
      {
        SCAT (buf2, victim->desc->user);
        SCAT (buf2, "@");
        SCAT (buf2, victim->desc->host);
      }
      SCAT (buf2, "\n\r");
      send_to_char(buf2, ch);
    }
    if (get_trust(ch) >= LEVEL_GOD && get_trust(ch) >= get_trust( victim ) && victim->pcdata )
    {
        sprintf (buf2, "Email: %s\n\r" , victim->pcdata->email );
        send_to_char(buf2, ch);
    }
  }
}

void do_pager( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  
  if ( IS_NPC(ch) )
    return;
  argument = one_argument(argument, arg);
  if ( !*arg )
  {
    if ( IS_SET(ch->pcdata->flags, PCFLAG_PAGERON) )
      do_config(ch, "-pager");
    else
      do_config(ch, "+pager");
    return;
  }
  if ( !is_number(arg) )
  {
    send_to_char( "¿A cuántas líneas?\n\r", ch );
    return;
  }
  ch->pcdata->pagerlen = atoi(arg);
  if ( ch->pcdata->pagerlen < 5 )
    ch->pcdata->pagerlen = 5;
  ch_printf( ch, "Pausa extablecida a %d líneas.\n\r", ch->pcdata->pagerlen );
  return;
}

bool is_online( char * argument )
{
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
       return FALSE;
    
    for ( d = last_descriptor; d; d = d->prev )
    {
	CHAR_DATA *wch;

	if ( (d->connected != CON_PLAYING && d->connected != CON_EDITING)
	|| d->original)
	    continue;
	wch   = d->original ? d->original : d->character;
    
        if ( !str_cmp( argument , wch->name ) )
           return TRUE;
    }
    
    return FALSE;
    
}

void do_whoinvis( CHAR_DATA *ch, char *argument )
{
     if ( IS_NPC(ch) )
     return;
     
     if IS_SET(ch->act, PLR_WHOINVIS)
     {
    	REMOVE_BIT(ch->act, PLR_WHOINVIS);
	send_to_char( "Ahora apareces en el quien.\n\r", ch );
     }
     else
     {
	SET_BIT(ch->act, PLR_WHOINVIS);
	send_to_char( "Ya no apareces en el quien.\n\r", ch );
	return;
     }
         
}

void do_introduce( CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *victim;
   CHAR_DATA *rch;
   char arg1[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   FELLOW_DATA *vfellow;
   FELLOW_DATA *fellow;
   int count;

   argument = one_argument( argument , arg1 );
   argument = remand(argument);

   if(IS_NPC(ch))
     return;

   if(arg1[0] =='\0' || argument[0] == '\0' || (!isalpha(argument[0]) && !isdigit(argument[0])))
   {
     send_to_char("&RSintaxis: presentarse <persona/todos> <Tu nombre preferido>\n\r", ch);
     return;
   }

   arg1[0] = UPPER(arg1[0]);
   argument[0] = UPPER(argument[0]);

   if ((str_cmp(arg1, "todos")) && (victim = get_char_room(ch, arg1)) == NULL)
   {
     send_to_char("No está aquí.\n\r", ch);
     return;
   }

   if ( ch == victim)
   {
     send_to_char("¿esquizofrenia, eh?\n\r", ch);
     return;
   }

   if (ch->pcdata->disguise[0] != '\0')
   {
     send_to_char("¿No deberías quitarte el disfraz primero?\n\r", ch);
     return;
   }

   if (strlen(argument) < 3)
   {
     send_to_char("es difícil tener un nombre tan corto.\n\r", ch);
     return;
   }

   if(strlen(argument) > 40)
	argument[40] = '\0';

   if(argument[1] == '.')
        argument[1] = 'x';

   if(!str_cmp(argument, "Alguien"))
   {
     send_to_char("buen intento.\n\r" ,ch);
     return;
   }

      if(!str_cmp(arg1, "todos"))
      {
        count = 0;
        for ( rch = ch->in_room->first_person; rch; rch = rch->next_in_room )
        {
	  if(IS_NPC(rch) || rch == ch)
	     continue;

          if(can_see(ch, rch))
          {
            sprintf(buf, "__%s ", rch->name);
	    strcat(buf, argument);
	        do_introduce(ch, buf);
            count++;
          }
        }
        if(count == 0)
        {
         send_to_char("No hay nadie aquí.\n\r", ch);
		 return;
        }
       return;
      }


   if (IS_NPC(victim) || IS_NPC(ch) )
   {
     send_to_char("No con mobs.\n\r", ch);
     return;
   }
     
 smash_tilde( argument );

     for(vfellow=victim->first_fellow; vfellow; vfellow=vfellow->next)
     {
      if(!str_cmp(vfellow->victim, ch->name))
      {
		if(!str_cmp(vfellow->knownas,argument))
		{
		 ch_printf(ch,"Ahora te conocen como %s.\n\r", argument);
		 return;
		}

        ch_printf(ch, "&GTe vuelves a presentar a %s como %s.\n\r", PERS(victim, ch), argument);
        ch_printf(victim, "&G%s se vuelve a presentar como %s.\n\r", PERS(ch, victim),
		   argument);

		STRFREE(vfellow->knownas);
       	vfellow->knownas = STRALLOC(argument);
	    return;
      }
    }
      ch_printf(victim, "&G%s se presenta como %s.\n\r", PERS(ch, victim), argument);

       CREATE(fellow, FELLOW_DATA, 1);
       fellow->victim = STRALLOC(ch->name);
       fellow->knownas = STRALLOC(argument);
       LINK(fellow, victim->first_fellow, victim->last_fellow,next, prev);

       ch_printf(ch, "&GTe presentas a %s como %s.\n\r", PERS(victim, ch), argument);
       return;


}

void do_remember( CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *victim;
   char arg1[MAX_INPUT_LENGTH];
   FELLOW_DATA *fellow;
   FELLOW_DATA *nfellow;

   argument = one_argument( argument , arg1 );
   argument = remand(argument);

   if(IS_NPC(ch))
     return;

   if(arg1[0] =='\0' || argument[0] == '\0')
   {
     send_to_char("&RSintaxis: recordar <persona> <nombre>\n\r", ch);
     return;
   }

   arg1[0] = UPPER(arg1[0]);
   argument[0] = UPPER(argument[0]);

   if ((victim = get_char_room(ch, arg1)) == NULL)
   {
     send_to_char("No está aquí.\n\r", ch);
     return;
   }

   if ( ch == victim)
   {
     send_to_char("¿Recordar quién eres? ¿Te atreverías a rolear una pequeña pérdida de memoria?\n\r", ch);
     return;
   }

   if (strlen(argument) < 3)
   {
     send_to_char("encuentras algo complicado recordar un nombre tan corto.\n\r", ch);
     return;
   }

   if(strlen(argument) > 40)
	argument[40] = '\0';

   if(argument[1] == '.')
        argument[1] = 'x';

   if(!str_cmp(argument, "Alguien"))
   {
     send_to_char("¿Puedes ser algo más descriptivo?\n\r", ch);
     return;
   }

   if (IS_NPC(victim) || IS_NPC(ch) )
   {
     send_to_char("Bah. Es un mob.\n\r", ch);
     return;
   }
     
   smash_tilde( argument );

   for(fellow=ch->first_fellow; fellow; fellow=fellow->next)
   {
    if(!str_cmp(fellow->victim, victim->name))
    {
	if(!str_cmp(fellow->knownas,argument))
	{
	 ch_printf(ch,"Ya le conoces como %s.\n\r", argument);
	 return;
	}

	ch_printf(ch, "Recordarás a %s como %s.\n\r", PERS(victim, ch), argument);

	STRFREE(fellow->knownas);
       	fellow->knownas = STRALLOC(argument);
	return;
      }
    }
       ch_printf(ch, "Recordarás a %s como %s.\n\r", PERS(victim, ch), argument);

       CREATE(nfellow, FELLOW_DATA, 1);
       nfellow->victim = STRALLOC(victim->name);
       nfellow->knownas = STRALLOC(argument);
       LINK(nfellow, ch->first_fellow, ch->last_fellow,next, prev);
       return;

}

void do_describe( CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *victim;
   char arg1[MAX_INPUT_LENGTH];
   FELLOW_DATA *fellow;
   FELLOW_DATA *nfellow;
   FELLOW_DATA *vfellow;

   argument = one_argument( argument , arg1 );
   argument = remand(argument);

   if(IS_NPC(ch))
     return;

   if(arg1[0] =='\0' || argument[0] == '\0')
   {
     send_to_char("&RSintaxis: describir <a persona> <persona que quieres describir>\n\r", ch);
     return;
   }

   arg1[0] = UPPER(arg1[0]);
   argument[0] = UPPER(argument[0]);

   if ((victim = get_char_room(ch, arg1)) == NULL)
   {
     send_to_char("No está aquí.\n\r", ch);
     return;
   }

   if ( ch == victim)
   {
     send_to_char("No puedes describirte a alguien.\n\r", ch);
     return;
   }

   if (strlen(argument) < 3)
   {
     send_to_char("Encuentras difícil describir a alguien con un nombre tan corto.\n\r", ch);
     return;
   }

   if(strlen(argument) > 40)
	argument[40] = '\0';

   if(argument[1] == '.')
        argument[1] = 'x';

   if(!str_cmp(argument, "Alguien"))
   {
     send_to_char("¿Puedes ser algo más descriptivo?\n\r", ch);
     return;
   }

   if (IS_NPC(victim) || IS_NPC(ch) )
   {
     send_to_char("Bah. Es un mob.\n\r", ch);
     return;
   }
     
   smash_tilde( argument );

   for(fellow=ch->first_fellow; fellow; fellow=fellow->next)
   {
    if(nifty_is_name(argument, fellow->knownas))
    {
	for(vfellow=victim->first_fellow; vfellow; vfellow=vfellow->next)
	{
	  if(!str_cmp(fellow->victim, vfellow->victim))
	   {
		if(!str_cmp(fellow->knownas,vfellow->knownas))
		{
		 ch_printf(ch,"ya le conoce como %s.\n\r", fellow->knownas);
		 return;
		}
		ch_printf(victim, "%s te describe a %s. Te suena similar a %s.n\r", PERS(ch, victim), fellow->knownas, vfellow->knownas);
		ch_printf(ch, "Describes a %s a %s.n\r", fellow->knownas, PERS(victim, ch));
		STRFREE(vfellow->knownas);
	       	vfellow->knownas = STRALLOC(fellow->knownas);
		return;
	   }    
	}
       ch_printf(ch, "Describes a %s a %s.\n\r", fellow->knownas, PERS(victim, ch));
       ch_printf(victim, "%s te describe a %s.\n\r", PERS(ch, victim), fellow->knownas);
       CREATE(nfellow, FELLOW_DATA, 1);
       nfellow->victim = STRALLOC(fellow->victim);
       nfellow->knownas = STRALLOC(fellow->knownas);
       LINK(nfellow, victim->first_fellow, victim->last_fellow,next, prev);
       return;
    }
   }
   send_to_char("No conoces a nadie así...\n\r", ch);
   return;
}


void do_newexits( CHAR_DATA *ch, char *argument ) 
{
        char buf[MAX_STRING_LENGTH];
	char line0[MAX_STRING_LENGTH];
	char line1[MAX_STRING_LENGTH];
	char line2[MAX_STRING_LENGTH];
	char line3[MAX_STRING_LENGTH];
	char line4[MAX_STRING_LENGTH];
	char line5[MAX_STRING_LENGTH];
	char line6[MAX_STRING_LENGTH];
	char line7[MAX_STRING_LENGTH];
	char line8[MAX_STRING_LENGTH];
	char line9[MAX_STRING_LENGTH];
        EXIT_DATA *pexit;
        bool found, nfound, efound, sfound, wfound, ufound, dfound, nefound, nwfound, sefound, swfound;
        bool fAuto;
	int count;
    
    set_char_color( AT_EXITS, ch );
    buf[0] = line0[0] = line1[0] = line2[0] = line3[0] = line4[0]
	= line5[0] = line6[0] = line7[0] = line8[0] = line9[0] = '\0';
    fAuto = !str_cmp( argument, "auto" );
    if ( !check_blind( ch ) )
	return;
    found = nfound = efound = sfound = wfound = ufound = dfound = nefound = nwfound = sefound = swfound =FALSE;
	count = 0;
    for ( pexit = ch->in_room->first_exit; pexit; pexit = pexit->next )
    {
	if ( pexit->to_room
	&& !IS_SET(pexit->exit_info, EX_HIDDEN) )
	{
	    found = TRUE;
		count++;
		buf[0] = '\0';
	    if ( !fAuto )
	    {
		if ( IS_SET(pexit->exit_info, EX_CLOSED) )
		{
		    sprintf( buf + strlen(buf), "&G&W%-9s &R- (closed)",
		    capitalize( dir_name[pexit->vdir] ) );
		}
		else if ( IS_SET(pexit->exit_info, EX_WINDOW) )
		{
		    sprintf( buf + strlen(buf), "&G&W%-9s &R- (window)",
		    capitalize( dir_name[pexit->vdir] ) );
		}
		else if ( IS_SET(pexit->exit_info, EX_xAUTO) )
		{
		   sprintf( buf + strlen(buf), "&G&W%-9s &R- &G&W%s%s",
		    capitalize( pexit->keyword ), color_str( AT_RMNAME, ch),
		    room_is_dark( pexit->to_room )
			?  "Too dark to tell"
			: pexit->to_room->name );
		}
		else
		    sprintf( buf + strlen(buf), "&G&W%-9s &R- &G&W%s%s",
		    capitalize( dir_name[pexit->vdir] ), color_str( AT_RMNAME, ch),
		    room_is_dark( pexit->to_room )
			?  "Too dark to tell"
			: pexit->to_room->name );
	    }
	    else
	    {
	        sprintf( buf + strlen(buf), "&R&W %s",
		    capitalize( dir_name[pexit->vdir] ) );
	    }
		if(pexit->vdir == 0) nfound=TRUE;
		if(pexit->vdir == 1) efound=TRUE;
		if(pexit->vdir == 2)  sfound=TRUE;
		if(pexit->vdir == 3)  wfound=TRUE;
		if(pexit->vdir == 4) ufound=TRUE;
		if(pexit->vdir == 5) dfound=TRUE;
		if(pexit->vdir == 6) nefound=TRUE;
		if(pexit->vdir == 7) nwfound=TRUE;
		if(pexit->vdir == 8) sefound=TRUE;
		if(pexit->vdir == 9) swfound=TRUE;
		if(count==1) strcpy(line0, buf);
		if(count==2) strcpy(line1, buf);
		if(count==3) strcpy(line2, buf);
		if(count==4) strcpy(line3, buf);
		if(count==5) strcpy(line4, buf);
		if(count==6) strcpy(line5, buf);
		if(count==7) strcpy(line6, buf);
		if(count==8) strcpy(line7, buf);
		if(count==9) strcpy(line8, buf);
		if(count==10) strcpy(line9, buf);
	}
    }//end for
    if ( !found )
	strcat( line3, "&R&WThere are no obvious exits from here." );
	
	// compass. |noma|
	sprintf(buf, "\n\r       %s%-9.9s %s\n\r" 
		  "    %s%2.2s &G&z| %s%-7.7s %s\n\r"
		  "      &G&z\\|/        %s\n\r" 
		  " %s%1.1s&G&z--%s%1.1s&G&z--&R&Wo&G&z--%s%1.1s&G&z--%s%-3.3s %s\n\r" 
		  "      &G&z/|\\        %s\n\r"
		  "    %s%2.2s &G&z| %s%-7.7s %s\n\r"
		  "       %s%-9.9s %s\n\r",
						nfound == TRUE ? "&R" : "&z", nfound == TRUE ? "N" : "*", line0[0] == '\0' ? "" : line0,
						nwfound == TRUE ? "&R" : "&z", nwfound == TRUE ? "NW" : " *",
						nefound == TRUE ? "&R" : "&z", nefound == TRUE ? "NE" : "* ", line1[0] == '\0' ? "" : line1,
            					line2[0] == '\0' ? "" : line2,
						wfound == TRUE ? "&R" : "&z", wfound == TRUE ? "W" : "*",
						ufound == TRUE ? "&R" : "&z", ufound == TRUE ? "U" : "*",
						dfound == TRUE ? "&R" : "&z", dfound == TRUE ? "D" : "*",
						efound == TRUE ? "&R" : "&z", efound == TRUE ? "E" : "*", line3[0] == '\0' ? "" : line3,
						line4[0] == '\0' ? "" : line4,
						swfound == TRUE ? "&R" : "&z", swfound == TRUE ? "SW" : " *",
						sefound == TRUE ? "&R" : "&z", sefound == TRUE ? "SE" : "* ", line5[0] == '\0' ? "" : line5,
						sfound == TRUE ? "&R" : "&z", sfound == TRUE ? "S" : "*", line6[0] == '\0' ? "" : line6);
	ch_printf(ch, buf);
	if(line7[0] != '\0') ch_printf(ch, "  %s\n\r", line7);
	if(line8[0] != '\0') ch_printf(ch, "  %s\n\r", line8);
	if(line9[0] != '\0') ch_printf(ch, "  %s\n\r", line9);
	ch_printf(ch, "\n\r");
    return;
}
