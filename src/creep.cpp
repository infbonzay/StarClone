
#include "wmem.h"

#include "CrossPlatform.h"
#include "vars.h"
#include "man.h"
#include "diap.h"
#include "load.h"
#include "auxil.h"
#include "maps.h"
#include "walk.h"
#include "rand.h"
#include "lists.h"
#include "objs.h"
#include "objinfo.h"
#include "creep.h"

//===========================================
//creeps count around build, see doc later
#define BEGINCREEPTABLE0		42
#define BEGINCREEPTABLE1		0
#define MAXCREEPTABLE0			(188-BEGINCREEPTABLE0)
#define MAXCREEPTABLE1			(192-BEGINCREEPTABLE1)

#define CREEP_FILL_TABLE		0x01
#define CREEP_NEED_TABLE		0x02

#define NOCREEPLATER			0
#define WITHCREEPLATER			1
//===========================================
//creeptable deltax,deltay coords values from center of build(a->xkart(a->sourcex/32) and a->ykart((a->sourcey/32))
//one table present creep around build without underbuild place
//begincreeptableX is table in time of appear build(MODEWARP)
//maxcreeptableX is table with all creeps around build
//X=0 is for hatchery,lair,hive
//X=1 for creep colony,sunken colony,spore colony
signed char begincreeptable0[BEGINCREEPTABLE0*2]=
	{
	-2,-2, -1,-2, 0,-2, 1,-2, -3,-1, 2,-1, -3,0, 2,0, -3,1, 2,1, -2,2, -1,2, 0,2, 1,2,
	-2,-3, -1,-3, 0,-3, 1,-3, -3,-2, 2,-2, -4,-1, 3,-1, -4,0, 3,0, -4,1, 3,1, -3,2, 2,2, -2,3, -1,3, 0,3, 1,3,
	-4,-2, 3,-2, -5,-1, 4,-1, -5,0, 4,0, -5,1, 4,1, -4,2, 3,2
	};

signed char begincreeptable1[BEGINCREEPTABLE1*2]=
	{
	};

signed char maxcreeptable0[MAXCREEPTABLE0*2]=
	{
	-2,-4, -1,-4,  0,-4, 1,-4, -3,-3, 2,-3, -3, 3, 2, 3, -2, 4, -1,4, 0, 4, 1, 4,
	-2,-5, -1,-5,  0,-5, 1,-5, -3,-4, 2,-4, -4,-3, 3,-3, -5,-2, 4,-2, -6,-1, 5,-1, -6,0, 5,0, -6,1, 5,1, -5,2, 4,2, -4,3, 3,3, -3,4, 2,4, -2,5, -1,5, 0,5, 1,5,
	-2,-6, -1,-6,  0,-6, 1,-6, -3,-5, 2,-5, -4,-4, 3,-4, -5,-3, 4,-3, -6,-2, 5,-2, -7,-1, 6,-1, -7,0, 6,0, -7,1, 6,1, -6,2, 5,2, -5,3, 4,3, -4,4, 3,4, -3,5, 2,5, -2,6, -1,6, 0,6, 1,6,
	-3,-6,	2,-6, -4,-5, 3,-5, -5,-4, 4,-4, -6,-3, 5,-3, -7,-2, 6,-2, -8,-1, 7,-1, -8,0, 7,0, -8,1, 7,1, -7,2, 6,2, -6,3, 5,3, -5,4, 4,4, -4,5, 3,5, -3,6, 2,6,
	-5,-5,	4,-5, -6,-4, 5,-4, -7,-3, 6,-3, -8,-2, 7,-2, -9,-1, 8,-1, -9,0, 8,0, -9,1, 8,1, -8,2, 7,2, -7,3, 6,3, -6,4, 5,4, -5,5, 4,5,
	-6,-5,	5,-5, -7,-4, 6,-4, -8,-3, 7,-3, -9,-2, 8,-2, -10,-1, 9,-1, -10,0, 9,0, -10,1, 9,1, -9,2, 8,2, -8,3, 7,3, -7,4, 6,4, -6,5, 5,5,
	-8,-4,	7,-4, -9,-3, 8,-3, -9, 3, 8, 3, -8, 4, 7, 4
	};

