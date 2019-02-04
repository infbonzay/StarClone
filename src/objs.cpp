
#include <math.h>

#include "auxil.h"
#include "debug.h"
#include "man.h"
#include "vars.h"
#include "putobj.h"
#include "lists.h"
#include "mage.h"
#include "gener.h"
#include "gr.h"
#include "wmem.h"
#include "load.h"
#include "diap.h"
#include "maps.h"
#include "audio.h"
#include "creep.h"
#include "actiondefault.h"
#include "action.h"
#include "pylon.h"
#include "unitaction.h"
#include "flingy.h"
#include "walk.h"
#include "upgrade.h"
#include "objinfo.h"
#include "rand.h"
#include "pylon.h"
#include "tempobj.h"
#include "doodad.h"
#include "construct.h"
#include "scripts.h"
#include "market.h"
#include "portrait.h"
#include "objinfo.h"
#include "weapons.h"
#include "starmap.h"
#include "images.h"
#include "iscript.h"
#include "overlay.h"
#include "sigorder.h"
#include "key.h"
#include "objs.h"

//===========================================
//Unicitojiti OBJECT
void destroyobj(struct OBJ *a)
{
	if (a)
	{
		deselectobj(a);
		if (a->childs)
		{
			for (int i = 0;i < MAXCHILDS;i++)
			{
				if (a->childs->parentof[i])
				{
					if (a->childs->parentof[i]->modemove == MODERECHARGE)
						destroyobj(a->childs->parentof[i]);
					if (a->childs->parentof[i])
					{
						QueueDelObj(a->childs->parentof[i]);
						delchild(a, a->childs->parentof[i]);
					}
				}
			}
			wfree(a->childs);	//destroy struct CHILDS
			a->childs = NULL;
		}
		//delete me from my parent list of childs
		if (a->myparent)
		{
			delchild(a->myparent, a);
		}
		if (a->loaded)
		{
			wfree(a->loaded);
			a->loaded = NULL;
		}

		if (a->SC_Unit != SC_SCARABOBJ)
			weaponflingy.RemoveOBJFromFlingyobj(a, NULL);
		else
			weaponflingy.RemoveOBJFromFlingyobj(a, a->whoatack);

		DeleteOldObjPointers(a);
		if (a->subunit)
		{
			a->subunit->subunit = NULL;
			a->subunit = NULL;
		}
		regenObjMap->DelObjList(a);

		//refreshMap->DelUpdateSequenseObjs();


		DelAllModeMoves(a, 1);
		wfree(a);
		//refreshMap->CreateObjRegen();
	}
}
//==========================================
OBJ *CreateArchon(OBJ *a, OBJ *a2)
{
	OBJ *newobj;
	int midlife, midshield;
	switch (a->SC_Unit)
	{
	case SC_HIGHTEMPLAROBJ:
		midlife = (int)(a->health + a2->health) * 100 / (GetUnitMaxHealth(a->SC_Unit)) / 2;
		midshield = (int)(a->shield + a2->shield) * 100 / GetUnitMaxShield(a->SC_Unit) / 2;
		newobj = createobjman(GetOBJx(a), GetOBJy(a), SC_ARCHONOBJ, a->playernr, TYPEMOVEATBEGIN, midshield, midlife, 100);
		break;
	case SC_DARKTEMPLAROBJ:
		midlife = (int)(a->health + a2->health) * 100 / (GetUnitMaxHealth(a->SC_Unit)) / 2;
		midshield = (int)(a->shield + a2->shield) * 100 / GetUnitMaxShield(a->SC_Unit) / 2;
		newobj = createobjman(GetOBJx(a), GetOBJy(a), SC_DARKARCHONOBJ, a->playernr, TYPEMOVEATBEGIN, midshield, midlife, 100);
		break;
	default:
		printf("error occured obj is not templars\n");
		return NULL;
	}
	dieobj_silently(a);
	dieobj_silently(a2);
	SetOBJIScriptNr(newobj, ISCRIPTNR_SPECIALSTATE1, ISCRIPTNR_EXECUTE);	//set warping archon
	ChangeTypeOfProp(newobj, PROPNORMAL2);
	SetBeginSelfConstruct(newobj);
	newobj->selfconstruct.currentconstrstep = CONSTR_STEP_WARPARCHONS;
	return(newobj);
}
//==========================================
struct OBJ *createobjfulllife(int x, int y, SCUNIT SC_Unit, int player)
{
	return(createobjmanwithlife(x, y, SC_Unit, player, 100, 100, 100, 1));
}
//==========================================
//sozdati OBJECT
struct OBJ *createobjmanwithlife(int x, int y, SCUNIT SC_Unit, int Pl,
	int persshield, int perslife, int persenergy, int supplyflag)
{
	OBJ *a;
	OBJ *a2 = NULL;
	OBJstruct *b;
	switch (SC_Unit)
	{
	case SC_ASSIMILATOROBJ:
	case SC_EXTRACTOROBJ:
	case SC_REFINERYOBJ:
		a2 = GetGEYSERfromMAP(x / SIZESPRLANSHX, y / SIZESPRLANSHY);
		if (!a2)
		{
			a2 = CreateGeyserOnMap(x, y, SC_GEYSEROBJ, Pl, persenergy >> 8, persenergy >> 8, &map);
		}
		if (!a2)
		{
			return(NULL);
		}
		break;
	case SC_MINERAL1OBJ:
	case SC_MINERAL2OBJ:
	case SC_MINERAL3OBJ:
		if (persenergy > 0xff)
			return(CreateMineralsOnMap(x, y, SC_GEYSEROBJ, Pl, persenergy, persenergy >> 8, &map));
		break;
	case SC_GEYSEROBJ:
		if (persenergy > 0xff)
			return (CreateGeyserOnMap(x, y, SC_GEYSEROBJ, Pl, persenergy, persenergy >> 8, &map));
		break;
	}
	a = createobjman(x, y, SC_Unit, Pl, 1);
	if (a2)
	{
		GeyserDisactivate(a, a2);
		a2->mainimage->HideChildsImgFlag();
		a2->mainimage->HideImgFlag();
	}
	b = loadobj(a->SC_Unit);
	AdditionalProperties(a);
	a->prop |= VARREADY;
	if (a->subunit)
		a->subunit->prop |= VARREADY;
	if (b)
	{
		SetUnitPercentHealth(a, perslife);
		SetUnitPercentShield(a, persshield);
		if (IsSpellCaster(SC_Unit))
			SetUnitPercentMana(a, persenergy);
	}
	ChangeTypeOfProp(a, b, PROPNORMAL1);

	a->prop |= VARKARTCHANGES;
	makeoneobjseeopen(a, b);
	calcfullinvandseeobj(a);

	if (b->creeptype != NOCREEPBUILD)//if have creep, maximize the creep at once
	{
		FillWithCreepNow(a, b, MAXCREEPTABLE);
	}
	if (supplyflag)
	{
		ChangeSupply(a->playernr, SC_Unit, PLUSFACTOR);
		ChangeMaxSupply(a->playernr, SC_Unit, PLUSFACTOR);
	}
	if (SC_Unit == SC_PYLONOBJ)
	{
		AddPylonArea(&map, a, a->playernr);
	}
	else
	{
		CheckBuildForPower(&map, a, a->playernr);
	}
	//if addon - need to atach mother build
	if (IsAddon(SC_Unit))
	{
		if (AttachAddonToBuild(a) == ADDON_ERROR_ATTACH)
		{
			a->playernr = GREYNEUTRALCOLORPLAYER;
		}
	}
	else
	{
		AttachNearestAddon(a);
	}
	//	  a->lastdamageoverlays = -1;
	AddRemoveBloodFlameOverlays(a);
	add_unit_stat(&map, UNITSTAT_HAVE, a->playernr, a->SC_Unit);
	addremoveuniteffectfrommap(a, 1, &map);
	if (a->modemove != MODECARRYME)
		SetModeMove(a, MODESTOP);
	return (a);
}
//==========================================
//sozdati OBJECT
struct OBJ *createobjman(int x, int y, SCUNIT SC_Unit, int playernr)
{
	return(createobjman(x, y, SC_Unit, playernr, 0));
}
//==========================================
//sozdati OBJECT
struct OBJ *createobjman(int x, int y, SCUNIT SC_Unit, int playernr, int readyatbegin)
{
	return (createobjman(x, y, SC_Unit, playernr, readyatbegin, 100, 100, 100));
}
//==========================================
struct OBJ *createobjman(int x, int y, SCUNIT SC_Unit, int playernr, int readyatbegin, int persshield, int perslife, int energypers)
{
	return(createobjlowlevel(NULL, x, y, SC_Unit, playernr, readyatbegin, persshield, perslife, energypers, NOLOIMAGE));
}
//==========================================
struct OBJ *createreschunk(OBJ *workerobj, int x, int y, SCUNIT SC_Unit)
{
	return(createobjlowlevel(workerobj, x, y, SC_Unit, GREYNEUTRALCOLORPLAYER, 1, 100, 100, 30, NOLOIMAGE));
}
//==========================================
//primary function to create an object
struct OBJ *createobjlowlevel(OBJ *workerobj, int x, int y, SCUNIT SC_Unit, int playernr,
	int readyatbegin, int persshield, int perslife, int energypers, unsigned short imagelo_id)
{
	OBJ *a;
	OBJ *a2 = NULL;
	OBJstruct *b;
	unsigned char subunitnr;
	if (SC_Unit == SC_ASSIMILATOROBJ || SC_Unit == SC_EXTRACTOROBJ || SC_Unit == SC_REFINERYOBJ)
	{
		a2 = GetGEYSERfromMAP(x / SIZESPRLANSHX, y / SIZESPRLANSHY);
		if (!a2)
			return(NULL);
	}

	a = (struct OBJ *)wmalloc(sizeof(struct OBJ));
	if (!a)
		return(NULL);
	memset(a, 0, sizeof(struct OBJ));
	if (a2)
	{
		GeyserDisactivate(a, a2);
	}

	b = loadobj(SC_Unit);
	if (IsDoodadState(SC_Unit))
	{
		if (IsInvincibleUnit(SC_Unit))
			playernr = GREYNEUTRALCOLORPLAYER;//doodad door,etc
	}
	else
	{
		if (IsGroupNeutralFlag(SC_Unit) && !IsNonNeutralFlag(SC_Unit))
			playernr = GREYNEUTRALCOLORPLAYER;//disruptwave,darkswarm
		else
			if (playernr >= PLAYEDPLAYERS && playernr != GREYNEUTRALCOLORPLAYER)
				playernr = GREYNEUTRALCOLORPLAYER;
	}

	//	  SetOrder(a,4,&SIGOrder_UnitInitComplete);

	a->startx = x;
	a->starty = y;
	a->lastdamageoverlays = -1;
	a->SC_Unit = SC_Unit;
	a->SC_ConstrUnit = SC_NOUNITNR;
	a->SC_FromUnit = b->SC_CreateFromUnit;
	if (a->SC_FromUnit != SC_NOUNITNR)
	{
		a->temphealth = GetUnitMaxHealth(a->SC_FromUnit);
		a->tempshield = GetUnitMaxShield(a->SC_FromUnit);
	}
	a->creepbuildlist_ID = CREEPLISTEMPTY;
	regenObjMap->AddObjList(a);

	//refreshMap->AddUpdateSequenseObjs();


	//	  for (i=0;i<PLAYEDPLAYERS;i++)
	//		  a->select[i] = b->UNITprop & (VARINVSEE|VARSEE);

	//	  OBJ_VAR_MASK_SET(a,obj_invsee,0xff);
	//	  OBJ_VAR_MASK_SET(a,obj_see,0xff);

	if (IsInvincibleUnit(a->SC_Unit))
		SetInvincibleOBJ(a, 1);
	if (IsAirUnit(a->SC_Unit))
		SetOnSkyOBJ(a, 1);
	a->playernr = playernr;
	if (IsSpellCaster(a->SC_Unit))//&&(!IsHallucination(a)) )
	{
		SetUnitPercentMana(a, MANAATBIRTH);
	}
	//	  ChangeObjXY(a,x,y);

	a->timeleft = b->maxtimeleft;
	if (!IsBuild(a->SC_Unit))
	{
		SetUnitPercentHealth(a, perslife);
		//		if (IsShieldEnable(SC_Unit))
		SetUnitPercentShield(a, persshield);
	}
	else
	{
		a->shield = STARTBUILDSHIELD;
		a->health = STARTBUILDLIFE;
	}
	//refreshMap->CreateObjRegen();
	a->sfxplay.sfxtypeplayed = -1;
	a->sfxplay.nrsfxplayed = -1;
	a->sfxplay.nrselectplayed = 0;
	if (b->UNITprop & VARSELECT4NOTSEEPROP)
		a->prop |= VARNOTSEEPROP;
	ChangeTypeOfProp(a, b, PROPDECONSTRUCT);
	if (workerobj)
	{
		//carry unit to worker
		workerobj->carryobj = a;
		a->carryobj = workerobj;
	}
	a->createcycle = gamecycle;
	a->color = PLAYER[playernr].colorRACE;
	if (IsPermanentCloak(a->SC_Unit))
		SetMageAtr(&a->atrobj, ATRINVISIBLE, ATRMAGEINFINITE);
	if (IsDetector(a->SC_Unit))
		SetMageAtr(&a->atrobj, ATRDETECTOR, ATRMAGEINFINITE);
	if (!IsHallucination(a))
	{
		if (TRIG_ChangeStat)
		{
			if (IsBuild(a->SC_Unit))
				PLAYER[playernr].statplayer.stat[STATPLAYER_BUILDSCONSTRUCTED] += GetBuildUnitScore(a->SC_Unit);
			else
				PLAYER[playernr].statplayer.stat[STATPLAYER_UNITSPRODUCED] += GetBuildUnitScore(a->SC_Unit);
		}
	}
	a->atackernr = -1;
	a->whoatack = NULL;
	SetAtackTick(a);
	add_unit_stat(&map, UNITSTAT_PRODUCING, a->playernr, a->SC_Unit);
	//	  SetModeMove(a,MODESTOP);
	a->modemove = MODESTOP;
	if (IsDoodadState(a->SC_Unit))
	{
		//		CreateImageAndAddToList(a,x<<8,y<<8,5,NOLOIMAGE);//5 is for doodadunit and no execute scripts
		CreateImageAndAddToList(a, x << 8, y << 8, readyatbegin, NOLOIMAGE);//5 is for doodadunit and no execute scripts
	}
	else
	{
		subunitnr = alldattbl.units_dat->Subunit1[SC_Unit];
		if (subunitnr < MAX_UNITS_ELEM)
		{
			a->subunit = createobjlowlevel(NULL, x, y, subunitnr, playernr, 3, 100, 100, 100, NOLOIMAGE);
			a->subunit->subunit = a;
		}
		//if subunit skip creation image (this is will do at base from initturret script)
		if (IsSubUnit(SC_Unit))
		{
			return(a);
		}
		CreateImageAndAddToList(a, x << 8, y << 8, readyatbegin, imagelo_id);
		if (a->subunit)
		{
			//subunit turn face like a base
			a->subunit->mainimage->AllUnitDirection256(a->mainimage->side);
		}
	}
	if (a->xkart >= MAXXMAP || a->ykart >= MAXYMAP)
	{
		sprintf(forexit, "coords error UNIT=%d (mapx,mapy)=(%d,%d)\n", SC_Unit, a->xkart, a->ykart);
		gameend(forexit);
	}
	calcfullinvandseeobj(a);
	if (!IsOnSkyOBJ(a))
		SetObjWalk8(&map, a, SETWALK);
	if (IsPickupUnit(a->SC_Unit) && !a->carryobj)
	{
		//try to attach to near worker if any
		a2 = SearchUnitFunc(a->playernr, &IsWorkerUnit, x, y, 24);
		if (a2)
			PickupObj(a2, a);
	}
	return(a);
}
//=====================================
void GeyserDisactivate(OBJ *a, OBJ *geyser)
{
	a->replaceobj = geyser;
	geyser->replaceobj = a;
	geyser->prop |= (VARCANTSELECT | VARNOTWORK);
	a->data.resource.resource_count = geyser->data.resource.resource_count;
	a->data.resource.max_resource_count = geyser->data.resource.max_resource_count;

	geyser->mainimage->DisableExecScript();//disable all overlays exec scripts

}
//=====================================
#define SELECTUNITTYPES						8
#define SELECT_BUILD_ENEMY					ENEMYOBJ
#define SELECT_BUILD_NEUTRAL				NEUTRALOBJ
#define SELECT_BUILD_ALLIANCE				ALLIANCEOBJ
#define SELECT_BUILD_MY						MYOBJ
#define SELECT_UNIT_ENEMY					(ENEMYOBJ + 4)
#define SELECT_UNIT_NEUTRAL					(NEUTRALOBJ + 4)
#define SELECT_UNIT_ALLIANCE				(ALLIANCEOBJ +4)
#define SELECT_UNIT_MY						(MYOBJ + 4)

static int selectunittypes[SELECTUNITTYPES];
//=====================================
int GetSelectedUnitType(SCUNIT SC_Unit, int playernr)
{
	if (IsBuild(SC_Unit))
		return(player_aliance(NUMBGAMER, playernr));
	else
		return(player_aliance(NUMBGAMER, playernr) + 4);
}
//=====================================

void deselectallexcludeone(struct OBJ *a1)
{
	struct OBJ *a;
	int i;
	SelectedUnits.EnumListInit();
	while ((a = (OBJ *)SelectedUnits.GetNextListElem(&i)))
	{
		if (a != a1)
		{
			SelectedUnits.MarkForRemove(i);
			doselectedOBJbit(a, NUMBGAMER, 0);
		}
	}
	SelectedUnits.RemoveMarked();
}
//=====================================
void deselectallexcludeonetypeobj(struct OBJ *a1)
{
	struct OBJ *a;
	int i;
	SelectedUnits.EnumListInit();
	while ((a = (OBJ *)SelectedUnits.GetNextListElem(&i)))
	{
		if (a->SC_Unit != a1->SC_Unit)
		{
			SelectedUnits.MarkForRemove(i);
			doselectedOBJbit(a, NUMBGAMER, 0);
		}
	}
	SelectedUnits.RemoveMarked();
}
//=====================================
void DetectIfAnyPylonOnSelected(void)
{
#ifdef DESENPYLONAREA
	SCUNIT SC_Unit;
	pylonselected = 0;
	if (highMouse->Construct.SC_BuildUnit != SC_NOUNITNR)
	{
		SC_Unit = highMouse->Construct.SC_BuildUnit;
		if (GetUnitRace(SC_Unit) == PROTOSSRACE)
		{
			switch (SC_Unit)
			{
			case SC_NEXUSOBJ:
			case SC_ASSIMILATOROBJ:
				break;
			default:
				pylonselected = 1;
				return;
			}
		}
	}
	OBJ *o;
	SelectedUnits.EnumListInit();
	while ((o = (OBJ *)SelectedUnits.GetNextListElem(NULL)))
	{
		if (o->SC_Unit == SC_PYLONOBJ &&
			o->playernr == NUMBGAMER &&
			!IsOBJUnderConstruct(o))
		{
			pylonselected = 1;
			break;
		}
	}
#endif
}
//=====================================
void deselectvars(void)
{
	for (int i = 0;i < SELECTUNITTYPES; i++)
		selectunittypes[i] = 0;
}
//=====================================
void deselectallobj(int playernr)
{
	OBJ *o;
	SelectedUnits.EnumListInit();
	while ((o = (OBJ *)SelectedUnits.GetNextListElem(NULL)))
	{
		doselectedOBJbit(o, playernr, 0);
	}
	SelectedUnits.FreeAndEmptyAll();
}
//=====================================
void addtolist_onetypeobj(SelectionObjs *list, OBJ *a, int x1, int y1, int x2, int y2)
{
	int i, x, y;
	struct OBJ *c;
	for (i = 0;i < MaxObjects;i++)
	{
		c = objects[i];
		if (IsInvincibleUnit(c->SC_Unit))
			continue;
		if (c->playernr == a->playernr && a->SC_Unit == c->SC_Unit)
		{
			x = GetOBJx(c);
			y = GetOBJy(c);
			if (!OBJ_VAR_CHK(c, obj_notdetect, NUMBGAMER))
			{
				if (x > x1 - GetUnitWidthAndHeight(a->SC_Unit, UNITDIM_WIDTH) / 2 &&
					y > y1 - GetUnitWidthAndHeight(a->SC_Unit, UNITDIM_HEIGHT) / 2 &&
					x < x2 + GetUnitWidthAndHeight(a->SC_Unit, UNITDIM_WIDTH) / 2 &&
					y < y2 + GetUnitWidthAndHeight(a->SC_Unit, UNITDIM_HEIGHT) / 2)
				{
					list->Add(c);
				}
			}
		}
	}//for
}//end func
//=====================================
struct OBJ *founduniqueobj(int x1, int y1)
{
	int i, nodoodad, x, y, unittype;
	struct OBJ *o;
	SelectionObjs *selectunique[SELECTUNITTYPES] =
	{
		new SelectionObjs(1),
		new SelectionObjs(1),
		new SelectionObjs(1),
		new SelectionObjs(1),
		new SelectionObjs(1),
		new SelectionObjs(1),
		new SelectionObjs(MAXSELECTMAN),
		new SelectionObjs(MAXSELECTMAN),
	};
	for (i = 0;i < MaxObjects;i++)
	{
		o = objects[i];
		if (o->mainimage->flags & SC_IMAGE_FLAG_DISABLEDRAW)
			continue;
		nodoodad = 1;
		if (IsDoodadState(o->SC_Unit) && (IsInvincibleUnit(o->SC_Unit) || GetDoodadState(o) == DOODAD_BOTTOM_STATE))
			nodoodad = 0;
		if (IfCanClickOBJ(o->SC_Unit) && nodoodad && (!o->carryobj || !IsPickupUnit(o->SC_Unit)))
		{
			x = GetOBJx(o);
			y = GetOBJy(o);
			if (x > x1 - GetUnitWidthAndHeight(o->SC_Unit, UNITDIM_WIDTH) / 2 &&
				y > y1 - GetUnitWidthAndHeight(o->SC_Unit, UNITDIM_HEIGHT) / 2 &&
				x < x1 + GetUnitWidthAndHeight(o->SC_Unit, UNITDIM_WIDTH) / 2 &&
				y < y1 + GetUnitWidthAndHeight(o->SC_Unit, UNITDIM_HEIGHT) / 2)
			{
				if (IfUnitIsSelectable(o))
				{
					unittype = GetSelectedUnitType(o->SC_Unit, o->playernr);
					selectunique[unittype]->Add(o);
				}
			}
		}
	}//for
	o = NULL;
	for (i = SELECTUNITTYPES - 1; i >= 0; i--)
	{
		if (selectunique[i]->Count())
		{
			o = selectunique[i]->GetElem(0,NULL);
			break;
		}
	}
	for (i = 0;i < SELECTUNITTYPES; i++)
	{
		delete selectunique[i];
	}
	return(o);
}//end func
//=====================================
void ifselectedprobe(void)
{
	OBJ *o;
	SelectedUnits.EnumListInit();
	while ((o = (OBJ *)SelectedUnits.GetNextListElem(NULL)))
	{
		if (IsWorkerUnit(o->SC_Unit))
		{
			if (IsOBJBurrowed(o))
				ChangeTypeOfProp(o, PROPNORMAL2);
			else
				if (!o->constrobj)
					ChangeTypeOfProp(o, PROPNORMAL1);
		}
	}
}
//=====================================
void ifselectTEMPLARS(void)
{
	if (SelectedUnits.Count() >= 2)
	{
		warparchon = 1;
	}
	else
	{
		warparchon = 0;
	}
}
//=====================================
void ifselectTRANSPORTS(struct OBJ *a)
{
	if (a)
	{
		if (GetSpaceProvided(a->SC_Unit))
			AnalizeAllTransportUnits(a, transportplace, &transportplaceobj);
		else
			transportplaceobj = NULL;
	}
}
//=====================================
int IfUnitIsSelectable(OBJ *a)
{
	if (OBJ_VAR_MASK_CHK(a, obj_notdetect, map.pl_visionbits[NUMBGAMER]))
	{
		if (OBJ_VAR_MASK_CHK(a, obj_invsee, map.pl_visionbits[NUMBGAMER]))
			return(1);
	}
	else
	{
		if (OBJ_VAR_MASK_CHK(a, obj_see, map.pl_visionbits[NUMBGAMER]))
			return(1);
	}
	return(0);
}
//=====================================
int IfUnitIsSelectable2(OBJ *a)
{
	if (OBJ_VAR_CHK(a, obj_notdetect, NUMBGAMER))
	{
		if (OBJ_VAR_CHK(a, obj_invsee, NUMBGAMER))
			return(1);
	}
	else
	{
		if (OBJ_VAR_CHK(a, obj_see, NUMBGAMER))
			return(1);
	}
	return(0);
}
//=====================================
#define MINHAP 2
void selectMAN(int x1, int y1, int x2, int y2, int mode)
{
	int i, x, y, unittype, totalselected = 0;
	OBJ *firstobj, *a, *speakOBJ;
	SelectionObjs *selectnow[SELECTUNITTYPES] =
	{
		new SelectionObjs(1),
		new SelectionObjs(1),
		new SelectionObjs(1),
		new SelectionObjs(1),
		new SelectionObjs(1),
		new SelectionObjs(1),
		new SelectionObjs(MAXSELECTMAN),
		new SelectionObjs(MAXSELECTMAN),
	};
	deselectvars();
	if (x2 < x1)
		swap(&x1, &x2);
	if (y2 < y1)
		swap(&y1, &y2);
	x1 = x1 + map.MAPXGLOBAL;
	x2 = x2 + map.MAPXGLOBAL;
	y1 = y1 + map.MAPYGLOBAL;
	y2 = y2 + map.MAPYGLOBAL;
	a = NULL;
	//int ctrl = KEYPRESS(CTRLLKEY) || KEYPRESS(CTRLRKEY) || highMouse->DoubleClick;
	int ctrl = (mainController.KeyFlags & CTRLKEYMASK) || highMouse->DoubleClick;
	if (ctrl || ((x2 - x1 <= MINHAP) && (y2 - y1 <= MINHAP)))
		a = founduniqueobj(x1, y1);
	if (a)
	{
		unittype = GetSelectedUnitType(a->SC_Unit, a->playernr);
		if (ctrl)
		{
			addtolist_onetypeobj(selectnow[unittype], a,
				map.MAPXGLOBAL,
				map.MAPYGLOBAL,
				map.MAPXGLOBAL + screenMapInfo->SizeX32 * SIZESPRLANSHX,
				map.MAPYGLOBAL + screenMapInfo->SizeY32 * SIZESPRLANSHY);
			totalselected = selectnow[unittype]->Count();
		}
		else
		{
			selectnow[unittype]->Add(a);
			totalselected++;
		}
	}
	else
	{
		for (i = 0;i < MaxObjects;i++)
		{
			a = objects[i];
			if (a->mainimage->flags & SC_IMAGE_FLAG_DISABLEDRAW)
				continue;
			bool nodoodad = true;
			if (IsDoodadState(a->SC_Unit) && (IsInvincibleUnit(a->SC_Unit) || GetDoodadState(a) == DOODAD_BOTTOM_STATE))
				nodoodad = false;
			if (IfCanClickOBJ(a->SC_Unit) && nodoodad && (!a->carryobj || !IsPickupUnit(a->SC_Unit)))
			{
				x = GetOBJx(a);
				y = GetOBJy(a);
				if (x > x1 - GetUnitWidthAndHeight(a->SC_Unit, UNITDIM_WIDTH) / 2 &&
					y > y1 - GetUnitWidthAndHeight(a->SC_Unit, UNITDIM_HEIGHT) / 2 &&
					x < x2 + GetUnitWidthAndHeight(a->SC_Unit, UNITDIM_WIDTH) / 2 &&
					y < y2 + GetUnitWidthAndHeight(a->SC_Unit, UNITDIM_HEIGHT) / 2)
				{
					if (IfUnitIsSelectable(a))
					{
						unittype = GetSelectedUnitType(a->SC_Unit, a->playernr);
						selectnow[unittype]->Add(a);
						totalselected++;
					}
				}
			}
		}
	}
	if (mode || totalselected)
	{
		//deselect all selected units
		SelectedUnits.EnumListInit();
		while ((a = (OBJ *)SelectedUnits.GetNextListElem(NULL)))
		{
			doselectedOBJbit(a, NUMBGAMER, 0);
		}

		if (!mode)
		{
			for (i = SELECTUNITTYPES - 1; i >= 0; i--)
				if (selectnow[i]->Count())
				{
					selectnow[i]->CopyTo(&SelectedUnits);
					SelectedUnitTypes = i;
					break;
				}
		}
		else
		{
			if (SelectedUnitTypes == SELECT_UNIT_MY)
				selectnow[SELECT_UNIT_MY]->AppendTo(&SelectedUnits);
			else
				selectnow[SELECT_UNIT_MY]->CopyTo(&SelectedUnits);
			SelectedUnitTypes = SELECT_UNIT_MY;
		}
		SelectedUnits.EnumListInit();
		while ((a = (OBJ *)SelectedUnits.GetNextListElem(NULL)))
		{
			doselectedOBJbit(a, NUMBGAMER, 1);
		}
		if (SelectedUnits.Count())
		{
			ifselectedprobe();
			firstobj = (OBJ *)SelectedUnits.GetElem(0, NULL);
			if (firstobj->prop & VARREADY)
			{
				if (firstobj->prop & VARPOWEROFF)
				{
					if (gameconf.audioconf.buildsounds)
						if (firstobj && firstobj->playernr == NUMBGAMER)
							if (mapSEE(firstobj, NUMBGAMER) > 1)
								Play_sfxdata_id(firstobj, sfx_powerdown[GetUnitRace(firstobj->SC_Unit)], 2, 0);
				}
				else
				{
					speakOBJ = GetMaxRankOBJ(&SelectedUnits);
					if (speakOBJ)
						activatesound(speakOBJ, MODESOUNDSELECT, 0, NOSTOPCURRENTSOUNDS);
				}
			}
		}
		ifselectTEMPLARS();
		if (SelectedUnits.Count() == 1)
		{
			OBJ *o = (OBJ *)SelectedUnits.GetElem(0, NULL);
			ifselectTRANSPORTS(o);
			ifselectedDAMAGEDBUILD(o);
		}
		else
		{
			transportplaceobj = NULL;
		}
	}
	for (i = 0;i < SELECTUNITTYPES; i++)
	{
		delete selectnow[i];
	}
}
//=====================================
void changedeselectobj(struct OBJ *a, struct OBJ *aa)
{
	int i;
	OBJ *o;
	SelectedUnits.EnumListInit();
	while ((o = (OBJ *)SelectedUnits.GetNextListElem(&i)))
	{
		if (o == a)
		{
			SelectedUnits.Set(i, aa);
			selectobj(aa);
			return;
		}
	}
}
//========================================
int selectobj(struct OBJ *a)
{
	if (addselectobj(a) == 1)
	{
		OBJ *o;
		SelectedUnits.EnumListInit();
		while ((o = (OBJ *)SelectedUnits.GetNextListElem(NULL)))
		{
			if (o == a)
			{
				doselectedOBJbit(o, NUMBGAMER, 1);
				AddCircleImage(a);
				return(1);
			}
		}
	}
	return(0);
}
//========================================
int deselectobj(struct OBJ *a)
{
	int i = SelectedUnits.RetIfContains(a);
	if (i >= 0)
	{
		SelectedUnits.Remove(i);
		if (GetPortraitOBJ() == a)
			ShowAdvisorPortrait();
		doselectedOBJbit(a, NUMBGAMER, 0);
		DelCircleImage(a);
	}
	return(0);
}
//========================================
int addselectobj(struct OBJ *a)
{
	if (a->prop & VARCANTSELECT)
		return 0;
	if (!SelectedUnits.Contains(a))
	{
		SelectedUnits.Add(a);
	}
	return(0);
}
//========================================
#define REGENERATESHIELD		5
#define REGENERATELIFE			2
#define DEREGENERATELIFE		10
#define DEREGENERATETEMPLIFE	15
//#define DELTAMANAONCYCLE		7		//add mana in 1 cycle	in mage.h

