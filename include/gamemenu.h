#if !defined(_GAMEMENU_W)
#define _GAMEMENU_W

#include "defs.h"
#include "menu.h"
#include "mylist.h"
#include "pcx.h"


class MENUAPPEAR : public mylistsimple
{
public:
	MENUAPPEAR(int elems);
	~MENUAPPEAR();
};

#define MAXGATEWAYS				4

#define GDECOR_DISABLED			144
#define GDECOR_NOFOCUS			123
#define GDECOR_ONSELECT			165
#define GDECOR_ONFOCUS			123
#define GDECOR_NONE				0


#define GTEXTCOLORWHITE ((GWHITECOLORFONT<<24)|(GWHITECOLORFONT<<16)|(GWHITECOLORFONT<<8)|(GGREYCOLORFONT))
#define GTEXTCOLORBLUE	((GBLUECOLORFONT<<24)|(GBLUECOLORFONT<<16)|(GBLUECOLORFONT<<8)|(GGREYCOLORFONT))
#define GTEXTCOLORGREEN ((GGREENCOLORFONT<<24)|(GGREENCOLORFONT<<16)|(GGREENCOLORFONT<<8)|(GGREYCOLORFONT))
#define GTEXTCOLORRED	((GREDCOLORFONT<<24)|(GREDCOLORFONT<<16)|(GREDCOLORFONT<<8)|(GGREYCOLORFONT))
#define GTEXTCOLORYELLOW ((GYELLOWCOLORFONT<<24)|(GYELLOWCOLORFONT<<16)|(GYELLOWCOLORFONT<<8)|(GGREYCOLORFONT))



#define GBUTTONCOLOR2	((GWHITECOLORFONT<<24)|(GGREENCOLORFONT<<16)|(GYELLOWCOLORFONT<<8)|(GGREYCOLORFONT))
#define GHORIZCOLOR1	((GWHITECOLORFONT<<24)|(GGREENCOLORFONT<<16)|(GYELLOWCOLORFONT<<8)|(GGREYCOLORFONT))

#define GBUTTONCOLOR1			((GYELLOWCOLORFONT<<24)|(GWHITECOLORFONT<<16)|(GGREENCOLORFONT<<8)|(GGREYCOLORFONT))
#define GBUTTONCOLOR2			((GWHITECOLORFONT<<24)|(GGREENCOLORFONT<<16)|(GYELLOWCOLORFONT<<8)|(GGREYCOLORFONT))
#define GCHECKBOXCOLOR1			((GYELLOWCOLORFONT<<24)|(GWHITECOLORFONT<<16)|(GBLUECOLORFONT<<8)|(GGREYCOLORFONT))
#define GRADIOBUTTONCOLOR1		((GYELLOWCOLORFONT<<24)|(GWHITECOLORFONT<<16)|(GBLUECOLORFONT<<8)|(GGREYCOLORFONT))
#define GLISTCOLOR1				((GWHITECOLORFONT<<24)|(GYELLOWCOLORFONT<<16)|(GBLUECOLORFONT<<8)|(GGREYCOLORFONT))


#define GDECOREDIT1		((GDECOR_ONFOCUS<<24)|(GDECOR_ONSELECT<<16)|(GDECOR_NOFOCUS<<8)|(GDECOR_DISABLED))
#define GDECORLIST1		((GDECOR_ONFOCUS<<24)|(GDECOR_ONSELECT<<16)|(GDECOR_NOFOCUS<<8)|(GDECOR_DISABLED))
#define GDECORBUTTON1	((GDECOR_NONE<<24)|(GDECOR_ONSELECT<<16)|(GDECOR_NONE<<8)|(GDECOR_DISABLED))

#define NODECORSELECTEDLISTITEM			0
#define DECORSELECTEDLISTITEM			255



#define STAR_PROTOSS	 0x00
#define STAR_TERRAN		 0x01
#define STAR_ZERG		 0x02
#define STAR_LOADSAVED	 0x03
#define STAR_PLAYCUSTOM	 0x04
#define STAR_LOADREPLAY	 0x05
#define STAR_CANCEL		 0x06
#define STAR_ERROR		 0x07

