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

void do_plantbug( CHAR_DATA *ch, char *argument )
{
 CHAR_DATA *victim;
 BUG_DATA *bug;
 BUG_DATA *cbug;
 OBJ_DATA *obj;
 bool checkbug = FALSE;
 int chance;
	
	if(IS_NPC(ch)) return;
	
	if ((victim = get_char_room(ch, argument)) == NULL){
		send_to_char("No está aquí.\n\r", ch);
		return;
	}

	if (IS_NPC(victim))
	{
	  send_to_char("¡No en mobs!\n\r", ch);
	  return;
	}

        if (IS_IMMORTAL(victim))
	{
	  send_to_char("Es inmortal, no fastidies.\n\r", ch);
	  return;
	} 

	if(in_arena(ch))
	{
	  send_to_char("Termina el combate primero.\n\r", ch);
	  return;
	}

        if( ch == victim)
	{
	  send_to_char("¿Para qué?\n\r", ch);
	  return;
	}

        for(obj = ch->last_carrying; obj; obj = obj->prev_content)
	 if(obj->item_type == ITEM_BUG)
	  checkbug = TRUE;
	
        if(checkbug == FALSE)
	{
	 send_to_char("No tienes bichos.\n\r", ch);
	 return;
        }

	for(cbug=victim->first_bug;cbug;cbug=cbug->next_in_bug)
	  if(!str_cmp(ch->name, cbug->name)){
	  	send_to_char("Ya le has puesto un bicho.\n\r", ch);
	  	return;
	  }
	
	chance = number_percent( ) - UMIN(0,(get_curr_lck(ch) - 14)) 
        + UMIN(0,(get_curr_lck(victim) - 13));
        
        if (chance < ch->pcdata->learned[gsn_plantbug])
        {
        	act(AT_WHITE, "Cuidadosamente metes un bicho en el bolsillo de $N.", ch, NULL, victim, TO_CHAR);
        	CREATE(bug, BUG_DATA, 1);
        	bug->name = STRALLOC(ch->name);
        	LINK(bug, victim->first_bug, victim->last_bug, next_in_bug, prev_in_bug);
        	learn_from_success(ch, gsn_plantbug);

	  for(obj = ch->last_carrying; obj; obj = obj->prev_content)
	  {
	   if(obj->item_type == ITEM_BUG)
	   {
	    separate_obj(obj); obj_from_char(obj); extract_obj(obj);
	    break;
	   }
          }
        	return;
        }
        else {
        	send_to_char("&RFallaste.\n\r", ch);
        	learn_from_failure(ch, gsn_plantbug);
		if(number_bits(0) == 0)
		  ch_printf(victim, "Sientes que rozan tu bolsillo y encuentras la mano de %s allí.\n\r", PERS(ch, victim));
        	return;
        }
}

void do_showbugs( CHAR_DATA *ch, char *argument ){
  DESCRIPTOR_DATA *d;
  CHAR_DATA *victim;
  int chance;
  char buf2[MAX_STRING_LENGTH];
  SHIP_DATA *ship;
  BUG_DATA *bug;
  char buf[MAX_STRING_LENGTH];
  chance = number_percent() - 20;
  if (chance > ch->pcdata->learned[gsn_showbugs])
  {
  	send_to_char("You can't figure out what to do.\n\r", ch);
	learn_from_failure(ch,gsn_showbugs);
  	return;
  }
  send_to_char("Player                Planet/Ship        Room Name\n\r", ch);
  send_to_char("------                -----------        ---------\n\r", ch);
    
  for ( d = first_descriptor; d; d = d->next )
    if ( (d->connected == CON_PLAYING || d->connected == CON_EDITING )
    && ( victim = d->character ) != NULL){
      for(bug=victim->first_bug;bug;bug=bug->next_in_bug)
        if(!str_cmp(bug->name, ch->name)){
          if(victim->in_room->area && victim->in_room->area->planet)
            sprintf(buf2, "%s", victim->in_room->area->planet->name);
          else if((ship = ship_from_room(victim->in_room->vnum)) != NULL)
            sprintf(buf2, "%s", ship->name);
          else
            sprintf(buf2, "Desconocido");  
          sprintf(buf, "%-21.21s %-18.18s %s\n\r", PERS(victim, ch), buf2, victim->in_room->name);
          send_to_char(buf, ch);
          break;
        }
    }
    learn_from_success(ch,gsn_showbugs);
}
		
