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
#include <time.h>
#include "mud.h"

/*Global Variables*/
char title[MAX_INPUT_LENGTH];
char disable[MAX_INPUT_LENGTH]; /*stores what portion of the ship will be disabled.*/
                                /*Used in void do_disable*/
bool    autofly(SHIP_DATA *ship);

/*
 * Slicers.c Containing skills created by Ackbar, Eleven, and Tawnos. *plug*
 *
*/

/* 
 * Tell snoop - modified to set tell_snoop to commfreq, then show all 
 *  incoming/outgoing whatnot on tell
*/
void do_tellsnoop( CHAR_DATA *ch, char *argument )
{
  char buf[MAX_INPUT_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  int chance;
  int i=0;

  argument = one_argument (argument, arg);

  if(arg[0] == '\0')
  {
     send_to_char("&RSintaxis: espiar <frecuencia/limpiar>\n\r", ch);
     return;
  }

  if ( !str_cmp(arg, "limpiar") || !str_cmp(arg, "self"))
    {
      send_to_char("Apagas tu radio.\n\r", ch);
      ch->pcdata->tell_snoop = NULL;
      return;
    }

  for(i=0; i < strlen(arg); i++)
  {
   if(isalpha(arg[i]))
   {
     send_to_char("&RSintaxis: espiar <frecuencia/limpiar>\n\r", ch);
     return;
   }
  }

  if(strlen(arg) > 3 && arg[3] != '.')
  {
     send_to_char("&RSintaxis: espiar <frecuencia/limpiar>\n\r", ch);
     return;
  }

  chance = IS_NPC(ch) ? ch->top_level
    : (int)  (ch->pcdata->learned[gsn_spy]) ;

  if ( number_percent( ) < chance )
   {
     learn_from_success( ch, gsn_spy );
     ch->pcdata->tell_snoop = STRALLOC(arg);
     sprintf(buf,"Ahora espías las comunicaciones de la frecuencia %s.\n\r", ch->pcdata->tell_snoop);
     send_to_char(buf, ch);
   }
  else
   {
     send_to_char("Fallas al encontrar la frecuencia.\n\r", ch);
     learn_from_failure( ch, gsn_spy );
   }
}

char blaha[MAX_STRING_LENGTH];

char *acctname (CHAR_DATA *ch) {
  static char buf[MAX_STRING_LENGTH];
  const char *name;
  char *s;
  int len;

  *buf = '\0';
  s = buf;
  len = 0;
  name = ch->name;
#define add_to_s(chr) (*s++ = chr, ++len)
  for (; *name && len < 15; ++name) {
    if (isalpha (*name)) {
      switch (tolower (*name)) {
      case 'a':
        add_to_s ('9');
        add_to_s ('1');
        break;
      case 'b':
        add_to_s ('0');
        add_to_s ('2');
        break;
      case 'c':
        add_to_s ('5');
        add_to_s ('9');
        break;
      case 'd':
        add_to_s ('1');
        add_to_s ('4');
        break;
      case 'e':
        add_to_s ('5');
        break;
      case 'f':
        add_to_s ('6');
        break;
      case 'g':
        add_to_s ('7');
        break;
      case 'h':
        add_to_s ('8');
        break;
      case 'i':
        add_to_s ('9');
        break; 
      case 'j':
        add_to_s ('1');
        add_to_s ('0');
        break;
      case 'k':
        add_to_s ('1');
        add_to_s ('1');
        break;
      case 'l':
        add_to_s ('2');
        add_to_s ('0');
        break;
      case 'm':
        add_to_s ('0');
        add_to_s ('1');
        break;
      case 'n':
        add_to_s ('1');
        add_to_s ('4');
        break;
      case 'o':
        add_to_s ('0');
        add_to_s ('9');
        break;
      case 'p':
        add_to_s ('1');
        add_to_s ('6');
        break;
      case 'q':
        add_to_s ('1');
        add_to_s ('7');
        break;
      case 'r':
        add_to_s ('5');
        add_to_s ('4');
        break;
      case 's':
        add_to_s ('1');
        add_to_s ('9');
        break;
      case 't':
        add_to_s ('2');
        add_to_s ('0');
        break;
      case 'u':
        add_to_s ('2');
        add_to_s ('1');
        break;
      case 'v':
        add_to_s ('2');
        add_to_s ('2');
        break;
      case 'w':
        add_to_s ('2');
        add_to_s ('4');
        break; 
      case 'x':
        add_to_s ('2');
        add_to_s ('3');
        break;
      case 'y': 
        add_to_s ('5');
        add_to_s ('2');
        break;
      case 'z':
        add_to_s ('2');
        add_to_s ('6');
        break;
      }
    }
  }
  if (len < 15) {
    size_t namelen;
    char *filler;
    char fillerbuf[MAX_STRING_LENGTH];
    const char *const fillers[] = { "gewhinnqnppali", "hmmithinkishou",
                                    "ldinsertsomehi", "ddenmessagesin",
                                    "thisforfuturec", "coderstolaughat",
                                    "ireallyshouldb", "esleepingnowbu",
                                    "timaddictedtot", "hisshit" };

    *fillerbuf = '\0';
    name = ch->name;
    namelen = strlen (name);
    strcpy (fillerbuf, name);
    if (namelen == 3)
      strcpy (fillerbuf + namelen, fillers[0]);
    else if (namelen > 11 || namelen < 3)
      strcpy (fillerbuf + namelen, fillers[9]);
    else
      strcpy (fillerbuf + namelen, fillers[namelen - 3]);

    *s = '\0';
    filler = fillerbuf + strlen (buf);

    for (; *filler && len < 15; ++filler) {
      if (isalpha (*filler)) {
        switch (tolower (*filler)) {
        case 'a':
          add_to_s ('6');
          add_to_s ('6');
          break;
        case 'b':
          add_to_s ('9');
          add_to_s ('0');
          break;
        case 'c':
          add_to_s ('2');
          add_to_s ('7');
          break;
        case 'd':
          add_to_s ('2');
          add_to_s ('1');
          break;
        case 'e':
          add_to_s ('2');
          add_to_s ('2');
          break;
        case 'f':
          add_to_s ('6');
          break;
        case 'g':
          add_to_s ('7');
          break;
        case 'h':
          add_to_s ('5');
          add_to_s ('0');
          break;
        case 'i':
          add_to_s ('9');
          break;
        case 'j':
          add_to_s ('1');
          add_to_s ('0');
          break;
        case 'k':
          add_to_s ('1');
          add_to_s ('1');
          break;
        case 'l':
          add_to_s ('1');
          add_to_s ('2');
          break;
        case 'm':
          add_to_s ('1');
          add_to_s ('3');
          break;
        case 'n':
          add_to_s ('0');
          add_to_s ('1');
          break;
        case 'o':
          add_to_s ('1');
          add_to_s ('5');
          break;
        case 'p':
          add_to_s ('1');
          add_to_s ('6');
          break;
        case 'q':
          add_to_s ('2');
          break;
        case 'r':
          add_to_s ('5');
          add_to_s ('1');
          break;
        case 's':
          add_to_s ('1');
          add_to_s ('8');
          break;
        case 't':
          add_to_s ('7');
          add_to_s ('2');
          break;
        case 'u':
          add_to_s ('4');
          add_to_s ('4');
          break;
        case 'v':
          add_to_s ('9');
          break;
        case 'w':
          add_to_s ('8');
          add_to_s ('2');
          break;
        case 'x':
          add_to_s ('1');
          add_to_s ('1');
          break;
        case 'y':
          add_to_s ('1');
          add_to_s ('4');
          break;
        case 'z':
          add_to_s ('5');
          break;
        }

        if (len >= 15)
          break;

        switch (tolower (*filler)) {
        case 'a':
          add_to_s ('2');
          break;
        case 'b':
          add_to_s ('1');
          add_to_s ('7');
          add_to_s ('3');
          break;
        case 'c':
          add_to_s ('5');
          add_to_s ('5');
          add_to_s ('8');
          break;
        case 'd':
          add_to_s ('8');
          add_to_s ('1');
          break;
        case 'e':
          add_to_s ('3');
          add_to_s ('0');
          add_to_s ('9');
          break;
        case 'f':
          add_to_s ('6');
          add_to_s ('4');
          add_to_s ('1');
          break;
        case 'g':
          add_to_s ('6');
          add_to_s ('7');
          add_to_s ('8');
          break;
        case 'h':
          add_to_s ('5');
          break;
        case 'i':
          add_to_s ('2');
          add_to_s ('0');
          add_to_s ('2');
          break;
        case 'j':
          add_to_s ('1');
          add_to_s ('7');
          add_to_s ('0');
          break;
        case 'k':
          add_to_s ('1');
          add_to_s ('1');
          add_to_s ('1');
          break;
        case 'l':
          add_to_s ('1');
          add_to_s ('5');
          add_to_s ('2');
          break;
        case 'm':
          add_to_s ('1');
          add_to_s ('3');
          break;
        case 'n':
          add_to_s ('0');
          add_to_s ('1');
          break;
        case 'o':
          add_to_s ('6');
          add_to_s ('1');
          add_to_s ('5');
          break;
        case 'p':
          add_to_s ('1');
          add_to_s ('6');
          break;
        case 'q':
          add_to_s ('2');
          break;
        case 'r':
          add_to_s ('3');
          add_to_s ('3');
          break;
        case 's':
          add_to_s ('0');
          add_to_s ('6');
          break;
        case 't':
          add_to_s ('7');
          add_to_s ('2');
          break;
        case 'u':
          add_to_s ('9');
          add_to_s ('4');
          add_to_s ('7');
          break;
        case 'v':
          add_to_s ('5');
          add_to_s ('9');
          break;
        case 'w':
          add_to_s ('7');
          add_to_s ('1');
          break;
        case 'x':
          add_to_s ('6');
          add_to_s ('1');
          add_to_s ('1');
          break;
        case 'y':
          add_to_s ('2');
          add_to_s ('4');
          break;
        case 'z':
          add_to_s ('6');
          add_to_s ('1');
          add_to_s ('7');
          break;
        }

        if (len >= 15)
          break;

        switch (tolower (*filler)) {
        case 'a':
          add_to_s ('1');
          break;
        case 'b':
          add_to_s ('2');
          break;
        case 'c':
          add_to_s ('3');
          break;
        case 'd':
          add_to_s ('4');
          break;
        case 'e':
          add_to_s ('5');
          break;
        case 'f':
          add_to_s ('6');
          break;
        case 'g':
          add_to_s ('7');
          break;
        case 'h':
          add_to_s ('8');
          break;
        case 'i':
          add_to_s ('9');
          break;
        case 'j':
          add_to_s ('1');
          add_to_s ('0');
          break;
        case 'k':
          add_to_s ('1');
          add_to_s ('1');
          break;
        case 'l':
          add_to_s ('1');
          add_to_s ('2');
          break;
        case 'm':
          add_to_s ('1');
          add_to_s ('3');
          break;
        case 'n':
          add_to_s ('1');
          add_to_s ('4');
          break;
        case 'o':
          add_to_s ('1');
          add_to_s ('5');
          break;
        case 'p':
          add_to_s ('1');
          add_to_s ('6');
          break;
        case 'q':
          add_to_s ('1');
          add_to_s ('7');
          break;
        case 'r':
          add_to_s ('1');
          add_to_s ('8');
          break;
        case 's':
          add_to_s ('1');
          add_to_s ('9');
          break;
        case 't':
          add_to_s ('2');
          add_to_s ('0');
          break;
        case 'u':
          add_to_s ('2');
          add_to_s ('1');
          break;
        case 'v':
          add_to_s ('2');
          add_to_s ('2');
          break;
        case 'w':
          add_to_s ('2');
          add_to_s ('3');
          break;
        case 'x':
          add_to_s ('2');
          add_to_s ('4');
          break;
        case 'y':
          add_to_s ('2');
          add_to_s ('5');
          break;
        case 'z':
          add_to_s ('2');
          add_to_s ('6');
          break;
        }
      }
    }
#undef add_to_s
  }
  buf[15] = '\0';
  return buf;
}

void do_inquire(CHAR_DATA *ch, char *argument )
{
  DESCRIPTOR_DATA *d;
  bool checkdata;
  OBJ_DATA *obj;
  int  x;
  long xpgain;
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_INPUT_LENGTH];
  int  chance;

  strcpy( arg , argument );
  checkdata = FALSE;
  switch( ch->substate )
   {
    default:

     for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
      {
      	if (obj->item_type == ITEM_DATAPAD)
      	  checkdata = TRUE;
      }

     if ( ( checkdata == FALSE ) )
      {
      	send_to_char("Necesitas un datapad.\n\r", ch);
      	return;
      }
     if(!IS_SET(ch->in_room->room_flags, ROOM_BANK))
      {
		send_to_char("Debes estar en un banco.\n\r", ch);
		return;
      }
     chance = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_inquire]);
     if ( number_percent( ) < chance )
      {
        
       	  send_to_char( "&GComienzas el largo proceso de hackear el sistema bancario.\n\r", ch);
    	  sprintf(buf, "$n comienza a usar su datapad." );
     	  act( AT_PLAIN, buf, ch, NULL, argument , TO_ROOM );
     	  add_timer ( ch , TIMER_DO_FUN , 10 , do_inquire , 1 );
    	  return;
    
      }
	send_to_char("&RNo consigues encontrar un punto de acceso al sistema bancario.\n\r",ch);
	learn_from_failure( ch, gsn_inquire );
        return;

		case 1: break;
    	case SUB_TIMER_DO_ABORT:
    	  ch->substate = SUB_NONE;
    	  send_to_char("&RTe distraes y no consigues completar tu trabajo.\n\r", ch);
    	  return;
   }

     ch->substate = SUB_NONE;

     chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_inquire]);

    x = number_percent( );

    if ( number_percent( ) > chance*2 )
     {
		ch_printf(ch,"&z|+---------------------------------------------------------------------+|&w\n\r");
		ch_printf(ch,"&z|^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|&x^g Bienvenido al sistema bancario galáctico. Prohibida la entrada no autorizada. &z^x|\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|&x^g Login: %d                                                          &z^x|\n\r",number_range(11111,99999));
		ch_printf(ch,"&z|&x^g contraseña: *********                                                   &z^x|\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|&x^g Contraseña inválida.                                                     &z^x|\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|+---------------------------------------------------------------------+|&w\n\r");
       learn_from_failure( ch, gsn_inquire );
       return;
     }
      
		ch_printf(ch,"&z|+---------------------------------------------------------------------+|&w\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|&x^g Bienvenido al sistema bancario galáctico. Prohibida la entrada no autorizada. &z^x|\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|&x^g Login: %d                                                          &z^x|\n\r",number_range(11111,99999));
		ch_printf(ch,"&z|&x^g Contraseña: *********                                                   &z^x|\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|&x^g Login aceptado...recuperando datos de cuentas, Espere.                   &z^x|\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|&x^g _______  Cuenta  _____________________________________ Ahorross _____ &z^x|\n\r");
       for(d = first_descriptor; d; d = d->next)
       {
         if(!d->character)  continue;
		 if(d->connected != CON_PLAYING)  continue;
	if(IS_IMMORTAL(d->character)) continue;
		ch_printf(ch,"&z|&x^g     # %s                                  %-9.9d      &z^x|\n\r", acctname(d->character), d->character->pcdata->bank);
       }
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|+---------------------------------------------------------------------+|&w\n\r");

    xpgain = 3000;
      gain_exp(ch, xpgain, SLICER_ABILITY);
      ch_printf( ch , " Recibes %d puntos de experiencia en hacker.\n\r", xpgain );
      learn_from_success( ch, gsn_inquire) ;
      return;

}


