#if !defined(_OBJS_W)
#define _OBJS_W

#include "mytime.h"

#define TANKSIEGESIDE		(12*8)
#define TANKNORMALSIDE		(28*8)
#define WAITTOHARVEST		15

#define DRONE_START_YPOS	7
#define AIRUNITS_START_YPOS	42

#define DECREMENTATRIBUTEMAGE	79
#define MAGEATRIBUTEDOTHINGS	5
#define DECREMENTLIFEMAGE	49
#define INCREMENTLIFE		159		
#define DECREMENTLIFE		60
#define INCREMENTSHIELD		119

#define NOUNITSTOUNLOAD	0
#define UNLOADEDUNIT	1
#define NOUNLOADEDUNIT	2

#define INTERCEPTORFLYRANGE		200
#define CRITTERRANGE			200
#define TIMETOWAITBEFOREMOVECRITTER 	100
#define MAXLARVAEPRODUCE		3
#define MINREACTIONDISTANCEFORATACKING	160


#define NOTYPEMOVEATBEGIN	0
#define TYPEMOVEATBEGIN		1

#define NOSHOWERROR		0x00
#define SHOWERRORTEXT		0x01
#define ATACKMOVEBIT		0x02

#define NOCHANGEXYCOORDS	0
#define NEWXYCOORDS		1

#define CHANGESC_UNIT_CONSTR			0
#define CHANGESC_UNIT_READY			1
#define CHANGESC_UNIT_READYWITHOUTMOVECHANGE	2

#define MAXWAIT_SEARCHATACK_TICK	(MAXGAMECYCLESPERSECOND*1)


extern	struct OBJ *ObjsRevealMap[];

extern	int MaxObjsRevealMap;

//==========================================
void destroyobj(struct OBJ *a);
OBJ *CreateArchon(OBJ *a,OBJ *a2);
struct OBJ *createobjfulllife(int x,int y,int nobj,int Pl);
struct OBJ *createobjmanwithlife(int x,int y,int nobj,int Pl,
				 int persshield,int perslife,int energypers,int supplyflag);
struct OBJ *createobjman(int x,int y,int nobj,int Pl,int typemoveatbegin,int pers_shield,int pers_life,int pers_enercy);
struct OBJ *createobjman(int x,int y,int nobj,int Pl,int typemoveatbegin);
struct OBJ *createobjman(int x,int y,int nobj,int Pl);
struct OBJ *createreschunk(OBJ *workerobj,int x,int y,unsigned char SC_Unit);
struct OBJ *createobjlowlevel(OBJ *workerobj,int x,int y,int SC_Unit,int playernr,
			      int readyatbegin,int persshield,int perslife,int energypers,unsigned short imagelo_id);
void GeyserDisactivate(OBJ *a,OBJ *geyser);

void deselectallexcludeone(struct OBJ *a1);
void deselectallexcludeonetypeobj(struct OBJ *a1);
void DetectIfAnyPylonOnSelected(void);
void deselectvars(void);
void deselectallobj(int nr_player);
struct OBJ *getfirstselectedobj(struct OBJ *a[]);
void controlbu(int typeunits,int typeb_or_u,int mode);
void grupingobj(int mode);
int foundonetypeobj(struct OBJ *a,struct OBJstruct *b);
struct OBJ *founduniqueobj(int x1,int y1);
void selectMAN(int x1,int y1,int x2,int y2,int mode);
struct OBJ* GetMaxRankOBJ(int maxobjs,OBJ *objs[]);
void changedeselectobj(struct OBJ *a,struct OBJ *aa);
int selectobjcount(void);
int selectobj(struct OBJ *a);
int deselectobj(struct OBJ *a);
int addselectobj(struct OBJ *a);
void createobjregen(void);
void allobjdecrmtimemage(void);
void ReleaseResource(OBJ *a);
void allobj_dieheal(void);
void allobjconstr(void);
int  moveobj(struct OBJ *a,struct OBJ *destobj,int mode,int x,int y,int showerrorflag,int shiftbit);
void CheckIfGotoTransport(OBJ *a,OBJ *destobj);
void CheckIfGotoBattery(OBJ *a,OBJ *destobj);

