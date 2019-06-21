 /***********************************************************************************
 *                                                                                  *
 *          _______ ____    __    ____       _______                 _______        *
 *         /       )\   \  /  \  /   /  _   |   ____)         __    |   ____)       *
 *        (   (----' \   \/    \/   /  (_)  |  |__    ___   _/  |_  |  |__          *
 *         \   \      \            /    _   |   __)  / _ \ (_   __) |   __)         *
 *     .----)   )      \    /\    /    (_)  |  |    ( (_) )  |  |   |  |____        *
 *    (________/        \__/  \__/          |__|     \___/   |__|   |_______)       *
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
#include "mud.h"

/*
 *  Externals
 */
void send_obj_page_to_char(CHAR_DATA * ch, OBJ_INDEX_DATA * idx, char page);
void send_room_page_to_char(CHAR_DATA * ch, ROOM_INDEX_DATA * idx, char page);
void send_page_to_char(CHAR_DATA * ch, MOB_INDEX_DATA * idx, char page);
void send_control_page_to_char(CHAR_DATA * ch, char page);
void sportschan(char *);

/*
 * Local functions.
 */
void	talk_channel	args( ( CHAR_DATA *ch, char *argument,
			    int channel, const char *verb ) );

char *  scramble        args( ( const char *argument, int modifier ) );			    
char *  drunk_speech    args( ( const char *argument, CHAR_DATA *ch ) ); 
void   generate_com_freq  args( ( CHAR_DATA *ch ) );
void   show_spys	args( ( CHAR_DATA *ch, CHAR_DATA *victim, char *tell) );

void generate_com_freq(CHAR_DATA *ch)
{
 char buf[MAX_STRING_LENGTH];

 sprintf(buf, "%d%d%d.%d%d%d",
	number_range(0,9),  number_range(0,9),  number_range(0,9),
	number_range(0,9),  number_range(0,9),  number_range(0,9));
 if(ch->comfreq)
   STRFREE(ch->comfreq);
 ch->comfreq = STRALLOC(buf);
 save_char_obj(ch);
 return;
}

void sound_to_room( ROOM_INDEX_DATA *room , char *argument )
{
   CHAR_DATA *vic;

        if ( room == NULL ) return;
        
        for ( vic = room->first_person; vic; vic = vic->next_in_room )
	   if ( !IS_NPC(vic) && IS_SET( vic->act, PLR_SOUND ) )
	     send_to_char( argument, vic );
     
}

