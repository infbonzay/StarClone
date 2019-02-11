
#include "diap.h"
#include "lists.h"
#include "audio.h"
#include "action.h"
#include "objs.h"

#include "auxil.h"
#include "doodad.h"
#include "maps.h"
#include "walk.h"
#include "upgrade.h"
#include "rand.h"
#include "unitaction.h"
#include "market.h"
#include "images.h"
#include "iscript.h"
#include "ObjChecks.h"
//=================================
int GhostNUKEAction(OBJ *o)
{
	int ret;
	if (o->finalOBJ)
		ret = CreateNuke(o, GetOBJx(o->finalOBJ), GetOBJy(o->finalOBJ));
	else
		ret = CreateNuke(o, o->finalx >> 8, o->finaly >> 8);
	o->finalOBJ = NULL;
	return(ret);
}
//=================================
void GhostNUKECancel(OBJ *o, int mode)//GHOSTCANCELNUKE-if cancel,GHOSTRELEASENUKE-if bomb APEAR AND NEED to release ghost
{
	if (o->doubleunit)
	{
		//destroy nuke target dot, if any
		o->doubleunit->data.nukehitdot.hitdot->SetIScriptNr(ISCRIPTNR_DEATH);
		if (mode == GHOSTCANCELNUKE)
		{
			o->doubleunit->doubleunit = NULL;
			//			dieobj_silently(o->doubleunit);		//destroy nuke
			dieobj(o->doubleunit);				//die nuke
			o->doubleunit = NULL;
		}
		moveobj(o, NULL, MODESTOP, NOSHOWERROR);
	}
	if (o->prop & VARDECRMAGE)	//decreasing mage (now is cloaked)
		ChangeTypeOfProp(o, PROPNORMAL2);
	else
		ChangeTypeOfProp(o, PROPNORMAL1);//or normal2

}
//=================================
int CheckIfCanBuild(OBJ *a, SCUNIT SC_Unit, int x, int y, int *constrerror)
{
	unsigned short retarray[MAXBUILDSPRSIZE*MAXBUILDSPRSIZE];
	int xp, yp, xs, ys, ret;
	xp = (x - GetUnitWidthAndHeight(SC_Unit, UNITDIM_WIDTH) / 2) / SIZESPRLANSHX;
	yp = (y - GetUnitWidthAndHeight(SC_Unit, UNITDIM_HEIGHT) / 2) / SIZESPRLANSHY;
	xs = GetUnitWidthAndHeight(SC_Unit, UNITDIM_WIDTH) / SIZESPRLANSHX;
	ys = GetUnitWidthAndHeight(SC_Unit, UNITDIM_HEIGHT) / SIZESPRLANSHY;
	ret = CanBuildOnPlace(xp, yp, xs, ys, retarray, NULL, SC_Unit, a->playernr, constrerror);
	return(ret);
}
//=================================
int DecrUnitCost(SCUNIT SC_Unit, int playernr)
{
	int mcost, gcost, retstatus;
	GetCostUnit(SC_Unit, &mcost, &gcost);
	retstatus = CheckResourcePlayer(playernr, mcost, gcost);
	if (retstatus == CHECKRES_OK)
	{
		ChangeResourcePlayer(playernr, MINUSFACTOR, mcost, gcost);
		return(1);
	}
	playadvisorerr(playernr, GetUnitRace(SC_Unit), retstatus);
	return(0);
}
//=================================
int CheckIfCanLand(OBJ *a, int x, int y, int *constrerror)
{
	unsigned short retarray[MAXBUILDSPRSIZE*MAXBUILDSPRSIZE];
	int xp, yp, xs, ys, ret, temperror;
	xp = (x - GetUnitWidthAndHeight(a->SC_Unit, UNITDIM_WIDTH) / 2) / SIZESPRLANSHX;
	yp = (y - GetUnitWidthAndHeight(a->SC_Unit, UNITDIM_HEIGHT) / 2) / SIZESPRLANSHY;
	xs = GetUnitWidthAndHeight(a->SC_Unit, UNITDIM_WIDTH) / SIZESPRLANSHX;
	ys = GetUnitWidthAndHeight(a->SC_Unit, UNITDIM_HEIGHT) / SIZESPRLANSHY;
	ret = CanBuildOnPlace(xp, yp, xs, ys, retarray, NULL, a->SC_Unit, a->playernr, &temperror);
	if (!ret)
		*constrerror = TOBELAND_CANTLANDHERE;
	return(ret);
}
//=================================