//========================================
int RegenerateShield(OBJ *a)
{
	int change = 0;
	if (a->shield < GetUnitMaxShield(a->SC_Unit))
	{
		AddUnitShield(a, REGENERATESHIELD);
	}
	return(change);
}
//========================================
int RegenerateHealth(OBJ *a)
{
	int change = 0;
	if (IsInvincibleUnit(a->SC_Unit))
		return change;
	if (IsRegenerate(a->SC_Unit))
	{
		if (a->health < GetUnitMaxHealth(a->SC_Unit))
		{
			if (IsOBJBurrowed(a))
				AddUnitHealth(a, REGENERATELIFE + 1);
			else
				AddUnitHealth(a, REGENERATELIFE);
			change = 1;
		}
	}
	else
	{
		if (IsBuild(a->SC_Unit) && GetUnitRace(a->SC_Unit) == TERRANRACE)
			if (a->health * 3 < (GetUnitMaxHealth(a->SC_Unit)))
			{
				a->health -= DEREGENERATELIFE;
				change = 1;
				if (a->health <= 0)
					dieobj(a);
			}
	}
	return(change);
}
//========================================
int DecrementLifeMage(struct OBJ *a)
{
	if (a->timeleft > 0)
	{
		if (--a->timeleft <= 0)
		{
			dieobj(a);
			return -1;
		}
	}
	return 0;
}
//========================================
void allobjdecrmtimemage(void)
{
	int i;
	struct OBJ *a;
	for (i = 0;i < MaxObjects;i++)
	{
		a = objects[i];
		DecrementOBJAtr(a);
	}
}
//========================================
int ApplyDamageToUnit(struct OBJ *a)
{
	unsigned short image_id, imgflags;
	int lifechange;
	MAIN_IMG *img;
	OVERLAY_IMG *newimg;

	lifechange = 0;
	if (a->psistormdamage)
	{
		if (IsHallucination(a))
		{
			dieobj(a);
			return(2);
		}
		LowLevelDamage(NULL, a, WEAPONID_PSISTORM, DAMAGE_IGNOREARMOR, a->psistormdamage, 0, DAMAGE_SPLASH | DAMAGE_NOSHOWHIT);
		a->psistormdamage = 0;
	}
	if (a->dmatrixdamage)
	{
		//show dmatrixpicture;
		img = a->mainimage;
		image_id = IMAGEID_DEFENSIVEMATRIXHIT1 + GetUnitOverlaySize(a->SC_Unit);
		//permits only one image of damage to matrixshield
		if (!CheckForSpecificChildsImageID(img, image_id, image_id))
		{
			//show damage to defensive matrix
			imgflags = 0;
			if (IsOnSkyOBJ(a))
				imgflags = SC_IMAGE_FLAG_AIRIMG;
			newimg = new OVERLAY_IMG(img, image_id, 0, 0, a->mainimage->elevationlevel + 1, imgflags | SC_IMAGE_FLAG_IMGOVER, ISCRIPTNR_INIT);
		}
		a->dmatrixdamage = 0;
	}
	if (a->shielddamage > 0)
	{
		a->shield -= a->shielddamage;
		if (a->shield < 0)
		{
			DEBUGMESSCR("shield/shielddamage:%d/%d\n", a->shield, a->shielddamage);
		}
	}
	if (a->lifedamage > 0)
	{
		a->health -= a->lifedamage;
		if (a->health <= 0)
		{
			dieobj(a);
			lifechange = 2;
		}
		else
			lifechange = 1;
	}
	a->lifedamage = 0;
	a->shielddamage = 0;
	return(lifechange);
}
//========================================
void allobj_dieheal(void)
{
	int i, lifechange;
	struct OBJ *a;
	for (i = 0;i < MaxObjects;i++)
	{
		a = objects[i];
		if (a->modemove == MODEDIE)
			continue;
		lifechange = ApplyDamageToUnit(a);
		if (lifechange == 2)
			continue;
		//		  if (a->modemove == MODEDIE)
		//			continue;
		mageattributedothings(a);
		if (IsShieldEnable(a->SC_Unit))
			if (!a->shielddamage)
				lifechange += RegenerateShield(a);
		if (!IsOBJUnderConstruct(a))
		{
			if (!a->lifedamage)
				lifechange += RegenerateHealth(a);
			DecrementLifeMage(a);
		}
		if (lifechange)
		{
			AddRemoveBloodFlameOverlays(a);
		}
	}
}
//========================================
void allobjconstr(void)
{
	int i;
	struct OBJ *a;
	for (i = 0;i < MaxObjects;i++)
	{
		a = objects[i];
		TickUnderConstruct(a);	//perform construct build
		if (!(a->prop&VARNOTWORK))
			workingbuilds(a);
	}
}
//=============================================
#define FACTORINV 3
int makeinvisibles(struct OBJ *a)
{
	int i, deltafactor, distance;
	if (!IsOBJBurrowed(a))
	{
		i = a->mainimage->invisiblecolors;
		if (GetMageAtr(&a->atrobj, ATRINVISIBLE) == 0) //nevidimii obj
			deltafactor = -FACTORINV;
		else
			deltafactor = FACTORINV;
		i += deltafactor;
		if (i < 0)
			i = MININVISIBLECOLOR;
		else
			if (i > MAXINVISIBLECOLOR)
				i = MAXINVISIBLECOLOR;
		if (deltafactor > 0)
		{
			if (!IsCloaked(a))
			{
				//need to play cloak sound
				SetCloakedFlag(a, 1);
				if (mapSEE(a->xkart, a->ykart) > 1)
				{
					distance = (int)hypot(GetOBJx(a) - map.MAPXGLOBAL, GetOBJy(a) - map.MAPYGLOBAL);
					Play_sfxdata_id(NULL, SFXDATA_CLOAKIN, 2, distance);
				}
			}
		}
		else
			if (IsCloaked(a))
			{
				//need to play decloak sound
				SetCloakedFlag(a, 0);
				if (mapSEE(a->xkart, a->ykart) > 1)
				{
					distance = (int)hypot(GetOBJx(a) - map.MAPXGLOBAL, GetOBJy(a) - map.MAPYGLOBAL);
					Play_sfxdata_id(NULL, SFXDATA_CLOAKOUT, 2, distance);
				}
			}
		a->mainimage->invisiblecolors = i;
		return(i);
	}
	return(a->mainimage->invisiblecolors);
}
//========================================
void invisiblestick(void)
{
	int i;
	OBJ *a;
	for (i = 0;i < MaxObjects;i++)
	{
		a = objects[i];
		if (a->modemove == MODEDIE || IsInvincibleOBJ(a))
			continue;
		if (IsDoodadState(a->SC_Unit))
			continue;
		if (GetMageAtr(&a->atrobj, ATRSTASIS) == 0)
		{
			if (a->prop & VARNOTHERE)
				continue;
			if (makeinvisibles(a))
			{
				if (OBJ_VAR_CHK(a, obj_invsee, NUMBGAMER))
				{
					//invisible detected
					if (IsOBJBurrowed(a))
						a->mainimage->newgrpmethod = DRAWBURROWED;
					else
						a->mainimage->newgrpmethod = TRANSPARENT;
				}
				else
				{
					//invisible notdetected
					a->mainimage->newgrpmethod = DISTORTION;
				}
			}
			else
			{
				//unit become full visible,change image grpmethod
				SetCloakedFlag(a, 0);
				a->mainimage->newgrpmethod = NORMAL;
			}
		}
		else
		{
			a->mainimage->newgrpmethod = TRANSPARENT;
		}
	}
}
//=============================================
void ChangeSubUnitCoords(OBJ *turret, OBJ *base)
{
	turret->mainimage->xpos = base->mainimage->xpos;
	turret->mainimage->ypos = base->mainimage->ypos;
}
//=============================================
void CreateImageAndAddToList(OBJ *a, int x256, int y256, int readyatbegin, unsigned short imagelo_id)
{
	unsigned char initscriptnr;
	unsigned short constr_id = 0;
	MAIN_IMG *img;
	int groundair, x, y;
	x = x256 >> 8;
	y = y256 >> 8;
	initscriptnr = ISCRIPTNR_INIT;
	switch (readyatbegin)
	{
	case 0:
		if (IsBuild(a->SC_Unit))
		{
			//prevent geyser to produce smokes
			if (a->replaceobj)
				a->replaceobj->mainimage->DisableExecScript();//disable all overlays exec scripts
			if (IsProtossRace(a->SC_Unit))
			{
				constr_id = IMAGEID_WARPANCHOR;
				Play_sfxdata(x, y, SFXDATA_PROTOSSBUILDWARPBEGIN, 2);
			}
			else
			{
				if (a->replaceobj)
				{
					//						a->replaceobj->mainimage->HideChildsImgFlag();
					//						a->replaceobj->mainimage->HideImgFlag();
				}
				constr_id = alldattbl.units_dat->ConstructionAnimation[a->SC_Unit];
			}
		}
		else
			constr_id = alldattbl.units_dat->ConstructionAnimation[a->SC_Unit];
		break;
	case 1:
	case 2:
	case 3:
		constr_id = 0;
		break;
	case 4://zerg unit birth
		break;
	case 5:
		initscriptnr = ISCRIPTNR_UNKNOWN;//for doodad unit
		break;
	}
	groundair = IsOnSkyOBJ(a);
	img = OBJCreateImage(a, x256, y256, initscriptnr, groundair, constr_id, imagelo_id);
	mainimageslist.Add(img);
	if (IsBuild(a->SC_Unit))
	{
		switch (readyatbegin)
		{
		case 0:
			SetBeginSelfConstruct(a);				//need to be constructed
			if (IsProtossRace(a->SC_Unit))
				a->selfconstruct.currentconstrstep = CONSTR_STEP_PROTOSSBUILD;
			else if (IsZergRace(a->SC_Unit))
				a->selfconstruct.currentconstrstep = CONSTR_STEP_ZERGBUILDLESS33;
			else if (IsTerranRace(a->SC_Unit))
				a->selfconstruct.currentconstrstep = CONSTR_STEP_TERRANBUILDLESS25;
			break;
		case 1:
			img->SetIScriptNr(ISCRIPTNR_BUILT);		//complete construction
			iscriptinfo.ExecuteScript(img);
			break;
		case 2:
			img->SetIScriptNr(ISCRIPTNR_ALMOSTBUILT);//begin appear at end of construction
			break;
		case 3:
			break;
		}
	}
}
//==================================
void ChangeUnitSubUnitAndImagesAssociated(OBJ *a, SCUNIT SC_NewUnit)
{
	int x256, y256;
	x256 = GetOBJx256(a);
	y256 = GetOBJy256(a);

	a->mainimage->DeleteMainImgAndChilds();
	a->mainimage = NULL;
	a->SC_Unit = SC_NewUnit;

	a->subunit->mainimage->DeleteMainImgAndChilds();
	a->subunit->mainimage = NULL;
	a->subunit->SC_Unit = alldattbl.units_dat->Subunit1[SC_NewUnit];

	CreateImageAndAddToList(a, x256, y256, 2, NOLOIMAGE);

}
//==================================
void ChangeUnitAndImagesAssociated(OBJ *a, SCUNIT SC_NewUnit)
{
	int x256, y256;
	x256 = GetOBJx256(a);
	y256 = GetOBJy256(a);
	a->mainimage->DeleteMainImgAndChilds();
	a->mainimage = NULL;
	a->SC_Unit = SC_NewUnit;
	CreateImageAndAddToList(a, x256, y256, 2, NOLOIMAGE);
}
//==================================
void ReleaseResource(OBJ *a)
{
	if (a->resourceobj)
	{
		a->resourceobj->resourceobj = NULL;
		a->resourceobj = NULL;
	}
}
//==================================
int moveobj_addonmode(struct OBJ *a, struct OBJ *destobj, int mode, int x, int y, int modemoveflags)
{
	OBJ *newobj;
	int deltax, deltay;
	AttachAddonDeltaCoords(a->SC_Unit, mode, &deltax, &deltay);
	if (x - GetOBJx(a) == deltax && y - GetOBJy(a) == deltay)
	{
		//the near position
		DoOrder(a->playernr, a, x, y, mageprop[mode].type_id, mageprop[mode].obj_id,
			mageprop[mode].obj_id, mageprop[mode].stattxt_id_enable, mode, modemoveflags);
		//		moveobj(a,NULL,x,y,a->playernr,mode,modemoveflags);

		newobj = createobjman(x, y, mageprop[mode].obj_id, a->playernr);
		newobj->health = STARTBUILDLIFE;
		newobj->shield = STARTBUILDSHIELD;
		a->constrobj = newobj;
		newobj->constrobj = a;
		a->addonobj = newobj;
		newobj->addonobj = a;
		ChangeTypeOfProp(a, PROPDECONSTRUCT);
		return(MOVEOBJ_DONE);
	}
	else
	{
		if (IfHaveNextModeMove(a))
		{
			moveobj(a, NULL, MODEMOVE, x - deltax, y - AIRUNITS_START_YPOS - deltay, modemoveflags);			//move to land destination
			AddModeMove(a, NULL, MODELANDING, x - deltax, y - deltay, modemoveflags);						//land to destination
			AddModeMove(a, NULL, mode, x, y, modemoveflags);													//create addon after
		}
		else
		{
			moveobj(a, NULL, MODELIFTOFF, x - deltax, y - deltay, modemoveflags);	//liftoff
			AddModeMove(a, NULL, MODEMOVE, x - deltax, y - AIRUNITS_START_YPOS - deltay, modemoveflags);		//move to land destination
			AddModeMove(a, NULL, MODELANDING, x - deltax, y - deltay, modemoveflags);						//land to destination
			AddModeMove(a, NULL, mode, x, y, modemoveflags);													//create addon after
		}
		//		printf("deltax=%d deltay=%d\n",x-GetOBJx(a),y-GetOBJy(a));
				//far position
	}
	return(MOVEOBJ_DONE);
}
//==================================
int moveobj_buildmode(struct OBJ *a, struct OBJ *destobj, int mode, int x, int y, int modemoveflags)
{
	OBJ *newobj;
	int mcost, gcost, x256, y256, constrerror;
	unsigned char obj_id;
	obj_id = mageprop[mode].obj_id;
	GetCostUnit(obj_id, &mcost, &gcost);
	if (IsBuild(a->SC_Unit))
	{
		if (a->SC_Unit == obj_id)
		{
			newobj = createobjman(x, y, SC_NYDUSCANALOBJ, a->playernr);
			newobj->health = GetUnitMaxHealth(a->SC_Unit);
			ChangeTypeOfProp(newobj, PROPEMPTY);
			ChangeTypeOfProp(a, PROPEMPTY);
			a->doubleunit = newobj;
			newobj->doubleunit = a;
			newobj->SC_FromUnit = a->SC_Unit;
		}
		else
		{
			//build make build (usually zerg building upgrade)
			a->SC_ConstrUnit = obj_id;							//construct unit
			a->SC_FromUnit = a->SC_Unit;						//save previous unit
			a->temphealth = a->health;
			a->tempshield = a->shield;
			x256 = GetOBJx256(a);
			y256 = GetOBJy256(a);
			a->mainimage->DeleteMainImgAndChilds();

			ChangeSC_Unit(a, a->playernr, obj_id, CHANGESC_UNIT_CONSTR);
			CreateImageAndAddToList(a, x256, y256, 0, NOLOIMAGE);
			SetUnitPercentHealth(a, 100);
			SetUnitPercentShield(a, 100);
			ChangeResourcePlayer(a->playernr, MINUSFACTOR, mcost, gcost);
			SetBeginSelfConstruct(a);
			a->selfconstruct.currentconstrstep = CONSTR_STEP_ZERGBUILDLESS99;
			SetOBJIScriptNr(a, ISCRIPTNR_SPECIALSTATE2, ISCRIPTNR_SETONLY);
			ChangeTypeOfProp(a, PROPDECONSTRUCT);
			//			a->lastdamageoverlays = -1;
			AddRemoveBloodFlameOverlays(a);				//remove any bloodflame if any
			return(MOVEOBJ_DONE);
		}
	}
	else
	{
		//unit make build
		if (a->carryobj && IsNonNeutralFlag(a->carryobj->SC_Unit))
			return(MOVEOBJ_NOACT);
		if (GetDistanceTo(a, x, y) > mageprop[mode].mindistance / 256 + 10)
		{
			ChangeTypeOfProp(a, PROPNORMAL1);
			a->modemove = mode;
			//move to destination
			initmoveaction(a, destobj, mode, 0, 0, x, y);
			if (destobj)
				a->data.geyserdest.obj = destobj;
			AddModeMove(a, NULL, mode, x, y, modemoveflags);//create build after
		}
		else
		{
			//change x,y to correct geyser coords
			if (obj_id == SC_EXTRACTOROBJ ||
				obj_id == SC_REFINERYOBJ ||
				obj_id == SC_ASSIMILATOROBJ)
			{
				//geyser under worker
				destobj = a->data.geyserdest.obj;
				if (!destobj)
				{
					DEBUGMESSCR("geyser is NULL\n");
					moveobj(a, NULL, MODESTOP, NOSHOWERROR);
					return(MOVEOBJ_NOACT);
				}
			}
			//			a->finalOBJ = destobj;
						//check if can construct on this place
			if (!CheckIfCanBuild(a, obj_id, x, y, &constrerror))
			{
				moveobj(a, NULL, MODESTOP, NOSHOWERROR);
				if (a->playernr == NUMBGAMER && (modemoveflags & SHOWERRORTEXT))
				{
					activatesound(a, MODESOUNDERROR, 2, NOSTOPCURRENTSOUNDS);
					putbuildplacemessage(-constrerror);
				}
				return(MOVEOBJ_NOACT);
			}
			//createbuild
			//check ass/ref/ext
			switch (a->SC_Unit)
			{
			case SC_PROBEOBJ:
				if (!DecrUnitCost(obj_id, a->playernr))
				{
					moveobj(a, NULL, MODESTOP, NOSHOWERROR);
					return(MOVEOBJ_NOACT);
				}
				createobjman(x, y, obj_id, a->playernr, 0, 1, 1, MANAATBIRTH);
				break;
			case SC_DRONEOBJ:
				//doliftdown
				SetModeMove(a, MODELANDING);
				SetOBJIScriptNr(a, ISCRIPTNR_LANDING, ISCRIPTNR_EXECUTE);
				a->deltavertpos = DRONE_START_YPOS;
				a->SC_ConstrUnit = obj_id;
				a->mainimage->UnitNeededDeltaDirection256(+128);
				break;
			case SC_SCVOBJ:
				if (!DecrUnitCost(obj_id, a->playernr))
				{
					moveobj(a, NULL, MODESTOP, NOSHOWERROR);
					return(MOVEOBJ_NOACT);
				}
				//create build
				newobj = createobjman(x, y, obj_id, a->playernr, 0, 1, 1, MANAATBIRTH);
				//lets begin construct
				ContinueSCVConstruct(a, newobj);
				SetModeMove(a, MODECONSTRUCT);
				break;
			}
		}
		return(MOVEOBJ_DONE);
	}
	return(MOVEOBJ_NOACT);
}
//==================================
int moveobj_unitmode(struct OBJ *a, struct OBJ *destobj, int mode, int x, int y, int modemoveflags)
{
	int needmana;
	OBJ *newobj;
	unsigned char obj_id;
	obj_id = mageprop[mode].obj_id;
	switch (obj_id)
	{
	case SC_SCANNERSWEEP:
		if (MageDepend(a, a->playernr, mode))
		{
			needmana = GetTechEnergyCost(mageprop[mode].depend.obj_id[0]);
			if (CheckForMana(a, needmana) == CHECKRES_OK)
			{
				DecrMana(a, needmana);
				newobj = createobjfulllife(x, y, obj_id, a->playernr);
				makeoneobjseeopen(newobj, loadobj(newobj->SC_Unit));
				//if scaner apear on closed area we don not hear 'music from sweepscan' so we need to
				//activate it again and prevent to hear it doubled if not
				activatesound(newobj, MODESOUNDREADY, 1, NOSTOPCURRENTSOUNDS);
				SetOrder(a, 1, &SIGOrder_ScannerSweepEnd);
			}
		}
		break;
	case SC_LURKEROBJ:
		UnitConstrUnit(a, obj_id, -1, -1);
		break;
	default:
		DEBUGMESSCR("unit %d make unit %d, not develop\n", a->SC_Unit, obj_id);
		break;
	}
	return(MOVEOBJ_DONE);
}
//==================================
short TARGETERROR[3] = { SFXDATA_ERRORTARGET_ZERG,SFXDATA_ERRORTARGET_TERRAN,SFXDATA_ERRORTARGET_PROTOSS };
//==================================
int moveobj(struct OBJ *a, struct OBJ *destobj, int mode, int modemoveflags)
{
	return(moveobj(a, destobj, mode, 0, 0, modemoveflags | XYNOTCOORDS));
}
//==================================
int moveobj(struct OBJ *a, struct OBJ *destobj, int mode, int x, int y, int modemoveflags)
{
	OBJ *newobj;
	unsigned char flingy_id, SC_res_type;
	int needmana, type_id, obj_id, ret, slotnr, deltaz, constrerror, resval, newx, newy;
	int i, j, tempvar, state, openstate;
	int(*comparefunc)(int *, int);										//for doodad
//	  if (a == destobj)
//		return(MOVEOBJ_NOACT);
	//prevent to move if is uninterrupted mode

	if (!(modemoveflags & XYNOTCOORDS))
	{
		FixMapCoords(x, y);
	}
	if (a->modemove == MODEDIE)
	{
		DEBUGMESSCR(" [%s][%d] set <mode=[%s][%d]> then died\n", getOBJname(a->SC_Unit), a->SC_Unit, mageprop[mode].namemage, mode);
		return(MOVEOBJ_NOACT);
	}
	if (mageprop[a->modemove].atr & ORDER_CANNOTBEINTERRUPTED)
		return(MOVEOBJ_NOACT);
	//add to list of moves if have flag and have some in listmove
	if ((modemoveflags & SHIFTBIT) && (mageprop[mode].atr & ORDER_CANBEQUEUED))
	{
		if (a->modemove != MODESTOP)
		{
			AddModeMove(a, destobj, mode, x, y, modemoveflags);
			return(MOVEOBJ_DONE);
		}
	}
	if ((a->prop & VARPOWEROFF) && mode == MODEPOWERON)
	{
		;
	}
	else
	{
		if (mode != MODEDIE)
		{
			if (IsSubUnit(a->SC_Unit))
			{
				if (a->subunit->prop & VARNOTWORK)
					return(MOVEOBJ_NOACT);
			}
			else
			{
				if (a->prop & VARNOTWORK)
					return(MOVEOBJ_NOACT);
			}
		}
	}
	//release any resource field
	if (mode != MODEDIE)
		ReleaseResource(a);
	OBJstruct *b = loadobj(a->SC_Unit);
	a->prop &= ~(VARPATROLFLAG | VARNEXTMODEMOVEIGNOREINERTION);
	if (a->mainimage->flags & SC_IMAGE_FLAG_CANTBREAKCODE)
		if (mode != MODEDIE)
		{
			//memorize next move in time of casting spell
			AddModeMove(a, destobj, mode, x, y, NOSHOWERROR);
			return(MOVEOBJ_DONE);
		}
	if (a->SC_Unit == SC_BUGGUYOBJ && mode == MODEATACK && !destobj)
	{
		//infested terrain atack to ground
		mode = MODESUICIDEATACK;
	}
	if (mode != MODECASTSPELL && a->castmagenr == MODEYAMATOGUN)
	{
		//stop cast spell
		//remove all over images
		a->mainimage->DeleteChilds(SC_IMAGE_FLAG_IMGOVER);
		a->castmagenr = 0;
	}
	if (!(modemoveflags & ATACKMOVEBIT))
		a->prop &= ~VARMOVEINATACKMODE;
	if (mageprop[mode].type_id == ORDERS_UNIT)
	{
		if (IsAddon(mageprop[mode].obj_id))
		{
			return(moveobj_addonmode(a, destobj, mode, x, y, modemoveflags));
		}
		else if (IsBuild(mageprop[mode].obj_id))
		{
			return(moveobj_buildmode(a, destobj, mode, x, y, modemoveflags));
		}
		else
		{
			return(moveobj_unitmode(a, destobj, mode, x, y, modemoveflags));
		}
	}
	else
		switch (mode)
		{
		case MODENUKESPECIALSTATE1:
			a->castmagenr = mode;
			a->modemove = mode;
			SetOrder(a, 1, NULL);
			SetOBJIScriptNr(a, ISCRIPTNR_SPECIALSTATE1, ISCRIPTNR_EXECUTE);
			break;
		case MODENUKEWAITFORLANDING:
			a->modemove = mode;
			SetOBJxy256(a, x << 8, y << 8);
			SetOrder(a, 2, &SIGOrder_NukeLanding);
			//hide nuke from players
			a->mainimage->HideChildsImgFlag();
			a->mainimage->HideImgFlag();
			//play "nuclear launch detected"
			playinfoadvisorsound(a->playernr, gameconf.pl_race[NUMBGAMER], ADVNUCLEARDETECT, PLAYADVISOR_TEXTANDSOUND);

			SetOBJIScriptNr(a, ISCRIPTNR_WARPIN, ISCRIPTNR_EXECUTE);
			break;
		case MODECANCELNUKE:
			GhostNUKECancel(a, GHOSTCANCELNUKE);
			break;
		case MODEGHOSTSEATNUKEPOS:
			if (GhostNUKEAction(a) == 0)
			{
				ChangeTypeOfProp(a, PROPNUKECANCEL);
				SetOBJIScriptNr(a, ISCRIPTNR_SPECIALSTATE1, ISCRIPTNR_EXECUTE);
			}
			else
			{
				//error to launch nuke
			}
			break;
		case MODENUKELAUNCH:
			a->castmagenr = mode;
			a->modemove = mode;
			//move to destination
			initmoveaction(a, destobj, mode, 0, 0, x, y);
			AddModeMove(a, destobj, MODEGHOSTSEATNUKEPOS, x, y, modemoveflags);
			break;
		case MODEINFEST:
			if (destobj)
			{
				//queen search command center by self
				a->castmagenr = mode;
				a->modemove = mode;
				if (GetDistanceTo(destobj, GetOBJx(a), GetOBJy(a)) <= 0)
				{
					a->finalOBJ = destobj;
					CastSpellWithOutWeaponnr(a, mode);
				}
				else
				{
					//move to destination
					initmoveaction(a, destobj, mode, 0, 0, x, y);
					AddModeMove(a, destobj, MODEINFEST, x, y, modemoveflags);//cast mage after
				}
			}
			break;
		case MODECASTSPELL:
			needmana = getusemanaformodemove(a->castmagenr);
			if (CheckForMana(a, needmana) != CHECKRES_OK)
			{
				playinfoadvisorsound(a->playernr, GetUnitRace(a->SC_Unit), ADVENERGY, PLAYADVISOR_TEXTANDSOUND);
				return(MOVEOBJ_NOACT);
			}
			DecrMana(a, needmana);
			SetOrder(a, 1, &SIGOrder_AfterCastMage);
			SetOBJIScriptNr(a, ISCRIPTNR_CASTSPELL, ISCRIPTNR_EXECUTE);
			break;
		case MODEPSIONICSTORM:
		case MODEHALLUCINATION:
		case MODEMAELSTROM:
		case MODEMINDCONTROL:
		case MODEFEEDBACK:
		case MODERECALL:
		case MODEDISRUPTION:
		case MODESTASISFIELD:
		case MODEPLAGUE:
		case MODECONSUME:
		case MODECONSUME_HERO:
		case MODEDARKSWARM:
		case MODEPARASITE:
		case MODEENSNARE:
		case MODEBROODLING:
			//		case MODEYAMATOGUN:
		case MODELOCKDOWN:
		case MODEOPTICALFLARE:
		case MODERESTORATION:
		case MODEDEFENSIVEMATRIX:
		case MODEIRRADIATE:
		case MODEEMPSHOCKWAVE:
			a->castmagenr = mode;
			a->modemove = mode;
			//move to destination
			initmoveaction(a, destobj, mode, 0, 0, x, y);
			AddModeMove(a, destobj, MODECASTSPELL, x, y, modemoveflags);//cast mage after
			break;
		case MODEYAMATOGUN:
			a->castmagenr = mode;
			a->modemove = mode;
			//move to destination
			initmoveaction(a, destobj, mode, 0, 0, x, y);
			AddModeMove(a, destobj, MODECASTSPELL, x, y, modemoveflags);//cast mage after
//			a->finalOBJ = NULL;
			a->prop &= ~VARMOVEOBJACT;	//this is prevent move to destobj at next tick, that create engine glowing then do yamatogun
			break;
		case MODEWAITHARVESTMINERAL:
			a->modemove = mode;
			a->data.gather.waittoharvest = WAITTOHARVEST;
			break;
		case MODEWAITHARVESTGAS:
			a->modemove = mode;
			a->data.gather.waittoharvest = WAITTOHARVEST;
			break;
		case MODEWAITRETURNCARGO:
			a->modemove = mode;
			a->data.gather.waittoharvest = WAITTOHARVEST;
			break;
		case MODEGIVERESOURCETOBASE:
			newobj = a->carryobj;
			if (newobj->data.resourcechunk.restype == SC_CARRY_MINERAL1)
			{
				SC_res_type = SC_MINERAL1OBJ;
				ChangeResourcePlayer(a->playernr, PLUSFACTOR, newobj->data.resourcechunk.rescnt, 0);
				if (TRIG_ChangeStat)
					PLAYER[a->playernr].statplayer.stat[STATPLAYER_MINERALSMINED] += newobj->data.resourcechunk.rescnt;
			}
			else
			{
				if (newobj->data.resourcechunk.restype == SC_CARRY_GAS[0])
				{
					SC_res_type = SC_ASSIMILATOROBJ;
					ChangeResourcePlayer(a->playernr, PLUSFACTOR, 0, newobj->data.resourcechunk.rescnt);
					if (TRIG_ChangeStat)
						PLAYER[a->playernr].statplayer.stat[STATPLAYER_GASMINED] += newobj->data.resourcechunk.rescnt;
				}
				else
					break;
			}
			changegoods = 1;

			dieobj_silently(newobj);
			//			ChangeTypeOfProp(a,b,PROPNORMAL1);
			if (!a->prevresourceobj)
			{
				newobj = GetNearResource(a, SC_res_type, &resval);
				if (newobj)
					moveobj(a, newobj, MODEGATHER, NOSHOWERROR);
				else
					if (resval)
					{
						if (SC_res_type == SC_MINERAL1OBJ)
							moveobj(a, NULL, MODEWAITHARVESTMINERAL, NOSHOWERROR);
						else
							moveobj(a, NULL, MODEWAITHARVESTGAS, NOSHOWERROR);
					}
					else
						moveobj(a, NULL, MODESTOP, NOSHOWERROR);
			}
			else
				moveobj(a, a->prevresourceobj, MODEGATHER, NOSHOWERROR);
			break;
		case MODERETURNCARGO:
			a->modemove = mode;
			SetOBJIScriptNr(a, ISCRIPTNR_WALKINGTOIDLE, ISCRIPTNR_EXECUTE);
			newobj = GetNearCenter(a);
			if (newobj)
			{
				a->modemove = mode;
				initmoveaction(a, newobj, mode, 0, 0, x, y);
				AddModeMove(a, newobj, MODEGIVERESOURCETOBASE, x, y, modemoveflags);
			}
			else
			{
				//set to wait to appear base build
				moveobj(a, NULL, MODEWAITRETURNCARGO, 0, 0, modemoveflags);
			}
			return(MOVEOBJ_DONE);
			break;
		case MODEGATHERMINERAL:
			//check if mineral is non-occuied
			if (destobj)
			{
				if (!destobj->resourceobj)				//somebody harvest from this resource?
				{
					a->modemove = mode;
					destobj->resourceobj = a;			//resources now is busy by unit
					a->resourceobj = destobj;			//unit harvest from this resource
					a->prevresourceobj = destobj;		//doublicate unit harvest from this resource

					a->data.gather.gathertime = TIMETOGETMINERALS;
					SetOBJIScriptNr(a, ISCRIPTNR_ALMOSTBUILT, ISCRIPTNR_EXECUTE);
					return(MOVEOBJ_DONE);
				}
			}
			//search for other mineral field
			a->resourceobj = NULL;
			a->prevresourceobj = NULL;
			newobj = GetNearResource(a, SC_MINERAL1OBJ, &resval);
			if (newobj)
				moveobj(a, newobj, MODEGATHER, 0, 0, modemoveflags);
			else
				if (resval)
					moveobj(a, NULL, MODEWAITHARVESTMINERAL, 0, 0, modemoveflags);
				else
					moveobj(a, NULL, MODESTOP, modemoveflags);
			break;
		case MODEGATHERGAS:
			if (destobj)
			{
				if (IsOBJUnderConstruct(destobj))
				{
					moveobj(a, NULL, MODESTOP, modemoveflags);
					return(MOVEOBJ_NOACT);
				}
				if (!destobj->resourceobj)				//somebody harvest from this resource?
				{
					a->modemove = mode;
					destobj->resourceobj = a;			//resources now is busy by unit
					a->resourceobj = destobj;			//unit harvest from this resource
					a->prevresourceobj = destobj;		//doublicate unit harvest from this resource
					a->data.gather.gathertime = TIMETOGETGAS;
					//need to disapear unit
					a->mainimage->HideChildsImgFlag();
					a->mainimage->HideImgFlag();
					a->mainimage->side += 128;			//razvorot na 180 grad
					a->prop |= VARNOTHERE;
					deselectobj(a);
					return(MOVEOBJ_DONE);
				}
				else
					moveobj(a, NULL, MODEWAITHARVESTGAS, 0, 0, modemoveflags);
			}
			else
				moveobj(a, NULL, MODESTOP, modemoveflags);
			break;
		case MODEGATHER:
			if (destobj)
			{
				if (a->carryobj)
				{
					moveobj(a, destobj, MODEMOVE, NOSHOWERROR);
				}
				else
				{
					a->modemove = mode;
					a->prop |= VARAUTOMATRESOURCERET;
					initmoveaction(a, destobj, mode, 0, 0, x, y);
					if (IsNonNeutralFlag(destobj->SC_Unit))		//dest unit is extractor or refinery or assimilator
						AddModeMove(a, destobj, MODEGATHERGAS, x, y, modemoveflags);
					else
						AddModeMove(a, destobj, MODEGATHERMINERAL, x, y, modemoveflags);
				}
			}
			break;
		case MODESETRALLYPOINT:
			a->finalx = x;
			a->finaly = y;
			break;
		case MODEREPAIR:
			if (a->carryobj && IsNonNeutralFlag(a->carryobj->SC_Unit))
				return(MOVEOBJ_NOACT);
			if (destobj)
			{
				//continue construction
				if (IsOBJUnderConstruct(destobj))
				{
					moveobj(a, NULL, MODEMOVE, GetOBJx(destobj), GetOBJy(destobj), NOSHOWERROR);
					if (!destobj->constrobj)
					{
						AddModeMove(a, destobj, MODECONSTRUCT, 0, 0, NOSHOWERROR);
					}
				}
				else
				{
					if (GetUnitMaxHealth(destobj->SC_Unit) == destobj->health)
					{
						showadvisortext(STATTXT_UNABLEREPAIRTARGET);
						return(MOVEOBJ_NOACT);
					}
					a->modemove = mode;
					initmoveaction(a, destobj, MODEREPAIR, 0, 0, x, y);
					AddModeMove(a, destobj, MODESTAYANDREPAIR, 0, 0, NOSHOWERROR);
				}
			}
			break;
		case MODESTAYANDREPAIR:
			if (GetDistanceTo(destobj, GetOBJx(a), GetOBJy(a)) <= 16)
			{
				a->modemove = mode;
				//				SetOBJIScriptNr(a,ISCRIPTNR_GNDATTKINIT,ISCRIPTNR_EXECUTE);
				SetOBJIScriptNr(a, ISCRIPTNR_ALMOSTBUILT, ISCRIPTNR_EXECUTE);
			}
			else
			{
				initmoveaction(a, destobj, MODEREPAIR, 0, 0, x, y);
				AddModeMove(a, destobj, MODESTAYANDREPAIR, 0, 0, NOSHOWERROR);
			}
			break;
		case MODETURN180:
			a->modemove = mode;
			a->mainimage->UnitNeededDeltaDirection256(+128);
			break;
		case MODEHALTCONSTRUCT:
			if (a->constrobj)
			{
				a->constrobj->constrobj = NULL;
				a->constrobj = NULL;
				moveobj(a, NULL, MODESTOP, NOSHOWERROR);
				ChangeTypeOfProp(a, b, PROPNORMAL1);
			}
			break;
		case MODECONSTRUCT:
			a->modemove = mode;
			if (!a->constrobj)
			{
				ContinueSCVConstruct(a, destobj);
				break;
			}
			SetOBJIScriptNr(a, ISCRIPTNR_ALMOSTBUILT, ISCRIPTNR_EXECUTE);
			break;
		case MODEHOLDPOS:
			a->finalOBJ = NULL;
			DelAllModeMoves(a, 0);
			if (!(a->prop & VARHOLDPOSBIT))
			{
				a->prop |= VARHOLDPOSBIT;
				//engines off
				a->prop &= ~(VARMOVEOBJACT | VARACCELERATIONBIT | VARPATROLFLAG);
				if (a->modemove == MODEATACK)
				{
					EndAtackAction(a);
				}
				else
				{
					SetOBJIScriptNr(a, ISCRIPTNR_WALKINGTOIDLE, ISCRIPTNR_EXECUTE);
				}
				//				SetModeMove(a,mode);
			}
			break;
		case MODESTOP:
			a->finalOBJ = NULL;
			if (alldattbl.units_dat->Subunit1[a->SC_Unit] < MAX_UNITS_ELEM)
			{
				moveobj(a->subunit, destobj, mode, x, y, NOSHOWERROR);
			}
			DelAllModeMoves(a, 0);
			if (a->modemove != mode)
			{
				a->prop &= ~(VARMOVEOBJACT | VARACCELERATIONBIT | VARPATROLFLAG);
				//engines off
				if (a->modemove == MODEATACK)
				{
					CALLBACK_OBJ_AtackedOBJISNULL(a);
				}
				else
				{
					SetOBJIScriptNr(a, ISCRIPTNR_WALKINGTOIDLE, ISCRIPTNR_EXECUTE);
					SetModeMove(a, mode);
				}
			}
			break;
		case MODEPATROL:
			SetModeMove(a, mode);
			if (alldattbl.units_dat->Subunit1[a->SC_Unit] < MAX_UNITS_ELEM)
			{
				moveobj(a->subunit, NULL, MODESTOP, x, y, NOSHOWERROR);
			}
			initmoveaction(a, destobj, mode, 0, 0, x, y);
			break;
		case MODEACTIVATE:
			if (destobj)
			{
				a->modemove = mode;
				initmoveaction(a, NULL, mode, 0, 0, GetOBJx(destobj), GetOBJy(destobj));
			}
			break;
		case MODEMOVE:
			a->modemove = mode;
			if (alldattbl.units_dat->Subunit1[a->SC_Unit] < MAX_UNITS_ELEM)
			{
				moveobj(a->subunit, NULL, MODESTOP, x, y, NOSHOWERROR);
			}
			if (destobj)
			{
				CheckIfGotoTransport(a, destobj);
				CheckIfGotoBattery(a, destobj);
				if (!a->currentspeed)
				{
					flingy_id = alldattbl.units_dat->flingy_id[a->SC_Unit];
					if (alldattbl.flingy_dat->MoveControl[flingy_id] == FLINGYMOVECONTROL_FLINGY ||
						alldattbl.flingy_dat->MoveControl[flingy_id] == FLINGYMOVECONTROL_WEAPON)
					{
						a->prop |= VARMOVEOBJACT;
						a->finalOBJ = destobj;
					}
				}
			}
			initmoveaction(a, destobj, mode, 0, 0, x, y);
			break;
		case MODETANKSIEGE:
			if (!MageDepend(a, a->playernr, mode))
			{
				return(MOVEOBJ_NOACT);
			}
			if (a->modemove == MODETANKSIEGE || a->modemove == MODETANKNORMAL)
				return(MOVEOBJ_NOACT);
			a->prop |= VARSPECIALWAIT;
			SetModeMove(a, mode);
			SetModeMove(a->subunit, mode);
			SetOrder(a, 1, &SIGOrder_Tank_EndRotationBeforeSiegeMode);
			SetOrder(a->subunit, 1, &SIGOrder_Tank_EndRotationBeforeSiegeMode);
			a->mainimage->UnitNeededDirection256(TANKSIEGESIDE);
			a->subunit->mainimage->UnitNeededDirection256(TANKSIEGESIDE);
			SetOBJIScriptNr(a, ISCRIPTNR_SPECIALSTATE1, ISCRIPTNR_SETONLY);
			SetOBJIScriptNr(a->subunit, ISCRIPTNR_SPECIALSTATE1, ISCRIPTNR_SETONLY);
			break;
		case MODETANKNORMAL:
			if (!MageDepend(a, a->playernr, mode))
			{
				return(MOVEOBJ_NOACT);
			}
			if (a->modemove == MODETANKSIEGE || a->modemove == MODETANKNORMAL)
				return(MOVEOBJ_NOACT);
			a->prop |= VARSPECIALWAIT;
			SetModeMove(a, mode);
			SetModeMove(a->subunit, mode);
			SetOrder(a->subunit, 1, &SIGOrder_Tank_EndRotationBeforeTankMode);
			a->subunit->mainimage->UnitNeededDirection256(TANKNORMALSIDE);
			SetOBJIScriptNr(a->subunit, ISCRIPTNR_SPECIALSTATE1, ISCRIPTNR_SETONLY);
			break;
		case MODEBURROW:
		case MODEBURROW2:
			if (!MageDepend(a, a->playernr, mode))
			{
				return(MOVEOBJ_NOACT);
			}
			if (a->modemove == mode)
				return(MOVEOBJ_NOACT);
			if (!SetOBJIScriptNr(a, ISCRIPTNR_BURROW, ISCRIPTNR_SETONLY))
			{
				if (a->SC_Unit == SC_LURKEROBJ)
					a->mainimage->UnitNeededDirection256(9 * 8);
				else
					a->mainimage->UnitNeededDirection256(12 * 8);
				//special action for drone
				if (a->SC_Unit == SC_DRONEOBJ)
				{
					a->mainimage->flags |= SC_IMAGE_FLAG_ISCRIPTROTATION;
					a->modemove = MODESTOP;
					a->prop &= ~VARACCELERATIONBIT;
					a->currentspeed = 0;
				}
				SetOrder(a, 4, &SIGOrder_AfterBurrow);
				SetModeMove(a, mode);
				if (mode == MODEBURROW || (mode == MODEBURROW2 && a->SC_Unit != SC_LURKEROBJ))
					Play_sfxdata(a->mainimage->xpos >> 8, a->mainimage->ypos >> 8, SFXDATA_BURROWDOWN, 2);
			}
			break;
		case MODEUNBURROW:
			if (!MageDepend(a, a->playernr, mode))
			{
				return(MOVEOBJ_NOACT);
			}
			if (a->modemove == mode)
				return(MOVEOBJ_NOACT);
			if (!SetOBJIScriptNr(a, ISCRIPTNR_UNBURROW, ISCRIPTNR_SETONLY))
			{
				SetBurrowFlag(a, 0);
				SetMageAtr(&a->atrobj, ATRINVISIBLE, 0);
				a->mainimage->invisiblecolors = MININVISIBLECOLOR;
				SetOrder(a, 4, &SIGOrder_AfterUnburrow);
				SetModeMove(a, mode);
			}
			break;
		case MODEDIE:
			doselectedOBJbit(a, NUMBGAMER, 0);
			deselectobj(a);
			a->blinkvalue = 0;
			DelAllModeMoves(a, 0);
			//			if (a->movelist)
			//				a->movelist->EmptyElemFifo();
			a->modemove = mode;
			if (IsHallucination(a))
			{
				a->mainimage->SetGrpFunc(IMAGEID_HALLUCINATIONDIE1);
				Play_sfxdata(a->mainimage->xpos >> 8, a->mainimage->ypos >> 8, SFXDATA_HALLUCINATIONDIE, 2);
				a->mainimage->offsetcmdinbuf = 0;
				a->mainimage->SetIScriptNr(ISCRIPTNR_INIT);
			}
			 a->mainimage->newgrpmethod = NORMAL;				//need to see how unit dies
			//if (a->mainimage->newgrpmethod == DISTORTION)
			//	 a->mainimage->newgrpmethod = TRANSPARENT;				//need to see how unit dies
			SetOBJIScriptNr(a, ISCRIPTNR_DEATH, ISCRIPTNR_SETONLY);
			break;
		case MODELANDING:
			deltaz = hypot(GetOBJx(a) - x, GetOBJy(a) - y + AIRUNITS_START_YPOS);
			if (deltaz <= 1)
			{
				if (!CheckIfCanLand(a, x, y, &constrerror))
				{
					moveobj(a, NULL, MODESTOP, NOSHOWERROR);
					if (a->playernr == NUMBGAMER && (modemoveflags & SHOWERRORTEXT))
					{
						Play_sfxdata_id((OBJ *)SelectedUnits.GetElem(0, NULL), PUTLIFTDOWNERROR, 2, 0);
						putbuildplacemessage(-constrerror);
					}
					return(MOVEOBJ_NOACT);
				}
				a->modemove = mode;
				a->deltavertpos = y - GetOBJy(a);

				a->mainimage->xpos = x << 8;

				SetOrder(a, 16, &SIGOrder_Landing);
				SetOBJIScriptNr(a, ISCRIPTNR_LANDING, ISCRIPTNR_EXECUTE);
				a->finalx = 0;//erase rallypoint
				a->finaly = 0;
				a->prop |= VARDONOTAPPLYNEXTMOVE;
			}
			else
			{
				moveobj(a, NULL, MODEMOVE, x, y - AIRUNITS_START_YPOS, NOSHOWERROR);
				InsertModeMove(a, NULL, mode, x, y, modemoveflags);
			}
			break;
		case MODELIFTOFF:
			a->modemove = mode;
			AddDustImages(a, IMAGE_OVERLAY_LIFTOFF);
			SetOrder(a, 16, &SIGOrder_LiftOff);
			SetOBJIScriptNr(a, ISCRIPTNR_LIFTOFF, ISCRIPTNR_SETONLY);
			DettachAddon(a);
			a->deltavertpos = AIRUNITS_START_YPOS;
			a->prop |= VARDONOTAPPLYNEXTMOVE;
			break;
		case MODECLOAKFIELD:
		case MODEPERSONNELCLOAK:
			if (accesstomove(a, b, mode, a->playernr))
			{
				if (!MageDepend(a, a->playernr, mode))
				{
					return(MOVEOBJ_NOACT);
				}
				needmana = getusemanaformodemove(mode);
				if (CheckForMana(a, needmana) != CHECKRES_OK)
				{
					playinfoadvisorsound(a->playernr, GetUnitRace(a->SC_Unit), ADVENERGY, PLAYADVISOR_TEXTANDSOUND);
					return(MOVEOBJ_NOACT);
				}
				DecrMana(a, needmana);
				addmage(a, ATRINVISIBLE, a->mana);
				a->prop |= VARDECRMAGE;
				ChangeTypeOfProp(a, b, PROPNORMAL2);
			}
			return(MOVEOBJ_DONE);
		case MODEDECLOAK:
			addmage(a, ATRINVISIBLE, 0);
			a->prop &= ~VARDECRMAGE;
			ChangeTypeOfProp(a, b, PROPNORMAL1);
			return(MOVEOBJ_DONE);
		case MODEESCAPE1:		//escape slots in construct mode
		case MODEESCAPE2:
		case MODEESCAPE3:
		case MODEESCAPE4:
		case MODEESCAPE5:
		escapeconstrslots:
			slotnr = mode - MODEESCAPE1;
			//			  if (!(a->prop & VARNOTWORK))
			{
				SlotReturnResources(a, a->playernr, slotnr);
				ret = DelConstruct(a, slotnr, &type_id, &obj_id);
				if (ret)
				{
					switch (type_id)
					{
					case ORDERS_UNIT:
						ChangeSupply(a->playernr, obj_id, MINUSFACTOR);
						if (IsAddon(obj_id))
						{
							a->addonobj->addonobj = NULL;
							dieobj(a->addonobj);
							a->addonobj = NULL;
						}
						break;
					case ORDERS_UPGRADE:
						break;
					case ORDERS_TECHNOLOGY:
						break;
					default:
						break;

					}
				}
			}
			break;
		case MODEESCAPE:
			if (IsOBJUnderConstruct(a))
			{
				SetObjWalk8(&map, a, CLEARWALK);
				switch (GetUnitRace(a->SC_Unit))
				{
				case ZERGRACE:
					switch (a->SC_FromUnit)
					{
					case SC_DRONEOBJ:
						SIGOrder_CancelMorthFromDrone(a);
						break;
					case SC_LARVAEOBJ:
						dieobj(a);
						break;
					case SC_HYDRALISKOBJ:
						ChangeSupply(a->playernr, a->SC_Unit, MINUSFACTOR);
						ChangeSupply(a->playernr, a->SC_FromUnit, MINUSFACTOR);
						SetOrder(a, 4, SIGOrder_CancelMorthFromHydralisk);
						SetOBJIScriptNr(a, ISCRIPTNR_SPECIALSTATE2, ISCRIPTNR_EXECUTE);			//egg deconstruct script
						break;
					case SC_MUTALISKOBJ:
						SetOrder(a, 4, SIGOrder_CancelMorthFromMutalisk);
						SetOBJIScriptNr(a, ISCRIPTNR_SPECIALSTATE2, ISCRIPTNR_EXECUTE);			//cocoon deconstruct script
						break;
					case SC_HATCHERYOBJ:
					case SC_LAIROBJ:
					case SC_SPIREOBJ:
					case SC_CREEPCOLONYOBJ:
						SIGOrder_CancelMorthFromBuild(a);
						break;
					case SC_NYDUSCANALOBJ:
						if (a->doubleunit)
							return(MOVEOBJ_NOACT);
						break;
					}
					ReturnResources(a->SC_Unit, a->playernr, 100);
					break;
				case TERRANRACE:
					ReturnResources(a->SC_Unit, a->playernr, a->selfconstruct.timeremained * 100 / a->selfconstruct.timemax);
					if (IsAddon(a->SC_Unit))
						delADVobj(a);
					else
						dieobj(a);
					break;
				case PROTOSSRACE:
					if (a->SC_Unit == SC_ARCHONOBJ || a->SC_Unit == SC_DARKARCHONOBJ)
						return(MOVEOBJ_NOACT);
					ReturnResources(a->SC_Unit, a->playernr, 100);
					dieobj(a);
					break;
				}
			}
			else
			{
				if (a->construct)
				{
					mode = a->construct->nrofelem + MODEESCAPE1 - 1;
					goto escapeconstrslots;
				}
			}
			break;
		case MODEPOWERON:
			a->prop &= ~(VARNOTWORK | VARPOWEROFF);
			if (GetConstructNR(a))
				SetOBJIScriptNr(a, ISCRIPTNR_ISWORKING, ISCRIPTNR_EXECUTE);
			break;
		case MODEPOWEROFF:
			a->prop |= (VARNOTWORK | VARPOWEROFF);
			if (a->SC_Unit == SC_PHOTONCANONOBJ && a->modemove == MODEATACK)
			{
				EndAtackAction(a);
				SetModeMove(a, MODESTOP);
			}
			else
				if (a->SC_Unit == SC_BATTERYOBJ || GetConstructNR(a))
					SetOBJIScriptNr(a, ISCRIPTNR_WORKINGTOIDLE, ISCRIPTNR_EXECUTE);
			break;
		case MODESTIMPACK1:
		case MODESTIMPACK2:
			if (a->health <= STIMPACKMINLIFE)
				return(MOVEOBJ_NOACT);
			addmage(a, ATRSTIMPACK, STIMPACKTIME);
			a->health -= STIMPACKMINLIFE;
			if (mageprop[mode].sound_id[SOUNDONCAST])
				Play_sfxdata(GetOBJx(a), GetOBJy(a), mageprop[mode].sound_id[SOUNDONCAST], 2);
			break;
		case MODEENTERTRANSPORT:
			if (destobj)
			{
				//check is too far to transport
				if (GetDistanceTo(destobj, GetOBJx(a), GetOBJy(a)) <= 48)
				{
					if (a->playernr == destobj->playernr)
					{
						if (destobj->SC_Unit == SC_BUNKEROBJ)
						{
							if (IsOrganic(a->SC_Unit) && GetUnitRace(a->SC_Unit) == TERRANRACE)
							{
								LoadObjInObj(a, destobj);
							}
						}
						else
						{
							LoadObjInObj(a, destobj);
						}
					}
				}
				else
				{
					moveobj(a, destobj, MODEMOVE, NOSHOWERROR);
				}
			}
			break;
		case MODEWARPINIT:
			CreateArchon(a, destobj);
			break;
		case MODEWARPARCHON:
		case MODEWARPDARCHON:
			if (destobj)
			{
				a->modemove = mode;
				//move to destination
				initmoveaction(a, destobj, mode, 0, 0, x, y);
				AddModeMove(a, destobj, MODEWARPINIT, x, y, modemoveflags);//cast mage after
			}
			return(MOVEOBJ_DONE);
		case MODEUNLOADALLUNITS:
			if (a->loaded)
			{
				tempvar = a->loaded->nrofloadedunits;
				for (i = 0;i < tempvar;i++)
					UnLoadObjInObj(a, 0, NEWXYCOORDS, 0, 0, 0);
				return(MOVEOBJ_DONE);
			}
			return(MOVEOBJ_NOACT);
			break;
		case MODEHEAL:
			if (destobj == NULL)
			{
				moveobj(a, NULL, MODESTOP, NOSHOWERROR);
				return(MOVEOBJ_NOACT);
			}
			if (ishealingobj(destobj))
			{
				//check is too far to transport
				a->modemove = mode;
				a->castmagenr = mode;
				a->finalOBJ = destobj;
				if (GetDistanceBetweenUnits256(a, destobj) <= mageprop[mode].mindistance + (10 << 8))
				{
					SetOrder(a, 1, &SIGOrder_AfterCastHeal);
					SetOBJIScriptNr(a, ISCRIPTNR_SPECIALSTATE1, ISCRIPTNR_EXECUTE);		//cast heal
				}
				else
				{
					initmoveaction(a, destobj, mode, 0, 0, GetOBJx(destobj), GetOBJy(destobj));
					AddModeMove(a, destobj, MODEHEAL, GetOBJx(destobj), GetOBJy(destobj), NOSHOWERROR);//cast mage after
				}
			}
			break;
		case MODERECHARGESHIELD:
			if (destobj)
			{
				if (!isrechargebleobj(destobj))
					return(MOVEOBJ_NOACT);
				a->finalOBJ = destobj;
				if (GetDistanceBetweenUnits256(a, destobj) <= mageprop[mode].mindistance)
				{
					a->data.battery.rechargetime = BATTERYRECHARGEWAITTIME;
					a->modemove = mode;
					a->castmagenr = mode;
					SetOBJIScriptNr(a, ISCRIPTNR_ISWORKING, ISCRIPTNR_EXECUTE);	//cast recharge shield
				}
				else
				{
					MakeQueueAction(WAITSOMETIMEACTION, a, destobj, 0, 0, 0, 30);
					AddModeMove(a, destobj, mode, x, y, NOSHOWERROR);
				}
			}
			break;
		case MODEUNLOADUNITNR:
			//			DEBUGMESSCR("unload %d\n",x);
			if (a->modemove != MODEUNLOADUNITS) //prevent unload unit nr then unload all units
			{
				if (a->loaded)
				{
					a->loaded->nrunittounload = x;
				}
			}
			break;
		case MODELOADUNITS:
			a->modemove = mode;
			initmoveaction(a, destobj, mode, 0, 0, x, y);
			moveobj(destobj, a, MODEENTERTRANSPORT, NOSHOWERROR);
			break;
		case MODEUNLOADUNITS:
			if (destobj)
			{
				x = GetOBJx(destobj);
				y = GetOBJy(destobj);
				destobj = NULL;
			}
			if (GetDistanceTo(a, x, y) <= 10)
			{
				a->modemove = mode;
			}
			else
			{
				a->modemove = MODEMOVE;
				initmoveaction(a, NULL, MODEMOVE, 0, 0, x, y);
				AddModeMove(a, NULL, MODEUNLOADUNITS, x, y, NOSHOWERROR);
			}
			break;
		case MODEATACKREADY:
			if (alldattbl.units_dat->Subunit1[a->SC_Unit] < MAX_UNITS_ELEM)
			{
				moveobj(a->subunit, destobj, mode, x, y, modemoveflags);
				return(MOVEOBJ_DONE);
			}
			if (destobj)
			{
				ret = AtackCoolDownEnds(a, destobj, 0, modemoveflags);
				switch (ret)
				{
				case MOVEOBJ_CONTINUEJOB:
					return(MOVEOBJ_WAITUNTIL);
				}
				a->finalOBJ = destobj;
				SetAtackType(a, destobj);
				switch (a->SC_Unit)
				{
				case SC_SCARABOBJ:
				case SC_VULTUREMINEOBJ:
				case SC_BUGGUYOBJ:
				bugguyexplode:
					SetOrder(a, 1, &SIGOrder_UnitDies);
					a->castmagenr = MODEMINEEXPLODE;
					SpecialAtackAction(a, ISCRIPTNR_SPECIALSTATE1);
					break;
				default:
					AtackAction(a, destobj, 0);
					break;
				}
			}
			else
			{
				if (a->SC_Unit == SC_BUGGUYOBJ)
					goto bugguyexplode;
				ApplyNextModeMove(a);
			}
			break;
		case MODEATACK:
			if (destobj)
			{
				if (alldattbl.units_dat->Subunit1[a->SC_Unit] < MAX_UNITS_ELEM)
				{
					moveobj(a->subunit, destobj, mode, x, y, modemoveflags);
					return(MOVEOBJ_DONE);
				}
				if (!(modemoveflags & ATACKMOVEBIT))
					a->prop &= ~VARMOVEINATACKMODE;
				ret = AtackCoolDownEnds(a, destobj, 0, modemoveflags);
				switch (ret)
				{
				case MOVEOBJ_DONE:
					a->modemove = MODEATACK;
					if (IsSubUnit(a->SC_Unit))
					{
						SetModeMove(a->subunit, MODESTOP);	//stop base of unit
						InitStopAfterMove(a->subunit);		//stop iscript for base
					}
					break;
				case MOVEOBJ_STOPANDCONTINUEJOB:
					a->modemove = MODEATACK;
					if (IsSubUnit(a->SC_Unit))
					{
						SetModeMove(a->subunit, MODESTOP);
						InitStopAfterMove(a->subunit);
					}
					break;
				case MOVEOBJ_CONTINUEJOB:
					a->modemove = MODEATACK;
					break;
				}
			}
			else
			{
				//atack to ground, check if object can move
				if (a->SC_Unit == SC_INTERCEPTOROBJ)
				{
					if (CALLBACK_OBJ_AtackedOBJISNULL(a))
					{
						return(MOVEOBJ_DONE);
					}
					else
					{
						DelAllModeMoves(a, 0);
						moveobj(a, a->myparent, MODEGOTORECHARGE, NOSHOWERROR);
					}
				}
				if (accesstomove(a, loadobj(a->SC_Unit), MODEMOVE, a->playernr))
				{
					a->prop |= VARMOVEINATACKMODE;
					a->searchforatack_tick = 0;//fast search
					a->modemove = MODEATACK;
					a->finalxatack = x;
					a->finalyatack = y;
					initmoveaction(a, NULL, MODEATACK, 0, 0, x, y);
					if (a->subunit)
						a->subunit->prop |= VARMOVEINATACKMODE;
				}
				else
					return(MOVEOBJ_NOACT);
			}
			break;
		case MODERELEASEMINENOW:
			if (a->ammo--)
				createobjfulllife(x, y, SC_VULTUREMINEOBJ, a->playernr);
			ApplyNextModeMove(a);
			break;
		case MODERELEASEMINE:
			initmoveaction(a, NULL, mode, 0, 0, x, y);
			AddModeMove(a, destobj, MODERELEASEMINENOW, x, y, NOSHOWERROR);
			break;
		case MODEDOODADCHANGESTATE:
			state = GetDoodadState(a);
			openstate = x;
			if (a->SC_Unit == SC_LEFTUPPERLEVELDOOROBJ ||
				a->SC_Unit == SC_RIGHTUPPERLEVELDOOROBJ ||
				a->SC_Unit == SC_LEFTPITDOOROBJ ||
				a->SC_Unit == SC_RIGHTPITDOOROBJ)
			{
				comparefunc = DoorConditionFunctions[openstate];
			}
			else
			{
				comparefunc = DoorConditionFunctions[openstate];
			}
			(*comparefunc)(&state, -1);
			DoodadChangeState(a, state);
			break;
		case MODESELECTLARVAE:
			j = getchilds(a);
			if (j)
			{
				deselectallobj(a->playernr);
				for (i = 0;i < MAXCHILDS;i++)
				{
					if (a->childs->parentof[i])
						selectobj(a->childs->parentof[i]);
				}
			}
			break;
		case MODEGOTORECHARGE:
			if (!a->myparent || !destobj)
			{
				dieobj(a);
				return(MOVEOBJ_NOACT);
			}
			initmoveaction(a, destobj, mode, 0, 0, x, y);
			AddModeMove(a, NULL, MODERECHARGE, 0, 0, 0);
			a->modemove = mode;
			break;
		case MODERECHARGE:				//recharge interceptor in carrier
			ReturnedToBase(a);
			break;
		case MODESUICIDEATACK:
			initmoveaction(a, NULL, mode, 0, 0, x, y);
			AddModeMove(a, NULL, MODEATACKREADY, x, y, NOSHOWERROR);
			break;
		case MODEMOVEFORWARD:
			a->prop |= VARNEXTMODEMOVEIGNOREINERTION;
			if (!a->currentspeed)
				a->currentspeed = 1;
			a->modemove = mode;
			if (!x)
				a->mainimage->UnitNeededDeltaDirection256(myrand(-64, 64));
			//				a->mainimage->neededside += myrand(-64,64);
						//move in new direction (front of new direction)
			newx = GetOBJx(a) + ((inertion256[a->mainimage->neededside][0] * INTERCEPTORDESTMOVEAFTERATACK) >> 16);
			newy = GetOBJy(a) + ((inertion256[a->mainimage->neededside][1] * INTERCEPTORDESTMOVEAFTERATACK) >> 16);
			FixMapCoords(newx, newy);
			//play launch interceptor song
			Play_sfxdata(a->mainimage->xpos >> 8, a->mainimage->ypos >> 8, SFXDATA_LAUNCHINTERCEPTOR, 4);
			initmoveaction(a, NULL, mode, 0, 0, newx, newy);
			if (destobj)
				AddModeMove(a, destobj, MODEATACK, 0, 0, NOSHOWERROR);
			else
				AddModeMove(a, NULL, MODESEARCHGOAL, 0, 0, NOSHOWERROR);
			break;
		case MODESEARCHGOAL:
			if (OneUnitSearchGoal(a, 1, 0))
			{
				a->searchforatack_tick = MAXWAIT_SEARCHATACK_TICK;
			}
			else
			{
				if (a->myparent && a->myparent->finalOBJ)
					moveobj(a, a->myparent->finalOBJ, MODEATACK, NOSHOWERROR);
			}
			break;
		default:
			DEBUGMESSCR("mode=%d not developed\n", mode);
			break;
		}
	return(MOVEOBJ_DONE);
}
//==========================================
void CheckIfGotoBattery(OBJ *a, OBJ *destobj)
{
	switch (destobj->SC_Unit)
	{
	case SC_BATTERYOBJ:
		moveobj(destobj, a, MODERECHARGESHIELD, NOSHOWERROR);
		break;
	}
}
//==========================================
void CheckIfGotoTransport(OBJ *a, OBJ *destobj)
{
	//check if can enter tarnsport
	if (GetSpaceRequired(a->SC_Unit) == 255)
		return;
	if (IsHallucination(destobj))
		return;
	switch (destobj->SC_Unit)
	{
	case SC_OVERLORDOBJ:
		//check if have transport ability
		if (!GetUpgradeTree(&map, destobj->playernr, UPGRADES_DAT_OVERLORDTRANSP))
			return;
	case SC_HERO_YGGDRASILLOBJ:
	case SC_DROPSHIPOBJ:
	case SC_SHUTTLEOBJ:
		AddModeMove(a, destobj, MODEENTERTRANSPORT, 0, 0, NOSHOWERROR);
	case SC_BUNKEROBJ:
		if (IsReadyOBJ(destobj))
			AddModeMove(a, destobj, MODEENTERTRANSPORT, 0, 0, NOSHOWERROR);
		break;
	}
}
//==========================================
void SlotReturnResources(OBJ *a, int playernr, int slotnr)
{
	int factorreturn, mcost, gcost, type_id, obj_id;
	if (!a->construct)
		return;
	if (!slotnr)
	{
		factorreturn = a->construct->timeremained * 100 / a->construct->timemax;
	}
	else
		factorreturn = 100;
	mcost = 0;
	gcost = 0;
	if (GetConstruct(a, slotnr, &type_id, &obj_id, NULL, NULL, NULL, NULL))
	{
		switch (type_id)
		{
		case ORDERS_UNIT:
			GetCostUnit(obj_id, &mcost, &gcost);
			break;
		case ORDERS_UPGRADE:
			GetCostUpgr(obj_id, a->playernr, &mcost, &gcost);
			break;
		case ORDERS_TECHNOLOGY:
			GetCostTech(obj_id, &mcost, &gcost);
			break;
		}
	}
	ChangeResourcePlayer(playernr, PLUSFACTOR, mcost*factorreturn / 100, gcost*factorreturn / 100);
}
//==========================================
void ReturnResources(SCUNIT SC_Unit, int playernr, int factorreturn)
{
	int mcost, gcost;
	GetCostUnit(SC_Unit, &mcost, &gcost);
	ChangeResourcePlayer(playernr, PLUSFACTOR, mcost*factorreturn / 100, gcost*factorreturn / 100);
}
//==========================================
void delADVobj(struct OBJ *a)
{
	if (a->addonobj)
	{
		ConstructUnitsReleaseSupply(a->addonobj);
		DestroyConstrStruct(a->addonobj);
	}
	dieobj(a);
}
//=================================
int accesstomove(struct OBJ *a, struct OBJstruct *b, int mode, int player)
{
	int j, prop;
	if (IsHallucination(a))
		if (!(mageprop[mode].atr & ORDER_SHOWHALLUCINATED))
			return(0);

	if (a->prop & (VARNOTWORK | VARPOWEROFF))
		return(0);
	if (IsSubUnit(a->SC_Unit))
	{
		a = a->subunit;
		b = loadobj(a->SC_Unit);
	}
	for (j = 0;j < MAXUNITPROPERTIES;j++)
	{
		prop = b->modemove[a->typeofprop][j];
		if (a->playernr == player)
		{
			if (prop == mode)
				return(1);
			switch (mode)
			{
			case MODEUNLOADUNITNR:
				if (prop == MODEUNLOADUNITS || prop == MODEUNLOADALLUNITS)
					return true;
				break;
			case MODEBURROW:
				if (prop == MODEBURROW2)
					return true;
				break;
			case MODEBURROW2:
				if (prop == MODEBURROW)
					return true;
				break;
			}
		}
	}
	switch (a->SC_Unit)
	{
	case SC_DRONEOBJ:
		if (IsOBJBurrowed(a))
			return(0);
		//proceed next like a other probe or scv
	case SC_PROBEOBJ:
	case SC_SCVOBJ:
		if (a->typeofprop != PROPNORMAL1)
			return 1;
		break;
	case SC_LARVAEOBJ:
		for (j = 0;j < MAXUNITPROPERTIES;j++)
			if (b->modemove[PROPNORMAL2][j] == mode && a->playernr == player)
				return(1);
		break;
	}
	return(0);
}
//==================================
//return 1 if i make move form right button of mouse
int movefrommouse(struct OBJ *a)
{
	switch (a->SC_Unit)
	{
	case SC_LARVAEOBJ:
		return 0;
		break;
	case SC_PROBEOBJ:
	case SC_DRONEOBJ:
	case SC_SCVOBJ:
		if ((a->typeofprop == PROPBUILDSIMPLE || a->typeofprop == PROPBUILDADVANCED) &&
			!highMouse->Construct.SC_BuildUnit)
		{
			ChangeTypeOfProp(a, PROPNORMAL1);
			return 0;
		}
		break;
	}
	return 1;
}
//==================================
int makemove(struct OBJ *a, struct OBJ *destobj, int locx, int locy, int mode, int player, int modemoveflags)
{
	COMMANDQUEUEELEMENT *cqe;
	struct OBJstruct *b;
	b = loadobj(a->SC_Unit);
	if (mode < MODEACTIVATE || mode > MODEESCAPE5)
	{
		if (!accesstomove(a, b, mode, player))
			return(0);
		if (!accesstomage(a, destobj, mode))
		{
			if (player == NUMBGAMER)
			{
				showadvisortext(875);//invalid target
				Play_sfxdata_id(a, PUTMAGEERROR, 1, 0);
			}
			return(0);
		}
	}

	if (mode != MODEHOLDPOS)
		a->prop &= ~VARHOLDPOSBIT;
	cqe = (COMMANDQUEUEELEMENT *) malloc(sizeof(COMMANDQUEUEELEMENT));
	cqe->queueaction.obj = a;
	cqe->queueaction.destobj = destobj;
	cqe->queueaction.actiontype = mode;
	cqe->queueaction.param0 = locx;
	cqe->queueaction.param1 = locy;
	cqe->queueaction.param2 = modemoveflags | PLAYERDOMOVE;
	if (!NETWORKGAME)
		cqe->executeTick = commandqueuetick + 1;
	else
		cqe->executeTick = commandqueuetick + 1;//TODO need to change if network latency selected
	return(UnitsCommandQueue.AppendToQueue(cqe) == QUEUEOK);
}
//==================================
int selectedobjmove(struct OBJ *destobj, int locx, int locy, int mode, int player, int modemoveflags, int rightclick)
{
	int minx, miny, maxx, maxy, midx = 0, midy = 0, deltax = 0, deltay = 0, flags = SHOWERRORTEXT;
	int k = 0;
	struct OBJ *speakOBJ, *todestobj;
	SelectionObjs *moveobjects = new SelectionObjs(MAXSELECTMAN);
	OBJ *o;
	SelectedUnits.EnumListInit();
	while ((o = (OBJ *)SelectedUnits.GetNextListElem(NULL)))
	{
		if ((!(o->prop & VARNOTSEEPROP))
			&&
			!(o->prop & VARNOTWORK))
		{
			if (movefrommouse(o))
			{
				moveobjects->Add(o);
			}
		}
	}
	minx = 256 * 32;
	miny = 256 * 32;
	maxx = 0;
	maxy = 0;
	moveobjects->EnumListInit();
	while ((o = (OBJ *)moveobjects->GetNextListElem(NULL)))
	{
		if (GetOBJx(o) < minx)
			minx = GetOBJx(o);
		if (GetOBJy(o) < miny)
			miny = GetOBJy(o);
		if (GetOBJx(o) > maxx)
			maxx = GetOBJx(o);
		if (GetOBJy(o) > maxy)
			maxy = GetOBJy(o);
	}
	if (maxx - minx <= 128 && maxy - miny <= 128)
	{
		//move in group
		if (locx < minx || locy < miny || locx > maxx || locy > maxy)
		{
			midx = (maxx - minx) / 2 + minx;
			midy = (maxy - miny) / 2 + miny;
		}
	}
	if (mode == MODEATACK)
		flags |= ATACKMOVEBIT;
	moveobjects->EnumListInit();
	while ((o = (OBJ *)moveobjects->GetNextListElem(NULL)))
	{
		//elsi move na sebea
		todestobj = destobj;
		if (o == destobj)
		{
			if (mode != MODELANDING)
			{
				locx = GetOBJx(destobj);
				locy = GetOBJy(destobj);
				todestobj = NULL;
			}
		}
		if (midx)
		{
			deltax = GetOBJx(o) - midx;
			deltay = GetOBJy(o) - midy;
		}
		o->prop &= ~VARAUTOMATRESOURCERET;
		if (rightclick)
		{
			if (o->SC_Unit == SC_SCVOBJ && o->constrobj)
			{
				continue;
			}
			if (todestobj)
				mode = GetDefaultModeForRightClick(o, todestobj, player);
		}
		if (mode != MODENON)
			k |= makemove(o, todestobj, locx + deltax, locy + deltay, mode, player, flags);
	}
	speakOBJ = GetMaxRankOBJ(moveobjects);
	if (speakOBJ)
		activatesound(speakOBJ, MODESOUNDACTION, 0, NOSTOPCURRENTSOUNDS);
	delete moveobjects;
	return(k);
}
//=================================
int GetDefaultModeForRightClick(OBJ *a, OBJ *destobj, int playernr)
{
	int isenemy = 0, modemove;
	isenemy = player_aliance(playernr, destobj->playernr);
	switch (alldattbl.units_dat->RightClickAction[a->SC_Unit])
	{
	case UNIT_AIACTION_RIGHTCLICK_NOCMD_AUTOATACK:				//0
	case UNIT_AIACTION_RIGHTCLICK_NOMOVE_NORMALATACK:			//3
		if (isenemy == ENEMYOBJ)
			modemove = MODEATACK;
		else
			modemove = MODENON;
		break;
	case UNIT_AIACTION_RIGHTCLICK_NORMALMOVE_NORMALATACK:		//1
		if (isenemy == ENEMYOBJ)
			modemove = MODEATACK;
		else
			modemove = MODEMOVE;
		break;
	case UNIT_AIACTION_RIGHTCLICK_NORMALMOVE_NOATACK:			//2
		if (GetSpaceProvided(a->SC_Unit))
			if (GetSpaceRequired(destobj->SC_Unit) > 0)
			{
				modemove = MODELOADUNITS;
				break;
			}
		modemove = MODEMOVE;
		break;
	case UNIT_AIACTION_RIGHTCLICK_HARVEST:						//4
		if (isenemy == ENEMYOBJ)
		{
			modemove = MODEATACK;
		}
		else
		{
			if (IsResourceContainer(destobj->SC_Unit) && !a->carryobj)
			{
				if (destobj->SC_Unit == SC_GEYSEROBJ)
				{
					//show cannot gather from geyser
					activatesound(a, MODESOUNDERROR, 0, STOPCURRENTSOUNDS);
					playinfoadvisorsound(playernr, GetUnitRace(a->SC_Unit), ADVGEYSERNOTREADY, PLAYADVISOR_ONLYTEXT);
					modemove = MODENON;
				}
				else
				{
					modemove = MODEGATHER;
				}
			}
			else
			{
				if (a->carryobj && IsResourceDepot(destobj->SC_Unit))
				{
					modemove = MODERETURNCARGO;
				}
				else
				{
					modemove = MODEMOVE;
				}
			}
		}
		break;
	case UNIT_AIACTION_RIGHTCLICK_HARVEST_REPAIR:				//5
		if (isenemy == ENEMYOBJ)
		{
			modemove = MODEATACK;
		}
		else
		{
			if (IsResourceContainer(destobj->SC_Unit) && !a->carryobj)
			{
				if (destobj->SC_Unit == SC_GEYSEROBJ)
				{
					//show cannot gather from geyser
					activatesound(a, MODESOUNDERROR, 0, STOPCURRENTSOUNDS);
					playinfoadvisorsound(playernr, GetUnitRace(a->SC_Unit), ADVGEYSERNOTREADY, PLAYADVISOR_ONLYTEXT);
					modemove = MODENON;
				}
				else
				{
					modemove = MODEGATHER;
				}
			}
			else		//check if need to repair
			{
				if (a->carryobj && IsResourceDepot(destobj->SC_Unit))
				{
					modemove = MODERETURNCARGO;
				}
				else
				{
					if (IsMechanical(destobj->SC_Unit) &&
						IsTerranRace(destobj->SC_Unit) &&
						destobj->health != GetUnitMaxHealth(destobj->SC_Unit))
					{
						modemove = MODEREPAIR;
					}
					else
					{
						modemove = MODEMOVE;
					}
				}
			}
		}
		break;
	case UNIT_AIACTION_RIGHTCLICK_NOTHING:						//6
		modemove = MODENON;
		break;
	default:
		modemove = MODENON;
		break;
	}
	if (IsInvincibleOBJ(destobj))
	{
		if (destobj->SC_Unit >= SC_ZERGBEACONOBJ && destobj->SC_Unit <= SC_PROTOSSFLAGBEACONOBJ)
			modemove = MODEACTIVATE;
		else
			if (modemove == MODEATACK)
				modemove = MODEMOVE;
	}
	return(modemove);
}
//=================================
OBJ* GetMaxRankOBJ(SelectionObjs *list)
{
	int maxrank = -1, unitrank;
	struct OBJ *maxrankOBJ = NULL;
	OBJ *o;
	list->EnumListInit();
	while ((o = (OBJ *)list->GetNextListElem(NULL)))
	{
		unitrank = GetUnitSublabel(o->SC_Unit);
		if (unitrank > maxrank)
		{
			maxrank = unitrank;
			maxrankOBJ = o;
		}
	}
	return(maxrankOBJ);
}
//=================================
void sortselectedunits(struct OBJ *a[], int maxcount)
{
	int i, j, size1, size2;
	for (i = 0;i < maxcount - 1;i++)
	{
		if (a[i])
			size1 = GetSpaceRequired(a[i]->SC_Unit);
		else
			size1 = 8;
		for (j = i + 1;j < maxcount;j++)
		{
			if (a[j])
				size2 = GetSpaceRequired(a[j]->SC_Unit);
			else
				size2 = 8;
			if (size1 > size2)
			{
				size1 = size2;
				swapOBJ(&a[i], &a[j]);
			}
		}
	}
}
//=================================
void transportanalizeobj(struct OBJ *a)
{
	int weightobj, i, j, pos = 0;
	struct OBJ *c;
	memset(&a->loaded->cellobj, -1, sizeof(a->loaded->cellobj));

	for (i = 0;i < a->loaded->nrofloadedunits;i++)
	{
		c = a->loaded->loadedunits[i];
		weightobj = GetSpaceRequired(c->SC_Unit);
		if (weightobj > 1)
			if (pos & 1)
				pos++;
		for (j = 0;j < weightobj;j++)
			a->loaded->cellobj[pos + j] = i;
		pos += weightobj;
	}
}
//=================================
int unloadcoordxy[4][2] = {
							{-15,-10},
							{ 15,-10},
							{-15, 20},
							{ 15, 20}
};
//=================================
void LoadObjInObj(struct OBJ *a, struct OBJ *c)
{
	signed char weightobj, maxweight;
	if (!c->loaded)
	{
		c->loaded = (struct LOADED *) wmalloc(sizeof(struct LOADED));
		memset(c->loaded, 0, sizeof(struct LOADED));
		c->loaded->nrunittounload = -1;
	}
	weightobj = GetSpaceRequired(a->SC_Unit);
	if (weightobj <= 0)
		return;
	maxweight = GetSpaceProvided(c->SC_Unit);
	if (c->loaded->weight + weightobj <= maxweight)
	{
		c->loaded->weight += weightobj;
		c->loaded->loadedunits[c->loaded->nrofloadedunits] = a;
		if (c->SC_Unit == SC_BUNKEROBJ)
		{
			//if transport is bunker need to chenge coordinates to centered in bunker
			SetOBJxy256(a, GetOBJx256(c) + (unloadcoordxy[c->loaded->nrofloadedunits][0] << 8),
				GetOBJy256(c) + (unloadcoordxy[c->loaded->nrofloadedunits][1] << 8));
			ForceKartChanges(a);
		}
		c->loaded->nrofloadedunits++;
		a->in_transport = c;
		moveobj(a, NULL, MODESTOP, NOSHOWERROR);
		Play_sfxdata(GetOBJx(c), GetOBJy(c), SFXDATA_ZINTRANSPORT + GetUnitRace(c->SC_Unit), 2);
		//disable drawing in transport unit
		a->prop |= (VARINTRANSPORT | VARCANTSELECT | VARNOTHERE);
		a->mainimage->HideChildsImgFlag();
		a->mainimage->HideImgFlag();
		if (a->subunit)
		{
			a->subunit->prop |= (VARINTRANSPORT | VARCANTSELECT | VARNOTHERE);
			a->subunit->mainimage->HideChildsImgFlag();
			a->subunit->mainimage->HideImgFlag();
		}

		sortselectedunits(c->loaded->loadedunits, 8);
		transportanalizeobj(c);
		doselectedOBJbit(a, a->playernr, 0);
		deselectobj(a);
		if (SelectedUnits.Count() == 1)
		{
			ifselectTRANSPORTS((OBJ *)SelectedUnits.GetElem(0, NULL));
		}
		DeleteOldObjPointers(a);
	}
	//if selected transport need to refresh cargo
	if (transportplaceobj == c)
	{
		AnalizeAllTransportUnits(c, transportplace, &transportplaceobj);
	}
}
//=================================
int GetOBJNrInTransport(OBJ *c, OBJ *a)
{
	int i;
	for (i = 0;i < c->loaded->nrofloadedunits;i++)
		if (c->loaded->loadedunits[i] == a)
			return(i);
	DEBUGMESSCR("BAD TRANSPORT UNIT\ncontinue run is bad way\n");
	return(-1);
}
//=================================
void FreeOBJFromTransport(OBJ *c, OBJ *a, int nrofunloadobj)
{
	int weightobj;
	a->in_transport = NULL;
	weightobj = GetSpaceRequired(a->SC_Unit);
	c->loaded->weight -= weightobj;
	c->loaded->nrofloadedunits--;
	if (c->loaded->nrofloadedunits)
	{
		c->loaded->loadedunits[nrofunloadobj] = c->loaded->loadedunits[c->loaded->nrofloadedunits];
	}
	c->loaded->loadedunits[c->loaded->nrofloadedunits] = NULL;
	if (!c->loaded->nrofloadedunits)
	{
		wfree(c->loaded);
		c->loaded = NULL;
	}
	if (c->loaded)
	{
		sortselectedunits(c->loaded->loadedunits, 8);
		transportanalizeobj(c);
	}
	if (transportplaceobj == c)
	{
		AnalizeAllTransportUnits(c, transportplace, &transportplaceobj);
	}
}
//=================================
void UnLoadObjInObj(struct OBJ *c, int nrofunloadobj, int typeofnewxycoords, int deltax, int deltay, int silentlyflag)
{
	if (c->loaded)
	{
		int x = -1, y = -1;
		struct OBJ *a;
		a = c->loaded->loadedunits[nrofunloadobj];
		if (!a)
			return;
		switch (typeofnewxycoords)
		{
		case NEWXYCOORDS:
			getcoordofnewunit(c, a->SC_Unit, &x, &y);
			if (x == -1 || y == -1)
				return;//cannot unload unit in unloaded space
			break;
		case NOCHANGEXYCOORDS:
			x = GetOBJx(a);
			y = GetOBJy(a);
			break;
		}
		FreeOBJFromTransport(c, a, nrofunloadobj);

		if (typeofnewxycoords == NEWXYCOORDS)
		{
			ChangeObjXY(a, x, y);
			ForceKartChanges(a);
		}
		a->prop &= ~(VARINTRANSPORT | VARCANTSELECT | VARNOTHERE);
		a->mainimage->ShowChildsImgFlag();
		a->mainimage->ShowImgFlag();
		if (a->subunit)
		{
			a->subunit->prop &= ~(VARINTRANSPORT | VARCANTSELECT | VARNOTHERE);
			a->subunit->mainimage->ShowChildsImgFlag();
			a->subunit->mainimage->ShowImgFlag();
			ChangeSubUnitCoords(a->subunit, a);
			ForceKartChanges(a->subunit);
		}
		moveobj(a, NULL, MODESTOP, NOSHOWERROR);
		if (!silentlyflag)
			Play_sfxdata(GetOBJx(c), GetOBJy(c), SFXDATA_ZOUTTRANSPORT + GetUnitRace(c->SC_Unit), 2);

		if (!IsOnSkyOBJ(a))
			SetObjWalk8(&map, a, SETWALK);
	}
}
//=================================
void dieobj_silently(OBJ *a)
{
	a->mainimage->HideChildsImgFlag();
	a->mainimage->HideImgFlag();
	a->mainimage->EnableExecScript();

	a->prop |= VARDIESILENTLY;
	dieobj(a);
}
//=================================
void dieobj(struct OBJ *a)
{
	SCUNIT SC_Unit;
	struct OBJstruct *b;
	int i, needtodestroyonexit, inegg, changesupply = 0;
	struct OBJ *a1, *newobj;

	if (a->prop & VARMARKFORDESTROY)
		return;			//already died
	if (a->in_transport)
	{
		FreeOBJFromTransport(a->in_transport, a, GetOBJNrInTransport(a->in_transport, a));
		dieobj_silently(a);
		return;
	}
	addremoveuniteffectfrommap(a, -1, &map);

	a->currentspeed = 0;
	a->prop &= ~VARACCELERATIONBIT;

	moveobj(a, NULL, MODEDIE, NOSHOWERROR);

	needtodestroyonexit = 0;
	deselectobj(a);
	SC_Unit = a->SC_Unit;
	b = loadobj(SC_Unit);
	switch (SC_Unit)
	{
	case SC_ASSIMILATOROBJ:
	case SC_EXTRACTOROBJ:
	case SC_REFINERYOBJ:
		newobj = a->resourceobj;
		if (newobj)			//somebody extarct gas from here
		{
			newobj->resourceobj = NULL;
			newobj->prevresourceobj = NULL;
			a->resourceobj = NULL;
			a->prevresourceobj = NULL;
			//restore visibility for unit
			newobj->finalOBJ = NULL;
			newobj->mainimage->ShowChildsImgFlag();
			newobj->mainimage->ShowImgFlag();
			newobj->prop &= ~VARNOTHERE;
			moveobj(newobj, NULL, MODESTOP, NOSHOWERROR);
		}
		break;
	case SC_NUCLEARSILOOBJ:
		if (getchilds(a))
		{
			a1 = a->childs->parentof[0];
			delchild(a, a1);
			dieobj_silently(a1);
			PLAYER[a->playernr].nukes--;
			//				ChangeSupply(a->playernr,SC_NUKEOBJ,MINUSFACTOR);
		}
		break;
	case SC_GHOSTOBJ:
		//remove nuke with trail if need
		if (a->doubleunit)
			GhostNUKECancel(a, GHOSTCANCELNUKE);
		break;
	}
	if (a->loaded)
	{
		int maxloadedunits = a->loaded->nrofloadedunits;
		if (SC_Unit != SC_BUNKEROBJ)
		{
			for (i = maxloadedunits - 1;i >= 0;i--)
			{
				//				ChangeSupply(a->loaded->loadedunits[i]->playernr,a->loaded->loadedunits[i]->SC_Unit,MINUSFACTOR);
				dieobj_silently(a->loaded->loadedunits[i]);
			}
		}
		else
		{
			//			oldsnd = BlockSoundToPlay();
			for (i = maxloadedunits - 1;i >= 0;i--)
				UnLoadObjInObj(a, 0, NOCHANGEXYCOORDS, 0, 0, 1);
			//			RestoreSoundToPlay(oldsnd);
		}
	}
	if (GetConstructNR(a))
	{
		ConstructUnitsReleaseSupply(a);
		DestroyConstrStruct(a);
	}
	if (a->addonobj)
	{
		if (IsAddon(a->addonobj->SC_Unit))
		{
			if (IsOBJUnderConstruct(a->addonobj))
			{
				dieobj(a->addonobj);
			}
			else
			{
				a->addonobj->playernr = GREYNEUTRALCOLORPLAYER;
			}
		}
		else
		{
			moveobj(a->addonobj, NULL, MODEESCAPE1, NOSHOWERROR);//cancel construct addon
		}
		if (a->addonobj)
		{
			DettachAddon(a);
		}
	}
	if (a->resourceobj)
	{
		a->resourceobj->resourceobj = NULL;
		a->resourceobj->prevresourceobj = NULL;
		a->resourceobj = NULL;
		a->prevresourceobj = NULL;
	}
	if (a->replaceobj)
	{
		a->replaceobj->prop &= ~(VARCANTSELECT | VARNOTWORK);
		a->replaceobj->mainimage->ShowChildsImgFlag();
		a->replaceobj->mainimage->ShowImgFlag();
		a->replaceobj->mainimage->EnableExecScript();
		a->replaceobj->data.resource.resource_count = a->data.resource.resource_count;
		if (a->replaceobj->replaceobj)
			a->replaceobj->replaceobj = NULL;
		a->replaceobj = NULL;
	}
	if (a->doubleunit)
	{
		a->doubleunit->doubleunit = NULL;
		dieobj(a->doubleunit);
		a->doubleunit = NULL;
	}
	if (a->constrobj)
	{
		FinishMainUnitConstruct(a);
	}
	if (a->myparent&&a->myparent->childs)
	{
		delchild(a->myparent, a);
	}
	//	  a->invisiblecolors=MININVISIBLECOLOR;
	SetCloakedFlag(a, 0);
	SetMageAtr(&a->atrobj, ATRINVISIBLE, 0);
	SetMageAtr(&a->atrobj, ATRDETECTOR, 0);

	if (SC_Unit == SC_EGGOBJ || SC_Unit == SC_MUTALISKCOCOONOBJ || SC_Unit == SC_LURKEREGGOBJ)
		SC_Unit = a->SC_ConstrUnit;
	if (SC_Unit != SC_NOUNITNR)
	{
		if (!IsHallucination(a))
		{
			if (IsReadyOBJ(a))
			{
				changesupply = 1;
			}
			else
			{
				if (!IsBuild(SC_Unit))
					changesupply = 1;
				else
					if (a->SC_FromUnit == SC_HIVEOBJ || a->SC_FromUnit == SC_LAIROBJ)	//hive or lair
						changesupply = 1;
			}
			if (changesupply)
			{
				switch (a->SC_Unit)
				{
				case SC_EGGOBJ:
					if (!GetSupplyProvided(SC_Unit))				//except for overlord & yggdrasill
					{
						inegg = GetInEgg(SC_Unit);
						for (i = 0;i <= inegg;i++)
						{
							ChangeSupply(a->playernr, SC_Unit, MINUSFACTOR);
							ChangeMaxSupply(a->playernr, SC_Unit, MINUSFACTOR);
						}
					}
					break;
				default:
					ChangeSupply(a->playernr, SC_Unit, MINUSFACTOR);
					ChangeMaxSupply(a->playernr, SC_Unit, MINUSFACTOR);
					break;
				}
			}
		}
	}
	//	  a->prop&=~VARBEGINFIREPARSED;
	RemoveFromDestination(a);

	//	  if (needtodestroyonexit)
	//		dieobj_silently(a);

	if (SC_Unit == SC_PYLONOBJ && !IsOBJUnderConstruct(a))
		DelPylonArea(&map, a, a->playernr);
	if (IsResource(a, SC_MINERALSOBJ))
	{
		int xp = (GetOBJx(a) - GetUnitWidthAndHeight(a->SC_Unit, UNITDIM_WIDTH) / 2) / SIZESPRLANSHX - 3;
		int yp = (GetOBJy(a) - GetUnitWidthAndHeight(a->SC_Unit, UNITDIM_HEIGHT) / 2) / SIZESPRLANSHY - 3;
		DelMarkOn32(xp, yp, 8, 7, map.constrmap);
	}
	if (!IsOnSkyOBJ(a))
		SetObjWalk8(&map, a, CLEARWALK);
	if (cbuild == a)
	{
		highMouse->Construct.SC_BuildUnit = 0;
		cbuild = NULL;
	}
	if (!IsHallucination(a))
	{
		if (a->atackernr != -1)
		{
			if (TRIG_ChangeStat)
			{
				if (IsBuild(a->SC_Unit))
				{
					PLAYER[a->playernr].statplayer.stat[STATPLAYER_BUILDSLOST] += GetBuildUnitScore(a->SC_Unit);
					PLAYER[a->atackernr].statplayer.stat[STATPLAYER_BUILDSRISED] += GetDestroyUnitScore(a->SC_Unit);
				}
				else
				{
					PLAYER[a->playernr].statplayer.stat[STATPLAYER_UNITSLOST] += GetBuildUnitScore(a->SC_Unit);
					PLAYER[a->atackernr].statplayer.stat[STATPLAYER_UNITSKILLED] += GetDestroyUnitScore(a->SC_Unit);
				}
			}
			if (a->whoatack)
				a->whoatack->kills++;
		}
		add_unit_stat(&map, UNITSTAT_DEATH, a->playernr, a->SC_Unit);

		//		sub_unit_stat(&map,UNITSTAT_PRODUCING,a->playernr,a->SC_Unit);
		if (IsReadyOBJ(a))
		{
			//			sub_unit_stat(&map,UNITSTAT_HAVE,a->playernr,a->SC_Unit);
		}
	}
	if (a->carryobj)
	{
		if (IsPickupUnit(a->carryobj->SC_Unit))
		{
			//pickup unit will stopped
			a->carryobj->modemove = MODESTOP;
			a->carryobj->prop &= ~VARCANTSELECT;

		}
		a->carryobj->carryobj = NULL;
		a->carryobj = NULL;
	}
	if (a->subunit)
	{
		if (!IsSubUnit(a->SC_Unit))
		{
			dieobj(a->subunit);
		}
		a->subunit = NULL;
	}
	RemoveBloodFlameOverlays(a, 0);
	if (b->creeptype != NOCREEPBUILD)
		if (!IsOBJUnderConstruct(a) || (a->SC_FromUnit != SC_NOUNITNR && a->SC_FromUnit != SC_DRONEOBJ))
		{
			EmptyCreepTimed(a, b);
		}

}
//=================================
void WakeUpOneInterceptor(OBJ *myparent, OBJ *a, OBJ *destobj, int childnr)
{
	//	  if (a && destobj)
	if (a)
	{
		a->prop &= ~(VARNOTHERE | VARINBASE);
		//		SetModeMove(a,MODESTOP);
		a->finalOBJ = NULL;
		a->data.interceptor.refreshmyparent = INTERCEPTORREFRESHMYPARENT;
		a->mainimage->AllUnitDirection256(myparent->mainimage->side + childnr * 32);

		a->mainimage->EnableExecScript();
		a->mainimage->ShowChildsImgFlag();
		a->mainimage->ShowImgFlag();
		ChangeTypeOfProp(a, PROPNORMAL1);
		SetOBJxy256(a, GetOBJx256(myparent), GetOBJy256(myparent));
		moveobj(a, destobj, MODEMOVEFORWARD, 1, 0, NOSHOWERROR | XYNOTCOORDS);
	}
}
//=================================
void WakeUpOneInterceptor(OBJ *a, OBJ *destobj)
{
	WakeUpOneInterceptor(a->myparent, a, destobj, a->childnr);
}
//=================================
void WakeUpInterceptors(OBJ *a, OBJ *destobj)
{
	//set childs to be unlocked for fly,atack,etc.
	if (a->childs)
		for (int i = 0;i < MAXCHILDS;i++)
			if (a->childs->parentof[i])
			{
				if (a->childs->parentof[i]->prop & VARINBASE)
				{
					WakeUpOneInterceptor(a, a->childs->parentof[i], destobj, i);
				}
				else
				{
					moveobj(a->childs->parentof[i], destobj, MODEMOVEFORWARD, 1, 0, NOSHOWERROR | XYNOTCOORDS);
				}
			}
}
//=================================
void ReturnedToBase(struct OBJ *a)//after moved to base
{
	if (a->myparent->childs)
	{
		//object now in base need wait factor
		a->prop |= VARNOTHERE | VARINBASE;
		a->finalOBJ = NULL;
		a->mainimage->DisableExecScript();
		a->mainimage->HideChildsImgFlag();
		a->mainimage->HideImgFlag();
		a->shield = GetUnitMaxShield(a->SC_Unit);
	}
	else
		savelog("return non-child unit to base", 0);
}
//=================================
void makeallblinking(void)
{
	int i;
	OBJ *a;
	for (i = 0;i < MaxObjects;i++)
	{
		a = objects[i];
		if (a->blinkvalue)
			a->blinkvalue--;
	}
}
//=================================
void SetBlinkOBJ(struct OBJ *destobj)
{
	if (destobj)
		destobj->blinkvalue = BLINKINGTICKS;
}
//=================================
int getcoordfrompatrate(int sx, int sy, int *retx, int *rety)
{
	//							a = 0
	//			 bbbbbb			b = 1
	//			c	   a		c = 2
	//			c	   a		d = 4
	//			 dddddd
	//
	int side = myrand(4);//get random side 0-3
	switch (side)
	{
	case 0:			//side a
		*retx = sx - 1;
		*rety = myrand(sy);
		break;
	case 1:			//side b
		*retx = myrand(sx);
		*rety = 0;
		break;
	case 2:			//side c
		*retx = 0;
		*rety = myrand(sy);
		break;
	case 3:			//side d
		*retx = myrand(sx);
		*rety = sy - 1;
		break;
	}
	return side;
}
//============================================
void ChangeObjXY(struct OBJ *a, int x, int y)
{
	if (x < 0)
		x = SIZESPRLANSHX / 2;
	if (x > MAXXMAP*SIZESPRLANSHX)
		x = MAXXMAP * SIZESPRLANSHX - SIZESPRLANSHX / 2;
	if (y < 0)
		y = SIZESPRLANSHY / 2;
	if (y > MAXYMAP*SIZESPRLANSHY)
		y = MAXYMAP * SIZESPRLANSHY - SIZESPRLANSHY / 2;
	SetOBJxy256(a, x << 8, y << 8);
}
//==========================================
void ChangeObjXYDelta(struct OBJ *a, int xdelta, int ydelta)
{
	ChangeObjXY(a, GetOBJx(a) + xdelta, GetOBJy(a) + ydelta);
}
//==========================================
void DeleteOldObjPointers(struct OBJ *a)
{
	int i;
	struct OBJ *b;
	//	  struct MAGEARRAY *c;
		//find pointers in all objects
	for (i = 0;i < MaxObjects;i++)
	{
		b = objects[i];
		if (b->finalOBJ == a)
		{
			b->finalOBJ = NULL;
			//			b->prop &= ~VARMOVEOBJACT;
		}
		if (b->carryobj == a)
			b->carryobj = NULL;
		if (b->atrobj.arbiterrecallobj == a)
			//found an recalled obj without arbiter - master of this mage
			b->atrobj.arbiterrecallobj = NULL;
		// if have child units verify if child dead
		if (b->childs)
		{
			for (int j = 0;j < getchilds(b);j++)
			{
				if (b->childs->parentof[j] == a)
					b->childs->parentof[j] = NULL;
			}
		}
		if (b->resourceobj == a)
			b->resourceobj = NULL;
		if (b->prevresourceobj == a)
			b->prevresourceobj = NULL;
		if (b->in_transport == a)
			b->in_transport = NULL;
		if (b->whoatack == a)
			b->whoatack = NULL;
		if (b->finalOBJ == a)
		{
			moveobj(b, NULL, MODESTOP, NOSHOWERROR);
			b->finalOBJ = NULL;
		}
		if (b->constrobj == a)
			b->constrobj = NULL;
		DelOBJFromModeList(b, a);		//search in obj b ref to obj a
	}
	OBJ *o;
	SelectedUnits.EnumListInit();
	while ((o = (OBJ *)SelectedUnits.GetNextListElem(NULL)))
	{
		if (o == a)
			SelectedUnits.Remove(a);
	}
	//find pointers in sound channels
	FreeChannelWithObj(a);
	QueueDelObj(a);
	//.RemoveOBJFromEffects(a);//done in destroyobj
}
//==================================
int QueueDelObj(struct OBJ *obj)
{
	int i, ret = 0;
	struct QUEUEACTION *act;
	for (i = 0;i < QueueGame.GetQueueElements();i++)
	{
		act = (struct QUEUEACTION *)(QueueGame.GetElemNr(i)->ID);
		if (act->destobj == obj || act->obj == obj)
		{
			QueueAction(act, DESTROYQUEUE);
			QueueGame.DelQueue(i--, 1);
			ret++;
		}
	}
	return ret;
}
//==========================================
void RemoveFromDestination(OBJ *a)
{
	int i, resval;
	OBJ *a1, *tempobj;
	if (IsUnitBaseBuild(a) == 1)
	{
		for (i = 0;i < MaxObjects;i++)
		{
			a1 = objects[i];
			if (IsWorkerUnit(a1->SC_Unit))
				if (a1->finalOBJ == a)
				{
					a1->finalOBJ = NULL;
				}
		}
	}
	if (IsResource(a, a->SC_Unit))
	{
		for (i = 0;i < MaxObjects;i++)
		{
			a1 = objects[i];
			if (IsWorkerUnit(a1->SC_Unit))
				if (a1->finalOBJ == a)
				{
					a1->finalOBJ = NULL;
					DelAllModeMoves(a, 0);
					if (IsNonNeutralFlag(a->SC_Unit))	//unit is extractor or refinery or assimilator
					{
						//if gas deposit is destroyed unit stopped
						moveobj(a1, NULL, MODESTOP, NOSHOWERROR);
					}
					else
					{
						//if mineral field (destroyed or dissapeared), unit stopped
						tempobj = GetNearResource(a1, a->SC_Unit, &resval);
						if (tempobj)
							moveobj(a1, tempobj, MODEGATHER, NOSHOWERROR);
						else
							if (resval)
								moveobj(a1, NULL, MODEWAITHARVESTMINERAL, NOSHOWERROR);
							else
								moveobj(a1, NULL, MODESTOP, NOSHOWERROR);
					}
				}
		}
	}
}
//==================================
void FinishMainUnitConstruct(OBJ *a)
{
	if (a->constrobj->SC_Unit == SC_SCVOBJ)
	{
		a->constrobj->prop &= ~VARMOVETHROWUNITS;
		DelAllModeMoves(a->constrobj, 0);
		ApplyNextModeMove(a->constrobj);
		ChangeTypeOfProp(a->constrobj, PROPNORMAL1);
	}
	a->constrobj->constrobj = NULL;
	a->constrobj = NULL;
	ChangeTypeOfProp(a, PROPNORMAL1);
}
//==================================
void ContinueSCVConstruct(OBJ *a, OBJ *aa)//a-mainunit,b-constructedunit
{
	a->constrobj = aa;
	aa->constrobj = a;
	a->prop |= VARMOVETHROWUNITS;
	a->timerepair = 0;
	ChangeTypeOfProp(a, PROPDECONSTRUCT);
}
//==================================
void applyrescuableunits(void)
{
	int i, haverescued = 0;
	OBJ *a, *c;
	//	  if (MINIMAPREFRESHCYCLE)
	{
		if (ifhaverescuableplayers)
		{
			for (i = 0;i < MaxObjects;i++)
			{
				a = objects[i];
				//if rescuable unit/player need to apply to seeit player
				if (map.pl_iowner[a->playernr] == OWNER_RESCUABLE)
				{
					//					printf("rescuable=%s\n",getOBJname(a->SC_Unit));
					c = SearchOBJforOBJ(a, SEARCHMODE_RESCUABLE);
					if (c)
					{
						MakeMindControl(a, NUMBGAMER, a->color);
						SetBlinkOBJ(a);
						if (++haverescued > 2)
							break;
					}
				}
			}
			if (haverescued)
			{
				play_race_rescue(gameconf.pl_race[NUMBGAMER], 4, 0);
			}
		}
	}
}
//==================================
void SetInvisibleUnit(struct OBJ *a)
{
	OBJSetInvisibleColors(a, MAXINVISIBLECOLOR);
	SetMageAtr(&a->atrobj, ATRINVISIBLE, ATRMAGEINFINITE);
	SetCloakedFlag(a, 1);
}
//==================================
void setpropertiestounit(struct OBJ *a, int special_props, int state_flags)
{
	int mask;
	OVERLAY_IMG *tempimg;
	if (state_flags&UNITONMAP_STATEFLAGS_CLOAK)
		if (special_props&UNITONMAP_STATEFLAGS_CLOAK)
		{
			//check if cloak is evolved
//			mask=0;
//			if (IsHeroUnit(a->SC_Unit))
//				mask=1;
//			else
//			{
//				SetInvisibleUnit(a);
//				a->mainimage->newgrpmethod = DISTORTION;
			addmage(a, ATRINVISIBLE, a->mana);
			a->prop |= VARDECRMAGE;
			ChangeTypeOfProp(a, PROPNORMAL2);
			//			}
		}
	if (state_flags&UNITONMAP_STATEFLAGS_BURROW)
		if (special_props&UNITONMAP_STATEFLAGS_BURROW)
		{
			/*			//check if burrow is evolved
						mask=0;
						if (IsHeroUnit(a->SC_Unit))
							mask=1;
						else
							if (a->SC_Unit==SC_LURKEROBJ || a->SC_Unit==SC_VULTUREMINEOBJ)
								mask=1;
							else
								mask = GetTechTree(&map,a->playernr,TECHDATA_DAT_BURROWINGTECH);
			*/
			mask = 1;
			if (mask && ChangeTypeOfProp(a, PROPNORMAL2) != -1)
			{
				SetBurrowFlag(a, 1);
				SetMageAtr(&a->atrobj, ATRINVISIBLE, WRAITHEPODARBITEROM);
				a->mainimage->invisiblecolors = MAXINVISIBLECOLOR;
				SetOBJIScriptNr(a, ISCRIPTNR_SPECIALSTATE2, ISCRIPTNR_EXECUTE);	//burrowing state
			}
		}
	if (state_flags&UNITONMAP_STATEFLAGS_HALLUCINATION)
	{
		if (special_props&UNITONMAP_STATEFLAGS_HALLUCINATION)
		{
			SetHallucinationOBJ(a);
		}
	}
	if (state_flags&UNITONMAP_STATEFLAGS_BUILDINTRANSIT)
	{
		if (special_props&UNITONMAP_STATEFLAGS_BUILDINTRANSIT)
		{
			SIGOrder_LiftOff(a);
			a->mainimage->childlists->EnumListInit();
			while ((tempimg = (OVERLAY_IMG *)a->mainimage->childlists->GetNextListElem(NULL)))
			{
				if (tempimg->flags & SC_IMAGE_FLAG_IMGUNDER)	//if this is shadow image
				{
					tempimg->ydelta = AIRUNITS_START_YPOS;
					break;
				}
			}
			SetOBJIScriptNr(a, ISCRIPTNR_SPECIALSTATE1, ISCRIPTNR_EXECUTE);		//air state
		}
	}
	if (state_flags&UNITONMAP_STATEFLAGS_INVINCIBLE)
	{
		if (special_props&UNITONMAP_STATEFLAGS_INVINCIBLE)
		{
			SetInvincibleOBJ(a, 1);
		}
	}
}
//==================================
struct OBJ *createunitwithproperties(int xpos, int ypos, int unit_id, int playernr,
	int special_prop, int special_prop2, int state_flags,
	int hp, int sp, int ep, int resnr, int unitsinhangar)
{
	SCUNIT hangarunit;
	OBJ *a;
	int hp_perc, sp_perc, ep_perc;
	if (special_prop2 & UNITONMAP_HPISVALID)
		hp_perc = hp;
	else
		hp_perc = 100;
	if (special_prop2 & UNITONMAP_SPISVALID)
		sp_perc = sp;
	else
		sp_perc = 100;
	if (special_prop2 & UNITONMAP_EPISVALID)
		ep_perc = ep;
	else
		ep_perc = 100;
	if (resnr)	//geyser or mineralfield
		ep_perc = resnr << 8;
	a = createobjmanwithlife(xpos, ypos, unit_id, playernr, sp_perc, hp_perc, ep_perc, 1);
	setpropertiestounit(a, special_prop, state_flags);
	if (a && unitsinhangar)
	{
		switch(unit_id)
		{
			case SC_REAVEROBJ:
			case SC_HERO_WARBRINGEROBJ:
				hangarunit = SC_SCARABOBJ;
				break;
			case SC_CARRIEROBJ:
			case SC_HERO_GANTRITHOROBJ:
				hangarunit = SC_INTERCEPTOROBJ;
				break;
			default:
				DEBUGMESSCR("unit(%d) with hangar nrs\n",unit_id);
				return(a);
		}
		for (int i=0; i < unitsinhangar;i++)
		{
			CreateUnitInUnit(a, hangarunit, 0, GetOBJx(a), GetOBJy(a) );
		}
	}
	return(a);
}
//==================================
int SearchObjInArea(struct OBJ *a, struct XY *area)
{
	if (GetOBJx(a) <= area->x1 || GetOBJx(a) >= area->x2 || GetOBJy(a) <= area->y1 || GetOBJy(a) >= area->y2)
		return(0);
	return(1);
}
//==================================
void SetAtackTick(OBJ *a)
{
	static unsigned char atacktick = 0;
	a->searchforatack_tick = atacktick++;
	if (atacktick >= MAXWAIT_SEARCHATACK_TICK)
		atacktick = 0;
}
//=================================
#define UNITATACKFUNCTYPE_NONE					0
#define UNITATACKFUNCTYPE_EVERYUNITS			1
#define UNITATACKFUNCTYPE_DOODADS				2
#define UNITATACKFUNCTYPE_BUNKERS				3
#define UNITATACKFUNCTYPE_REAVERS				4
#define UNITATACKFUNCTYPE_CARRIERS				5
#define UNITATACKFUNCTYPE_VULTUREMINES			6
#define UNITATACKFUNCTYPE_INTERCEPTORS			7
#define UNITATACKFUNCTYPE_NOTATACKINCLOAKEDMODE 8
//=================================
int GetOBJAtackWithoutWeapons(SCUNIT SC_Unit)
{
	switch (SC_Unit)
	{
	case SC_BUNKEROBJ:
		return(UNITATACKFUNCTYPE_BUNKERS);
	case SC_REAVEROBJ:
	case SC_HERO_WARBRINGEROBJ:
		return(UNITATACKFUNCTYPE_REAVERS);
	case SC_CARRIEROBJ:
	case SC_HERO_GANTRITHOROBJ:
		return(UNITATACKFUNCTYPE_CARRIERS);
	case SC_VULTUREMINEOBJ:
		return(UNITATACKFUNCTYPE_VULTUREMINES);
	case SC_INTERCEPTOROBJ:
		return(UNITATACKFUNCTYPE_INTERCEPTORS);
	default:
		if (IsGroundUnit(SC_Unit) && IsCloakable(SC_Unit))
			return(UNITATACKFUNCTYPE_NOTATACKINCLOAKEDMODE);
		if (IsDoodadState(SC_Unit) && !IsInvincibleUnit(SC_Unit))
		{
			//doodad traps
			return(UNITATACKFUNCTYPE_DOODADS);
		}
		return(UNITATACKFUNCTYPE_EVERYUNITS);
	}
}
//=================================
struct OBJ* EveryUnitAtackFunc(OBJ *a, unsigned char weaponmask, unsigned char groundweapon, unsigned char airweapon)
{
	OBJ *a2 = NULL;
	if (weaponmask)
	{
		a2 = FindObjForAtack(a, weaponmask, groundweapon, airweapon, NULL);
		unitprepareforatack(a, a2);
	}
	return(a2);
}
//=================================
struct OBJ* BunkerAtackFunc(OBJ *aa, unsigned char wm, unsigned char gw, unsigned char aw)
{
	unsigned char weaponmask, groundweapon, airweapon, SC_Unit;
	OBJ *a, *a2;
	if (aa->loaded)
	{
		for (int i = 0;i < aa->loaded->nrofloadedunits;i++)
		{
			a = aa->loaded->loadedunits[i];
			if (!a)
				continue;
			SC_Unit = a->SC_Unit;
			weaponmask = 0;
			groundweapon = alldattbl.units_dat->GroundWeapon[SC_Unit];
			if (groundweapon < MAX_WEAPONS_ELEM)
				weaponmask |= 1;
			airweapon = alldattbl.units_dat->AirWeapon[SC_Unit];
			if (airweapon < MAX_WEAPONS_ELEM)
				weaponmask |= 2;
			if (weaponmask)
			{
				a2 = FindObjForAtack(a, weaponmask, groundweapon, airweapon, NULL);
				unitprepareforatack(a, a2);
			}
		}
	}
	return(NULL);
}
//=================================
int CheckMineSpecialUnit(SCUNIT SC_Unit)
{
	return((GetUnitUnknownFlags(SC_Unit) & 0x80) || IsBuild(SC_Unit));//mine safe or build
}
//=================================
struct OBJ* VultureMineUnitAtackFunc(OBJ *a, unsigned char weaponmask, unsigned char groundweapon, unsigned char airweapon)
{
	OBJ *a2 = NULL;
	if (weaponmask)
	{
		a2 = FindObjForAtack(a, weaponmask, groundweapon, airweapon, &CheckMineSpecialUnit);
		unitprepareforatack(a, a2);
	}
	return(a2);
}
//=================================
struct OBJ* DoodadTrapAtackFunc(OBJ *a, unsigned char weaponmask, unsigned char groundweapon, unsigned char airweapon)
{
	OBJ *a2;
	if ((a->prop & VARAUTOMATICDOODAD) || GetDoodadState(a) == DOODAD_TOP_STATE)
		a2 = FindObjForAtack(a, weaponmask, groundweapon, airweapon, NULL);
	else
		a2 = NULL;
	trapprepareforatack(a, a2);
	return(a2);
}
//=================================
struct OBJ* CarrierUnitAtackFunc(OBJ *a, unsigned char weaponmask, unsigned char groundweapon, unsigned char airweapon)
{
	OBJ *a2;
	if (!getchilds(a))
		return(NULL);
	a2 = FindObjForAtack(a, 0x03, WEAPONID_CARRIERRANGE, WEAPONID_CARRIERRANGE, NULL);
	unitprepareforatack(a, a2);
	return(a2);
}
//=================================
struct OBJ* InterceptorUnitAtackFunc(OBJ *a, unsigned char weaponmask, unsigned char groundweapon, unsigned char airweapon)
{
	OBJ *a2 = NULL;
	if (weaponmask)
	{
		if (a->myparent)
		{
			a2 = FindObjForAtack(a->myparent, weaponmask, groundweapon, airweapon, NULL);
			unitprepareforatack(a, a2);
		}
	}
	return(a2);
}
//=================================
struct OBJ* GroundCloakableUnitAtackFunc(OBJ *a, unsigned char weaponmask, unsigned char groundweapon, unsigned char airweapon)
{
	OBJ *a2 = NULL;
	if (IsCloaked(a))
	{
		if (a->modemove != MODEPATROL && !(a->prop & VARMOVEINATACKMODE))
			return(NULL);
	}
	if (weaponmask)
	{
		a2 = FindObjForAtack(a, weaponmask, groundweapon, airweapon, NULL);
		unitprepareforatack(a, a2);
	}
	return(a2);
}
//=================================
struct OBJ* (*Atack_IDFunc[9])(OBJ *a, unsigned char weaponmask, unsigned char groundweapon, unsigned char airweapon) =
{
	NULL,
	&EveryUnitAtackFunc,
	&DoodadTrapAtackFunc,
	&BunkerAtackFunc,
	&EveryUnitAtackFunc,
	&CarrierUnitAtackFunc,
	&VultureMineUnitAtackFunc,
	&InterceptorUnitAtackFunc,
	&GroundCloakableUnitAtackFunc
};
//=================================
struct OBJ* OneUnitSearchGoal(OBJ *a, int ignoremodes, int facedirectionatackonly)
{
	unsigned char unitatack_id;
	unsigned char weaponmask = 0, groundweapon = 0, airweapon = 0;
	SCUNIT SC_Unit;
	OBJ *baseunit = a;
	//neutral units cannot automatic atack
	if (IsAtackerActiveUnit(a) && (map.pl_owner[a->playernr] == OWNER_HUMAN || map.pl_owner[a->playernr] == OWNER_COMPUTER))
	{
		if (IsSubUnit(a->SC_Unit))
			baseunit = a->subunit;
		if (ignoremodes || baseunit->modemove == MODESTOP || baseunit->prop & VARHOLDPOSBIT || baseunit->modemove == MODEPATROL ||
			(baseunit->prop & VARMOVEINATACKMODE) ||
			(IsDoodadState(baseunit->SC_Unit) && !IsInvincibleUnit(baseunit->SC_Unit) && (GetDoodadMoveDirection(baseunit) != DOODAD_MOVE_NONE)))
		{
			//zero if usual unit,	  bunker,reaver,carrie have a special unitatack_id
			SC_Unit = a->SC_Unit;
			unitatack_id = GetOBJAtackWithoutWeapons(SC_Unit);
			switch (unitatack_id)
			{
			case UNITATACKFUNCTYPE_EVERYUNITS:			//all units that not have a special functions
			case UNITATACKFUNCTYPE_DOODADS:				//doodadunits
			case UNITATACKFUNCTYPE_VULTUREMINES:		//vulturemine
			case UNITATACKFUNCTYPE_INTERCEPTORS:
			case UNITATACKFUNCTYPE_NOTATACKINCLOAKEDMODE:
				weaponmask = 0;
				groundweapon = alldattbl.units_dat->GroundWeapon[SC_Unit];
				if (groundweapon < MAX_WEAPONS_ELEM)
					weaponmask |= 1;
				airweapon = alldattbl.units_dat->AirWeapon[SC_Unit];
				if (airweapon < MAX_WEAPONS_ELEM)
					weaponmask |= 2;
				break;
			case UNITATACKFUNCTYPE_BUNKERS://bunker
				break;
			case UNITATACKFUNCTYPE_REAVERS://reaver
				groundweapon = WEAPONID_SCARAB;
				weaponmask = 1;
				if (!a->ammo)
					return NULL;
				break;
			case UNITATACKFUNCTYPE_CARRIERS://carrier
				break;
			case UNITATACKFUNCTYPE_NONE:		//base/turret
				return(NULL);
				break;
			}
			weaponmask |= facedirectionatackonly;
			return ((*Atack_IDFunc[unitatack_id])(a, weaponmask, groundweapon, airweapon));
		}
	}
	return(NULL);
}
//=================================
void SearchForAtacks(void)
{
	int i;
	OBJ *a;
	for (i = 0;i < MaxObjects;i++)
	{
		a = objects[i];
		if (IsFullAutoAttack(a->SC_Unit))
		{
			if (!a->finalOBJ)							//TODO i need to atack unit who atack me
			{
				if (a->searchforatack_tick-- == 0)
				{
					a->searchforatack_tick = MAXWAIT_SEARCHATACK_TICK;
					OneUnitSearchGoal(a, 0, 0);
				}
			}
		}
		else
		{
			if (!a->finalOBJ)
			{
				if (a->searchforatack_tick-- == 0)
				{
					a->searchforatack_tick = MAXWAIT_SEARCHATACK_TICK;
					OneUnitSearchGoal(a, 0, 4);			//atack only face direction
				}
			}
		}
	}
}
//=================================
int UnitIgnoreInvisibles(SCUNIT SC_Unit)
{
	if (SC_Unit == SC_VULTUREMINEOBJ)
		return(1);
	else
		return(0);
}
//=================================
OBJ *FindObjForAtack(OBJ *a,
	unsigned char weaponmask,
	unsigned char groundweapon,
	unsigned char airweapon,
	int(*checkspecialfunc)(SCUNIT))
{
	int deltaz, i, j, minrange, maxrange, mindeltaz[3], weapon_id, wmask;
	int addsiegerange = 0;
	unsigned char neededside;
	OBJ *a2, *findobj[3];
	findobj[NOATACKER] = NULL;					//no atacker
	mindeltaz[NOATACKER] = 256 * 256 * 256;

	findobj[GROUNDATACKER] = NULL;				//groundatacker
	mindeltaz[GROUNDATACKER] = 256 * 256 * 256;

	findobj[AIRATACKER] = NULL;					//airatacker
	mindeltaz[AIRATACKER] = 256 * 256 * 256;
	if (map.pl_owner[a->playernr] == OWNER_COMPUTER)	//if computer need to add range to try siege mode
	{
		if (a->SC_Unit == SC_TANKNORMALTURRETOBJ || a->SC_Unit == SC_HERO_EDMUNDDUKETMTURRETOBJ)
		{
			addsiegerange = 5 * 32;
		}
	}
	for (i = 0;i < MaxObjects;i++)
	{
		a2 = objects[i];
		if (checkspecialfunc)
			if (checkspecialfunc(a2->SC_Unit))
				continue;
		if (UnitIgnoreInvisibles(a->SC_Unit) || !OBJ_VAR_CHK(a2, obj_notdetect, a->playernr))
		{
			//we see this object
			if (player_aliance(a->playernr, a2->playernr) == ENEMYOBJ && OBJ_VAR_CHK(a2, obj_see, a->playernr))
			{
				weapon_id = MAX_WEAPONS_ELEM;
				if ((weaponmask & 1) && !IsOnSkyOBJ(a2))
					weapon_id = groundweapon;
				if ((weaponmask & 2) && IsOnSkyOBJ(a2))
					weapon_id = airweapon;
				if (weapon_id < MAX_WEAPONS_ELEM)
				{
					if (IsActiveUnitForAtack(a2, false))
					{
						//this is enemy, need to check detect vision radius
						minrange = alldattbl.weapons_dat->MinimumRange[weapon_id];
						maxrange = GetTargetAcquisitionRange(a->SC_Unit);
						if (!maxrange)
							maxrange = GetRangeWeaponInPixels(a, weapon_id, a->playernr);
						else
							maxrange *= SIZESPRLANSHX;
						maxrange += addsiegerange;
						deltaz = GetDistanceBetweenUnits256(a, a2) / 256;
						wmask = UnitWeaponMask(a2->SC_Unit);
						for (j = 0;wmask;j++, wmask >>= 1)
						{
							if (wmask & 1)
							{
								if (deltaz < mindeltaz[j] && (maxrange > deltaz) && (minrange <= deltaz))
								{
									if (weaponmask & 4)			//atackfacedirectiononly
									{
										neededside = CalcDirection(GetOBJx256(a), GetOBJy256(a), GetOBJx256(a2), GetOBJy256(a2));
										if (abs(a->mainimage->side - neededside) / 8 > 1)
											continue;
									}
									mindeltaz[j] = deltaz;
									findobj[j] = a2;
								}
							}
						}
					}//if unit can atack
				}//if have weapon
			}//if player aliance
		}//if not detect
	}
	if (IsGroundUnit(a->SC_Unit))
	{
		//atack ground unit if have
		if (findobj[GROUNDATACKER])				//if have ground atacker, atack him
			return(findobj[GROUNDATACKER]);
		if (findobj[AIRATACKER])		//if have air atacker, atack him
			return(findobj[AIRATACKER]);
	}
	else
	{
		//atack air unit if have
		if (findobj[AIRATACKER])		//if have air atacker, atack him
			return(findobj[AIRATACKER]);
		if (findobj[GROUNDATACKER])				//if have ground atacker, atack him
			return(findobj[GROUNDATACKER]);
	}
	return(findobj[NOATACKER]);			//return noatacker unit, if have
}
//=================================
void unitprepareforatack(OBJ *a, OBJ *a2)
{
	int aiaction = 0, doatack = 0, status;
	unsigned char groundweapon_id, NEWSC_Unit;
	if (UnitDoAiAction(a->playernr) || a->SC_Unit == SC_VULTUREMINEOBJ)
		aiaction = 1;
	if (a2)//have unit to atack
	{
		switch (a->SC_Unit)
		{
		case SC_LURKEROBJ:
			if (IsOBJBurrowed(a))
			{
				doatack = 1;
			}
			else
			{
				if (aiaction)
				{
					moveobj(a, NULL, MODEBURROW2, NOSHOWERROR);
				}
			}
			break;
		case SC_TANKNORMALTURRETOBJ:
		case SC_HERO_EDMUNDDUKETMTURRETOBJ:
			if (aiaction)
			{
				if (MageDepend(a->subunit, a->playernr, MODETANKSIEGE))
				{
					if (accesstomove(a->subunit, loadobj(a->SC_Unit), MODETANKSIEGE, a->playernr))//if can go siegemode
					{
						if (a->SC_Unit == SC_TANKNORMALTURRETOBJ)
						{
							NEWSC_Unit = SC_TANKSIEGETURRETOBJ;
						}
						else
						{
							if (a->SC_Unit == SC_HERO_EDMUNDDUKETMTURRETOBJ)
								NEWSC_Unit = SC_HERO_EDMUNDDUKESMTURRETOBJ;
							else
								break;
						}
						groundweapon_id = GetNewWeaponType(a, a2, NEWSC_Unit);
						if (groundweapon_id < MAX_WEAPONS_ELEM)
						{
							status = CheckWeaponRange(a, a2, groundweapon_id, a->playernr);
							if (status == 0)//siege weapon in range, we can go siege
								moveobj(a->subunit, NULL, MODETANKSIEGE, NOSHOWERROR);
							else
								//to close obj, do normal atack
								doatack = 1;
						}
						break;
					}
				}
			}
			doatack = 1;
			break;
		default:
			if (!IsOBJBurrowed(a))
			{
				doatack = 1;
			}
			else
			{
				if (aiaction)
				{
					moveobj(a, NULL, MODEUNBURROW, NOSHOWERROR);
					AddModeMove(a, a2, MODEATACK, 0, 0, NOSHOWERROR);
				}
			}
			break;
		}
		if (doatack)
		{
			moveobj(a, a2, MODEATACK, NOSHOWERROR | ATACKMOVEBIT);
		}
	}
	else//no body to atack
	{
		if (aiaction)
		{
			switch (a->SC_Unit)
			{
			case SC_VULTUREMINEOBJ:			//mine burrow of not have atack goal
				if (!IsOBJBurrowed(a))
					moveobj(a, NULL, MODEBURROW2, NOSHOWERROR);
				break;
			default:
				/*					if (!IsOBJBurrowed(a))
									{
										b=loadobj(a->SC_Unit);
										if (accesstomove(a,b,MODEBURROW,a->playernr))
										{
											moveobj(a,NULL,MODEBURROW,NOSHOWERROR);
										}
									}
				*/
				break;
			}
		}
	}
}
//=================================
void trapprepareforatack(OBJ *a, OBJ *a2)
{
	if (a2)
	{
		if (GetDoodadState(a) == DOODAD_BOTTOM_STATE)
		{
			//need show trap & atack
			DoodadChangeState(a, DOODAD_TOP_STATE);
		}
		else
		{
			DelAllModeMoves(a, 0);
			//			if (a->movelist)
			//				a->movelist->EmptyElemFifo();
			moveobj(a, a2, MODEATACK, NOSHOWERROR);
		}
	}
	else
	{
		a->finalOBJ = NULL;
		if (a->prop & VARAUTOMATICDOODAD)
		{
			if (GetDoodadState(a) == DOODAD_TOP_STATE)
			{
				//need hide trap
				DoodadChangeState(a, DOODAD_BOTTOM_STATE);
			}
		}
	}
}
//#define DONOTATACKBACKTEST
//=================================
void atackback(OBJ *firstatacker, OBJ *destobj, int directiondamage)
{
#ifdef DONOTATACKBACKTEST
	return;
#endif
	int err, aiaction = 0, notdetect, aiactionfailed = 1, actionfailed = 1;
	if (!firstatacker || !destobj)
		return;
	if (player_aliance(destobj->playernr, firstatacker->playernr) != ENEMYOBJ)
		return;
	if (IsSubUnit(firstatacker->SC_Unit))
		firstatacker = firstatacker->subunit;	//take base as atacker
	if (UnitDoAiAction(destobj->playernr))
		aiaction = 1;
	if (aiaction)
	{
		aiactionfailed = tryaiaction(destobj, firstatacker, directiondamage);
	}
	if (aiactionfailed && (destobj->modemove == MODESTOP || destobj->prop & VARHOLDPOSBIT))
	{
		notdetect = OBJ_VAR_CHK(firstatacker, obj_notdetect, destobj->playernr);
		if (!aiaction)
		{
			actionfailed = tryunitaction(destobj, firstatacker);
		}
		if (actionfailed)
		{
			if (aiaction && notdetect)
			{
				TryToScanArea(destobj, firstatacker);
			}
			if (!notdetect)
				err = IfCanCreateWeapon(destobj, firstatacker, NULL, NULL,
										CREATEWEAPON_IGNOREVISION | CREATEWEAPON_ISINTRANSPORT);
			if (notdetect || err == CREATEDWEAPONSTATUS_CANTATACKTHISUNIT)
			{
				//cannot atackback -> moveaway
				if (!(destobj->prop & VARHOLDPOSBIT))
					if (accesstomove(destobj, loadobj(destobj->SC_Unit), MODEMOVE, destobj->playernr))
						if (destobj->modemove == MODESTOP)
							moveaway(firstatacker, destobj, directiondamage, MODEMOVE, 0);
			}
			else
			{
				//try to atackback
				if (firstatacker->prop & VARINTRANSPORT)
				{
					if (destobj->SC_Unit == SC_VULTUREMINEOBJ &&
						firstatacker->in_transport &&
						IsBuild(firstatacker->in_transport->SC_Unit))
					{
					}
					else
						moveobj(destobj, firstatacker->in_transport, MODEATACK, NOSHOWERROR | ATACKMOVEBIT); //atack bunker back
				}
				else
				{
					if (IsOBJBurrowed(destobj))
					{
						moveobj(destobj, firstatacker, MODEUNBURROW, NOSHOWERROR | ATACKMOVEBIT);
						AddModeMove(destobj, firstatacker, MODEATACK, 0, 0, NOSHOWERROR);
					}
					else
					{
						moveobj(destobj, firstatacker, MODEATACK, NOSHOWERROR | ATACKMOVEBIT);
					}
				}
			}
		}
	}
	TellOtherUnitsAboutAtacking(destobj, firstatacker);
}
//=================================
void moveaway(OBJ *atacker, OBJ *a, int directiondamage, int modeofmove, int addtoqueue)
{
	int dx, dy;
	unsigned char side = CalcDirection(GetOBJx256(atacker), GetOBJy256(atacker), GetOBJx256(a), GetOBJy256(a));
	//	  unsigned char side = directiondamage + 128;
	dx = (inertion256[side][0] * SIZESPRLANSHX * 5) >> 16;
	dy = (inertion256[side][1] * SIZESPRLANSHY * 5) >> 16;
	if (addtoqueue)
		AddModeMove(a, NULL, modeofmove, GetOBJx(a) + dx, GetOBJy(a) + dy, NOSHOWERROR);
	else
		moveobj(a, NULL, modeofmove, GetOBJx(a) + dx, GetOBJy(a) + dy, NOSHOWERROR);

}
//=================================
int tryaiaction(OBJ *a, OBJ *atacker, int directiondamage)
{
	int err;
	switch (a->SC_Unit)
	{
	case SC_TANKSIEGEOBJ:
	case SC_HERO_EDMUNDDUKESMOBJ:
		//check for distance to unit to check if we need to go normal tank mode
		err = IfCanCreateWeapon(a, atacker, NULL, NULL, 0);
		if (err == CREATEDWEAPONSTATUS_DESTTOCLOSE)
			moveobj(a, NULL, MODETANKNORMAL, NOSHOWERROR);
		return(0);
	case SC_LURKEROBJ:
		err = IfCanCreateWeapon(a, atacker, NULL, NULL, 0);
		switch (err)
		{
			//cannot atack ignore this atacker
		case CREATEDWEAPONSTATUS_CANTATACKTHISUNIT:
		case CREATEDWEAPONSTATUS_UNDERDISRUPTION:
			if (IsOBJBurrowed(a))
			{
				//if cannot atack -> unburrow & get out of here
				if (!(a->prop & VARHOLDPOSBIT))
				{
					moveobj(a, NULL, MODEUNBURROW, NOSHOWERROR);
					moveaway(atacker, a, directiondamage, MODEMOVE, 1);//add this command to queue
				}
			}
			return(0);
		case CREATEDWEAPONSTATUS_ATACKSUCCESS:
		case CREATEDWEAPONSTATUS_ATACKSUCCESSWITHROTATION:
			//we in position, do not unburrow
			if (!IsOBJBurrowed(a))
				moveobj(a, NULL, MODEBURROW2, NOSHOWERROR);
			return(1);
		case CREATEDWEAPONSTATUS_DESTOUTOFRANGE:
		case CREATEDWEAPONSTATUS_DESTTOCLOSE://never
			if (IsOBJBurrowed(a))
			{
				moveobj(a, NULL, MODEUNBURROW, NOSHOWERROR);
			}
			else
			{
				//						moveobj(a,atacker,MODEATACK,NOSHOWERROR);
			}
			return(1);
		}
		return(0);//never
	}
	if (IsCloakable(a->SC_Unit))
	{
		if (accesstomove(a, loadobj(a->SC_Unit), MODECLOAKFIELD, a->playernr))
		{
			//cloakunit
			if (!GetMageAtr(&a->atrobj, ATRINVISIBLE))
			{
				moveobj(a, NULL, MODECLOAKFIELD, NOSHOWERROR);
			}
			return(0);
		}
		else
			if (accesstomove(a, loadobj(a->SC_Unit), MODEPERSONNELCLOAK, a->playernr))
			{
				//cloakunit
				if (!GetMageAtr(&a->atrobj, ATRINVISIBLE))
				{
					moveobj(a, NULL, MODEPERSONNELCLOAK, NOSHOWERROR);
				}
				return(0);
			}
	}
	return(1);//failed ai
}
//=================================
int tryunitaction(OBJ *a, OBJ *atacker)
{
	switch (a->SC_Unit)
	{
	case SC_INTERCEPTOROBJ:
		return(1);
	case SC_VULTUREMINEOBJ:
		return(1);
	case SC_LURKEROBJ:
		return(0);
		/*			if (IsOBJBurrowed(a))
					{
		//				moveobj(a,NULL,MODEUNBURROW,NOSHOWERROR);
					}
					else
					{
						moveobj(a,NULL,MODEBURROW2,NOSHOWERROR);
					}
		*/
	}
	if (IsOBJBurrowed(a))
	{
		moveobj(a, NULL, MODEUNBURROW, NOSHOWERROR);
		AddModeMove(a, atacker, MODEATACK, 0, 0, NOSHOWERROR);
		return(0);
	}
	return(1);
}
//=================================
void TryToScanArea(OBJ *a, OBJ *scanobjarea)
{
	int needmana;
	OBJ *a2;
	if (!a || !scanobjarea)
		return;
	//if not build we will not scan the region
	if (!IsBuild(a->SC_Unit))
	{
		needmana = GetTechEnergyCost(mageprop[MODESCANREGION].depend.obj_id[0]);
		a2 = SearchUnit(a->playernr, SC_COMSATSTATIONOBJ, -1, -1, needmana << 8);
		if (a2)
		{
			//scan the region
			moveobj(a2, NULL, MODESCANREGION, GetOBJx(scanobjarea), GetOBJy(scanobjarea), NOSHOWERROR);
			return;
		}
	}
	//TODO need to search nearest unit!!! or scan area
	a2 = SearchUnit(a->playernr, SC_COMSATSTATIONOBJ, -1, -1, 50 << 8);
	if (!a2)
	{
		a2 = SearchUnit(a->playernr, SC_OBSERVEROBJ, -1, -1, -1);
		if (!a2)
		{
			a2 = SearchUnit(a->playernr, SC_SCIENCEVESSELOBJ, -1, -1, -1);
			if (!a2)
			{
				a2 = SearchUnit(a->playernr, SC_OVERLORDOBJ, -1, -1, -1);
			}
		}
		if (a2)			//send observer obj to the area
			moveobj(a2, NULL, MODEMOVE, GetOBJx(scanobjarea), GetOBJy(scanobjarea), NOSHOWERROR);
	}
	else				//scan area
		moveobj(a2, NULL, MODESCANREGION, GetOBJx(scanobjarea), GetOBJy(scanobjarea), NOSHOWERROR);

}
//=================================
void TellOtherUnitsAboutAtacking(OBJ *a, OBJ *atacker)
{
	unsigned char weaponid;
	int i, allstat,canatack;
	OBJ *a2;
	for (i = 0;i < MaxObjects;i++)
	{
		a2 = objects[i];
		allstat = player_aliance(a->playernr, a2->playernr);
		if (allstat == MYOBJ || allstat == ALLIANCEOBJ)
		{
			if (a2->modemove == MODESTOP && a2 != a)
			{
				if (IsAtackerActiveUnit(a2))
				{
					if (controldistanceunit(GetOBJx(a), GetOBJy(a), GetOBJx(a2), GetOBJy(a2), MINREACTIONDISTANCEFORATACKING))
					{
						//tell other unit to atack my atacker
						if (IsOBJBurrowed(a2))
						{
							if (a2->SC_Unit != SC_LURKEROBJ)
							{
								if (a2->SC_Unit == SC_VULTUREMINEOBJ)
									continue;
								moveobj(a2, NULL, MODEUNBURROW, NOSHOWERROR);
							}
						}
						else
						{
							if (a2->SC_Unit != SC_LURKEROBJ)
								moveobj(a2, atacker, MODEATACK, NOSHOWERROR);
						}
					}
				}
			}
		}
	}
}
//=================================
void ShowCircleAndBar(OBJ *a)
{
	OBJstruct *b;
	int showcircle = 0, fromy = 0, maxshield;
	int manacnt, timeleft;
	int x, y;
	unsigned short flingy_id, sprites_id, sizexhealthbar;
	//	  if (a->modemove == MODEDIE)
	//		return;
	if (a->blinkvalue)
	{
		if (a->blinkvalue & BLINKSHOWHIDETICKS)
			showcircle = 1;
	}
	if (ifselectedOBJbit(a, NUMBGAMER))
		showcircle = 1;

	if (showcircle)
	{
		b = loadobj(a->SC_Unit);
		x = GetOBJx(a);
		y = GetOBJy(a);
		fromy = drawcircle(x - map.MAPXGLOBAL,
			y - map.MAPYGLOBAL,
			player_aliance(NUMBGAMER, a->playernr),
			a);

		if (!IsInvincibleOBJ(a))
		{
			flingy_id = alldattbl.units_dat->flingy_id[a->SC_Unit];
			sprites_id = alldattbl.flingy_dat->sprites_id[flingy_id];
			sizexhealthbar = alldattbl.sprites_dat->health_bar[sprites_id - 130];
			if (IsSpellCaster(a->SC_Unit))
				manacnt = GetUnitMaxMana(a->SC_Unit, a->playernr);
			else
				manacnt = 0;
			if (sizexhealthbar)
			{
				if (IsShieldEnable(a->SC_Unit))
					maxshield = GetUnitMaxShield(a->SC_Unit);
				else
					maxshield = 0;
				if (a->playernr == NUMBGAMER)
				{
					if (IsHallucination(a))
						timeleft = mageprop[MODEHALLUCINATION].timemageactive;
					else
						timeleft = b->maxtimeleft;
				}
				else
					timeleft = 0;
				desenlife(x - map.MAPXGLOBAL - sizexhealthbar / 2, fromy, sizexhealthbar,
					player_aliance(NUMBGAMER, a->playernr), a->health, a->shield, a->mana,
					a->timeleft, GetUnitMaxHealth(a->SC_Unit), maxshield, manacnt, timeleft);
			}
		}
	}
}
//=================================
int ifselectedOBJbit(OBJ *a, int playernr)
{
	return(a->selectedbits & (1 << playernr));
}
//=================================
void doselectedOBJbit(OBJ *a, int playernr, int flag)
{
	if (flag)
		a->selectedbits |= (1 << playernr);
	else
		a->selectedbits &= ~(1 << playernr);
}
//=================================
void SetHallucinationOBJ(OBJ *a)
{
	a->timeleft = mageprop[MODEHALLUCINATION].timemageactive;
	for (int i = ATRPARASITEFROM;i < MAXMAGEATR;i++)
		SetMageAtr(&a->atrobj, i, 0);
	SetMageAtr(&a->atrobj, ATRHALLUCINATION, ATRMAGEINFINITE);
	if (a->playernr == NUMBGAMER)
	{
		a->mainimage->imageusercolor = TOBLUE;
	}
	//hallucination dont need food, so decrease supply
	ChangeSupply(a->playernr, a->SC_Unit, MINUSFACTOR);
	ChangeMaxSupply(a->playernr, a->SC_Unit, MINUSFACTOR);

}
//=================================
void initmoveaction(OBJ *a, OBJ *destobj, int mode, int startx, int starty, int x, int y)
{
	int pathreterror;
	unsigned char flingy_id;
	flingy_id = alldattbl.units_dat->flingy_id[a->SC_Unit];
	if (mageprop[mode].createweapon != 255)
	{
		a->modemoveminusdistance = GetRangeWeaponInPixels(a, mageprop[mode].createweapon, a->playernr) << 8;
	}
	else
	{
		switch (mode)
		{
		case MODEATACK:
			if (destobj)
			{
				if (a->SC_Unit == SC_REAVEROBJ || a->SC_Unit == SC_HERO_WARBRINGEROBJ)
				{
					a->modemoveminusdistance = 256 << 8;
				}
				else
					a->modemoveminusdistance = GetRangeWeaponInPixels(a, startx, a->playernr) << 8;		//move minus range of weapos
			}
			else
				a->modemoveminusdistance = 0;												//move in atackmode to dest on ground
			break;
		case MODENUKELAUNCH:
			a->modemoveminusdistance = GetRangeWeaponInPixels(a, alldattbl.units_dat->GroundWeapon[a->SC_Unit], a->playernr) << 8;
			break;
		case MODEPATROL:
			if (destobj)
			{
				//do only patrol to terain not unit
				x = GetOBJx(destobj);
				y = GetOBJy(destobj);
				destobj = NULL;
			}
			if (startx == 0)
			{
				a->startx = GetOBJx(a);
				a->starty = GetOBJy(a);
			}
			else
			{
				a->startx = startx;			//from patrol command
				a->starty = starty;
			}
			a->prop |= VARPATROLFLAG;
			break;
		default:
			a->modemoveminusdistance = mageprop[mode].mindistance;
			break;

		}
	}
	switch (alldattbl.flingy_dat->MoveControl[flingy_id])
	{
	case FLINGYMOVECONTROL_ISCRIPT:
		a->finalx = x << 8;
		a->finaly = y << 8;
		//			a->prop |= VARMOVEACT;
		if (destobj)
		{
			a->finalOBJ = destobj;
			a->prop |= VARMOVEOBJACT;
		}
		else
		{
			a->finalOBJ = NULL;
			a->prop &= ~VARMOVEOBJACT;
		}
		PathFinding_InitType1(a, GetOBJx256(a), GetOBJy256(a), a->finalx, a->finaly);
		SetOBJIScriptNr(a, ISCRIPTNR_WALKING, ISCRIPTNR_SETONLY);
		break;
	case FLINGYMOVECONTROL_FLINGY:
	case FLINGYMOVECONTROL_WEAPON:
		a->prop |= VARACCELERATIONBIT;
		//			a->prop |= VARMOVEACT;
		if (destobj)
		{
			a->finalOBJ = destobj;
			a->prop |= VARMOVEOBJACT;
			a->finalx = GetOBJx256(destobj);
			a->finaly = GetOBJy256(destobj);
		}
		else
		{
			a->finalOBJ = NULL;
			a->prop &= ~VARMOVEOBJACT;
			a->finalx = x << 8;
			a->finaly = y << 8;
		}
		pathreterror = PathFinding_InitType2(a, GetOBJx256(a), GetOBJy256(a), a->finalx, a->finaly, flingy_id);
		//			if (pathreterror)
		//				SetModeMove(a,MODESTOP);
		//			else
		SetOBJIScriptNr(a, ISCRIPTNR_WALKING, ISCRIPTNR_EXECUTE);
		break;
	}
}
//=================================
void InitStopAfterMove(OBJ *a)
{
	//engines off
//	  a->modemove = MODESTOP;
//	  a->prop &= ~(VARACCELERATIONBIT | VARMOVEACT);
	a->prop &= ~VARACCELERATIONBIT;
	SetOBJIScriptNr(a, ISCRIPTNR_WALKINGTOIDLE, ISCRIPTNR_EXECUTE);
}
//=======================================
//for iscriptbin version
void PathFinding_InitType1(OBJ *a, int initx, int inity, int destx, int desty)
{
	a->mainimage->UnitNeededDirection256(CalcDirection(initx, inity, destx, desty));
}
//=================================
int PathFinding_MovePortionType1(OBJ *a, MAIN_IMG *img, int deltamove)
{
	int deltaz, deltax, deltay, destx, desty, widthsumm;
	if (a->prop & VARMOVEOBJACT)
	{
		// if move to a object
		if (!a->finalOBJ)		//destobj did not exist(died?)
		{
			a->prop &= ~VARMOVEOBJACT;
			if (a->prop & VARIFDIEDESTTOTERRAIN)
				goto movelikeatoterrain1;
			InitStopAfterMove(a);
			//apply next mode from list
			if (!(a->prop & VARHOLDPOSBIT))
			{
				if (ApplyNextModeMove(a))
					return(2);
			}
			return(2);			//change iscriptnr
		}
		destx = GetOBJx256(a->finalOBJ);
		desty = GetOBJy256(a->finalOBJ);
		img->UnitNeededDirection256(CalcDirection(img->xpos, img->ypos, destx, desty));

		deltax = destx - img->xpos;
		deltay = desty - img->ypos;
		deltaz = (int)hypot(deltax, deltay);
		widthsumm = GetWidthSummOfUnits(a->SC_Unit, a->finalOBJ->SC_Unit, deltax, deltay);
		if (deltaz - widthsumm <= a->modemoveminusdistance)
		{
			//			printf("deltaz=%d modemoveminusdistance=%d deltamove=%d mindist=%d\n",deltaz/256,a->modemoveminusdistance/256,deltamove/256,(deltaz - a->modemoveminusdistance)/256);
			if (a->finalOBJ->SC_Unit == SC_NYDUSCANALOBJ && a->finalOBJ->playernr == a->playernr)
			{
				if (TryToEnterNydus(a, a->finalOBJ))
					return(2);
			}
			if (ApplyNextModeMove(a))
				return(2);
			//here unit stopped and wait for change distance to move again
			a->prop |= VARWAITDISTANCE;
			if (a->modemove == MODEATACK)
				a->prop |= VARNEXTMODEMOVEIGNOREINERTION;
			SetOBJIScriptNr(a, ISCRIPTNR_WALKINGTOIDLE, ISCRIPTNR_EXECUTE);
			return(2);
		}
	}
	else
	{
	movelikeatoterrain1:
		//if move to a terrain
		deltax = a->finalx - img->xpos;
		deltay = a->finaly - img->ypos;
		deltaz = (int)hypot(deltax, deltay);

		if (deltaz - a->modemoveminusdistance < deltamove)
		{
			//we need to stop action
			if (a->prop & VARPATROLFLAG)
			{
				initmoveaction(a, NULL, MODEPATROL, a->finalx >> 8, a->finaly >> 8, a->startx, a->starty);
				return(0);		//not move but initiate other move
			}
			else
			{
				InitStopAfterMove(a);
				//apply next mode from list
				if (!a->finalOBJ)
					if (!(a->prop & VARHOLDPOSBIT))
					{
						if (ApplyNextModeMove(a))
							return(2);
					}
				return(2);				//change iscriptnr
			}
		}
	}
	deltax = (long long)deltamove*deltax / deltaz;
	deltay = (long long)deltamove*deltay / deltaz;
	img->SetDeltaCoords(a, deltax, deltay);
	return(1);//can pass
}
//=================================
//for flingy movecontrol version
int PathFinding_InitType2(OBJ *a, int initx, int inity, int destx, int desty, unsigned char flingy_id)
{
	int retcalc = 0;
	PathFinding_InitType1(a, initx, inity, destx, desty);
	if (a->mainimage->side == a->mainimage->neededside)
	{
		//if the same direction, calculate move vars
		retcalc = CalcDestVars(a, a->finalOBJ, initx, inity, destx, desty, flingy_id);
		if (retcalc)
		{
			a->currentspeed = 1;	//need to stop if dest is near
//			a->prop &= ~(VARACCELERATIONBIT | VARMOVEOBJACT | VARMOVEACT);
			a->prop &= ~(VARACCELERATIONBIT | VARMOVEOBJACT);
			a->finalOBJ = NULL;
		}
	}
	return(retcalc);
}
//=================================
int PathFinding_MovePortionType2(OBJ *a, MAIN_IMG *img, unsigned char flingy_id, int deltamove)
{
	int deltaz, deltax, deltay, destx, desty, widthsumm;
	if (a->prop & VARMOVEOBJACT)
	{
		if (!a->finalOBJ)		//destobj did not exist(died?)
		{
			a->prop &= ~VARMOVEOBJACT;
			if (a->prop & VARIFDIEDESTTOTERRAIN)
				goto movelikeatoterrain2;
			InitStopAfterMove(a);
			return(2);			//change iscriptnr
		}
		destx = GetOBJx256(a->finalOBJ);
		desty = GetOBJy256(a->finalOBJ);
		if (a->prop &  VARACCELERATIONBIT)
		{
			img->UnitNeededDirection256(CalcDirection(img->xpos, img->ypos, destx, desty));
			a->finalx = destx;
			a->finaly = desty;
		}
		else
		{
			img->MoveInUnitDirection(a, img->side, deltamove);
			return(1);
		}
		deltax = destx - img->xpos;
		deltay = desty - img->ypos;
		deltaz = (int)hypot(deltax, deltay);
		widthsumm = GetWidthSummOfUnits(a->SC_Unit, a->finalOBJ->SC_Unit, deltax, deltay);
		if (deltaz - widthsumm - a->modemoveminusdistance <= 2 * 256)
		{
			//			printf("deltaz=%d modemoveminusdistance=%d haltdist=%d deltaz-modemovedist=%d\n",
			//				deltaz/256,a->modemoveminusdistance/256,a->haltdistance/256,(deltaz-a->modemoveminusdistance)/256);
			InitStopAfterMove(a);
			a->prop |= VARWAITDISTANCE;
			if (a->modemove == MODEATACK)
				a->prop |= VARNEXTMODEMOVEIGNOREINERTION;
		}
		if (deltaz <= 2 * 256)			//prevent div 0
			return(1);
	}
	else
	{
	movelikeatoterrain2:
		//if move to a terrain
		deltax = a->finalx - img->xpos;
		deltay = a->finaly - img->ypos;
		deltaz = (int)hypot(deltax, deltay);
		if (a->prop & VARACCELERATIONBIT)
		{
			//				printf("deltaz=%d modemoveminusdistance=%d haltdist=%d deltaz-modemovedist=%d\n",
			//					deltaz/256,a->modemoveminusdistance/256,a->haltdistance/256,(deltaz-a->modemoveminusdistance)/256);
			if (deltaz - a->modemoveminusdistance <= a->haltdistance)
			{
				InitStopAfterMove(a);
				//				if (a->finalOBJ)
				//					ApplyNextModeMove(a);		//if have goal do not wait completly stop
			}
		}
		else
		{
			//do inertion move
			img->MoveInUnitDirection(a, img->side, deltamove);//NOTE: used a->currentspeed (no used 'deltamove' for correct inertion)
//			img->MoveInUnitDirection(a,img->side,a->currentspeed);//NOTE: used a->currentspeed (no used 'deltamove' for correct inertion)
			return(1);
		}
		if (deltaz < 256)				//prevent div 0
			return(1);
	}
	deltax = (long long)deltamove*deltax / deltaz;
	deltay = (long long)deltamove*deltay / deltaz;
	img->SetDeltaCoords(a, deltax, deltay);
	return(1);//can pass
}
//=================================
int CalcDestVars(OBJ *a, OBJ *destobj, int initx, int inity, int destx, int desty, unsigned char flingy_id)
{
	int haltdistance, topspeed, deltaz;
	topspeed = alldattbl.flingy_dat->TopSpeed[flingy_id];
	a->deacceleration = alldattbl.flingy_dat->Acceleration[flingy_id];
	if (alldattbl.flingy_dat->MoveControl[flingy_id] == FLINGYMOVECONTROL_WEAPON)
	{
		haltdistance = topspeed;
	}
	else
	{
		haltdistance = sqr(topspeed) / (2 * alldattbl.flingy_dat->Acceleration[flingy_id]);
		if (destobj)
			deltaz = GetDistanceTo256(destobj, initx, inity) / 2;
		else
			deltaz = hypot(destx - initx, desty - inity) / 2;
		if (deltaz <= haltdistance)
		{
			//			haltdistance = CalcTotalDistance(topspeed-a->currentspeed,0,alldattbl.flingy_dat->Acceleration[flingy_id]);
			//			if (deltaz >= haltdistance)
			//			{
			haltdistance = deltaz / 2;
			a->deacceleration *= 2;
			//			}
		}
	}
	a->topspeed = topspeed;
	a->haltdistance = haltdistance;
	return (haltdistance <= 256);
}
//=================================
void ForceKartChanges(OBJ *a)
{
	a->xkart = (GetOBJx256(a) >> 8) / 32;
	a->ykart = (GetOBJy256(a) >> 8) / 32;
	a->prop |= VARKARTCHANGES;
	revealMap->AddObj(a);
}
//=================================
void ForceKartChanges(OBJ *a, int xpos, int ypos)
{
	a->xkart = (xpos >> 8) / 32;
	a->ykart = (ypos >> 8) / 32;
	a->prop |= VARKARTCHANGES;
	revealMap->AddObj(a);
}
//=================================
void CheckForKartChanges(OBJ *a, int xpos, int ypos)
{
	unsigned char newxkart, newykart;
	newxkart = (xpos >> 8) / 32;
	newykart = (ypos >> 8) / 32;
	if (a->xkart != newxkart || a->ykart != newykart)
	{
		a->xkart = newxkart;
		a->ykart = newykart;
		a->prop |= VARKARTCHANGES;
		revealMap->AddObj(a);
		//		DEBUGMESSCR("kart changes %dx%d\n",a->xkart,a->ykart);
	}
}
//=================================
void CheckForWalkChanges(OBJ *a, int xpos, int ypos)
{

}
//=================================
//deltax,deltay - delta between movedunit & destination unit
void AdditionalUnitsProceed(void)
{
	int i;
	for (i = 0;i < MaxObjects;i++)
	{
		if (objects[i]->modemove != MODEDIE)
			AdditionalUnitProceed(objects[i], objects[i]->mainimage);
	}
}
//=================================
void AllOBJMoving(void)
{
	int i, rot;
	signed char nextmoveignorefullinertion;

	unsigned char flingy_id, side1;
	MAIN_IMG *img;
	OBJ *a;
	for (i = 0;i < MaxObjects;i++)
	{
		nextmoveignorefullinertion = 0;
		a = objects[i];
		img = a->mainimage;
		flingy_id = alldattbl.units_dat->flingy_id[a->SC_Unit];
		switch (alldattbl.flingy_dat->MoveControl[flingy_id])
		{
		case FLINGYMOVECONTROL_ISCRIPT:
			if (img->movefactor)
			{
				PathFinding_MovePortionType1(a, img, GetSpeed(a, img->movefactor));
				if (a->subunit)
					ChangeSubUnitCoords(a->subunit, a);
				img->movefactor = 0;
			}
			break;
		case FLINGYMOVECONTROL_FLINGY:
		case FLINGYMOVECONTROL_WEAPON:
			if (img->flags & SC_IMAGE_FLAG_NEEDROTATIONTODIRECTION)
			{
				side1 = img->side;
				rot = img->Rotation(img->TurnRadius);
				if (!rot)
				{
					//						img->MoveInUnitDirection(a,side1,GetSpeed(a,a->topspeed));
					img->MoveInUnitDirection(a, side1, GetSpeed(a, a->currentspeed));
					img->UnitNeededDirection256(CalcDirection(GetOBJx256(a), GetOBJy256(a), a->finalx, a->finaly));
					continue;
				}
				if (a->modemove == MODETURN180)
				{
					ApplyNextModeMove(a);
					continue;
				}
				if (CalcDestVars(a, a->finalOBJ, GetOBJx256(a), GetOBJy256(a), a->finalx, a->finaly, flingy_id))
				{
					//need to stop engines
					InitStopAfterMove(a);
				}
			}
			if (a->prop &  VARACCELERATIONBIT)
			{
				//accelerate speed
				a->currentspeed += alldattbl.flingy_dat->Acceleration[flingy_id];
				if (a->currentspeed > a->topspeed)
				{
					//reach topspeed
					a->currentspeed = a->topspeed;
				}
			}
			else
			{
				if (a->prop & VARDONOTAPPLYNEXTMOVE)
					continue;
				if (!a->currentspeed)
				{
					if (a->movelist && a->movelist->GetUsedElem())
						ApplyNextModeMove(a);
					if (a->finalOBJ && IsPickupUnit(a->finalOBJ->SC_Unit) && IsWorkerUnit(a->SC_Unit) && !a->carryobj)
					{
						PickupObj(a, a->finalOBJ);
					}
					continue;
				}
				if (alldattbl.flingy_dat->MoveControl[flingy_id] == FLINGYMOVECONTROL_WEAPON)
				{
					a->currentspeed = a->currentspeed / 2;
				}
				else
				{
					a->currentspeed -= a->deacceleration;
				}
				if (a->currentspeed <= 0)
				{
					//stop move
					a->currentspeed = 0;
					if (a->finalOBJ && a->finalOBJ->SC_Unit == SC_NYDUSCANALOBJ)
						if (TryToEnterNydus(a, a->finalOBJ))
							continue;
					if (a->prop & VARPATROLFLAG)
					{
						initmoveaction(a, NULL, MODEPATROL, a->finalx >> 8, a->finaly >> 8, a->startx, a->starty);
					}
					else
					{
						//set next move from list
						if (!a->finalOBJ)
						{
							if (!(a->prop & VARHOLDPOSBIT))
							{
								ApplyNextModeMove(a);
							}
						}
						else
						{
							if (a->modemove != MODEATACK)
								ApplyNextModeMove(a);
						}
					}
					continue;
				}
				else
				{
					if (a->prop & VARNEXTMODEMOVEIGNOREINERTION)
					{
						nextmoveignorefullinertion = 1;
						//  						ApplyNextModeMove(a);
						//  						break;
					}
				}
			}
			PathFinding_MovePortionType2(a, img, flingy_id, GetSpeed(a, a->currentspeed));
			if (nextmoveignorefullinertion)
			{
				ApplyNextModeMove(a);
				break;
			}
			//
			//				if (a->subunit)
			//					ChangeSubUnitCoords(a->subunit,a);

			//				printf("speed=%d ensnaired=%d\n",a->currentspeed,curspeed);
			break;
		}//switch
	}
}
//=================================
int TryToEnterNydus(OBJ *a, OBJ *nydus)
{
	int x, y;
	if (nydus->doubleunit && GetUnitRace(a->SC_Unit) == ZERGRACE && IsGroundUnit(a->SC_Unit))
	{
		Play_sfxdata_id(nydus, SFXDATA_INTONYDUS, 4, 0);
		getcoordofnewunit(nydus->doubleunit, a->SC_Unit, &x, &y);
		ChangeObjXY(a, x, y);
		ForceKartChanges(a);
		moveobj(a, NULL, MODESTOP, x, y, NOSHOWERROR);
		a->finalOBJ = NULL;
		return(1);
	}
	return(0);
}
//=================================
int IfHaveDistanceForMove(OBJ *a, MAIN_IMG *img, OBJ *destobj, int mindistance)
{
	int deltax, deltay, deltaz;
	deltax = GetOBJx256(destobj) - img->xpos;
	deltay = GetOBJy256(destobj) - img->ypos;

	deltaz = GetDistanceMinusSizes(a->SC_Unit, destobj->SC_Unit, deltax, deltay);
	return (deltaz > mindistance);
}
//==================================
void SetModeMove(OBJ *a, int mode)
{
	a->modemove = mode;
	DelAllModeMoves(a, 0);
	if (mode == MODESTOP)
	{
		if (IsReadyOBJ(a))
			CALLBACK_OBJ_AfterStop(a);
	}
}
//==================================
void InsertModeMove(OBJ *a, OBJ *destobj, int mode, int x, int y, int showmesflag)
{
	ONEMODEMOVE *onemodemove;
	if (!a->movelist)
	{
		a->movelist = new MODEMOVES(5);
	}
	if (!a->movelist->GetFreeElem())
	{
		DEBUGMESSCR("list of mode moves is full\n");
		return;
	}
	onemodemove = a->movelist->Insert(sizeof(ONEMODEMOVE));
	if (!onemodemove)
	{
		DEBUGMESSCR("cannot insert to list of mode moves :( \n");
		return;
	}
	onemodemove->destobj = destobj;
	onemodemove->modemove = mode;
	onemodemove->destx = x;
	onemodemove->desty = y;
	onemodemove->showerrflag = showmesflag | NOSHOWERROR;
}
//==================================
void AddModeMove(OBJ *a, OBJ *destobj, int mode, int x, int y, int showmesflag)
{
	ONEMODEMOVE *onemodemove;
	if (!a->movelist)
	{
		a->movelist = new MODEMOVES(5);
	}
	if (!a->movelist->GetFreeElem())
	{
		DEBUGMESSCR("list of mode moves is full\n");
		return;
	}
	onemodemove = a->movelist->Add(sizeof(ONEMODEMOVE));
	if (!onemodemove)
	{
		DEBUGMESSCR("cannot add to list of mode moves :( \n");
		return;
	}
	onemodemove->destobj = destobj;
	onemodemove->modemove = mode;
	onemodemove->destx = x;
	onemodemove->desty = y;
	onemodemove->showerrflag = showmesflag | NOSHOWERROR;
}
//==================================
int IfHaveNextModeMove(OBJ *a)
{
	if (!a->movelist)
		return(0);
	return(a->movelist->GetUsedElem());
}
//==================================
int ApplyNextModeMove(OBJ *a)
{
	int dx, dy, modemove, showerrmes;
	OBJ *destobj;
	ONEMODEMOVE *onemodemove;
	do {
		//check if we have move in lists, if have do that move
		if (!a->movelist || !a->movelist->GetUsedElem())
		{
			if (!a->finalOBJ)
				return(moveobj(a, NULL, MODESTOP, NOSHOWERROR));
			return(0);
		}
		onemodemove = a->movelist->GetCurElem();
		modemove = onemodemove->modemove;
		dx = onemodemove->destx;
		dy = onemodemove->desty;
		destobj = onemodemove->destobj;
		showerrmes = onemodemove->showerrflag;
		a->movelist->DelCurElem();
	} while (modemove == MODENON);
	moveobj(a, destobj, modemove, dx, dy, showerrmes | ATACKMOVEBIT);
	return(1);
}
//==================================
void DelOBJFromModeList(struct OBJ *a, struct OBJ *searchobj)
{
	int i;
	ONEMODEMOVE *onemodemove;
	if (a->movelist)
	{
		if (a->movelist->GetUsedElem())
		{
			for (i = 0;i < a->movelist->Capacity();i++)
			{
				onemodemove = a->movelist->At(i);
				if (onemodemove && onemodemove->destobj == searchobj)
				{
					//emtpy modemove
					onemodemove->modemove = MODENON;
					onemodemove->destobj = NULL;
				}
			}
		}
	}
}
//==============================================
void DelAllModeMoves(struct OBJ *a, int dealloc)
{
	if (a->movelist)
	{
		a->movelist->Clear();
		if (dealloc)
		{
			delete a->movelist;
			a->movelist = NULL;
		}
	}
}
//==============================================
int LaunchScarab(OBJ *a, OBJ *destobj)
{
	OBJ *temp;
	signed char deltax, deltay;
	a->usedweapon_id = WEAPONID_SCARAB;
	if (destobj)
	{
		if (a->ammo)
		{
			a->ammo--;
			//			CalcXYOffsets(a->mainimage->parentimg->side,GetUnitDimensions(a->SC_Unit,UNITDIM_DOWN),0,&deltax,&deltay);
			CalcXYOffsets(a->mainimage->parentimg->side, 32, 0, &deltax, &deltay);
			temp = createobjman(GetOBJx(a) + deltax, GetOBJy(a) + deltay, SC_SCARABOBJ, a->playernr);
			temp->whoatack = a;
			ChangeTypeOfProp(temp, PROPNORMAL1);
			temp->mainimage->elevationlevel = a->mainimage->elevationlevel - 2;
			temp->prop |= VARREADY | VARIFDIEDESTTOTERRAIN | VARCANTSELECT;
			temp->finalOBJ = NULL;
			SetModeMove(temp, MODESTOP);
			moveobj(temp, destobj, MODEATACK, GetOBJx(destobj), GetOBJy(destobj), NOSHOWERROR);
			return(1);
		}
	}
	return(0);
}
//=================================
void LaunchInterceptors(OBJ *a, OBJ *destobj)
{
	WakeUpInterceptors(a, destobj);
}
//=================================
void CALLBACK_OBJ_AfterStop(OBJ *a)
{
	switch (a->SC_Unit)
	{
	case SC_INTERCEPTOROBJ:
		if (OneUnitSearchGoal(a, 1, 0))
		{
			a->searchforatack_tick = MAXWAIT_SEARCHATACK_TICK;
		}
		else
		{
			moveobj(a, a->myparent, MODEGOTORECHARGE, NOSHOWERROR);
		}
		break;
	}
}
//=================================
int CALLBACK_OBJ_AtackedOBJISNULL(OBJ *a)
{
	if (OneUnitSearchGoal(a, 1, 0))
	{
		a->searchforatack_tick = MAXWAIT_SEARCHATACK_TICK;
		return(1);
	}
	else
	{
		EndAtackAction(a);
		a->finalOBJ = NULL;
		SetModeMove(a, MODESTOP);
		return(0);
	}
}
//=================================
