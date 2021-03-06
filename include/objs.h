#ifndef _SC_OBJS_H

#define _SC_OBJS_H

#include "images.h"
#include "defs.h"
#include "man.h"
#include "vars.h"

#define TANKSIEGESIDE			(12*8)
#define TANKNORMALSIDE			(28*8)
#define WAITTOHARVEST			15

#define DRONE_START_YPOS		7
#define AIRUNITS_START_YPOS		42

#define DECREMENTATRIBUTEMAGE	79
#define MAGEATRIBUTEDOTHINGS	5
#define DECREMENTLIFEMAGE		49
#define INCREMENTLIFE			159
#define DECREMENTLIFE			60
#define INCREMENTSHIELD			119

#define NOUNITSTOUNLOAD 0
#define UNLOADEDUNIT	1
#define NOUNLOADEDUNIT	2

#define INTERCEPTORFLYRANGE				200
#define CRITTERRANGE					200
#define TIMETOWAITBEFOREMOVECRITTER		100
#define MAXLARVAEPRODUCE				3
#define MINREACTIONDISTANCEFORATACKING	160


#define NOTYPEMOVEATBEGIN		0
#define TYPEMOVEATBEGIN			1

#define NOSHOWERROR				0x00
#define SHIFTBIT				0x01
#define SHOWERRORTEXT			0x02
#define ATACKMOVEBIT			0x04
#define XYNOTCOORDS				0x08
#define PLAYERDOMOVE			0x10

#define NOCHANGEXYCOORDS		0
#define NEWXYCOORDS				1

#define CHANGESC_UNIT_CONSTR					0
#define CHANGESC_UNIT_READY						1
#define CHANGESC_UNIT_READYWITHOUTMOVECHANGE	2

#define MAXWAIT_SEARCHATACK_TICK		(MAXGAMECYCLESPERSECOND*1)


//==========================================
void destroyobj(struct OBJ *a);
OBJ *CreateArchon(OBJ *a,OBJ *a2);
struct OBJ *createobjfulllife(int x,int y,SCUNIT SC_Unit,int Pl);
struct OBJ *createobjmanwithlife(int x,int y,SCUNIT SC_Unit,int Pl,
								 int persshield,int perslife,int energypers,int supplyflag);
struct OBJ *createobjman(int x,int y,SCUNIT SC_Unit,int Pl,int typemoveatbegin,int pers_shield,int pers_life,int pers_enercy);
struct OBJ *createobjman(int x,int y,SCUNIT SC_Unit,int Pl,int typemoveatbegin);
struct OBJ *createobjman(int x,int y,SCUNIT SC_Unit,int Pl);
struct OBJ *createreschunk(OBJ *workerobj,int x,int y,SCUNIT SC_Unit);
struct OBJ *createobjlowlevel(OBJ *workerobj,int x,int y,SCUNIT SC_Unit,int playernr,
							  int readyatbegin,int persshield,int perslife,int energypers,unsigned short imagelo_id);
void GeyserDisactivate(OBJ *a,OBJ *geyser);

void deselectallexcludeone(struct OBJ *a1);
void deselectallexcludeonetypeobj(struct OBJ *a1);
void DetectIfAnyPylonOnSelected(void);
void deselectvars(void);
void deselectallobj(int nr_player);
void controlbu(int typeunits,int typeb_or_u,int mode);
void grupingobj(int mode);
int foundonetypeobj(struct OBJ *a,struct OBJstruct *b);
OBJ *founduniqueobj(int x1,int y1);
void selectMAN(int x1,int y1,int x2,int y2,int mode);
OBJ* GetMaxRankOBJ(SelectionObjs *obj);
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
int	 moveobj(struct OBJ *a,struct OBJ *destobj,int mode,int modemoveflags);
int	 moveobj(struct OBJ *a,struct OBJ *destobj,int mode,int x,int y,int modemoveflags);
void CheckIfGotoTransport(OBJ *a,OBJ *destobj);
void CheckIfGotoBattery(OBJ *a,OBJ *destobj);

int accesstomove(struct OBJ *a,struct OBJstruct *b,int mode,int player);
int movefrommouse(struct OBJ *a);
int makemove(struct OBJ *a,struct OBJ *destobj,int locx,int locy,int mode,int player,int modemoveflags);
int selectedobjmove(struct OBJ *destobj,int locx,int locy,int mode,int player,int modemoveflags,int rightclick);
int GetDefaultModeForRightClick(OBJ *a,OBJ *destobj,int playernr);
int RazvorotNa180(struct OBJ *a,struct OBJstruct *b);
int RazvorotNa180NOW(struct OBJ *a,struct OBJstruct *b);
int RazvorotNaOBJ(struct OBJ *a,struct OBJ *a2);

void LoadObjInObj(OBJ *a,OBJ *c);
void UnLoadObjInObj(OBJ *c,int nrofunloadobj,int typeofnewxycoords,int x,int y,int silentlyflag);
void delADVobj(struct OBJ *a);
void dieobj_silently(OBJ *a);
void dieobj(struct OBJ *a);
void moveinstorona(float acc,struct OBJ *a);
void ReturnedToBase(struct OBJ *a);
void SetBlinkOBJ(struct OBJ *destobj);
int	 getcoordfrompatrate(int sx,int sy,int *retx,int *rety);
void ChangeObjXY(struct OBJ *a,int x,int y);
void ChangeObjXYDelta(struct OBJ *a,int xdelta,int ydelta);
void DeleteOldObjPointers(struct OBJ *a);
int	 QueueDelObj(struct OBJ *obj);
void WakeUpOneInterceptor(OBJ *myparent,OBJ *a,OBJ *destobj,int childnr);
void WakeUpOneInterceptor(OBJ *a,OBJ *destobj);
void WakeUpInterceptors(OBJ *a,OBJ *destobj);

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

