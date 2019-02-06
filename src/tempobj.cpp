
#include "diap.h"
#include "load.h"
#include "walk.h"
#include "lists.h"
#include "action.h"
#include "maps.h"
#include "objinfo.h"
#include "objs.h"
#include "market.h"
#include "portrait.h"
#include "doodad.h"
#include "pylon.h"
#include "stringfiles.h"
#include "tempobj.h"

int curentlocation;
char haveterranparied;
char havezergparied;
//=====================================
int IfPlayerHaveStartLocation(StarMapInfo *info, int playernr)
{
	return(info->pl_location[playernr]);
}
//=====================================
int GetPlayerByLocation(StarMapInfo *tempmap, int location)
{
	for (int i = 0;i < PLAYEDPLAYERS;i++)
		if (tempmap->pl_location[i] == location)
			return(i);
	return(-1);
}
//=====================================
unsigned char GetFirstUnitBuild(int race, int build_or_unit)
{
	if (race > 2 || race < 0)
		return SC_NOUNITNR;
	else
		return (First_Unit_Build[race][build_or_unit]);
}
//=====================================
int GetRaceFromMap(StarMapInfo *loadedmap, int location)
{
	return(loadedmap->pl_race[location]);
}
//=====================================
int GetPlayerColorFromMap(StarMapInfo *loadedmap, int location)
{
	return(location);
}
//=====================================
int GetTotalStartLocations(StarMapInfo *testmap)
{
	return(testmap->pl_startlocs);
}
//=====================================
OBJ *CreateMineralsOnMap(int x, int y, SCUNIT SC_Unit, int playernr, int count, int maxcount, StarMapInfo *loadedmap)
{
	int xp, yp, sx, sy;
	struct OBJ *a;
	a = createobjfulllife(x, y, SC_Unit, playernr);

	xp = (GetOBJx(a) - GetUnitWidthAndHeight(SC_Unit, UNITDIM_WIDTH) / 2) / SIZESPRLANSHX - 3;
	yp = (GetOBJy(a) - GetUnitWidthAndHeight(SC_Unit, UNITDIM_HEIGHT) / 2) / SIZESPRLANSHY - 3;
	sx = 8;
	sy = 7;
	AddMarkOn32(xp, yp, sx, sy, loadedmap->constrmap);
	if (count > maxcount)
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
OBJ *CreateGeyserOnMap(int x, int y, SCUNIT SC_Unit, int playernr, int count, int maxcount, StarMapInfo *loadedmap)
{
	int xp, yp, sx, sy;
	struct OBJ *a;
	a = createobjfulllife(x, y, SC_Unit, playernr);
	xp = (GetOBJx(a) - GetUnitWidthAndHeight(SC_Unit, UNITDIM_WIDTH) / 2) / SIZESPRLANSHX - 3;
	yp = (GetOBJy(a) - GetUnitWidthAndHeight(SC_Unit, UNITDIM_HEIGHT) / 2) / SIZESPRLANSHY - 3;
	sx = 10;
	sy = 8;
	AddMarkOn32(xp, yp, sx, sy, loadedmap->constrmap);
	if (count > maxcount)
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
OBJ *CreateUnitsFromMAP(int xpos, int ypos, int unit_type, int playernr, StarMapInfo *loadedmap)
{
	unit_on_map tempunit;
	memset(&tempunit, 0, sizeof(unit_on_map));
	tempunit.xpos = xpos;
	tempunit.ypos = ypos;
	tempunit.unit_type = unit_type;
	tempunit.player = playernr;
	return(CreateUnitsFromMAP(&tempunit, loadedmap));
}
//=====================================
OBJ *CreateUnitsFromMAP(struct unit_on_map *unit, StarMapInfo *loadedmap)
{
	struct OBJ *a = NULL;
	int i, playernr, xobj, yobj, race, owner;
	SCUNIT SC_Unit;
	if (unit->paried)
	{
		if (unit->paried == TERRANPARIED)
			haveterranparied = 1;
		else
			havezergparied = 1;
	}
	switch (unit->unit_type)
	{
	case SC_MINERAL1OBJ://minerals
	case SC_MINERAL2OBJ:
	case SC_MINERAL3OBJ:
		a = CreateMineralsOnMap(unit->xpos, unit->ypos, unit->unit_type, GREYNEUTRALCOLORPLAYER, unit->resource_count, MAXMINERALSINSTACK, loadedmap);
		break;
	case SC_GEYSEROBJ:
		a = CreateGeyserOnMap(unit->xpos, unit->ypos, unit->unit_type, GREYNEUTRALCOLORPLAYER, unit->resource_count, MAXGASINSTACK, loadedmap);
		break;
	case SC_STARTLOC://start location
		if (GAMETYPE == MAP_GAMETYPE_USEMAPSETTINGS)
		{
			if (loadedmap->pl_race[unit->player] == RACE_USER_SELECT)
			{
				playernr = GetPlayerByLocation(loadedmap, curentlocation);
				if (playernr == NUMBGAMER)
				{
					SetVisualMapPositionCenter(unit->xpos, unit->ypos);
				}
			}
			else
			{
				if (unit->player == NUMBGAMER)
					SetVisualMapPositionCenter(unit->xpos, unit->ypos);
				return(NULL);
			}
		}
		playernr = GetPlayerByLocation(loadedmap, curentlocation);
		if (playernr != -1)
		{
			owner = gameconf.pl_owner[playernr];
			// do not create units for observer players
			if ((owner == OWNER_HUMAN || owner == OWNER_COMPUTER))
			{
				race = gameconf.pl_race[playernr];
				if (race != RACE_OBSERVER)
				{
					SC_Unit = GetFirstUnitBuild(race, FIRSTBUILD);
					if (SC_Unit != SC_NOUNITNR)
					{
						a = createobjfulllife(unit->xpos, unit->ypos, SC_Unit, playernr);
						if (playernr == NUMBGAMER)
						{
							SetVisualMapPositionCenter(unit->xpos, unit->ypos);
						}
						if (SC_Unit == SC_HATCHERYOBJ)
						{
							//create additional overlord
							createobjfulllife(unit->xpos, unit->ypos, SC_OVERLORDOBJ, playernr);
							//set hatchery to be made by drone
							//a->SC_FromUnit=SC_DRONEOBJ;
						}
						SC_Unit = GetFirstUnitBuild(race, FIRSTUNIT);
						for (i = 0;i < MAXBEGINPROBE;i++)
						{
							getcoordofnewunit(a, SC_Unit, &xobj, &yobj);
							createobjfulllife(xobj, yobj, SC_Unit, playernr);
						}
					}
				}
			}
		}
		curentlocation++;
		break;
	default:
		if (GAMETYPE == MAP_GAMETYPE_USEMAPSETTINGS)
		{
			a = createunitwithproperties(unit->xpos, unit->ypos, unit->unit_type, unit->player,
				unit->special_props, unit->special_prop2, unit->state_flags,
				unit->hitpoints, unit->shieldpoints, unit->energypoints, unit->resource_count, unit->units_in_hangar);
			a->paried = unit->paried;
			a->unitserial1 = unit->unit_serialnumberfirst;
			a->unitserial2 = unit->unit_serialnumbersecond;
		}
		else
		{
			if (!IsGroupNeutralFlag(unit->unit_type))				//in melee do not create neutral units/builds
				return(NULL);
			a = createobjfulllife(unit->xpos, unit->ypos, unit->unit_type, unit->player);
		}
		break;
	}
	return(a);
}
//=====================================
void AddToMinimapNeutralObjs(mylist *units, StarMapInfo *loadedmap, int playortest)
{
	int sx, sy;
	struct unit_on_map *unit;
	units->EnumListInit();
	while ((unit = (unit_on_map *)units->GetNextListElem()))
	{
		sx = GetUnitBoxWidth(unit->unit_type);
		sy = GetUnitBoxHeight(unit->unit_type);
		if (unit->unit_type == SC_STARTLOC && playortest == STARMAP_LOADFORINFO)
		{
			ObjOnMiniMap(unit->xpos / 32 - sx / 64, unit->ypos / 32 - sy / 64, sx, sy, MINIMAP_COLORFORSTARTLOC, loadedmap->minimap);
		}
		else
		{
			if (unit->player >= PLAYEDPLAYERS)
			{
				if (IsBuild(unit->unit_type))
				{
					if (IfCanClickOBJ(unit->unit_type))
						ObjOnMiniMap(unit->xpos / 32 - sx / 64, unit->ypos / 32 - sy / 64, sx, sy, MINIMAP_COLORFORRESOURCES, loadedmap->minimap);
				}
			}
		}
	}
}
//=====================================
void CreateUnitsFromLists(mylist *units, StarMapInfo *loadedmap)
{
	int i;
	struct unit_on_map *unit;
	units->EnumListInit();
	while ((unit = (unit_on_map *)units->GetNextListElem()))
	{
		if (unit->unit_type != SC_STARTLOC)
		{
			CreateUnitsFromMAP(unit, loadedmap);
		}
	}
	//parse all base builds, because we need to know all resources before place creep
	units->EnumListInit();
	while ((unit = (unit_on_map *)units->GetNextListElem()))
	{
		if (unit->unit_type == SC_STARTLOC)
		{
			CreateUnitsFromMAP(unit, loadedmap);
		}
	}
	units->EnumListInit();
	while ((unit = (unit_on_map *)units->GetNextListElem(&i)))
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
void ConnectingPairBuilds(StarMapInfo *loadedmap)
{
	int i, j;
	OBJ *a;
	OBJstruct *b;
	if (havezergparied)
	{
		for (i = 0;i < MaxObjects;i++)
		{
			a = objects[i];
			if (a->paried == ZERGPARIED)
			for (j = i + 1;j < MaxObjects;j++)
			{
				if (a->unitserial2 == objects[j]->unitserial1)
				{
					//found doubles builds
					b = loadobj(objects[j]->SC_Unit);
					a->doubleunit = objects[j];
					ChangeTypeOfProp(a->doubleunit, b, PROPEMPTY);
					objects[j]->doubleunit = a;
					ChangeTypeOfProp(objects[j]->doubleunit, b, PROPEMPTY);
					break;
				}
			}
		}
	}
}
//=====================================
void ApplyObjectsToMap(StarMapInfo *loadedmap)
{
	int i, j;
	char compname[50];

	curentlocation = 0;
	haveterranparied = 0;
	havezergparied = 0;

	setplayername(NUMBGAMER, nickname);

	for (i = 0, j = 1;i < MAXPLAYERS;i++)
	{
		if (IfPlayerHaveStartLocation(loadedmap, i) != -1)
			if (gameconf.pl_owner[i] == OWNER_COMPUTER)
			{
				sprintf(compname, MYTBLSTR(MYINFO_TBL_COMPUTERNAME), j++);
				setplayername(i, compname);
			}
	}
	CreateDoodadsFromLists(&map_doodads, loadedmap);

	CreateUnitsFromLists(&map_units, loadedmap);
	ConnectingPairBuilds(loadedmap);

	ShowAdvisorPortrait();
	CheckProtossBuildsForPower(loadedmap);

}
//=====================================
