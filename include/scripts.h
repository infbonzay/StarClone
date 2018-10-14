#if !defined(_SCRIPTS_W)
#define _SCRIPTS_W

//=================================

#include <stdio.h>
#include <grplib/grp.h>
#include "mpq.h"
#include "loadlo.h"
#include "defs.h"
#include "portrait.h"

#define ORDERS_NONE		0
#define ORDERS_MODEMOVE		1
#define ORDERS_UNIT		2
#define ORDERS_UPGRADE		3
#define ORDERS_TECHNOLOGY	4


#define GRPFILEFORMAT	0
#define LOFILEFORMAT	1
#define WAVFILEFORMAT	2

#define TIMEMULL 	20
#ifdef UNDERLINUX
     void strupr(char *str);
     void strlwr(char *str);
#endif
//=====================================================
int getallnumbers(char *strvalue,int *maxnrforread,int maxnrs);
GRPFILE *getgrpnr(int nr_grp);
LOFILE  *getlonr(int nr_lo);
int  addgrpfile(char *filename);
int  createmantable(void);
//=====================================================
int  readrecordmans(FILE *f,int objnr);
void loaddefcfg(void);
void unloadcfg(void);
int  loadcfg(const char *filename,int *mpqresult);
void saveconf(void);
int  readmageproperties(void);
void FreeSMKInfo(struct SMKPORTRAITS *smkinfo);
struct SMKPORTRAITS *FillSMKInfo(SCUNIT SC_Unit);
extern MPQPRIO *mpqprio;


#define MOVECONTROL_FLINGYDAT		0
#define MOVECONTROL_PARTIALLYWEAPON 	1
#define MOVECONTROL_ISCRIPTBIN  	2

//=====================================================
#endif /* _SCRIPTS_W*/