void do_bind( CHAR_DATA *ch, char *argument )
{
  OBJ_DATA *obj;
  OBJ_DATA *tobj;
  int chance;
  CHAR_DATA *victim;
  bool checkbinders=FALSE;
  
  if (argument[0] == '\0')
  {
  	send_to_char("Syntax: Bind <victim>\n\r", ch);
  	return;
  }
  
  if ((victim = get_char_room(ch, argument)) == NULL)
  {
  	send_to_char("They are not here.\n\r", ch);
  	return;
  }

  if (victim == ch)
  {
  	send_to_char("You can not bind yourself!\n\r", ch);
  	return;
  }

  if (IS_NPC(victim))
  {
	send_to_char("You can only bind players.\n\r", ch);
	return;
  }
  
  if (IS_SET(victim->pcdata->act2, ACT_BOUND))
  {
  	send_to_char("They've already been bound!\n\r", ch);
  	return;
  }

  for(obj = ch->last_carrying; obj; obj = obj->prev_content)
   if(obj->item_type == ITEM_BINDERS)
   {
    checkbinders = TRUE;
    break;
   }

  if(checkbinders == FALSE)
  {
   send_to_char("You don't have any binders to bind them with.\n\r", ch);
   return;
  }

  if (victim->position != POS_STUNNED && victim->position != POS_SLEEPING)
  {
  	send_to_char("They need to be stunned or asleep.\n\r", ch);
  	return;
  }
  
  chance = (int) (ch->pcdata->learned[gsn_bind]);

  if( number_percent() < chance)
  {
  separate_obj(obj); obj_from_char(obj); obj_to_char(obj,victim);
  act(AT_WHITE, "You quickly bind $N's wrists.", ch, NULL, victim, TO_CHAR);
  act(AT_WHITE, "$n quickly binds your wrists.", ch, NULL, victim, TO_VICT);
  act(AT_WHITE, "$n quickly binds $N's wrists.", ch, NULL, victim, TO_NOTVICT);
  tobj = get_eq_char( ch, WEAR_BOTH_WRISTS );
  if(tobj)
    unequip_char(ch, tobj);

  equip_char(victim, obj, WEAR_BOTH_WRISTS);
  SET_BIT(victim->pcdata->act2, ACT_BOUND);
  learn_from_success(ch, gsn_bind);
  }
  else
  {
  send_to_char("You peer at the binders, curious upon how to use them.\n\r", ch);
  learn_from_failure(ch, gsn_bind);
  }

 return;
}

