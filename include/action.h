
#if !defined(_ACTION_W)
#define _ACTION_W

#include "defs.h"
#include "vars.h"
#include "lists.h"
#include "man.h"
#include "objs.h"
#include "putobj.h"
#include "audio.h"
#include "load.h"
#include "mage.h"
#include "maps.h"
#include "unitaction.h"

#define MOVEACTION		1
#define BACKEFFACTION		2
#define NONDETECTEDACTION	3
#define CREATELARVAEACTION	4
#define MAGETIMEDACTION		5
#define SOMENEEDEDACTION	6

#define SPECIALMAPID			(void *)1
#define SPECIALMAPUNITSID		(void *)2
#define SPECIALLIFEID			(void *)3
#define SPECIALCREEPID			(void *)4
#define SPECIALHEALID			(void *)5
#define SPECIALINFESTID			(void *)6
#define SPECIALGATHERRETID		(void *)7
#define SPECIALMOVEREFRESHID		(void *)8
#define SPECIALREPAIRID			(void *)9
#define SPECIALDOODADSCROLLID		(void *)10


#define MINIMAPREFRESHCYCLE		10
#define MINIMAPUNITSREFRESHCYCLE	10
#define CHANGEMANACYCLE			200
#define CHANGECREEPCYCLE		30
#define BATTERYMEDICCYCLE		15
#define QUEENINFESTCYCLE		200
#define DRONERETURNCYCLE		100
#define MOVEREFRESH1CYCLE		15
#define MOVEREFRESH2CYCLE		15
#define REPAIRREFRESHCYCLE		10
#define DOODADREFRESHCYCLE		1

struct QUEUEACTION
{
    int actiontype;
    void *obj;
    void *destobj;
    int param0;
    int param1;
    int param2;
};
//=================================
#define ADDON_ERROR_ATTACH	0
#define ADDON_SUCCESS_ATTACH	1

void PickupObj(OBJ *a,OBJ *pickupobj);
int SplashShotDistance(int xshot,int yshot,OBJ *destobj);
int GetMinDistanceBetweenUnits(OBJ *a1,OBJ *a2);
int getminmovedistobj(struct OBJ *a1,struct OBJstruct *b1,struct OBJ *a2);
int getminmovedistance(struct OBJ *a,struct OBJstruct *b);
void QueueAction(void *IDQueueAction,int DestroyQueue);
void MakeQueueAction(int actiontype,void *a,void *destobj,
		    int param0,int param1,int param2,int afterexpiriedticks);
void AdditionalMoveProceed(struct OBJ *a,int param0,int param1,int param2);
void DettachAddon(OBJ *a);

unsigned char GetMotherAddon(unsigned char SC_Unit);
void GetAddonType(unsigned char SC_Unit,unsigned char *addontype1,unsigned char *addontype2);

OBJ *SearchObjs(int x,int y,int range,int *array,int arraydim);
int  FindSC_UnitType(OBJ *a,int player,int typeofbuild);
int  MageDepend(OBJ *a,int player,int typeofbuild);

void getcoordofnewunit(OBJ *a,int nrunit,int *x,int *y);
void getcoordofnewunit(int nrunit,int *x,int *y,struct XY *area);

#define AROUNDTABLEX	(14*4)
#define AROUNDTABLEY	(12*4)
#define MAXAROUNDARRAY  (AROUNDTABLEX*AROUNDTABLEY)

struct AROUNDBUILD
{
	int deltax;
	int deltay;
	int method;
};
void SetAroundTable(void);
int CreateNuke(OBJ *a,int x,int y);

void addremoveuniteffectfrommap(OBJ *a,int plusminus,mapinfo *testmap);
int  RepairUnit(int nplayer,OBJ *scv,OBJ *a);
void UnitBeginATACK(OBJ *a,OBJ *destobj);
void UnitEndATACK(OBJ *a);
int  AttachAddonToBuild(OBJ *a1);
int  AttachNearestAddon(OBJ *a1);
void AttachAddonDeltaCoords(unsigned char base_SC_Unit,unsigned char addon_SC_Unit,int *deltax,int *deltay);
int SearchForUnitMask(int playernr,int mask,int *x,int *y);


#endif /*_ACTION_W */