#define BROOD_PROTOSS	 0x10
#define BROOD_TERRAN	 0x11
#define BROOD_ZERG		 0x12
#define BROOD_LOADSAVED	 0x13
#define BROOD_LOADREPLAY 0x14
#define BROOD_PLAYCUSTOM 0x15
#define BROOD_CANCEL	 0x16
#define BROOD_ERROR		 0x17

#define USERS_DIRECTORY	 "characters/"
#define SAVES_DIRECTORY	 "savegame/"
#define REPLAYS_DIRECTORY  "replays/"
struct MENUINFO_SMKS
{
		char *fn1;
		int x1;
		int y1;
		char *fn2;
		int x2;
		int y2;
		int sizex;
		int sizey;
};

#define WAITMENUAPPEAR			20000
#define MAXWAITMENUAPPEAR		40000
#define MENUAPPEAR_FROMLEFT		0
#define MENUAPPEAR_FROMRIGHT	1
#define MENUAPPEAR_FROMTOP		2
#define MENUAPPEAR_FROMBOTTOM	3

#define GLUALL_TBL_SCORETEXT			3		//3-14
#define GLUALL_TBL_ELAPSEDTIME			15
#define GLUALL_TBL_LOSEWIN				16		//16-18

#define GLUALL_TBL_DELETESAVED			21
#define GLUALL_TBL_NEWCHAR				22
#define GLUALL_TBL_DELETECHAR			23
#define GLUALL_TBL_ALRDCHAR				24
#define GLUALL_TBL_MANYCHAR				25
#define GLUALL_TBL_ERRCREATECHAR		26
#define GLUALL_TBL_UPONELEVEL			27

#define GLUALL_TBL_NRPLAYERS			30
#define GLUALL_TBL_MAPSIZEXY			31
#define GLUALL_TBL_MAPSIZE				32
#define GLUALL_TBL_TILESET				33
#define GLUALL_TBL_TILESETNAMES			38		//38-45



#define GLUALL_TBL_ERRMAP				48
#define GLUALL_TBL_MANYGAMES			54
//#define GLUALL_TBL_NEWGAMENETERR		55
#define GLUALL_TBL_ERRNOCOMPUTERS		56

#define GLUALL_TBL_SPEED_SLOWEST		84
#define GLUALL_TBL_SPEED_SLOWER			85
#define GLUALL_TBL_SPEED_SLOW			86
#define GLUALL_TBL_SPEED_NORMAL			87
#define GLUALL_TBL_SPEED_FAST			88
#define GLUALL_TBL_SPEED_FASTER			89
#define GLUALL_TBL_SPEED_FASTEST		90

#define GLUALL_TBL_NETWORKTYPES			95		//95-98
#define GLUALL_TBL_NETWORKREQ			99		//99-102
#define GLUALL_TBL_MAXPLAYERS			103
#define GLUALL_TBL_NETWORKERR			108
#define GLUALL_TBL_NETWORKBROADCASTERR	108
#define GLUALL_TBL_ERRDOWNLOADMAP		111
#define GLUALL_TBL_ERRNOPLAYERS			113
#define GLUALL_TBL_PLAYERNOTREADY		114
#define GLUALL_TBL_YOUHAVEBEENBANNED	116
#define GLUALL_TBL_TEXT_SECONDS			129
#define GLUALL_TBL_COMPUTERLIST			131
#define GLUALL_TBL_OPENLIST				132
#define GLUALL_TBL_CLOSEDLIST			133
#define GLUALL_TBL_BANPLAYERLIST		134
#define GLUALL_TBL_GAMESPEEDTXT			138
#define GLUALL_TBL_GAMEALRDEXIST		142
#define GLUALL_TBL_SELECTGAMETYPE		172
#define GLUALL_TBL_TEXT_SECOND			201


