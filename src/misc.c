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

extern int	top_exit;

void do_buyhome( CHAR_DATA *ch, char *argument )
{
     ROOM_INDEX_DATA *room;
     AREA_DATA *pArea;
     
     if ( !ch->in_room )
         return;
         
     if ( IS_NPC(ch) || !ch->pcdata )
         return;
         
     if ( ch->plr_home != NULL )
     {
         send_to_char( "&RYa tienes una casa!\n\r&w", ch);
         return;   
     }
     
     room = ch->in_room;
     
     for ( pArea = first_bsort; pArea; pArea = pArea->next_sort )
     {
         if ( room->area == pArea )
         {
             send_to_char( "&RThis area isn't installed yet!\n\r&w", ch);
             return;
         }
     }
     
     if ( !IS_SET( room->room_flags , ROOM_EMPTY_HOME ) )
     {
         send_to_char( "&R¡Este lugar no está a la venta!\n\r&w", ch);
         return;   
     }
     
     if ( ch->gold < 25000 )
     {
         send_to_char( "&R¡Cuesta 25000 créditos y no tienes tantos!\n\r&w", ch);
         return;   
     }
     
     if ( argument[0] == '\0' )
     {
	   send_to_char( "Teclea también el nombre de la casa. Una breve línea descriptiva.\n\r", ch );
	   send_to_char( "Uso: comprarcasa <Nombre de la casa>\n\r", ch );
	   return;
     }
      
     STRFREE( room->name );
     room->name = STRALLOC( argument );

     ch->gold -= 25000;
     
     REMOVE_BIT( room->room_flags , ROOM_EMPTY_HOME );
     SET_BIT( room->room_flags , ROOM_PLR_HOME );
     
     fold_area( room->area, room->area->filename, FALSE );

     ch->plr_home = room;
     do_save( ch , "" );

     send_to_char("&GCompras tu casa por 25000 créditos.\n\r", ch);
}

void do_sellhome( CHAR_DATA *ch, char *argument )
{
     ROOM_INDEX_DATA *room = ch->plr_home;
     AREA_DATA *pArea;

     if ( !ch->in_room )
         return;
         
     if ( IS_NPC(ch) || !ch->pcdata )
         return;
         
     room = ch->in_room;
	 
     if ( ch->plr_home != room )
     {
         send_to_char( "&R¡No eres el propietario!\n\r&w", ch);
         return;   
     }
     
     for ( pArea = first_bsort; pArea; pArea = pArea->next_sort )
     {
         if ( room->area == pArea )
         {
             send_to_char( "&RThis area isn't installed yet!\n\r&w", ch);
             return;
         }
     }
     
     if ( !IS_SET( room->room_flags , ROOM_PLR_HOME ) )
     {
         send_to_char( "&R¡No es de nadie!\n\r&w", ch);
         return;   
     }
     
      STRFREE( room->name );
      room->name = STRALLOC( "An Empty Apartment" );
      ch->gold += 10000;
      REMOVE_BIT( room->room_flags , ROOM_PLR_HOME );
      SET_BIT( room->room_flags , ROOM_EMPTY_HOME );
     
      fold_area( room->area, room->area->filename, FALSE );     

     ch->plr_home = NULL;
     do_save( ch , "" );

     send_to_char("&GVendes tu casa por 10000 créditos.\n\r", ch);

}

void do_clone( CHAR_DATA *ch, char *argument )
{
     long credits, bank;
     long played;
     int frclevel, frc;
     int salary;
     char clanname[MAX_STRING_LENGTH];
     char bestowments[MAX_STRING_LENGTH];
     int flags;
     ROOM_INDEX_DATA *home;
     
     if ( IS_NPC(ch) )
     {
       ch_printf( ch, "¡Los mobs no necesitan clonarse!\n\r" );
       return;
     }
     
     if ( !IS_IMMORTAL(ch) && ch->in_room && !IS_SET( ch->in_room->room_flags , ROOM_HOTEL ) && !NOT_AUTHED(ch) )
     {
       ch_printf( ch, "¡Aquí no hay ningún dispositivo de clonación!\n\r" );
       return;
     }

     if( IS_DROID(ch))
     {
	ch_printf(ch,"Los droides no se clonan, ¡Ellos hacen &W&RBackup&w&W!\n\r");
	return;
     }

     if ( ch->rppoints < 0 )
     {
       ch_printf( ch, "Clonarse cuesta 4 puntos de rol. No tienes suficientes.\n\rAyuda puntos_rol para más información." );
       return;
     }
     else
     {
       //ch->rppoints -= 4;
            
     flags   = ch->act;
     frc = ch->perm_frc;
     frclevel = ch->skill_level[FORCE_ABILITY];
     REMOVE_BIT( ch->act, PLR_KILLER );
     credits = ch->gold;
     ch->gold = 0;
     played = ch->played;
     ch->played = ch->played/2;
     salary = ch->pcdata->salary;
     bank = ch->pcdata->bank;
     ch->pcdata->bank = 0;
     ch->pcdata->salary = 0;
     home = ch->plr_home;
     if( ch->perm_frc > 0)
      {
	if( ch->skill_level[FORCE_ABILITY] > (ch->perm_frc-2)*5)
	  ch->skill_level[FORCE_ABILITY] = (ch->perm_frc-2)*5;
	ch->perm_frc = UMAX(0,ch->perm_frc-2);
      }

     ch->plr_home = NULL;

     if ( ch->pcdata->clan_name && ch->pcdata->clan_name[0] != '\0' )
     {
         strcpy( clanname, ch->pcdata->clan_name);
         STRFREE( ch->pcdata->clan_name );
         ch->pcdata->clan_name = STRALLOC( "" );
	 if(ch->pcdata->bestowments)
	 {
           strcpy( bestowments, ch->pcdata->bestowments);
           DISPOSE( ch->pcdata->bestowments );
           ch->pcdata->bestowments = str_dup( "" );
	 }
         save_clone(ch);
         STRFREE( ch->pcdata->clan_name );
         ch->pcdata->clan_name = STRALLOC( clanname );
	 if(bestowments[0] != '\0')
	 {
           DISPOSE( ch->pcdata->bestowments );
           ch->pcdata->bestowments = str_dup( bestowments );
	 }
     } 
     else
       save_clone( ch );
     ch->pcdata->salary = salary;
     ch->plr_home = home;
     ch->played = played;
     ch->gold = credits;
     ch->pcdata->bank = bank;     
     ch->act = flags;
     ch->perm_frc = frc;
     ch->skill_level[FORCE_ABILITY] = frclevel;
     }
	 ch_printf( ch, "\n\r&Wintroduces el brazo en el lugar indicado y sientes un pinchazo cuando toman una pequeña muestra de tejido.\n\r" );
     ch_printf( ch, "&R¡Ouch!\n\r\n\r" );
     ch_printf( ch, "&WEl proceso de clonación ha finalizado con éxito.\n\r" );
     
     ch->hit--;
}

void do_backup( CHAR_DATA *ch, char *argument )
{
     long credits, bank;
     long played;
     int frclevel, frc;
     char clanname[MAX_STRING_LENGTH];
     char bestowments[MAX_STRING_LENGTH];
     int flags;
     ROOM_INDEX_DATA *home;
     
     if ( IS_NPC(ch) )
     {
       ch_printf( ch, "¡Eres un mob, no necesitas clonarte!\n\r" );
       return;
     }
     
          if ( !IS_IMMORTAL(ch) && ch->in_room && !IS_SET( ch->in_room->room_flags , ROOM_HOTEL ) && !NOT_AUTHED(ch) )
     {
       ch_printf( ch, "No ves ningún dispositivo galáctico en el que copiarte.\n\r" );
       return;
     }

     if( !IS_DROID(ch))
     {
	ch_printf( ch, "¡Solo los droides necesitan hacer backup de su software!\n\r");
	return;
     }

     if ( ch->rppoints < 0 )
     {
       ch_printf( ch, "hacer BackUp cuesta 4 puntos de rol. No tienes suficientes.\n\rAyuda puntos_rol para más información." );
       return;
     }
     else
     {
       //ch->rppoints -= 4;
     }
     flags   = ch->act;
     frc = ch->perm_frc;
     frclevel = ch->skill_level[FORCE_ABILITY];
     REMOVE_BIT( ch->act, PLR_KILLER );
     credits = ch->gold;
     ch->gold = 0;
     played = ch->played;
     ch->played = ch->played/2;
     bank = ch->pcdata->bank;
     ch->pcdata->bank = 0;
     home = ch->plr_home;
     if( ch->perm_frc > 0)
      {
	if( ch->skill_level[FORCE_ABILITY] > (ch->perm_frc-2)*5)
	  ch->skill_level[FORCE_ABILITY] = (ch->perm_frc-2)*5;
	ch->perm_frc = UMAX(0,ch->perm_frc-2);
      }

     ch->plr_home = NULL;

     if ( ch->pcdata->clan_name && ch->pcdata->clan_name[0] != '\0' )
     {
         strcpy( clanname, ch->pcdata->clan_name);
         STRFREE( ch->pcdata->clan_name );
         ch->pcdata->clan_name = STRALLOC( "" );
         strcpy( bestowments, ch->pcdata->bestowments);
         DISPOSE( ch->pcdata->bestowments );
         ch->pcdata->bestowments = str_dup( "" );
         save_clone(ch);
         STRFREE( ch->pcdata->clan_name );
         ch->pcdata->clan_name = STRALLOC( clanname );
         DISPOSE( ch->pcdata->bestowments );
         ch->pcdata->bestowments = str_dup( clanname );
     } 
     else
       save_clone( ch );
     ch->plr_home = home;
     ch->played = played;
     ch->gold = credits;
     ch->pcdata->bank = bank;     
     ch->act = flags;
     ch->perm_frc = frc;
     ch->skill_level[FORCE_ABILITY] = frclevel;
     ch_printf( ch, "\n\r&WConectas tu cabeza a un puerto en la pared.\n\r" );
     ch_printf( ch, "&RUn zumbido en tu cerebro te provoca una pequeña incomudidad...\n\r\n\r" );
     ch_printf( ch, "&WTus datos han sido copiados en un disco duro de una instalación de recuperación antidesastres de la red galáctica.\n\r" );
}

void do_ammo( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *wield;
    OBJ_DATA *obj;
    bool checkammo = FALSE;
    int charge =0;
    
    obj = NULL;
    wield = get_eq_char( ch, WEAR_WIELD );
    if (wield)
    {
       obj = get_eq_char( ch, WEAR_DUAL_WIELD );
       if (!obj) 
          obj = get_eq_char( ch, WEAR_HOLD );
    } 
    else 
    { 
      wield = get_eq_char( ch, WEAR_HOLD );
      obj = NULL;
    } 
    
    if (!wield || wield->item_type != ITEM_WEAPON )
    {
        send_to_char( "&RNo estás empuñando ningún arma.\n\r&w", ch);
        return; 
    }
    
    if ( wield->value[3] == WEAPON_BLASTER )
    {
    
      if ( obj && obj->item_type != ITEM_AMMO )
      {
        send_to_char( "&RTienes las manos ocupadas, no puedes recargar el bláster.\n\r&w", ch);
        return;
      }
    
      if (obj)
      {
        if ( obj->value[0] > wield->value[5] )
        {
            send_to_char( "Ese cartucho es demasiado grande para tu bláster.", ch);
            return;
        }
        unequip_char( ch, obj );
        checkammo = TRUE;
        charge = obj->value[0];
        separate_obj( obj );
        extract_obj( obj );
      }
      else
      {
        for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
        {
           if ( obj->item_type == ITEM_AMMO)
           {
                 if ( obj->value[0] > wield->value[5] )
                 {
                    send_to_char( "Ese cartucho es demasiado grande para tu bláster.", ch);
                    continue;
                 }
                 checkammo = TRUE;
                 charge = obj->value[0];
                 separate_obj( obj );
                 extract_obj( obj );
                 break;                         
           }  
        }
      }
    
      if (!checkammo)
      {
        send_to_char( "&RNo tienes munición para recargar tu bláster.\n\r&w", ch);
        return;
      }
      
      ch_printf( ch, "Reemplazas el cartucho de municiones.\n\rTu bláster ha sido cargada con %d disparos a alta potencia o %d disparos a baja.\n\r", charge/5, charge );
      act( AT_PLAIN, "$n reemplaza la munición de $p.", ch, wield, NULL, TO_ROOM );
	
    }
    else  if ( wield->value[3] == WEAPON_BOWCASTER )
    {
    
      if ( obj && obj->item_type != ITEM_BOLT )
      {
        send_to_char( "&RNo tienes manos libres para recargar tu bowcaster.\n\r&w", ch);
        return;
      }
    
      if (obj)
      {
        if ( obj->value[0] > wield->value[5] )
        {
            send_to_char( "Ese cartucho es demasiado grande para tu bowcaster.", ch);
            return;
        }
        unequip_char( ch, obj );
        checkammo = TRUE;
        charge = obj->value[0];
        separate_obj( obj );
        extract_obj( obj );
      }
      else
      {
        for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
        {
           if ( obj->item_type == ITEM_BOLT)
           {
                 if ( obj->value[0] > wield->value[5] )
                 {
                    send_to_char( "Ese cartucho es demasiado grande para tu bowcaster.", ch);
                    continue;
                 }
                 checkammo = TRUE;
                 charge = obj->value[0];
                 separate_obj( obj );
                 extract_obj( obj );
                 break;                         
           }  
        }
      }
    
      if (!checkammo)
      {
        send_to_char( "&RNo tienes munición para recargar tu bowcaster.\n\r&w", ch);
        return;
      }
      
      ch_printf( ch, "Reemplazas la munición.\n\rTu bowcaster está cargado con %d flechas de energía.\n\r", charge );
      act( AT_PLAIN, "$n reemplaza la munición de $p.", ch, wield, NULL, TO_ROOM );
	
    }
    else
    {
    
      if ( obj && obj->item_type != ITEM_BATTERY )
      {
        send_to_char( "&RTus manos están ocupadas para reemplazar la célula de energía.\n\r&w", ch);
        return;
      }
    
      if (obj)
      {
        unequip_char( ch, obj );
        checkammo = TRUE;
        charge = obj->value[0];
        separate_obj( obj );
        extract_obj( obj );
      }
      else
      {
        for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
        {
           if ( obj->item_type == ITEM_BATTERY)
           {
                 checkammo = TRUE;
                 charge = obj->value[0];
                 separate_obj( obj );
                 extract_obj( obj );
                 break;                         
           }  
        }
      }
    
      if (!checkammo)
      {
        send_to_char( "&RNo tienes células de energía.\n\r&w", ch);
        return;
      }
      
      if (wield->value[3] == WEAPON_LIGHTSABER || wield->value[3] == WEAPON_DUAL_LIGHTSABER)
      {
         ch_printf( ch, "Reemplazas tu célula de energía.\n\rTu sable láser está cargado con %d/%d unidades.\n\r", charge, charge );
         act( AT_PLAIN, "$n reemplaza la célula de nergía de $p.", ch, wield, NULL, TO_ROOM );
	 act( AT_PLAIN, "$p se enciende con un zumbido y comienza a brillar.", ch, wield, NULL, TO_ROOM );
      }
      else if (wield->value[3] == WEAPON_VIBRO_BLADE )
      {
         ch_printf( ch, "Reemplazas la célula de energía.\n\rTu vibrocuchillo está cargado con %d/%d unidades.\n\r", charge, charge );
         act( AT_PLAIN, "$n reemplaza la célula de energía de $p.", ch, wield, NULL, TO_ROOM );
      }
      else if (wield->value[3] == WEAPON_FORCE_PIKE )
      {
         ch_printf( ch, "Reemplazas la célula de energía.\n\rTu pica de fuerza está cargada con %d/%d unidades.\n\r", charge, charge );
         act( AT_PLAIN, "$n reemplaza la célula de energía de $p.", ch, wield, NULL, TO_ROOM );
      }
      else
      {
         ch_printf( ch, "Eso no funcionaría.\n\r" );
         act( AT_PLAIN, "$n intenta poner una célula de energía en $p.", ch, wield, NULL, TO_ROOM );
      }
    }
    
    wield->value[4] = charge;
        
}

