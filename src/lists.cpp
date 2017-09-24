#include <stdio.h>
#include <math.h>

#include "auxil.h"
#include "man.h"
#include "maps.h"
#include "objinfo.h"
#include "objs.h"
#include "load.h"
#include "wmem.h"
#include "mage.h"
#include "vars.h"
#include "scripts.h"
#include "market.h"
#include "putobj.h"
#include "rand.h"
#include "images.h"
#include "lists.h"

//=========================================
void UnitsMAPRefresh(void)
{
    int i;
    OBJ *a;
    for (i=0;i<MaxObjects;i++)
    {
	a=objects[i];
	a->prop|=VARKARTCHANGES;
    }
}
//=====================================
void addobjlist(struct OBJ *a)
{
    objects[MaxObjects]=a;
    a->nrobject=MaxObjects;
    MaxObjects++;
}
//==========================================
void delobjlist(struct OBJ *a)
{
    int xx;
    MaxObjects--;
    xx = objcmp(objects,a,MaxObjects+1);
    objects[xx]=objects[MaxObjects];
    objects[xx]->nrobject=xx;
    objects[MaxObjects]=NULL;
}
//==========================================
void freeOBJS(void)
{
    for (int i=MaxObjects-1;i>=0;i--)
    {
        destroyobj(objects[i]);
    }
}
//==========================================
void PrepareForDeallocOBJstruct(mylist *pointerlist,OBJstruct *obj)
{
    int i,j;
    if (obj->portraits)
        pointerlist->AddUniqueElem(obj->portraits);
}
//==========================================
void freeOBJstructs(int withupgradeandtechrace)
{
    int i,j;
    void *tempelem;
    mylist pointerlists;
    for (int i=0;i<NUMBRSPROBJ;i++)
    {
        if (allobj[i])
        {
	    if (withupgradeandtechrace)
	    {
		//add pointers from OBJstruct
		PrepareForDeallocOBJstruct(&pointerlists,allobj[i]);
    		wfree(allobj[i]);
        	allobj[i]=NULL;
	    }
        }
    }
    pointerlists.DeallocList();
}
//==========================================
void freeMAGEstructs(void)
{
    void *tempelem;
    mylist pointerlists;
    int i;
    for (i=0;i<=MAXIMMODE;i++)
    {
	if (mageprop[i].appear)
	{
//	    if (mageprop[i].appear->grp)
//		pointerlists.AddUniqueElem(mageprop[i].appear->grp);
	    pointerlists.AddUniqueElem(mageprop[i].appear);
	    mageprop[i].appear=NULL;
	}
//	mageprop[i].namemage[0]=0;
    }
    for (i=0;i<MAXMAGEATR;i++)
	magenrfrommageatr[i]=0;
    pointerlists.DeallocList();
}
//==========================================
int isrechargebleobj(struct OBJ *a)
{
    if (IsShieldEnable(a->SC_Unit))
	if ((a->shield<GetUnitMaxShield(a->SC_Unit))&&!(a->prop&VARNOTHERE)&&!IsOBJUnderConstruct(a)&&
	    GetMageAtr(&a->atrobj,ATRSTASIS)==0)
	    if (!IsBuild(a->SC_Unit))
		return 1;
    return 0;
}
//==========================================
int ishealingobj(struct OBJ *a)
{
    if (IsOrganic(a->SC_Unit)&&!(a->prop&VARNOTHERE)&&!IsOBJUnderConstruct(a)&&
	    GetMageAtr(&a->atrobj,ATRSTASIS)==0 && a->health<GetUnitMaxHealth(a->SC_Unit))
	if (!IsOnSkyOBJ(a))
	    return 1;
    return 0;
}
//==========================================
int isobjtobeinfest(struct OBJ *a)
{
    if (a->SC_Unit==SC_COMMCENTEROBJ && (a->health < GetUnitMaxHealth(a->SC_Unit)/2) && !IsOBJUnderConstruct(a))
	    return 1;
    return 0;
}
//==========================================
int canatackobj(struct OBJ *a,struct OBJ *a2)
{
    return 0;
}
//==========================================
struct OBJ *SearchUnitFunc(int playernr,int (*funccheckunit)(int SC_Unit),int nearx,int neary,int dist)
{
    int i,allstat;
    OBJ *a;
    for (i=0;i<MaxObjects;i++)
    {
	a = objects[i];
	if (a->mainimage)
	    if (controldistanceunit(nearx,neary,GetOBJx(a),GetOBJy(a),dist))
	    {
		if ((*funccheckunit)(a->SC_Unit))
		{
		    if (IsActiveUnit(a) && !a->carryobj)
		    {
			return(a);
	    	    }
		}
	    }
    }
    return(NULL);
}
//==========================================
//==========================================
struct OBJ *SearchUnit(int playernr,int SC_Unit,int shieldp,int healthp,int energyp)
{
    int i,allstat;
    OBJ *a;
    for (i=0;i<MaxObjects;i++)
    {
	a = objects[i];
	if (a->SC_Unit==SC_Unit)
	{
	    if (IsActiveUnit(a))
	    {
		allstat = player_aliance(a->playernr,playernr);
		if (allstat == MYOBJ || allstat == ALLIANCEOBJ)
		{
		    if (a->shield > shieldp && a->health > healthp && a->mana > energyp)
			return(a);
		}
	    }
	}
    }
    return(NULL);
}
//==========================================
//need to add if object is critical to die need to heal him first
//==========================================
struct OBJ *SearchOBJforOBJ(struct OBJ *a,int modemage)
{
    struct OBJ *destobj;
    int temp,mindistance,i,findobj=0;
    mindistance = mageprop[modemage].diapazone;
    for (i=0;i<MaxObjects;i++)
    {
	    destobj = objects[i];
	    if (a == destobj || destobj->modemove == MODEDIE)
		continue;
	    switch(modemage)
	    {
		case MODEHEAL:
		    //need to healing obj and my troops
		    if (ishealingobj(destobj) && a->playernr == destobj->playernr)
			if (GetDistanceBetweenUnits256(a,destobj) < mindistance)
			    findobj=1;
		    break;
		case MODERECHARGESHIELD:
		    if (isrechargebleobj(destobj))
			if (GetDistanceBetweenUnits256(a,destobj) < mindistance)
			    findobj=1;
		    break;
		case MODEATACK:
		    if (canatackobj(a,destobj))
			if (GetDistanceBetweenUnits256(a,destobj) < mindistance)
			    findobj=1;
		    break;
		case MODEINFEST:
	    	    if (isobjtobeinfest(destobj))
			if (GetDistanceBetweenUnits256(a,destobj) < mindistance)
			    findobj=1;
		    break;
		case SEARCHMODE_RESCUABLE:
		    if (destobj->playernr==NUMBGAMER)
			if (GetDistanceBetweenUnits256(a,destobj) < mindistance)
			    findobj=1;
		case MODEREPAIR:
		    if (IsBuild(destobj->SC_Unit) && GetUnitRace(destobj->SC_Unit)==TERRANRACE && IsReadyOBJ(destobj))
		    {
			if (destobj->health != GetUnitMaxHealth(destobj->SC_Unit))
			    if (GetDistanceBetweenUnits256(a,destobj) < (48<<8))
			    {
				findobj=1;
			    }
		    }
		    break;
		default:
		    DEBUGMESSCR("searchobj: %d mode not developed\n",modemage);
		    break;
	    }
	    if (findobj)
		return(destobj);
    }
    return NULL;
}
//==========================================
OBJ *SearchObjs(int x,int y,int range,int *array,int arraydim)
{
    int i,j;
    OBJ *a;
    for (i=0;i<MaxObjects;i++)
    {
	a = objects[i];
	for (j=0;j<arraydim;j++)
	    if (a->SC_Unit==array[j])
	        if (controldistanceunit(x,y,GetOBJx(a),GetOBJy(a),range))
	        {
		    return(a);
		}
    }
    return(NULL);
}
//=================================
OBJ *SearchForObjInXY(int x,int y,unsigned char *SC_Units,int nrofunits)
{
    int i,j;
    OBJ *a;
    for (i=0;i<MaxObjects;i++)
    {
	a = objects[i];
	for (j=0;j<nrofunits;j++)
	    if (a->SC_Unit == SC_Units[j] && !IsOnSkyOBJ(a))
		if (GetOBJx(a) == x && GetOBJy(a) == y)
		    return(a);
    }
    return(NULL);
}
//=================================
int SearchForUnitMask(int playernr,int mask,int *x,int *y)
{
    OBJ *a;
    for (int i=0;i<MaxObjects;i++)
    {
	a = objects[i];
	if ( a->playernr == playernr && (alldattbl.units_dat->SpecialAbilityFlags[a->SC_Unit] & mask))
	{
	    *x = (a->mainimage->xpos>>8);
	    *y = (a->mainimage->ypos>>8);
	    return(TRUE);
	}
    }
    return(FALSE);
}
//=================================
int FindSC_UnitType(OBJ *a2,int player,int SC_Unit,int SC_AddonUnit)
{
    OBJ *a;
    //if check for addon, we need to check addon in selected build(OBJ *a2)
    if (IsAddon(SC_Unit))
    {
	if (a2->addonobj)
	    if (a2->addonobj->SC_Unit==SC_Unit)
		return(1);
    }
    for (int i=0;i<MaxObjects;i++)
    {
	a=objects[i];
	if (a->playernr==player)
	{
	    if (a->SC_Unit==SC_Unit)
	    {
		if (IsReadyOBJ(a))
		{
		    if (SC_AddonUnit==SC_NOUNITNR)
		    {
		        return(1);
		    }
		    else
		    {
	    	        if (a->addonobj && a->addonobj->SC_Unit==SC_AddonUnit && IsReadyOBJ(a->addonobj))
			    return(1);
		    }
		}
	    }
	    else
		if (SC_Unit==SC_HATCHERYOBJ)
		{
		    if ((a->SC_Unit==SC_LAIROBJ||a->SC_Unit==SC_HIVEOBJ))
			return(1);
		}
		else
		    if (SC_Unit==SC_LAIROBJ)
		    {
	    		if (a->SC_Unit==SC_HIVEOBJ)
			    return(1);
		    }
		    else
	    		if (SC_Unit==SC_SPIREOBJ)
			{
		    	    if (a->SC_Unit==SC_GREATERSPIREOBJ)
				return(1);
			}
	}
    }
    return(0);
}
//=================================
int controlifunitinunit(struct OBJ *a,struct OBJ *a2)
{
    int sx,sy;
    sx = GetUnitWidthAndHeight(a->SC_Unit,UNITDIM_WIDTH)/2;
    sy = GetUnitWidthAndHeight(a->SC_Unit,UNITDIM_HEIGHT)/2;

    if (sx>10)
       sx=10;
    if (sy>10)
       sy=10;
    if (GetOBJx(a)>=GetOBJx(a2)-sx &&
        GetOBJy(a)>=GetOBJy(a2)-sy &&
        GetOBJx(a)<=GetOBJx(a2)+sx &&
        GetOBJy(a)<=GetOBJy(a2)+sy)
        return(1);
    return(0);
}
//=================================
//esli do (dx,dy)  <= rasstoianie dlia actiona(magii) to return non 0 
int controldistancemode(int dx1,int dy1,int dx2,int dy2,int modemove)
{
    int distance=mageprop[modemove].mindistance;
    return(controldistanceunit(dx1,dy1,dx2,dy2,distance));
}
//=================================
int controldistanceunit(int dx1,int dy1,int dx2,int dy2,int range)
{
    int sqq=(int)hypot(dx2-dx1,dy2-dy1);
    if (sqq<range)
        return(sqq+1);
//        return(-1);
    else
        return(0);
}
//=================================
struct OBJ *GetNearCenter(struct OBJ *a)
{
    int i;
    struct OBJ *a2,*retobj=NULL;
    float distance=32000,tempdist;
    for (i=0;i<MaxObjects;i++)
    {
	    a2 = objects[i];
	    if (a2->playernr == a->playernr && IsUnitBaseBuild(a2)==1)
	    {
		tempdist = hypot(GetOBJx(a)-GetOBJx(a2),GetOBJy(a)-GetOBJy(a2));
		if (tempdist<distance)
		{
		    distance = tempdist;
		    retobj = a2;
		}
	    }
    }
    return retobj;
}
//=================================
struct OBJ *GetNearResource(struct OBJ *a,int SC_res_nr,int *resvalability)
{
    int i,tempdist;
    struct OBJ *a2,*retobj=NULL;
    struct OBJstruct *astr = loadobj(a->SC_Unit);
    int distance = GetUnitSightRange(a,astr)*SIZESPRLANSHX;
    *resvalability = 0;
    for (i=0;i<MaxObjects;i++)
    {
	a2 = objects[i];
	if (IsResource(a2,SC_res_nr))
	{
	    tempdist = (int) hypot(GetOBJx(a)-GetOBJx(a2),GetOBJy(a)-GetOBJy(a2));
	    if (tempdist < distance)
	    {
	        if (a2->resourceobj)
		{
		    *resvalability = 1;
		}
		else
		{
		    distance = tempdist;
		    retobj = a2;
		}
	    }
	}
    }
    return(retobj);
}
//=================================
int GetUnitSublabel(int SC_Unit)
{
    return(alldattbl.units_dat->Sublabel[SC_Unit]);
}
//=================================
int GetTargetAcquisitionRange(int SC_Unit)
{
    return(alldattbl.units_dat->TargetAcquisitionRange[SC_Unit]);
}
//=================================
int IsShieldEnable(int SC_Unit)
{
    return(alldattbl.units_dat->ShieldEnable[SC_Unit]);
}
//=================================
int IsProducesUnits(int SC_Unit)
{
    return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_PRODUCESUNITS)!=0);
}
//=================================
int IsBuild(int SC_Unit)
{
    return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_ISBUILD)!=0);
}
//=================================
int IsCloakable(int SC_Unit)
{
    return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_CLOAKABLE)!=0);
}
//=================================
int IsOrganic(int SC_Unit)
{
    return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_ORGANICUNIT)!=0);
}
//=================================
int IsBattleReactions(int SC_Unit)
{
    return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_BATTLEREACTIONS)!=0);
}
//=================================
int IsGroundUnit(int SC_Unit)
{
    return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_FLY)==0);
}
//=================================
int IsOnSkyOBJ(OBJ *a)
{
    return((a->prop & VARONSKY) !=0);
}
//=================================
void SetOnSkyOBJ(OBJ *a,int flag)
{
    if (flag)
	a->prop |= VARONSKY;
    else
	a->prop &= ~VARONSKY;
}
//=================================
int IsAirUnit(int SC_Unit)
{
    return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_FLY)!=0);
}
//=================================
int IsHeroUnit(int SC_Unit)
{
    return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_HERO)!=0);
}
//=================================
int IsRegenerate(int SC_Unit)
{
    return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_REGENERATE)!=0);
}
//=================================
int IsMechanical(int SC_Unit)
{
    return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_MECHANICALUNIT)!=0);
}
//=================================
int IsRobotic(int SC_Unit)
{
    return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_ROBOTICUNIT)!=0);
}
//=================================
int IsResourceContainer(int SC_Unit)
{
    return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_RESOURCECONTAINER)!=0);
}
//=================================
int IsResourceDepot(int SC_Unit)
{
    return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_RESOURCEDEPOT)!=0);
}
//=================================
int RequiresPylonEnergy(int SC_Unit)
{
    return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_REQUIRESPYLONAREA)!=0);
}
//=================================
int RequiresCreep(int SC_Unit)
{
    return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_REQUIRESCREEP)!=0);
}
//=================================
int IsPickupUnit(int SC_Unit)
{
    return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_PICKUPITEM)!=0);
}
//=================================
int IsNonNeutralFlag(int SC_Unit)
{
    return((alldattbl.units_dat->StarEditAvailabilityFlags[SC_Unit]&STAREDIT_AF_NONNEUTRAL)!=0);
}
//=================================
int IsExpansionUnit(int SC_Unit)
{
    return((alldattbl.units_dat->StarEditAvailabilityFlags[SC_Unit]&STAREDIT_AF_EXPANSIONUNIT)!=0);
}
//=================================
int IsDoodadState(int SC_Unit)
{
    return((alldattbl.units_dat->StarEditAvailabilityFlags[SC_Unit]&STAREDIT_AF_DOODADSTATE)!=0);
}
//=================================
int IsZergRace(int SC_Unit)
{
    return((alldattbl.units_dat->StarEditGroupFlags[SC_Unit]&STAREDIT_GF_ZERGRACE)!=0);
}
//=================================
int IsTerranRace(int SC_Unit)
{
    return((alldattbl.units_dat->StarEditGroupFlags[SC_Unit]&STAREDIT_GF_TERRANRACE)!=0);
}
//=================================
int IsProtossRace(int SC_Unit)
{
    return((alldattbl.units_dat->StarEditGroupFlags[SC_Unit]&STAREDIT_GF_PROTOSSRACE)!=0);
}
//=================================
int IsNoAnyGroupFlag(int SC_Unit)
{
    return((alldattbl.units_dat->StarEditGroupFlags[SC_Unit]&STAREDIT_GF_NOANYGROUPFLAG)==0);
}
//=================================
int IsGroupBuildFlag(int SC_Unit)
{
    return((alldattbl.units_dat->StarEditGroupFlags[SC_Unit]&STAREDIT_GF_BUILDING)!=0);
}
//=================================
int IsGroupMenFlag(int SC_Unit)
{
    return((alldattbl.units_dat->StarEditGroupFlags[SC_Unit]&STAREDIT_GF_MEN)!=0);
}
//=================================
int IsGroupFactoryFlag(int SC_Unit)
{
    return((alldattbl.units_dat->StarEditGroupFlags[SC_Unit]&STAREDIT_GF_FACTORY)!=0);
}
//=================================
int IsGroupNeutralFlag(int SC_Unit)
{
    return((alldattbl.units_dat->StarEditGroupFlags[SC_Unit]&STAREDIT_GF_NEUTRAL)!=0);
}
//=================================
int IsAddon(int SC_Unit)
{
    return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_ADDON)!=0);
}
//=================================
int IsSpellCaster(int SC_Unit)
{
    return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_SPELLCASTER)!=0);
}
//=================================
int IsDetector(int SC_Unit)
{
    return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_DETECTOR)!=0);
}
//=================================
int IsInvincibleUnit(int SC_Unit)
{
    return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_INVINCIBLE)!=0);
}
//=================================
int IsPermanentCloak(int SC_Unit)
{
    return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_PERMANENTCLOAK)!=0);
}
//=================================
int  IsSubUnit(int SC_Unit)
{
    return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_SUBUNIT)!=0);
}
//=================================
int IfHaveSubUnit(int SC_Unit)
{
    return(alldattbl.units_dat->Subunit1[SC_Unit] < MAX_UNITS_ELEM);
}
//=================================
int GetUnitSoundID(unsigned char SC_Unit,unsigned char sndtype)
{
    switch(sndtype)
    {
	case UNITSOUND_READY:
	    return(alldattbl.units_dat->ReadySound[SC_Unit]);
	case UNITSOUND_WHATFIRST:
	    return(alldattbl.units_dat->WhatSoundStart[SC_Unit]);
	case UNITSOUND_WHATLAST:
	    return(alldattbl.units_dat->WhatSoundEnd[SC_Unit]);
	case UNITSOUND_PISSFIRST:
	    return(alldattbl.units_dat->PissSoundStart[SC_Unit]);
	case UNITSOUND_PISSLAST:
	    return(alldattbl.units_dat->PissSoundStart[SC_Unit]);
	case UNITSOUND_YESFIRST:
	    return(alldattbl.units_dat->YesSoundStart[SC_Unit]);
	case UNITSOUND_YESLAST:
	    return(alldattbl.units_dat->YesSoundStart[SC_Unit]);
    }    
}
//=================================
int GetUnitSound(OBJ *a,unsigned char fromsndtype,unsigned char tosndtype)
{
    return( myrand( GetUnitSoundID(a->SC_Unit,fromsndtype), GetUnitSoundID(a->SC_Unit,tosndtype) ) );
}
//=================================
int IsInvincibleOBJ(OBJ *a)
{
    return(a->prop&VARINVINCIBILITY);
}
//=================================
int GetUnitWidthAndHeight(int SC_Unit,int pos)
{
    return(alldattbl.units_dat->StarEditPlacementBoxWidthAndHeight[SC_Unit][pos]);
}
//=================================
int GetUnitDimensions(int SC_Unit,int pos)	//leftuprightdown
{
    return(alldattbl.units_dat->UnitSizeLeftUpRightDown[SC_Unit][pos]);
}
//=================================
void SetInvincibleOBJ(OBJ *a,int flag)
{
    if (flag)
	a->prop|=VARINVINCIBILITY;
    else
	a->prop&=~VARINVINCIBILITY;
}
//=================================
int IsResource(struct OBJ *a,int SC_typeresource)
{
    switch(SC_typeresource)
    {
	case SC_EXTRACTOROBJ:
	case SC_ASSIMILATOROBJ:
	case SC_REFINERYOBJ:
	    switch(a->SC_Unit)
	    {
	        case SC_EXTRACTOROBJ:
	        case SC_ASSIMILATOROBJ:
	        case SC_REFINERYOBJ:
	    	return 1;
	    }
	    break;
	case SC_MINERAL1OBJ:
	case SC_MINERAL2OBJ:
	case SC_MINERAL3OBJ:
	    switch(a->SC_Unit)
	    {
		case SC_MINERAL1OBJ:
		case SC_MINERAL2OBJ:
		case SC_MINERAL3OBJ:
		    return 1;
	    }
	    break;
	case SC_GEYSEROBJ:
	    switch(a->SC_Unit)
	    {
		case SC_GEYSEROBJ:
		    return 1;
	    }
	    break;
    }
    return 0;
}
//=================================
int IsUnitBaseBuild(struct OBJ *a)
{
    switch(a->SC_Unit)
    {
	case SC_COMMCENTEROBJ:
	case SC_NEXUSOBJ:
	case SC_HATCHERYOBJ:
	    if (!IsOnSkyOBJ(a))
	    {
		if (IsReadyOBJ(a))
		    return(1);
		else
		    return(3);
	    }
	    else
		return(2);
	case SC_LAIROBJ:
	case SC_HIVEOBJ:
	    return(1);
    }
    return 0;
}
//=================================
int IsPsiUnit(int SC_Unit)
{
    return(GetSupplyProvided(SC_Unit)!=0);
}
//=================================
int IsWorkerUnit(int SC_Unit)
{
    switch(SC_Unit)
    {
	case SC_DRONEOBJ:
	case SC_SCVOBJ:
	case SC_PROBEOBJ:
	    return 1;
	default:
	    return 0;
    }
}
//=================================
OBJ *GetGEYSERfromMAP(int xmap,int ymap)
{
    OBJ *a;
    for (int i=0;i<MaxObjects;i++)
    {
	a=objects[i];
	if (a->SC_Unit==SC_GEYSEROBJ)
	{
	    if (a->xkart==xmap&&a->ykart==ymap)
		return(a);
	}
    }
    return(NULL);
}
//==========================================
int GetUnitRace(int SC_Unit)
{
    int i;
    for (i=0;i<3;i++)
    {
	if (alldattbl.units_dat->StarEditGroupFlags[SC_Unit] & (1<<i))
	    return(i);
    }
    return(NEUTRALRACE);
}
//==================================
int IsCloaked(OBJ *a)
{
    return(a->prop&UNITPROP_PREVCLOAK);
}
//==================================
int SetCloakedFlag(OBJ *a,int flag)
{
    if (flag)
	a->prop |= UNITPROP_PREVCLOAK;
    else
	a->prop &= ~UNITPROP_PREVCLOAK;
}
//==================================
int IsOBJUnderConstruct(OBJ *a)
{
    return(!(a->prop&VARREADY));
}
//==================================
int IsReadyOBJ(OBJ *a)
{
    return(a->prop&VARREADY);
}
//==================================
int IsHallucination(OBJ *a)
{
    return(GetMageAtr(&a->atrobj,ATRHALLUCINATION));
}
//==================================
int ChangeUnitIconProp(OBJ *a,int typeofprop)
{
    a->typeofprop = typeofprop;
}
//==================================
void ChangeSC_Unit(struct OBJ *a,int newplayernr,int SC_Unit,int flagready)
{
    int inegg,i,ready;
    if (flagready==CHANGESC_UNIT_CONSTR)
    {
	a->prop &= ~VARREADY;
    }
    else
    {
	a->prop |= VARREADY;
    }	
    a->SC_Unit=SC_Unit;
    inegg = GetInEgg(a->SC_ConstrUnit);
    for (i=0;i<=inegg;i++)
    {
	if (a->SC_FromUnit!=SC_NOUNITNR)
	{
	    ChangeSupply(a->playernr,a->SC_FromUnit,MINUSFACTOR);
	    if (IsReadyOBJ(a))
		ChangeMaxSupply(a->playernr,a->SC_FromUnit,MINUSFACTOR);
	}
	if (a->SC_ConstrUnit!=SC_NOUNITNR)
	{
	    ChangeSupply(newplayernr,a->SC_ConstrUnit,PLUSFACTOR);
	    if (IsReadyOBJ(a))
		ChangeMaxSupply(newplayernr,a->SC_ConstrUnit,PLUSFACTOR);
	}
    }
}
//==================================
int GetBirthDirection(int SC_Unit)
{
    unsigned char birthdir=alldattbl.units_dat->UnitDirection[SC_Unit];
    if (birthdir>=32)
	birthdir=myrand(32);
    return(birthdir);
}
//==================================
int IsOBJBurrowed(OBJ *a)
{
    return(a->prop & VARBURROW);
}
//==================================
int SetBurrowFlag(OBJ *a,int flag)
{
    if (flag)
    {
	a->prop|=VARBURROW;
	if (a->carryobj)
	    a->carryobj->prop|=VARBURROW;
    }
    else
    {
	a->prop&=~VARBURROW;
	if (a->carryobj)
	    a->carryobj->prop&=~VARBURROW;
    }
}
//==================================
char *getmapSTR(struct mapinfo *testmap,int nrofstr)
{
    char *str,*buf;
    int offset;
    MAPStrings *strs;
    strs=(MAPStrings *)testmap->STR;
    buf=testmap->STR;
    offset=strs->offset[nrofstr];
    str=buf+offset;
    return(str);
}
//==========================================
int GetDeltaAddonHorizontal(int SC_Unit)
{
    return(alldattbl.units_dat->AddonHorizontalVertical[SC_Unit - 106][0]);
}
//==========================================
int GetDeltaAddonVertical(int SC_Unit)
{
    return(alldattbl.units_dat->AddonHorizontalVertical[SC_Unit - 106][1]);
}
//==========================================
int IfGeyserIsDepleted(OBJ *a)
{
    return(a->data.resource.resource_count == 0);
}
//==========================================
