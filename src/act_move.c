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
#include <ctype.h>
#include <time.h>
#include "mud.h"


const	sh_int	movement_loss	[SECT_MAX]	=
{
    1, 2, 2, 3, 4, 6, 4, 1, 6, 10, 6, 5, 7, 4
};

char *	const	dir_name	[]		=
{
    "north", "east", "south", "west", "up", "down",
    "northeast", "northwest", "southeast", "southwest", "somewhere"
};
char *	const	dir_name2	[]		=
{
    "norte", "este", "sur", "oeste", "arriba", "abajo",
    "noreste", "noroeste", "sureste", "suroeste", "lugar"
};
char *	const	dir_name3	[]		=
{
    "el norte", "el este", "el sur", "el oeste", "arriba", "abajo",
    "el noreste", "el noroeste", "el sureste", "el suroeste", "lugar"
};
const	int	trap_door	[]		=
{
    TRAP_N, TRAP_E, TRAP_S, TRAP_W, TRAP_U, TRAP_D,
    TRAP_NE, TRAP_NW, TRAP_SE, TRAP_SW
};


const	sh_int	rev_dir		[]		=
{
    2, 3, 0, 1, 5, 4, 9, 8, 7, 6, 10
};


ROOM_INDEX_DATA * vroom_hash [64];


/*
 * Local functions.
 */
bool	has_key		args( ( CHAR_DATA *ch, int key ) );


char *	const		sect_names[SECT_MAX][2] =
{
    { "En una habitación","inside"	},	{ "Una calle de ciudad","cities"      },
    { "En un campo","fields"	},	{ "En un bosque","forests"	},
    { "colina",	"hills"		},	{ "en una montaña","mountains"	},
    { "En el agua","waters"	},	{ "En aguas turbulentas","waters"	},
    { "Bajo el agua", "underwaters" },	{ "En el aire",	"air"		},
    { "En el desierto","deserts"	},	{ "Algún sitio",	"unknown"	},
    { "El suelo del océano", "ocean floor" },	{ "Bajo tierra", "underground"	}
};


const	int		sent_total[SECT_MAX]		=
{
    4, 24, 4, 4, 1, 1, 1, 1, 1, 2, 2, 1, 1, 1
};

char *	const		room_sents[SECT_MAX][25]	=
{
    {
	"Las paredes, toscamente talladas, están hechas de granito.",
	"Ves ocasionalmente una araña arrastrándose.",
	"Ves signos de una batalla reciente. el suelo está lleno de sangre.",
	"Este lugar tiene un desagradable olor a humedad y vegetación podrida."
    },

    {
	"De vez en cuando hueles comida.",
	"A cada lado se extienden altos edificios hacia el cielo.",
	"Varias personas muestran sus talentos por la calle para ganar algunos créditos.",
	"Dos personas se gritan acaloradas palabras cerca de aquí.",
	"Crees distinguir varias figuras sombrías hablando en un callejón oscuro."
        "Una ligera brisa sopla a través de los altos edificios.",
        "Una pequeña multitud de personas se reúne a un lado de la calle.",
        "Nubes muy por encima de ti oscurecen las cimas de los rascacielos más altos.",
        "Un aerodeslizador se mueve lentamente por la calle evitando peatones.",
        "Un aerodeslizador sobrevuela el lugar.",
        "El aire es difícil de respirar.",
        "Los muchos olores de la ciudad asaltan tus sentidos.",
        "Oyes un grito lejos de aquí.",
        "No puedes contar la cantidad de edificios que te rodean.",
        "La ciudad se extiende en todas direcciones.",
        "La calle es ancha y larga.",
        "Un hombre montado en un swoop esquiva vehículos y peatones.",
        "La carretera está desgastada por su gran tránsito.",
        "Sientes que sería muy fácil perderse en una ciudad tan enorme.",
        "Ves mucha gente corriendo en todas direcciones.",
        "Hay muchas entradas a edificios.",
        "A lo largo del borde de la calle, las barandillas evitan que los peatones caigan a la muerte.",
        "Observas la ciudad, repleta de grandes edificios.",
        "Una rejilla en la calle evita que se acumule agua de lluvia.",
        "Puedes ver tu reflejo en varias ventanas de transpariacero al pasar."
        "Oyes un grito lejos de aquí.",
    },

    {
	"Observas la maleza y los arbustos.",
	"A lo lejos distingues un pequeño grupo de árbols.",
	"Hasta donde alcanza la vista solo puedes ver hierba y más hierba.",
	"A lo largo de las llanuras hay dispersas una gran variedad de malezas y flores silvestres."
    },

    {
	"Los altos árboles de hoja perene te impiden ver mucho.",
	"Ves robles enormes que parecen tener varios cientoss de años.",
	"Ves un solitario sauce llorón.",
	"A tu izquierda ves un grupo de brillantes abedules blancos, delgados y altos."
    },

    {
	"Las colinas están ligeramente moteadas con flores silvestres violetas."
    },

    {
	"El paso rocoso en la montaña ofrece numerosos escondites."
    },

    {
	"El agua es clara y transparente como el cristal."
    },

    {
	"Las olas salpican todo el lugar."
    },

    {
	"Un pequeño grupo de peces nada cerca."
    },

    {
	"La tierra está muy abajo.",
	"Una brumosa niebla se desplaza por el lugar."
    },

    {
	"Hasta donde alcanza la vista solo ves arena.",
	"Crees ver un oasis a lo lejos."
    },

    {
	"No ves nada inusual."
    },

    { 
        "Ves varias rocas y corales en el suelo del océano."
    },

    {
	"Estás en un largo túnel de roca."
    }

};

int wherehome( CHAR_DATA *ch)
{
    if( ch->plr_home )
      return ch->plr_home->vnum;

    if( get_trust(ch) >= LEVEL_IMMORTAL )
       return ROOM_START_IMMORTAL;
    if( ch->race  == RACE_HUMAN)
       return ROOM_START_HUMAN;
    if( ch->race  == RACE_WOOKIEE)
       return ROOM_START_WOOKIEE;
    if( ch->race  == RACE_RODIAN)
       return ROOM_START_RODIAN;
    if( ch->race  == RACE_MON_CALAMARI)
       return ROOM_START_MON_CALAMARIAN;
    if( ch->race  == RACE_TWI_LEK)
       return ROOM_START_TWILEK;
    if( ch->race  == RACE_HUTT)
       return ROOM_START_HUTT;
    if( ch->race  == RACE_GAMORREAN)
       return ROOM_START_GAMORREAN;
    if( ch->race  == RACE_JAWA)
       return ROOM_START_JAWA;
    if( ch->race  == RACE_ADARIAN)
       return ROOM_START_ADARIAN;
    if( ch->race  == RACE_EWOK)
       return ROOM_START_EWOK;
    if( ch->race  == RACE_VERPINE)
       return ROOM_START_VERPINE;
    if( ch->race  == RACE_DEFEL)
       return ROOM_START_DEFEL;
    if( ch->race  == RACE_TRANDOSHAN)
       return ROOM_START_TRANDOSHAN;
    if( ch->race  == RACE_HAPAN)
       return ROOM_START_HAPAN;
    if( ch->race  == RACE_QUARREN)
       return ROOM_START_QUARREN;
    if( ch->race  == RACE_NOGHRI)
       return ROOM_START_NOGHRI;
       
    return ROOM_VNUM_TEMPLE;   
}

char *grab_word( char *argument, char *arg_first )
{
    char cEnd;
    sh_int count;

    count = 0;

    while ( isspace(*argument) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' || ++count >= 255 )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*arg_first++ = *argument++;
    }
    *arg_first = '\0';

    while ( isspace(*argument) )
	argument++;

    return argument;
}

char *wordwrap( char *txt, sh_int wrap )
{
    static char buf[MAX_STRING_LENGTH];
    char *bufp;

    buf[0] = '\0';
    bufp = buf;
    if ( txt != NULL )
    {
        char line[MAX_STRING_LENGTH];
        char temp[MAX_STRING_LENGTH];
        char *ptr, *p;
        int ln, x;

	++bufp;
        line[0] = '\0';
        ptr = txt;
        while ( *ptr )
        {
	  ptr = grab_word( ptr, temp );
          ln = strlen( line );  x = strlen( temp );
          if ( (ln + x + 1) < wrap )
          {
	    if ( line[ln-1] == '.' )
              strcat( line, "  " );
	    else
              strcat( line, " " );
            strcat( line, temp );
            p = strchr( line, '\n' );
            if ( !p )
              p = strchr( line, '\r' );
            if ( p )
            {
                strcat( buf, line );
                line[0] = '\0';
            }
          }
          else
          {
            strcat( line, "\r\n" );
            strcat( buf, line );
            strcpy( line, temp );
          }
        }
        if ( line[0] != '\0' )
            strcat( buf, line );
    }
    return bufp;
}

void decorate_room( ROOM_INDEX_DATA *room )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    int nRand;
    int iRand, len;
    int previous[8];
    int sector = room->sector_type;

    if ( room->name )
      STRFREE( room->name );
    if ( room->description )
      STRFREE( room->description );

    room->name	= STRALLOC( sect_names[sector][0] );
    buf[0] = '\0';
    nRand = number_range( 1, UMIN(8,sent_total[sector]) );

    for ( iRand = 0; iRand < nRand; iRand++ )
	previous[iRand] = -1;

    for ( iRand = 0; iRand < nRand; iRand++ )
    {
	while ( previous[iRand] == -1 )
	{
	    int x, z;

	    x = number_range( 0, sent_total[sector]-1 );

	    for ( z = 0; z < iRand; z++ )
		if ( previous[z] == x )
		  break;

	    if ( z < iRand )
		  continue;

	    previous[iRand] = x;

	    len = strlen(buf);
	    sprintf( buf2, "%s", room_sents[sector][x] );
	    if ( len > 5 && buf[len-1] == '.' )
	    {
		strcat( buf, "  " );
		buf2[0] = UPPER(buf2[0] );
	    }
	    else
	    if ( len == 0 )
	        buf2[0] = UPPER(buf2[0] );
	    strcat( buf, buf2 );
	}
    }
    sprintf( buf2, "%s\n\r", wordwrap(buf, 78) );
    room->description = STRALLOC( buf2 );
}

/*
 * Remove any unused virtual rooms				-Thoric
 */
void clear_vrooms( )
{
    int hash;
    ROOM_INDEX_DATA *room, *room_next, *prev;

    for ( hash = 0; hash < 64; hash++ )
    {
	while ( vroom_hash[hash]
	&&     !vroom_hash[hash]->first_person
	&&     !vroom_hash[hash]->first_content )
	{
	    room = vroom_hash[hash];
	    vroom_hash[hash] = room->next;
	    clean_room( room );
	    DISPOSE( room );
	    --top_vroom;
	}
	prev = NULL;
	for ( room = vroom_hash[hash]; room; room = room_next )
	{
	    room_next = room->next;
	    if ( !room->first_person && !room->first_content )
	    {
		if ( prev )
		  prev->next = room_next;
		clean_room( room );
		DISPOSE( room );
		--top_vroom;
	    }
	    if ( room )
	      prev = room;
	}
    }
}

/*
 * Function to get the equivelant exit of DIR 0-MAXDIR out of linked list.
 * Made to allow old-style diku-merc exit functions to work.	-Thoric
 */
EXIT_DATA *get_exit( ROOM_INDEX_DATA *room, sh_int dir )
{
    EXIT_DATA *xit;

    if ( !room )
    {
	bug( "Get_exit: NULL room", 0 );
	return NULL;
    }

    for (xit = room->first_exit; xit; xit = xit->next )
       if ( xit->vdir == dir )
         return xit;
    return NULL;
}

/*
 * Function to get an exit, leading the the specified room
 */
EXIT_DATA *get_exit_to( ROOM_INDEX_DATA *room, sh_int dir, int vnum )
{
    EXIT_DATA *xit;

    if ( !room )
    {
	bug( "Get_exit: NULL room", 0 );
	return NULL;
    }

    for (xit = room->first_exit; xit; xit = xit->next )
       if ( xit->vdir == dir && xit->vnum == vnum )
         return xit;
    return NULL;
}

/*
 * Function to get the nth exit of a room			-Thoric
 */