void do_makecommsystem ( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int level,chance;
    bool checktool, checkdura,checkbattery,checkcrystal,checkcircuit;
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;
    strcpy( arg , argument );

 switch( ch->substate )
    {
    	default:
    if ( arg[0] == '\0' )
     {
       send_to_char( "&RUso: fabricarcomunicador <nombre>\n\r&w", ch);
       return;
     }

    checktool    = FALSE;
    checkdura    = FALSE;
    checkbattery = FALSE;
    checkcrystal = FALSE;
    checkcircuit = FALSE;

    if ( !IS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
      {
      	 send_to_char("Necesitas estar en una fábrica para fabricar sistemas de comunicación.", ch);
      	 return;
      }

    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
      {
        if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;
        if (obj->item_type == ITEM_DURAPLAST)
          checkdura = TRUE;
        if (obj->item_type == ITEM_BATTERY)
          checkbattery = TRUE;
        if (obj->item_type == ITEM_CRYSTAL)
          checkcrystal = TRUE;
        if (obj->item_type == ITEM_CIRCUIT)
          checkcircuit = TRUE;
      }

    if (!checktool)
     {
     	send_to_char("¡Necesitas una caja de herramientas!\n\r", ch);
     	return;
     }

    if (!checkdura)
     {
        send_to_char("¡Necesitas algo de duraplástico!\n\r", ch);
        return;
     }

    if (!checkbattery)
     {
        send_to_char("¡necesitas una batería!\n\r", ch);
        return;
     }

    if (!checkcrystal)
     {
     	send_to_char("¡Necesitas un cristal pequeño!\n\r", ch);
     	return;
     }

    if (!checkcircuit)
     {
     	send_to_char("¡Necesitas un circuito pequeño!\n\r", ch);
     	return;
     }

     chance = IS_NPC(ch) ? ch->top_level
        : (int) (ch->pcdata->learned[gsn_makecommsystem]);

    if ( number_percent( ) < chance )
      {
    	send_to_char( "&GComienzas el largo proceso para crear un sistema de comunicación.\n\r", ch);
    	act( AT_PLAIN, "$n comienza a trabajar en algo con su caja de herramientas.", ch,
	NULL, argument , TO_ROOM );
	add_timer ( ch , TIMER_DO_FUN , 5 , do_makecommsystem , 1 );
    	ch->dest_buf   = str_dup(arg);
    	return;
      }
	send_to_char("&RNo consigues poner todas las piezas juntas.\n\r",ch);
	learn_from_failure( ch, gsn_makecommsystem );
        return;

        case 1:
    	  if ( !ch->dest_buf )
    	     return;
    	  strcpy(arg, ch->dest_buf);
    	  DISPOSE( ch->dest_buf);
    	  break;

    	case SUB_TIMER_DO_ABORT:
    	  DISPOSE( ch->dest_buf );
    	  ch->substate = SUB_NONE;
    	  send_to_char("&RTe distraes y finalizas tu trabajo.\n\r", ch);
    	  return;
       }

    ch->substate = SUB_NONE;

    level = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makecommsystem]);
    vnum = COMMSYS_VNUM;

    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
         send_to_char( "&Rel objeto que intentas crear no existe en la base de datos.\n\rPor favor avisa a la administración del error.\n\r", ch );
         return;
    }

    checktool    = FALSE;
    checkdura    = FALSE;
    checkbattery = FALSE;
    checkcrystal = FALSE;
    checkcircuit = FALSE;
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
      {
         if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;
        if (obj->item_type == ITEM_DURAPLAST && checkdura == FALSE)

          {
          	checkdura = TRUE;
          	separate_obj( obj );
                obj_from_char( obj );
                extract_obj( obj );
          }

         if (obj->item_type == ITEM_BATTERY && checkbattery == FALSE)
          {
          	checkbattery = TRUE;
          	separate_obj( obj );
                obj_from_char( obj );
                extract_obj( obj );
          }
         if (obj->item_type == ITEM_CRYSTAL && checkcrystal == FALSE)
          {
          	checkcrystal = TRUE;
          	separate_obj( obj );
                obj_from_char( obj );
                extract_obj( obj );
          }
         if (obj->item_type == ITEM_CIRCUIT && checkcircuit == FALSE)
          {
          	checkcircuit = TRUE;
          	separate_obj( obj );
                obj_from_char( obj );
                extract_obj( obj );
          }
       }
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makecommsystem]) ;

    if ( number_percent( ) > chance*2  || ( !checktool ) || ( !checkdura ) || ( !checkcrystal ) || ( !checkbattery ) || (
!checkcircuit ) )
    {
       send_to_char( "&RSostienes tu nuevo comunicador y pulsas un par de botones.\n\r", ch);
       send_to_char( "&R¡Tu nuevo comunicador explota!\n\r", ch);
       ch->hit -= 15;
       learn_from_failure( ch, gsn_makecommsystem );
       return;
    }

    obj = create_object( pObjIndex, level );
    obj->item_type = ITEM_COMMSYSTEM;
    SET_BIT( obj->wear_flags, ITEM_HOLD );
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    obj->level = level;
    obj->weight = 2+level/10;
    STRFREE( obj->name );
    strcpy( buf , arg );
    strcat( buf , " CommSystem");
    obj->name = STRALLOC( buf );
    strcpy( buf, arg );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );
    STRFREE( obj->description );
    strcat( buf, " was dropped on the floor." );
    obj->description = STRALLOC( buf );;
    obj->cost = 45000;
    obj = obj_to_char( obj, ch );

    send_to_char( "&Gfinalizas tu trabajo y admiras tu nuevo comunicador.&w\n\r", ch);
    act( AT_PLAIN, "$n finaliza su trabajo y contempla su nuevo comunicador.", ch,
         NULL, argument , TO_ROOM );

    {
         long xpgain;

         xpgain = UMIN( obj->cost*10 ,( exp_level(ch->skill_level[SLICER_ABILITY]+1) - exp_level(ch->skill_level[SLICER_ABILITY]) )
);
         gain_exp(ch, xpgain, SLICER_ABILITY);
         ch_printf( ch , "Recibes %d Puntos de experiencia en hacker.", xpgain );
    }
    learn_from_success( ch, gsn_makecommsystem );
}


