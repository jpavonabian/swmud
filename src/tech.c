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

/* This is for the new technician class. The technicians are going to be totally in control of ship related
   abilities. These include, creation of, and use of ship modules. Ship Maintanance, Custom Ship Design, and
   Ship Sabotage. Taking some of the abilities of engineers and getting some more.
*/

/* Modules are how ships are upgraded. Ships can have only so many modules, Depending on the type of ship. */
/* The effectiveness of the modules can vary depending on the level of the technitian's makemodule skill */
void do_makemodule( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int affecttype, affectammount;
    char name[MAX_STRING_LENGTH];
    int level, chance;
    bool checklens, checkbat, checksuper, checkcircuit, checktool; 
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
            
    argument = one_argument( argument, arg );


    switch( ch->substate )
    { 
    	default:
    	        
     		if(str_cmp(arg, "casco") && str_cmp( arg, "esclavo") && str_cmp( arg, "tractor") && str_cmp(arg, "torpedo") && str_cmp(arg, "cohete") && str_cmp(arg, "misil") && str_cmp(arg, "primario") && str_cmp(arg, "secundario") && str_cmp(arg, "escudo") && str_cmp(arg, "velocidad") && str_cmp(arg, "hiperespacio") && str_cmp(arg, "energia") && str_cmp(arg, "maniobrabilidad") && str_cmp(arg, "contramedida") && str_cmp(arg, "alarma")){
    		  send_to_char("Los módulos pueden ser de los siguientes tipos:\n\rPrimario, Secundario, Misil, cohete, Torpedo, Casco, Escudo, velocidad, Hiperespacio, energia, maniobrabilidad, esclavo, Tractor, Contramedida, y Alarma.\n\r", ch);
    		  return;
    		}
                checklens = FALSE;
                checkbat = FALSE;
                checksuper = FALSE;
                checkcircuit = FALSE;
                checktool = FALSE;
                if ( !IS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
                {
                   send_to_char( "&RDebes estar en una fábrica o taller para hacer esto.\n\r", ch);
                   return;
                }
                
                for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
                {
                  if (obj->item_type == ITEM_LENS)
                    checklens = TRUE;
                  if (obj->item_type == ITEM_BATTERY)
          	    checkbat = TRUE;
                  if (obj->item_type == ITEM_SUPERCONDUCTOR)
                    checksuper = TRUE;
                  if (obj->item_type == ITEM_CIRCUIT)
                    checkcircuit = TRUE;
                  if (obj->item_type == ITEM_TOOLKIT)
                    checktool = TRUE;
          	    
                }
                
                if ( !checklens )
                {
                   send_to_char( "&RNecesitas una lente.\n\r", ch);
                   return;
                }
 
                if ( !checkbat )
                {
                   send_to_char( "&Rnecesitas una batería.\n\r", ch);
                   return;
                }

                if ( !checksuper )
                {
                   send_to_char( "&RNecesitas un superconductor.\n\r", ch);
                   return;
                }

                if ( !checkcircuit )
                {
                   send_to_char( "&RNecesitas un circuito.\n\r", ch);
                   return;
                }

                if ( !checktool )
                {
                   send_to_char( "&Rnecesitas una caja de herramientas.\n\r", ch);
                   return;
                }
                
                
                

    	        chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_makemodule]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GComienzas el largo proceso de crear un módulo.\n\r", ch);
    		   act( AT_PLAIN, "$n coge sus herramientas y comienza a trabajar.", ch,
		        NULL, argument , TO_ROOM );
		   add_timer ( ch , TIMER_DO_FUN , 10 , do_makemodule , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   return;
	        }
	        send_to_char("&RNo tienes claro como juntar las piezas.\n\r",ch);
	        learn_from_failure( ch, gsn_makemodule );
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
    	        send_to_char("&RTe distraes y no consigues terminar tu trabajo.\n\r", ch);
    	        return;
    }
    
    ch->substate = SUB_NONE;
    
    level = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makemodule]);



    if ( ( pObjIndex = get_obj_index( MODULE_VNUM ) ) == NULL )
    {
         send_to_char( "&Rel objeto que tratas de crear no está en la base de datos.\n\rNotifícalo urgentemente a la administración.\n\r", ch );
         return;
    }    
    
    checklens = FALSE;
    checkbat = FALSE;
    checksuper = FALSE;
    checkcircuit = FALSE;
    checktool = FALSE;
    
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
       if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;
       if (obj->item_type == ITEM_LENS && checklens == FALSE)
       {
          checklens = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
       }
       if (obj->item_type == ITEM_BATTERY && checkbat == FALSE)
       {
          checkbat = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
       }
       if (obj->item_type == ITEM_SUPERCONDUCTOR && checksuper == FALSE)
       {
          checksuper = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
       }
       if (obj->item_type == ITEM_CIRCUIT && checkcircuit == FALSE)
       {
          checkcircuit = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
       }
       
    }                            
    
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makemodule]) ;            
                
    if ( number_percent( ) > chance*2  || ( !checklens ) || ( !checktool ) || ( !checkbat ) || ( !checksuper ) || ( !checkcircuit ) )
    {
       send_to_char( "&RSostienes tu recién creado módulo.\n\r", ch);
       send_to_char( "&R¡El módlo comienza a soltar chispas!\n\r", ch);
       send_to_char( "&R¡El módulo se desintegra cuando lo dejas caer al suelo!\n\r", ch);
       learn_from_failure( ch, gsn_makemodule );
       return;
    }

    if(!str_cmp(arg, "primario")){
      affecttype = AFFECT_PRIMARY;
      affectammount = 1;
      strcpy(name, "Un módulo de arma primaria");
    }

    if(!str_cmp(arg, "secundario")){
      affecttype = AFFECT_SECONDARY;
      affectammount = 1;
      strcpy(name, "Un módulo de arma secundaria");
    }

    if(!str_cmp(arg, "esclavo"))
    {
      affecttype = AFFECT_SLAVE;
      affectammount = (level / 4);
      strcpy(name, "Un módulo esclavo");
    }

    if(!str_cmp(arg, "tractor"))
    {
      affecttype = AFFECT_TRACTOR;
      affectammount = 1;
      strcpy(name, "Un módulo de rayo tractor");
    }

    if(!str_cmp(arg, "misil")){
      affecttype = AFFECT_MISSILE;
      affectammount = (level / 20);
      strcpy(name, "un módulo de misil");
    }

    if(!str_cmp(arg, "cohete")){
      affecttype = AFFECT_ROCKET;
      affectammount = (level / 20);
      strcpy(name, "Un módulo de cohete");
    }

    if(!str_cmp(arg, "torpedo")){
      affecttype = AFFECT_TORPEDO;
      affectammount = (level / 20);
      strcpy(name, "Un módulo de torpedo");
    }

    if(!str_cmp(arg, "casco")){
      affecttype = AFFECT_HULL;
      affectammount = (level / 2);
      strcpy(name, "Un módulo de casco");
    }

    if(!str_cmp(arg, "escudo")){
      affecttype = AFFECT_SHIELD;
      affectammount = (level/5);
      strcpy(name, "Un módulo de escudo");
    }
    if(!str_cmp(arg, "velocidad")){
      affecttype = AFFECT_SPEED;
      affectammount = (level / 10);
      strcpy(name, "Un módulo de velocidad");
    }
    if(!str_cmp(arg, "hiperespacio")){
      affecttype = AFFECT_HYPER;
      affectammount = 1;
      strcpy(name, "Un módulo de hipervelocidad");
    }
    if(!str_cmp(arg, "energia")){
      affecttype = AFFECT_ENERGY;
      affectammount = (level * 5);
      strcpy(name, "Un módulo de energía");
    }
    if(!str_cmp(arg, "maniobrabilidad")){
      affecttype = AFFECT_MANUEVER;
      affectammount = (level / 10);
      strcpy(name, "Un módulo de maniobravilidad");
    }
    if(!str_cmp(arg, "alarma"))
    {
      affecttype = AFFECT_ALARM;
      affectammount = 1;
      strcpy(name, "Un módulo de alarma");
    }
    if(!str_cmp(arg, "contramedida"))
    {
      affecttype = AFFECT_CHAFF;
      affectammount = URANGE(1,(level / 33), 3);
      strcpy(name, "Un módulo de contramedida");
    }


    
    obj = create_object( pObjIndex, level );
    obj->item_type = ITEM_MODULE;
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    obj->level = level;
    STRFREE( obj->name );
    obj->name = STRALLOC( name );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( name );        
    STRFREE( obj->description );
    strcat( name, " está aquí." );
    obj->description = STRALLOC( name );
    
    obj->value[0] = affecttype;
    obj->value[1] = affectammount;
    obj->value[2] = 0;      
    obj->cost = (level * affecttype * affectammount);
                                                                    
    obj = obj_to_char( obj, ch );
                                                            
    send_to_char( "&GTerminas tu trabajo y admiras tu nuevo módulo.&w\n\r", ch);
    act( AT_PLAIN, "$n finaliza de crear su módulo.", ch,
         NULL, argument , TO_ROOM );
    
    {
         long xpgain;
         
         xpgain = ((ch->skill_level[TECHNICIAN_ABILITY]+1) * 200);
         gain_exp(ch, xpgain, TECHNICIAN_ABILITY);
         ch_printf( ch , "Recibes %d puntos de experiencia en técnico.", xpgain );
    }
        learn_from_success( ch, gsn_makemodule );
}