void do_unbind( CHAR_DATA *ch, char *argument )
{
  OBJ_DATA *obj;
  bool checkbinders=FALSE;
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  
  if (IS_NPC(ch))
  {
	send_to_char("You're a mob.\n\r", ch);
	return;
  }

  if (argument[0] == '\0')
  {
  	send_to_char("Syntax: Unbind <victim>\n\r", ch);
  	return;
  }
  
  if ((victim = get_char_room(ch, argument)) == NULL){
  	send_to_char("They aren't here.\n\r", ch);
  	return;
  }
  
  if (victim == ch){
  	send_to_char("You can not unbind yourself!\n\r", ch);
  	return;
  }
  
  if (IS_NPC(victim))
  {
	send_to_char("You can only unbind players.\n\r", ch);
	return;
  }

  if (IS_SET(ch->pcdata->act2, ACT_BOUND))
  {
  	send_to_char("Nice try. You're bound yourself!\n\r", ch);
  	return;
  }

  if (!IS_SET(victim->pcdata->act2, ACT_BOUND))
  {
  	send_to_char("But they're not bound.\n\r", ch);
  	return;
  }


  obj = get_eq_char( victim, WEAR_BOTH_WRISTS );
  if(obj)
    unequip_char(victim, obj);
  else
  {
    send_to_char("Something went wrong. get an imm.\n\r", ch);
    sprintf(buf, "%s unbinding %s: has no bothwrists object!", ch->name, victim->name);
    bug(buf);
    return;
  }

  for(obj = victim->last_carrying; obj; obj = obj->prev_content)
   if(obj->item_type == ITEM_BINDERS)
   {
    checkbinders = TRUE;
    break;
   }

  if(checkbinders == FALSE)
  {
   bug("Unbind: no binders in victims inventory.");
   send_to_char("Something went wrong. get an imm.\n\r", ch);
   return;
  }

  separate_obj(obj); obj_from_char(obj); obj_to_char(obj, ch);
  act(AT_WHITE, "You quickly unbind $N's wrists.", ch, NULL, victim, TO_CHAR);
  act(AT_WHITE, "$n quickly unbinds your wrists.", ch, NULL, victim, TO_VICT);
  act(AT_WHITE, "$n quickly unbinds $N's wrists.", ch, NULL, victim, TO_NOTVICT);
  REMOVE_BIT(victim->pcdata->act2, ACT_BOUND);
}  

void do_gag( CHAR_DATA *ch, char *argument )
{  
  CHAR_DATA *victim;
  int chance;

  if (argument[0] == '\0')
  {
  	send_to_char("Sintaxis: amordazar <víctima>\n\r", ch);
  	return;
  }
  
  if ((victim = get_char_room(ch, argument)) == NULL)
  {
  	send_to_char("No está aquí.\n\r", ch);
  	return;
  }

  if (victim == ch)
  {
  	send_to_char("¡Eso sería estúpido!\n\r", ch);
  	return;
  }
  
  if (IS_NPC(victim))
  {
	send_to_char("Solo puedes amordazar a jugadores.\n\r", ch);
	return;
  }

  if ((victim->position != POS_STUNNED) && (victim->position != POS_SLEEPING) && !IS_SET(victim->pcdata->act2, ACT_BOUND))
  {
  	send_to_char("La víctima debe estar inmovilizada, aturdida o dormida.\n\r", ch);
  	return;
  }

  chance = (int) (ch->pcdata->learned[gsn_gag]);

  if( number_percent() < chance)  
  {
   act(AT_WHITE, "Colocas una mordaza sobre la boca de $N.", ch, NULL, victim, TO_CHAR);
   act(AT_WHITE, "$n te pone una mordaza en la boca.", ch, NULL, victim, TO_VICT);
   act(AT_WHITE, "$n coloca una mordaza en la boca de $N.", ch, NULL, victim, TO_NOTVICT);
   SET_BIT(victim->pcdata->act2, ACT_GAGGED);
   learn_from_success(ch, gsn_gag);
  }
  else
  {
   send_to_char("Fallaste.\n\r",ch);
   learn_from_failure(ch, gsn_gag);
  }

  return;
}