void do_makedatapad ( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int level,chance;
    bool checktool, checklens, checkdura,checkbattery,checksuper,checkcircuit;
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;
    strcpy( arg , argument );

 switch( ch->substate )
    {
    	default:
    if ( arg[0] == '\0' )
     {
       send_to_char( "&RSintaxis: Fabricardatapad <nombre>\n\r&w", ch);
       return;
     }

    checktool    = FALSE;
    checkdura    = FALSE;
    checkbattery = FALSE;
    checksuper   = FALSE;
    checkcircuit = FALSE;
    checklens    = FALSE;

    if ( !IS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
      {

      	 send_to_char("Debes estar en una fábrica para construir un datapad.", ch);
      	 return;
      }

    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
      {
        if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;
        if (obj->item_type == ITEM_DURASTEEL)
          checkdura = TRUE;
        if (obj->item_type == ITEM_BATTERY)
          checkbattery = TRUE;
        if (obj->item_type == ITEM_SUPERCONDUCTOR)
          checksuper = TRUE;
        if (obj->item_type == ITEM_CIRCUIT)
          checkcircuit = TRUE;
        if (obj->item_type == ITEM_LENS)
          checklens = TRUE;
      }

    if (!checktool)
     {
     	send_to_char("Necesitas una caja de herramientas.\n\r", ch);
     	return;
     }

    if (!checkdura)
     {
        send_to_char("Necesitas duracero.\n\r", ch);
        return;
     }

    if (!checkbattery)
     {
        send_to_char("Necesitas una batería.\n\r", ch);
        return;
     }

    if (!checksuper)
     {
     	send_to_char("Necesitas un superconductor.!\n\r", ch);
     	return;
     }

    if (!checkcircuit)
     {
     	send_to_char("Necesitas un circuito.\n\r", ch);
     	return;
     }


    if (!checklens)
     {
     	send_to_char("Necesitas una lente.\n\r", ch);
     	return;
     }

     chance = IS_NPC(ch) ? ch->top_level
        : (int) (ch->pcdata->learned[gsn_makedatapad]);

    if ( number_percent( ) < chance )
      {
    	send_to_char( "&GComienzas el largo proceso de creación de un datapad.\n\r", ch);
    	act( AT_PLAIN, "$n comienza a trabajar en algo con su caja de herramientas.", ch,
	NULL, argument , TO_ROOM );
	add_timer ( ch , TIMER_DO_FUN , 5 , do_makedatapad , 1 );
    	ch->dest_buf   = str_dup(arg);
    	return;
      }
	send_to_char("&RNo consigues mantener las piezas juntas.\n\r",ch);
	learn_from_failure( ch, gsn_makedatapad );
        return;

        case 1:
    	  if ( !ch->dest_buf )
    	     return;
    	  strcpy(arg, ch->dest_buf);
    	  DISPOSE( ch->dest_buf);
    	  break;

    	case SUB_TIMER_DO_ABORT:
    	  DISPOSE( ch->dest_buf );
    	  ch->substate = SUB_NONE;
    	  send_to_char("&RTe distraes y no logras terminar el trabajo.\n\r", ch);
    	  return;
       }

    ch->substate = SUB_NONE;

    level = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makedatapad]);
    vnum = DATAPAD_VNUM;

    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
         send_to_char( "&REl objeto que intentas crear no está en la base de datos.\n\rInforma a algún administrador del error.\n\r", ch );
         return;
    }

    checktool    = FALSE;
    checkdura    = FALSE;
    checkbattery = FALSE;
    checksuper   = FALSE;
    checkcircuit = FALSE;
    checklens    = FALSE;
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
      {
         if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;
         if (obj->item_type == ITEM_DURASTEEL && checkdura == FALSE)
          {
          	checkdura = TRUE;
          	separate_obj( obj );
                obj_from_char( obj );
                extract_obj( obj );
          }

         if (obj->item_type == ITEM_BATTERY && checkbattery == FALSE)
          {
          	checkbattery = TRUE;
          	separate_obj( obj );
                obj_from_char( obj );
                extract_obj( obj );
          }
         if (obj->item_type == ITEM_SUPERCONDUCTOR && checksuper == FALSE)
          {
          	checksuper = TRUE;
          	separate_obj( obj );
                obj_from_char( obj );
                extract_obj( obj );
          }
         if (obj->item_type == ITEM_CIRCUIT && checkcircuit == FALSE)
          {
          	checkcircuit = TRUE;
          	separate_obj( obj );
                obj_from_char( obj );
                extract_obj( obj );
          }
         if (obj->item_type == ITEM_LENS && checklens == FALSE)
          {
          	checklens = TRUE;
          	separate_obj( obj );
                obj_from_char( obj );
                extract_obj( obj );
          }
       }
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makedatapad]) ;

    if ( number_percent( ) > chance*2  || ( !checktool ) || ( !checkdura ) || ( !checksuper ) || ( !checkbattery ) || (
!checkcircuit ) || ( !checklens ) )
    {
       send_to_char( "&RSostienes tu nuevo datapad y comienzas a introducir datos.\n\r", ch);
       send_to_char( "&R¡Tu nuevo datapad explota violentamente!\n\r", ch);
              ch->hit -= 15;
       learn_from_failure( ch, gsn_makedatapad );
       return;
    }

    obj = create_object( pObjIndex, level );
    obj->item_type = ITEM_DATAPAD;
    SET_BIT( obj->wear_flags, ITEM_HOLD );
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    obj->level = level;
    obj->weight = 2+level/10;
    STRFREE( obj->name );
    strcpy( buf , arg );
    strcat( buf , " CommSystem");
    obj->name = STRALLOC( buf );
    strcpy( buf, arg );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );
    STRFREE( obj->description );
    strcat( buf, " está aquí." );
    obj->description = STRALLOC( buf );;
    obj->cost = 45000;
    obj = obj_to_char( obj, ch );

    send_to_char( "&GFinalizas tu trabajo y admiras tu nuevo datapad.&w\n\r", ch);
    act( AT_PLAIN, "$n termina de construir un datapad.", ch,
         NULL, argument , TO_ROOM );

    {
         long xpgain;

         xpgain = UMIN( obj->cost*10 ,( exp_level(ch->skill_level[SLICER_ABILITY]+1) - exp_level(ch->skill_level[SLICER_ABILITY]) )
);
         gain_exp(ch, xpgain, SLICER_ABILITY);
         ch_printf( ch , "Recibhes %d puntos de experiencia en hacker.", xpgain );
    }
    learn_from_success( ch, gsn_makedatapad );
}