void do_showmodules( CHAR_DATA *ch, char *argument ){
  SHIP_DATA *ship;
  MODULE_DATA *mod;
  char buf[MAX_STRING_LENGTH];
  char str[MAX_STRING_LENGTH];
  int i;
  long xpgain;
  int chance;
  
  chance = IS_NPC(ch) ? ch->top_level
     : (int) (ch->pcdata->learned[gsn_showmodules]);
    
  if((ship = ship_from_engine(ch->in_room->vnum)) == NULL){
  	send_to_char("Debes estar en la sala de motores de una nave.\n\r", ch);
  	return;
  }
  
  if ( number_percent( ) > chance ){
  	send_to_char("&RNo consigues encontrar el panel de control de módulos.\n\r", ch);
  	learn_from_failure(ch, gsn_showmodules);
  	return;
  }  
  send_to_char("&z+--------------------------------------+\n\r", ch);
  send_to_char("&z| &RNum  Tipo                   Cantidad &z|\n\r", ch);
  send_to_char("&z| &r---  ----                   -------- &z|\n\r", ch);
  i=0;
  for(mod=ship->first_module;mod;mod=mod->next){
      i++;
      if(mod->affect == AFFECT_PRIMARY)
        strcpy(str, "Arma primaria");
      if(mod->affect == AFFECT_SECONDARY)
        strcpy(str, "Arma secundaria");
      if(mod->affect == AFFECT_MISSILE)
        strcpy(str, "Misil");
      if(mod->affect == AFFECT_ROCKET)
        strcpy(str, "Cohete");
      if(mod->affect == AFFECT_TORPEDO)
        strcpy(str, "Torpedo");
      if(mod->affect == AFFECT_HULL)
        strcpy(str, "Casco");
      if(mod->affect == AFFECT_SHIELD)
        strcpy(str, "Escudos");
      if(mod->affect == AFFECT_SPEED)
        strcpy(str, "Velocidad");
      if(mod->affect == AFFECT_HYPER)
        strcpy(str, "Hiperespacio");
      if(mod->affect == AFFECT_ENERGY)
        strcpy(str, "Energía");
      if(mod->affect == AFFECT_MANUEVER)
        strcpy(str, "Maniobravilidad");
      if(mod->affect == AFFECT_ALARM)
        strcpy(str, "Alarma");
      if(mod->affect == AFFECT_CHAFF)
        strcpy(str, "Contramedida");
      if(mod->affect == AFFECT_SLAVE)
        strcpy(str, "Esclavo");
      if(mod->affect == AFFECT_TRACTOR)
        strcpy(str, "TRACTOR");     
 
      sprintf(buf, "&z| &P%2d&p)  &G&W%-22.22s %-8.8d &z|\n\r", i, str, mod->ammount);
      send_to_char(buf, ch);
  }
  send_to_char("&z+--------------------------------------+\n\r", ch);
  xpgain = UMAX(100,i * 100);
  gain_exp(ch, xpgain, TECHNICIAN_ABILITY);
  ch_printf( ch , " Recibes %d puntos de experiencia en técnico.\n\r", xpgain );
  learn_from_success(ch, gsn_showmodules);
}

