#include <string.h>
#include <stdlib.h>

//#include <limits.h>
#include <dirent.h>


#include "wmem.h"

#include "man.h"
#include "vars.h"
#include "diap.h"
#include "objs.h"
#include "maps.h"
#include "upgrade.h"
#include "market.h"
#include "scripts.h"
#include "putobj.h"
#include "lists.h"
#include "load.h"
#include "ObjChecks.h"
#include "iscript.h"
#include "images.h"
#include "sigorder.h"
#include "construct.h"
//==================================
CONSTRUCT *CreateConstrStruct(void)
{
	CONSTRUCT *constr=(CONSTRUCT *)wmalloc(sizeof(CONSTRUCT));
	memset(constr,0,sizeof(CONSTRUCT));
	return(constr);
}
//==================================
void DestroyConstrStruct(OBJ *a)
{
	if (a->construct)
	{
		wfree(a->construct);
		a->construct=NULL;
		SetOBJIScriptNr(a,ISCRIPTNR_WORKINGTOIDLE,ISCRIPTNR_EXECUTE);
		ChangeTypeOfProp(a,PROPNORMAL1);
	}
}
//==================================
void AddToConstr(CONSTRUCT *construct,int constrelem,int type_id,int object_id,int upgr_nr,int icon_nr,int stattxt_nr)
{
	construct->type[constrelem]=type_id;
	construct->objs[constrelem]=object_id;
	construct->upgr[constrelem]=upgr_nr;
	construct->icon[constrelem]=icon_nr;
	construct->stattxt[constrelem]=stattxt_nr;
	if (!constrelem)
	{
		switch (type_id)
		{
		case ORDERS_NONE:
		case ORDERS_MODEMOVE:
			break;
		case ORDERS_UNIT:
			construct->timeremained = GetUnitBuildTime(object_id);
			construct->timemax = construct->timeremained;
			break;
		case ORDERS_UPGRADE:
			construct->timeremained = GetUpgradeResearchTime(object_id, upgr_nr);
			construct->timemax = construct->timeremained;
			break;
		case ORDERS_TECHNOLOGY:
			construct->timeremained = GetTechResearchTime(object_id);
			construct->timemax = construct->timeremained;
			break;
		}
	}
}
//==================================
int AddConstruct(OBJ *a, int type_id, int obj_id, int upgr_nr, int icon_nr, int stattxt_id)	//upgrd_nr from 1-...
{
	if (!a->construct)
	{
		a->construct = CreateConstrStruct();
		SetOBJIScriptNr(a, ISCRIPTNR_ISWORKING, ISCRIPTNR_EXECUTE);
	}
	if (a->construct->nrofelem < MAXCONSTRUCTINBUILD)
	{
		AddToConstr(a->construct, a->construct->nrofelem, type_id, obj_id, upgr_nr, icon_nr, stattxt_id);
		a->construct->nrofelem++;
		//if upgrade or tech we need to set tempflag
		switch (type_id)
		{
		case ORDERS_UPGRADE:
			SetTempUpgradeTree(&map, a->playernr, obj_id, ADDUPGRADE);
			break;
		case ORDERS_TECHNOLOGY:
			SetTempTechTree(&map, a->playernr, obj_id, ADDUPGRADE);
			break;
		}
		return(1);
	}
	return(0);
}
//=======================================
int DelLastConstruct(OBJ *a,int constrobj,int *type_id,int *obj_id)
{
	if (!a->construct)
		return(0);
	if (a->construct->nrofelem)
		return(DelConstruct(a,a->construct->nrofelem-1,type_id,obj_id));
	return(0);
}
//=======================================
int DelConstruct(OBJ *a, int constrobj, int *type_id, int *obj_id)
{
	int upgr_nr, timeremain, icon_nr, stattxt_nr;
	int i;
	if (!a->construct)
		return(0);
	if (constrobj < a->construct->nrofelem)
	{
		switch (a->construct->type[constrobj])
		{
		case ORDERS_UPGRADE:
			SetTempUpgradeTree(&map, a->playernr, a->construct->objs[constrobj], DELUPGRADE);
			break;
		case ORDERS_TECHNOLOGY:
			SetTempTechTree(&map, a->playernr, a->construct->objs[constrobj], DELUPGRADE);
			break;
		}
		//we need to do next objects, lets scroll construct objects
		if (constrobj + 1 >= a->construct->nrofelem)
		{
			GetConstruct(a, constrobj, type_id, obj_id, &upgr_nr, &timeremain, &icon_nr, &stattxt_nr);
		}
		else
		{
			for (i = constrobj + 1;i < a->construct->nrofelem;i++)
			{
				if (!GetConstruct(a, i, type_id, obj_id, &upgr_nr, &timeremain, &icon_nr, &stattxt_nr))
					break;//no objects to deconstruct
				AddToConstr(a->construct, i - 1, *type_id, *obj_id, upgr_nr, icon_nr, stattxt_nr);
			}
		}
		//if upgrade or tech we need to clear tempflag
		if (--a->construct->nrofelem == 0)
		{
			DestroyConstrStruct(a);
		}
		return(1);
	}
	return(0);
}
//=======================================
int GetConstructNR(OBJ *a)
{
	if (!a->construct)
		return(0);
	if (a->construct->nrofelem>0)
	{
		if (a->construct->type[0]!=ORDERS_UNIT)
			return(1);
	}
	return(a->construct->nrofelem);
}
//=======================================
int IfConstructAddon(OBJ *a)
{
	if (!a->construct)
		return(0);
	if (a->construct->nrofelem > 0)
	{
		if (a->construct->type[0] == ORDERS_UNIT && IsAddon(a->construct->objs[0]))
			return(1);
	}
	return(0);
}
//=======================================
int GetConstrComplete(OBJ *a,int percent)
{
	int ret=0;
	if (a->construct)
	{
		if (a->construct->nrofelem)
			ret=(a->construct->timemax-a->construct->timeremained)*percent/a->construct->timemax;
	}
	return(ret);
}
//=======================================
int GetConstruct(OBJ *a, int elemnr, int *type_id, int *obj_id, int *upgr_nr, int *timeremained, int *icon_nr, int *stattxt_nr)
{
	if (!a->construct)
		return(0);
	if (elemnr < a->construct->nrofelem)
	{
		if (type_id)
			*type_id = a->construct->type[elemnr];
		if (obj_id)
			*obj_id = a->construct->objs[elemnr];
		if (upgr_nr)
			*upgr_nr = a->construct->upgr[elemnr];
		if (timeremained)
			*timeremained = a->construct->timeremained;
		if (icon_nr)
			*icon_nr = a->construct->icon[elemnr];
		if (stattxt_nr)
			*stattxt_nr = a->construct->stattxt[elemnr];
		return(a->construct->nrofelem);
	}
	return(0);
}
//=======================================
void ConstructUnitsReleaseSupply(OBJ *a)
{
	int type_id, obj_id, i;
	if (!a->construct)
		return;
	for (i = 0;i < a->construct->nrofelem;i++)
	{
		GetConstruct(a, i, &type_id, &obj_id, NULL, NULL, NULL, NULL);
		switch (type_id)
		{
		case ORDERS_UNIT:
			ChangeSupply(a->playernr, obj_id, MINUSFACTOR);
			break;
		case ORDERS_UPGRADE:
			SetTempUpgradeTree(&map, a->playernr, a->construct->objs[i], DELUPGRADE);
			break;
		case ORDERS_TECHNOLOGY:
			SetTempTechTree(&map, a->playernr, a->construct->objs[i], DELUPGRADE);
			break;

		}
	}
}
//=======================================
void immediateSTOP(OBJ *a)
{
	a->finalx=0;
	a->finaly=0;
	a->prop &= ~(VARACCELERATIONBIT | VARMOVEOBJACT);
	moveobj(a,NULL,MODESTOP,NOSHOWERROR);
	a->currentspeed = 0;
}
//=======================================
void UnitConstrUnit(OBJ *a, SCUNIT SC_Unit, int icon_nr, int stattxt_nr)
{
	int mcost, gcost;
	GetCostUnit(SC_Unit, &mcost, &gcost);
	if (CheckSupplyPlayer(a->playernr, SC_Unit) == CHECKRES_OK)
		if (CheckResourcePlayer(a->playernr, mcost, gcost) == CHECKRES_OK)
		{
			switch (a->SC_Unit)
			{
			case SC_LARVAEOBJ:
				if (a->myparent)
				{
					a->finalx = a->myparent->finalx;
					a->finaly = a->myparent->finaly;
					delchild(a->myparent, a);
				}
				a->SC_ConstrUnit = SC_Unit;					//construct unit
				a->SC_FromUnit = a->SC_Unit;				//save previous unit
				ChangeUnitAndImagesAssociated(a, SC_EGGOBJ);
				ChangeSC_Unit(a, a->playernr, SC_EGGOBJ, CHANGESC_UNIT_CONSTR);
				SetUnitPercentHealth(a, 100);
				SetUnitPercentShield(a, 100);
				ChangeResourcePlayer(a->playernr, MINUSFACTOR, mcost, gcost);
				SetBeginSelfConstruct(a);
				a->selfconstruct.currentconstrstep = CONSTR_STEP_ZERGUNITLESS99;
				break;
			case SC_HYDRALISKOBJ:
				if (IsOBJBurrowed(a))
				{
					moveobj(a, NULL, MODEUNBURROW, NOSHOWERROR);
					AddModeMove(a, NULL, MODEMORPHINLURKER, 0, 0, NOSHOWERROR);
					break;
				}
				a->SC_ConstrUnit = SC_Unit;					//construct unit
				a->SC_FromUnit = a->SC_Unit;					//save previous unit
				ChangeUnitAndImagesAssociated(a, SC_LURKEREGGOBJ);
				ChangeSC_Unit(a, a->playernr, SC_LURKEREGGOBJ, CHANGESC_UNIT_CONSTR);
				a->temphealth = a->health;					//if cancel construct restore to those perameters
				a->tempshield = a->shield;
				SetUnitPercentHealth(a, 100);
				SetUnitPercentShield(a, 100);
				ChangeResourcePlayer(a->playernr, MINUSFACTOR, mcost, gcost);
				SetBeginSelfConstruct(a);
				a->selfconstruct.currentconstrstep = CONSTR_STEP_ZERGUNITLESS99;
				immediateSTOP(a);
				break;
			case SC_MUTALISKOBJ:
				a->SC_ConstrUnit = SC_Unit;					//construct unit
				a->SC_FromUnit = a->SC_Unit;					//save previous unit
				ChangeUnitAndImagesAssociated(a, SC_MUTALISKCOCOONOBJ);
				ChangeSC_Unit(a, a->playernr, SC_MUTALISKCOCOONOBJ, CHANGESC_UNIT_CONSTR);
				a->temphealth = a->health;
				a->tempshield = a->shield;
				a->health = GetUnitMaxHealth(a->SC_Unit);
				a->shield = GetUnitMaxShield(a->SC_Unit);
				ChangeResourcePlayer(a->playernr, MINUSFACTOR, mcost, gcost);
				SetBeginSelfConstruct(a);
				a->selfconstruct.currentconstrstep = CONSTR_STEP_ZERGUNITLESS99;
				immediateSTOP(a);
				break;
			default:
				a->SC_FromUnit = SC_NOUNITNR;
				if (AddConstruct(a, ORDERS_UNIT, SC_Unit, 0, icon_nr, stattxt_nr))
				{
					ChangeSupply(a->playernr, SC_Unit, PLUSFACTOR);
					ChangeResourcePlayer(a->playernr, MINUSFACTOR, mcost, gcost);
				}
				break;

			}
		}
}
//=======================================
void UnitConstrUpgr(OBJ *a,int upgr_id,int upgr_nr,int icon_nr,int stattxt_nr)
{
	int mcost,gcost;
	GetCostUpgr(upgr_id,a->playernr,&mcost,&gcost);
	if (CheckResourcePlayer(a->playernr,mcost,gcost)==CHECKRES_OK)
		if (AddConstruct(a,ORDERS_UPGRADE,upgr_id,upgr_nr,icon_nr,stattxt_nr))
		{
			ChangeResourcePlayer(a->playernr,MINUSFACTOR,mcost,gcost);
		}
}
//=======================================
void UnitConstrTech(OBJ *a,int tech_id,int icon_nr,int stattxt_nr)
{
	int mcost,gcost;
	GetCostTech(tech_id,&mcost,&gcost);
	if (CheckResourcePlayer(a->playernr,mcost,gcost)==CHECKRES_OK)
		if (AddConstruct(a,ORDERS_TECHNOLOGY,tech_id,0,icon_nr,stattxt_nr))
		{
			ChangeResourcePlayer(a->playernr,MINUSFACTOR,mcost,gcost);
		}
}
//=======================================
void DoOrder(int playernr,OBJ *a,int x,int y,int modemove,int modemoveflags)
{
	MAGEP *mp;
	mp = &mageprop[modemove];
	DoOrder(playernr,
			a,
			x,
			y,
			mageprop[modemove].type_id,
			mageprop[modemove].obj_id,
			mageprop[modemove].icon_id,
			mageprop[modemove].stattxt_id_enable,
			modemove,
			modemoveflags);
}
//=======================================
void DoOrder(int playernr, OBJ *a, int x, int y, int type_id, int obj_id, int icon_nr, int stattxt_nr, int modemove, int modemoveflags)
{
	int upgr_level = 0;
	switch (type_id)
	{
	case ORDERS_NONE:
		break;
	case ORDERS_MODEMOVE:
		makemove(a, NULL, x, y, modemove, playernr, modemoveflags);
		break;
	case ORDERS_UNIT:
		if (IsBuild(obj_id) && !IsAddon(obj_id))
			makemove(a, NULL, x, y, modemove, playernr, modemoveflags);
		else
			UnitConstrUnit(a, obj_id, icon_nr, stattxt_nr);
		break;
	case ORDERS_UPGRADE:
		upgr_level = GetUpgradeTree(&map, playernr, obj_id) + 1;//curent level + 1
		UnitConstrUpgr(a, obj_id, upgr_level, icon_nr, stattxt_nr);
		break;
	case ORDERS_TECHNOLOGY:
		UnitConstrTech(a, obj_id, icon_nr, stattxt_nr);
		break;
	}
}
//=======================================
//return type 1,2,3,4 - if done constr, 0 - if in process
//=======================================
int TickBuildMakeConstruct(OBJ *a,int *doneobj_id)
{
	int type_id,obj_id;
	int donetype;
	if (!a->construct)
		return(0);
	if (!a->construct->nrofelem)
		return(0);
	a->construct->timeremained -= CODEFORQUICKMAKE;
	if (a->construct->timeremained <= 0)
	{
		if (a->addonobj)
			TickUnderConstruct(a->addonobj); //need	 last tick of construct
		donetype = a->construct->type[0];//return finished type
		*doneobj_id = a->construct->objs[0];//return finished obj_id
		DelConstruct(a,0,&type_id,&obj_id);
		return(donetype);
	}
	return(0);
}
//==================================
int TickUnderConstruct(OBJ *a)
{
	int race, decrparam;
	OBJstruct *b;
	if (!IsReadyOBJ(a))
	{
		if (a->selfconstruct.currentconstrstep == CONSTR_STEP_ALLBUILDSEXACT100)		//wait for build teleport whitegrd done
			return(0);
		if (IsBuild(a->SC_Unit))
		{
			race = GetUnitRace(a->SC_Unit);
			if (race == TERRANRACE && !a->constrobj)//no body construct terran unit/build exit from construction
				return(0);
		}
		decrparam = CODEFORQUICKMAKE;
		a->selfconstruct.timeremained -= decrparam;
		if (a->selfconstruct.timeremained <= 0)
		{
			decrparam += a->selfconstruct.timeremained;
			a->selfconstruct.timeremained = 0;
		}
		if (decrparam)
		{
			b = loadobj(a->SC_Unit);
			if (!(b->UNITprop & NOADDSTATSATCONSTR))
			{
				AddUnitShield(a, a->selfconstruct.incrshield*decrparam);
				AddUnitHealth(a, a->selfconstruct.incrlife*decrparam);
			}
		}
		switch (a->selfconstruct.currentconstrstep)
		{
		case CONSTR_STEP_ZERGUNITLESS99:
			if (!a->selfconstruct.timeremained)
			{
				a->selfconstruct.currentconstrstep = CONSTR_STEP_ALLBUILDSEXACT100;
				SIGOrder_ZergConstrFromLarvaeEnd(a);
			}
			break;
		case CONSTR_STEP_ZERGBUILDLESS33:
			if (a->selfconstruct.timeremained <= a->selfconstruct.timemax * 2 / 3)
			{
				a->selfconstruct.currentconstrstep = CONSTR_STEP_ZERGBUILDLESS66;
				SetOBJIScriptNr(a, ISCRIPTNR_SPECIALSTATE1, ISCRIPTNR_EXECUTE);
			}
			break;
		case CONSTR_STEP_ZERGBUILDLESS66:
			if (a->selfconstruct.timeremained <= a->selfconstruct.timemax * 1 / 3)
			{
				a->selfconstruct.currentconstrstep = CONSTR_STEP_ZERGBUILDLESS99;
				SetOBJIScriptNr(a, ISCRIPTNR_SPECIALSTATE2, ISCRIPTNR_EXECUTE);
			}
			break;
		case CONSTR_STEP_ZERGBUILDLESS99:
			if (!a->selfconstruct.timeremained)
			{
				a->selfconstruct.currentconstrstep = CONSTR_STEP_ALLBUILDSEXACT100;
				//					a->prop |= VARREADY;
				SetOrder(a, 1, &SIGOrder_ZergBuildFinishConstruct);
				SetOBJIScriptNr(a, ISCRIPTNR_ALMOSTBUILT, ISCRIPTNR_EXECUTE);
			}
			break;
		case CONSTR_STEP_TERRANBUILDLESS25:
			if (a->selfconstruct.timeremained <= a->selfconstruct.timemax * 3 / 4)
			{
				a->selfconstruct.currentconstrstep = CONSTR_STEP_TERRANBUILDLESS50;
				SetOBJIScriptNr(a, ISCRIPTNR_SPECIALSTATE1, ISCRIPTNR_EXECUTE);
			}
			break;
		case CONSTR_STEP_TERRANBUILDLESS50:
			if (a->selfconstruct.timeremained <= a->selfconstruct.timemax * 2 / 4)
			{
				a->selfconstruct.currentconstrstep = CONSTR_STEP_TERRANBUILDLESS75;
				SetOBJIScriptNr(a, ISCRIPTNR_SPECIALSTATE2, ISCRIPTNR_EXECUTE);
			}
			break;
		case CONSTR_STEP_TERRANBUILDLESS75:
			if (a->selfconstruct.timeremained <= a->selfconstruct.timemax * 1 / 4)
			{
				a->selfconstruct.currentconstrstep = CONSTR_STEP_TERRANBUILDLESS99;
				//run directly
				SIGOrder_TerranBuildLastConstructSprite(a);
			}
			break;
		case CONSTR_STEP_TERRANBUILDLESS99:
			if (!a->selfconstruct.timeremained)
			{
				a->selfconstruct.currentconstrstep = CONSTR_STEP_ALLBUILDSEXACT100;
				SetOBJIScriptNr(a, ISCRIPTNR_BUILT, ISCRIPTNR_EXECUTE);
				SIGOrder_TerranBuildFinishConstruct(a);
			}
			break;
		case CONSTR_STEP_PROTOSSBUILD:
			if (!a->selfconstruct.timeremained)
			{
				a->selfconstruct.currentconstrstep = CONSTR_STEP_ALLBUILDSEXACT100;
				SetOrder(a, 1, &SIGOrder_ProtossBuildShowTexture);
				SetOBJIScriptNr(a, ISCRIPTNR_SPECIALSTATE1, ISCRIPTNR_EXECUTE);
			}
			break;
		case CONSTR_STEP_WARPARCHONS:
			if (!a->selfconstruct.timeremained)
			{
				a->selfconstruct.currentconstrstep = CONSTR_STEP_ALLBUILDSEXACT100;
				SetOrder(a, 1, &SIGOrder_UnitInitComplete);
				SetOBJIScriptNr(a, ISCRIPTNR_INIT, ISCRIPTNR_EXECUTE);
			}
			break;
		}
	}
	return(0);
}
//==================================
void SetBeginSelfConstruct(OBJ *a)
{
	int fromshield, fromlife;
	SCUNIT SC_Unit = a->SC_Unit;
	OBJstruct *b = loadobj(a->SC_Unit);
	if (a->SC_ConstrUnit != 255)
	{
		SC_Unit = a->SC_ConstrUnit;
	}
	fromshield = STARTBUILDSHIELD;
	fromlife = STARTBUILDLIFE;
	a->selfconstruct.timeremained = alldattbl.units_dat->BuildTime[SC_Unit];
	a->selfconstruct.timemax = a->selfconstruct.timeremained;
	if (!(b->UNITprop & NOADDSTATSATCONSTR))
	{
		//		a->selfconstruct.incrshield=(GetUnitMaxShield(SC_Unit)-fromshield+0x4ff)/a->selfconstruct.timemax;
		//		a->selfconstruct.incrlife=(GetUnitMaxHealth(SC_Unit)-fromlife+0x4ff)/a->selfconstruct.timemax;
		a->selfconstruct.incrshield = (GetUnitMaxShield(SC_Unit) - fromshield + a->selfconstruct.timemax - 1) / a->selfconstruct.timemax;
		a->selfconstruct.incrlife = (GetUnitMaxHealth(SC_Unit) - fromlife + a->selfconstruct.timemax - 1) / a->selfconstruct.timemax;
	}
}
//==================================
int GetSelfConstrComplete(OBJ *a,int percent)
{
	if (!a->selfconstruct.timemax)
		return(0);
	if (IsOBJUnderConstruct(a))
	{
		percent=(a->selfconstruct.timemax-a->selfconstruct.timeremained)*percent/a->selfconstruct.timemax;
	}
	return(percent);
}
