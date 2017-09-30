
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
#include "objinfo.h"
//=================================
//=================================
int GhostNUKEAction(OBJ *a)
{
    int ret;
    if (a->finalOBJ)
        ret = CreateNuke(a,GetOBJx(a->finalOBJ),GetOBJy(a->finalOBJ));
    else
        ret = CreateNuke(a,a->finalx>>8,a->finaly>>8);
    a->finalOBJ = NULL;
    return(ret);
}
//=================================
void GhostNUKECancel(OBJ *a,int mode)//GHOSTCANCELNUKE-if cancel,GHOSTRELEASENUKE-if bomb APEAR AND NEED to release ghost
{
    if (a->doubleunit)
    {
	//destroy nuke target dot, if any
	a->doubleunit->data.nukehitdot.hitdot->SetIScriptNr(ISCRIPTNR_DEATH);
	if (mode == GHOSTCANCELNUKE)
	{
    	    a->doubleunit->doubleunit=NULL;
//	    dieobj_silently(a->doubleunit);	//destroy nuke
	    dieobj(a->doubleunit);		//die nuke
	    a->doubleunit=NULL;
	}
	moveobj(a,NULL,MODESTOP,0,0,NOSHOWERROR,0);
    }
}
//=================================
int CheckIfCanBuild(OBJ *a,SCUNIT SC_Unit,int x,int y,int *constrerror)
{
    unsigned short retarray[MAXBUILDSPRSIZE*MAXBUILDSPRSIZE];
    int xp,yp,xs,ys,ret,goods,mcost,gcost;
    xp = (x - GetUnitWidthAndHeight(SC_Unit,UNITDIM_WIDTH)/2)/SIZESPRLANSHX;
    yp = (y - GetUnitWidthAndHeight(SC_Unit,UNITDIM_HEIGHT)/2)/SIZESPRLANSHY;
    xs = GetUnitWidthAndHeight(SC_Unit,UNITDIM_WIDTH)/SIZESPRLANSHX;
    ys = GetUnitWidthAndHeight(SC_Unit,UNITDIM_HEIGHT)/SIZESPRLANSHY;
    ret = CanBuildOnPlace(xp,yp,xs,ys,retarray,NULL,SC_Unit,a->playernr,constrerror);
    return(ret);
}
//=================================
int DecrUnitCost(SCUNIT SC_Unit,int playernr)
{
    int mcost,gcost,retstatus;
    GetCostUnit(SC_Unit,&mcost,&gcost);
    retstatus = CheckResourcePlayer(playernr,mcost,gcost);
    if ( retstatus == CHECKRES_OK)
    {
        ChangeResourcePlayer(playernr,MINUSFACTOR,mcost,gcost);
        return(1);
    }
    playadvisorerr(playernr,GetUnitRace(SC_Unit),retstatus);
    return(0);
}
//=================================
int CheckIfCanLand(OBJ *a,int x,int y,int *constrerror)
{
    unsigned short retarray[MAXBUILDSPRSIZE*MAXBUILDSPRSIZE];
    int xp,yp,xs,ys,ret,goods,temperror;
    xp = (x - GetUnitWidthAndHeight(a->SC_Unit,UNITDIM_WIDTH)/2)/SIZESPRLANSHX;
    yp = (y - GetUnitWidthAndHeight(a->SC_Unit,UNITDIM_HEIGHT)/2)/SIZESPRLANSHY;
    xs = GetUnitWidthAndHeight(a->SC_Unit,UNITDIM_WIDTH)/SIZESPRLANSHX;
    ys = GetUnitWidthAndHeight(a->SC_Unit,UNITDIM_HEIGHT)/SIZESPRLANSHY;
    ret = CanBuildOnPlace(xp,yp,xs,ys,retarray,NULL,a->SC_Unit,a->playernr,&temperror);
    if (!ret)
	*constrerror=TOBELAND_CANTLANDHERE;
    return(ret);
}
//=================================