void do_codecrack( CHAR_DATA *ch, char *argument )
{
  SHIP_DATA *ship;
  CHAR_DATA *victim;
  bool checkdata;
  OBJ_DATA *obj;
  int  x;
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_INPUT_LENGTH];
  int  chance;

 strcpy( arg , argument );
 checkdata = FALSE;
  switch( ch->substate )
   {
    default:
    if( argument[0] == '\0')
    {
	send_to_char("Sintaxis: rompercodigo <nave>\n\r", ch);
	return;
    }
    else if( (ship = ship_in_room(ch->in_room, arg)) != NULL)
     {
	ship = ship_in_room(ch->in_room, arg);
      	strcpy(arg, ship->name);
     }
    else
      {
    	send_to_char("No ves esa nave aquí.\n\r", ch);
    	return;
      }

     for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
      {
      	if (obj->item_type == ITEM_DATAPAD)
      	  checkdata = TRUE;
      }

     if ( checkdata == FALSE) 
      {
      	send_to_char("Necesitas un datapad para acceder a los sistemas de la nave.\n\r", ch);
      	return;
      }

     chance = IS_NPC(ch) ? ch->top_level
        : (int) (ch->pcdata->learned[gsn_codecrack]);
     if ( number_percent( ) < chance )
      {

    	if ( (ship = ship_in_room( ch->in_room , arg )) != NULL)
    	 {
    	  ship = get_ship(arg);
    	  ch->dest_buf   = str_dup(arg);
       	  send_to_char( "&Gcomienzas el largo proceso para conseguir acceso al sistema de seguridad.\n\r", ch);
    	  sprintf(buf, "$n conecta su datapad a un puerto de %s.\n\r", ship->name);
     	  act( AT_PLAIN, buf, ch, NULL, argument , TO_ROOM );
     	  add_timer ( ch , TIMER_DO_FUN , 25 , do_codecrack , 1 );
    	  return;
    	 }
    	else
    	 {
	   send_to_char("Esa nave no está aquí.\n\r", ch);
	   return;
    	 }
      }

	send_to_char("&RNo consigues encontrar el puerto adecuado en la nave.\n\r",ch);
	ch->pcdata->is_hacking = FALSE;
	learn_from_failure( ch, gsn_codecrack );
        return;

        case 1:
    	  if ( !ch->dest_buf )
    	     return;
    	  strcpy(arg, ch->dest_buf);
    	  DISPOSE( ch->dest_buf);
    	  break;

    	case SUB_TIMER_DO_ABORT:
    	  DISPOSE( ch->dest_buf );
    	  ch->substate = SUB_NONE;
    	  ch->pcdata->is_hacking = FALSE;
    	  send_to_char("&RTe distraes y no finalizas el trabajo.\n\r", ch);
    	  return;
       }

     ch->substate = SUB_NONE;

     for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
      {
         if (obj->item_type == ITEM_DATAPAD)
           checkdata = TRUE;
      }
     chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_codecrack]);

    ship = ship_in_room(ch->in_room,arg);
    if (!ship)
    { 
    send_to_char ("&RLa nave se ha ido.\n\r", ch); 
    return; 
    }
    x = number_percent( );
    if (ship->alarm == 1)
      x = x * 2;
    if ( x > chance*2  || !checkdata  || ( !ship ) )
     {
        ch_printf(ch,"&B[+-----+-----+-----+-----+-----+-----+-----+-----+-]&W\r\n");
        ch_printf(ch,"&B[&r^O Terminal de&W^O: %-30.30s &B^x]&W^x\r\n",ship->name);
        ch_printf(ch,"&B[&r^O Login           &W^O: %-20.20s           &B^x]&W^x\r\n",ch->name);
        ch_printf(ch,"&B[&r^O Clave     &W^O: %-10d                     &B^x]&W^x\r\n",number_range(0,999999));
        ch_printf(ch,"&B[&r^O Accediendo  &W^O: Espere                       &B^x]&W^x\r\n");
        ch_printf(ch,"&B[&r^O                                                  &B^x]&W^x\r\n");
        ch_printf(ch,"&B[&r^O                                                  &B^x]&W^x\r\n");
        ch_printf(ch,"&B[&r^O                                                  &B^x]&W^x\r\n");
        ch_printf(ch,"&B[&r^O                                                  &B^x]&W^x\r\n");
        ch_printf(ch,"&B[&r^O Acceso     &W^O: Denegado                         &B^x]&W^x\r\n");
        ch_printf(ch,"&B[+-----+-----+-----+-----+-----+-----+-----+-----+-]&W\r\n");
       learn_from_failure( ch, gsn_codecrack );
       if (ship->alarm == 1)
        {
         if((victim = get_char_world_ooc(ch, ship->owner)) != NULL) 
           ch_printf(victim, "&RRecibes una señal del módulo de alarma de %s.&W", ship->name);
        }
       ch->pcdata->is_hacking = FALSE;
       return;
     }
    ch->pcdata->is_hacking = FALSE;
    if (!ship->password)
     {
      sprintf(buf, "Error... %s no tiene contraseña.\n\r", ship->name);
      send_to_char(buf, ch);
      log_string(buf);
      return;
     }
      {
      long xpgain;
      int pssword;
       if ( IS_SET(ship->flags, SHIP_NOSLICER) )
           pssword = number_range(1111,9999);
       else
           pssword = ship->password;

        ch_printf(ch,"&B[+-----+-----+-----+-----+-----+-----+-----+-----+-]&W\r\n");
        ch_printf(ch,"&B[&r^O Terminal de&W^O: %-30.30s &B^x]&W^x\r\n",ship->name);
        ch_printf(ch,"&B[&r^O Login           &W^O: %-20.20s           &B^x]&W^x\r\n",ch->name);
        ch_printf(ch,"&B[&r^O Clave     &W^O: %-10d                     &B^x]&W^x\r\n",number_range(0,999999));
        ch_printf(ch,"&B[&r^O Accediendo  &W^O: Espere                       &B^x]&W^x\r\n");
        ch_printf(ch,"&B[&r^O                                                  &B^x]&W^x\r\n");
        ch_printf(ch,"&B[&r^O                                                  &B^x]&W^x\r\n");
        ch_printf(ch,"&B[&r^O                                                  &B^x]&W^x\r\n");
        ch_printf(ch,"&B[&r^O                                                  &B^x]&W^x\r\n");
        ch_printf(ch,"&B[&r^O Acceso     &W^O: Autorizado                        &B^x]&W^x\r\n");
        ch_printf(ch,"&B[&r^O Contraseña de la nave   &W^O: %-7d                        &B^x]&W^x\r\n",pssword);
        ch_printf(ch,"&B[+-----+-----+-----+-----+-----+-----+-----+-----+-]&W\r\n");
      //xpgain = UMIN( obj->cost*10 ,( exp_level(ch->skill_level[SLICER_ABILITY]+1) - exp_level(ch->skill_level[SLICER_ABILITY]) ) );
      xpgain = 3000;
      ch->pcdata->is_hacking = FALSE;
      gain_exp(ch, xpgain, SLICER_ABILITY);
      ch_printf( ch , " Recibes %d puntos de experiencia en hacker.\n\r", xpgain );
      learn_from_success( ch, gsn_codecrack ) ;
      }
      return;

}