void do_beep( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    bool ch_comlink, victim_comlink;
    
    argument = one_argument( argument, arg );
    
    REMOVE_BIT( ch->deaf, CHANNEL_TELLS );
    if ( IS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
    {
        send_to_char( "No puedes hacer eso aquí.\n\r", ch );
        return;
    }
                                
    if (!IS_NPC(ch)
        && ( IS_SET(ch->act, PLR_SILENCE)
        ||   IS_SET(ch->act, PLR_NO_TELL) ) )
    {
         send_to_char( "No puedes hacer eso.\n\r", ch );
         return;
    }

    if (!IS_NPC(ch) && IS_SET(ch->pcdata->act2, ACT_GAGGED))
    {
	send_to_char( "¡No puedes decir nada! ¡Te han amordazado!\n\r", ch);
	return;
    }
                                    
    if ( arg[0] == '\0' )
    {
         send_to_char( "¿A quién?\n\r", ch );
         return;
    }
                            
    if ( ( victim = get_char_world( ch, arg ) ) == NULL 
       || ( IS_NPC(victim) && victim->in_room != ch->in_room ) 
       || (!NOT_AUTHED(ch) && NOT_AUTHED(victim) && !IS_IMMORTAL(ch) ) )
    {
	send_to_char( "No está aquí.\n\r", ch );
	return;
    }
    
      ch_comlink = FALSE;
      victim_comlink = FALSE;
      
      if ( IS_IMMORTAL( ch ) )
      {
         ch_comlink = TRUE;
         victim_comlink = TRUE;
      }
      
      if ( IS_IMMORTAL( victim ) )
         victim_comlink = TRUE;
      
      for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
      {
        if (obj->pIndexData->item_type == ITEM_COMLINK)
        ch_comlink = TRUE;
      }
    
      if ( !ch_comlink )
      {
	send_to_char( "¡Necesitas un sistema de enlaces para hacer eso!\n\r", ch);
	return;
      }
      
      for ( obj = victim->last_carrying; obj; obj = obj->prev_content )
      {
        if (obj->pIndexData->item_type == ITEM_COMLINK)
        victim_comlink = TRUE;
      }

      if ( !victim_comlink )
      {
	send_to_char( "¡No parece tener un sistema de enlaces!\n\r", ch);
	return;
      }
    
    if (NOT_AUTHED(ch) && !NOT_AUTHED(victim) && !IS_IMMORTAL(victim) )
    {
	send_to_char( "Espera a que te autoricen primero.\n\r", ch);
	return;
    }

    if ( !IS_NPC( victim ) && ( victim->switched ) 
	&& ( get_trust( ch ) > LEVEL_AVATAR ) )
    {
      send_to_char( "Este jugador no está en su cuerpo.\n\r", ch );
      return;
    }
   else if ( !IS_NPC( victim ) && ( !victim->desc ) )
    {
      send_to_char( "No tiene conexión.\n\r", ch );
      return;
    }

    if ( IS_SET( victim->deaf, CHANNEL_TELLS ) 
    && ( !IS_IMMORTAL( ch ) || ( get_trust( ch ) < get_trust( victim ) ) ) )
    {
      act( AT_PLAIN, "Tiene el canal contar desactivado.", ch, NULL, victim,
		TO_CHAR );
      return;
    }

    if ( !IS_NPC (victim) && ( IS_SET (victim->act, PLR_SILENCE ) ) )
      {
      send_to_char( "Este jugador ha sido silenciado. No podrá responder a tu mensaje.\n\r", ch );
      }   

    if ( (!IS_IMMORTAL(ch) && !IS_AWAKE(victim) )
    || (!IS_NPC(victim)&&IS_SET(victim->in_room->room_flags, ROOM_SILENCE ) ) )
    {
    act( AT_PLAIN, "No puede oírte.", ch, 0, victim, TO_CHAR );
	return;
    }

    if ( victim->desc		/* make sure desc exists first  -Thoric */
    &&   victim->desc->connected == CON_EDITING 
    &&   get_trust(ch) < LEVEL_GOD )
    {
	act( AT_PLAIN, "Está escribiendo. Prueba en unos minutos.", ch, 0, victim, TO_CHAR );
        return;
    }

    show_spys(ch, victim, argument);

    ch_printf(ch , "&WMandas un beep a %s: %s\n\r\a" , victim->name, argument );
    send_to_char("\a",victim);    

    if ( knows_language( victim, ch->speaking, ch )
    ||  (IS_NPC(ch) && !ch->speaking) )
	act( AT_WHITE, "$n te manda un beep: '$t'", ch, argument, victim, TO_VICT );
    else
	act( AT_WHITE, "$n manda un beep: '$t'", ch, scramble(argument, ch->speaking), victim, TO_VICT );
}

/* Text scrambler -- Altrag */
char *scramble( const char *argument, int modifier )
{
    static char arg[MAX_INPUT_LENGTH];
    sh_int position;
    sh_int conversion = 0;
    
	modifier %= number_range( 80, 300 ); /* Bitvectors get way too large #s */
    for ( position = 0; position < MAX_INPUT_LENGTH; position++ )
    {
    	if ( argument[position] == '\0' )
    	{
    		arg[position] = '\0';
    		return arg;
    	}
    	else if ( argument[position] >= 'A' && argument[position] <= 'Z' )
	    {
	    	conversion = -conversion + position - modifier + argument[position] - 'A';
	    	conversion = number_range( conversion - 5, conversion + 5 );
	    	while ( conversion > 25 )
	    		conversion -= 26;
	    	while ( conversion < 0 )
	    		conversion += 26;
	    	arg[position] = conversion + 'A';
	    }
	    else if ( argument[position] >= 'a' && argument[position] <= 'z' )
	    {
	    	conversion = -conversion + position - modifier + argument[position] - 'a';
	    	conversion = number_range( conversion - 5, conversion + 5 );
	    	while ( conversion > 25 )
	    		conversion -= 26;
	    	while ( conversion < 0 )
	    		conversion += 26;
	    	arg[position] = conversion + 'a';
	    }
	    else if ( argument[position] >= '0' && argument[position] <= '9' )
	    {
	    	conversion = -conversion + position - modifier + argument[position] - '0';
	    	conversion = number_range( conversion - 2, conversion + 2 );
	    	while ( conversion > 9 )
	    		conversion -= 10;
	    	while ( conversion < 0 )
	    		conversion += 10;
	    	arg[position] = conversion + '0';
	    }
	    else
	    	arg[position] = argument[position];
	}
	arg[position] = '\0';
	return arg;	     
}

char *drunk_speech( const char *argument, CHAR_DATA *ch )
{
  const char *arg = argument;
  static char buf[MAX_INPUT_LENGTH*2];
  char buf1[MAX_INPUT_LENGTH*2];
  sh_int drunk;
  char *txt;
  char *txt1;  

  if ( IS_NPC( ch ) || !ch->pcdata ) return (char *) argument;

  drunk = ch->pcdata->condition[COND_DRUNK];

  if ( drunk <= 0 )
    return (char *) argument;

  buf[0] = '\0';
  buf1[0] = '\0';

  if ( !argument )
  {
     bug( "Drunk_speech: NULL argument", 0 );
     return "";
  }

  txt = buf;
  txt1 = buf1;

  while ( *arg != '\0' )
  {
    if ( toupper(*arg) == 'S' )
    {
	if ( number_percent() < ( drunk * 2 ) )		/* add 'h' after an 's' */
	{
	   *txt++ = *arg;
	   *txt++ = 'h';
	}
       else
	*txt++ = *arg;
    }
   else if ( toupper(*arg) == 'X' )
    {
	if ( number_percent() < ( drunk * 2 / 2 ) )
	{
	  *txt++ = 'c', *txt++ = 's', *txt++ = 'h';
	}
       else
	*txt++ = *arg;
    }
   else if ( number_percent() < ( drunk * 2 / 5 ) )  /* slurred letters */
    {
      sh_int slurn = number_range( 1, 2 );
      sh_int currslur = 0;	

      while ( currslur < slurn )
	*txt++ = *arg, currslur++;
    }
   else
    *txt++ = *arg;

    arg++;
  };

  *txt = '\0';

  txt = buf;

  while ( *txt != '\0' )   /* Let's mess with the string's caps */
  {
    if ( number_percent() < ( 2 * drunk / 2.5 ) )
    {
      if ( isupper(*txt) )
        *txt1 = tolower( *txt );
      else
      if ( islower(*txt) )
        *txt1 = toupper( *txt );
      else
        *txt1 = *txt;
    }
    else
      *txt1 = *txt;

    txt1++, txt++;
  };

  *txt1 = '\0';
  txt1 = buf1;
  txt = buf;

  while ( *txt1 != '\0' )   /* Let's make them stutter */
  {
    if ( *txt1 == ' ' )  /* If there's a space, then there's gotta be a */
    {			 /* along there somewhere soon */

      while ( *txt1 == ' ' )  /* Don't stutter on spaces */
        *txt++ = *txt1++;

      if ( ( number_percent() < ( 2 * drunk / 4 ) ) && *txt1 != '\0' )
      {
	sh_int offset = number_range( 0, 2 );
	sh_int pos = 0;

	while ( *txt1 != '\0' && pos < offset )
	  *txt++ = *txt1++, pos++;

	if ( *txt1 == ' ' )  /* Make sure not to stutter a space after */
	{		     /* the initial offset into the word */
	  *txt++ = *txt1++;
	  continue;
	}

	pos = 0;
	offset = number_range( 2, 4 );	
	while (	*txt1 != '\0' && pos < offset )
	{
	  *txt++ = *txt1;
	  pos++;
	  if ( *txt1 == ' ' || pos == offset )  /* Make sure we don't stick */ 
	  {		               /* A hyphen right before a space	*/
	    txt1--;
	    break;
	  }
	  *txt++ = '-';
	}
	if ( *txt1 != '\0' )
	  txt1++;
      }     
    }
   else
    *txt++ = *txt1++;
  }

  *txt = '\0';

  return buf;
}


/*
 * Generic channel function. --Heavily modified for the new color system -->KeB
 */
void talk_channel( CHAR_DATA *ch, char *argument, int channel, const char *verb )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char garb[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
   
    SHIP_DATA *ship = ship_from_cockpit( ch->in_room->vnum);
    int position;
    CLAN_DATA *clan = NULL;
        
    bool  ch_comlink = FALSE;
    bool  garble = FALSE;


    if(ch->comfreq == NULL || !str_cmp(ch->comfreq, "(null)")
	 || (strlen(ch->comfreq) != 7))
	 generate_com_freq(ch);


    if ( channel != CHANNEL_SHOUT && channel != CHANNEL_YELL && channel != CHANNEL_IMMTALK && channel != CHANNEL_OOC 
         && channel != CHANNEL_ASK && channel != CHANNEL_NEWBIE && channel != CHANNEL_AVTALK
         && channel != CHANNEL_SHIP && channel != CHANNEL_SYSTEM && channel != CHANNEL_SPACE 
         && channel != CHANNEL_103 && channel != CHANNEL_104 && channel != CHANNEL_105  )
    {
      OBJ_DATA *obj;
      
      if ( IS_IMMORTAL( ch ) )
          ch_comlink = TRUE;
      else
        for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
        {
           if (obj->pIndexData->item_type == ITEM_COMLINK)
	   {
           	ch_comlink = TRUE;
	   	break;
	   }
        }
    
      if ( !ch_comlink )
      {
	send_to_char( "¡Necesitas un sistema de enlaces para hacer eso!\n\r", ch);
	return;
      }

    }
     
    if ( IS_NPC( ch ) && (channel == CHANNEL_CLAN || channel == CHANNEL_ORDER || channel == CHANNEL_GUILD) )
    {
	send_to_char( "¡No molestes, mob!\n\r", ch );
	return;
    }


     if ( channel == CHANNEL_CLAN )
    {
        if ( ch->pcdata->clan->mainclan )
           clan = ch->pcdata->clan->mainclan;
        else
           clan = ch->pcdata->clan;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
    {
	send_to_char( "No puedes hacer eso aquí.\n\r", ch );
	return;
    }
    
    if ( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
      if ( ch->master )
	    send_to_char( "No lo creo...\n\r", ch->master );
      return;
    }

    if ( argument[0] == '\0' )
    {
	   sprintf( buf, "¿%s qué?\n\r", verb );
	   buf[0] = UPPER(buf[0]);
	   send_to_char( buf, ch );	/* where'd this line go? */
	   return;
    }

    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE) )
    {
	   ch_printf( ch, "No puedes %s.\n\r", verb );
	   return;
    }

    if (!IS_NPC(ch) && IS_SET(ch->pcdata->act2, ACT_GAGGED) && channel != CHANNEL_OOC)
    {
        send_to_char( "¡No puedes decir nada! ¡Te han amordazado!\n\r", ch);
        return;
    }

    REMOVE_BIT(ch->deaf, channel);

    switch ( channel )
    {

      case CHANNEL_CHAT:
      	 ch_printf( ch, "&R(&YFrec &w%s&R) &W%sEnvías: %s\n\r", ch->comfreq, color_str(AT_CHAT, ch), argument );
	  break;

      case CHANNEL_HOLONET:
	     ch_printf( ch, "&R<&B|&YHolonet&B|&R> &Y&Wtú: &W%s%s\n\r", color_str(AT_HOLONET, ch), argument);
	  break;

      case CHANNEL_CLANTALK:
	     ch_printf( ch, "&G&z(&pRed del clan&z) envías:&P %s%s\n\r", color_str(AT_CLAN, ch), argument );
	  break;

      case CHANNEL_SHIP:
        set_char_color(AT_SHIPTALK, ch);
	    ch_printf( ch, "%sCuentas a la nave '%s'\n\r", color_str(AT_SHIPTALK, ch), argument );
	  break;

      case CHANNEL_MUSIC:
        set_char_color(AT_MUSIC, ch);
        ch_printf( ch, "&R(&YCantar&R) %sTú: '%s'\n\r", color_str(AT_MUSIC, ch), argument );
      break;

      case CHANNEL_SYSTEM:
	    ch_printf( ch, "&b(&G&WSistema: %s&b)(&G&W%d&b) &G&w'&G&W%s&w'\n\r", ship->name, ship->channel, argument );
	  break;

      case CHANNEL_YELL:
        set_char_color(AT_YELL, ch);
        ch_printf( ch, "%ss '%s'\n\r", verb, argument );
      break;

      case CHANNEL_SHOUT:
        set_char_color(AT_SHOUT, ch);
	    ch_printf( ch, "%ss '%s'\n\r", verb, argument );
	  break;

      case CHANNEL_ASK:
	     ch_printf( ch, "&G(&gPregunta: &G&W%s&G) &W'%s'\n\r", ch->name, argument );
	  break;

      case CHANNEL_NEWBIE:
         set_char_color( AT_OOC, ch );
	     ch_printf( ch, "(NOVATO) %s: %s\n\r", ch->name, argument );
	  break;

      case CHANNEL_OOC:
          if (ch->top_level < LEVEL_IMMORTAL)
          {
             ch_printf(ch,"&Y(&OOOC&Y) &G&W%s%s: %s\n\r", color_str(AT_OOC,ch),ch->name, argument);
          }
          else
          {
             ch_printf(ch,"&Y(&OIMM&Y)&Y(&OOOC&Y) &G&W%s%s: %s\n\r", color_str(AT_OOC, ch), IS_SET(ch->act, PLR_WIZINVIS) ? "Immortal" : ch->name,  argument);
          }
          position        = ch->position;
          ch->position    = POS_STANDING;        
	      ch->position    = position;
	  break;

      case CHANNEL_WARTALK:
          set_char_color( AT_WARTALK, ch );
          ch_printf( ch, "%s '%s'\n\r", verb, argument );
	break;

      case CHANNEL_AVTALK:
      case CHANNEL_IMMTALK:
      case CHANNEL_103:
      case CHANNEL_104:
      case CHANNEL_105:
          if ( channel == CHANNEL_AVTALK )
               ch_printf( ch, "&R[&W%sAVTALK: %s&R] &W%s%s\n\r", color_str(AT_IMMTALK, ch), ch->name,color_str(AT_IMMTALK, ch), argument );
          else if ( channel == CHANNEL_IMMTALK )
               ch_printf( ch, "&R[&W%sIMM: %s&R] &W%s%s\n\r", color_str(AT_IMMTALK, ch), ch->name,color_str(AT_IMMTALK, ch), argument );
          else if ( channel == CHANNEL_103 )
               ch_printf( ch, "&R[&W%si103: %s&R] &W%s%s\n\r", color_str(AT_IMMTALK, ch), ch->name,color_str(AT_IMMTALK, ch), argument );
          else if ( channel == CHANNEL_104 )
               ch_printf( ch, "&R[&W%si104: %s&R] &W%s%s\n\r", color_str(AT_IMMTALK, ch), ch->name,color_str(AT_IMMTALK, ch), argument );
          else if ( channel == CHANNEL_105 )
                       ch_printf( ch, "\n\r&G&w&g[&w&W%sAdmin&g] &w&W%s%s&w&g: &w&W%s%s&w&g\n\r", color_str(AT_AVATAR, ch), color_str(AT_AVATAR, ch), ch->name, color_str(AT_AVATAR, ch), argument );
              position  = ch->position;
              ch->position      = POS_STANDING;
              ch->position      = position;
        break;

      default:
            ch_printf( ch, "&R(&Y%s&R) &Wtú: %s\n\r", verb, argument );
          break;
  
  }


    if ( IS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
    {
	sprintf( buf2, "%s: %s (%s)", IS_NPC( ch ) ? ch->short_descr : ch->name,
		 argument, verb );
	append_to_file( LOG_FILE, buf2 );
    }

    for ( d = first_descriptor; d; d = d->next )
    {
	CHAR_DATA *och;
	CHAR_DATA *vch;

	och = d->original ? d->original : d->character;
	vch = d->character;

	if ( d->connected == CON_PLAYING
        &&  vch != ch
	&&  !IS_SET(och->deaf, channel) )
	{
            char *sbuf = argument;
  	    ch_comlink = FALSE;
    
	if ( !str_cmp( vch->name, ch->name ))
	  continue;

    switch ( channel )
    {
      default:
	    sprintf( buf, "&R(&Y%s&R) &W$n: $t",verb );
	  break;

      case CHANNEL_HOLONET:
         sprintf( buf, "&R<&B|&YHolonet&B|&R> &W$n&Y: &W%s$t\n\r", color_str(AT_HOLONET, vch) );
	  break;

      case CHANNEL_CLANTALK:
	     sprintf( buf, "&G&z(&pRed del Clan&z) $n&z envía:&P %s$t", color_str(AT_CLAN, vch)  );
	  break;

      case CHANNEL_SHIP:
	     sprintf( buf, "%s$n dice por el sistema de comunicación de la nave '$t'", color_str(AT_SHIPTALK, vch)   );
	  break;

      case CHANNEL_MUSIC:
         sprintf( buf, "&R(&YCantar&R) %s$n sings: '$t'", color_str(AT_MUSIC, vch)   );
      break;

      case CHANNEL_SYSTEM:
	     sprintf( buf,  "&b(&G&WSistema: %s&b)(&G&W%d&b) &G&w'&G&W%s&w'\n\r", ship->name, ship->channel, argument );
	     sprintf( garb,  "&b(&G&WSistema: %s&b)(&G&W%d&b) &G&w'&G&WUn mensaje confuso llega&w'\n\r", ship->name, number_range(1, 100) );
	  break;

      case CHANNEL_YELL:
        sprintf( buf, "%s$n %s '$t'",    color_str(AT_YELL, vch), verb );
      break;
 
	  case CHANNEL_SHOUT:
	    sprintf( buf, "%s$n %s, '$t'",    color_str(AT_SHOUT, vch), verb );
	  break;

      case CHANNEL_ASK:
	     sprintf( buf, "&G(&gPregunta: &G&W%s&G) &W'%s'\n\r", ch->name, argument );
	  break;

      case CHANNEL_NEWBIE:
	    sprintf( buf, "(NOVATO) $n: $t" );
	  break;

      case CHANNEL_OOC:
          if (ch->top_level < LEVEL_IMMORTAL)
          {
             sprintf( buf, "&Y(&OOOC&Y) &G&W%s%s: $t", color_str(AT_OOC,vch), ch->name);
          }
          else
          {
              sprintf( buf, "&Y(&OIMM&Y)&Y(&OOOC&Y) &G&W%s%s: $t", color_str(AT_OOC,vch), IS_SET(ch->act, PLR_WIZINVIS) ? "Immortal" : ch->name );  
          }    
	  break;

      case CHANNEL_WARTALK:
        sprintf( buf, "$n %s '$t'", verb );
      break;

      case CHANNEL_AVTALK:
      case CHANNEL_IMMTALK:
      case CHANNEL_103:
      case CHANNEL_104:
      case CHANNEL_105:
          if ( channel == CHANNEL_AVTALK )
               sprintf( buf, "$n: $t" );
          else if ( channel == CHANNEL_IMMTALK )
               sprintf( buf, "&R[&W%sIMM: $n&R] &W%s$t", color_str(AT_IMMTALK, vch), color_str(AT_IMMTALK, vch) );
          else if ( channel == CHANNEL_103 )
        	   sprintf( buf, "(i103) $n> $t" );
          else if ( channel == CHANNEL_104 )
		       sprintf( buf, "(i104) $n> $t" );
          else if ( channel == CHANNEL_105 )
		       sprintf( buf, "&G&w&g[&w&W%sAdmin&g] &w&W%s$n&w&g: &w&W%s$t&w&g", color_str(AT_AVATAR, vch),color_str(AT_AVATAR, vch),color_str(AT_AVATAR, vch) );
	break;
    }

            if ( channel != CHANNEL_SHOUT && channel != CHANNEL_YELL && channel != CHANNEL_IMMTALK && channel != CHANNEL_OOC 
            && channel != CHANNEL_ASK && channel != CHANNEL_NEWBIE && channel != CHANNEL_AVTALK
            && channel != CHANNEL_SHIP && channel != CHANNEL_SYSTEM && channel != CHANNEL_SPACE
            && channel != CHANNEL_103 && channel != CHANNEL_104 && channel != CHANNEL_105
            )
            {
               OBJ_DATA *obj;
      
               if ( IS_IMMORTAL( och ) )
                ch_comlink = TRUE;
               else
               for ( obj = och->last_carrying; obj; obj = obj->prev_content )
               {
                  if (obj->pIndexData->item_type == ITEM_COMLINK)
                  ch_comlink = TRUE;
               }
    
               if ( !ch_comlink )
                 continue;
            }
  	    
	    if ( channel == CHANNEL_IMMTALK && !IS_IMMORTAL(och) )
		continue;
            if ( channel == CHANNEL_103 && get_trust(och) < 34 )
		continue;
            if ( channel == CHANNEL_104 && get_trust(och) < 35 )
		continue;
            if ( channel == CHANNEL_105 && get_trust(och) < 36)
		continue;
            if ( channel == CHANNEL_WARTALK && NOT_AUTHED( och ) )
                continue;
	    if ( channel == CHANNEL_AVTALK && !IS_HERO(och) )
		continue;

           if ( IS_SET( vch->in_room->room_flags, ROOM_SILENCE ) )
	    	continue;
	   
	   if ( channel == CHANNEL_YELL || channel == CHANNEL_SHOUT )
	   {
	      if ( ch->in_room != och->in_room )
	         continue;
	   }

	    if ( channel == CHANNEL_CLAN || channel == CHANNEL_ORDER
	    ||   channel == CHANNEL_GUILD )
	    {
		
		if ( IS_NPC( vch ) )
		  continue;
		
		if ( !vch->pcdata->clan )
	    	  continue;
		
		if ( vch->pcdata->clan != clan && vch->pcdata->clan->mainclan != clan )
	    	  continue;
	    }

            if ( channel == CHANNEL_SYSTEM )
            {
                SHIP_DATA *ship = ship_from_cockpit( ch->in_room->vnum );
                SHIP_DATA *target;
                bool commsys;
                int chance;
                OBJ_DATA *obj;
                commsys = FALSE;
                garble = FALSE;
                if ( !ship )
                   continue;
                if ( !vch->in_room )
                    continue;
                
                target = ship_from_cockpit( vch->in_room->vnum );
                
                if (!target) continue;
                
                if ( channel == CHANNEL_SYSTEM )
                   if (target->starsystem != ship->starsystem )
                      continue;                            
                for ( obj = vch->last_carrying; obj; obj = obj->prev_content )
                {
                  if (obj->pIndexData->item_type == ITEM_COMMSYSTEM){
                    commsys = TRUE;
                    break;
                  }
                }                      
                if ( target->channel != ship->channel && !commsys )
                  continue;
                
                if ( target->channel != ship->channel && commsys ){
		  chance = IS_NPC(vch) ? vch->top_level
    		    : (int)  (vch->pcdata->learned[gsn_commsystem]) ;
    		    if(number_percent() > chance){
    		    	garble = TRUE;
    		    	learn_from_failure(vch, gsn_commsystem);
    		    }
    		    else
    		      learn_from_success(vch, gsn_commsystem);
    		}
    		    

            }
            if ( channel == CHANNEL_ASK )
            {
              if(vch->top_level < 32)
                continue;
                
            }
            if ( channel == CHANNEL_SHIP  )
            {
                SHIP_DATA *ship = ship_from_room( ch->in_room->vnum );
                SHIP_DATA *target;
                
                if ( !ship )
                   continue;
                
                if ( !vch->in_room )
                    continue;
                
                target = ship_from_room( vch->in_room->vnum );
                
                if (!target) continue;
                
                if (target != ship )
                      continue;                            
            }
            
	    position		= vch->position;
	    if ( channel != CHANNEL_SHOUT && channel != CHANNEL_YELL )
		vch->position	= POS_STANDING;
	    if ( !knows_language( vch, ch->speaking, ch ) &&
			 (!IS_NPC(ch) || ch->speaking != 0)   &&
			 ( channel != CHANNEL_NEWBIE &&
			   channel != CHANNEL_OOC &&
			   channel != CHANNEL_AUCTION &&
			   channel != CHANNEL_ASK && 
			   channel != CHANNEL_AVTALK 
			   )  )
			sbuf = scramble(argument, ch->speaking);
	    MOBtrigger = FALSE;
	    if ( channel == CHANNEL_IMMTALK || channel == CHANNEL_AVTALK
	    || channel == CHANNEL_103 || channel == CHANNEL_104 || channel == CHANNEL_105 )
	    {
	      act( channel == CHANNEL_AVTALK ? AT_AVATAR : AT_IMMORT , buf, ch, sbuf, vch, TO_VICT );
	    }
            else if (channel == CHANNEL_WARTALK)
              act( AT_WARTALK, buf, ch, sbuf, vch, TO_VICT );
	    else if (channel == CHANNEL_OOC || channel == CHANNEL_NEWBIE )
              act( AT_OOC, buf, ch, sbuf, vch, TO_VICT );
            else if (channel == CHANNEL_ASK)
              act( AT_OOC, buf, ch, sbuf, vch, TO_VICT );
	    else if ( channel == CHANNEL_SHIP )
	      act( AT_SHIP, buf, ch, sbuf, vch, TO_VICT );
	    else if ( channel == CHANNEL_CLAN )
	      act( AT_CLAN, buf, ch, sbuf, vch, TO_VICT );
	    else if ( channel == CHANNEL_CHAT )
	     {

		ch_printf( vch, "&R(&YFrec: &w%s&R)&W%s%s%s%s: %s\n\r", ch->comfreq, color_str(AT_CHAT, vch),
          IS_IMMORTAL(vch) ? " &R(&W" : "",
          IS_IMMORTAL(vch) ? ch->name : "",
          IS_IMMORTAL(vch) ? "&R)&W" : "", argument );
	     }
	    else if ( channel == CHANNEL_SYSTEM)
            {
	      if (garble)
	        act( AT_SHIP, garb, ch, sbuf, vch, TO_VICT);  		
	      else
	        act( AT_SHIP, buf, ch, sbuf, vch, TO_VICT);
	    }    
	    else
	      act( AT_GOSSIP, buf, ch, sbuf, vch, TO_VICT );
	    vch->position	= position;
	}
    }

    return;
}

void do_retune(CHAR_DATA *ch, char *argument)
{
 char buf[MAX_STRING_LENGTH];

 if(IS_NPC(ch)) return;

 if(argument[0] != '\0')
 {
  if(ch->main_ability != SLICER_ABILITY)
  {
   send_to_char("No tienes idea de cómo sintonizar una frecuencia.\n\r", ch);
   return;
  }
  if(ch->skill_level[SLICER_ABILITY] < 20)
  {
   send_to_char("No tienes idea de como hacer eso todavía.\n\r", ch);
   return;
  }
  if( (strlen(argument) != 7) || isalpha(argument[0]) || isalpha(argument[1]) || isalpha(argument[2]) ||
   (argument[3] != '.') || isalpha(argument[4]) || isalpha(argument[5]) || isalpha(argument[6]) )
  {
   send_to_char("Uso: 'resintonizar ###.###'.\n\r", ch);
   return;
  }
  if(!str_cmp(argument, "111.111") )
  {
   send_to_char("No. Heh.\n\r", ch);
   return;
  }
  STRFREE(ch->comfreq);
  ch->comfreq = STRALLOC(argument);
  ch_printf(ch, "&wSintonizas tu sistema de enlaces que muestra: &z(&gFrecuencia:&G %s&z)&w.\n\r", ch->comfreq);
  return;
 }
  
 sprintf(buf, "%d%d%d.%d%d%d", number_range(0,9), number_range(0,9), number_range(0,9),
							   number_range(0,9), number_range(0,9), number_range(0,9) );
 if(ch->comfreq)
  STRFREE(ch->comfreq);

 ch->comfreq = STRALLOC(buf);
 ch_printf(ch, "&R&wJugueteas con tu sistema de enlaces un poco, y este muestra &z(&gFrecuencia:&G %s&z)&w.\n\r", ch->comfreq);
 return;
}

void to_channel( const char *argument, int channel, const char *verb, sh_int level )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if ( !first_descriptor || argument[0] == '\0' )
      return;

    sprintf(buf, "%s: %s\r\n", verb, argument );

    for ( d = first_descriptor; d; d = d->next )
    {
	CHAR_DATA *och;
	CHAR_DATA *vch;

	och = d->original ? d->original : d->character;
	vch = d->character;

	if ( !och || !vch )
	  continue;
	if ( !IS_IMMORTAL(vch)
	|| ( get_trust(vch) < sysdata.build_level && channel == CHANNEL_BUILD )
	|| ( get_trust(vch) < sysdata.log_level
	&& ( channel == CHANNEL_LOG || channel == CHANNEL_COMM) ) )
	  continue;

	if ( d->connected == CON_PLAYING
	&&  !IS_SET(och->deaf, channel)
	&&   get_trust( vch ) >= level )
	{
          if(channel == CHANNEL_LOG)
	    set_char_color( AT_LOG, vch );
           
          if(channel == CHANNEL_BUILD)
            set_char_color(AT_BUILD, vch);

          if(channel == CHANNEL_COMM)
            set_char_color(AT_COMM, vch);
        
	  send_to_char( buf, vch );
	}
    }

    return;
}


