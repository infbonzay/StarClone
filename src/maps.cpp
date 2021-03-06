#include <grplib/gr8.h>
#include <grplib/usegrp.h>

#include "lists.h"
#include "auxil.h"
#include "man.h"
#include "doodad.h"
#include "gr.h"
#include "load.h"
#include "mage.h"
#include "vars.h"
#include "diap.h"
#include "gr.h"
#include "objs.h"
#include "tempobj.h"
#include "starmap.h"
#include "creep.h"
#include "action.h"
#include "ObjChecks.h"
#include "vision.h"
#include "fog.h"
#include "gener.h"
#include "market.h"
#include "images.h"
#include "walk.h"
#include "man.h"
#include "Enumerate.h"
#include "maps.h"

StarMapInfo map;
float factorx, factory;
int sizemaprectx, sizemaprecty;
ScreenMapInfo *screenMapInfo;
RevealMap *revealMap;
//==============================
void CreateMiniMapPixels(StarMapInfo *map)
{
	MAXXMAP = map->map_width;
	MAXYMAP = map->map_height;
	MAXXMAP8 = map->map_width * 4;
	MAXYMAP8 = map->map_height * 4;
	screenMapInfo->MinimapStartX = map->minimap_startx;
	screenMapInfo->MinimapStartY = map->minimap_starty;
	screenMapInfo->MinimapEndX = map->minimap_endx;
	screenMapInfo->MinimapEndY = map->minimap_endy;
	highMouse->SetRestrictCoords(MOUSEMODE4, screenMapInfo->MinimapPosX + screenMapInfo->MinimapStartX, screenMapInfo->MinimapPosY + screenMapInfo->MinimapStartY, screenMapInfo->MinimapPosX + screenMapInfo->MinimapEndX, screenMapInfo->MinimapPosY + screenMapInfo->MinimapEndY);
	factorx = (float)((screenMapInfo->MinimapEndX - screenMapInfo->MinimapStartX)) / MAXXMAP;
	factory = (float)((screenMapInfo->MinimapEndY - screenMapInfo->MinimapStartY)) / MAXYMAP;
	sizemaprectx = (int)(screenMapInfo->SizeX32*factorx);
	sizemaprecty = (int)(screenMapInfo->SizeY32*factory);
}
//==============================
//x32,y32 - coords in map 128x128 for example,
//sizex,sizey - size of object 1 - is 32 pixels
//==============================
void GetMiniMapCoord(int x32, int y32,
	int sizex, int sizey,
	int *xminimap, int *yminimap,
	int *sizexinpixels, int *sizeyinpixels)
{
	*xminimap = screenMapInfo->MinimapPosX + screenMapInfo->MinimapStartX + (int)(x32*factorx);
	*yminimap = screenMapInfo->MinimapPosY + screenMapInfo->MinimapStartY + (int)(y32*factory);
	*sizexinpixels = (int)(sizex*factorx + 0.5);
	*sizeyinpixels = (int)(sizey*factory + 0.5);
}
//==============================
void minimap_showobj(OBJ *a)
{
	int sx, sy;
	if (a->SC_Unit == SC_MAPREVEALEROBJ)
		return;
	if (!OBJ_VAR_CHK(a, obj_notdetect, NUMBGAMER) || (IsDoodadState(a->SC_Unit) && GetDoodadState(a) == DOODAD_TOP_STATE))
	{
		sx = GetUnitBoxWidth(a->SC_Unit);
		sy = GetUnitBoxHeight(a->SC_Unit);
		ObjOnMiniMap(a->xkart - sx / 64, a->ykart - sy / 64, sx, sy, tableforminimap[a->color], Minimap);
	}
}
//==============================================
void getminimapcoord(StarMapInfo *info, int x, int y, int *xm, int *ym)
{
	*xm = (int)((x*factorx / SIZESPRLANSHX) + screenMapInfo->MinimapStartX) + screenMapInfo->MinimapPosX;
	*ym = (int)((y*factory / SIZESPRLANSHY) + screenMapInfo->MinimapStartY) + screenMapInfo->MinimapPosY;
}
//==============================================
void ObjOnMiniMap(int xkart, int ykart, int sizex, int sizey, char color, char *minimapadr)
{
	int i, j, minix, miniy, pixx, pixy;
	pixx = (int)((sizex + 16) / 32 * factorx + 0.5);
	pixy = (int)((sizey + 16) / 32 * factory + 0.5);
	minix = (int)((xkart*factorx) + screenMapInfo->MinimapStartX);
	for (i = 0;i < pixy;i++)
	{
		miniy = (int)((ykart*factory) + screenMapInfo->MinimapStartY + i)*MAXIMSIZEMINIMAP;
		for (j = 0;j < pixx;j++)
		{
			minimapadr[miniy + minix + j] = color;
		}
	}
}
//==============================
void drawMINIMAP(void)
{
	int xbeg, ybeg, xend, yend;
	if (MAPDEF)
	{
		GRP_PutPartVisibleSpr(screenMapInfo->MinimapPosX, screenMapInfo->MinimapPosY, 128, 128, Minimap);
		GetMiniMapCoord(map.MAPX, map.MAPY, screenMapInfo->SizeX32, screenMapInfo->SizeY32, &xbeg, &ybeg, &xend, &yend);
		xend += xbeg;
		yend += ybeg;
		wrectangle(WHITECOLOR, xbeg, ybeg, xend, yend);
		if (screenMapInfo->MinimapStartY > 0)
			horizline(WHITECOLOR, screenMapInfo->MinimapStartX + screenMapInfo->MinimapPosX - 1, screenMapInfo->MinimapEndX + screenMapInfo->MinimapPosX + 1, screenMapInfo->MinimapStartY + screenMapInfo->MinimapPosY);
		if (screenMapInfo->MinimapEndY < 128)
			horizline(WHITECOLOR, screenMapInfo->MinimapStartX + screenMapInfo->MinimapPosX - 1, screenMapInfo->MinimapEndX + screenMapInfo->MinimapPosX + 1, screenMapInfo->MinimapEndY + screenMapInfo->MinimapPosY);
		if (screenMapInfo->MinimapStartX > 0)
			vertline(WHITECOLOR, screenMapInfo->MinimapStartX + screenMapInfo->MinimapPosX, screenMapInfo->MinimapStartY + screenMapInfo->MinimapPosY - 1, screenMapInfo->MinimapEndY + screenMapInfo->MinimapPosY + 1);
		if (screenMapInfo->MinimapEndX < 128)
			vertline(WHITECOLOR, screenMapInfo->MinimapEndX + screenMapInfo->MinimapPosX, screenMapInfo->MinimapStartY + screenMapInfo->MinimapPosY - 1, screenMapInfo->MinimapEndY + screenMapInfo->MinimapPosY + 1);
	}
}
//==================================
int MAPAROUND[8][2] = {
						{-1,-1},{ 0,-1},{ 1,-1},
						{-1, 0},		{ 1, 0},
						{-1, 1},{ 0, 1},{ 1, 1}
};

