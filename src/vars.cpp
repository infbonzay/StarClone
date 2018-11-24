
#include <grplib/gr8.h>
#include <grplib/usegrp.h>

#include "defs.h"
#include "stringfiles.h"
#include "const.h"
#include "auxil.h"
#include "gr.h"
#include "key.h"
#include "man.h"
#include "pcx.h"
#include "mylist.h"
#include "maps.h"
#include "objs.h"
#include "fonts.h"
#include "diap.h"
#include "vision.h"
#include "fog.h"
#include "tbl.h"
#include "action.h"
#include "network_general.h"
#include "gener.h"
#include "vars.h"

#ifdef	WITHSDL
#include "sdl/grsdl.h"
#include "sdl/mousesdl.h"
#endif
#ifdef UNDERDOS
#include "dos/handlers.h"
#endif
//=============================
unsigned char First_Unit_Build[3][2] = {
							{SC_HATCHERYOBJ,SC_DRONEOBJ},
							{SC_COMMCENTEROBJ,SC_SCVOBJ},
							{SC_NEXUSOBJ,SC_PROBEOBJ}
};
char icontype[4] = { 13,15,00,17 };
char KUBIK[MAXCOLORHPBAR][SIZEXKUBIK*SIZEYKUBIK];
char PKUBIK[MAXCOLORHPBAR][SIZEXKUBIK*(SIZEYKUBIK - 1)];
char EMPTYSTR = 0;
//====================
DATTBLSTRUCT alldattbl;
POSINFILE pos_in_file_man[NUMBRSPROBJ];

BARMESSAGES		chatbar(13);
BARMESSAGES		infobar(1);

PCX		consoleraw, consoleover, iconraw, buildemptyraw, buildfullraw;
char	*packedconsole, *packedconsoleover;

char	*missionobj;

char	*grptexture;
int		DELTASCREENX;
int		DELTASCREENY, DELTASCREENY2;

int		nextscenario;
int		curentreadkey;
int		changegoods = 1;
long	gamecycle;
int		helpgame;
int		MaxObjects;

int		oldselectbuton, existinbuildconstr;
int		mouseonconstrunit, selecteddeconstrunit, selectedunloadunit;
int		oldmouseonconstrunit = -1;
int		mouseontranspunit, oldmouseontranspunit = -1;

int		method = 0;
//int	  leftkart,upkart;
int		KEYSCROLLTIME, MOUSESCROLLTIME, MOUSESCROLLON = 1;

int		groupmove;
char	tableforminimap[16];

int		pylonselected;
unsigned char drawpylonareaactive;
int		countofpylonsselected;
struct	OBJ *objects[MAXOBJECTONMAP];

struct	XYcoord nextpoint[1000];
struct	XYcoord found[1000];

struct DISPLAY grmode;

struct MAGEP mageprop[MAXIMMODE + 1];//pointer to MAGEP array

SelectionObjs	SelectedUnits(MAXSELECTMAN);
int 			SelectedUnitTypes;


struct	OBJ *curentoperationobj;
short	int xpoint1l, xpoint2l, wmaxcol, wmaxrow;
int		ctextures;
//		int xk,yk;
short	int maxway;

const char		*racename[3] = { "Zerg","Terran","Protoss" };

const char	  *cmpgn_cmd[12] = { "/COMMENT","/FADESPEED","/DISPLAYTIME","/BACKGROUND","/FONTCOLOR",
						"/SCREENLOWERLEFT","/SCREENLEFT","/SCREENRIGHT","/SCREENCENTER","/SCREENTOP","/SCREENBOTTOM",
						"/PAGE" };

const char	  *rezu[14] = { "[NEWOBJECT]","ENDOBJ","NOADDSTATSATCONSTR",
					   "SPEEDUPG","ULTRALISKSECONDARMOR","SIGHTUPG",
					   "MANAUPG","PROPERTIES","TIMELEFT","CREATEFROMUNIT","ATACKSPEEDUPG",
					   "CREEP","CLOAKRANGE","ADDITIONALTEXT"
};