void do_setblaster( CHAR_DATA *ch, char *argument )
{
   OBJ_DATA *wield;
   OBJ_DATA *wield2;
 
   wield = get_eq_char( ch, WEAR_WIELD );
   if( wield && !( wield->item_type == ITEM_WEAPON && wield->value[3] == WEAPON_BLASTER ) )
      wield = NULL;
   wield2 = get_eq_char( ch, WEAR_DUAL_WIELD );
   if( wield2 && !( wield2->item_type == ITEM_WEAPON && wield2->value[3] == WEAPON_BLASTER ) )
      wield2 = NULL;
   
   if ( !wield && !wield2 )
   {
      send_to_char( "&RNo estás empuñando un bláster.\n\r&w", ch);
      return;
   }
   
   if ( argument[0] == '\0' )
   {
      send_to_char( "&RUso: ajustarblaster <maxima|alta|normal|media|baja|aturdir>\n\r&w", ch);
      return;
   }
   
   if ( wield )
     act( AT_PLAIN, "$n ajusta la configuración de $p.", ch, wield, NULL, TO_ROOM );
  
   if ( wield2 )
    act( AT_PLAIN, "$n ajusta la configuración de $p.", ch, wield2, NULL, TO_ROOM );
          
   if ( !str_cmp( argument, "maxima" ) )
   {
      if (wield)
      {
        wield->blaster_setting = BLASTER_FULL;
        send_to_char( "&YAjustas la potencia de tu arma láser principal al máximo.\n\r&w", ch);
      }
      if (wield2)
      {
        wield2->blaster_setting = BLASTER_FULL;
        send_to_char( "&YAjustas la potencia de tu arma láser secundaria al máximo.\n\r&w", ch);
      }
      return;
   } 
   if ( !str_cmp( argument, "alta" ) )
   {
      if (wield)
      {
        wield->blaster_setting = BLASTER_HIGH;
        send_to_char( "&YAjustas la potencia de tu arma láser principal a alta.\n\r&w", ch);
      }
      if (wield2)
      {
        wield2->blaster_setting = BLASTER_HIGH;
        send_to_char( "&YAjustas la potencia de tu arma láser secundaria a alta.\n\r&w", ch);
      }
      return;
   } 
   if ( !str_cmp( argument, "normal" ) )
   {
      if (wield)
      {
        wield->blaster_setting = BLASTER_NORMAL;
        send_to_char( "&YAjustas la potencia de tu arma láser principal a normal.\n\r&w", ch);
      }
      if (wield2)
      {
        wield2->blaster_setting = BLASTER_NORMAL;
        send_to_char( "&YAjustas la potencia de tu arma láser secundaria a normal.\n\r&w", ch);
      }
      return;
   } 
   if ( !str_cmp( argument, "media" ) )
   {
      if (wield)
      {
        wield->blaster_setting = BLASTER_HALF;
        send_to_char( "&YAjustas tu arma láser principal a media potencia.\n\r&w", ch);
      }
      if (wield2)
      {
        wield2->blaster_setting = BLASTER_HALF;
        send_to_char( "&YAjustas tu arma láser secundaria a media potencia.\n\r&w", ch);
      }
      return;
   } 
   if ( !str_cmp( argument, "baja" ) )
   {
      if (wield)
      {
        wield->blaster_setting = BLASTER_LOW;
        send_to_char( "&YAjustas tu arma láser principal a baja potencia.\n\r&w", ch);
      }
      if (wield2)
      {
        wield2->blaster_setting = BLASTER_LOW;
        send_to_char( "&YAjustas tu arma láser secundaria a baja potencia.\n\r&w", ch);
      }
      return;
   } 
   if ( !str_cmp( argument, "aturdir" ) )
   {
      if (wield)
      {
        wield->blaster_setting = BLASTER_STUN;
        send_to_char( "&YAjustas tu arma láser principal a aturdir.\n\r&w", ch);
      }
      if (wield2)
      {
        wield2->blaster_setting = BLASTER_STUN;
        send_to_char( "&YAjustas tu arma láser principal a aturdir.\n\r&w", ch);
      }
      return;
   } 
   else
     do_setblaster( ch , "" );

}

void do_use( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char argd[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *device;
    OBJ_DATA *obj;
    ch_ret retcode;

    argument = one_argument( argument, argd );
    argument = one_argument( argument, arg );
    
    if ( !str_cmp( arg , "en" ) )
       argument = one_argument( argument, arg );
    
    if ( argd[0] == '\0' )
    {
	send_to_char( "¿Usar qué?\n\r", ch );
	return;
    }

    if ( ( device = get_eq_char( ch, WEAR_HOLD ) ) == NULL ||
       !nifty_is_name(argd, device->name) )
    {
        do_takedrug( ch , argd );    
	return;
    }
    
    if ( device->item_type == ITEM_SPICE )
    {
        do_takedrug( ch , argd );
        return;
    }
    
    if ( device->item_type != ITEM_DEVICE )
    {
	send_to_char( "No encuentras la forma de hacerlo.\n\r", ch );
	return;
    }
    
    if ( device->value[2] <= 0 )
    {
        send_to_char( "No tiene más cargas.", ch);
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
	    send_to_char( "¿Usar qué con quien?\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( ( victim = get_char_room ( ch, arg ) ) == NULL
	&&   ( obj    = get_obj_here  ( ch, arg ) ) == NULL )
	{
	    send_to_char( "No puedes encontrrar a tu objetivo.\n\r", ch );
	    return;
	}
    }

    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );

    if ( device->value[2] > 0 )
    {
        device->value[2]--;
	if ( victim )
	{
          if ( !oprog_use_trigger( ch, device, victim, NULL, NULL ) )
          {
	    act( AT_MAGIC, "$n usa $p en $N.", ch, device, victim, TO_ROOM );
	    act( AT_MAGIC, "Usas $p en $N.", ch, device, victim, TO_CHAR );
          }
	}
	else
	{
          if ( !oprog_use_trigger( ch, device, NULL, obj, NULL ) )
          {
	    act( AT_MAGIC, "$n usa $p en $P.", ch, device, obj, TO_ROOM );
	    act( AT_MAGIC, "Usas $p en $P.", ch, device, obj, TO_CHAR );
          }
	}

	retcode = obj_cast_spell( device->value[3], device->value[0], ch, victim, obj );
	if ( retcode == rCHAR_DIED || retcode == rBOTH_DIED )
	{
	   bug( "do_use: char died", 0 );
	   return;
	}
    }


    return;
}

void do_takedrug( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    AFFECT_DATA af;
    int drug;
    int sn;
    
    if ( argument[0] == '\0' || !str_cmp(argument, "") )
    {
	send_to_char( "¿usar qué?\n\r", ch );
	return;
    }

    if ( (obj = find_obj(ch, argument, TRUE)) == NULL )
	return;

    if ( obj->item_type == ITEM_DEVICE )
    {
        send_to_char( "Sostenlo primero.\n\r", ch );
	return;
    }

    if ( obj->item_type != ITEM_SPICE )
    {
	act( AT_ACTION, "$n Mira $p y araña su cabeza.", ch, obj, NULL, TO_ROOM );
	act( AT_ACTION, "No encuentras la forma de usar $p.", ch, obj, NULL, TO_CHAR );
	return;
    }

    separate_obj( obj );
    if ( obj->in_obj )
    {
	act( AT_PLAIN, "Tomas $p de $P.", ch, obj, obj->in_obj, TO_CHAR );
	act( AT_PLAIN, "$n toma $p de $P.", ch, obj, obj->in_obj, TO_ROOM );
    }

    if ( ch->fighting && number_percent( ) > (get_curr_dex(ch) * 2 + 48) )
    {
	act( AT_MAGIC, "$n deja caer accidentalmente $p esparciendo su contenido.", ch, obj, NULL, TO_ROOM );
	act( AT_MAGIC, "¡Oops! ¡$p se te cae de las manos y su contenido se esparce por el suelo!", ch, obj, NULL ,TO_CHAR );
    }
    else
    {
	if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
	{
	    act( AT_ACTION, "$n toma $p.",  ch, obj, NULL, TO_ROOM );
	    act( AT_ACTION, "Tomas $p.", ch, obj, NULL, TO_CHAR );
	}
        
        if ( IS_NPC(ch) )
        {
          extract_obj( obj );
          return;
        }
        
        drug = obj->value[0];
        
        WAIT_STATE( ch, PULSE_PER_SECOND/4 );
        
	gain_condition( ch, COND_THIRST, 1 );
	
	ch->pcdata->drug_level[drug] = UMIN(ch->pcdata->drug_level[drug]+obj->value[1] , 255);
	if ( ch->pcdata->drug_level[drug] >=255 
	     || ch->pcdata->drug_level[drug] > ( ch->pcdata->addiction[drug]+100 ) ) 
	{
	   act( AT_POISON, "$n parece estar muy mal.", ch, NULL, NULL, TO_ROOM );
	   act( AT_POISON, "Te sientes enfermo. Quizás hayas consumido mucho.", ch, NULL, NULL, TO_CHAR );
	     ch->mental_state = URANGE( 20, ch->mental_state + 5, 100 );
	     af.type      = gsn_poison;
	     af.location  = APPLY_INT;
	     af.modifier  = -5;
	     af.duration  = ch->pcdata->drug_level[drug];
	     af.bitvector = AFF_POISON;
	     affect_to_char( ch, &af );
	     ch->hit = 1;
	}
		  
	switch (drug)
	{ 
	    default:
	    case SPICE_GLITTERSTIM:
	
	       sn=skill_lookup("true sight");
	       if ( sn < MAX_SKILL && !IS_AFFECTED( ch, AFF_TRUESIGHT ) )
	       { 
	  	   af.type      = sn;
	  	   af.location  = APPLY_AC;
	  	   af.modifier  = -10;
	  	   af.duration  = URANGE( 1, ch->pcdata->drug_level[drug] - ch->pcdata->addiction[drug] ,obj->value[1] );
	  	   af.bitvector = AFF_TRUESIGHT;
	  	   affect_to_char( ch, &af );
	       }
	       break;
        
            case SPICE_CARSANUM:
            
               sn=skill_lookup("sanctuary");
	       if ( sn < MAX_SKILL && !IS_AFFECTED( ch, AFF_SANCTUARY ) )
	       { 
	  	   af.type      = sn;
	  	   af.location  = APPLY_NONE;
	  	   af.modifier  = 0;
	  	   af.duration  = URANGE( 1, ch->pcdata->drug_level[drug] - ch->pcdata->addiction[drug] ,obj->value[1] );
	  	   af.bitvector = AFF_SANCTUARY;
	  	   affect_to_char( ch, &af );
	       }
	       break;
         
            case SPICE_RYLL:
            
	  	   af.type      = -1;
	  	   af.location  = APPLY_DEX;
	  	   af.modifier  = 1;
	  	   af.duration  = URANGE( 1, 2*(ch->pcdata->drug_level[drug] - ch->pcdata->addiction[drug]) ,2*obj->value[1] );
	  	   af.bitvector = AFF_NONE;
	  	   affect_to_char( ch, &af );
	  	   
	  	   af.type      = -1;
	  	   af.location  = APPLY_HITROLL;
	  	   af.modifier  = 1;
	  	   af.duration  = URANGE( 1, 2*(ch->pcdata->drug_level[drug] - ch->pcdata->addiction[drug]) ,2*obj->value[1] );
	  	   af.bitvector = AFF_NONE;
	  	   affect_to_char( ch, &af );
	  	   
	        break;
        
            case SPICE_ANDRIS:
     
	  	   af.type      = -1;
	  	   af.location  = APPLY_HIT;
	  	   af.modifier  = 10;
	  	   af.duration  = URANGE( 1, 2*(ch->pcdata->drug_level[drug] - ch->pcdata->addiction[drug]) ,2*obj->value[1] );
	  	   af.bitvector = AFF_NONE;
	  	   affect_to_char( ch, &af );
	           
	           af.type      = sn;
	  	   af.location  = APPLY_CON;
	  	   af.modifier  = 1;
	  	   af.duration  = URANGE( 1, 2*(ch->pcdata->drug_level[drug] - ch->pcdata->addiction[drug]) ,2*obj->value[1] );
	  	   af.bitvector = AFF_NONE;
	  	   affect_to_char( ch, &af );
	           
	       break;
           
        }
        	
    }
    if ( cur_obj == obj->serial )
      global_objcode = rOBJ_EATEN;
    extract_obj( obj );
    return;
}

void jedi_bonus( CHAR_DATA *ch )
{
   if ( number_range( 1 , 100 ) == 1 )
   { 
        ch->max_mana++;
   	send_to_char("&YUsas la fuerza sabiamente.\n\r", ch);
   	send_to_char("Te sientes más sabio y fuerte.&w\n\r", ch);
   }
}

void sith_penalty( CHAR_DATA *ch )
{
   if ( number_range( 1 , 100 ) == 1 )
   { 
        ch->max_mana++ ;
        if (ch->max_hit > 100)
   	  ch->max_hit--   ;
   	ch->hit--   ;
   	send_to_char("&zTu cuerpo se debilita a medida que crece tu fuerza en el lado oscuro.&w\n\r",ch);
   }
}

/*
 * Fill a container
 * Many enhancements added by Thoric (ie: filling non-drink containers)
 */