EXIT_DATA *get_exit_num( ROOM_INDEX_DATA *room, sh_int count )
{
    EXIT_DATA *xit;
    int cnt;

    if ( !room )
    {
	bug( "Get_exit: NULL room", 0 );
	return NULL;
    }

    for (cnt = 0, xit = room->first_exit; xit; xit = xit->next )
       if ( ++cnt == count )
         return xit;
    return NULL;
}


/*
 * Modify movement due to encumbrance				-Thoric
 */
sh_int encumbrance( CHAR_DATA *ch, sh_int move )
{
    int cur, max;

    max = can_carry_w(ch);
    cur = ch->carry_weight;
    if ( cur >= max )
      return move * 4;
    else
    if ( cur >= max * 0.95 )
      return move * 3.5;
    else
    if ( cur >= max * 0.90 )
      return move * 3;
    else
    if ( cur >= max * 0.85 )
      return move * 2.5;
    else
    if ( cur >= max * 0.80 )
      return move * 2;
    else
    if ( cur >= max * 0.75 )
      return move * 1.5;
    else
      return move;
}


/*
 * Check to see if a character can fall down, checks for looping   -Thoric
 */
bool will_fall( CHAR_DATA *ch, int fall )
{
    if ( IS_SET( ch->in_room->room_flags, ROOM_NOFLOOR )
    &&   CAN_GO(ch, DIR_DOWN)
    && (!IS_AFFECTED( ch, AFF_FLYING )
    || ( ch->mount && !IS_AFFECTED( ch->mount, AFF_FLYING ) ) ) )
    {
	if ( fall > 80 )
	{
	   bug( "Falling (in a loop?) more than 80 rooms: vnum %d", ch->in_room->vnum );
	   char_from_room( ch );
	   char_to_room( ch, get_room_index( wherehome(ch) ) );
	   fall = 0;
	   return TRUE;
	}
	set_char_color( AT_FALLING, ch );
	send_to_char( "Caes...\n\r", ch );
	move_char( ch, get_exit(ch->in_room, DIR_DOWN), ++fall );
	return TRUE;
    }
    return FALSE;
}


/*
 * create a 'virtual' room					-Thoric
 */
ROOM_INDEX_DATA *generate_exit( ROOM_INDEX_DATA *in_room, EXIT_DATA **pexit )
{
    EXIT_DATA *xit, *bxit;
    EXIT_DATA *orig_exit = (EXIT_DATA *) *pexit;
    ROOM_INDEX_DATA *room, *backroom;
    int brvnum;
    int serial;
    int roomnum;
    int distance = -1;
    int vdir = orig_exit->vdir;
    sh_int hash;
    bool found = FALSE;

    if ( in_room->vnum > MAX_VNUMS )	/* room is virtual */
    {
	serial = in_room->vnum;
	roomnum = in_room->tele_vnum;
	if ( (serial & MAX_VNUMS) == orig_exit->vnum )
	{
	  brvnum = serial >> 16;
	  --roomnum;
	  distance = roomnum;
	}
	else
	{
	  brvnum = serial & MAX_VNUMS;
	  ++roomnum;
	  distance = orig_exit->distance - 1;
	}
	backroom = get_room_index( brvnum );
    }
    else
    {
	int r1 = in_room->vnum;
	int r2 = orig_exit->vnum;

	brvnum = r1;
	backroom = in_room;
	serial = (UMAX( r1, r2 ) << 16) | UMIN( r1, r2 );
	distance = orig_exit->distance - 1;
	roomnum = r1 < r2 ? 1 : distance;
    }
    hash = serial % 64;
    
    for ( room = vroom_hash[hash]; room; room = room->next )
	if ( room->vnum == serial && room->tele_vnum == roomnum )
	{
	    found = TRUE;
	    break;
	}
    if ( !found )
    {
	CREATE( room, ROOM_INDEX_DATA, 1 );
	room->area	  = in_room->area;
	room->vnum	  = serial;
	room->tele_vnum	  = roomnum;
	room->sector_type = in_room->sector_type;
	room->room_flags  = in_room->room_flags;
	decorate_room( room );
	room->next	  = vroom_hash[hash];
	vroom_hash[hash]  = room;
	++top_vroom;
    }
    if ( !found || (xit=get_exit(room, vdir))==NULL )
    {
	xit = make_exit(room, orig_exit->to_room, vdir);
	xit->keyword		= STRALLOC( "" );
	xit->description	= STRALLOC( "" );
	xit->key		= -1;
	xit->distance = distance;
    }
    if ( !found )
    {
	bxit = make_exit(room, backroom, rev_dir[vdir]);
	bxit->keyword		= STRALLOC( "" );
	bxit->description	= STRALLOC( "" );
	bxit->key		= -1;
	if ( (serial & MAX_VNUMS) != orig_exit->vnum )
	  bxit->distance = roomnum;
	else
	{
	  EXIT_DATA *tmp = get_exit( backroom, vdir );
	  int fulldist = tmp->distance;
	  
	  bxit->distance = fulldist - distance;
	}
    }
//    (EXIT_DATA *) pexit = xit;
      pexit = (EXIT_DATA **) xit;
    return room;
}

