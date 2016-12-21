#if !defined(_DOODAD_W)
#define _DOODAD_W

#include "defs.h"

#define DOODADMIRROR		0x4000
#define DOODADINCRSPR		0x1000
#define DOODADMIRRORTHENDECRSPR	0x0002		//my own bit
#define DOODADINCRDECRSPR	0x0004		//my own bit
#define DOODADBOTTOMSTATE	0x0008		//my own bit bottom(open) state, (trap automatically work if bottom state)

#define DOODADISUNIT		0x2000		

#define DOODAD_TOP_STATE	0
#define DOODAD_BOTTOM_STATE	1
#define DOODAD_MOVING_STATE	2

void UnloadDoodadList(mylist *mapdoodads);
void CreateDoodadsFromLists(mylist *mapdoodads,struct mapinfo *loadedmap);
int  GetDoodadState(struct OBJ *a);
void SetDoodadState(struct OBJ *a,int state);
void DoodadChangeState(OBJ *a,int state);

#endif /*_DOODAD_W*/
