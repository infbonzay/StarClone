#if !defined(_WEAPONS_W)
#define _WEAPONS_W

#include <grplib/grp.h>
#include "loadlo.h"
#include "flingy.h"
#include "tbl.h"

#define WEAPONRANGEFROM		(3*16)	//define if range of weapon is large than 3 it is range atack else melee atack

#define DRAWWEAPON_APPEAR	0
#define DRAWWEAPON_MOVE		1
#define DRAWWEAPON_HIT		2
#define DRAWWEAPON_TRAIL	3
#define DRAWWEAPON_MAX		4

#define WB_FLY_DONT_FOLLOW_TARGET	0
#define WB_FLY_AND_FOLLOW_TARGET	1
#define WB_APPEAR_ON_TARGET_UNIT	2
#define WB_PERSIST_ON_TARGET_SITE	3
#define WB_APPEAR_ON_TARGET_SITE	4
#define WB_APPEAR_ON_ATACKER		5
#define WB_ATACK_AND_SELF_DESTRUCT	6
#define WB_BOUNCE			7
#define WB_ATACK_3X3_AREA		8
#define WB_GOTO_MAX_RANGE		9

struct DRAW_WEAPON
{
    GRPFILE	*grp;
    char	maxsides;
    char	grpfunc;
    char	grptable;
    char	flags;
    short	fromsfx;
    char	sfxnrs;
};
struct WEAPONS_UNIT
{
    unsigned	char 	rangeupgnr;	//range upgr nr for weapon
    unsigned	char	rangeupgaddfactor;
};

int  getgrpfunc(char *grpfunc,char *grptable,char *strvalue);
int  GetAtackWeaponLevel(OBJ *atacker,OBJ *destobj);
int  CreateWeapon(OBJ *atacker,OBJ *destobj);
int  IfCanCreateWeapon(OBJ *atacker,OBJ *destobj,int *errmes,unsigned char *weapon_id,int flags);
int  IsSplashDamage(unsigned char weapon_id);
void WeaponPlaySFX(OBJ *a,int sfxdata_id,int distance,int x,int y);
int  CalcMaxRangeCoordsXY(int deltax,int deltay,int *destx,int *desty,unsigned char weapon_id,unsigned char flingy_id);
void GetLaunchOffsets(int direction,int maxdirection,int weapon_id,int *xoffs,int *yoffs);
void CreateWeaponID(OBJ *a,OBJ *destobj,int xdest,int ydest,unsigned char weapon_id,int createflingy);
void GetOBJXYSideOffsets(OBJ *a,MAIN_IMG *img,int overlaytype,signed char *x,signed char *y);
void WeaponDoDamage(OBJ *atacker,OBJ *destobj,int x256,int y256,unsigned char SC_Unit,
		    int playernr,unsigned char weapon_id,int castmagenr,int targetnr,mylist *listdamaged);
void AddOverlayAtrImages(OBJ *destobj,int castmagenr,int independentofmain);
int  GetDeltaWeaponElevationLevel(unsigned char weapon_id);
int  WeaponCanApplyOnUnit(OBJ *a,int playernr,unsigned char weapon_id);

void unloadweapons(void);
int  loadweapons(void);



#define CREATEDWEAPONSTATUS_DESTTOCLOSE				-3
#define CREATEDWEAPONSTATUS_UNDERDISRUPTION			-2
#define CREATEDWEAPONSTATUS_DESTOUTOFRANGE			-1
#define CREATEDWEAPONSTATUS_CANTATACKTHISUNIT			0
#define CREATEDWEAPONSTATUS_ATACKSUCCESS			1
#define CREATEDWEAPONSTATUS_ATACKSUCCESSWITHROTATION		2
#define CREATEDWEAPONSTATUS_NOTAMMO				3
#define CREATEDWEAPONSTATUS_LAUNCHINTERCEPTORS			4


#define CREATEWEAPON_IGNOREVISION				0x01

extern WEAPONS_UNIT weapons[];

extern short TARGETERROR[3];


#endif