void do_chat( CHAR_DATA *ch, char *argument )
{    
    talk_channel( ch, argument, CHANNEL_CHAT, "Charlas" );
    return;
}

void do_holonet( CHAR_DATA *ch, char *argument )
{
    talk_channel(ch, argument, CHANNEL_HOLONET, "Holonet" );
    return;
}

void do_shiptalk( CHAR_DATA *ch, char *argument )
{
     SHIP_DATA *ship;

     if ( (ship = ship_from_cockpit(ch->in_room->vnum)) == NULL )  
     {
    	  send_to_char("&R¡Debes estar en la cabina de una nave para hacer esto!\n\r",ch);
    	  return;
     }
     talk_channel( ch, argument, CHANNEL_SHIP, "shiptalk" );
     return;
}    	        

void do_systemtalk( CHAR_DATA *ch, char *argument )
{
     SHIP_DATA *ship;
     
     if ( (ship = ship_from_cockpit(ch->in_room->vnum)) == NULL )  
     {
    	  send_to_char("&R¡Debes estar en la cabina de una nave para hacer esto!\n\r",ch);
    	  return;
     }
     talk_channel( ch, argument, CHANNEL_SYSTEM, "systemtalk" );
     return;
}    	        

void do_spacetalk( CHAR_DATA *ch, char *argument )
{
     SHIP_DATA *ship;

     if ( (ship = ship_from_cockpit(ch->in_room->vnum)) == NULL )  
     {
    	  send_to_char("&R¡Debes estar en la cabina de una nave para hacer esto!\n\r",ch);
    	  return;
     }
     talk_channel( ch, argument, CHANNEL_SPACE, "Comunicaciones Espaciales" );
     return;
}    	        

void do_gocial (CHAR_DATA * ch, char *command, char *argument)
{
  char arg[MAX_INPUT_LENGTH], buf[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  SOCIALTYPE *social;

  if ((social = find_social (command)) == NULL)
    {
      send_to_char ("Eso no es un social.\n\r", ch);
      return;
    }

  if (!IS_NPC (ch) && (IS_SET(ch->act, PLR_NO_EMOTE) || IS_SET(ch->act, PLR_SILENCE)))
    {
      send_to_char ("Eres antisocial!\n\r", ch);
      return;
    }

  switch (ch->position)
    {
    case POS_DEAD:
      send_to_char ("¡Has muerto!\n\r", ch);
      return;
    case POS_INCAP:
    case POS_MORTAL:
      send_to_char ("Estás demasiado mal para hacer eso.\n\r", ch);
      return;
    case POS_STUNNED:
      send_to_char ("El aturdimiento te lo impide.\n\r", ch);
      return;
    case POS_SLEEPING:
      /*
         * I just know this is the path to a 12" 'if' statement.  :(
         * But two players asked for it already!  -- Furey
       */
      if (!str_cmp (social->name, "roncar"))
	break;
      send_to_char ("¿En tus sueños?\n\r", ch);
      return;
    }

  one_argument (argument, arg);
  victim = NULL;

  if (arg[0] == '\0')
    {
      if (social->others_no_arg == NULL) 
      {
	send_to_char( "¡Necesitas hacer un social que los demás puedan ver!\n\r", ch);
	return;
      }
      sprintf (buf, "&Y(&OOOC&Y)&W %s", social->others_no_arg);
      act (AT_SOCIAL, buf, ch, NULL, victim, TO_MUD);
      sprintf (buf, "&Y(&OOOC&Y)&W %s", social->char_no_arg);
      act (AT_SOCIAL, buf, ch, NULL, victim, TO_CHAR);
      return;
    }

  if ((victim = get_char_world (ch, arg)) == NULL)
    {
      send_to_char ("En realidad no debería hablar de la gente que no está jugando.\n\r", ch);
      return;
    }

  if (victim == ch)
    {
      if (social->others_auto == NULL)
      {
	send_to_char( "¡Necesitas un social que los demás puedan ver!\n\r", ch);
	return;
      }
      sprintf (buf, "&Y(&OOOC&Y)&W %s", social->others_auto);
      act (AT_SOCIAL, buf, ch, NULL, victim, TO_MUD);
      sprintf (buf, "&Y(&OOOC&Y)&W %s", social->char_auto);
      act (AT_SOCIAL, buf, ch, NULL, victim, TO_CHAR);
      return;
    }
  else
    {
      if (social->others_found == NULL)
      {
	send_to_char( "¡Necesitas hacer un social que los demás puedan ver!\n\r", ch);
	return;
      }
      sprintf (buf, "&Y(&OOOC&Y)&W %s", social->others_found);
      act (AT_SOCIAL, buf, ch, NULL, victim, TO_MUD);
      sprintf (buf, "&Y(&OOOC&Y)&W %s", social->char_found);
      act (AT_SOCIAL, buf, ch, NULL, victim, TO_CHAR);
      sprintf (buf, "&Y(&OOOC&Y)&W %s", social->vict_found);
      act (AT_SOCIAL, buf, ch, NULL, victim, TO_VICT);
      return;
    }
}


void do_ooc( CHAR_DATA *ch, char *argument )
{
   DESCRIPTOR_DATA *d;
   char buf[MAX_INPUT_LENGTH];

  if (!IS_NPC (ch) && (IS_SET(ch->act, PLR_NO_EMOTE) || IS_SET(ch->act, PLR_SILENCE)))
    {
      send_to_char ("Eres antisocial!\n\r", ch);
      return;
    }

   if (argument[0]=='@')
    {
       char arg[MAX_INPUT_LENGTH],buf[MAX_INPUT_LENGTH];
       int loop;
    
       argument = one_argument(argument,arg);
       for (loop = 0; loop < strlen(arg); loop++)
       {
          buf[loop]=arg[loop+1];
       }
       do_gocial(ch, buf, argument);
       return;
    }
    else if (argument[0] == ':' && strlen(argument) > 3){
      int i;
      for(i=1;i<=strlen(argument);i++){
      	if(argument[i] != ' ')
      	  break;
      }
      argument+=i;
      sprintf(buf, "&Y(&OOOC&Y) &W%s %s\n\r", ch->name, argument);
      for ( d = first_descriptor; d; d = d->next )
      {
	
	if ( d->connected == CON_PLAYING || d->connected == CON_EDITING )
	  send_to_char(buf, d->character);
      }
	  
    }
    else {  
      talk_channel( ch, argument, CHANNEL_OOC, "ooc" );
      return;
    }
}

void do_clantalk( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }

    if ( IS_NPC( ch ) || !ch->pcdata->clan )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }
    talk_channel( ch, argument, CHANNEL_CLAN, "clantalk" );
    return;
}

void do_newbiechat( CHAR_DATA *ch, char *argument )
{
    if ( ch->top_level > 5 )
    {
	send_to_char( "¿No sabes ya suficiente para usar el canal de novatos?\n\r", ch );
	return;
    }
    talk_channel( ch, argument, CHANNEL_OOC, "ooc" );
    return;
}

