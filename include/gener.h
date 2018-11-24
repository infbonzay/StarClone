#include "RefreshMap.h"

#if !defined(_GENER_W)
#define _GENER_W

#ifdef WITHSDL
#include "sdl/grsdl.h"
#endif

#include "commandqueue.h"
#include "gamemenu.h"
#include "netwplay.h"
#include "menu.h"
#include "maps.h"
#include "queue.h"
#include "dialogbin.h"
#include "mylist.h"
#include "mouse.h"

#define GAMEBUTTON_CLEARALL		0x00
#define GAMEBUTTON_KEYPRESS		0x01
#define GAMEBUTTON_KEYRELEASE	0x02
#define GAMEBUTTON_MOUSEPRESS	0x04
#define GAMEBUTTON_MOUSERELEASE 0x08
#define GAMEBUTTON_PRESS		(GAMEBUTTON_KEYPRESS | GAMEBUTTON_MOUSEPRESS)
#define GAMEBUTTON_RELEASE		(GAMEBUTTON_KEYRELEASE | GAMEBUTTON_MOUSERELEASE)
#define GAMEBUTTON_SHOW			0x10

#define OWNERGAMER						0
#define GUESTGAMER						1

#define CONTINUEGAME					-2
#define EXITGAME						-1
#define SINGLEGAME						1
#define MULTIPLAYERGAME					2
#define EDITORGAME						3
#define RESTARTGAME						4
#define QUITMISSION						5
#define PREVIOUSMENU					5
#define BECOMEOBSERVER					6
#define JOINGAME						7
#define CREATEGAME						8
#define CREATEDGAME						9
#define EXITCREATEDGAME					10
#define PLAYNETWORKGAME					11
#define QUITGAME						12
#define VIEWINTRO						13
#define VIEWCREDITS						14

#define MAINMENUSTATUS_LOADGAMEOK			1000
#define MAINMENUSTATUS_LOADGAMEFAILED	1001
#define MAINMENUSTATUS_SAVEGAMEFAILED	1002

#define NETWORKGAMETYPE1				9
#define NETWORKGAMETYPE2				10
#define NETWORKGAMETYPE3				11
#define NETWORKGAMETYPE4				12



#define STARTGAME						0
#define CANCELGAME						1
#define SHOWVIDEO						2
#define PREVIEWTEXT						3
#define ENDCAMPAIGN						4
#define SKIPMISSION						5


#define MAXQUEUEDELEM					1000

#define MAXCOMMANDELEM					(1000 * 24)

#define MOUSEONNONE						-2
#define MOUSEONMINIMAP					0
#define MOUSEONSTATUNIT					1
#define MOUSEONTRANSP					5
#define MOUSEONUPGRDICONS				6
#define MOUSEONICONSMIN					13
#define MOUSEONICONSMAX					21
#define MOUSEONMENUBUTTON				22
#define MOUSEONTERRAINBUTTON			23
#define MOUSEONMESSAGEBUTTON			24
#define MOUSEONDIPLOMACYBUTTON			25
#define MOUSEONMAP						26




#define F10BUTTON_PICT					0
#define TERRAINBUTTON_PICT				3
#define DIPLOMACYBUTTON_PICT			83
#define MESSAGEBUTTON_PICT				130

#define MENUBUTTON_DISABLED				0
#define MENUBUTTON_ENABLED				1
#define MENUBUTTON_CLICKED				2



void FreeQueues(void);
void transfto(int table);
int	 gogame(struct mapinfo *info);
void showramka(void);
void showiconramka(void);
void chdirgame(char *s);
void wscreenonmem(int nrregions,SCREEN_REGION regions[]);
void gameend(const char *mes);
void logend(void);
void clearallplayerinfo(void);
void clearplayersconfig(void);
int	 letsplaygame(int race,char *mypath);

void drawGAMEMENUbutton(char *button,DIALOGBIN_INFO *menuinfo,int buttonnr,
						 int dlggrp_firstpict,int mousehotnr,int stattxt_nr);

bool ShowButtonMenu(char *button,int (*adrmenu)(MENUDRAW *menudraw,MENUPARAMS *menuparams));
void ChangeTerrain(void);

int	 ChangeMenuStatus(int mstatus);
int	 IfTimeForTrigger(struct mapinfo *info,int *prevgameticks);
void ShowFirstRunVideo(int ignorefirstrunbit);
void GoPlayNextMusic(void);
void ShowGameStatusMenu(int *prevticks);
void ActivateCheat(int cheatid);


#define MINIMAPDIALOG_MAIN						0
#define MINIMAPDIALOG_MINIMAP					1
#define MINIMAPDIALOG_TERRAINBUTTON				2
#define MINIMAPDIALOG_DIPLOMACYBUTTON			3
#define MINIMAPDIALOG_MESSAGEBUTTON				4

#define F10DIALOG_MAIN							0
#define F10DIALOG_MENUBUTTON					1

extern int 				bitsplayer;

extern char				select_aria,karta_aria,mode_aria;
extern char 			f10_menu,terr_menu,mess_menu,dipl_menu;
extern DIALOGBIN_INFO	*minimapmenu,*f10menu,*statbtnmenu;
extern FORCE_SLOTS		force_slots;
extern NETW_PLAY		netplay;
extern MENUDRAW			showedmenu;
extern int				menustatus;
extern int				startmission;
extern int				campaign_id;

extern Queue			QueueGame;

extern long				commandqueuetick;
extern CommandQueue		UnitsCommandQueue;
extern HighMouse		*highMouse;
extern DestCursor		*destCursor;
extern bool				movieminikarta;
extern int				mousehotpos;
extern RefreshMap		*refreshMap;



#endif /*_GENER_W*/