ch_ret move_char( CHAR_DATA *ch, EXIT_DATA *pexit, int fall )
{
    ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *to_room;
    ROOM_INDEX_DATA *from_room;
    char buf[MAX_STRING_LENGTH];
    char *txt;
    char *dtxt;
    ch_ret retcode;
    sh_int door, distance;
    bool drunk = FALSE;
    bool brief = FALSE;

    if ( !IS_NPC( ch ) )
      if ( IS_DRUNK( ch, 2 ) && ( ch->position != POS_SHOVE )
	&& ( ch->position != POS_DRAG ) )
	drunk = TRUE;

    if ( drunk && !fall )
    {
      door = number_door();
      pexit = get_exit( ch->in_room, door );
    }

#ifdef DEBUG
    if ( pexit )
    {
	sprintf( buf, "move_char: %s to door %d", ch->name, pexit->vdir );
	log_string( buf );
    }
#endif

    retcode = rNONE;
    txt = NULL;

    if ( IS_NPC(ch) && IS_SET( ch->act, ACT_MOUNTED ) )
      return retcode;

    in_room = ch->in_room;
    from_room = in_room;
    if ( !pexit || (to_room = pexit->to_room) == NULL )
    {
	if ( drunk )
	  send_to_char( "Te golpeas con una pared.\n\r", ch );
	 else
	  send_to_char( "Por desgracia, no puedes ir por ahí.\n\r", ch );
	return rNONE;
    }

    door = pexit->vdir;
    distance = pexit->distance;

    /*
     * Exit is only a "window", there is no way to travel in that direction
     * unless it's a door with a window in it		-Thoric
     */
	 if(IS_AFFECTED(ch, AFF_PARALYSIS))
	{
		 send_to_char("No eres capaz de moverte.\n\r", ch);
		 return rNONE;
	}


    if ( IS_SET( pexit->exit_info, EX_WINDOW )
    &&  !IS_SET( pexit->exit_info, EX_ISDOOR ) )
    {
	send_to_char( "Por desgracia, no puedes ir por ahí.\n\r", ch );
	return rNONE;
    }

    if (  IS_SET(pexit->exit_info, EX_PORTAL) 
       && IS_NPC(ch) )
    {
        act( AT_PLAIN, "Los mobs no pueden usar portales.", ch, NULL, NULL, TO_CHAR );
	return rNONE;
    }

    if ( IS_SET(pexit->exit_info, EX_NOMOB) && IS_NPC(ch) )
    {
	act( AT_PLAIN, "Los mobs no pueden entrar aquí.", ch, NULL, NULL, TO_CHAR );
	return rNONE;
    }

    if( IS_SET( to_room->room_flags, ROOM_NO_MOB ) && IS_NPC( ch ) )
    {
        act( AT_PLAIN, "Los mobs no pueden entrar aquí.", ch, NULL, NULL, TO_CHAR );
        return rNONE;
    }

    if ( IS_SET(pexit->exit_info, EX_CLOSED)
    && (!IS_AFFECTED(ch, AFF_PASS_DOOR)
    ||   IS_SET(pexit->exit_info, EX_NOPASSDOOR)) )
    {
	if ( !IS_SET( pexit->exit_info, EX_SECRET )
	&&   !IS_SET( pexit->exit_info, EX_DIG ) )
	{
	  if ( drunk )
	  {
	    act( AT_PLAIN, "$n Corre hacia $d y se da con la puerta en las narices devido a su borrachera.", ch,
		NULL, pexit->keyword, TO_ROOM );
	    act( AT_PLAIN, "Corres hacia $d y te das con la puerta en las narices en tu estado de borrachera.", ch,
		NULL, pexit->keyword, TO_CHAR ); 
	  }
	 else
	  act( AT_PLAIN, "$d está cerrada.", ch, NULL, pexit->keyword, TO_CHAR );
	}
       else
	{
	  if ( drunk )
	    send_to_char( "Te golpeas contra una pared.\n\r", ch );
	   else
	    send_to_char( "Por desgracia, no puedes ir por ahí.\n\r", ch );
	}

	return rNONE;
    }

    /*
     * Crazy virtual room idea, created upon demand.		-Thoric
     */
    if ( distance > 1 )
	if ( (to_room=generate_exit(in_room, &pexit)) == NULL )
	    send_to_char( "Por desgracia, no puedes ir por ahí.\n\r", ch );

    if ( !fall
    &&   IS_AFFECTED(ch, AFF_CHARM)
    &&   ch->master
    &&   in_room == ch->master->in_room )
    {
	send_to_char( "¿Qué?  ¿y dejar a tu querido dueño?\n\r", ch );
	return rNONE;
    }

    if ( room_is_private( ch, to_room ) && (ch->top_level < sysdata.privwoverride)  )
    {
	send_to_char( "Esa habitación es privada en este momento.\n\r", ch );
	return rNONE;
    }

    if ( room_is_private( ch, to_room ) && (ch->top_level > sysdata.privwoverride)  )
    {
        send_to_char( "¡Ignoras el flag privado y accedes a la habitación!\n\r", ch );
    }

    if ( !IS_IMMORTAL(ch)
    &&  !IS_NPC(ch)
    &&  ch->in_room->area != to_room->area )
    {
	if ( ch->top_level < to_room->area->low_hard_range )
	{
	    set_char_color( AT_TELL, ch );
	    switch( to_room->area->low_hard_range - ch->top_level )
	    {
		case 1:
		  send_to_char( "Una voz en tu mente dice: 'Estás casi listo para ir por ese camino ...'", ch );
		  break;
		case 2:
		  send_to_char( "Una voz en tu mente dice: 'Pronto estarás listo para viajar por este camino ... pronto'.", ch );
		  break;
		case 3:
		  send_to_char( "Una voz en tu mente dice: 'No estás listo para seguir por ese camino ... todavía'.\n\r", ch);
		  break;
		default:
		  send_to_char( "Una voz en tu mente dice: 'No estás listo para seguir ese camino'.\n\r", ch);
	    }
	    return rNONE;
	}
	else
	if ( ch->top_level > to_room->area->hi_hard_range )
	{
	    set_char_color( AT_TELL, ch );
	    send_to_char( "Una voz en tu mente dice: 'No hay nada más para ti en ese camino'.", ch );
	    return rNONE;
	}          
    }

    if ( !fall && !IS_NPC(ch) )
    {
	/*int iClass;*/
	int move;
/* Pretty sure we don't need to check for guilds anymore now that we have
   the new dh. -- Narn
*/
/*
	for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
	{
	    if ( iClass != ch->class
	    &&   to_room->vnum == class_table[iClass]->guild )
	    {
		send_to_char( "No se te permite entrar ahí.\n\r", ch );
		return rNONE;
	    }
	}
*/

/* Prevent deadlies from entering a nopkill-flagged area from a 
   non-flagged area, but allow them to move around if already
   inside a nopkill area. - Blodkai
*/

	if ( in_room->sector_type == SECT_AIR
	||   to_room->sector_type == SECT_AIR
	||   IS_SET( pexit->exit_info, EX_FLY ) )
	{
	    if ( ch->mount && !IS_AFFECTED( ch->mount, AFF_FLYING ) )
	    {
		send_to_char( "Tu montura no puede volar.\n\r", ch );
		return rNONE;
	    }
	    if ( !ch->mount && !IS_AFFECTED(ch, AFF_FLYING) )
	    {
		send_to_char( "Necesitas volar para ir por ahí.\n\r", ch );
		return rNONE;
	    }
	}

	if ( in_room->sector_type == SECT_WATER_NOSWIM
	||   to_room->sector_type == SECT_WATER_NOSWIM )
	{
	    OBJ_DATA *obj;
	    bool found;

	    found = FALSE;
	    if ( ch->mount )
	    {
		if ( IS_AFFECTED( ch->mount, AFF_FLYING )
		||   IS_AFFECTED( ch->mount, AFF_FLOATING ) )
		  found = TRUE;
	    }
	    else
	    if ( IS_AFFECTED(ch, AFF_FLYING)
	    ||   IS_AFFECTED(ch, AFF_FLOATING) )
		found = TRUE;

	    /*
	     * Look for a boat.
	     */
	    if ( !found )
	      for ( obj = ch->first_carrying; obj; obj = obj->next_content )
	      {
		 if ( obj->item_type == ITEM_BOAT )
		 {
		    found = TRUE;
		    if ( drunk )
		      txt = "rema torpemente";
		     else
		      txt = "rema";
		    break;
		 }
	      }

	    if ( !found )
	    {
		send_to_char( "Necesitas un bote para ir por ahí.\n\r", ch );
		return rNONE;
	    }
	}

	if ( IS_SET( pexit->exit_info, EX_CLIMB ) )
	{
	    bool found;

	    found = FALSE;
	    if ( ch->mount && IS_AFFECTED( ch->mount, AFF_FLYING ) )
	      found = TRUE;
	    else
	    if ( IS_AFFECTED(ch, AFF_FLYING) )
	      found = TRUE;

	    if ( !found && !ch->mount )
	    {
		if ( ( !IS_NPC(ch) && number_percent( ) > ch->pcdata->learned[gsn_climb] )
		||      drunk || ch->mental_state < -90 )
		{
		   send_to_char( "¡Comienzas a subir ... pero pierdes el agarre y caes!\n\r", ch);
		   learn_from_failure( ch, gsn_climb );
		   if ( pexit->vdir == DIR_DOWN )
		   {
			retcode = move_char( ch, pexit, 1 );
			return retcode;
		   }
		   set_char_color( AT_HURT, ch );
		   send_to_char( "¡OUCH! ¡Te golpeas contra el suelo!\n\r", ch );
		   WAIT_STATE( ch, 20 );
		   retcode = damage( ch, ch, (pexit->vdir == DIR_UP ? 10 : 5),
					TYPE_UNDEFINED );
		   return retcode;
		}
		found = TRUE;
		learn_from_success( ch, gsn_climb );
		WAIT_STATE( ch, skill_table[gsn_climb]->beats );
		txt = "escala";
	    }

	    if ( !found )
	    {
		send_to_char( "No puedes escalar.\n\r", ch );
		return rNONE;
	    }
	}

	if ( ch->mount )
	{
	  switch (ch->mount->position)
	  {
	    case POS_DEAD:
            send_to_char( "¡Tu montura está muerta!\n\r", ch );
	    return rNONE;
            break;
        
            case POS_MORTAL:
            case POS_INCAP:
            send_to_char( "Tu montura tiene demasiadas heridas para moverse.\n\r", ch );
	    return rNONE;
            break;
            
            case POS_STUNNED:
            send_to_char( "Tu montura está demasiado aturdida para hacer eso.\n\r", ch );
     	    return rNONE;
            break;
       
            case POS_SLEEPING:
            send_to_char( "Tu montura está durmiendo.\n\r", ch );
	    return rNONE;
            break;
         
            case POS_RESTING:
            send_to_char( "Tu montura está descansando.\n\r", ch);
	    return rNONE;
            break;
        
            case POS_SITTING:
            send_to_char( "Tu montura está sentada.\n\r", ch);
	    return rNONE;
            break;

	    default:
	    break;
  	  }

	  if ( !IS_AFFECTED(ch->mount, AFF_FLYING)
	  &&   !IS_AFFECTED(ch->mount, AFF_FLOATING) )
	    move = movement_loss[UMIN(SECT_MAX-1, in_room->sector_type)];
	  else
	    move = 1;
	  if ( ch->mount->move < move )
	  {
	    send_to_char( "Tu montura está demasiado cansada.\n\r", ch );
	    return rNONE;
	  }
	}
	else
	{
	  if ( !IS_AFFECTED(ch, AFF_FLYING)
	  &&   !IS_AFFECTED(ch, AFF_FLOATING) )
	    move = encumbrance( ch, movement_loss[UMIN(SECT_MAX-1, in_room->sector_type)] );
	  else
	    move = 1;
	  if ( ch->move < move )
	  {
	    send_to_char( "El cansancio te impide dar un paso más.\n\r", ch );
	    return rNONE;
	  }
	}

	WAIT_STATE( ch, move );
	if ( ch->mount )
	  ch->mount->move -= move;
	else
	  ch->move -= move;
    }

    /*
     * Check if player can fit in the room
     */
    if ( to_room->tunnel > 0 )
    {
	CHAR_DATA *ctmp;
	int count = ch->mount ? 1 : 0;
	
	for ( ctmp = to_room->first_person; ctmp; ctmp = ctmp->next_in_room )
	  if ( ++count >= to_room->tunnel )
	  {
		if ( ch->mount && count == to_room->tunnel )
		  send_to_char( "No hay espacio allí para ti y tu montura.\n\r", ch );
		else
		  send_to_char( "No hay espacio ahí para tí.\n\r", ch );
		return rNONE;
	  }
    }

    /* check for traps on exit - later */

    if ( !IS_AFFECTED(ch, AFF_SNEAK)
    && ( IS_NPC(ch) || !IS_SET(ch->act, PLR_WIZINVIS) ) )
    {
      if ( fall )
        txt = "cae hacia";
      else
      if ( !txt )
      {
        if ( ch->mount )
        {
	  if ( IS_AFFECTED( ch->mount, AFF_FLOATING ) )
	    txt = "flota hacia";
 	  else
	  if ( IS_AFFECTED( ch->mount, AFF_FLYING ) )
	    txt = "vuela hacia";
	  else
	    txt = "cavalga hacia";
        }
        else
        {
	  if ( IS_AFFECTED( ch, AFF_FLOATING ) )
	  {
	    if ( drunk )
	      txt = "flota inestablemente hacia";
	     else
	      txt = "flota hacia";
	  }
	  else
	  if ( IS_AFFECTED( ch, AFF_FLYING ) )
	  {
	    if ( drunk )
	      txt = "vuela inestablemente";
	     else
	      txt = "vuela hacia";
	  }
	  else
          if ( ch->position == POS_SHOVE )
            txt = "es empujado hacia";
 	  else
	  if ( ch->position == POS_DRAG )
            txt = "es arrastrado hacia";
  	  else
	  {
	    if ( drunk )
	      txt = "tropieza hacia";
	     else
	     {
		if(ch->race == RACE_ASTROMECH_DROID)
		  txt = "rueda hacia";
		else
  	          txt = "se va hacia";
	     }
	  }
        }
      }
      if ( ch->mount )
      {
	sprintf( buf, "$n %s %s encima de $N.", txt, dir_name3[door] );
	act( AT_ACTION, buf, ch, NULL, ch->mount, TO_NOTVICT );
      }
      else
      {
	sprintf( buf, "$n %s $T.", txt );
	act( AT_ACTION, buf, ch, NULL, dir_name3[door], TO_ROOM );
      }
    }

    rprog_leave_trigger( ch );
    if( char_died(ch) )
      return global_retcode;

    char_from_room( ch );
    char_to_room(ch, to_room); 
   if ( ch->mount )
    {
      rprog_leave_trigger( ch->mount );

      if( char_died(ch->mount) )
        return global_retcode;

      if( ch->mount )
      {
        char_from_room( ch->mount );
        char_to_room( ch->mount, to_room );
      }
    }


 //   char_to_room( ch, to_room );
    if ( !IS_AFFECTED(ch, AFF_SNEAK)
    && ( IS_NPC(ch) || !IS_SET(ch->act, PLR_WIZINVIS) ) )
    {
      if ( fall )
        txt = "cae";
      else
      if ( ch->mount )
      {
	if ( IS_AFFECTED( ch->mount, AFF_FLOATING ) )
	  txt = "flota";
	else
	if ( IS_AFFECTED( ch->mount, AFF_FLYING ) )
	  txt = "vuela";
	else
	  txt = "cavalga en";
      }
      else
      {
	if ( IS_AFFECTED( ch, AFF_FLOATING ) )
	{
	  if ( drunk )
	    txt = "flota inestablemente";
	   else
	    txt = "flota";
	}
	else
	if ( IS_AFFECTED( ch, AFF_FLYING ) )
	{
	  if ( drunk )
	    txt = "vuela temblorosamente";
	   else
	    txt = "vuela";
	}
	else
	if ( ch->position == POS_SHOVE )
          txt = "es empujado";
	else
	if ( ch->position == POS_DRAG )
	  txt = "es arrastrado";
  	else
	{
	  if ( drunk )
	    txt = "tropieza";
	   else
	   {
            if(ch->race == RACE_ASTROMECH_DROID) 
	      txt = "rueda";
	    else
	      txt = "llega";
	   }
	}
      }
      switch( door )
      {
      default: dtxt = "algún lugar";	break;
      case 0:  dtxt = "el sur";	break;
      case 1:  dtxt = "el oeste";	break;
      case 2:  dtxt = "el norte";	break;
      case 3:  dtxt = "el este";	break;
      case 4:  dtxt = "abajo";		break;
      case 5:  dtxt = "arriba";		break;
      case 6:  dtxt = "el suroeste";	break;
      case 7:  dtxt = "el sureste";	break;
      case 8:  dtxt = "el noroeste";	break;
      case 9:  dtxt = "el noreste";	break;
      }
      if ( ch->mount )
      {
	sprintf( buf, "$n %s desde %s encima de $N.", txt, dtxt );
	act( AT_ACTION, buf, ch, NULL, ch->mount, TO_ROOM );
      }
      else
      {
	sprintf( buf, "$n %s desde %s.", txt, dtxt );
	act( AT_ACTION, buf, ch, NULL, NULL, TO_ROOM );
      }
    }

    if ( !IS_IMMORTAL(ch)
    &&  !IS_NPC(ch)
    &&  ch->in_room->area != to_room->area )
    {
	if ( ch->top_level < to_room->area->low_soft_range )
	{
	   set_char_color( AT_MAGIC, ch );
	   send_to_char("Te sientes mal en esta tierra extraña...\n\r", ch);
	}
	else
	if ( ch->top_level > to_room->area->hi_soft_range )
	{
	   set_char_color( AT_MAGIC, ch );
	   send_to_char("No deberías estar aquí...\n\r", ch);
	}
    }

    do_look( ch, "auto" );
    if ( brief ) 
      SET_BIT( ch->act, PLR_BRIEF );


    /* BIG ugly looping problem here when the character is mptransed back
       to the starting room.  To avoid this, check how many chars are in 
       the room at the start and stop processing followers after doing
       the right number of them.  -- Narn
    */
    if ( !fall )
    {
      CHAR_DATA *fch;
      CHAR_DATA *nextinroom;
      int chars = 0, count = 0;

      for ( fch = from_room->first_person; fch; fch = fch->next_in_room )
        chars++;

      for ( fch = from_room->first_person; fch && ( count < chars ); fch = nextinroom )
      {
	nextinroom = fch->next_in_room;
        count++;
	if ( fch != ch		/* loop room bug fix here by Thoric */
	&& fch->master == ch
	&& fch->position == POS_STANDING )
	{
            if( !get_exit( from_room, door ) )
            {
              act( AT_ACTION, "¡La entrada se cierra tras $N, impidiendo que le sigas!", fch, NULL, ch, TO_CHAR );
              continue;
            }
            act( AT_ACTION, "Sigues a $N.", fch, NULL, ch, TO_CHAR ); 
            move_char( fch, pexit, 0 );
	}
      }
    }

    if ( ch->in_room->first_content )
      retcode = check_room_for_traps( ch, TRAP_ENTER_ROOM );
    if ( retcode != rNONE )
      return retcode;

    if ( char_died(ch) )
      return retcode;

    mprog_entry_trigger( ch );
    if ( char_died(ch) )
      return retcode;

    rprog_enter_trigger( ch );
    if ( char_died(ch) )
       return retcode;

    mprog_greet_trigger( ch );
    if ( char_died(ch) )
       return retcode;

    oprog_greet_trigger( ch );
    if ( char_died(ch) )
       return retcode;

    if (!will_fall( ch, fall )
    &&   fall > 0 )
    {
	if (!IS_AFFECTED( ch, AFF_FLOATING )
	|| ( ch->mount && !IS_AFFECTED( ch->mount, AFF_FLOATING ) ) )
	{
	  set_char_color( AT_HURT, ch );
	  send_to_char( "¡OUCH! ¡Te golpeas contra el suelo!\n\r", ch );
	  WAIT_STATE( ch, 20 );
	  retcode = damage( ch, ch, 50 * fall, TYPE_UNDEFINED );
	}
	else
	{
	  set_char_color( AT_MAGIC, ch );
	  send_to_char( "Flotas hacia el suelo tranquilamente.\n\r", ch );
	}
    }
    return retcode;
}


