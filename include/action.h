
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

#define NONDETECTEDACTION	1
#define CREATELARVAEACTION	2
#define WAITSOMETIMEACTION	3
#define SOMENEEDEDACTION	4

#define SPECIALMAPID			(void *)1
#define SPECIALMAPUNITSID		(void *)2
#define SPECIALCREEPID			(void *)3
#define SPECIALHEALID			(void *)4
#define SPECIALINFESTID			(void *)5
#define SPECIALMOVEREFRESHID		(void *)6
#define SPECIALREPAIRID			(void *)7


#define MINIMAPREFRESHCYCLE		10
#define MINIMAPUNITSREFRESHCYCLE	10
#define CHANGECREEPCYCLE		30
#define BATTERYMEDICCYCLE		15
#define QUEENINFESTCYCLE		200
#define MOVEREFRESH1CYCLE		15
#define REPAIRREFRESHCYCLE		10

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
int  GetMinDistanceBetweenUnits(OBJ *a1,OBJ *a2);
int  getminmovedistobj(struct OBJ *a1,struct OBJstruct *b1,struct OBJ *a2);
int  getminmovedistance(struct OBJ *a,struct OBJstruct *b);
void QueueAction(void *IDQueueAction,int DestroyQueue);
void MakeQueueAction(int actiontype,void *a,void *destobj,
		    int param0,int param1,int param2,int afterexpiriedticks);
void AdditionalMoveProceed(struct OBJ *a,int param0,int param1,int param2);
void DettachAddon(OBJ *a);

unsigned char GetMotherAddon(unsigned char SC_Unit);
void GetAddonType(unsigned char SC_Unit,unsigned char *addontype1,unsigned char *addontype2);

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


#endif /*_ACTION_W */


