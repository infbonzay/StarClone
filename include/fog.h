#ifndef _SC_FOG_H
#define _SC_FOG_H

#include "starmap.h"
//#define WITHOUTHALFFOG //unit forgot what he see before(only current view)

#define OPENCELL				4
#define OPENCELLNEAROPEN		3
#define OPENCELLNEARFOG			2
#define FOGCELLNEAROPEN			1
#define FOGCELL					0

#ifndef REMOVEFOGBUG
	#define REMOVEFOGBUG
	//#define REMOVEFOGBUG_FUNC1
	#define REMOVEFOGBUG_FUNC2
#endif

extern unsigned char *screenfog,*screenmapused;
extern char *fogsprp[14];
extern int SCRFOGX,SCRFOGY;

int	 loadfogtables(void);
void unloadfogtables(void);
void putfog(void);
void calculatefog(int bitplayers);
void copywhitefogtoblackfog(int bitplayers);
void ApplyFogMask(mapinfo *info,int playernr);
//=================================================


#endif

