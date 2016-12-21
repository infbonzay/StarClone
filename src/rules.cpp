
#include "rules.h"
#include "man.h"
#include "maps.h"

char allow_display_unitstatus( struct OBJ *a, int nr_player )
{
    return(1);
//  char st = player_aliance(nr_player,a->playernr);
//  return st == MYOBJ || st == ALLIANCEOBJ;
}

char allow_display_mineralstatus( struct OBJ *a, int nr_player )
{
  return 1;
}

char allow_display_powerstatus( struct OBJ *a, int nr_player )
{
  return 1;
}

char allow_display_transportcontents( struct OBJ *a, int nr_player )
{
  char st = player_aliance(nr_player,a->playernr);
  return st == MYOBJ || st == ALLIANCEOBJ;
}