void do_north( CHAR_DATA *ch, char *argument )
{
    if (!IS_NPC(ch) && IS_SET(ch->pcdata->act2, ACT_BOUND))
    {
        send_to_char("Luchas con tus ataduras, pero es en vano.\n\r", ch);
        return;
    }

    move_char( ch, get_exit(ch->in_room, DIR_NORTH), 0 );
    return;
}


void do_east( CHAR_DATA *ch, char *argument )
{
    if (!IS_NPC(ch) && IS_SET(ch->pcdata->act2, ACT_BOUND))
    {
        send_to_char("Luchas con tus ataduras, pero es en vano.\n\r", ch);
        return;
    }

    move_char( ch, get_exit(ch->in_room, DIR_EAST), 0 );
    return;
}


void do_south( CHAR_DATA *ch, char *argument )
{
    if (!IS_NPC(ch) && IS_SET(ch->pcdata->act2, ACT_BOUND))
    {
        send_to_char("Luchas con tus ataduras, pero es en vano.\n\r", ch);
        return;
    }

    move_char( ch, get_exit(ch->in_room, DIR_SOUTH), 0 );
    return;
}


void do_west( CHAR_DATA *ch, char *argument )
{
    if (!IS_NPC(ch) && IS_SET(ch->pcdata->act2, ACT_BOUND))
    {
        send_to_char("luchas con tus ataduras, pero es en vano.\n\r", ch);
        return;
    }

    move_char( ch, get_exit(ch->in_room, DIR_WEST), 0 );
    return;
}


void do_up( CHAR_DATA *ch, char *argument )
{
    if (!IS_NPC(ch) && IS_SET(ch->pcdata->act2, ACT_BOUND))
    {
        send_to_char("luchas con tus ataduras, pero es en vano.\n\r", ch);
        return;
    }

    move_char( ch, get_exit(ch->in_room, DIR_UP), 0 );
    return;
}


void do_down( CHAR_DATA *ch, char *argument )
{
    if (!IS_NPC(ch) && IS_SET(ch->pcdata->act2, ACT_BOUND))
    {
        send_to_char("luchas con tus ataduras, pero es en vano.\n\r", ch);
        return;
    }

    move_char( ch, get_exit(ch->in_room, DIR_DOWN), 0 );
    return;
}

void do_northeast( CHAR_DATA *ch, char *argument )
{
    if (!IS_NPC(ch) && IS_SET(ch->pcdata->act2, ACT_BOUND))
    {
        send_to_char("luchas con tus ataduras, pero es en vano.\n\r", ch);
        return;
    }

    move_char( ch, get_exit(ch->in_room, DIR_NORTHEAST), 0 );
    return;
}

void do_northwest( CHAR_DATA *ch, char *argument )
{
    if (!IS_NPC(ch) && IS_SET(ch->pcdata->act2, ACT_BOUND))
    {
        send_to_char("luchas con tus ataduras, pero es en vano.\n\r", ch);
        return;
    }

    move_char( ch, get_exit(ch->in_room, DIR_NORTHWEST), 0 );
    return;
}

void do_southeast( CHAR_DATA *ch, char *argument )
{
    if (!IS_NPC(ch) && IS_SET(ch->pcdata->act2, ACT_BOUND))
    {
        send_to_char("luchas con tus ataduras, pero es en vano.\n\r", ch);
        return;
    }

    move_char( ch, get_exit(ch->in_room, DIR_SOUTHEAST), 0 );
    return;
}

void do_southwest( CHAR_DATA *ch, char *argument )
{
    if (!IS_NPC(ch) && IS_SET(ch->pcdata->act2, ACT_BOUND))
    {
        send_to_char("luchas con tus ataduras, pero es en vano.\n\r", ch);
        return;
    }

    move_char( ch, get_exit(ch->in_room, DIR_SOUTHWEST), 0 );
    return;
}

EXIT_DATA *find_door( CHAR_DATA *ch, char *arg, bool quiet )
{
    EXIT_DATA *pexit;
    int door;

    if (arg == NULL || !str_cmp(arg,""))
      return NULL;

    pexit = NULL;
	 if ( !str_cmp( arg, "n"  ) || !str_cmp( arg, "north"	  ) || !str_cmp( arg, "norte"	  ) ) door = 0;
    else if ( !str_cmp( arg, "e"  ) || !str_cmp( arg, "east"	  ) || !str_cmp( arg, "este"	  ) ) door = 1;
    else if ( !str_cmp( arg, "s"  ) || !str_cmp( arg, "south"	  ) || !str_cmp( arg, "sur"	  ) ) door = 2;
    else if ( !str_cmp( arg, "w"  ) || !str_cmp( arg, "west"	  ) || !str_cmp( arg, "oeste"	  ) || !str_cmp( arg, "o"	  ) ) door = 3;
    else if ( !str_cmp( arg, "u"  ) || !str_cmp( arg, "up"	  ) || !str_cmp( arg, "ar"	  ) || !str_cmp( arg, "arriba"	  ) ) door = 4;
    else if ( !str_cmp( arg, "d"  ) || !str_cmp( arg, "down"	  ) || !str_cmp( arg, "ab"	  ) || !str_cmp( arg, "abajo"	  ) ) door = 5;
    else if ( !str_cmp( arg, "ne" ) || !str_cmp( arg, "northeast" ) || !str_cmp( arg, "noreste"	  ) ) door = 6;
    else if ( !str_cmp( arg, "nw" ) || !str_cmp( arg, "northwest" ) || !str_cmp( arg, "noroeste"	  ) || !str_cmp( arg, "no"	  ) ) door = 7;
    else if ( !str_cmp( arg, "se" ) || !str_cmp( arg, "southeast" ) || !str_cmp( arg, "sureste"	  ) ) door = 8;
    else if ( !str_cmp( arg, "sw" ) || !str_cmp( arg, "southwest" ) || !str_cmp( arg, "so"	  ) || !str_cmp( arg, "suroeste"	  ) ) door = 9;
    else
    {
	for ( pexit = ch->in_room->first_exit; pexit; pexit = pexit->next )
	{
	    if ( (quiet || IS_SET(pexit->exit_info, EX_ISDOOR))
	    &&    pexit->keyword
	    &&    nifty_is_name( arg, pexit->keyword ) )
		return pexit;
	}
	if ( !quiet )
	  act( AT_PLAIN, "No ves $T aquí.", ch, NULL, arg, TO_CHAR );
	return NULL;
    }

    if ( (pexit = get_exit( ch->in_room, door )) == NULL )
    {
	if ( !quiet)
	  act( AT_PLAIN, "No ves $T aquí.", ch, NULL, arg, TO_CHAR );
	return NULL;
    }

    if ( quiet )
	return pexit;

    if ( IS_SET(pexit->exit_info, EX_SECRET) )
    {
	act( AT_PLAIN, "No ves $T aquí.", ch, NULL, arg, TO_CHAR );
	return NULL;
    }

    if ( !IS_SET(pexit->exit_info, EX_ISDOOR) )
    {
	send_to_char( "No puedes hacer eso.\n\r", ch );
	return NULL;
    }

    return pexit;
}


void toggle_bexit_flag( EXIT_DATA *pexit, int flag )
{
    EXIT_DATA *pexit_rev;

    TOGGLE_BIT(pexit->exit_info, flag);
    if ( (pexit_rev = pexit->rexit) != NULL
    &&   pexit_rev != pexit )
	TOGGLE_BIT( pexit_rev->exit_info, flag );
}

void set_bexit_flag( EXIT_DATA *pexit, int flag )
{
    EXIT_DATA *pexit_rev;

    SET_BIT(pexit->exit_info, flag);
    if ( (pexit_rev = pexit->rexit) != NULL
    &&   pexit_rev != pexit )
	SET_BIT( pexit_rev->exit_info, flag );
}

void remove_bexit_flag( EXIT_DATA *pexit, int flag )
{
    EXIT_DATA *pexit_rev;

    REMOVE_BIT(pexit->exit_info, flag);
    if ( (pexit_rev = pexit->rexit) != NULL
    &&   pexit_rev != pexit )
	REMOVE_BIT( pexit_rev->exit_info, flag );
}

void do_open( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    EXIT_DATA *pexit;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	do_openhatch( ch , "" );
	return;
    }

    if ( ( pexit = find_door( ch, arg, TRUE ) ) != NULL )
    {
	/* 'open door' */
	EXIT_DATA *pexit_rev;

	if ( !IS_SET(pexit->exit_info, EX_ISDOOR) )
	    { send_to_char( "No puedes hacer eso.\n\r",      ch ); return; }
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "No es necesario.\n\r",      ch ); return; }
	if (  IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "Deberías desbloquear primero la cerradura.\n\r",            ch ); return; }

	if ( !IS_SET(pexit->exit_info, EX_SECRET)
	||   (pexit->keyword && nifty_is_name( arg, pexit->keyword )) )
	{
	    act( AT_ACTION, "$n abre el $d.", ch, NULL, pexit->keyword, TO_ROOM );
	    act( AT_ACTION, "Abres el $d.", ch, NULL, pexit->keyword, TO_CHAR );
	    if ( (pexit_rev = pexit->rexit) != NULL
	    &&   pexit_rev->to_room == ch->in_room )
	    {
		CHAR_DATA *rch;

		for ( rch = pexit->to_room->first_person; rch; rch = rch->next_in_room )
		    act( AT_ACTION, "el $d se abre.", rch, NULL, pexit_rev->keyword, TO_CHAR );
	        sound_to_room( pexit->to_room , "!!SOUND(door)" );
	    }
	    remove_bexit_flag( pexit, EX_CLOSED );
	    if ( (door=pexit->vdir) >= 0 && door < 10 )
		check_room_for_traps( ch, trap_door[door]);
            
            sound_to_room( ch->in_room , "!!SOUND(door)" );
            return;
	}
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* 'open object' */
	if ( obj->item_type != ITEM_CONTAINER )
	{ 
          ch_printf( ch, "%s no es un contenedor.\n\r", capitalize( obj->short_descr ) ); 
          return;
        } 
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	{ 
          ch_printf( ch, "%s ya se ha abierto.\n\r", capitalize( obj->short_descr ) ); 
          return;
        } 
	if ( !IS_SET(obj->value[1], CONT_CLOSEABLE) )
	{ 
          ch_printf( ch, "%s no se puede ni abrir ni cerrar.\n\r", capitalize( obj->short_descr ) ); 
          return;
        } 
	if ( IS_SET(obj->value[1], CONT_LOCKED) )
	{ 
          ch_printf( ch, "%s tiene la cerradura bloqueada.\n\r", capitalize( obj->short_descr ) ); 
          return;
        } 

	REMOVE_BIT(obj->value[1], CONT_CLOSED);
	act( AT_ACTION, "Abres $p.", ch, obj, NULL, TO_CHAR );
	act( AT_ACTION, "$n abre $p.", ch, obj, NULL, TO_ROOM );
	check_for_trap( ch, obj, TRAP_OPEN );
	return;
    }

    if ( !str_cmp(arg , "hatch") );
    {
      do_openhatch( ch , argument );
      return;
    }

    do_openhatch( ch , arg );
    return;
}



