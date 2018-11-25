
#if !defined(_LISTS_W)
#define _LISTS_W

#include "tbl.h"
#include "vars.h"
#include "defs.h"
#include "man.h"

#define UNITSOUND_READY			0
#define UNITSOUND_WHATFIRST		1
#define UNITSOUND_WHATLAST		2
#define UNITSOUND_PISSFIRST		3
#define UNITSOUND_PISSLAST		4
#define UNITSOUND_YESFIRST		5
#define UNITSOUND_YESLAST		6

void UnitsMAPRefresh(void);
void freeOBJS(void);
void freeOBJstructs(int withupgrades);
void freeMAGEstructs(void);
struct OBJ *SearchOBJforOBJ(struct OBJ *a,int modemage);
int	 controlifunitinunit(struct OBJ *a,struct OBJ *b);
int	 controldistancemode(int dx1,int dy1,int dx2,int dy2,int modemove);
int	 controldistanceunit(int dx1,int dy1,int dx2,int dy2,int range);
int	 ishealingobj(struct OBJ *a);
int	 isrechargebleobj(struct OBJ *a);
int	 isobjtobeinfest(struct OBJ *a);
struct OBJ *GetNearCenter(struct OBJ *a);
struct OBJ *GetNearResource(struct OBJ *a,SCUNIT SC_Unit,int *resvalability);

void SetInvincibleOBJ(OBJ *a,int flag);

#define UNITDIM_LEFT	0
#define UNITDIM_UP		1
#define UNITDIM_RIGHT	2
#define UNITDIM_DOWN	3

#define UNITDIM_WIDTH	0
#define UNITDIM_HEIGHT	1

int	 IsResource(struct OBJ *a,SCUNIT SC_Unit);

int	 IsWorkerUnit(SCUNIT SC_Unit);
OBJ	 *GetGEYSERfromMAP(int xmap,int ymap);
int	 GetUnitRace(SCUNIT SC_Unit);
int	 GetBirthDirection(SCUNIT SC_Unit);

void ChangeSC_Unit(struct OBJ *a,int playernr,SCUNIT SC_Unit,int flagready);
void SetBurrowFlag(struct OBJ *a,int flag);
int	 IsHallucination(struct OBJ *a);
int	 IsUnitBaseBuild(struct OBJ *a);
int	 IfAtackCoolDown(struct OBJ *a);
void ClearAtackCoolDown(struct OBJ *a);
void SetAtackCoolDownAction(struct OBJ *a,int x,int y,struct OBJ *destobj,int mode);
void SetCloakedFlag(OBJ *a,int flag);
struct OBJ *SearchUnit(int playernr,SCUNIT SC_Unit,int shieldp,int lifep,int energyp);
struct OBJ *SearchUnitFunc(int playernr,int (*funccheckunit)(SCUNIT SC_Unit),int nearx,int neary,int dist);
OBJ *SearchObjs(int x,int y,int range,int *array,int arraydim);
int	 FindSC_UnitType(OBJ *a,int player,int typeofbuild);
int	 SearchForUnitMask(int playernr,int mask,int *x,int *y);
int	 FindSC_UnitType(OBJ *a2,int player,SCUNIT SC_Unit,SCUNIT SC_AddonUnit);
OBJ *SearchForObjInXY(int x,int y,SCUNIT *SC_Units,int nrofunits);


int	 GetUnitSoundID(SCUNIT SC_Unit,unsigned char sndtype);
int	 GetUnitSound(OBJ *a,unsigned char fromsndtype,unsigned char tosndtype);


void freeall(void);
void freeforrestart(void);


inline int	IsOBJUnderConstruct(struct OBJ *a) { return(!(a->prop & VARREADY)); };
inline int	IsReadyOBJ(struct OBJ *a) { return( a->prop & VARREADY); };
inline void ChangeUnitIconProp(OBJ *a,int typeofprop) { a->typeofprop = typeofprop; };
inline int	IsCloaked(OBJ *a) { return(a->prop & UNITPROP_PREVCLOAK); };
inline int	IsBattleReactions(SCUNIT SC_Unit){ return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit] & SPECIAL_BATTLEREACTIONS)!=0); };
inline int	IsFullAutoAttack(SCUNIT SC_Unit){ return((alldattbl.units_dat->SpecialAbilityFlags[SC_Unit] & SPECIAL_FULLAUTOATTACK)!=0);};


#endif /*_LISTS_W*/

