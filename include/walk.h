#if !defined(_WALK_W)
#define _WALK_W

#include "starmap.h"

#define SETWALK		0
#define CLEARWALK	1

#define TOBECONSTRUCT_NOTOPENMAP	-1
#define TOBECONSTRUCT_NOTSEEMAP		-2
#define TOBECONSTRUCT_NOTGEYSER		-3
#define TOBECONSTRUCT_NEARRESOURCE	-4
#define TOBECONSTRUCT_WRONGPLACE	-5
#define TOBECONSTRUCT_NOTCREEP		-6
#define TOBECONSTRUCT_NOTPYLONAREA	-7
#define TOBELAND_CANTLANDHERE		-8


//=====================================
int CanBuildOnPlace(int xb,int yb,int xsize,int ysize,
		    unsigned short *returnedarray,
		    char *ignorecells,
		    int buildtype,int playernr,int *constrerror);
int CanBuildOnPlaceWithAddon(int xb,int yb,int parentx,int parenty,
		    int xsize,int ysize,unsigned short *returnedarray,int buildtype,int playernr,int *constrerror);
int CanBuildOnCreep(int xb,int yb,int xsize,int ysize,
		    unsigned short *returnedarray);
int CanBuildOnPylonArea(int xb,int yb,int xsize,int ysize,
		    unsigned short *returnedarray,int playernr);

int CanBuildOnTile(int xb,int yb,int xsize,int ysize,
		    unsigned short *returnedarray,
		    char *ignorecells,
		    int ignorecreep);
int CanBuildOnEarth(int xb,int yb,int xsize,int ysize,
		    unsigned short *returnedarray,char *ignorecells);
int CanBuildOnBuildPlace(int xb,int yb,int xsize,int ysize,
		    unsigned short *returnedarray,char *ignorecells);
int CanBuildForResource(int xb,int yb,int xsize,int ysize,
		    unsigned short *returnedarray,char *ignorecells);

void SetMarkOn32(int xb,int yb,int xsize,int ysize,unsigned char *usedmap,unsigned char value);
void AddMarkOn32(int xb,int yb,int xsize,int ysize,unsigned char *usedmap);
void DelMarkOn32(int xb,int yb,int xsize,int ysize,unsigned char *usedmap);
void SetObjWalk8(struct mapinfo *loadedmap,OBJ *a,int flag);
void SetMapWalk8(int x,int y,int sx,int sy,int flag);
int  GetMapWalk8(int x,int y,int sx,int sy);

#endif

