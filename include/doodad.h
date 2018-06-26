#if !defined(_DOODAD_W)
#define _DOODAD_W

#include "starmap.h"
#include "mylist.h"
#include "man.h"

#define DOODADBOTTOMSTATE	0x0008		//my own bit bottom(open) state, (trap automatically work if bottom state)
#define DOODADISUNIT		0x2000		

#define DOODAD_TOP_STATE	0
#define DOODAD_BOTTOM_STATE	1

#define DOODAD_NEXTSTATE_NONE	-1
#define DOODAD_NEXTSTATE_TOP	0
#define DOODAD_NEXTSTATE_BOTTOM	1

#define DOODAD_MOVE_NONE	-1
#define DOODAD_MOVE_TOTOP	0
#define DOODAD_MOVE_TOBOTTOM	1



void UnloadDoodadList(mylist *mapdoodads);
void CreateDoodadsFromLists(mylist *mapdoodads,struct mapinfo *loadedmap);
int  GetDoodadState(struct OBJ *a);
void SetDoodadState(struct OBJ *a,int state);
void DoodadChangeState(OBJ *a,int state);
void SetDoodadMoveDirection(OBJ *a,int movedirection);
int  GetDoodadMoveDirection(OBJ *a);

#endif /*_DOODAD_W*/