void do_close( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    EXIT_DATA *pexit;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	do_closehatch(  ch , "" );
	return;
    }

    if ( ( pexit = find_door( ch, arg, TRUE ) ) != NULL )
    {
	/* 'close door' */
	EXIT_DATA *pexit_rev;

	if ( !IS_SET(pexit->exit_info, EX_ISDOOR) )
	    { send_to_char( "No puedes hacer eso.\n\r",      ch ); return; }
	if ( IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "Eso no es necesario.\n\r",    ch ); return; }

	act( AT_ACTION, "$n cierra el $d.", ch, NULL, pexit->keyword, TO_ROOM );
	act( AT_ACTION, "cierras el $d.", ch, NULL, pexit->keyword, TO_CHAR );

	/* close the other side */
	if ( ( pexit_rev = pexit->rexit ) != NULL
	&&   pexit_rev->to_room == ch->in_room )
	{
	    CHAR_DATA *rch;

	    SET_BIT( pexit_rev->exit_info, EX_CLOSED );
	    for ( rch = pexit->to_room->first_person; rch; rch = rch->next_in_room )
		act( AT_ACTION, "El $d se cierra.", rch, NULL, pexit_rev->keyword, TO_CHAR );
	}
	set_bexit_flag( pexit, EX_CLOSED );
	if ( (door=pexit->vdir) >= 0 && door < 10 )
	  check_room_for_traps( ch, trap_door[door]);
        return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* 'close object' */
	if ( obj->item_type != ITEM_CONTAINER )
	{ 
          ch_printf( ch, "%s no es un contenedor.\n\r", capitalize( obj->short_descr ) ); 
          return;
        } 
	if ( IS_SET(obj->value[1], CONT_CLOSED) )
	{ 
          ch_printf( ch, "%s no necesita cerrarse.\n\r", capitalize( obj->short_descr ) ); 
          return;
        } 
	if ( !IS_SET(obj->value[1], CONT_CLOSEABLE) )
        { 
          ch_printf( ch, "%s no necesita abrirse ni cerrarse.\n\r", capitalize( obj->short_descr ) ); 
          return;
        } 

	SET_BIT(obj->value[1], CONT_CLOSED);
	act( AT_ACTION, "Cierras $p.", ch, obj, NULL, TO_CHAR );
	act( AT_ACTION, "$n cierra $p.", ch, obj, NULL, TO_ROOM );
	check_for_trap( ch, obj, TRAP_CLOSE );
	return;
    }

    if ( !str_cmp( arg , "hatch" ) )
    {
    	do_closehatch( ch , argument );
    	return;
    }

    	do_closehatch( ch , arg );
    return;
}


bool has_key( CHAR_DATA *ch, int key )
{
    OBJ_DATA *obj;

    for ( obj = ch->first_carrying; obj; obj = obj->next_content )
	if ( obj->pIndexData->vnum == key || obj->value[0] == key )
	    return TRUE;

    return FALSE;
}


void do_lock( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    EXIT_DATA *pexit;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "¿Bloquear qué?\n\r", ch );
	return;
    }

    if ( ( pexit = find_door( ch, arg, TRUE ) ) != NULL )
    {
	/* 'lock door' */

	if ( !IS_SET(pexit->exit_info, EX_ISDOOR) )
	    { send_to_char( "No puedes hacer eso.\n\r",      ch ); return; }
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "Cierra primero.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "No tiene cerradura.\n\r",     ch ); return; }
	if ( !has_key( ch, pexit->key) )
	    { send_to_char( "Te falta la llave.\n\r",       ch ); return; }
	if ( IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "Ya está.\n\r",    ch ); return; }

	if ( !IS_SET(pexit->exit_info, EX_SECRET)
	||   (pexit->keyword && nifty_is_name( arg, pexit->keyword )) )
	{
	    send_to_char( "*Click*\n\r", ch );
	    act( AT_ACTION, "$n desbloquea el $d.", ch, NULL, pexit->keyword, TO_ROOM );
	    set_bexit_flag( pexit, EX_LOCKED );
            return;
        }
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* 'lock object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "No es un contenedor.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "Cierra primero.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "No tiene cerradura.\n\r",     ch ); return; }
	if ( !has_key( ch, obj->value[2] ) )
	    { send_to_char( "Te falta la llave.\n\r",       ch ); return; }
	if ( IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "Ya está.\n\r",    ch ); return; }

	SET_BIT(obj->value[1], CONT_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( AT_ACTION, "$n bloquea $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    ch_printf( ch, "No ves %s aquí.\n\r", arg );
    return;
}



void do_unlock( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    EXIT_DATA *pexit;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "¿Desbloquear aqué?\n\r", ch );
	return;
    }

    if ( ( pexit = find_door( ch, arg, TRUE ) ) != NULL )
    {
	/* 'unlock door' */

	if ( !IS_SET(pexit->exit_info, EX_ISDOOR) )
	    { send_to_char( "No puedes hacer eso.\n\r",      ch ); return; }
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "Cierra primero.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "No tiene cerradura.\n\r",   ch ); return; }
	if ( !has_key( ch, pexit->key) )
	    { send_to_char( "Te falta la llave.\n\r",       ch ); return; }
	if ( !IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "ya está.\n\r",  ch ); return; }

	if ( !IS_SET(pexit->exit_info, EX_SECRET)
	||   (pexit->keyword && nifty_is_name( arg, pexit->keyword )) )
	{
	    send_to_char( "*Click*\n\r", ch );
	    act( AT_ACTION, "$n desbloquea $d.", ch, NULL, pexit->keyword, TO_ROOM );
	    remove_bexit_flag( pexit, EX_LOCKED );
            return;   
	}
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* 'unlock object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "No es un contenedor.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "Cierra primero.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "No tiene cerradura.\n\r",   ch ); return; }
	if ( !has_key( ch, obj->value[2] ) )
	    { send_to_char( "Te falta la llave.\n\r",       ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "Ya está.\n\r",  ch ); return; }

	REMOVE_BIT(obj->value[1], CONT_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( AT_ACTION, "$n desbloquea $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    ch_printf( ch, "You see no %s here.\n\r", arg );
    return;
}

void do_bashdoor( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA *gch;
	EXIT_DATA *pexit;
	char       arg [ MAX_INPUT_LENGTH ];

	if ( !IS_NPC( ch )
	&&  ch->pcdata->learned[gsn_bashdoor] <= 0  )
	{
	    send_to_char( "¡No eres un guerrero para ir derribando puertas!\n\r", ch );
	    return;
	}

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
	    send_to_char( "¿Derribar qué?\n\r", ch );
	    return;
	}

	if ( ch->fighting )
	{
	    send_to_char( "Termina tu combate primero.\n\r", ch );
	    return;
	}

	if ( ( pexit = find_door( ch, arg, FALSE ) ) != NULL )
	{
	    ROOM_INDEX_DATA *to_room;
	    EXIT_DATA       *pexit_rev;
	    int              chance;
	    char	    *keyword;

	    if ( !IS_SET( pexit->exit_info, EX_CLOSED ) )
	    {
		send_to_char( "¡Calma! La salida está abierta.\n\r", ch );
		return;
	    }

	    WAIT_STATE( ch, skill_table[gsn_bashdoor]->beats );

	    if ( IS_SET( pexit->exit_info, EX_SECRET ) )
		keyword = "la pared";
	    else
		keyword = pexit->keyword;
	    if ( !IS_NPC(ch) )
		chance = ch->pcdata->learned[gsn_bashdoor] / 2;
	    else
		chance = 90;

	    if ( !IS_SET( pexit->exit_info, EX_BASHPROOF )
	    &&   ch->move >= 15
	    &&   number_percent( ) < ( chance + 4 * ( get_curr_str( ch ) - 19 ) ) )
	    {
		REMOVE_BIT( pexit->exit_info, EX_CLOSED );
		if ( IS_SET( pexit->exit_info, EX_LOCKED ) )
		REMOVE_BIT( pexit->exit_info, EX_LOCKED );
		SET_BIT( pexit->exit_info, EX_BASHED );

		act(AT_SKILL, "¡Crash!  ¡Derribas $d!", ch, NULL, keyword, TO_CHAR );
		act(AT_SKILL, "¡$n derriba $d!",          ch, NULL, keyword, TO_ROOM );
		learn_from_success(ch, gsn_bashdoor);

		if ( (to_room = pexit->to_room) != NULL
		&&   (pexit_rev = pexit->rexit) != NULL
		&&    pexit_rev->to_room	== ch->in_room )
		{
			CHAR_DATA *rch;

			REMOVE_BIT( pexit_rev->exit_info, EX_CLOSED );
			if ( IS_SET( pexit_rev->exit_info, EX_LOCKED ) )
			  REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
			SET_BIT( pexit_rev->exit_info, EX_BASHED );

			for ( rch = to_room->first_person; rch; rch = rch->next_in_room )
			{
			    act(AT_SKILL, "El $d se cae!",
				rch, NULL, pexit_rev->keyword, TO_CHAR );
			}
		}
		damage( ch, ch, ( ch->max_hit / 20 ), gsn_bashdoor );

	    }
	    else
	    {
		act(AT_SKILL, "¡BAAAAM!!!  Tratas de derribar $d, pero ni se mueve.",
			ch, NULL, keyword, TO_CHAR );
		act(AT_SKILL, "¡BAAAAM!!!  $n trata de derribar $d, pero ni se mueve.",
			ch, NULL, keyword, TO_ROOM );
		damage( ch, ch, ( ch->max_hit / 20 ) + 10, gsn_bashdoor );
		learn_from_failure(ch, gsn_bashdoor);
	    }
	}    
	else
	{
	    act(AT_SKILL, "¡BAAAAM!!!  Tratas de derribar la pared pero ni se mueve.",
		ch, NULL, NULL, TO_CHAR );
	    act(AT_SKILL, "¡BAAAAM!!!  $n trata de derribar la pared pero ni se mueve.",
		ch, NULL, NULL, TO_ROOM );
	    damage( ch, ch, ( ch->max_hit / 20 ) + 10, gsn_bashdoor );
	    learn_from_failure(ch, gsn_bashdoor);
	}
	if ( !char_died( ch ) )
	   for ( gch = ch->in_room->first_person; gch; gch = gch->next_in_room )
	   {
		 if ( IS_AWAKE( gch )
		 && !gch->fighting
		 && ( IS_NPC( gch ) && !IS_AFFECTED( gch, AFF_CHARM ) )
		 && ( ch->top_level - gch->top_level <= 4 )
		 && number_bits( 2 ) == 0 )
		 multi_hit( gch, ch, TYPE_UNDEFINED );
	   }

        return;
}