signed char maxcreeptable1[MAXCREEPTABLE1*2]=
{
	-1,-2, 0,-2, -2,-1, 1,-1, -2,0, 1,0, -1,1, 0,1,
	-1,-3, 0,-3, -2,-2, 1,-2, -3,-1, 2,-1, -3,0, 2,0, -2,1, 1,1, -1,2, 0,2,
	-1,-4, 0,-4, -2,-3, 1,-3, -3,-2, 2,-2, -4,-1, 3,-1, -4,0, 3,0, -3,1, 2,1, -2,2, 1,2, -1,3, 0,3,
	-1,-5, 0,-5, -2,-4, 1,-4, -3,-3, 2,-3, -4,-2, 3,-2, -5,-1, 4,-1, -5,0, 4,0, -4,1, 3,1, -3,2, 2,2, -2,3, 1,3, -1,4, 0,4,
	-1,-6, 0,-6, -2,-5, 1,-5, -3,-4, 2,-4, -4,-3, 3,-3, -5,-2, 4,-2, -6,-1, 5,-1, -6,0, 5,0, -5,1, 4,1, -4,2, 3,2, -3,3, 2,3, -2,4, 1,4, -1,5, 0,5,
	-2,-6, 1,-6, -3,-5, 2,-5, -4,-4, 3,-4, -5,-3, 4,-3, -6,-2, 5,-2, -7,-1, 6,-1, -7,0, 6,0, -6,1, 5,1, -5,2, 4,2, -4,3, 3,3, -3,4, 2,4, -2,5, 1,5,
	-3,-6, 2,-6, -4,-5, 3,-5, -5,-4, 4,-4, -6,-3, 5,-3, -7,-2, 6,-2, -8,-1, 7,-1, -8,0, 7,0, -7,1, 6,1, -6,2, 5,2, -5,3, 4,3, -4,4, 3,4, -3,5, 2,5,
	-4,-6, 3,-6, -5,-5, 4,-5, -6,-4, 5,-4, -7,-3, 6,-3, -8,-2, 7,-2, -9,-1, 8,-1, -9,0, 8,0, -8,1, 7,1, -7,2, 6,2, -6,3, 5,3, -5,4, 4,4, -4,5, 3,5,
	-5,-6, 4,-6, -6,-5, 5,-5, -7,-4, 6,-4, -8,-3, 7,-3, -9,-2, 8,-2, -10,-1, 9,-1, -10,0, 9,0, -9,1, 8,1, -8,2, 7,2, -7,3, 6,3, -6,4, 5,4, -5,5, 4,5,
	-7,-5, 6,-5, -8,-4, 7,-4, -9,-3, 8,-3, -10,-2, 9,-2, -10,1, 9,1, -9,2, 8,2, -8,3, 7,3, -7,4, 6,4
	};