void do_ot( CHAR_DATA *ch, char *argument )
{
  do_ordertalk( ch, argument );
}

void do_ordertalk( CHAR_DATA *ch, char *argument )
{
      send_to_char("Huh?\n\r", ch);
      return;
}

void do_guildtalk( CHAR_DATA *ch, char *argument )
{
      send_to_char("Huh?\n\r", ch);
      return;
}

void do_music( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, argument, CHANNEL_MUSIC, "sing" );
    return;
}


void do_quest( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, argument, CHANNEL_QUEST, "quest" );
    return;
}

void do_answer( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, argument, CHANNEL_ASK, "responde" );
    return;
}



void do_shout( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
  talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_SHOUT, "grita" );
  WAIT_STATE( ch, 12 );
  return;
}



void do_yell( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
  talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_YELL, "chilla" );
  return;
}



void do_immtalk( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, argument, CHANNEL_IMMTALK, "immtalk" );
    return;
}


void do_ask( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, argument, CHANNEL_ASK, "pregunta" );
    return;
}


void do_i103( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, argument, CHANNEL_103, "i103" );
    return;
}

void do_i104( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, argument, CHANNEL_104, "i104" );
    return;
}

void do_i105( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, argument, CHANNEL_105, "i105" );
    return;
}


void do_avtalk( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, drunk_speech( argument, ch ), CHANNEL_AVTALK, "avtalk" );
    return;
}