void do_removemodule( CHAR_DATA *ch, char *argument )
{
  SHIP_DATA *ship;
  bool checktool;
  OBJ_DATA *obj;
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_INPUT_LENGTH];
  MODULE_DATA *mod;
  int  chance;
  int num,i;

  strcpy( arg , argument );
  checktool = FALSE;
  switch( ch->substate )
   {
    default:
    if ( (ship = ship_from_engine(ch->in_room->vnum)) != NULL )
      {
      	ship = ship_from_engine(ch->in_room->vnum);
      }

     if (!ship)
      {
    	send_to_char("necesitas estar en la sala de motores de la nave.\n\r", ch);
    	return;
      }

     if(arg[0] == '\0' || atoi(arg) == 0)
      {
	send_to_char("¿Desinstalar qué módulo? Usa los números proporcionados por vermodulos.\n\r", ch);
	return;
      }

     for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
      {
      	if (obj->item_type == ITEM_TOOLKIT)
      	  checktool = TRUE;
      }

     if ( checktool == FALSE )
      {
      	send_to_char("Necesitas una caja de herramientas.\n\r", ch);
      	return;
      }
     i=0;
     num = atoi(argument);
     for(mod=ship->first_module;mod;mod=mod->next)
       i++;
     
     if (i < num || i == 0)
     {
       send_to_char("No such module installed.\n\r", ch);
       return;
     }
           
     chance = IS_NPC(ch) ? ch->top_level
        : (int) (ch->pcdata->learned[gsn_removemodule]);
     if ( number_percent( ) < chance )
      {
      	strcpy(arg, argument);
  	ch->dest_buf   = str_dup(arg);
       	send_to_char( "&Gcomienzas el largo proceso de desinstalación de un módulo.\n\r", ch);
    	sprintf(buf, "$n coge su caja de herramientas y comienza a trabajar.\n\r");
     	act( AT_PLAIN, buf, ch, NULL, argument , TO_ROOM );
     	
     	add_timer ( ch , TIMER_DO_FUN , 5 , do_removemodule , 1 );
    	return;
      }

      send_to_char("&RNo consigues nada.\n\r",ch);
      learn_from_failure( ch, gsn_removemodule );
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
         if (obj->item_type == ITEM_TOOLKIT)
           checktool = TRUE;
           
      }
    chance = IS_NPC(ch) ? ch->top_level
               : (int) (ch->pcdata->learned[gsn_removemodule]);

    ship = ship_from_engine(ch->in_room->vnum);
    if (!ship){
      send_to_char("Error: algo ha ido mal. ¡Notifícalo urgentemente a la administración!\n\r", ch);
      return;
    }

    if ( number_percent( ) > chance*2 )
     {
       send_to_char("&RDesinstalas el módulo y todo parece ir bien...\n\r", ch);
       send_to_char("&RTe das cuenta de que has desinstalado el motor de la nave y lo vuelves a instalar corriendo. OOPS!\n\r", ch);
       learn_from_failure( ch, gsn_removemodule );
       return;
     }
      num = atoi(arg);
      i=0;
      for(mod=ship->first_module;mod;mod=mod->next)
      {
        i++;
        if (i == num)
	{
    	 if(mod->affect == AFFECT_PRIMARY)
          ship->primaryCount-=mod->ammount;
         if(mod->affect == AFFECT_SECONDARY)
          ship->secondaryCount-=mod->ammount;
         if(mod->affect == AFFECT_MISSILE)
           {
              ship->maxmissiles-=mod->ammount;
              ship->missiles=ship->maxmissiles;
           }
         if(mod->affect == AFFECT_ROCKET)
           {
              ship->maxrockets-=mod->ammount;
              ship->rockets=ship->maxrockets;
           }
         if(mod->affect == AFFECT_TORPEDO)
           {
              ship->maxtorpedos-=mod->ammount;
              ship->torpedos=ship->maxtorpedos;
           }
      	 if(mod->affect == AFFECT_HULL)
          ship->maxhull-=mod->ammount;
      	 if(mod->affect == AFFECT_SHIELD)
          ship->maxshield-=mod->ammount;
      	 if(mod->affect == AFFECT_SPEED)
          ship->realspeed-=mod->ammount;
      	 if(mod->affect == AFFECT_HYPER)
          ship->hyperspeed+=mod->ammount;
      	 if(mod->affect == AFFECT_ENERGY)
          ship->maxenergy-=mod->ammount;
      	 if(mod->affect == AFFECT_MANUEVER)
          ship->manuever-=mod->ammount;
         if(mod->affect == AFFECT_ALARM)
          ship->alarm-=mod->ammount;
      	 if(mod->affect == AFFECT_CHAFF)
          ship->maxchaff-=mod->ammount;
         if(mod->affect == AFFECT_SLAVE)
          ship->slave-=mod->ammount;     
         if(mod->affect == AFFECT_TRACTOR)
          ship->tractorbeam-=mod->ammount;

     UNLINK(mod, ship->first_module, ship->last_module, next, prev);
          DISPOSE(mod);
          save_ship(ship);
          break;
        }        
      }
      
      send_to_char("Desinstalas el módulo con éxito.\n\r", ch);

     {
      long xpgain;      
      xpgain = ((ch->skill_level[TECHNICIAN_ABILITY]+1) * 300);
      gain_exp(ch, xpgain, TECHNICIAN_ABILITY);
      ch_printf( ch , " Recibes %d puntos de experiencia en técnico.\n\r", xpgain );
      learn_from_success( ch, gsn_removemodule ) ;
     }
      return;
}

