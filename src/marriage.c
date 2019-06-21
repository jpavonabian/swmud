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

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "mud.h"

void do_marry( CHAR_DATA *ch, char *argument)
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char buf[MAX_INPUT_LENGTH];
  CHAR_DATA *vic1;
  CHAR_DATA *vic2;
  
  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  
  if (arg1[0] == '\0' || arg2[0] == '\0')
    {
    	send_to_char("Syntax: Marry <victim1> <victim2>\n\r", ch);
    	return;
    }
  
  if ( ( vic1 = get_char_room( ch, arg1 ) ) == NULL )
   {
      sprintf(buf, "%s doesn't appear to be here.\n\r", arg1);
      send_to_char(buf, ch);
      return;
   }

  if ( ( vic2 = get_char_room( ch, arg2 ) ) == NULL )
   {
      sprintf(buf, "%s doesn't appear to be here.\n\r", arg2);
      send_to_char(buf, ch);
      return;
   }  
  
  if(IS_NPC(vic1))
   {
     sprintf(buf, "%s appears to be a mob. Why marry it off to anyone?\n\r", vic1->name);
     send_to_char(buf, ch);
     return;
   }

  if(IS_NPC(vic2))
   {
     sprintf(buf, "%s appears to be a mob. Why marry it off to anyone?\n\r", vic2->name);
     send_to_char(buf, ch);
     return;
   }

  if (!vic1->pcdata->fiance || vic1->pcdata->fiance == NULL || vic1->pcdata->fiance != vic2->name)
   {
      sprintf(buf, "%s is not engaged to %s. Perhaps a proposal would be a good first step.\n\r", vic1->name, vic2->name);
      send_to_char(buf, ch);
      return;
   }
  
  if (!vic2->pcdata->fiance || vic2->pcdata->fiance == NULL || vic2->pcdata->fiance != vic1->name)
   {
      sprintf(buf, "%s is not engaged to %s. Perhaps a proposal would be a good first step.\n\r", vic2->name, vic1->name);
      send_to_char(buf, ch);
      return;
   }
   
  if(vic1->top_level < 5)
   {
     sprintf(buf, "%s is to low a level to get married.\n\r", vic1->name);
     send_to_char(buf, ch);
     return;
   }
   
  if(vic2->top_level < 5)
   {
     sprintf(buf, "%s is to low a level to get married.\n\r", vic2->name);
     send_to_char(buf, ch);
     return;
   }
  if (vic1->sex == SEX_MALE && vic2->sex == SEX_MALE)
    act(AT_WHITE, "A continuación, por el poder de la República os declaro Marido y Marido. Puede besar a su marido.", ch, NULL, NULL, TO_ROOM);
  else if (vic1->sex == SEX_FEMALE && vic2->sex == SEX_FEMALE)
    act(AT_WHITE, "A continuación, por el poder de la República os declaro Mujer y Mujer. Puede besar a su Mujer.", ch, NULL, NULL, TO_ROOM);
  else if (vic1->sex == SEX_MALE && vic2->sex == SEX_FEMALE)
    act(AT_WHITE, "A continuación, por el poder de la República os declaro Esposo y Esposa. Puede besar a su esposa.", ch, NULL, NULL, TO_ROOM);
  else
    act(AT_WHITE, "A continuación, por el poder de la República os declaro Esposo y Esposa. Puede besar a su esposa.", ch, NULL, NULL, TO_ROOM);
  
  vic1->pcdata->spouse = vic2->name;
  vic2->pcdata->spouse = vic1->name;
  /*if (vic1->sex == SEX_MALE && vic2->sex == SEX_FEMALE)
    vic2->pcdata->last_name = vic1->pcdata->last_name;
  else if (vic1->sex == SEX_FEMALE && vic2->sex == SEX_MALE)
    vic1->pcdata->last_name = vic2->pcdata->last_name;
  */ 
  sprintf(buf, "&R(&WCasamientos&R} &W%s: Me complace anunciar que %s y %s se han casado.", ch->name, vic1->name, vic2->name);
  echo_to_all(AT_YELLOW, buf, ECHOTAR_ALL);
  return;
}