int	 SearchObjInArea(struct OBJ *a,struct XY *area);
void makeallblinking(void);
void SetAtackTick(OBJ *a);
OBJ *FindObjForAtack(OBJ *a,unsigned char weaponmask,
										unsigned char groundweapon,
										unsigned char airweapon,
										int (*specialcheckfunc)(SCUNIT));
int	 UnitIgnoreInvisibles(SCUNIT SC_Unit);
void unitprepareforatack(OBJ *a,OBJ *a2);
void trapprepareforatack(OBJ *a,OBJ *a2);
struct OBJ* OneUnitSearchGoal(OBJ *a,int ignoremodes,int facedirectionatackmode);
void SearchForAtacks(void);

void atackback(OBJ *firstatacker,OBJ *destobj,int directiondamage);
void moveaway(OBJ *atacker,OBJ *a,int directiondamage,int modeofmove,int addtoqueue);
int	 tryaiaction(OBJ *a,OBJ *atacker,int directiondamage);
int	 tryunitaction(OBJ *a,OBJ *atacker);
void TryToScanArea(OBJ *a,OBJ *scanobjarea);
void TellOtherUnitsAboutAtacking(OBJ *a,OBJ *atacker);
void LoadSubUnitLoFile(OBJ *a,OBJstruct *b);
void FixSubUnitCoords(OBJ *a,OBJstruct *b);
void ShowCircleAndBar(OBJ *a);
void CreateImageAndAddToList(OBJ *a,int x256,int y256,int readyatbegin,unsigned short imagelo_id);

int	 ifselectedOBJbit(OBJ *a,int playernr);
void doselectedOBJbit(OBJ *a,int playernr,int flag);
int	 makeinvisibles(struct OBJ *a);
void invisiblestick(void);
void SetHallucinationOBJ(OBJ *a);
int	 IfUnitIsSelectable(OBJ *a);
void ChangeUnitSubUnitAndImagesAssociated(OBJ *a,SCUNIT SC_NewUnit);
void ChangeUnitAndImagesAssociated(OBJ *a,SCUNIT SC_NewUnit);
void SlotReturnResources(OBJ *a,int playernr,int slotnr);
void ReturnResources(SCUNIT SC_Unit,int playernr,int factorreturn);
void initmoveaction(OBJ *a,OBJ *destobj,int mode,int startx,int starty,int x,int y);
int	 CalcDestVars(OBJ *a,OBJ *destobj,int x1,int y1,int x2,int y2,unsigned char flingy_id);
void ForceKartChanges(OBJ *a);
void ForceKartChanges(OBJ *a,int xpos,int ypos);
void CheckForKartChanges(OBJ *a,int xpos,int ypos);
void CheckForWalkChanges(OBJ *a,int xpos,int ypos);

void InitStopAfterMove(OBJ *a);

void PathFinding_InitType1(OBJ *a,int initx,int inity,int destx,int desty);
int	 PathFinding_MovePortionType1(OBJ *a,MAIN_IMG *img,int deltamove);

int	 PathFinding_InitType2(OBJ *a,int initx,int inity,int destx,int desty,unsigned char flingy_id);
int	 PathFinding_MovePortionType2(OBJ *a,MAIN_IMG *img,unsigned char flingy_id,int deltamove);

void AdditionalUnitsProceed(void);
void AllOBJMoving(void);
int	 IfHaveDistanceForMove(OBJ *a,MAIN_IMG *img,OBJ *destobj,int mindistance);
void SetModeMove(OBJ *a,int mode);
int	 ApplyNextModeMove(OBJ *a);
int	 IfHaveNextModeMove(OBJ *a);

void InsertModeMove(OBJ *a,OBJ *destobj,int mode,int x,int y,int showmesflag);
void AddModeMove(OBJ *a,OBJ *destobj,int mode,int x,int y,int showmesflag);
void DelAllModeMoves(struct OBJ *a,int dealloc);
void DelOBJFromModeList(struct OBJ *a,struct OBJ *searchobj);
int	 GetOBJNrInTransport(OBJ *c,OBJ *a);
void FreeOBJFromTransport(OBJ *c,OBJ *a,int objnr);
void ChangeSubUnitCoords(OBJ *turret,OBJ *base);
int	 TryToEnterNydus(OBJ *a,OBJ *nydus);
int	 RegenerateShield(OBJ *a);
int	 RegenerateHealth(OBJ *a);
int	 LaunchScarab(OBJ *reaver,OBJ *destobj);
void LaunchInterceptors(OBJ *a,OBJ *destobj);
void SetInvisibleUnit(struct OBJ *a);

void CALLBACK_OBJ_AfterStop(OBJ *a);
int	 CALLBACK_OBJ_AtackedOBJISNULL(OBJ *a);


#define FixMapCoords(x,y)								\
		if (x < 16) x = 16;								\
		if (y < 16) y = 16;								\
		if (x >= MAXXMAP*32-16) x = MAXXMAP*32-16;		\
		if (y >= MAXYMAP*32-16) y = MAXYMAP*32-16;

#define UnitDoAiAction(playernr) (map.pl_owner[playernr] == OWNER_COMPUTER)// || map.pl_owner[playernr] == OWNER_HUMAN)


#endif /*_OBJS_W*/