void do_disableship ( CHAR_DATA *ch, char *argument )
{

  SHIP_DATA *ship1;
  SHIP_DATA *ship2;
  SHIP_DATA *ship;
  int chance,x;
  bool checkcomm,checkdata;
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  long xpgain;
  checkcomm = FALSE;
  checkdata = FALSE;
  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
 switch( ch->substate )
 {
  default:
  if (arg1[0] == '\0' || arg2[0] == '\0')
  {
    send_to_char("Sintaxis: desactivarnave <nave> <sistema>\n\rsistema puede ser: escudos, primario, hyper, lanzadores.\n\r", ch);
    return;
  }

  if ((ship1 = ship_from_cockpit(ch->in_room->vnum)) == NULL)
  {
    send_to_char("Debes estar en el asiento del piloto de una nave para usar esta habilidad.\n\r", ch);
    return;
  }

  if (str_cmp(arg2, "primario") && str_cmp(arg2, "escudos") && str_cmp(arg2, "lanzadores") && str_cmp(arg2, "hyper"))
  {
    send_to_char("Necesitas especificar el sistema a desactivar. Por favor escoge uno:\n\rprimario, escudos, lanzadores, hyper.\n\r", ch);
    return;
  }
  
  if (!ship1->starsystem){
  	send_to_char("solo puedes hacer esto en espacio real.?\n\r", ch);
  	return;
  }

  if( IS_SET(ship1->flags, SHIP_SIMULATOR))
  {
    send_to_char("&R¿En serio? ¡Estás en un simulador!\n\r", ch);
    return;
  }


  ship2 = get_ship_here(arg1, ship1->starsystem);

  if (!ship2)
  {
    send_to_char("Esa nave no está en este sistema estelar.\n\r", ch);
    return;
  }

  if (ship2->class == 3)
  {
    send_to_char("Esa nave tiene mucha seguridad para tratar de desactivarla.\n\r", ch);
    return;
  }

  for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
  {
    if (obj->item_type == ITEM_DATAPAD)
      checkdata = TRUE;
    if (obj->item_type == ITEM_COMMSYSTEM)
      checkcomm = TRUE;
  }

  if (!checkdata)
  {
    send_to_char("necesitas un Datapad.\n\r", ch);
    return;
  }

  if (!checkcomm)
  {
    send_to_char("necesitas un comunicador.\n\r", ch);
    return;
  }

  chance = IS_NPC(ch) ? ch->top_level
   : (int) (ch->pcdata->learned[gsn_disable]);
 if ( number_percent( ) < chance )
 {
  if ( !str_cmp(arg2, "lanzadores") )
  {
    /*Ship Launcher Disable Code*/
    strcpy(disable, arg2);
    send_to_char("usas tu datapad para tratar de desactivar la nave.\n\r", ch);
    act(AT_PLAIN, "$n usa su datapad.\n\r", ch, NULL, NULL, TO_ROOM);
    ch->dest_buf   = str_dup(arg1);
    add_timer ( ch , TIMER_DO_FUN , 5 , do_disableship , 1 );
    return;
  }

  if ( !str_cmp(arg2, "escudos") )
  {
    strcpy(disable, arg2);
    send_to_char("usas tu datapad para tratar de desactivar la nave.\n\r", ch);
    act(AT_PLAIN, "$n usa su datapad.\n\r", ch, NULL, NULL, TO_ROOM);
    ch->dest_buf   = str_dup(arg1);
    add_timer ( ch , TIMER_DO_FUN , 15 , do_disableship , 1 );
    return;
  }

  if ( !str_cmp(arg2, "hyper") )
  {
    strcpy(disable, arg2);
    send_to_char("usas tu datapad para tratar de desactivar la nave.\n\r", ch);
    act(AT_PLAIN, "$n usa su datapad.\n\r", ch, NULL, NULL, TO_ROOM);
    ch->dest_buf   = str_dup(arg1);
    add_timer ( ch , TIMER_DO_FUN , 10 , do_disableship , 1 );
    return;
  }

  if ( !str_cmp(arg2, "primario") )
  {
    strcpy(disable, arg2);
    send_to_char("usas tu datapad para tratar de desactivar la nave.\n\r", ch);
    act(AT_PLAIN, "$n usa su datapad.\n\r", ch, NULL, NULL, TO_ROOM);
    ch->dest_buf   = str_dup(arg1);
    add_timer ( ch , TIMER_DO_FUN , 30 , do_disableship , 1 );
    return;
  }
 }
   send_to_char("&RNo consigues desactivar la nave.\n\r",ch);
   learn_from_failure( ch, gsn_disable );
  return;

   case 1:
   if ( !ch->dest_buf )
   return;
   strcpy(arg1, ch->dest_buf);
   DISPOSE( ch->dest_buf);
   break;

   case SUB_TIMER_DO_ABORT:
   DISPOSE( ch->dest_buf );
   ch->substate = SUB_NONE;
   send_to_char("&RTe distraes y no consigues terminar tu trabajo.\n\r", ch);
   return;
 }

  for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
  {
    if (obj->item_type == ITEM_DATAPAD)
      checkdata = TRUE;
    if (obj->item_type == ITEM_COMMSYSTEM)
      checkcomm = TRUE;
  }

  chance = IS_NPC(ch) ? ch->top_level
   : (int) (ch->pcdata->learned[gsn_disable]);

  ship = get_ship(arg1);
  x = number_percent( );

  if ( number_percent( ) > chance*2  || (!checkdata) || (!checkcomm) || ( !ship ) )
  {
    ch_printf(ch,"&B[+-----+-----+-----+-----+-----+-----+-----+-----+-]&W\r\n");
    ch_printf(ch,"&B[&r^O Terminal inicial de&W^O: %-30.30s &B^x]&W^x\r\n",ship->name);
    ch_printf(ch,"&B[&r^O Login           &W^O: %-20.20s           &B^x]&W^x\r\n",ch->name);
    ch_printf(ch,"&B[&r^O clave     &W^O: %-10d                     &B^x]&W^x\r\n",number_range(0,999999));
    ch_printf(ch,"&B[&r^O Accediendo  &W^O: Espere                       &B^x]&W^x\r\n");
    ch_printf(ch,"&B[&r^O                                                  &B^x]&W^x\r\n");
    ch_printf(ch,"&B[&r^O                                                  &B^x]&W^x\r\n");
    ch_printf(ch,"&B[&r^O                                                  &B^x]&W^x\r\n");
    ch_printf(ch,"&B[&r^O                                                  &B^x]&W^x\r\n");
    ch_printf(ch,"&B[&r^O Acceso     &W^O: Denegado                         &B^x]&W^x\r\n");
    ch_printf(ch,"&B[+-----+-----+-----+-----+-----+-----+-----+-----+-]&W\r\n");
    learn_from_failure( ch, gsn_disable );
    return;
  }
/*for ( roomnum = ship->firstroom ; roomnum <= ship->lastroom ; roomnum++ )
{
  room = get_room_index(roomnum);*/
  if ( IS_SET(ship->flags, SHIP_NOSLICER))// &&  ch->pcdata->in_room != room)
  {
    ch_printf(ch,"&B[+-----+-----+-----+-----+-----+-----+-----+-----+-]&W\r\n");
    ch_printf(ch,"&B[&r^O Terminal inicial de&W^O: %-30.30s &B^x]&W^x\r\n",ship->name);
    ch_printf(ch,"&B[&r^O Login           &W^O: %-20.20s           &B^x]&W^x\r\n",ch->name);
    ch_printf(ch,"&B[&r^O clave     &W^O: %-10d                     &B^x]&W^x\r\n",number_range(0,999999));
    ch_printf(ch,"&B[&r^O Accediendo  &W^O: Espere                       &B^x]&W^x\r\n");
    ch_printf(ch,"&B[&r^O                                                  &B^x]&W^x\r\n");
    ch_printf(ch,"&B[&r^O                                                  &B^x]&W^x\r\n");
    ch_printf(ch,"&B[&r^O                                                  &B^x]&W^x\r\n");
    ch_printf(ch,"&B[&r^O                                                  &B^x]&W^x\r\n");
    ch_printf(ch,"&B[&r^O Acceso     &W^O: Denegado - Login Desactivado        &B^x]&W^x\r\n");
    ch_printf(ch,"&B[+-----+-----+-----+-----+-----+-----+-----+-----+-]&W\r\n");
    learn_from_failure( ch, gsn_disable );
    return;
  }
// }

        if (!str_cmp(disable, "escudos") )
        {
          ship->autorecharge = FALSE;
          ship->shield = 0;
        }

        if (!str_cmp(disable, "lanzadores") )
        {
	  ship->missilestate = MISSILE_DAMAGED;
        }

        if (!str_cmp(disable, "primary") )
        {
          ship->primaryState = LASER_DAMAGED;
        }

        if (!str_cmp(disable, "hyper") )
        {
          ship->hyperstate = LASER_DAMAGED;
        }

        ch_printf(ch,"&B[+-----+-----+-----+-----+-----+-----+-----+-----+-]&W\r\n");
        ch_printf(ch,"&B[^O &rTerminal inicial de&W: %-30.30s &B^x]&W^x\r\n",ship->name);
        ch_printf(ch,"&B[^O &rLogin           &W: %-20.20s           &B^x]&W^x\r\n",ch->name);
        ch_printf(ch,"&B[^O &rclave     &W: %-10d                     &B^x]&W^x\r\n",number_range(0,999999));
        ch_printf(ch,"&B[^O &rAccediendo  &W: Por favor espere                       &B^x]&W^x\r\n");
        ch_printf(ch,"&B[^O                                                  &B^x]&W^x\r\n");
        ch_printf(ch,"&B[^O                                                  &B^x]&W^x\r\n");
        ch_printf(ch,"&B[^O                                                  &B^x]&W^x\r\n");
        ch_printf(ch,"&B[^O                                                  &B^x]&W^x\r\n");
        ch_printf(ch,"&B[^O &rAcceso     &W: Autorizado                        &B^x]&W^x\r\n");
        ch_printf(ch,"&B[^O &rCorrecto. %-8.8s   &W: Desactivado                       &B^x]&W^x\r\n",disable);
        ch_printf(ch,"&B[+-----+-----+-----+-----+-----+-----+-----+-----+-]&W\r\n");
      //xpgain = UMIN( obj->cost*10 ,( exp_level(ch->skill_level[SLICER_ABILITY]+1) - exp_level(ch->skill_level[SLICER_ABILITY]) ) );
      xpgain = 3000;
      learn_from_success( ch, gsn_disable ) ;
      gain_exp(ch, xpgain, SLICER_ABILITY);
      ch_printf( ch , " Recibes %d puntos de experiencia en hacker.\n\r", xpgain );
      return;


}

