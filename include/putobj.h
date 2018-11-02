
#if !defined(_PUTOBJ_W)
#define _PUTOBJ_W

#include <grplib/grp.h>
#include "man.h"
#include "mylist.h"

#define NORAMKA			0
#define WITHRAMKA		1

#define ICONUNIT		0
#define ICONDAMAGEUP	1
#define ICONDAMAGEDOWN	2
#define ICONARMOR		3
#define ICONSOMEOBJECT	4				//like spider mine

#define STATTXT_TBL_GEYSERDEPLETEDMES	874
#define STATTXT_TBL_DEPLETEDTEXT		797
#define STATTXT_TBL_UNPOWEREDTEXT		829
#define STATTXT_TBL_DAMAGETEXT			776
#define STATTXT_TBL_ARMORTEXT			777
#define STATTXT_TBL_SHIELDSTEXT			778
#define STATTXT_TBL_GAMEMENUTEXT		813
#define STATTXT_TBL_UNLOAD_UNIT			807
#define STATTXT_TBL_HELPSELECTUNITS		808
#define STATTXT_TBL_TERRAINONTEXT		809
#define STATTXT_TBL_TERRAINOFFTEXT		810
#define STATTXT_TBL_DIPLOMACY			811
#define STATTXT_TBL_MESSAGING			812
#define STATTXT_TBL_PERROCKETTEXT		1300

#define STATTXT_TBL_INTERCEPTORSTEXT	790
#define STATTXT_TBL_SCARABSTEXT			791
#define STATTXT_TBL_NUKESTEXT			792
#define STATTXT_TBL_MINESTEXT			793
#define STATTXT_TBL_NEXTLEVELTEXT		794

#define UPGRADES_DAT_PLASMASHIELDICON	15

#define WEAPONS_DAT_VALKYRIEHALOROCKET	103


#define IMAGES_DAT_FIRSTCIRCLENORMAL	561
#define IMAGES_DAT_FIRSTCIRCLEDASHED	571

//========================================
void desenlife(int x,int y,
			   int sizex,int statusplayer,
			   int health,int shield,
			   int mage,int timelife,
			   int maxhealth,int maxshield,
			   int maxmage,int maxtimelife);
//========================================
void desenhealth(int x,int y,int kubik,int count,int max);
void desenhealth2(int x,int y,int kubik,int count,int max);
int	 drawcircle(int x,int y,int st,struct OBJ *a);
void workingbuilds(struct OBJ *a);
void minimap_showobjs(void);
void putobjsonminimap(void);
void printobjparam(void);


void resetmode(struct OBJ *a,int mode,
				int sprmode,int newmode,
				int newsprmode,int posmode);
int addmaninbuild(struct OBJ *a,int nrunit,int maxtimes);
void FindLineOfProp(struct OBJ *a,struct OBJstruct *b);
void drawunitsintransport(int XWINPOS,int YWINPOS,struct OBJ *a);
void drawmineralinbar(int x,int y,GRPFILE *grp,int iconnr);
void drawunitinbar( int x,int y,int weight,int objtype,int ramka,
					int shield_pers,int life_pers,
					GRPFILE *grp1,GRPFILE *grp2,int iconnr2);
int getlifepercent(int number,int maxnumber);
void drawallunitsinbar(int XWINPOS,int YWINPOS);
void AnalizeAllTransportUnits(struct OBJ *a,unsigned char *rettransp,struct OBJ **trtplobj);
void putbuildneededtodraw(struct OBJ *a);
void clearshowgoods(void);
void showgoods(void);
int	 desenbuildifconstr(void);
void ifselectedDAMAGEDBUILD(OBJ *a);


void showadvisortext(int stat_txt_nr);
void putbuildplacemessage(int errorstate);
OBJ	 *CreateUnitInUnit(struct OBJ *a,int obj_id,int playsound,int xpos,int ypos);
void DrawBuildPlace(void);

//====================================================
extern mylist minimap_showedobjs;




#endif /*_PUTOBJ_W*/

