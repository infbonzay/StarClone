
#if !defined(_PYLON_W)
#define _PYLON_W

#include "man.h"
#include "starmap.h"

#define PYLONAREAX				16
#define PYLONAREAY				10

#endif /*_PYLON_W */

void FreePylonArea(mapinfo *mymap);
unsigned char *CreatePylonAreaArray(mapinfo *mymap,int nplayer);
void AddPylonArea(mapinfo *mymap,OBJ *a,int playernr);
void DelPylonArea(mapinfo *mymap,OBJ *a,int playernr);
void AddBuildsInPower(unsigned char *pylonarea,int playernr);
void DelBuildsFromPower(unsigned char *pylonarea,int playernr);
void CheckBuildForPower(mapinfo *mymap,OBJ *a,int playernr);
void CheckProtossBuildsForPower(mapinfo *mymap);