const char	  *rezo[24] = { "LANG","GAMEPATH","MAPPATH","GAMESPEED","MUSICVOLUME","SOUNDVOLUME","UNITSPEACH",
					"ADVISORSOUND","BUILDSOUND","SUBTITLES","GAMMA","SATURATE","ANIMATION","PORTRAITS",
					"MPQFILES","GRMODE","MOUSESCROLLSPEED","KEYSCROLLSPEED","CPUTHROTTLING","VIEWENTIREMAP",
					"NETWORKLATENCY","INSTALLFILES","FIRSTRUN","TITLEDELAY"
};
const char	  *rezmp[16] = { "[NEWMAGE]","MINDISTANCE","DIAPAZONE","RIGHTWAY","IGNOREREQ","TIMEMAGEACTIVE",
				  "IMAGES_ID","USETECHID","DEPENDONTYPEID","CREATEWEAPON",
				  "QUEUEDTOMOVELIST","CANNOTBEINTERRUPTED","SOUNDONCASTANDHIT","RACE","SHOWHALLUCINATED","ORDERTYPE"
};
const char	  *rezwp[2] = { "[WEAPON]","RANGEUPGNR" };

GRPFILE *gametilegrp, *gamedlggrp;

unsigned char iresp[3][3][14 * 14];
unsigned char imanap[14 * 14], iemptycubep[14 * 14];

short int resource_sprites_id[3][2] = { {292,296},{292,298},{292,294} };

int		MAPREGENERATIONBIT, MAPUNITSREGENERATIONBIT, NEEDTOCHANGEMANABIT, NEEDTOCHANGECREEPBIT;
GRPFILE *znakgrp, *grpicons, *creepgrp;
GRPFILE *grpwire1, *grpwire2, *grpwire3, *blinkgrp;
GRPFILE *grpminmineral[3], *grpmingas[3];
char	needclearmap, ifhaverescuableplayers;
int		magenrfrommageatr[MAXMAGEATR];
OBJ		*cbuild;
int		circl[10] = { 22,32,48,62,72,94,110,122,146,224 };
unsigned char transportplace[8] = { 0,0,0,0,0,0,0,0 };
struct	OBJ *transportplaceobj;
int		iconxcoord[3][3] = { {500,546,592},{505,552,598},{503,550,596} };
int		iconycoord[3][3] = { {360,399,440},{358,398,439},{357,397,438} };
int		xicon[3];				//x&y coordinates of properties of unit
int		yicon[3];				//on screen	 - move,atack,stop

char	forexit[500];

int		properties[9];
char	selectableproperties[9];

char	nenemyobj, oldnenemy;
char	canceloperation;
char	pasw[MAXPASW + 2];
char	mess[MAXPASW + 30];
char	gasminpos[3] = { 17,1,17 };
struct	PLAYGAMER_TEMP PLAYER[MAXPLAYERS];
char	_RACE, GAME, REQMEM;
signed	char NUMBGAMER = -1, SERVERGAMER = -1;
int		MAXXMAP, MAXYMAP, MAXXMAP8, MAXYMAP8;
int		xy[2][5] = { {0,0,39,78,118},{0,39,39,39,39} };

char	neutralbuild = 0, neutralman = 0;
char	enemybuild = 0, enemyman = 0;
char	aliancebuild = 0, alianceman = 0;
char	mybuild = 0, myman = 0;
char	selectnomyobj, firstselect;
char	MAPDEF;//bit 0-desen terrain,
			   //bit 1-desen units

char	curentadd = 0;
char	rus, scrnew = 1, shareversion = 0;

char	tfontgamp[48];
char	tblinkp[152];
char	thpbarp[19];
char	tunitp[256];
char	twirep[24];
SC_FontHeader *fonts[MAXFONTS];

short	sfx_buildplace[3] = { 4,5,6 };
short	sfx_powerdown[3] = { 114,115,116 };

