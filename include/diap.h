#ifndef _SC_DIAP_H

#define _SC_DIAP_H

#include "defs.h"
#include "man.h"

#define XPR				500
#define YPR				362
#define ENABLECONSTRUCT	 158
#define DISABLECONSTRUCT 111

void desenproperties(int *localprop,char *selectableicons);
int	 CreateMenuProperties(int *prop,char *selectableicons,int player);
int	 ChangeTypeOfProp(struct OBJ *a,struct OBJstruct *b,int typeofprop);
int	 ChangeTypeOfProp(struct OBJ *a,int typeofprop);
int	 ExistProp(struct OBJstruct *b,int verifymode);

#endif /*_DIAP_W*/