void do_say( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *vch;
    int actflags,x;

    if ( argument[0] == '\0' )
    {
	send_to_char( "¿Decir qué?\n\r", ch );
	return;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
    {
	send_to_char( "No puedes hacer eso aquí.\n\r", ch );
	return;
    }

    actflags = ch->act;
    if ( IS_NPC( ch ) ) REMOVE_BIT( ch->act, ACT_SECRETIVE );
	for ( vch = ch->in_room->first_person; vch; vch = vch->next_in_room )
	{
		char *sbuf = argument;

		if ( vch == ch )
			continue;
		if ( !knows_language(vch, ch->speaking, ch) &&
			 (!IS_NPC(ch) || ch->speaking != 0) )
			sbuf = scramble(argument, ch->speaking);
	      sbuf = drunk_speech( sbuf, ch );

		MOBtrigger = FALSE;

// Fun gag trick. Heh. Mmf mmmMmmf mmmf.

        if( ch && ch->pcdata && IS_SET(ch->pcdata->act2, ACT_GAGGED))
		{
		  for(x = 0; x < strlen(sbuf); x++)
		  {
			if(x == 0)
			{
			 sbuf[x] = 'M';
			 continue;
			}	
			if(isspace(sbuf[x]))
			{
			 if(x != 0)
			  if(x >= 2 && !isspace(sbuf[x-2]) )
			  sbuf[x-1] = 'f';
			 continue;
            }
			if(number_range(1,7) == 3)
			 sbuf[x] = 'M';
			else
			 sbuf[x] = 'm';

			if(x == strlen(sbuf))
			 sbuf[x] = 'f';
          }
        }
		act( AT_SAY, "$n dice: $t", ch, sbuf, vch, TO_VICT );
	}
/*    MOBtrigger = FALSE;
    act( AT_SAY, "$n dice '$T'", ch, NULL, argument, TO_ROOM );*/
    ch->act = actflags;
    MOBtrigger = FALSE;

    act( AT_SAY, "Dices: $T", ch, NULL, drunk_speech( argument, ch ), TO_CHAR ); 

    if ( IS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
    {
	sprintf( buf, "%s: %s", IS_NPC( ch ) ? ch->short_descr : ch->name,
		 argument );
	append_to_file( LOG_FILE, buf );
    }
    mprog_speech_trigger( argument, ch );
    if ( char_died(ch) )
      return;
    oprog_speech_trigger( argument, ch ); 
    if ( char_died(ch) )
      return;
    rprog_speech_trigger( argument, ch ); 
    return;
}

void do_oldtell( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    DESCRIPTOR_DATA *i;
    CHAR_DATA *och;
    int position;
    CHAR_DATA *switched_victim;
    bool ch_comlink;
    bool victim_comlink;
    OBJ_DATA *obj;
    
    switched_victim = NULL;

    if ( IS_SET( ch->deaf, CHANNEL_TELLS ) 
    && !IS_IMMORTAL( ch ) )
    {
      act( AT_PLAIN, "You have tells turned off... try chan +tells first", ch, NULL, NULL,
		TO_CHAR );
      return;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
    {
	send_to_char( "No puedes hacer eso aquí.\n\r", ch );
	return;
    }

    if (!IS_NPC(ch)
    && ( IS_SET(ch->act, PLR_SILENCE)
    ||   IS_SET(ch->act, PLR_NO_TELL) ) )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }

    if (!IS_NPC(ch) && IS_SET(ch->pcdata->act2, ACT_GAGGED))
    {
        send_to_char( "You can't say anything! You're gagged!\n\r", ch);
        return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Tell whom what?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL 
       || ( IS_NPC(victim) && victim->in_room != ch->in_room ) 
       || (!NOT_AUTHED(ch) && NOT_AUTHED(victim) && !IS_IMMORTAL(ch) ) )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "You have a nice little chat with yourself.\n\r", ch );
	return;
    }
    
    if (victim->in_room != ch->in_room && !IS_IMMORTAL(ch) )
    {
      ch_comlink = FALSE;
      victim_comlink = FALSE;
    
      for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
      {
        if (obj->pIndexData->item_type == ITEM_COMLINK)
        ch_comlink = TRUE;
      }
   
      if ( IS_IMMORTAL ( victim ) )
         ch_comlink = TRUE;
         victim_comlink = TRUE; 
      
      if ( !ch_comlink )
      {
        send_to_char( "You need a comlink to do that!\n\r", ch);
        return;
      }

      for ( obj = victim->last_carrying; obj; obj = obj->prev_content )
      {
        if (obj->pIndexData->item_type == ITEM_COMLINK)
        victim_comlink = TRUE;
      }

      if ( !victim_comlink )
      {
	send_to_char( "They don't seem to have a comlink!\n\r", ch);
	return;
      }
    
    }    
    
    if (NOT_AUTHED(ch) && !NOT_AUTHED(victim) && !IS_IMMORTAL(victim) )
    {
	send_to_char( "They can't hear you because you are not authorized.\n\r", ch);
	return;
    }

    if ( !IS_NPC( victim ) && ( victim->switched ) 
	&& ( get_trust( ch ) > LEVEL_AVATAR ) 
        && !IS_SET(victim->switched->act, ACT_POLYMORPHED)
	&& !IS_AFFECTED(victim->switched, AFF_POSSESS) )
    {
      send_to_char( "That player is switched.\n\r", ch );
      return;
    }

   else if ( !IS_NPC( victim ) && ( victim->switched ) 
        && (IS_SET(victim->switched->act, ACT_POLYMORPHED) 
 	||  IS_AFFECTED(victim->switched, AFF_POSSESS) ) )
     switched_victim = victim->switched;

   else if ( !IS_NPC( victim ) && ( !victim->desc ) )
    {
      send_to_char( "That player is link-dead.\n\r", ch );
      return;
    }

    if ( !IS_NPC (victim) && ( IS_SET (victim->act, PLR_AFK ) ) )
      {
      send_to_char( "That player is afk.\n\r", ch );
      return;
      }

    if ( IS_SET( victim->deaf, CHANNEL_TELLS ) 
    && ( !IS_IMMORTAL( ch ) || ( get_trust( ch ) < get_trust( victim ) ) ) )
    {
      act( AT_PLAIN, "$E has $S tells turned off.", ch, NULL, victim,
		TO_CHAR );
      return;
    }

    if ( !IS_NPC (victim) && ( IS_SET (victim->act, PLR_SILENCE ) ) )
      {
      send_to_char( "That player is silenced.  They will receive your message but can not respond.\n\r", ch );
      }   

    if ( (!IS_IMMORTAL(ch) && !IS_AWAKE(victim) )
    || (!IS_NPC(victim)&&IS_SET(victim->in_room->room_flags, ROOM_SILENCE ) ) )
    {
    act( AT_PLAIN, "$E can't hear you.", ch, 0, victim, TO_CHAR );
	return;
    }

    if ( victim->desc		/* make sure desc exists first  -Thoric */
    &&   victim->desc->connected == CON_EDITING 
    &&   get_trust(ch) < LEVEL_GOD )
    {
	act( AT_PLAIN, "$E is currently in a writing buffer.  Please try again in a few minutes.", ch, 0, victim, TO_CHAR );
        return;
    }

 
    if(switched_victim)
      victim = switched_victim;

    for (i = first_descriptor; i; i = i->next)
     {
       if (!i->connected && i->character)
         {
           och = i->original ? i->original : i->character;
            if (och->pcdata->tell_snoop == ch->name && !IS_IMMORTAL(och) && !IS_IMMORTAL(victim) && !IS_IMMORTAL(ch))
              {
              	sprintf(buf, "&B[&cINTERCEPTED MESSAGE&b]\n\r&B(&YSent: &G&W%s&B) (&Yto: &G&W%s&B) (&Ymessage&B) &G&W %s\n\r", ch->name, victim->name, argument);
              	send_to_char(buf, och);
              }
         }
     }
    
   for (i = first_descriptor; i; i = i->next)
     {
       if (!i->connected && i->character)
         {
           och = i->original ? i->original : i->character;
            if (och->pcdata->tell_snoop == victim->name && !IS_IMMORTAL(och) && !IS_IMMORTAL(victim) && !IS_IMMORTAL(ch))
              {
              	sprintf(buf, "&B[&cINTERCEPTED MESSAGE&b]\n\r&B(&YSent: &G&W%s&B) (&Yto: &G&W%s&B) (&Ymessage&B)&G&W %s\n\r", ch->name, victim->name, argument);
              	send_to_char(buf, och);
              }
         }
     }
   
    act( AT_TELL, "&W&G[&WTell&G] &G(&WTo: $N&G)&W '$t'", ch, argument, victim, TO_CHAR );
    position		= victim->position;
    victim->position	= POS_STANDING;
    if ( knows_language( victim, ch->speaking, ch )
    ||  (IS_NPC(ch) && !ch->speaking) )
	act( AT_TELL, "&W&G[&WTell&G] &G(&WFrom: $n&G)&W '$t'", ch, argument, victim, TO_VICT );
    else
	act( AT_TELL, "&W&G[&WTell&G] &G(&WFrom: $n&G)&W '$t'", ch, scramble(argument, ch->speaking), victim, TO_VICT );
    victim->position	= position;
    victim->reply	= ch;
    if ( IS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
    {
	sprintf( buf, "%s: %s (tell to) %s.",
		 IS_NPC( ch ) ? ch->short_descr : ch->name,
		 argument,
		 IS_NPC( victim ) ? victim->short_descr : victim->name );
	append_to_file( LOG_FILE, buf );
    }
    mprog_speech_trigger( argument, ch );
    return;
}

/* New tell - utilizing comfreq (Still in the works) */
void do_tell( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int position;
    CHAR_DATA *switched_victim;
    bool ch_comlink;
    bool victim_comlink;
    OBJ_DATA *obj;
	CHAR_DATA *vict2;
    
    switched_victim = NULL;

    if ( IS_SET( ch->deaf, CHANNEL_TELLS ) 
    && !IS_IMMORTAL( ch ) )
    {
      act( AT_PLAIN, "tienes el canal contar desactivado... Prueba 'canal +contar' primero.\r\n", ch, NULL, NULL,
		TO_CHAR );
      return;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
    {
	send_to_char( "No puedes hacer eso aquí.\n\r", ch );
	return;
    }

    if (!IS_NPC(ch)
    && ( IS_SET(ch->act, PLR_SILENCE)
    ||   IS_SET(ch->act, PLR_NO_TELL) ) )
    {
	send_to_char( "No puedes hacer eso.\n\r", ch );
	return;
    }

    if (!IS_NPC(ch) && IS_SET(ch->pcdata->act2, ACT_GAGGED))
    {
        send_to_char( "¡No puedes decir nada! ¡Te han amordazado!\n\r", ch);
        return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Uso: contar <frecuencia> <mensaje>.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_from_comfreq( ch, arg ) ) == NULL 
       || ( IS_NPC(victim) && victim->in_room != ch->in_room ) 
       || (!NOT_AUTHED(ch) && NOT_AUTHED(victim) && !IS_IMMORTAL(ch) )
       || (IS_IMMORTAL(victim) && !can_see(ch, victim)))
    {
	send_to_char( "No hay respuesta.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "Tienes una bonita conversación contigo mismo.\n\r", ch );
	return;
    }
    
    if (victim->in_room != ch->in_room && !IS_IMMORTAL(ch) )
    {
      ch_comlink = FALSE;
      victim_comlink = FALSE;
    
      for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
      {
        if (obj->pIndexData->item_type == ITEM_COMLINK)
        ch_comlink = TRUE;
      }
   
      if ( IS_IMMORTAL ( victim ) )
         ch_comlink = TRUE;
         victim_comlink = TRUE; 
      
      if ( !ch_comlink )
      {
        send_to_char( "Necesitas un sistema de enlaces para hacer eso.\n\r", ch);
        return;
      }

      for ( obj = victim->last_carrying; obj; obj = obj->prev_content )
      {
        if (obj->pIndexData->item_type == ITEM_COMLINK)
        victim_comlink = TRUE;
      }

      if ( !victim_comlink )
      {
	send_to_char( "No hay respuesta.\n\r", ch);
	return;
      }
    
    }    
    
    if (NOT_AUTHED(ch) && !NOT_AUTHED(victim) && !IS_IMMORTAL(victim) )
    {
	send_to_char( "No puede oírte porque no te han autorizado.\n\r", ch);
	return;
    }

    if ( !IS_NPC( victim ) && ( victim->switched ) 
	&& ( get_trust( ch ) > LEVEL_AVATAR ) 
        && !IS_SET(victim->switched->act, ACT_POLYMORPHED)
	&& !IS_AFFECTED(victim->switched, AFF_POSSESS) )
    {
      send_to_char( "Este jugador está fuera de su cuerpo.\n\r", ch );
      return;
    }

   else if ( !IS_NPC( victim ) && ( victim->switched ) 
        && (IS_SET(victim->switched->act, ACT_POLYMORPHED) 
 	||  IS_AFFECTED(victim->switched, AFF_POSSESS) ) )
     switched_victim = victim->switched;

   else if ( !IS_NPC( victim ) && ( !victim->desc ) )
    {
      send_to_char( "Este jugador no tiene conexión.\n\r", ch );
      return;
    }

    if ( !IS_NPC (victim) && ( IS_SET (victim->act, PLR_AFK ) ) )
      {
      send_to_char( "Este jugador está afk.\n\r", ch );
      return;
      }

    if ( IS_SET( victim->deaf, CHANNEL_TELLS ) 
    && ( !IS_IMMORTAL( ch ) || ( get_trust( ch ) < get_trust( victim ) ) ) )
    {
      act( AT_PLAIN, "Tiene el canal contar desactivado.", ch, NULL, victim,
		TO_CHAR );
      return;
    }

    if ( !IS_NPC (victim) && ( IS_SET (victim->act, PLR_SILENCE ) ) )
      {
      send_to_char( "Este jugador ha sido silenciado. Recibirá tu mensaje pero no podrá responderte.\n\r", ch );
      }   

    if ( (!IS_IMMORTAL(ch) && !IS_AWAKE(victim) )
    || (!IS_NPC(victim)&&IS_SET(victim->in_room->room_flags, ROOM_SILENCE ) ) )
    {
    act( AT_PLAIN, "No hay respuesta.", ch, 0, victim, TO_CHAR );
	return;
    }

    if ( victim->desc	
    &&   victim->desc->connected == CON_EDITING 
    &&   get_trust(ch) < LEVEL_GOD )
    {
	act( AT_PLAIN, "Está escribiendo. Prueba dentro de unos minutos.", ch, 0, victim, TO_CHAR );
        return;
    }

 
    if(switched_victim)
      victim = switched_victim;

    show_spys(ch, victim, argument);

if(!IS_IMMORTAL(victim) && !IS_IMMORTAL(ch))
{
 for(vict2 = first_char; vict2; vict2 = vict2->next)
 {
  if(!str_cmp(arg, vict2->comfreq))
  {
   if(knows_language(vict2, ch->speaking, ch) || (IS_NPC(ch) && !ch->speaking))
    ch_printf(vict2, "&W&G[&WMensaje entrante de: &w%s&G]&W: %s\n\r",
		IS_IMMORTAL(ch) ? ch->name :
	    IS_IMMORTAL(vict2) ? ch->name : ch->comfreq, argument);
   else
	ch_printf(vict2, "&W&G[&WMensaje entrante de: &w%s&G]&W: %s\n\r",
	    IS_IMMORTAL(ch) ? ch->name :
	    IS_IMMORTAL(vict2) ? ch->name : ch->comfreq, argument);

  }
 }
}
else
 	ch_printf(victim, "&W&G[&WMensaje entrante de: &w%s&G]&W: %s%s\n\r",
	    ch->name,  color_str(AT_TELL, victim),argument);

  if(IS_IMMORTAL(ch) || IS_IMMORTAL(victim))
   ch_printf(ch, "&W&G[&WMensaje enviado a: &w%s&G]&W: %s%s\n\r", victim->name, color_str(AT_TELL, ch),argument);
  else
   ch_printf(ch, "&W&G[&WMensaje enviado a: &w%s&G]&W: %s%s\n\r", arg, color_str(AT_TELL, ch), argument);

    position		= victim->position;
    victim->position	= POS_STANDING;
    victim->position	= position;
    victim->reply	= ch;
    if ( IS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
    {
	sprintf( buf, "%s: %s (tell to) %s.",
		 IS_NPC( ch ) ? ch->short_descr : ch->name,
		 argument,
		 IS_NPC( victim ) ? victim->short_descr : victim->name );
	append_to_file( LOG_FILE, buf );
    }
    mprog_speech_trigger( argument, ch );
    return;
}



void do_oldreply( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *i;
    CHAR_DATA *och;
    CHAR_DATA *victim;
    int position;
    bool ch_comlink;
    bool victim_comlink;
    OBJ_DATA *obj;

    if ( argument[0] == '\0')
    {
	send_to_char( "Reply with what?\n\r", ch);
	return;
    }

    REMOVE_BIT( ch->deaf, CHANNEL_TELLS );
    if ( IS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
    {
	send_to_char( "No puedes hacer eso aquí.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE) )
    {
	send_to_char( "Your message didn't get through.\n\r", ch );
	return;
    }

    if (!IS_NPC(ch) && IS_SET(ch->pcdata->act2, ACT_GAGGED))
    {
        send_to_char( "You can't say anything! You're gagged!\n\r", ch);
        return;
    }

    if ( ( victim = ch->reply ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( !IS_NPC( victim ) && ( victim->switched )
	&& can_see( ch, victim ) && ( get_trust( ch ) > LEVEL_AVATAR ) )
    {
      send_to_char( "That player is switched.\n\r", ch );
      return;
    }
   else if ( !IS_NPC( victim ) && ( !victim->desc ) )
    {
      send_to_char( "That player is link-dead.\n\r", ch );
      return;
    }

    if ( !IS_NPC (victim) && ( IS_SET (victim->act, PLR_AFK ) ) )
      {
      send_to_char( "That player is afk.\n\r", ch );
      return;
      }

    if ( IS_SET( victim->deaf, CHANNEL_TELLS ) 
    && ( !IS_IMMORTAL( ch ) || ( get_trust( ch ) < get_trust( victim ) ) ) )
    {
      act( AT_PLAIN, "$E has $S tells turned off.", ch, NULL, victim,
	TO_CHAR );
      return;
    }

    if (victim->in_room != ch->in_room && !IS_IMMORTAL(ch) )
    {
      ch_comlink = FALSE;
      victim_comlink = FALSE;
    
      for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
      {
        if (obj->pIndexData->item_type == ITEM_COMLINK)
        ch_comlink = TRUE;
      }
   
      if ( IS_IMMORTAL ( victim ) )
         ch_comlink = TRUE;
         victim_comlink = TRUE; 
      
      if ( !ch_comlink )
      {
        send_to_char( "You need a comlink to do that!\n\r", ch);
        return;
      }

      for ( obj = victim->last_carrying; obj; obj = obj->prev_content )
      {
        if (obj->pIndexData->item_type == ITEM_COMLINK)
        victim_comlink = TRUE;
      }

      if ( !victim_comlink )
      {
	send_to_char( "They don't seem to have a comlink!\n\r", ch);
	return;
      }
    
    }    

    if ( ( !IS_IMMORTAL(ch) && !IS_AWAKE(victim) )
    || ( !IS_NPC(victim) && IS_SET( victim->in_room->room_flags, ROOM_SILENCE ) ) )
    {
    act( AT_PLAIN, "$E can't hear you.", ch, 0, victim, TO_CHAR );
	return;
    }

    for (i = first_descriptor; i; i = i->next)
     {
       if (!i->connected && i->character)
         {
           och = i->original ? i->original : i->character;
            if (och->pcdata->tell_snoop == ch->name && !IS_IMMORTAL(och) && !IS_IMMORTAL(victim) && !IS_IMMORTAL(ch))
              {
              	sprintf(buf, "&B[&cINTERCEPTED MESSAGE&b]\n\r&B(&YSent: &G&W%s&B) (&Yto: &G&W%s&B) (&Ymessage&B) &G&W %s\n\r", ch->name, victim->name, argument);
              	send_to_char(buf, och);
              }
         }
     }
    
   for (i = first_descriptor; i; i = i->next)
     {
       if (!i->connected && i->character)
         {
           och = i->original ? i->original : i->character;
            if (och->pcdata->tell_snoop == victim->name && !IS_IMMORTAL(och) && !IS_IMMORTAL(victim) && !IS_IMMORTAL(ch))
              {
              	sprintf(buf, "&B[&cINTERCEPTED MESSAGE&b]\n\r&B(&YSent: &G&W%s&B) (&Yto: &G&W%s&B) (&Ymessage&B)&G&W %s\n\r", ch->name, victim->name, argument);
              	send_to_char(buf, och);
              }
         }
     }


    act( AT_TELL, "&W&G[&WTell&G] &G(&WTo: $N&G)&W '$t'", ch, argument, victim, TO_CHAR);
    position            = victim->position;
    victim->position    = POS_STANDING;
    if ( knows_language( victim, ch->speaking, ch )
    ||  (IS_NPC(ch) && !ch->speaking) )
        act( AT_TELL, "&W&G[&WTell&G] &G(&WFrom: $n&G)&W '$t'", ch, argument, victim, TO_VICT);
    else
        act( AT_TELL, "&W&G[&WTell&G] &G(&WFrom: $n&G)&W '$t'", ch, scramble(argument,ch->speaking), victim, TO_VICT);
    victim->position	= position;
    victim->reply	= ch;
    if ( IS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
    {
	sprintf( buf, "%s: %s (reply to) %s.",
		 IS_NPC( ch ) ? ch->short_descr : ch->name,
		 argument,
		 IS_NPC( victim ) ? victim->short_descr : victim->name );
	append_to_file( LOG_FILE, buf );
    }

    return;
}

void do_reply( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int position;
    bool ch_comlink;
    bool victim_comlink;
    OBJ_DATA *obj;

    if ( argument[0] == '\0')
    {
	send_to_char( "¿Responder con qué?\n\r", ch);
	return;
    }

    REMOVE_BIT( ch->deaf, CHANNEL_TELLS );
    if ( IS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
    {
	send_to_char( "No puedes hacer eso aquí.\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE) )
    {
	send_to_char( "Tu mensaje no ha llegado.\n\r", ch );
	return;
    }

    if (!IS_NPC(ch) && IS_SET(ch->pcdata->act2, ACT_GAGGED))
    {
        send_to_char( "¡No puedes decir nada! ¡Te han amordazado!\n\r", ch);
        return;
    }

    if ( ( victim = ch->reply ) == NULL )
    {
	send_to_char( "No hay respuesta.\n\r", ch );
	return;
    }

    if ( !IS_NPC( victim ) && ( victim->switched )
	&& can_see( ch, victim ) && ( get_trust( ch ) > LEVEL_AVATAR ) )
    {
      send_to_char( "Este jugador está fuera de su cuerpo.\n\r", ch );
      return;
    }
   else if ( !IS_NPC( victim ) && ( !victim->desc ) )
    {
      send_to_char( "Este jugador no tiene conexión.\n\r", ch );
      return;
    }

    if ( !IS_NPC (victim) && ( IS_SET (victim->act, PLR_AFK ) ) )
      {
      send_to_char( "Este jugador está afk.\n\r", ch );
      return;
      }

    if ( IS_SET( victim->deaf, CHANNEL_TELLS ) 
    && ( !IS_IMMORTAL( ch ) || ( get_trust( ch ) < get_trust( victim ) ) ) )
    {
      act( AT_PLAIN, "Tiene el canal contar desactivado.", ch, NULL, victim,
	TO_CHAR );
      return;
    }

    if (victim->in_room != ch->in_room && !IS_IMMORTAL(ch) )
    {
      ch_comlink = FALSE;
      victim_comlink = FALSE;
    
      for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
      {
        if (obj->pIndexData->item_type == ITEM_COMLINK)
        ch_comlink = TRUE;
      }
   
      if ( IS_IMMORTAL ( victim ) )
         ch_comlink = TRUE;
         victim_comlink = TRUE; 
      
      if ( !ch_comlink )
      {
        send_to_char( "Necesitas un sistema de enlaces para hacer eso.\n\r", ch);
        return;
      }

      for ( obj = victim->last_carrying; obj; obj = obj->prev_content )
      {
        if (obj->pIndexData->item_type == ITEM_COMLINK)
        victim_comlink = TRUE;
      }

      if ( !victim_comlink )
      {
	send_to_char( "No hay respuesta.\n\r", ch);
	return;
      }
    
    }    

    if ( ( !IS_IMMORTAL(ch) && !IS_AWAKE(victim) )
    || ( !IS_NPC(victim) && IS_SET( victim->in_room->room_flags, ROOM_SILENCE ) ) )
    {
    send_to_char("No hay respuesta.\n\r", ch );
	return;
    }

    show_spys(ch, victim, argument);

   if(knows_language(victim, ch->speaking, ch) || (IS_NPC(ch) && !ch->speaking))
    ch_printf(victim, "&W&G[&WMensaje entrante de: &w%s&G]&W: %s\n\r",
		IS_IMMORTAL(ch) ? ch->name :
	    IS_IMMORTAL(victim) ? ch->name : ch->comfreq, argument);
   else
	ch_printf(victim, "&W&G[&WMensaje entrante de: &w%s&G]&W: %s\n\r",
	    IS_IMMORTAL(ch) ? ch->name :
	    IS_IMMORTAL(victim) ? ch->name : ch->comfreq, argument);

   ch_printf(ch, "&W&G[&WMensaje enviado a: &w%s&G]&W: %s\n\r",
	    IS_IMMORTAL(ch) ? victim->name :
	    IS_IMMORTAL(victim) ? victim->name : victim->comfreq, argument);


    position            = victim->position;
    victim->position    = POS_STANDING;
    victim->position	= position;
    victim->reply	= ch;
    if ( IS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
    {
	sprintf( buf, "%s: %s (reply to) %s.",
		 IS_NPC( ch ) ? ch->short_descr : ch->name,
		 argument,
		 IS_NPC( victim ) ? victim->short_descr : victim->name );
	append_to_file( LOG_FILE, buf );
    }

    return;
}

void do_emote( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char *plast;
    int actflags;

    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_NO_EMOTE) )
    {
	send_to_char( "No puedes mostrar tus emociones.\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	send_to_char( "¿Emoción qué?\n\r", ch );
	return;
    }

    actflags = ch->act;
    if ( IS_NPC( ch ) ) REMOVE_BIT( ch->act, ACT_SECRETIVE );
    for ( plast = argument; *plast != '\0'; plast++ )
	;

    strcpy( buf, argument );
    if ( isalpha(plast[-1]) )
	strcat( buf, "." );

    MOBtrigger = FALSE;
    act( AT_ACTION, "$n $T", ch, NULL, buf, TO_ROOM );
    MOBtrigger = FALSE;
    act( AT_ACTION, "$n $T", ch, NULL, buf, TO_CHAR );
    ch->act = actflags;
    if ( IS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
    {
	sprintf( buf, "%s %s (emote)", IS_NPC( ch ) ? ch->short_descr : ch->name,
		 argument );
	append_to_file( LOG_FILE, buf );
    }
    return;
}




void do_sysbuglist( CHAR_DATA *ch, char *argument ) 
{
    set_char_color( AT_PLAIN, ch );
    if ( argument[0] == '\0' )
    {
        send_to_char_color( "\n\rUsage:  'sysbuglist list' or 'sysbuglist clear now'\n\r", ch);
        return;
    }
    if ( !str_cmp( argument, "clear now" ) )
    {
        FILE *fp = fopen( BUG_FILE, "w" );
        if ( fp )
          fclose( fp );
        send_to_char_color( "SysBugList file cleared.\n\r", ch);
        return;
    }
    if ( !str_cmp( argument, "list" ) )
    {
        show_file( ch, BUG_FILE );
    }
    else
    {
        send_to_char_color( "\n\rUsage:  'sysbuglist list' or 'sysbuglist clear now'\n\r", ch);
        return;
    }
 return;
}

void do_plrbuglist( CHAR_DATA *ch, char *argument )
{
    set_char_color( AT_PLAIN, ch );
    if ( argument[0] == '\0' )
    {
        send_to_char_color( "\n\rUsage:  'plrbuglist list' or 'plrbuglist clear now'\n\r", ch);
        return;
    }
    if ( !str_cmp( argument, "clear now" ) )
    {
        FILE *fp = fopen( PLRBUG_FILE, "w" );
        if ( fp )
          fclose( fp );
        send_to_char_color( "PlrBugList file cleared.\n\r", ch);
        return;
    }
    if ( !str_cmp( argument, "list" ) )
    {
        show_file( ch, PLRBUG_FILE );
    }
    else
    {
        send_to_char_color( "\n\rUsage:  'plrbuglist list' or 'plrbuglist clear now'\n\r", ch);
        return;
    }
 return;
}


void do_typoslist( CHAR_DATA *ch, char *argument )
{
    set_char_color( AT_PLAIN, ch );
    if ( argument[0] == '\0' )
    {
        send_to_char_color( "\n\rUsage:  'typoslist list' or 'typoslist clear now'\n\r", ch);
        return;
    }
    if ( !str_cmp( argument, "clear now" ) )
    {
        FILE *fp = fopen( TYPO_FILE, "w" );
        if ( fp )
          fclose( fp );
        send_to_char_color( "TyposList file cleared.\n\r", ch);
        return;
    }
    if ( !str_cmp( argument, "list" ) )
    {
        show_file( ch, TYPO_FILE );
    }
    else
    {
        send_to_char_color( "\n\rUsage:  'typoslist list' or 'typoslist clear now'\n\r", ch);
        return;
    }
 return;
}



void do_bug( CHAR_DATA *ch, char *argument )
{
    if( argument[0] == '\0' )
    {
        send_to_char("sintaxis: bug <Descripción del error>", ch);
        return;
    }

    append_file( ch, PLRBUG_FILE, argument );
    send_to_char( "Gracias por reportar el error.\n\r", ch );
    return;
}


void do_ide( CHAR_DATA *ch, char *argument )
{
    send_to_char("si quieres enviar una idea, teclea 'idea <mensaje>'.\n\r", ch);
    return;
}

void do_idea( CHAR_DATA *ch, char *argument )
{
    if( argument[0] == '\0' )
    {
	send_to_char("sintaxis: idea <Tu idea>", ch);
	return;
    }

    append_file( ch, IDEA_FILE, argument );
    ch_printf( ch, "Gracias por tu idea, %s agradece sugerencias. si tienes más, siéntete libre de escribirlas. - La administración de%s.", sysdata.mud_acronym, sysdata.mud_acronym);
    return;
}


/* Check the add helpfile file -- Xerves 8/24/99 */
void do_ahelp( CHAR_DATA *ch, char *argument )
{
    set_char_color( AT_PLAIN, ch );
    if ( argument[0] == '\0' )
    {
        send_to_char_color( "\n\rUsage:  'ahelp list' or 'ahelp clear now'\n\r", ch);
        return;
    }
    if ( !str_cmp( argument, "clear now" ) )
    {
        FILE *fp = fopen( HELP_FILE, "w" );
        if ( fp )
          fclose( fp );
        send_to_char_color( "Add Help file cleared.\n\r", ch);
        return;
    }
    if ( !str_cmp( argument, "list" ) )
    {
	  send_to_char_color( "\n\r VNUM \n\r.......\n\r", ch );
        show_file( ch, HELP_FILE );
    }
    else
    {
        send_to_char_color( "\n\rUsage:  'ahelp list' or 'ahelp clear now'\n\r", ch);
        return;
    }
    return;
}

void do_typo( CHAR_DATA *ch, char *argument )
{
    if( argument[0] == '\0' )
    {
        send_to_char("Sintaxis: tipo <Descripción del tipo>", ch);
        return;
    }

    append_file( ch, TYPO_FILE, argument );
    send_to_char( "Gracias por reportarlo.\n\r", ch );
    return;
}



void do_rent( CHAR_DATA *ch, char *argument )
{
    set_char_color( AT_WHITE, ch );
    send_to_char( "simplemente sal del juego.\n\r", ch );
    return;
}



void do_qui( CHAR_DATA *ch, char *argument )
{
    set_char_color( AT_RED, ch );
    send_to_char( "Si quieres abandonar el juego, escribe quit entero.\n\r", ch );
    return;
}

void do_quit( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    OBJ_DATA  *obj_next;
    int x, y;
    int level;
    char qbuf[MAX_INPUT_LENGTH];
    CHAR_DATA *fch;

    if ( IS_NPC(ch) && IS_SET(ch->act, ACT_POLYMORPHED))
    { 
      send_to_char("No puedes salir estando polimorfado.\n\r", ch);
      return;
    }

    if ( IS_NPC(ch) )
	return;

    if ( ch->position == POS_FIGHTING )
    {
	set_char_color( AT_RED, ch );
	send_to_char( "¡No! Estás luchando.\n\r", ch );
	return;
    }

    if ( ch->position  < POS_STUNNED  )
    {
	set_char_color( AT_BLOOD, ch );
	send_to_char( "No has muerto todavía.\n\r", ch );
	return;
    }

    if ( auction->item != NULL && ((ch == auction->buyer) || (ch == auction->seller) ) )
    {
	send_to_char("Espera a que termine la subasta actual.\n\r", ch);
	return;
    }
    
    if ( !IS_IMMORTAL(ch) && ch->in_room && !IS_SET( ch->in_room->room_flags , ROOM_HOTEL ) && !NOT_AUTHED(ch) )
    {
	send_to_char("No puedes salir del juego aquí.\n\r", ch);
	send_to_char("Necesitas encontrar un lugar de descanso, como un hotel...\n\r", ch);
	send_to_char("Quizás quieras 'llamar' a un aerodeslizador.\n\r", ch);
	return;
    }
    
    set_char_color( AT_QUIT, ch );
    send_to_char( "empiezas a desvanecerte en un remolino de colores\n\rque envuelve tu cuerpo... Cuando vuelvas, quizás nada sea igual.\n\r\n\r", ch );
    set_char_color( AT_QUIT, ch );
    ch_printf(ch, "Una extraña voz te dice 'Esperamos tu regreso, %s...'", ch->name);
//    act( AT_BYE, "$n has left the game.", ch, NULL, NULL, TO_ROOM );
//    set_char_color( AT_GREY, ch);

    if ( ch->challenged )
    {
      sprintf(qbuf,"¡%s ha salido! ¡La arena está vacía!",ch->name);
      ch->challenged=NULL;
      sportschan(qbuf);
    }

    for ( fch = first_char; fch; fch = fch->next )
    {
	if(IS_NPC(fch))
	  continue;

	if(fch->challenged && fch->challenged == ch)
	  fch->challenged=NULL;
    }

    sprintf( log_buf, "%s has quit.", ch->name );
    quitting_char = ch;
    save_char_obj( ch );
    save_home(ch);

    if ( ch->plr_home )
    {
      for ( obj = ch->plr_home->first_content; obj; obj = obj_next )
      {
        obj_next = obj->next_content;
        if ( obj->item_type == ITEM_SPACECRAFT )
            continue;
        extract_obj( obj );
      }
    }

    saving_char = NULL;

    level = get_trust(ch);
    /*
     * After extract_char the ch is no longer valid!
     */
    extract_char( ch, TRUE );
    for ( x = 0; x < MAX_WEAR; x++ )
	for ( y = 0; y < MAX_LAYERS; y++ )
	    save_equipment[x][y] = NULL;

    /* don't show who's logging off to leaving player */
/*
    to_channel( log_buf, CHANNEL_MONITOR, "Monitor", level ); 
*/
    log_string_plus( log_buf, LOG_COMM, level );

    return;
}


void send_rip_screen( CHAR_DATA *ch )
{
    FILE *rpfile;
    int num=0;
    char BUFF[MAX_STRING_LENGTH*2];

    if ((rpfile = fopen(RIPSCREEN_FILE,"r")) !=NULL) {
      while ((BUFF[num]=fgetc(rpfile)) != EOF)
	 num++;
      fclose(rpfile);
      BUFF[num] = 0;
      write_to_buffer(ch->desc,BUFF,num);
    }
}

void send_rip_title( CHAR_DATA *ch )
{
    FILE *rpfile;
    int num=0;
    char BUFF[MAX_STRING_LENGTH*2];

    if ((rpfile = fopen(RIPTITLE_FILE,"r")) !=NULL) {
      while ((BUFF[num]=fgetc(rpfile)) != EOF)
	 num++;
      fclose(rpfile);
      BUFF[num] = 0;
      write_to_buffer(ch->desc,BUFF,num);
    }
}

void send_ansi_title( CHAR_DATA *ch )
{
    FILE *rpfile;
    int num=0;
    char BUFF[MAX_STRING_LENGTH*2];

    if ((rpfile = fopen(ANSITITLE_FILE,"r")) !=NULL) {
      while ((BUFF[num]=fgetc(rpfile)) != EOF)
	 num++;
      fclose(rpfile);
      BUFF[num] = 0;
      write_to_buffer(ch->desc,BUFF,num);
    }
}

void send_ascii_title( CHAR_DATA *ch )
{
    FILE *rpfile;
    int num=0;
    char BUFF[MAX_STRING_LENGTH];

    if ((rpfile = fopen(ASCTITLE_FILE,"r")) !=NULL) {
      while ((BUFF[num]=fgetc(rpfile)) != EOF)
	 num++;
      fclose(rpfile);
      BUFF[num] = 0;
      write_to_buffer(ch->desc,BUFF,num);
    }
}


void do_rip( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "¿Rip ON o OFF?\n\r", ch );
	return;
    }
    if ( (strcmp(arg,"on")==0) || (strcmp(arg,"ON") == 0) ) {
	send_rip_screen(ch);
	SET_BIT(ch->act,PLR_ANSI);
	return;
    }

    if ( (strcmp(arg,"off")==0) || (strcmp(arg,"OFF") == 0) ) {
	send_to_char( "!|*\n\rRIP desactivado...\n\r", ch );
	return;
    }
}

void do_ansi( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "¿ANSI ON o OFF?\n\r", ch );
	return;
    }
    if ( (strcmp(arg,"on")==0) || (strcmp(arg,"ON") == 0) ) {
	SET_BIT(ch->act,PLR_ANSI);
	set_char_color( AT_WHITE + AT_BLINK, ch);
	send_to_char( "¡ANSI activado!!!\n\r", ch);
	return;
    }

    if ( (strcmp(arg,"off")==0) || (strcmp(arg,"OFF") == 0) ) {
	REMOVE_BIT(ch->act,PLR_ANSI);
	send_to_char( "Vale, soporte ANSI desactivado.\n\r", ch );
	return;
    }
}

void do_sound( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "¿SOUND ON o OFF?\n\r", ch );
	return;
    }
    if ( (strcmp(arg,"on")==0) || (strcmp(arg,"ON") == 0) ) {
	SET_BIT(ch->act,PLR_SOUND);
	set_char_color( AT_WHITE + AT_BLINK, ch);
	send_to_char( "¡Sonidos activados!!!\n\r", ch);
	send_to_char( "!!SOUND(hopeknow)", ch);
	return;
    }

    if ( (strcmp(arg,"off")==0) || (strcmp(arg,"OFF") == 0) ) {
	REMOVE_BIT(ch->act,PLR_SOUND);
	send_to_char( "Sonidos desactivados.\n\r", ch );
	return;
    }
}

void do_save( CHAR_DATA *ch, char *argument )
{
 char strsave[MAX_STRING_LENGTH];
 char strback[MAX_STRING_LENGTH];
    if ( IS_NPC(ch) && IS_SET(ch->act, ACT_POLYMORPHED))
    { 
      send_to_char("No mientras estés polimorfado.\n\r", ch);
      return;
    }

    if ( IS_NPC(ch) )
	return;

    if ( !IS_SET( ch->affected_by, race_table[ch->race].affected ) )
	SET_BIT( ch->affected_by, race_table[ch->race].affected );
    if ( !IS_SET( ch->resistant, race_table[ch->race].resist ) )
	SET_BIT( ch->resistant, race_table[ch->race].resist );
    if ( !IS_SET( ch->susceptible, race_table[ch->race].suscept ) )
	SET_BIT( ch->susceptible, race_table[ch->race].suscept );	   

    if ( NOT_AUTHED(ch) )
    { 
      send_to_char("No puedes salvar hasta que no te gradúes en la academia.\n\r", ch);
      return;
    }

//Save backups. Might cause troubles with file accessing.
    sprintf( strsave, "%s%c/%s", PLAYER_DIR, tolower(ch->name[0]),
                                 capitalize( ch->name ) );

    sprintf( strback, "%s%c/%s", BACKUP_DIR, tolower(ch->name[0]),
                                 capitalize( ch->name ) );
    rename( strsave, strback );


    save_char_obj( ch );
    save_home (ch );
    if(ch->top_level > 10)
     save_profile(ch);
    saving_char = NULL;
    send_to_char( "ficha salvada.\n\r", ch );
    return;
}


/*
 * Something from original DikuMUD that Merc yanked out.
 * Used to prevent following loops, which can cause problems if people
 * follow in a loop through an exit leading back into the same room
 * (Which exists in many maze areas)			-Thoric
 */
bool circle_follow( CHAR_DATA *ch, CHAR_DATA *victim )
{
    CHAR_DATA *tmp;

    for ( tmp = victim; tmp; tmp = tmp->master )
	if ( tmp == ch )
	  return TRUE;
    return FALSE;
}


void do_follow( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "¿Seguir a quién?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "No está aquí.\n\r", ch );
	return;
    }

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master )
    {
	act( AT_PLAIN, "¡Prefieres seguir a $N!", ch, NULL, ch->master, TO_CHAR );
	return;
    }

    if ( victim == ch )
    {
	if ( !ch->master )
	{
	    send_to_char( "No sigues a nadie.\n\r", ch );
	    return;
	}
	stop_follower( ch );
	return;
    }

    if ( circle_follow( ch, victim ) )
    {
	send_to_char( "No está permitido seguirse en círculos... Lo sentimos.\n\r", ch );
	return;
    }

    if ( ch->master )
	stop_follower( ch );

    add_follower( ch, victim );
    return;
}



void add_follower( CHAR_DATA *ch, CHAR_DATA *master )
{
    if ( ch->master )
    {
	bug( "Add_follower: non-null master.", 0 );
	return;
    }

    ch->master        = master;
    ch->leader        = NULL;

    if ( can_see( master, ch ) )
    act( AT_ACTION, "$n empieza a seguirte.", ch, NULL, master, TO_VICT );

    act( AT_ACTION, "Empiezas a seguir a $N.",  ch, NULL, master, TO_CHAR );

    return;
}



void stop_follower( CHAR_DATA *ch )
{
    if ( !ch->master )
    {
	bug( "Stop_follower: null master.", 0 );
	return;
    }

    if ( IS_AFFECTED(ch, AFF_CHARM) )
    {
	REMOVE_BIT( ch->affected_by, AFF_CHARM );
	affect_strip( ch, gsn_charm_person );
    }

    if(ch->master)
    {
      if ( can_see( ch->master, ch ) )
        act( AT_ACTION, "$n deja de seguirte.",     ch, NULL, ch->master, TO_VICT    );
      act( AT_ACTION, "Dejas de seguir a $N.",      ch, NULL, ch->master, TO_CHAR    );
    }

    ch->master = NULL;
    ch->leader = NULL;
    return;
}



void die_follower( CHAR_DATA *ch )
{
    CHAR_DATA *fch;

    if ( ch->master )
	stop_follower( ch );

    ch->leader = NULL;

    for ( fch = first_char; fch; fch = fch->next )
    {
	if ( fch->master == ch )
	    stop_follower( fch );
	if ( fch->leader == ch )
	    fch->leader = fch;
    }
    return;
}



void do_order( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char argbuf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *och;
    CHAR_DATA *och_next;
    bool found;
    bool fAll;

    strcpy( argbuf, argument );
    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "¿Ordenar qué a quién?\n\r", ch );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "Te apetece cumplir, no ordenar.\n\r", ch );
	return;
    }

    if(!str_prefix(argument,"mp"))
    {
       send_to_char("No... No lo creo.\n\r",ch);
       return;
    }

    if ( !str_cmp( arg, "todos" ) )
    {
	fAll   = TRUE;
	victim = NULL;
    }
    else
    {
	fAll   = FALSE;
	if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{
	    send_to_char( "No está aquí.\n\r", ch );
	    return;
	}

	if ( victim == ch )
	{
	    send_to_char( "¡qué pretendes exactamente!\n\r", ch );
	    return;
	}

	if ( !IS_AFFECTED(victim, AFF_CHARM) || victim->master != ch )
	{
	    send_to_char( "¡Hazlo tú!\n\r", ch );
	    return;
	}
    }

    found = FALSE;
    for ( och = ch->in_room->first_person; och; och = och_next )
    {
	och_next = och->next_in_room;

	if ( IS_AFFECTED(och, AFF_CHARM)
	&&   och->master == ch
	&& ( fAll || och == victim ) )
	{
	    found = TRUE;
	act( AT_ACTION, "$n te ordena '$t'.", ch, argument, och, TO_VICT );
	    interpret( och, argument );
	}
    }

    if ( found )
    {
        sprintf( log_buf, "%s: order %s.", ch->name, argbuf );
        log_string_plus( log_buf, LOG_NORMAL, ch->top_level );
 	send_to_char( "Bien.\n\r", ch );
        WAIT_STATE( ch, 12 );
    }
    else
	send_to_char( "No tienes seguidores aquí.\n\r", ch );
    return;
}

/*
char *itoa(int foo)
{
  static char bar[256];

  sprintf(bar,"%d",foo);
  return(bar);

}
*/

void do_group( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	CHAR_DATA *gch;
	CHAR_DATA *leader;

	leader = ch->leader ? ch->leader : ch;
	set_char_color( AT_GREEN, ch );
	ch_printf( ch, "Grupo de %s:\n\r", PERS(leader, ch) );

/* Changed so that no info revealed on possess */
	for ( gch = first_char; gch; gch = gch->next )
	{
	    if ( is_same_group( gch, ch ) )
	    {
		set_char_color( AT_DGREEN, ch );
		if (IS_AFFECTED(gch, AFF_POSSESS))
		  ch_printf( ch,
		    "[%2d %s] %-16s %4s/%4s hp %4s/%4s mv %5s xp\n\r",
		    gch->top_level,
		    IS_NPC(gch) ? "Mob" : race_table[gch->race].race_name,
		    capitalize( PERS(gch, ch) ),
		    "????",   
		    "????",
		    "????",
		    "????",
		    "?????"    );

		else
		  ch_printf( ch,
		    "[%2d %s] %-16s %4d/%4d hp %4d/%4d mv\n\r",
		    gch->top_level,
		    IS_NPC(gch) ? "Mob" : race_table[gch->race].race_name,
		    capitalize( PERS(gch, ch) ),
		    gch->hit,   
		    gch->max_hit,
		    gch->move,  
		    gch->max_move   );
	    }
	}
	return;
    }

    if ( !strcmp( arg, "deshacer" ))
    {
	CHAR_DATA *gch;
	int count = 0;

	if ( ch->leader || ch->master )
	{
	    send_to_char( "No puedes deshacer un grupo si sigues a alguien.\n\r", ch );
	    return;
	}
	
	for ( gch = first_char; gch; gch = gch->next )
	{
	    if ( is_same_group( ch, gch )
	    && ( ch != gch ) )
	    {
		gch->leader = NULL;
		gch->master = NULL;
		count++;
		send_to_char( "El grupo se deshace.\n\r", gch );
	    }
	}

	if ( count == 0 )
	   send_to_char( "No tienes ningún grupo que deshacer.\n\r", ch );
	else
	   send_to_char( "Deshaces tu grupo.\n\r", ch );
	
    return;
    }

    if ( !strcmp( arg, "todos" ) )
    {
	CHAR_DATA *rch;
	int count = 0;

        for ( rch = ch->in_room->first_person; rch; rch = rch->next_in_room )
	{
           if ( ch != rch
           &&   !IS_NPC( rch )
	   &&   rch->master == ch
	   &&   !ch->master
	   &&   !ch->leader
    	   &&   !is_same_group( rch, ch )
	      )
	   {
		rch->leader = ch;
		count++;
	   }
	}
	
	if ( count == 0 )
	  send_to_char( "No tienes a nadie que añadir a tu grupo.\n\r", ch );
	else
	{
     	   act( AT_ACTION, "$n agrupa a sus seguidores.", ch, NULL, victim, TO_ROOM );
	   send_to_char( "Agrupas a tus seguidores.\n\r", ch );
	}
    return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "No está aquí.\n\r", ch );
	return;
    }

    if ( ch->master || ( ch->leader && ch->leader != ch ) )
    {
	send_to_char( "¡Estás siguiendo a alguien!\n\r", ch );
	return;
    }

    if ( victim->master != ch && ch != victim )
    {
    act( AT_PLAIN, "$N no te está siguiendo.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( is_same_group( victim, ch ) && ch != victim )
    {
	victim->leader = NULL;
    act( AT_ACTION, "$n saca a $N de su grupo.",   ch, NULL, victim, TO_NOTVICT );
    act( AT_ACTION, "$n te saca del grupo.",  ch, NULL, victim, TO_VICT    );
    act( AT_ACTION, "Sacas a $N de tu grupo.", ch, NULL, victim, TO_CHAR    );
	return;
    }

    victim->leader = ch;
    act( AT_ACTION, "$N se une al grupo de $n.", ch, NULL, victim, TO_NOTVICT );
    act( AT_ACTION, "Te unes al grupo de $n.", ch, NULL, victim, TO_VICT    );
    act( AT_ACTION, "$N se une a tu grupo.", ch, NULL, victim, TO_CHAR    );
    return;
}



/*
 * 'Split' originally by Gnort, God of Chaos.
 */
void do_split( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *gch;
    int members;
    int amount;
    int share;
    int extra;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "¿Repartir cuanto?\n\r", ch );
	return;
    }

    amount = atoi( arg );

    if ( amount < 0 )
    {
	send_to_char( "A tu grupo no le gustaría eso.\n\r", ch );
	return;
    }

    if ( amount == 0 )
    {
	send_to_char( "No tienes créditos, esperas que nadie se dé cuenta.\n\r", ch );
	return;
    }

    if ( ch->gold < amount )
    {
	send_to_char( "No tienes tantos créditos.\n\r", ch );
	return;
    }

    members = 0;
    for ( gch = ch->in_room->first_person; gch; gch = gch->next_in_room )
    {
	if ( is_same_group( gch, ch ) )
	    members++;
    }

    
    if (( IS_SET(ch->act, PLR_AUTOGOLD)) && (members < 2))
    return;

    if ( members < 2 )
    {
	send_to_char( "No tienes a nadie en el grupo a quien darle nada.\n\r", ch );
	return;
    }

    share = amount / members;
    extra = amount % members;

    if ( share == 0 )
    {
	send_to_char( "No te molestes, tacaño.\n\r", ch );
	return;
    }

    ch->gold -= amount;
    ch->gold += share + extra;

    set_char_color( AT_GOLD, ch );
    ch_printf( ch,
	"Repartes %d créditos.  Your share is %d credits.\n\r",
	amount, share + extra );

    sprintf( buf, "$n reparte %d créditos.  tu parte es %d créditos.",
	amount, share );

    for ( gch = ch->in_room->first_person; gch; gch = gch->next_in_room )
    {
	if ( gch != ch && is_same_group( gch, ch ) )
	{
	    act( AT_GOLD, buf, ch, NULL, gch, TO_VICT );
	    gch->gold += share;
	}
    }
    return;
}



