
#include "lists.h"

#include "defs.h"
#include "auxil.h"
#include "vars.h"
#include "maps.h"
#include "starmap.h"
#include "load.h"
#include "action.h"
#include "images.h"
#include "objinfo.h"
#include "walk.h"
//=====================================
//=====================================
//return 1- if posible to construct , 0 - no posible to construct
int CanBuildOnPlace(int xb, int yb, int xsize, int ysize, unsigned short *returnedarray,
	char *ignorecells, SCUNIT SC_Unit, int playernr, int *error)
{
	int constrerror = 0;
	OBJ *a, *a2;
	int ret = 1, ret2;
	int ass_extr_ref[4] = { SC_ASSIMILATOROBJ,SC_EXTRACTOROBJ,SC_REFINERYOBJ,SC_GEYSEROBJ };
	memset(returnedarray, 0xff, xsize*ysize * sizeof(unsigned short));
	switch (SC_Unit)
	{
	case SC_EXTRACTOROBJ:
	case SC_REFINERYOBJ:
	case SC_ASSIMILATOROBJ:
		a = SearchObjs(xb*SIZESPRLANSHX + GetUnitWidthAndHeight(SC_Unit, UNITDIM_WIDTH) / 2, yb*SIZESPRLANSHY + GetUnitWidthAndHeight(SC_Unit, UNITDIM_HEIGHT) / 2, 10, &ass_extr_ref[3], 1);
		a2 = SearchObjs(xb*SIZESPRLANSHX + GetUnitWidthAndHeight(SC_Unit, UNITDIM_WIDTH) / 2, yb*SIZESPRLANSHY + GetUnitWidthAndHeight(SC_Unit, UNITDIM_HEIGHT) / 2, 10, &ass_extr_ref[0], 3);
		if (a && !a2)
		{
			constrerror = 0;
			ret = 1;
		}
		else
		{
			memset(returnedarray, 0x00, xsize*ysize * sizeof(unsigned short));
			constrerror = TOBECONSTRUCT_NOTGEYSER;
			ret = 0;
		}
		break;
	default:
		if (IsResourceDepot(SC_Unit))
		{
			ret2 = CanBuildForResource(xb, yb, xsize, ysize, returnedarray, ignorecells);
			if (ret && !ret2)
				constrerror = TOBECONSTRUCT_NEARRESOURCE;
			ret &= ret2;
		}
		if (RequiresPylonEnergy(SC_Unit))
		{
			ret2 = CanBuildOnPylonArea(xb, yb, xsize, ysize, returnedarray, playernr);
			if (ret && !ret2)
				constrerror = TOBECONSTRUCT_NOTPYLONAREA;
			ret &= ret2;
		}
		if (RequiresCreep(SC_Unit))
		{
			ret2 = CanBuildOnCreep(xb, yb, xsize, ysize, returnedarray);
			if (ret && !ret2)
				constrerror = TOBECONSTRUCT_NOTCREEP;
			ret &= ret2;
		}
		ret2 = CanBuildOnBuildPlace(xb, yb, xsize, ysize, returnedarray, ignorecells);
		if (ret && !ret2)
			constrerror = TOBECONSTRUCT_WRONGPLACE;
		ret &= ret2;
		ret2 = CanBuildOnTile(xb, yb, xsize, ysize, returnedarray, ignorecells, GetUnitRace(SC_Unit) == ZERGRACE);
		if (ret && !ret2)
			constrerror = TOBECONSTRUCT_WRONGPLACE;
		ret &= ret2;
		ret2 = CanBuildOnEarth(xb, yb, xsize, ysize, returnedarray, ignorecells);
		if (ret && !ret2)
			constrerror = TOBECONSTRUCT_WRONGPLACE;
		ret &= ret2;
		break;
	}
	*error = constrerror;
	return(ret);
}
//=============================================
//return 1- if posible to construct , 0 - no posible to construct
int CanBuildOnPlaceWithAddon(int xb, int yb, int xparent, int yparent, int xsize, int ysize,
	unsigned short *returnedarray, SCUNIT SC_Unit, int playernr, int *constrerror)
{
	memset(returnedarray, 0xff, xsize*ysize * sizeof(unsigned short));
	//check if not same place
	if (abs(xb - xparent) >= xsize || abs(yb - yparent) >= ysize)
		return(CanBuildOnPlace(xb, yb, xsize, ysize, returnedarray, NULL, SC_Unit, playernr, constrerror));

	char ignorecells[4 * 3];
	memset(ignorecells, 0x00, sizeof(ignorecells));
	int i, j, k = 0;

	for (i = 0;i < ysize;i++)
		for (j = 0;j < xsize;j++)
		{
			if (xb + j >= xparent && xb + j < xparent + xsize &&
				yb + i >= yparent && yb + i < yparent + ysize)
				ignorecells[k] = 1;
			k++;
		}
	return(CanBuildOnPlace(xb, yb, xsize, ysize, returnedarray, ignorecells, SC_Unit, playernr, constrerror));
}
//=============================================
//return 1- if posible to construct , 0 - no posible to construct
int CanBuildOnPylonArea(int xb, int yb, int xsize, int ysize,
	unsigned short *returnedarray, int playernr)
{
	int i, j, offset, totalpower = 0;
	unsigned short ret = 0xffff;
	unsigned char *parea;
	parea = map.pylonarea[playernr];
	for (i = 0;i < ysize;i++)
	{
		offset = (yb + i)*MAXXMAP;
		for (j = 0;j < xsize;j++)
			if (parea[offset + xb + j])
				totalpower++;
	}
	if (totalpower < xsize*ysize / 2)
	{
		memset(returnedarray, 0, xsize*ysize * sizeof(unsigned short));
		ret = 0x0;
	}
	return (ret == 0xffff);
}
//=============================================
//return 1- if posible to construct , 0 - no posible to construct
int CanBuildOnTile(int xb, int yb, int xsize, int ysize,
	unsigned short *returnedarray,
	char *ignorecells,
	int ignorecreep)
{
	int i, j, offset, k = 0;
	unsigned short ret = 0xffff, tempret;
	if (returnedarray)
		for (i = 0;i < ysize;i++)
		{
			offset = (yb + i)*MAXXMAP;
			for (j = 0;j < xsize;j++)
			{
				if (ignorecells&&ignorecells[k++])
					returnedarray[i*xsize + j] = 0xffff;
				else
					if (!ignorecreep&&map.creepflagplace[offset + xb + j])
						returnedarray[i*xsize + j] &= 0x0;
					else
						returnedarray[i*xsize + j] &= map.tileswalk[map.display_map[offset + xb + j]];
				ret &= returnedarray[i*xsize + j];
			}
		}
	else
		for (i = 0;i < ysize;i++)
		{
			offset = (yb + i)*MAXXMAP;
			for (j = 0;j < xsize;j++)
			{
				if (ignorecells&&ignorecells[k++])
					tempret = 0xffff;
				else
					if (!ignorecreep&&map.creepflagplace[offset + xb + j])
						tempret = 0x0;
					else
						tempret = map.tileswalk[map.display_map[offset + xb + j]];
				ret &= tempret;
			}
		}
	return (ret == 0xffff);
}
//=============================================
//return 1- if posible to construct , 0 - no posible to construct
int	 CanBuildOnEarth(int xb, int yb, int xsize, int ysize,
	unsigned short *returnedarray, char *ignorecells)
{
	int i, j, k = 0;
	unsigned short ret = 0xffff, tempret;
	if (returnedarray)
		if (ignorecells)
		{
			for (i = 0;i < ysize;i++)
				for (j = 0;j < xsize;j++)
				{
					if (!ignorecells[k++])
					{
						if (!GetMapWalk8((xb + j) * 4, (yb + i) * 4, 4, 4))
							returnedarray[i*xsize + j] &= 0x0;
						ret &= returnedarray[i*xsize + j];
					}
				}
		}
		else
		{
			for (i = 0;i < ysize;i++)
				for (j = 0;j < xsize;j++)
				{
					if (!GetMapWalk8((xb + j) * 4, (yb + i) * 4, 4, 4))
						returnedarray[i*xsize + j] &= 0x0;
					ret &= returnedarray[i*xsize + j];
				}
		}
	else
		if (ignorecells)
		{
			for (i = 0;i < ysize;i++)
				for (j = 0;j < xsize;j++)
				{
					if (!ignorecells[k++])
					{
						if (!GetMapWalk8((xb + j) * 4, (yb + i) * 4, 4, 4))
							tempret = 0x0000;
						else
							tempret = 0xffff;
						ret &= tempret;
					}
				}
		}
		else
		{
			return(GetMapWalk8(xb * 4, yb * 4, xsize * 4, ysize * 4));
		}
	return (ret == 0xffff);
}
//=============================================
//return 1- if posible to construct , 0 - no posible to construct
int	 CanBuildOnBuildPlace(int xb, int yb, int xsize, int ysize,
	unsigned short *returnedarray, char *ignorecells)
{
	int i, j, offset, k = 0;
	unsigned short ret = 0xffff, tempret;
	if (returnedarray)
	{
		for (i = 0;i < ysize;i++)
		{
			offset = (yb + i)*MAXXMAP;
			for (j = 0;j < xsize;j++)
			{
				if (ignorecells&&ignorecells[k++])
					returnedarray[i*xsize + j] &= 0xffff;
				else
					if (map.buildswalk[offset + xb + j])
						returnedarray[i*xsize + j] &= 0xffff;
					else
						returnedarray[i*xsize + j] &= 0x0000;
				ret &= returnedarray[i*xsize + j];
			}
		}
	}
	else
	{
		for (i = 0;i < ysize;i++)
		{
			offset = (yb + i)*MAXXMAP;
			for (j = 0;j < xsize;j++)
			{
				if (ignorecells&&ignorecells[k++])
					tempret = 0xffff;
				else
					if (map.buildswalk[offset + xb + j])
						tempret = 0xffff;
					else
						tempret = 0x0000;
				ret &= tempret;
			}
		}
	}
	return (ret == 0xffff);
}
//=============================================
//return 1- if posible to construct , 0 - no posible to construct
int CanBuildOnCreep(int xb, int yb, int xsize, int ysize, unsigned short *returnedarray)
{
	//	  return(0xffff);
	int i, j, offset, totalcreep = 0;
	unsigned short ret = 0xffff;
	for (i = 0;i < ysize;i++)
	{
		offset = (yb + i)*MAXXMAP;
		for (j = 0;j < xsize;j++)
			if (map.creepflagplace[offset + xb + j])
			{
				returnedarray[i*xsize + j] &= 0xffff;
				totalcreep++;
			}
			else
				returnedarray[i*xsize + j] &= 0x0;

	}
	if (totalcreep != xsize * ysize)
	{
		//		memset(returnedarray,0,xsize*ysize*sizeof(unsigned short));
		ret = 0x0;
	}
	return (ret == 0xffff);
}
//=============================================
//return 1- if posible to construct , 0 - no posible to construct
int CanBuildForResource(int xb, int yb, int xsize, int ysize,
	unsigned short *returnedarray, char *ignorecells)
{
	int i, j, offset, k = 0;
	unsigned short ret = 0xffff;
	for (i = 0;i < ysize;i++)
	{
		offset = (yb + i)*MAXXMAP;
		for (j = 0;j < xsize;j++)
		{
			if (ignorecells&&ignorecells[k++])
				returnedarray[i*xsize + j] &= 0xffff;
			else
				if (!map.constrmap[offset + xb + j])
					returnedarray[i*xsize + j] &= 0xffff;
				else
					returnedarray[i*xsize + j] &= 0x0000;
			ret &= returnedarray[i*xsize + j];
		}
	}
	return (ret == 0xffff);
}
//=====================================
void SetMarkOn32(int xp, int yp, int xsize, int ysize, unsigned char *usedmap, int setvalue)
{
	int i, j, offset;
	for (i = 0;i < ysize;i++)
	{
		if (yp + i < 0)
			continue;
		if (yp + i >= MAXYMAP)
			break;
		offset = (yp + i)*MAXXMAP;
		for (j = 0;j < xsize;j++)
		{
			if (xp + j < 0)
				continue;
			if (xp + j >= MAXXMAP)
				break;
			usedmap[offset + xp + j] = setvalue;
		}
	}
}
//=============================================
void AddMarkOn32(int xp, int yp, int xsize, int ysize, unsigned char *usedmap)
{
	int i, j, offset;
	for (i = 0;i < ysize;i++)
	{
		if (yp + i < 0)
			continue;
		if (yp + i >= MAXYMAP)
			break;
		offset = (yp + i)*MAXXMAP;
		for (j = 0;j < xsize;j++)
		{
			if (xp + j < 0)
				continue;
			if (xp + j >= MAXXMAP)
				break;
			usedmap[offset + xp + j]++;
		}
	}
}
//=============================================
void DelMarkOn32(int xp, int yp, int xsize, int ysize, unsigned char *usedmap)
{
	int i, j, offset;
	for (i = 0;i < ysize;i++)
	{
		if (yp + i < 0)
			continue;
		if (yp + i >= MAXYMAP)
			break;
		offset = (yp + i)*MAXXMAP;
		for (j = 0;j < xsize;j++)
		{
			if (xp + j < 0)
				continue;
			if (xp + j >= MAXXMAP)
				break;
			usedmap[offset + xp + j]--;
		}
	}
}
//=============================================
void SetMapWalk8(int x, int y, int sx, int sy, int flag)//coords and size divided by 8
{
	int i, j;
	for (i = 0;i < sy;i++)
	{
		if (y + i < 0)
			continue;
		if (y + i >= MAXYMAP8)
			break;
		for (j = 0;j < sx;j++)
		{
			if (x + j < 0)
				break;
			if (x + j >= MAXXMAP8)
				break;
			//			if ( (map.mapwalk[(y+i)*MAXXMAP8+x+j] & 1) == SETWALK)
			//				printf("x=%d,y=%d,j=%d,i=%d\n",x,y,j,i);
			map.mapwalk[(y + i)*MAXXMAP8 + x + j] = flag;
		}
	}
}
//=============================================
//return 1 if on map8 can constr can put unit(empty place)
//
int GetMapWalk8(int x, int y, int sx, int sy)//coords and size divided by 8
{
	int i, j, ret = 1;
	for (i = 0;i < sy;i++)
	{
		if (y + i < 0)
			return(0);
		if (y + i >= MAXYMAP8)
			return(0);
		for (j = 0;j < sx;j++)
		{
			if (x + j < 0)
				return(0);
			if (x + j >= MAXXMAP8)
				return(0);
			ret &= (map.mapwalk[(y + i)*MAXXMAP8 + x + j] & 1);
			//			ret &= (map.buildswalk[(y+i)*MAXXMAP8+x+j]&1);
		}
	}
	return(ret);
}
//=============================================
void SetObjWalk8(struct mapinfo *map, OBJ *a, int flag)
{
	int xbeg, ybeg, sx, sy;
	if (a->SC_Unit == SC_EXTRACTOROBJ || a->SC_Unit == SC_REFINERYOBJ || a->SC_Unit == SC_ASSIMILATOROBJ)
		return;//this build can do this because the geyser do this for them
	if (IsBuild(a->SC_Unit))
	{
		sx = GetUnitWidthAndHeight(a->SC_Unit, UNITDIM_WIDTH) / SIZESPRLANSHX;
		sy = GetUnitWidthAndHeight(a->SC_Unit, UNITDIM_HEIGHT) / SIZESPRLANSHY;
		xbeg = a->xkart - sx / 2;
		ybeg = a->ykart - sy / 2;
		SetMarkOn32(xbeg, ybeg, sx, sy, map->buildswalk, flag);
	}
	else
	{
		xbeg = (GetOBJx(a) - GetUnitWidthAndHeight(a->SC_Unit, UNITDIM_WIDTH) / 2) / 8;
		ybeg = (GetOBJy(a) - GetUnitWidthAndHeight(a->SC_Unit, UNITDIM_HEIGHT) / 2) / 8;
		sx = GetUnitWidthAndHeight(a->SC_Unit, UNITDIM_WIDTH) / 8;
		sy = GetUnitWidthAndHeight(a->SC_Unit, UNITDIM_HEIGHT) / 8;
		SetMapWalk8(xbeg, ybeg, sx, sy, flag);
	}
}
//=============================================