void do_assignpilot ( CHAR_DATA *ch, char *argument )
{

  SHIP_DATA *ship1;
  SHIP_DATA *ship;
  int chance;
  long xpgain;
  bool checkdata;
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  checkdata = FALSE;
  argument = one_argument(argument, arg1);
 switch( ch->substate )
 {
  default:
  if (arg1[0] == '\0')
  {
    send_to_char("Sintaxis: asignarpiloto <nombre>\n\r", ch);
    return;
  }

  if ((ship1 = ship_from_cockpit(ch->in_room->vnum)) == NULL)
  {
    send_to_char("Debes estar en el asiento del piloto para usar esta habilidad.\n\r", ch);
    return;
  }


  for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
  {
    if (obj->item_type == ITEM_DATAPAD)
      checkdata = TRUE;
  }

  if (!checkdata)
  {
    send_to_char("Necesitas un datapad.\n\r", ch);
    return;
  }

  if( autofly(ship1))
  {
    send_to_char("&W&REl sistema de autopiloto debe estar desactivado.\n\r", ch);
    return;
  }

  chance = IS_NPC(ch) ? ch->top_level
   : (int) (ch->pcdata->learned[gsn_assignpilot]);
 if ( number_percent( ) < chance )
 {   
    strcpy(disable, arg2);
    send_to_char("Utilizas tu datapad para intentar cambiar el piloto de la nave.\n\r", ch);
    act(AT_PLAIN, "$n se pone a trabajar en algo con su datapad.\n\r", ch, NULL, argument, TO_ROOM);
    ch->dest_buf   = str_dup(arg1);
    add_timer ( ch , TIMER_DO_FUN , 5 , do_assignpilot , 1 );
    return;
 }
   send_to_char("&RNo eres capaz.\n\r",ch);
   learn_from_failure( ch, gsn_assignpilot );
  return;

   case 1:
   if ( !ch->dest_buf )
   return;
   strcpy(arg1, ch->dest_buf);
   DISPOSE( ch->dest_buf);
   break;

   case SUB_TIMER_DO_ABORT:
   DISPOSE( ch->dest_buf );
   ch->substate = SUB_NONE;
   send_to_char("&RTe distraes y no completas tu trabajo.\n\r", ch);
   return;
 }

  for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
  {
    if (obj->item_type == ITEM_DATAPAD)
      checkdata = TRUE;
  }

  chance = IS_NPC(ch) ? ch->top_level
   : (int) (ch->pcdata->learned[gsn_assignpilot]);

  ship = ship_from_cockpit(ch->in_room->vnum);
  if (ship == NULL) return;
  if ( number_percent( ) > chance*2  || (!checkdata) || ( !ship ) )
  {
    ch_printf(ch,"&B[+-----+-----+-----+-----+-----+-----+-----+-----+-]&W\r\n");
    ch_printf(ch,"&B[&r^O Terminal de inicio&W^O: %-30.30s &B^x]&W^x\r\n",ship->name);
    ch_printf(ch,"&B[&r^O Login           &W^O: %-20.20s           &B^x]&W^x\r\n",ch->name);
    ch_printf(ch,"&B[&r^O Clave     &W^O: %-10d                     &B^x]&W^x\r\n",number_range(0,999999));
    ch_printf(ch,"&B[&r^O Accediendo  &W^O: Espere                       &B^x]&W^x\r\n");
    ch_printf(ch,"&B[&r^O                                                  &B^x]&W^x\r\n");
    ch_printf(ch,"&B[&r^O                                                  &B^x]&W^x\r\n");
    ch_printf(ch,"&B[&r^O                                                  &B^x]&W^x\r\n");
    ch_printf(ch,"&B[&r^O                                                  &B^x]&W^x\r\n");
    ch_printf(ch,"&B[&r^O Acceso     &W^O: Denegado                         &B^x]&W^x\r\n");
    ch_printf(ch,"&B[+-----+-----+-----+-----+-----+-----+-----+-----+-]&W\r\n");
    learn_from_failure( ch, gsn_assignpilot );
    return;
  }
       if ( IS_SET(ship->flags, SHIP_NOSLICER) )
       {
    ch_printf(ch,"&B[+-----+-----+-----+-----+-----+-----+-----+-----+-]&W\r\n");
    ch_printf(ch,"&B[&r^O Terminal de inicio&W^O: %-30.30s &B^x]&W^x\r\n",ship->name);
    ch_printf(ch,"&B[&r^O Login           &W^O: %-20.20s           &B^x]&W^x\r\n",ch->name);
    ch_printf(ch,"&B[&r^O Clave     &W^O: %-10d                     &B^x]&W^x\r\n",number_range(0,999999));
    ch_printf(ch,"&B[&r^O Accediendo  &W^O: Espere                       &B^x]&W^x\r\n");
    ch_printf(ch,"&B[&r^O                                                  &B^x]&W^x\r\n");
    ch_printf(ch,"&B[&r^O                                                  &B^x]&W^x\r\n");
    ch_printf(ch,"&B[&r^O                                                  &B^x]&W^x\r\n");
    ch_printf(ch,"&B[&r^O                                                  &B^x]&W^x\r\n");
    ch_printf(ch,"&B[&r^O Acceso     &W^O: Denegado - Login desactivado        &B^x]&W^x\r\n");
    ch_printf(ch,"&B[+-----+-----+-----+-----+-----+-----+-----+-----+-]&W\r\n");
    learn_from_failure( ch, gsn_assignpilot );
    return;
  }

     STRFREE( ship->pilot );
     ship->pilot = STRALLOC( arg1 );

        ch_printf(ch,"&B[+-----+-----+-----+-----+-----+-----+-----+-----+-]&W\r\n");
        ch_printf(ch,"&B[&r^O Terminal de inicio&W^O: %-30.30s &B^x]&W^x\r\n",ship->name);
       ch_printf(ch,"&B[&r^O Login           &W^O: %-20.20s           &B^x]&W^x\r\n",ch->name);

        ch_printf(ch,"&B[&r^O Clave     &W^O: %-10d                     &B^x]&W^x\r\n",number_range(0,999999));
        ch_printf(ch,"&B[&r^O Accediendo  &W^O: Espere                       &B^x]&W^x\r\n");
        ch_printf(ch,"&B[&r^O                                                  &B^x]&W^x\r\n");
        ch_printf(ch,"&B[&r^O                                                  &B^x]&W^x\r\n");
        ch_printf(ch,"&B[&r^O                                                  &B^x]&W^x\r\n");
        ch_printf(ch,"&B[&r^O                                                  &B^x]&W^x\r\n");
        ch_printf(ch,"&B[&r^O acceso     &W^O: Autorizado                        &B^x]&W^x\r\n");
        ch_printf(ch,"&B[&r^O Piloto añadido a la nave&W^O: %-10.10s                     &B^x]&W^x\r\n", arg1);
        ch_printf(ch,"&B[+-----+-----+-----+-----+-----+-----+-----+-----+-]&W\r\n");
      //xpgain = UMIN( obj->cost*10 ,( exp_level(ch->skill_level[SLICER_ABILITY]+1) - exp_level(ch->skill_level[SLICER_ABILITY]) ) );
      xpgain = 3000;
      learn_from_success( ch, gsn_assignpilot ) ;
      gain_exp(ch, xpgain, SLICER_ABILITY);
      ch_printf( ch , "Recibes %d puntos de experiencia en hacker.\n\r", xpgain );
      return;


}

