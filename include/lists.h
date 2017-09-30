
#if !defined(_LIST_W)
#define _LIST_W

#include "man.h"
#define UNITSOUND_READY		0
#define UNITSOUND_WHATFIRST	1
#define UNITSOUND_WHATLAST	2
#define UNITSOUND_PISSFIRST	3
#define UNITSOUND_PISSLAST	4
#define UNITSOUND_YESFIRST	5
#define UNITSOUND_YESLAST	6
/*
struct OBJ *foundobjman(int x,int y);
struct OBJ *foundobjbuild(struct OBJ *a);
void delobjxykart(struct OBJ *a);
void putobjxykart(struct OBJ *a);
*/
void UnitsMAPRefresh(void);
void addobjlist(struct OBJ *a);
void delobjlist(struct OBJ *a);
void freeOBJS(void);
void freeOBJstructs(int withupgrades);
void freeMAGEstructs(void);
struct OBJ *SearchOBJforOBJ(struct OBJ *a,int modemage);
int  controlifunitinunit(struct OBJ *a,struct OBJ *b);
int  controldistancemode(int dx1,int dy1,int dx2,int dy2,int modemove);
int  controldistanceunit(int dx1,int dy1,int dx2,int dy2,int range);
int  ishealingobj(struct OBJ *a);
int  isrechargebleobj(struct OBJ *a);
int  isobjtobeinfest(struct OBJ *a);
struct OBJ *GetNearCenter(struct OBJ *a);
struct OBJ *GetNearResource(struct OBJ *a,SCUNIT SC_Unit,int *resvalability);

int GetDeltaAddonHorizontal(SCUNIT SC_Unit);
int GetDeltaAddonVertical(SCUNIT SC_Unit);

int  IsResourceContainer(SCUNIT SC_Unit);
int  IsRegenerate(SCUNIT SC_Unit);

int  IsAddon(SCUNIT SC_Unit);
int  IsSpellCaster(SCUNIT SC_Unit);
int  IsOrganic(SCUNIT SC_Unit);
int  IsBattleReactions(SCUNIT SC_Unit);
int  IsGroundUnit(SCUNIT SC_Unit);
int  IsAirUnit(SCUNIT SC_Unit);
int  IsHeroUnit(SCUNIT SC_Unit);

int  IsOnSkyOBJ(OBJ *a);
void SetOnSkyOBJ(OBJ *a,int flag);

int  IsInvincibleUnit(SCUNIT SC_Unit);
int  IsInvincibleOBJ(OBJ *a);
void SetInvincibleOBJ(OBJ *a,int flag);

int  IsMechanical(SCUNIT SC_Unit);
int  IsRobotic(SCUNIT SC_Unit);
#define UNITDIM_LEFT	0
#define UNITDIM_UP	1
#define UNITDIM_RIGHT	2
#define UNITDIM_DOWN	3

#define UNITDIM_WIDTH	0
#define UNITDIM_HEIGHT	1

int  GetUnitWidthAndHeight(SCUNIT SC_Unit,int pos);
int  GetUnitDimensions(SCUNIT SC_Unit,int pos);

int  GetTargetAcquisitionRange(SCUNIT SC_Unit);
int  IsShieldEnable(SCUNIT SC_Unit);
int  IsProducesUnits(SCUNIT SC_Unit);
int  IsResource(struct OBJ *a,SCUNIT SC_Unit);
int  IsPsiUnit(SCUNIT SC_Unit);
int  IsResourceDepot(SCUNIT SC_Unit);
int  RequiresPylonEnergy(SCUNIT SC_Unit);
int  RequiresCreep(SCUNIT SC_Unit);
int  IsPickupUnit(SCUNIT SC_Unit);

int  IsWorkerUnit(SCUNIT SC_Unit);
OBJ  *GetGEYSERfromMAP(int xmap,int ymap);
int  GetUnitRace(SCUNIT SC_Unit);
int  GetBirthDirection(SCUNIT SC_Unit);
int  IsCloakable(SCUNIT SC_Unit);
int  IsBuild(SCUNIT SC_Unit);
int  IsDetector(SCUNIT SC_Unit);
int  IsPermanentCloak(SCUNIT SC_Unit);

int  IsExpansionUnit(SCUNIT SC_Unit);
int  IsDoodadState(SCUNIT SC_Unit);

int  IsZergRace(SCUNIT SC_Unit);
int  IsTerranRace(SCUNIT SC_Unit);
int  IsProtossRace(SCUNIT SC_Unit);

int  IsNoAnyGroupFlag(SCUNIT SC_Unit);
int  IsGroupBuildFlag(SCUNIT SC_Unit);
int  IsGroupMenFlag(SCUNIT SC_Unit);
int  IsGroupFactoryFlag(SCUNIT SC_Unit);
int  IsGroupNeutralFlag(SCUNIT SC_Unit);
int  IsSubUnit(SCUNIT SC_Unit);
int  IfHaveSubUnit(SCUNIT SC_Unit);

int  GetUnitSublabel(SCUNIT SC_Unit);


void ChangeSC_Unit(struct OBJ *a,int playernr,SCUNIT SC_Unit,int flagready);
int  IsOBJBurrowed(struct OBJ *a);
int  SetBurrowFlag(struct OBJ *a,int flag);
int  IsNonNeutralFlag(SCUNIT SC_Unit);
int  IsHallucination(struct OBJ *a);
int  IsUnitBaseBuild(struct OBJ *a);
int  IfAtackCoolDown(struct OBJ *a);
void ClearAtackCoolDown(struct OBJ *a);
void SetAtackCoolDownAction(struct OBJ *a,int x,int y,struct OBJ *destobj,int mode);
int  SetCloakedFlag(OBJ *a,int flag);
char *getmapSTR(struct mapinfo *testmap,int nrofstr);
struct OBJ *SearchUnit(int playernr,SCUNIT SC_Unit,int shieldp,int lifep,int energyp);
struct OBJ *SearchUnitFunc(int playernr,int (*funccheckunit)(SCUNIT SC_Unit),int nearx,int neary,int dist);
OBJ *SearchObjs(int x,int y,int range,int *array,int arraydim);
int  FindSC_UnitType(OBJ *a,int player,int typeofbuild);
int  SearchForUnitMask(int playernr,int mask,int *x,int *y);
int  FindSC_UnitType(OBJ *a2,int player,SCUNIT SC_Unit,SCUNIT SC_AddonUnit);
OBJ *SearchForObjInXY(int x,int y,SCUNIT *SC_Units,int nrofunits);


int  IfGeyserIsDepleted(OBJ *a);
int  GetUnitSoundID(SCUNIT SC_Unit,unsigned char sndtype);
int  GetUnitSound(OBJ *a,unsigned char fromsndtype,unsigned char tosndtype);


void freeall(void);
void freeforrestart(void);


inline int  IsOBJUnderConstruct(struct OBJ *a) { return(!(a->prop & VARREADY)); };
inline int  IsReadyOBJ(struct OBJ *a) { return( a->prop & VARREADY); };
inline int  ChangeUnitIconProp(OBJ *a,int typeofprop) { a->typeofprop = typeofprop; };
inline int  IsCloaked(OBJ *a) { return(a->prop & UNITPROP_PREVCLOAK); };
inline int  IsBattleReactions(SCUNIT SC_Unit){ return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit] & SPECIAL_BATTLEREACTIONS)!=0); };
inline int  IsFullAutoAttack(SCUNIT SC_Unit){ return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit] & SPECIAL_FULLAUTOATTACK)!=0);};


#endif /*_LIST_W*/

