
#include "wmem.h"
#include "audio.h"

#include "defs.h"
#include "vars.h"
#include "load.h"
#include "putobj.h"
#include "objinfo.h"
#include "gr.h"
#include "lists.h"
#include "market.h"
#include "overlay.h"
#include "objs.h"
#include "images.h"
#include "pylon.h"
//=================================
unsigned char PylonTable[PYLONAREAY][PYLONAREAX]=
{
    {0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0},
    {0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0},
    {0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0}
};
//=================================
void FreePylonArea(struct mapinfo *mymap)
{
    int i;
    for (i=0;i<MAXPLAYERS;i++)
    {
        if (mymap->pylonarea[i])
        {
            wfree(mymap->pylonarea[i]);
	    mymap->pylonarea[i]=NULL;
        }
    }
}
//=================================
unsigned char *CreatePylonAreaArray(struct mapinfo *mymap,int playernr)
{
    unsigned char *parea;
    parea=(unsigned char*)wmalloc(mymap->map_width*mymap->map_height);
    memset(parea,0x00,mymap->map_width*mymap->map_height);
    mymap->pylonarea[playernr]=parea;
    return(parea);
}
//=================================
void AddPylonArea(struct mapinfo *mymap,OBJ *a,int playernr)
{
    int xp,yp,i,j,offset,x,y;
    unsigned char *parea=mymap->pylonarea[playernr];
    if (!parea)
	parea=CreatePylonAreaArray(mymap,playernr);
    xp = GetOBJx(a)/SIZESPRLANSHX-PYLONAREAX/2;
    yp = GetOBJy(a)/SIZESPRLANSHY-PYLONAREAY/2;
    for (i=0;i<PYLONAREAY;i++)
    {
	y=yp+i;
	if (y<0)
	    continue;
	if (y>=MAXYMAP)
	    break;
	offset=y*MAXXMAP;
	for (j=0;j<PYLONAREAX;j++)
	{
	    x=xp+j;
	    if (x<0)
		continue;
	    if (x>=MAXXMAP)
		break;
	    parea[offset+x] += PylonTable[i][j];
	}
    }
    AddBuildsInPower(parea,playernr);
}
//=================================
void DelPylonArea(struct mapinfo *mymap,OBJ *a,int playernr)
{
    int xp,yp,i,j,offset,x,y;
    unsigned char *parea=mymap->pylonarea[playernr];
    if (!parea)
	parea=CreatePylonAreaArray(mymap,playernr);
    xp = GetOBJx(a)/SIZESPRLANSHX-PYLONAREAX/2;
    yp = GetOBJy(a)/SIZESPRLANSHY-PYLONAREAY/2;
    for (i=0;i<PYLONAREAY;i++)
    {
	y=yp+i;
	if (y<0)
	    continue;
	if (y>=MAXYMAP)
	    break;
	offset=y*MAXXMAP;
	for (j=0;j<PYLONAREAX;j++)
	{
	    x=xp+j;
	    if (x<0)
		continue;
	    if (x>=MAXXMAP)
		break;
	    parea[offset+x]-=PylonTable[i][j];
	}
    }
    DelBuildsFromPower(parea,playernr);
}
//=================================
void AddBuildsInPower(unsigned char *pylonarea,int playernr)
{
    int i,j,k,nrofpower,xp,yp,xs,ys;
    OBJ *a;
    for (k=0;k<MaxObjects;k++)
    {
	a = objects[k];
	if (a->playernr != playernr)
	    continue;
	if (a->prop & VARPOWEROFF)
	{
	    if (RequiresPylonEnergy(a->SC_Unit))
	    {
		nrofpower = 0;
		xp = (GetOBJx(a) - GetUnitWidthAndHeight(a->SC_Unit,UNITDIM_WIDTH)/2)/SIZESPRLANSHX;
		yp = (GetOBJy(a) - GetUnitWidthAndHeight(a->SC_Unit,UNITDIM_HEIGHT)/2)/SIZESPRLANSHY;
		xs = GetUnitWidthAndHeight(a->SC_Unit,UNITDIM_WIDTH)/SIZESPRLANSHX;
		ys = GetUnitWidthAndHeight(a->SC_Unit,UNITDIM_HEIGHT)/SIZESPRLANSHY;
		for (i=0;i<ys;i++)
		    for (j=0;j<xs;j++)
			if (pylonarea[(i+yp)*MAXXMAP+j+xp])
			    nrofpower++;
		if (nrofpower >= xs*ys/2)
		{
		    moveobj(a,NULL,MODEPOWERON,NOSHOWERROR);
		}
	    }
	}
    }
}
//=================================
void DelBuildsFromPower(unsigned char *pylonarea,int playernr)
{
    int i,j,k,nrofpower,xp,yp,xs,ys;
    OBJ *a;
    for (k=0;k<MaxObjects;k++)
    {
	a = objects[k];
	if (a->playernr != playernr)
	    continue;
	if ( !(a->prop & VARPOWEROFF) && a->modemove != MODEDIE )
	{
	    if (!IsOBJUnderConstruct(a))
		if (RequiresPylonEnergy(a->SC_Unit))
		{
		    nrofpower=0;
		    xp = (GetOBJx(a) - GetUnitWidthAndHeight(a->SC_Unit,UNITDIM_WIDTH)/2)/SIZESPRLANSHX;
		    yp = (GetOBJy(a) - GetUnitWidthAndHeight(a->SC_Unit,UNITDIM_HEIGHT)/2)/SIZESPRLANSHY;
		    xs = GetUnitWidthAndHeight(a->SC_Unit,UNITDIM_WIDTH)/SIZESPRLANSHX;
		    ys = GetUnitWidthAndHeight(a->SC_Unit,UNITDIM_HEIGHT)/SIZESPRLANSHY;
		    for (i=0;i<ys;i++)
		        for (j=0;j<xs;j++)
		    	if (pylonarea[(i+yp)*MAXXMAP+j+xp])
		    	    nrofpower++;
		    if (nrofpower < xs*ys/2)
		    {
		        if (gameconf.audioconf.buildsounds && a->playernr == NUMBGAMER)
			    Play_sfxdata_id(a,sfx_powerdown[PROTOSSRACE],2,0);
			moveobj(a,NULL,MODEPOWEROFF,NOSHOWERROR);
		    }
		}
	}
    }
}
//=================================
void CheckBuildForPower(struct mapinfo *mymap,OBJ *a,int playernr)
{
    int i,j,nrofpower,xp,yp,xs,ys;
    unsigned char *parea=mymap->pylonarea[playernr];
    if (!parea)
	parea = CreatePylonAreaArray(mymap,playernr);
    if (IsReadyOBJ(a) && !(a->modemove & MODEDIE) )
	if (RequiresPylonEnergy(a->SC_Unit))
	{
    	    nrofpower = 0;
	    xp = (GetOBJx(a) - GetUnitWidthAndHeight(a->SC_Unit,UNITDIM_WIDTH)/2)/SIZESPRLANSHX;
	    yp = (GetOBJy(a) - GetUnitWidthAndHeight(a->SC_Unit,UNITDIM_HEIGHT)/2)/SIZESPRLANSHY;
	    xs = GetUnitWidthAndHeight(a->SC_Unit,UNITDIM_WIDTH)/SIZESPRLANSHX;
	    ys = GetUnitWidthAndHeight(a->SC_Unit,UNITDIM_HEIGHT)/SIZESPRLANSHY;
    	    for (i=0;i<ys;i++)
    	        for (j=0;j<xs;j++)
    		    if (parea[(i+yp)*MAXXMAP+j+xp])
		        nrofpower++;
    	    if (nrofpower < xs*ys/2)
	    {
		moveobj(a,NULL,MODEPOWEROFF,NOSHOWERROR);
	    }
	    else
	    {
		moveobj(a,NULL,MODEPOWERON,NOSHOWERROR);
	    }
	}
}
//=================================
void CheckProtossBuildsForPower(struct mapinfo *mymap)
{
    int i,pylonarray;
    unsigned char *parea;
    for (i=0;i<PLAYEDPLAYERS;i++)
    {
	parea=mymap->pylonarea[i];
	if (!parea)
	    parea=CreatePylonAreaArray(mymap,i);
	DelBuildsFromPower(parea,i);
    }
}
//=================================

