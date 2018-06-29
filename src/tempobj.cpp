
#include "diap.h"
#include "maps.h"

#include "defs.h"
#include "auxil.h"
#include "objinfo.h"
#include "objs.h"
#include "vars.h"
#include "gener.h"
#include "audio.h"
#include "load.h"
#include "man.h"
#include "creep.h"
#include "lists.h"
#include "mylist.h"
#include "mouse.h"
#include "objs.h"
#include "mytime.h"
#include "starmap.h"
#include "doodad.h"
#include "overlay.h"
#include "pylon.h"
#include "walk.h"
#include "action.h"
#include "rand.h"
#include "portrait.h"
#include "market.h"
#include "weapons.h"
#include "player.h"
#include "gr.h"
#include "images.h"
#include "tempobj.h"

#include <grplib/grp.h>
#include <grplib/usegrp.h>

int curentlocation;
int myplayer;
char haveterranparied;
char havezergparied;
//=====================================
//#define TEST
//#define ONEUNITEACHRACE
//#define NONBUILD
//=====================================
int IfPlayerHaveStartLocation(struct mapinfo *info,int playernr)
{
    return(info->pl_location[playernr]);
}
//=====================================
int GetPlayerByLocation(struct mapinfo *tempmap,int location)
{
    for (int i=0;i<PLAYEDPLAYERS;i++)
	if (tempmap->pl_location[i]==location)
	    return(i);
    return(-1);
}
//=====================================
unsigned char GetFirstUnitBuild(int race,int build_or_unit)
{
    if (race>2||race<0)
	return SC_NOUNITNR;
    else
	return (First_Unit_Build[race][build_or_unit]);
}
//=====================================
int GetRaceFromMap(struct mapinfo *loadedmap,int location)
{
    return(loadedmap->pl_race[location]);
}
//=====================================
int GetPlayerColorFromMap(struct mapinfo *loadedmap,int location)
{
    return(location);
}
//=====================================
int GetTotalStartLocations(struct mapinfo *testmap)
{
    return(testmap->pl_startlocs);
}
//=====================================
OBJ *CreateMineralsOnMap(int x,int y,SCUNIT SC_Unit,int playernr,int count,int maxcount,mapinfo *loadedmap)
{
    int xp,yp,sx,sy;
    struct OBJ *a;
    a = createobjfulllife(x,y,SC_Unit,playernr);

    xp = (GetOBJx(a) - GetUnitWidthAndHeight(SC_Unit,UNITDIM_WIDTH)/2)/SIZESPRLANSHX-3;
    yp = (GetOBJy(a) - GetUnitWidthAndHeight(SC_Unit,UNITDIM_HEIGHT)/2)/SIZESPRLANSHY-3;
    sx=8;
    sy=7;
    AddMarkOn32(xp,yp,sx,sy,loadedmap->constrmap);
    if (count>maxcount)
    {
	a->data.resource.resource_count = count;
	a->data.resource.max_resource_count = count;
    }
    else
    {
	a->data.resource.resource_count = count;
	a->data.resource.max_resource_count = maxcount;
    }
    ChangeMineralSprite(a);
    if (gameconf.videoconf.visiblemap)
	SaveOBJImagesToFogTable(a);
    return(a);
}
//=====================================
OBJ *CreateGeyserOnMap(int x,int y,SCUNIT SC_Unit,int playernr,int count,int maxcount,mapinfo *loadedmap)
{
    int xp,yp,sx,sy;
    struct OBJ *a;
    a = createobjfulllife(x,y,SC_Unit,playernr);
    xp = ( GetOBJx(a) - GetUnitWidthAndHeight(SC_Unit,UNITDIM_WIDTH)/2 ) / SIZESPRLANSHX - 3;
    yp = ( GetOBJy(a) - GetUnitWidthAndHeight(SC_Unit,UNITDIM_HEIGHT)/2 ) / SIZESPRLANSHY - 3;
    sx=10;
    sy=8;
    AddMarkOn32(xp,yp,sx,sy,loadedmap->constrmap);
    if (count>maxcount)
    {
	a->data.resource.resource_count = count;
	a->data.resource.max_resource_count = count;
    }
    else
    {
	a->data.resource.resource_count = count;
	a->data.resource.max_resource_count = maxcount;
    }
    if (gameconf.videoconf.visiblemap)
	SaveOBJImagesToFogTable(a);
    return(a);
}
//=====================================
OBJ *CreateUnitsFromMAP(int xpos,int ypos,int unit_type,int playernr,struct mapinfo *loadedmap)
{
    unit_on_map tempunit;
    memset(&tempunit,0,sizeof(unit_on_map));
    tempunit.xpos=xpos;
    tempunit.ypos=ypos;
    tempunit.unit_type=unit_type;
    tempunit.player=playernr;
    return(CreateUnitsFromMAP(&tempunit,loadedmap));
}
//=====================================
OBJ *CreateUnitsFromMAP(struct unit_on_map *unit,struct mapinfo *loadedmap)
{
    int xp,yp,sx,sy;
    struct OBJ *a,*aa,*a2,*a3=NULL;
    int i,playernr,xobj,yobj,race,owner;
    int hp,sp,ep;
    SCUNIT SC_Unit;
    if (unit->paried)
    {
	if (unit->paried==TERRANPARIED)
	    haveterranparied=1;
	else
	    havezergparied=1;
    }
    switch(unit->unit_type)
    {
	case SC_MINERAL1OBJ://minerals
	case SC_MINERAL2OBJ:
	case SC_MINERAL3OBJ:
	    a=CreateMineralsOnMap(unit->xpos,unit->ypos,unit->unit_type,GREYNEUTRALCOLORPLAYER,unit->resource_count,MAXMINERALSINSTACK,loadedmap);
	    break;
	case SC_GEYSEROBJ:
	    a=CreateGeyserOnMap(unit->xpos,unit->ypos,unit->unit_type,GREYNEUTRALCOLORPLAYER,unit->resource_count,MAXGASINSTACK,loadedmap);
	    break;
	case SC_STARTLOC://start location
	    if (GAMETYPE == MAP_GAMETYPE_USEMAPSETTINGS)
	    {
	        if (loadedmap->pl_race[unit->player] == RACE_USER_SELECT)
	        {
		    playernr = GetPlayerByLocation(loadedmap,curentlocation);
	    	    if (playernr == NUMBGAMER)
	    	    {
	    		SetVisualMapPositionCenter(unit->xpos,unit->ypos);
	    	    }
	    	}
	    	else
	    	{
		    if (unit->player == NUMBGAMER)
	    		SetVisualMapPositionCenter(unit->xpos,unit->ypos);
	    	    return(NULL);
	        }
	    }
	    playernr=GetPlayerByLocation(loadedmap,curentlocation);
	    if (playernr!=-1)
	    {
		owner = gameconf.pl_owner[playernr];
		// do not create units for observer players
		if (( owner == OWNER_HUMAN || owner == OWNER_COMPUTER))
		{
		    race = gameconf.pl_race[playernr];
		    if (race!=RACE_OBSERVER)
		    {
			SC_Unit = GetFirstUnitBuild(race,FIRSTBUILD);
			if (SC_Unit!=SC_NOUNITNR)
			{
			    a = createobjfulllife(unit->xpos,unit->ypos,SC_Unit,playernr);
			    if (playernr==NUMBGAMER)
			    {
				SetVisualMapPositionCenter(unit->xpos,unit->ypos);
			    }
			    if (SC_Unit == SC_HATCHERYOBJ)
			    {
				    //create additional overlord
				    aa=createobjfulllife(unit->xpos,unit->ypos,SC_OVERLORDOBJ,playernr);
				    //set hatchery to be made by drone
//				    a->SC_FromUnit=SC_DRONEOBJ;
			    }
			    SC_Unit = GetFirstUnitBuild(race,FIRSTUNIT);
			    for (i=0;i<MAXBEGINPROBE;i++)
			    {
				    getcoordofnewunit(a,SC_Unit,&xobj,&yobj);
				    a2 = createobjfulllife(xobj,yobj,SC_Unit,playernr);
/*				    if (!i)
					a3 = GetNearCenter(a2);
				    a2->baseobj = a3;
*/			    }
			}
		    }
		}
	    }
	    curentlocation++;
	    break;
	default:
	    if (GAMETYPE == MAP_GAMETYPE_USEMAPSETTINGS)
	    {
		a = createunitwithproperties(unit->xpos,unit->ypos,unit->unit_type,unit->player,
					     unit->special_props,unit->special_prop2,unit->state_flags,
					     unit->hitpoints,unit->shieldpoints,unit->energypoints,unit->resource_count,unit->units_in_hangar);
//		a->serial1 = unit->unit_serialnumberfirst;
//		a->serial2 = unit->unit_serialnumbersecond;
		a->paried = unit->paried;
		a->data.beforestartplay.serial1 = unit->unit_serialnumberfirst;
		a->data.beforestartplay.serial2 = unit->unit_serialnumbersecond;
//		a->data.beforestartplay.paried = unit->paried;
	    }
	    else
	    {
#ifndef TEST
		if (!IsGroupNeutralFlag(unit->unit_type))		//in melee do not create neutral units/builds
		    return(NULL);
#endif
		a = createobjfulllife(unit->xpos,unit->ypos,unit->unit_type,unit->player);
//		a = createobjman(unit->xpos,unit->ypos,unit->unit_type,unit->player,0,1,1,1);
	    }
	    break;
    }
    return(a);
}
//=====================================
void AddToMinimapNeutralObjs(mylist *units,struct mapinfo *loadedmap,int playortest)
{
    int i,j,sx,sy;
    struct unit_on_map *unit;
    units->EnumListInit();
    while( (unit = (unit_on_map *)units->GetNextListElem()) )
    {
	sx=GetUnitBoxWidth(unit->unit_type);
	sy=GetUnitBoxHeight(unit->unit_type);
	if (unit->unit_type == SC_STARTLOC && playortest == STARMAP_LOADFORINFO)
	{
//	    if (GAMETYPE != MAP_GAMETYPE_USEMAPSETTINGS)
		ObjOnMiniMap(unit->xpos/32-sx/64,unit->ypos/32-sy/64,sx,sy,MINIMAP_COLORFORSTARTLOC,loadedmap->minimap);
	}
	else
	    if (unit->player >= PLAYEDPLAYERS)
	    {
		if (IsBuild(unit->unit_type))
		{
		    if (IfCanClickOBJ(unit->unit_type))
			ObjOnMiniMap(unit->xpos/32-sx/64,unit->ypos/32-sy/64,sx,sy,MINIMAP_COLORFORRESOURCES,loadedmap->minimap);
		}
	    }
    }
}
//=====================================
//=====================================
void CreateUnitsFromLists(mylist *units,struct mapinfo *loadedmap)
{
    int i,j;
    struct unit_on_map *unit;
    units->EnumListInit();
    while( (unit = (unit_on_map *)units->GetNextListElem()) )
    {
	    if (unit->unit_type != SC_STARTLOC)
	    {
		CreateUnitsFromMAP(unit,loadedmap);
//		wfree(unit);
	    }
    }
    //parse all base builds, because we need to know all resources before place creep
    units->EnumListInit();
    while( (unit=(unit_on_map *)units->GetNextListElem()) )
    {
	    if (unit->unit_type == SC_STARTLOC)
	    {
		CreateUnitsFromMAP(unit,loadedmap);
//		wfree(unit);
	    }
    }
    units->EnumListInit();
    while( (unit=(unit_on_map *)units->GetNextListElem(&i)) )
    {
	wfree(unit);
	units->DelList(i);
    }
}
//=====================================
void RemoveUnitsFromLists(mylist *units)
{
    units->DeallocList();
}
//=====================================
void testprotoss(int deltax,int deltay,int player,struct mapinfo *loadedmap)
{
    OBJ *xx;
//    PLAYER[player].minerals=5000<<8;
//    PLAYER[player].gas=5000<<8;
    gameconf.pl_race[player]=PROTOSSRACE;
    loadedmap->pl_location[player]=player;
    gameconf.pl_owner[player]=OWNER_HUMAN;

    xx=CreateUnitsFromMAP((25+deltax)*SIZESPRLANSHX+16,(25+deltay)*SIZESPRLANSHY+16,SC_PROBEOBJ,player,loadedmap);
#ifdef ONEUNITEACHRACE
    return;
#endif
    xx=CreateUnitsFromMAP((26+deltax)*SIZESPRLANSHX+16,(25+deltay)*SIZESPRLANSHY+16,SC_ZEALOTOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((28+deltax)*SIZESPRLANSHX+16,(25+deltay)*SIZESPRLANSHY+16,SC_DRAGOONOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((30+deltax)*SIZESPRLANSHX+16,(25+deltay)*SIZESPRLANSHY+16,SC_HIGHTEMPLAROBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((30+deltax)*SIZESPRLANSHX,(23+deltay)*SIZESPRLANSHY+16,SC_HIGHTEMPLAROBJ,player,loadedmap);
    if (xx)
	SetUnitMana(xx,200);
    xx=CreateUnitsFromMAP((31+deltax)*SIZESPRLANSHX+16,(25+deltay)*SIZESPRLANSHY+16,SC_DARKTEMPLAROBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((31+deltax)*SIZESPRLANSHX+16,(23+deltay)*SIZESPRLANSHY+16,SC_DARKTEMPLAROBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((33+deltax)*SIZESPRLANSHX+16,(25+deltay)*SIZESPRLANSHY+16,SC_ARCHONOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((35+deltax)*SIZESPRLANSHX+16,(25+deltay)*SIZESPRLANSHY+16,SC_DARKARCHONOBJ,player,loadedmap);
    if (xx)
        SetUnitMana(xx,200);
    xx=CreateUnitsFromMAP((25+deltax)*SIZESPRLANSHX+16,(28+deltay)*SIZESPRLANSHY+16,SC_SHUTTLEOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((27+deltax)*SIZESPRLANSHX+16,(28+deltay)*SIZESPRLANSHY+16,SC_REAVEROBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((27+deltax)*SIZESPRLANSHX+16,(26+deltay)*SIZESPRLANSHY+16,SC_SCARABOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((29+deltax)*SIZESPRLANSHX+16,(28+deltay)*SIZESPRLANSHY+16,SC_OBSERVEROBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((25+deltax)*SIZESPRLANSHX+16,(30+deltay)*SIZESPRLANSHY+16,SC_SCOUTOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((27+deltax)*SIZESPRLANSHX+16,(30+deltay)*SIZESPRLANSHY+16,SC_CARRIEROBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((29+deltax)*SIZESPRLANSHX+16,(30+deltay)*SIZESPRLANSHY+16,SC_ARBITEROBJ,player,loadedmap);
    if (xx)
        SetUnitMana(xx,200);
    xx=CreateUnitsFromMAP((31+deltax)*SIZESPRLANSHX+16,(30+deltay)*SIZESPRLANSHY+16,SC_CORSAIROBJ,player,loadedmap);
    if (xx)
	SetUnitMana(xx,200);
#ifdef NONBUILD
    return;
#endif
    xx=CreateUnitsFromMAP((31+deltax)*SIZESPRLANSHX,(30+deltay)*SIZESPRLANSHY,SC_NEXUSOBJ,player,loadedmap);
//    xx=CreateUnitsFromMAP((31+deltax)*SIZESPRLANSHX,(30+deltay)*SIZESPRLANSHY,SC_STARTLOC,player,loadedmap);
    xx=CreateUnitsFromMAP((31+deltax)*SIZESPRLANSHX,(33+deltay)*SIZESPRLANSHY,SC_PYLONOBJ,player,loadedmap);
    xx=CreateGeyserOnMap((31+deltax)*SIZESPRLANSHX,(36+deltay)*SIZESPRLANSHY,SC_GEYSEROBJ,GREYNEUTRALCOLORPLAYER,MAXGEYSERGASNR,MAXGEYSERGASNR,loadedmap);
    xx=CreateUnitsFromMAP((31+deltax)*SIZESPRLANSHX,(36+deltay)*SIZESPRLANSHY,SC_ASSIMILATOROBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((31+deltax)*SIZESPRLANSHX,(39+deltay)*SIZESPRLANSHY,SC_GATEWAYOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((31+deltax)*SIZESPRLANSHX,(42+deltay)*SIZESPRLANSHY,SC_FORGEOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((35+deltax)*SIZESPRLANSHX,(30+deltay)*SIZESPRLANSHY,SC_PHOTONCANONOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((35+deltax)*SIZESPRLANSHX,(33+deltay)*SIZESPRLANSHY,SC_CYBERNETICCOREOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((35+deltax)*SIZESPRLANSHX,(36+deltay)*SIZESPRLANSHY,SC_BATTERYOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((35+deltax)*SIZESPRLANSHX,(39+deltay)*SIZESPRLANSHY,SC_ROBOTICSFACILITYOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((35+deltax)*SIZESPRLANSHX,(42+deltay)*SIZESPRLANSHY,SC_STARGATEOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((41+deltax)*SIZESPRLANSHX,(30+deltay)*SIZESPRLANSHY,SC_CITADELOFADUNOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((41+deltax)*SIZESPRLANSHX,(33+deltay)*SIZESPRLANSHY,SC_ROBOTICSSUPPORTBAYOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((41+deltax)*SIZESPRLANSHX,(36+deltay)*SIZESPRLANSHY,SC_FLEETBEACONOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((41+deltax)*SIZESPRLANSHX,(39+deltay)*SIZESPRLANSHY,SC_TEMPLARARCHIVESOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((41+deltax)*SIZESPRLANSHX,(42+deltay)*SIZESPRLANSHY,SC_OBSERVATORYOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((41+deltax)*SIZESPRLANSHX,(46+deltay)*SIZESPRLANSHY,SC_ARBITERTRIBUNALOBJ,player,loadedmap);

}
//=====================================
void testzerg(int deltax,int deltay,int player,struct mapinfo *loadedmap)
{
    OBJ *xx;
    gameconf.pl_race[player]=ZERGRACE;
    loadedmap->pl_location[player]=player;
    gameconf.pl_owner[player]=OWNER_HUMAN;

    xx=CreateUnitsFromMAP((27+deltax)*SIZESPRLANSHX+16,(29+deltay)*SIZESPRLANSHY+16,SC_DRONEOBJ,player,loadedmap);
#ifdef ONEUNITEACHRACE
    return;
#endif
    xx=CreateUnitsFromMAP((27+deltax)*SIZESPRLANSHX+16,(25+deltay)*SIZESPRLANSHY+16,SC_ZERGLINGOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((29+deltax)*SIZESPRLANSHX+16,(25+deltay)*SIZESPRLANSHY+16,SC_OVERLORDOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((31+deltax)*SIZESPRLANSHX+16,(25+deltay)*SIZESPRLANSHY+16,SC_HYDRALISKOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((25+deltax)*SIZESPRLANSHX+16,(27+deltay)*SIZESPRLANSHY+16,SC_LURKEROBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((27+deltax)*SIZESPRLANSHX+16,(27+deltay)*SIZESPRLANSHY+16,SC_SCOURGEOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((29+deltax)*SIZESPRLANSHX+16,(27+deltay)*SIZESPRLANSHY+16,SC_MUTALISKOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((31+deltax)*SIZESPRLANSHX+16,(27+deltay)*SIZESPRLANSHY+16,SC_GUARDIANOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((25+deltax)*SIZESPRLANSHX+16,(29+deltay)*SIZESPRLANSHY+16,SC_DEVOUREROBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((27+deltax)*SIZESPRLANSHX+16,(29+deltay)*SIZESPRLANSHY+16,SC_QUEENOBJ,player,loadedmap);
    if (xx)
	    SetUnitMana(xx,200);
    xx=CreateUnitsFromMAP((29+deltax)*SIZESPRLANSHX+16,(29+deltay)*SIZESPRLANSHY+16,SC_ULTRALISKOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((31+deltax)*SIZESPRLANSHX+16,(29+deltay)*SIZESPRLANSHY+16,SC_DEFILEROBJ,player,loadedmap);
    if (xx)
	    SetUnitMana(xx,200);
    xx=CreateUnitsFromMAP((33+deltax)*SIZESPRLANSHX+16,(31+deltay)*SIZESPRLANSHY+16,SC_BROODLINGOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((35+deltax)*SIZESPRLANSHX,(31+deltay)*SIZESPRLANSHY,SC_BUGGUYOBJ,player,loadedmap);
#ifdef NONBUILD
    return;
#endif
    xx=CreateUnitsFromMAP((31+deltax)*SIZESPRLANSHX,(30+deltay)*SIZESPRLANSHY+16,SC_HATCHERYOBJ,player,loadedmap);
//    xx=CreateUnitsFromMAP((31+deltax)*SIZESPRLANSHX,(30+deltay)*SIZESPRLANSHY+16,SC_STARTLOC,player,loadedmap);
    xx=CreateUnitsFromMAP((31+deltax)*SIZESPRLANSHX,(33+deltay)*SIZESPRLANSHY,SC_CREEPCOLONYOBJ,player,loadedmap);
    xx=CreateGeyserOnMap((31+deltax)*SIZESPRLANSHX,(36+deltay)*SIZESPRLANSHY,SC_GEYSEROBJ,GREYNEUTRALCOLORPLAYER,MAXGEYSERGASNR,MAXGEYSERGASNR,loadedmap);
    xx=CreateUnitsFromMAP((31+deltax)*SIZESPRLANSHX,(36+deltay)*SIZESPRLANSHY,SC_EXTRACTOROBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((31+deltax)*SIZESPRLANSHX+16,(39+deltay)*SIZESPRLANSHY,SC_SPAWNINGPOOLOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((31+deltax)*SIZESPRLANSHX+16,(42+deltay)*SIZESPRLANSHY,SC_EVOLUTIONCHAMBEROBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((35+deltax)*SIZESPRLANSHX+16,(30+deltay)*SIZESPRLANSHY,SC_HYDRALISKDENOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((35+deltax)*SIZESPRLANSHX+16,(33+deltay)*SIZESPRLANSHY,SC_QUEENNESTOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((35+deltax)*SIZESPRLANSHX,(36+deltay)*SIZESPRLANSHY,SC_SPIREOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((35+deltax)*SIZESPRLANSHX,(39+deltay)*SIZESPRLANSHY,SC_NYDUSCANALOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((35+deltax)*SIZESPRLANSHX+16,(42+deltay)*SIZESPRLANSHY,SC_ULTRALISKCAVERNOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((41+deltax)*SIZESPRLANSHX,(30+deltay)*SIZESPRLANSHY,SC_DEFILERMOUNDOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((41+deltax)*SIZESPRLANSHX,(33+deltay)*SIZESPRLANSHY+16,SC_HIVEOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((41+deltax)*SIZESPRLANSHX,(36+deltay)*SIZESPRLANSHY+16,SC_LAIROBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((41+deltax)*SIZESPRLANSHX,(39+deltay)*SIZESPRLANSHY,SC_GREATERSPIREOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((41+deltax)*SIZESPRLANSHX,(42+deltay)*SIZESPRLANSHY,SC_SPORECOLONYOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((41+deltax)*SIZESPRLANSHX,(46+deltay)*SIZESPRLANSHY,SC_SUNKENCOLONYOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((46+deltax)*SIZESPRLANSHX,(42+deltay)*SIZESPRLANSHY+16,SC_INFCOMMCENTEROBJ,player,loadedmap);
}
//=====================================
void testterran(int deltax,int deltay,int player,struct mapinfo *loadedmap)
{
    OBJ *xx;
    gameconf.pl_race[player]=TERRANRACE;
    loadedmap->pl_location[player]=player;
    gameconf.pl_owner[player]=OWNER_HUMAN;

    xx=CreateUnitsFromMAP((25+deltax)*SIZESPRLANSHX+16,(25+deltay)*SIZESPRLANSHY+16,SC_SCVOBJ,player,loadedmap);
#ifdef ONEUNITEACHRACE
    return;
#endif
    xx=CreateUnitsFromMAP((27+deltax)*SIZESPRLANSHX+16,(25+deltay)*SIZESPRLANSHY+16,SC_MARINEOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((29+deltax)*SIZESPRLANSHX+16,(25+deltay)*SIZESPRLANSHY+16,SC_FIREBATOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((31+deltax)*SIZESPRLANSHX+16,(25+deltay)*SIZESPRLANSHY+16,SC_GHOSTOBJ,player,loadedmap);
    if (xx)
	    SetUnitMana(xx,200);
    xx=CreateUnitsFromMAP((33+deltax)*SIZESPRLANSHX+16,(25+deltay)*SIZESPRLANSHY+16,SC_MEDICOBJ,player,loadedmap);
    if (xx)
	    SetUnitMana(xx,200);
    xx=CreateUnitsFromMAP((35+deltax)*SIZESPRLANSHX+16,(25+deltay)*SIZESPRLANSHY+16,SC_VULTUREMINEOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((25+deltax)*SIZESPRLANSHX+16,(27+deltay)*SIZESPRLANSHY+16,SC_VULTUREOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((27+deltax)*SIZESPRLANSHX+16,(27+deltay)*SIZESPRLANSHY+16,SC_TANKNORMALOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((29+deltax)*SIZESPRLANSHX+16,(27+deltay)*SIZESPRLANSHY+16,SC_TANKSIEGEOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((31+deltax)*SIZESPRLANSHX+16,(27+deltay)*SIZESPRLANSHY+16,SC_GOLIATHOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((33+deltax)*SIZESPRLANSHX+16,(27+deltay)*SIZESPRLANSHY+16,SC_WRAITHOBJ,player,loadedmap);
    if (xx)
	    SetUnitMana(xx,200);
    xx=CreateUnitsFromMAP((25+deltax)*SIZESPRLANSHX+16,(29+deltay)*SIZESPRLANSHY+16,SC_DROPSHIPOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((27+deltax)*SIZESPRLANSHX+16,(29+deltay)*SIZESPRLANSHY+16,SC_SCIENCEVESSELOBJ,player,loadedmap);
    if (xx)
	    SetUnitMana(xx,200);
    xx=CreateUnitsFromMAP((29+deltax)*SIZESPRLANSHX+16,(29+deltay)*SIZESPRLANSHY+16,SC_BATTLECRUISEROBJ,player,loadedmap);
    if (xx)
	    SetUnitMana(xx,200);
    xx=CreateUnitsFromMAP((31+deltax)*SIZESPRLANSHX+16,(29+deltay)*SIZESPRLANSHY+16,SC_VALKYRIEOBJ,player,loadedmap);
#ifdef NONBUILD
    return;
#endif
    xx=CreateUnitsFromMAP((31+deltax)*SIZESPRLANSHX,(33+deltay)*SIZESPRLANSHY,SC_SUPPLYDEPOTOBJ,player,loadedmap);
    xx=CreateGeyserOnMap((31+deltax)*SIZESPRLANSHX,(36+deltay)*SIZESPRLANSHY,SC_GEYSEROBJ,GREYNEUTRALCOLORPLAYER,MAXGEYSERGASNR,MAXGEYSERGASNR,loadedmap);
    xx=CreateUnitsFromMAP((31+deltax)*SIZESPRLANSHX,(36+deltay)*SIZESPRLANSHY,SC_REFINERYOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((31+deltax)*SIZESPRLANSHX,(39+deltay)*SIZESPRLANSHY+16,SC_BARRACKSOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((31+deltax)*SIZESPRLANSHX,(42+deltay)*SIZESPRLANSHY+16,SC_ENGINEERINGBAYOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((35+deltax)*SIZESPRLANSHX,(30+deltay)*SIZESPRLANSHY,SC_MISSILETURRETOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((35+deltax)*SIZESPRLANSHX,(33+deltay)*SIZESPRLANSHY,SC_ACCADEMYOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((35+deltax)*SIZESPRLANSHX,(36+deltay)*SIZESPRLANSHY,SC_BUNKEROBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((35+deltax)*SIZESPRLANSHX,(39+deltay)*SIZESPRLANSHY+16,SC_FACTORYOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((35+deltax)*SIZESPRLANSHX,(42+deltay)*SIZESPRLANSHY+16,SC_STARPORTOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((41+deltax)*SIZESPRLANSHX,(30+deltay)*SIZESPRLANSHY,SC_ARMORYOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((41+deltax)*SIZESPRLANSHX,(33+deltay)*SIZESPRLANSHY+16,SC_SCIENCEFACILITYOBJ,player,loadedmap);

//    xx=CreateUnitsFromMAP((21+deltax)*SIZESPRLANSHX,(30+deltay)*SIZESPRLANSHY+16,SC_STARTLOC,player,loadedmap);
    xx=CreateUnitsFromMAP((21+deltax)*SIZESPRLANSHX,(30+deltay)*SIZESPRLANSHY+16,SC_COMMCENTEROBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((24+deltax)*SIZESPRLANSHX,(31+deltay)*SIZESPRLANSHY,SC_COMSATSTATIONOBJ,player,loadedmap);
    if (xx)
	    SetUnitMana(xx,200);
    xx=CreateUnitsFromMAP((24+deltax)*SIZESPRLANSHX,(33+deltay)*SIZESPRLANSHY,SC_COMSATSTATIONOBJ,player,loadedmap);
    if (xx)
	    SetUnitMana(xx,200);

    xx=CreateUnitsFromMAP((21+deltax)*SIZESPRLANSHX,(36+deltay)*SIZESPRLANSHY+16,SC_COMMCENTEROBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((24+deltax)*SIZESPRLANSHX,(37+deltay)*SIZESPRLANSHY,SC_NUCLEARSILOOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((24+deltax)*SIZESPRLANSHX,(39+deltay)*SIZESPRLANSHY,SC_NUCLEARSILOOBJ,player,loadedmap);

    xx=CreateUnitsFromMAP((21+deltax)*SIZESPRLANSHX,(42+deltay)*SIZESPRLANSHY+16,SC_FACTORYOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((24+deltax)*SIZESPRLANSHX,(43+deltay)*SIZESPRLANSHY,SC_MACHINESHOPOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((24+deltax)*SIZESPRLANSHX,(45+deltay)*SIZESPRLANSHY,SC_MACHINESHOPOBJ,player,loadedmap);

    xx=CreateUnitsFromMAP((11+deltax)*SIZESPRLANSHX,(42+deltay)*SIZESPRLANSHY+16,SC_STARPORTOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((14+deltax)*SIZESPRLANSHX,(43+deltay)*SIZESPRLANSHY,SC_CONTROLTOWEROBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((14+deltax)*SIZESPRLANSHX,(45+deltay)*SIZESPRLANSHY,SC_CONTROLTOWEROBJ,player,loadedmap);

    xx=CreateUnitsFromMAP((11+deltax)*SIZESPRLANSHX,(48+deltay)*SIZESPRLANSHY+16,SC_SCIENCEFACILITYOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((14+deltax)*SIZESPRLANSHX,(49+deltay)*SIZESPRLANSHY,SC_COVERTOPSOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((14+deltax)*SIZESPRLANSHX,(51+deltay)*SIZESPRLANSHY,SC_COVERTOPSOBJ,player,loadedmap);

    xx=CreateUnitsFromMAP((21+deltax)*SIZESPRLANSHX,(48+deltay)*SIZESPRLANSHY+16,SC_SCIENCEFACILITYOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((24+deltax)*SIZESPRLANSHX,(49+deltay)*SIZESPRLANSHY,SC_PHYSICSLABOBJ,player,loadedmap);
    xx=CreateUnitsFromMAP((24+deltax)*SIZESPRLANSHX,(51+deltay)*SIZESPRLANSHY,SC_PHYSICSLABOBJ,player,loadedmap);

}
//=====================================
void testmisc(int deltax,int deltay,int player,struct mapinfo *loadedmap)
{
    OBJ *xx;
    xx=CreateMineralsOnMap((29+deltax)*SIZESPRLANSHX+16,(25+deltay)*SIZESPRLANSHY+16,SC_MINERALSOBJ,GREYNEUTRALCOLORPLAYER,1000,1500,loadedmap);

#ifdef ONEUNITEACHRACE
    return;
#endif
    xx=CreateUnitsFromMAP((25+deltax)*SIZESPRLANSHX+16,(25+deltay)*SIZESPRLANSHY+16,SC_MISC_POWERGENERATOR,player,loadedmap);
    xx=CreateUnitsFromMAP((35+deltax)*SIZESPRLANSHX+16,(27+deltay)*SIZESPRLANSHY+16,SC_WARPGATE,player,loadedmap);
#ifdef NONBUILD
    return;
#endif
    xx=CreateUnitsFromMAP((31+deltax)*SIZESPRLANSHX+16,(25+deltay)*SIZESPRLANSHY+16,SC_NEUTRAL_CRITTER1,player,loadedmap);
    xx=CreateUnitsFromMAP((33+deltax)*SIZESPRLANSHX+16,(25+deltay)*SIZESPRLANSHY+16,SC_NEUTRAL_CRITTER2,player,loadedmap);
    xx=CreateUnitsFromMAP((25+deltax)*SIZESPRLANSHX+16,(27+deltay)*SIZESPRLANSHY+16,SC_NEUTRAL_CRITTER3,player,loadedmap);
    xx=CreateUnitsFromMAP((27+deltax)*SIZESPRLANSHX+16,(27+deltay)*SIZESPRLANSHY+16,SC_NEUTRAL_CRITTER4,player,loadedmap);
    xx=CreateUnitsFromMAP((29+deltax)*SIZESPRLANSHX+16,(27+deltay)*SIZESPRLANSHY+16,SC_NEUTRAL_CRITTER5,player,loadedmap);
    xx=CreateUnitsFromMAP((31+deltax)*SIZESPRLANSHX+16,(27+deltay)*SIZESPRLANSHY+16,SC_NEUTRAL_CRITTER6,player,loadedmap);
    xx=CreateGeyserOnMap((41+deltax)*SIZESPRLANSHX,(31+deltay)*SIZESPRLANSHY,SC_GEYSEROBJ,GREYNEUTRALCOLORPLAYER,MAXGEYSERGASNR,MAXGEYSERGASNR,loadedmap);
}
//=====================================
void testunits(int deltax,int deltay,int player,struct mapinfo *loadedmap)
{
    OBJ *xx;
    gameconf.pl_race[player] = TERRANRACE;
    loadedmap->pl_location[player] = player;
    gameconf.pl_owner[player] = OWNER_HUMAN;

//    xx = CreateUnitsFromMAP(200,200,SC_ZERGLINGOBJ,0,loadedmap);
//    xx = CreateUnitsFromMAP(250,250,SC_ZERGLINGOBJ,0,loadedmap);
//    ApplyCastedMage(xx,NULL,MODEDEFENSIVEMATRIX);

//    createobjman(200,256,SC_PYLONOBJ,0,0,1,1,1);
/*    xx = CreateUnitsFromMAP(256+64,256,SC_PYLONOBJ,0,loadedmap);
    CreateUnitsFromMAP(256+64+64+64,256+16,SC_GATEWAYOBJ,0,loadedmap);
    CreateUnitsFromMAP(256+64+64+64+64,256+16+64,SC_COMMCENTEROBJ,0,loadedmap);
    CreateUnitsFromMAP(256+64+64+64+64+96,256+16+64+16,SC_COMSATSTATIONOBJ,0,loadedmap);

    CreateGeyserOnMap(512,64,SC_GEYSEROBJ,GREYNEUTRALCOLORPLAYER,MAXGEYSERGASNR,MAXGEYSERGASNR,loadedmap);
    createobjman(512,64,SC_ASSIMILATOROBJ,1,0,1,1,1);

    CreateMineralsOnMap(128,128,SC_MINERAL1OBJ,GREYNEUTRALCOLORPLAYER,1000,1000,loadedmap);
    CreateMineralsOnMap(128,128+64,SC_MINERAL1OBJ,GREYNEUTRALCOLORPLAYER,700,1000,loadedmap);
    CreateMineralsOnMap(128,128+64+64,SC_MINERAL1OBJ,GREYNEUTRALCOLORPLAYER,400,1000,loadedmap);
    CreateMineralsOnMap(128,128+64+64+64,SC_MINERAL1OBJ,GREYNEUTRALCOLORPLAYER,200,1000,loadedmap);
*/
//    xx = CreateUnitsFromMAP(250,250,SC_OVERLORDOBJ,0,loadedmap);
//    CreateGeyserOnMap(512,164,SC_GEYSEROBJ,GREYNEUTRALCOLORPLAYER,MAXGEYSERGASNR,MAXGEYSERGASNR,loadedmap);

//    xx = CreateUnitsFromMAP(250,250,SC_SHUTTLEOBJ,0,loadedmap);
//    addmage(xx,ATRCORROSIVEACID,CORROSIVEACIDMAXATRVAL);
//    addmage(xx,ATRCORROSIVEACID,55<<8);
//    ApplyCastedMage(xx,NULL,MODEDEFENSIVEMATRIX);

//    CreateUnitsFromMAP(32*11,256+16,SC_HATCHERYOBJ,0,loadedmap);

    createobjman(32*11,256+16,SC_HATCHERYOBJ,0,0,1,1,1);
//    createobjman(32*5,256,SC_CREEPCOLONYOBJ,0,0,1,1,1);

/*    createobjman(200,128,SC_NEXUSOBJ,0,0,1,1,1);
    createobjman(200,256,SC_PYLONOBJ,0,0,1,1,1);
    createobjman(320,320,SC_GATEWAYOBJ,0,0,1,1,1);
    createobjman(100,128,SC_HATCHERYOBJ,0,0,1,1,1);
    createobjman(100,256,SC_COMMCENTEROBJ,0,0,1,1,1);
*/

/*    CreateGeyserOnMap(512,164,SC_GEYSEROBJ,GREYNEUTRALCOLORPLAYER,MAXGEYSERGASNR,MAXGEYSERGASNR,loadedmap);
    createobjman(512,164,SC_EXTRACTOROBJ,0,0,1,1,1);
    CreateGeyserOnMap(512,264,SC_GEYSEROBJ,GREYNEUTRALCOLORPLAYER,MAXGEYSERGASNR,MAXGEYSERGASNR,loadedmap);
    createobjman(512,264,SC_REFINERYOBJ,0,0,1,1,1);
    CreateGeyserOnMap(512,64,SC_GEYSEROBJ,GREYNEUTRALCOLORPLAYER,MAXGEYSERGASNR,MAXGEYSERGASNR,loadedmap);
    createobjman(512,64,SC_ASSIMILATOROBJ,0,0,1,1,1);

    CreateMineralsOnMap(128,128,SC_MINERAL1OBJ,GREYNEUTRALCOLORPLAYER,1000,1000,loadedmap);
    CreateMineralsOnMap(128,128+64,SC_MINERAL1OBJ,GREYNEUTRALCOLORPLAYER,700,1000,loadedmap);
    CreateMineralsOnMap(128,128+64+64,SC_MINERAL1OBJ,GREYNEUTRALCOLORPLAYER,400,1000,loadedmap);
    CreateMineralsOnMap(128,128+64+64+64,SC_MINERAL1OBJ,GREYNEUTRALCOLORPLAYER,200,1000,loadedmap);
*/
//    xx=CreateUnitsFromMAP(256,256,SC_TANKNORMALOBJ,0,loadedmap);
//    createobjman(100,256,SC_COMMCENTEROBJ,0,0,1,1,1);
//    createobjman(256+96,128+16,SC_SUPPLYDEPOTOBJ,1,0,1,1,1);
//    CreateUnitsFromMAP(256+96,256+32,SC_COMSATSTATIONOBJ,0,loadedmap);
//    CreateUnitsFromMAP(256,256,SC_COMMCENTEROBJ,0,loadedmap);
//    CreateUnitsFromMAP(256+96,256+16,SC_NUCLEARSILOOBJ,0,loadedmap);
//    createobjman(256+96,256+16,SC_NUCLEARSILOOBJ,1,0,1,1,1);
//    createobjman(256+128+32,256,SC_PYLONOBJ,0,0,1,1,1);
//    CreateGeyserOnMap(712,64,SC_GEYSEROBJ,GREYNEUTRALCOLORPLAYER,MAXGEYSERGASNR,MAXGEYSERGASNR,loadedmap);
//    CreateUnitsFromMAP(600,150,SC_MARINEOBJ,0,loadedmap);

//    CreateUnitsFromMAP(128+16,128,SC_SPAWNINGPOOLOBJ,0,loadedmap);
//    xx=createobjman(512,512+16,SC_HATCHERYOBJ,0,0,1,1,1);
//    CreateUnitsFromMAP(256+256+64,256+16,SC_GATEWAYOBJ,0,loadedmap);

//    xx = CreateUnitsFromMAP(50,50,SC_OVERLORDOBJ,0,loadedmap);
//    xx = CreateUnitsFromMAP(60,50,SC_OVERLORDOBJ,0,loadedmap);
//    xx = CreateUnitsFromMAP(70,50,SC_OVERLORDOBJ,0,loadedmap);
//    xx = CreateUnitsFromMAP(80,50,SC_OVERLORDOBJ,0,loadedmap);

//    xx = CreateUnitsFromMAP(100+32*1,156,SC_LARVAEOBJ,0,loadedmap);
//    DoOrder(NUMBGAMER,xx,0,0,SC_ZERGLINGOBJ,NOSHOWERROR);
//    xx = CreateUnitsFromMAP(100+32*2,156,SC_LARVAEOBJ,0,loadedmap);
//    DoOrder(NUMBGAMER,xx,0,0,SC_OVERLORDOBJ,NOSHOWERROR);

//    xx = CreateUnitsFromMAP(100+32*3,156,SC_LARVAEOBJ,0,loadedmap);
//    DoOrder(NUMBGAMER,xx,0,0,SC_HYDRALISKOBJ,NOSHOWERROR);
//    xx = CreateUnitsFromMAP(100+32*4,156,SC_LARVAEOBJ,0,loadedmap);
//    DoOrder(NUMBGAMER,xx,0,0,SC_SCOURGEOBJ,NOSHOWERROR);
//    xx = CreateUnitsFromMAP(100+32*5,156,SC_LARVAEOBJ,0,loadedmap);
//    DoOrder(NUMBGAMER,xx,0,0,SC_MUTALISKOBJ,NOSHOWERROR);

//    xx = CreateUnitsFromMAP(100+32*6,156,SC_LARVAEOBJ,0,loadedmap);
//    DoOrder(NUMBGAMER,xx,0,0,SC_QUEENOBJ,NOSHOWERROR);
//    xx = CreateUnitsFromMAP(100+32*7,156,SC_LARVAEOBJ,0,loadedmap);
//    DoOrder(NUMBGAMER,xx,0,0,SC_DEFILEROBJ,NOSHOWERROR);
//    xx = CreateUnitsFromMAP(100+32*8,156,SC_LARVAEOBJ,0,loadedmap);
//    DoOrder(NUMBGAMER,xx,0,0,SC_ULTRALISKOBJ,NOSHOWERROR);
//    xx = CreateUnitsFromMAP(100+32*9,156,SC_HYDRALISKOBJ,0,loadedmap);
//    DoOrder(NUMBGAMER,xx,0,0,SC_LURKEROBJ,NOSHOWERROR);
//    xx = CreateUnitsFromMAP(100+32*10,156,SC_MUTALISKOBJ,0,loadedmap);
//    DoOrder(NUMBGAMER,xx,0,0,SC_GUARDIANOBJ,NOSHOWERROR);
//    xx = CreateUnitsFromMAP(100+32*11,156,SC_MUTALISKOBJ,0,loadedmap);
//    DoOrder(NUMBGAMER,xx,0,0,SC_DEVOUREROBJ,NOSHOWERROR);
//    createobjman(32*15,320+16,SC_NEXUSOBJ,0,0,1,1,1);
//    createobjman(32*10,64,SC_PYLONOBJ,0,0,1,1,1);

//    xx = createobjman(32*11,32*5+16,SC_COMMCENTEROBJ,0,0,1,1,1);
//    xx = CreateUnitsFromMAP(50,50,SC_OVERLORDOBJ,0,loadedmap);
//    xx = CreateUnitsFromMAP(100+32*3,156,SC_LARVAEOBJ,0,loadedmap);
//    createobjman(32*10,64,SC_PYLONOBJ,0,0,1,1,1);
//    createobjman(32*5,256,SC_COMMCENTEROBJ,0,0,1,1,1);
//    xx = CreateUnitsFromMAP(32*5,32*2+16,SC_COMMCENTEROBJ,0,loadedmap);
//    moveobj(xx,NULL,SC_COMSATSTATIONOBJ,32*5+96,32*2+16+16,NOSHOWERROR);
/*    xx = CreateUnitsFromMAP(32*5,32*5+16,SC_COMMCENTEROBJ,0,loadedmap);
    moveobj(xx,NULL,SC_NUCLEARSILOOBJ,32*5+96,32*5+16+16,NOSHOWERROR);
    xx = CreateUnitsFromMAP(32*5,32*8+16,SC_FACTORYOBJ,0,loadedmap);
    moveobj(xx,NULL,SC_MACHINESHOPOBJ,32*5+96,32*8+16+16,NOSHOWERROR);

    xx = CreateUnitsFromMAP(32*11,32*2+16,SC_STARPORTOBJ,0,loadedmap);
    moveobj(xx,NULL,SC_CONTROLTOWEROBJ,32*11+96,32*2+16+16,NOSHOWERROR);
    xx = CreateUnitsFromMAP(32*11,32*5+16,SC_SCIENCEFACILITYOBJ,0,loadedmap);
    moveobj(xx,NULL,SC_COVERTOPSOBJ,32*11+96,32*5+16+16,NOSHOWERROR);
    xx = CreateUnitsFromMAP(32*11,32*8+16,SC_SCIENCEFACILITYOBJ,0,loadedmap);
    moveobj(xx,NULL,SC_PHYSICSLABOBJ,32*11+96,32*8+16+16,NOSHOWERROR);
*/
}
//=====================================
//=====================================
char *CLANNAMES[3][8]=	{ { "Zerg Brood Swarm",
			    "Clatuzg Zerg Swarm",
			    "Ghumatz Other Swarm",
			    "Elite Overmind Broods",
			    "Zerg Slayer Animals",
			    "Sarah's Defenders",
			    "Serebrate's Controls",
			    "Overlord's Minions" } ,
			  { "Veteran Troopers",
			    "Blackwater Elite",
			    "Wayland Cyborg Corporation",
			    "Sarah's Elite Cloakers",
			    "Federation Elite Forces",
			    "Renegate Fighters",
			    "Ungovernmental Coalition",
			    "Earth's Troopships"},
			  { "Executor's Special Forces",
			    "Aiur Fanatic Defenders",
			    "Universe Life Destroyers",
			    "Unknown Dark Forces",
			    "Truth Seekers",
			    "Lost Templars",
			    "Adun's Followers",
			    "Tasadar's Revengers "}
			};
int nextstring[3];
//=====================================
void ConnectingPairBuilds(struct mapinfo *loadedmap)
{
    int i,j;
    OBJ *a;
    OBJstruct *b;
    if (havezergparied)
    {
	for (i=0;i<MaxObjects;i++)
	{
	    a=objects[i];
	    if (a->paried==ZERGPARIED)
//	    if (a->data.beforestartplay.paried==ZERGPARIED)
	    {
		for (j=i+1;j<MaxObjects;j++)
		{
		    if (a->data.beforestartplay.serial2==objects[j]->data.beforestartplay.serial1)
		    {
			//found doubles builds
			b=loadobj(objects[j]->SC_Unit);
			a->doubleunit = objects[j];
			ChangeTypeOfProp(a->doubleunit,b,PROPEMPTY);
			objects[j]->doubleunit=a;
			ChangeTypeOfProp(objects[j]->doubleunit,b,PROPEMPTY);
			break;
		    }
		}
	    }
	}
    }
}
//=====================================
void createallobj(struct mapinfo *loadedmap)
{
    int i,j,pl;
    char compname[50];
    
    curentlocation=0;
    haveterranparied=0;
    havezergparied=0;

    setplayername(NUMBGAMER,nickname);

    for (i=0,j=1;i<MAXPLAYERS;i++)
    {
	if (IfPlayerHaveStartLocation(loadedmap,i)!=-1)
	    if (gameconf.pl_owner[i] == OWNER_COMPUTER)
	    {
		sprintf(compname,MYTBLSTR(MYINFO_TBL_COMPUTERNAME),j++);
		setplayername(i,compname);
	    }
    }
    CreateDoodadsFromLists(&map_doodads,loadedmap);
#ifndef TEST
    CreateUnitsFromLists(&map_units,loadedmap);
    ConnectingPairBuilds(loadedmap);
#endif
    ShowAdvisorPortrait();
    CheckProtossBuildsForPower(loadedmap);
#ifdef TEST
    SetVisualMapPosition(0,0);
    GAMETYPE = MAP_GAMETYPE_MELEE;
    int plnr=0;
//    testunits(10,5,plnr++,loadedmap);
//    testterran(10,5,plnr++,loadedmap);
//    testzerg(-20,-20,plnr++,loadedmap);
//    testprotoss(10,-20,plnr++,loadedmap);
//    testmisc(20,20,GREYNEUTRALCOLORPLAYER,loadedmap);

#endif
    
}
//=====================================
