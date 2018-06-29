#if !defined(_CONSTRUCT_W)
#define _CONSTRUCT_W

#include "defs.h"
#include "man.h"

#define CONSTR_STEP_ALLBUILDSEXACT100	0

#define CONSTR_STEP_ZERGBUILDLESS33	1
#define CONSTR_STEP_ZERGBUILDLESS66	2
#define CONSTR_STEP_ZERGBUILDLESS99	3

#define CONSTR_STEP_TERRANBUILDLESS25	4
#define CONSTR_STEP_TERRANBUILDLESS50	5
#define CONSTR_STEP_TERRANBUILDLESS75	6
#define CONSTR_STEP_TERRANBUILDLESS99	7

#define CONSTR_STEP_ZERGUNITLESS99	8
#define CONSTR_STEP_PROTOSSBUILD	9
#define CONSTR_STEP_WARPARCHONS		10


struct CONSTRUCT
{
	short    int  timeremained;			//time remained for current construction;
	short    int  timemax;				//time remained for current construction;
	unsigned char nrofelem;				// nr of objs
	unsigned char type[MAXCONSTRUCTINBUILD];		//construct type 0-none,1-modemove,2-unit,3-upgrade,4-techs
	short    int  objs[MAXCONSTRUCTINBUILD];		//all objects(they numbers depend on type);
	short    int  icon[MAXCONSTRUCTINBUILD];		//nr icon of object in cmdicons.grp
	short    int  stattxt[MAXCONSTRUCTINBUILD];		//nr stattxt of object
	unsigned char upgr[MAXCONSTRUCTINBUILD];		//object upgrd(only for type=2(upgrade))
};

struct SELFCONSTRUCT
{
	short int 	timeremained;				//time remained for current construction;
	short int 	timemax;				//time remained for current construction;
	unsigned char	currentconstrstep;			//0 - 0-33%,1-33-66%,2-66-99%,3-100%
	
	int 		incrlife;
	int		incrshield;
	struct  OBJ	*obj;					//obj who construct the build(only terran)
};
//=======================================
CONSTRUCT *CreateConstrStruct(void);
void DestroyConstrStruct(struct OBJ *a);
int AddConstruct(struct OBJ *a,int type_id,int object_id,int upgr_nr,int icon_nr,int stattxt_id);
int DelLastConstruct(struct OBJ *a,int constrobj,int *type_id,int *obj_id);
int DelConstruct(struct OBJ *a,int constrobj,int *type_id,int *obj_id);
int IfConstructAddon(OBJ *a);
int GetConstructNR(struct OBJ *a);
int GetConstrComplete(struct OBJ *a,int percent);
int GetConstruct(struct OBJ *a,int elemnr,int *type_id,int *obj_id,int *upgr_nr,int *timeremained,int *icon_nr,int *stattxt_id);
void ConstructUnitsReleaseSupply(OBJ *a);
void UnitConstrUnit(struct OBJ *a,SCUNIT SC_Unit,int icon_nr,int stattxt_nr);
void UnitConstrUpgr(struct OBJ *a,int upgr_id,int upgr_nr,int icon_nr,int stattxt_nr);
void UnitConstrTech(struct OBJ *a,int tech_id,int icon_nr,int stattxt_nr);
void DoOrder(int playernr,struct OBJ *a,int x,int y,int type_id,int obj_id,int icon_nr,int stattxt_nr,int modemove,int showerrorflag);
void DoOrder(int playernr,OBJ *a,int x,int y,int modemove,int showerrorflag);

int  TickBuildMakeConstruct(struct OBJ *a,int *doneobj_id);
int  TickUnderConstruct(struct OBJ *a);
void SetBeginSelfConstruct(struct OBJ *a);
int  GetSelfConstrComplete(struct OBJ *a,int percent);
//=======================================

#define STARTBUILDSHIELD	(10<<8)
#define STARTBUILDLIFE		(10<<8)

#endif