char	dirsave[] = "savegame";
char	gamesavv[] = "OC_save";
char	extsavv[] = ".sav ";
char	identgame[] = "save game <OC> Wabo";
char	levelstr[5];
char	olddir[70];
char	nickname[MAXPLAYERNAMESYMBOLS];
char	GAMEPATH[MAXFILENAMESYMBOLS];
char	GAMEMAPPATH[MAXFILENAMESYMBOLS];
char	ADDGAMEPATH[] = { "BroodWar" };
char	SELECTMAP[MAXFILENAMESYMBOLS];
char	FULLFILENAME[MAXFILENAMESYMBOLS];
char	SAVELOADFILENAME[MAXFILENAMESYMBOLS];
char	MPQMISSING[MAXFILENAMESYMBOLS];
char	GAMETYPE, GAMESPEED, JOINSLOT, MAPVERIFIED, MAPUPLOAD, HOSTGAMER;
unsigned int MY_PCID, SERVER_PCID, CHECKSUMMMAP, FILESIZEMAP, MAPLOADEDLENGTH, RNDSEED;
char	*GAMETITLE, *BASENAMEMAP;
char	*prefix_campaignpath;
char	GAMETITLESTR[STARCLONE_NETWORKJOIN_MAXGAMETITLESYMB + 1];
char	GAMEHOSTIP[15 + 1];				//255.255.255.255+\0x0
char	TXTCHAT[MAXCHATSYMB + 1];
char	MAPNAME[STARCLONE_NETWORKJOIN_MAXMAPNAMESYMB + 1];
int		MAPSIZEX, MAPSIZEY;
char	MAXPLRS;
char	PAUSEGAME, PAUSEINTRIG, MENUACTIVE, gamestatus;
unsigned char gamestatusremainticks;

unsigned short PORT;
GRPFILE *fullc[10], *puncc[10];
char	*transpadr;
mylist	mousehotspot;
char	networkresend, networkreceive, networkgametick, menutimerupdate;
signed	char waittimeremain;
int		oneWarpPictureSize;


GAMECONF gameconf;

char	PREFIX_UNIT[200] = "unit\\";

//=============== MAGEICONS FEEDBACK VARIABLES =============
unsigned char warparchon;
//int totalimgs, drawedimgs;
int irradiatedamagepertick, plaguedamagepertick;
//============================CHEAT CODES===================
char	CODEFORSCREEN = 0;
char	CODEFORGOD = 0;
char	CODEFORQUICKMAKE = 1;	//1-32	// 0 is bad idea
char	CHEATERBIT = 0;
char	CODEFORWITHOUTGOODS = 0;
char	CODEFORWITHOUTTECHTREE = 0;
char	CODEFORPSIUNLIMIT = 0;
char	SHOWCELLS = 0;
char	NETWORKGAME = 0;			//set to value	if network
char	EXPANSIONSET = 1;
//==========================================================
void inivars(void)
{
	GAME = 0;
	keyactive = 0;
	lastkey = 0;
}
//================================
void addvars(void)
{
	screenMapInfo->MinimapPosX = 5 + DELTASCREENX;
	screenMapInfo->MinimapPosY = 348 + DELTASCREENY;
	screenMapInfo->SizeX32 = gameconf.grmode.x / SIZESPRLANSHX;
	screenMapInfo->SizeY32 = (gameconf.grmode.y - 32 * 3) / SIZESPRLANSHY;

	SCRFOGX = screenMapInfo->SizeX32 + 2 + 1;
	SCRFOGY = screenMapInfo->SizeY32 + 2 + 1;


	highMouse->SetRestrictCoords(MOUSEMODE1, 0, 0, gameconf.grmode.x - 1, gameconf.grmode.y - 1);
	highMouse->SetRestrictCoords(MOUSEMODE2, 0, 0, gameconf.grmode.x - 1, gameconf.grmode.y - 1);
	highMouse->SetRestrictCoords(MOUSEMODE3, 0, 0, gameconf.grmode.x - 1, gameconf.grmode.y - 110);

	screenfog = (unsigned char *)wmalloc(SCRFOGX*SCRFOGY);
	screenmapused = (unsigned char *)wmalloc((screenMapInfo->SizeY32 + 1)*(screenMapInfo->SizeX32 + 1));
	calcinert();
	calcsintables();
	SetAroundTable();
}
//================================
void deinit(void)
{
	if (screenfog)
	{
		wfree(screenfog);
		screenfog = NULL;
	}
	if (screenmapused)
	{
		wfree(screenmapused);
		screenmapused = NULL;
	}
}
//================================
