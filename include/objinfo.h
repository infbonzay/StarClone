#if !defined(_OBJINFO_W)
#define _OBJINFO_W

#include "defs.h"
#include "man.h"
//=================================

#define ATACKUP		0
#define ATACKDOWN	1
int  GetMaxChildUnits(OBJ *a);
void AdditionalProperties(OBJ *a);
int  GetSpeed(OBJ *a,int speed);
int  GetDistances(int x,int y,int x2,int y2);
int  GetDistanceMinusSizes(int unit1,int unit2,int deltax,int deltay);
int  GetDistanceBetweenUnits256(OBJ *a,OBJ *a2);
int  GetDistanceTo256(OBJ *a,int x,int y);
int  GetDistanceTo(OBJ *a,int x256,int y256);

int  GetRangeWeaponInPixels(OBJ *atacker,int weapon_id,int playernr);
int  CheckWeaponRange(OBJ *atacker,OBJ *destobj,int weapon_id,int nplayer);
int  GetUnitSightRange(OBJ *a,OBJstruct *b);
int  GetAddManaFactor(int SC_Unit,int playernr);
int  GetWidthSummOfUnits(unsigned char SC_Unit1,unsigned char SC_Unit2,int deltax,int deltay);
int  GetDestUnitHalfSize(unsigned char SC_Unit,int deltax,int deltay);


int  IsRangeWeapon(int weapon_id);
int  UniqueWeapon(int weapon_id);

int  GetInfoWeapon(int SC_Unit,int nplayer,int nrofhits,int updown,int *upgradenr,int *upgdamage,char **finalmes);

int  GetAtackDamage(int SC_Unit,int nplayer,int nrofhits,int updown,int *upgradenr,int *upgdamage);
int  GetWeaponDamage(int SC_Unit,int playernr,int weapon_id);

int  GetShieldArmor(OBJ *a,OBJstruct *b,int updown,int *upgradenr);
int  GetArmor(OBJ *a,int armorupgnr,int *upgradenr,int *upgarmor);
int  GetShieldArmor(OBJ *a,OBJstruct *b,int *upgradenr);
int  GetRotatePos(OBJ *a);
int  GetDestRotatePos(OBJ *a);
void SetRotatePos(OBJ *a,float storonasveta);
int  ModifyRotatePos(OBJ *a,int maxsides,float factor,int deststorona);
void SetDestRotatePos(OBJ *a,int deststoronasveta);
int  GetCloakRange(OBJ *a,OBJstruct *b);

int  GetNrSomeObjects(OBJ *a,int *newunit,char **countmes);

void SetInertionObj(OBJ *a,int setclear);
int  IfInertionObj(OBJ *a);
char *getOBJname(unsigned char SC_Unit);
void MarkForDestroy(OBJ *a);
void DestroyMarked(void);
int  GetKvadrant(int storonasveta);
int  IsActiveUnit(OBJ *a);
int  IsAtackerActiveUnit(OBJ *a);
int  IsActiveUnitForAtack(OBJ *a);

#define NOATACKER	0
#define GROUNDATACKER	1
#define AIRATACKER	2

#define NOSUBUNIT	0
#define SUBUNITTURRET	1
#define SUBUNITBASE	2

int  GetSubUnitType(OBJ *a);
int  UnitWeaponMask(int SC_Unit);



unsigned char GetNewWeaponType(OBJ *atacker,OBJ *destobj,int newatackerunitusedweapon);


OBJ *SearchNewBounceOBJ(int x,int y,int nplayer,OBJ *obj1,OBJ *obj2,int maxdist);
int getchilds(struct OBJ *a);
int getfirstchildemptyslot(struct OBJ *a);
int addchild(struct OBJ *a,struct OBJ *child);
int delchild(struct OBJ *a,struct OBJ *child);
int objcmp(struct OBJ *a[],struct OBJ *b,int c);
int GetRangeObjSize(unsigned char SC_Unit,int *x,int *y);
void swapOBJ(struct OBJ **a,struct OBJ **b);
int  GetDeltaDirection(OBJ *a);
void AlignMAPXYCoordLU(int *l,int *u);
void AlignMAPXYCoordRD(int *r,int *d);
int  IfCanClickOBJ(unsigned char SC_Unit);
void SetTriggeredUnitState(struct OBJ *a,int state);
int  GetTriggeredUnitState(struct OBJ *a);

//=====================================================


#endif /* _OBJINFO_W*/