void do_fill( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *source;
    sh_int    dest_item, src_item1, src_item2, src_item3, src_item4;
    int       diff;
    bool      all = FALSE;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    /* munch optional words */
    if ( (!str_cmp( arg2, "de" ) || !str_cmp( arg2, "con" ))
    &&    argument[0] != '\0' )
	argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "¿Llenar qué?\n\r", ch );
	return;
    }

    if ( ms_find_obj(ch) )
	return;

    if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
    {
	send_to_char( "No tienes ese objeto.\n\r", ch );
	return;
    }
    else
	dest_item = obj->item_type;

    src_item1 = src_item2 = src_item3 = src_item4 = -1;
    switch( dest_item )
    {
	default:
	  act( AT_ACTION, "$n intenta llenar $p... (No me preguntes cómo)", ch, obj, NULL, TO_ROOM );
	  send_to_char( "No puedes llenar eso.\n\r", ch );
	  return;
	/* place all fillable item types here */
	case ITEM_DRINK_CON:
	  src_item1 = ITEM_FOUNTAIN;	src_item2 = ITEM_BLOOD;		break;
	case ITEM_HERB_CON:
	  src_item1 = ITEM_HERB;	src_item2 = ITEM_HERB_CON;	break;
	case ITEM_PIPE:
	  src_item1 = ITEM_HERB;	src_item2 = ITEM_HERB_CON;	break;
	case ITEM_CONTAINER:
	  src_item1 = ITEM_CONTAINER;	src_item2 = ITEM_CORPSE_NPC;
	  src_item3 = ITEM_CORPSE_PC;	src_item4 = ITEM_CORPSE_NPC;    break;
    }

    if ( dest_item == ITEM_CONTAINER )
    {
	if ( IS_SET(obj->value[1], CONT_CLOSED) )
	{
	    act( AT_PLAIN, "Abre $d primero.", ch, NULL, obj->name, TO_CHAR );
	    return;
	}
	if ( get_obj_weight( obj ) / obj->count
	>=   obj->value[0] )
	{
	   send_to_char( "Ya se ha llenado.\n\r", ch );
	   return;
	}
    }
    else
    {
	diff = obj->value[0] - obj->value[1];
	if ( diff < 1 || obj->value[1] >= obj->value[0] )
	{
	   send_to_char( "Ya se ha llenado.\n\r", ch );
	   return;
	}
    }

    if ( dest_item == ITEM_PIPE
    &&   IS_SET( obj->value[3], PIPE_FULLOFASH ) )
    {
	send_to_char( "Debería vaciarse primero.\n\r", ch );
	return;
    }

    if ( arg2[0] != '\0' )
    {
      if ( dest_item == ITEM_CONTAINER
      && (!str_cmp( arg2, "todo" ) || !str_prefix( "todo.", arg2 )) )
      {
	all = TRUE;
	source = NULL;
      }
      else
      /* This used to let you fill a pipe from an object on the ground.  Seems
         to me you should be holding whatever you want to fill a pipe with.
         It's nitpicking, but I needed to change it to get a mobprog to work
         right.  Check out Lord Fitzgibbon if you're curious.  -Narn */
      if ( dest_item == ITEM_PIPE )
      {
        if ( ( source = get_obj_carry( ch, arg2 ) ) == NULL )
	{
	   send_to_char( "No tienes ese objeto.\n\r", ch );
	   return;
	}
	if ( source->item_type != src_item1 && source->item_type != src_item2
	&&   source->item_type != src_item3 &&   source->item_type != src_item4  )
	{
	   act( AT_PLAIN, "¡No puedes llenar $p con $P!", ch, obj, source, TO_CHAR );
	   return;
	}
      }
      else
      {
	if ( ( source =  get_obj_here( ch, arg2 ) ) == NULL )
	{
	   send_to_char( "No encuentras ese objeto.\n\r", ch );
	   return;
	}
      }
    }
    else
	source = NULL;

    if ( !source && dest_item == ITEM_PIPE )
    {
	send_to_char( "¿Con qué?\n\r", ch );
	return;
    }

    if ( !source )
    {
	bool      found = FALSE;
	OBJ_DATA *src_next;

	found = FALSE;
	separate_obj( obj );
	for ( source = ch->in_room->first_content;
	      source;
	      source = src_next )
	{
	    src_next = source->next_content;
	    if (dest_item == ITEM_CONTAINER)
	    {
		if ( !CAN_WEAR(source, ITEM_TAKE)
		||   (IS_OBJ_STAT( source, ITEM_PROTOTYPE) && !can_take_proto(ch))
		||    ch->carry_weight + get_obj_weight(source) > can_carry_w(ch)
		||   (get_obj_weight(source) + get_obj_weight(obj)/obj->count)
		    > obj->value[0] )
		  continue;
		if ( all && arg2[3] == '.'
		&&  !nifty_is_name( &arg2[4], source->name ) )
		   continue;
		obj_from_room(source);
		if ( source->item_type == ITEM_MONEY )
		{
		   ch->gold += source->value[0];
		   extract_obj( source );
		}
		else
		   obj_to_obj(source, obj);
		found = TRUE;
	    }
	    else
	    if (source->item_type == src_item1
	    ||  source->item_type == src_item2
	    ||  source->item_type == src_item3
	    ||  source->item_type == src_item4 )
	    {
		found = TRUE;
		break;
	    }
	}
	if ( !found )
	{
	    switch( src_item1 )
	    {
		default:
		  send_to_char( "¡No hay nada apropiado aquí!\n\r", ch );
		  return;
		case ITEM_FOUNTAIN:
		  send_to_char( "¡No ves ninguna fuente o piscina aquí!\n\r", ch );
		  return;
		case ITEM_BLOOD:
		  send_to_char( "¡No hay fuente o piscina de sangre aquí!\n\r", ch );
		  return;
		case ITEM_HERB_CON:
		  send_to_char( "¡No hay hierbas aquí!\n\r", ch );
		  return;
		case ITEM_HERB:
		  send_to_char( "No hay hierba fumable aquí.\n\r", ch );
		  return;
	    }
	}
	if (dest_item == ITEM_CONTAINER)
	{
	  act( AT_ACTION, "Llenas $p.", ch, obj, NULL, TO_CHAR );
	  act( AT_ACTION, "$n llena $p.", ch, obj, NULL, TO_ROOM );
	  return;
	}
    }

    if (dest_item == ITEM_CONTAINER)
    {
	OBJ_DATA *otmp, *otmp_next;
	char name[MAX_INPUT_LENGTH];
	CHAR_DATA *gch;
	char *pd;
	bool found = FALSE;

	if ( source == obj )
	{
	    send_to_char( "¿Estás tratando de doblar la realidad!\n\r", ch );
	    return;
	}

	switch( source->item_type )
	{
	    default:	/* put something in container */
		if ( !source->in_room	/* disallow inventory items */
		||   !CAN_WEAR(source, ITEM_TAKE)
		||   (IS_OBJ_STAT( source, ITEM_PROTOTYPE) && !can_take_proto(ch))
		||    ch->carry_weight + get_obj_weight(source) > can_carry_w(ch)
		||   (get_obj_weight(source) + get_obj_weight(obj)/obj->count)
		    > obj->value[0] )
		{
		    send_to_char( "No puedes hacer eso.\n\r", ch );
		    return;
		}
		separate_obj( obj );
		act( AT_ACTION, "Coges $P y lo pones dentro de $p.", ch, obj, source, TO_CHAR );
		act( AT_ACTION, "$n coge $P y lo pone dentro de $p.", ch, obj, source, TO_ROOM );
		obj_from_room(source);
		obj_to_obj(source, obj);
		break;
	    case ITEM_MONEY:
		send_to_char( "No puedes hacer eso... Todavía.\n\r", ch );
		break;
	    case ITEM_CORPSE_PC:
		if ( IS_NPC(ch) )
		{
		    send_to_char( "No puedes hacer eso.\n\r", ch );
		    return;
		}
		
		    pd = source->short_descr;
		    pd = one_argument( pd, name );
		    pd = one_argument( pd, name );
		    pd = one_argument( pd, name );
		    pd = one_argument( pd, name );

		    if ( str_cmp( name, ch->name ) && !IS_IMMORTAL(ch) )
		    {
			bool fGroup;

			fGroup = FALSE;
			for ( gch = first_char; gch; gch = gch->next )
			{
			    if ( !IS_NPC(gch)
			    &&   is_same_group( ch, gch )
			    &&   !str_cmp( name, gch->name ) )
			    {
				fGroup = TRUE;
				break;
			    }
			}
			if ( !fGroup )
			{
			    send_to_char( "Ese es el cadáver de otra persona.\n\r", ch );
			    return;
			}
		     }
		      
	    case ITEM_CONTAINER:
		if ( source->item_type == ITEM_CONTAINER  /* don't remove */
		&&   IS_SET(source->value[1], CONT_CLOSED) )
		{
		    act( AT_PLAIN, "Necesitas abrir primero $d.", ch, NULL, source->name, TO_CHAR );
		    return;
		}
	    case ITEM_DROID_CORPSE:
	    case ITEM_CORPSE_NPC:
		if ( (otmp=source->first_content) == NULL )
		{
		    send_to_char( "No tiene nada.\n\r", ch );
		    return;
		}
		separate_obj( obj );
		for ( ; otmp; otmp = otmp_next )
		{
		    otmp_next = otmp->next_content;

		    if ( !CAN_WEAR(otmp, ITEM_TAKE)
		    ||   (IS_OBJ_STAT( otmp, ITEM_PROTOTYPE) && !can_take_proto(ch))
		    ||    ch->carry_number + otmp->count > can_carry_n(ch)
		    ||    ch->carry_weight + get_obj_weight(otmp) > can_carry_w(ch)
		    ||   (get_obj_weight(source) + get_obj_weight(obj)/obj->count)
			> obj->value[0] )
			continue;
		    obj_from_obj(otmp);
		    obj_to_obj(otmp, obj);
		    found = TRUE;
		}
		if ( found )
		{
		   act( AT_ACTION, "Llenas $p de $P.", ch, obj, source, TO_CHAR );
		   act( AT_ACTION, "$n llena $p de $P.", ch, obj, source, TO_ROOM );
		}
		else
		   send_to_char( "No hay nada apropiado allí.\n\r", ch );
		break;
	}
	return;
    }

    if ( source->value[1] < 1 )
    {
	send_to_char( "¡No queda nada!\n\r", ch );
	return;
    }
    if ( source->count > 1 && source->item_type != ITEM_FOUNTAIN )
      separate_obj( source );
    separate_obj( obj );

    switch( source->item_type )
    {
	default:
	  bug( "do_fill: got bad item type: %d", source->item_type );
	  send_to_char( "Algo ha fallado...\n\r", ch );
	  return;
	case ITEM_FOUNTAIN:
	  if ( obj->value[1] != 0 && obj->value[2] != 0 )
	  {
	     send_to_char( "Ya contiene otro líquido in it.\n\r", ch );
	     return;
	  }
	  obj->value[2] = 0;
	  obj->value[1] = obj->value[0];
	  act( AT_ACTION, "Llenas $p de $P.", ch, obj, source, TO_CHAR );
	  act( AT_ACTION, "$n llena $p de $P.", ch, obj, source, TO_ROOM );
	  return;
	case ITEM_BLOOD:
	  if ( obj->value[1] != 0 && obj->value[2] != 13 )
	  {
	     send_to_char( "Ya tiene otro líquido en su interior.\n\r", ch );
	     return;
	  }
	  obj->value[2] = 13;
	  if ( source->value[1] < diff )
	    diff = source->value[1];
	  obj->value[1] += diff;
	  act( AT_ACTION, "Llenas $p de $P.", ch, obj, source, TO_CHAR );
	  act( AT_ACTION, "$n llena $p de $P.", ch, obj, source, TO_ROOM );
	  if ( (source->value[1] -= diff) < 1 )
	  {
 	     extract_obj( source );
	     make_bloodstain( ch );
	  }
	  return;
	case ITEM_HERB:
	  if ( obj->value[1] != 0 && obj->value[2] != source->value[2] )
	  {
	     send_to_char( "Tiene otro tipo de hierba en su interior.\n\r", ch );
	     return;
	  }
	  obj->value[2] = source->value[2];
	  if ( source->value[1] < diff )
	    diff = source->value[1];
	  obj->value[1] += diff;
	  act( AT_ACTION, "Llenas $p con $P.", ch, obj, source, TO_CHAR );
	  act( AT_ACTION, "$n llena $p con $P.", ch, obj, source, TO_ROOM );
	  if ( (source->value[1] -= diff) < 1 )
 	     extract_obj( source );
	  return;
	case ITEM_HERB_CON:
	  if ( obj->value[1] != 0 && obj->value[2] != source->value[2] )
	  {
	     send_to_char( "tiene otro tipo de hierba en su interior.\n\r", ch );
	     return;
	  }
	  obj->value[2] = source->value[2];
	  if ( source->value[1] < diff )
	    diff = source->value[1];
	  obj->value[1] += diff;
	  source->value[1] -= diff;
	  act( AT_ACTION, "Llenas $p de $P.", ch, obj, source, TO_CHAR );
	  act( AT_ACTION, "$n llena $p de $P.", ch, obj, source, TO_ROOM );
	  return;
	case ITEM_DRINK_CON:
	  if ( obj->value[1] != 0 && obj->value[2] != source->value[2] )
	  {
	     send_to_char( "Tiene otro líquido en su interior.\n\r", ch );
	     return;
	  }
	  obj->value[2] = source->value[2];
	  if ( source->value[1] < diff )
	    diff = source->value[1];
	  obj->value[1] += diff;
	  source->value[1] -= diff;
	  act( AT_ACTION, "Llenas $p de $P.", ch, obj, source, TO_CHAR );
	  act( AT_ACTION, "$n llena $p de $P.", ch, obj, source, TO_ROOM );
	  return;
    }
}

void do_drink( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int amount;
    int liquid;

    argument = one_argument( argument, arg );
    /* munch optional words */
    if ( !str_cmp( arg, "de" ) && argument[0] != '\0' )
	argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	for ( obj = ch->in_room->first_content; obj; obj = obj->next_content )
	    if ( (obj->item_type == ITEM_FOUNTAIN)
	    ||   (obj->item_type == ITEM_BLOOD) )
		break;

	if ( !obj )
	{
	    send_to_char( "¿Beber qué?\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( ( obj = get_obj_here( ch, arg ) ) == NULL )
	{
	    send_to_char( "No encuentras eso.\n\r", ch );
	    return;
	}
    }

    if ( obj->count > 1 && obj->item_type != ITEM_FOUNTAIN )
	separate_obj(obj);

    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 40 )
    {
	send_to_char( "Fallas al encontrar tu boca.  *Hip*\n\r", ch );
	return;
    }

    switch ( obj->item_type )
    {
    default:
	if ( obj->carried_by == ch )
	{
	    act( AT_ACTION, "$n se lleva $p a la boca y trata de beber...", ch, obj, NULL, TO_ROOM );
	    act( AT_ACTION, "Te llevas $p a la boca e intentas beber...", ch, obj, NULL, TO_CHAR );
	}
	else
	{
	    act( AT_ACTION, "$n se arrodilla e intenta beber de $p... (Te preguntas si se estará sintiendo bien)", ch, obj, NULL, TO_ROOM );
	    act( AT_ACTION, "Te arrodillas y tratas de beber de $p...", ch, obj, NULL, TO_CHAR );
	}
	break;

    case ITEM_POTION:
	if ( obj->carried_by == ch )
	   do_quaff( ch, obj->name );
	else
	   send_to_char( "No estás llevando eso.\n\r", ch );
	break;

    case ITEM_FOUNTAIN:
	if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
	{
	   act( AT_ACTION, "$n bebe de la fuente.", ch, NULL, NULL, TO_ROOM );
	   send_to_char( "Tomas un largo trago que te calma la sed.\n\r", ch );
	}

	if ( !IS_NPC(ch) )
	    ch->pcdata->condition[COND_THIRST] = 40;
	break;

    case ITEM_DRINK_CON:
	if ( obj->value[1] <= 0 )
	{
	    send_to_char( "No contiene nada.\n\r", ch );
	    return;
	}

	if ( ( liquid = obj->value[2] ) >= LIQ_MAX )
	{
	    bug( "Do_drink: bad liquid number %d.", liquid );
	    liquid = obj->value[2] = 0;
	}

	if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
	{
	   act( AT_ACTION, "$n bebe $T de $p.",
		ch, obj, liq_table[liquid].liq_name, TO_ROOM );
	   act( AT_ACTION, "Bebes $T de $p.",
		ch, obj, liq_table[liquid].liq_name, TO_CHAR );
	}

	amount = 1; /* UMIN(amount, obj->value[1]); */
	/* what was this? concentrated drinks?  concentrated water
	   too I suppose... sheesh! */

	gain_condition( ch, COND_DRUNK,
	    amount * liq_table[liquid].liq_affect[COND_DRUNK  ] );
	gain_condition( ch, COND_FULL,
	    amount * liq_table[liquid].liq_affect[COND_FULL   ] );
	gain_condition( ch, COND_THIRST,
	    amount * liq_table[liquid].liq_affect[COND_THIRST ] );

	if ( !IS_NPC(ch) )
	{
	    if ( ch->pcdata->condition[COND_DRUNK]  > 24 )
		send_to_char( "Te has enguachinado.\n\r", ch );
	    else
	    if ( ch->pcdata->condition[COND_DRUNK]  > 18 )
		send_to_char( "Te empiezas a marear.\n\r", ch );
	    else
	    if ( ch->pcdata->condition[COND_DRUNK]  > 12 )
		send_to_char( "Te mareas.\n\r", ch );
	    else
	    if ( ch->pcdata->condition[COND_DRUNK]  > 8 )
		send_to_char( "Te mareas un poco.\n\r", ch );
	    else
	    if ( ch->pcdata->condition[COND_DRUNK]  > 5 )
		send_to_char( "Te mareas.\n\r", ch );

	    if ( ch->pcdata->condition[COND_FULL]   > 40 )
		send_to_char( "Te has llenado.\n\r", ch );

	    if ( ch->pcdata->condition[COND_THIRST] > 40 )
		send_to_char( "Te sientes a punto de reventar.\n\r", ch );
	    else
	    if ( ch->pcdata->condition[COND_THIRST] > 36 )
		send_to_char( "Tu estómago está revosante.\n\r", ch );
	    else
	    if ( ch->pcdata->condition[COND_THIRST] > 30 )
		send_to_char( "Ya no tienes sed.\n\r", ch );
	}

	if ( obj->value[3] )
	{
	    /* The drink was poisoned! */
	    AFFECT_DATA af;

	    act( AT_POISON, "$n escupe de pronto.", ch, NULL, NULL, TO_ROOM );
	    act( AT_POISON, "Escupes.", ch, NULL, NULL, TO_CHAR );
	    ch->mental_state = URANGE( 20, ch->mental_state + 5, 100 );
	    af.type      = gsn_poison;
	    af.duration  = 3 * obj->value[3];
	    af.location  = APPLY_NONE;
	    af.modifier  = 0;
	    af.bitvector = AFF_POISON;
	    affect_join( ch, &af );
	}

	obj->value[1] -= amount;
	break;
    }
    WAIT_STATE(ch, PULSE_PER_SECOND );
    return;
}

