
#include "mage.h"
#include "objinfo.h"
#include "load.h"
#include "lists.h"
#include "maps.h"
#include "vars.h"
#include "RegenObjMap.h"

//=====================================
void RegenObjMap::Init(void)
{
	NrRegenObjs = 0;
	RegenCycleNr = INT_MAX;
	RegenFromNr = 0;
}
//=====================================
void RegenObjMap::OpenMapCycle(void)
{
	if (RegenCycleNr >= FULLMAPREGENERATIONCYCLES)
	{
		RegenCycleNr = 0;
		RegenFromNr = 0;
	}
	else
	{
		RegenCycleNr++;
		RegenFromNr += NrRegenObjs;
	}
	if (!RegenCycleNr)
	{
		//first cycle
		NrRegenObjs = MaxObjects / FULLMAPREGENERATIONCYCLES + 1;
		if (!NrRegenObjs)
			NrRegenObjs++;
		map.clearfog[NUMBGAMER] = 1;
		clearopenseeKarta();	//clear map
		UnitsMAPRefresh();
	}
	OpenMapByRange(RegenFromNr, RegenFromNr + NrRegenObjs - 1);//open map by some of units
}
//=====================================
void RegenObjMap::OpenMapByRange(int from, int to)
{
	OBJ *a;
	OBJstruct *b;
	int i;
	if (!MaxObjects)
		return;
	if (from >= MaxObjects)
		return;
	if (to >= MaxObjects)
		to = MaxObjects - 1;
	for (i = from;i <= to;i++)
	{
		a = objects[i];
		b = loadobj(a->SC_Unit);
		if (GetUnitSightRange(a, b) > 0)
		{
			makeoneobjseeopen(a, b);
			arbitermakewarpfield(a, b);
		}
	}
	for (i = from;i <= to;i++)
	{
		a = objects[i];
		if (!IsBuild(a->SC_Unit))
		{
			if (a->SC_Unit != SC_ARBITEROBJ && a->SC_Unit != SC_HERO_DANIMOTHOBJ)
			{
				if (a->playernr < PLAYEDPLAYERS)
				{
					if (map.mapbits.arbiterfield[a->ykart*MAXXMAP + a->xkart] & (1 << a->playernr))
					{
						if (GetMageAtr(&a->atrobj, ATRINVISIBLE) <= MAXATRMAGE)
						{
							if (GetMageAtr(&a->atrobj, ATRINVISIBLE) > 0)
								//esli cloakwrite pod arbiterom
								addmage(a, ATRINVISIBLE, WRAITHEPODARBITEROM);
							else
								//esli drug iuniti pod arbiterom
								addmage(a, ATRINVISIBLE, NEWRAITHEPODARBITEROM);
						}
					}
					else
					{
						if (GetMageAtr(&a->atrobj, ATRINVISIBLE) == NEWRAITHEPODARBITEROM)
							addmage(a, ATRINVISIBLE, 0);//prostoi obj pod arbit
						else
							if (GetMageAtr(&a->atrobj, ATRINVISIBLE) == WRAITHEPODARBITEROM)
								addmage(a, ATRINVISIBLE, a->mana);//writhe clockanii
							else
								if (GetMageAtr(&a->atrobj, ATRINVISIBLE) > MAXATRMAGE&&
									GetMageAtr(&a->atrobj, ATRINVISIBLE) < ATRMAGEINFINITE)
									addmage(a, ATRINVISIBLE, 0);//others obj
					}
				}
			}
		}
	}
}
//=================================
void RegenObjMap::ChangeObjPos(OBJ *o, int newposinlist)
{
	if (RegenFromNr + NrRegenObjs - 1 >= MaxObjects)
	{
		//Last cycles of update was finish so object already regen the map and no need to regen again
		return;
	}
	if (newposinlist < RegenFromNr)
	{
		//the object move to range of already units that open the map, so need to manual refresh the map for this unit
		revealMap->AddObj(o);
	}
}
//=====================================
void RegenObjMap::AddObjList(OBJ *o)
{
	objects[MaxObjects] = o;
	o->nrobject = MaxObjects;
	MaxObjects++;
}
//==========================================
void RegenObjMap::DelObjList(OBJ *o)
{
	int nrinlist = o->nrobject;
	MaxObjects--;
	objects[nrinlist] = objects[MaxObjects];
	objects[nrinlist]->nrobject = nrinlist;
	objects[MaxObjects] = NULL;
	ChangeObjPos(objects[nrinlist], nrinlist);
}