void do_stand(CHAR_DATA * ch, char *argument)
{

   OBJ_DATA *obj = NULL;
   int aon = 0;
   CHAR_DATA *fch = NULL;
   int val0;
   int val1;

   if (ch->position == POS_FIGHTING)
   {
      send_to_char("¡Termina primero el combate?\n\r", ch);
      return;
   }

   if (ch->position == POS_MOUNTED)
   {
      send_to_char("Estás sobre tu montura.\n\r", ch);
      return;
   }
   /* okay, now that we know we can sit, find an object to sit on */
   if (argument[0] != '\0')
   {
      obj = get_obj_list(ch, argument, ch->in_room->first_content);
      if (obj == NULL)
      {
         send_to_char("No ves eso aquí.\n\r", ch);
         return;
      }
      if (obj->item_type != ITEM_FURNITURE)
      {
         send_to_char("Eso no es un mueble.\n\r", ch);
         return;
      }
      if (!IS_SET(obj->value[2], STAND_ON) && !IS_SET(obj->value[2], STAND_IN) && !IS_SET(obj->value[2], STAND_AT))
      {
         send_to_char("No puedes ponerte de pie ahí.\n\r", ch);
         return;
      }
      if (obj->value[0] == 0)
         val0 = 1;
      else
         val0 = obj->value[0];
      if (ch->on != obj && count_users(obj) >= val0)
      {
         act(AT_ACTION, "No hay lugar para subirse a $p.", ch, obj, NULL, TO_CHAR);
         return;
      }
      if (ch->on == obj)
         aon = 1;
      else
         ch->on = obj;
   }
   switch (ch->position)
   {
      case POS_SLEEPING:
         if (IS_AFFECTED(ch, AFF_SLEEP))
         {
            send_to_char("¡No puedes despertar!\n\r", ch);
            return;
         }

         if (obj == NULL)
         {
            send_to_char("Te despiertas y te pones en pie.\n\r", ch);
            act(AT_ACTION, "$n se despierta y se pone en pie.", ch, NULL, NULL, TO_ROOM);
            ch->on = NULL;
         }
         else if (IS_SET(obj->value[2], STAND_AT))
         {
            act(AT_ACTION, "Te despiertas y te pones en pie en $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n se despierta y se pone en pie en $p.", ch, obj, NULL, TO_ROOM);
         }
         else if (IS_SET(obj->value[2], STAND_ON))
         {
act(AT_ACTION, "Te despiertas y te pones en pie en $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n se despierta y se pone en pie en $p.", ch, obj, NULL, TO_ROOM);
         }
         else
         {
            act(AT_ACTION, "Te despiertas y te pones en pie en $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n se despierta y se pone en pie en $p.", ch, obj, NULL, TO_ROOM);
         }
         ch->position = POS_STANDING;
         do_look(ch, "auto");
         break;

      case POS_RESTING:
      case POS_SITTING:
         if (obj == NULL)
         {
            send_to_char("Te pones en pie.\n\r", ch);
            act(AT_ACTION, "$n se pone en pie.", ch, NULL, NULL, TO_ROOM);
            ch->on = NULL;
         }
         else if (IS_SET(obj->value[2], STAND_AT))
         {
            act(AT_ACTION, "Te pones  de pie en $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n se pone de pie en $p.", ch, obj, NULL, TO_ROOM);
         }
         else if (IS_SET(obj->value[2], STAND_ON))
         {
act(AT_ACTION, "Te pones  de pie en $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n se pone de pie en $p.", ch, obj, NULL, TO_ROOM);
                  }
         else
         {
            act(AT_ACTION, "Te pones  de pie en $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n se pone de pie en $p.", ch, obj, NULL, TO_ROOM);
         }
         ch->position = POS_STANDING;
         break;

      case POS_STANDING:
         if (obj != NULL && aon != 1)
         {

            if (IS_SET(obj->value[2], STAND_AT))
            {
               act(AT_ACTION, "Te subes a $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n se sube a $p.", ch, obj, NULL, TO_ROOM);
         }
            else if (IS_SET(obj->value[2], STAND_ON))
            {
act(AT_ACTION, "Te subes a $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n se sube a $p.", ch, obj, NULL, TO_ROOM);
                     }
            else
            {
               act(AT_ACTION, "Te subes a $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n se sube a $p.", ch, obj, NULL, TO_ROOM);
         }
         }
         else if (aon == 1)
         {
            act(AT_ACTION, "Ya estás usando $p como mueble.", ch, obj, NULL, TO_CHAR);
         }
         else if (ch->on != NULL && obj == NULL)
         {
            act(AT_ACTION, "Te bajas de $p y quedas de pie en el suelo.", ch, ch->on, NULL, TO_CHAR);
            act(AT_ACTION, "$n se baja de $p y queda de pie en el suelo.", ch, ch->on, NULL, TO_ROOM);
            ch->on = NULL;
         }
         else
            send_to_char("Ya estás en pie.\n\r", ch);
         break;

   }
   if (obj != NULL)
   {
      if (obj->value[1] == 0)
         val1 = 750;
      else
         val1 = obj->value[1];
      if (max_weight(obj) > val1)
      {
         act(AT_ACTION, "$n pesa mucho para subirse a $p y lo rompe.", ch, ch->on, NULL, TO_ROOM);
         act(AT_ACTION, "Lo intentas, pero tu peso hace que $p se rompa.", ch, ch->on, NULL, TO_CHAR);
         for (fch = obj->in_room->first_person; fch != NULL; fch = fch->next_in_room)
         {
            if (fch->on == obj)
            {
               if (fch->position == POS_RESTING)
               {
                  fch->hit = (fch->hit - 30);
                  if (fch->hit <= 0)
                     fch->hit = 1;
                  act(AT_ACTION, "Tu descanso se interrumpe cuando caes al suelo al romperse $p.", fch, fch->on, NULL, TO_CHAR);
               }
               if (fch->position == POS_SLEEPING)
               {
                  fch->hit = (fch->hit - 40);
                  if (fch->hit <= 0)
                     fch->hit = 1;
                  fch->position = POS_RESTING;
                  act(AT_ACTION, "Tu sueño se interrumpe cuando caes al suelo al romperse $p.", fch, fch->on, NULL, TO_CHAR);
               }
               if (fch->position == POS_SITTING)
               {
                  fch->hit = (fch->hit - 5);
                  if (fch->hit <= 0)
                     fch->hit = 1;
                  act(AT_ACTION, "$p se rompe y caes al suelo.", fch, fch->on, NULL, TO_CHAR);
               }
               if (fch->position == POS_STANDING)
               {
                  fch->hit = (fch->hit - 55);
                  if (fch->hit <= 0)
                     fch->hit = 1;
                  act(AT_ACTION, "Caes al suelo cuando $p se rompe.", fch, fch->on, NULL, TO_CHAR);
               }
               fch->on = NULL;
            }
         }
         make_scraps(obj);
      }
   }

   return;
}


void do_sit(CHAR_DATA * ch, char *argument)
{

   OBJ_DATA *obj = NULL;
   int aon = 0;
   CHAR_DATA *fch = NULL;
   int val0;
   int val1;

   if (ch->position == POS_FIGHTING)
   {
      send_to_char("¡Deberías terminar el combate primero?\n\r", ch);
      return;
   }

   if (ch->position == POS_MOUNTED)
   {
      send_to_char("Ya estás sobre tu montura.\n\r", ch);
      return;
   }
   /* okay, now that we know we can sit, find an object to sit on */
   if (argument[0] != '\0')
   {
      obj = get_obj_list(ch, argument, ch->in_room->first_content);
      if (obj == NULL)
      {
         send_to_char("No ves eso aquí.\n\r", ch);
         return;
      }
      if (obj->item_type != ITEM_FURNITURE)
      {
         send_to_char("Eso no es un mueble.\n\r", ch);
         return;
      }
      if (!IS_SET(obj->value[2], SIT_ON) && !IS_SET(obj->value[2], SIT_IN) && !IS_SET(obj->value[2], SIT_AT))
      {
         send_to_char("No puedes sentarte ahí.\n\r", ch);
         return;
      }
      if (obj->value[0] == 0)
         val0 = 1;
      else
         val0 = obj->value[0];
      if (ch->on != obj && count_users(obj) >= val0)
      {
         act(AT_ACTION, "No hay lugar para sentarte en $p.", ch, obj, NULL, TO_CHAR);
         return;
      }
      if (ch->on == obj)
         aon = 1;
      else
         ch->on = obj;
   }
   if (ch->on && argument[0] == '\0')
   {
      if (!IS_SET(ch->on->value[2], SIT_AT) && !IS_SET(ch->on->value[2], SIT_ON) &&
          !IS_SET(ch->on->value[2], SIT_IN))
      {
         send_to_char("No puedes descansar ahí.\n\r", ch);
         return;
      }
   }
   switch (ch->position)
   {
      case POS_SLEEPING:
         if (obj == NULL)
         {
            send_to_char("Te despiertas y te sientas.\n\r", ch);
            act(AT_ACTION, "$n se despierta y se sienta.", ch, NULL, NULL, TO_ROOM);
            ch->on = NULL;
         }
         else if (IS_SET(obj->value[2], SIT_AT))
         {
            act(AT_ACTION, "Te despiertas y te sientas en $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n se despierta y se sienta en $p.", ch, obj, NULL, TO_ROOM);
         }
         else if (IS_SET(obj->value[2], SIT_ON))
         {
act(AT_ACTION, "Te despiertas y te sientas en $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n se despierta y se sienta en $p.", ch, obj, NULL, TO_ROOM);
                  }
         else
         {
            act(AT_ACTION, "Te despiertas y te sientas en $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n se despierta y se sienta en $p.", ch, obj, NULL, TO_ROOM);
         }

         ch->position = POS_SITTING;
         break;
      case POS_RESTING:
         if (obj == NULL)
         {
            send_to_char("Dejas de descansar.\n\r", ch);
            ch->on = NULL;
         }
         else if (IS_SET(obj->value[2], SIT_AT))
         {
            act(AT_ACTION, "Te sientas en $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n se sienta en $p.", ch, obj, NULL, TO_ROOM);
         }

         else if (IS_SET(obj->value[2], SIT_ON))
         {
act(AT_ACTION, "Te sientas en $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n se sienta en $p.", ch, obj, NULL, TO_ROOM);
                  }
         ch->position = POS_SITTING;
         break;
      case POS_SITTING:
         if (obj != NULL && aon != 1)
         {

            if (IS_SET(obj->value[2], SIT_AT))
            {
               act(AT_ACTION, "Te sientas en $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n se sienta en $p.", ch, obj, NULL, TO_ROOM);
         }
            else if (IS_SET(obj->value[2], STAND_ON))
            {
               act(AT_ACTION, "Te sientas en $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n se sienta en $p.", ch, obj, NULL, TO_ROOM);
         }
            else
            {
               act(AT_ACTION, "Te sientas en $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n se sienta en $p.", ch, obj, NULL, TO_ROOM);
         }
         }
         else if (aon == 1)
         {
            act(AT_ACTION, "Ya estás usando $p como mueble.", ch, obj, NULL, TO_CHAR);
         }
         else if (ch->on != NULL && obj == NULL)
         {
            act(AT_ACTION, "Te bajas de $p y te sientas en el suelo.", ch, ch->on, NULL, TO_CHAR);
            act(AT_ACTION, "$n se baja de $p y se sienta en el suelo.", ch, ch->on, NULL, TO_ROOM);
            ch->on = NULL;
         }
         else
            send_to_char("ya lo estás.\n\r", ch);
         break;
      case POS_STANDING:
         if (obj == NULL)
         {
            send_to_char("Te sientas en el suelo.\n\r", ch);
            act(AT_ACTION, "$n se sienta en el suelo.", ch, NULL, NULL, TO_ROOM);
            ch->on = NULL;
         }
         else if (IS_SET(obj->value[2], SIT_AT))
         {
            act(AT_ACTION, "Te sientas en $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n se sienta en $p.", ch, obj, NULL, TO_ROOM);
         }
         else if (IS_SET(obj->value[2], SIT_ON))
         {
            act(AT_ACTION, "Te sientas en $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n se sienta en $p.", ch, obj, NULL, TO_ROOM);
         }
         else
         {
            act(AT_ACTION, "Te sientas en $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n se sienta en $p.", ch, obj, NULL, TO_ROOM);
         }
         ch->position = POS_SITTING;
         break;
   }
   if (obj != NULL)
   {
      if (obj->value[1] == 0)
         val1 = 750;
      else
         val1 = obj->value[1];
      if (max_weight(obj) > val1)
      {
         act(AT_ACTION, "El peso de $n es demasiado para $p.", ch, ch->on, NULL, TO_ROOM);
         act(AT_ACTION, "Tu intento de sentarte sobre $p causa que se rompa.", ch, ch->on, NULL, TO_CHAR);
         for (fch = obj->in_room->first_person; fch != NULL; fch = fch->next_in_room)
         {
            if (fch->on == obj)
            {
               if (fch->position == POS_RESTING)
               {
                  fch->hit = (fch->hit - 30);
                  if (fch->hit <= 0)
                     fch->hit = 1;
                  act(AT_ACTION, "Tu descanso es interrumpido cuando $p se rompe.", fch, fch->on, NULL, TO_CHAR);
               }
               if (fch->position == POS_SLEEPING)
               {
                  fch->hit = (fch->hit - 40);
                  if (fch->hit <= 0)
                     fch->hit = 1;
                  fch->position = POS_RESTING;
                  act(AT_ACTION, "Tu sueño es interrumpido cuando $p se rompe.", fch, fch->on, NULL, TO_CHAR);
               }

               if (fch->position == POS_SITTING)
               {
                  fch->hit = (fch->hit - 5);
                  if (fch->hit <= 0)
                     fch->hit = 1;
                  act(AT_ACTION, "Tu descanso es interrumpido cuando $p se rompe.", fch, fch->on, NULL, TO_CHAR);
               }
               if (fch->position == POS_STANDING)
               {
                  fch->hit = (fch->hit - 55);
                  if (fch->hit <= 0)
                     fch->hit = 1;
                  act(AT_ACTION, "Caes al suelo cuando $p se compe.", fch, fch->on, NULL, TO_CHAR);
               }
               fch->on = NULL;
            }
         }
         make_scraps(obj);
      }
   }

   return;
}


