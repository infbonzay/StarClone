
#if !defined(_UNITACTION_W)
#define _UNITACTION_W

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
#include "action.h"

//=================================

int  GhostNUKEAction(OBJ *a);
void GhostNUKECancel(OBJ *a,int mode);

int  CheckIfCanBuild(OBJ *a,SCUNIT SC_Unit,int x,int y,int *constrerror);
int  CheckIfCanLand(OBJ *a,int x,int y,int *constrerror);
int  DecrUnitCost(SCUNIT SC_Unit,int playernr);

//=================================

//=================================
#endif

