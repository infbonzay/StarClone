#if !defined(_VARS_W)
#define _VARS_W

#include <grplib/usegrp.h>
#include "defs.h"
#include "audio.h"
#include "const.h"
#include "mylist.h"
#include "mytime.h"
#include "pcx.h"
#include "fonts.h"
#include "tbl.h"
#include "messages.h"
#include "version.hxx"
#include "player.h"

#define  SC_CONFIGFILE	GAMENAME".conf"
//=============================
extern char    PREFIX_UNIT[200];
#define PREFIX_UNIT_OFFSET	5
//===========================================

#define CONF_VIDEO_PORTRAITS_ANIMATED	0
#define CONF_VIDEO_PORTRAITS_STATIC	1
#define CONF_VIDEO_PORTRAITS_DISABLED	2

typedef enum 
{ 
	TERRAIN_BADLANDS, 
	TERRAIN_SPACEPLATFORM,
	TERRAIN_INSTALLATION,
	TERRAIN_ASHWORLD,
	TERRAIN_JUNGLE,
	TERRAIN_DESERT,
	TERRAIN_ARCTIC,
	TERRAIN_TWILIGHT
}terrain_type; 

typedef enum
{
	RACE_ZERG=0,
	RACE_TERRAN=1,
	RACE_PROTOSS=2,
	RACE_INDEPENDENT=3,
	RACE_OBSERVER=4,
	RACE_NEUTRAL=4,
	RACE_USER_SELECT=5,
	RACE_INACTIVE=7,
	RACE_HUMAN=10
} player_race;


#define AUDIOFLAGS_MUSICDISABLE		0x01
#define AUDIOFLAGS_SOUNDDISABLE		0x02

#define MAP_GAMETYPE_MELEE		0
#define MAP_GAMETYPE_FREEFORALL		1
#define MAP_GAMETYPE_USEMAPSETTINGS	2

struct GAMECONF
{
	struct VIDEO
	{
	    int  gamma;		// from 0 , to 99
	    int  saturate;	//from 0 to 99
	    char animation;	// 0/1	no,yes
	    char portraits;	// 0/1/2 yes,static,no
	    char visiblemap;	//show no visible map under fog
	    char preview;	//bit 0 - star ,bit 1 expansion is set if played intro
	}videoconf;
	struct AUDIO
	{
	    int  audioflags;	// 0/1
	    int  musicvolume;	// from 0 , to 100
	    int  soundvolume;	// from 0 , to 100
	    char unitspeach;
	    char advisorsounds;
	    char buildsounds;
	    char subtitles;
	}audioconf;
	struct SPEED
	{
	    int gamespeed;	//0-6
	    int mousescroll;	//0-6
	    int keyscroll;	//0-6
	    char cputhrottling;	//0-1 no/yes
	}speedconf;
	struct NETWORK
	{
	    int  networklatency; //0-2
	}networkconf;
	DISPLAY grmode;
	char lang;
	char pl_race[MAXPLAYERS];
	char pl_owner[MAXPLAYERS];
	char allied_victory;
	char gametype;		//melee, use map settings, ...
};

extern unsigned char First_Unit_Build[3][2];
extern char icontype[];
extern char KUBIK[6][SIZEXKUBIK*SIZEYKUBIK];
extern char PKUBIK[6][SIZEXKUBIK*(SIZEYKUBIK-1)];
//====================
extern POSINFILE pos_in_file_man[];
extern POSINFILE pos_in_file_lansh[];

extern BARMESSAGES	chatbar;
extern BARMESSAGES	infobar;

extern PCX	consoleraw,consoleover,iconraw,buildemptyraw,buildfullraw;
extern char	*packedconsole,*packedconsoleover;
extern char	*missionobj;

extern char	*grptexture;
extern int 	DELTASCREENX;
extern int 	DELTASCREENY,DELTASCREENY2;

extern int	speedignore;
extern int	curentreadkey;
extern int	maxlanshaftparts;
extern int      changegoods;
extern long	gamecycle;
extern int 	helpgame;
extern int      MinObjRegen,MaxObjects;
extern int      NrObjRegen;
extern int      xpointconstr,ypointconstr,oldselectbuton,existinbuildconstr;
extern int	mouseonconstrunit,selecteddeconstrunit,selectedunloadunit;
extern int	oldmouseonconstrunit;
extern int	mouseontranspunit,oldmouseontranspunit;

extern int      method;
//extern int      leftkart,upkart;
extern int 	KEYSCROLLTIME,MOUSESCROLLTIME,MOUSESCROLLON;

extern int      groupmove;

extern int 	pylonselected;
extern unsigned char drawpylonareaactive;
extern int 	countofpylonsselected;

extern struct OBJ *objects[];

extern struct  XYcoord nextpoint[];
extern struct  XYcoord found[];

extern struct GEN_ANIM_LANSH *alllansh[];
extern struct _BE_ diapazone[];
extern unsigned short MaxRegen;
extern struct MAGEP mageprop[];

extern struct  OBJ *fordeselect[],*fordeselectcopy[];
extern struct  OBJ *curentoperationobj;
extern short    int xpoint1l,xpoint2l,wmaxcol,wmaxrow;
//extern		int xk,yk;
extern short    int maxway;

extern char	*cmpgn_cmd[12];
extern char	*rezu[45];
extern char	*soundtypes[MAXTYPESOFSOUND];
extern char	*rezo[23];
extern char    	*rezmp[27];
extern char	*rezwp[2];