void do_rest(CHAR_DATA * ch, char *argument)
{

   OBJ_DATA *obj = NULL;
   int aon = 0;
   CHAR_DATA *fch = NULL;
   int val0;
   int val1;

   if (ch->position == POS_FIGHTING)
   {
      send_to_char("¡Termina primero tu combate?\n\r", ch);
      return;
   }

   if (ch->position == POS_MOUNTED)
   {
      send_to_char("Ya estás en tu montura.\n\r", ch);
      return;
   }
   /* okay, now that we know we can sit, find an object to sit on */
   if (argument[0] != '\0')
   {
      obj = get_obj_list(ch, argument, ch->in_room->first_content);
      if (obj == NULL)
      {
         send_to_char("NO ves eso aquí.\n\r", ch);
         return;
      }
      if (obj->item_type != ITEM_FURNITURE)
      {
         send_to_char("Eso no es un mueble.\n\r", ch);
         return;
      }
      if (!IS_SET(obj->value[2], REST_ON) && !IS_SET(obj->value[2], REST_IN) && !IS_SET(obj->value[2], REST_AT))
      {
         send_to_char("No puedes descansar ahí.\n\r", ch);
         return;
      }
      if (obj->value[0] == 0)
         val0 = 1;
      else
         val0 = obj->value[0];
      if (ch->on != obj && count_users(obj) >= val0)
      {
         act(AT_ACTION, "No hay espacio para descansar en $p.", ch, obj, NULL, TO_CHAR);
         return;
      }
      if (ch->on == obj)
         aon = 1;
      else
         ch->on = obj;
   }
   if (ch->on && argument[0] == '\0')
   {
      if (!IS_SET(ch->on->value[2], REST_AT) && !IS_SET(ch->on->value[2], REST_ON) &&
          !IS_SET(ch->on->value[2], REST_IN))
      {
         send_to_char("No puedes descansaar ahí.\n\r", ch);
         return;
      }
   }

   switch (ch->position)
   {
      case POS_SLEEPING:
         if (obj == NULL)
         {
            send_to_char("Te despiertas y comienzas a descansar.\n\r", ch);
            act(AT_ACTION, "$n se despierta y comienza a descansar.", ch, NULL, NULL, TO_ROOM);
            ch->on = NULL;
         }
         else if (IS_SET(obj->value[2], REST_AT))
         {
            act(AT_ACTION, "Te despiertas y comienzas a descansar en $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n se despierta y comienza a descansar en $p.", ch, obj, NULL, TO_ROOM);
         }
         else if (IS_SET(obj->value[2], REST_ON))
         {
act(AT_ACTION, "Te despiertas y comienzas a descansar en $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n se despierta y comienza a descansar en $p.", ch, obj, NULL, TO_ROOM);
                  }
         else
         {
            act(AT_ACTION, "Te despiertas y comienzas a descansar en $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n se despierta y comienza a descansar en $p.", ch, obj, NULL, TO_ROOM);
         }
         ch->position = POS_RESTING;
         break;

      case POS_RESTING:
         if (obj != NULL && aon != 1)
         {
            if (IS_SET(obj->value[2], REST_AT))
            {
               act(AT_ACTION, "Te pones a descansar en $p.", ch, obj, NULL, TO_CHAR);
               act(AT_ACTION, "$n se pone a descansar en $p.", ch, obj, NULL, TO_ROOM);
            }
            else if (IS_SET(obj->value[2], REST_ON))
            {
act(AT_ACTION, "Te pones a descansar en $p.", ch, obj, NULL, TO_CHAR);
               act(AT_ACTION, "$n se pone a descansar en $p.", ch, obj, NULL, TO_ROOM);
             }
            else
            {
               act(AT_ACTION, "Te pones a descansar en $p.", ch, obj, NULL, TO_CHAR);
               act(AT_ACTION, "$n se pone a descansar en $p.", ch, obj, NULL, TO_ROOM);
            }
         }
         else if (aon == 1)
         {
            act(AT_ACTION, "Ya usas $p como mueble.", ch, obj, NULL, TO_CHAR);
         }
         else if (ch->on != NULL && obj == NULL)
         {
            act(AT_ACTION, "Te bajas de $p y comienzas a descansar en el suelo.", ch, ch->on, NULL, TO_CHAR);
            act(AT_ACTION, "$n se baja de $p y comienza a descansar en el suelo.", ch, ch->on, NULL, TO_ROOM);
            ch->on = NULL;
         }
         else
            send_to_char("Ya estás descansando.\n\r", ch);
         break;

      case POS_STANDING:
         if (obj == NULL)
         {
            send_to_char("Te sientas a descansar.\n\r", ch);
            act(AT_ACTION, "$n se sienta a descansar.", ch, NULL, NULL, TO_ROOM);
            ch->on = NULL;
         }
         else if (IS_SET(obj->value[2], REST_AT))
         {
            act(AT_ACTION, "Te sientas a descansar en $p and rest.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n se sienta a descansar en $p and rests.", ch, obj, NULL, TO_ROOM);
         }
         else if (IS_SET(obj->value[2], REST_ON))
         {
act(AT_ACTION, "Te sientas a descansar en $p and rest.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n se sienta a descansar en $p and rests.", ch, obj, NULL, TO_ROOM);
                  }
         else
         {
            act(AT_ACTION, "Te sientas a descansar en $p and rest.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n se sienta a descansar en $p and rests.", ch, obj, NULL, TO_ROOM);
         }
         ch->position = POS_RESTING;
         break;

      case POS_SITTING:
         if (obj == NULL)
         {
            send_to_char("Te sientas a descansar.\n\r", ch);
            act(AT_ACTION, "$n se sienta a descansar.", ch, NULL, NULL, TO_ROOM);
            ch->on = NULL;
         }
         else if (IS_SET(obj->value[2], REST_AT))
         {
            act(AT_ACTION, "Te sientas a descansar en $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n se sienta a descansar en $p.", ch, obj, NULL, TO_ROOM);
         }
         else if (IS_SET(obj->value[2], REST_ON))
         {
                        act(AT_ACTION, "Te sientas a descansar en $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n se sienta a descansar en $p.", ch, obj, NULL, TO_ROOM);
		 }
else
         {
                        act(AT_ACTION, "Te sientas a descansar en $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n se sienta a descansar en $p.", ch, obj, NULL, TO_ROOM);
		 }
         ch->position = POS_RESTING;
         break;
   }
   if (obj != NULL)
   {
      if (obj->value[1] == 0)
         val1 = 750;
      else
         val1 = obj->value[1];
      if (max_weight(obj) > val1)
      {
         act(AT_ACTION, "El peso de $n es demasiado para $p.", ch, ch->on, NULL, TO_ROOM);
         act(AT_ACTION, "Tu intento de sentarte sobre $p causa que se rompa.", ch, ch->on, NULL, TO_CHAR);
         for (fch = obj->in_room->first_person; fch != NULL; fch = fch->next_in_room)
         {
            if (fch->on == obj)
            {
               if (fch->position == POS_RESTING)
               {
                  fch->hit = (fch->hit - 30);
                  if (fch->hit <= 0)
                     fch->hit = 1;
                  act(AT_ACTION, "Tu descanso se interrumpe cuando caes al suelo al romperse $p.", fch, fch->on, NULL, TO_CHAR);
               }
               if (fch->position == POS_SLEEPING)
               {
                  fch->hit = (fch->hit - 40);
                  if (fch->hit <= 0)
                     fch->hit = 1;
                  fch->position = POS_RESTING;
                  act(AT_ACTION, "Tu sueño se interrumpe cuando caes al suelo después de que $p se rompa.", fch, fch->on, NULL, TO_CHAR);
               }
               if (fch->position == POS_SITTING)
               {
                  fch->hit = (fch->hit - 5);
                  if (fch->hit <= 0)
                     fch->hit = 1;
                  act(AT_ACTION, "Tu descanso se interrumpe cuando $p se rompe y caes al suelo.", fch, fch->on, NULL, TO_CHAR);
               }
               if (fch->position == POS_STANDING)
               {
                  fch->hit = (fch->hit - 55);
                  if (fch->hit <= 0)
                     fch->hit = 1;
                  act(AT_ACTION, "Caes al suelo cuando $p se rompe.", fch, fch->on, NULL, TO_CHAR);
               }
               fch->on = NULL;
            }
         }
         make_scraps(obj);
      }
   }

   rprog_rest_trigger(ch);
   return;
}