void do_eat( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    ch_ret retcode;
    int foodcond;

    if ( argument[0] == '\0' )
    {
	send_to_char( "¿Comer qué?\n\r", ch );
	return;
    }

    if ( IS_NPC(ch) || ch->pcdata->condition[COND_FULL] > 5 )
	if ( ms_find_obj(ch) )
	    return;

    if ( (obj = find_obj(ch, argument, TRUE)) == NULL )
	return;

    if ( !IS_IMMORTAL(ch) )
    {
	if ( obj->item_type != ITEM_FOOD && obj->item_type != ITEM_PILL )
	{
	    act( AT_ACTION, "$n mordisquea $p... ¡Tiene que tener mucha hambre)",  ch, obj, NULL, TO_ROOM );
	    act( AT_ACTION, "Mordisqueas $p...", ch, obj, NULL, TO_CHAR );
	    return;
	}

	if ( !IS_NPC(ch) && ch->pcdata->condition[COND_FULL] > 40 )
	{
	    send_to_char( "No puedes comer más.\n\r", ch );
	    return;
	}
    }

    /* required due to object grouping */
    separate_obj( obj );
    
    WAIT_STATE( ch, PULSE_PER_SECOND/2 );
    
    if ( obj->in_obj )
    {
	act( AT_PLAIN, "Coges $p de $P.", ch, obj, obj->in_obj, TO_CHAR );
	act( AT_PLAIN, "$n coge $p de $P.", ch, obj, obj->in_obj, TO_ROOM );
    }
    if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
    {
      if ( !obj->action_desc || obj->action_desc[0]=='\0' )
      {
        act( AT_ACTION, "$n se come $p.",  ch, obj, NULL, TO_ROOM );
        act( AT_ACTION, "Te comes $p.", ch, obj, NULL, TO_CHAR );
      }
      else
        actiondesc( ch, obj, NULL ); 
    }

    switch ( obj->item_type )
    {

    case ITEM_FOOD:
	if ( obj->timer > 0 && obj->value[1] > 0 )
	   foodcond = (obj->timer * 10) / obj->value[1];
	else
	   foodcond = 10;

	if ( !IS_NPC(ch) )
	{
	    int condition;

	    condition = ch->pcdata->condition[COND_FULL];
	    gain_condition( ch, COND_FULL, (obj->value[0] * foodcond) / 10 );
	    if ( condition <= 1 && ch->pcdata->condition[COND_FULL] > 1 )
		send_to_char( "Ya no tienes hambre.\n\r", ch );
	    else if ( ch->pcdata->condition[COND_FULL] > 40 )
		send_to_char( "Te has llenado.\n\r", ch );
	}

	if (  obj->value[3] != 0
	||   (foodcond < 4 && number_range( 0, foodcond + 1 ) == 0) )
	{
	    /* The food was poisoned! */
	    AFFECT_DATA af;

	    if ( obj->value[3] != 0 )
	    {
		act( AT_POISON, "$n vomita.", ch, NULL, NULL, TO_ROOM );
		act( AT_POISON, "vomitas.", ch, NULL, NULL, TO_CHAR );
		ch->mental_state = URANGE( 20, ch->mental_state + 5, 100 );
	    }
	    else
	    {
		act( AT_POISON, "$n vomita $p.", ch, obj, NULL, TO_ROOM );
		act( AT_POISON, "Vomitas $p.", ch, obj, NULL, TO_CHAR );
		ch->mental_state = URANGE( 15, ch->mental_state + 5, 100 );
	    }

	    af.type      = gsn_poison;
	    af.duration  = 2 * obj->value[0]
	    		 * (obj->value[3] > 0 ? obj->value[3] : 1);
	    af.location  = APPLY_NONE;
	    af.modifier  = 0;
	    af.bitvector = AFF_POISON;
	    affect_join( ch, &af );
	}
	break;

    case ITEM_PILL:
	/* allow pills to fill you, if so desired */
	if ( !IS_NPC(ch) && obj->value[4] )
	{
	    int condition;

	    condition = ch->pcdata->condition[COND_FULL];
	    gain_condition( ch, COND_FULL, obj->value[4] );
	    if ( condition <= 1 && ch->pcdata->condition[COND_FULL] > 1 )
		send_to_char( "Ya no tienes hambre.\n\r", ch );
	    else if ( ch->pcdata->condition[COND_FULL] > 40 )
		send_to_char( "Te has llenado.\n\r", ch );
	}
	retcode = obj_cast_spell( obj->value[1], obj->value[0], ch, ch, NULL );
	if ( retcode == rNONE )
	  retcode = obj_cast_spell( obj->value[2], obj->value[0], ch, ch, NULL );
	if ( retcode == rNONE )
	  retcode = obj_cast_spell( obj->value[3], obj->value[0], ch, ch, NULL );
	break;
    }

    if ( obj->serial == cur_obj )
      global_objcode = rOBJ_EATEN;
    extract_obj( obj );
    return;
}

void do_quaff( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    ch_ret retcode;

    if ( argument[0] == '\0' || !str_cmp(argument, "") )
    {
	send_to_char( "¿Tragar qué?\n\r", ch );
	return;
    }

    if ( (obj = find_obj(ch, argument, TRUE)) == NULL )
	return;

    if ( obj->item_type != ITEM_POTION )
    {
	if ( obj->item_type == ITEM_DRINK_CON )
	   do_drink( ch, obj->name );
	else
	{
	   act( AT_ACTION, "$n se lleva $p a la boca e intenta beber...", ch, obj, NULL, TO_ROOM );
	   act( AT_ACTION, "Te llevas $p a la boca e intentas beber...", ch, obj, NULL, TO_CHAR );
	}
	return;
    }

    /*
     * Fullness checking					-Thoric
     */
    if ( !IS_NPC(ch)
    && ( ch->pcdata->condition[COND_FULL] >= 48
    ||   ch->pcdata->condition[COND_THIRST] >= 48 ) )
    {
	send_to_char( "Tu estómago no puede contener nada más.\n\r", ch );
	return;
    }

    separate_obj( obj );
    if ( obj->in_obj )
    {
	act( AT_PLAIN, "Coges $p de $P.", ch, obj, obj->in_obj, TO_CHAR );
	act( AT_PLAIN, "$n coge $p de $P.", ch, obj, obj->in_obj, TO_ROOM );
    }

    /*
     * If fighting, chance of dropping potion			-Thoric
     */
    if ( ch->fighting && number_percent( ) > (get_curr_dex(ch) * 2 + 48) )
    {
	act( AT_MAGIC, "$n deja caer accidentalmente $p que se rompe en fragmentos.", ch, obj, NULL, TO_ROOM );
	act( AT_MAGIC, "¡Oops! ¡$p se te cae de las manos y se rompe en fragmentos!", ch, obj, NULL ,TO_CHAR );
    }
    else
    {
	if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
	{
	    act( AT_ACTION, "$n traga $p.",  ch, obj, NULL, TO_ROOM );
	    act( AT_ACTION, "Tragas $p.", ch, obj, NULL, TO_CHAR );
	}

        WAIT_STATE( ch, PULSE_PER_SECOND/4 );
        
	gain_condition( ch, COND_THIRST, 1 );
	retcode = obj_cast_spell( obj->value[1], obj->value[0], ch, ch, NULL );
	if ( retcode == rNONE )
	  retcode = obj_cast_spell( obj->value[2], obj->value[0], ch, ch, NULL );
	if ( retcode == rNONE )
	  retcode = obj_cast_spell( obj->value[3], obj->value[0], ch, ch, NULL );
    }
    if ( cur_obj == obj->serial )
      global_objcode = rOBJ_QUAFFED;
    extract_obj( obj );
    return;
}


void do_recite( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *scroll;
    OBJ_DATA *obj;
    ch_ret    retcode;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "¿Activar qué?\n\r", ch );
	return;
    }

    if ( ms_find_obj(ch) )
	return;

    if ( ( scroll = get_obj_carry( ch, arg1 ) ) == NULL )
    {
	send_to_char( "No tienes ese objeto.\n\r", ch );
	return;
    }

    if ( scroll->item_type != ITEM_SCROLL )
    {
	act( AT_ACTION, "$n intenta activar $p...",  ch, scroll, NULL, TO_ROOM );
	act( AT_ACTION, "Intentas activar$p. (No sé cómo)", ch, scroll, NULL, TO_CHAR );
	return;
    }

    if ( IS_NPC(ch) 
    && (scroll->pIndexData->vnum == OBJ_VNUM_SCROLL_SCRIBING) )
    {
	send_to_char( "As a mob, this dialect is foreign to you.\n\r", ch );
	return;
    }

    if( ( scroll->pIndexData->vnum == OBJ_VNUM_SCROLL_SCRIBING)
      &&(ch->top_level + 10 < scroll->value[0]))
    {
        send_to_char( "Este objeto es demasiado complejo para ti.\n\r", ch);
        return;
    }

    obj = NULL;
    if ( arg2[0] == '\0' )
	victim = ch;
    else
    {
	if ( ( victim = get_char_room ( ch, arg2 ) ) == NULL
	&&   ( obj    = get_obj_here  ( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "No puedes encontrarlo.\n\r", ch );
	    return;
	}
    }

    separate_obj( scroll );
    act( AT_MAGIC, "$n activa $p.", ch, scroll, NULL, TO_ROOM );
    act( AT_MAGIC, "Activas $p.", ch, scroll, NULL, TO_CHAR );

    
    WAIT_STATE( ch, PULSE_PER_SECOND/2 );

    retcode = obj_cast_spell( scroll->value[1], scroll->value[0], ch, victim, obj );
    if ( retcode == rNONE )
      retcode = obj_cast_spell( scroll->value[2], scroll->value[0], ch, victim, obj );
    if ( retcode == rNONE )
      retcode = obj_cast_spell( scroll->value[3], scroll->value[0], ch, victim, obj );

    if ( scroll->serial == cur_obj )
      global_objcode = rOBJ_USED;
    extract_obj( scroll );
    return;
}


/*
 * Function to handle the state changing of a triggerobject (lever)  -Thoric
 */
