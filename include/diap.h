#if !defined(_DIAP_W)
#define _DIAP_W

#include "defs.h"
#include "man.h"

#define XPR				500
#define YPR				362
#define ENABLECONSTRUCT	 158
#define DISABLECONSTRUCT 111

void makesteps(int speed);
int	 constrbuild(int nb);
int	 probeconstruct(int nb);
void desenproperties(int *localprop,char *selectableicons);
int	 CreateMenuProperties(int *prop,char *selectableicons,int player);
int	 ChangeTypeOfProp(struct OBJ *a,struct OBJstruct *b,int typeofprop);
int	 ChangeTypeOfProp(struct OBJ *a,int typeofprop);
int	 ExistProp(struct OBJstruct *b,int verifymode);
void maketypeofmove(struct OBJ *a,struct OBJstruct *b);
void allobjtypemove(void);
int	 getborderbuild(int mx,int my,int *x,int *y);
void playadvisorerr(int playernr,int race,int retreserror);
int	 CheckCoordinates(int newx256,int newy256,SCUNIT SC_Unit);


#endif /*_DIAP_W*/
