#ifndef _SC_OBJINFO_H

#define _SC_OBJINFO_H

#include "vars.h"
#include "defs.h"
#include "man.h"
//=================================

void AdditionalProperties(OBJ *a);
int	 GetSpeed(OBJ *a,int speed);
int	 GetDistances(int x,int y,int x2,int y2);
int	 GetDistanceMinusSizes(int unit1,int unit2,int deltax,int deltay);
int	 GetDistanceBetweenUnits256(OBJ *a,OBJ *a2);
int	 GetDistanceTo256(OBJ *a,int x,int y);
int	 GetDistanceTo(OBJ *a,int x256,int y256);

int	 GetRangeWeaponInPixels(OBJ *atacker,int weapon_id,int playernr);
int	 CheckWeaponRange(OBJ *atacker,OBJ *destobj,int weapon_id,int nplayer);
int	 GetUnitSightRange(OBJ *a,OBJstruct *b);
int	 GetAddManaFactor(SCUNIT SC_Unit,int playernr);
int	 GetWidthSummOfUnits(SCUNIT SC_Unit1,SCUNIT SC_Unit2,int deltax,int deltay);
int	 GetDestUnitHalfSize(SCUNIT SC_Unit,int deltax,int deltay);


int	 IsRangeWeapon(int weapon_id);
int	 UniqueWeapon(int weapon_id);

int	 GetInfoWeapon(SCUNIT SC_Unit,int nplayer,int nrofhits,int updown,int *upgradenr,int *upgdamage,char **finalmes);

int	 GetAtackDamage(SCUNIT SC_Unit,int nplayer,int nrofhits,int updown,int *upgradenr,int *upgdamage);
int	 GetWeaponDamage(SCUNIT SC_Unit,int playernr,int weapon_id);

int	 GetArmor(OBJ *a,int armorupgnr,int *upgradenr,int *upgarmor);
int	 GetShieldArmor(OBJ *a,int *upgradenr);

int	 GetNrSomeObjects(OBJ *a,int *newunit,char **countmes);

void SetInertionObj(OBJ *a,int setclear);
int	 IfInertionObj(OBJ *a);
char *getOBJname(SCUNIT SC_Unit);
void DestroyMarked(void);
int	 GetKvadrant(int storonasveta);
int	 IsActiveUnit(OBJ *a);
int	 IsAtackerActiveUnit(OBJ *a);
int	 IsActiveUnitForAtack(OBJ *a, bool bunkerunits);

int	 INBAY_GetCreatedUnits(OBJ *a);
int	 INBAY_GetMaxUnitsToBeCreated(OBJ *a);

#define NOATACKER		0
#define GROUNDATACKER	1
#define AIRATACKER		2

#define NOSUBUNIT		0
#define SUBUNITTURRET	1
#define SUBUNITBASE		2

int	 GetSubUnitType(OBJ *a);
int	 UnitWeaponMask(SCUNIT SC_Unit);



unsigned char GetNewWeaponType(OBJ *atacker,OBJ *destobj,int newatackerunitusedweapon);


OBJ  *SearchNewBounceOBJ(int x,int y,int nplayer,OBJ *obj1,OBJ *obj2,int maxdist);
int  getchilds(struct OBJ *a);
int  getfirstchildemptyslot(struct OBJ *a);
int  addchild(struct OBJ *a,struct OBJ *child);
int  delchild(struct OBJ *a,struct OBJ *child);
int  objcmp(struct OBJ *a[],struct OBJ *b,int c);
int  GetRangeObjSize(SCUNIT SC_Unit,int *x,int *y);
void swapOBJ(struct OBJ **a,struct OBJ **b);
int	 GetDeltaDirection(OBJ *a);
void AlignMAPXYCoordLU(int *l,int *u);
void AlignMAPXYCoordRD(int *r,int *d);
int	 IfCanClickOBJ(SCUNIT SC_Unit);
int  CheckCoordinates(int newx256,int newy256,SCUNIT SC_Unit);
int  getborderbuild(int mx,int my,int *x,int *y);
bool IfCanTransport(SCUNIT SC_Unit, int playernr);