int accesstomove(struct OBJ *a,struct OBJstruct *b,int mode,int player);
int movefrommouse(struct OBJ *a);
int makemove(struct OBJ *a,struct OBJ *destobj,int locx,int locy,int mode,int player,int showerrorflag);
int selectedobjmove(struct OBJ *destobj,int locx,int locy,int mode,int player,int showerrorflag,int rightclick);
int GetDefaultModeForRightClick(OBJ *a,OBJ *destobj,int playernr);
int makestorona(struct OBJ *a,int dx,int dy);
int  corectstoronasveta(struct OBJ *a,struct OBJstruct *b);
int ifneedrotation(struct OBJ *a);
int rotationobj(struct OBJ *a,struct OBJstruct *b);
float mintormoz(struct OBJ *a,struct OBJstruct *b);
float mintormoz(struct OBJ *a,struct OBJstruct *b,int firststep);
int RazvorotNa180(struct OBJ *a,struct OBJstruct *b);
int RazvorotNa180NOW(struct OBJ *a,struct OBJstruct *b);
int RazvorotNaOBJ(struct OBJ *a,struct OBJ *a2);

void LoadObjInObj(struct OBJ *a,struct OBJstruct *b,struct OBJ *c,
		 int maxweighttoenter,int maxweight);
void UnLoadObjInObj(struct OBJ *c,int nrofunloadobj,int typeofnewxycoords,int x,int y,int silentlyflag);
void delADVobj(struct OBJ *a);
void dieobj_silently(OBJ *a);
void dieobj(struct OBJ *a);
void moveinstorona(float acc,struct OBJ *a);
void ReturnedToBase(struct OBJ *a,struct OBJstruct *b);
void RechargingChilds(struct OBJ *a,struct OBJstruct *b);
void coordcalc(struct OBJ *a,struct OBJstruct *b,int destx,int desty);
void SetBlinkOBJ(struct OBJ *destobj);
int  getcoordfrompatrate(int sx,int sy,int *retx,int *rety);
void ChangeObjXY(struct OBJ *a,int x,int y);
void ChangeObjXYDelta(struct OBJ *a,int xdelta,int ydelta);
void DeleteOldObjPointers(struct OBJ *a);
int  QueueDelObj(struct OBJ *obj);
void WakeUpChild(struct OBJ *a);
void WakeUpAllChilds(struct OBJ *a,struct OBJstruct *b);

void sortselectedunits(struct OBJ *a[],int maxcount);
void RemoveFromDestination(OBJ *a);
void AddRefreshedOBJ(OBJ *a);
void FinishMainUnitConstruct(OBJ *a);
void ContinueSCVConstruct(OBJ *a,OBJ *b);
void applyrescuableunits(void);
void setpropertiestounit(struct OBJ *a,int special_prop,int state_flags);
struct OBJ *createunitwithproperties(int xpos,int ypos,int unit_id,int playernr,
				int special_prop,int special_prop2,int state_flags,
				int hp,int sp,int ep,int resnr,int unitsinhanger);

int  SearchObjInArea(struct OBJ *a,struct XY *area);
void makeallblinking(void);
void MakeCoordsOfRes(struct OBJ *a,struct OBJstruct *b,int *resposx,int *resposy);

void SetAtackTick(OBJ *a);
OBJ *FindObjForAtack(OBJ *a,unsigned char weaponmask,
		    			unsigned char groundweapon,
					unsigned char airweapon,
					int (*specialcheckfunc)(int));
int  UnitIgnoreInvisibles(int SC_Unit);
void unitprepareforatack(OBJ *a,OBJ *a2);
void trapprepareforatack(OBJ *a,OBJ *a2);
struct OBJ* OneUnitSearchGoal(OBJ *a,int ignoremodes);
void SearchForAtacks(void);

