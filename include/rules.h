#ifndef _RULES_W
#define _RULES_W

/* status display rules */
#include "man.h"

char allow_display_unitstatus( struct OBJ *a,
	int nr_player );

char allow_display_mineralstatus( struct OBJ *a, 
	int nr_player );

char allow_display_powerstatus( struct OBJ *a, 
	int nr_player );

char allow_display_transportcontents( struct OBJ *a, 
	int nr_player );


#endif /*RULES_H*/
