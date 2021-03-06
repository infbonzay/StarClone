

#include <math.h>

#include "load.h"
#include "mage.h"
#include "wmem.h"
#include "putobj.h"
#include "defsunits.h"
#include "maps.h"
#include "objs.h"
#include "defs.h"
#include "man.h"
#include "auxil.h"
#include "vars.h"
#include "upgrade.h"
#include "action.h"
#include "rand.h"
#include "vision.h"
#include "weapons.h"
#include "market.h"
#include "lists.h"
#include "actiondefault.h"
#include "images.h"
#include "gener.h"
#include "Enumerate.h"
#include "ObjChecks.h"

#define MINCARRIERCAPACITY		4
#define MINREAVERCAPACITY		5
//====================================
int INBAY_GetCreatedUnits(OBJ *a)
{
	switch (a->SC_Unit)
	{
	case SC_CARRIEROBJ:
	case SC_HERO_GANTRITHOROBJ:
	case SC_NUCLEARSILOOBJ:
		return(getchilds(a));
	case SC_REAVEROBJ:
	case SC_HERO_WARBRINGEROBJ:
		return(a->ammo);
	}
	return(0);
}
//====================================
int INBAY_GetMaxUnitsToBeCreated(OBJ *a)
{
	int upgradenr;
	switch (a->SC_Unit)
	{
	case SC_HERO_GANTRITHOROBJ:
		return(MINCARRIERCAPACITY*(1 + 1));
	case SC_CARRIEROBJ:
		upgradenr = GetUpgradeTree(&map, a->playernr, TECHDATA_DAT_CARRIERCAPACITYTECH);
		return(MINCARRIERCAPACITY*(upgradenr + 1));
	case SC_HERO_WARBRINGEROBJ:
		return(MINREAVERCAPACITY*(1 + 1));
	case SC_REAVEROBJ:
		upgradenr = GetUpgradeTree(&map, a->playernr, TECHDATA_DAT_REAVERCAPACITYTECH);
		return(MINREAVERCAPACITY*(upgradenr + 1));
	case SC_NUCLEARSILOOBJ:
		return(1);
	default:
		return(MAXCONSTRUCTINBUILD);
	}
}
//====================================
void AdditionalProperties(OBJ *a)
{
	switch (a->SC_Unit)
	{
	case SC_HERO_JIMRAYNORVOBJ:
	case SC_VULTUREOBJ:
		a->ammo = 3;
		break;
	case SC_HATCHERYOBJ:
		HiveActionFirst(a, a->mainimage);
		break;
	}
}
//====================================
int GetSpeed(OBJ *a, int speed)
{
	OBJstruct *b = loadobj(a->SC_Unit);
	if (b->speedupgnr != 0xff)
	{
		if (IsHeroUnit(a->SC_Unit))
		{
			speed += speed * b->speeduppercentage / 25600;
		}
		else
		{
			if (b->speedupgnr == 0xfe || GetUpgradeTree(&map, a->playernr, b->speedupgnr))
				speed += speed * b->speeduppercentage / 25600;
		}
	}
	if (GetMageAtr(&a->atrobj, ATRENSNARE) > 0)
		speed /= 2;
	if (GetMageAtr(&a->atrobj, ATRSTIMPACK) > 0)
		speed += speed / 2;
	return(speed);
}
//====================================
int GetUnitSightRange(OBJ *a, OBJstruct *b)
{
	char seerange = alldattbl.units_dat->SightRange[a->SC_Unit];
	if (GetMageAtr(&a->atrobj, ATRBLIND) > 0)
		return(MINSEERANGE);
	if (b->sightupgnr != -1)
	{
		if (IsHeroUnit(a->SC_Unit))
			seerange += 2;
		else
			seerange += GetUpgradeTree(&map, a->playernr, b->sightupgnr) * 2;//each upgrade add +2 to sight range
	}
	return(seerange);
}
//=================================
int GetWidthSummOfUnits(SCUNIT SC_Unit1, SCUNIT SC_Unit2, int deltax, int deltay)
{
	if (abs(deltax) > abs(deltay))
		return ((GetUnitWidthAndHeight(SC_Unit1, UNITDIM_WIDTH) / 2 + GetUnitWidthAndHeight(SC_Unit2, UNITDIM_WIDTH) / 2) << 8);
	else
		return ((GetUnitWidthAndHeight(SC_Unit1, UNITDIM_HEIGHT) / 2 + GetUnitWidthAndHeight(SC_Unit2, UNITDIM_HEIGHT) / 2) << 8);
}
//=================================
int GetDestUnitHalfSize(SCUNIT SC_Unit, int deltax, int deltay)
{
	if (abs(deltax) > abs(deltay))
		return (GetUnitWidthAndHeight(SC_Unit, UNITDIM_WIDTH) / 2);
	else
		return (GetUnitWidthAndHeight(SC_Unit, UNITDIM_HEIGHT) / 2);
}
//====================================
int GetDistances(int x, int y, int x2, int y2)
{
	return((int)hypot(x2 - x, y2 - y));
}
//====================================
int GetDistanceMinusSizes(int unit1, int unit2, int deltax, int deltay)
{
	int deltaz;
	deltaz = ((int)hypot(deltax, deltay)) - GetWidthSummOfUnits(unit1, unit2, deltax, deltay);
	if (deltaz < 0)
		deltaz = 0;
	return(deltaz);
}
//====================================
int GetDistanceBetweenUnits256(OBJ *a, OBJ *a2)
{
	return(GetDistanceMinusSizes(a->SC_Unit, a2->SC_Unit, GetOBJx256(a2) - GetOBJx256(a), GetOBJy256(a2) - GetOBJy256(a)));
}
//====================================
int GetDistanceTo256(OBJ *a, int x256, int y256)
{
	int deltaz, deltax, deltay;
	deltax = x256 - GetOBJx256(a);
	deltay = y256 - GetOBJy256(a);
	deltaz = ((int)hypot(deltax, deltay));
	if (abs(deltax) > abs(deltay))
		deltaz -= (GetUnitWidthAndHeight(a->SC_Unit, UNITDIM_WIDTH) / 2) << 8;
	else
		deltaz -= (GetUnitWidthAndHeight(a->SC_Unit, UNITDIM_HEIGHT) / 2) << 8;
	if (deltaz < 0)
		deltaz = 0;
	return(deltaz);
}
//====================================
int GetDistanceTo(OBJ *a, int x, int y)
{
	int deltaz, deltax, deltay;
	deltax = x - GetOBJx(a);
	deltay = y - GetOBJy(a);
	deltaz = ((int)hypot(deltax, deltay));
	if (abs(deltax) > abs(deltay))
		deltaz -= GetUnitWidthAndHeight(a->SC_Unit, UNITDIM_WIDTH) / 2;
	else
		deltaz -= GetUnitWidthAndHeight(a->SC_Unit, UNITDIM_HEIGHT) / 2;
	if (deltaz < 0)
		deltaz = 0;
	return(deltaz);
}
//====================================
int GetRangeWeaponInPixels(OBJ *a, int weapon_id, int playernr)
{
	int maxdist = alldattbl.weapons_dat->MaximumRange[weapon_id];
	if (weapons[weapon_id].rangeupgnr != 255)
	{
		if (IsHeroUnit(a->SC_Unit))
			maxdist += weapons[weapon_id].rangeupgaddfactor;
		else
			maxdist += GetUpgradeTree(&map, playernr, weapons[weapon_id].rangeupgnr)*weapons[weapon_id].rangeupgaddfactor;
	}
	if (a->prop & VARINTRANSPORT)
		maxdist += 48;
	return(maxdist);
}
//====================================
int CheckWeaponRange(OBJ *a, OBJ *destobj, int weapon_id, int playernr)
{
	int deltaz, maxrange, minrange;
	//	  OBJ *base = a;
	OBJ *base;
	if (GetSubUnitType(a) == SUBUNITTURRET)
		base = a->subunit;
	else
		base = a;

	deltaz = GetDistanceBetweenUnits256(base, destobj) >> 8;
	minrange = alldattbl.weapons_dat->MinimumRange[weapon_id];
	maxrange = GetRangeWeaponInPixels(base, weapon_id, a->playernr);
	//	  printf("deltaz=%d maxrange=%d\n",deltaz,maxrange);
	if (deltaz < maxrange)
	{
		if (deltaz >= minrange)
			return(0);//in atack range;
		else
			return(2);//to close
	}
	return(1);//too far
}
//====================================
int UnitWeaponMask(SCUNIT SC_Unit)
{
	SCUNIT Subunit1;
	if (SC_Unit == SC_BUNKEROBJ)
	{
		return((1 << GROUNDATACKER) | (1 << AIRATACKER));
	}
	int weaponmask = 1 << NOATACKER;
	Subunit1 = (SCUNIT) alldattbl.units_dat->Subunit1[SC_Unit];
	if (Subunit1 < MAX_UNITS_ELEM)
		SC_Unit = Subunit1;
	if (alldattbl.units_dat->GroundWeapon[SC_Unit] < MAX_WEAPONS_ELEM)
		weaponmask |= 1 << GROUNDATACKER;
	if (alldattbl.units_dat->AirWeapon[SC_Unit] < MAX_WEAPONS_ELEM)
		weaponmask |= 1 << AIRATACKER;
	return(weaponmask);
}
//====================================
unsigned char GetNewWeaponType(OBJ *a, OBJ *destobj, int newatackerunitusedweapon)
{
	unsigned char groundweapon, airweapon, weapon_id;
	SCUNIT SC_Unit;
	//	  Subunit1 = alldattbl.units_dat->Subunit1[newatackerunitusedweapon];
	//	  if (Subunit1<MAX_UNITS_ELEM)
	//		SC_Unit = Subunit1;
	//	  else
	SC_Unit = newatackerunitusedweapon;
	weapon_id = MAX_WEAPONS_ELEM;
	groundweapon = alldattbl.units_dat->GroundWeapon[SC_Unit];
	if (groundweapon < MAX_WEAPONS_ELEM && !IsOnSkyOBJ(destobj))
		weapon_id = groundweapon;
	airweapon = alldattbl.units_dat->AirWeapon[SC_Unit];
	if (airweapon < MAX_WEAPONS_ELEM && IsOnSkyOBJ(destobj))
		weapon_id = airweapon;
	return(weapon_id);
}
//====================================
int GetAddManaFactor(SCUNIT SC_Unit, int playernr)
{
	OBJstruct *b = loadobj(SC_Unit);
	if (IsHeroUnit(SC_Unit))
		return(50 << 8);//heroes add +50 to maxmana
	if (b->manaupgnr)
	{
		if (GetUpgradeTree(&map, playernr, b->manaupgnr))
			return(50 << 8);//upgrade add +50 to maxmana
	}
	return(0);
}
//====================================
//return
//damage - nr of someobjects
int GetNrSomeObjects(OBJ *a, int *newunit, char **iconmes)
{
	int weapon_id, tech_id;
	if (player_aliance(NUMBGAMER, a->playernr) != MYOBJ)
		return(-1);
	switch (a->SC_Unit)
	{
	case SC_VULTUREOBJ:
	case SC_HERO_JIMRAYNORVOBJ:
		if (a->ammo == 0)
			return(-1);
		*newunit = SC_VULTUREMINEOBJ;
		weapon_id = alldattbl.units_dat->GroundWeapon[SC_VULTUREMINEOBJ];
		tech_id = alldattbl.weapons_dat->Technology_id[weapon_id];
		if (GetTechTree(&map, a->playernr, tech_id) || IsHeroUnit(a->SC_Unit))
		{
			*iconmes = alldattbl.stattxt_tbl->get_TBL_STR(STATTXT_TBL_MINESTEXT);
			return(a->ammo);
		}
		return -1;
	case SC_NUCLEARSILOOBJ:
		*iconmes = alldattbl.stattxt_tbl->get_TBL_STR(STATTXT_TBL_NUKESTEXT);
		*newunit = SC_NUKEOBJ;
		return(getchilds(a));
	case SC_REAVEROBJ:
	case SC_HERO_WARBRINGEROBJ:
		*iconmes = alldattbl.stattxt_tbl->get_TBL_STR(STATTXT_TBL_SCARABSTEXT);
		*newunit = SC_SCARABOBJ;
		return(a->ammo);
	case SC_CARRIEROBJ:
	case SC_HERO_GANTRITHOROBJ:
		*iconmes = alldattbl.stattxt_tbl->get_TBL_STR(STATTXT_TBL_INTERCEPTORSTEXT);
		*newunit = SC_INTERCEPTOROBJ;
		return(getchilds(a));
	default:
		return(-1);
	}
}
//====================================
//return
//damage - simple damage without upgrades
//*upgradenr - upgrade number they have
//*upgdamage - damage that have upgrade technology
//so totaldamage=damage+*upgdamage
int GetAtackDamage(SCUNIT SC_Unit, int playernr, int nrofhits, int weapon_id, int *upgradenr, int *upgdamage)
{
	unsigned char weapon_upgr;
	int nrofmissiles;
	int onemissiledamageperupgrade;
	weapon_upgr = alldattbl.weapons_dat->DamageUpgrade[weapon_id];
	*upgradenr = GetUpgradeTree(&map, playernr, weapon_upgr);
	nrofmissiles = alldattbl.weapons_dat->DamageFactor[weapon_id];
	if (map.unit_settings.use_defaults[SC_Unit])
	{
		onemissiledamageperupgrade = alldattbl.weapons_dat->DamageBonus[weapon_id];
		*upgdamage = (*upgradenr)*onemissiledamageperupgrade*nrofmissiles*nrofhits;
		return(alldattbl.weapons_dat->DamageAmount[weapon_id] * nrofhits);
	}
	else
	{
		onemissiledamageperupgrade = map.unit_settings.weaponbonusdamage[weapon_id];
		*upgdamage = (*upgradenr)*onemissiledamageperupgrade*nrofmissiles*nrofhits;
		return(map.unit_settings.weaponbasedamage[weapon_id] * nrofhits);
	}
}
//====================================
int GetWeaponDamage(SCUNIT SC_Unit, int playernr, int weapon_id)
{
	unsigned char weapon_upgr;
	int upgradenr, upgradedamage;
	weapon_upgr = alldattbl.weapons_dat->DamageUpgrade[weapon_id];
	upgradenr = GetUpgradeTree(&map, playernr, weapon_upgr);
	if (map.unit_settings.use_defaults[SC_Unit])
	{
		upgradedamage = upgradenr * alldattbl.weapons_dat->DamageBonus[weapon_id];
		return(alldattbl.weapons_dat->DamageAmount[weapon_id] + upgradedamage);
	}
	else
	{
		upgradedamage = upgradenr * map.unit_settings.weaponbonusdamage[weapon_id];
		return(map.unit_settings.weaponbasedamage[weapon_id] + upgradedamage);
	}
}
//====================================
int IsRangeWeapon(int weapon_id)
{
	if (alldattbl.weapons_dat->MaximumRange[weapon_id] >= WEAPONRANGEFROM)
		return(1);
	return(0);
}
//====================================
int UniqueWeapon(int weapon_id)
{
	if (alldattbl.weapons_dat->WeaponEffect[weapon_id] >= WEFFECT_UNIQUE)
		return(1);
	return(0);
}
//====================================
int GetInfoWeapon(SCUNIT SC_Unit, int playernr, int nrofhits, int weapon_id, int *upgradenr, int *upgdamage, char **finalmes)
{
	unsigned char weapon_upgr;
	int nrofmissiles;
	int onemissiledamageperupgrade;
	weapon_upgr = alldattbl.weapons_dat->DamageUpgrade[weapon_id];
	*upgradenr = GetUpgradeTree(&map, playernr, weapon_upgr);
	nrofmissiles = alldattbl.weapons_dat->DamageFactor[weapon_id];
	if (map.unit_settings.use_defaults[SC_Unit])
	{
		onemissiledamageperupgrade = alldattbl.weapons_dat->DamageBonus[weapon_id];
		if (weapon_id == WEAPONS_DAT_VALKYRIEHALOROCKET)
		{
			*upgdamage = (*upgradenr)*onemissiledamageperupgrade;
			*finalmes = alldattbl.stattxt_tbl->get_TBL_STR(STATTXT_TBL_PERROCKETTEXT);
			return(alldattbl.weapons_dat->DamageAmount[weapon_id]);
		}
		*upgdamage = (*upgradenr)*nrofmissiles*nrofhits*onemissiledamageperupgrade;
		return(alldattbl.weapons_dat->DamageAmount[weapon_id] * nrofhits*nrofmissiles);
	}
	else
	{
		onemissiledamageperupgrade = map.unit_settings.weaponbonusdamage[weapon_id];
		if (weapon_id == WEAPONS_DAT_VALKYRIEHALOROCKET)
		{
			*upgdamage = (*upgradenr)*onemissiledamageperupgrade;
			*finalmes = alldattbl.stattxt_tbl->get_TBL_STR(STATTXT_TBL_PERROCKETTEXT);
			return(map.unit_settings.weaponbasedamage[weapon_id]);
		}
		*upgdamage = (*upgradenr)*nrofmissiles*nrofhits*onemissiledamageperupgrade;
		return(map.unit_settings.weaponbasedamage[weapon_id] * nrofhits*nrofmissiles);
	}
}
//====================================
//return
//armor - simple armor without upgrades
//*upgradenr - upgrade number they have
//*upgaramor - armor that have upgrade technology
//so totalarmor=armor+*upgarmor
//
int GetArmor(OBJ *a, int armorupgnr, int *upgradenr, int *upgarmor)
{
	//NOTE:
	//made:ultralisk have 2 armors
	int upgr = 0, upgradearmor = 0, upgrade_id;
	if (armorupgnr != -1)
		upgradearmor += GetUpgradeTree(&map, a->playernr, armorupgnr) * 2;//ultralisk
	upgrade_id = alldattbl.units_dat->ArmorUpgrade[a->SC_Unit];
	if (upgrade_id < MAX_UPGRADES_ELEM)
	{
		upgr = GetUpgradeTree(&map, a->playernr, upgrade_id);
	}
	if (upgr)
		upgradearmor += upgr;
	if (upgarmor)
		*upgarmor = upgradearmor;
	if (upgradenr)
		*upgradenr = upgr;
	if (map.unit_settings.use_defaults[a->SC_Unit])
	{
		return(alldattbl.units_dat->Armor[a->SC_Unit]);
	}
	else
	{
		return(map.unit_settings.armorpoints[a->SC_Unit]);
	}
}
//====================================
//return
//armor - simple armor without upgrades
//*upgradenr - upgrade number they have
//so totalarmor=armor+*upgradenr
int GetShieldArmor(OBJ *a, int *upgradenr)
{
	int sharmor;
	sharmor = GetUpgradeTree(&map, a->playernr, UPGRADES_DAT_PLASMASHIELDICON);
	if (upgradenr)
		*upgradenr = sharmor;
	return(sharmor);
}
//====================================
char *getOBJname(SCUNIT SC_Unit)
{
	if (SC_Unit != SC_NOUNITNR)
	{
		if (map.unit_settings.use_defaults[SC_Unit])
		{
			return(alldattbl.stattxt_tbl->get_TBL_STR(SC_Unit));
		}
		else
		{
			if (map.unit_settings.unitname[SC_Unit])
				return(getmapSTR(&map, map.unit_settings.unitname[SC_Unit] - 1));
			else
				return(alldattbl.stattxt_tbl->get_TBL_STR(SC_Unit));
		}
	}
	return(NULL);
}
//=================================
void DestroyMarked(void)
{
	OBJ *o;
	Enumerate<OBJ *> EnumObj(&MaxObjects, objects, MaxObjects - 1);
	while( (o = EnumObj.GetBack()) )
//	for (int i = MaxObjects - 1;i >= 0;i--)
	{
//		o = objects[i];
		if (o->prop & VARDIESILENTLY)
		{
			ForceDestroyAllOBJsImages(o);
		}
		if (o->prop & VARMARKFORDESTROY)
		{
			destroyobj(o);
		}
	}
}
//=================================
int IsActiveUnitForAtack(OBJ *a,bool bunkerunits)
{
	if (a->modemove == MODEDIE)
		return(0);
	if (IsPickupUnit(a->SC_Unit))
		return(0);
	if (a->prop & VARINVINCIBILITY)
		return(0);
	if (a->prop & VARINTRANSPORT)
	{
		if (!bunkerunits)
			return(0);
	}
	else
	{
		if (a->prop & VARNOTHERE)
			return(0);
	}
	return(1);
}
//=================================
int IsActiveUnit(OBJ *a)
{
	int intransport = 0;
	if (a->modemove == MODEDIE)
		return(0);
	if (!IsReadyOBJ(a))
		return(0);
	if (a->prop & VARINTRANSPORT)
	{
		if (a->in_transport->SC_Unit != SC_BUNKEROBJ)
			intransport = 1;
	}
	if (intransport || (a->prop & (VARNOTHERE | VARINVINCIBILITY)))// || GetMageAtr(&a->atrobj,ATRSTASIS))
		return(0);
	return(1);
}
//=================================
int IsAtackerActiveUnit(OBJ *a)
{
	if (a->modemove == MODEDIE)
		return(0);
	if (!IsReadyOBJ(a))
		return(0);
	if ((a->prop & (VARINTRANSPORT | VARNOTHERE))) //|| GetMageAtr(&a->atrobj,ATRSTASIS))
		return(0);
	return(1);
}
//=================================
//search for nearest object to weapon flingy
//=================================
OBJ *SearchNewBounceOBJ(int x, int y, int playernr, OBJ *obj1, OBJ *obj2, int maxdist)
{
	int dist, mindist = 65535;
	OBJ *o;
	OBJ *finda = NULL;
	Enumerate<OBJ *> EnumObj(&MaxObjects, objects);
	while( (o = EnumObj.GetNext()) )
	{
		if (player_aliance(playernr, o->playernr) == ENEMYOBJ)
		{
			if (!OBJ_VAR_CHK(o, obj_notdetect, playernr))
				if (IsActiveUnit(o))
				{
					if (o == obj1 || o == obj2)
						continue;
					dist = GetDistanceTo(o, x, y);
					//				dist=(int)hypot(GetOBJx(o) - x,GetOBJy(o) - y);
					if (dist < maxdist)
						if (dist < mindist)
						{
							mindist = dist;
							finda = o;
						}
					//					return(o);
				}
		}
	}
	return(finda);
}
//=====================================================
int getchilds(struct OBJ *a)
{
	if (a->childs)
		return(a->childs->nrofchildunits);
	else
		return 0;
}
//=====================
int getfirstchildemptyslot(struct OBJ *a)
{
	if (a->childs)
		for (int i = 0;i < MAXCHILDS;i++)
			if (!a->childs->parentof[i])
				return i;
	return -1;
}
//=====================
int addchild(struct OBJ *a, struct OBJ *child)	//add child on parentof[] & pointers
{
	if (!a->childs)
	{
		a->childs = (struct CHILDS *) wmalloc(sizeof(CHILDS));
		memset(a->childs, 0, sizeof(CHILDS));
	}
	int i = getfirstchildemptyslot(a);
	if (i == -1)
		return -1;
	a->childs->parentof[i] = child;
	child->myparent = a;
	child->childnr = i;
	a->childs->nrofchildunits++;
	return 0;
}
//=====================
int delchild(struct OBJ *a, struct OBJ *child)	//del child from parentof[] & pointers
{
	//!note don't delete a->childs if non exist childs
	if (a->childs)
	{
		if (--a->childs->nrofchildunits < 0)
		{
			savelog("something wrong with my parent(2), me=", a->SC_Unit);
		}
		for (int i = 0;i < MAXCHILDS;i++)
			if (a->childs->parentof[i] == child)
			{
				a->childs->parentof[i] = NULL;
				child->myparent = NULL;
				return 0;
			}
	}
	else
		savelog("not have a->childs,something wrong with my parent(1), me=", a->SC_Unit);
	return -1;
}
//=====================
int objcmp(struct OBJ *a[], struct OBJ *b, int c)
{
	for (int i = 0;i < c;i++)
		if (a[i] == b)
			return i;
	return -1;
}
//==================================
int GetRangeObjSize(SCUNIT SC_Unit, int *sizex, int *sizey)
{
	int sizeret = 0, sx, sy;
	sx = POSX(GetUnitWidthAndHeight(SC_Unit, UNITDIM_WIDTH));
	sy = POSY(GetUnitWidthAndHeight(SC_Unit, UNITDIM_HEIGHT));
	if (!(sx & 1))
		sizeret |= 1;
	if (!(sy & 1))
		sizeret |= 2;
	if (sizex)
		*sizex = sx;
	if (sizey)
		*sizey = sy;
	//	  return(0);
	return(sizeret);
}
//=======================================
void swapOBJ(struct OBJ **a, struct OBJ **b)
{
	struct OBJ *c, *d;
	c = *a;
	d = *b;
	*a = d;
	*b = c;
}
//==================================
void AlignMAPXYCoordLU(int *l, int *u)
{
	if (*u < 0)
		*u = 0;
	if (*l < 0)
		*l = 0;
}
//=======================================
void AlignMAPXYCoordRD(int *r, int *d)
{
	if (*r >= MAXXMAP)
		*r = MAXXMAP - 1;
	if (*d >= MAXYMAP)
		*d = MAXYMAP - 1;
}
//=======================================
int IfCanClickOBJ(SCUNIT SC_Unit)
{
	unsigned char flingy_id;
	short sprites_id, images_id;
	if (IsSubUnit(SC_Unit))
		return(0);
	flingy_id = alldattbl.units_dat->flingy_id[SC_Unit];
	sprites_id = alldattbl.flingy_dat->sprites_id[flingy_id];
	images_id = alldattbl.sprites_dat->images_id[sprites_id];
	return(alldattbl.images_dat->Clickable[images_id]);
}
//=======================================
int GetSubUnitType(OBJ *a)
{
	if (a->subunit)
	{
		if (IsSubUnit(a->SC_Unit))
			return(SUBUNITTURRET);
		else
			return(SUBUNITBASE);
	}
	return(NOSUBUNIT);
}
//=======================================
bool IfCanTransport(SCUNIT SC_Unit, int playernr)
{
	if (GetSpaceProvided(SC_Unit))
	{
		if (SC_Unit == SC_OVERLORDOBJ)
			if (!GetUpgradeTree(&map, playernr, UPGRADES_DAT_OVERLORDTRANSP))
				return false;
		return true;
	}
	return false;
}
//=======================================
int CheckCoordinates(int newx256,int newy256,SCUNIT SC_Unit)
{
	if (newx256/256 + GetUnitWidthAndHeight(SC_Unit,UNITDIM_WIDTH)/2 < 0)
		return(0);
	if (newy256/256 + GetUnitWidthAndHeight(SC_Unit,UNITDIM_HEIGHT)/2 < 0)
		return(0);
	if (newx256/256 - GetUnitWidthAndHeight(SC_Unit,UNITDIM_WIDTH)/2 > MAXXMAP*32)
		return(0);
	if (newy256/256 - GetUnitWidthAndHeight(SC_Unit,UNITDIM_HEIGHT)/2 > MAXYMAP*32)
		return(0);
	return(1);
}
//================================
//fill properties of unit depend were he stand on map and what atributes he have
//==================================
//	 return x and y new position of build unit
//						  xxxxx
//						y aaaaa		x,y is form one coordinates of (a)
//						y a***a		if buid with (*) size 3,2
//						y a***a
//						y aaaaa
//
int getborderbuild(int mx,int my,int *x,int *y)
{
	int a = myrand(2);
	int b = myrand(2);
	if (a)
		*x = mx - 1;
	else
		*x = 0;
	if (b)
		*y = my - 1;
	else
		*y = 0;
	int kvadrant;
	if (a)
		if (b)
			kvadrant = 1;
		else
			kvadrant = 3;
		else
			if (b)
				kvadrant = 0;
			else
				kvadrant = 2;
	return(kvadrant);
}
//=====================================

