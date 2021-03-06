
#include "wmem.h"
#include "audio.h"

#include "defs.h"
#include "vars.h"
#include "load.h"
#include "putobj.h"
#include "ObjChecks.h"
#include "gr.h"
#include "lists.h"
#include "market.h"
#include "overlay.h"
#include "objs.h"
#include "images.h"
#include "gener.h"
#include "man.h"
#include "Enumerate.h"
#include "pylon.h"

//=================================
unsigned char PylonTable[PYLONAREAY][PYLONAREAX] =
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
void FreePylonArea(StarMapInfo *mymap)
{
	int i;
	for (i = 0;i < MAXPLAYERS;i++)
	{
		if (mymap->pylonarea[i])
		{
			wfree(mymap->pylonarea[i]);
			mymap->pylonarea[i] = NULL;
		}
	}
}
//=================================
unsigned char *CreatePylonAreaArray(StarMapInfo *mymap, int playernr)
{
	unsigned char *parea;
	parea = (unsigned char*)wmalloc(mymap->map_width*mymap->map_height);
	memset(parea, 0x00, mymap->map_width*mymap->map_height);
	mymap->pylonarea[playernr] = parea;
	return(parea);
}
//=================================
void AddPylonArea(StarMapInfo *mymap, OBJ *a, int playernr)
{
	int xp, yp, i, j, offset, x, y;
	unsigned char *parea = mymap->pylonarea[playernr];
	if (!parea)
		parea = CreatePylonAreaArray(mymap, playernr);
	xp = GetOBJx(a) / SIZESPRLANSHX - PYLONAREAX / 2;
	yp = GetOBJy(a) / SIZESPRLANSHY - PYLONAREAY / 2;
	for (i = 0;i < PYLONAREAY;i++)
	{
		y = yp + i;
		if (y < 0)
			continue;
		if (y >= MAXYMAP)
			break;
		offset = y * MAXXMAP;
		for (j = 0;j < PYLONAREAX;j++)
		{
			x = xp + j;
			if (x < 0)
				continue;
			if (x >= MAXXMAP)
				break;
			parea[offset + x] += PylonTable[i][j];
		}
	}
	AddBuildsInPower(parea, playernr);
}
//=================================
void DelPylonArea(StarMapInfo *mymap, OBJ *a, int playernr)
{
	int xp, yp, i, j, offset, x, y;
	unsigned char *parea = mymap->pylonarea[playernr];
	if (!parea)
		parea = CreatePylonAreaArray(mymap, playernr);
	xp = GetOBJx(a) / SIZESPRLANSHX - PYLONAREAX / 2;
	yp = GetOBJy(a) / SIZESPRLANSHY - PYLONAREAY / 2;
	for (i = 0;i < PYLONAREAY;i++)
	{
		y = yp + i;
		if (y < 0)
			continue;
		if (y >= MAXYMAP)
			break;
		offset = y * MAXXMAP;
		for (j = 0;j < PYLONAREAX;j++)
		{
			x = xp + j;
			if (x < 0)
				continue;
			if (x >= MAXXMAP)
				break;
			parea[offset + x] -= PylonTable[i][j];
		}
	}
	DelBuildsFromPower(parea, playernr);
}
//=================================
void AddBuildsInPower(unsigned char *pylonarea, int playernr)
{
	int i, j, nrofpower, xp, yp, xs, ys;
	OBJ *o;
	Enumerate<OBJ *> EnumObj(&MaxObjects, objects);
	while( (o = EnumObj.GetNext()) )
	{
		if (o->playernr != playernr)
			continue;
		if (o->prop & VARPOWEROFF)
		{
			if (RequiresPylonEnergy(o->SC_Unit))
			{
				nrofpower = 0;
				xp = (GetOBJx(o) - GetUnitWidthAndHeight(o->SC_Unit, UNITDIM_WIDTH) / 2) / SIZESPRLANSHX;
				yp = (GetOBJy(o) - GetUnitWidthAndHeight(o->SC_Unit, UNITDIM_HEIGHT) / 2) / SIZESPRLANSHY;
				xs = GetUnitWidthAndHeight(o->SC_Unit, UNITDIM_WIDTH) / SIZESPRLANSHX;
				ys = GetUnitWidthAndHeight(o->SC_Unit, UNITDIM_HEIGHT) / SIZESPRLANSHY;
				for (i = 0;i < ys;i++)
					for (j = 0;j < xs;j++)
						if (pylonarea[(i + yp)*MAXXMAP + j + xp])
							nrofpower++;
				if (nrofpower >= xs * ys / 2)
				{
					moveobj(o, NULL, MODEPOWERON, NOSHOWERROR);
				}
			}
		}
	}
}
//=================================
void DelBuildsFromPower(unsigned char *pylonarea, int playernr)
{
	int i, j, nrofpower, xp, yp, xs, ys;
	OBJ *o;
	Enumerate<OBJ *> EnumObj(&MaxObjects, objects);
	while( (o = EnumObj.GetNext()) )
	{
		if (o->playernr != playernr)
			continue;
		if (!(o->prop & VARPOWEROFF) && o->modemove != MODEDIE)
		{
			if (!IsOBJUnderConstruct(o))
				if (RequiresPylonEnergy(o->SC_Unit))
				{
					nrofpower = 0;
					xp = (GetOBJx(o) - GetUnitWidthAndHeight(o->SC_Unit, UNITDIM_WIDTH) / 2) / SIZESPRLANSHX;
					yp = (GetOBJy(o) - GetUnitWidthAndHeight(o->SC_Unit, UNITDIM_HEIGHT) / 2) / SIZESPRLANSHY;
					xs = GetUnitWidthAndHeight(o->SC_Unit, UNITDIM_WIDTH) / SIZESPRLANSHX;
					ys = GetUnitWidthAndHeight(o->SC_Unit, UNITDIM_HEIGHT) / SIZESPRLANSHY;
					for (i = 0;i < ys;i++)
						for (j = 0;j < xs;j++)
							if (pylonarea[(i + yp)*MAXXMAP + j + xp])
								nrofpower++;
					if (nrofpower < xs*ys / 2)
					{
						if (gameconf.audioconf.buildsounds && o->playernr == NUMBGAMER)
							Play_sfxdata_id(o, sfx_powerdown[PROTOSSRACE], 2, 0);
						moveobj(o, NULL, MODEPOWEROFF, NOSHOWERROR);
					}
				}
		}
	}
}
//=================================
void CheckBuildForPower(StarMapInfo *mymap, OBJ *a, int playernr)
{
	int i, j, nrofpower, xp, yp, xs, ys;
	unsigned char *parea = mymap->pylonarea[playernr];
	if (!parea)
		parea = CreatePylonAreaArray(mymap, playernr);
	if (IsReadyOBJ(a) && !(a->modemove & MODEDIE))
		if (RequiresPylonEnergy(a->SC_Unit))
		{
			nrofpower = 0;
			xp = (GetOBJx(a) - GetUnitWidthAndHeight(a->SC_Unit, UNITDIM_WIDTH) / 2) / SIZESPRLANSHX;
			yp = (GetOBJy(a) - GetUnitWidthAndHeight(a->SC_Unit, UNITDIM_HEIGHT) / 2) / SIZESPRLANSHY;
			xs = GetUnitWidthAndHeight(a->SC_Unit, UNITDIM_WIDTH) / SIZESPRLANSHX;
			ys = GetUnitWidthAndHeight(a->SC_Unit, UNITDIM_HEIGHT) / SIZESPRLANSHY;
			for (i = 0;i < ys;i++)
				for (j = 0;j < xs;j++)
					if (parea[(i + yp)*MAXXMAP + j + xp])
						nrofpower++;
			if (nrofpower < xs*ys / 2)
			{
				moveobj(a, NULL, MODEPOWEROFF, NOSHOWERROR);
			}
			else
			{
				moveobj(a, NULL, MODEPOWERON, NOSHOWERROR);
			}
		}
}
//=================================
void CheckProtossBuildsForPower(StarMapInfo *mymap)
{
	int i;
	unsigned char *parea;
	for (i = 0;i < PLAYEDPLAYERS;i++)
	{
		parea = mymap->pylonarea[i];
		if (!parea)
			parea = CreatePylonAreaArray(mymap, i);
		DelBuildsFromPower(parea, i);
	}
}
//=================================

