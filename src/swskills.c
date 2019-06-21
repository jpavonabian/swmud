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

#include <math.h> 
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

void    add_reinforcements  args( ( CHAR_DATA *ch ) );
ch_ret  one_hit             args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
int     xp_compute                ( CHAR_DATA *ch , CHAR_DATA *victim );
ROOM_INDEX_DATA *generate_exit( ROOM_INDEX_DATA *in_room, EXIT_DATA **pexit );
int ris_save( CHAR_DATA *ch, int chance, int ris );
void explode_emissile  args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *proom, int mindam, int maxdam, bool incendiary) );
CHAR_DATA *get_char_room_mp( CHAR_DATA *ch, char *argument );

/* from shops.c */
CHAR_DATA * find_keeper  args( ( CHAR_DATA *ch ) );

extern int      top_affect;

const	char *	sector_name	[SECT_MAX]	=
{
    "inside", "city", "field", "forest", "hills", "mountain", "water swim", "water noswim", 
    "underwater", "air", "desert", "unknown", "ocean floor", "underground",
    "scrub", "rocky", "savanna", "tundra", "glacial", "rainforest", "jungle", 
    "swamp", "wetlands", "brush", "steppe", "farmland", "volcanic"
};

void do_makeblade( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int level, chance, charge;
    bool checktool, checkdura, checkbatt, checkoven; 
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;
    AFFECT_DATA *paf;
    AFFECT_DATA *paf2;
            
    strcpy( arg , argument );
    
    switch( ch->substate )
    { 
    	default:
    	        
    	        if ( arg[0] == '\0' )
                {
                  send_to_char( "&RSintaxis: fabricarhoja <nombre>\n\r&w", ch);
                  return;   
                }
 
    	        checktool = FALSE;
                checkdura = FALSE;
                checkbatt = FALSE;
                checkoven = FALSE;
        
                if ( !IS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
                {
                   send_to_char( "&Rnecesitas estar en una fábrica o taller para hacer eso.\n\r", ch);
                   return;
                }
                
                for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
                {
                  if (obj->item_type == ITEM_TOOLKIT)
                    checktool = TRUE;
                  if (obj->item_type == ITEM_DURASTEEL)
          	    checkdura = TRUE;
                  if (obj->item_type == ITEM_BATTERY)
                  checkbatt = TRUE;

                  if (obj->item_type == ITEM_OVEN)
                  checkoven = TRUE;                  
                }
                
                if ( !checktool )
                {
                   send_to_char( "&RNecesitas una caja de herramientas.\n\r", ch);
                   return;
                }
 
                if ( !checkdura )
                {
                   send_to_char( "&RNecesitas algo de duracero.\n\r", ch);
                   return;
                }

                if ( !checkbatt )
                {
                   send_to_char( "&RNecesitas una batería.\n\r", ch);
                   return;
                }
                
                if ( !checkoven )
                {
                   send_to_char( "&RNecesitas un horno pequeño.\n\r", ch);
                   return;
                }
 
    	        chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_makeblade]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GComienzas el largo proceso de crear una vibrohoja.\n\r", ch);
    		   act( AT_PLAIN, "$n comienza a trabajar en algo con un horno pequeño y una caja de herramientas.", ch,
		        NULL, argument , TO_ROOM );
		   add_timer ( ch , TIMER_DO_FUN , 25 , do_makeblade , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   return;
	        }
	        send_to_char("&RNo consigues colocar las piezas juntas.\n\r",ch);
	        learn_from_failure( ch, gsn_makeblade );
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
    	        send_to_char("&RTe interrumpes y no logras finalizar tu trabajo.\n\r", ch);
    	        return;
    }
    
    ch->substate = SUB_NONE;

    level = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makeblade]);
    vnum = 66;
    
    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
         send_to_char( "&REl objeto que estás intentando crear no está en la base de datos.\n\rPor favor notifica a la administración del error urgentemente.\n\r", ch );
         return;
    }

    checktool = FALSE;
    checkdura = FALSE;
    checkbatt = FALSE;
    checkoven = FALSE;
    
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
       if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;
       if (obj->item_type == ITEM_OVEN)
          checkoven = TRUE;
       if (obj->item_type == ITEM_DURASTEEL && checkdura == FALSE)
       {
          checkdura = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
       }
       if (obj->item_type == ITEM_BATTERY && checkbatt == FALSE )
       {
          charge = UMAX( 5, obj->value[0] ); 
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkbatt = TRUE;
       }
    }                            
    
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makeblade]) ;
                
    if ( number_percent( ) > chance*2  || ( !checktool ) || ( !checkdura ) || ( !checkbatt ) || ( !checkoven ) )
    {
       send_to_char( "&RActivas tu nueva vibrahoja.\n\r", ch);
       send_to_char( "&RLa vibrohoja humea durante unos segundos y se sacude violentamente.\n\r", ch);
       send_to_char( "&RFinalmente se rompe en trocitos.\n\r", ch);
       learn_from_failure( ch, gsn_makeblade );
       return;
    }

    obj = create_object( pObjIndex, level );
    
    obj->item_type = ITEM_WEAPON;
    SET_BIT( obj->wear_flags, ITEM_WIELD );
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    obj->level = level;
    obj->weight = 3;
    STRFREE( obj->name );
    strcpy( buf, arg );
    strcat( buf, " vibro-blade blade hoja vibrohoja vibro-hoja" );
    strcat( buf, remand(buf));
    obj->name = STRALLOC( buf );
    strcpy( buf, arg );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );        
    STRFREE( obj->description );
    strcat( buf, " está aquí." );
    obj->description = STRALLOC( buf );
    CREATE( paf, AFFECT_DATA, 1 );
    paf->type               = -1;
    paf->duration           = -1;
    paf->location           = get_atype( "backstab" );
    paf->modifier           = level/3;
    paf->bitvector          = 0;
    paf->next               = NULL;
    LINK( paf, obj->first_affect, obj->last_affect, next, prev );
    ++top_affect;
    CREATE( paf2, AFFECT_DATA, 1 );
    paf2->type               = -1;
    paf2->duration           = -1;
    paf2->location           = get_atype( "hitroll" );
    paf2->modifier           = -2;
    paf2->bitvector          = 0;
    paf2->next               = NULL;
    LINK( paf2, obj->first_affect, obj->last_affect, next, prev );
    ++top_affect;
    obj->value[0] = INIT_WEAPON_CONDITION;      
    obj->value[1] = (int) (level/10+15);      /* min dmg  */
    obj->value[2] = (int) (level/5+20);      /* max dmg */
    obj->value[3] = WEAPON_VIBRO_BLADE;
    obj->value[4] = charge;
    obj->value[5] = charge;
    obj->cost = obj->value[2]*10;
                                                                    
    obj = obj_to_char( obj, ch );
                                                            
    send_to_char( "&GFinalizas tu trabajo y admiras tu nueva vibrohoja.&w\n\r", ch);
    act( AT_PLAIN, "$n termina de construir una vibrohoja.", ch,
         NULL, argument , TO_ROOM );
    
    {
         long xpgain;
         
         xpgain = UMIN( obj->cost*200 ,( exp_level(ch->skill_level[ENGINEERING_ABILITY]+1) - exp_level(ch->skill_level[ENGINEERING_ABILITY]) ) );
         gain_exp(ch, xpgain, ENGINEERING_ABILITY);
         ch_printf( ch , "Recibes %d puntos de experiencia en ingeniero.", xpgain );
    }
        
    learn_from_success( ch, gsn_makeblade );
}

void do_makeblaster( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int level, chance;
    bool checktool, checkdura, checkbatt, checkoven, checkcond, checkcirc, checkammo;
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum, power, scope, ammo;
    AFFECT_DATA *paf;
    AFFECT_DATA *paf2;
    
    strcpy( arg , argument );
    
    switch( ch->substate )
    { 
    	default:
    	        if ( arg[0] == '\0' )
                {
                  send_to_char( "&RSintaxis: fabricararmalaser <nombre>\n\r&w", ch);
                  return;   
                }

    	        checktool = FALSE;
                checkdura = FALSE;
                checkbatt = FALSE;
                checkoven = FALSE;
                checkcond = FALSE;
                checkcirc = FALSE;

                if ( !IS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
                {
                   send_to_char( "&Rnecesitas estar en una fábrica o taller para hacer eso.\n\r", ch);
                   return;
                }
                
                for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
                {
                  if (obj->item_type == ITEM_TOOLKIT)
                    checktool = TRUE;
                  if (obj->item_type == ITEM_DURAPLAST)
          	    checkdura = TRUE;
                  if (obj->item_type == ITEM_BATTERY)
                    checkbatt = TRUE;
                  if (obj->item_type == ITEM_OVEN)
                    checkoven = TRUE;
                  if (obj->item_type == ITEM_CIRCUIT)
                    checkcirc = TRUE;
                  if (obj->item_type == ITEM_SUPERCONDUCTOR)
                    checkcond = TRUE;                  
                }
                
                if ( !checktool )
                {
                   send_to_char( "&RNecesitas una caja de herramientas.\n\r", ch);
                   return;
                }
 
                if ( !checkdura )
                {
                   send_to_char( "&RNecesitas algo de duracero.\n\r", ch);
                   return;
                }

                if ( !checkbatt )
                {
                   send_to_char( "&RNecesitas una batería.\n\r", ch);
                   return;
                }
                
                if ( !checkoven )
                {
                   send_to_char( "&RNecesitas un horno.\n\r", ch);
                   return;
                }
                
                if ( !checkcirc )
                {
                   send_to_char( "&RNecesitas un circuito.\n\r", ch);
                   return;
                }
                
                if ( !checkcond )
                {
                   send_to_char( "&RNecesitas un superconductor.\n\r", ch);
                   return;
                }
 
    	        chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_makeblaster]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GComienzas el largo proceso de construir un arma láser.\n\r", ch);
    		   act( AT_PLAIN, "$n usa su caja de herramientas y un horno y se pone a trabajar en algo.", ch,
		        NULL, argument , TO_ROOM );
		   add_timer ( ch , TIMER_DO_FUN , 25 , do_makeblaster , 1 );
    		   ch->dest_buf   = str_dup(arg);
    		   return;
	        }
	        send_to_char("&RNo consigues juntar todas las piezas.\n\r",ch);
	        learn_from_failure( ch, gsn_makeblaster );
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
    
    level = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makeblaster]);
    vnum = 50;
    
    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
         send_to_char( "&REl objeto que tratas de crear no está en la base de datos.\n\rPor favor, notifícalo urgentemente a la administración.\n\r", ch );
         return;
    }

    checkammo= FALSE;
    checktool = FALSE;
    checkdura = FALSE;
    checkbatt = FALSE;
    checkoven = FALSE;
    checkcond = FALSE;
    checkcirc = FALSE;
    power     = 0;
    scope     = 0;
    ammo = 0;
    
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
       if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;
       if (obj->item_type == ITEM_OVEN)
          checkoven = TRUE;
       if (obj->item_type == ITEM_DURAPLAST && checkdura == FALSE)
       {
          checkdura = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
       }
       if (obj->item_type == ITEM_AMMO && checkammo == FALSE)
       {
          ammo = obj->value[0];
          checkammo = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
       }
       if (obj->item_type == ITEM_BATTERY && checkbatt == FALSE)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkbatt = TRUE;
       }
       if (obj->item_type == ITEM_LENS && scope == 0)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          scope++;
       }
       if (obj->item_type == ITEM_SUPERCONDUCTOR && power<2)
       {
          power++;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkcond = TRUE;
       }
       if (obj->item_type == ITEM_CIRCUIT && checkcirc == FALSE)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkcirc = TRUE;
       }
    }                            
    
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makeblaster]) ;
                
    if ( number_percent( ) > chance*2  || ( !checktool ) || ( !checkdura ) || ( !checkbatt ) || ( !checkoven )  || ( !checkcond ) || ( !checkcirc) )
    {
       send_to_char( "&REmpuñas tu nueva arma láser y apuntas a un trozo de duraplástico sobrante.\n\r", ch);
       send_to_char( "&RAprietas el gatillo deseando lo mejor...\n\r", ch);
       send_to_char( "&RTu arma láser comienza a arder, destruyéndose y quemándote la mano.\n\r", ch);
       learn_from_failure( ch, gsn_makeblaster );
       return;
    }

    obj = create_object( pObjIndex, level );
    
    obj->item_type = ITEM_WEAPON;
    SET_BIT( obj->wear_flags, ITEM_WIELD );
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    obj->level = level;
    obj->weight = 2+level/10;
    STRFREE( obj->name );
    strcpy( buf , arg );
    strcat( buf , " blaster ");
    strcat( buf, remand(buf));
    obj->name = STRALLOC( buf );
    strcpy( buf, arg );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );        
    STRFREE( obj->description );
    strcat( buf, " está aquí." );
    obj->description = STRALLOC( buf );
    CREATE( paf, AFFECT_DATA, 1 );
    paf->type               = -1;
    paf->duration           = -1;
    paf->location           = get_atype( "hitroll" );
    paf->modifier           = URANGE( 0, 1+scope, level/30 );
    paf->bitvector          = 0;
    paf->next               = NULL;
    LINK( paf, obj->first_affect, obj->last_affect, next, prev );
    ++top_affect;
    CREATE( paf2, AFFECT_DATA, 1 );
    paf2->type               = -1;
    paf2->duration           = -1;
    paf2->location           = get_atype( "damroll" );
    paf2->modifier           = URANGE( 0, power, level/30);
    paf2->bitvector          = 0;
    paf2->next               = NULL;
    LINK( paf2, obj->first_affect, obj->last_affect, next, prev );
    ++top_affect;
    obj->value[0] = INIT_WEAPON_CONDITION;       /* condition  */
    obj->value[1] = (int) (level/10+15);      /* min dmg  */
    obj->value[2] = (int) (level/5+25);      /* max dmg  */
    obj->value[3] = WEAPON_BLASTER;
    obj->value[4] = ammo;
    obj->value[5] = 2000;
    obj->cost = obj->value[2]*50;
                                                                   
    obj = obj_to_char( obj, ch );
                                                            
    send_to_char( "&GFinalizas la construcción de tu arma láser.&w\n\r", ch);
    act( AT_PLAIN, "$n termina de construir un arma láser.", ch,
         NULL, argument , TO_ROOM );
    
    {
         long xpgain;
         
         xpgain = UMIN( obj->cost*50 ,( exp_level(ch->skill_level[ENGINEERING_ABILITY]+1) - exp_level(ch->skill_level[ENGINEERING_ABILITY]) ) );
         gain_exp(ch, xpgain, ENGINEERING_ABILITY);
         ch_printf( ch , "Recibes %d puntos de experiencia en ingeniero.", xpgain );
    }
    learn_from_success( ch, gsn_makeblaster );
}

void do_makelightsaber( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int chance;
    bool checktool, checkdura, checkbatt, 
         checkoven, checkcond, checkcirc, checklens, checkgems, checkmirr;
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum, level, gems, charge, gemtype;
    AFFECT_DATA *paf;
    AFFECT_DATA *paf2;
    
    strcpy( arg, argument );    
    
    switch( ch->substate )
    { 
    	default:
    	        if ( arg[0] == '\0' )
                {
                  send_to_char( "&RUsage: Makelightsaber <name>\n\r&w", ch);
                  return;   
                }

    	        checktool = FALSE;
                checkdura = FALSE;
                checkbatt = FALSE;
                checkoven = FALSE;
                checkcond = FALSE;
                checkcirc = FALSE;
                checklens = FALSE;
                checkgems = FALSE;
                checkmirr = FALSE;

                if ( !IS_SET( ch->in_room->room_flags, ROOM_SAFE ) || !IS_SET( ch->in_room->room_flags, ROOM_SILENCE ))
                {
                   send_to_char( "&RYou need to be in a quiet peaceful place to craft a lightsaber.\n\r", ch);
                   return;
                }
                
                for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
                {
                  if (obj->item_type == ITEM_TOOLKIT)
                    checktool = TRUE;
                  if (obj->item_type == ITEM_LENS)
                    checklens = TRUE;
                  if (obj->item_type == ITEM_CRYSTAL)
                    checkgems = TRUE;                    
                  if (obj->item_type == ITEM_MIRROR)
                    checkmirr = TRUE;
                  if (obj->item_type == ITEM_DURAPLAST || obj->item_type == ITEM_DURASTEEL )
          	    checkdura = TRUE;
                  if (obj->item_type == ITEM_BATTERY)
                    checkbatt = TRUE;
                  if (obj->item_type == ITEM_OVEN)
                    checkoven = TRUE;
                  if (obj->item_type == ITEM_CIRCUIT)
                    checkcirc = TRUE;
                  if (obj->item_type == ITEM_SUPERCONDUCTOR)
                    checkcond = TRUE;                  
                }
                
                if ( !checktool )
                {
                   send_to_char( "&RYou need toolkit to make a lightsaber.\n\r", ch);
                   return;
                }
 
                if ( !checkdura )
                {
                   send_to_char( "&RYou need something to make it out of.\n\r", ch);
                   return;
                }

                if ( !checkbatt )
                {
                   send_to_char( "&RYou need a power source for your lightsaber.\n\r", ch);
                   return;
                }
                
                if ( !checkoven )
                {
                   send_to_char( "&RYou need a small furnace to heat and shape the components.\n\r", ch);
                   return;
                }
                
                if ( !checkcirc )
                {
                   send_to_char( "&RYou need a small circuit board.\n\r", ch);
                   return;
                }
                
                if ( !checkcond )
                {
                   send_to_char( "&RYou still need a small superconductor for your lightsaber.\n\r", ch);
                   return;
                }
                
                if ( !checklens )
                {
                   send_to_char( "&RYou still need a lens to focus the beam.\n\r", ch);
                   return;
                }
                
                if ( !checkgems )
                {
                   send_to_char( "&RLightsabers require 1 to 3 gems to work properly.\n\r", ch);
                   return;
                }
                
                if ( !checkmirr )
                {
                   send_to_char( "&RYou need a high intesity reflective cup to create a lightsaber.\n\r", ch);
                   return;
                }
 
    	        chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_makelightsaber]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GYou begin the long process of crafting a lightsaber.\n\r", ch);
    		   act( AT_PLAIN, "$n takes $s tools and a small oven and begins to work on something.", ch,
		        NULL, argument , TO_ROOM );
		   add_timer ( ch , TIMER_DO_FUN , 25 , do_makelightsaber , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   return;
	        }
	        send_to_char("&RYou can't figure out how to fit the parts together.\n\r",ch);
	        learn_from_failure( ch, gsn_makelightsaber );
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
    
    level = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makelightsaber]);
    vnum = 72;
    
    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
         send_to_char( "&REl objeto que tratas de crear no está en la base de datos.\n\rPor favor, notifícalo urgentemente a la administración.\n\r", ch );
         return;
    }

    checktool = FALSE;
    checkdura = FALSE;
    checkbatt = FALSE;
    checkoven = FALSE;
    checkcond = FALSE;
    checkcirc = FALSE;
    checklens = FALSE;
    checkgems = FALSE;
    checkmirr = FALSE;
    gems = 0;
    charge = 0;
    gemtype =0;

    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
       if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;
       if (obj->item_type == ITEM_OVEN)
          checkoven = TRUE;
       if ( (obj->item_type == ITEM_DURAPLAST || obj->item_type == ITEM_DURASTEEL) && checkdura == FALSE)
       {
          checkdura = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
       }
       if (obj->item_type == ITEM_DURASTEEL && checkdura == FALSE)
       {
          checkdura = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
       }
       if (obj->item_type == ITEM_BATTERY && checkbatt == FALSE)
       {
          charge = UMIN(obj->value[1], 10);
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkbatt = TRUE;
       }
       if (obj->item_type == ITEM_SUPERCONDUCTOR && checkcond == FALSE)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkcond = TRUE;
       }
       if (obj->item_type == ITEM_CIRCUIT && checkcirc == FALSE)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkcirc = TRUE;
       }
       if (obj->item_type == ITEM_LENS && checklens == FALSE)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checklens = TRUE;
       }
       if (obj->item_type == ITEM_MIRROR && checkmirr == FALSE)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkmirr = TRUE;
       }
       if (obj->item_type == ITEM_CRYSTAL && gems < 3)
       {
          gems++;
          if ( gemtype < obj->value[0] )
             gemtype = obj->value[0];
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkgems = TRUE;
       }
    }                            
    
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makelightsaber]) ;
                
    if ( number_percent( ) > chance*2  || ( !checktool ) || ( !checkdura ) || ( !checkbatt ) || ( !checkoven ) 
                                       || ( !checkmirr ) || ( !checklens ) || ( !checkgems ) || ( !checkcond ) || ( !checkcirc) )
    
    {
       send_to_char( "&RYou hold up your new lightsaber and press the switch hoping for the best.\n\r", ch);
       send_to_char( "&RInstead of a blade of light, smoke starts pouring from the handle.\n\r", ch);
       send_to_char( "&RYou drop the hot handle and watch as it melts on away on the floor.\n\r", ch);
       learn_from_failure( ch, gsn_makelightsaber );
       return;
    }

    obj = create_object( pObjIndex, level );
    
    obj->item_type = ITEM_WEAPON;
    SET_BIT( obj->wear_flags, ITEM_WIELD );
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    SET_BIT( obj->extra_flags, ITEM_ANTI_SOLDIER );
    SET_BIT( obj->extra_flags, ITEM_ANTI_THIEF );
    SET_BIT( obj->extra_flags, ITEM_ANTI_HUNTER );
    SET_BIT( obj->extra_flags, ITEM_ANTI_PILOT );
    SET_BIT( obj->extra_flags, ITEM_ANTI_CITIZEN );
    obj->level = level;
    obj->weight = 5;
    STRFREE( obj->name );
    obj->name = STRALLOC( "lightsaber saber" );
    strcpy( buf, arg );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );        
    STRFREE( obj->description );
    strcat( buf, " was carelessly misplaced here." );
    obj->description = STRALLOC( buf );
    STRFREE( obj->action_desc );
    strcpy( buf, arg );
    strcat( buf, " ignites with a hum and a soft glow." );
    obj->action_desc = STRALLOC( buf );
    CREATE( paf, AFFECT_DATA, 1 );
    paf->type               = -1;
    paf->duration           = -1;
    paf->location           = get_atype( "hitroll" );
    paf->modifier           = URANGE( 0, gems, level/10 );
    paf->bitvector          = 0;
    paf->next               = NULL;
    LINK( paf, obj->first_affect, obj->last_affect, next, prev );
    ++top_affect;
    CREATE( paf2, AFFECT_DATA, 1 );
    paf2->type               = -1;
    paf2->duration           = -1;
    paf2->location           = get_atype( "parry" );
    paf2->modifier           = ( level/3 );
    paf2->bitvector          = 0;
    paf2->next               = NULL;
    LINK( paf2, obj->first_affect, obj->last_affect, next, prev );
    ++top_affect;
    obj->value[0] = INIT_WEAPON_CONDITION;       /* condition  */
    obj->value[1] = (int) (level/10+gemtype*2);      /* min dmg  */
    obj->value[2] = (int) (level/5+gemtype*6);      /* max dmg */
    obj->value[3] = WEAPON_LIGHTSABER;
    obj->value[4] = charge;
    obj->value[5] = charge;
    obj->cost = obj->value[2]*75;
                                                                    
    obj = obj_to_char( obj, ch );
                                                            
    send_to_char( "&GYou finish your work and hold up your newly created lightsaber.&w\n\r", ch);
    act( AT_PLAIN, "$n finishes making $s new lightsaber.", ch,
         NULL, argument , TO_ROOM );
    
    {
         long xpgain;
         
	 xpgain = UMIN( obj->cost*50 , (ch->pcdata->learned[gsn_makelightsaber] * 50));
// Changed. -Tawnos
//         xpgain = UMIN( obj->cost*50 ,( exp_level(ch->skill_level[FORCE_ABILITY]+1) - exp_level(ch->skill_level[ENGINEERING_ABILITY]) ) );
         gain_exp(ch, xpgain, FORCE_ABILITY);
         ch_printf( ch , "You gain %d force experience.", xpgain );
    }
        learn_from_success( ch, gsn_makelightsaber );
}