void do_divorce( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *vic1;
  CHAR_DATA *vic2;
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char buf[MAX_INPUT_LENGTH];
  
  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  
  if (arg1[0] == '\0' && arg2[0] == '\0')
   {
     send_to_char("Syntax: Divorce <victim1> <victim2>\n\r", ch);
     return;
   }
  
  if ((vic1 = get_char_room(ch, arg1)) == NULL)
   {
     sprintf(buf, "%s doesn't appear to be here. You should wait for them.\n\r", arg1);
     send_to_char(buf, ch);
     return;
   }
  
  if ((vic2 = get_char_room(ch, arg2)) == NULL)
   {
     sprintf(buf, "%s doesn't appear to be here. You should wait for then.\n\r", arg2);
     send_to_char(buf, ch);
     return;
   }
  
  if (!vic1->pcdata->spouse || vic1->pcdata->spouse == NULL)
   {
     sprintf(buf, "%s doesn't appear to be married.\n\r", vic1->name);
     send_to_char(buf, ch);
     return;
   }

  if (!vic2->pcdata->spouse || vic2->pcdata->spouse == NULL)
   {
     sprintf(buf, "%s doesn't appear to be married.\n\r", vic2->name);
     send_to_char(buf, ch);
     return;
   }
  
  if (vic1->pcdata->spouse != vic2->pcdata->spouse)
   {
     sprintf(buf, "%s is not married to %s\n\r", vic1->name, vic2->name);
     send_to_char(buf, ch);
     return;
   }
  
  vic1->pcdata->spouse = NULL;
  vic2->pcdata->spouse = NULL;
  do_say(ch, "500 créditos cada uno, por favor.");
  do_say(ch, "Gracias.");
  ch->gold += 1000;
  vic1->gold -= 500;
  vic2->gold -= 500;
    sprintf(buf, "&R(&WCasamientos&R} &W%s: Me provoca una gran trsiteza anunciar que %s y %s ya no están casados.", ch->name, vic1->name, vic2->name);
}   

void do_propose( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_INPUT_LENGTH];
  
  argument = one_argument(argument, arg);
  
  if (arg[0] == '\0'){
    send_to_char("Sintaxis: proponer <personaje>\n\r", ch);
    return;
  }
  
  if (( victim = get_char_room(ch, arg)) == NULL){
    send_to_char("No está aquí.\n\r", ch);
    return;
  }
  
  if ( ch == victim ){
    send_to_char("¿Por qué quieres proponerte matrimonio a ti mismo?\n\r", ch);
    return;
  }
  
  
  if ( IS_NPC(victim) ){
    send_to_char("¿Por qué quieres casarte con un mob?\n\r", ch);
    return;
  }
  
  if ( IS_NPC(ch) ){
    send_to_char("¿no es un poco absurdo?\n\r", ch);
    return;
  }
  
  if (victim->pcdata->fiance && victim->pcdata->fiance != NULL){
    send_to_char("ya le han propuesto matrimonio.\n\r", ch);
    return;
  }
  
  if (victim->pcdata->spouse && victim->pcdata->spouse != NULL){
    send_to_char("Ya se ha casado.\n\r", ch);
  }
  if (victim->pcdata->proposed && victim->pcdata->proposed != NULL){ 
    sprintf(buf, "They have already been proposed to by %s.\n\r", victim->pcdata->proposed);
    return;
  }
  
  if (victim->pcdata->propose && victim->pcdata->propose != NULL){ 
    sprintf(buf, "They have already been proposed to %s.\n\r", victim->pcdata->propose);
    return;
  }
  
  ch->pcdata->propose = victim->name;
  victim->pcdata->proposed = ch->name;
  act(AT_WHITE, "$n se arrodilla ante ti y te pide la mano en matrimonio.", ch, NULL, victim, TO_VICT);
  act(AT_WHITE, "Te arrodillas y pides la mano de$N en matrimonio.", ch, NULL, victim, TO_CHAR);
  act(AT_WHITE, "$n se arrodilla y pide la mano de $N en matrimonio.", ch, NULL, victim, TO_NOTVICT);
  return;
}