void atackback(OBJ *firstatacker,OBJ *destobj,int directiondamage);
void moveaway(OBJ *atacker,OBJ *a,int directiondamage,int modeofmove,int addtoqueue);
int  tryaiaction(OBJ *a,OBJ *atacker,int directiondamage);
int  tryunitaction(OBJ *a,OBJ *atacker);
void TryToScanArea(OBJ *a,OBJ *scanobjarea);
void TellOtherUnitsAboutAtacking(OBJ *a,OBJ *atacker);
void LoadSubUnitLoFile(OBJ *a,OBJstruct *b);
void FixSubUnitCoords(OBJ *a,OBJstruct *b);
void ShowCircleAndBar(OBJ *a);
void CreateImageAndAddToList(OBJ *a,int x256,int y256,int readyatbegin,unsigned short imagelo_id);

int  ifselectedOBJbit(OBJ *a,int playernr);
void doselectedOBJbit(OBJ *a,int playernr,int flag);
int  makeinvisibles(struct OBJ *a);
void invisiblestick(void);
void SetHallucinationOBJ(OBJ *a);
int  IfUnitIsSelectable(OBJ *a);
void ChangeUnitSubUnitAndImagesAssociated(OBJ *a,int SC_NewUnit);
void ChangeUnitAndImagesAssociated(OBJ *a,int SC_NewUnit);
void SlotReturnResources(OBJ *a,int playernr,int slotnr);
void ReturnResources(unsigned char SC_Unit,int playernr,int factorreturn);
void initmoveaction(OBJ *a,OBJ *destobj,int mode,int startx,int starty,int x,int y);
int  CalcDestVars(OBJ *a,OBJ *destobj,int x1,int y1,int x2,int y2,unsigned char flingy_id);
int  moveaction(MAIN_IMG *img,int deltamove);
void ForceKartChanges(OBJ *a);
void ForceKartChanges(OBJ *a,int xpos,int ypos);
void CheckForKartChanges(OBJ *a,int xpos,int ypos);
void InitStopAfterMove(OBJ *a);

void PathFinding_InitType1(OBJ *a,int initx,int inity,int destx,int desty);
int  PathFinding_MovePortionType1(OBJ *a,MAIN_IMG *img,int deltamove);

void PathFinding_InitType2(OBJ *a,int initx,int inity,int destx,int desty,unsigned char flingy_id);
int  PathFinding_MovePortionType2(OBJ *a,MAIN_IMG *img,unsigned char flingy_id,int deltamove);

void AdditionalUnitsProceed(void);
void AllFlingyControlOBJMoving(void);
int  IfHaveDistanceForMove(OBJ *a,MAIN_IMG *img,OBJ *destobj,int mindistance);
void SetModeMove(OBJ *a,int mode);
int  ApplyNextModeMove(OBJ *a);
int  IfHaveNextModeMove(OBJ *a);

void InsertModeMove(OBJ *a,OBJ *destobj,int mode,int x,int y,int showmesflag);
void AddModeMove(OBJ *a,OBJ *destobj,int mode,int x,int y,int showmesflag);
void DelAllModeMoves(struct OBJ *a);
void DelOBJFromModeList(struct OBJ *a,struct OBJ *searchobj);
int  GetOBJNrInTransport(OBJ *c,OBJ *a);
void FreeOBJFromTransport(OBJ *c,OBJ *a,int objnr);
void ChangeSubUnitCoords(OBJ *turret,OBJ *base);
int TryToEnterNydus(OBJ *a,OBJ *nydus);
int RegenerateShield(OBJ *a);
int RegenerateLife(OBJ *a);


#define UnitDoAiAction(playernr) (map.pl_owner[playernr] == OWNER_COMPUTER)// || map.pl_owner[playernr] == OWNER_HUMAN)


#endif /*_OBJS_W*/

