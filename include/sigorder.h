#if !defined(_SIGORDER_W)
#define _SIGORDER_W


#define ADDBUILD_AIR_ELEVATIONLEVEL	10
int  SIGOrder_NULL(OBJ *a);
void SetOrder(OBJ *a,int signal,int (*func)(OBJ *a));
int  SIGOrder_ZergBuildFinishConstruct(OBJ *a);
int  SIGOrder_TerranBuildLastConstructSprite(OBJ *a);
int  SIGOrder_TerranBuildFinishConstruct(OBJ *a);
int  SIGOrder_ProtossBuildShowTexture(OBJ *a);
int  SIGOrder_ProtossBuildWhiteAppear(OBJ *a);
int  SIGOrder_ProtossBuildFinishConstruct(OBJ *a);
int  SIGOrder_Tank_EndRotationBeforeChangeMode(OBJ *a);
int  SIGOrder_Tank_AfterSiegeCmd(OBJ *a);
int  SIGOrder_Tank_AfterNormalCmd(OBJ *a);
int  SIGOrder_AfterBurrow(OBJ *a);
int  SIGOrder_AfterUnburrow(OBJ *a);
int  SIGOrder_LiftOff(OBJ *a);
int  SIGOrder_Landing(OBJ *a);
int  SIGOrder_ScannerSweepEnd(OBJ *a);
int  SIGOrder_ZergConstrFromLarvaeEnd(OBJ *a);
int  SIGOrder_ZergEggBirth(OBJ *a);
int  SIGOrder_ZergUnitBirth(OBJ *a);
int  SIGOrder_CancelMorthFromHydralisk(OBJ *a);
int  SIGOrder_CancelMorthFromDrone(OBJ *a);
int  SIGOrder_DroneLiftOff(OBJ *a);
int  SIGOrder_CancelMorthFromBuild(OBJ *a);
int  SIGOrder_CancelMorthFromMutalisk(OBJ *a);
int  SIGOrder_DoodadAfterTop(OBJ *a);
int  SIGOrder_DoodadAfterBottom(OBJ *a);
int  SIGOrder_UnitInitComplete(OBJ *a);
int  SIGOrder_AfterCastMage(OBJ *a);
int  SIGOrder_NukeLanding(OBJ *a);
int  SIGOrder_AfterCastHeal(OBJ *a);


void AfterConstruct(OBJ *a);


#endif /*_SIGORDER_W*/