void pullorpush( CHAR_DATA *ch, OBJ_DATA *obj, bool pull )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA		*rch;
    bool		 isup;
    ROOM_INDEX_DATA	*room,  *to_room;
    EXIT_DATA		*pexit, *pexit_rev;
    int			 edir;
    char		*txt;

    if ( IS_SET( obj->value[0], TRIG_UP ) )
      isup = TRUE;
    else
      isup = FALSE;
    switch( obj->item_type )
    {
	default:
	  sprintf( buf, "¡No puedes %s eso!\n\r", pull ? "tirar de" : "pulsar" );
	  send_to_char( buf, ch );
	  return;
	  break;
	case ITEM_SWITCH:
	case ITEM_LEVER:
	case ITEM_PULLCHAIN:
	  if ( (!pull && isup) || (pull && !isup) )
	  {
		sprintf( buf, "ya está %s.\n\r", isup ? "arriba" : "abajo" );
		send_to_char( buf, ch );
		return;
 	  }
	case ITEM_BUTTON:
	  if ( (!pull && isup) || (pull & !isup) )
	  {
		sprintf( buf, "Ya está %s.\n\r", isup ? "dentro" : "fuera" );
		send_to_char( buf, ch );
		return;
	  }
	  break;
    }
    if( (pull) && IS_SET(obj->pIndexData->progtypes,PULL_PROG) )
    {
	if ( !IS_SET(obj->value[0], TRIG_AUTORETURN ) )
 	  REMOVE_BIT( obj->value[0], TRIG_UP );
 	oprog_pull_trigger( ch, obj );
        return;
    }
    if( (!pull) && IS_SET(obj->pIndexData->progtypes,PUSH_PROG) )
    {
	if ( !IS_SET(obj->value[0], TRIG_AUTORETURN ) )
	  SET_BIT( obj->value[0], TRIG_UP );
	oprog_push_trigger( ch, obj );
        return;
    }

    if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
    {
      sprintf( buf, "$n %s $p.", pull ? "tira de" : "pulsa" );
      act( AT_ACTION, buf,  ch, obj, NULL, TO_ROOM );
      sprintf( buf, "%s $p.", pull ? "tiras de" : "pulsas" );
      act( AT_ACTION, buf, ch, obj, NULL, TO_CHAR );
    }

    if ( !IS_SET(obj->value[0], TRIG_AUTORETURN ) )
    {
	if ( pull )
	  REMOVE_BIT( obj->value[0], TRIG_UP );
	else
	  SET_BIT( obj->value[0], TRIG_UP );
    }
    if ( IS_SET( obj->value[0], TRIG_TELEPORT )
    ||   IS_SET( obj->value[0], TRIG_TELEPORTALL )
    ||   IS_SET( obj->value[0], TRIG_TELEPORTPLUS ) )
    {
	int flags;

	if ( ( room = get_room_index( obj->value[1] ) ) == NULL )
	{
	    bug( "PullOrPush: obj points to invalid room %d", obj->value[1] );
	    return;
	}
	flags = 0;
	if ( IS_SET( obj->value[0], TRIG_SHOWROOMDESC ) )
	  SET_BIT( flags, TELE_SHOWDESC );
	if ( IS_SET( obj->value[0], TRIG_TELEPORTALL ) )
	  SET_BIT( flags, TELE_TRANSALL );
	if ( IS_SET( obj->value[0], TRIG_TELEPORTPLUS ) )
	  SET_BIT( flags, TELE_TRANSALLPLUS );

	teleport( ch, obj->value[1], flags );
	return;
    }

    if ( IS_SET( obj->value[0], TRIG_RAND4 )
    ||	 IS_SET( obj->value[0], TRIG_RAND6 ) )
    {
	int maxd;

	if ( ( room = get_room_index( obj->value[1] ) ) == NULL )
	{
	    bug( "PullOrPush: obj points to invalid room %d", obj->value[1] );
	    return;
	}

	if ( IS_SET( obj->value[0], TRIG_RAND4 ) )
	  maxd = 3;
	else
	  maxd = 5;

	randomize_exits( room, maxd );
	for ( rch = room->first_person; rch; rch = rch->next_in_room )
	{
	   send_to_char( "Escuchas un extruendoso sonido.\n\r", rch );
	   send_to_char( "Algo es diferente...\n\r", rch );
	}
    }
    if ( IS_SET( obj->value[0], TRIG_DOOR ) )
    {
	room = get_room_index( obj->value[1] );
	if ( !room )
	  room = obj->in_room;
	if ( !room )
	{
	  bug( "PullOrPush: obj points to invalid room %d", obj->value[1] );
	  return;
	}	
	if ( IS_SET( obj->value[0], TRIG_D_NORTH ) )
	{
	  edir = DIR_NORTH;
	  txt = "al norte";
	}
	else
	if ( IS_SET( obj->value[0], TRIG_D_SOUTH ) )
	{
	  edir = DIR_SOUTH;
	  txt = "Al sur";
	}
	else
	if ( IS_SET( obj->value[0], TRIG_D_EAST ) )
	{
	  edir = DIR_EAST;
	  txt = "al este";
	}
	else
	if ( IS_SET( obj->value[0], TRIG_D_WEST ) )
	{
	  edir = DIR_WEST;
	  txt = "al oeste";
	}
	else
	if ( IS_SET( obj->value[0], TRIG_D_UP ) )
	{
	  edir = DIR_UP;
	  txt = "desde arriba";
	}
	else
	if ( IS_SET( obj->value[0], TRIG_D_DOWN ) )
	{
	  edir = DIR_DOWN;
	  txt = "desde abajo";
	}
	else
	{
	  bug( "PullOrPush: door: no direction flag set.", 0 );
	  return;
	}
	pexit = get_exit( room, edir );
	if ( !pexit )
	{
	    if ( !IS_SET( obj->value[0], TRIG_PASSAGE ) )
	    {
		bug( "PullOrPush: obj points to non-exit %d", obj->value[1] );
		return;
	    }
	    to_room = get_room_index( obj->value[2] );
	    if ( !to_room )
	    {
		bug( "PullOrPush: dest points to invalid room %d", obj->value[2] );
		return;
	    }
	    pexit = make_exit( room, to_room, edir );
	    pexit->keyword	= STRALLOC( "" );
	    pexit->description	= STRALLOC( "" );
	    pexit->key		= -1;
	    pexit->exit_info	= 0;
	    top_exit++;
	    act( AT_PLAIN, "¡Se abre una salida!", ch, NULL, NULL, TO_CHAR );
	    act( AT_PLAIN, "¡Se abre una salida!", ch, NULL, NULL, TO_ROOM );
	    return;
	}
	if ( IS_SET( obj->value[0], TRIG_UNLOCK )
	&&   IS_SET( pexit->exit_info, EX_LOCKED) )
	{
	    REMOVE_BIT(pexit->exit_info, EX_LOCKED);
	    act( AT_PLAIN, "Escuchas un click $T.", ch, NULL, txt, TO_CHAR );
	    act( AT_PLAIN, "Escuchas un click $T.", ch, NULL, txt, TO_ROOM );
	    if ( ( pexit_rev = pexit->rexit ) != NULL
	    &&   pexit_rev->to_room == ch->in_room )
		REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
	    return;
	}
	if ( IS_SET( obj->value[0], TRIG_LOCK   )
	&&  !IS_SET( pexit->exit_info, EX_LOCKED) )
	{
	    SET_BIT(pexit->exit_info, EX_LOCKED);
	    act( AT_PLAIN, "Escuchas un click $T.", ch, NULL, txt, TO_CHAR );
	    act( AT_PLAIN, "Escuchas un click $T.", ch, NULL, txt, TO_ROOM );
	    if ( ( pexit_rev = pexit->rexit ) != NULL
	    &&   pexit_rev->to_room == ch->in_room )
		SET_BIT( pexit_rev->exit_info, EX_LOCKED );
	    return;
	}
	if ( IS_SET( obj->value[0], TRIG_OPEN   )
	&&   IS_SET( pexit->exit_info, EX_CLOSED) )
	{
	    REMOVE_BIT(pexit->exit_info, EX_CLOSED);
	    for ( rch = room->first_person; rch; rch = rch->next_in_room )
		act( AT_ACTION, "$d se abre.", rch, NULL, pexit->keyword, TO_CHAR );
	    if ( ( pexit_rev = pexit->rexit ) != NULL
	    &&   pexit_rev->to_room == ch->in_room )
	    {
		REMOVE_BIT( pexit_rev->exit_info, EX_CLOSED );
		for ( rch = to_room->first_person; rch; rch = rch->next_in_room )
		    act( AT_ACTION, "$d se abre.", rch, NULL, pexit_rev->keyword, TO_CHAR );
	    }
	    check_room_for_traps( ch, trap_door[edir]);
	    return;
	}
	if ( IS_SET( obj->value[0], TRIG_CLOSE   )
	&&  !IS_SET( pexit->exit_info, EX_CLOSED) )
	{
	    SET_BIT(pexit->exit_info, EX_CLOSED);
	    for ( rch = room->first_person; rch; rch = rch->next_in_room )
		act( AT_ACTION, "$d se cierra.", rch, NULL, pexit->keyword, TO_CHAR );
	    if ( ( pexit_rev = pexit->rexit ) != NULL
	    &&   pexit_rev->to_room == ch->in_room )
	    {
		SET_BIT( pexit_rev->exit_info, EX_CLOSED );
		for ( rch = to_room->first_person; rch; rch = rch->next_in_room )
		    act( AT_ACTION, "$d se cierra.", rch, NULL, pexit_rev->keyword, TO_CHAR );
	    }
	    check_room_for_traps( ch, trap_door[edir]);
	    return;
	}
    }
}


void do_pull( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "¿tirar de qué?\n\r", ch );
	return;
    }

    if ( ms_find_obj(ch) )
	return;

    if ( ( obj = get_obj_here( ch, arg ) ) == NULL )
    {
	act( AT_PLAIN, "No ves $T aquí.", ch, NULL, arg, TO_CHAR );
	return;
    }

    pullorpush( ch, obj, TRUE );
}

void do_push( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "¿Pulsar qué?\n\r", ch );
	return;
    }

    if ( ms_find_obj(ch) )
	return;

    if ( ( obj = get_obj_here( ch, arg ) ) == NULL )
    {
	act( AT_PLAIN, "No ves $T aquí.", ch, NULL, arg, TO_CHAR );
	return;
    }

    pullorpush( ch, obj, FALSE );
}

/* pipe commands (light, tamp, smoke) by Thoric */
void do_tamp( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *pipe;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "¿Encender qué?\n\r", ch );
	return;
    }

    if ( ms_find_obj(ch) )
	return;

    if ( (pipe = get_obj_carry( ch, arg )) == NULL )
    {
	send_to_char( "No estás cargando eso.\n\r", ch );
	return;
    }
    if ( pipe->item_type != ITEM_PIPE )
    {
	send_to_char( "No puedes apagar eso.\n\r", ch );
	return;
    }
    if ( !IS_SET( pipe->value[3], PIPE_TAMPED ) )
    {
	act( AT_ACTION, "Apagas $p.", ch, pipe, NULL, TO_CHAR );
	act( AT_ACTION, "$n apaga $p.", ch, pipe, NULL, TO_ROOM );
	SET_BIT( pipe->value[3], PIPE_TAMPED );
	return;
    }
    send_to_char( "No es necesario.\n\r", ch );
}

void do_smoke( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *pipe;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "¿Fumar qué?\n\r", ch );
	return;
    }

    if ( ms_find_obj(ch) )
	return;

    if ( (pipe = get_obj_carry( ch, arg )) == NULL )
    {
	send_to_char( "No estás cargando eso.\n\r", ch );
	return;
    }
    if ( pipe->item_type != ITEM_PIPE )
    {
	act( AT_ACTION, "Tratas de fumar $p... Pero es imposible.", ch, pipe, NULL, TO_CHAR );
	act( AT_ACTION, "$n intenta fumarse  $p... (ya me dirás cómo)", ch, pipe, NULL, TO_ROOM );
	return;
    }
    if ( !IS_SET( pipe->value[3], PIPE_LIT ) )
    {
	act( AT_ACTION, "Intentas fumar $p, pero no está encendida.", ch, pipe, NULL, TO_CHAR );
	act( AT_ACTION, "$n trata de fumar $p, pero no está encendida.", ch, pipe, NULL, TO_ROOM );
	return;
    }
    if ( pipe->value[1] > 0 )
    {
	if ( !oprog_use_trigger( ch, pipe, NULL, NULL, NULL ) )
	{
	   act( AT_ACTION, "Aspiras lentamente de $p.", ch, pipe, NULL, TO_CHAR );
	   act( AT_ACTION, "$n aspira lentamente de $p.", ch, pipe, NULL, TO_ROOM );
	}

	if ( IS_VALID_HERB( pipe->value[2] ) && pipe->value[2] < top_herb )
	{
	    int sn		= pipe->value[2] + TYPE_HERB;
	    SKILLTYPE *skill	= get_skilltype( sn );

	    WAIT_STATE( ch, skill->beats );
	    if ( skill->spell_fun )
		obj_cast_spell( sn, UMIN(skill->min_level, ch->top_level),
			ch, ch, NULL );
	    if ( obj_extracted( pipe ) )
		return;
	}
	else
	    bug( "do_smoke: bad herb type %d", pipe->value[2] );

	SET_BIT( pipe->value[3], PIPE_HOT );
	if ( --pipe->value[1] < 1 )
	{
	   REMOVE_BIT( pipe->value[3], PIPE_LIT );
	   SET_BIT( pipe->value[3], PIPE_DIRTY );
	   SET_BIT( pipe->value[3], PIPE_FULLOFASH );
	}
    }
}

void do_light( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *pipe;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "¿encender qué?\n\r", ch );
	return;
    }

    if ( ms_find_obj(ch) )
	return;

    if ( (pipe = get_obj_carry( ch, arg )) == NULL )
    {
	send_to_char( "No estás cargando eso.\n\r", ch );
	return;
    }
    if ( pipe->item_type != ITEM_PIPE )
    {
	send_to_char( "No puedes encender eso.\n\r", ch );
	return;
    }
    if ( !IS_SET( pipe->value[3], PIPE_LIT ) )
    {
	if ( pipe->value[1] < 1 )
	{
	  act( AT_ACTION, "Intentas encender $p, pero no contiene nada.", ch, pipe, NULL, TO_CHAR );
	  act( AT_ACTION, "$n intenta encender $p, pero no contiene nada.", ch, pipe, NULL, TO_ROOM );
	  return;
	}
	act( AT_ACTION, "Enciendes cuidadosamente $p.", ch, pipe, NULL, TO_CHAR );
	act( AT_ACTION, "$n enciende cuidadodsamente $p.", ch, pipe, NULL, TO_ROOM );
	SET_BIT( pipe->value[3], PIPE_LIT );
	return;
    }
    send_to_char( "No es necesario.\n\r", ch );
}

void do_empty( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( !str_cmp( arg2, "into" ) && argument[0] != '\0' )
	argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "¿Vaciar qué?\n\r", ch );
	return;
    }
    if ( ms_find_obj(ch) )
	return;

    if ( (obj = get_obj_carry( ch, arg1 )) == NULL )
    {
	send_to_char( "No tienes ese objeto.\n\r", ch );
	return;
    }
    if ( obj->count > 1 )
      separate_obj(obj);

    switch( obj->item_type )
    {
	default:
	  act( AT_ACTION, "Sacudes $p en un intento de vaciarlo...", ch, obj, NULL, TO_CHAR );
	  act( AT_ACTION, "$n empieza a sacudir $p en un claro intento de vaciarlo...", ch, obj, NULL, TO_ROOM );
	  return;
	case ITEM_PIPE:
	  act( AT_ACTION, "Golpeas suavemente $p y lo vacías.", ch, obj, NULL, TO_CHAR );
	  act( AT_ACTION, "$n golpea suavemente $p y lo vacía.", ch, obj, NULL, TO_ROOM );
	  REMOVE_BIT( obj->value[3], PIPE_FULLOFASH );
	  REMOVE_BIT( obj->value[3], PIPE_LIT );
	  obj->value[1] = 0;
	  return;
	case ITEM_DRINK_CON:
	  if ( obj->value[1] < 1 )
	  {
		send_to_char( "No es necesario.\n\r", ch );
		return;
	  }
	  act( AT_ACTION, "Vacías $p.", ch, obj, NULL, TO_CHAR );
	  act( AT_ACTION, "$n vacía $p.", ch, obj, NULL, TO_ROOM );
	  obj->value[1] = 0;
	  return;
	case ITEM_CONTAINER:
	  if ( IS_SET(obj->value[1], CONT_CLOSED) )
	  {
		act( AT_PLAIN, "abre $d primero.", ch, NULL, obj->name, TO_CHAR );
		return;
	  }
	  if ( !obj->first_content )
	  {
		send_to_char( "No es necesario.\n\r", ch );
		return;
	  }
	  if ( arg2[0] == '\0' )
	  {
		if ( IS_SET( ch->in_room->room_flags, ROOM_NODROP )
		|| ( !IS_NPC(ch) &&  IS_SET( ch->act, PLR_LITTERBUG ) ) )
		{
		       set_char_color( AT_MAGIC, ch );
		       send_to_char( "Algo te dice que vaciar aquí eso sería una mala idea.\n\r", ch );
		       return;
		}
		if ( IS_SET( ch->in_room->room_flags, ROOM_NODROPALL ) )
		{
		   send_to_char( "No encuentras la forma de hacer eso aquí...\n\r", ch );
		   return;
		}
		if ( empty_obj( obj, NULL, ch->in_room ) )
		{
		    act( AT_ACTION, "Vacías $p.", ch, obj, NULL, TO_CHAR );
		    act( AT_ACTION, "$n vacía $p.", ch, obj, NULL, TO_ROOM );
		    if ( IS_SET( sysdata.save_flags, SV_DROP ) )
			save_char_obj( ch );
		}
		else
		    send_to_char( "Hmmm... Eso no funciona.\n\r", ch );
	  }
	  else
	  {
		OBJ_DATA *dest = get_obj_here( ch, arg2 );

		if ( !dest )
		{
		    send_to_char( "No lo encuentras.\n\r", ch );
		    return;
		}
		if ( dest == obj )
		{
		    send_to_char( "¿Tratando de doblar la realidad!\n\r", ch );
		    return;
		}
		if ( dest->item_type != ITEM_CONTAINER )
		{
		    send_to_char( "¡Eso no es un contenedor!\n\r", ch );
		    return;
		}
		if ( IS_SET(dest->value[1], CONT_CLOSED) )
		{
		    act( AT_PLAIN, "Abre $d primero.", ch, NULL, dest->name, TO_CHAR );
		    return;
		}
		separate_obj( dest );
		if ( empty_obj( obj, dest, NULL ) )
		{
		    act( AT_ACTION, "Vacías $p en $P.", ch, obj, dest, TO_CHAR );
		    act( AT_ACTION, "$n vacía $p en $P.", ch, obj, dest, TO_ROOM );
		    if ( !dest->carried_by
		    &&    IS_SET( sysdata.save_flags, SV_PUT ) )
			save_char_obj( ch );
		}
		else
		    act( AT_ACTION, "$P no tiene hueco para nada más en su interior.", ch, obj, dest, TO_CHAR );
	  }
	  return;
    }
}
 