#define NETWORK_TBL_ERRSAVE				1
#define NETWORK_TBL_REPLACESAVE			2
#define NETWORK_TBL_DELSAVE				3
#define NETWORK_TBL_ZERGRACE			6
#define NETWORK_TBL_TERRANRACE			7
#define NETWORK_TBL_PROTOSSRACE			8
#define NETWORK_TBL_RANDOMRACE			9
#define NETWORK_TBL_OBSERVERRACE		26
#define NETWORK_TBL_SETPAUSE			56
#define NETWORK_TBL_SETRESUME			57

#define NETWORK_TBL_NETWORKLOWLATENCY	68
#define NETWORK_TBL_NETWORKHIGHLATENCY	69
#define NETWORK_TBL_NETWORKXHIGHLATENCY 70

#define NETWORK_TBL_PLAYERJOIN			73
#define NETWORK_TBL_PLAYERLEAVE			74
#define NETWORK_TBL_PLAYERLEAVETIMEOUT	75

#define NETWORK_TBL_ZERGCAMPAIGNERR1	140
#define NETWORK_TBL_PROTOSSCAMPAIGNERR1 141
#define NETWORK_TBL_XTERRANCAMPAIGNERR1 142
#define NETWORK_TBL_XZERGCAMPAIGNERR1	143

#define NETWORK_TBL_TERRANCLANNAMES		144		//144-151
#define NETWORK_TBL_ZERGCLANNAMES		152		//152-159
#define NETWORK_TBL_PROTOSSCLANNAMES	160		//160-167 - the last at this tbl
#define NETWORK_TBL_OBSERVERCLANNAMES	NETWORK_TBL_OBSERVERRACE
#define NETWORK_TBL_ERRDELSAVE			1

#define MYINFO_TBL_MELEE				1
#define MYINFO_TBL_FREEFORALL			2
#define MYINFO_TBL_USEMAPSETT			3

#define MYINFO_TBL_NETTYPE1				4
#define MYINFO_TBL_NETTYPE2				5
#define MYINFO_TBL_NETTYPE3				6
#define MYINFO_TBL_NETTYPE4				7
#define MYINFO_TBL_NETTYPE5				8

#define MYINFO_TBL_NETREQ1				9
#define MYINFO_TBL_NETREQ2				10
#define MYINFO_TBL_NETREQ3				11
#define MYINFO_TBL_NETREQ4				12
#define MYINFO_TBL_NETREQ5				13