void do_makespice( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int chance;
    OBJ_DATA *obj;
        
    switch( ch->substate )
    { 
    	default:
    	        strcpy( arg, argument );
    	        
    	        if ( arg[0] == '\0' )
                {
                  send_to_char( "&R¿De qué?\n\r&w", ch);
                  return;   
                }
    	        
    	        if ( !IS_SET( ch->in_room->room_flags, ROOM_REFINERY ) )
                {
                   send_to_char( "&RNecesitas estar en una refinería para crear drogas a partir especias.\n\r", ch);
                   return;
                }
                
                if ( ms_find_obj(ch) )
                      return;
                
                if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
                {
                        send_to_char( "&RNo tienes ese objeto.\n\r&w", ch );
                        return;
                }                                                            
                
                if ( obj->item_type != ITEM_RAWSPICE )
                {
                       send_to_char( "&RNo puedes hacer una droga con eso.\n\r&w",ch);
                       return;
                }
                
    	        chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_spice_refining]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GComienzas el delicado y largo proceso de refinar una especie para crear drogas.\n\r", ch);
    		   act( AT_PLAIN, "$n comienza a trabajar en algo.", ch,
		        NULL, argument , TO_ROOM );
		   add_timer ( ch , TIMER_DO_FUN , 10 , do_makespice , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   return;
	        }
	        send_to_char("&RNo consigues nada útil.\n\r",ch);
	        learn_from_failure( ch, gsn_spice_refining );
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
    	        send_to_char("&RTe distraes y no consigues finalizar tu trabajo.\n\r&w", ch);
    	        return;
    }
    
    ch->substate = SUB_NONE;
    
    if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
    {
           send_to_char( "Pierdes la especia.\n\r", ch );
           return;
    }                                                            
    if ( obj->item_type != ITEM_RAWSPICE )
    {
          send_to_char( "&RNo consigues terminar tu trabajo.\n\r&w",ch);
          return;
    }
    
    obj->value[1] = URANGE (10, obj->value[1], ( IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_spice_refining]) ) +10);
    strcpy( buf, obj->name );
    STRFREE( obj->name );
    strcat( buf, " droga especie " );
    strcat( buf, remand(buf));
    obj->name = STRALLOC( buf );
    strcpy( buf, "una droga hecha de " );
    strcat( buf, obj->short_descr );   
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf ); 
    strcat( buf, " se extiende por aquí." );
    STRFREE( obj->description );
    obj->description = STRALLOC( buf ); 
    obj->item_type = ITEM_SPICE;
    
    send_to_char( "&GFinalizas tu trabajo.\n\r", ch);
    act( AT_PLAIN, "$n finaliza su trabajo.", ch,
         NULL, argument , TO_ROOM );
    
    obj->cost += obj->value[1]*10;
    {
         long xpgain;
         
         xpgain = UMIN( obj->cost*50 ,( exp_level(ch->skill_level[ENGINEERING_ABILITY]+1) - exp_level(ch->skill_level[ENGINEERING_ABILITY]) ) );
         gain_exp(ch, xpgain, ENGINEERING_ABILITY);
         ch_printf( ch , "Recibes %d puntos de experiencia en ingeniero.", xpgain );
    }
                 
    learn_from_success( ch, gsn_spice_refining );
    	
}

void do_makegrenade( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int level, chance, strength, weight;
    bool checktool, checkdrink, checkbatt, checkchem, checkcirc;
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;
    
    strcpy( arg , argument );
    
    switch( ch->substate )
    { 
    	default:
    	        if ( arg[0] == '\0' )
                {
                  send_to_char( "&RSintaxis: fabricargranadae <nombre>\n\r&w", ch);
                  return;   
                }

    	        checktool  = FALSE;
                checkdrink = FALSE;
                checkbatt  = FALSE;
                checkchem  = FALSE;
                checkcirc  = FALSE;

                if ( !IS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
                {
                   send_to_char( "&Rnecesitas estar en una fábrica o taller para hacer eso.\n\r", ch);
                   return;
                }
                
                for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
                {
                  if (obj->item_type == ITEM_TOOLKIT)
                    checktool = TRUE;
                  if (obj->item_type == ITEM_DRINK_CON && obj->value[1] == 0 )
          	    checkdrink = TRUE;
                  if (obj->item_type == ITEM_BATTERY)
                    checkbatt = TRUE;
                  if (obj->item_type == ITEM_CIRCUIT)
                    checkcirc = TRUE;
                  if (obj->item_type == ITEM_CHEMICAL)
                    checkchem = TRUE;                  
                }
                
                if ( !checktool )
                {
                   send_to_char( "&Rnecesitas una caja de herramientas.\n\r", ch);
                   return;
                }
 
                if ( !checkdrink )
                {
                   send_to_char( "&RNecesitas un contenedor de líquidos vacío para mezclar los productos químicos.\n\r", ch);
                   return;
                }

                if ( !checkbatt )
                {
                   send_to_char( "&RNecesitas una batería.\n\r", ch);
                   return;
                }
                
                if ( !checkcirc )
                {
                   send_to_char( "&RNecesitas un circuito.\n\r", ch);
                   return;
                }
                
                if ( !checkchem )
                {
                   send_to_char( "&RNecesitas productos químicos explosivos.\n\r", ch);
                   return;
                }
 
    	        chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_makegrenade]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GComienzas el largo proceso de fabricación de una granada.\n\r", ch);
    		   act( AT_PLAIN, "$n coge su caja de herramientas y comienza a trabajar en algo.", ch,
		        NULL, argument , TO_ROOM );
		   add_timer ( ch , TIMER_DO_FUN , 25 , do_makegrenade , 1 );
    		   ch->dest_buf   = str_dup(arg);
    		   return;
	        }
	        send_to_char("&RNo consigues juntar las piezas.\n\r",ch);
	        learn_from_failure( ch, gsn_makegrenade );
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
    	        send_to_char("&RTe distraes y no logras finalizar tu trabajo.\n\r", ch);
    	        return;
    }
    
    ch->substate = SUB_NONE;
    
    level = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makegrenade]);
    vnum = 71;
    
    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
         send_to_char( "&Rel objeto que tratas de crear no se encuentra en la base de datos.\n\rPor favor notifícaselo urgentemente a la administración.\n\r", ch );
         return;
    }

    checktool = FALSE;
    checkdrink = FALSE;
    checkbatt = FALSE;
    checkchem = FALSE;
    checkcirc = FALSE;
    
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
       if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;
       if (obj->item_type == ITEM_DRINK_CON && checkdrink == FALSE && obj->value[1] == 0 )
       {
          checkdrink = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
       }
       if (obj->item_type == ITEM_BATTERY && checkbatt == FALSE)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkbatt = TRUE;
       }
       if (obj->item_type == ITEM_CHEMICAL)
       {
          strength = URANGE( 10, obj->value[0], level * 5 );
          weight = obj->weight;
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
    }                            
    
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makegrenade]) ;
                
    if ( number_percent( ) > chance*2  || ( !checktool ) || ( !checkdrink ) || ( !checkbatt ) || ( !checkchem ) || ( !checkcirc) )
    {
       send_to_char( "&R¡cuando vas a terminar la granhada te explota en las manos!\n\r", ch);
       learn_from_failure( ch, gsn_makegrenade );
       return;
    }

    obj = create_object( pObjIndex, level );
    
    obj->item_type = ITEM_GRENADE;
    SET_BIT( obj->wear_flags, ITEM_HOLD );
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    obj->level = level;
    obj->weight = weight;
    STRFREE( obj->name );
    strcpy( buf , arg );
    strcat( buf , " granada ");
    strcat( buf, remand(buf));
    obj->name = STRALLOC( buf );
    strcpy( buf, arg );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );        
    STRFREE( obj->description );
    strcat( buf, " was carelessly misplaced here." );
    obj->description = STRALLOC( buf );
    obj->value[0] = strength*2;
    obj->value[1] = strength*3;
    obj->cost = obj->value[1]*5;
                                                                   
    obj = obj_to_char( obj, ch );
                                                            
    send_to_char( "&GFinalizas tu trabajo y admiras tu nueva granada.&w\n\r", ch);
    act( AT_PLAIN, "$n termina de construir una granada.", ch,
         NULL, argument , TO_ROOM );
    
    {
         long xpgain;
         
         xpgain = UMIN( obj->cost*50 ,( exp_level(ch->skill_level[ENGINEERING_ABILITY]+1) - exp_level(ch->skill_level[ENGINEERING_ABILITY]) ) );
         gain_exp(ch, xpgain, ENGINEERING_ABILITY);
         ch_printf( ch , "Recibes %d puntos de experiencia en ingeniero.", xpgain );
    }
        learn_from_success( ch, gsn_makegrenade );
}

void do_makelandmine( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int level, chance, strength, weight;
    bool checktool, checkdrink, checkbatt, checkchem, checkcirc;
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;
    
    strcpy( arg , argument );
    
    switch( ch->substate )
    { 
    	default:
    	        if ( arg[0] == '\0' )
                {
                  send_to_char( "&RSintaxis: fabricarmina <nombre>\n\r&w", ch);
                  return;   
                }

    	        checktool  = FALSE;
                checkdrink = FALSE;
                checkbatt  = FALSE;
                checkchem  = FALSE;
                checkcirc  = FALSE;

                if ( !IS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
                {
                   send_to_char( "&Rnecesitas estar en una fábrica o taller para hacer eso.\n\r", ch);
                   return;
                }
                
                for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
                {
                  if (obj->item_type == ITEM_TOOLKIT)
                    checktool = TRUE;
                  if (obj->item_type == ITEM_DRINK_CON && obj->value[1] == 0 )
          	    checkdrink = TRUE;
                  if (obj->item_type == ITEM_BATTERY)
                    checkbatt = TRUE;
                  if (obj->item_type == ITEM_CIRCUIT)
                    checkcirc = TRUE;
                  if (obj->item_type == ITEM_CHEMICAL)
                    checkchem = TRUE;                  
                }
                
                if ( !checktool )
                {
                   send_to_char( "&RYou need toolkit to make a landmine.\n\r", ch);
                   return;
                }
 
                if ( !checkdrink )
                {
                   send_to_char( "&RYou will need an empty drink container to mix and hold the chemicals.\n\r", ch);
                   return;
                }

                if ( !checkbatt )
                {
                   send_to_char( "&RYou need a small battery for the detonator.\n\r", ch);
                   return;
                }
                
                if ( !checkcirc )
                {
                   send_to_char( "&RYou need a small circuit for the detonator.\n\r", ch);
                   return;
                }
                
                if ( !checkchem )
                {
                   send_to_char( "&RSome explosive chemicals would come in handy!\n\r", ch);
                   return;
                }
 
    	        chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_makelandmine]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GYou begin the long process of making a landmine.\n\r", ch);
    		   act( AT_PLAIN, "$n takes $s tools and a drink container and begins to work on something.", ch,
		        NULL, argument , TO_ROOM );
		   add_timer ( ch , TIMER_DO_FUN , 25 , do_makelandmine , 1 );
    		   ch->dest_buf   = str_dup(arg);
    		   return;
	        }
	        send_to_char("&RYou can't figure out how to fit the parts together.\n\r",ch);
	        learn_from_failure( ch, gsn_makelandmine );
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
    
    level = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makelandmine]);
    vnum = 70;
    
    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
         send_to_char( "&REl objeto que tratas de crear no está en la base de datos.\n\rPor favor, notifícalo urgentemente a la administración.\n\r", ch );
         return;
    }

    checktool = FALSE;
    checkdrink = FALSE;
    checkbatt = FALSE;
    checkchem = FALSE;
    checkcirc = FALSE;
    
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
       if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;
       if (obj->item_type == ITEM_DRINK_CON && checkdrink == FALSE && obj->value[1] == 0 )
       {
          checkdrink = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
       }
       if (obj->item_type == ITEM_BATTERY && checkbatt == FALSE)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkbatt = TRUE;
       }
       if (obj->item_type == ITEM_CHEMICAL)
       {
          strength = URANGE( 10, obj->value[0], level * 5 );
          weight = obj->weight;
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
    }                            
    
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makelandmine]) ;
                
    if ( number_percent( ) > chance*2  || ( !checktool ) || ( !checkdrink ) || ( !checkbatt ) || ( !checkchem ) || ( !checkcirc) )
    {
       send_to_char( "&RJust as you are about to finish your work,\n\ryour newly created landmine explodes in your hands...doh!\n\r", ch);
       learn_from_failure( ch, gsn_makelandmine );
       return;
    }

    obj = create_object( pObjIndex, level );
    
    obj->item_type = ITEM_LANDMINE;
    SET_BIT( obj->wear_flags, ITEM_HOLD );
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    obj->level = level;
    obj->weight = weight;
    STRFREE( obj->name );
    strcpy( buf , arg );
    strcat( buf , " landmine ");
    strcat( buf, remand(buf));
    obj->name = STRALLOC( buf );
    strcpy( buf, arg );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );        
    STRFREE( obj->description );
    strcat( buf, " was carelessly misplaced here." );
    obj->description = STRALLOC( buf );
    obj->value[0] = strength/2;
    obj->value[1] = strength;
    obj->cost = obj->value[1]*5;
                                                                   
    obj = obj_to_char( obj, ch );
                                                            
    send_to_char( "&GYou finish your work and hold up your newly created landmine.&w\n\r", ch);
    act( AT_PLAIN, "$n finishes making $s new landmine.", ch,
         NULL, argument , TO_ROOM );
    
    {
         long xpgain;
         
         xpgain = UMIN( obj->cost*50 ,( exp_level(ch->skill_level[ENGINEERING_ABILITY]+1) - exp_level(ch->skill_level[ENGINEERING_ABILITY]) ) );
         gain_exp(ch, xpgain, ENGINEERING_ABILITY);
         ch_printf( ch , "Recibes %d puntos de experiencia en ingeniero.", xpgain );
    }
        learn_from_success( ch, gsn_makelandmine );
}
void do_makelight( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int level, chance, strength;
    bool checktool, checkbatt, checkchem, checkcirc, checklens;
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;
    
    strcpy( arg , argument );
    
    switch( ch->substate )
    { 
    	default:
    	        if ( arg[0] == '\0' )
                {
                  send_to_char( "&Rsintaxis: fabricarlinterna <nombre>\n\r&w", ch);
                  return;   
                }

    	        checktool  = FALSE;
                checkbatt  = FALSE;
                checkchem  = FALSE;
                checkcirc  = FALSE;
                checklens = FALSE;

                if ( !IS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
                {
                   send_to_char( "&RNecesitas estar en una fábrica o taller para hacer esto.\n\r", ch);
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
                  if (obj->item_type == ITEM_LENS)
                    checklens = TRUE;                  
                }
                
                if ( !checktool )
                {
                   send_to_char( "&RNecesitas una caja de herramientas.\n\r", ch);
                   return;
                }
                
                if ( !checklens )
                {
                   send_to_char( "&RNecesitas una lente.\n\r", ch);
                   return;
                }
 
                if ( !checkbatt )
                {
                   send_to_char( "&RNecesitas una batería.\n\r", ch);
                   return;
                }
                
                if ( !checkcirc )
                {
                   send_to_char( "&RNecesitas un pequeño circuito.\n\r", ch);
                   return;
                }
                
                if ( !checkchem )
                {
                   send_to_char( "&R¡Necesitas algún producto químico!\n\r", ch);
                   return;
                }
 
    	        chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_makelight]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GComienzas el largo proceso de fabricar una linterna.\n\r", ch);
    		   act( AT_PLAIN, "$n comienza a fabricar algo con su caja de herramientas.", ch,
		        NULL, argument , TO_ROOM );
		   add_timer ( ch , TIMER_DO_FUN , 10 , do_makelight , 1 );
    		   ch->dest_buf   = str_dup(arg);
    		   return;
	        }
	        send_to_char("&RNo consigues colocar las piezas juntas.\n\r",ch);
	        learn_from_failure( ch, gsn_makelight );
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
    
    level = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makelight]);
    vnum = 65;
    
    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
         send_to_char( "&REl objeto que intentas crear no está en la base de datos.\n\rPlease inform the administration of this error.\n\r", ch );
         return;
    }

    checktool = FALSE;
    checklens = FALSE;
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
       if (obj->item_type == ITEM_LENS && checklens == FALSE)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checklens = TRUE;
       }
    }                            
    
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makelight]) ;
                
    if ( number_percent( ) > chance*2  || ( !checktool ) || ( !checklens ) || ( !checkbatt ) || ( !checkchem ) || ( !checkcirc) )
    {
       send_to_char( "&RCuando estás a punto de finalizar tu trabajo,\n\rla linterna explota en tus manos...doh!\n\r", ch);
       learn_from_failure( ch, gsn_makelight );
       return;
    }

    obj = create_object( pObjIndex, level );
    
    obj->item_type = ITEM_LIGHT;
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    obj->level = level;
    obj->weight = 3;
    STRFREE( obj->name );
    strcpy( buf , arg );
    strcat( buf , " light ");
    strcat( buf, remand(buf));
    obj->name = STRALLOC( buf );
    strcpy( buf, arg );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );        
    STRFREE( obj->description );
    strcat( buf, " ilumina el lugar." );
    obj->description = STRALLOC( buf );
    obj->value[2] = strength;
    obj->cost = obj->value[2];
                                                                   
    obj = obj_to_char( obj, ch );
                                                            
    send_to_char( "&GTerminas tu trabajo y admiras tu nueva linterna.&w\n\r", ch);
    act( AT_PLAIN, "$n termina de fabricar una linterna.", ch,
         NULL, argument , TO_ROOM );
    
    {
         long xpgain;
         
         xpgain = UMIN( obj->cost*100 ,( exp_level(ch->skill_level[ENGINEERING_ABILITY]+1) - exp_level(ch->skill_level[ENGINEERING_ABILITY]) ) );
         gain_exp(ch, xpgain, ENGINEERING_ABILITY);
         ch_printf( ch , "Recibes %d puntos de experiencia en ingeniero.", xpgain );
    }
        learn_from_success( ch, gsn_makelight );
}

void do_makejewelry( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int level, chance;
    bool checktool, checkoven, checkmetal; 
    OBJ_DATA *obj;
    OBJ_DATA *metal;
    AFFECT_DATA *paf;
    int value, cost;
            
    argument = one_argument( argument, arg );
    strcpy ( arg2, argument);
    
    if ( !str_cmp( arg, "cuerpo" )
    || !str_cmp( arg, "cabeza" )
    || !str_cmp( arg, "piernas" )
    || !str_cmp( arg, "brazos" )
    || !str_cmp( arg, "sobre" )
    || !str_cmp( arg, "ojos" )
    || !str_cmp( arg, "cintura" )
    || !str_cmp( arg, "sostenido" )
    || !str_cmp( arg, "pies" )
    || !str_cmp( arg, "manos" ))
    {
        send_to_char( "&RNo puedes fabricar joyas para esa parte del cuerpo.\n\r&w", ch);
        send_to_char( "&RPrueba fabricararmadura.\n\r&w", ch);
        return;
    }
    if ( !str_cmp( arg, "escudo" ) )
    {
        send_to_char( "&RNo puedes usar una joya como escudo.\n\r&w", ch);
        send_to_char( "&RPrueba fabricarescudo.\n\r&w", ch);
        return;
    }
    if ( !str_cmp( arg, "blandido" ) )
    {
        send_to_char( "&R¿Estás intentando fabricar un arma de joyas?\n\r&w", ch);
        send_to_char( "&RPrueba fabricarhoja...\n\r&w", ch);
        return;
    }
    if ( !str_cmp( arg, "holster1")
    || !str_cmp( arg, "holster2") )
    {
	send_to_char( "&RNo puedes usar fabricarjoya ahí.&w\n\r", ch);
	send_to_char( "&RPrueba fabricarcontenedor...\n\r&w", ch);
        return;
    }

    switch( ch->substate )
    { 
    	default:
    	        
    	        if ( arg2[0] == '\0' )
                {
                  send_to_char( "&RSintaxis: fabricarjoya <localización> <nombre>\n\r&w", ch);
                  return;   
                }
 
    	        checktool = FALSE;
                checkoven = FALSE;
                checkmetal = FALSE;
        
                if ( !IS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
                {
                   send_to_char( "&Rnecesitas estar en una fábrica o taller para hacer eso.\n\r", ch);
                   return;
                }
                
                for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
                {
                  if (obj->item_type == ITEM_TOOLKIT)
                    checktool = TRUE;
                  if (obj->item_type == ITEM_OVEN)
          	    checkoven = TRUE;
                  if (obj->item_type == ITEM_RARE_METAL)
          	    checkmetal = TRUE;
                }
                
                if ( !checktool )
                {
                   send_to_char( "&RNecesitas una caja de herramientas.\n\r", ch);
                   return;
                }
 
                if ( !checkoven )
                {
                   send_to_char( "&RNecesitas un horno.\n\r", ch);
                   return;
                }
                
                if ( !checkmetal )
                {
                   send_to_char( "&RNecesitas algún metal precioso.\n\r", ch);
                   return;
                }

    	        chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_makejewelry]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GComienzas el largo proceso de construir una joya.\n\r", ch);
    		   act( AT_PLAIN, "$n coge su caja de herramientas y comienza a tgrabajar en algo.", ch,
		        NULL, argument , TO_ROOM );
		   add_timer ( ch , TIMER_DO_FUN , 15 , do_makejewelry , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   ch->dest_buf_2 = str_dup(arg2);
    		   return;
	        }
	        send_to_char("&RNo consigues hacer nada útil.\n\r",ch);
	        learn_from_failure( ch, gsn_makejewelry );
    	   	return;	
    	
    	case 1: 
    		if ( !ch->dest_buf )
    		     return;
    		if ( !ch->dest_buf_2 )
    		     return;
    		strcpy(arg, ch->dest_buf);
    		DISPOSE( ch->dest_buf);
    		strcpy(arg2, ch->dest_buf_2);
    		DISPOSE( ch->dest_buf_2);
    		break;
    		
    	case SUB_TIMER_DO_ABORT:
    		DISPOSE( ch->dest_buf );
    		DISPOSE( ch->dest_buf_2 );
    		ch->substate = SUB_NONE;    		                                   
    	        send_to_char("&RTe distraes y no consigues terminar tu trabajo.\n\r", ch);
    	        return;
    }
    
    ch->substate = SUB_NONE;
    
    level = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makejewelry]);
    
    checkmetal = FALSE;
    checkoven = FALSE;
    checktool = FALSE;
    value=0;
    cost=0;
    
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
       if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;
       if (obj->item_type == ITEM_OVEN)
          checkoven = TRUE;
       if (obj->item_type == ITEM_RARE_METAL && checkmetal == FALSE)
       {
          checkmetal = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          metal = obj;
       }
       if (obj->item_type == ITEM_CRYSTAL)
       {
          cost += obj->cost;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
       }
    }                            
    
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makejewelry]) ;
                
    if ( number_percent( ) > chance*2  || ( !checkoven ) || ( !checktool ) || ( !checkmetal ) )
    {
       send_to_char( "&RSostienes tu nueva joya.\n\r", ch);
       send_to_char( "&RDe pronto te das cuenta que has creado algo sin valor.\n\r", ch);
             learn_from_failure( ch, gsn_makejewelry );
       return;
    }

    obj = metal; 

    obj->item_type = ITEM_ARMOR;
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    value = get_wflag( arg );
    if ( value < 0 || value > 31 )
        SET_BIT( obj->wear_flags, ITEM_WEAR_NECK );                    
    else
        SET_BIT( obj->wear_flags, 1 << value );
    obj->level = level;
    STRFREE( obj->name );
    sprintf( buf, "%s ", arg2);
    strcat( buf, remand(buf));
    obj->name = STRALLOC( buf );
    strcpy( buf, arg2 );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );        
    STRFREE( obj->description );
    strcat( buf, " está aquí." );