void do_ungag( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  
  if (argument[0] == '\0')
  {
  	send_to_char("sintaxis: desamordazar <víctima>\n\r", ch);
  	return;
  }
  
  if ((victim = get_char_room(ch, argument)) == NULL)
  {
  	send_to_char("No está aquí.\n\r", ch);
  	return;
  }
  
  if (victim == ch && IS_SET(victim->pcdata->act2, ACT_BOUND))
  {
  	send_to_char("¡Estás inmóvil! ¡Es un poco complicado!\n\r", ch);
  	return;
  }
  
  if (IS_NPC(victim))
  {
	send_to_char("solo puedes hacerle esto a jugadores.\n\r", ch);
	return;
  }

  if (!IS_SET(victim->pcdata->act2, ACT_GAGGED)){
  	send_to_char("No lleva una mordaza.\n\r", ch);
  	return;
  }

  if( victim != ch)
  {
   act(AT_WHITE, "Le quitas la mordaza a $N.", ch, NULL, victim, TO_CHAR);
   act(AT_WHITE, "$n te quita la mordaza.", ch, NULL, victim, TO_VICT);
   act(AT_WHITE, "$n le quita la mordaza a $N.", ch, NULL, victim, TO_NOTVICT);
  }
  else
  {
   act(AT_WHITE, "Te quitas la mordaza.", ch, NULL, victim, TO_CHAR);
   act(AT_WHITE, "$n se quita la mordaza.", ch, NULL, victim, TO_NOTVICT);
  }

  REMOVE_BIT(victim->pcdata->act2, ACT_GAGGED);
}  
  
void do_ambush( CHAR_DATA *ch, char *argument ){
  CHAR_DATA *victim;
  int percent;
  
  if (argument[0] == '\0'){
  	send_to_char("sintaxis: emboscar <víctima>\n\r", ch);
  	return;
  }
  
  if (IS_NPC(ch)){
  	send_to_char("solo los jugadores pueden usar esta habilidad.\n\r", ch);
  	return;
  }
  
  if ((victim = get_char_room(ch, argument)) == NULL){
  	send_to_char("¿emboscar a quién?\n\r", ch);
  	return;
  }
  
  if (ch == victim){
  	send_to_char("¿Te falta algún tornillo?\n\r", ch);
  	return;
  }
  
  if (ch->position == POS_FIGHTING){
  	send_to_char("Termina el combate primero.\n\r", ch);
  	return;
  } 
  
  if (victim->position == POS_FIGHTING){
  	send_to_char("Está luchando.\n\r", ch);
  	return;
  }
  
  if ( victim->position <= POS_STUNNED )
  {
      send_to_char( "Eso no es honorable.\n\r", ch);
      return;
  }

  if (!IS_SET(ch->affected_by, AFF_SNEAK)){
  	send_to_char("¡Haces demasiado ruído!\n\r", ch);
  	return;
  }
  percent = number_percent( ) - (get_curr_lck(ch) - 14) 
      + (get_curr_lck(victim) - 13);

    affect_strip ( ch, gsn_sneak                        );
    affect_strip ( ch, gsn_silent                       );
    REMOVE_BIT   ( ch->affected_by, AFF_HIDE            );
    if (ch->race != RACE_DEFEL) /* Defel has perm invis */
    REMOVE_BIT   ( ch->affected_by, AFF_INVISIBLE       );
    if (ch->race != RACE_NOGHRI) /* Noghri has perm sneak */
    REMOVE_BIT   ( ch->affected_by, AFF_SNEAK           );

    
  act(AT_RED, "¡Saltas desde detrás de $N y le atacas!", ch, NULL, victim, TO_CHAR);
  act(AT_RED, "¡$n salta desde detrás de ti y te ataca!", ch, NULL, victim, TO_VICT);
  act(AT_RED, "¡$n salta desde detrás de $N y le ataca!", ch, NULL, victim, TO_NOTVICT);
    if ( !IS_AWAKE(victim) || percent < ch->pcdata->learned[gsn_ambush] ){
       multi_hit(ch, victim, gsn_ambush);
       learn_from_success( ch, gsn_ambush );
    }
    else {
	learn_from_failure( ch, gsn_ambush );
	global_retcode = damage( ch, victim, 0, gsn_ambush );
    }    	
}