#define MYINFO_TBL_NETPORTINUSE			14
#define MYINFO_TBL_ERRPLCMPGN			15
#define MYINFO_TBL_ERRLOADDISABLED		16
#define MYINFO_TBL_ERRLOADREPL			17
#define MYINFO_TBL_DELREPL				18
#define MYINFO_TBL_ERRDELREPL			19
#define MYINFO_TBL_ERRDELCHAR			20
#define MYINFO_TBL_SAVEGAMEERR1			21
#define MYINFO_TBL_LOADGAMEERR1			22
#define MYINFO_TBL_GOAL1				23
#define MYINFO_TBL_HOSTDOWN				24
#define MYINFO_TBL_CHKDUBLGAME			25
#define MYINFO_TBL_GAMEALRDEXIST		26
#define MYINFO_TBL_SENDJOINREQUEST		27
#define MYINFO_TBL_HOSTFULL				28
#define MYINFO_TBL_ALLWAITPORTSUSED		29
#define MYINFO_TBL_SENDERROR			30
#define MYINFO_TBL_WRONGSERVERINFO		31
#define MYINFO_TBL_NEWGAMENETERR		32
#define MYINFO_TBL_SERVERTIMEOUT		33
#define MYINFO_TBL_HOSTHASLEFT			34
#define MYINFO_TBL_NETWORKERRORSEND		35
#define MYINFO_TBL_PLAYERBANNED			36
#define MYINFO_TBL_ALREADYBANNED		37
#define MYINFO_TBL_NAMEEXISTS			38
#define MYINFO_TBL_ERRLOADMAP			39
#define MYINFO_TBL_COMPUTERNAME			40
#define MYINFO_TBL_MISSINGMPQS			41
#define MYINFO_TBL_MISSINGWGET			42
#define MYINFO_TBL_DOWNLOADABORTED		43
#define MYINFO_TBL_STARTDOWNLOAD		44
#define MYINFO_TBL_DOWNLOADINGTEXT		45
#define MYINFO_TBL_FILENOTFOUND			46
#define MYINFO_TBL_BRIEFTIMEOUT			47
#define MYINFO_TBL_ERRNOPLAYERS			48
#define MYINFO_TBL_MESSAGETOSYSTEM		49
#define MYINFO_TBL_MESSAGETOALL			50
#define MYINFO_TBL_MESSAGETOALLIES		51
#define MYINFO_TBL_MESSAGETOPLAYER		52
#define MYINFO_TBL_STARTINGIN			53
#define MYINFO_TBL_ERRNOSTARINSTALL		54
#define MYINFO_TBL_CORRUPTCHAR			55
#define MYINFO_TBL_GATEWAY1				56
#define MYINFO_TBL_GATEWAY2				57
#define MYINFO_TBL_GATEWAY3				58
#define MYINFO_TBL_GATEWAY4				59
#define MYINFO_TBL_GATEWAY1_DESCR		60
#define MYINFO_TBL_GATEWAY2_DESCR		61
#define MYINFO_TBL_GATEWAY3_DESCR		62
#define MYINFO_TBL_GATEWAY4_DESCR		63
#define MYINFO_TBL_CHEATER_TEXT			64


#define GLUALLSTR(strnr)				alldattbl.gluall_tbl->get_TBL_STR(strnr)
#define NETWSTR(strnr)					alldattbl.network_tbl->get_TBL_STR(strnr)
#define MYTBLSTR(strnr)					alldattbl.myinfo_tbl->get_TBL_STR(strnr)

#define NETWORKWAITFORFILLALLGAMES		3				//wait 3 sec
#define NETWORKWAITFORSERVER			5				//wait 5 sec


struct MENUFIRSTDATA
{
		char	elemid;
		char	appearposition;
		char	disabled;
};
struct MENUAPPEARDATA
{
	short int	xpos;
	short int	ypos;
	short int	deltax;
	short int	deltay;
	PCX			*pcx;
	char		color1;
	char		color2;
	char		transvalue;
};

class FORCE_SLOTS
{
public:
	char		forces;
	char		openedslots;
	char		usedslots1[MAXPLAYERS];
	char		usedslots2[MAXPLAYERS];
	char		*forcenames[4];
	char		realplayers;
	char		playedplayers;
	char		networkplayers;
	unsigned int bitsnetwplayers;
	signed char playernr[MAXPLAYERS];
	char		force_nr[PLAYEDPLAYERS];

	void		AddForceName(int i,char *forcename);
	void		CreatePlayersNr();
	int			GetPlayerNr(int playerslot);
	void		Clear(void);
	
	~FORCE_SLOTS();
};

void LoadTransPal(char *filename,char *palette,char *menutranspcolors,float factor);
void LoadTransPal(char *filename,char *palette,char *menutranspcolors,float factor,int creationflag);

int	 campaignselect(void);
int	 xcampaignselect(void);
int	 glu_loadgame(void);
int	 glu_loadreplay(void);
int	 glu_briefing(int race,int networksingle,struct mapinfo *map,char *prefix_campaignpath, int skipmissionbutton);
int	 mainmenu(void);
int	 glu_login(void);
int	 selectmapmenu(void);

int putgamemenu(MENUDRAW *menudraw,MENUPARAMS *menuparams);
int missionobjectives(MENUDRAW *menudraw,MENUPARAMS *menuparams);
int wmission(MENUDRAW *menudraw,MENUPARAMS *menuparams);
int lmission(MENUDRAW *menudraw,MENUPARAMS *menuparams);

