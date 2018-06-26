#if !defined(_TEMPOBJ_W)
#define _TEMPOBJ_W

#include "defs.h"
#include "mylist.h"
#include "starmap.h"

#define FIRSTBUILD	0
#define FIRSTUNIT	1

#define MINIMAP_COLORFORRESOURCES	53
#define MINIMAP_COLORFORSTARTLOC	117

unsigned char GetFirstUnitBuild(int race,int build_or_unit);

OBJ *CreateMineralsOnMap(int x,int y,SCUNIT SC_Unit,int player,int count,int maxcount,mapinfo *loadedmap);
OBJ *CreateGeyserOnMap(int x,int y,SCUNIT SC_Unit,int player,int count,int maxcount,mapinfo *loadedmap);
OBJ *CreateUnitsFromMAP(int xpos,int ypos,int unit_type,int playernr,struct mapinfo *loadedmap);
OBJ *CreateUnitsFromMAP(struct unit_on_map *unit,struct mapinfo *loadedmap);
void RemoveUnitsFromLists(mylist *units);


int IfPlayerHaveStartLocation(struct mapinfo *info,int playernr);
int GetTotalStartLocations(struct mapinfo *testmap);
void createallobj(struct mapinfo *loadedmap);
int GetRaceFromMap(struct mapinfo *loadedmap,int location);
void AddToMinimapNeutralObjs(mylist *units,struct mapinfo *loadedmap,int playortest);



//void creeptest(int value);
extern signed char startlocations[MAXPLAYERS];
extern OBJ *testobj;

#define MAXMINERALSINSTACK	1500
#define MAXGASINSTACK		5000

#endif /*_TEMPOBJ_W*/