//===========================================
//sprite positions in ./<tileset>.grp in different positions
signed char AllCreep[512] =
{
		-1,-1,0,1,-1,-1,2,3,4,4,5,5,4,4,23,23,					//000-00f
		36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,		//010-01f
		15,15,16,26,15,15,16,26,27,27,28,28,27,27,28,28,		//020-02f
		36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,		//030-03f
		-1,-1,0,1,-1,-1,2,3,4,4,5,5,4,4,23,23,					//040-04f
		36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,		//050-05f
		15,15,16,26,15,15,16,26,27,27,28,28,27,27,28,28,		//060-06f
		36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,		//070-07f
		6,6,7,8,6,6,10,3,12,12,14,14,12,12,25,25,				//080-08f
		36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,		//090-09f
		19,19,20,30,19,19,20,30,34,34,36,36,34,34,36,36,		//0a0-0af
		36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,		//0b0-0bf
		11,11,13,14,11,11,24,25,12,12,14,14,12,12,25,25,		//0c0-0cf
		36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,		//0d0-0df
		33,33,35,36,33,33,35,36,34,34,36,36,34,34,36,36,		//0e0-0ef
		36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,		//0f0-0ff
		-1,-1,0,1,-1,-1,2,3,4,4,5,5,4,4,23,23,					//100-10f
		36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,		//110-11f
		15,15,16,26,15,15,16,26,27,27,28,28,27,27,28,28,		//120-12f
		36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,		//130-13f
		-1,-1,0,1,-1,-1,2,3,4,4,5,5,4,4,23,23,					//140-14f
		36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,		//150-15f
		15,15,16,26,15,15,16,26,27,27,28,28,27,27,28,28,		//160-16f
		36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,		//170-17f
		17,17,29,29,17,17,20,30,31,31,32,32,31,31,36,36,		//180-18f
		36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,		//190-19f
		19,19,20,30,19,19,20,30,34,34,36,36,34,34,36,36,		//1a0-1af
		36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,		//1b0-1bf
		21,21,22,32,21,21,35,36,31,31,32,32,31,31,36,36,		//1c0-1cf
		36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,		//1d0-1df
		33,33,35,36,33,33,35,36,34,34,36,36,34,34,36,36,		//1e0-1ef
		36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36			//1f0-1ff
};
//===========================================
mylist creepbuilds;
//===========================================
int allcreepbits[9]={0x1,0x2,0x4,0x8,0x10,0x20,0x40,0x80,0x100};
//===========================================
int CREEPAROUND[9][2]={
						{-1,-1},{ 0,-1},{ 1,-1},
						{-1, 0},{ 0, 0},{ 1, 0},
						{-1, 1},{ 0, 1},{ 1, 1}
};
//===========================================
int GetCreepAroundWithFog(int x, int y, int indextile32)
{
	signed char creepnr;
	int mycreepbits = 0, creepbits = 0, newx, newy;
	for (int i = 0;i < 9;i++)
	{
		newx = x + CREEPAROUND[i][0];
		newy = y + CREEPAROUND[i][1];
		if (CHECKFORMAPBORDERS(newx, newy))
			continue;
		if (mapSEE(newx, newy) > 1)
		{
			if (map.creepflagplace[newy*MAXXMAP + newx] >= 1)
			{
				creepbits |= allcreepbits[i];
				mycreepbits++;
			}
		}
		else
		{
			if (mapOPEN(newx, newy) > 1)
			{
				if (map.mapbits.savedcreep[NUMBGAMER] && map.mapbits.savedcreep[NUMBGAMER][newy*MAXXMAP + newx] >= 1)
				{
					creepbits |= allcreepbits[i];
					mycreepbits++;
				}
			}
		}
	}
	creepnr = AllCreep[creepbits];
	if (creepnr >= MAXCREEPNR)
	{
		if (map.creeptilecnt)
		{
			creepnr = MAXCREEPNR + indextile32 % map.creeptilecnt;
		}
		else
			creepnr = MAXCREEPNR - 1;
	}
	return(creepnr);
}
//===========================================
int GetCreepNr(int x, int y, int *myret)
{
	signed char creepnr;
	int mycreepbits = 0, creepbits = 0, newx, newy;
	for (int i = 0;i < 9;i++)
	{
		newx = x + CREEPAROUND[i][0];
		newy = y + CREEPAROUND[i][1];
		if (CHECKFORMAPBORDERS(newx, newy))
			continue;
		//		if (newx<0||newy<0||newx>=MAXXMAP||newy>=MAXYMAP)
		//			continue;
		if (map.creepflagplace[newy*MAXXMAP + newx] >= 1)
		{
			creepbits |= allcreepbits[i];
			mycreepbits++;
		}
	}
	if (myret)
		*myret = mycreepbits;
	creepnr = AllCreep[creepbits];
	if (creepnr >= MAXCREEPNR)
	{
		if (map.creeptilecnt)
		{
			creepnr = MAXCREEPNR + map.display_map[y*MAXXMAP + x] % map.creeptilecnt;
		}
		else
			creepnr = MAXCREEPNR - 1;
	}
	return(creepnr);
}
//===========================================
inline void SetCreepNoCheck(int x,int y)
{
	map.creepflagplace[y*MAXXMAP+x] = 1;
}
//===========================================
inline void ClearCreepNoCheck(int x,int y)
{
	map.creepflagplace[y*MAXXMAP+x] = 0;
}
//===========================================
inline void AddCreepNoCheck(int x,int y)
{
	++map.creepflagneeded[y*MAXXMAP+x];
}
//===========================================
inline unsigned char RemoveCreepNoCheck(int x,int y)
{
	return(--map.creepflagneeded[y*MAXXMAP+x]);
}
//===========================================
int SetCreep(int x,int y)
{
	if (CanBuildOnBuildPlace(x,y,1,1,NULL,NULL))
	{
		SetCreepNoCheck(x,y);
		return(1);
	}
	return(0);
}
//===========================================
int ClearCreep(int x,int y)
{
//	  if (CanBuildOnBuildPlace(x,y,1,1,NULL,NULL))
	{
		ClearCreepNoCheck(x,y);
		return(1);
	}
	return(0);
}
//===========================================
void AddCreep(int x,int y)
{
	if (CanBuildOnTile(x,y,1,1,NULL,NULL,1))
		AddCreepNoCheck(x,y);
}
//===========================================
void RemoveCreep(int x,int y)
{
	if (CanBuildOnTile(x,y,1,1,NULL,NULL,1))
		RemoveCreepNoCheck(x,y);
}
//===========================================
void EmptyAllCreeps(void)
{
	int i,j;
	for (i=0;i<MAXYMAP;i++)
		for (j=0;j<MAXXMAP;j++)
		{
			map.creepflagplace[i*MAXXMAP+j]=0;
			map.creepflagneeded[i*MAXXMAP+j]=0;
		}
}
//===========================================
void FillWithCreepNowByTable(struct OBJ *a, struct OBJstruct *b, int tabletype)
{
	CreepAddRemoveTables *tempcreeptable;
	signed char *creeptable;
	int i, x, y, deltax, deltay, creepbits, maxcreeps;
	if (tabletype == BEGINCREEPTABLE)
	{
		if (b->creeptype == CREEPBUILDTYPE0)
		{
			creeptable = begincreeptable0;
			maxcreeps = BEGINCREEPTABLE0;
		}
		else
		{
			creeptable = begincreeptable1;
			maxcreeps = BEGINCREEPTABLE1;
		}
	}
	else
	{
		if (b->creeptype == CREEPBUILDTYPE0)
		{
			creeptable = maxcreeptable0;
			maxcreeps = MAXCREEPTABLE0;
		}
		else
		{
			creeptable = maxcreeptable1;
			maxcreeps = MAXCREEPTABLE1;
		}
	}
	if (a->creepbuildlist_ID != CREEPLISTEMPTY)
		tempcreeptable = (CreepAddRemoveTables *)creepbuilds.GetElemNr(a->creepbuildlist_ID);
	else
		tempcreeptable = NULL;
	for (i = 0;i < maxcreeps;i++)
	{
		deltax = creeptable[i * 2 + 0];
		x = a->xkart + deltax;
		if (x >= MAXXMAP || x < 0)
			continue;
		deltay = creeptable[i * 2 + 1];
		y = a->ykart + deltay;
		if (y >= MAXYMAP || y < 0)
			continue;
		GetCreepNr(x, y, &creepbits);
		if (creepbits)
			if (SetCreep(x, y))//if not placed creep need to memorize to creep later
			{
				if (tempcreeptable)
					AddToFilledCreepTable(tempcreeptable, deltax, deltay);
				continue;
			}
		if (tempcreeptable)
			AddToNeededCreepTable(tempcreeptable, deltax, deltay);//add to creep later
	}
}
//===========================================
void FillWithCreepTimedByTable(struct OBJ *a, struct OBJstruct *b, int tabletype, int creeplater)
{
	CreepAddRemoveTables *tempcreeptable;
	signed char *creeptable;
	int i, x, y, deltax, deltay, maxcreeps;
	if (tabletype == BEGINCREEPTABLE)
	{
		if (b->creeptype == CREEPBUILDTYPE0)
		{
			creeptable = begincreeptable0;
			maxcreeps = BEGINCREEPTABLE0;
		}
		else
		{
			creeptable = begincreeptable1;
			maxcreeps = BEGINCREEPTABLE1;
		}
	}
	else
	{
		if (b->creeptype == CREEPBUILDTYPE0)
		{
			creeptable = maxcreeptable0;
			maxcreeps = MAXCREEPTABLE0;
		}
		else
		{
			creeptable = maxcreeptable1;
			maxcreeps = MAXCREEPTABLE1;
		}
	}
	if (a->creepbuildlist_ID != CREEPLISTEMPTY)
		tempcreeptable = (CreepAddRemoveTables *)creepbuilds.GetElemNr(a->creepbuildlist_ID);
	else
		tempcreeptable = NULL;
	for (i = 0;i < maxcreeps;i++)
	{
		deltax = creeptable[i * 2 + 0];
		x = a->xkart + deltax;
		if (x >= MAXXMAP || x < 0)
			continue;
		deltay = creeptable[i * 2 + 1];
		y = a->ykart + deltay;
		if (y >= MAXYMAP || y < 0)
			continue;
		AddCreep(x, y);
		if (creeplater == WITHCREEPLATER && tempcreeptable)
			AddToNeededCreepTable(tempcreeptable, deltax, deltay);//add to creep later
	}
}
//===========================================
//main function to create creep
//===========================================
void FillWithCreepNow(struct OBJ *a, struct OBJstruct *b, int tabletype)
{
	ForceKartChanges(a);

	AddOBJCreepTable(a, b->creeptype, tabletype);//create creep arrays
	FillWithCreepUnderBuild(a);
	if (b->creeptype != CREEPUNDERBUILD)
	{
		FillWithCreepNowByTable(a, b, BEGINCREEPTABLE);
		FillWithCreepTimedByTable(a, b, BEGINCREEPTABLE, NOCREEPLATER);
		if (tabletype == MAXCREEPTABLE)
		{
			FillWithCreepNowByTable(a, b, MAXCREEPTABLE);
			FillWithCreepTimedByTable(a, b, MAXCREEPTABLE, NOCREEPLATER);
		}
		else
		{
			FillWithCreepTimedByTable(a, b, MAXCREEPTABLE, WITHCREEPLATER);
		}
	}
}
//===========================================
void EmptyCreepTimedByTable(struct OBJ *a, struct OBJstruct *b, int tabletype)
{
	signed char *creeptable;
	int i, x, y, deltax, deltay, maxcreeps;
	if (tabletype == BEGINCREEPTABLE)
	{
		if (b->creeptype == CREEPBUILDTYPE0)
		{
			creeptable = begincreeptable0;
			maxcreeps = BEGINCREEPTABLE0;
		}
		else
		{
			creeptable = begincreeptable1;
			maxcreeps = BEGINCREEPTABLE1;
		}
	}
	else
	{
		if (b->creeptype == CREEPBUILDTYPE0)
		{
			creeptable = maxcreeptable0;
			maxcreeps = MAXCREEPTABLE0;
		}
		else
		{
			creeptable = maxcreeptable1;
			maxcreeps = MAXCREEPTABLE1;
		}
	}
	for (i = 0;i < maxcreeps;i++)
	{
		deltax = creeptable[i * 2 + 0];
		x = a->xkart + deltax;
		if (x >= MAXXMAP || x < 0)
			continue;
		deltay = creeptable[i * 2 + 1];
		y = a->ykart + deltay;
		if (y >= MAXYMAP || y < 0)
			continue;
		RemoveCreep(x, y);
	}
}
//===========================================
//main function to remove creep
//===========================================
void EmptyCreepTimed(struct OBJ *a, struct OBJstruct *b)
{
	CreepAddRemoveTables *creeptable;
	ForceKartChanges(a);
	if (b->creeptype == CREEPUNDERBUILD)
	{
		EmptyCreepUnderBuild(a, AFTERREMOVECLEARCREEP);
	}
	else
	{
		EmptyCreepUnderBuild(a, AFTERREMOVENOCLEARCREEP);
		EmptyCreepTimedByTable(a, b, BEGINCREEPTABLE);
		EmptyCreepTimedByTable(a, b, MAXCREEPTABLE);
	}
	if (a->creepbuildlist_ID != CREEPLISTEMPTY)
	{
		creeptable = (CreepAddRemoveTables *)creepbuilds.GetElemNr(a->creepbuildlist_ID);
		creeptable->growthcreepflag &= ~FLAG_CREEP_INCREASE;
	}
}
//===========================================
void FillWithCreepUnderBuild(struct OBJ *a)
{
	int i, j, xp, yp, szx, szy;
	CreepAddRemoveTables *tempcreeptable;
	ForceKartChanges(a);
	szx = GetUnitWidthAndHeight(a->SC_Unit, UNITDIM_WIDTH) / SIZESPRLANSHX;
	szy = GetUnitWidthAndHeight(a->SC_Unit, UNITDIM_HEIGHT) / SIZESPRLANSHY;
	xp = a->xkart - szx / 2;
	yp = a->ykart - szy / 2;
	if (a->creepbuildlist_ID != CREEPLISTEMPTY)
		tempcreeptable = (CreepAddRemoveTables *)creepbuilds.GetElemNr(a->creepbuildlist_ID);
	else
		tempcreeptable = NULL;
	for (i = 0;i < szy;i++)
		for (j = 0;j < szx;j++)
		{
			SetCreepNoCheck(xp + j, yp + i);					//set tile to be creeped
			if (tempcreeptable)
				AddToFilledCreepTable(tempcreeptable, j - szx / 2, i - szy / 2);	//fill table for future uncreep
			AddCreepNoCheck(xp + j, yp + i);					//add creep value)
		}
}
//===========================================
void EmptyCreepUnderBuild(struct OBJ *a, int clearcreepflag)
{
	int i, j, xp, yp, szx, szy;
	ForceKartChanges(a);
	szx = GetUnitWidthAndHeight(a->SC_Unit, UNITDIM_WIDTH) / SIZESPRLANSHX;
	szy = GetUnitWidthAndHeight(a->SC_Unit, UNITDIM_HEIGHT) / SIZESPRLANSHY;
	xp = a->xkart - szx / 2;
	yp = a->ykart - szy / 2;
	for (i = 0;i < szy;i++)
		for (j = 0;j < szx;j++)
		{
			if (!RemoveCreepNoCheck(xp + j, yp + i))
				if (clearcreepflag)
					ClearCreepNoCheck(xp + j, yp + i);
		}
}
//===========================================
void AddToFilledCreepTable(CreepAddRemoveTables *creeptable,int deltax,int deltay)
{
	creeptable->creepinfo[creeptable->curnrofcreep*2+0]=deltax;
	creeptable->creepinfo[creeptable->curnrofcreep*2+1]=deltay;
	creeptable->curnrofcreep++;
}
//===========================================
void AddToNeededCreepTable(CreepAddRemoveTables *creeptable,int deltax,int deltay)
{
	creeptable->needcreepinfo[creeptable->remaintogrowth*2+0]=deltax;
	creeptable->needcreepinfo[creeptable->remaintogrowth*2+1]=deltay;
	creeptable->remaintogrowth++;
}
//===========================================
void EmptyOneElemInArray(signed char *myarray,int elemnr,unsigned char *maxelem)
{
	int len=(*maxelem-elemnr-1)*2;
	if (len)
		memmove(myarray+elemnr*2,myarray+(elemnr+1)*2,len);
	*maxelem=*maxelem-1;
}
//===========================================
void AddOBJCreepTable(OBJ *a, int typecreep, int table)
{
	int maxnrofcreep = 0;
	CreepAddRemoveTables *creeptable;
	if (a->creepbuildlist_ID == CREEPLISTEMPTY)
	{
		creeptable = (CreepAddRemoveTables *)wmalloc(sizeof(CreepAddRemoveTables));
		memset(creeptable, 0, sizeof(CreepAddRemoveTables));
		switch (typecreep)
		{
		case CREEPBUILDTYPE0:
			maxnrofcreep = MAXCREEPTABLE0 + BEGINCREEPTABLE0;
			creeptable->growthcreepflag |= FLAG_CREEP_TABLEID0;
			if (table != MAXCREEPTABLE)
				creeptable->growthcreepflag |= FLAG_CREEP_INCREASE;
			break;
		case CREEPBUILDTYPE1:
			maxnrofcreep = MAXCREEPTABLE1 + BEGINCREEPTABLE1;
			creeptable->growthcreepflag |= FLAG_CREEP_TABLEID1;
			if (table != MAXCREEPTABLE)
				creeptable->growthcreepflag |= FLAG_CREEP_INCREASE;
			break;
		case CREEPUNDERBUILD:
			break;
		}
		creeptable->xpos = a->xkart;
		creeptable->ypos = a->ykart;
		creeptable->creepinfo = (signed char *)wmalloc((maxnrofcreep + 4 * 4) * 2);
		creeptable->needcreepinfo = (signed char *)wmalloc((maxnrofcreep + 4 * 4) * 2);
		a->creepbuildlist_ID = creepbuilds.AddList(creeptable);
	}
}
//===========================================
void IncreaseCreep(void)
{
	int j, x, y;
	int creepbits, allempty, creepnr, place, need;
	signed char deltax, deltay;
	CreepAddRemoveTables *tempcreeptable;
	creepbuilds.EnumListInit();
	while ((tempcreeptable = (CreepAddRemoveTables *)creepbuilds.GetNextListElem()))
	{
		if (tempcreeptable->growthcreepflag&FLAG_CREEP_INCREASE)
		{
			allempty = 0;
			for (j = 0;j < tempcreeptable->remaintogrowth;j++)
			{
				deltax = tempcreeptable->needcreepinfo[j * 2 + 0];
				if (deltax == CREEPNEEDTABLEEMPTYFLAG)
				{
					allempty++;
					continue;
				}
				deltay = tempcreeptable->needcreepinfo[j * 2 + 1];
				x = tempcreeptable->xpos + deltax;
				y = tempcreeptable->ypos + deltay;
				creepnr = GetCreepNr(x, y, &creepbits);
				if (creepnr != NOCREEPNR)
				{
					place = map.creepflagplace[y*MAXXMAP + x];
					need = map.creepflagneeded[y*MAXXMAP + x];
					if ((!place) && need)
						if (SetCreep(x, y))
						{
							AddToFilledCreepTable(tempcreeptable, deltax, deltay);
							EmptyOneElemInArray(tempcreeptable->needcreepinfo, j, &tempcreeptable->remaintogrowth);
							break;
						}
				}
			}
			if (allempty == tempcreeptable->remaintogrowth)
				tempcreeptable->remaintogrowth = 0;//no need to growth any more
		}
	}
}
//===========================================
void DecreaseCreep(void)
{
	int j, x, y;
	int creepbits, place, need;
	signed char deltax, deltay;
	CreepAddRemoveTables *tempcreeptable;
	creepbuilds.EnumListInit();
	while ((tempcreeptable = (CreepAddRemoveTables *)creepbuilds.GetNextListElem()))
	{
		if (!(tempcreeptable->growthcreepflag&FLAG_CREEP_INCREASE))
		{
			for (j = tempcreeptable->curnrofcreep - 1;j >= 0;j--)
			{
				deltax = tempcreeptable->creepinfo[j * 2 + 0];
				deltay = tempcreeptable->creepinfo[j * 2 + 1];
				x = tempcreeptable->xpos + deltax;
				y = tempcreeptable->ypos + deltay;
				place = map.creepflagplace[y*MAXXMAP + x];
				need = map.creepflagneeded[y*MAXXMAP + x];
				GetCreepNr(x, y, &creepbits);
				if (!place)
				{
					EmptyOneElemInArray(tempcreeptable->creepinfo, j, &tempcreeptable->curnrofcreep);
					if (!tempcreeptable->curnrofcreep)
						RemoveOBJCreepTable(tempcreeptable);
					continue;
				}
				if ((!need) && creepbits <= 5)// we need 5 around creep, no more, to decresea without coasts
				{
					ClearCreep(x, y);
					break;
				}
			}
		}
	}
}
//===========================================
void RemoveAddOneCreep(void)
{
	IncreaseCreep();
	DecreaseCreep();
}
//===========================================
void DeallocCreepArray(void *listelem)
{
	CreepAddRemoveTables *tempcreeptable=(CreepAddRemoveTables *)listelem;
	wfree(tempcreeptable->needcreepinfo);
	wfree(tempcreeptable->creepinfo);
}
//===========================================
void RemoveOBJCreepTable(CreepAddRemoveTables *tempcreeptable)
{
	//need to remove remain creep, if exist any, from removable table
	creepbuilds.FreeAndDelList(tempcreeptable,&DeallocCreepArray);
}
//===========================================
void DestroyCreepLists(void)
{
	creepbuilds.DeallocList(&DeallocCreepArray);
}
//===========================================