//==================================
//return 0 if around not open map
int checkaroundtile(int x, int y)
{
	int i, newx, newy, ret = 0;
	for (i = 0;i < 8;i++)
	{
		newx = x + MAPAROUND[i][0];
		newy = y + MAPAROUND[i][1];
		if (CHECKFORMAPBORDERS(newx, newy))
			continue;
		if (mapSEE(newx, newy) > 0 || mapOPEN(newx, newy) > 0)
			ret += 1;
	}
	return(ret);
}

//==================================
#define SHADOWFOROPENEDCELLMAP			0x10
#define SHADOWFORCLOSEDCELLMAP			0x08	//work only if visiblemap is set
//==================================

void saveoldtileandcreep(void)
{
	int i, j, nosave, indextile32;
	unsigned char wf, bf;
	signed char creepnr;
	for (i = 0;i < MAXYMAP;i++)
		for (j = 0;j < MAXXMAP;j++)
		{
			nosave = 0;
			wf = mapSEE(j, i);
			bf = mapOPEN(j, i);
			if (wf > 1 || bf > 1)//if opened map
			{
				indextile32 = map.display_map[i*MAXXMAP + j];
				creepnr = GetCreepAroundWithFog(j, i, indextile32);
				if (wf <= 0)//if we see it before,but now fog we put saved pictures
				{
					if (map.mapbits.savedtile[NUMBGAMER])
						if (map.mapbits.savedtile[NUMBGAMER][i*MAXXMAP + j] != 0xffff)
							indextile32 = map.mapbits.savedtile[NUMBGAMER][i*MAXXMAP + j];
					nosave = 1;
				}
				if (creepnr == -1)
				{
					if (!nosave)
						savemaptileadr(j, i, indextile32);
				}
				if (!nosave)
					savemapcreepadr(j, i, map.creepflagplace[i*MAXXMAP + j]);
			}
		}
}
//==================================
void drawMAP(int ignorefirstwaiting)
{
	unsigned char wf, bf;
	signed char creepnr;
	int i, j, indextile32, method, open, see;
	int bh, eh, bw, ew, ii, jj, xk, yk, xkk, ykk;
	unsigned char minimapcolor;
	if (!CODEFORSCREEN)
		GRP_ClrScr(0);
	else
	{
		if (ignorefirstwaiting)
		{
			for (i = 0;i < PLAYEDPLAYERS;i++)
			{
				if (IfPlayerHaveStartLocation(&map, i) != -1)
				{
					memset(map.mapbits.whitefog[i], 0x4, map.map_width*map.map_height);
					memset(map.mapbits.whitefog2[i], 0x4, map.map_width*map.map_height);
					memset(map.mapbits.blackfog[i], 0x4, map.map_width*map.map_height);
				}
			}
		}
	}
	bh = map.MAPY;
	eh = bh + screenMapInfo->SizeY32 + 1;
	if (eh > MAXYMAP)
		eh = MAXYMAP;
	bw = map.MAPX;
	ew = bw + screenMapInfo->SizeX32 + 1;
	if (ew > MAXXMAP)
		ew = MAXXMAP;
	xk = map.MAPXGLOBAL;
	yk = map.MAPYGLOBAL;
	xkk = xk % SIZESPRLANSHX;
	ykk = yk % SIZESPRLANSHY;
	memset(screenmapused, 0, (screenMapInfo->SizeY32 + 1)*(screenMapInfo->SizeX32 + 1));
	for (ii = 0, i = bh;i < eh;i++, ii++)
	{
		for (jj = 0, j = bw;j < ew;j++, jj++)
		{
			wf = mapSEE(j, i);
			bf = mapOPEN(j, i);
			if (wf > 1 || bf > 1)//if opened map
			{
				indextile32 = map.display_map[i*MAXXMAP + j];
				putlansh(jj*SIZESPRLANSHX - xkk, ii*SIZESPRLANSHY - ykk, j, i, indextile32, wf, bf);
				screenmapused[ii*(screenMapInfo->SizeX32 + 1) + jj] = 1;
			}
			else
				if (gameconf.videoconf.visiblemap || checkaroundtile(j, i))//if around not open we don't show tile
				{
					indextile32 = map.display_map[i*MAXXMAP + j];
					putlansh(jj*SIZESPRLANSHX - xkk, ii*SIZESPRLANSHY - ykk, j, i, indextile32, wf, bf);
					screenmapused[ii*(screenMapInfo->SizeX32 + 1) + jj] = 1;
				}
			if (SHOWCELLS)
			{
				if (mapEFFECT(j, i, DSWARMEFFECT))
					GRP_SetImage8x(jj*SIZESPRLANSHX - xkk, ii*SIZESPRLANSHY - ykk, 32, 32, 200);
				if (mapEFFECT(j, i, DISRUPTEFFECT))
					GRP_SetImage8x(jj*SIZESPRLANSHX - xkk, ii*SIZESPRLANSHY - ykk, 32, 32, 200);
			}
		}
	}
	//if time to regeniration minimap and need to put terrain
	if (MAPREGENERATIONBIT && (MAPDEF&(TERRAIN | UNITS)))
		setmemd(Minimap, MAXIMSIZEMINIMAP*MAXIMSIZEMINIMAP / 4, BLACKCOLOR);
	//drawminimap terrain with creep
	if ((MAPREGENERATIONBIT && (MAPDEF&TERRAIN)) || ignorefirstwaiting)
	{
		//if time to regeniration minimap
		for (i = screenMapInfo->MinimapStartY;i < screenMapInfo->MinimapEndY;i++)
		{
			ii = (int)((i - screenMapInfo->MinimapStartY) / factory);
			for (j = screenMapInfo->MinimapStartX;j < screenMapInfo->MinimapEndX;j++)
			{
				jj = (int)((j - screenMapInfo->MinimapStartX) / factorx);
				if (mapSEE(jj, ii) > 1)
					creepnr = map.creepflagplace[ii*MAXXMAP + jj];
				else
					if (map.mapbits.savedcreep[NUMBGAMER])
						creepnr = map.mapbits.savedcreep[NUMBGAMER][ii*MAXXMAP + jj];
					else
						creepnr = 0;
				if (map.mapbits.underfog[NUMBGAMER] &&
					map.mapbits.underfog[NUMBGAMER][ii*MAXXMAP + jj] &&
					(map.mapbits.underfog[NUMBGAMER][ii*MAXXMAP + jj]->nrofslots > 0))
				{
					if (map.mapbits.underfog[NUMBGAMER][ii*MAXXMAP + jj]->pictureslot[0].flags & UNDERFOG_MAINIMAGE)
					{
						minimapcolor = map.mapbits.underfog[NUMBGAMER][ii*MAXXMAP + jj]->pictureslot[0].grcolor;		//putobjonminimap
						if (!minimapcolor)
							continue;
					}
					else
						continue;
					//previous color
				}
				else
					if (creepnr == 0)
					{
						minimapcolor = map.minimap[i*MAXIMSIZEMINIMAP + j];
					}
					else
					{
						//if (creepnr>=MAXCREEPNR)
						//	minimapcolor = (map.creeptileminimapcolors[CREEPCOLOR-MAXCREEPNR]>>creepoffset)&0xff;
						//else
						minimapcolor = CREEPCOLOR;
					}
				if (mapSEE(jj, ii) > 1)
					//					if (minimapcolor)
					Minimap[i*MAXIMSIZEMINIMAP + j] = minimapcolor;
				else
					if (mapOPEN(jj, ii) > 1)
						Minimap[i*MAXIMSIZEMINIMAP + j] = _toblack_[minimapcolor + 256 * SHADOWFOROPENEDCELLMAP];
					else
						if (gameconf.videoconf.visiblemap)
							Minimap[i*MAXIMSIZEMINIMAP + j] = _toblack_[minimapcolor + 256 * SHADOWFORCLOSEDCELLMAP];
			}
		}
	}
	//put objects under fog if any
	for (ii = 0, i = bh - 2;i <= eh;i++, ii++)
	{
		if (i < 0)
			continue;
		if (i >= MAXYMAP)
			break;
		for (jj = 0, j = bw - 2;j <= ew;j++, jj++)
		{
			if (j < 0)
				continue;
			if (j >= MAXXMAP)
				break;

			see = mapSEE(j, i);
			open = mapOPEN(j, i);
			if (see <= 1 && open > 1)
				method = 0;
			else
				if (see <= 1 && open < 1)
					if (gameconf.videoconf.visiblemap)
						method = 2;
					else
						method = 1;
				else
					method = 1;
			switch (method)
			{
			case 0:
			case 2:
				if (map.mapbits.underfog[NUMBGAMER])
					if (map.mapbits.underfog[NUMBGAMER][i*MAXXMAP + j])
						if (map.mapbits.underfog[NUMBGAMER][i*MAXXMAP + j]->nrofslots)
							loadandputimage(i*MAXXMAP + j);
				break;
			case 1:
				if (map.mapbits.underfog[NUMBGAMER])
					if (map.mapbits.underfog[NUMBGAMER][i*MAXXMAP + j])
					{
						map.mapbits.underfog[NUMBGAMER][i*MAXXMAP + j]->nrofslots = 0;
						//							wfree(map.mapbits.underfog[NUMBGAMER][i*MAXXMAP+j]);
						//							map.mapbits.underfog[NUMBGAMER][i*MAXXMAP+j]=NULL;
					}
				break;
			}
		}
	}

	/*	  FILE *f=fopen("screenmapused.txt","wb");
		if (f)
		{
			for (i=0;i<screenMapInfo->SizeY32+1;i++)
			{
				for (j=0;j<screenMapInfo->SizeX32+1;j++)
				{
					fprintf(f,"%02d ",screenmapused[i*(screenMapInfo->SizeX32+1)+j]);
				}
				fprintf(f,"\n");
			}
			fclose(f);
		}
	*/
}
//==================================
#define MAPMOVETIMES			4
void AutoMoveMap(void)
{
	if (map.flags & STARMAP_FLAG_AUTOMOVE)
	{
		int mapmovedone = 0;
		map.flags |= STARMAP_FLAG_MAPMOVE;
		map.totaldeltax -= map.deltax;
		map.totaldeltay -= map.deltay;
		//		if (!mapmovedone)
		if (map.deltax <= 0)
		{
			if (map.totaldeltax >= 0)
				mapmovedone = 1;
		}
		else
			if (map.deltax >= 0)
			{
				if (map.totaldeltax <= 0)
					mapmovedone = 1;
			}
		if (!mapmovedone)
		{
			if (map.deltay <= 0)
			{
				if (map.totaldeltay >= 0)
					mapmovedone = 1;
			}
			else
				if (map.deltay >= 0)
				{
					if (map.totaldeltay <= 0)
						mapmovedone = 1;
				}
		}
		if (mapmovedone)
		{
			//finish automove
			map.flags &= ~STARMAP_FLAG_AUTOMOVE;
			map.MAPX = map.newx / SIZESPRLANSHX;
			map.MAPY = map.newy / SIZESPRLANSHY;
			map.MAPXGLOBAL = map.MAPX * SIZESPRLANSHX;
			map.MAPYGLOBAL = map.MAPY * SIZESPRLANSHY;
		}
		else
		{
			map.MAPXGLOBAL += map.deltax;
			map.MAPYGLOBAL += map.deltay;
			map.MAPX = map.MAPXGLOBAL / SIZESPRLANSHX;
			map.MAPY = map.MAPYGLOBAL / SIZESPRLANSHY;
		}
	}
}
//==================================
void MoveVisualMapPosition(int x, int y)
{
	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;
	if (x > SIZESPRLANSHX*(MAXXMAP - screenMapInfo->SizeX32))
		x = SIZESPRLANSHX * (MAXXMAP - screenMapInfo->SizeX32);
	if (y > SIZESPRLANSHY*(MAXYMAP - screenMapInfo->SizeY32))
		y = SIZESPRLANSHY * (MAXYMAP - screenMapInfo->SizeY32);
	map.newx = x;
	map.newy = y;
	map.totaldeltax = x - map.MAPXGLOBAL;
	map.totaldeltay = y - map.MAPYGLOBAL;
	map.deltax = map.totaldeltax / MAPMOVETIMES;
	map.deltay = map.totaldeltay / MAPMOVETIMES;
	if (map.totaldeltax || map.totaldeltay)
	{
		map.flags |= STARMAP_FLAG_AUTOMOVE;
	}
}
//==================================
void MoveVisualMapPositionCenter(int x, int y)
{
	MoveVisualMapPosition(x - screenMapInfo->SizeX32*SIZESPRLANSHX / 2, y - screenMapInfo->SizeY32*SIZESPRLANSHY / 2 - SIZESPRLANSHY);
}
//==================================
bool SetVisualMapPosition(int x, int y)//x,y-0..MAX?MAP*32
{
	bool retval = false;
	if (map.flags & STARMAP_FLAG_AUTOMOVE)
		return(0);
	x &= ~0x1f;
	y &= ~0x1f;
	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;
	if (x > SIZESPRLANSHX*(MAXXMAP - screenMapInfo->SizeX32))
		x = SIZESPRLANSHX * (MAXXMAP - screenMapInfo->SizeX32);
	if (y > SIZESPRLANSHY*(MAXYMAP - screenMapInfo->SizeY32))
		y = SIZESPRLANSHY * (MAXYMAP - screenMapInfo->SizeY32);
	map.MAPXGLOBAL = x;
	map.MAPYGLOBAL = y;
	if (map.MAPX != x / SIZESPRLANSHX || map.MAPY != y / SIZESPRLANSHX)
	{
		map.flags |= STARMAP_FLAG_MAPMOVE;
		retval = true;
		map.MAPX = x / SIZESPRLANSHX;
		map.MAPY = y / SIZESPRLANSHY;
		map.newx = x;
		map.newy = y;
	}
	return(retval);
}
//==================================
void SetVisualMapPositionCenter(int x, int y)
{
	SetVisualMapPosition(x - screenMapInfo->SizeX32*SIZESPRLANSHX / 2, y - screenMapInfo->SizeY32*SIZESPRLANSHY / 2 - SIZESPRLANSHY);
}
//==================================
void putfullcreep(int xglob, int yglob, int x, int y, int creepnrspr)
{
	char *adrspr = map.creeptiles + creepnrspr * 32 * 32;
	GRP_PutRow32x32(xglob, yglob, adrspr);
}
//==================================
//save tile for later use
void savemaptileadr(int x, int y, int indextile32)
{
	if (map.mapbits.savedtile[NUMBGAMER])
		map.mapbits.savedtile[NUMBGAMER][y*MAXXMAP + x] = indextile32;
}
//==================================
//save creepnr for later use
void savemapcreepadr(int x, int y, int creepnr)
{
	if (map.mapbits.savedcreep[NUMBGAMER])
		map.mapbits.savedcreep[NUMBGAMER][y*MAXXMAP + x] = creepnr;
}
//==================================
//risovati lanshaft xglob,yglob (0-639,0-479) x,y 0-maxxmap 0-maxymap
void putlansh(int xglob, int yglob, int x, int y, int indextile32, char wfog, char bfog)
{
	signed char creepnr;
	//	  if (indextile32==1)
	//		return;
	char *adrspr = &map.tiles[indextile32 * 32 * 32];
	int nosave = 0;
	creepnr = GetCreepAroundWithFog(x, y, indextile32);		//get creep picture with fog and without fog
	if (wfog <= 0)//if we see it before,but now fog we put saved pictures
	{
		if (map.mapbits.savedtile[NUMBGAMER])
			if (map.mapbits.savedtile[NUMBGAMER][y*MAXXMAP + x] != 0xffff)
				indextile32 = map.mapbits.savedtile[NUMBGAMER][y*MAXXMAP + x];//get previous tile
		nosave = 1;
	}
	if (creepnr == -1)
	{
		GRP_PutRow32x32(xglob, yglob, adrspr);
		if (!nosave)
			savemaptileadr(x, y, indextile32);
		/*		struct XY a;
				for (int i=0;i<4;i++)
					for (int j=0;j<4;j++)
					{
						a.x1 = xglob + j*8;
						a.y1 = yglob + i*8;
						a.x2 = a.x1 + 8;
						a.y2 = a.y1 + 8;
						if (!GetMapWalk8(x*4+j,y*4+i,1,1))
							wsetimage8x(DISABLECONSTRUCT,&a);
		//					wrectangle(DISABLECONSTRUCT,a.x1,a.y1,a.x2,a.y2);
					}
		*/
	}
	else
	{
		if (creepnr < MAXCREEPNR)
		{
			GRP_PutRow32x32(xglob, yglob, adrspr);
			GRP_PutGrp(xglob, yglob, creepgrp, NORMAL, 255, 0, NULL, creepnr);
			//			putgrp(xglob,yglob,creepgrp,creepnr,0);
		}
		else
		{
			putfullcreep(xglob, yglob, x, y, creepnr - MAXCREEPNR);
		}
	}
	if (!nosave)
		savemapcreepadr(x, y, map.creepflagplace[y*MAXXMAP + x]);

}
//==================================
void clearopenseeKarta(void)
{
	int i;
	memcpy(map.mapbits.seedetector, map.mapbits.seedetector2, MAXXMAP*MAXYMAP);
	memcpy(map.mapbits.arbiterfield, map.mapbits.arbiterfield2, MAXXMAP*MAXYMAP);


	for (i = 0;i < PLAYEDPLAYERS;i++)
	{
		if (IfPlayerHaveStartLocation(&map, i) != -1)
		{
			memcpy(map.mapbits.whitefog[i], map.mapbits.whitefog2[i], MAXXMAP*MAXYMAP);
		}
	}
	memset(map.mapbits.seedetector2, 0, MAXXMAP*MAXYMAP);
	memset(map.mapbits.arbiterfield2, 0, MAXXMAP*MAXYMAP);
	if (CODEFORSCREEN)
	{
	}
	else
	{
		for (i = 0;i < PLAYEDPLAYERS;i++)
		{
			if (IfPlayerHaveStartLocation(&map, i) != -1)
			{
				memset(map.mapbits.whitefog2[i], 0, MAXXMAP*MAXYMAP);
			}
		}
	}
}
//=================================
char xypos[2][4] = { {0,1,0,1},{0,0,1,1} };
//=================================
void arbitermakewarpfield(OBJ *a, OBJstruct *b)
{
	int g, x, y, mybits, xcenter, ycenter, see, objxysize;
	signed char vis;
	int elemnr;
	MAPOFFSETELEMENT *cmdoffs;

	if (a->SC_Unit != SC_ARBITEROBJ && a->SC_Unit != SC_HERO_DANIMOTHOBJ)
		return;
	if (a->prop&VARNOTWORK)
		return;
	if (IsHallucination(a))
		return;
	if (PLAYER[a->playernr].isobserverflag)
		return;
	see = GetUnitCloakRange(b);
	objxysize = GetRangeObjSize(a->SC_Unit, NULL, NULL);

	xcenter = a->xkart - xypos[0][objxysize];
	ycenter = a->ykart - xypos[1][objxysize];
	//	  xcenter=a->xkart;
	//	  ycenter=a->ykart;
	mybits = 1 << a->playernr;

	for (g = 0;g < MAXANGLES;g++)
	{
		elemnr = mapvisiontables[objxysize]->offsetelemnr[g];
		cmdoffs = &mapvisiontables[objxysize]->mapelement[elemnr];
		for (;;cmdoffs++)
		{
			vis = cmdoffs->rangevision;
			if (!vis || vis > see)
				break;
			y = ycenter + cmdoffs->yoffset;
			if (y < 0)
				continue;
			if (y >= MAXYMAP)
				continue;
			x = xcenter + cmdoffs->xoffset;
			if (x < 0)
				continue;
			if (x >= MAXXMAP)
				continue;
			map.mapbits.arbiterfield[y*MAXXMAP + x] |= mybits;
			map.mapbits.arbiterfield2[y*MAXXMAP + x] |= mybits;
		}
	}
}
//=================================
void makeoneobjseeopen(OBJ *a, OBJstruct *b)
{
	int x, y, opendelta, mapchanges = 0, objxysize;
	int g, pp, see, prst = 0, opn, onsky = 0;
	int xcenter, ycenter, level;

	int nosave, indextile32;
	signed char creepnr, vis;

	int elemnr;
	MAPOFFSETELEMENT *cmdoffs;

	if (a->prop & VARKARTCHANGES)
		a->prop &= ~VARKARTCHANGES;
	else
		return;
	if ((a->prop & VARNOTHERE) && GetMageAtr(&a->atrobj, ATRSTASIS) == 0)
		return;
	if (a->in_transport)//in buncker/transport
		return;
	if (IsSubUnit(a->SC_Unit))
		return;			//subunit not reveal the map
	if (a->playernr >= PLAYEDPLAYERS)			//this is neutral unit
		return;
	if (!IsOBJUnderConstruct(a))
	{
		prst = GetMageAtr(&a->atrobj, ATRPARASITEFROM);
		see = GetUnitSightRange(a, b) + 2;
	}
	else
		see = SEERANGEIFNOTREADY;
	if (IsOnSkyOBJ(a))
		onsky = 1;
	objxysize = GetRangeObjSize(a->SC_Unit, NULL, NULL);

	xcenter = a->xkart - xypos[0][objxysize];
	ycenter = a->ykart - xypos[1][objxysize];

	level = map.maplevel[a->ykart*MAXXMAP + a->xkart];
	opn = GetVisionBitsPlayer(a->playernr);
	if (!opn && !prst)
		return;
	for (g = 0;g < MAXANGLES;g++)
	{
		elemnr = mapvisiontables[objxysize]->offsetelemnr[g];
		cmdoffs = &mapvisiontables[objxysize]->mapelement[elemnr];
		for (;;cmdoffs++)
		{
			vis = cmdoffs->rangevision;
			if (!vis)
				break;
			y = ycenter + cmdoffs->yoffset;
			if (y < 0)
				continue;
			if (y >= MAXYMAP)
				continue;
			x = xcenter + cmdoffs->xoffset;
			if (x < 0)
				continue;
			if (x >= MAXXMAP)
				continue;
			opendelta = see - vis + 1;
			if (opendelta < 1)
				break;
			if (opendelta > 4)
				opendelta = 4;
			if (!onsky)
				if (map.maplevel[y*MAXXMAP + x] > level)
					break;
			if (!CODEFORSCREEN)
			{
				pp = a->playernr;
				if (opn&(1 << pp))
				{
					if (mapSEE(x, y, pp) < opendelta)
					{
						map.mapbits.whitefog[pp][y*MAXXMAP + x] = opendelta;
						mapchanges = 1;
					}
					if (mapSEE2(x, y, pp) < opendelta)
					{
						map.mapbits.whitefog2[pp][y*MAXXMAP + x] = opendelta;
						mapchanges = 1;
					}
				}
				if (prst)
				{
					for (pp = 0;pp < PLAYEDPLAYERS;pp++)
					{
						if (prst&(1 << pp))
						{
							if (mapSEE(x, y, pp) < opendelta)
							{
								map.mapbits.whitefog[pp][y*MAXXMAP + x] = opendelta;
								mapchanges = 1;
							}
							if (mapSEE2(x, y, pp) < opendelta)
							{
								map.mapbits.whitefog2[pp][y*MAXXMAP + x] = opendelta;
								mapchanges = 1;
							}
						}
					}
				}
				if (opendelta == 1)
					continue;
				a->visibleby = opn;
			}//codeforscreen
			if (IsDetector(a->SC_Unit) && GetMageAtr(&a->atrobj, ATRBLIND) == 0)
			{
				if ((!(a->prop & VARNOTWORK)) && !IsOBJUnderConstruct(a) && !(a->prop & VARPOWEROFF))//esli dvigaetsea
				{
					if (opn)//esli alienseobj|parasite
					{
						map.mapbits.seedetector2[y*MAXXMAP + x] |= opn;
						map.mapbits.seedetector[y*MAXXMAP + x] |= opn;
					}
				}
			}
			nosave = 0;
			if (mapchanges && a->playernr == NUMBGAMER)
			{
				indextile32 = map.display_map[y*MAXXMAP + x];
				creepnr = GetCreepAroundWithFog(x, y, indextile32);
				if (creepnr == -1)
				{
					savemaptileadr(x, y, indextile32);
				}
				savemapcreepadr(x, y, map.creepflagplace[y*MAXXMAP + x]);
			}
		}//for
	}//for g
	if (mapchanges)
		map.clearfog[a->playernr] = 1;	//TODO may be need to open who see the playernr???
}
//=================================
void opentempmap(int playernr, int xkart, int ykart, int sizex, int sizey)
{
	int i, j, fromx, fromy, tox, toy;
	if (playernr >= PLAYEDPLAYERS || !map.mapbits.whitefog[playernr])
		return;
	fromx = xkart - sizex / 2;
	if (fromx < 0)
		fromx = 0;
	fromy = ykart - sizey / 2;
	if (fromy < 0)
		fromy = 0;
	tox = xkart + sizex / 2;
	if (tox >= MAXXMAP)
		tox = MAXXMAP - 1;
	toy = ykart + sizey / 2;
	if (toy >= MAXYMAP)
		toy = MAXYMAP - 1;
	for (i = fromy;i <= toy;i++)
		for (j = fromx;j <= tox;j++)
		{
			map.mapbits.whitefog[playernr][i*MAXXMAP + j] = 4;
			map.mapbits.whitefog2[playernr][i*MAXXMAP + j] = 4;
		}
	map.clearfog[playernr] = 1;
}
//=================================
void calcfullinvandsee(void)
{
	OBJ *o;
	Enumerate<OBJ *> EnumObj(&MaxObjects, objects);
	while( (o = EnumObj.GetNext()) )
	{
		calcfullinvandseeobj(o);
	}
}
//=================================
void calcfullinvandseeobj(OBJ *a)
{
	int i, decloaked, cloaked;
	unsigned char visbits, plmask;
	OBJ_VAR_MASK_CLR(a, obj_invsee, 0xff);
	OBJ_VAR_MASK_CLR(a, obj_see, 0xff);
	if (IsCloaked(a) || IsOBJBurrowed(a))
	{
		cloaked = 1;
		OBJ_VAR_MASK_SET(a, obj_notdetect, 0xff);
		decloaked = existatrdecloak(a);
		if (decloaked)
		{
			OBJ_VAR_MASK_SET(a, obj_invsee, 0xff);
			cloaked = 0;
		}
	}
	else
	{
		cloaked = 0;
		OBJ_VAR_MASK_CLR(a, obj_notdetect, 0xff);
	}
	for (i = 0, plmask = 1;i < PLAYEDPLAYERS;i++, plmask <<= 1)
	{
		if (map.pl_vision[i][a->playernr] ||				//if player i see OBJ or OBJaliance or OBJparasite
			player_aliance(i, a->playernr) == ALLIANCEOBJ ||
			existparasitebit(a, i))
		{
			OBJ_VAR_MASK_SET(a, obj_invsee, plmask);
			OBJ_VAR_MASK_SET(a, obj_see, plmask);
			OBJ_VAR_MASK_CLR(a, obj_notdetect, plmask);
		}
		else
		{
			if (mapSEE(a->xkart, a->ykart, i) > 1)
				OBJ_VAR_MASK_SET(a, obj_see, plmask);
			visbits = GetVisionBitsPlayer(i);
			if (map.mapbits.seedetector[a->ykart*MAXXMAP + a->xkart] & visbits)
			{
				//esli vidim detectorom
				OBJ_VAR_MASK_SET(a, obj_invsee, plmask);
				OBJ_VAR_MASK_CLR(a, obj_notdetect, plmask);
			}
		}
	}//for
	if (cloaked)
		ClearFinalOBJ(a);
}
//==================================
void ClearFinalOBJ(OBJ *a)
{
	OBJ *o;
	int i = 0;
	Enumerate<OBJ *> EnumObj(&MaxObjects, objects);
	while( (o = EnumObj.GetNext()) )
	{
		if (a == o->finalOBJ)
		{
			if (OBJ_VAR_CHK(a, obj_notdetect, o->playernr))
				moveobj(o, NULL, MODESTOP, NOSHOWERROR);
		}
	}
}
//==================================
void _putcells(void)
{
	int i, j, xkk, ykk;
	xkk = map.MAPXGLOBAL%SIZESPRLANSHX;
	ykk = map.MAPYGLOBAL%SIZESPRLANSHY;
	for (i = 0;i < screenMapInfo->SizeY32 + 1;i++)
	{
		horizline(245, GRP_wminx, GRP_wmaxx, i*SIZESPRLANSHY - ykk);
	}
	for (j = 0;j < screenMapInfo->SizeX32 + 1;j++)
	{
		vertline(245, j*SIZESPRLANSHX - xkk, GRP_wminy, GRP_wmaxy);
	}
}
//==================================
int CorrectRaceType(int ret, int playernr)
{
	switch (ret)
	{
	case RACE_ZERG:
	case RACE_TERRAN:
	case RACE_PROTOSS:
		return(ret);
	case RACE_INDEPENDENT:
	case RACE_USER_SELECT:
		return(gameconf.pl_race[playernr]);
	default:
		return(RACE_TERRAN);
	}
}
//==================================
int getplayerrace(StarMapInfo *map, int playernr)
{
	int ret, i;
	ret = CorrectRaceType(map->pl_race[playernr], playernr);
	for (i = 0;i < 3;i++)
	{
		xicon[i] = iconxcoord[ret][i];
		yicon[i] = iconycoord[ret][i];
	}
	return ret;
}
//=============================================
int mapSEE(OBJ *a, int player)
{
	if (!map.mapbits.whitefog[player])
		return(0);
	return(map.mapbits.whitefog[player][a->ykart*MAXXMAP + a->xkart]);
}
//=============================================
int mapSEE(int xkart, int ykart, int player)
{
	if (!map.mapbits.whitefog[player])
		return 0;
	return(map.mapbits.whitefog[player][ykart*MAXXMAP + xkart]);
}
//=============================================
int mapSEE2(int xkart, int ykart, int player)
{
	if (!map.mapbits.whitefog2[player])
		return 0;
	return(map.mapbits.whitefog2[player][ykart*MAXXMAP + xkart]);
}
//=============================================
int mapOPEN(OBJ *a, int player)
{
	if (!map.mapbits.blackfog[player])
		return 0;
	return(map.mapbits.blackfog[player][a->ykart*MAXXMAP + a->xkart]);
}
//=============================================
int mapOPEN(int xkart, int ykart, int player)
{
	if (!map.mapbits.blackfog[player])
		return 0;
	return(map.mapbits.blackfog[player][ykart*MAXXMAP + xkart]);
}
//=============================================
int mapSEE(int xkart, int ykart)
{
	int i;
	unsigned char fogvalue = 0;
	for (i = 0;i < PLAYEDPLAYERS;i++)
	{
		if (IfPlayerHaveStartLocation(&map, i) != -1)
		{
			if (player_vision(NUMBGAMER, i))
				if (fogvalue < map.mapbits.whitefog[i][ykart*MAXXMAP + xkart])
					fogvalue = map.mapbits.whitefog[i][ykart*MAXXMAP + xkart];
		}
	}
	return(fogvalue);
}
//=============================================
int mapOPEN(int xkart, int ykart)
{
	int i;
	unsigned char fogvalue = 0;
	for (i = 0;i < PLAYEDPLAYERS;i++)
	{
		if (IfPlayerHaveStartLocation(&map, i) != -1)
		{
			if (player_vision(NUMBGAMER, i))
				if (fogvalue < map.mapbits.blackfog[i][ykart*MAXXMAP + xkart])
					fogvalue = map.mapbits.blackfog[i][ykart*MAXXMAP + xkart];
		}
	}
	return(fogvalue);
}
//=============================================
int CreateAlianceBitsPlayer(int playernr)
{
	int i, allbits = 0;
	for (i = 0;i < PLAYEDPLAYERS;i++)
	{
		if (IfPlayerHaveStartLocation(&map, i) != -1)
		{
			if (player_aliance(playernr, i) == PLAYER_VISION)
				allbits |= (1 << i);
		}
	}
	return(allbits);
}
//================================================
int GetMapFog(int x, int y)
{
	if (CHECKFORMAPBORDERS(x / 32, y / 32))
		return(0);
	if (mapSEE(x / 32, y / 32) > 1)
		return(2);
	else
		if (mapOPEN(x / 32, y / 32) > 1)
			return(1);
		else
			return(0);
}
//==============================================
void RevealMap::AdditionalOpen(void)
{
	OBJ *o;
	OBJstruct *b;
	EnumListInit();
	while ((o = GetNextListElem(NULL)))
	{
		b = loadobj(o->SC_Unit);
		makeoneobjseeopen(o, b);
	}
	_Count = 0;
}
//=====================================
void RevealMap::AddObj(OBJ *o)
{
//	if (player_aliance(o->playernr, NUMBGAMER) == MYOBJ || player_aliance(o->playernr, NUMBGAMER) == ALLIANCEOBJ)
	{
		revealMap->Add(o);
	}
}
//=====================================
void TryScrollMap(void )
{
//	DEBUGMESSCR("refresh\n");	
}	