// Create stat bonuses for Jewelry - Tawnos
    CREATE( paf, AFFECT_DATA, 1 );
    paf->type               = -1;
    paf->duration           = -1;
// Use smaller if check to minimize redundant code.
    if( IS_SET( obj->wear_flags, ITEM_WEAR_FINGER ) )
    {
    if(number_bits(1) == 0)
      paf->location           = get_atype( "hitroll" );
    else
      paf->location           = get_atype( "damroll" );

    paf->modifier           = number_bits(1)+1;
    }
    else if( IS_SET( obj->wear_flags, ITEM_WEAR_EARS ) )
    {
    if(number_bits(1) == 0)
      paf->location           = get_atype( "intelligence" );
    else
      paf->location           = get_atype( "wisdom" );

    paf->modifier           = number_bits(1)+1;
    }
    else if( IS_SET( obj->wear_flags, ITEM_WEAR_NECK ) )
    {
    if(number_bits(1) == 0)
      paf->location           = get_atype( "charisma" );
    else
      paf->location           = get_atype( "luck" );

    paf->modifier           = 1;
    }
    else if( IS_SET( obj->wear_flags, ITEM_WEAR_WRIST ) )
    {
    if(number_bits(1) == 0)
      paf->location           = get_atype( "strength" );
    else
      paf->location           = get_atype( "dexterity" );

    paf->modifier           = 1;
    }
    else
    {
    paf->location = get_atype("strength");
    paf->modifier = 1;
    }

    paf->bitvector          = 0;
    paf->next               = NULL;
    LINK( paf, obj->first_affect, obj->last_affect, next, prev );
    ++top_affect;
// End stat bonuses.

    obj->description = STRALLOC( buf );
    obj->value[1] = (int)((ch->pcdata->learned[gsn_makejewelry])/20) + metal->value[1];
    obj->value[0] = obj->value[1];      
    obj->cost *= 10;
    obj->cost += cost;
                                                                    
    obj = obj_to_char( obj, ch );
                                                            
    send_to_char( "&GFinalizas tu trabajo y admiras tu nueva joya.&w\n\r", ch);
    act( AT_PLAIN, "$n finaliza una joya.", ch,
         NULL, argument , TO_ROOM );
    
    {
         long xpgain;
         
         xpgain = UMIN( obj->cost*100 ,( exp_level(ch->skill_level[ENGINEERING_ABILITY]+1) - exp_level(ch->skill_level[ENGINEERING_ABILITY]) ) );
         gain_exp(ch, xpgain, ENGINEERING_ABILITY);
         ch_printf( ch , "Recibes %d puntos de experiencia en ingeniero.", xpgain );
    }
        learn_from_success( ch, gsn_makejewelry );
     
}

void do_makearmor( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int level, chance;
    bool checksew, checkfab; 
    OBJ_DATA *obj;
    OBJ_DATA *material;
    int value;
            
    argument = one_argument( argument, arg );
    strcpy ( arg2, argument);
    
    if ( !str_cmp( arg, "ojos" )
    || !str_cmp( arg, "orejas" )
    || !str_cmp( arg, "dedo" )
    || !str_cmp( arg, "cuello" )
    || !str_cmp( arg, "munyeca" ) )
    {
        send_to_char( "&RNo puedes fabricar ropa para esa parte del cuerpo.\n\r&w", ch);
        send_to_char( "&RPrueba fabricarjoya.\n\r&w", ch);
        return;
    }
    if ( !str_cmp( arg, "escudo" ) )
    {
        send_to_char( "&RNo puedes usar ropa como escudo.\n\r&w", ch);
        send_to_char( "&RPrueba fabricarescudo.\n\r&w", ch);
        return;
    }
    if ( !str_cmp( arg, "blandido" ) )
    {
        send_to_char( "&R¿Estás tratando hacer ropa láser?\n\r&w", ch);
        send_to_char( "&RPrueba fabricarbláster, fabricarpica o fabricarhoja...\n\r&w", ch);
        return;
    }
    if ( !str_cmp( arg, "holster1")
    || !str_cmp( arg, "holster2") )
    {
	send_to_char( "&RNo puedes usar armaduras ahí.&w\n\r", ch);
	send_to_char( "&RPrueba fabricarcontenedor...\n\r&w", ch);
        return;
    }
    
    switch( ch->substate )
    { 
    	default:
    	        
    	        if ( arg2[0] == '\0' )
                {
                  send_to_char( "&RSintaxis: fabricararmadura <localización> <nombre>\n\r&w", ch);
                  return;   
                }
 
    	        checksew = FALSE;
                checkfab = FALSE;
        
                if ( !IS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
                {
                   send_to_char( "&Rnecesitas estar en una fábrica o taller para hacer eso.\n\r", ch);
                   return;
                }
                
                for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
                {
                  if (obj->item_type == ITEM_FABRIC)
                    checkfab = TRUE;
                  if (obj->item_type == ITEM_THREAD)
          	    checksew = TRUE;
                }
                
                if ( !checkfab )
                {
                   send_to_char( "&RNecesitas tela o material.\n\r", ch);
                   return;
                }
 
                if ( !checksew )
                {
                   send_to_char( "&RNecesitas ilo y aguja.\n\r", ch);
                   return;
                }

    	        chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_makearmor]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GComienzas el largo proceso de crear una armadura.\n\r", ch);
    		   act( AT_PLAIN, "$n comienza a coser algo.", ch,
		        NULL, argument , TO_ROOM );
		   add_timer ( ch , TIMER_DO_FUN , 15 , do_makearmor , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   ch->dest_buf_2 = str_dup(arg2);
    		   return;
	        }
	        send_to_char("&RNo consigues hacer nada útil.\n\r",ch);
	        learn_from_failure( ch, gsn_makearmor );
    	   	return;	
    	
    	case 1: 
    		if ( !ch->dest_buf )
    		     return;
    		if ( !ch->dest_buf_2 )
    		     return;
    		strcpy(arg, ch->dest_buf);
    		DISPOSE( ch->dest_buf);
    		strcpy(arg2, ch->dest_buf_2);
    		DISPOSE( ch->dest_buf_2);
    		break;
    		
    	case SUB_TIMER_DO_ABORT:
    		DISPOSE( ch->dest_buf );
    		DISPOSE( ch->dest_buf_2 );
    		ch->substate = SUB_NONE;    		                                   
    	        send_to_char("&RTe distraes y no consigues terminar tu trabajo.\n\r", ch);
    	        return;
    }
    
    ch->substate = SUB_NONE;
    
    level = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makearmor]);
    
    checksew = FALSE;
    checkfab = FALSE;
    
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
       if (obj->item_type == ITEM_THREAD)
          checksew = TRUE;
       if (obj->item_type == ITEM_FABRIC && checkfab == FALSE)
       {
          checkfab = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          material = obj;
       }
    }                            
    
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makearmor]) ;
                
    if ( number_percent( ) > chance*2  || ( !checkfab ) || ( !checksew ) )
    {
       send_to_char( "&RTerminas tu armadura.\n\r", ch);
       send_to_char( "&RLa observas, algo no está bien...\n\r", ch);
       send_to_char( "&RLa armadura se desilacha. You quickly hide your mistake...\n\r", ch);
       learn_from_failure( ch, gsn_makearmor );
       return;
    }

    obj = material; 

    obj->item_type = ITEM_ARMOR;
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    value = get_wflag( arg );
    if ( value < 0 || value > 31 )
        SET_BIT( obj->wear_flags, ITEM_WEAR_BODY );                    
    else
        SET_BIT( obj->wear_flags, 1 << value );
    obj->level = level;
    STRFREE( obj->name );
    sprintf(buf, "%s ", arg2);
    strcat( buf, remand(buf));
    obj->name = STRALLOC( buf );
    strcpy( buf, arg2 );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );        
    STRFREE( obj->description );
    strcat( buf, " está aquí." );
    obj->description = STRALLOC( buf );
    obj->value[0] = obj->value[1];      
    obj->cost *= 10;
                                                                    
    obj = obj_to_char( obj, ch );
                                                            
    send_to_char( "&GFinalizas la costura de una nueva armadura.&w\n\r", ch);
    act( AT_PLAIN, "$n termihna de coser una armadura.", ch,
         NULL, argument , TO_ROOM );
    
    {
         long xpgain;
         
         xpgain = UMIN( obj->cost*100 ,( exp_level(ch->skill_level[ENGINEERING_ABILITY]+1) - exp_level(ch->skill_level[ENGINEERING_ABILITY]) ) );
         gain_exp(ch, xpgain, ENGINEERING_ABILITY);
         ch_printf( ch , "Recibes %d puntos de experiencia en ingeniero.", xpgain );
    }
        learn_from_success( ch, gsn_makearmor );
}


void do_makecomlink( CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int chance;
    bool checktool, checkgem, checkbatt, checkcirc; 
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;
            
    argument = one_argument( argument, arg );
    strcpy ( arg2, argument);
    
	switch( ch->substate )
    { 
    	default:
    	        
    	        if ( arg2[0] == '\0' )
                {
                  send_to_char( "&RSintaxis: fabricarenlacedecomunicaciones <Localización> <nombre>\n\r&w", ch);
				  send_to_char( "&RLocalizaciones disponibles: sostenido, munyeca, orejas&w\n\r", ch);
                  return;   
                }
 
    	        checktool = FALSE;
                checkgem  = FALSE;
                checkbatt = FALSE;
                checkcirc = FALSE;
        
                if ( !IS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
                {
                   send_to_char( "&RDebes estar en una fábrica o taller para hacer esto.\n\r", ch);
                   return;
                }
                
                for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
                {
                  if (obj->item_type == ITEM_TOOLKIT)
                    checktool = TRUE;
                  if (obj->item_type == ITEM_CRYSTAL)
          	    checkgem = TRUE;
                  if (obj->item_type == ITEM_BATTERY)
                  checkbatt = TRUE;
                  if (obj->item_type == ITEM_CIRCUIT)
                  checkcirc = TRUE;                  
                }
                
                if ( !checktool )
                {
                   send_to_char( "&RNecesitas una caja de herramientas.\n\r", ch);
                   return;
                }
 
                if ( !checkgem )
                {
                   send_to_char( "&RNecesitas un cristal pequeño.\n\r", ch);
                   return;
                }

                if ( !checkbatt )
                {
                   send_to_char( "&RNecesitas una batería.\n\r", ch);
                   return;
                }
                
                if ( !checkcirc )
                {
                   send_to_char( "&RNecesitas un circuito.\n\r", ch);
                   return;
                }
 
    	        chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_makecomlink]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GComienzas el largo proceso de crear un enlace de comunicación.\n\r", ch);
    		   act( AT_PLAIN, "$n comienza a trabajar en algo con su caja de herramientas.", ch,
		        NULL, argument , TO_ROOM );
		   add_timer ( ch , TIMER_DO_FUN , 10 , do_makecomlink , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   ch->dest_buf_2 = str_dup(arg2);
    		   return;
	        }
	        send_to_char("&RNo encuentras la forma de colocar las piezas juntas.\n\r",ch);
	        learn_from_failure( ch, gsn_makecomlink );
    	   	return;	
    	
    	case 1: 
    		if ( !ch->dest_buf )
    		     return;
    		strcpy(arg, ch->dest_buf);
    		DISPOSE( ch->dest_buf);
			strcpy(arg2, ch->dest_buf_2);
			DISPOSE( ch->dest_buf_2);
    		break;
    		
    	case SUB_TIMER_DO_ABORT:
    		DISPOSE( ch->dest_buf );
    		ch->substate = SUB_NONE;    		                                   
    	        send_to_char("&RTe distraes y no consigues terminar tu trabajo.\n\r", ch);
    	        return;
    }
    
    ch->substate = SUB_NONE;

    vnum = 64;
    
    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
         send_to_char( "&REl objeto que intentas crear no está en la base de datos.\n\rPlease inform the administration of this error.\n\r", ch );
         return;
    }

    checktool = FALSE;
    checkgem  = FALSE;
    checkbatt = FALSE;
    checkcirc = FALSE;
    
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
       if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;
       if (obj->item_type == ITEM_CRYSTAL && checkgem == FALSE)
       {
          checkgem = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
       }
       if (obj->item_type == ITEM_CIRCUIT && checkcirc == FALSE)
       {
          checkcirc = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
       }
       if (obj->item_type == ITEM_BATTERY && checkbatt == FALSE )
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkbatt = TRUE;
       }
    }                            
    
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makecomlink]) ;
                
    if ( number_percent( ) > chance*2  || ( !checktool ) || ( !checkcirc ) || ( !checkbatt ) || ( !checkgem ) )
    {
       send_to_char( "&RSostienes tu nuevo enlace de comunicaciones....\n\r", ch);
       send_to_char( "&RSe te cae de las manos y se hace pedazos.\n\r", ch);
       learn_from_failure( ch, gsn_makecomlink );
       return;
    }

    obj = create_object( pObjIndex, ch->top_level );
    
    obj->item_type = ITEM_COMLINK;
    if (arg==NULL || !str_cmp(arg, "hold" ))
	    SET_BIT( obj->wear_flags, ITEM_HOLD );
	if ( !str_cmp(arg, "orejas"))
	{
		SET_BIT( obj->wear_flags, ITEM_WEAR_EARS );
		REMOVE_BIT( obj->wear_flags, ITEM_HOLD);
	}
	if ( !str_cmp(arg, "munyeca"))
	{
		SET_BIT( obj->wear_flags, ITEM_WEAR_WRIST );
		REMOVE_BIT( obj->wear_flags, ITEM_HOLD);
	}
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    obj->weight = 3;
    STRFREE( obj->name );
    strcpy( buf, arg2 );
    strcat( buf, " comlink " );
    strcat( buf, remand(buf));
    obj->name = STRALLOC( buf );
    strcpy( buf, arg2 );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );        
    STRFREE( obj->description );
    strcat( buf, " está aquí." );
    obj->description = STRALLOC( buf );
    obj->cost = 50;
                                                                    
    obj = obj_to_char( obj, ch );
                                                            
    send_to_char( "&GFinalizas tu trabajo y admiras tu nuevo enlace de comunicaciones.&w\n\r", ch);
    act( AT_PLAIN, "$n termina su enlace de comunicaciones.", ch,
         NULL, argument , TO_ROOM );
    
    {
         long xpgain;
         
         xpgain = UMIN( obj->cost*100 ,( exp_level(ch->skill_level[ENGINEERING_ABILITY]+1) - exp_level(ch->skill_level[ENGINEERING_ABILITY]) ) );
         gain_exp(ch, xpgain, ENGINEERING_ABILITY);
         ch_printf( ch , "Recibes %d puntos de experiencia en ingeniero.", xpgain );
    }
        learn_from_success( ch, gsn_makecomlink );
     
}

void do_makeshield( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int chance;
    bool checktool, checkbatt, checkcond, checkcirc, checkgems;
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum, level, charge, gemtype;
    
    strcpy( arg, argument );    
    
    switch( ch->substate )
    { 
    	default:
    	        if ( arg[0] == '\0' )
                {
                  send_to_char( "&RSintaxis: fabricarescudo <nombre>\n\r&w", ch);
                  return;   
                }

    	        checktool = FALSE;
                checkbatt = FALSE;
                checkcond = FALSE;
                checkcirc = FALSE;
                checkgems = FALSE;

                if ( !IS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
                {
                   send_to_char( "&RNecesitas estar en una fábrica o taller para hacer esto.\n\r", ch);
                   return;
                }
                
                for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
                {
                  if (obj->item_type == ITEM_TOOLKIT)
                    checktool = TRUE;
                  if (obj->item_type == ITEM_CRYSTAL)
                    checkgems = TRUE;                    
                  if (obj->item_type == ITEM_BATTERY)
                    checkbatt = TRUE;
                  if (obj->item_type == ITEM_CIRCUIT)
                    checkcirc = TRUE;
                  if (obj->item_type == ITEM_SUPERCONDUCTOR)
                    checkcond = TRUE;                  
                }
                
                if ( !checktool )
                {
                   send_to_char( "&RNecesitas una caja de herramientas.\n\r", ch);
                   return;
                }
 
                if ( !checkbatt )
                {
                   send_to_char( "&RNecesitas una batería.\n\r", ch);
                   return;
                }
                
                if ( !checkcirc )
                {
                   send_to_char( "&RNecesitas un circuito.\n\r", ch);
                   return;
                }
                
                if ( !checkcond )
                {
                   send_to_char( "&RNecesitas un superconductor.\n\r", ch);
                   return;
                }
                
                if ( !checkgems )
                {
                   send_to_char( "&RNecesitas un cristal.\n\r", ch);
                   return;
                }
                
    	        chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_makeshield]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GComienzas el largo proceso de construir un escudo de energía.\n\r", ch);
    		   act( AT_PLAIN, "$n se pone a trabajar en algo con su caja de herramientas.", ch,
		        NULL, argument , TO_ROOM );
		   add_timer ( ch , TIMER_DO_FUN , 20 , do_makeshield , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   return;
	        }
	        send_to_char("&RNo consigues colocar las piezas juntas.\n\r",ch);
	        learn_from_failure( ch, gsn_makeshield );
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
    
    level = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makeshield]);
    vnum = 28;
    
    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
         send_to_char( "&REl objeto que tratas de crear no está en la base de datos.\n\rPor favor, notifícalo urgentemente a la administración.\n\r", ch );
         return;
    }

    checktool = FALSE;
    checkbatt = FALSE;
    checkcond = FALSE;
    checkcirc = FALSE;
    checkgems = FALSE;
    charge = 0;

    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
       if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;

       if (obj->item_type == ITEM_BATTERY && checkbatt == FALSE)
       {
          charge = UMIN(obj->value[1], 10);
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkbatt = TRUE;
       }
       if (obj->item_type == ITEM_SUPERCONDUCTOR && checkcond == FALSE)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkcond = TRUE;
       }
       if (obj->item_type == ITEM_CIRCUIT && checkcirc == FALSE)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkcirc = TRUE;
       }
       if (obj->item_type == ITEM_CRYSTAL && checkgems == FALSE)
       {
          gemtype = obj->value[0];
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkgems = TRUE;
       }
    }                            
    
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makeshield]) ;
                
    if ( number_percent( ) > chance*2  || ( !checktool ) || ( !checkbatt )
                                       || ( !checkgems ) || ( !checkcond ) || ( !checkcirc) )
    
    {
       send_to_char( "&RSostienes el escudo y accionas el interruptor...\n\r", ch);
       send_to_char( "&REn lugar de crearse el campo de energía, humo comienza a surgir del escudo.\n\r", ch);
       send_to_char( "&RSueltas el escudo que se hace pedazos en el suelo.\n\r", ch);
       learn_from_failure( ch, gsn_makeshield );
       return;
    }

    obj = create_object( pObjIndex, level );
    
    obj->item_type = ITEM_ARMOR;
    SET_BIT( obj->wear_flags, ITEM_WIELD );
    SET_BIT( obj->wear_flags, ITEM_WEAR_SHIELD );
    obj->level = level;
    obj->weight = 2;
    STRFREE( obj->name );
    obj->name = STRALLOC( "escudo energia" );
    strcpy( buf, arg );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );        
    STRFREE( obj->description );
    strcat( buf, " está aquí." );
    obj->description = STRALLOC( buf );
    obj->value[0] = (int) (level/10+gemtype*10);      /* condition */
    obj->value[1] = (int) (level/10+gemtype*10);      /* armor */
    obj->value[4] = charge;
    obj->value[5] = charge;
    obj->cost = obj->value[2]*100;
                                                                    
    obj = obj_to_char( obj, ch );
                                                            
    send_to_char( "&GFinalizas tu trabajo y admiras el nuevo escudo de energía.&w\n\r", ch);
    act( AT_PLAIN, "$n finaliza la construcción de un escudo de energía.", ch,
         NULL, argument , TO_ROOM );
    
    {
         long xpgain;
         
         xpgain = UMIN( obj->cost*50 ,( exp_level(ch->skill_level[ENGINEERING_ABILITY]+1) - exp_level(ch->skill_level[ENGINEERING_ABILITY]) ) );
         gain_exp(ch, xpgain, ENGINEERING_ABILITY);
         ch_printf( ch , "Recibes %d puntos de experiencia en ingeniero.", xpgain );
    }
        learn_from_success( ch, gsn_makeshield );

}