void do_shipmaintenance(CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char log_buf[MAX_INPUT_LENGTH];
    int chance, change, bombs=0;
    long xp;
    SHIP_DATA *ship;
    OBJ_DATA *obj;
    int oldbombs;
    
    strcpy( arg, argument );    
    
    if( (ch->pcdata->learned[gsn_shipmaintenance]) <= 0)
    {
	send_to_char("Huh?\n\r", ch);
	return;
    }

    switch( ch->substate )
    { 
    	default:
    	        if (  (ship = ship_from_engine(ch->in_room->vnum))  == NULL )
    	        {
    	            send_to_char("&R¡Necesitas estar en la sala de motores de una nave para hacer esto!\n\r",ch);
    	            return;
    	        }
                
                chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_shipmaintenance]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GComienzas a realizar acciones básicas de mantenimiento en la nave...\n\r", ch);
    		   act( AT_PLAIN, "$n comienza a realizar acciones básicas de mantenimiento en la nave.", ch,
		        NULL, argument , TO_ROOM );
    		     add_timer ( ch , TIMER_DO_FUN , 15 , do_shipmaintenance , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   return;
	        }
	        send_to_char("&RNo consigues hacer nada útil.\n\r",ch);
		learn_from_failure(ch,gsn_shipmaintenance);
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
    	        send_to_char("&RTe distraes y decides finalizar el mantenimiento de la nave.\n\r", ch);
    		return;
    }
    
    ch->substate = SUB_NONE;
    
    if ( (ship = ship_from_engine(ch->in_room->vnum)) == NULL )
    {  
       return;
    }
    
        change = URANGE( 0 , 
                         number_range( (int) ( ch->pcdata->learned[gsn_shipmaintenance] / 2 ) , (int) (ch->pcdata->learned[gsn_shipmaintenance]) ),
                         ( ship->maxhull - ship->hull ) );
        ship->hull += change;
	ship->chaff = ship->maxchaff;
	ship->missiles = ship->maxmissiles;
	ship->torpedos = ship->maxtorpedos;
	ship->rockets = ship->maxrockets;

	oldbombs = ship->bombs;


	for(obj = ch->last_carrying; obj; obj = obj->prev_content )
	{
	 if(ship->maxbombs - bombs == 0)
	  break;

	if(obj->item_type == ITEM_SHIPBOMB)
	{

	if(ship->bombs > 0 && ship->lowbombstr > 0 && ship->hibombstr > 0)
	{
	ship->lowbombstr = (((ship->lowbombstr*ship->bombs)+obj->value[0])/(ship->bombs + 1));
	    sprintf(log_buf, "Ships lowbombstr is %d", ship->lowbombstr);
	    log_string(log_buf);

	ship->hibombstr = ((ship->hibombstr * ship->bombs)+(int)obj->value[1])/(ship->bombs + 1);
	    sprintf(log_buf, "Ships hibombstr is %d", ship->hibombstr);
	    log_string(log_buf);
	  }
	  else
	  {
	    ship->lowbombstr = obj->value[0];
	    ship->hibombstr = obj->value[1];
	  }

	    if(obj->count > (ship->maxbombs - bombs))
	    {
	      obj->count -= (ship->maxbombs - bombs);
	      ship->bombs += (ship->maxbombs - bombs);
	      break;
	    }
	    else
	    {
	      ship->bombs += obj->count;
	      obj_from_char(obj);
	      extract_obj(obj);
	    }
	 }//if check
 	}// for loop	

        ch_printf( ch, "&GReparación completa... Casco incrementado en %d puntos, Armas y contramedidas de la nave restauradas.\n\r", change );
	if(ship->bombs > oldbombs)
	   ch_printf( ch, "&G%d bombas cargadas en la nave desde el inventario.\n\r", ship->bombs - oldbombs);

    act( AT_PLAIN, "$n finaliza la reparación del casco y restaura las armas de la nave.", ch,
         NULL, argument , TO_ROOM );

    xp = get_ship_value( ship )/100;
    ch_printf( ch, "&W¡Recibes %ld experiencia en técnico!\n\r", (get_ship_value(ship)/100) );
    gain_exp( ch , xp , TECHNICIAN_ABILITY );    	
    
    learn_from_success( ch, gsn_shipmaintenance );
    return;
    

}