void do_accept( CHAR_DATA *ch, char *argument ){
  
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_INPUT_LENGTH];
  
  argument = one_argument(argument, arg);
  
  if (arg[0] == '\0'){
    send_to_char("sintaxis: aceptar <personaje>\n\r", ch);
    return;
  }
  
  if (( victim = get_char_room(ch, arg)) == NULL){
    send_to_char("No está aquí.\n\r", ch);
    return;
  }
  
  if (IS_NPC(victim) ){
    send_to_char("Un mob no puede casarse.\n\r", ch);
    return;
  }
  
  if (IS_NPC(ch) ){
    send_to_char("¿Por qué alguien querría casarse contigo? Eres un mob.\n\r", ch);
    return;
  }
  
  
  if (victim->name != ch->pcdata->proposed){
    send_to_char("Pero si no te ha propuesto nada.\n\r", ch);
    return;
  }
  
  if (victim->pcdata->propose != ch->name){
    send_to_char("Tan triste...\n\r", ch);
    return;
  }
  
  sprintf(buf, "%s ha aceptado la mano de %s en matrimonio. They will soon be wed.", ch->name, victim->name);
  echo_to_all( AT_WHITE, buf, ECHOTAR_ALL ); 
  ch->pcdata->propose = NULL;
  victim->pcdata->proposed = NULL;
  ch->pcdata->fiance = victim->name;
  victim->pcdata->fiance = ch->name;
  return;
}
 
 
void do_decline( CHAR_DATA *ch, char *argument ){
  
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_INPUT_LENGTH];
  
  argument = one_argument(argument, arg);
  
  if (arg[0] == '\0'){
    send_to_char("Sintaxis: declinar <personaje>\n\r", ch);
    return;
  }
  
  if (( victim = get_char_room(ch, arg)) == NULL){
    send_to_char("No está aquí.\n\r", ch);
    return;
  }
  
  if (IS_NPC(victim) ){
    send_to_char("Los mobs no se pueden casar.\n\r", ch);
    return;
  }
  
  if (IS_NPC(ch) ){
    send_to_char("¿Por qué alguien querría casarse contigo? Eres un mob.\n\r", ch);
    return;
  }
  
  
  if (victim->name != ch->pcdata->proposed){
    send_to_char("No te ha propuesto nada.\n\r", ch);
    return;
  }
  
  if (victim->pcdata->propose != ch->name){
    send_to_char("Es tan triste...\n\r", ch);
    return;
  }
  
  sprintf(buf, "%s ha rechazado la mano de %s en matrimonio.", ch->name, victim->name);
  echo_to_all( AT_WHITE, buf, ECHOTAR_ALL ); 
  ch->pcdata->propose = NULL;
  ch->pcdata->proposed = NULL;
  return;
}

void do_spousetalk (CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *victim;

  if(!argument || argument[0] == '\0')
    {
      send_to_char ("¿Qué le quieres decir a tu pareja?\n\r", ch);
      return;
    }
  

  if ( ch->position == POS_SLEEPING)
    {
      send_to_char ("¿En tus sueños?\n\r", ch);
      return;
    }

if ( ch->position == POS_FIGHTING)
  {
    send_to_char("¡Termina primero el combate!\n\r", ch);
    return;
  }
    
  if ( IS_NPC( ch ) )
    {
	send_to_char( "¡Eres un mob!\n\r", ch );
	return;	  
    }
  
  if (!ch->pcdata->spouse)
    {
      send_to_char ("¡Pero tú no tienes pareja!\n\r", ch);
      return;
    }

  if ((victim = get_char_world (ch, ch->pcdata->spouse)) == NULL)
    {
      sprintf (buf, "%s no está jugando ahora.\n\r", ch->pcdata->spouse);
      send_to_char (buf, ch);
      return;
    }
    
    sprintf(buf, "&G[&PPareja&G] &Pa %s: &W%s\n\r", victim->name, argument);
    send_to_char(buf, ch); 
    sprintf(buf, "&G[&PPareja&G] &P%s te dice: &W%s\n\r", ch->name, argument); 
    send_to_char(buf, victim); 
}
