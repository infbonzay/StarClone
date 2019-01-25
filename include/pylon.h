#ifndef _SC_PYLON_H

#define _SC_PYLON_H

#include "man.h"
#include "starmap.h"

#define PYLONAREAX				16
#define PYLONAREAY				10

void FreePylonArea(StarMapInfo *mymap);
unsigned char *CreatePylonAreaArray(StarMapInfo *mymap,int nplayer);
void AddPylonArea(StarMapInfo *mymap,OBJ *a,int playernr);
void DelPylonArea(StarMapInfo *mymap,OBJ *a,int playernr);
void AddBuildsInPower(unsigned char *pylonarea,int playernr);
void DelBuildsFromPower(unsigned char *pylonarea,int playernr);
void CheckBuildForPower(StarMapInfo *mymap,OBJ *a,int playernr);
void CheckProtossBuildsForPower(StarMapInfo *mymap);

#endif /*_PYLON_W */
