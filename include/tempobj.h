#ifndef _SC_TEMPOBJ_H

#define _SC_TEMPOBJ_H

#include "defs.h"
#include "mylist.h"
#include "man.h"
#include "starmap.h"

#define FIRSTBUILD		0
#define FIRSTUNIT		1

#define MINIMAP_COLORFORRESOURCES		53
#define MINIMAP_COLORFORSTARTLOC		117

unsigned char GetFirstUnitBuild(int race,int build_or_unit);

OBJ *CreateMineralsOnMap(int x,int y,SCUNIT SC_Unit,int player,int count,int maxcount,mapinfo *loadedmap);
OBJ *CreateGeyserOnMap(int x,int y,SCUNIT SC_Unit,int player,int count,int maxcount,mapinfo *loadedmap);
OBJ *CreateUnitsFromMAP(int xpos,int ypos,int unit_type,int playernr,mapinfo *loadedmap);
OBJ *CreateUnitsFromMAP(struct unit_on_map *unit,mapinfo *loadedmap);
void RemoveUnitsFromLists(mylist *units);


int IfPlayerHaveStartLocation(mapinfo *info,int playernr);
int GetTotalStartLocations(mapinfo *testmap);
void createallobj(mapinfo *loadedmap);
int GetRaceFromMap(mapinfo *loadedmap,int location);
void AddToMinimapNeutralObjs(mylist *units,mapinfo *loadedmap,int playortest);



//void creeptest(int value);
extern signed char startlocations[MAXPLAYERS];
extern OBJ *testobj;

#define MAXMINERALSINSTACK		1500
#define MAXGASINSTACK			5000

#endif /*_TEMPOBJ_W*/