int	 abrtmenu(MENUDRAW *menudraw,MENUPARAMS *menuparams);
int	 quitgame(MENUDRAW *menudraw,MENUPARAMS *menuparams);
int	 restartgame(MENUDRAW *menudraw,MENUPARAMS *menuparams);
int	 leavegame(MENUDRAW *menudraw,MENUPARAMS *menuparams);
int	 puthelpfromgame(MENUDRAW *menudraw,MENUPARAMS *menuparams);
int	 optionsgame(MENUDRAW *menudraw,MENUPARAMS *menuparams);
int	 networkopt(MENUDRAW *menudraw,MENUPARAMS *menuparams);
int	 speedopt(MENUDRAW *menudraw,MENUPARAMS *menuparams);
int	 videoopt(MENUDRAW *menudraw,MENUPARAMS *menuparams);
int	 audioopt(MENUDRAW *menudraw,MENUPARAMS *menuparams);
int	 loadgame(MENUDRAW *menudraw,MENUPARAMS *menuparams);
int	 savegame(MENUDRAW *menudraw,MENUPARAMS *menuparams);


int savegametofile(void);
void loadgamefromfile(void);
int	 savegametofile_do(void);

void palchange(const char *pal,int gamma,int saturate);

int	 glu_putmenu(MENUSTR *prevmenu,const char *dialogbinfile,char *errormessage,char *fonttable,GRPFILE *dlg,
				int (*callbackfunc)(MENUSTR *,void *info),void *somecallbackdata,int sound);
int	 glu_putmenu(MENUSTR *prevmenu,const char *dialogbinfile,char *errormessage,char *fonttable,GRPFILE *dlg);
int	 glu_putmenu(MENUSTR *prevmenu,const char *dialogbinfile,char *errormessage,char *fonttable,GRPFILE *dlg,int sound);
void putokmenu(char *errormessage);
int	 putokcancelmenu(char *errormessage);
int	 glu_selectmission(MENUSTR *prevmenu,int campaign_race,char *fonttable,GRPFILE *dlg);

int	 messagingmenu(void);
MENUSTR *showtitle(void);
void cleartitle(MENUSTR *menu);
void MenuDisappear(MENUSTR *allmenus,MENUAPPEAR *items,MENUSTR *staticmenu);
MENUAPPEAR *MenuAppear(MENUSTR *allmenus,int elemnr,MENUFIRSTDATA *menudata,MENUSTR *staticmenu);
void glu_score(struct mapinfo *info);
void updatescoremenu(void);
int	 glu_conn(void);
int	 glu_join(FORCE_SLOTS *slots);
int	 glu_creat(FORCE_SLOTS *slots);
int	 glu_chat(int masterjoin,int playernr,FORCE_SLOTS *slots);
void gluchat_initmenus(MENUSTR *menu,int masterjoin,int playernr,FORCE_SLOTS *slots);
void SendTextToChatItem(MENUSTR *gluchat,int nr,char *txt);
void MakeObserver(int playernr);
void MakeDisconnect(int playernr);
int	 PauseGame(int sendornot,int playernr);
int	 ResumeGame(int sendornot,int playernr);


int	 diplomacymenu(MENUDRAW *menudraw,MENUPARAMS *menuparams);
int	 messagingmenu(MENUDRAW *menudraw,MENUPARAMS *menuparams);
int	 WaitingPlayersMenu(MENUDRAW *menudraw,MENUPARAMS *menuparams);
int	 chatboxmenu(MENUDRAW *menudraw,MENUPARAMS *menuparams);
int	 DownloadMpqMenu(int mpqremaintoload);
int	 xputokmenu(MENUDRAW *menudraw,MENUPARAMS *menuparams);
int	 xputokcancelmenu(MENUDRAW *menudraw,MENUPARAMS *menuparams);
void testmenu(void);
void createtemptables(void);
void copytempowners(struct mapinfo *testmap);
void preparegameconf_ums(void);


#endif /* _GAMEMENU_W*/