extern char	*rezl[];
extern char	*rezd[];

extern char	*racename[];
//extern char 	*resourcefilesname[3][2];
extern short int resource_sprites_id[3][2];

extern unsigned char 	iresp[3][3][14*14];
extern unsigned char 	imanap[],iemptycubep[];

extern GRPFILE *gametilegrp,*gamedlggrp;
extern DATTBLSTRUCT alldattbl;


extern int 	MAPREGENERATIONBIT,MAPUNITSREGENERATIONBIT,NEEDTOCHANGECREEPBIT,NEEDTOHEALBIT,NEEDTOINFESTBIT;
extern int	NEEDTOMOVE1BIT,NEEDTOREPAIRREFRESHBIT;
extern GRPFILE 	*znakgrp,*grpicons,*creepgrp;
extern GRPFILE	*grpminmineral[],*grpmingas[];
extern char	twirep[];
extern int	needclearmap;


extern GRPFILE *grpwire1,*grpwire2,*grpwire3,*blinkgrp;

extern int 	magenrfrommageatr[];
extern int     	buildconstr,posibleconstruct;
extern OBJ	*cbuild;
extern int     	circl[];
extern unsigned char transportplace[];
extern struct OBJ *transportplaceobj;

extern int 	iconxcoord[][3];
extern int 	iconycoord[][3];
extern int 	xicon[];
extern int 	yicon[];
extern char	forexit[];
extern int	properties[];
extern char	selectableproperties[];

extern char    	nenemyobj,oldnenemy;
extern char    	canceloperation;
extern char    	pasw[];
extern char    	mess[];
extern struct  	PLAYGAMER_TEMP PLAYER[MAXPLAYERS];
extern char	gasminpos[3];
extern char    	_RACE,GAME,REQMEM;
extern signed char NUMBGAMER,SERVERGAMER;
extern int 	MAXXMAP,MAXYMAP,MAXXMAP8,MAXYMAP8;
extern int 	xy[][5];

extern char    neutralbuild,neutralman;
extern char    enemybuild,enemyman;
extern char    aliancebuild,alianceman;
extern char    mybuild,myman;
extern char    selectnomyobj,firstselect;
extern char    MAPDEF;//bit 0-desen terrain,
            	      //bit 1-desen units

extern char    _endegame,curentadd;
extern char    rus,scrnew,shareversion;

extern char	thpbarp[];
extern char	tunitp[];
extern char	tfontgamp[];
extern char	tblinkp[];
extern char	tableforminimap[];

extern SC_FontHeader *fonts[];


extern short 	sfx_buildplace[3];
extern short	sfx_powerdown[3];

extern char	includedfile[];
extern char	dirsave[];
extern char	gamesavv[];
extern char	extsavv[];
extern char	identgame[];
extern char	levelstr[];
extern char	ind_game[];
extern char	strname[];
extern char	pathul[];
extern char	olddir[];
extern GRPFILE	*fullc[],*puncc[];
extern char	*transpadr;
extern int 	ctextures;
extern mylist	mousehotspot;
extern char	networkresend,networkreceive,networkgametick,menutimerupdate;
extern signed char waittimeremain;


extern unsigned char actionsvars[][MAXIMMODE];

extern char	nickname[25];
extern char	GAMEPATH[512];
extern char	GAMEMAPPATH[512];
extern char	ADDGAMEPATH[512];
extern char	SELECTMAP[1024];
extern char 	FULLFILENAME[1024];
extern char 	SAVELOADFILENAME[1024];
extern char	MPQMISSING[1024];
extern char	GAMETYPE,GAMESPEED,JOINSLOT,MAPVERIFIED,MAPUPLOAD,HOSTGAMER;
extern unsigned int MY_PCID,SERVER_PCID,CHECKSUMMMAP,MAPLOADEDLENGTH,FILESIZEMAP,MAPLOADEDLENGTH,RNDSEED;
extern char	*GAMETITLE,*BASENAMEMAP;
extern char 	*prefix_campaignpath;

extern char 	GAMETITLESTR[];
extern char 	GAMEHOSTIP[];
extern char 	TXTCHAT[];


extern char	MAPNAME[];
extern int 	MAPSIZEX,MAPSIZEY;
extern char	MAXPLRS,PAUSEGAME,PAUSEINTRIG,MENUACTIVE,gamestatus;
extern unsigned char gamestatusremainticks,warparchon;

extern unsigned short PORT;
extern int totalimgs,drawedimgs;
extern int irradiatedamagepertick,plaguedamagepertick;

//============================CHEAT CODES===================
extern char	CODEFORSCREEN;
extern char	CODEFORGODMODE;
extern char	CODEFORMINERALS;
extern char	CODEFORGAZ;
extern char	CODEFORQUICKMAKE;
extern char	CHEATERBIT;
extern char	CODEFORWITHOUTGOODS;
extern char	CODEFORWITHOUTTECHTREE;
extern char	SHOWCELLS;
extern char	CODEFORPSIUNLIMIT;
extern char	HALFFOGUNDERALLMAP;
extern char	NETWORKGAME;
extern char	EXPANSIONSET;
extern int	nextscenario;
extern char	EMPTYSTR;
//==========================================================
extern GAMECONF gameconf;
void inivars(void);
void addvars(void);
void deinit(void);

#endif /*_VARS_W*/

