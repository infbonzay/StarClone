#if !defined(_CREEP_W)
#define _CREEP_W

#include <limits.h>

#define NOCREEPNR	-1
#define MAXCREEPNR	36

#define DOLATERCREEPNR	0
#define DOCREEPNRNOW	1

#define BEGINCREEPTABLE	0
#define MAXCREEPTABLE	1

#define CREEPCOLOR	0xf2
#define CREEPEXPANDNR	40

#define NOCREEPBUILD		-1
#define CREEPBUILDTYPE0		0
#define CREEPBUILDTYPE1		1
#define CREEPUNDERBUILD		2

#define AFTERREMOVENOCLEARCREEP	0
#define AFTERREMOVECLEARCREEP	1

#define CREEPNEEDTABLEEMPTYFLAG	SCHAR_MAX
#define CREEPFILLTABLEEMPTYFLAG	SCHAR_MAX

#define FLAG_CREEP_INCREASE	0x01
#define FLAG_CREEP_TABLEID0	0x02
#define FLAG_CREEP_TABLEID1	0x04
#define FLAG_CREEP_DEGROWTHEND	0x08

#define CREEPLISTEMPTY		-1

struct CreepAddRemoveTables
{
	signed char	*creepinfo;
	signed char	*needcreepinfo;
	signed   short  xpos;
	signed   short  ypos;
	unsigned char	curnrofcreep;
	unsigned char	remaintogrowth;
//	unsigned char	creepsremoved;
	unsigned char	growthcreepflag;
};

    int  GetCreepAroundWithFog(int x,int y,int indextile32);
    int  GetCreepNr(int x,int y,int *myret);
    int  SetCreep(int x,int y);
    int  ClearCreep(int x,int y);
    void AddCreep(int x,int y);
    void RemoveCreep(int x,int y);
    void EmptyAllCreeps(void);
    void NowCreepAll(int xb,int yb,int xe,int ye);
    void FillWithCreepNow(struct OBJ *a,struct OBJstruct *b,int table);
    void FillWithCreepTimed(struct OBJ *a,struct OBJstruct *b);
    void EmptyCreepTimed(struct OBJ *a,struct OBJstruct *b);
    void RemoveAddOneCreep(void);
    void FillWithCreepUnderBuild(struct OBJ *a);
    void EmptyCreepUnderBuild(struct OBJ *a,int clearcreepflag);
    void AddOBJCreepTable(OBJ *a,int creeptype,int table);
    void RemoveOBJCreepTable(CreepAddRemoveTables *tempcreeptable);
    void IncreaseCreep(void);
    void DecreaseCreep(void);
    void AddToFilledCreepTable(CreepAddRemoveTables *tempcreeptable,int deltax,int deltay);
    void AddToNeededCreepTable(CreepAddRemoveTables *tempcreeptable,int deltax,int deltay);
    void DestroyCreepLists(void);


#endif /*_CREEP_W*/