void do_gtell( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *gch;

    if ( argument[0] == '\0' )
    {
	send_to_char( "¿Contar al grupo qué?\n\r", ch );
	return;
    }

    if ( IS_SET( ch->act, PLR_NO_TELL ) )
    {
	send_to_char( "¡Tu mensaje no ha llegado!\n\r", ch );
	return;
    }

    /*
     * Note use of send_to_char, so gtell works on sleepers.
     */
/*    sprintf( buf, "%s tells the group '%s'.\n\r", ch->name, argument );*/
    for ( gch = first_char; gch; gch = gch->next )
    {
	if ( is_same_group( gch, ch ) )
	{
	    set_char_color( AT_GTELL, gch );
	    /* Groups unscrambled regardless of clan language.  Other languages
		   still garble though. -- Altrag */
	    if ( knows_language( gch, ch->speaking, gch )
	    ||  (IS_NPC(ch) && !ch->speaking) )
		ch_printf( gch, "&G&g(&G&W%sGrupo: &W%s&g)&G&W %s%s\n\r", color_str(AT_GTELL, gch), PERS(ch, gch),color_str(AT_GTELL, gch), argument );
	    else
		ch_printf( gch, "&G&g(&G&W%sGrupo: %s&g)&G&W %s%s\n\r",color_str(AT_GTELL, gch), PERS(ch, gch), color_str(AT_GTELL, gch), scramble(argument, ch->speaking) );
	}
    }

    return;
}