void do_makecontainer( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int level, chance;
    bool checksew, checkfab; 
    OBJ_DATA *obj;
    OBJ_DATA *material;
    int value;
            
    argument = one_argument( argument, arg );
    strcpy( arg2 , argument );
    
    if ( !str_cmp( arg, "ojos" )
    || !str_cmp( arg, "orejas" )
    || !str_cmp( arg, "dedo" )
    || !str_cmp( arg, "cuello" )
    || !str_cmp( arg, "muñeca" ) )
    {
        send_to_char( "&RNo puedes fabricar un contenedor para esas partes del cuerpo.\n\r&w", ch);
        send_to_char( "&RPrueba fabricarjoya.\n\r&w", ch);
        return;
    }
    if ( !str_cmp( arg, "pies" ) 
    || !str_cmp( arg, "manos" ) )
    {
        send_to_char( "&RNo puedes fabricar un contenedor para esa parte del cuerpo.\n\r&w", ch);
        send_to_char( "&RPrueba fabricararmadura.\n\r&w", ch);
        return;
    }
    if ( !str_cmp( arg, "escudo" ) )
    {
        send_to_char( "&RNo puedes hacer un escudo contenedor.\n\r&w", ch);
        send_to_char( "&RPrueba fabricarescudo.\n\r&w", ch);
        return;
    }
    if ( !str_cmp( arg, "blandir" ) )
    {
        send_to_char( "&R¿Planeas luchar con un contenedor?\n\r&w", ch);
        send_to_char( "&RPrueba fabricarhoja...\n\r&w", ch);
        return;
    }
    
    switch( ch->substate )
    { 
    	default:
    	        
    	        if ( arg2[0] == '\0' )
                {
                  send_to_char( "&RUso: fabricarcontenedor <parte del cuerpo> <nombre>\n\r&w", ch);
                  return;   
                }
 
    	        checksew = FALSE;
                checkfab = FALSE;
        
                if ( !IS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
                {
                   send_to_char( "&RNecesitas estár en una fábrica o taller para hacer esto.\n\r", ch);
                   return;
                }
                
                for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
                {
                  if (obj->item_type == ITEM_FABRIC)
                    checkfab = TRUE;
                  if (obj->item_type == ITEM_THREAD)
          	    checksew = TRUE;
                }
                
                if ( !checkfab )
                {
                   send_to_char( "&RNecesitas algún tipo de tela o material.\n\r", ch);
                   return;
                }
 
                if ( !checksew )
                {
                   send_to_char( "&RNecesitas aguja e hilo.\n\r", ch);
                   return;
                }

    	        chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_makecontainer]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&cComienzas el largo proceso de crear una bolsa.\n\r", ch);
    		   act( AT_PLAIN, "$n saca su kit de costura y algo de material y comienza a trabajar.", ch,
		        NULL, argument , TO_ROOM );
		   add_timer ( ch , TIMER_DO_FUN , 10 , do_makecontainer , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   ch->dest_buf_2 = str_dup(arg2);
    		   return;
	        }
	        send_to_char("&RNo consigues hacer nada útil.\n\r",ch);
	        learn_from_failure( ch, gsn_makecontainer );
    	   	return;	
    	
    	case 1: 
    		if ( !ch->dest_buf )
    		     return;
    		if ( !ch->dest_buf_2 )
    		     return;
    		strcpy(arg, ch->dest_buf);
    		DISPOSE( ch->dest_buf);
    		strcpy(arg2, ch->dest_buf_2);
    		DISPOSE( ch->dest_buf_2);
    		break;
    		
    	case SUB_TIMER_DO_ABORT:
    		DISPOSE( ch->dest_buf );
    		DISPOSE( ch->dest_buf_2 );
    		ch->substate = SUB_NONE;    		                                   
    	        send_to_char("&RTe distraes y abandonas tu trabajo.\n\r", ch);
    	        return;
    }
    
    ch->substate = SUB_NONE;
    
    level = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makecontainer]);
    
    checksew = FALSE;
    checkfab = FALSE;
    
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
       if (obj->item_type == ITEM_THREAD)
          checksew = TRUE;
       if (obj->item_type == ITEM_FABRIC && checkfab == FALSE)
       {
          checkfab = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          material = obj;
       }
    }                            
    
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makecontainer]) ;
                
    if ( number_percent( ) > chance*2  || ( !checkfab ) || ( !checksew ) )
    {
       send_to_char( "&RSostienes tu nuevo contenedor.\n\r", ch);
       send_to_char( "&RDe pronto te das cuenta de que se desmonta\n\r", ch);
       send_to_char( "&Ry destruye. Deberías ocultar tu error...\n\r", ch);
       learn_from_failure( ch, gsn_makecontainer );
       return;
    }

    obj = material; 

    obj->item_type = ITEM_CONTAINER;
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    value = get_wflag( arg );
    if ( value < 0 || value > 31 )
        SET_BIT( obj->wear_flags, ITEM_HOLD );                    
    else
        SET_BIT( obj->wear_flags, 1 << value );
    obj->level = level;
    STRFREE( obj->name );
    strcpy( buf, arg2 );
    strcat( buf, " ");
    strcat( buf, remand(buf));
    obj->name = STRALLOC( buf );
    strcpy( buf, arg2 );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );        
    STRFREE( obj->description );
    strcat( buf, " está aquí." );
    obj->description = STRALLOC( buf );
    obj->value[0] = level;
    obj->value[1] = 0;
    obj->value[2] = 0;      
    obj->value[3] = 10;      
    obj->cost *= 2;
                                                                    
    obj = obj_to_char( obj, ch );
                                                            
    send_to_char( "&GFinalizas tu trabajo y admiras tu nuevo contenedor.&w\n\r", ch);
    act( AT_PLAIN, "$n termina de construir un contenedor.", ch,
         NULL, argument , TO_ROOM );
    
    {
         long xpgain;
         
         xpgain = UMIN( obj->cost*100 ,( exp_level(ch->skill_level[ENGINEERING_ABILITY]+1) - exp_level(ch->skill_level[ENGINEERING_ABILITY]) ) );
         gain_exp(ch, xpgain, ENGINEERING_ABILITY);
         ch_printf( ch , "Recibes %d puntos de experiencia en ingeniero.\n\r", xpgain );
    }
        learn_from_success( ch, gsn_makecontainer );
}

void do_gemcutting( CHAR_DATA *ch, char *argument )
{
     send_to_char( "&RSorry, this skill isn't finished yet :(\n\r", ch);
}

