
#if !defined(_LIST_W)
#define _LIST_W

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
struct OBJ *GetNearResource(struct OBJ *a,int res_nr,int *resvalability);

int GetDeltaAddonHorizontal(int SC_Unit);
int GetDeltaAddonVertical(int SC_Unit);

int  IsResourceContainer(int SC_Unit);
int  IsRegenerate(int SC_Unit);

int  IsAddon(int SC_Unit);
int  IsSpellCaster(int SC_Unit);
int  IsOrganic(int SC_Unit);
int  IsGroundUnit(int SC_Unit);
int  IsAirUnit(int SC_Unit);
int  IsHeroUnit(int SC_Unit);

int  IsOnSkyOBJ(OBJ *a);
void SetOnSkyOBJ(OBJ *a,int flag);

int  IsInvincibleUnit(int SC_Unit);
int  IsInvincibleOBJ(OBJ *a);
void SetInvincibleOBJ(OBJ *a,int flag);

int  IsMechanical(int SC_Unit);
int  IsRobotic(int SC_Unit);
#define UNITDIM_LEFT	0
#define UNITDIM_UP	1
#define UNITDIM_RIGHT	2
#define UNITDIM_DOWN	3

#define UNITDIM_WIDTH	0
#define UNITDIM_HEIGHT	1

int  GetUnitWidthAndHeight(int SC_Unit,int pos);
int  GetUnitDimensions(int SC_Unit,int pos);

int  GetTargetAcquisitionRange(int SC_Unit);
int  IsShieldEnable(int SC_Unit);
int  IsProducesUnits(int SC_Unit);
int  IsResource(struct OBJ *a,int typeresource);
int  IsPsiUnit(int SC_Unit);
int  IsResourceDepot(int SC_Unit);
int  RequiresPylonEnergy(int SC_Unit);
int  RequiresCreep(int SC_Unit);
int  IsPickupUnit(int SC_Unit);

int  IsWorkerUnit(int SC_Unit);
OBJ  *GetGEYSERfromMAP(int xmap,int ymap);
int  GetUnitRace(int SC_Unit);
int  GetBirthDirection(int SC_Unit);
int  IsCloakable(int SC_Unit);
int  IsBuild(int SC_Unit);
int  IsDetector(int SC_Unit);
int  IsPermanentCloak(int SC_Unit);

int  IsExpansionUnit(int SC_Unit);
int  IsDoodadState(int SC_Unit);

int  IsZergRace(int SC_Unit);
int  IsTerranRace(int SC_Unit);
int  IsProtossRace(int SC_Unit);

int  IsNoAnyGroupFlag(int SC_Unit);
int  IsGroupBuildFlag(int SC_Unit);
int  IsGroupMenFlag(int SC_Unit);
int  IsGroupFactoryFlag(int SC_Unit);
int  IsGroupNeutralFlag(int SC_Unit);
int  IsSubUnit(int SC_Unit);
int  IfHaveSubUnit(int SC_Unit);

int  GetUnitSublabel(int SC_Unit);


int  ChangeUnitIconProp(OBJ *a,int typeofprop);
void ChangeSC_Unit(struct OBJ *a,int playernr,int SC_Unit,int flagready);
int  IsOBJBurrowed(struct OBJ *a);
int  SetBurrowFlag(struct OBJ *a,int flag);
int  IsNonNeutralFlag(int SC_Unit);
int  IsOBJUnderConstruct(struct OBJ *a);
int  IsHallucination(struct OBJ *a);
int  IsReadyOBJ(struct OBJ *a);
int  IsUnitBaseBuild(struct OBJ *a);
int  IfAtackCoolDown(struct OBJ *a);
void ClearAtackCoolDown(struct OBJ *a);
void SetAtackCoolDownAction(struct OBJ *a,int x,int y,struct OBJ *destobj,int mode);
int  IsCloaked(OBJ *a);
int  SetCloakedFlag(OBJ *a,int flag);
char *getmapSTR(struct mapinfo *testmap,int nrofstr);
struct OBJ *SearchUnit(int playernr,int SC_Unit,int shieldp,int lifep,int energyp);
struct OBJ *SearchUnitFunc(int playernr,int (*funccheckunit)(int SC_Unit),int nearx,int neary,int dist);
OBJ *SearchObjs(int x,int y,int range,int *array,int arraydim);
int  FindSC_UnitType(OBJ *a,int player,int typeofbuild);
int  SearchForUnitMask(int playernr,int mask,int *x,int *y);
int  FindSC_UnitType(OBJ *a2,int player,int SC_Unit,int SC_AddonUnit);
OBJ *SearchForObjInXY(int x,int y,unsigned char *SC_Units,int nrofunits);


int  IfGeyserIsDepleted(OBJ *a);
int  GetUnitSoundID(unsigned char SC_Unit,unsigned char sndtype);
int  GetUnitSound(OBJ *a,unsigned char fromsndtype,unsigned char tosndtype);


void freeall(void);
void freeforrestart(void);

#endif /*_LIST_W*/