/*
 * It is very important that this be an equivalence relation:
 * (1) A ~ A
 * (2) if A ~ B then B ~ A
 * (3) if A ~ B  and B ~ C, then A ~ C
 */
bool is_same_group( CHAR_DATA *ach, CHAR_DATA *bch )
{
    if ( ach->leader ) ach = ach->leader;
    if ( bch->leader ) bch = bch->leader;
    return ach == bch;
}

/*
 * this function sends raw argument over the AUCTION: channel
 * I am not too sure if this method is right..
 */

void talk_auction (char *argument)
{
    DESCRIPTOR_DATA *d;
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *original;


    for (d = first_descriptor; d; d = d->next)
    {
        sprintf (buf,"&R&W[&W%sSubasta&W] %s", color_str(AT_AUCTION, d->character),argument); /* last %s to reset color */
        original = d->original ? d->original : d->character; /* if switched */
        if ((d->connected == CON_PLAYING) && !IS_SET(original->deaf,CHANNEL_AUCTION) 
        && !IS_SET(original->in_room->room_flags, ROOM_SILENCE) && !NOT_AUTHED(original))
            act( AT_GOSSIP, buf, original, NULL, NULL, TO_CHAR );
    }
}

/*
 * Language support functions. -- Altrag
 * 07/01/96
 */