void do_reinforcements( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int chance, credits;
    
    if ( IS_NPC( ch ) || !ch->pcdata )
    	return;
    	
    strcpy( arg, argument );    
    
    switch( ch->substate )
    { 
    	default:
    	        if ( ch->backup_wait )
    	        {
    	            send_to_char( "&RYour reinforcements are already on the way.\n\r", ch );
    	            return;
    	        }
    	        
    	        if ( !ch->pcdata->clan )
    	        {
    	            send_to_char( "&RYou need to be a member of an organization before you can call for reinforcements.\n\r", ch );
    	            return;
    	        }    
    	        
    	        if ( ch->gold < ch->skill_level[POLITICIAN_ABILITY] * 50 )
    	        {
    	            ch_printf( ch, "&RYou dont have enough credits to send for reinforcements.\n\r" );
    	            return;
    	        }    
    	        
    	        chance = (int) (ch->pcdata->learned[gsn_reinforcements]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GYou begin making the call for reinforcements.\n\r", ch);
    		   act( AT_PLAIN, "$n begins issuing orders int $s comlink.", ch,
		        NULL, argument , TO_ROOM );
		   add_timer ( ch , TIMER_DO_FUN , 1 , do_reinforcements , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   return;
	        }
	        send_to_char("&RYou call for reinforcements but nobody answers.\n\r",ch);
	        learn_from_failure( ch, gsn_reinforcements );
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
    	        send_to_char("&RYou are interupted before you can finish your call.\n\r", ch);
    	        return;
    }
    
    ch->substate = SUB_NONE;
    
    send_to_char( "&GYour reinforcements are on the way.\n\r", ch);
    credits = ch->skill_level[POLITICIAN_ABILITY] * 50;
    ch_printf( ch, "It cost you %d credits.\n\r", credits);
    ch->gold -= UMIN( credits , ch->gold );
             
    learn_from_success( ch, gsn_reinforcements );
    
    if ( nifty_is_name( "empire" , ch->pcdata->clan->name ) )
       ch->backup_mob = MOB_VNUM_STORMTROOPER;
    else if ( nifty_is_name( "republic" , ch->pcdata->clan->name ) )
       ch->backup_mob = MOB_VNUM_NR_TROOPER;
    else
       ch->backup_mob = MOB_VNUM_MERCINARY;

    ch->backup_wait = number_range(1,2);
    
}

void do_postguard( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int chance, credits;
    
    if ( IS_NPC( ch ) || !ch->pcdata )
    	return;
    	
    strcpy( arg, argument );    
    
    switch( ch->substate )
    { 
    	default:
    	        if ( ch->backup_wait )
    	        {
    	            send_to_char( "&RYou already have backup coming.\n\r", ch );
    	            return;
    	        }
    	        
    	        if ( !ch->pcdata->clan )
    	        {
    	            send_to_char( "&RYou need to be a member of an organization before you can call for a guard.\n\r", ch );
    	            return;
    	        }    
    	        
    	        if ( ch->gold < ch->skill_level[POLITICIAN_ABILITY] * 30 )
    	        {
    	            ch_printf( ch, "&RYou dont have enough credits.\n\r", ch );
    	            return;
    	        }    
    	        
    	        chance = (int) (ch->pcdata->learned[gsn_postguard]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GYou begin making the call for reinforcements.\n\r", ch);
    		   act( AT_PLAIN, "$n begins issuing orders int $s comlink.", ch,
		        NULL, argument , TO_ROOM );
		   add_timer ( ch , TIMER_DO_FUN , 1 , do_postguard , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   return;
	        }
	        send_to_char("&RYou call for a guard but nobody answers.\n\r",ch);
	        learn_from_failure( ch, gsn_postguard );
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
    	        send_to_char("&RYou are interupted before you can finish your call.\n\r", ch);
    	        return;
    }
    
    ch->substate = SUB_NONE;
    
    send_to_char( "&GYour guard is on the way.\n\r", ch);
    
    credits = ch->skill_level[POLITICIAN_ABILITY] * 30;
    ch_printf( ch, "It cost you %d credits.\n\r", credits);
    ch->gold -= UMIN( credits , ch->gold );

    learn_from_success( ch, gsn_postguard );
    
    if ( nifty_is_name( "empire" , ch->pcdata->clan->name ) )
       ch->backup_mob = MOB_VNUM_IMP_GUARD;
    else if ( nifty_is_name( "republic" , ch->pcdata->clan->name ) )
       ch->backup_mob = MOB_VNUM_NR_GUARD;
    else
       ch->backup_mob = MOB_VNUM_BOUNCER;

    ch->backup_wait = 1;

}

void add_reinforcements( CHAR_DATA *ch )
{
     MOB_INDEX_DATA  * pMobIndex;
     OBJ_DATA        * blaster;
     OBJ_INDEX_DATA  * pObjIndex;
     int max=1;
     
     if ( ( pMobIndex = get_mob_index( ch->backup_mob ) ) == NULL )
        return;         

     if ( ch->backup_mob == MOB_VNUM_STORMTROOPER ||
          ch->backup_mob == MOB_VNUM_NR_TROOPER   ||
          ch->backup_mob == MOB_VNUM_MERCINARY    ||
       	  ch->backup_mob == MOB_VNUM_IMP_FORCES   ||
	  ch->backup_mob == MOB_VNUM_NR_FORCES	  ||
      	  ch->backup_mob == MOB_VNUM_MERC_FORCES )
     {
        CHAR_DATA * mob[3];
        int         mob_cnt;
        
        send_to_char( "Your reinforcements have arrived.\n\r", ch );
        if( ch->backup_mob == MOB_VNUM_STORMTROOPER || ch->backup_mob ==  
	    MOB_VNUM_NR_TROOPER || ch->backup_mob == MOB_VNUM_MERCINARY )
          max=3;
        for ( mob_cnt = 0 ; mob_cnt < max ; mob_cnt++ )
        {
            int ability;
            mob[mob_cnt] = create_mobile( pMobIndex );
            char_to_room( mob[mob_cnt], ch->in_room );
            act( AT_IMMORT, "$N has arrived.", ch, NULL, mob[mob_cnt], TO_ROOM );
            mob[mob_cnt]->top_level = ch->skill_level[POLITICIAN_ABILITY]/3;
            for ( ability = 0 ; ability < MAX_ABILITY ; ability++ )
                     mob[mob_cnt]->skill_level[ability] = mob[mob_cnt]->top_level;
            mob[mob_cnt]->hit = mob[mob_cnt]->top_level*15;
            mob[mob_cnt]->max_hit = mob[mob_cnt]->hit;
            mob[mob_cnt]->armor = 100- mob[mob_cnt]->top_level*2.5;
            mob[mob_cnt]->damroll = mob[mob_cnt]->top_level/5;
            mob[mob_cnt]->hitroll = mob[mob_cnt]->top_level/5;
            if ( ( pObjIndex = get_obj_index( OBJ_VNUM_BLASTECH_E11 ) ) != NULL )
            {
                 blaster = create_object( pObjIndex, mob[mob_cnt]->top_level );
                 obj_to_char( blaster, mob[mob_cnt] );
                 equip_char( mob[mob_cnt], blaster, WEAR_WIELD );                        
            } 
            if ( mob[mob_cnt]->master )
	       stop_follower( mob[mob_cnt] );
    	    add_follower( mob[mob_cnt], ch );
            SET_BIT( mob[mob_cnt]->affected_by, AFF_CHARM );
            do_setblaster( mob[mob_cnt] , "full" );
        }
     }
     else
     {
        CHAR_DATA *mob;
        int ability;
        
        mob = create_mobile( pMobIndex );
        char_to_room( mob, ch->in_room );
        if ( ch->pcdata && ch->pcdata->clan )
        {
          char tmpbuf[MAX_STRING_LENGTH];
        
          //STRFREE( mob->name );
          //mob->name = STRALLOC( ch->pcdata->clan->name );
          sprintf( tmpbuf , "(%s) %s" , ch->pcdata->clan->name  , mob->long_descr );
          STRFREE( mob->long_descr );
          mob->long_descr = STRALLOC( tmpbuf );
        }
        act( AT_IMMORT, "$N has arrived.", ch, NULL, mob, TO_ROOM );
        send_to_char( "Your guard has arrived.\n\r", ch );
        mob->top_level = UMIN(ch->skill_level[POLITICIAN_ABILITY], 30);
        for ( ability = 0 ; ability < MAX_ABILITY ; ability++ )
            mob->skill_level[ability] = mob->top_level;
        mob->hit = mob->top_level*15;
        mob->max_hit = mob->hit;
        mob->armor = 100- mob->top_level*2.5;
        mob->damroll = mob->top_level/5;
        mob->hitroll = mob->top_level/5;
        if ( ( pObjIndex = get_obj_index( OBJ_VNUM_BLASTECH_E11 ) ) != NULL )
        {
            blaster = create_object( pObjIndex, mob->top_level );
            obj_to_char( blaster, mob );
            equip_char( mob, blaster, WEAR_WIELD );                        
        }
        
        /* for making this more accurate in the future */
        
        if ( mob->mob_clan )
           STRFREE ( mob->mob_clan );
        if ( ch->pcdata && ch->pcdata->clan )   
           mob->mob_clan = STRALLOC( ch->pcdata->clan->name );
     }                    
}

void do_torture( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance, dam;
    bool fail;
    
    if ( !IS_NPC(ch)
    &&  ch->pcdata->learned[gsn_torture] <= 0  )
    {
	send_to_char( "No sabrías cómo hacer esto.\n\r", ch );
	return;
    }

    if ( IS_NPC(ch) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "No puedes hacer esto.\n\r", ch );
	return;
    }

    one_argument( argument, arg );

    if ( ch->mount )
    {
	send_to_char( "Desmonta primero.\n\r", ch );
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "¿Torturar a quién?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "¿eres masoquista?\n\r", ch );
	return;
    }
    
    if ( !IS_AWAKE(victim) )
    {
	send_to_char( "Despiértale primero.\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
      return;

    if ( victim->fighting )
    {
	send_to_char( "Espera a que termine de luchar.\n\r", ch );
	return;
    }
    
    ch->alignment = ch->alignment -= 100;
    ch->alignment = URANGE( -1000, ch->alignment, 1000 );
    
    WAIT_STATE( ch, skill_table[gsn_torture]->beats );
    
    fail = FALSE;
    chance = ris_save( victim, ch->skill_level[POLITICIAN_ABILITY], RIS_PARALYSIS );
    if ( chance == 1000 )
      fail = TRUE;
    else
      fail = saves_para_petri( chance, victim );

    if ( !IS_NPC(ch) && !IS_NPC(victim) )
      chance = sysdata.stun_plr_vs_plr;
    else
      chance = sysdata.stun_regular;
    if ( ( !fail
    && (  IS_NPC(ch)
    || (number_percent( ) + chance) < ch->pcdata->learned[gsn_torture] +20 ) ) || ((!IS_NPC(victim)) && (IS_SET(victim->pcdata->act2, ACT_BOUND))))
    {
	learn_from_success( ch, gsn_torture );
	WAIT_STATE( ch,     2 * PULSE_VIOLENCE );
	WAIT_STATE( victim, PULSE_VIOLENCE );
	act( AT_SKILL, "$N te tortura lentamente. el dolor es insoportable.", victim, NULL, ch, TO_CHAR );
	act( AT_SKILL, "Torturas a $N, haciéndole gritar de dolor.", ch, NULL, victim, TO_CHAR );
	act( AT_SKILL, "$n tortures $N, leaving $M screaming in agony!", ch, NULL, victim, TO_NOTVICT );
        
        dam = dice( ch->skill_level[POLITICIAN_ABILITY]/10 , 4 );
        dam = URANGE( 0, victim->max_hit-10, dam ); 
        victim->hit -= dam;
        victim->max_hit -= dam;
        
        ch_printf( victim, "Pierdes %d puntos de vida permanentemente." ,dam);
        ch_printf( ch, "Pierde %d puntos de vida permanentemente." , dam);
         
    }
    else
    {
	act( AT_SKILL, "¡$N te intenta cortar un dedo!", victim, NULL, ch, TO_CHAR );
	act( AT_SKILL, "No consigues nada productivo.", ch, NULL, victim, TO_CHAR );
	act( AT_SKILL, "$n intenta corturar a $N.", ch, NULL, victim, TO_NOTVICT );
	WAIT_STATE( ch,     2 * PULSE_VIOLENCE );
        global_retcode = multi_hit( victim, ch, TYPE_UNDEFINED );
    }
    return;
    
}

void do_disguise( CHAR_DATA *ch, char *argument )
{
    int chance;

    if ( IS_NPC(ch) )
	return;

    if ( IS_SET( ch->pcdata->flags, PCFLAG_NOTITLE ))
    {
        send_to_char( "La fuerza te lo impide.\n\r", ch );
        return;
    }

    if ( argument[0] == '\0' )
    {
	send_to_char( "Sintaxis: disfrazarse <disfraz|limpiar>\n\r", ch );
	return;
    }
    
    if ( !str_cmp(argument, "limpiar") )
    {
        STRFREE(ch->pcdata->disguise);
        ch->pcdata->disguise = STRALLOC( "" );
	send_to_char( "Te despojas del disfraz.\n\r", ch);
	return;
    }

    chance = (int) (ch->pcdata->learned[gsn_disguise]);
    
    if ( number_percent( ) > chance )
    {
        send_to_char( "Fallas.\n\r", ch );
        return;
    }

    if(strlen(argument) > 40)
      argument[40] = '\0';
                
    learn_from_success(ch, gsn_disguise);
    
    smash_tilde( argument );

    STRFREE( ch->pcdata->disguise );
    ch->pcdata->disguise = STRALLOC( argument );
    send_to_char( "Vale.\n\r", ch );
}


void do_deception( CHAR_DATA *ch, char *argument )
{
    int chance;

    if ( IS_NPC(ch) )
	return;

    if ( IS_SET( ch->pcdata->flags, PCFLAG_NOTITLE ))
    {
        send_to_char( "Lo intentas pero la fuerza te lo impide.\n\r", ch );
        return;
    }

    if ( argument[0] == '\0' )
    {
	send_to_char( "Sintaxis: enganyar <nombre|limpiar>\n\r", ch );
	return;
    }
    
    if ( !str_cmp(argument, "limpiar") )
    {
        STRFREE(ch->pcdata->disguise);
        ch->pcdata->disguise = STRALLOC( "" );
	send_to_char( "Dejas de engañar a la gente.\n\r", ch);
	return;
    }

    chance = (int) (ch->pcdata->learned[gsn_deception]);
    
    if ( number_percent( ) > chance )
    {
        send_to_char( "Fallas.\n\r", ch );
        return;
    }                

    if(strlen(argument) > 40)
      argument[40] = '\0';
                
    learn_from_success(ch, gsn_deception);
    
    smash_tilde( argument );

    STRFREE( ch->pcdata->disguise );
    ch->pcdata->disguise = STRALLOC( argument );

    send_to_char( "Vale.\n\r", ch );
}

void do_mine( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    bool shovel;
    sh_int move;

   if ( ch->pcdata->learned[gsn_mine] <= 0 )
    {
      ch_printf( ch, "No tienes ni idea de cómo hacer eso.\n\r" );
      return;
    }

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {    
        send_to_char( "¿Planeas usar tus propias manos como palas?\n\r", ch );
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
        send_to_char( "No ves nada aquí.\n\r", ch );
        return;
    }

    separate_obj(obj);
    if ( obj->item_type != ITEM_LANDMINE )
    {
	act( AT_PLAIN, "¡Eso no es una mina!", ch, obj, 0, TO_CHAR );
        return;
    }

    if ( !CAN_WEAR(obj, ITEM_TAKE) )
    {
	act( AT_PLAIN, "No puedes enterrar $p.", ch, obj, 0, TO_CHAR );
        return;
    }
  
    switch( ch->in_room->sector_type )
    {
	case SECT_CITY:
	case SECT_INSIDE:
	    send_to_char( "El suelo está demasiado duro para cavar en él.\n\r", ch );
	    return;
	case SECT_WATER_SWIM:
	case SECT_WATER_NOSWIM:
	case SECT_UNDERWATER:
	    send_to_char( "No puedes poner una mina en el agua.\n\r", ch );
	    return;
	case SECT_AIR:
	    send_to_char( "¿Qué? ¿Poner una mina en el aire?!\n\r", ch );
	    return;
    }

    if ( obj->weight > (UMAX(5, (can_carry_w(ch) / 10)))
    &&  !shovel )
    {
	send_to_char( "Necesitas una pala.\n\r", ch );
	return;
    }
    
    move = (obj->weight * 50 * (shovel ? 1 : 5)) / UMAX(1, can_carry_w(ch));
    move = URANGE( 2, move, 1000 );
    if ( move > ch->move )
    {
	send_to_char( "El cansancio te lo impide.\n\r", ch );
	return;
    }
    ch->move -= move;
    
    SET_BIT( obj->extra_flags, ITEM_BURRIED );
    WAIT_STATE( ch, URANGE( 10, move / 2, 100 ) );
       
    STRFREE ( obj->armed_by ); 
    obj->armed_by = STRALLOC ( ch->name );

    ch_printf( ch, "Armas y entierras %s.\n\r", obj->short_descr );
    act( AT_PLAIN, "$n arma y entierra $p.", ch, obj, NULL, TO_ROOM );

    learn_from_success( ch, gsn_mine );
    
    return;
}

void do_first_aid( CHAR_DATA *ch, char *argument )
{
   OBJ_DATA   *medpac;
   CHAR_DATA  *victim;
   int         heal;
   char        buf[MAX_STRING_LENGTH];

   if ( ch->position == POS_FIGHTING )
   {
         send_to_char( "¡Termina el combate primero!\n\r",ch );
         return;
   }
   
   medpac = get_eq_char( ch, WEAR_HOLD );
   if ( !medpac || medpac->item_type != ITEM_MEDPAC )
   {
         send_to_char( "Debes sostener un medpac.\n\r",ch );
         return;
   }  

   if ( IS_SET(ch->in_room->room_flags2, ROOM_ARENA) )
   {
	send_to_char("¡Eso no sería honorable!\n\r", ch);
	return;
   }

   if ( medpac->value[0] <= 0 )
   {
         send_to_char( "Tu medpac parece estar vacío.\n\r",ch );
         return;         
   }
   
   if ( argument[0] == '\0' )
      victim = ch;
   else
      victim = get_char_room( ch, argument );
            
   if ( !victim )
   {
       ch_printf( ch, "No veo a %s aquí...\n\r" , argument );     
       return;
   }
   
   heal = number_range( 1, 150 );
   
   if ( heal > ch->pcdata->learned[gsn_first_aid]*2 )
   {
       ch_printf( ch, "Fallaste.\n\r");     
       learn_from_failure( ch , gsn_first_aid );
       return;
   } 
      
   if ( victim == ch )
   {
       ch_printf( ch, "Tratas tus heridas.\n\r");
       sprintf( buf , "$n usa %s para tratar sus heridas." , medpac->short_descr );        
       act( AT_ACTION, buf, ch, NULL, victim, TO_ROOM );  
   }
   else
   {
       sprintf( buf , "Tratas las heridas de $N." );        
       act( AT_ACTION, buf, ch, NULL, victim, TO_CHAR );  
       sprintf( buf , "$n usa %s para tratar las heridas de $N." , medpac->short_descr );        
       act( AT_ACTION, buf, ch, NULL, victim, TO_NOTVICT );  
       sprintf( buf , "$n usa %s para tratarte las heridas." , medpac->short_descr );        
       act( AT_ACTION, buf, ch, NULL, victim, TO_VICT );  
   }

   --medpac->value[0];
   victim->hit += URANGE ( 0, heal , victim->max_hit - victim->hit );

   learn_from_success( ch , gsn_first_aid );
}

void do_snipe( CHAR_DATA *ch, char *argument )
{
   OBJ_DATA        * wield;
   char              arg[MAX_INPUT_LENGTH];  
   char              arg2[MAX_INPUT_LENGTH];
   sh_int            dir, dist;
   sh_int            max_dist = 3;
   EXIT_DATA       * pexit;
   ROOM_INDEX_DATA * was_in_room;
   ROOM_INDEX_DATA * to_room;
   CHAR_DATA       * victim;
   int               chance;
   char              buf[MAX_STRING_LENGTH];
   bool              pfound = FALSE;
   
   if ( IS_SET( ch->in_room->room_flags, ROOM_SAFE ) )
    {
	set_char_color( AT_MAGIC, ch );
	send_to_char( "No puedes hacer eso aquí.\n\r", ch );
	return;
    }
     
   if ( get_eq_char( ch, WEAR_DUAL_WIELD ) != NULL )
   {
         send_to_char( "No puedes hacer eso empuñando dos armas.",ch );
         return;
   }
    
   wield = get_eq_char( ch, WEAR_WIELD );
   if ( !wield || wield->item_type != ITEM_WEAPON || wield->value[3] != WEAPON_BLASTER )
   {
         send_to_char( "No estás empuñando un bláster.",ch );
         return;
   }  

   argument = one_argument( argument, arg );
   argument = one_argument( argument, arg2 );
   
   if ( ( dir = get_door( arg ) ) == -1 || arg2[0] == '\0' )
   {
     send_to_char( "Sintaxis: disparar <dir> <objetivo>\n\r", ch );
     return;
   }
 
   if ( ( pexit = get_exit( ch->in_room, dir ) ) == NULL )
   {
     send_to_char( "¿Esperas dispararle a una pared?\n\r", ch );
     return;
   }

   if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
   {
     send_to_char( "¿Esperas dispararle a una puerta?\n\r", ch );
     return;
   }

   was_in_room = ch->in_room;
   
   for ( dist = 0; dist <= max_dist; dist++ )   
   {
     if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
        break; 
     
     if ( !pexit->to_room )
        break;
     
    to_room = NULL;
    if ( pexit->distance > 1 )
       to_room = generate_exit( ch->in_room , &pexit );
    
    if ( to_room == NULL )
       to_room = pexit->to_room;
    
     char_from_room( ch );
     char_to_room( ch, to_room );    
     

     if ( IS_NPC(ch) && ( victim = get_char_room_mp( ch, arg2 ) ) != NULL )
     {
        pfound = TRUE;
        break;
     }
     else if ( !IS_NPC(ch) && ( victim = get_char_room( ch, arg2 ) ) != NULL )
     {
        pfound = TRUE;
        break;
     }


     if ( ( pexit = get_exit( ch->in_room, dir ) ) == NULL )
        break;
            
   }
   
   char_from_room( ch );
   char_to_room( ch, was_in_room );    
       
   if ( !pfound )
   {
       ch_printf( ch, "¡No ves a nadie con ese nombre hacia %s!\n\r", dir_name3[dir] );
       char_from_room( ch );
       char_to_room( ch, was_in_room );    
       return;
   }
   
    if ( victim == ch )
    {
	send_to_char( "¿en serio?\n\r", ch );
	return;
    }
    
    if ( IS_SET( victim->in_room->room_flags, ROOM_SAFE ) )
    {
	set_char_color( AT_MAGIC, ch );
	send_to_char( "No puedes disparar hacia allí.\n\r", ch );
	return;
    }
 
    if ( is_safe( ch, victim ) )
	return;
    
    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
        act( AT_PLAIN, "No se te ocurriría hacerle eso a $N.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "¡Lo haces lo mejor que puedes!\n\r", ch );
	return;
    }
    
    if ( !IS_NPC( victim ) && IS_SET( ch->act, PLR_NICE ) )
    {
      send_to_char( "Te sientes tan bien...\n\r", ch );
      return;
    }
    
    chance = IS_NPC(ch) ? 100
           : (int)  (ch->pcdata->learned[gsn_snipe]) ; 
    
    switch ( dir )
    {
        case 0:
        case 1:
           dir += 2;
           break;
        case 2:
        case 3:
           dir -= 2;
           break;
        case 4:
        case 7:
           dir += 1;
           break;
        case 5:
        case 8:
           dir -= 1;
           break;
        case 6:
           dir += 3;
           break;
        case 9:
           dir -=3;
           break;
    }
    
    char_from_room( ch );
    char_to_room( ch, victim->in_room );    
                
    if ( number_percent() < chance )
    {                         
       sprintf( buf , "Te disparan con un bláster desde %s." , dir_name3[dir] );
       act( AT_ACTION, buf , victim, NULL, ch, TO_CHAR );      
       act( AT_ACTION, "Disparas a $N.", ch, NULL, victim, TO_CHAR );         
       sprintf( buf, "Disparan a $N con un bláster desde %s." , dir_name[dir] );
       act( AT_ACTION, buf, ch, NULL, victim, TO_NOTVICT );  
                                                   
       one_hit( ch, victim, TYPE_UNDEFINED );  
       
       if ( char_died(ch) ) 
          return;
          
       stop_fighting( ch , TRUE );
       
       learn_from_success( ch, gsn_snipe );              
    }
    else
    {
       act( AT_ACTION, "Disparas a $N pero fallas horriblemente.", ch, NULL, victim, TO_CHAR );         
       sprintf( buf, "Te disparan con un bláster desde %s pero fallan." , dir_name[dir] );
       act( AT_ACTION, buf, ch, NULL, victim, TO_ROOM );  
       learn_from_failure( ch, gsn_snipe );
    }
            
    char_from_room( ch );
    char_to_room( ch, was_in_room );    
     
   if ( IS_NPC(ch) )                               
      WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
   else
   {
   	if ( number_percent() < ch->pcdata->learned[gsn_third_attack] )
   	     WAIT_STATE( ch, 1 * PULSE_PER_SECOND );
   	else if ( number_percent() < ch->pcdata->learned[gsn_second_attack] )
   	     WAIT_STATE( ch, 2 * PULSE_PER_SECOND );
   	else 
   	     WAIT_STATE( ch, 3 * PULSE_PER_SECOND );
   }
   if ( IS_NPC( victim ) && !char_died(victim) )
   {
      if ( IS_SET( victim->act , ACT_SENTINEL ) )
      {
         victim->was_sentinel = victim->in_room;
         REMOVE_BIT( victim->act, ACT_SENTINEL );
      }
      
      start_hating( victim , ch );
      start_hunting( victim, ch );
     
   } 
   
}

/* syntax throw <obj> [direction] [target] */

void do_throw( CHAR_DATA *ch, char *argument )
{
   OBJ_DATA        * obj;
   OBJ_DATA        * tmpobj;
   char              arg[MAX_INPUT_LENGTH];  
   char              arg2[MAX_INPUT_LENGTH];
   char              arg3[MAX_INPUT_LENGTH];
   sh_int            dir;
   SHIP_DATA	   * ship;
   EXIT_DATA       * pexit;
   ROOM_INDEX_DATA * was_in_room;
   ROOM_INDEX_DATA * to_room;
   CHAR_DATA       * victim;
   char              buf[MAX_STRING_LENGTH];


   argument = one_argument( argument, arg );
   argument = one_argument( argument, arg2 );
   argument = one_argument( argument, arg3 );
   
   was_in_room = ch->in_room;
      
   if ( arg[0] == '\0' )
   {
     send_to_char( "Sintaxis: lanzar <objeto> [dirección] [objetivo]\n\r", ch );
     return;
   }

     
   obj = get_eq_char( ch, WEAR_MISSILE_WIELD );
   if ( !obj || !nifty_is_name( arg, obj->name ) )
      obj = get_eq_char( ch, WEAR_HOLD );
      if ( !obj || !nifty_is_name( arg, obj->name ) )
          obj = get_eq_char( ch, WEAR_WIELD );
          if ( !obj || !nifty_is_name( arg, obj->name ) )
              obj = get_eq_char( ch, WEAR_DUAL_WIELD );
              if ( !obj || !nifty_is_name( arg, obj->name ) )
   if ( !obj || !nifty_is_name_prefix( arg, obj->name ) )
      obj = get_eq_char( ch, WEAR_HOLD );
      if ( !obj || !nifty_is_name_prefix( arg, obj->name ) )
          obj = get_eq_char( ch, WEAR_WIELD );
          if ( !obj || !nifty_is_name_prefix( arg, obj->name ) )
              obj = get_eq_char( ch, WEAR_DUAL_WIELD );
              if ( !obj || !nifty_is_name_prefix( arg, obj->name ) )
   {
         ch_printf( ch, "No estás sosteniendo o empuñando %s.\n\r", arg );
         return;
   }  

    if ( IS_OBJ_STAT(obj, ITEM_NOREMOVE) )
    {
	act( AT_PLAIN, "No puedes lanzar $p.", ch, obj, NULL, TO_CHAR );
	return;
    }

   if ( ch->position == POS_FIGHTING )
   {
       victim = who_fighting( ch );
       if ( char_died ( victim ) )
           return;
       act( AT_ACTION, "Lanzas $p a $N.", ch, obj, victim, TO_CHAR );
       act( AT_ACTION, "$n lanza $p a $N.", ch, obj, victim, TO_NOTVICT );
       act( AT_ACTION, "$n te lanza $p.", ch, obj, victim, TO_VICT );        
   }
   else if ( arg2[0] == '\0' )
   {
       sprintf( buf, "$n lanza %s al suelo." , obj->short_descr );
       act( AT_ACTION, buf, ch, NULL, NULL, TO_ROOM );  
       ch_printf( ch, "Lanzas %s al suelo.\n\r", obj->short_descr );
       
       victim = NULL;
   }
   else  if ( ( dir = get_door( arg2 ) ) != -1 )
   {
      if ( ( pexit = get_exit( ch->in_room, dir ) ) == NULL )
      {
         send_to_char( "¿Estás intentando atravesar la pared?\n\r", ch );
         return;
      }

      
      if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
      {
          send_to_char( "¿Intentas atravesar la puerta?\n\r", ch );
          return;
      }
      
      
      switch ( dir )
      {
        case 0:
        case 1:
           dir += 2;
           break;
        case 2:
        case 3:
           dir -= 2;
           break;
        case 4:
        case 7:
           dir += 1;
           break;
        case 5:
        case 8:
           dir -= 1;
           break;
        case 6:
           dir += 3;
           break;
        case 9:
           dir -=3;
           break;
      }

      to_room = NULL;
      if ( pexit->distance > 1 )
       to_room = generate_exit( ch->in_room , &pexit );
    
      if ( to_room == NULL )
       to_room = pexit->to_room;
    

      char_from_room( ch );
      char_to_room( ch, to_room );    
     
      victim = get_char_room( ch, arg3 );

      if ( victim )
      { 
        if ( is_safe( ch, victim ) )
	return;
    
        if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
        {
        act( AT_PLAIN, "No puedes hacerle eso a $N.", ch, NULL, victim, TO_CHAR );
	return;
        }
    
        if ( !IS_NPC( victim ) && IS_SET( ch->act, PLR_NICE ) )
        {
        send_to_char( "No puedes hacer eso ahora mismo.\n\r", ch );
        return;
        }
    
        char_from_room( ch );
        char_to_room( ch, was_in_room );    

      
        if ( IS_SET( ch->in_room->room_flags, ROOM_SAFE ) )
        {
	set_char_color( AT_MAGIC, ch );
	send_to_char( "No puedes hacer eso.\n\r", ch );
	return;
        }
       
        to_room = NULL;
        if ( pexit->distance > 1 )
           to_room = generate_exit( ch->in_room , &pexit );
    
        if ( to_room == NULL )
           to_room = pexit->to_room;
    
       
        char_from_room( ch );
        char_to_room( ch, to_room );    
        
        sprintf( buf , "Alguien te lanza %s desde %s." , obj->short_descr , dir_name3[dir] );
        act( AT_ACTION, buf , victim, NULL, ch, TO_CHAR );      
        act( AT_ACTION, "Lanzas $p a $N.", ch, obj, victim, TO_CHAR );         
        sprintf( buf, "%s ha sido lanzado a $N desde %s." , obj->short_descr , dir_name[dir] );
        act( AT_ACTION, buf, ch, NULL, victim, TO_NOTVICT );  


      }
      else
      {   
         ch_printf( ch, "Lanzas %s hacia %s.\n\r", obj->short_descr , dir_name3[get_dir( arg2 )] );
         sprintf( buf, "%s es lanzado desde %s." , obj->short_descr , dir_name[dir] );
         act( AT_ACTION, buf, ch, NULL, NULL, TO_ROOM );  

      }
   }
   else if ( ( victim = get_char_room( ch, arg2 ) ) != NULL )
   {
        if ( is_safe( ch, victim ) )
	return;
    
        if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
        {
        act( AT_PLAIN, "No puedes hacer eso a $N.", ch, NULL, victim, TO_CHAR );
	return;
        }
    
        if ( !IS_NPC( victim ) && IS_SET( ch->act, PLR_NICE ) )
        {
        send_to_char( "No puedes hacer eso ahora mismo.\n\r", ch );
        return;
        }
    
   }
   else
   {
     if (( ship = ship_in_room( ch->in_room , arg2 ) ) == NULL )
     {
            act( AT_PLAIN, "No veo esa persona o nave aquí.", ch, NULL, argument, TO_CHAR );
            return;
     }
     else
     {
        if ( !ship->hatchopen )
        {
	 send_to_char("La escotilla está cerrada.\n\r", ch);
	 return;
	}

	if( !ship->entrance )
        {
	 send_to_char("¡Esta nave no tiene entrada!\n\r", ch);
	 return;
	}

	unequip_char( ch, obj );
   	separate_obj( obj );
	obj_from_char( obj );
	obj = obj_to_room( obj, get_room_index(ship->entrance) );
        sprintf(buf, "Lanzas %s a %s.", obj->short_descr, ship->name );
	send_to_char(buf, ch);
        sprintf(buf, "%s lanza %s a %s.\n\r", ch->name, obj->short_descr, ship->name);
        act( AT_ACTION, buf, ch, NULL, NULL, TO_ROOM );
	sprintf(buf, "%s es lanzado hacia la nave con un *clink-clink-clink*.\n\r", obj->short_descr);
       	echo_to_room( AT_WHITE , get_room_index(ship->entrance) , buf );
        return;
     }       
   }

   
   if ( obj == get_eq_char( ch, WEAR_WIELD )
   && ( tmpobj = get_eq_char( ch, WEAR_DUAL_WIELD)) != NULL )
       tmpobj->wear_loc = WEAR_WIELD;

   unequip_char( ch, obj );
   separate_obj( obj );
   obj_from_char( obj );
   obj = obj_to_room( obj, ch->in_room );
   
   if ( obj->item_type != ITEM_GRENADE )
       damage_obj ( obj );
   
/* NOT NEEDED UNLESS REFERING TO OBJECT AGAIN 

   if( obj_extracted(obj) )
      return;
*/
   if ( ch->in_room !=  was_in_room )
   {
     char_from_room( ch );
     char_to_room( ch, was_in_room );    
   }
   
   if ( !victim || char_died( victim ) )
       learn_from_failure( ch, gsn_throw );
   else
   {
       
       WAIT_STATE( ch, skill_table[gsn_throw]->beats );
       if ( IS_NPC(ch) || number_percent( ) < ch->pcdata->learned[gsn_throw] )
       {
	 learn_from_success( ch, gsn_throw );
	 global_retcode = damage( ch, victim, number_range( obj->weight*2 , (obj->weight*2 + ch->perm_str) ), TYPE_HIT );
       }
       else
       {
	 learn_from_failure( ch, gsn_throw );
	 global_retcode = damage( ch, victim, 0, TYPE_HIT );
       }
    
       if ( IS_NPC( victim ) && !char_died ( victim) )
       {
          if ( IS_SET( victim->act , ACT_SENTINEL ) )
          {
             victim->was_sentinel = victim->in_room;
             REMOVE_BIT( victim->act, ACT_SENTINEL );
          }
      
          start_hating( victim , ch );
          start_hunting( victim, ch );
     
       } 

   }
   
   return;                                   
  
}

void do_beg( CHAR_DATA *ch, char *argument )
{
    char buf  [MAX_STRING_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int percent, xp;
    int amount;

    if ( IS_NPC (ch) ) return;
    
    argument = one_argument( argument, arg1 );

    if ( ch->mount )
    {
	send_to_char( "Desmonta primero.\n\r", ch );
	return;
    }

    if ( arg1[0] == '\0' )
    {
	send_to_char( "¿Rogarle dinero a quién?\n\r", ch );
	return;
    }

    if ( ms_find_obj(ch) )
	return;

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
	send_to_char( "No está aquí.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Eso es absurdo.\n\r", ch );
	return;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_SAFE ) )
    {
	set_char_color( AT_MAGIC, ch );
	send_to_char( "No es un buen lugar para hacer eso.\n\r", ch );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
        send_to_char( "Una técnica de combate interesante.\n\r" , ch );
        return;
    }
    
    if ( victim->position == POS_FIGHTING )
    {
        send_to_char( "Ahora mismo está un poquito ocupado.\n\r" , ch );
        return;
    }
    
    if ( ch->position <= POS_SLEEPING )
    {
        send_to_char( "¿En tus sueños?\n\r" , ch );
        return;
    }
    
    if ( victim->position <= POS_SLEEPING )
    {
        send_to_char( "Qizás deberías despertarle primero...\n\r" , ch );
        return;
    }

    if ( !IS_NPC( victim ) )
    {
	send_to_char( "Le ruegas algo de dinero.\n\r", ch );
	act( AT_ACTION, "$n te pide que le des algo a cambio.\n\r", ch, NULL, victim, TO_VICT    );
	act( AT_ACTION, "$n ruega a $N algo de dinero.\n\r",  ch, NULL, victim, TO_NOTVICT );
	return;
    }
    
    WAIT_STATE( ch, skill_table[gsn_beg]->beats );
    percent  = number_percent( ) + ch->skill_level[SMUGGLING_ABILITY] + victim->top_level;

    if ( percent > ch->pcdata->learned[gsn_beg]  ) 
    {
	/*
	 * Failure.
	 */
	send_to_char( "Le ruegas algo de dinero... Pero no consigues nada.\n\r", ch );
	act( AT_ACTION, "¡$n te está sacando de tus casillas tanto mendigar!\n\r", ch, NULL, victim, TO_VICT    );
	act( AT_ACTION, "$n ruega a $N algo de dinero.\n\r",  ch, NULL, victim, TO_NOTVICT );

        if ( victim->alignment < 0 && victim->top_level >= ch->top_level+5 )
	{
	  sprintf( buf, "¡%s está molestándome y necesita aprender una lección!", ch->name );
	  do_yell( victim, buf );
          global_retcode = multi_hit( victim, ch, TYPE_UNDEFINED );
	}
	
	learn_from_failure( ch, gsn_beg );

	return;
    }

    
    act( AT_ACTION, "$n ruega a $N algo de dinero.\n\r",  ch, NULL, victim, TO_NOTVICT );
    act( AT_ACTION, "¡$n te ruega algo de dinero!\n\r", ch, NULL, victim, TO_VICT    );

    amount = UMIN( victim->gold , number_range(1, 10) );
    if ( amount <= 0 )
    {
        do_look( victim , ch->name );
	do_say( victim , "Lo siento, no tengo nada para tí.\n\r" );
	learn_from_failure( ch, gsn_beg );
	return;
    }

	ch->gold     += amount;
	victim->gold -= amount;
	ch_printf( ch, "%s te da %d créditos.\n\r", victim->short_descr , amount );
	learn_from_success( ch, gsn_beg );
	xp = UMIN( amount*10 , ( exp_level( ch->skill_level[SMUGGLING_ABILITY]+1) - exp_level( ch->skill_level[SMUGGLING_ABILITY])  )  );    
        xp = UMIN( xp , xp_compute( ch, victim ) );
        gain_exp( ch, xp, SMUGGLING_ABILITY );  
        ch_printf( ch, "&WRecibes %ld puntos de experiencia en contrabando.\n\r", xp );
        act( AT_ACTION, "$N da a $n algunos créditos.\n\r",  ch, NULL, victim, TO_NOTVICT );
        act( AT_ACTION, "Das a $n algunos créditos.\n\r", ch, NULL, victim, TO_VICT    );

	return;

}

void do_pickshiplock( CHAR_DATA *ch, char *argument )
{
   do_pick( ch, argument );
}

void do_hijack( CHAR_DATA *ch, char *argument )
{
    int chance; 
    int x;
    SHIP_DATA *ship;
    char buf[MAX_STRING_LENGTH];
    bool uhoh = FALSE;
//    CHAR_DATA *guard;      <--- For the guard shits below
//    ROOM_INDEX_DATA *room;
            
    	        if ( (ship = ship_from_cockpit(ch->in_room->vnum)) == NULL )  
    	        {
    	            send_to_char("&RDebes estar en el asiento del piloto para hacer esto!\n\r",ch);
    	            return;
    	        }
    	        
    	        if ( ship->class > SHIP_SPACE_STATION )
    	        {
    	            send_to_char("&R¡Esto no es una nave!\n\r",ch);
    	            return;
    	        }
    	        
    	        if ( check_pilot( ch , ship ) )
    	        {
    	            send_to_char("&R¡Eso sería absurdo!\n\r",ch);
    	            return;
    	        }
    	        
    	        if ( ship->type == MOB_SHIP && get_trust(ch) < 102 )
    	        {
    	            send_to_char("&REsta nave no se puede pilotar por mortales...\n\r",ch);
    	            return;
    	        }
    	        
                if  ( ship->class == SHIP_SPACE_STATION )
                {
                   send_to_char( "no puedes hacer eso aquí.\n\r" , ch );
                   return;
                }   
    
    	        if ( ship->lastdoc != ship->location )
                {
                     send_to_char("&rLa nave no está aterrizada.\n\r",ch);
                     return;
                }
    
    	        if ( ship->shipstate != SHIP_DOCKED && ship->shipstate != SHIP_DISABLED )
    	        {
    	            send_to_char("La nave no está aterrizada.\n\r",ch);
    	            return;
    	        }
                
                if ( ship->shipstate == SHIP_DISABLED )
    	        {
    	            send_to_char("El motor de la nave está desactivado.\n\r",ch);
    	            return;
    	        }
 /* Remind to fix later, 'cause I'm sick of reading through all this fucking code  
              
                for ( room = ship->firstroom ; room ; room = room->next_in_ship )
		{
		   for ( guard = room->first_person; guard ; guard = guard->next_in_room )
		      if ( IS_NPC(guard) && guard->pIndexData && guard->pIndexData->vnum == MOB_VNUM_SHIP_GUARD 
		      && guard->position > POS_SLEEPING && !guard->fighting )
                      {
                         start_hating( guard, ch );
                         start_hunting( guard , ch );
                         uhoh = TRUE;
                      }   
		}
 */
		
                if ( uhoh )
    	        {
    	            send_to_char("Uh oh....\n\r",ch);
    	            return;
    	        }
		                
                chance = IS_NPC(ch) ? 0
	                 : (int)  (ch->pcdata->learned[gsn_hijack]) ;
                x = number_percent( );
                if ( x > chance )
    		{  
    		    send_to_char("No consigues introducir el código de lanzamiento.\n\r",ch);
    	            return;
                }
                
                chance = IS_NPC(ch) ? 0
	                 : (int)  (ch->pcdata->learned[gsn_shipsystems]) ;
                if ( number_percent( ) < chance )
    		{  
                
    		   if (ship->hatchopen)
    		   {
    		     ship->hatchopen = FALSE;
    		     sprintf( buf , "la escotilla de %s se cierra." , ship->name);  
       	             echo_to_room( AT_YELLOW , get_room_index(ship->location) , buf );
       	             echo_to_room( AT_YELLOW , get_room_index(ship->entrance) , "La escotilla se cierra." );
       	           }
    		   set_char_color( AT_GREEN, ch );
    		   send_to_char( "Secuencia de despegue iniciada.\n\r", ch);
    		   act( AT_PLAIN, "$n inicia la secuencia de despegue.", ch,
		        NULL, argument , TO_ROOM );
		   echo_to_ship( AT_YELLOW , ship , "la nave ruje a medida que se aleja de la tierra.");
    		   sprintf( buf, "%s comienza a despegar.", ship->name );
    		   echo_to_room( AT_YELLOW , get_room_index(ship->location) , buf );
    		   ship->shipstate = SHIP_LAUNCH;
    		   ship->currspeed = ship->realspeed;
                   learn_from_success( ch, gsn_shipsystems );
                   learn_from_success( ch, gsn_hijack );
//                   sprintf( buf, "%s has been hijacked!", ship->name );
//    		   echo_to_all( AT_RED , buf, 0 );
    		   
                   return;   	   	
                }
                set_char_color( AT_RED, ch );
	        send_to_char("Fallaste.\n\r",ch);
    	   	return;	
    	
}

void do_special_forces ( CHAR_DATA *ch , char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int chance, credits;
    
    if ( IS_NPC( ch ) || !ch->pcdata )
    	return;
    	
    strcpy( arg, argument );    
    
    switch( ch->substate )
    { 
    	default:
    	        if ( ch->backup_wait )
    	        {
    	            send_to_char( "&RYour reinforcements are already on the way.\n\r", ch );
    	            return;
    	        }

    	        if ( !ch->pcdata->clan )
    	        {
    	            send_to_char( "&RYou need to be a member of an organization before you can call for a guard.\n\r", ch );
    	            return;
    	        }

    	        if ( ch->gold < ch->skill_level[POLITICIAN_ABILITY] * 350 )
    	        {
    	            ch_printf( ch, "&RYou dont have enough credits to send for reinforcements.\n\r" );
    	            return;
    	        }

    	        chance = (int) (ch->pcdata->learned[gsn_specialforces]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GYou begin making the call for reinforcements.\n\r", ch);
    		   act( AT_PLAIN, "$n begins issuing orders int $s comlink.", ch,
		        NULL, argument , TO_ROOM );
		   add_timer ( ch , TIMER_DO_FUN , 1 , do_special_forces , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   return;
	        }
	        send_to_char("&RYou call for reinforcements but nobody answers.\n\r",ch);
	        learn_from_failure( ch, gsn_specialforces );
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
    	        send_to_char("&RYou are interupted before you can finish your call.\n\r", ch);
    	        return;
    }

    ch->substate = SUB_NONE;

    send_to_char( "&GYour reinforcements are on the way.\n\r", ch);
    credits = ch->skill_level[POLITICIAN_ABILITY] * 175;
    ch_printf( ch, "It cost you %d credits.\n\r", credits);
    ch->gold -= UMIN( credits , ch->gold );

    learn_from_success( ch, gsn_specialforces );

    if ( nifty_is_name( "empire" , ch->pcdata->clan->name ) )
       ch->backup_mob = MOB_VNUM_IMP_FORCES;
    else if ( nifty_is_name( "republic" , ch->pcdata->clan->name ) )
       ch->backup_mob = MOB_VNUM_NR_FORCES;
    else
       ch->backup_mob = MOB_VNUM_MERC_FORCES;

    ch->backup_wait = number_range(1,2);

}


void do_elite_guard ( CHAR_DATA *ch , char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int chance, credits;
    
    if ( IS_NPC( ch ) || !ch->pcdata )
    	return;

    strcpy( arg, argument );

    switch( ch->substate )
    {
    	default:
    	        if ( ch->backup_wait )
    	        {
    	            send_to_char( "&RYou already have backup coming.\n\r", ch );
    	            return;
    	        }

    	        if ( !ch->pcdata->clan )
    	        {
    	            send_to_char( "&RYou need to be a member of an organization before you can call for a guard.\n\r", ch );
    	            return;
    	        }

    	        if ( ch->gold < ch->skill_level[POLITICIAN_ABILITY] * 200 )
    	        {
    	            ch_printf( ch, "&RYou dont have enough credits.\n\r", ch );
    	            return;
    	        }

    	        chance = (int) (ch->pcdata->learned[gsn_eliteguard]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GYou begin making the call for reinforcements.\n\r", ch);
    		   act( AT_PLAIN, "$n begins issuing orders into $s comlink.", ch,
		        NULL, argument , TO_ROOM );
		   add_timer ( ch , TIMER_DO_FUN , 1 , do_elite_guard , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   return;
	        }
	        send_to_char("&RYou call for a guard but nobody answers.\n\r",ch);
	        learn_from_failure( ch, gsn_eliteguard );
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
    	        send_to_char("&RYou are interupted before you can finish your call.\n\r", ch);
    	        return;
    }

    ch->substate = SUB_NONE;

    send_to_char( "&GYour guard is on the way.\n\r", ch);

    credits = ch->skill_level[POLITICIAN_ABILITY] * 200;
    ch_printf( ch, "It cost you %d credits.\n\r", credits);
    ch->gold -= UMIN( credits , ch->gold );

    learn_from_success( ch, gsn_eliteguard );

    if ( nifty_is_name( "empire" , ch->pcdata->clan->name ) )
       ch->backup_mob = MOB_VNUM_IMP_ELITE;
    else if ( nifty_is_name( "republic" , ch->pcdata->clan->name ) )
       ch->backup_mob = MOB_VNUM_NR_ELITE;
    else
       ch->backup_mob = MOB_VNUM_MERC_ELITE;

    ch->backup_wait = 1;

}

void do_add_patrol ( CHAR_DATA *ch , char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int chance, credits;
    
    if ( IS_NPC( ch ) || !ch->pcdata )
    	return;

    strcpy( arg, argument );    
    
    switch( ch->substate )
    { 
    	default:
    	        if ( ch->backup_wait )
    	        {
    	            send_to_char( "&RYou already have backup coming.\n\r", ch );
    	            return;
    	        }
    	        
    	        if ( !ch->pcdata->clan )
    	        {
    	            send_to_char( "&RYou need to be a member of an organization before you can call for a guard.\n\r", ch );
    	            return;
    	        }    
    	        
    	        if ( ch->gold < ch->skill_level[POLITICIAN_ABILITY] * 30 )
    	        {
    	            ch_printf( ch, "&RYou dont have enough credits.\n\r", ch );
    	            return;
    	        }    
    	        
    	        chance = (int) (ch->pcdata->learned[gsn_addpatrol]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GYou begin making the call for reinforcements.\n\r", ch);
    		   act( AT_PLAIN, "$n begins issuing orders int $s comlink.", ch,
		        NULL, argument , TO_ROOM );
		   add_timer ( ch , TIMER_DO_FUN , 1 , do_add_patrol , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   return;
	        }
	        send_to_char("&RYou call for a guard but nobody answers.\n\r",ch);
	        learn_from_failure( ch, gsn_addpatrol );
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
    	        send_to_char("&RYou are interupted before you can finish your call.\n\r", ch);
    	        return;
    }
    
    ch->substate = SUB_NONE;
    
    send_to_char( "&GYour guard is on the way.\n\r", ch);
    
    credits = ch->skill_level[POLITICIAN_ABILITY] * 30;
    ch_printf( ch, "It cost you %d credits.\n\r", credits);
    ch->gold -= UMIN( credits , ch->gold );

    learn_from_success( ch, gsn_addpatrol );
    
    if ( nifty_is_name( "empire" , ch->pcdata->clan->name ) )
       ch->backup_mob = MOB_VNUM_IMP_PATROL;
    else if ( nifty_is_name( "republic" , ch->pcdata->clan->name ) )
       ch->backup_mob = MOB_VNUM_NR_PATROL;
    else
       ch->backup_mob = MOB_VNUM_MERC_PATROL;

    ch->backup_wait = 1;

}

void do_jail ( CHAR_DATA *ch , char *argument )
{
    CHAR_DATA *victim =NULL;
    CLAN_DATA   *clan =NULL;
    ROOM_INDEX_DATA *jail =NULL;
    
    if ( IS_NPC (ch) ) return;
    
    if ( !ch->pcdata || ( clan = ch->pcdata->clan ) == NULL )
    {
	send_to_char( "Solo los miembros de una organización pueden encarcelar a sus enemigos.\n\r", ch );
	return;
    }

    jail = get_room_index( clan->jail );
    if ( !jail && clan->mainclan )
       jail = get_room_index( clan->mainclan->jail );
    
    if ( !jail )
    {
	send_to_char( "Tu organización no tiene una prisión adecuada.\n\r", ch );
	return;
    }
        
    if ( ch->mount )
    {
	send_to_char( "Desmotna primero.\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	send_to_char( "¿Encarcelar a quién?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, argument ) ) == NULL )
    {
	send_to_char( "No está aquí.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Eso es absurdo.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Eso sería perder el tiempo.\n\r", ch );
	return;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_SAFE ) )
    {
	set_char_color( AT_MAGIC, ch );
	send_to_char( "Este no es un buen lugar para eso.\n\r", ch );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
        send_to_char( "Una técnica de combate interesante.\n\r" , ch );
        return;
    }
    
    if ( ch->position <= POS_SLEEPING )
    {
        send_to_char( "¿En tus sueños?\n\r" , ch );
        return;
    }
    
    if ( victim->position >= POS_SLEEPING )
    {
        send_to_char( "Debes aturdirle primero.\n\r" , ch );
        return;
    }

	send_to_char( "Le escoltas a la cárcel.\n\r", ch );
	act( AT_ACTION, "Tienes la extraña sensación de que te han movido.\n\r", ch, NULL, victim, TO_VICT    );
	act( AT_ACTION, "$n excolta a $N.\n\r",  ch, NULL, victim, TO_NOTVICT );

    char_from_room ( victim );
    char_to_room ( victim , jail );

    act( AT_ACTION, "Las puertas se abren brevemente y arrojan a $n.\n\r",  victim, NULL, NULL, TO_ROOM );

    learn_from_success( ch , gsn_jail );
    
    return;        
}

void do_smalltalk ( CHAR_DATA *ch , char *argument )
{
    char buf  [MAX_STRING_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    CHAR_DATA *victim = NULL;
    PLANET_DATA *planet = NULL;
    CLAN_DATA   *clan = NULL;
    int percent;
    
   if ( IS_NPC(ch) || !ch->pcdata )
   {
       send_to_char( "No lo ves útil.\n\r", ch );
   }
    
    argument = one_argument( argument, arg1 );

    if ( ch->mount )
    {
	send_to_char( "Desmonta primero.\n\r", ch );
	return;
    }

    if ( arg1[0] == '\0' )
    {
	send_to_char( "¿Soltar el discursito a quién?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
	send_to_char( "No está aquí.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "No necesitas convencerte de tus propios ideales.\n\r", ch );
	return;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_SAFE ) )
    {
	set_char_color( AT_MAGIC, ch );
	send_to_char( "No es un buen lugar para hacer esto.\n\r", ch );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
        send_to_char( "Una técnica de combate interesante.\n\r" , ch );
        return;
    }
    
    if ( victim->position == POS_FIGHTING )
    {
        send_to_char( "No te puede prestar atención.\n\r" , ch );
        return;
    }
    

    if ( !IS_NPC(victim) || victim->vip_flags == 0 )
    {
        send_to_char( "No merece la pena gastar diplomacia.\n\r" , ch );
        return;
    }
    
    if ( ch->position <= POS_SLEEPING )
    {
        send_to_char( "¿En tus sueños?\n\r" , ch );
        return;
    }
    
    if ( victim->position <= POS_SLEEPING )
    {
        send_to_char( "Despiértale primero...\n\r" , ch );
        return;
    }

    percent = number_percent( );

    WAIT_STATE( ch, skill_table[gsn_smalltalk]->beats );

    if ( percent - ch->skill_level[POLITICIAN_ABILITY] + victim->top_level > ch->pcdata->learned[gsn_smalltalk]  ) 
    {
	/*
	 * Failure.
	 */
	send_to_char( "Te ignora olímpicamente.\n\r", ch );
	act( AT_ACTION, "$n te molesta con tanta palabrería.\n\r", ch, NULL, victim, TO_VICT    );
	act( AT_ACTION, "$n pregunta a $N sobre el tiempo pero le ignora.\n\r",  ch, NULL, victim, TO_NOTVICT );

        if ( victim->alignment < -500 && victim->top_level >= ch->top_level+5 )
	{
	  sprintf( buf, "¡Cállate %s!", ch->name );
	  do_yell( victim, buf );
          global_retcode = multi_hit( victim, ch, TYPE_UNDEFINED );
	}
	
	return;
    }
    
    send_to_char( "Le sueltas tu pequeño discurso y mantienes una pequeña conversación.\n\r", ch );
    act( AT_ACTION, "$n te sonríe y dice: 'hola'.\n\r", ch, NULL, victim, TO_VICT    );
    act( AT_ACTION, "$n mantiene una pequeña conversación con $N.\n\r",  ch, NULL, victim, TO_NOTVICT );
    
    if ( IS_NPC(ch) || !ch->pcdata || !ch->pcdata->clan || !ch->in_room->area || !ch->in_room->area->planet )
      return;
    
    if ( ( clan = ch->pcdata->clan->mainclan ) == NULL )
       clan = ch->pcdata->clan;
       
    planet = ch->in_room->area->planet;
    
    if ( clan != planet->governed_by )
       return;
       
    planet->pop_support += 0.2;
    send_to_char( "El soporte popular de tu organización se incrementa ligeramente.\n\r", ch );
         
    gain_exp(ch, victim->top_level*10, POLITICIAN_ABILITY);
    ch_printf( ch , "Recibes %d puntos de experiencia en diplomacia.\n\r", victim->top_level*10 );
    
    learn_from_success( ch, gsn_smalltalk );
        
    if ( planet->pop_support > 100 )
        planet->pop_support = 100;
}

void do_propeganda ( CHAR_DATA *ch , char *argument )
{
    char buf  [MAX_STRING_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    PLANET_DATA *planet;
    CLAN_DATA   *clan;
    int percent;
    
   if ( IS_NPC(ch) || !ch->pcdata || !ch->pcdata->clan || !ch->in_room->area || !ch->in_room->area->planet )
   {
       send_to_char( "Eso sería inútil.\n\r", ch );
       return;
   }
    
    argument = one_argument( argument, arg1 );

    if ( ch->mount )
    {
	send_to_char( "Desmonta primero.\n\r", ch );
	return;
    }

    if ( arg1[0] == '\0' )
    {
	send_to_char( "¿hacer propaganda a quién?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
	send_to_char( "No está aquí.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Es absurdo.\n\r", ch );
	return;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_SAFE ) )
    {
	set_char_color( AT_MAGIC, ch );
	send_to_char( "Este no es un buen lugar para hacer eso.\n\r", ch );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
        send_to_char( "Una técnica de combate interesante.\n\r" , ch );
        return;
    }
    
    if ( victim->position == POS_FIGHTING )
    {
        send_to_char( "No te va a hacer caso hasta que no deje de luchar.\n\r" , ch );
        return;
    }
    

    if ( victim->vip_flags == 0 )
    {
        send_to_char( "Sería gastar esfuerzos inútilmente.\n\r" , ch );
        return;
    }
    
    if ( ch->position <= POS_SLEEPING )
    {
        send_to_char( "¿En tus sueños o qué?\n\r" , ch );
        return;
    }
    
    if ( victim->position <= POS_SLEEPING )
    {
        send_to_char( "Despiértale primero...\n\r" , ch );
        return;
    }

    if ( ( clan = ch->pcdata->clan->mainclan ) == NULL )
       clan = ch->pcdata->clan;
       
    planet = ch->in_room->area->planet;
        
    sprintf( buf, ", y los malvados de %s" , planet->governed_by ? planet->governed_by->name : "sus líderes actuales" );
    ch_printf( ch, "Le hablas sobre los beneficios de %s%s.\n\r", ch->pcdata->clan->name,
        planet->governed_by == clan ? "" : buf );
    act( AT_ACTION, "$n habla sobre su organización.\n\r", ch, NULL, victim, TO_VICT    );
    act( AT_ACTION, "$n habla a $N sobre su organización.\n\r",  ch, NULL, victim, TO_NOTVICT );

    WAIT_STATE( ch, skill_table[gsn_propeganda]->beats );

    percent = number_percent();

    if ( percent - get_curr_cha(ch) + victim->top_level > ch->pcdata->learned[gsn_propeganda]  ) 
    {

        if ( planet->governed_by != clan )
	{
	  sprintf( buf, "¡%s nos ha traicionado!" , ch->name);
	  do_yell( victim, buf );
          global_retcode = multi_hit( victim, ch, TYPE_UNDEFINED );
	}
	
	return;
    }
    
    if ( planet->governed_by == clan )
    { 
       planet->pop_support += .5 + ch->top_level/15;
       send_to_char( "El soporte popular de tu organización se incrementa.\n\r", ch );
    }     
    else
    {
       planet->pop_support -= .5 + ch->top_level/15;
       send_to_char( "El soporte popular del gobierno actual disminuye.\n\r", ch );
    }
    
    gain_exp(ch, victim->top_level*100, POLITICIAN_ABILITY);
    ch_printf( ch , "Recibes %d puntos de experiencia en diplomacia.\n\r", victim->top_level*100 );
    
    learn_from_success( ch, gsn_propeganda );
        
    if ( planet->pop_support > 100 )
        planet->pop_support = 100;
    if ( planet->pop_support < -100 )
        planet->pop_support = -100;
    save_planet(planet);
}

void do_bribe ( CHAR_DATA *ch , char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    PLANET_DATA *planet;
    CLAN_DATA   *clan;
    int percent, amount;
    
    if ( IS_NPC(ch) || !ch->pcdata || !ch->pcdata->clan || !ch->in_room->area || !ch->in_room->area->planet )
    {
      send_to_char( "Eso no tiene sentido.\n\r", ch );
      return;
    }
    
    argument = one_argument( argument, arg1 );

    if ( ch->mount )
    {
	send_to_char( "Desmonta primero.\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	send_to_char( "¿Sobornar a quién con cuanto?\n\r", ch );
	return;
    }

    amount = atoi( argument );

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
	send_to_char( "No está aquí.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "Eso sería absurdo.\n\r", ch );
	return;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_SAFE ) )
    {
	set_char_color( AT_MAGIC, ch );
	send_to_char( "Este no es el lugar adecuado para hacer esto.\n\r", ch );
	return;
    }

    if ( amount > ch->gold)
    {
	send_to_char( "No tienes tantos créditos...\n\r", ch);
	return;
    }

    if ( amount <= 0 )
    {
       	send_to_char( "Quizás si ofrecieras más créditos te haría caso.\n\r", ch );
	return;
    }

    if ( amount > 10000)
    {
	send_to_char( "No puedes sobornar con tantos créditos de golpe.\n\r", ch);
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
        send_to_char( "Una técnica de combate interesante.\n\r" , ch );
        return;
    }
    
    if ( victim->position == POS_FIGHTING )
    {
        send_to_char( "Es un poco complicado.\n\r" , ch );
        return;
    }
    
    if ( ch->position <= POS_SLEEPING )
    {
        send_to_char( "¿en tus sueños?\n\r" , ch );
        return;
    }
    
    if ( victim->position <= POS_SLEEPING )
    {
        send_to_char( "Despiértale primero...\n\r" , ch );
        return;
    }

    if ( victim->vip_flags == 0 )
    {
        send_to_char( "Sería gastar diplomacia tontamente.\n\r" , ch );
        return;
    }
    
    ch->gold -= amount;
    victim->gold += amount;

	ch_printf( ch, "Le das un pequeño regalo en nombre de %s.\n\r", ch->pcdata->clan->name );
	act( AT_ACTION, "$n te ofrece un pequeño regalo.\n\r", ch, NULL, victim, TO_VICT    );
	act( AT_ACTION, "$n da a $N algunos créditos.\n\r",  ch, NULL, victim, TO_NOTVICT );

    if ( !IS_NPC( victim ) )
	return;
    
    WAIT_STATE( ch, skill_table[gsn_bribe]->beats );

    if ( percent - amount + victim->top_level > ch->pcdata->learned[gsn_bribe]  ) 
        return;
            
    if ( ( clan = ch->pcdata->clan->mainclan ) == NULL )
       clan = ch->pcdata->clan;
       
    planet = ch->in_room->area->planet;
    
    
    if ( clan == planet->governed_by )
    {
      planet->pop_support += URANGE( 0.1 , amount/1000 , 2 );
      send_to_char( "el soporte popular de tu organización se incrementa ligeramente.\n\r", ch );
    
      amount = UMIN( amount ,( exp_level(ch->skill_level[POLITICIAN_ABILITY]+1) - exp_level(ch->skill_level[POLITICIAN_ABILITY]) ) );

      gain_exp(ch, amount , POLITICIAN_ABILITY);
      ch_printf( ch , "Recibes %d puntos de experiencia en diplomacia.\n\r", amount );
    
      learn_from_success( ch, gsn_bribe );
    }
    
    if ( planet->pop_support > 100 )
        planet->pop_support = 100;
}

void do_seduce ( CHAR_DATA *ch , char *argument )
{
    AFFECT_DATA af;
    int chance;
    int level;
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *rch;

    if ( argument[0] == '\0' )
    {
        send_to_char( "¿Seducir a quién?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room( ch, argument ) ) == NULL )
    {
	send_to_char( "No está aquí.\n\r", ch);
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "¡Ya te amas!\n\r", ch );
	return;
    }

    if ( IS_SET( victim->immune, RIS_CHARM ) )
    {
	send_to_char( "Es inmune a tus encantos.\n\r", ch);
	sprintf(buf, "%s trata de seducirte sin éxito.\n\r", ch->name);
        act( AT_MAGIC, buf, ch, NULL, victim, TO_ROOM );
	return;
    }

    if ( find_keeper(victim) != NULL )
    {
        send_to_char( "¡No lo lograrías!\n\r", ch);
	return;
    }

    if ( !IS_NPC( victim ) && !IS_NPC( ch ) )
    {
	send_to_char( "No lo creo...\n\r", ch );
	sprintf(buf, "%s trata de seducirte.\n\r", ch->name);
        act( AT_MAGIC, buf, ch, NULL, victim, TO_ROOM );
	return;
    }

    level = !IS_NPC(ch) ? (int)ch->pcdata->learned[gsn_seduce] : ch->top_level;
    chance = ris_save( victim, level, RIS_CHARM );

    if ( IS_AFFECTED(victim, AFF_CHARM) ||   chance == 1000 || IS_AFFECTED(ch, AFF_CHARM) || ch->top_level < victim->top_level || circle_follow( victim, ch ) || saves_spell_staff( chance, victim ) || ch->sex == victim->sex )
    {
	send_to_char("&w&BFallaste.\n\r", ch);
	sprintf(buf, "%s trata de seducirte.\n\r", ch->name);
        act( AT_MAGIC, buf, ch, NULL, victim, TO_ROOM );
	learn_from_failure(ch, gsn_seduce);
	return;
    }

    if ( victim->master )
	stop_follower( victim );
    add_follower( victim, ch );
    for(rch = ch->in_room->first_person; rch; rch = rch->next_in_room)
    {
     if(rch->master == ch && IS_AFFECTED(rch, AFF_CHARM) && rch != victim)
     {
		send_to_char("Recuperas algo de sentido.\n\r", rch);
		ch_printf(ch, "&B%s parece más consciente.\n\r", PERS(rch, ch));
		stop_follower(rch);
     }
    }
    af.type      = gsn_seduce;
    af.duration  = (number_fuzzy( (level + 1) / 3 ) + 1) * DUR_CONV;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( victim, &af );
    act( AT_MAGIC, "$n te atrae demasiado...", ch, NULL, victim, TO_VICT);
    act( AT_MAGIC, "A $N se le ponen los ojos vidriosos...", ch, NULL, victim, TO_ROOM );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );

    learn_from_success(ch, gsn_seduce);
    sprintf( buf, "%s ha seducido a %s.", ch->name, victim->name);
    log_string_plus( buf, LOG_NORMAL, ch->top_level );

    return;

}

void do_mass_propeganda ( CHAR_DATA *ch , char *argument )
{
    char buf  [MAX_STRING_LENGTH];
    CHAR_DATA *rch;
    PLANET_DATA *planet;
    CLAN_DATA   *clan;
    int victims = 0;

    if ( IS_NPC(ch) || !ch->pcdata || !ch->pcdata->clan || !ch->in_room->area || !ch->in_room->area->planet )
    {
        send_to_char( "Eso sería absurdo.\n\r", ch );
        return;
    }
        
    if ( ch->mount )
    {
        send_to_char( "Desmonta primero.\n\r", ch );
        return;
    }

    if ( IS_SET( ch->in_room->room_flags, ROOM_SAFE ) )
    {
        set_char_color( AT_MAGIC, ch );
        send_to_char( "Este no es un buen lugar para hacer eso.\n\r", ch );
        return;
    }

    if ( ch->position == POS_FIGHTING )
    {
        send_to_char( "Una técnica de combate interesante.\n\r" , ch );
        return;
    }
    
    if ( ch->position <= POS_SLEEPING )
    {
        send_to_char( "¿En tus sueños?\n\r" , ch );
        return;
    }
    
    if ( ( clan = ch->pcdata->clan->mainclan ) == NULL )
       clan = ch->pcdata->clan;
       
    planet = ch->in_room->area->planet;
        
    sprintf( buf, ", y los malvados de %s" , planet->governed_by ? planet->governed_by->name : "their current leaders" );
    ch_printf( ch, "Hablas a la gente acerca de los beneficios de %s%s.\n\r", ch->pcdata->clan->name,
        planet->governed_by == clan ? "" : buf );
    act( AT_ACTION, "$n habla sobre su organización.\n\r", ch, NULL, NULL, TO_ROOM    );

    WAIT_STATE( ch, skill_table[gsn_masspropeganda]->beats );

    if ( number_percent() < ch->pcdata->learned[gsn_masspropeganda]  ) 
    {
        for ( rch = ch->in_room->first_person; rch; rch = rch->next_in_room )
        {
            if ( rch == ch )
              continue;

            if (!IS_NPC(rch))
              continue;

            if ( rch->vip_flags == 0 )
              continue;

            if ( can_see( ch, rch ) )
              victims++;
            else
              continue;
        }
    
        if ( planet->governed_by == clan )
        { 
            planet->pop_support += (.5 + ch->top_level/10)*victims;
            send_to_char( "El soporte de tu organización se incrementa.\n\r", ch );
        }     
        else
        {
            planet->pop_support -= (ch->top_level/10)*victims;
            send_to_char( "el soporte popular del gobierno actual disminuye.\n\r", ch );
        }
    
        gain_exp(ch, ch->top_level*100, POLITICIAN_ABILITY);
        ch_printf( ch , "Recibes %d puntos de experiencia en diplomacia.\n\r", ch->top_level*100 );
    
        learn_from_success( ch, gsn_masspropeganda );
       
        if ( planet->pop_support > 100 )
          planet->pop_support = 100;
        if ( planet->pop_support < -100 )
          planet->pop_support = -100;
        save_planet(planet);
        return;
    }
    else
    {
        send_to_char("No ves a nadie que le interese lo que tienes que decir.\n\r",ch);
        return;
    }
    return;
}

void do_gather_intelligence ( CHAR_DATA *ch , char *argument )
{}

void do_repair( CHAR_DATA *ch, char *argument)
{
 OBJ_DATA *obj,*cobj;
 char arg[MAX_STRING_LENGTH];
 int chance;
 bool checktool, checksew;

 strcpy(arg, argument);

 switch( ch->substate )
    { 
    	default:

 if ( arg[0] == '\0' )
  {
    send_to_char( "¿Reparar qué?\n\r", ch );
    return;
  }

 if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
  {
    send_to_char( "&RNo tienes ese objeto.\n\r&w", ch);
    return;
  }

 checktool = FALSE;
 checksew = FALSE;

 if ( !IS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
  {
   send_to_char( "&Rnecesitas estar en una fábrica o taller para hacer eso.\n\r", ch);
   return;
  }

 if (obj->item_type != ITEM_WEAPON && obj->item_type != ITEM_ARMOR)
  {
    send_to_char( "&Rsolo puedes reparar armas y armaduras.&w\n\r", ch);
    return;
  }

 if (obj->item_type == ITEM_WEAPON && obj->value[0] == INIT_WEAPON_CONDITION)
  {
	send_to_char("&WNo parece que necesite repararse.\n\r", ch);
 	return;
  }
 else if(obj->item_type == ITEM_ARMOR && obj->value[0] == obj->value[1])
  {
	send_to_char("&WParece que no necesita repararse.\n\r", ch);
 	return;
  }

 for ( cobj = ch->last_carrying; cobj; cobj = cobj->prev_content )
  {
        if (cobj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;
        if (cobj->item_type == ITEM_THREAD)
	  checksew = TRUE;
  }

 if(!checktool && obj->item_type == ITEM_WEAPON)
  {
	send_to_char("&w&RNecesitas una caja de herramientas.\n\r", ch);
	return;
  }

 if(!checksew && obj->item_type == ITEM_ARMOR)
  {
	send_to_char("&w&RNecesitas ilo y aguja.\n\r", ch);
	return;
  }

   send_to_char( "&W&Gcomienzas a reparar tu equipo...&W\n\r", ch);
   act( AT_PLAIN, "$n coge su caja de herramientas y comienza a reparar algo.", ch, NULL, argument , TO_ROOM );
   add_timer ( ch , TIMER_DO_FUN , 5 , do_repair , 1 );
   ch->dest_buf   = str_dup(arg);
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

    chance = IS_NPC(ch) ? ch->top_level : (int)(ch->pcdata->learned[gsn_repair]);

    if ( number_percent( ) > chance*2 )
    {
	send_to_char("&RNo consigues reparar el equipo.\n\r", ch);
	learn_from_failure(ch, gsn_repair);
	return;
    }

 if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
  {
    send_to_char( "&RError S3. Reporta a la administración\n\r&w", ch);
    return;
  }

      switch ( obj->item_type )
      {
          default:
            send_to_char( "Error S4. Repórtaselo a la administración.\n\r", ch);
            return;
          case ITEM_ARMOR:
            obj->value[0] = obj->value[1];
            break;
          case ITEM_WEAPON:
            obj->value[0] = INIT_WEAPON_CONDITION;
            break;
          case ITEM_DEVICE:
            obj->value[2] = obj->value[1];
            break;
      }

 send_to_char("&GReparas tu equipo que queda en perfectas condiciones.&W\n\r", ch);

    {
         long xpgain;
         
         xpgain =  (number_percent( ) * 6) * ch->skill_level[ENGINEERING_ABILITY];
         gain_exp(ch, xpgain, ENGINEERING_ABILITY);
         ch_printf( ch , "Recibes %d puntos de experiencia en ingeniero.\n\r", xpgain );
    }

    learn_from_success( ch, gsn_repair );
 }

void do_makeduallightsaber( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int chance;
    bool checktool, checkdura, checkbatt, 
         checkoven, checkcond, checkcirc, checklens, checkgems, checkmirr;
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum, level, gems, charge, gemtype;
    AFFECT_DATA *paf;
    AFFECT_DATA *paf2;
    
    strcpy( arg, argument );    
    
    switch( ch->substate )
    { 
    	default:
    	        if ( arg[0] == '\0' )
                {
                  send_to_char( "&RUsage: makeduallightsaber <name>\n\r&w", ch);
                  return;   
                }

    	        checktool = FALSE;
                checkdura = FALSE;
                checkbatt = FALSE;
                checkoven = FALSE;
                checkcond = FALSE;
                checkcirc = FALSE;
                checklens = FALSE;
                checkgems = FALSE;
                checkmirr = FALSE;

                if ( !IS_SET( ch->in_room->room_flags, ROOM_SAFE ) || !IS_SET( ch->in_room->room_flags, ROOM_SILENCE ))
                {
                   send_to_char( "&RYou need to be in a quiet peaceful place to craft a lightsaber.\n\r", ch);
                   return;
                }
                
                for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
                {
                  if (obj->item_type == ITEM_TOOLKIT)
                    checktool = TRUE;
                  if (obj->item_type == ITEM_LENS)
                    checklens = TRUE;
                  if (obj->item_type == ITEM_CRYSTAL)
                    checkgems = TRUE;                    
                  if (obj->item_type == ITEM_MIRROR)
                    checkmirr = TRUE;
                  if (obj->item_type == ITEM_DURAPLAST || obj->item_type == ITEM_DURASTEEL )
          	    checkdura = TRUE;
                  if (obj->item_type == ITEM_BATTERY)
                    checkbatt = TRUE;
                  if (obj->item_type == ITEM_OVEN)
                    checkoven = TRUE;
                  if (obj->item_type == ITEM_CIRCUIT)
                    checkcirc = TRUE;
                  if (obj->item_type == ITEM_SUPERCONDUCTOR)
                    checkcond = TRUE;                  
                }
                
                if ( !checktool )
                {
                   send_to_char( "&RYou need toolkit to make a lightsaber.\n\r", ch);
                   return;
                }
 
                if ( !checkdura )
                {
                   send_to_char( "&RYou need something to make it out of.\n\r", ch);
                   return;
                }

                if ( !checkbatt )
                {
                   send_to_char( "&RYou need a power source for your lightsaber.\n\r", ch);
                   return;
                }
                
                if ( !checkoven )
                {
                   send_to_char( "&RYou need a small furnace to heat and shape the components.\n\r", ch);
                   return;
                }
                
                if ( !checkcirc )
                {
                   send_to_char( "&RYou need a small circuit board.\n\r", ch);
                   return;
                }
                
                if ( !checkcond )
                {
                   send_to_char( "&RYou still need a small superconductor for your lightsaber.\n\r", ch);
                   return;
                }
                
                if ( !checklens )
                {
                   send_to_char( "&RYou still need a lens to focus the beam.\n\r", ch);
                   return;
                }
                
                if ( !checkgems )
                {
                   send_to_char( "&RLightsabers require 1 to 3 gems to work properly.\n\r", ch);
                   return;
                }
                
                if ( !checkmirr )
                {
                   send_to_char( "&RYou need a high intesity reflective cup to create a lightsaber.\n\r", ch);
                   return;
                }
 
    	        chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_makeduallightsaber]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GYou begin the long process of crafting a lightsaber.\n\r", ch);
    		   act( AT_PLAIN, "$n takes $s tools and a small oven and begins to work on something.", ch,
		        NULL, argument , TO_ROOM );
		   add_timer ( ch , TIMER_DO_FUN , 25 , do_makeduallightsaber , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   return;
	        }
	        send_to_char("&RYou can't figure out how to fit the parts together.\n\r",ch);
	        learn_from_failure( ch, gsn_makeduallightsaber );
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
    
    level = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makeduallightsaber]);
    vnum = 72;
    
    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
         send_to_char( "&REl objeto que tratas de crear no está en la base de datos.\n\rPor favor, notifícalo urgentemente a la administración.\n\r", ch );
         return;
    }

    checktool = FALSE;
    checkdura = FALSE;
    checkbatt = FALSE;
    checkoven = FALSE;
    checkcond = FALSE;
    checkcirc = FALSE;
    checklens = FALSE;
    checkgems = FALSE;
    checkmirr = FALSE;
    gems = 0;
    charge = 0;
    gemtype =0;

    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
       if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;
       if (obj->item_type == ITEM_OVEN)
          checkoven = TRUE;
       if ( (obj->item_type == ITEM_DURAPLAST || obj->item_type == ITEM_DURASTEEL) && checkdura == FALSE)
       {
          checkdura = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
       }
       if (obj->item_type == ITEM_DURASTEEL && checkdura == FALSE)
       {
          checkdura = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
       }
       if (obj->item_type == ITEM_BATTERY && checkbatt == FALSE)
       {
          charge = UMIN(obj->value[1], 10);
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkbatt = TRUE;
       }
       if (obj->item_type == ITEM_SUPERCONDUCTOR && checkcond == FALSE)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkcond = TRUE;
       }
       if (obj->item_type == ITEM_CIRCUIT && checkcirc == FALSE)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkcirc = TRUE;
       }
       if (obj->item_type == ITEM_LENS && checklens == FALSE)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checklens = TRUE;
       }
       if (obj->item_type == ITEM_MIRROR && checkmirr == FALSE)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkmirr = TRUE;
       }
       if (obj->item_type == ITEM_CRYSTAL && gems < 3)
       {
          gems++;
          if ( gemtype < obj->value[0] )
             gemtype = obj->value[0];
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkgems = TRUE;
       }
    }                            
    
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makeduallightsaber]) ;
                
    if ( number_percent( ) > chance*2  || ( !checktool ) || ( !checkdura ) || ( !checkbatt ) || ( !checkoven ) 
                                       || ( !checkmirr ) || ( !checklens ) || ( !checkgems ) || ( !checkcond ) || ( !checkcirc) )
    
    {
       send_to_char( "&RYou hold up your new lightsaber and press the switch hoping for the best.\n\r", ch);
       send_to_char( "&RInstead of a blade of light, smoke starts pouring from the handle.\n\r", ch);
       send_to_char( "&RYou drop the hot handle and watch as it melts on away on the floor.\n\r", ch);
       learn_from_failure( ch, gsn_makeduallightsaber );
       return;
    }

    obj = create_object( pObjIndex, level );
    
    obj->item_type = ITEM_WEAPON;
    SET_BIT( obj->wear_flags, ITEM_WIELD );
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    SET_BIT( obj->extra_flags, ITEM_ANTI_SOLDIER );
    SET_BIT( obj->extra_flags, ITEM_ANTI_THIEF );
    SET_BIT( obj->extra_flags, ITEM_ANTI_HUNTER );
    SET_BIT( obj->extra_flags, ITEM_ANTI_PILOT );
    SET_BIT( obj->extra_flags, ITEM_ANTI_CITIZEN );
    obj->level = level;
    obj->weight = 5;
    STRFREE( obj->name );
    obj->name = STRALLOC( "lightsaber saber" );
    strcpy( buf, arg );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );        
    STRFREE( obj->description );
    strcat( buf, " was carelessly misplaced here." );
    obj->description = STRALLOC( buf );
    STRFREE( obj->action_desc );
    strcpy( buf, arg );
    strcat( buf, " ignites with a hum and a soft glow." );
    obj->action_desc = STRALLOC( buf );
    CREATE( paf, AFFECT_DATA, 1 );
    paf->type               = -1;
    paf->duration           = -1;
    paf->location           = get_atype( "hitroll" );
    paf->modifier           = URANGE( 0, gems, level/8 );
    paf->bitvector          = 0;
    paf->next               = NULL;
    LINK( paf, obj->first_affect, obj->last_affect, next, prev );
    ++top_affect;
    CREATE( paf2, AFFECT_DATA, 1 );
    paf2->type               = -1;
    paf2->duration           = -1;
    paf2->location           = get_atype( "parry" );
    paf2->modifier           = ( 100 );
    paf2->bitvector          = 0;
    paf2->next               = NULL;
    LINK( paf2, obj->first_affect, obj->last_affect, next, prev );
    ++top_affect;
    obj->value[0] = INIT_WEAPON_CONDITION;       /* condition  */
    obj->value[1] = (int) (level/10+gemtype*2);      /* min dmg  */
    obj->value[2] = (int) (level/5+gemtype*6);      /* max dmg */
    obj->value[3] = WEAPON_DUAL_LIGHTSABER;
    obj->value[4] = charge;
    obj->value[5] = charge;
    obj->cost = obj->value[2]*75;
                                                                    
    obj = obj_to_char( obj, ch );
                                                            
    send_to_char( "&GYou finish your work and hold up your newly created lightsaber.&w\n\r", ch);
    act( AT_PLAIN, "$n finishes making $s new lightsaber.", ch,
         NULL, argument , TO_ROOM );
    
    {
         long xpgain;
         
         xpgain = UMIN( obj->cost*50 ,( exp_level(ch->skill_level[FORCE_ABILITY]+1) - exp_level(ch->skill_level[ENGINEERING_ABILITY]) ) );
         gain_exp(ch, xpgain, FORCE_ABILITY);
         ch_printf( ch , "You gain %d force experience.", xpgain );
    }
        learn_from_success( ch, gsn_makeduallightsaber );
}