inline int GetUnitUnknownFlags(SCUNIT SC_Unit) { return(alldattbl.units_dat->Unknown1[SC_Unit]); };
inline int GetUnitSublabel(SCUNIT SC_Unit) { return(alldattbl.units_dat->Sublabel[SC_Unit]); };
inline int GetTargetAcquisitionRange(SCUNIT SC_Unit) { return(alldattbl.units_dat->TargetAcquisitionRange[SC_Unit]); };
inline int IsShieldEnable(SCUNIT SC_Unit) { return(alldattbl.units_dat->ShieldEnable[SC_Unit]); };
inline int IsProducesUnits(SCUNIT SC_Unit) { return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_PRODUCESUNITS)!=0); };
inline int IsBuild(SCUNIT SC_Unit) { return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_ISBUILD)!=0); };
inline int IsCloakable(SCUNIT SC_Unit) { return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_CLOAKABLE)!=0); };
inline int IsOrganic(SCUNIT SC_Unit) { return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_ORGANICUNIT)!=0); };
inline int IsGroundUnit(SCUNIT SC_Unit) { return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_FLY)==0); };
inline int IsOnSkyOBJ(OBJ *a) { return((a->prop & VARONSKY) !=0); };
inline void SetOnSkyOBJ(OBJ *a,int flag) { if (flag) a->prop |= VARONSKY; else	a->prop &= ~VARONSKY; };
inline int IsAirUnit(SCUNIT SC_Unit) { return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_FLY)!=0); };
inline int IsHeroUnit(SCUNIT SC_Unit) { return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_HERO)!=0); };
inline int IsRegenerate(SCUNIT SC_Unit) { return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_REGENERATE)!=0); };
inline int IsMechanical(SCUNIT SC_Unit) { return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_MECHANICALUNIT)!=0); };
inline int IsRobotic(SCUNIT SC_Unit) { return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_ROBOTICUNIT)!=0); };
inline int IsResourceContainer(SCUNIT SC_Unit) { return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_RESOURCECONTAINER)!=0); };
inline int IsResourceDepot(SCUNIT SC_Unit) { return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_RESOURCEDEPOT)!=0); };
inline int RequiresPylonEnergy(SCUNIT SC_Unit) { return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_REQUIRESPYLONAREA)!=0); };
inline int RequiresCreep(SCUNIT SC_Unit) { return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_REQUIRESCREEP)!=0); };
inline int IsPickupUnit(SCUNIT SC_Unit) { return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_PICKUPITEM)!=0); };
inline int IsNonNeutralFlag(SCUNIT SC_Unit) { return((alldattbl.units_dat->StarEditAvailabilityFlags[SC_Unit]&STAREDIT_AF_NONNEUTRAL)!=0); };
inline int IsExpansionUnit(SCUNIT SC_Unit) { return((alldattbl.units_dat->StarEditAvailabilityFlags[SC_Unit]&STAREDIT_AF_EXPANSIONUNIT)!=0); };
inline int IsDoodadState(SCUNIT SC_Unit) { return((alldattbl.units_dat->StarEditAvailabilityFlags[SC_Unit]&STAREDIT_AF_DOODADSTATE)!=0); };
inline int IsZergRace(SCUNIT SC_Unit) { return((alldattbl.units_dat->StarEditGroupFlags[SC_Unit]&STAREDIT_GF_ZERGRACE)!=0); };
inline int IsTerranRace(SCUNIT SC_Unit) { return((alldattbl.units_dat->StarEditGroupFlags[SC_Unit]&STAREDIT_GF_TERRANRACE)!=0); };
inline int IsProtossRace(SCUNIT SC_Unit) { return((alldattbl.units_dat->StarEditGroupFlags[SC_Unit]&STAREDIT_GF_PROTOSSRACE)!=0); };
inline int IsNoAnyGroupFlag(SCUNIT SC_Unit) { return((alldattbl.units_dat->StarEditGroupFlags[SC_Unit]&STAREDIT_GF_NOANYGROUPFLAG)==0); };
inline int IsGroupBuildFlag(SCUNIT SC_Unit){ return((alldattbl.units_dat->StarEditGroupFlags[SC_Unit]&STAREDIT_GF_BUILDING)!=0); };
inline int IsGroupMenFlag(SCUNIT SC_Unit) { return((alldattbl.units_dat->StarEditGroupFlags[SC_Unit]&STAREDIT_GF_MEN)!=0); };
inline int IsGroupFactoryFlag(SCUNIT SC_Unit) { return((alldattbl.units_dat->StarEditGroupFlags[SC_Unit]&STAREDIT_GF_FACTORY)!=0); };
inline int IsGroupNeutralFlag(SCUNIT SC_Unit) { return((alldattbl.units_dat->StarEditGroupFlags[SC_Unit]&STAREDIT_GF_NEUTRAL)!=0); };
inline int IsAddon(SCUNIT SC_Unit) { return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_ADDON)!=0); };
inline int IsSpellCaster(SCUNIT SC_Unit) { return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_SPELLCASTER)!=0); };
inline int IsDetector(SCUNIT SC_Unit) { return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_DETECTOR)!=0); };
inline int IsInvincibleUnit(SCUNIT SC_Unit) { return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_INVINCIBLE)!=0); };
inline int IsPermanentCloak(SCUNIT SC_Unit) { return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_PERMANENTCLOAK)!=0); };
inline int IsSubUnit(SCUNIT SC_Unit) { return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_SUBUNIT)!=0); };
inline int IfHaveSubUnit(SCUNIT SC_Unit) { return(alldattbl.units_dat->Subunit1[SC_Unit] < MAX_UNITS_ELEM); };
inline int GetDeltaAddonHorizontal(SCUNIT SC_Unit) { return(alldattbl.units_dat->AddonHorizontalVertical[SC_Unit - 106][0]); };
inline int GetDeltaAddonVertical(SCUNIT SC_Unit) { return(alldattbl.units_dat->AddonHorizontalVertical[SC_Unit - 106][1]); };
inline int IfGeyserIsDepleted(OBJ *a) { return(a->data.resource.resource_count == 0); };
inline int IsOBJBurrowed(OBJ *a) { return(a->prop & VARBURROW); };
inline int GetSupplyProvided(SCUNIT SC_Unit) { return(alldattbl.units_dat->SupplyProvided[SC_Unit]); };
inline int GetSpaceProvided(SCUNIT SC_Unit) { return(alldattbl.units_dat->SpaceProvided[SC_Unit]); };
inline int GetSpaceRequired(SCUNIT SC_Unit) { return(alldattbl.units_dat->SpaceRequired[SC_Unit]); };
inline int GetUnitSize(SCUNIT SC_Unit) { return(alldattbl.units_dat->UnitSize[SC_Unit]); };
inline int IsPsiUnit(SCUNIT SC_Unit) { return(GetSupplyProvided(SC_Unit)!=0); };
inline int IsInvincibleOBJ(OBJ *a) { return(a->prop&VARINVINCIBILITY); };
inline int GetUnitWidthAndHeight(SCUNIT SC_Unit,int pos) { return(alldattbl.units_dat->StarEditPlacementBoxWidthAndHeight[SC_Unit][pos]); };
inline int GetUnitDimensions(SCUNIT SC_Unit,int pos) { return(alldattbl.units_dat->UnitSizeLeftUpRightDown[SC_Unit][pos]); };
inline int GetInEgg(SCUNIT SC_Unit) { return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit]&SPECIAL_TWOINONEEGG)!=0); };
inline int GetSupplyUnit(SCUNIT SC_Unit) { return(alldattbl.units_dat->SupplyRequired[SC_Unit]); };
inline int GetUnitBoxWidth(int SC_Unit) { return(alldattbl.units_dat->StarEditPlacementBoxWidthAndHeight[SC_Unit][0]); };
inline int GetUnitBoxHeight(int SC_Unit) { return(alldattbl.units_dat->StarEditPlacementBoxWidthAndHeight[SC_Unit][1]); };
inline void SetUnitHealth(OBJ *a,int health) { a->health=health; };
inline void SetUnitShield(OBJ *a,int shield) { a->shield=shield; };
inline int GetBuildUnitScore(SCUNIT SC_Unit) { return(alldattbl.units_dat->BuildScore[SC_Unit]); };
inline int GetDestroyUnitScore(SCUNIT SC_Unit) { return(alldattbl.units_dat->DestroyScore[SC_Unit]); };
inline unsigned char GetUnitElevationLevel(SCUNIT SC_Unit) { return(alldattbl.units_dat->ElevationLevel[SC_Unit]); };
inline void SetTriggeredOBJState(OBJ *a,int state) { a->triggerstate = state; };
inline int	GetTriggeredOBJState(OBJ *a) { return(a->triggerstate); };
inline int	GetUnitCloakRange(OBJstruct *b) { return(b->cloakrange); };
inline void OBJMarkForDestroy(OBJ *a) { a->prop |= (VARNOTHERE|VARNOTWORK|VARNOTSEEPROP|VARCANTSELECT|VARMARKFORDESTROY); };
//=================================

//=====================================================


#endif /* _OBJINFO_W*/
