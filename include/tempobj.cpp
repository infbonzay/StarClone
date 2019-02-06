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

OBJ *CreateMineralsOnMap(int x,int y,SCUNIT SC_Unit,int player,int count,int maxcount,StarMapInfo *loadedmap);
OBJ *CreateGeyserOnMap(int x,int y,SCUNIT SC_Unit,int player,int count,int maxcount,StarMapInfo *loadedmap);
OBJ *CreateUnitsFromMAP(int xpos,int ypos,int unit_type,int playernr,StarMapInfo *loadedmap);
OBJ *CreateUnitsFromMAP(struct unit_on_map *unit,StarMapInfo *loadedmap);
void RemoveUnitsFromLists(mylist *units);


int IfPlayerHaveStartLocation(StarMapInfo *info,int playernr);
int GetTotalStartLocations(StarMapInfo *testmap);
int GetRaceFromMap(StarMapInfo *loadedmap,int location);
void AddToMinimapNeutralObjs(mylist *units,StarMapInfo *loadedmap,int playortest);
void ApplyObjectsToMap(StarMapInfo *loadedmap);


extern signed char startlocations[MAXPLAYERS];

#define MAXMINERALSINSTACK		1500
#define MAXGASINSTACK			5000

#endif /*_TEMPOBJ_W*/