void do_makepike( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int level, chance, charge;
    bool checktool, checkdura, checkbatt, checkoven; 
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;
    AFFECT_DATA *paf;

    strcpy( arg , argument );
    
    switch( ch->substate )
    { 
    	default:
    	        
    	        if ( arg[0] == '\0' )
                {
                  send_to_char( "&Rsintaxis: fabricarpica <nombre>\n\r&w", ch);
                  return;   
                }
 
    	        checktool = FALSE;
                checkdura = FALSE;
                checkbatt = FALSE;
                checkoven = FALSE;
        
                if ( !IS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
                {
                   send_to_char( "&Rnecesitas estar en una fábrica o taller para hacer eso.\n\r", ch);
                   return;
                }
                
                for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
                {
                  if (obj->item_type == ITEM_TOOLKIT)
                    checktool = TRUE;
                  if (obj->item_type == ITEM_DURASTEEL)
          	    checkdura = TRUE;
                  if (obj->item_type == ITEM_BATTERY)
                  checkbatt = TRUE;

                  if (obj->item_type == ITEM_OVEN)
                  checkoven = TRUE;                  
                }
                
                if ( !checktool )
                {
                   send_to_char( "&Rnecesitas una caja de herramientas.\n\r", ch);
                   return;
                }
 
                if ( !checkdura )
                {
                   send_to_char( "&RNecesitas algo de duracero.\n\r", ch);
                   return;
                }

                if ( !checkbatt )
                {
                   send_to_char( "&RNecesitas una batería.\n\r", ch);
                   return;
                }
                
                if ( !checkoven )
                {
                   send_to_char( "&RNecesitas un horno.\n\r", ch);
                   return;
                }
 
    	        chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_makepike]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GComienzas el proceso de construir una pica de fuerza.\n\r", ch);
    		   act( AT_PLAIN, "$n usa su caja de herramientas y un horno y comienza a trabajar en algo.", ch,
		        NULL, argument , TO_ROOM );
		   add_timer ( ch , TIMER_DO_FUN , 30 , do_makepike , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   return;
	        }
	        send_to_char("&RNo consigues juntar todas las piezas.\n\r",ch);
	        learn_from_failure( ch, gsn_makepike );
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

    level = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makepike]);
    vnum = 74;
    
    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
         send_to_char( "&REl objeto que tratas de crear no está en la base de datos.\n\rPor favor, notifícalo urgentemente a la administración.\n\r", ch );
         return;
    }

    checktool = FALSE;
    checkdura = FALSE;
    checkbatt = FALSE;
    checkoven = FALSE;
    
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
       if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;
       if (obj->item_type == ITEM_OVEN)
          checkoven = TRUE;
       if (obj->item_type == ITEM_DURASTEEL && checkdura == FALSE)
       {
          checkdura = TRUE;
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
       }
       if (obj->item_type == ITEM_BATTERY && checkbatt == FALSE )
       {
          charge = UMAX( 5, obj->value[0] ); 
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkbatt = TRUE;
       }
    }                            
    
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makepike]) ;
                
    if ( number_percent( ) > chance*2  || ( !checktool ) || ( !checkdura ) || ( !checkbatt ) || ( !checkoven ) )
    {
       send_to_char( "&RActivas tu nueva pica de fuerza.\n\r", ch);
       send_to_char( "&RLa pica de fuerza se rompe en pedazos.\n\r", ch);
       learn_from_failure( ch, gsn_makepike );
       return;
    }

    obj = create_object( pObjIndex, level );
    
    obj->item_type = ITEM_WEAPON;
    SET_BIT( obj->wear_flags, ITEM_WIELD );
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    obj->level = level;
    obj->weight = 3;
    STRFREE( obj->name );
    strcpy( buf, arg );
    strcat( buf, " pica fuerza" );
    strcat( buf, remand(buf));
    obj->name = STRALLOC( buf );
    strcpy( buf, arg );
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( buf );        
    STRFREE( obj->description );
    strcat( buf, " está aquí." );
    CREATE( paf, AFFECT_DATA, 1 );
    paf->type               = -1;
    paf->duration           = -1;
    paf->location           = get_atype( "parry" );
    paf->modifier           = level/3;
    paf->bitvector          = 0;
    paf->next               = NULL;
    LINK( paf, obj->first_affect, obj->last_affect, next, prev );
    ++top_affect;
    obj->description = STRALLOC( buf );
    obj->value[0] = INIT_WEAPON_CONDITION;      
    obj->value[1] = (int) (level/10+10);      /* min dmg  */
    obj->value[2] = (int) (level/2+20);      /* max dmg */
    obj->value[3] = WEAPON_FORCE_PIKE;
    obj->value[4] = charge;
    obj->value[5] = charge;
    obj->cost = obj->value[2]*10;
                                                                    
    obj = obj_to_char( obj, ch );
                                                            
    send_to_char( "&GFinalizas tu trabajo y admiras tu nueva pica de fuerza.&w\n\r", ch);
    act( AT_PLAIN, "$n termina de construir una pica de fuerza.", ch,
         NULL, argument , TO_ROOM );
    
    {
         long xpgain;
         
         xpgain = UMIN( obj->cost*200 ,( exp_level(ch->skill_level[ENGINEERING_ABILITY]+1) - exp_level(ch->skill_level[ENGINEERING_ABILITY]) ) );
         gain_exp(ch, xpgain, ENGINEERING_ABILITY);
         ch_printf( ch , "Recibes %d puntos de experiencia en ingeniero.", xpgain );
    }
        
    learn_from_success( ch, gsn_makepike );
}