void do_scanbugs( CHAR_DATA *ch, char *argument )
{
 CHAR_DATA *victim;
 
 char arg[MAX_STRING_LENGTH];
 
 int chance;
 
 BUG_DATA *bugs;
 int i;
 
 argument = one_argument( argument, arg ); 

switch( ch->substate )
{
  default:

   if( arg[0] == '\0')
   {
     send_to_char("sintaxis: comprobarbichos <persona>\n\r", ch);
     return;
   }


   victim = get_char_room(ch, arg);

   if(!victim)
   {
     send_to_char("No está aquí.\n\r", ch);
     return;
   }

   if( IS_NPC(victim))
   {
     send_to_char("Solo puedes hacer esto con jugadores.\n\r", ch);
     return;
   }
   
   chance = IS_NPC(ch) ? ch->top_level : (int)(ch->pcdata->learned[gsn_scanbugs]);

   if(number_percent() - 20 < chance)
   {
    send_to_char("&w&GEscaneando...\n\r", ch);
    act( AT_PLAIN, "$n coge su escáner y comienza a escanear a $N.", ch, NULL, victim, TO_NOTVICT );
    act( AT_PLAIN, "$n te escanea en busca de bichos.", ch, NULL, victim, TO_VICT);
    add_timer ( ch , TIMER_DO_FUN , 10 , do_scanbugs , 1 );
    ch->dest_buf = str_dup(arg);
    return;
   }
   send_to_char("Pulsas un botón al azar en el escáner.\n\r", ch);
   learn_from_failure(ch, gsn_scanbugs);
   return;

  case 1:
 	if ( !ch->dest_buf )
       		return;
	 strcpy(arg, ch->dest_buf);
 	 DISPOSE( ch->dest_buf);
	 ch->dest_buf = NULL;
	 break;

  case SUB_TIMER_DO_ABORT:
	DISPOSE( ch->dest_buf );
	 ch->dest_buf = NULL;
	ch->substate = SUB_NONE;                                       
  	send_to_char("&RTe distraes y no terminas el escáner.\n\r", ch);
  	return;

}

    ch->substate = SUB_NONE;

 chance = IS_NPC(ch) ? ch->top_level : (int)(ch->pcdata->learned[gsn_scanbugs]);

  if ( number_percent( ) > chance*2 + 20)
   {
	send_to_char("&w&RTe peleas con el escáner pero no logras manejarlo.\n\r", ch);
	learn_from_failure(ch, gsn_scanbugs);
	return;
   }

   victim = get_char_room(ch, arg);

   if(!victim)
   {
    send_to_char("¡Tu víctima se ha ido antes de que termines el escáner!\n\r", ch);
    return;
   }

   i=0;
   for(bugs=victim->first_bug;bugs;bugs=bugs->next_in_bug)
     i++;

   if(i >= 1)
	ch_printf(ch, "&w&GEscáner completado. %d bichos encontrados.\n\r", i);
   else
	send_to_char("&w&GEscáner completado. No tiene bichos aparentemente.\n\r", ch);

   learn_from_success(ch, gsn_scanbugs);

    {
         long xpgain;

         xpgain = (ch->experience[TECHNICIAN_ABILITY]/30);
         gain_exp(ch, xpgain, TECHNICIAN_ABILITY);
         ch_printf( ch , "Recibes %d puntos de experiencia en técnico.", xpgain );
    } 

return;
}

