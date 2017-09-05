#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include "man.h"
#include "vars.h"
#include "auxil.h"
#include "maps.h"
#include "upgrade.h"
#include "scripts.h"
#include "lists.h"
#include "objs.h"
#include "objinfo.h"
#include "load.h"
#include "putobj.h"
#include "mage.h"
#include "action.h"
#include "weapons.h"
#include "overlay.h"
#include "images.h"
#include "market.h"
//=======================================
int getusemanaformodemove(int modemove)
{
    if (mageprop[modemove].type_id==ORDERS_MODEMOVE)
    {
	if (mageprop[modemove].obj_id!=-1)
	    return(GetTechEnergyCost(mageprop[modemove].obj_id));
    }
    return(0);
}
//=======================================
int CheckForMana(OBJ *a,int needmana)
{
    if ((needmana<<8)<=(a->mana))
	return(CHECKRES_OK);
    return(CHECKRES_MANA);
}
//=======================================
void DecrMana(OBJ *a,int decreasemana)
{
    a->mana-=(decreasemana<<8);
    if (a->mana<0)
	a->mana=0;
}
//=======================================
void DecrManaBITS(OBJ *a,int decreasemana)
{
    a->mana-=decreasemana;
    if (a->mana<0)
	a->mana=0;
}
//=======================================
void IncrMana(OBJ *a,int increasemana)
{
    a->mana+=(increasemana<<8);
    if (a->mana>GetUnitMaxMana(a->SC_Unit,a->playernr))
        a->mana=GetUnitMaxMana(a->SC_Unit,a->playernr);
}
//=======================================
void IncrManaBITS(OBJ *a,int increasemana)
{
    a->mana+=increasemana;
    if (a->mana>GetUnitMaxMana(a->SC_Unit,a->playernr))
        a->mana=GetUnitMaxMana(a->SC_Unit,a->playernr);
}
//=======================================
int GetUnitMaxMana(int SC_Unit,int playernr)
{
    return(MAXMANA+GetAddManaFactor(SC_Unit,playernr));
}
//=======================================
void SetUnitMana(OBJ *a,int mana)
{
    a->mana=mana<<8;
    if (a->mana>GetUnitMaxMana(a->SC_Unit,a->playernr))
        a->mana=GetUnitMaxMana(a->SC_Unit,a->playernr);
}
//=======================================
void SetUnitPercentMana(OBJ *a,int persmana)
{
    a->mana=GetUnitMaxMana(a->SC_Unit,a->playernr)*persmana/100;
}
//=======================================
int CheckResourcePlayer(int playernr,int mcost,int gcost)
{
    if (CODEFORWITHOUTGOODS)
	return(CHECKRES_OK);
    if ((mcost<<8)>PLAYER[playernr].minerals)
	return(CHECKRES_MIN);
    if ((gcost<<8)>PLAYER[playernr].gas)
	return(CHECKRES_GAS);
    return(CHECKRES_OK);
}
//==================================
int CheckResourcePlayerBITS(int playernr,int mcostbits,int gcostbits)
{
    if (CODEFORWITHOUTGOODS)
	return(CHECKRES_OK);
    if (mcostbits>PLAYER[playernr].minerals)
	return(CHECKRES_MIN);
    if (gcostbits>PLAYER[playernr].gas)
	return(CHECKRES_GAS);
    return(CHECKRES_OK);
}
//==================================
int GetUnitBoxWidth(int SC_Unit)
{
    return(alldattbl.units_dat->StarEditPlacementBoxWidthAndHeight[SC_Unit][0]);
}
//==================================
int GetUnitBoxHeight(int SC_Unit)
{
    return(alldattbl.units_dat->StarEditPlacementBoxWidthAndHeight[SC_Unit][1]);
}
//==================================
int GetInEgg(int SC_Unit)
{
    return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_TWOINONEEGG)!=0);
}
//==================================
int GetSupplyUnit(int SC_Unit)
{
    return(alldattbl.units_dat->SupplyRequired[SC_Unit]);
}
//==================================
int GetSupplyProvided(int SC_Unit)
{
    return(alldattbl.units_dat->SupplyProvided[SC_Unit]);
}
//==================================
int GetSpaceProvided(int SC_Unit)
{
    return(alldattbl.units_dat->SpaceProvided[SC_Unit]);
}
//==================================
int GetSpaceRequired(int SC_Unit)
{
    return(alldattbl.units_dat->SpaceRequired[SC_Unit]);
}
//==================================
int GetUnitSize(int SC_Unit)
{
    return(alldattbl.units_dat->UnitSize[SC_Unit]);
}
//==================================
int GetUnitOverlaySize(int SC_Unit)
{
    if (alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_USELARGEOVERLAY)
	return(LARGEOVERLAY);
    if (alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_USEMEDIUMOVERLAY)
	return(MEDIUMOVERLAY);
    return(SMALLOVERLAY);
}
//==================================
int CheckSupplyPlayer(int playernr,int SC_Unit)
{
    int race,supply,inegg;
    if (CODEFORPSIUNLIMIT)
	return(CHECKRES_OK);
    supply=GetSupplyUnit(SC_Unit);
    inegg=GetInEgg(SC_Unit);
    if (inegg)
	supply*=2;
    if (supply)
    {
	race=GetUnitRace(SC_Unit);
	if (race<3)
	    if (PLAYER[playernr].curentpsi[race]+supply>PLAYER[playernr].maxcurentpsi[race])
		return(CHECKRES_SUPPLY);
    }
    return(CHECKRES_OK);
}
//=======================================
void ChangeSupply(int playernr,int SC_Unit,int FACTOR)
{
    if (playernr<PLAYEDPLAYERS)
    if (SC_Unit!=SC_NOUNITNR)
    {
	int race=GetUnitRace(SC_Unit);
	if (race<3)
	{
	    PLAYER[playernr].curentpsi[race]+=GetSupplyUnit(SC_Unit)*FACTOR;
        }
    }
}
//=======================================
void ChangeMaxSupply(int playernr,int SC_Unit,int FACTOR)
{
    int race;
    if (SC_Unit!=SC_NOUNITNR)
    {
	if (IsPsiUnit(SC_Unit))
	{
	    race=GetUnitRace(SC_Unit);
	    if (race<3)
		PLAYER[playernr].maxcurentpsi[race]+=GetSupplyProvided(SC_Unit)*FACTOR;
	}
    }
}
//=======================================
void ChangeResourcePlayer(int playernr,int FACTOR,int mcost,int gcost)
{
    PLAYER[playernr].minerals+=(mcost<<8)*FACTOR;
    PLAYER[playernr].gas+=(gcost<<8)*FACTOR;
}
//=======================================
void ChangeResourcePlayerBITS(int playernr,int FACTOR,int mcostbit,int gcostbit)
{
    PLAYER[playernr].minerals+=mcostbit*FACTOR;
    PLAYER[playernr].gas+=gcostbit*FACTOR;
}
//=======================================
//return nr of ticks
int GetUnitBuildTime(int SC_Unit)
{
    if (map.unit_settings.use_defaults[SC_Unit])
    {
	return(alldattbl.units_dat->BuildTime[SC_Unit]);
    }
    else
    {
	if (!map.unit_settings.buildtime[SC_Unit])
	    return(alldattbl.units_dat->BuildTime[SC_Unit]);
    	else
    	    return(map.unit_settings.buildtime[SC_Unit]);
    }
}
//=======================================
void GetCostUnit(int SC_Unit,int *mcost,int *gcost)
{
    if (map.unit_settings.use_defaults[SC_Unit])
    {
	*mcost=alldattbl.units_dat->MineralCost[SC_Unit];
	*gcost=alldattbl.units_dat->VespeneCost[SC_Unit];
    }
    else
    {
	*mcost=map.unit_settings.mineralcost[SC_Unit];
	*gcost=map.unit_settings.gascost[SC_Unit];
    }
}
//=======================================
int GetBuildUnitScore(int SC_Unit)
{
    return(alldattbl.units_dat->BuildScore[SC_Unit]);
}
//=======================================
int GetDestroyUnitScore(int SC_Unit)
{
    return(alldattbl.units_dat->DestroyScore[SC_Unit]);
}
//=======================================
void GetCostUpgr(int upgr_id,int playernr,int *mcost,int *gcost)
{
    int upgr_level=GetUpgradeTree(&map,playernr,upgr_id);
    if (map.upgr_settings.use_defaults[upgr_id])
    {
	*mcost=alldattbl.upgrades_dat->BaseMineralCost[upgr_id]+alldattbl.upgrades_dat->FactorMineralCost[upgr_id]*upgr_level;
	*gcost=alldattbl.upgrades_dat->BaseVespeneCost[upgr_id]+alldattbl.upgrades_dat->FactorVespeneCost[upgr_id]*upgr_level;
    }
    else
    {
	*mcost=map.upgr_settings.basecostminerals[upgr_id]+map.upgr_settings.upgradecostminerals[upgr_id]*upgr_level>>8;
	*gcost=map.upgr_settings.basecostgas[upgr_id]+map.upgr_settings.upgradecostgas[upgr_id]*upgr_level>>8;
    }
}
//=======================================
int GetUpgradeResearchTime(int upgr_id,int upgrlevel)
{
    if (map.upgr_settings.use_defaults[upgr_id])
    {
	return(alldattbl.upgrades_dat->BaseResearchTime[upgr_id]+
		alldattbl.upgrades_dat->FactorResearchTime[upgr_id]*(upgrlevel-1));
    }
    else
    {
	return((map.upgr_settings.basecosttime[upgr_id]+map.upgr_settings.upgradecosttime[upgr_id]*(upgrlevel-1))>>8);
    }
}
//=======================================
void GetCostTech(int tech_id,int *mcost,int *gcost)
{
    if (map.tech_settings.use_defaults[tech_id])
    {
	*mcost=alldattbl.techdata_dat->MineralCost[tech_id];
	*gcost=alldattbl.techdata_dat->VespeneCost[tech_id];
    }
    else
    {
	*mcost=map.tech_settings.costminerals[tech_id];
	*gcost=map.tech_settings.costgas[tech_id];
    }
}
//=======================================
int GetTechEnergyCost(int tech_id)
{
    if (map.tech_settings.use_defaults[tech_id])
    {
	return(alldattbl.techdata_dat->EnergyCost[tech_id]);
    }
    else
    {
	return(map.tech_settings.energycost[tech_id]);
    }
}
//=======================================
int GetTechResearchTime(int tech_id)
{
    if (map.tech_settings.use_defaults[tech_id])
    {
	return(alldattbl.techdata_dat->ResearchTime[tech_id]);
    }
    else
    {
	return(map.tech_settings.researchtime[tech_id]);
    }
}
//=======================================
int CheckForResource_typeid(OBJ *a,int playernr,int type_id,int obj_id)
{
    int err=CHECKRES_OK;
    int mcost,gcost,needmana;
    switch(type_id)
    {
        case ORDERS_NONE:
	    break;
	case ORDERS_MODEMOVE:
	    if (obj_id!=-1)
	    {
		needmana=GetTechEnergyCost(obj_id);
		err=CheckForMana(a,needmana);
	    }
	    break;
	case ORDERS_UNIT:	//(check for resources, psi)
	    err=CheckSupplyPlayer(playernr,obj_id);
	    if (err==CHECKRES_OK)
	    {
		GetCostUnit(obj_id,&mcost,&gcost);
		err=CheckResourcePlayer(playernr,mcost,gcost);
	    }
	    break;
	case ORDERS_UPGRADE:
	    GetCostUpgr(obj_id,playernr,&mcost,&gcost);
	    err=CheckResourcePlayer(playernr,mcost,gcost);
	    break;
	case ORDERS_TECHNOLOGY:
	    GetCostTech(obj_id,&mcost,&gcost);
	    err=CheckResourcePlayer(playernr,mcost,gcost);
	    break;
    }
    return(err);
}
//=======================================
unsigned char GetUnitElevationLevel(int SC_Unit)
{
    return(alldattbl.units_dat->ElevationLevel[SC_Unit]);
}
//=======================================
int GetUnitMaxHitPoints(int SC_Unit)
{
    int hp;
    hp = GetUnitMaxLife(SC_Unit);
    if (IsShieldEnable(SC_Unit))
	hp += GetUnitMaxShield(SC_Unit);
    return(hp);
}
//=======================================
unsigned int GetUnitMaxLife(int SC_Unit)
{
    if (map.unit_settings.use_defaults[SC_Unit])
    {
	return(alldattbl.units_dat->HitPoints[SC_Unit]);
    }
    else
    {
	return(map.unit_settings.unitlife[SC_Unit].hitpoints<<8);
    }
}
//=======================================
void SetUnitLife(OBJ *a,int life)
{
    a->life=life;
}
//=======================================
void SetUnitPercentLife(OBJ *a,int perslife)
{
    SetUnitLife(a,GetUnitMaxLife(a->SC_Unit)*perslife/100);
}
//=======================================
void AddUnitLife(OBJ *a,int addlife)
{
    a->life += addlife;
    if (a->life > GetUnitMaxLife(a->SC_Unit))
	a->life = GetUnitMaxLife(a->SC_Unit);
}
//=======================================
unsigned int GetUnitMaxShield(int SC_Unit)
{
//    if (IsShieldEnable(SC_Unit))
    {
	if (map.unit_settings.use_defaults[SC_Unit])
    	    return(alldattbl.units_dat->ShieldAmount[SC_Unit]<<8);
    	else
	    return(map.unit_settings.shieldpoints[SC_Unit]<<8);
    }
    return(0);
}
//=======================================
void SetUnitShield(OBJ *a,int shield)
{
    a->shield=shield;
}
//=======================================
void SetUnitPercentShield(OBJ *a,int persshield)
{
    SetUnitShield(a,GetUnitMaxShield(a->SC_Unit)*persshield/100);
}
//=======================================
void AddUnitShield(OBJ *a,int addshield)
{
    a->shield+=addshield;
    if (a->shield>GetUnitMaxShield(a->SC_Unit))
	a->shield=GetUnitMaxShield(a->SC_Unit);
}
//=======================================
int damages[UNITSIZE_TYPES][DAMAGE_TYPES]=
{
    {  0,  0,  0,  0,  0},
    {  0, 50,100,100,100},
    {  0, 75, 50,100,100},
    {  0,100, 25,100,100}
};
//=======================================
int LowLevelDamage(OBJ *atacker,OBJ *a,OBJstruct *b,int weapon_id,int typedamage,int totaldamage,int directiondamage,int directorsplash)
{
    int remaindamage,armor,upgnr,upgarmor,shieldarmor;
    if (atacker && IsHallucination(atacker))
	return(0);
//    if (IsInvincibleUnit(a->SC_Unit))
//	return(0);
    //10 point for every of acid spore (9-max);
    totaldamage += (GetCorrosiveAcidValue(a)<<8);
    remaindamage = totaldamage;
    if (IsRangeWeapon(weapon_id) && !UniqueWeapon(weapon_id))
	if (directorsplash & DAMAGE_SPLASH)
	{
	    if (!IsBuild(a->SC_Unit))
		if (IsOBJBurrowed(a))//if burrowed under swarm - splash do no damage
		    if (mapEFFECT(a->xkart,a->ykart,DSWARMEFFECT))
		    {
			return(0);
		    }
	}
	else
	{
	    if (!IsBuild(a->SC_Unit)&&!IsOnSkyOBJ(a))
		if (mapEFFECT(a->xkart,a->ykart,DSWARMEFFECT))//if direct range atack under swarm - do no damage
		{
		    return(0);
		}
	}
    if (atacker && a)
//	atacker->select[a->playernr] |= VARSEE;
	OBJ_VAR_SET(atacker,obj_see,a->playernr);
    atackback(atacker,a,directiondamage);
    if (atacker)
    {
	a->atackernr=atacker->playernr;
	a->whoatack=atacker;
    }
    if (a->dmatrix-a->dmatrixdamage>0&&remaindamage)
    {
	remaindamage=totaldamage-(a->dmatrix-a->dmatrixdamage);
	if (remaindamage>=0)
	{
	    a->dmatrix=0;
	    SetMageAtr(&a->atrobj,ATRMATRIX,0);
	    a->dmatrixdamage=1;
	    DelImagesForAtribute(a,ATRMATRIX);
	}
	else
	{
	    a->dmatrix=-remaindamage;
	    a->dmatrixdamage+=totaldamage;
	    remaindamage=totaldamage/28;
	}
    }
    totaldamage=remaindamage;
    if (IsShieldEnable(a->SC_Unit)&&remaindamage&&weapon_id!=WEAPONID_PLAGUE)
    {
	shieldarmor = GetUpgradeTree(&map,a->playernr,UPGRADES_DAT_PLASMASHIELDICON)<<8;
	remaindamage = totaldamage-(a->shield-a->shielddamage+shieldarmor);
	AddShieldImage(a,directiondamage);
	if (remaindamage>0)
	{
	    a->shielddamage += a->shield-a->shielddamage;		//damage to shield equal to current shield-shieldarmor
	}
	else
	{
	    remaindamage=0;
	    a->shielddamage += totaldamage-shieldarmor;		//damage to shield equal to totaldamage
	}
    }
    int unitsize=GetUnitSize(a->SC_Unit);
    
    switch(typedamage)
    {
	case DAMAGE_INDEPENDENT:
	    if (atacker)
	    {
		ApplyCastedMage(a,atacker,atacker->memoryformage);
		if (!remaindamage)
		    return(1);
	    }
	    armor=0;
	    break;
	case DAMAGE_IGNOREARMOR:
	case DAMAGE_EXPLOSIVE:
	case DAMAGE_CONCUSSIVE:
	case DAMAGE_NORMAL:
	    if (!remaindamage)
		return(1);
	    remaindamage=remaindamage*damages[unitsize][typedamage]/100;
    	    if (typedamage!=DAMAGE_IGNOREARMOR)
    	    {
		armor=GetArmor(a,b->armorupgnr,&upgnr,&upgarmor);
		armor+=upgarmor;
	    }
	    else
		armor=0;
	    break;
    }
    totaldamage=remaindamage;
    remaindamage=totaldamage-(a->life-a->lifedamage+(armor<<8));
    if (remaindamage>0)
    {
	if (weapon_id==WEAPONID_PLAGUE)
	    a->lifedamage+=(a->life-a->lifedamage)-((armor+1)<<8);
	else
	    a->lifedamage+=a->life-a->lifedamage;
    }
    else
    {
	if (totaldamage-(armor<<8)<0)
	    a->lifedamage+=0x80;
	else
    	    a->lifedamage+=totaldamage-(armor<<8);
    }
//    if (atacker && (!GetUnitProp(atacker,a->playernr,VARNOTDETECT)))
    if (atacker && !OBJ_VAR_CHK(atacker,obj_notdetect,a->playernr))
    {
	opentempmap(a->playernr,atacker->xkart,atacker->ykart,3,3);
    }
    return(0);
}
//=======================================
int GetAtackCoolDown(OBJ *atacker,int weapon_id)
{
    int cooldown;
    OBJstruct *batacker;
    cooldown = alldattbl.weapons_dat->WeaponCooldown[weapon_id];
    batacker = loadobj(atacker->SC_Unit);
    if (batacker->atackspeedupgnr != -1)
	if (GetUpgradeTree(&map,atacker->playernr,batacker->atackspeedupgnr))
	    cooldown = cooldown*2/3;
    cooldown += cooldown*GetCorrosiveAcidValue(atacker)/8;
    if (GetMageAtr(&atacker->atrobj,ATRSTIMPACK)>0)
	cooldown = cooldown*2/3;
    return(cooldown);
}

