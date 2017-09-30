
#if !defined(_ACTIONDEFAULT_W)
#define _ACTIONDEFAULT_W

#include "defs.h"
#include "vars.h"
#include "lists.h"
#include "man.h"
#include "objs.h"
#include "putobj.h"
#include "audio.h"
#include "load.h"
#include "mage.h"
#include "wmem.h"
#include "diap.h"
#include "images.h"
#include "action.h"

#define LARVAEMOVEDELTA (SIZESPRLANSHX*2)
#define WAITTICKBEFORESEARCHREPAIR	24

int CarrierAction( OBJ *a,MAIN_IMG *img);
int InterceptorsAction( OBJ *a,MAIN_IMG *img);
int LarvaeAction( OBJ *a,MAIN_IMG *img);
int HiveActionFirst( OBJ *a,MAIN_IMG *img);
int HiveAction( OBJ *a,MAIN_IMG *img);
int MedicAction( OBJ *a,MAIN_IMG *img);
int BatteryAction( OBJ *a,MAIN_IMG *img);
int QueenAction( OBJ *a,MAIN_IMG *img);
int CritterAction( OBJ *a,MAIN_IMG *img);

int  AdditionalUnitProceed(OBJ *a,MAIN_IMG *img);
int  AtackCoolDownEnds(OBJ *a,OBJ *destobj,int continueatack,int showerrorflag);
void SpecialAtackAction(OBJ *a,int iscriptstate);
void AtackAction(OBJ *a,OBJ *destobj,int continueatack);
void EndAtackAction(OBJ *a);
void SetAtackType(OBJ *a,OBJ *destobj);


//=================================
extern SCUNIT SC_CARRY_GAS[];
extern SCUNIT SC_CARRY_MIN[];

#endif

