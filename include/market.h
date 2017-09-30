#if !defined(_MARKET_W)
#define _MARKET_W

#include "defs.h"
#include "vars.h"
#include "mylist.h"

#define CHECKRES_OK		0
#define CHECKRES_MIN		1
#define CHECKRES_GAS		2
#define CHECKRES_SUPPLY		3
#define CHECKRES_MANA		4

#define SMALLOVERLAY		0
#define MEDIUMOVERLAY		1
#define LARGEOVERLAY		2

#define DAMAGE_NOSHOWHIT	0x0
#define DAMAGE_SHOWHIT		0x1
#define DAMAGE_SPLASH		0x2

int  getusemanaformodemove(int modemove);
int  CheckForMana(OBJ *a,int needmana);
void DecrMana(OBJ *a,int needmana);
void IncrMana(OBJ *a,int needmana);
int  GetUnitMaxMana(SCUNIT SC_Unit,int playernr);
void SetUnitMana(OBJ *a,int mana);
void SetUnitPercentMana(OBJ *a,int persmana);

void DecrManaBITS(OBJ *a,int needmana);
void IncrManaBITS(OBJ *a,int needmana);
int  CheckResourcePlayer(int nplayer,int mcost,int gcost);
int  CheckResourcePlayerBITS(int nplayer,int mcostbits,int gcostbits);
int  CheckSupplyPlayer(int nplayer,SCUNIT SC_Unit);
void ChangeSupply(int nplayer,SCUNIT SC_Unit,int FACTOR);
void ChangeMaxSupply(int nplayer,SCUNIT SC_Unit,int FACTOR);
void ChangeResourcePlayer(int nplayer,int FACTOR,int mcost,int gcost);
void ChangeResourcePlayerBITS(int nplayer,int FACTOR,int mcostbits,int gcostbits);

int  GetInEgg(SCUNIT SC_Unit);
int  GetSupplyUnit(SCUNIT SC_Unit);
int  GetSupplyProvided(SCUNIT SC_Unit);
int  GetSpaceProvided(SCUNIT SC_Unit);
int  GetSpaceRequired(SCUNIT SC_Unit);
int  GetUnitSize(SCUNIT SC_Unit);
int  GetUnitOverlaySize(SCUNIT SC_Unit);

int  GetUnitBuildTime(SCUNIT SC_Unit);
void GetCostUnit(SCUNIT SC_Unit,int *mcost,int *gcost);
int  GetBuildUnitScore(SCUNIT SC_Unit);
int  GetDestroyUnitScore(SCUNIT SC_Unit);

void GetCostUpgr(int upgr_nr,int nplayer,int *mcost,int *gcost);
int  GetUpgradeResearchTime(int upgr_id,int upgrlevel);

void GetCostTech(int tech_id,int *mcost,int *gcost);
int  GetTechEnergyCost(int tech_id);
int  GetTechResearchTime(int upgr_id);
int  CheckForResource_typeid(OBJ *a,int nplayer,int type_id,int obj_id);

unsigned int GetUnitMaxHealth(SCUNIT SC_Unit);
void AddUnitHealth(struct OBJ *a,int addhealth);
void SetUnitHealth(struct OBJ *a,int health);
void SetUnitPercentHealth(OBJ *a,int pershealth);
int GetUnitMaxHitPoints(SCUNIT SC_Unit);
unsigned int GetUnitMaxShield(SCUNIT SC_Unit);
void AddUnitShield(struct OBJ *a,int addshield);
void SetUnitShield(struct OBJ *a,int shield);
void SetUnitPercentShield(OBJ *a,int persmana);
unsigned char GetUnitElevationLevel(SCUNIT SC_Unit);

int  LowLevelDamage(OBJ *atacker,OBJ *a,int weapon_id,int typedamage,int totaldamage,int directiondamage,int directorsplash);

//int  DoDamage(OBJ *atacker,OBJ *a,int xshot,int yshot,int weapon_id,int totaldamage,int directiondamage,mylist *dobjs,int damageby);
//int  CheckWeaponOnUnitType(OBJ *atacker,OBJ *a2,unsigned short t_flags);
//int  DoDamageToManyUnits(OBJ *atacker,OBJ *a,int xshot,int yshot,int weapon_id,int typedamage,int totaldamage,int directiondamage,mylist *dobjs,int damageby);
//int  DoDamageToOneUnit(OBJ *atacker,OBJ *a,int xshot,int yshot,int weapon_id,int typedamage,int totaldamage,int directorsplash,int directiondamage,int damageby);
//int  DoLineSplashDamage(OBJ *atacker,OBJ *destobj,int weapon_id,int totaldamage,int directiondamage,mylist *dobjs,int damageby);
int  GetUnitBoxWidth(SCUNIT SC_Unit);
int  GetUnitBoxHeight(SCUNIT SC_Unit);
int  GetAtackCoolDown(OBJ *atacker,int weapon_id);


#define DAMAGE_INDEPENDENT	0
#define DAMAGE_EXPLOSIVE	1
#define DAMAGE_CONCUSSIVE	2
#define DAMAGE_NORMAL		3
#define DAMAGE_IGNOREARMOR	4
#define DAMAGE_TYPES		5


#define UNITSIZE_INDEPENDENT	0
#define UNITSIZE_SMALL		1
#define UNITSIZE_MEDIUM		2
#define UNITSIZE_LARGE		3
#define UNITSIZE_TYPES		4



#endif