void do_makebug( CHAR_DATA *ch, char *argument )
{
    int level, chance;
    bool checktool,checkbatt,checkcirc;
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;

    switch( ch->substate )
    { 
    	default:
    	        checktool  = FALSE;
                checkbatt  = FALSE;
                checkcirc  = FALSE;

                if ( !IS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
                {
                   send_to_char( "&Rnecesitas estar en una fábrica o taller para hacer eso.\n\r", ch);
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
                }
                
                if ( !checktool )
                {
                   send_to_char( "&RNecesitas una caja de herramientas para fabricar un bicho.\n\r", ch);
                   return;
                }

                if ( !checkbatt )
                {
                   send_to_char( "&RNecesitas una pequeña batería.\n\r", ch);
                   return;
                }
                
                if ( !checkcirc )
                {
                   send_to_char( "&RNecesitarás algún circuito.\n\r", ch);
                   return;
                }
                
 
    	        chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_makebug]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GComienzas el proceso de fabricar un bicho.\n\r", ch);
    		   act( AT_PLAIN, "$n coge sus herramientas y comienza a trabajar en algo.", ch,
		        NULL, argument , TO_ROOM );
		   add_timer ( ch , TIMER_DO_FUN , 15 , do_makebug , 1 );
    		   ch->dest_buf   = str_dup("blah");
    		   return;
	        }
	        send_to_char("&RNo estás realmente seguro de como hacer esto...\n\r",ch);
	        learn_from_failure( ch, gsn_makebug );
    	   	return;	
    	
    	case 1: 
    		if ( !ch->dest_buf )
    		     return;
			ch->dest_buf = str_dup("blah");
    		DISPOSE( ch->dest_buf);
    		break;
    		
    	case SUB_TIMER_DO_ABORT:
    		DISPOSE( ch->dest_buf );
    		ch->substate = SUB_NONE;    		                                   
    	        send_to_char("&RTe distraes y no terminas tu trabajo.\n\r", ch);
    	        return;
    }
    
    ch->substate = SUB_NONE;
    
    level = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makebug]);
    vnum = 77;
    
    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
         send_to_char( "&REl objeto que tratas de crear no está en la base de datos.\n\rPor favor, notifícalo urgentemente a la administración.\n\r", ch );
         return;
    }

    checktool = FALSE;
    checkbatt = FALSE;
    checkcirc = FALSE;
    
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
       if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;

       if (obj->item_type == ITEM_BATTERY && checkbatt == FALSE)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkbatt = TRUE;
       }

       if (obj->item_type == ITEM_CIRCUIT && checkcirc == FALSE)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkcirc = TRUE;
       }
    }                            
    
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makebug]) ;
                
    if ( number_percent( ) > chance*2  || ( !checktool )  || ( !checkbatt ) || ( !checkcirc) )
    {
       send_to_char( "&RFinalizas y activas el bicho, pero lamentablemente no parece funcionar.\n\r", ch);
       learn_from_failure( ch, gsn_makebug );
       return;
    }

    obj = create_object( pObjIndex, level );
    
    obj->item_type = ITEM_BUG;
    SET_BIT( obj->wear_flags, ITEM_HOLD );
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    obj->level = level;
    obj->weight = 1;
    STRFREE( obj->name );
    obj->name = STRALLOC("device bug");
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( "a small bug" );        
    STRFREE( obj->description );
    obj->description = STRALLOC( "A small electronic device está aquí." );
                                                                   
    obj = obj_to_char( obj, ch );
                                                            
    send_to_char( "&GFinalizas y activas el bicho. Funciona perfectamente.&w\n\r", ch);
    act( AT_PLAIN, "$n termina de fabricar un bicho.", ch,
         NULL, argument , TO_ROOM );
    
    {
         long xpgain;
         
         xpgain = number_range(1550, 2100);
         gain_exp(ch, xpgain, ENGINEERING_ABILITY);
         ch_printf( ch , "Recibes %d puntos de experiencia en ingeniero.", xpgain );
    }
        learn_from_success( ch, gsn_makebug );
}