/*
 * Apply a salve/ointment					-Thoric
 */
void do_apply( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    ch_ret retcode;

    if ( argument[0] == '\0' )
    {
	send_to_char( "¿aplicar qué?\n\r", ch );
	return;
    }

    if ( ms_find_obj(ch) )
	return;

    if ( ( obj = get_obj_carry( ch, argument ) ) == NULL )
    {
	send_to_char( "no tienes eso.\n\r", ch );
	return;
    }

    if ( obj->item_type != ITEM_SALVE )
    {
	act( AT_ACTION, "$n Comienza a frotarse con $p...",  ch, obj, NULL, TO_ROOM );
	act( AT_ACTION, "Te frotas $p...", ch, obj, NULL, TO_CHAR );
	return;
    }

    separate_obj( obj );

    --obj->value[1];
    if ( !oprog_use_trigger( ch, obj, NULL, NULL, NULL ) )
    {
	if ( !obj->action_desc || obj->action_desc[0]=='\0' )
	{
	    act( AT_ACTION, "$n frota $p por todo su cuerpo.",  ch, obj, NULL, TO_ROOM );
	    if ( obj->value[1] <= 0 )
		act( AT_ACTION, "Aplicas lo último que queda de $p sobre tu cuerpo.", ch, obj, NULL, TO_CHAR );
	    else
		act( AT_ACTION, "Aplicas $p sobre tu cuerpo.", ch, obj, NULL, TO_CHAR );
	}
	else
	    actiondesc( ch, obj, NULL ); 
    }

    WAIT_STATE( ch, obj->value[2] );
    retcode = obj_cast_spell( obj->value[4], obj->value[0], ch, ch, NULL );
    if ( retcode == rNONE )
	retcode = obj_cast_spell( obj->value[5], obj->value[0], ch, ch, NULL );

    if ( !obj_extracted(obj) && obj->value[1] <= 0 )
	extract_obj( obj );

    return;
}

void actiondesc( CHAR_DATA *ch, OBJ_DATA *obj, void *vo )
{
    char charbuf[MAX_STRING_LENGTH];
    char roombuf[MAX_STRING_LENGTH];
    char *srcptr = obj->action_desc;
    char *charptr = charbuf;
    char *roomptr = roombuf;
    const char *ichar;
    const char *iroom;

while ( *srcptr != '\0' )
{
  if ( *srcptr == '$' ) 
  {
    srcptr++;
    switch ( *srcptr )
    {
      case 'e':
        ichar = "tu";
        iroom = "$e";
        break;

      case 'm':
        ichar = "tu";
        iroom = "$m";
        break;

      case 'n':
        ichar = "tu";
        iroom = "$n";
        break;

      case 's':
        ichar = "tu";
        iroom = "$s";
        break;

      /*case 'q':
        iroom = "s";
        break;*/

      default: 
        srcptr--;
        *charptr++ = *srcptr;
        *roomptr++ = *srcptr;
        break;
    }
  }
  else if ( *srcptr == '%' && *++srcptr == 's' ) 
  {
    ichar = "Tu";
    iroom = "$n";
  }
  else
  {
    *charptr++ = *srcptr;
    *roomptr++ = *srcptr;
    srcptr++;
    continue;
  }

  while ( ( *charptr = *ichar ) != '\0' )
  {
    charptr++;
    ichar++;
  }

  while ( ( *roomptr = *iroom ) != '\0' )
  {
    roomptr++;
    iroom++;
  }
  srcptr++;
}

*charptr = '\0';
*roomptr = '\0';

/*
sprintf( buf, "Charbuf: %s", charbuf );
log_string_plus( buf, LOG_HIGH, LEVEL_LESSER ); 
sprintf( buf, "Roombuf: %s", roombuf );
log_string_plus( buf, LOG_HIGH, LEVEL_LESSER ); 
*/

switch( obj->item_type )
{
  case ITEM_BLOOD:
  case ITEM_FOUNTAIN:
    act( AT_ACTION, charbuf, ch, obj, ch, TO_CHAR );
    act( AT_ACTION, roombuf, ch, obj, ch, TO_ROOM );
    return;

  case ITEM_DRINK_CON:
    act( AT_ACTION, charbuf, ch, obj, liq_table[obj->value[2]].liq_name, TO_CHAR );
    act( AT_ACTION, roombuf, ch, obj, liq_table[obj->value[2]].liq_name, TO_ROOM );
    return;

  case ITEM_PIPE:
    return;

  case ITEM_ARMOR:
  case ITEM_WEAPON:
  case ITEM_LIGHT:
    act( AT_ACTION, charbuf, ch, obj, ch, TO_CHAR );
    act( AT_ACTION, roombuf, ch, obj, ch, TO_ROOM );
    return;
 
  case ITEM_FOOD:
  case ITEM_PILL:
    act( AT_ACTION, charbuf, ch, obj, ch, TO_CHAR );
    act( AT_ACTION, roombuf, ch, obj, ch, TO_ROOM );
    return;

  default:
    return;
}
return;
}

void do_hail( CHAR_DATA *ch , char *argument )
{
    int vnum;
    ROOM_INDEX_DATA *room;
    
    if ( !ch->in_room )
       return;

    if ( ch->position < POS_FIGHTING )
    {
       send_to_char( "¡Finaliza el combate primero!\n\r", ch );
       return;
    }
    
    if ( ch->position < POS_STANDING )
    {
       send_to_char( "¡Levántate primero!\n\r", ch );
       return;
    }
    
    if ( IS_SET( ch->in_room->room_flags , ROOM_INDOORS ) )
    {
       send_to_char( "¡Debes estar fuera para hacer eso!\n\r", ch );
       return;
    }
    
    if ( IS_SET( ch->in_room->room_flags , ROOM_SPACECRAFT ) )
    {
       send_to_char( "¡No puedes hacer eso en el espacio!\n\r", ch );
       return;
    }

    if ( ch->gold < (ch->top_level-9)  )
    {
       send_to_char( "¡No tienes tantos créditos!\n\r", ch );
       return;
    }

    if (!IS_NPC(ch) && IS_SET(ch->pcdata->act2, ACT_BOUND))
    {
	send_to_char( "¡Alzas la mano para llamar un taxi pero pasa de ti!\n\r", ch);
	return;
    }

    vnum = ch->in_room->vnum;
    
    for ( vnum = ch->in_room->area->low_r_vnum  ;  vnum <= ch->in_room->area->hi_r_vnum  ;  vnum++ )
    {
            room = get_room_index ( vnum );
            
            if ( room != NULL ){
             if ( IS_SET(room->room_flags , ROOM_HOTEL ) )
                break;   
             else 
                room = NULL;   
            }
    }
    
    if ( room == NULL )
    {
       send_to_char( "¡No ves ningún taxi cerca!\n\r", ch );
       return;
    }
    
    ch->gold -= UMAX(ch->top_level-9 , 0);
    
    act( AT_ACTION, "$n llama a un aerodeslizador y se marcha en busca del sitio seguro más cercano.", ch, NULL, NULL,  TO_ROOM );

    char_from_room( ch );
    char_to_room( ch, room );
   
    send_to_char( "Un aerodeslizador te lleva al sitio seguro más cercano.\n\rpagas al conductor 20 créditos.\n\r\n\n" , ch );
    act( AT_ACTION, "$n $T", ch, NULL, "llega en un aerodeslizador y paga al conductor antes de que se marche.",  TO_ROOM );
                               
    do_look( ch, "auto" );
   
}

void do_train( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *mob;
    bool tfound = FALSE;
    bool successful = FALSE;
    
    if ( IS_NPC(ch) )
	return;

    strcpy( arg, argument );    
    
    /*switch( ch->substate )
    { 
    	default:*/

	    	if ( arg[0] == '\0' )
                {
                   send_to_char( "¿entrenar qué?\n\r", ch );
	           send_to_char( "\n\rOpciones: Fuerza, inteligencia, sabiduría, destreza, constitución o carisma\n\r", ch );
	           return;	
                }
    
	    	if ( !IS_AWAKE(ch) )
	    	{
	          send_to_char( "¿en tus sueños?\n\r", ch );
	          return;
	    	}

	    	for ( mob = ch->in_room->first_person; mob; mob = mob->next_in_room )
	       	   if ( IS_NPC(mob) && IS_SET(mob->act, ACT_TRAIN) )
	           {
	              tfound = TRUE;
	    	      break;
                   }
            
	    	if ( (!mob) || (!tfound) )
	    	{
	          send_to_char( "No puedes hacer eso aquí.\n\r", ch );
	          return;
	    	}

	        if ( str_cmp( arg, "fue" ) && str_cmp( arg, "fuerza" )
	        && str_cmp( arg, "des" ) && str_cmp( arg, "destreza" )
                && str_cmp( arg, "con" ) && str_cmp( arg, "constitucion" )
                && str_cmp( arg, "car" ) && str_cmp( arg, "carisma" )
                && str_cmp( arg, "sab" ) && str_cmp( arg, "sabiduria" )
                && str_cmp( arg, "int" ) && str_cmp( arg, "inteligencia" ) )
                {
                    do_train ( ch , "" );
                    return;
                }	        
                
                if ( !str_cmp( arg, "fue" ) || !str_cmp( arg, "fuerza" ) )
                {
                      if( mob->perm_str <= ch->perm_str || ch->perm_str >= 20 + race_table[ch->race].str_plus || ch->perm_str >= 25 )
                      {
                          act( AT_TELL, "$n te cuenta 'Ya eres más fuerte que yo.'",
		             mob, NULL, ch, TO_VICT );
		          return;
                      }
                      send_to_char("&GComienzas tu entrenamiento para hacerte más fuerte.\n\r", ch);
                }
          	if ( !str_cmp( arg, "des" ) || !str_cmp( arg, "destreza" ) )
	    	{
                      if( mob->perm_dex <= ch->perm_dex || ch->perm_dex >= 20 + race_table[ch->race].dex_plus || ch->perm_dex >= 25 )
                      {
                          act( AT_TELL, "$n te cuenta 'Ya eres más hábil que yo.'",
		             mob, NULL, ch, TO_VICT );
		          return;
                      }
                      send_to_char("&GComienzas una serie de pruebas de coordinación.\n\r", ch);
                }
          	if ( !str_cmp( arg, "int" ) || !str_cmp( arg, "inteligencia" ) )
    		{
                      if( mob->perm_int <= ch->perm_int || ch->perm_int >= 20 + race_table[ch->race].int_plus || ch->perm_int >= 25 )
                      {
                          act( AT_TELL, "$n te cuenta 'Ya eres más inteligente que yo.'",
		             mob, NULL, ch, TO_VICT );
		          return;
                      }
                      send_to_char("&GComienzas a estudiar.\n\r", ch);
                }
          	if ( !str_cmp( arg, "sab" ) || !str_cmp( arg, "sabiduria" ) )
    		{
                      if( mob->perm_wis <= ch->perm_wis || ch->perm_wis >= 20 + race_table[ch->race].wis_plus || ch->perm_wis >= 25 )
                      {
                          act( AT_TELL, "$n te cuenta 'Ya sabes más que yo.'",
		             mob, NULL, ch, TO_VICT );
		          return;
                      }
                      send_to_char("&GComienzas a contemplar varios textos antiguos en un esfuerzo por adquirir sabiduría.\n\r", ch);
                }
          	if ( !str_cmp( arg, "con" ) || !str_cmp( arg, "constitucion" ) )
    		{
                      if( mob->perm_con <= ch->perm_con || ch->perm_con >= 20 + race_table[ch->race].con_plus || ch->perm_con >= 25 )
                      {
                          act( AT_TELL, "$n te cuenta 'Ya eres más saludable que yo.'",
		             mob, NULL, ch, TO_VICT );
		          return;
                      }
                      send_to_char("&GComienzas un reguroso entrenamiento para aumentar tu aguante.\n\r", ch);
                }
          	if ( !str_cmp( arg, "car" ) || !str_cmp( arg, "carisma" ) )
            	{
                      if( mob->perm_cha <= ch->perm_cha || ch->perm_cha >= 20 + race_table[ch->race].cha_plus || ch->perm_cha >= 25 )
                      {
                          act( AT_TELL, "$n te cuenta 'Ya tienes más carisma que yo.'",
		             mob, NULL, ch, TO_VICT );
		          return;
                      }
                      send_to_char("&GComienzas lecciones de maneras y etiqueta.\n\r", ch);
                }
            	//add_timer ( ch , TIMER_DO_FUN , 10 , do_train , 1 );
    	    	//ch->dest_buf = str_dup(arg);
    	    	//return;
    	
    	/*case 1:
    		if ( !ch->dest_buf )
    		   return;
    		strcpy(arg, ch->dest_buf);
    		DISPOSE( ch->dest_buf);
    		break;
    		
    	case SUB_TIMER_DO_ABORT:
    		DISPOSE( ch->dest_buf );
    		ch->substate = SUB_NONE;
    	        send_to_char("&RNo consigues nada con el entrenamiento.\n\r", ch);
    		return;
    }
    
    ch->substate = SUB_NONE;
    
    if ( number_bits ( 2 ) == 0 )
    {
        successful = TRUE;
    }
    */
    successful = TRUE;
    if ( !str_cmp( arg, "fue" ) || !str_cmp( arg, "fuerza" ) )
    {
        if ( !successful )
        {
             send_to_char("&RSientes que te has esforzado para nada...\n\r", ch);
             return;	
        } 
        send_to_char("&GDespués de mucho ejercicio te sientes más fuerte.\n\r", ch);
    	ch->perm_str++;
    	return;
    }
    
    if ( !str_cmp( arg, "des" ) || !str_cmp( arg, "destreza" ) )
    {
        if ( !successful )
        {
             send_to_char("&RTanto esfuerzo... para nada...\n\r", ch);
             return;	
        } 
        send_to_char("&GDespués de tanto trabajo sientes que tu cordinación es mejor.\n\r", ch);
    	ch->perm_dex++;
    	return;
    }
    
    if ( !str_cmp( arg, "int" ) || !str_cmp( arg, "inteligencia" ) )
    {
        if ( !successful )
        {
             send_to_char("&RHas conseguido que te duelan los ojos...\n\r", ch);
             return;	
        } 
        send_to_char("&GDespués de tanto estudiar sientes que has aprendido algo nuevo.\n\r", ch);
    	ch->perm_int++;
    	return;
    }
    
    if ( !str_cmp( arg, "sab" ) || !str_cmp( arg, "sabiduria" ) )
    {
        if ( !successful )
        {
             send_to_char("&RTanto estudiar textos antiguos solo consigue causarte una gran confusión...\n\r", ch);
             return;	
        } 
        send_to_char("&GDespués de contemplar varios eventos sin sentido\n\rrecibes conocimientos sobre el universo.\n\r", ch);
    	ch->perm_wis++;
    	return;
    }
    
    if ( !str_cmp( arg, "con" ) || !str_cmp( arg, "constitucion" ) )
    {
        if ( !successful )
        {
             send_to_char("&RTanto ejercicio para nada...\n\r", ch);
             return;	
        } 
        send_to_char("&GSientes que tu aguante ha aumentado.\n\r", ch);
    	ch->perm_con++;
    	return;
    }
    
    
    if ( !str_cmp( arg, "car" ) || !str_cmp( arg, "carisma" ) )
    {
        if ( !successful )
        {
             send_to_char("&RSientes una enorme depresión al finalzar tu entrenamiento.\n\r", ch);
             return;	
        } 
        send_to_char("&GDespués de mucho estudiar etiqueta te sientes con más carisma.\n\r", ch);
    	ch->perm_cha++;
    	return;
    }
    	
}