void do_sleep(CHAR_DATA * ch, char *argument)
{
   OBJ_DATA *obj = NULL;
   int aon = 0;
   CHAR_DATA *fch = NULL;
   int val0;
   int val1;

   if (ch->position == POS_FIGHTING)
   {
      send_to_char("¡Termina el combate primero?\n\r", ch);
      return;
   }

   if (ch->position == POS_MOUNTED)
   {
      send_to_char("Si quieres poder dormir, desmonta primero.\n\r", ch);
      return;
   }
   /* okay, now that we know we can sit, find an object to sit on */
   if (argument[0] != '\0')
   {
      obj = get_obj_list(ch, argument, ch->in_room->first_content);
      if (obj == NULL)
      {
         send_to_char("No puedes dormir ahí.\n\r", ch);
         return;
      }
      if (obj->item_type != ITEM_FURNITURE)
      {
         send_to_char("Eso no es un mueble.\n\r", ch);
         return;
      }
      if (!IS_SET(obj->value[2], SLEEP_ON) && !IS_SET(obj->value[2], SLEEP_IN) && !IS_SET(obj->value[2], SLEEP_AT))
      {
         send_to_char("No puedes dormir ahí.\n\r", ch);
         return;
      }
      if (obj->value[0] == 0)
         val0 = 1;
      else
         val0 = obj->value[0];
      if (ch->on != obj && count_users(obj) >= val0)
      {
         act(AT_ACTION, "No hay espacio para dormir en $p.", ch, obj, NULL, TO_CHAR);
         return;
      }
      if (ch->on == obj)
         aon = 1;
      else
         ch->on = obj;
   }
   if (ch->on && argument[0] == '\0')
   {
      if (!IS_SET(ch->on->value[2], SLEEP_AT) && !IS_SET(ch->on->value[2], SLEEP_ON) &&
          !IS_SET(ch->on->value[2], SLEEP_IN))
      {
         send_to_char("No puedes dormir ahí.\n\r", ch);
         return;
      }
   }
   switch (ch->position)
   {
      case POS_SLEEPING:
         if (obj != NULL && aon != 1)
         {

            if (IS_SET(obj->value[2], SLEEP_AT))
            {
               act(AT_ACTION, "Te pones a dormir en $p.", ch, obj, NULL, TO_CHAR);
               act(AT_ACTION, "$n se pone a dormir en $p.", ch, obj, NULL, TO_ROOM);
            }
            else if (IS_SET(obj->value[2], SLEEP_ON))
            {
act(AT_ACTION, "Te pones a dormir en $p.", ch, obj, NULL, TO_CHAR);
               act(AT_ACTION, "$n se pone a dormir en $p.", ch, obj, NULL, TO_ROOM);
                        }
            else
            {
               act(AT_ACTION, "Te pones a dormir en $p.", ch, obj, NULL, TO_CHAR);
               act(AT_ACTION, "$n se pone a dormir en $p.", ch, obj, NULL, TO_ROOM);
            }
         }
         else if (aon == 1)
         {
            act(AT_ACTION, "Ya estás usando $p como mueble.", ch, obj, NULL, TO_CHAR);
         }
         else if (ch->on != NULL && obj == NULL)
         {
            act(AT_ACTION, "Te bajas de $p e intentas dormir en el suelo.", ch, ch->on, NULL, TO_CHAR);
            act(AT_ACTION, "$n se baja de $p e intenta dormir en el suelo.", ch, ch->on, NULL, TO_ROOM);
            ch->on = NULL;
         }
         else
            send_to_char("Ya estás durmiendo.\n\r", ch);
         break;
      case POS_RESTING:
         if (obj == NULL)
         {
            send_to_char("Te tumbas y comienzas a dormir en el suelo.\n\r", ch);
            act(AT_ACTION, "$n se tumba y comienza a dormir en el suelo.", ch, NULL, NULL, TO_ROOM);
            ch->on = NULL;
         }
         else if (IS_SET(obj->value[2], SLEEP_AT))
         {
            act(AT_ACTION, "Comienzas a dormir en $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n comienza a dormir en $p.", ch, obj, NULL, TO_ROOM);
         }

         else if (IS_SET(obj->value[2], SLEEP_ON))
         {
            act(AT_ACTION, "Comienzas a dormir en $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n comienza a dormir en $p.", ch, obj, NULL, TO_ROOM);
         }

         else
         {
            act(AT_ACTION, "Comienzas a dormir en $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n comienza a dormir en $p.", ch, obj, NULL, TO_ROOM);
         }
         ch->position = POS_SLEEPING;
         break;

      case POS_SITTING:
         if (obj == NULL)
         {
            send_to_char("Te tumbas en el suelo y comienzas a dormir.\n\r", ch);
            act(AT_ACTION, "$n se tumba en el suelo y comienza a dormir.", ch, NULL, NULL, TO_ROOM);
            ch->on = NULL;
         }
         else if (IS_SET(obj->value[2], SLEEP_AT))
         {
            act(AT_ACTION, "Comienzas a dormir en $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n comienza a dormir en $p.", ch, obj, NULL, TO_ROOM);
         }

         else if (IS_SET(obj->value[2], SLEEP_ON))
         {
            act(AT_ACTION, "Comienzas a dormir en $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n comienza a dormir en $p.", ch, obj, NULL, TO_ROOM);
         }

         else
         {
            act(AT_ACTION, "Comienzas a dormir en $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n comienza a dormir en $p.", ch, obj, NULL, TO_ROOM);
         }
         ch->position = POS_SLEEPING;

         break;
      case POS_STANDING:
         if (obj == NULL)
         {
            send_to_char("Te tumbas en el suelo y comienzas a dormir.\n\r", ch);
            act(AT_ACTION, "$n se tumba en el suelo y comienza a dormir.", ch, NULL, NULL, TO_ROOM);
            ch->on = NULL;
         }
         else if (IS_SET(obj->value[2], SLEEP_AT))
         {
            act(AT_ACTION, "Comienzas a dormir en $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n comienza a dormir en $p.", ch, obj, NULL, TO_ROOM);
         }
         else if (IS_SET(obj->value[2], SLEEP_ON))
         {
act(AT_ACTION, "Comienzas a dormir en $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n comienza a dormir en $p.", ch, obj, NULL, TO_ROOM);
                  }
         else
         {
            act(AT_ACTION, "Comienzas a dormir en $p.", ch, obj, NULL, TO_CHAR);
            act(AT_ACTION, "$n comienza a dormir en $p.", ch, obj, NULL, TO_ROOM);
         }
         ch->position = POS_SLEEPING;
         break;
   }
   if (obj != NULL)
   {
      if (obj->value[1] == 0)
         val1 = 750;
      else
         val1 = obj->value[1];
      if (max_weight(obj) > val1)
      {
         act(AT_ACTION, "El peso de $n es demasiado para $p.", ch, ch->on, NULL, TO_ROOM);
         act(AT_ACTION, "Tu intento de dormir en $p causa que se rompa.", ch, ch->on, NULL, TO_CHAR);
         for (fch = obj->in_room->first_person; fch != NULL; fch = fch->next_in_room)
         {
            if (fch->on == obj)
            {
               if (fch->position == POS_RESTING)
               {
                  fch->hit = (fch->hit - 30);
                  if (fch->hit <= 0)
                     fch->hit = 1;
                  act(AT_ACTION, "Tu descanso se interrumpe cuando caes al suelo al romperse $p.", fch, fch->on, NULL, TO_CHAR);
               }
               if (fch->position == POS_SLEEPING)
               {
                  fch->hit = (fch->hit - 40);
                  if (fch->hit <= 0)
                     fch->hit = 1;
                  fch->position = POS_RESTING;
                  act(AT_ACTION, "Tu sueño se interrumpe cuando caes al suelo al romperse $p.", fch, fch->on, NULL, TO_CHAR);
               }
               if (fch->position == POS_SITTING)
               {
                  fch->hit = (fch->hit - 5);
                  if (fch->hit <= 0)
                     fch->hit = 1;
                  act(AT_ACTION, "Tu descanso se interrumpe cuando caes al suelo al romperse $p.", fch, fch->on, NULL, TO_CHAR);
               }
               if (fch->position == POS_STANDING)
               {
                  fch->hit = (fch->hit - 55);
                  if (fch->hit <= 0)
                     fch->hit = 1;
                  act(AT_ACTION, "Caes al suelo cuando $p se rompe.", fch, fch->on, NULL, TO_CHAR);
               }
               fch->on = NULL;
            }
         }
         make_scraps(obj);
      }
   }

   rprog_sleep_trigger(ch);
   return;
}


void do_wake(CHAR_DATA * ch, char *argument)
{
   char arg[MIL];
   CHAR_DATA *victim;

   one_argument(argument, arg);
   if (arg[0] == '\0')
   {
      do_stand(ch, argument);
      return;
   }

   if (!IS_AWAKE(ch))
   {
      send_to_char("¿Cómo pretendes hacer eso?\n\r", ch);
      return;
   }

   if ( ( victim = get_char_room( ch, arg ) ) == NULL )
   { 
      send_to_char( "No está aquí.\n\r", ch ); 
      return; 
   } 

   if (IS_AWAKE(victim))
   {
      act(AT_PLAIN, "$N no necesita despertarse.", ch, NULL, victim, TO_CHAR);
      return;
   }

   if (IS_AFFECTED(victim, AFF_SLEEP) || victim->position < POS_SLEEPING)
   {
      act(AT_PLAIN, "¡No puedes despertarle!", ch, NULL, victim, TO_CHAR);
      return;
   }

   act(AT_ACTION, "Despiertas a $M.", ch, NULL, victim, TO_CHAR);
   victim->position = POS_STANDING;
   ch->on = NULL;
   act(AT_ACTION, "$n te despierta.", ch, NULL, victim, TO_VICT);
   return;
}

/*
 * teleport a character to another room
 */
void teleportch( CHAR_DATA *ch, ROOM_INDEX_DATA *room, bool show )
{
    if ( room_is_private( ch, room ) )
      return;
    act( AT_ACTION, "¡$n desaparece de pronto!", ch, NULL, NULL, TO_ROOM );
    char_from_room( ch );
    char_to_room( ch, room );
    act( AT_ACTION, "¡$n aparece de pronto!", ch, NULL, NULL, TO_ROOM );
    if ( show )
      do_look( ch, "auto" );
}

void teleport( CHAR_DATA *ch, int room, int flags )
{
    CHAR_DATA *nch, *nch_next;
    ROOM_INDEX_DATA *pRoomIndex;
    bool show;

    pRoomIndex = get_room_index( room );
    if ( !pRoomIndex )
    {
	bug( "teleport: bad room vnum %d", room );
	return;
    }

    if ( IS_SET( flags, TELE_SHOWDESC ) )
      show = TRUE;
    else
      show = FALSE;
    if ( !IS_SET( flags, TELE_TRANSALL ) )
    {
	teleportch( ch, pRoomIndex, show );
	return;
    }
    for ( nch = ch->in_room->first_person; nch; nch = nch_next )
    {
	nch_next = nch->next_in_room;
	teleportch( nch, pRoomIndex, show );
    }
}

/*
 * "Climb" in a certain direction.				-Thoric
 */
void do_climb( CHAR_DATA *ch, char *argument )
{
    EXIT_DATA *pexit;
    bool found;

    found = FALSE;
    if ( argument[0] == '\0' )
    {
	for ( pexit = ch->in_room->first_exit; pexit; pexit = pexit->next )
	    if ( IS_SET( pexit->exit_info, EX_xCLIMB ) )
	    {
		move_char( ch, pexit, 0 );
		return;
	    }
	send_to_char( "No puedes trepar hacia allí.\n\r", ch );
	return;
    }

    if ( (pexit = find_door( ch, argument, TRUE )) != NULL
    &&   IS_SET( pexit->exit_info, EX_xCLIMB ))
    {
	move_char( ch, pexit, 0 );
	return;
    }
    send_to_char( "No puedes trepar hacia allí.\n\r", ch );
    return;
}

/*
 * "enter" something (moves through an exit)			-Thoric
 */
void do_enter( CHAR_DATA *ch, char *argument )
{
    EXIT_DATA *pexit;
    bool found;

    found = FALSE;
    if ( argument[0] == '\0' )
    {
	for ( pexit = ch->in_room->first_exit; pexit; pexit = pexit->next )
	    if ( IS_SET( pexit->exit_info, EX_xENTER ) )
	    {
		move_char( ch, pexit, 0 );
		return;
	    }
	send_to_char( "No ves ninguna entrada aquí.\n\r", ch );
	return;
    }

    if ( (pexit = find_door( ch, argument, TRUE )) != NULL
    &&   IS_SET( pexit->exit_info, EX_xENTER ))
    {
	move_char( ch, pexit, 0 );
	return;
    }
    do_board(ch,argument);
    return;
}

/*
 * Leave through an exit.					-Thoric
 */
void do_leave( CHAR_DATA *ch, char *argument )
{
    EXIT_DATA *pexit;
    bool found;

    found = FALSE;
    if ( argument[0] == '\0' )
    {
	for ( pexit = ch->in_room->first_exit; pexit; pexit = pexit->next )
	    if ( IS_SET( pexit->exit_info, EX_xLEAVE ) )
	    {
		move_char( ch, pexit, 0 );
		return;
	    }
	do_leaveship( ch , "" );
	return;
    }

    if ( (pexit = find_door( ch, argument, TRUE )) != NULL
    &&   IS_SET( pexit->exit_info, EX_xLEAVE ))
    {
	move_char( ch, pexit, 0 );
	return;
    }
    do_leaveship( ch, "" );
    return;
}

void do_keypad( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    EXIT_DATA *pexit;
    int password;
    argument = one_argument( argument, arg1 );

    password = atoi(argument);

    if ( arg1[0] == '\0' || argument[0] == '\0')
    {
        send_to_char( "Syntax: Keypad <dirección> #####\n\r", ch );
        return;
    }
    if ( ( pexit = find_door( ch, arg1, TRUE ) ) != NULL )
    {
        /* 'unlock door' */
        if (IS_SET(pexit->exit_info, EX_LOCKED)   )
        {
         if ( pexit->keypad == 0 )
             { send_to_char( "No puedes hacer eso.\n\r",      ch ); return; }

         if ( !IS_SET(pexit->exit_info, EX_ISDOOR) )
             { send_to_char( "No puedes hacer eso.\n\r",      ch ); return; }

         if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
             { send_to_char( "No está cerrada.\n\r",        ch ); return; }

         if ( pexit->keypad < 1 && !IS_SET(pexit->exit_info, EX_SECRET) )
             { send_to_char( "No puede desbloquearse.\n\r",   ch ); return; }

         if ( password != pexit->keypad )
             { send_to_char( "&RAcceso denegado.\n\r",       ch ); return; }

         if ( !IS_SET(pexit->exit_info, EX_SECRET))
    //     ||   (pexit->keyword && nifty_is_name( arg, pexit->keyword )) )
         {
             send_to_char( "&G¡Acceso autorizado!\n\rDesbloqueando...\n\r", ch );
             act( AT_ACTION, "$n teclea un código en el keypad.", ch, NULL, NULL, TO_ROOM );
             remove_bexit_flag( pexit, EX_LOCKED );
             return;

         }
        }
        else if (!IS_SET(pexit->exit_info, EX_LOCKED)   )
        {
         /*lock door*/
         if ( pexit->keypad == 0 )
             { send_to_char( "No puedes hacer eso.\n\r",      ch ); return; }

         if ( !IS_SET(pexit->exit_info, EX_ISDOOR) )
             { send_to_char( "No puedes hacer eso.\n\r",      ch ); return; }

         if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
             { send_to_char( "No está cerrada.\n\r",        ch ); return; }

         if ( pexit->keypad < 1 && !IS_SET(pexit->exit_info, EX_SECRET) )
             { send_to_char( "No se puede bloquear.\n\r",   ch ); return; }

         if ( password != pexit->keypad )
             { send_to_char( "&RAcceso denegado.\n\r",       ch ); return; }

         if ( !IS_SET(pexit->exit_info, EX_SECRET))
  //       ||   (pexit->keyword && strcmp(argument, pexit->keyword )) )
         {
             send_to_char( "&G¡Acceso autorizado!\n\rBloqueando...\n\r", ch );
             act( AT_ACTION, "$n teclea un código en el keypad.", ch, NULL, NULL, TO_ROOM );
             set_bexit_flag( pexit, EX_LOCKED );
             return;
         }
        }
    }
    ch_printf( ch, "No ves %s aquí.\n\r", arg );
    return;
}