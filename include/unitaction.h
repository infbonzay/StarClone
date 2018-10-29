
#if !defined(_UNITACTION_W)
#define _UNITACTION_W

#include "man.h"
#include "defs.h"

int	 GhostNUKEAction(OBJ *a);
void GhostNUKECancel(OBJ *a,int mode);

int	 CheckIfCanBuild(OBJ *a,SCUNIT SC_Unit,int x,int y,int *constrerror);
int	 CheckIfCanLand(OBJ *a,int x,int y,int *constrerror);
int	 DecrUnitCost(SCUNIT SC_Unit,int playernr);

//=================================

//=================================
#endif