void do_slicebank(CHAR_DATA *ch, char *argument )
{
  DESCRIPTOR_DATA *d;
  bool checkdata;
  OBJ_DATA *obj;
  long xpgain;
  char arg[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char buf[MAX_INPUT_LENGTH];
  long steal;
  int  chance;
  bool found;

  
  argument = one_argument(argument, arg2);
  strcpy( arg , argument );
  checkdata = FALSE;
  switch( ch->substate )
   {
    default:
    if ( arg[0] == '\0' || arg2[0] == '\0')
      {
       send_to_char("sintaxis: hackearbanco <cuenta> <cantidad>\n\r", ch);
    	return;
      }

     if ( ch->fighting )
      {
       send_to_char( "Termina el combate primero...\n\r", ch );
       return;
      }


     if(!IS_SET(ch->in_room->room_flags, ROOM_BANK))
      {
		send_to_char("Debes estar en un banco.\n\r", ch);
		return;
      }

     for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
      {
      	if (obj->item_type == ITEM_DATAPAD)
      	  checkdata = TRUE;
      }
     if ( ( checkdata == FALSE ) )
      {
      	send_to_char("necesitas un datapad.\n\r", ch);
      	return;
      }
     if(!str_cmp(arg2, acctname(ch)))
     {
      ch_printf(ch, "Esta es tu cuenta. Bonita forma de tratar de engañar al sistema de %s.\n\r", sysdata.mud_acronym);
      return;
     }
  
    if ( atoi(arg) < 0)
    {
	send_to_char( "¿Cuánto quieres robar?\n\r", ch);
	return;
    }
 
    	  ch->dest_buf   = str_dup(arg);
    	  ch->dest_buf_2   = str_dup(arg2);
       	  send_to_char( "&GComienzas el largo proceso de romper la seguridad del banco.\n\r", ch);
    	  sprintf(buf, "$n conecta su datapad a un puerto de datos." );
     	  act( AT_PLAIN, buf, ch, NULL, argument , TO_ROOM );
     	  add_timer ( ch , TIMER_DO_FUN , 10 , do_slicebank , 1 );
    	  return;

        case 1:
    	  if ( !ch->dest_buf )
    	     return;
    	  if ( !ch->dest_buf_2 )
    	     return;
    	     
    	  strcpy(arg, ch->dest_buf);
    	  strcpy(arg2, ch->dest_buf_2);
    	  DISPOSE( ch->dest_buf);
    	  DISPOSE( ch->dest_buf_2);
    	  break;

    	case SUB_TIMER_DO_ABORT:
    	  DISPOSE( ch->dest_buf );
    	  DISPOSE( ch->dest_buf_2 );
    	  ch->substate = SUB_NONE;
    	  send_to_char("&RTe distraes y no terminas tu trabajo.\n\r", ch);
    	  return;
       }

     ch->substate = SUB_NONE;
     
     for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
      {
         if (obj->item_type == ITEM_DATAPAD)
           checkdata = TRUE;
      }
     chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_slicebank]);
     chance = UMIN(chance, 70);
      found = FALSE;

	for(d = first_descriptor; d; d = d->next)
    {
     if(!d->character) continue;
     if(d->connected != CON_PLAYING) continue;
     if(IS_IMMORTAL(d->character)) continue;

     if(!str_cmp(arg2, acctname(d->character))){
		found = TRUE;
		break;}
    }
   if ( number_percent( ) > chance )
   {
		ch_printf(ch,"&z|+---------------------------------------------------------------------+|&w\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|&x^g Bienvenido al sistema bancario galáctico. Entradas no autorizadas prohibidas. &z^x|\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|&x^g Login: %d                                                          &z^x|\n\r",number_range(11111,99999));
		ch_printf(ch,"&z|&x^g Clave: *********                                                     &z^x|\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|&x^g clave inválida.                                                      &z^x|\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|+---------------------------------------------------------------------+|&w\n\r");
       learn_from_failure( ch, gsn_slicebank );
       return;
    }
   if ( number_percent( ) > chance*2 && found)
     {
		ch_printf(ch,"&z|+---------------------------------------------------------------------+|&w\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|&x^g Bienvenido al sistema bancario galáctico. Entradas no autorizadas prohibidas. &z^x|\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|&x^g Login: %d                                                          &z^x|\n\r",number_range(11111,99999));
		ch_printf(ch,"&z|&x^g Clave: *********                                                     &z^x|\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|&x^g Login correcto...recuperando información de cuentas, espere.            &z^x|\n\r");
		ch_printf(ch,"&z|&x^g procesando solicitud, Espere.                                         &z^x|\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
        ch_printf(ch,"&z|&x^g solicitud denegada. el dueño de la cuenta ha sido notificado.                      &z^x|\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|+---------------------------------------------------------------------+|&w\n\r");

       learn_from_failure( ch, gsn_slicebank );
       send_to_char("&R[&YBanco: &WALERTA&R] &WSe ha producido una solicitud de transferencia sospechosa en su cuenta bancaria.\n\r", d->character);
       return;
     }
    if(!found)
    {
		ch_printf(ch,"&z|+---------------------------------------------------------------------+|&w\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|&x^g Bienvenido al sistema bancario galáctico. Entradas no autorizadas prohibidas. &z^x|\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|&x^g Login: %d                                                          &z^x|\n\r",number_range(11111,99999));
		ch_printf(ch,"&z|&x^g Clave: *********                                                     &z^x|\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|&x^g Login correcto...recuperando información de cuentas, espere.            &z^x|\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
        ch_printf(ch,"&z|&x^g Cuenta %-15.15s desactivada.                                &z^x|\n\r", arg2);
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|+---------------------------------------------------------------------+|&w\n\r");
        return;
     }

    steal = atoi(arg);
    if (steal > d->character->pcdata->bank/20)
    {
		ch_printf(ch,"&z|+---------------------------------------------------------------------+|&w\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|&x^g Bienvenido al sistema bancario galáctico. Entradas no autorizadas prohibidas. &z^x|\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|&x^g Login: %d                                                          &z^x|\n\r",number_range(11111,99999));
		ch_printf(ch,"&z|&x^g Clave: *********                                                     &z^x|\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|&x^g Login correcto...recuperando información de cuentas, espere.            &z^x|\n\r");
		ch_printf(ch,"&z|&x^g Procesando solicitud, espere.                                         &z^x|\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
        ch_printf(ch,"&z|&x^g solicitud denegada, transferencia demasiado alta. Se ha notificado al propietario de la cuenta.   &z^x|\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|+---------------------------------------------------------------------+|&w\n\r");
       send_to_char("&R[&YBanco: &WALERTA&R] &Wha habido un intento sospechoso de transferencia en su cuenta.\n\r", d->character);  	
        return;
    }	

		ch_printf(ch,"&z|+---------------------------------------------------------------------+|&w\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|&x^g Bienvenido al sistema bancario galáctico. Entradas no autorizadas prohibidas. &z^x|\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|&x^g Login: %d                                                          &z^x|\n\r",number_range(11111,99999));
		ch_printf(ch,"&z|&x^g Clave: *********                                                     &z^x|\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|&x^g Login correcto...recuperando información de cuentas, espere.            &z^x|\n\r");
		ch_printf(ch,"&z|&x^g Procesando solicitud, espere.                                         &z^x|\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
        ch_printf(ch,"&z|&x^g solicitud aceptada. Créditos transferidos.                                &z^x|\n\r");
		ch_printf(ch,"&z|&x^g                                                                       &z^x|\n\r");
		ch_printf(ch,"&z|+---------------------------------------------------------------------+|&w\n\r");
 
        ch->pcdata->bank+=steal;
        d->character->pcdata->bank-=steal;
      //xpgain = UMIN( obj->cost*10 ,( exp_level(ch->skill_level[SLICER_ABILITY]+1) - exp_level(ch->skill_level[SLICER_ABILITY]) ) );
      xpgain = 3000;
      gain_exp(ch, xpgain, SLICER_ABILITY);
      ch_printf( ch , " recibes %d puntos de experiencia en hacker.\n\r", xpgain );
      learn_from_success( ch, gsn_slicebank);
      return;

}

void do_checkprints(CHAR_DATA *ch, char *argument )
{
  bool checkdata;
  bool checkcomm;
  OBJ_DATA *obj;
  int  x;
  long xpgain;
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_INPUT_LENGTH];
  int  chance;

  strcpy( arg , argument );
  checkdata = FALSE;
  checkcomm = FALSE;
  switch( ch->substate )
   {
    default:

     if (arg[0] == '\0')
      {
       send_to_char("Sintaxis: verificarhuellas <cuerpo>\n\r", ch);
    	return;
      }


     for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
      {
      	if (obj->item_type == ITEM_DATAPAD)
      	  checkdata = TRUE;
      	if (obj->item_type == ITEM_COMMSYSTEM)
      	  checkcomm = TRUE;
      }

     if ( ( checkdata == FALSE ) )
      {
      	send_to_char("Necesitas un datapad para acceder al ordenador de huellas.\n\r", ch);
      	return;
      }
      
     if (( checkcomm == FALSE ) ){
     	send_to_char("Necesitas un comunicador para acceder a la base de datos de huellas.\n\r", ch);
     	return;
     }

     if ( ch->fighting )
      {
       send_to_char( "¿Luchando? Buen intento.\n\r", ch );
       return;
      }

     obj = get_obj_list( ch, argument, ch->in_room->first_content );
     if (!obj){
       send_to_char("Ese cuerpo no está aquí.\n\r", ch);
       return;
     }
     
     if (obj->item_type != ITEM_CORPSE_NPC && obj->item_type != ITEM_CORPSE_PC){ 
       send_to_char("Eso no es un cuerpo.\n\r", ch);
       return;
     }
     
     if (!obj->killer){
     	send_to_char("Error: No hay asesino. Avisa a la administración urgentemente.\n\r", ch);
     	return;
     }
     
     chance = IS_NPC(ch) ? ch->top_level
        : (int) (ch->pcdata->learned[gsn_checkprints]);
     if ( number_percent( ) < chance )
      {
      	  ch->dest_buf   = str_dup(arg);
       	  send_to_char( "&GComienzas el largo proceso de verificación de huellas.\n\r", ch);
    	  sprintf(buf, "$n conecta su datapad a su comunicador.\n\r" );
     	  act( AT_PLAIN, buf, ch, NULL, argument , TO_ROOM );
     	  add_timer ( ch , TIMER_DO_FUN , 5 , do_checkprints , 1 );
    	  return;
      }
	send_to_char("&RNo consigues nada útil.\n\r",ch);
	learn_from_failure( ch, gsn_checkprints );
        return;

        case 1:
    	  if ( !ch->dest_buf )
    	     return;
    	  strcpy(arg, ch->dest_buf);
    	  DISPOSE( ch->dest_buf);
    	  break;

    	case SUB_TIMER_DO_ABORT:
    	  DISPOSE( ch->dest_buf );
    	  ch->substate = SUB_NONE;
    	  send_to_char("&RTe distraes y no terminas tu trabajo.\n\r", ch);
    	  return;
       }

     ch->substate = SUB_NONE;

     for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
      {
         if (obj->item_type == ITEM_DATAPAD)
           checkdata = TRUE;
         if (obj->item_type == ITEM_COMMSYSTEM)
           checkcomm = TRUE;
      }
     chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_checkprints]);
    
     obj = get_obj_list( ch, arg, ch->in_room->first_content );
     if (!obj){
       send_to_char("No ves ese cuerpo aquí.\n\r", ch);
       return;
     }
     
     if (obj->item_type != ITEM_CORPSE_NPC && obj->item_type != ITEM_CORPSE_PC){ 
       send_to_char("Eso no es un cuerpo.\n\r", ch);
       return;
     }
     
     if (!obj->killer){
     	send_to_char("Error: No hay asesino. Avisa urgentemente a la administración.\n\r", ch);
     	return;
     }
    x = number_percent( );

    if ( number_percent( ) > chance*2 )
     {
        ch_printf(ch,"&B[+-----+-----+-----+-----+-----+-----+-----+-----+-]&W\r\n");
        ch_printf(ch,"&B[^O &rTerminal iniciada&W: %-30.30s &B^x]&W^x\r\n",obj->name);
        ch_printf(ch,"&B[^O &rLogin           &W: %-20.20s           &B^x]&W^x\r\n",ch->name);
        ch_printf(ch,"&B[^O &rClave     &W: %-10d                     &B^x]&W^x\r\n",number_range(0,999999));
        ch_printf(ch,"&B[^O &rAccediendo  &W: Espere                       &B^x]&W^x\r\n");
        ch_printf(ch,"&B[^O                                                  &B^x]&W^x\r\n");
        ch_printf(ch,"&B[^O                                                  &B^x]&W^x\r\n");
        ch_printf(ch,"&B[^O                                                  &B^x]&W^x\r\n");
        ch_printf(ch,"&B[^O                                                  &B^x]&W^x\r\n");
        ch_printf(ch,"&B[^O &rAcceso al sistema     &W: Denegado                         &B^x]&W^x\r\n");
        ch_printf(ch,"&B[+-----+-----+-----+-----+-----+-----+-----+-----+-]&W\r\n");
       learn_from_failure( ch, gsn_checkprints );
       return;
     }
      

        ch_printf(ch,"&B[+-----+-----+-----+-----+-----+-----+-----+-----+-]&W\r\n");
        ch_printf(ch,"&B[^O &rTerminal iniciada&W: %-30.30s &B^x]&W^x\r\n",obj->name);
        ch_printf(ch,"&B[^O &rLogin           &W: %-20.20s           &B^x]&W^x\r\n",ch->name);
        ch_printf(ch,"&B[^O &rClave     &W: %-10d                     &B^x]&W^x\r\n",number_range(0,999999));
        ch_printf(ch,"&B[^O &rAccediendo  &W: Espere                       &B^x]&W^x\r\n");
        ch_printf(ch,"&B[^O                                                  &B^x]&W^x\r\n");
        ch_printf(ch,"&B[^O                                                  &B^x]&W^x\r\n");
        ch_printf(ch,"&B[^O                                                  &B^x]&W^x\r\n");
        ch_printf(ch,"&B[^O                                                  &B^x]&W^x\r\n");
        ch_printf(ch,"&B[^O &rAcceso al sistema     &W: Autorizado                        &B^x]&W^x\r\n");
        ch_printf(ch,"&B[^O &rVerificación de huellas     &W: %-15.15s                &B^x]&W^x\r\n", obj->killer ? obj->killer : "Error, Show imm");
        ch_printf(ch,"&B[+-----+-----+-----+-----+-----+-----+-----+-----+-]&W\r\n");
      //xpgain = UMIN( obj->cost*10 ,( exp_level(ch->skill_level[SLICER_ABILITY]+1) - exp_level(ch->skill_level[SLICER_ABILITY]) ) );
      xpgain = 3000;
      gain_exp(ch, xpgain, SLICER_ABILITY);
      ch_printf( ch , " Recibes %d puntos de experiencia en hacker.\n\r", xpgain );
      learn_from_success( ch, gsn_checkprints) ;
      return;

}