bool knows_language( CHAR_DATA *ch, int language, CHAR_DATA *cch )
{
	sh_int sn;

	if ( !IS_NPC(ch) && IS_IMMORTAL(ch) )
		return TRUE;
	if ( IS_NPC(ch) && !ch->speaks ) /* No langs = knows all for npcs */
		return TRUE;
	if ( IS_NPC(ch) && IS_SET(ch->speaks, (language & ~LANG_CLAN)) )
		return TRUE;
	/* everyone does not KNOW basic tongue
	if ( IS_SET(language, LANG_BASIC) )
		return TRUE;
	*/
	if ( language & LANG_CLAN )
	{
		/* Clan = basic for mobs.. snicker.. -- Altrag */
		if ( IS_NPC(ch) || IS_NPC(cch) )
			return TRUE;
		if ( ch->pcdata->clan == cch->pcdata->clan &&
			 ch->pcdata->clan != NULL )
			return TRUE;
	}
	if ( !IS_NPC( ch ) )
	{
	    int lang;
	    
		/* Racial languages for PCs */
	    if ( IS_SET(race_table[ch->race].language, language) )
	    	return TRUE;

	    for ( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
	      if ( IS_SET(language, lang_array[lang]) &&
	      	   IS_SET(ch->speaks, lang_array[lang]) )
	      {
		  if ( (sn = skill_lookup(lang_names[lang])) != -1
		  &&    ch->pcdata->learned[sn] >= 60 )
		    return TRUE;
	      }
	}
	return FALSE;
}

bool can_learn_lang( CHAR_DATA *ch, int language )
{
	if ( language & LANG_CLAN )
		return FALSE;
	if ( IS_NPC(ch) || IS_IMMORTAL(ch) )
		return FALSE;
	if ( race_table[ch->race].language & language )
		return FALSE;
	if ( ch->speaks & language )
	{
		int lang;
		
		for ( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
			if ( language & lang_array[lang] )
			{
				int sn;
				
				if ( !(VALID_LANGS & lang_array[lang]) )
					return FALSE;
				if ( ( sn = skill_lookup( lang_names[lang] ) ) < 0 )
				{
					bug( "Can_learn_lang: valid language without sn: %d", lang );
					continue;
				}
				if ( ch->pcdata->learned[sn] >= 99 )
					return FALSE;
			}
	}
	if ( VALID_LANGS & language )
		return TRUE;
	return FALSE;
}

int const lang_array[] = { 
LANG_BASIC, LANG_WOOKIEE, LANG_TWI_LEK, LANG_RODIAN,
LANG_HUTT, LANG_MON_CALAMARI, LANG_NOGHRI, LANG_GAMORREAN, 
LANG_JAWA, LANG_ADARIAN, LANG_EWOK, LANG_VERPINE, LANG_DEFEL,
LANG_TRANDOSHAN, LANG_HAPAN, LANG_QUARREN, LANG_SULLUSTAN, LANG_BINARY,
LANG_FIRRERREO, LANG_CLAN, LANG_TOGORIAN, LANG_SHISTAVANEN,
LANG_KUBAZ, LANG_YEVETHAN, LANG_GAND, LANG_DUROS, LANG_COYNITE,
LANG_GOTAL, LANG_DEVARONIAN, LANG_FALLEEN, LANG_ITHORIAN, LANG_BARABEL,
LANG_UNKNOWN
};

/* Note: does not count racial language.  This is intentional (for now). */
int countlangs( int languages )
{
	int numlangs = 0;
	int looper;

	for ( looper = 0; lang_array[looper] != LANG_UNKNOWN; looper++ )
	{
		if ( lang_array[looper] == LANG_CLAN )
			continue;
		if ( languages & lang_array[looper] )
			numlangs++;
	}
	return numlangs;
}

char * const lang_names[] = { 
/*
"basic", "wookiee", "twilek", "rodian", "hutt",
 "mon calamari", "noghri", "ewok", "ithorian",
 "gotal", "devaronian", "binary", "gamorrean",
 "jawa", "adarian", "verpine", "defel", 
 "trandoshan", "hapan", "quarren", "firrerreo", "togorian", "shistavanen",
 "kubaz", "adarian", "sullustan", "falleen", "barabel", "yevethan", "gand",
 "duros", "coynite" 
*/
"basico", "wookiee", "twilek", "rodiano",
"hutt", "mon calamari", "noghri", "gamorreano", 
"jawa", "adariano", "ewok", "verpine", "defel",
"trandoshano", "hapano", "quarren", "sullustano", "binario",
"firrerreo", "clan", "togoriano", "shistavanen",
"kubaz", "yevethano", "gand", "duros", "coynite",
"gotal", "devaroniano", "falleen", "ithoriano", "barabel"
};
void do_speak( CHAR_DATA *ch, char *argument )
{
	int langs;
	char arg[MAX_INPUT_LENGTH];
	
	//argument = one_argument(argument, arg );
	one_argument( argument, arg );
	    if ( arg[0] == '\0' )
    {
	send_to_char( "¿hablar cuál idioma?\n\r", ch );
	return;
    }
	
	if ( !str_cmp( arg, "todos" ) && IS_IMMORTAL( ch ) )
	{
		set_char_color( AT_SAY, ch );
		ch->speaking = ~LANG_CLAN;
		send_to_char( "ahora hablas todos los idiomas.\n\r", ch );
		return;
	}
	if ( !str_prefix( arg, "basico" ) && ch->race == RACE_WOOKIEE )
	{
		set_char_color( AT_SAY, ch );
		send_to_char( "Los Wookiees no pueden hablar básico, aunque algunos pueden entenderlo.\n\r", ch );
		return;
	}
	if ( !str_prefix( arg, "twilek" ) && ch->race != RACE_TWI_LEK )
	{
		set_char_color( AT_SAY, ch );
		send_to_char( "Para hablar Twi'lek se necesitan partes del cuerpo que tú no tienes.\n\r", ch );
		return;
	}
	for ( langs = 0; lang_array[langs] != LANG_UNKNOWN; langs++ )
		if ( !str_prefix( arg, lang_names[langs] ) )
			if ( knows_language( ch, lang_array[langs], ch ) )
			{
				if ( lang_array[langs] == LANG_CLAN &&
					(IS_NPC(ch) || !ch->pcdata->clan) )
					continue;
				ch->speaking = lang_array[langs];
				set_char_color( AT_SAY, ch );
				ch_printf( ch, "ahora hablas %s.\n\r", lang_names[langs] );
				return;
			}
	set_char_color( AT_SAY, ch );
	send_to_char( "No conoces ese idioma.\n\r", ch );
}

void do_languages( CHAR_DATA *ch, char *argument )
{
	char arg[MAX_INPUT_LENGTH];
	int lang;
        int sn;
        	
	argument = one_argument( argument, arg );
	if ( arg[0] != '\0' && !str_prefix( arg, "aprender" ) &&
		!IS_IMMORTAL(ch) && !IS_NPC(ch) )
	{
		CHAR_DATA *sch;
		char arg2[MAX_INPUT_LENGTH];
		int prct;
		
		argument = one_argument( argument, arg2 );
		if ( arg2[0] == '\0' )
		{
			send_to_char( "¿Aprender qué idioma?\n\r", ch );
			return;
		}
		for ( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
		{
			if ( lang_array[lang] == LANG_CLAN )
				continue;
			if ( !str_prefix( arg2, lang_names[lang] ) )
				break;
		}
		if ( lang_array[lang] == LANG_UNKNOWN )
		{
			send_to_char( "Eso no es un idioma.\n\r", ch );
			return;
		}
		if ( !(VALID_LANGS & lang_array[lang]) )
		{
			send_to_char( "No puedes aprender ese lenguaje.\n\r", ch );
			return;
		}
		if ( ( sn = skill_lookup( lang_names[lang] ) ) < 0 )
		{
			send_to_char( "Eso no es un idioma.\n\r", ch );
			return;
		}
		if ( race_table[ch->race].language & lang_array[lang] ||
			 ch->pcdata->learned[sn] >= 99 )
		{
			act( AT_PLAIN, "ya eres fluente en $t.", ch,
				 lang_names[lang], NULL, TO_CHAR );
			return;
		}
		for ( sch = ch->in_room->first_person; sch; sch = sch->next )
			if ( IS_NPC(sch) && IS_SET(sch->act, ACT_SCHOLAR) &&
					knows_language( sch, ch->speaking, ch ) &&
					knows_language( sch, lang_array[lang], sch ) &&
					(!sch->speaking || knows_language( ch, sch->speaking, sch )) )
				break;
		if ( !sch )
		{
			send_to_char( "No hay nadie aquí que pueda enseñarte ese idioma.\n\r", ch );
			return;
		}
		/* Max 12% (5 + 4 + 3) at 24+ int and 21+ wis. -- Altrag */
		prct = 5 + (get_curr_int(ch) / 6) + (get_curr_wis(ch) / 7);
		ch->pcdata->learned[sn] += prct;
		ch->pcdata->learned[sn] = UMIN(ch->pcdata->learned[sn], 99);
		SET_BIT( ch->speaks, lang_array[lang] );
		if ( ch->pcdata->learned[sn] == prct )
			act( AT_PLAIN, "comienzas tus lecciones de $t.", ch, lang_names[lang],
				 NULL, TO_CHAR );
		else if ( ch->pcdata->learned[sn] < 60 )
			act( AT_PLAIN, "Continúas tus lecciones de $t.", ch, lang_names[lang],
				 NULL, TO_CHAR );
		else if ( ch->pcdata->learned[sn] < 60 + prct )
			act( AT_PLAIN, "Sientes que puedes comunicarte en $t.", ch,
				 lang_names[lang], NULL, TO_CHAR );
		else if ( ch->pcdata->learned[sn] < 99 )
			act( AT_PLAIN, "Ahora eres fluente en $t.", ch,
				 lang_names[lang], NULL, TO_CHAR );
		else
			act( AT_PLAIN, "Ahora hablas perfectamente $t.", ch, lang_names[lang],
				 NULL, TO_CHAR );
		return;
	}
	for ( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
		{
		       if ( !(VALID_LANGS & lang_array[lang]) )
                                continue;		
		        if ( lang_array[lang] == LANG_CLAN )
			        continue;
			if ( ch->speaking & lang_array[lang] ||
				(IS_NPC(ch) && !ch->speaking) )
				set_char_color( AT_RED, ch );
			else
				set_char_color( AT_SAY, ch );
	                if ( ( sn = skill_lookup( lang_names[lang] ) ) < 0 )
			   send_to_char( "(  0) ", ch );
			else
			   ch_printf( ch , "(%3d) ", ch->pcdata->learned[sn] );
		        send_to_char( lang_names[lang], ch );
			send_to_char( "\n\r", ch );			
		}
	send_to_char( "\n\r", ch );
	return;
}

void do_wartalk( CHAR_DATA *ch, char *argument )
{
    if (NOT_AUTHED(ch))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
    talk_channel( ch, argument, CHANNEL_WARTALK, "war" );
    return;
}

void show_spys(CHAR_DATA *ch, CHAR_DATA *victim, char *tell)
{
 CHAR_DATA *och;

 if(IS_NPC(ch) || IS_NPC(victim) || IS_IMMORTAL(ch) || IS_IMMORTAL(victim))
  return;

 for(och = first_char; och; och = och->next)
 {
   if(IS_NPC(och) || !och->comfreq)
    continue;

   if(!str_cmp(och->pcdata->tell_snoop, ch->comfreq) || !str_cmp(och->pcdata->tell_snoop, victim->comfreq))
     ch_printf(och, "&B[&cMensaje interceptado&b]\n\r&B(&YEnviado: &G&W%s&B) (&Ya: &G&W%s&B) (&Ymessage&B) &G&W %s\n\r", ch->comfreq, victim->comfreq, tell);

 }
}