void do_suicide( CHAR_DATA *ch, char *argument )
{
        char  logbuf[MAX_STRING_LENGTH];
	OBJ_DATA *wield;
   
        if ( IS_NPC(ch) || !ch->pcdata )
        {
            send_to_char( "un mob!\n\r", ch );
	    return;
        }
        
        if ( argument[0] == '\0' )
        {
            send_to_char( "&RSi quieres borrar el personaje teclea suicidar y tu contraseña.\n\r", ch );
	    return;
        }
        
        if ( strcmp( smaug_crypt( argument), ch->pcdata->pwd ) )
	{
	    send_to_char( "contraseña incorrecta.\n\r", ch );
	    sprintf( logbuf , "%s attempting to commit suicide... WRONG PASSWORD!" , ch->name );
	    log_string( logbuf );
	    return;
	}

	wield = get_eq_char(ch, WEAR_WIELD);
	
	if(!wield)
	{
	   act( AT_BLOOD, "Te metes los pulgares en los ojos... La sangre salpica el suelo...\n\r",  ch, NULL, NULL, TO_CHAR    );
    	   act( AT_BLOOD, "$n se suicida metiéndose los pulgares en los ojos.", ch, NULL, NULL, TO_ROOM ); 
	}
	else
	{
	 switch (wield->value[3])
         {
	 default:
	   act( AT_BLOOD, "You ram your thumbs into your eyesockets, blood spurts onto the floor.\n\r",  ch, NULL, NULL, TO_CHAR    );
    	   act( AT_BLOOD, "Blood spurts onto your shoes as $n rams his thumbs into $s eyesockets.", ch, NULL, NULL, TO_ROOM ); 
	   break;

	 case WEAPON_BLASTER:
	   act( AT_BLOOD, "You calmly place the barrel of your blaster into your mouth, and a spurt of brains falls to the ground behind you.", ch, NULL, NULL, TO_CHAR    );
	   act( AT_BLOOD, "A spurt of brains hits the ground as $n blows $s head off.", ch, NULL, NULL, TO_ROOM );
	   break;

	 case WEAPON_VIBRO_BLADE:
           act( AT_BLOOD, "With a sad determination and trembling hands you slit your own throat!",  ch, NULL, NULL, TO_CHAR    );
           act( AT_BLOOD, "Cold shivers run down your spine as you watch $n slit $s own throat!",  ch, NULL, NULL, TO_ROOM );
	   break;

	 case WEAPON_FORCE_PIKE:
	   act( AT_BLOOD, "You wedge your force pike into the ground and slam your face into the blade.", ch, NULL, NULL, TO_CHAR    );
	   act( AT_BLOOD, "$n slams $s face into $s force pike, pieces of face fall to the ground as the pike slices through it like butter.", ch, NULL, NULL, TO_ROOM );
	   break;

	 case WEAPON_LIGHTSABER:
	 case WEAPON_DUAL_LIGHTSABER:
	   act( AT_BLOOD, "You calmly kneel, and hold your saber hilt to your sternum, and without a moments hesitation, turn it on.", ch, NULL, NULL, TO_CHAR );
	   act( AT_BLOOD, "$n calmly kneels and holds $s saber hilt to $s chest, and turns it on. $e falls over in agony.", ch, NULL, NULL, TO_ROOM );
	   break;

	 case WEAPON_BOWCASTER:
           act( AT_BLOOD, "You pull back the lever, hold the bowcaster to your face and... death.", ch, NULL, NULL, TO_CHAR );
           act( AT_BLOOD, "$n blows his head off with a bowcaster quarrel!", ch, NULL, NULL, TO_ROOM );
	   break;

	 }
	}

       sprintf(logbuf, "%s has commited suicide.", ch->name );
       log_string( logbuf );

       set_cur_char(ch);
       raw_kill( ch, ch );
}

void do_bank( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    char logbuf[MAX_INPUT_LENGTH];
    long amount = 0;
    
    argument = one_argument( argument , arg1 );
    argument = one_argument( argument , arg2 );
    argument = one_argument( argument , arg3 );

    if ( IS_NPC(ch) || !ch->pcdata )
       return;
    
    if (!ch->in_room || !IS_SET(ch->in_room->room_flags, ROOM_BANK) )
    {
       send_to_char( "¡Debes estar en un banco para hacer esto!\n\r", ch );
       return;
    }

    if ( arg1[0] == '\0' )
    {
       send_to_char( "Uso: banco <depositar|retirar|saldo|transferir> [cantidad] (personaje si transfieres)\n\r", ch );
       return;
    }

    if (arg2[0] != '\0' )
        amount = atoi(arg2);

    if ( !str_prefix( arg1 , "depositar" ) )
    {
       if ( amount  <= 0 )
       {
          send_to_char( "Debes introducir una cantidad mayor a 0.\n\r", ch );
          do_bank( ch , "" );
          return;
       }
       
       if ( ch->gold < amount )
       {
          send_to_char( "No tienes tantos créditos encima.\n\r", ch );
          return;
       }
       
       ch->gold -= amount;
       ch->pcdata->bank += amount;
       
       ch_printf( ch , "Depositas %ld créditos en tu cuenta.\n\r" ,amount );
       return;
    }
    else if ( !str_prefix( arg1 , "retirar" ) )
    {
       if ( amount  <= 0 )
       {
          send_to_char( "Debes introducir una cantidad mayor que 0.\n\r", ch );
          do_bank( ch , "" );
          return;
       }
     
       if ( ch->pcdata->bank < amount )
       {
          send_to_char( "No tienes tantos créditos en la cuenta.\n\r", ch );
          return;
       }

       ch->gold += amount;
       ch->pcdata->bank -= amount;
       
       ch_printf( ch , "Retiras %ld créditos de tu cuenta.\n\r" ,amount );
       return;

    }
    else if ( !str_prefix( arg1 , "cantidad" ) || !str_prefix( arg1, "saldo" ))
    {
        ch_printf( ch , "Tienes %ld créditos en el banco.\n\r" , ch->pcdata->bank );
        return; 
    }
    else if ( !str_prefix( arg1 , "transferir" ) )
    {
       victim = get_char_world(ch, arg3);

       if ( victim == NULL || IS_NPC(victim) )
       {
           send_to_char( "No está aquí.\n\r", ch );
           return;
       }

       if ( ch->top_level <= 10)
       {
	   send_to_char("Los personajes de tu nivel no puede mover créditos para evitar trampas.\n\r", ch);
	   return;
       }

      if ( ch->pcdata->bank < amount )
       {
          send_to_char( "No tienes tantos créditos.\n\r", ch );
          return;
       }

       if ( amount  <= 0 )
       {
          send_to_char( "Debes introducir una cantidad de créditos mayor a 0.\n\r", ch );
          return;
       }
       if(victim->pcdata->bank > 0 && victim->pcdata->bank + amount < 0)
       {
	  send_to_char( "No tienes tantos créditos en la cuenta!\n\r", ch);
	  return;
       }
	ch_printf( ch , "&W&GTransfieres %ld créditos a la cuenta de %s.\n\r", amount, arg3);
	sprintf( logbuf, "%s transfers %ld credits to %s", ch->name, amount, victim->name);
	log_string( logbuf ); 
        ch->pcdata->bank -= amount;
	victim->pcdata->bank += amount;
	ch_printf( ch, "Correcto.\n\r");
	ch_printf( victim, "&W&G%s ha depositado %ld créditos en tu cuenta.\n\r", ch->name, amount);
	return;
    }
    else
    {
      do_bank( ch , "" );
      return;
    }
    
        
}

void do_showsocial(CHAR_DATA *ch, char *argument)
{ 
  SOCIALTYPE *social;

  if(argument[0] == '\0' || !argument)
   {
	send_to_char("&RSyntax: showsocial <social>&W\n\r", ch);
	return;
   }

  social = find_social(argument);

  if(!social)
   {
	send_to_char("No such social.\n\r", ch);
	return;
   }

   ch_printf( ch, "&GSocial&B:&W %s\n\r\n\r&GChar, No Arg&B:&W %s\n\r", social->name, social->char_no_arg );
   ch_printf( ch, "&GOthers, No Arg&B:&W %s\n\r&GChar, Vict found&B:&W %s\n\r&GOthers, Vict Found&B:&W %s\n\r", social->others_no_arg ? social->others_no_arg : "(not set)", social->char_found ? social->char_found : "(not set)", social->others_found ? social->others_found  : "(not set)" );
   ch_printf( ch, "&GVict, Found&B:&W %s\n\r&GChar, Self &B:&W %s\n\r&GOthers, Self &B:&W %s\n\r", social->vict_found  ? social->vict_found    : "(not set)", social->char_auto   ? social->char_auto     : "(not set)", social->others_auto ? social->others_auto   : "(not set)" );
   return;
}

/* Defunct - Now use introduce to show people your last name
void do_lastname( CHAR_DATA *ch, char *argument )
{
  if ( IS_NPC(ch))
  {
    send_to_char("¡Los mobs no tienen apellidos!\n\r", ch);
    return;
  }

  if ( argument[0] == '\0' && ch->pcdata->last_name )
  {
    send_to_char("Apellido limpiado. Por favor usa 'apellido <argumento>' para ajustar tu apellido.\n\r", ch);
    ch->pcdata->last_name = NULL;
    return;
  }
  else if( argument[0] == '\0' )
  {
    send_to_char("¿y tu apellido será?\n\r", ch);
    return;
  }

  if ( !strcmp( argument, " " ) || !strcmp( argument, "&" ))
  {
    send_to_char("No puedes usar colores o espacios en tu apellido. Elije otro.\n\r", ch);  
    return;
  }

  if ( !check_parse_name( argument ) )
  {
    send_to_char("Apellido ilegal. Prueba otro.\n\r", ch);
    return;
  }

  argument[0] = UPPER(argument[0]);

  if (!str_cmp(ch->pcdata->last_name, argument))
  {
    send_to_char("¡Ese es tu apellido!\n\r", ch);
    return;
  }

  ch->pcdata->last_name = STRALLOC( argument );
  send_to_char("Apellido cambiado.\n\r", ch);
  return;
}
*/