void do_removebug(CHAR_DATA *ch, char *argument)
{
 CHAR_DATA *victim;
 char arg[MAX_STRING_LENGTH];
 int chance;
 BUG_DATA *bugs;

 argument = one_argument( argument, arg ); 

switch( ch->substate )
{
  default:

   if( arg[0] == '\0')
   {
     send_to_char("Sintaxis: quitarbicho <jugador>\n\r", ch);
     return;
   }


   victim = get_char_room(ch, arg);

   if(!victim)
   {
     send_to_char("No está aquí.\n\r", ch);
     return;
   }

   if( IS_NPC(victim))
   {
     send_to_char("Solo puedes hacer eso a jugadores.\n\r", ch);
     return;
   }

   chance = IS_NPC(ch) ? ch->top_level : (int)(ch->pcdata->learned[gsn_scanbugs]);

   if(number_percent() < chance + 20)
   {
    send_to_char("&w&GComienzas la búsqueda de bichos en tu objetivo.\n\r", ch);
    act( AT_PLAIN, "$n toma su caja de herramientas y comienza a quitar un bicho de $N.", ch, NULL, victim, TO_NOTVICT );
    act( AT_PLAIN, "$n toma su caja de herramientas y comienza a quitarte un bicho.", ch, NULL, victim, TO_VICT);
    add_timer ( ch , TIMER_DO_FUN , 1 , do_removebug , 1 );
    ch->dest_buf = str_dup(arg);
    return;
   }
   send_to_char("Observas un extraño bicho, pero no estás seguro de como quitarlo.\n\r", ch);
   learn_from_failure(ch, gsn_removebug);
   return;

  case 1:
 	if ( !ch->dest_buf )
       		return;
	 strcpy(arg, ch->dest_buf);
 	 DISPOSE( ch->dest_buf);
	 ch->dest_buf = NULL;
	 break;

  case SUB_TIMER_DO_ABORT:
	DISPOSE( ch->dest_buf );
	 ch->dest_buf = NULL;
	ch->substate = SUB_NONE;                                       
  	send_to_char("&REres interrumpido, pierdes la concentración y fallas al intentar remover el bicho.\n\r", ch);
  	return;

}

    ch->substate = SUB_NONE;

 chance = IS_NPC(ch) ? ch->top_level : (int)(ch->pcdata->learned[gsn_scanbugs]);


   victim = get_char_room(ch, arg);

   if(!victim)
   {
    send_to_char("¡Tu objetivo se ha ido!\n\r", ch);
    return;
   }

  if ( number_percent( ) > chance*2 || !victim->first_bug)
   {
	send_to_char("&w&REstás muy concentrado intentando remover el bicho de tu objetivo, pero un leve pitido rompe tu concentración...\n\r", ch);
	send_to_char("&w&RAl parecer, has fallado.\n\r", ch);
	learn_from_failure(ch, gsn_scanbugs);
	return;
   }

   if(victim->first_bug)
      bugs = victim->first_bug;
  
   UNLINK(bugs, victim->first_bug, victim->last_bug, next_in_bug, prev_in_bug);
   STRFREE(bugs->name);
   DISPOSE(bugs);
   send_to_char("&w&GCon un satisfactorio *click*, logras remover el bicho y lo destruyes de un martillazo.\n\r", ch);
   learn_from_success( ch, gsn_removebug);
    {
         long xpgain;

         xpgain = (ch->experience[TECHNICIAN_ABILITY]/25);
         gain_exp(ch, xpgain, TECHNICIAN_ABILITY);
         ch_printf( ch , "Recibes %d puntos de experiencia en técnico.", xpgain );
    } 

return;
}