//Contract System by Tawnos.
void do_contract( CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *victim;
   CHAR_DATA *target;
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char arg3[MAX_INPUT_LENGTH];
   long amount = 0;
   CONTRACT_DATA *ccontract;
   CONTRACT_DATA *contract;

   argument = one_argument( argument , arg1 );
   argument = one_argument( argument , arg2 );
   argument = one_argument( argument , arg3 );

   if(IS_NPC(ch))
     return;

   if(arg1[0] =='\0' || arg2[0] == '\0' || arg3[0] == '\0')
   {
     send_to_char("&RSintaxis: contratar <persona> <objetivo> <cantidad>\n\r", ch);
     return;
   }

   if ((victim = get_char_room(ch, arg1)) == NULL)
   {
     send_to_char("No está aquí.\n\r", ch);
     return;
   }

   if ((target = get_char_world_ooc(ch, arg2)) == NULL)
   {
     send_to_char("Tu objetivo no está jugando.\n\r", ch);
     return;
   }

   if ( ch == victim)
   {
     send_to_char("No puedes contratarte a ti.\n\r", ch);
     return;
   }

   if (IS_NPC(victim) || IS_NPC(target) )
   {
     send_to_char("No puedes contratar npcs.\n\r", ch);
     return;
   }

   if ( ch == target)
   {
     send_to_char("¡No puedes!\n\r", ch);
     return;
   }

   if ( target == victim)
   {
     send_to_char("¡Eso es absurdo!\n\r", ch);
     return;
   }

    amount = atoi(arg3);

    if(amount < 5000)
    {
      send_to_char("&REl contrato tiene que ser de al menos 5000 créditos.\n\r", ch);
      return;
    }
    
    if( ch->gold < amount)
    {
      send_to_char("&R¡No tienes tantos créditos!\n\r", ch);
      return;
    }

    for(ccontract=victim->first_contract;ccontract;ccontract=ccontract->next_in_contract)
    {
      if(!str_cmp(ccontract->target, target->name))
      {
       ch->gold -= amount;
       ccontract->amount += amount;
       ch_printf(ch, "&GTe ha contratado %s para matar a %s por %d créditos.\n\r", PERS(victim, ch), target->name, amount);
       ch_printf(victim, "&G%s te ha contratado para matar a %s, cambiando el contrato por %d créditos.\n\r",PERS(ch, victim), target->name, amount);
       return;
      }
    }

     CREATE(contract, CONTRACT_DATA, 1);
       contract->target = target->name;
       contract->amount = amount;
       LINK(contract, victim->first_contract, victim->last_contract,next_in_contract, prev_in_contract);

       ch->gold -= amount;
       ch_printf(ch, "&GHas contratado a %s para matar a %s por %d créditos.\n\r", PERS(victim, ch), target->name, amount);
       ch_printf(victim, "&G%s te ha contratado para matar a %s por %d créditos.\n\r", PERS(ch, victim), target->name, amount);    

}

void do_showcontracts(CHAR_DATA *ch, char *argument)
{
 CONTRACT_DATA *contract;

 send_to_char("&R   Objetivo   &W|&R Cantidad\n\r", ch);
 send_to_char("&W------------|----------\n\r", ch);
 
for(contract=ch->first_contract;contract;contract=contract->next_in_contract)
    {
	ch_printf(ch, "&R%-12s&W|&R %d&W\n\r", contract->target, 
contract->amount);
    }

}

void do_remcontract(CHAR_DATA *ch, char *argument)
{
 CONTRACT_DATA *contract;
 CONTRACT_DATA *scontract=NULL;

 if(argument[0] == '\0')
 {
   send_to_char("&RSintaxis: cancelarcontrato <objetivo>\n\r", ch);
   return;
 }

for(contract=ch->first_contract;contract;contract=contract->next_in_contract)
    {
      if(!str_cmp(contract->target, argument))
      {
	scontract = contract;
	break;
      }
    }

 if(!scontract || scontract == NULL)
 {
   send_to_char("El objetivo no existe.\n\r", ch);
   return;
 }

  STRFREE(scontract->target);
  UNLINK(scontract, ch->first_contract, ch->last_contract, next_in_contract, prev_in_contract);
  DISPOSE(scontract);

 send_to_char("Contrato cancelado.\n\r", ch);
 return;

}