void do_makebeacon( CHAR_DATA *ch, char *argument )
{
    int level, chance;
    bool checktool,checkbatt,checkcirc;
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;

    switch( ch->substate )
    { 
    	default:
    	        checktool  = FALSE;
                checkbatt  = FALSE;
                checkcirc  = FALSE;

                if ( !IS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
                {
                   send_to_char( "&Rnecesitas estar en una fábrica o taller para hacer eso.\n\r", ch);
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
                }
                
                if ( !checktool )
                {
                   send_to_char( "&RNecesitas una caja de herramientas.\n\r", ch);
                   return;
                }

                if ( !checkbatt )
                {
                   send_to_char( "&RNecesitas una batería.\n\r", ch);
                   return;
                }
                
                if ( !checkcirc )
                {
                   send_to_char( "&RNecesitas un circuito.\n\r", ch);
                   return;
                }
                
 
    	        chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_makebeacon]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GComienzas a construir un localizador.\n\r", ch);
    		   act( AT_PLAIN, "$n comienza a trabajar en algo con su caja de herramientas.", ch,
		        NULL, argument , TO_ROOM );
		   add_timer ( ch , TIMER_DO_FUN , 15 , do_makebeacon , 1 );
    		   ch->dest_buf   = str_dup("blah");
    		   return;
	        }
	        send_to_char("&RNo consigues hacer nada útil...\n\r",ch);
	        learn_from_failure( ch, gsn_makebeacon );
    	   	return;	
    	
    	case 1: 
    		if ( !ch->dest_buf )
    		     return;
			ch->dest_buf = str_dup("blah");
    		DISPOSE( ch->dest_buf);
    		break;
    		
    	case SUB_TIMER_DO_ABORT:
    		DISPOSE( ch->dest_buf );
    		ch->substate = SUB_NONE;    		                                   
    	        send_to_char("&RTe distraes y no terminas tu trabajo.\n\r", ch);
    	        return;
    }
    
    ch->substate = SUB_NONE;
    
    level = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makebeacon]);
    vnum = 78;
    
    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
         send_to_char( "&REl objeto que tratas de crear no está en la base de datos.\n\rPor favor, notifícalo urgentemente a la administración.\n\r", ch );
         return;
    }

    checktool = FALSE;
    checkbatt = FALSE;
    checkcirc = FALSE;
    
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
       if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;

       if (obj->item_type == ITEM_BATTERY && checkbatt == FALSE)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkbatt = TRUE;
       }

       if (obj->item_type == ITEM_CIRCUIT && checkcirc == FALSE)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkcirc = TRUE;
       }
    }                            
    
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makebeacon]) ;
                
    if ( number_percent( ) > chance*2  || ( !checktool )  || ( !checkbatt ) || ( !checkcirc) )
    {
       send_to_char( "&RFinalizas y activas el localizador, pero tristemente algo has hecho mal, pues no funciona.\n\r", ch);
       learn_from_failure( ch, gsn_makebeacon );
       return;
    }

    obj = create_object( pObjIndex, level );
    
    obj->item_type = ITEM_BEACON;
    SET_BIT( obj->wear_flags, ITEM_HOLD );
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    obj->level = level;
    obj->weight = 1;
    STRFREE( obj->name );
    obj->name = STRALLOC("dispositivo localizador");
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( "a locator beacon" );        
    STRFREE( obj->description );
    obj->description = STRALLOC( "Un pequeño dispositivo electrónico está aquí." );
                                                                   
    obj = obj_to_char( obj, ch );
                                                            
    send_to_char( "&GTerminas y activas el localizador. Funciona perfectamente.&w\n\r", ch);
    act( AT_PLAIN, "$n termina de construir un localizador.", ch,
         NULL, argument , TO_ROOM );
    
    {
         long xpgain;
         
         xpgain = number_range(2500, 3750);
         gain_exp(ch, xpgain, TECHNICIAN_ABILITY);
         ch_printf( ch , "Recibes %d puntos de experiencia en técnico.", xpgain );
    }
        learn_from_success( ch, gsn_makebeacon );
}

void do_plantbeacon( CHAR_DATA *ch, char *argument )
{
 SHIP_DATA *ship;
 OBJ_DATA *obj;
 bool checkbeacon = FALSE;
 char buf[MAX_STRING_LENGTH];
	
	if(IS_NPC(ch)) return;
	if(argument[0] == '\0')
	{
	 send_to_char("sintaxis: plantarlocalizador <nave>\n\r", ch);
	 return;
	}

	if ((ship = ship_in_room(ch->in_room, argument)) == NULL){
		send_to_char("Esa nave no está aquí.\n\r", ch);
		return;
	}
	if(is_name(ch->name, ship->pbeacon)){
	  	send_to_char("Ya le has puesto un localizador a esa nave.\n\r", ch);
	  	return;
	  }

        for(obj = ch->last_carrying; obj; obj = obj->prev_content)
	 if(obj->item_type == ITEM_BEACON)
	  checkbeacon = TRUE;
        if(checkbeacon == FALSE)
	{
	 send_to_char("No tienes localizador.\n\r", ch);
	 return;
	}

    if (number_percent() < ch->pcdata->learned[gsn_plantbeacon]){
      sprintf(buf, "colocas un localizador en el casco de %s.\n\r", ship->name);
	  send_to_char(buf, ch);
      sprintf(buf, "%s coloca un localizador en el casco de %s.", ch->name, ship->name);
	  act(AT_PLAIN, buf, ch, NULL, NULL, TO_ROOM);
	  sprintf(buf, "%s %s", ship->pbeacon, ch->name);
	  STRFREE(ship->pbeacon);
	  ship->pbeacon = STRALLOC(buf);
	  save_ship(ship);
	
        for(obj = ch->last_carrying; obj; obj = obj->prev_content)
        {
			if(obj->item_type == ITEM_BEACON)
			{
			 separate_obj(obj); obj_from_char(obj); extract_obj(obj);
			 break;
			}
		}

      learn_from_success(ch, gsn_plantbeacon);
        	return;
        }
        else {
        	send_to_char("&RNo consigues colocar el localizador.\n\r", ch);
        	learn_from_failure(ch, gsn_plantbeacon);
        	return;
        }
}

void do_showbeacons( CHAR_DATA *ch, char *argument )
{
  SHIP_DATA *ship;
  SHIP_DATA *ship2;
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  char buf3[MAX_STRING_LENGTH];
  int count = 0;

  if(IS_NPC(ch)) return;
  if (number_percent() > ch->pcdata->learned[gsn_showbeacons])
  {
  	send_to_char("Fallaste.\n\r", ch);
	learn_from_failure(ch,gsn_showbeacons);
  	return;
  }
  
  send_to_char("\n\r",ch);
  send_to_char("&zSolicitando respuesta de localizadores activos...\n\r", ch);
  send_to_char("&w__________________________________________________\n\r\n\r",ch);
  
  for ( ship = first_ship; ship; ship = ship->next )
  {
   if(is_name(ch->name, ship->pbeacon))
   {
    if(!ship->in_room && ship->shipstate != SHIP_HYPERSPACE)
     sprintf(buf3, "&w%.0f&z, &w%.0f&z, &w%.0f", ship->vx, ship->vy, ship->vz);
    if(ship->shipstate == SHIP_HYPERSPACE)
     sprintf(buf3, "en el hiperespacio");

    for(ship2 = first_ship; ship2; ship2 = ship2->next)
    {
	if(ship->in_room)
	{
     if(ship->in_room->vnum == ship2->hanger1)
     {
      sprintf(buf, "%s", ship2->name);
      sprintf(buf2, "%s", ship->in_room->name);
	  break;
     }
     if(ship->in_room->vnum == ship2->hanger2)
     {
      sprintf(buf, "%s", ship2->name);
      sprintf(buf2, "%s", ship->in_room->name);
	  break;
     }
     if(ship->in_room->vnum == ship2->hanger3)
     {
      sprintf(buf, "%s", ship2->name);
      sprintf(buf2, "%s", ship->in_room->name);
	  break;
     }
     if(ship->in_room->vnum == ship2->hanger4)
     {
      sprintf(buf, "%s", ship2->name);
      sprintf(buf2, "%s", ship->in_room->name);
	  break;
     }
    }
   }

  ch_printf(ch,"^g&xActivo^x&z: &w%-15.15s&z Localización:&w %s &z(&w%s&z)&w\n\r", ship->name,

   (ship->in_room && ship->in_room->area->planet) ?
     ship->in_room->area->planet->name :
   (ship->in_room && !ship->in_room->area->planet) ?
     buf :
   (!ship->in_room && ship->starsystem) ? ship->starsystem->name :
   ship->shipstate == SHIP_HYPERSPACE ? "Unknown" : "Desconocida",

   (ship->in_room && ship->in_room->area->planet) ?
     ship->in_room->name :
   (ship->in_room && !ship->in_room->area->planet) ?
     buf2 :
   ship->shipstate == SHIP_HYPERSPACE ? "In Hyperspace" :
   (!ship->in_room) ? buf3 : "Desconocida");
   count++;
   }
  }
   if(count == 0)
    send_to_char("            &zNo se han encontrado localizadores.\n\r", ch);
    learn_from_success(ch,gsn_showbeacons);
}

void do_checkbeacons( CHAR_DATA *ch, char *argument )
{
 SHIP_DATA *ship;
 char arg[MAX_STRING_LENGTH];
 int chance;
 
 if(IS_NPC(ch)) return;

 argument = one_argument( argument, arg ); 
 
switch( ch->substate )
{
  default:

   if( arg[0] == '\0')
   {
     send_to_char("Sintaxis: comprobarlocalizadores <nave>\n\r", ch);
     return;
   }

   if((ship = ship_in_room(ch->in_room, arg)) == NULL)
   {
     send_to_char("Esa nave no está aquí.\n\r", ch);
     return;
   }
   chance = IS_NPC(ch) ? ch->top_level : (int)(ch->pcdata->learned[gsn_checkbeacons]);

   if(number_percent() - 20 < chance)
   {
    send_to_char("&w&GUsas un escáner para comprobar si la nave tiene localizadores.\n\r", ch);
    act( AT_PLAIN, "$n utiliza un escáner.", ch, NULL, NULL, TO_ROOM );
    add_timer ( ch , TIMER_DO_FUN , 6 , do_checkbeacons , 1 );
    ch->dest_buf = str_dup(arg);
    return;
   }
   send_to_char("Pulsas botones al azar, no tienes claro qué estás haciendo.\n\r", ch);
   learn_from_failure(ch, gsn_checkbeacons);
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
  	send_to_char("&RTe distraes y no terminas el escaneo.\n\r", ch);
  	return;

}

    ch->substate = SUB_NONE;

   if((ship = ship_in_room(ch->in_room, arg)) == NULL)
   {
    send_to_char("La nave ha despegado antes de que puedas completar el escaneo.\n\r", ch);
    return;
   }

   if(ship->pbeacon[0] == '\0')
	send_to_char("&w&GEscáner completado. No se han detectado señales.\n\r", ch);
   else
	send_to_char("&w&GEscáner completado. Has detectado una señal extraña...\n\r", ch);

   learn_from_success(ch, gsn_checkbeacons);
    {
         long xpgain;

         xpgain = (ch->experience[TECHNICIAN_ABILITY]/30);
         gain_exp(ch, xpgain, TECHNICIAN_ABILITY);
         ch_printf( ch , "Recibes %d puntos de experiencia en técnico.", xpgain );
    } 

return;
}

void do_nullifybeacons(CHAR_DATA *ch, char *argument)
{
 SHIP_DATA *ship;
 char arg[MAX_STRING_LENGTH];
 int chance;

 if(IS_NPC(ch)) return;
 argument = one_argument( argument, arg ); 

switch( ch->substate )
{
  default:

   if( arg[0] == '\0')
   {
     send_to_char("Sintaxis: quitarlocalizadores <nave>\n\r", ch);
     return;
   }

   if((ship = ship_in_room(ch->in_room, arg)) == NULL)
   {
     send_to_char("Esa nave no está aquí.\n\r", ch);
     return;
   }

   chance = IS_NPC(ch) ? ch->top_level : (int)(ch->pcdata->learned[gsn_nullifybeacons]);

   if(number_percent() < chance + 20)
   {
    send_to_char("&w&GColocas un pequeño dispositivo en la nave e introduces un par de órdenes.\n\r", ch);
    act( AT_PLAIN, "$n coloca un pequeño dispositivo en una nave e introduce un par de órdenes.", ch, NULL, NULL, TO_ROOM );
    add_timer ( ch , TIMER_DO_FUN , 1 , do_nullifybeacons , 1 );
    ch->dest_buf = str_dup(arg);
    return;
   }
   send_to_char("Fallaste.\n\r", ch);
   learn_from_failure(ch, gsn_nullifybeacons);
   return;

  case 1:
 	if ( !ch->dest_buf )
       		return;
	 strcpy(arg, ch->dest_buf);
 	 DISPOSE( ch->dest_buf);
	 ch->dest_buf = NULL;
	 break;
}

    ch->substate = SUB_NONE;

 chance = IS_NPC(ch) ? ch->top_level : (int)(ch->pcdata->learned[gsn_nullifybeacons]);

   if((ship = ship_in_room(ch->in_room, arg)) == NULL)
   {
    send_to_char("La nave ha despegado antes de que pudieras terminar tu trabajo.\n\r", ch);
    return;
   }

   STRFREE(ship->pbeacon);
   ship->pbeacon = STRALLOC("");
   save_ship(ship);
   send_to_char("&w&GEl dispositivo emite un largo pitido.\n\r", ch);
   send_to_char("&GMirando el escáner te das cuenta que la nave no tiene localizadores.\n\r", ch);
   send_to_char("&wRecuperas el dispositivo.\n\r", ch);
   act(AT_PLAIN, "Un dispositivo de una nave emite un largo pitido.\n\r", ch, NULL, NULL, TO_ROOM);
   act(AT_PLAIN, "$n recupera el dispositivo y mira el escáner.\n\r", ch, NULL, NULL, TO_ROOM);
   learn_from_success( ch, gsn_nullifybeacons);
    {
         long xpgain;

         xpgain = (ch->experience[TECHNICIAN_ABILITY]/25);
         gain_exp(ch, xpgain, TECHNICIAN_ABILITY);
         ch_printf( ch , "Recibes %d puntos de experiencia experiencia en técnico.", xpgain );
    } 

return;
}

void do_makebinders( CHAR_DATA *ch, char *argument )
{
    int level, chance;
    bool checktool,checkoven,checkdura;
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;

    switch( ch->substate )
    { 
    	default:
    	        checktool  = FALSE;
                checkdura  = FALSE;
                checkoven  = FALSE;

                if ( !IS_SET( ch->in_room->room_flags, ROOM_FACTORY ) )
                {
                   send_to_char( "&Rnecesitas estar en una fábrica o taller para hacer eso.\n\r", ch);
                   return;
                }
                
                for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
                {
                  if (obj->item_type == ITEM_TOOLKIT)
                    checktool = TRUE;
                  if (obj->item_type == ITEM_OVEN)
                    checkoven = TRUE;
                  if (obj->item_type == ITEM_DURASTEEL)
                    checkdura = TRUE;                  
                }
                
                if ( !checktool )
                {
                   send_to_char( "&RYou need a toolkit.\n\r", ch);
                   return;
                }

                if ( !checkdura )
                {
                   send_to_char( "&RYou'll need some durasteel to mold.\n\r", ch);
                   return;
                }
                
                if ( !checkoven )
                {
                   send_to_char( "&RYou're going to need an oven to heat the durasteel.\n\r", ch);
                   return;
                }
                
 
    	        chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_makebinders]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GYou begin the process of making a pair of binders.\n\r", ch);
    		   act( AT_PLAIN, "$n takes $s tools and begins to work on something.", ch,
		        NULL, argument , TO_ROOM );
		   add_timer ( ch , TIMER_DO_FUN , 15 , do_makebinders , 1 );
    		   ch->dest_buf   = str_dup("blah");
    		   return;
	        }
	        send_to_char("&RYou're not quite sure how to do it...\n\r",ch);
	        learn_from_failure( ch, gsn_makebinders );
    	   	return;	
    	
    	case 1: 
    		if ( !ch->dest_buf )
    		     return;
			ch->dest_buf = str_dup("blah");
    		DISPOSE( ch->dest_buf);
    		break;
    		
    	case SUB_TIMER_DO_ABORT:
    		DISPOSE( ch->dest_buf );
    		ch->substate = SUB_NONE;    		                                   
    	        send_to_char("&RTe distraes y no terminas tu trabajo.\n\r", ch);
    	        return;
    }
    
    ch->substate = SUB_NONE;
    
    level = IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_makebinders]);
    vnum = 79;
    
    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
         send_to_char( "&REl objeto que tratas de crear no está en la base de datos.\n\rPor favor, notifícalo urgentemente a la administración.\n\r", ch );
         return;
    }

    checktool = FALSE;
    checkoven = FALSE;
    checkdura = FALSE;
    
    for ( obj = ch->last_carrying; obj; obj = obj->prev_content )     
    {
       if (obj->item_type == ITEM_TOOLKIT)
          checktool = TRUE;
		if(obj->item_type == ITEM_OVEN)
			checkoven = TRUE;
       if (obj->item_type == ITEM_DURASTEEL && checkdura == FALSE)
       {
          separate_obj( obj );
          obj_from_char( obj );
          extract_obj( obj );
          checkdura = TRUE;
       }
    }                            
    
    chance = IS_NPC(ch) ? ch->top_level
                : (int) (ch->pcdata->learned[gsn_makebinders]) ;
                
    if ( number_percent( ) > chance*2  || ( !checktool )  || ( !checkdura ) || ( !checkoven) )
    {
       send_to_char( "&RYou finish making your binders, but the locking mechanism doesn't work.\n\r", ch);
       learn_from_failure( ch, gsn_makebinders );
       return;
    }

    obj = create_object( pObjIndex, level );
    
    obj->item_type = ITEM_BINDERS;
    SET_BIT( obj->wear_flags, ITEM_HOLD );
    SET_BIT( obj->wear_flags, ITEM_TAKE );
    obj->level = level;
    obj->weight = 1;
    STRFREE( obj->name );
    obj->name = STRALLOC("binders");
    STRFREE( obj->short_descr );
    obj->short_descr = STRALLOC( "&Ya pair of binders&w" );        
    STRFREE( obj->description );
    obj->description = STRALLOC( "A pair of binders was discarded here." );
                                                                   
    obj = obj_to_char( obj, ch );
                                                            
    send_to_char( "&GYou finish constructing a pair of binders.&w\n\r", ch);
    act( AT_PLAIN, "$n finishes making a pair of binders.", ch,
         NULL, argument , TO_ROOM );
    
    {
         long xpgain;
         
         xpgain = number_range(2500, 3750); 
         gain_exp(ch, xpgain, ENGINEERING_ABILITY);
         ch_printf( ch , "Recibes %d puntos de experiencia en ingeniero.", xpgain );
    }
        learn_from_success( ch, gsn_makebinders );
}


void do_setinfrared( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if(!IS_DROID(ch) || IS_NPC(ch))
    {
	send_to_char("You turn your 'infrared knob'.. and, well, uh..Hm.\n\r", ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Setinfrared ON or OFF?\n\r", ch );
	return;
    }

    if ( (strcmp(arg,"on")==0) || (strcmp(arg,"ON") == 0) ) {
	SET_BIT(ch->affected_by,AFF_INFRARED);
	send_to_char( "You activate your infrared vision unit.\n\r", ch);
	return;
    }

    if ( (strcmp(arg,"off")==0) || (strcmp(arg,"OFF") == 0) ) {
	REMOVE_BIT(ch->affected_by, AFF_INFRARED);
	send_to_char( "You deactivate your infrared vision unit.\n\r", ch);
	return;
    }
}


void do_battle_command(CHAR_DATA *ch, char *argument)
{
 CHAR_DATA *gch;
 AFFECT_DATA af;
 int chance;

 if( is_affected( ch, gsn_battle_command ))
 {
   send_to_char("&RYa tienes el mando del grupo.\n\r", ch);
   return;
 }

 chance = ch->pcdata->learned[gsn_battle_command];

 if(number_percent() > chance)
 {
   send_to_char("&RTratas de tomar el mando del grupo pero te ignoran.\n\r", ch);
   learn_from_failure(ch, gsn_battle_command);
   return;
 }

 for ( gch = ch->in_room->first_person; gch; gch = gch->next_in_room )
 {
   if ( is_same_group( gch, ch ))
   {
	if( is_affected( gch, gsn_battle_command ))
	  continue;

	if(gch != ch)
  	  ch_printf(gch, "&G%s toma el mando del grupo, te sientes más seguro y fuerte en batalla.\n\r", PERS(ch, gch));
	else
	  send_to_char("&GTomas el mando del grupo, te sientes más fuerte en la batalla.\n\r", ch);

        af.type      = gsn_battle_command;
        af.duration  = (number_fuzzy( (ch->pcdata->learned[gsn_battle_command] + 1) / 3 ) + 1) * DUR_CONV;
        af.location  = APPLY_AC;
        af.modifier  = -(ch->pcdata->learned[gsn_battle_command]/2);
        af.bitvector = 0;
        affect_to_char( gch, &af );
	af.location = APPLY_HIT;
	af.modifier = ch->pcdata->learned[gsn_battle_command]/2;
	affect_to_char( gch, &af );
   }
 }
 learn_from_success(ch, gsn_battle_command);

return; 
}