void do_makejetpack( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int level, chance, strength;
    bool checktool, checkbatt, checkchem, checkcirc, checkmetal;
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;

    strcpy( arg , argument );

    switch( ch->substate )
    {
    	default:
    	        if ( arg[0] == '\0' )
                {
                  send_to_char( "&RUsage: Makejetpack <Name>\n\r&w", ch);
                  return;
                }

                checktool  = FALSE;
                checkbatt  = FALSE;
                checkchem  = FALSE;
                checkcirc  = FALSE;
                checkmetal = FALSE;

                if ( !IS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
                {
                   send_to_char( "&RYou need to be in a factory or workshop to do that.\n\r", ch);
                   return;
                }

                for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
                {
                  if (obj->item_type == ITEM_TOOLKIT)
                    checktool = TRUE;
                  if (obj->item_type == ITEM_BATTERY)
                    checkbatt = TRUE;
                  if (obj->item_type == ITEM_CIRCUIT)
                    checkcirc = TRUE;
                  if (obj->item_type == ITEM_CHEMICAL)
                    checkchem = TRUE;
                  if (obj->item_type == ITEM_RARE_METAL)
                    checkmetal = TRUE;
                }

                if ( !checktool )
                {
                   send_to_char( "&RYou need toolkit to make the Jetpack\n\r", ch);
                   return;
                }

                if ( !checkmetal )
                {
                   send_to_char( "&RYou need a piece metal to craft the Jetpack.\n\r", ch);
                   return;
                }

                if ( !checkbatt )
                {
                   send_to_char( "&RYou need a battery for the mechanism to work.\n\r", ch);
                   return;
                }

                if ( !checkcirc )
                {
                   send_to_char( "&RYou need a small circuit.\n\r", ch);
                   return;
                }

                if ( !checkchem )
                {
                   send_to_char( "&RSome chemicals for the combustion.\n\r", ch);
                   return;
                }

                chance = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makejetpack]);
                if ( number_percent( ) < chance )
                {
                   send_to_char( "&GYou begin the long process of crafting a Jetpack\n\r", ch);
                   act( AT_PLAIN, "$n takes $s tools and begins to work on something.", ch, NULL, argument , TO_ROOM );
                   add_timer ( ch , TIMER_DO_FUN , 15 , do_makejetpack , 1 );
                   ch->dest_buf   = str_dup(arg);
                   return;
                }
                send_to_char("&RYou can't figure out how to fit the parts together.\n\r",ch);
                learn_from_failure( ch, gsn_makejetpack );
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
                send_to_char("&RYou are interupted and fail to finish your work.\n\r", ch);
                return;
    }

    ch->substate = SUB_NONE;

    level = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makejetpack]);
    vnum = 88;

    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
         send_to_char( "&RThe item you are trying to create is missing from the database.\n\rPlease inform the administration of this error.\n\r", ch );
         return;
    }

    checktool = FALSE;
    checkmetal = FALSE;
    checkbatt = FALSE;
    checkchem = FALSE;
    checkcirc = FALSE;

    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
    {
       if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;
       if (obj->item_type == ITEM_BATTERY && checkbatt == FALSE)
       {
          strength = obj->value[0];
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkbatt = TRUE;
       }
       if (obj->item_type == ITEM_CHEMICAL)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkchem = TRUE;
       }
       if (obj->item_type == ITEM_CIRCUIT && checkcirc == FALSE)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkcirc = TRUE;
       }
       if (obj->item_type == ITEM_RARE_METAL && checkmetal == FALSE)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkmetal = TRUE;
       }
    }

    chance = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makejetpack]) ;

    if ( number_percent( ) > chance*2  || ( !checktool ) || ( !checkmetal )
         || ( !checkbatt ) || ( !checkchem ) || ( !checkcirc) )
    {
       send_to_char( "You hit the 'on' switch and watch the Jetpack explode into pieces.", ch);
       learn_from_failure( ch, gsn_makejetpack );
       return;
    }

    obj = create_object( pObjIndex, level );

    obj->item_type = ITEM_ARMOR;
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    SET_BIT( obj->wear_flags, ITEM_WEAR_BACK );
    obj->level = level;
    obj->weight = 1;
    STRFREE( obj->name );
    strcpy( buf , arg );
    obj->name = STRALLOC( buf );
    strcpy( buf, arg );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );
    STRFREE( obj->description );
    strcat( buf, " was left behind here." );
    obj->description = STRALLOC( buf );

    obj->value[0] = 0;
    obj->value[0] = 0;
    obj->value[0] = 0;
    obj->cost = 5000;

    obj = obj_to_char( obj, ch );

    send_to_char( "&GYou finish your work and hold up your newly created Jetpack.&w\n\r", ch);
    act( AT_PLAIN, "$n finishes making $s new Jetpack.", ch,
         NULL, argument , TO_ROOM );

    {
         long xpgain;

         xpgain = UMIN( obj->cost*100 ,
            ( exp_level(ch->skill_level[ENGINEERING_ABILITY]+1) -
              exp_level(ch->skill_level[ENGINEERING_ABILITY]) ) );
         gain_exp(ch, xpgain, ENGINEERING_ABILITY);
         ch_printf( ch , "You gain %d engineering experience.", xpgain );
    }
        learn_from_success( ch, gsn_makejetpack );
}