void do_tune(CHAR_DATA *ch, char *argument){
  SHIP_DATA *ship;
  char buf[MAX_STRING_LENGTH];
  int num;
  if ((ship = ship_from_cockpit(ch->in_room->vnum)) == NULL){
  	send_to_char("Debes estar en la cabina de una nave para sintonizar su canal.\n\r", ch);
  	return;
  }
  
  if (argument[0] == '\0'){
  	send_to_char("Sintaxis: sintonizar <canal>\n\r", ch);
  	return;
  }
  
  num = atoi(argument);
  if (num > 100 || num < 0){
    send_to_char("El rango aceptado de canales es  1-100 o para público.\n\r", ch);
    return;
  }
  act(AT_WHITE, "Realizas algunos ajustes en el comunicador de la nave.", ch, NULL, NULL, TO_CHAR);
  act(AT_WHITE, "$n realiza algunos ajustes en el comunicador de la nave.", ch, NULL, NULL, TO_ROOM);
  sprintf(buf, "Canal de la nave configurado a %s%d.\n\r", num == 0 ? "el canal público" : "", num);
  send_to_char(buf, ch);
  ship->channel = num;
  save_ship(ship);
}
void do_whisper( CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *victim;
   char arg1[MAX_STRING_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char buf[MAX_INPUT_LENGTH];
   argument = one_argument(argument, arg1);
   strcpy(arg2,argument);
   if (( victim = get_char_room(ch, arg1)) == NULL)
   {
    send_to_char("No está aquí.\n\r", ch);
    return;
   }
    sprintf(buf, "&B[%ssusurrras&B] &Ca %s: &W%s%s\n\r", color_str( AT_WHISPER, ch), PERS(victim, ch), color_str(AT_WHISPER, ch),arg2);
    send_to_char(buf, ch);
    sprintf(buf, "&B[%ssusurros&B] &C%s susurra: &W%s%s\n\r", color_str( AT_WHISPER, victim), PERS(ch, victim), color_str(AT_WHISPER, victim), arg2);
    send_to_char(buf, victim);
}

#define MAX_DROP_TROOPS 20 

//do_droptroops fixed by KeB 10/24/06 
void do_droptroops( CHAR_DATA *ch, char *argument ) 
{ 
  int num,vnum,i; 
  SHIP_DATA *ship; 
  CHAR_DATA *mob; 
  MOB_INDEX_DATA  * pMobIndex; 
  OBJ_DATA        * blaster; 
  OBJ_INDEX_DATA  * pObjIndex; 
  ROOM_INDEX_DATA *room, *rtest; 
  char tmpbuf[MAX_STRING_LENGTH]; 

  if (ch->pcdata->clan == NULL) 
  { 
        send_to_char("You must be in a clan to drop troops.\n\r", ch); 
        return; 
  } 

  if (ch->pcdata->clan->troops < 1) 
  { 
        send_to_char("Your clan has no ground assault troops.\n\r", ch); 
        return; 
  } 

  num = atoi(argument); 
  if (num > ch->pcdata->clan->troops) 
  { 
        send_to_char("Your clan doesn't have that many ground assault troops.\n\r", ch); 
        return; 
  } 

  if ( (ship = ship_from_cockpit(ch->in_room->vnum)) == NULL ) 
  { 
        send_to_char("You must be in the cockpit of the dropship to do this.\n\r", ch); 
        return; 
  } 

  vnum = ship->location; 
  if ((room = get_room_index(vnum)) == NULL) 
  { 
        send_to_char("This ship is not in a room.\n\r", ch); 
        return; 
  } 

  if (num > MAX_DROP_TROOPS) 
  { 
      ch_printf(ch, "You can only drop %d troops at once!\n\r", MAX_DROP_TROOPS ); 
      return; 
  } 
  if (num > ch->pcdata->clan->troops) 
    num = ch->pcdata->clan->troops; 

  if ( ( pMobIndex = get_mob_index( 82 ) ) == NULL ) 
      return; 

  if (room->area == NULL) 
    return; 

  for( i = 1; i <= num; i++ ) 
  { 
    //Can be any room but the hi_room and low_room 
    vnum = number_range(room->area->low_r_vnum + 1, room->area->hi_r_vnum - 1); 

    if ((rtest = get_room_index(vnum)) == NULL) 
      continue; 

    //Now lets make sure the room isn't safe and isn't a player home 
    if( IS_SET( rtest->room_flags, ROOM_SAFE ) || IS_SET(rtest->room_flags, ROOM_PLR_HOME) ) 
    { 
        --i; 
        continue; 
    } 
    mob = create_mobile( pMobIndex ); 
    char_to_room( mob, rtest ); 
    if ( ch->pcdata && ch->pcdata->clan ) 
    sprintf( tmpbuf , "(%s) %s" , ch->pcdata->clan->name  , mob->long_descr ); 
    STRFREE( mob->long_descr ); 
    mob->mob_clan = QUICKLINK(ch->pcdata->clan->name); 
    mob->long_descr = STRALLOC( tmpbuf ); 
    if ( ( pObjIndex = get_obj_index( OBJ_VNUM_BLASTECH_E11 ) ) != NULL ) 
    { 
      blaster = create_object( pObjIndex, mob->top_level ); 
      obj_to_char( blaster, mob ); 
      equip_char( mob, blaster, WEAR_WIELD ); 
    } 
  } 
  sprintf(tmpbuf, "&RYelling and the thunder of feet is heard from the troop hold as %d troops exit the ship and fan out.\n\r", num); 
  ch->pcdata->clan->troops-=num; 
  echo_to_cockpit(AT_RED, ship, tmpbuf); 
} 

   
void do_hale( CHAR_DATA *ch, char *argument )
{
   SHIP_DATA *ship;
   SHIP_DATA *ship2 = ship_from_cockpit( ch->in_room->vnum );
   char buf[MAX_STRING_LENGTH];
   char buf1[MAX_STRING_LENGTH];
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   argument = one_argument( argument, arg1 );
   strcpy(arg2,argument);
   if (arg1[0] == '\0' || arg2[0] == '\0')
   {
   	send_to_char ("Sintaxis: transmitir <nave> <mensaje>\n\r", ch);
   	return;
   }

   ship = get_ship( arg1 );
   send_to_char ("\n\r", ch);
   if (!ship)
   {
     send_to_char ("¡No ves esa nave!\n\r", ch);
     return;
   }
   if (!ship2)
   {
   	send_to_char("¡No estás en la cavina!\n\r", ch);
   	return;
   }

   if (IS_SET(ship->flags, SHIP_SIMULATOR) && !IS_SET(ship2->flags, SHIP_SIMULATOR) )
    {
    	send_to_char("¡No ves esa nave!\n\r", ch);
        return;
    }

   if (!IS_SET(ship->flags, SHIP_SIMULATOR) && IS_SET(ship2->flags, SHIP_SIMULATOR) )
    {
    	send_to_char("¡No ves esa nave!\n\r", ch);
        return;
    }

   sprintf (buf, "&B[&CTransmisión de naves&B] &C%s&B: &w%s", ship2->name, arg2);
   sprintf (buf1, "&B[&CTransmisión de naves&B] &CEnvías&B: &w%s", arg2);
   send_to_char(buf1, ch);
   sprintf (buf1, "&g%s realiza algunos ajustes en el sistema de comunicación de la nave y dice:\n\r&C'&w%s&C'", ch->name, arg2);
   act( AT_GREEN, buf1,  ch, NULL, NULL, TO_NOTVICT );
   echo_to_cockpit(AT_BLUE, ship, buf);
   return;
}

void do_rpconvert( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

 if(IS_NPC(ch)) return;

 if(argument[0] == '\0')
 {
  send_to_char("&w&WSpend your RP points on what? The various options are:\n\r", ch);
  send_to_char("&G+-----------------------------------------------------+\n\r", ch);
  send_to_char("&G| &W1&G | &WBonus Level to any Class  &G| &W             10 RPP &G|&W\n\r", ch);
  send_to_char("&G| &W2&G | &WClone                     &G| &W              4 RPP &G|&W\n\r", ch);
  send_to_char("&G| &W3&G | &W5% to any skill(max 100%) &G| &W              1 RPP &G|&W\n\r", ch);
  send_to_char("&G+-----------------------------------------------------+\n\r\n\r", ch);

  send_to_char("&WFor more information on any bonus, type 'help rpconvert'\n\r", ch);
  send_to_char("&WTo buy your bonus, type 'rpconvert <number of bonus> [extra arguments]'\n\r", ch);
  send_to_char("&WException: For cloning, type 'clone' at the cloning center or 'backup' if you're droid.\n\r", ch);
  return;
 }

  if(atoi(argument) == 1)
  {
    char arg1[MAX_STRING_LENGTH];
    int iClass;
    argument = one_argument( argument, arg1 );

    if(argument[0] == '\0' || !argument)
    {
	send_to_char("&RSyntax: rpconvert 1 <class>\n\r", ch);
	return;
    }

    if(ch->rppoints < 10)
    {
	send_to_char("&RThis bonus costs 10 RP points. You don't have enough.\n\r", ch);
	return;
    }

        for ( iClass = 0; iClass < MAX_ABILITY; iClass++ )
        {
            if ( UPPER(argument[0]) == UPPER(ability_name[iClass][0])
            &&   !str_prefix( argument, ability_name[iClass] ) && str_prefix(argument, "force"))
            {
                ch->bonus[iClass] += 1;
		ch_printf(ch, "&GYour ability in %s has been increased by 1!\n\r", ability_name[iClass] );
		sprintf(buf, "%s increased class %s with rpconvert.", ch->name, ability_name[iClass]);
		log_string(buf);
		ch->rppoints = ch->rppoints - 10;
                break;
            }
        }

	if(iClass == MAX_ABILITY)
	{
	  send_to_char("No such class.\n\r", ch);
	  return;
	}

	return;
  }

  if(atoi(argument) == 2)
  {
    send_to_char("&RJust type 'clone' or 'backup' at a cloning facility!\n\r", ch);
    return;
  }

  if(atoi(argument) == 3)
  {
    char arg1[MAX_STRING_LENGTH];
    int sn;
    argument = one_argument( argument, arg1 );

    if(argument[0] == '\0' || !argument)
    {
	send_to_char("&RSyntax: rpconvert 2 <skill>\n\r", ch);
	return;
    }

    if(ch->rppoints < 1)
    {
	send_to_char("&RThis bonus costs 1 RP point. You don't have enough.\n\r", ch);
	return;
    }

    sn = skill_lookup( argument );

    if(!sn || sn < 1)
    {
	send_to_char("&RNo such skill.\n\r", ch);
	return;
    }

    if(ch->pcdata->learned[sn] < 1)
    {
	send_to_char("&RYou must have at least practiced the skill to increase its percentage!\n\r", ch);
	return;
    }

    if(ch->pcdata->learned[sn] >= 100)
    {
	send_to_char("&RYou already have this skill adepted!\n\r", ch);
	return;
    }

    ch->pcdata->learned[sn] = UMIN(ch->pcdata->learned[sn] + 5, 100);
    ch->rppoints -= 1;
    
    ch_printf( ch, "&GYou have spent 1 RPP to increase %s by 5%!\n\r", skill_table[sn]->name);
    sprintf(buf, "%s increased %s by 5%% with rpconvert.\n\r", ch->name, skill_table[sn]->name);
    log_string(buf);
  
   return;
  }

  do_rpconvert(ch, "");    
}

void do_arm( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char arg[MIL];
    sh_int password, timer;

    argument = one_argument( argument, arg );

    if ( IS_NPC(ch) || !ch->pcdata )
    {
      ch_printf( ch, "No tienes ni idea de cómo hacer eso.\n\r" );
      return;
    }

    obj = get_eq_char( ch, WEAR_HOLD );

    if ( !obj || obj->item_type != ITEM_GRENADE )
    {
       ch_printf( ch, "¡No parece que estés sosteniendo una granada!\n\r" );
       return;
    }

    if(obj->timer > 0 ) 
    { 
        send_to_char("¡Ya está armada!\n\r", ch ); 
        return; 
    } 

    if ( argument[0] == '\0' || arg[0] == '\0' )
    {
        send_to_char( "Uso: armar <tiempo> ###\n\r", ch );
        return;
    }
    if (!is_number(arg) || !is_number(argument) )
    {
        send_to_char( "El tiempo y la contraseña han de ser números\n\rSsintaxis: armar <tiempo> ###\n\r", ch );
        return;
    }
    
    timer = atoi(arg);

    if ( timer < 1 )
    {
        send_to_char( "El tiempo ha de ser superior o igual a uno.\n\r", ch );
        return;
    }

    if ( timer > 10 )
    {
        send_to_char( "El tiempo ha de ser más bajo de 10.\n\r", ch );
        return;
    }

    if ( strlen(argument) != 3 )
    {
        send_to_char( "La contraseña ha de tener 3 dígitos.\n\r", ch );
        return;
    }

    password = atoi(argument);


    obj->timer = timer;
    if(obj->armed_by != NULL)
      STRFREE ( obj->armed_by );
    obj->armed_by = STRALLOC ( ch->name );
    obj->value[5] = password;
    ch_printf( ch, "Armas %s.\n\r", obj->short_descr );
    act( AT_PLAIN, "$n arma $p.", ch, obj, NULL, TO_ROOM );
}

void do_disarmgrenade( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    sh_int password;

    if ( IS_NPC(ch) || !ch->pcdata )
    {
      ch_printf( ch, "No tienes idea de cómo hacer eso.\n\r" );
      return;
    }

    obj = get_eq_char( ch, WEAR_HOLD );

    if ( !obj || obj->item_type != ITEM_GRENADE )
    {
       ch_printf( ch, "¡No parece que estés sosteniendo una granada!\n\r" );
       return;
    }

    if ( argument[0] == '\0' || !argument )
    {
        send_to_char( "Uso: desarmar ###\n\r", ch );
        return;
    }
   
    if (!is_number(argument))
    {
        send_to_char( "Uso: desarmar ###\n\r", ch );
        return;
    }
   
    if(obj->timer <= 0 )
    {
        send_to_char("No ha sido armada todavía.\n\r", ch );
        return;
    }

    if ( strlen(argument) != 3 )
    {
        send_to_char( "Uso: desarmar ###\n\r", ch );
        return;
    }

    password = atoi(argument);
    if ( obj->value[5] != password)
    {
       ch_printf( ch, "&RContraseña incorrecta.\n\r" );
       return;
    }
    obj->timer = 0;
    obj->value[5] = 0;
    ch_printf( ch, "Desarmas %s.\n\r", obj->short_descr );
    act( AT_PLAIN, "$n desarma $p.", ch, obj, NULL, TO_ROOM );
}

void do_invite( CHAR_DATA *ch, char *argument ) 
{
	char arg[MAX_INPUT_LENGTH];
	ROOM_INDEX_DATA *room;
	CHAR_DATA *rch;

	one_argument( argument, arg );
	
	if (IS_NPC(ch))
		return;
	
	if ( arg[0] == '\0' )
	{
		send_to_char( "¿Invitar a quién?\n\r", ch );
		return;
	}
	if ( ch->fighting )
	{
		send_to_char( "Tienes mejores cosas entre manos ahora mismo.\n\r", ch );
		return;
	}
	if (!IS_SET(ch->in_room->room_flags,ROOM_PLR_HOME) )
	{
		if (ch->plr_home && ch->plr_home->vnum != ch->in_room->vnum )
		{
			send_to_char("¡No estás en tu casa!",ch);
			return;
		}
	}
	else
	{
		if (ch->plr_home && ch->plr_home->vnum != ch->in_room->vnum) 
		{
			send_to_char("¡No estás en tu casa!",ch);
		        return;
		}
	}
	
	rch = get_char_world(ch, arg);
	if (!rch)
	{
		send_to_char("¡No está aquí!\n\r",ch);
		return;
	}
	if ( !rch->buzzed || !rch->buzzedfrom )
		return;
	if ( rch->buzzed != ch->in_room->vnum || rch->buzzedfrom != rch->in_room->vnum )
	{
		ch_printf(ch,"¡No quiere!\n\r" );
		return;
	}
	
	room = ch->in_room;
	ch_printf( rch, "¡%s te invita a entrar!\n\r", ch->name );
	ch_printf( ch, "¡Invitas a %s a entrar!\n\r", rch->name );
	
	char_from_room( rch );
	char_to_room( rch , ch->in_room );
	act( AT_ACTION, "¡$N ha sido invitado a la casa de $n!", ch, NULL, rch, TO_NOTVICT);
	return; 
} 

void do_buzz( CHAR_DATA *ch, char *argument ) 
{
	EXIT_DATA *pexit;
	char arg [ MAX_INPUT_LENGTH ];
	char buf [ MAX_INPUT_LENGTH ];
	
	one_argument( argument, arg );
	
	if ( arg[0] == '\0' )
	{
		send_to_char( "¿usar qué intercomunicador?\n\r", ch );
		return;
	}
	if ( ch->fighting )
	{
		send_to_char( "Tienes mejores cosas que hacer ahora mismo.\n\r", ch );
		return;
	}
	if ( ( pexit = get_exit(ch->in_room,get_dir(arg) )) != NULL)
	{
		ROOM_INDEX_DATA *to_room;
		EXIT_DATA *pexit_rev;
		char *keyword;
		
		if ( (to_room = pexit->to_room) != NULL && IS_SET( to_room->room_flags , ROOM_EMPTY_HOME ) )
		{
			send_to_char( "¡Nadie vive allí!\n\r", ch);
			return;
		}
		else if ( (to_room = pexit->to_room) != NULL && !IS_SET(to_room->room_flags, ROOM_PLR_HOME) )
		{
			send_to_char( "¡No hay nadie en esa casa!\n\r", ch);
			return;
		}
		keyword = capitalize( dir_name[pexit->vdir] );
		act( AT_ACTION, "Usas el intercomunicador de $d.", ch, NULL, keyword, TO_CHAR );
		act( AT_ACTION, "$n usa el intercomunicador de $d.", ch, NULL, keyword, TO_ROOM );
		if ( (to_room = pexit->to_room) != NULL && (pexit_rev = pexit->rexit) != NULL
			&& pexit_rev->to_room == ch->in_room )
		{
			sprintf( buf, "¡%s usa el intercomunicador desde fuera!\n\r", ch->name );
			echo_to_room( AT_ACTION , to_room , buf );
			ch->buzzedfrom = ch->in_room->vnum;
			ch->buzzed = pexit->to_room->vnum;
		}
	}
	else
	{
		send_to_char("¡No hay una casa allí!\n\r",ch);
	}
	return;
}

void do_debitorder( CHAR_DATA *ch, char *argument) 
{
        OBJ_DATA *obj;
        OBJ_INDEX_DATA *objindex;
	if( !IS_SET(ch->in_room->room_flags, ROOM_BANK) )
        {
		send_to_char("¡Debes estar en un banco para pedir una tarjeta de débito!\n\r", ch);
		return;
	}
 
        if ( (!argument) || (argument[0] == '\0') )
        {
                send_to_char( "sintaxis: pedirtarjeta <si/no>\n\r", ch);
		send_to_char( "¡Tienes que pagar 1000 créditos para disponer de una tarjeta de crédito!\n\r",ch);
                return;
        }
        
         
	if ( !strcmp(argument,"no") )
	{
		send_to_char("¡Está bien, por favor no dejes que eso te impida hacer negocios con nuestro banco!\n\r", ch);
		return;
	}
       
	else if( !strcmp(argument, "si") )
	{
	        if( (ch->pcdata->bank - 1000) < 0)
		{
			send_to_char("¡No tienes suficientes créditos en tu cuenta!\n\r", ch);
			return;
		}
                if( (ch->pcdata->bank - 1000) < 1000)
                {
                        send_to_char("¡El banco cree que no sería razonable que compres una tarjeta de débito en este momento!\n\r", ch);
                        return;
                }
                        objindex = get_obj_index( VNUM_DEBIT_CARD );
                        obj = create_object( objindex, 0 );
                        obj_to_char( obj, ch );
			ch->pcdata->bank -= 1000;
                        send_to_char("¡Pagas 1000 créditos para conseguir tu tarjeta!\n\r", ch );
        }
 return;
}
