
#include "lists.h"
#include "maps.h"
#include "vars.h"


#include "RefreshMap.h"

void RefreshMap::UpdateSequenseObjs(void)
{
	MinObjRegen = MaxObjects / MAPREGENERATION;
	if (MaxObjects % MAPREGENERATION)
		MinObjRegen++;
}

void RefreshMap::Init(void)
{
	MinObjRegen = 0;
	MaxRegen = 0;
	NrObjRegen = INT_MAX;
}

void RefreshMap::OpenMapByRangeOfUnits(void)
{

	if (NrObjRegen >= MaxRegen)
	{
		NrObjRegen = 0;
	}
	else
		NrObjRegen++;
	if (MaxObjects >= 1000 || needclearmap)
	{
		if (NrObjRegen == 0)
		{
			needclearmap = 0;
			map.clearfog[NUMBGAMER] = 1;
			clearopenseeKarta();	//clear map
			UnitsMAPRefresh();
		}
	}
	makeopenseeKarta(diapazone[NrObjRegen].begin, diapazone[NrObjRegen].end);//open map by some of units
}
//=====================================
void RefreshMap::CreateObjRegen(void)
{
	int i, nrobj = 0, regen = 0;
	struct OBJ *a;
	for (i = 0;i < MAPREGENERATION;i++)
		diapazone[i].begin = -1;
	for (i = 0;i < MaxObjects;i++)
	{
		a = objects[i];
		if (++nrobj > MinObjRegen)
		{
			nrobj = 0;
			regen++;
		}
		a->whenregen = regen;
		if (diapazone[regen].begin == -1)
			diapazone[regen].begin = i;
		diapazone[regen].end = i;
	}
	MaxRegen = regen;
}

