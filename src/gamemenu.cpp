

#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <signal.h>

#include <grplib/grp.h>
#include <grplib/palette.h>
#include <grplib/gr8.h>

#include "auxil.h"
#include "vars.h"
#include "mpq.h"
#include "gr.h"
#include "maps.h"
#include "pcx.h"
#include "fonts.h"
#include "debug.h"
#include "mylist.h"
#include "mouse.h"
#include "mytime.h"
#include "menu.h"
#include "music.h"
#include "scripts.h"
#include "gener.h"
#include "wmem.h"
#include "rand.h"
#include "objs.h"
#include "load.h"
#include "tempobj.h"
#include "version.hxx"
#include "player.h"
#include "dialogbin.h"
#include "network_general.h"
#include "netwgame.h"
#include "netwplay.h"
#include "stringfiles.h"
#include "lists.h"
#include "triggers.h"
#include "briefing.h"
#include "MenuItem.h"
#include "gamemenu.h"

#define CHECKFORINSTALLEXE
#define STARMENUMAP_SHOWNEW

#define GLUEPAL_OFFSET		8
char GLUEPAL_NAME[256]="glue\\Pal";

#define GLUESCORE_OFFSET	10
char GLUESCORE_NAME[256]="glue\\score";

#define GLUETITLE_OFFSET	10
char GLUETITLE_NAME[256]="glue\\title";

char BACKGND_STR[]="\\backgnd.pcx";
char FNT_STR[]="\\tFont.pcx";
char DLGGRP_STR[]="\\Dlg.grp";
char TRANS25_STR[]="\\trans25.dat";
char TRANS50_STR[]="\\trans50.dat";
char SCOREMAIN_STR[]="\\pmain.pcx";
char SCOREGRP_STR[]="\\iscore.grp";
char SCOREOPTBTN_STR[]="\\untab.pcx";
char TITLE_STR[]="\\title.pcx";

char CAMPAIGN_STR[]="Cs";
char SINGLEGAME_STR[]="Nl";
char MAINMENU_STR[]="Mm";
char DLNDMENU_STR[]="Dn";
char *PCXFRAME_STR[8]={"1.pcx","2.pcx","3.pcx","4.pcx",
			"H1.pcx","H2.pcx","H3.pcx","H4.pcx"};


#define REZBRIEF_OFFSET				10
char REZBRIEF_NAME[]="rez\\glurdy?.bin";
#define GLUEBRIEFMENUOK_OFFSET			11
char GLUEBRIEFMENUOK_NAME[]="rez\\glupokR?.bin";
#define CHATBOXDIALOGFILE_OFFSET		4
char	CHATBOXDIALOGFILE[]="rez\\?textbox.bin";

#define REZBRIEFFRAMEPCX_OFFSET1	10
#define REZBRIEFFRAMEPCX_OFFSET2	12
#define REZBRIEFFRAMEPCX_OFFSET3	18
char	REZBRIEFFRAMEPCX_NAME[]="glue\\ready?\\?Frame??.pcx";

//==========================================
MENUAPPEAR::MENUAPPEAR(int elems) : mylistsimple(elems)
{
}
//==========================================
MENUAPPEAR::~MENUAPPEAR()
{
}
//==========================================
FORCE_SLOTS::~FORCE_SLOTS()
{
    for (int i=0;i<4;i++)
	AddForceName(i,NULL);
}
//==========================================
void FORCE_SLOTS::Clear(void)
{
    int i;
    for (i=0;i<4;i++)
	AddForceName(i,NULL);
    memset(this,0,sizeof(FORCE_SLOTS));
    for (i=0;i<MAXPLAYERS;i++)
	playernr[i]=-1;
}
//==========================================
void FORCE_SLOTS::AddForceName(int forcenr,char *forcename)
{
    if (forcenames[forcenr])
    {
	wfree(forcenames[forcenr]);
	forcenames[forcenr]=NULL;
    }
    if (forcename)
    {
	forcenames[forcenr]=(char *) wmalloc(strlen(forcename)+1);
	strcpy(forcenames[forcenr],forcename);
    }
}
//==========================================
void FORCE_SLOTS::CreatePlayersNr(void)
{
    int i,curpl;
    realplayers=0;	//observers include
    playedplayers=0;	//observers exclude
    networkplayers=0;	//all network players include observers (no computers)
    for (i=0,curpl=0;i<MAXPLAYERS;i++)
    {
	if (gameconf.pl_owner[i]==OWNER_FORCENAME)
	    continue;
	if (gameconf.pl_owner[i]==OWNER_COMPUTER || gameconf.pl_owner[i]==OWNER_HUMAN || gameconf.pl_race[i]==RACE_OBSERVER)
	if (IfPlayerHaveStartLocation(&map,i)!=-1)
	{
	    if (gameconf.pl_race[i]!=RACE_OBSERVER)
	    {
		playedplayers++;
	    }
	    playernr[realplayers++]=curpl;
	}
	if (gameconf.pl_owner[i]==OWNER_HUMAN || gameconf.pl_race[i]==RACE_OBSERVER)
	{
	    networkplayers++;
	}
	curpl++;
    }
}
//==========================================
int FORCE_SLOTS::GetPlayerNr(int player)
{
    int i;
    for (i=0;i<MAXPLAYERS;i++)
	if (playernr[i]==player)
	    return(i);
}
//==========================================
int parselistfiles(mylist *flist)
{
    char *oneelem;
    for (int i=0;i<flist->GetMaxElements();i++)
    {
	oneelem=(char *)flist->GetElemNr(i);
	if (oneelem[0]==COMMANDSYMB)
	    if (oneelem[1]==ICONFORNOSELFILE || oneelem[1]==ICONFORSELFILE)
		return(i);
    }
    return(0);
}
//==========================================
void listfiles(char *fromdir,mylist *flist,char *mask,char *rootdir,char *removeext,int flags)
{
    int isdir,needaddtolist,levelup,istoplevel,nroffiles=0;
    char templistname[512];
    char tempdir[1024];
    DIR *dir;
    char *pos,*pointposition;
    struct dirent *ent;
    getcwd(tempdir,sizeof(tempdir)-1);
    if (!strcmp(tempdir,rootdir))
	istoplevel=1;
    else
	istoplevel=0;
    dir = opendir(fromdir);
    if (dir != NULL) 
    {
        while ((ent = readdir (dir)) != NULL) 
        {
	    needaddtolist=0;
	    isdir=0;
	    levelup=0;
	    if ((ent->d_type==DT_DIR||ent->d_type==DT_REG))
	    {
		if (strcmp(ent->d_name,"."))
		{
		    if (!strcmp(ent->d_name,".."))
		    {
			if (istoplevel!=1)
			{
			    needaddtolist=1;
			    isdir=1;
			    levelup=1;
			}
		    }
		    else
		    {
			if (ent->d_type==DT_DIR)
			{
			    needaddtolist=1;
			    isdir=1;
			}
			else
			{
//			    if (strstr(ent->d_name,mask))
			    pointposition = strrchr(ent->d_name,'.');
			    if (pointposition && !strncasecmp(pointposition,mask,strlen(mask)))
//			    if (!strncasecmp(ent->d_name+strlen(ent->d_name)-strlen(mask)-1,mask,strlen(mask)))
			    {
				nroffiles++;
			        needaddtolist=1;
				isdir=0;
			    }
			}
		    }
		}
	    }
	    if (needaddtolist)
	    {
		templistname[0]=0;
		if (isdir)
		{
		    if (flags&NOADDDIRTOLIST)
			continue;
		    if (flags&ADDICONSTOLIST)
		    {
			templistname[0]=COMMANDSYMB;
			templistname[1]=ICONFORCLOSENOSELDIR;
			templistname[2]='[';
			templistname[3]=0;
		    }
		    if (levelup)
		    {
			if (flags&ADDICONSTOLIST)
			    strcat(templistname,GLUALLSTR(GLUALL_TBL_UPONELEVEL));
			else
			    continue;
		    }
		    else
			strcat(templistname,ent->d_name);
		    if (flags&ADDICONSTOLIST)
			strcat(templistname,"]");
		}
		else
		{
		    if (flags&ADDICONSTOLIST)
		    {
			templistname[0]=COMMANDSYMB;
			templistname[1]=ICONFORNOSELFILE;
			templistname[2]=0;
		    }
		    strcat(templistname,ent->d_name);
		    if (removeext[0]!=0)
		    {
			pos=strstr(templistname,removeext);
			templistname[(long)pos-(long)templistname]=0;
		    }
		}
		flist->AllocAndAddList(templistname,strlen(templistname)+1);
	    }
        }
        closedir (dir);
	flist->SortA();
    } 
}
//==========================================
void listusers(char *path,mylist *flist)
{
    char *tempstr;
    DIR *dir;
    struct dirent *ent;
    dir = opendir (path);
    if (dir != NULL) 
    {
        while ((ent = readdir (dir)) != NULL) 
        {
	    if (ent->d_type==DT_REG)
	    {
		if (tempstr=strstr(ent->d_name,STARCLONEEXT))
		{
		    tempstr[0]=0;
		    flist->AllocAndAddList(ent->d_name,strlen(ent->d_name)+1);
		}
	    }
        }
        closedir (dir);
    } 
}
//==========================================
void LoadTransPal(char *filename,char *palette,char *menutranspcolors,float factor)
{
    LoadTransPal(filename,palette,menutranspcolors,factor,0);
}
//==========================================
void LoadTransPal(char *filename,char *palette,char *menutranspcolors,float factor,int creationflag)
{
//    char *filename2=filename2unix(filename);
    char *filename2=filename2temp(filename);
    FILE *f=fopen(filename2,"r");
    if (!f)
    {
	CreateSpecialTranspTable(palette,menutranspcolors,factor);
	f=fopen(filename2,"w");
	if (f)
	{
	    fwrite(menutranspcolors,256,256,f);
	    fclose(f);
	}
	else
	{
	    printf("can't create %s file\n",filename2);
	    //cant create the file :((
	}
    }
    else
    {
	if (!creationflag)
	    fread(menutranspcolors,256,256,f);
	fclose(f);
    }
    wfree(filename2);
}
//==========================================
void MenuAppearDrawCycle(MENUAPPEAR *items, MENUSTR *staticmenu, TICKCOUNTER *timer)
{
    int i,stopscript;
    MenuItemPcx *oneitem;
    char *savedscreen;
    TIMER_TICK	deltatime;
    
    savedscreen = savescreen();
    do{
	stopscript = items->GetMaxElements();
	//draw all elements
	for (i = 0; i < items->GetMaxElements(); i++)
	{
	    oneitem = (MenuItemPcx *) items->GetElem(i,NULL);
	    stopscript -= oneitem->moveaction->MoveScript();		//decrement if one script finishes
	    oneitem->Draw();
	}
	if (staticmenu)
	    checkanddrawmenu(staticmenu,ITEMNOONEACTIVE,ITEM_NOSAVELOADUNDER);
	eventwindowloop();
	putmouseonscreen();
	wscreenon();
	restorescreen(savedscreen);
	//calculate remain time to sleep (try to sleep MAXWAITMENUAPPEAR 'minus' time of draw entire one cycle)
	deltatime = MAXWAITMENUAPPEAR - mytimer.GetDeltaCounter(timer) / 1000;
	if (deltatime > 0)
	    usleep(deltatime);
    }while( stopscript );		//wait before all scripts are go at finish
    wfree(savedscreen);
}
//==========================================
void MenuDisappear(MENUAPPEAR *items,MENUSTR *staticmenu)
{
    MenuItemPcx *oneitem;
    int i;
    TICKCOUNTER *time1;
    
    time1 = mytimer.CreateTickCounter();
    Play_sfxdata_id(NULL,SFXDATA_SNDMENUOUT,-1,0);
    //enable move scripting
    for (i = 0; i < items->GetMaxElements(); i++)
    {
        oneitem = (MenuItemPcx *) items->GetElem(i,NULL);
        oneitem->moveaction->EnableMoveScript();
    }

    MenuAppearDrawCycle(items,staticmenu,time1);

    for (i = 0; i < items->GetMaxElements(); i++)
        delete (MenuItemPcx *) items->GetElem(i,NULL);
    delete items;
    items = NULL;
    mytimer.DestroyTickCounter(time1);
}
//==========================================
MENUAPPEAR *MenuAppear(MENUSTR *allmenus,int elems,MENUFIRSTDATA *menudata,MENUSTR *staticmenu)
{
    MENUAPPEAR *items;
    MenuItemPcx *oneitem;
    int i,e;
    TICKCOUNTER *time1;
    time1 = mytimer.CreateTickCounter();
    items = new MENUAPPEAR(elems);
    for (i=0; i < elems; i++)
    {
	e = menudata[i].elemid;
	if ( allmenus->menu[e].itemtype != ISIMAGE )
	    printf("Error elem(%d) is not IMAGE\n",i);
	items->AddElem( oneitem = new MenuItemPcx(allmenus->menu[e].item.image->pcx) );
	oneitem->AddMoveAction();
	oneitem->EnableVisible();
	oneitem->SetPcxParam(allmenus->menu[e].item.image->color1,
			     allmenus->menu[e].item.image->color2,
			     allmenus->menu[e].item.image->transvalue);
	oneitem->moveaction->SetMoveScript(&MoveItem::SimpleMoveScript);
	oneitem->moveaction->SetTempVars();
	switch(menudata[i].appearposition)
	{
		case MENUAPPEAR_FROMLEFT:
		    oneitem->moveaction->SetParams(1, 0, 0);
		    oneitem->SetXYPos(allmenus->menu[e].hotdeltax - oneitem->moveaction->SimpleScriptCalcMaxDistance(),
				      allmenus->menu[e].hotdeltay);
		    break;
		case MENUAPPEAR_FROMRIGHT:
		    oneitem->moveaction->SetParams(-1, 0, 0);
		    oneitem->SetXYPos(allmenus->menu[e].hotdeltax + oneitem->moveaction->SimpleScriptCalcMaxDistance(),
				      allmenus->menu[e].hotdeltay);
		    break;
		case MENUAPPEAR_FROMTOP:
		    oneitem->moveaction->SetParams(0, 1, 0);
		    oneitem->SetXYPos(allmenus->menu[e].hotdeltax,
				      allmenus->menu[e].hotdeltay - oneitem->moveaction->SimpleScriptCalcMaxDistance());
		    break;
		case MENUAPPEAR_FROMBOTTOM:
		    oneitem->moveaction->SetParams(0, -1, 0);
		    oneitem->SetXYPos(allmenus->menu[e].hotdeltax,
				      allmenus->menu[e].hotdeltay + oneitem->moveaction->SimpleScriptCalcMaxDistance());
		    break;
	}
    }
    Play_sfxdata_id(NULL,SFXDATA_SNDMENUIN,-1,0);
    //enable move scripting
    for (i = 0; i < items->GetMaxElements(); i++)
    {
        oneitem = (MenuItemPcx *) items->GetElem(i,NULL);
        oneitem->moveaction->EnableMoveScript();
    }
    mytimer.GetDeltaCounter(time1);			//reset delta time	

    MenuAppearDrawCycle(items,staticmenu,time1);

    Play_sfxdata_id(NULL,SFXDATA_SNDMENULOCK,-1,0);
    mytimer.DestroyTickCounter(time1);
    return(items);
}
//==========================================
char campaign_race[3]={STAR_PROTOSS_CAMPAIGN,STAR_TERRAN_CAMPAIGN,STAR_ZERG_CAMPAIGN};
//==========================================
int campaignselect(void)
{
    int err,exitstatus,i,mission_id,retstatus,retstatus2,campaignnr,retval,selecteditem;
    GRPFILE *dlg;
    PCX backgnd,fontpcx;
    char *menutranspcolors,*fntadr;
    char pal[256*4];

    mpqloadfile(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,CAMPAIGN_STR[0],CAMPAIGN_STR[1],DLGGRP_STR),(char **)&dlg);

    backgnd.openMpqPcx(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,CAMPAIGN_STR[0],CAMPAIGN_STR[1],BACKGND_STR));
    backgnd.readPalFromPcx(pal,0);//readfourbytepalette
    memcpy(GRP_vidmem,backgnd.GetPcxRawBytes(),backgnd.xsizePcx()*backgnd.ysizePcx());
//    backgnd.PutPcx(DELTASCREENX,DELTASCREENY2,PCX_EMPTYCOLOR1);
    fontpcx.openMpqPcx(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,CAMPAIGN_STR[0],CAMPAIGN_STR[1],FNT_STR));
    fntadr=fontpcx.GetPcxRawBytes();//for glowtext
    fntadr[21]=fntadr[11];
    fntadr[22]=fntadr[12];
    palchange(pal,gameconf.videoconf.gamma,gameconf.videoconf.saturate);
    if (!gameconf.grmode.emulationmode)
	wscreenon();
    menutranspcolors=(char *)wmalloc(2*256*256);
    LoadTransPal(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,CAMPAIGN_STR[0],CAMPAIGN_STR[1],TRANS25_STR),
		backgnd.GetRawPal768(),menutranspcolors,0.25f);
    LoadTransPal(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,CAMPAIGN_STR[0],CAMPAIGN_STR[1],TRANS50_STR),
		backgnd.GetRawPal768(),menutranspcolors+256*256,0.5f);
    SetTranspTable(menutranspcolors);

    MENUSTR *raceselection=LoadDialogBin("rez\\glucmpgn.bin",DIALOGBIN_MAINDIALOGS,IDFONT16X);
    menuspecialtables(raceselection,fntadr,dlg);
    setmenuflags(raceselection,MENUFLAGS_ALWAYSDRAW);
    if (!curplayer[0].missions[STAR_TERRAN_CAMPAIGN].campaigndone)
    {
	setmenuitem_VISIBLED(raceselection, 9,TRUE);
	setmenuitem_VISIBLED(raceselection,10,TRUE);
	setmenuitem_VISIBLED(raceselection,11,FALSE);
    }
    else
	if (!curplayer[0].missions[STAR_ZERG_CAMPAIGN].campaigndone)
	{
	    setmenuitem_VISIBLED(raceselection, 9,FALSE);
	    setmenuitem_VISIBLED(raceselection,10,FALSE);
	    setmenuitem_VISIBLED(raceselection,11,TRUE);
	}
	else
	{
	    setmenuitem_VISIBLED(raceselection, 9,FALSE);
	    setmenuitem_VISIBLED(raceselection,10,FALSE);
	    setmenuitem_VISIBLED(raceselection,11,FALSE);
	}

    mousetype=NORMALMOUSE;
    loadonecursor(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,CAMPAIGN_STR[0],CAMPAIGN_STR[1],ARROW_STR),mousetype);
    mytimer.SetMyTimerFunc(&campaignselecttimer,NULL);
    restrictmousecoords(MOUSEMODE1);
    installmousemoveevent(&mymousemoveevent);
    
    MENUFIRSTDATA menushow[2];
    menushow[0].elemid=0;
    menushow[0].appearposition=MENUAPPEAR_FROMLEFT;
    menushow[1].elemid=1;
    menushow[1].appearposition=MENUAPPEAR_FROMRIGHT;

    MENUAPPEAR *items = MenuAppear(raceselection,2,menushow,NULL);

    setdefaultbutton(raceselection,-1);
    setmenuflags(raceselection,MENUFLAGS_ALWAYSDRAW);
    int repeat=1;
    int misstoverify;
    do{
	retstatus=drawmenu(raceselection,MENUFLAGS_EMPTY);
//	doubleclick=retstatus&SELECTITEMDOUBLECLICK;
	retstatus&=~SELECTITEMDOUBLECLICK;
	switch(retstatus)
	{
	    case 2://loadsaved
		repeat=0;
		exitstatus=STAR_LOADSAVED;
		break;
	    case 3://loadreplay
		repeat=0;
		exitstatus=STAR_LOADREPLAY;
		break;
	    case 4://protoss
	    case 5://terran
	    case 6://zerg
		repeat=1;
#ifdef	CHECKFORINSTALLEXE
		if (!install1mpq)
		{
		    glu_putmenu(raceselection,"rez\\glupok2.bin",MYTBLSTR(MYINFO_TBL_ERRNOSTARINSTALL),fntadr,dlg);
		    break;
		}
#endif
		campaignnr = campaign_race[retstatus-4];
		if (retstatus==6)
		{
		    //check for terran campaign
		    if (!curplayer[0].missions[STAR_TERRAN_CAMPAIGN].campaigndone)
		    {
			retstatus2=glu_putmenu(raceselection,"rez\\glupokcancel2.bin",NETWSTR(NETWORK_TBL_ZERGCAMPAIGNERR1),fntadr,dlg);
			if (retstatus2!=1)
			{
			    //cancel operation
			    break;
			}
			//play anyway
		    }
		}
		if (retstatus==4)
		{
		    //check for terran and zerg campaign
		    if (!curplayer[0].missions[STAR_TERRAN_CAMPAIGN].campaigndone||
			!curplayer[0].missions[STAR_ZERG_CAMPAIGN].campaigndone)
		    {
			retstatus2=glu_putmenu(raceselection,"rez\\glupokcancel2.bin",NETWSTR(NETWORK_TBL_PROTOSSCAMPAIGNERR1),fntadr,dlg);
			if (retstatus2!=1)
			{
			    //cancel operation
			    break;
			}
			//play anyway
		    }
		}
		//check if need a select mission (first launch campaign)
		if (curplayer[0].missions[campaignnr].campaigndone||(curplayer[0].missions[campaignnr].seq_missions[1]&0x80))
		{
		    startmission = glu_selectmission(raceselection,campaignnr,fntadr,dlg);
		    if (startmission == -1)
			break;
		}
		else
		{
		    startmission = 0;
		}
		switch(retstatus)
		{
		    case 4:
		        exitstatus=STAR_PROTOSS;
		    	break;
		    case 5:
			exitstatus=STAR_TERRAN;
		    	break;
		    case 6:
		    	exitstatus=STAR_ZERG;
		    	break;
		}
		startmission |= (campaignnr<<8);
		repeat=0;
		break;
	    case CANCELFROMMENU://ESC
	    case 7://cancel
		repeat=0;
		exitstatus=STAR_CANCEL;
		break;
	    case 8://play custom
		    repeat=0;
		    exitstatus=STAR_PLAYCUSTOM;
		break;
	}
    }while(repeat);

    installmousemoveevent(&mymousemoveevent);

    MenuDisappear(items,NULL);

    uninstallmousemoveevent();
    mytimer.ClearMyTimerFunc();
    unloadmousecursors();

    UnloadDialogBin(raceselection);
    fontpcx.closePcx();
    SetTranspTable(NULL);
    wfree(menutranspcolors);
    backgnd.closePcx();
    wfree(dlg);
    backgnd.closePcx();
    return(exitstatus);
}
//==========================================
char xcampaign_race[3]={BROOD_ZERG_CAMPAIGN,BROOD_PROTOSS_CAMPAIGN,BROOD_TERRAN_CAMPAIGN};
//==========================================
//==========================================
int xcampaignselect(void)
{
    int err,exitstatus,i,mission_id,retstatus,retstatus2,campaignnr;
    GRPFILE *dlg;
    PCX backgnd,fontpcx;
    char *menutranspcolors,*fntadr;
    char pal[256*4];
    mpqloadfile(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,CAMPAIGN_STR[0],CAMPAIGN_STR[1],DLGGRP_STR),(char **)&dlg);
    backgnd.openMpqPcx(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,CAMPAIGN_STR[0],CAMPAIGN_STR[1],BACKGND_STR));
    backgnd.readPalFromPcx(pal,0);//readfourbytepalette
    memcpy(GRP_vidmem,backgnd.GetPcxRawBytes(),backgnd.xsizePcx()*backgnd.ysizePcx());
//    backgnd.PutPcx(DELTASCREENX,DELTASCREENY2,PCX_EMPTYCOLOR1);
    fontpcx.openMpqPcx(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,CAMPAIGN_STR[0],CAMPAIGN_STR[1],FNT_STR));
    fntadr=fontpcx.GetPcxRawBytes();//for glowtext
    fntadr[21]=fntadr[11];
    fntadr[22]=fntadr[12];
    palchange(pal,gameconf.videoconf.gamma,gameconf.videoconf.saturate);
    if (!gameconf.grmode.emulationmode)
	wscreenon();
    menutranspcolors=(char *)wmalloc(2*256*256);
    LoadTransPal(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,CAMPAIGN_STR[0],CAMPAIGN_STR[1],TRANS25_STR),
		backgnd.GetRawPal768(),menutranspcolors,0.25f);
    LoadTransPal(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,CAMPAIGN_STR[0],CAMPAIGN_STR[1],TRANS50_STR),
		backgnd.GetRawPal768(),menutranspcolors+256*256,0.5f);
    SetTranspTable(menutranspcolors);
    MENUSTR *raceselection=LoadDialogBin("rez\\gluexpcmpgn.bin",DIALOGBIN_MAINDIALOGS,IDFONT16X);
//!!!!need to correct ZERG smksizes
    raceselection->menu[4].hotxsize+=11;

    menuspecialtables(raceselection,fntadr,dlg);
    setmenuflags(raceselection,MENUFLAGS_ALWAYSDRAW);
    if (!curplayer[0].missions[BROOD_PROTOSS_CAMPAIGN].campaigndone)
    {
	setmenuitem_VISIBLED(raceselection, 9,TRUE);
	setmenuitem_VISIBLED(raceselection,10,TRUE);
	setmenuitem_VISIBLED(raceselection,11,FALSE);
    }
    else
	if (!curplayer[0].missions[BROOD_TERRAN_CAMPAIGN].campaigndone)
	{
	    setmenuitem_VISIBLED(raceselection, 9,FALSE);
	    setmenuitem_VISIBLED(raceselection,10,FALSE);
	    setmenuitem_VISIBLED(raceselection,11,TRUE);
	}
	else
	{
	    setmenuitem_VISIBLED(raceselection, 9,FALSE);
	    setmenuitem_VISIBLED(raceselection,10,FALSE);
	    setmenuitem_VISIBLED(raceselection,11,FALSE);
	}

    mousetype=NORMALMOUSE;
    loadonecursor(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,CAMPAIGN_STR[0],CAMPAIGN_STR[1],ARROW_STR),mousetype);
    mytimer.SetMyTimerFunc(&campaignselecttimer,NULL);
    restrictmousecoords(MOUSEMODE1);
    installmousemoveevent(&mymousemoveevent);

    MENUFIRSTDATA menushow[2];
    menushow[0].elemid=0;
    menushow[0].appearposition=MENUAPPEAR_FROMLEFT;
    menushow[1].elemid=1;
    menushow[1].appearposition=MENUAPPEAR_FROMRIGHT;

    MENUAPPEAR *items = MenuAppear(raceselection,2,menushow,NULL);

    setdefaultbutton(raceselection,-1);
    setmenuflags(raceselection,MENUFLAGS_ALWAYSDRAW);
    int repeat=1;
    do{
	retstatus=drawmenu(raceselection,MENUFLAGS_EMPTY);
//	doubleclick=retstatus&SELECTITEMDOUBLECLICK;
	retstatus&=~SELECTITEMDOUBLECLICK;
	switch(retstatus)
	{
	    case 2://loadsaved
		repeat=0;
		exitstatus=BROOD_LOADSAVED;
		break;
	    case 3://loadreplay
		repeat=0;
		exitstatus=BROOD_LOADREPLAY;
		break;
	    case 4://zerg
	    case 5://protoss
	    case 6://terran
		repeat=1;
#ifdef	CHECKFORINSTALLEXE
		if (!install2mpq)
		{
		    glu_putmenu(raceselection,"rez\\glupok2.bin",MYTBLSTR(MYINFO_TBL_ERRNOSTARINSTALL),fntadr,dlg);
		    break;
		}
#endif
		campaignnr = xcampaign_race[retstatus-4];
		if (retstatus==6)
		{
		    //check for protoss campaign
		    if (!curplayer[0].missions[BROOD_PROTOSS_CAMPAIGN].campaigndone)
		    {
			retstatus2=glu_putmenu(raceselection,"rez\\glupokcancel2.bin",NETWSTR(NETWORK_TBL_XTERRANCAMPAIGNERR1),fntadr,dlg);
			if (retstatus2!=1)
			{
			    //cancel operation
			    break;
			}
			//play anyway
		    }
		}
		if (retstatus==4)
		{
		    //check for protoss & terran campaign
		    if (!curplayer[0].missions[BROOD_PROTOSS_CAMPAIGN].campaigndone||
			!curplayer[0].missions[BROOD_TERRAN_CAMPAIGN].campaigndone)
		    {
			retstatus2=glu_putmenu(raceselection,"rez\\glupokcancel2.bin",NETWSTR(NETWORK_TBL_XZERGCAMPAIGNERR1),fntadr,dlg);
			if (retstatus2!=1)
			{
			    //cancel operation
			    break;
			}
			//play anyway
		    }
		}
		//check if need a select mission (first launch campaign)
		if (curplayer[0].missions[campaignnr].campaigndone||(curplayer[0].missions[campaignnr].seq_missions[1]&0x80))
		{
		    startmission = glu_selectmission(raceselection,campaignnr,fntadr,dlg);
		    if (startmission == -1)
			break;
		}
		else
		{
		    startmission = 0;
		}
		switch(retstatus)
		{
		    case 4:
		        exitstatus=BROOD_ZERG;
		    	break;
		    case 5:
			exitstatus=BROOD_PROTOSS;
		    	break;
		    case 6:
		    	exitstatus=BROOD_TERRAN;
		    	break;
		}
		startmission |= (campaignnr<<8);
		repeat=0;
		break;
	    case CANCELFROMMENU://ESC
	    case 7://cancel
		repeat=0;
		exitstatus=BROOD_CANCEL;
		break;
	    case 8://play custom
		    repeat=0;
		    exitstatus=BROOD_PLAYCUSTOM;
		break;
	}
    }while(repeat);

    installmousemoveevent(&mymousemoveevent);
    MenuDisappear(items,NULL);

    uninstallmousemoveevent();
    mytimer.ClearMyTimerFunc();
    unloadmousecursors();


    UnloadDialogBin(raceselection);
    fontpcx.closePcx();
    SetTranspTable(NULL);
    wfree(menutranspcolors);
    backgnd.closePcx();
    wfree(dlg);
    backgnd.closePcx();
    return(exitstatus);
}
//==========================================
int glu_loadgame(void)
{
    int retstatus,retstatus2,exitstatus,i,repeat,elemnr,err;
    GRPFILE *dlg;
    PCX backgnd,fontpcx;
    char *menutranspcolors,*fntadr;
    char LOADPATH[512];
    char pal[256*4];

    mpqloadfile(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],DLGGRP_STR),(char **)&dlg);
    backgnd.openMpqPcx(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],BACKGND_STR));
    backgnd.readPalFromPcx(pal,0);
    memcpy(GRP_vidmem,backgnd.GetPcxRawBytes(),backgnd.xsizePcx()*backgnd.ysizePcx());
//    backgnd.PutPcx(DELTASCREENX,DELTASCREENY2,PCX_EMPTYCOLOR1);
    palchange(pal,gameconf.videoconf.gamma,gameconf.videoconf.saturate);
    if (!gameconf.grmode.emulationmode)
	wscreenon();
    menutranspcolors=(char *)wmalloc(2*256*256);
    LoadTransPal(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],TRANS25_STR),
		backgnd.GetRawPal768(),menutranspcolors,0.25f);
    LoadTransPal(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],TRANS50_STR),
		backgnd.GetRawPal768(),menutranspcolors+256*256,0.5f);
    SetTranspTable(menutranspcolors);

    fontpcx.openMpqPcx(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],FNT_STR));
    fntadr=fontpcx.GetPcxRawBytes();//for glowtext
    fntadr[21]=fntadr[11];
    fntadr[22]=fntadr[12];

    MENUSTR *gluload=LoadDialogBin("rez\\gluload.bin",DIALOGBIN_MAINDIALOGS,IDFONT16);
    listboxlineitems(gluload,4,11,20);
    menuspecialtables(gluload,fntadr,dlg);
    setmenuflags(gluload,MENUFLAGS_ALWAYSDRAW);

    mousetype=NORMALMOUSE;
    loadonecursor(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],ARROW_STR),mousetype);
    mytimer.SetMyTimerFunc(&menutimer,NULL);
    restrictmousecoords(MOUSEMODE1);
    installmousemoveevent(&mymousemoveevent);

    MENUFIRSTDATA menushow[3];
    menushow[0].elemid=0;
    menushow[0].appearposition=MENUAPPEAR_FROMLEFT;
    menushow[1].elemid=1;
    menushow[1].appearposition=MENUAPPEAR_FROMBOTTOM;
    menushow[2].elemid=2;
    menushow[2].appearposition=MENUAPPEAR_FROMRIGHT;

    MENUAPPEAR *items = MenuAppear(gluload,3,menushow,NULL);

    mylist saves;
    do{
	strcpy(LOADPATH,SAVES_DIRECTORY);
	strcat(LOADPATH,nickname);
        strcat(LOADPATH,"/");
	mkdir(LOADPATH,0755);
        listfiles(LOADPATH,&saves,".sav",LOADPATH,".sav",NOADDDIRTOLIST);
	setlistbox_lists(gluload,4,gluload->menu[4].item.listbox->selectednr,&saves,NULL);
	if (!saves.GetMaxElements())
	{
	    setmenuitem_DISABLED(gluload,5,TRUE);	//ok
	    setmenuitem_DISABLED(gluload,7,TRUE);	//delete
	}
	else
	{
	    setmenuitem_DISABLED(gluload,5,FALSE);	//ok
	    setmenuitem_DISABLED(gluload,7,FALSE);	//delete
	}
	retstatus=drawmenu(gluload,MENUFLAGS_EMPTY);
//	doubleclick=retstatus&SELECTITEMDOUBLECLICK;
	retstatus&=~SELECTITEMDOUBLECLICK;
	repeat=1;
	switch(retstatus)
	{
	    case 4://dblclick on listmenu
	    case 5://ok
		glu_putmenu(gluload,"rez\\glupok.bin",MYTBLSTR(MYINFO_TBL_ERRLOADDISABLED),fntadr,dlg);
		break;
	    case CANCELFROMMENU://ESC
	    case 6://cancel
		repeat=0;
		exitstatus=PREVIOUSMENU;
		break;
	    case 7://delete
		elemnr=gluload->menu[4].item.listbox->selectednr;
		if (elemnr==-1)
		    break;
		strcat(LOADPATH,(char *)gluload->menu[4].item.listbox->flist->GetElemNr(elemnr));
		strcat(LOADPATH,".sav");
		retstatus2=glu_putmenu(gluload,"rez\\glupokcancel.bin",GLUALLSTR(GLUALL_TBL_DELETESAVED),fntadr,dlg);
		err=0;
		if (retstatus2==1)
		{
		    //confirmed delete file
		    err=unlink(LOADPATH);
		}
		if (err)
		{
		    glu_putmenu(gluload,"rez\\glupok.bin",NETWSTR(NETWORK_TBL_ERRDELSAVE),fntadr,dlg);
		}
		repeat=1;
		break;
	}
	saves.DeallocList();
    }while(repeat);

    installmousemoveevent(&mymousemoveevent);
    MenuDisappear(items,NULL);

    uninstallmousemoveevent();
    mytimer.ClearMyTimerFunc();
    unloadmousecursors();


    UnloadDialogBin(gluload);
    fontpcx.closePcx();
    SetTranspTable(NULL);
    wfree(menutranspcolors);
    wfree(dlg);
    backgnd.closePcx();
    return(exitstatus);
}
//==========================================
int glu_loadreplay(void)
{
    int retstatus,retstatus2,exitstatus,i,repeat,elemnr,err;
    GRPFILE *dlg;
    PCX backgnd,fontpcx;
    char *menutranspcolors,*fntadr;
    char LOADPATH[512];
    char pal[256*4];

    mpqloadfile(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],DLGGRP_STR),(char **)&dlg);
    backgnd.openMpqPcx(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],BACKGND_STR));
    backgnd.readPalFromPcx(pal,0);
    memcpy(GRP_vidmem,backgnd.GetPcxRawBytes(),backgnd.xsizePcx()*backgnd.ysizePcx());
//    backgnd.PutPcx(DELTASCREENX,DELTASCREENY2,PCX_EMPTYCOLOR1);
    palchange(pal,gameconf.videoconf.gamma,gameconf.videoconf.saturate);
    if (!gameconf.grmode.emulationmode)
	wscreenon();
    menutranspcolors=(char *)wmalloc(2*256*256);
    LoadTransPal(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],TRANS25_STR),
		backgnd.GetRawPal768(),menutranspcolors,0.25f);
    LoadTransPal(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],TRANS50_STR),
		backgnd.GetRawPal768(),menutranspcolors+256*256,0.5f);
    SetTranspTable(menutranspcolors);

    fontpcx.openMpqPcx(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],FNT_STR));
    fntadr=fontpcx.GetPcxRawBytes();//for glowtext
    fntadr[21]=fntadr[11];
    fntadr[22]=fntadr[12];

    MENUSTR *gluloadrep=LoadDialogBin("rez\\gluload.bin",DIALOGBIN_MAINDIALOGS,IDFONT16);
    listboxlineitems(gluloadrep,4,11,20);
    changetextitem(gluloadrep,3,"Save Replays");
    menuspecialtables(gluloadrep,fntadr,dlg);
    setmenuflags(gluloadrep,MENUFLAGS_ALWAYSDRAW);

    mousetype=NORMALMOUSE;
    loadonecursor(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],ARROW_STR),mousetype);
    mytimer.SetMyTimerFunc(&menutimer,NULL);
    restrictmousecoords(MOUSEMODE1);
    installmousemoveevent(&mymousemoveevent);

    MENUFIRSTDATA menushow[3];
    menushow[0].elemid=0;
    menushow[0].appearposition=MENUAPPEAR_FROMLEFT;
    menushow[1].elemid=1;
    menushow[1].appearposition=MENUAPPEAR_FROMBOTTOM;
    menushow[2].elemid=2;
    menushow[2].appearposition=MENUAPPEAR_FROMRIGHT;

    MENUAPPEAR *items = MenuAppear(gluloadrep,3,menushow,NULL);

    mylist saves;
    do{
	strcpy(LOADPATH,REPLAYS_DIRECTORY);
        listfiles(LOADPATH,&saves,".rep",LOADPATH,".rep",NOADDDIRTOLIST);
	setlistbox_lists(gluloadrep,4,gluloadrep->menu[4].item.listbox->selectednr,&saves,NULL);
	if (!saves.GetMaxElements())
	{
	    setmenuitem_DISABLED(gluloadrep,5,TRUE);	//ok
	    setmenuitem_DISABLED(gluloadrep,7,TRUE);	//delete
	}
	else
	{
	    setmenuitem_DISABLED(gluloadrep,5,FALSE);	//ok
	    setmenuitem_DISABLED(gluloadrep,7,FALSE);	//delete
	}
	retstatus=drawmenu(gluloadrep,MENUFLAGS_EMPTY);
//	doubleclick=retstatus&SELECTITEMDOUBLECLICK;
	retstatus&=~SELECTITEMDOUBLECLICK;
	repeat=1;
	switch(retstatus)
	{
	    case 4://dblclick on listmenu
	    case 5://ok
		glu_putmenu(gluloadrep,"rez\\glupok.bin",MYTBLSTR(MYINFO_TBL_ERRLOADREPL),fntadr,dlg);
		break;
	    case CANCELFROMMENU://ESC
	    case 6://cancel
		repeat=0;
		exitstatus=PREVIOUSMENU;
		break;
	    case 7://delete
		elemnr=gluloadrep->menu[4].item.listbox->selectednr;
		if (elemnr==-1)
		    break;
		strcat(LOADPATH,(char *)gluloadrep->menu[4].item.listbox->flist->GetElemNr(elemnr));
		strcat(LOADPATH,".rep");
		retstatus2=glu_putmenu(gluloadrep,"rez\\glupokcancel.bin",MYTBLSTR(MYINFO_TBL_DELREPL),fntadr,dlg);
		err=0;
		if (retstatus2==1)
		{
		    //confirmed delete file
		    err=unlink(LOADPATH);
		}
		if (err)
		{
		    glu_putmenu(gluloadrep,"rez\\glupok.bin",MYTBLSTR(MYINFO_TBL_ERRDELREPL),fntadr,dlg);
		}
		repeat=1;
		break;
	}
	saves.DeallocList();
    }while(repeat);

    installmousemoveevent(&mymousemoveevent);
    MenuDisappear(items,NULL);

    uninstallmousemoveevent();
    mytimer.ClearMyTimerFunc();
    unloadmousecursors();


    UnloadDialogBin(gluloadrep);
    fontpcx.closePcx();
    SetTranspTable(NULL);
    wfree(menutranspcolors);
    wfree(dlg);
    backgnd.closePcx();
    return(exitstatus);
}
//==========================================
int BRIEFING_ELEMS[3]={2,1,1};
int BRIEFING_TRANSLUCENCY=138;
//==========================================
int BriefingTriggersCheck(MENUSTR *allmenus,void *data)
{
    struct mapinfo *info;
    if (menutimerupdate)
    {
	menutimerupdate=0;
	info=(struct mapinfo *)data;
	Briefing_Parce(info,allmenus,1000/(60/TIMETOMAINMENUUPDATE));
    }
    return(0);    
}
//==========================================
#define NETW_WAITFORPLAYERS_DONE	1000
#define NETW_WAITFORPLAYERS_TIMEOUT	1001
#define NETW_WAITFORPLAYERS_ERRNET	1002
#define WAITFORBRIEFINGSEND		1003
//==========================================
int CheckOtherPlayers(MENUSTR *allmenus,void *data)
{
    static int timeout=0,firsttime=0;
    int okconnect=0;
    ALLNETWCONNECTION *conn;
    if (menutimerupdate)
    {
	menutimerupdate=0;
	conn=(ALLNETWCONNECTION *)data;
	timeout++;
	if (timeout>=(60/TIMETOMAINMENUUPDATE)*1)	//after 1sec check network
	{
	    if (conn->netwplayers==1)
	    {
		okconnect=1;
	    }
	    else
	    {
		netplay.ReceiveGame_InfoFromPlayers(conn);
		if (Check_IfServerPlayer())
		{
		    if (netplay.Check_ReceiveFromPlayers())
		    {
			okconnect=1;
			//send to all about to let's go to play
			netplay.Server_SendInfoToPlayers(conn,SC_NETCMD_BRIEF,GAMETICK_GAMEBRIEF,NULL,SENDPARAM_SENDTOALLPLAYERS,1,0);
		    }
		}
		else
		{
		    if (netplay.Check_ReceiveFromServer())
		    {
			okconnect=1;
			//info comming from server so we can play
		    }
		    if (firsttime==0||(timeout%(5*60/TIMETOMAINMENUUPDATE)==0))	//every 5 sec try to send info to server if no responces
		    {
			//send to server i'm ready
			netplay.Client_SendInfoToServer(conn,SC_NETCMD_BRIEF,GAMETICK_GAMEBRIEF,NULL,1,0);
			firsttime=1;
		    }
		}
	    }
	    if (okconnect)
	    {
		//all players synchronized
		firsttime=0;
		timeout=0;
		gameconf.speedconf.gamespeed=GAMESPEED;
		netplay.Make_AllNetCmdsBRIEF();
		netplay.Next_CurrentPacketTick();
		return(NETW_WAITFORPLAYERS_DONE);
	    }
	}
	if (timeout>=(60/TIMETOMAINMENUUPDATE)*9)	//9 seconds of timeout
	{
	    firsttime=0;
	    timeout=0;
	    return(NETW_WAITFORPLAYERS_TIMEOUT);
	}
    }
    return(0);
}
//==========================================
#define FRAMEPCXS	8
int glu_briefing(int race,int networksingle,struct mapinfo *info,char *prefix_campaignpath)
{
    int retstatus,retstatus2,exitstatus,i,repeat,e,err;
    int totalpcx;
    PCX backgnd,fontpcx,framepcxs[FRAMEPCXS];
    char *menutranspcolors,*fntadr;
    char pal[256*4];
    MENUSTR *errmenu,*glubrief;
    
    StopMusic(MUSIC_STOPWITHFADE);
    sprintf(FULLFILENAME,"music\\%crdyroom.wav",RACE_CHAR[race]);
    PlayMusic(FULLFILENAME,-1);

    backgnd.openMpqPcx(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,'R',RACE_CHAR[race],BACKGND_STR));
    backgnd.readPalFromPcx(pal,0);
    memcpy(GRP_vidmem,backgnd.GetPcxRawBytes(),backgnd.xsizePcx()*backgnd.ysizePcx());
//    backgnd.PutPcx(DELTASCREENX,DELTASCREENY2,PCX_EMPTYCOLOR1);
    palchange(pal,gameconf.videoconf.gamma,gameconf.videoconf.saturate);
    if (!gameconf.grmode.emulationmode)
	wscreenon();
    menutranspcolors=(char *)wmalloc(2*256*256);
    LoadTransPal(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,'R',RACE_CHAR[race],TRANS25_STR),
		backgnd.GetRawPal768(),menutranspcolors,0.25f);
    LoadTransPal(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,'R',RACE_CHAR[race],TRANS50_STR),
		backgnd.GetRawPal768(),menutranspcolors+256*256,0.5f);
    SetTranspTable(menutranspcolors);

    fontpcx.openMpqPcx(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,'R',RACE_CHAR[race],FNT_STR));
    fntadr=fontpcx.GetPcxRawBytes();//for glowtext
    fntadr[21]=fntadr[11];
    fntadr[22]=fntadr[12];
    for(i=0;i<FRAMEPCXS;i++)
    {
	REZBRIEFFRAMEPCX_NAME[REZBRIEFFRAMEPCX_OFFSET1]=RACE_CHAR[race];
	REZBRIEFFRAMEPCX_NAME[REZBRIEFFRAMEPCX_OFFSET2]=RACE_CHAR[race];
	framepcxs[i].openMpqPcx(makefilename(REZBRIEFFRAMEPCX_NAME,REZBRIEFFRAMEPCX_OFFSET3,0,0,PCXFRAME_STR[i]));
    }

    glubrief=LoadDialogBin(makefilename(REZBRIEF_NAME,REZBRIEF_OFFSET,RACE_CHAR[race],'.',NULL),
				    DIALOGBIN_MAINDIALOGS,IDFONT16);

    menuspecialtables(glubrief,fntadr,NULL);
    setmenuflags(glubrief,MENUFLAGS_ALWAYSDRAW);

    mousetype=NORMALMOUSE;
    loadonecursor(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,'R',RACE_CHAR[race],ARROW_STR),mousetype);
    mytimer.SetMyTimerFunc(&menutimer,NULL);
    restrictmousecoords(MOUSEMODE1);
    installmousemoveevent(&mymousemoveevent);

    e=BRIEFING_ELEMS[race];

    MENUFIRSTDATA menushow[7];
    totalpcx=7-networksingle;
    menushow[0].elemid=e;
    menushow[0].appearposition=MENUAPPEAR_FROMRIGHT;
    menushow[1].elemid=e+1;
    menushow[1].appearposition=MENUAPPEAR_FROMLEFT;
    menushow[2].elemid=e+3;
    menushow[2].appearposition=MENUAPPEAR_FROMLEFT;
    menushow[3].elemid=e+4;
    menushow[3].appearposition=MENUAPPEAR_FROMTOP;
    menushow[4].elemid=e+5;
    menushow[4].appearposition=MENUAPPEAR_FROMTOP;
    menushow[5].elemid=e+6;
    menushow[5].appearposition=MENUAPPEAR_FROMRIGHT;
    if (networksingle)
    {
	setmenuitem_VISIBLED(glubrief,e+2,FALSE);	//pcx replay
	setmenuitem_VISIBLED(glubrief,e+10,FALSE);	//replay button
	glubrief->menu[e+7].dialogbin_flags&=~DIALOGBIN_FLAGS_RESPONDTOESCKEY;//prevent press ESC key
    }
    else
    {
	menushow[6].elemid=e+2;
	menushow[6].appearposition=MENUAPPEAR_FROMBOTTOM;
    }
    for (i=e+0;i<e+7;i++)
    {
	glubrief->menu[i].item.image->color2=BRIEFING_TRANSLUCENCY;
    }
    
    GLUEBRIEFMENUOK_NAME[GLUEBRIEFMENUOK_OFFSET]=RACE_CHAR[race];

    errmenu=LoadDialogBin(GLUEBRIEFMENUOK_NAME,DIALOGBIN_MAINDIALOGS,IDFONT16);
    errmenu->menu[0].item.image->color2=BRIEFING_TRANSLUCENCY;
    menuspecialtables(errmenu,fntadr,NULL);
    setmenuflags(errmenu,MENUFLAGS_ALWAYSDRAW);
    AddPrevMenuShowing(errmenu,glubrief);
    
//    DEBUGMES("SERVERGAMER=%d\n",SERVERGAMER);
    if (networksingle)
    {

	setmenuitem_VISIBLED(glubrief,e+17,TRUE);//waiting for other players
	setmenuitem_DISABLED(glubrief,0,TRUE);//start
	setmenuitem_DISABLED(glubrief,e+7,TRUE);//cancel

	netplay.InitNetworkTicks(GAMETICK_GAMEBRIEF);
        err=CreateAllConnectionsWithPlayers(&playersconn);
        if (!err)
        {
	    playersconn.bitsnetwplayers=force_slots.bitsnetwplayers;
	    playersconn.netwplayers=force_slots.networkplayers;
	    AddMenu_SomeCallback(glubrief,&CheckOtherPlayers,&playersconn);
	}
	else
	{
	    changetextitem(errmenu,2,MYTBLSTR(MYINFO_TBL_NEWGAMENETERR));
	    DestroyAllConnectionsWithPlayers(&playersconn);
	    exitstatus=CANCELGAME;
	}
    }
    else
    {
	err=0;
    }
    MENUAPPEAR *items;
    if (err)
	items = MenuAppear(glubrief,totalpcx,menushow,errmenu);
    else
	items = MenuAppear(glubrief,totalpcx,menushow,NULL);

    memcpy(GRP_vidmem,backgnd.GetPcxRawBytes(),backgnd.xsizePcx()*backgnd.ysizePcx());
//    backgnd.PutPcx(DELTASCREENX,DELTASCREENY2,PCX_EMPTYCOLOR1);

    if (info&&info->BRIEFS_NR&&!networksingle&&GAMETYPE==MAP_GAMETYPE_USEMAPSETTINGS)
    {
        glubrief->menu[e+15].fontnr=IDFONT16;
        glubrief->menu[e+15].colors4=glubrief->menu[e+18].colors4;
        glubrief->menu[e+15].hotysize++;//113+1

        glubrief->menu[e+15].item.textitem->rowsize=glubrief->menu[e+18].item.textitem->rowsize;
        First_Briefing_Prepare(info,e,prefix_campaignpath,framepcxs);
	AddMenu_SomeCallback(glubrief,&BriefingTriggersCheck,info);
    }
    else
    {
	fill_missionobjectives(MYTBLSTR(MYINFO_TBL_GOAL1));
	setmenuitem_VISIBLED(glubrief,e+18,TRUE);//no briefing data
    }
    setmenuitem_VISIBLED(glubrief,e+16,TRUE);
    changetextitem(glubrief,e+16,missionobj);

    changemenuitemtype(glubrief,e+11,ISIMAGE,IDFONT8,MTEXTCOLOR1);
    changemenuitemtype(glubrief,e+12,ISIMAGE,IDFONT8,MTEXTCOLOR1);
    changemenuitemtype(glubrief,e+13,ISIMAGE,IDFONT8,MTEXTCOLOR1);
    changemenuitemtype(glubrief,e+14,ISIMAGE,IDFONT8,MTEXTCOLOR1);
    
    addimagearray(glubrief,e+11,-1,-1,-1,-1);
    addimagearray(glubrief,e+12,-1,-1,-1,-1);
    addimagearray(glubrief,e+13,-1,-1,-1,-1);
    addimagearray(glubrief,e+14,-1,-1,-1,-1);
    
    setimgtransparentcolors(glubrief,e+11,0,255);
    setimgtransparentcolors(glubrief,e+12,0,255);
    setimgtransparentcolors(glubrief,e+13,0,255);
    setimgtransparentcolors(glubrief,e+14,0,255);

    repeat=1;
    do{
	if (!err)
	{
	    retstatus=drawmenu(glubrief,MENUFLAGS_EMPTY);
	}
	else
	{
	    RemoveMenu_Callback(glubrief);
	    drawmenu(errmenu,MENUFLAGS_EMPTY);
	    repeat=0;
	    exitstatus=CANCELGAME;
	    break;
	}
	if (retstatus==NETW_WAITFORPLAYERS_DONE||retstatus==0)//||retstatus==WAITFORBRIEFINGSEND)
	{
	    repeat=0;
	    exitstatus=STARTGAME;
	}
	else if (retstatus==NETW_WAITFORPLAYERS_TIMEOUT)
	{
	    RemoveMenu_Callback(glubrief);
	    changetextitem(errmenu,2,MYTBLSTR(MYINFO_TBL_BRIEFTIMEOUT));
	    drawmenu(errmenu,MENUFLAGS_EMPTY);
	    DestroyAllConnectionsWithPlayers(&playersconn);
	    repeat=0;
	    exitstatus=CANCELGAME;
	    //need some message
	}
	else if (retstatus==e+10)
	{
	    repeat=1;
	    if (info&&info->BRIEFS_NR&&!networksingle&&GAMETYPE==MAP_GAMETYPE_USEMAPSETTINGS)
	    {
		RemoveMenu_Callback(glubrief);
		Reload_Briefing(info,glubrief);
		AddMenu_SomeCallback(glubrief,&BriefingTriggersCheck,info);
	    }
	}else if (retstatus==e+7 || retstatus==CANCELFROMMENU)
	{
	    repeat=0;
	    exitstatus=CANCELGAME;
	}
    }while(repeat);
    Remove_Briefing(info);

    StopMusic(MUSIC_STOPWITHFADE);

    installmousemoveevent(&mymousemoveevent);
    MenuDisappear(items,NULL);

    uninstallmousemoveevent();
    mytimer.ClearMyTimerFunc();
    unloadmousecursors();


    UnloadDialogBin(errmenu);
    UnloadDialogBin(glubrief);
    for (i=0;i<FRAMEPCXS;i++)
	framepcxs[i].closePcx();
    fontpcx.closePcx();
    SetTranspTable(NULL);
    wfree(menutranspcolors);
    backgnd.closePcx();
    return(exitstatus);
}
//==========================================
MENUSTR *gluscore;
short int gluscore_volumechannel=-1;
void updatescoremenu(void)
{
    int pl,i,j,jj,flags,total,selectedlist,deltaincr;
    char txt[90];
    if (gluscore)
    if (!(gluscore->mainmenuflags&DIALOGBIN_FLAGS_NONEEDRECALCINFO))
    {
	flags=0;
	selectedlist=getradiobuttonstate(gluscore,2)-2;
	for (pl=0;pl<force_slots.realplayers;pl++)
	{
	    i=force_slots.playernr[pl];
	    if (map.pl_race[i]==RACE_OBSERVER)
		continue;
	    total=0;
	    for (j=0;j<3;j++)
	    {
		jj=j+selectedlist*3;
		deltaincr=map.score[i][jj]/(TIMETOCHANGESCOREMENU*100*3/60);
		if (!deltaincr)
		    deltaincr++;
		map.tempscore[i][j]+=deltaincr;
		if (map.tempscore[i][j]>=map.score[i][jj])
		{
		    map.tempscore[i][j]=map.score[i][jj];
		    flags++;
		}
		if (selectedlist==3 && j==2)
		    ;
		else
		    total+=map.tempscore[i][j];
		sprintf(txt,"%%%d %d",map.tempscore[i][j],map.maxscore[jj]);
		changetextitem(gluscore,pl*6+15+j,txt);
	    }
	    sprintf(txt,"%d",total);					//total
	    changetextitem(gluscore,pl*6+15+3,txt);			//total
	}
	if (flags==3*force_slots.realplayers)
	{
	    //all updates is done no need refresh
	    gluscore->mainmenuflags|=DIALOGBIN_FLAGS_NONEEDRECALCINFO;
	    //stop scorefill sound and lauch endsound
	    if (gluscore_volumechannel!=-1)
	    {
		StopPlayChannel(gluscore_volumechannel);
		gluscore_volumechannel=-1;
		i=Play_sfxdata_id(NULL,SFXDATA_SCOREFILLEND,-1,0);
	    }
	}
    }
}
//==========================================
void cleartempscore(struct mapinfo *info)
{
    int i,pl;
    for (pl=0;pl<force_slots.realplayers;pl++)
    {
        i=force_slots.playernr[pl];
	if (info->pl_race[i]==RACE_OBSERVER)
	    continue;
	info->tempscore[i][0]=0;
	info->tempscore[i][1]=0;
	info->tempscore[i][2]=0;
    }
    gluscore->mainmenuflags&=~DIALOGBIN_FLAGS_NONEEDRECALCINFO;
}
//==========================================
void initscoreinfo(struct mapinfo *info,int fromoptid)
{
    int i,j,pl;
    char txt[90];
    for (pl=0;pl<force_slots.realplayers;pl++)
    {
        i=force_slots.playernr[pl];
	if (info->pl_race[i]==RACE_OBSERVER)
	    continue;
	for (j=0;j<3;j++)
	{
	    sprintf(txt,"%%%d %d",0,info->maxscore[j+fromoptid]);
	    changetextitem(gluscore,pl*6+15+j,txt);
	    gluscore->menu[pl*6+15+j].colors4=PLAYER[i].colorRACE;// color of bar percentage
	}
	changetextitem(gluscore,pl*6+15+3,"0");
    }
    //check if it is play
    if (gluscore_volumechannel==-1)
    {
	i=Play_sfxdata_id(NULL,SFXDATA_SCOREFILL,-2,0);
	if (i>=0)
	    gluscore_volumechannel=i;
    }
}
//==========================================
void setmaxscore(struct mapinfo *info)
{
    int i,j,k,val,pl,maxscore;
    for (pl=0;pl<force_slots.realplayers;pl++)
    {
        i=force_slots.playernr[pl];
	if (info->pl_race[i]==RACE_OBSERVER)
	    continue;
	for (j=0;j<3;j++)
	{
//	    if (j<2)
//		maxscore=3;
//	    else
//		maxscore=2;
	    info->score[i][j] = 0;
//	    for (k=0;k<maxscore;k++)
	    for (k=0;k<3;k++)
		info->score[i][j] += info->score[i][3+j*3+k];
	}
	info->score[i][2] = info->score[i][3+2*3+2];
    }
    for (j=0;j<12;j++)
    {
	val=0;
	for (pl=0;pl<force_slots.realplayers;pl++)
	{
	    i=force_slots.playernr[pl];
	    if (info->pl_race[i]==RACE_OBSERVER)
		continue;
	    if (info->score[i][j]>val)
		val=info->score[i][j];
	}
	info->maxscore[j]=val;
    }
}
//==========================================
int SCORE_TRANSLUCENCY[3][2]={{147,9},{9,95},{9,9}};
int races_clan[3]={NETWORK_TBL_ZERGCLANNAMES,NETWORK_TBL_TERRANCLANNAMES,NETWORK_TBL_PROTOSSCLANNAMES};
void glu_score(struct mapinfo *info)
{
    int retstatus,repeat,i,j,pl,selectedlist;
    PCX backgnd,fontpcx,optbtn;
    GRPFILE *dlg,*scoregrp;
    char *menutranspcolors,*fntadr,*forcename;
    int race,losewin,myrace;
    char pal[256*4];
    char txt[90];
    char txt2[15];

    race=info->pl_race[info->played_as_nr];
    if (race==RACE_OBSERVER)
	race=RACE_TERRAN;
    losewin=(map.flags & STARMAP_FLAG_WINGAME)!=0;
    
    StopMusic(MUSIC_STOPWITHFADE);
    sprintf(FULLFILENAME,"music\\%c%s.wav",RACE_CHAR[race],DEFEAT_VICTORY[losewin]);
    PlayMusic(FULLFILENAME,-1);

    mpqloadfile(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,RACE_CHAR[race],LOSEWIN_STR[losewin],DLGGRP_STR),(char **)&dlg);
    mpqloadfile(makefilename(GLUESCORE_NAME,GLUESCORE_OFFSET,RACE_CHAR[race],LOSEWIN_STR[losewin],SCOREGRP_STR),(char **)&scoregrp);
    optbtn.openMpqPcx(makefilename(GLUESCORE_NAME,GLUESCORE_OFFSET,RACE_CHAR[race],LOSEWIN_STR[losewin],SCOREOPTBTN_STR));

    backgnd.openMpqPcx(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,RACE_CHAR[race],LOSEWIN_STR[losewin],BACKGND_STR));

    backgnd.readPalFromPcx(pal,0);
    memcpy(GRP_vidmem,backgnd.GetPcxRawBytes(),backgnd.xsizePcx()*backgnd.ysizePcx());
//    backgnd.PutPcx(DELTASCREENX,DELTASCREENY2,PCX_EMPTYCOLOR1);
    palchange(pal,gameconf.videoconf.gamma,gameconf.videoconf.saturate);
    if (!gameconf.grmode.emulationmode)
	wscreenon();
    menutranspcolors=(char *)wmalloc(2*256*256);
    LoadTransPal(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,RACE_CHAR[race],LOSEWIN_STR[losewin],TRANS25_STR),
		backgnd.GetRawPal768(),menutranspcolors,0.25f);
    LoadTransPal(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,RACE_CHAR[race],LOSEWIN_STR[losewin],TRANS50_STR),
		backgnd.GetRawPal768(),menutranspcolors+256*256,0.5f);
    SetTranspTable(menutranspcolors);

    fontpcx.openMpqPcx(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,RACE_CHAR[race],LOSEWIN_STR[losewin],FNT_STR));
    fntadr=fontpcx.GetPcxRawBytes();//for glowtext
    fntadr[21]=fntadr[11];
    fntadr[22]=fntadr[12];


    gluscore=LoadDialogBin("rez\\gluscore.bin",DIALOGBIN_MAINDIALOGS,IDFONT16);
//    setmenuitem_DISABLED(gluscore,7,TRUE);
    setradiobuttonstate(gluscore,2);
    menuspecialtables(gluscore,fntadr,scoregrp);
    setmenuflags(gluscore,MENUFLAGS_ALWAYSDRAW);
    changeimageitem(gluscore,0,makefilename(GLUESCORE_NAME,GLUESCORE_OFFSET,RACE_CHAR[race],LOSEWIN_STR[losewin],SCOREMAIN_STR));
    gluscore->menu[0].item.image->color2=SCORE_TRANSLUCENCY[race][losewin];

    addradiobuttonimg(gluscore,2,NULL,&optbtn,0,28);
    addradiobuttonimg(gluscore,3,NULL,&optbtn,0,28);
    addradiobuttonimg(gluscore,4,NULL,&optbtn,0,28);
    addradiobuttonimg(gluscore,5,NULL,&optbtn,0,28);

    gluscore->menu[2].dialogbin_flags|=DIALOGBIN_FLAGS_NODLGGRP;
    gluscore->menu[3].dialogbin_flags|=DIALOGBIN_FLAGS_NODLGGRP;
    gluscore->menu[4].dialogbin_flags|=DIALOGBIN_FLAGS_NODLGGRP;
    gluscore->menu[5].dialogbin_flags|=DIALOGBIN_FLAGS_NODLGGRP;
    
    changetextitem(gluscore,1,GLUALLSTR(GLUALL_TBL_LOSEWIN+losewin));
    if (info->secondsplayed/3600)
    {
	sprintf(txt2,"%02d:%02d:%02d",info->secondsplayed/3600,(info->secondsplayed/60)%60,info->secondsplayed%60);
    }
    else
    {
	sprintf(txt2,"% 2d:%02d",info->secondsplayed/60,info->secondsplayed%60);
    }
    sprintf(txt,GLUALLSTR(GLUALL_TBL_ELAPSEDTIME),txt2);
    changetextitem(gluscore,9,txt);

    mytimer.SetMyTimerFunc(&scoremenutimer,NULL);
    restrictmousecoords(MOUSEMODE1);
    installmousemoveevent(&mymousemoveevent);
    mousetype=NORMALMOUSE;
    loadonecursor(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,RACE_CHAR[race],LOSEWIN_STR[losewin],ARROW_STR),mousetype);

    MENUFIRSTDATA menushow[1];
    menushow[0].elemid=0;
    menushow[0].appearposition=MENUAPPEAR_FROMLEFT;

    MENUAPPEAR *items = MenuAppear(gluscore,1,menushow,NULL);
    memcpy(GRP_vidmem,backgnd.GetPcxRawBytes(),backgnd.xsizePcx()*backgnd.ysizePcx());
//    backgnd.PutPcx(DELTASCREENX,DELTASCREENY2,PCX_EMPTYCOLOR1);


//	only for test how it works random numbers

/*    for (pl=0;pl<force_slots.realplayers;pl++)
    {
        i=force_slots.playernr[pl];
	for (j=3;j<12;j++)
//	    info->score[i][j]=myrand(20000)+1000;
	    info->score[i][j]=(i+1)*1+j;
    }
*/    
    for (pl=0;pl<force_slots.realplayers;pl++)
    {
        i=force_slots.playernr[pl];
	info->score[i][3]=PLAYER[i].statplayer.stat[STATPLAYER_UNITSPRODUCED];
	info->score[i][4]=PLAYER[i].statplayer.stat[STATPLAYER_UNITSKILLED];
	info->score[i][5]=PLAYER[i].statplayer.stat[STATPLAYER_UNITSLOST];
	
	info->score[i][6]=PLAYER[i].statplayer.stat[STATPLAYER_BUILDSCONSTRUCTED];
	info->score[i][7]=PLAYER[i].statplayer.stat[STATPLAYER_BUILDSRISED];
	info->score[i][8]=PLAYER[i].statplayer.stat[STATPLAYER_BUILDSLOST];
	
	info->score[i][9]=PLAYER[i].statplayer.stat[STATPLAYER_GASMINED];
	info->score[i][10]=PLAYER[i].statplayer.stat[STATPLAYER_MINERALSMINED];
	info->score[i][11]=PLAYER[i].statplayer.stat[STATPLAYER_GASMINED]+PLAYER[i].statplayer.stat[STATPLAYER_MINERALSMINED];
    }
    setmaxscore(info);
    cleartempscore(info);
    initscoreinfo(info,getradiobuttonstate(gluscore,2)-2);

    repeat=1;

    int races[3]={0,0,0};
    for (pl=0;pl<force_slots.realplayers;pl++)
    {
    	    i=force_slots.playernr[pl];
	    myrace=info->pl_race[i];
//	    if (myrace==RACE_OBSERVER)
//		continue;
	    if (GAMETYPE==MAP_GAMETYPE_USEMAPSETTINGS)
	    {
		    int forcenr=info->pl_force.force_nr[i];
		    int forceid=info->pl_force.force_ID[forcenr];
		    if (forceid>0)
			forcename=getmapSTR(info,forceid-1);
		    else
			forcename=&EMPTYSTR;
		    
		    sprintf(txt,"%s\n%c%c%c%c%c      %c%c%c%c %s",NETWSTR(races_clan[myrace]+races[myrace]),
	    		    COMMANDSYMB,DLGGRPSYMB,myrace+3,10,10,
			    COMMANDSYMB,CHANGEFONTSYMB,IDFONT10,
			    MWHITECOLORFONT,forcename);
	    }
	    else
	    {
		    sprintf(txt,"%s\n%c%c%c%c%c      %c%c%c%c %s",NETWSTR(races_clan[myrace]+races[myrace]),
	    		    COMMANDSYMB,DLGGRPSYMB,myrace+3,10,10,
			    COMMANDSYMB,CHANGEFONTSYMB,IDFONT10,
			    MWHITECOLORFONT,getplayername(i));
	    }
	    races[myrace]++;
	    changetextitem(gluscore,pl*6+14,txt);
	    gluscore->menu[pl*6+14].item.textitem->rowsize=12;
	    setmenuitem_VISIBLED(gluscore,pl*6+13,TRUE);
    }
    do{
	selectedlist=getradiobuttonstate(gluscore,2);
	for (i=0;i<3;i++)
	{
	    changetextitem(gluscore,i+10,GLUALLSTR(GLUALL_TBL_SCORETEXT+(selectedlist-2)*3+i));
	}
	retstatus=drawmenu(gluscore,MENUFLAGS_EMPTY);
	switch(retstatus)
	{
	    case 2:
	    case 3:
	    case 4:
	    case 5:
		if (selectedlist!=retstatus)
		{
		    cleartempscore(info);
		    initscoreinfo(info,(retstatus-2)*3);
		    setdefaultbutton(gluscore,retstatus);
		}
		break;
	    case CANCELFROMMENU://ESC
	    case 6://ok
		repeat=0;
		break;
	    case 7://save replay
		break;
	    
	}
    }while(repeat);
    
    if (gluscore_volumechannel!=-1)
    {
	StopPlayChannel(gluscore_volumechannel);
	gluscore_volumechannel=-1;
    }

    StopMusic(MUSIC_STOPWITHFADE);

    installmousemoveevent(&mymousemoveevent);
    MenuDisappear(items,NULL);

    uninstallmousemoveevent();
    mytimer.ClearMyTimerFunc();
    unloadmousecursors();
    //stop wav if not stopped


    UnloadDialogBin(gluscore);


    gluscore=NULL;
    optbtn.closePcx();
    fontpcx.closePcx();
    SetTranspTable(NULL);
    wfree(menutranspcolors);
    wfree(scoregrp);
    wfree(dlg);
    backgnd.closePcx();
    return;
}
//==========================================
int mainmenu(void)
{
    PCX backgnd,fontpcx;
    char newpal[256*4];
    char *fntadr;
    backgnd.openMpqPcx(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,MAINMENU_STR[0],MAINMENU_STR[1],BACKGND_STR));
    backgnd.readPalFromPcx(newpal,0);//readfourbytepalette
    memcpy(GRP_vidmem,backgnd.GetPcxRawBytes(),backgnd.xsizePcx()*backgnd.ysizePcx());
//    backgnd.PutScaledPcx(DELTASCREENX,DELTASCREENY2,0);
    fontpcx.openMpqPcx(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,MAINMENU_STR[0],MAINMENU_STR[1],FNT_STR));
    fntadr=fontpcx.GetPcxRawBytes();//for glowtext
    fntadr[21]=fntadr[11];
    fntadr[22]=fntadr[12];
    palchange(newpal,gameconf.videoconf.gamma,gameconf.videoconf.saturate);
    if (!gameconf.grmode.emulationmode)
	wscreenon();
    loadonecursor(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,MAINMENU_STR[0],MAINMENU_STR[1],ARROW_STR),NORMALMOUSE);
    mytimer.SetMyTimerFunc(&mainmenutimer,NULL);
    restrictmousecoords(MOUSEMODE1);
    installmousemoveevent(&mymousemoveevent);
    MENUSTR *mainmenu=LoadDialogBin("rez\\glumain.bin",DIALOGBIN_MAINDIALOGS,IDFONT16X);
    menuspecialtables(mainmenu,fntadr,NULL);
    changetextitem(mainmenu,9,GAMEVERSION);
    setmenuflags(mainmenu,MENUFLAGS_ALWAYSDRAW);
    int retstatus,repeat=1,retvalue;
    do{
	retstatus=drawmenu(mainmenu,MENUFLAGS_EMPTY);
	switch(retstatus)
	{
	    case CANCELFROMMENU:
	    case 1://cancel
		repeat=0;
		retvalue=EXITGAME;
		break;
	    case 2:
		repeat=0;
		retvalue=SINGLEGAME;
		break;
	    case 3:
		repeat=0;
		retvalue=MULTIPLAYERGAME;
		break;
	    case 4:
		repeat=0;
		retvalue=EDITORGAME;
		break;
	    case 7:
		repeat=0;
		retvalue=VIEWINTRO;
		break;
	    case 8:
		repeat=0;
		retvalue=VIEWCREDITS;
		break;
	}
    }while(repeat);

    UnloadDialogBin(mainmenu);
    uninstallmousemoveevent();
    mytimer.ClearMyTimerFunc();
    unloadmousecursors();

    fontpcx.closePcx();
    backgnd.closePcx();
    return retvalue;
}
//==========================================
char CONNMAXPLAYERS[MAXCONNTYPES]={8,2,2,8};
//==========================================
void selconn_callback(MENUSTR *allmenus,int nr,int listnr)
{
    char txt[50];
    if (listnr!=-1)
    {
	changetextitem(allmenus, 10,MYTBLSTR(MYINFO_TBL_NETTYPE1+listnr));			//connection type
	sprintf(txt,GLUALLSTR(GLUALL_TBL_MAXPLAYERS),CONNMAXPLAYERS[listnr]);
	changetextitem(allmenus,11,txt);									//players info
	changetextitem(allmenus,12,MYTBLSTR(MYINFO_TBL_NETREQ1+listnr));		//additional info about connection
	setmenuitem_VISIBLED(allmenus,10,TRUE);
	setmenuitem_VISIBLED(allmenus,11,TRUE);
	setmenuitem_VISIBLED(allmenus,12,TRUE);
    }
    else
    {
	setmenuitem_VISIBLED(allmenus,10,FALSE);
	setmenuitem_VISIBLED(allmenus,11,FALSE);
	setmenuitem_VISIBLED(allmenus,12,FALSE);
    }
}
//==========================================
int glu_conn(void)
{
    int retstatus,retstatus2,exitstatus,i,repeat,elemnr,err;
    GRPFILE *dlg;
    PCX backgnd,fontpcx;
    char *menutranspcolors,*fntadr;
    char pal[256*4];
    MENUSTR *gluerr,*gluconn;
    NETWORK_INFO recvbcast;

    mpqloadfile(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],DLGGRP_STR),(char **)&dlg);
    backgnd.openMpqPcx(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],BACKGND_STR));
    backgnd.readPalFromPcx(pal,0);
    memcpy(GRP_vidmem,backgnd.GetPcxRawBytes(),backgnd.xsizePcx()*backgnd.ysizePcx());
//    backgnd.PutPcx(DELTASCREENX,DELTASCREENY2,PCX_EMPTYCOLOR1);
    palchange(pal,gameconf.videoconf.gamma,gameconf.videoconf.saturate);
    if (!gameconf.grmode.emulationmode)
	wscreenon();
    menutranspcolors=(char *)wmalloc(2*256*256);
    LoadTransPal(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],TRANS25_STR),
		backgnd.GetRawPal768(),menutranspcolors,0.25f);
    LoadTransPal(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],TRANS50_STR),
		backgnd.GetRawPal768(),menutranspcolors+256*256,0.5f);
    SetTranspTable(menutranspcolors);

    fontpcx.openMpqPcx(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],FNT_STR));
    fntadr=fontpcx.GetPcxRawBytes();//for glowtext
    fntadr[21]=fntadr[11];
    fntadr[22]=fntadr[12];

    gluconn=LoadDialogBin("rez\\gluconn.bin",DIALOGBIN_MAINDIALOGS,IDFONT16);
    listboxlineitems(gluconn,6,6,20);
    //disable gateway pcx
    setmenuitem_VISIBLED(gluconn,4,FALSE);
    setmenuitem_VISIBLED(gluconn,7,FALSE);
    setmenuitem_VISIBLED(gluconn,8,FALSE);
    setmenuitem_VISIBLED(gluconn,9,FALSE);
//    gluconn->defaultlistitem=6;

    gluconn->menu[1].hotdeltax-=2;
    gluconn->menu[12].item.textitem->rowsize+=2;
    menuspecialtables(gluconn,fntadr,dlg);
    setmenuflags(gluconn,MENUFLAGS_ALWAYSDRAW);

    mousetype=NORMALMOUSE;
    loadonecursor(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],ARROW_STR),mousetype);
    mytimer.SetMyTimerFunc(&menutimer,NULL);
    restrictmousecoords(MOUSEMODE1);
    installmousemoveevent(&mymousemoveevent);

    MENUFIRSTDATA menushow[4];
    menushow[0].elemid=0;
    menushow[0].appearposition=MENUAPPEAR_FROMLEFT;
    menushow[1].elemid=1;
    menushow[1].appearposition=MENUAPPEAR_FROMRIGHT;
    menushow[2].elemid=2;
    menushow[2].appearposition=MENUAPPEAR_FROMBOTTOM;
    menushow[3].elemid=3;
    menushow[3].appearposition=MENUAPPEAR_FROMRIGHT;

    MENUAPPEAR *items = MenuAppear(gluconn,4,menushow,NULL);

    mylist saves;
    for (i=0;i<MAXCONNTYPES;i++)
	saves.AddList(MYTBLSTR(MYINFO_TBL_NETTYPE1+i));
    setlistbox_lists(gluconn,6,0,&saves,selconn_callback);
    repeat=1;
    do{
	if (!saves.GetMaxElements())
	{
	    setmenuitem_DISABLED(gluconn,13,TRUE);	//ok
	}
	else
	{
	    setmenuitem_DISABLED(gluconn,13,FALSE);	//ok
	}
	retstatus=drawmenu(gluconn,MENUFLAGS_EMPTY);
	retstatus&=~SELECTITEMDOUBLECLICK;
	switch(retstatus)
	{
	    case 6://dblclick on listmenu
	    case 13://ok
		//check if can use this network type
		SetNetworkType(gluconn->menu[6].item.listbox->selectednr);
		memset(&recvbcast,0,sizeof(NETWORK_INFO));
		err=CallNetwork(NETWORK_INITNETWORK,&recvbcast);
		if (err!=NETWORK_OK)
		{
		    gluerr=LoadDialogBin("rez\\glupok.bin",DIALOGBIN_MAINDIALOGS,IDFONT16);
		    menuspecialtables(gluerr,fntadr,dlg);
		    setmenuflags(gluerr,MENUFLAGS_ALWAYSDRAW);
    		    AddPrevMenuShowing(gluerr,gluconn);
		    changetextitem(gluerr,2,GLUALLSTR(GLUALL_TBL_NETWORKERR));
		    drawmenu(gluerr,MENUFLAGS_EMPTY);
		    break;
		}
		repeat=0;
		exitstatus=NETWORKGAMETYPE1+gluconn->menu[6].item.listbox->selectednr;
		break;
	    case CANCELFROMMENU://ESC
	    case 14://cancel
		repeat=0;
		exitstatus=PREVIOUSMENU;
		break;
	}
    }while(repeat);
    saves.FlushList();

    installmousemoveevent(&mymousemoveevent);
    MenuDisappear(items,NULL);

    uninstallmousemoveevent();
    mytimer.ClearMyTimerFunc();
    unloadmousecursors();

    UnloadDialogBin(gluconn);
    fontpcx.closePcx();
    SetTranspTable(NULL);
    wfree(menutranspcolors);
    wfree(dlg);
    backgnd.closePcx();
    return(exitstatus);
}
//==========================================
void updateinfogame_callback(MENUSTR *allmenus,int nr,int listnr)
{
    UpdateGameInfo(allmenus,nr,listnr);
}
//==========================================
int Guest_WaitJoinResponce(MENUSTR *allmenus,void *data)
{
    static int waitcycle=0,retcallback=0;
    int err,continuereadnetw,updatelists,ret;
    NETWORK_INFO *info;
    info=(NETWORK_INFO *) data;
    do{
	err=CallNetwork(NETWORK_DATAREADY,info);
	if (err==NETWORK_OK_RECVDATA)
	{
	    err=CallNetwork(NETWORK_RECVPACKET,info);
	    ret=Guest_CheckJoinPacket(allmenus,NULL,info);
	    if (ret)
	        waitcycle=0;
	    return(ret);
	}
	else
	    break;
    }while(1);
    if (networkreceive)
    {
	networkreceive=0;
	if (waitcycle++>=NETWORKWAITFORFILLALLGAMES*60/TIMETORECVDATA)
	{
	    waitcycle=0;
	    return(2);
	}
    }
    return(0);
    //1-deny
    //2-host down
    //3-14-accept at slotX 0-11
}
//==========================================
//==========================================
int Guest_CheckNetwork(MENUSTR *allmenus,void *data)
{
    int err,updatelists;
    NETWORK_INFO *info;
    updatelists=0;
    info=(NETWORK_INFO *) data;
    do{
	err=CallNetwork(NETWORK_DATAREADY,info);
	if (err==NETWORK_OK_RECVDATA)
	{
	    err=CallNetwork(NETWORK_RECVPACKET,info);
	    updatelists+=AddListCreatedGame((NETWORK_JOIN_PACKET *)info->recvbuffer,&info->Addr);
    	}
	if (networkresend)
	{
	    networkresend=0;
	    updatelists+=UpdateAllGamesTime();
	}
    	if (updatelists&&allmenus)
    	{
    	    UpdateGamesList(allmenus,5);
    	}
    }while(err==NETWORK_OK_RECVDATA);
    return(0);
}
//==========================================
int glu_join(FORCE_SLOTS *fslots)
{
    int retstatus,retstatus2,exitstatus,i,repeat,elemnr,error,PCID,sellist,err1,err2;
    int errtxt;
    GRPFILE *dlg;
    PCX backgnd,fontpcx;
    MENUSTR *glujoin,*gluerr;
    NETWORK_INFO recvbcast,sendsock,waitresp;

    char *menutranspcolors,*fntadr;
    char pal[256*4];

    memset(&recvbcast,0,sizeof(NETWORK_INFO));
    mpqloadfile(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],DLGGRP_STR),(char **)&dlg);
    backgnd.openMpqPcx(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],BACKGND_STR));
    backgnd.readPalFromPcx(pal,0);
    memcpy(GRP_vidmem,backgnd.GetPcxRawBytes(),backgnd.xsizePcx()*backgnd.ysizePcx());
//    backgnd.PutPcx(DELTASCREENX,DELTASCREENY2,PCX_EMPTYCOLOR1);
    palchange(pal,gameconf.videoconf.gamma,gameconf.videoconf.saturate);
    if (!gameconf.grmode.emulationmode)
	wscreenon();
    menutranspcolors=(char *)wmalloc(2*256*256);
    LoadTransPal(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],TRANS25_STR),
		backgnd.GetRawPal768(),menutranspcolors,0.25f);
    LoadTransPal(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],TRANS50_STR),
		backgnd.GetRawPal768(),menutranspcolors+256*256,0.5f);
    SetTranspTable(menutranspcolors);

    fontpcx.openMpqPcx(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],FNT_STR));
    fntadr=fontpcx.GetPcxRawBytes();//for glowtext
    fntadr[21]=fntadr[11];
    fntadr[22]=fntadr[12];

    glujoin=LoadDialogBin("rez\\glujoin.bin",DIALOGBIN_MAINDIALOGS,IDFONT16);
    listboxlineitems(glujoin,5,11,20);
    menuspecialtables(glujoin,fntadr,dlg);
    setmenuflags(glujoin,MENUFLAGS_ALWAYSDRAW);
    
    error=CallNetwork(NETWORK_INITNETWORK,&recvbcast);
    if (error>=0)
    {
	MY_PCID=recvbcast.PCID;
	error=CallNetwork(NETWORK_INITBROADCAST,&recvbcast);
	if (!error)
	    error=CallNetwork(NETWORK_INITRECVIDENTIFICATION,&recvbcast);
    }
    if (error<0)
    {
	char *errtxt;
	if (error==NETWORK_ERROR_SOCKETBIND)
	    errtxt=MYTBLSTR(MYINFO_TBL_NETPORTINUSE);
	else
	    if (error==NETWORK_ERROR_BROADCAST)
		errtxt=GLUALLSTR(GLUALL_TBL_NETWORKBROADCASTERR);
	    else
		errtxt=GLUALLSTR(GLUALL_TBL_NETWORKERR);
	gluerr=LoadDialogBin("rez\\glupok.bin",DIALOGBIN_MAINDIALOGS,IDFONT16);
	menuspecialtables(gluerr,fntadr,dlg);
	setmenuflags(gluerr,MENUFLAGS_ALWAYSDRAW);
	AddPrevMenuShowing(gluerr,glujoin);
	changetextitem(gluerr,2,errtxt);
    }

    mousetype=NORMALMOUSE;
    loadonecursor(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],ARROW_STR),mousetype);
    mytimer.SetMyTimerFunc(&menutimerandnetwork,NULL);
    restrictmousecoords(MOUSEMODE1);
    installmousemoveevent(&mymousemoveevent);

    MENUFIRSTDATA menushow[4];
    menushow[0].elemid=0;
    menushow[0].appearposition=MENUAPPEAR_FROMLEFT;
    menushow[1].elemid=1;
    menushow[1].appearposition=MENUAPPEAR_FROMRIGHT;
    menushow[2].elemid=2;
    menushow[2].appearposition=MENUAPPEAR_FROMBOTTOM;
    menushow[3].elemid=3;
    menushow[3].appearposition=MENUAPPEAR_FROMRIGHT;

    MENUAPPEAR *items;
    if (error < 0)
	items = MenuAppear(glujoin,4,menushow,gluerr);
    else
	items = MenuAppear(glujoin,4,menushow,NULL);

    if (error<0)
    {
        drawmenu(gluerr,MENUFLAGS_EMPTY);
	UnloadDialogBin(gluerr);
	exitstatus=PREVIOUSMENU;
    }
    else
    {
        AddMenu_SomeCallback(glujoin,&Guest_CheckNetwork,(void *)&recvbcast);
	setmenuitem_DISABLED(glujoin,18,TRUE);	//ok
	repeat=1;
	do{
	setlistbox_lists(glujoin,5,glujoin->menu[5].item.listbox->selectednr,NULL,updateinfogame_callback);
        retstatus=drawmenu(glujoin,MENUFLAGS_EMPTY);
	retstatus&=~SELECTITEMDOUBLECLICK;
	switch(retstatus)
	{
	    case 5://dblclick on listmenu
	    case 18://ok
		sellist=getlistbox_selecteditem(glujoin,5);
		if (sellist>=0)
		{
		    PCID=GetGameId(sellist,getlistbox_selecteditemname(glujoin,5));
		}
		memset(&sendsock,0,sizeof(NETWORK_INFO));
		err1=CallNetwork(NETWORK_INITNETWORK,&sendsock);

		memset(&waitresp,0,sizeof(NETWORK_INFO));
		err2=CallNetwork(NETWORK_INITNETWORK,&waitresp);
		if (err1==NETWORK_OK&&err2==NETWORK_OK)
		{
	    	    err2=CallNetwork(NETWORK_INITRECVPACKET,&waitresp);
    		    if (err2!=NETWORK_OK)
    		    {
    			errtxt=MYINFO_TBL_ALLWAITPORTSUSED;
    		    }
    		    else
    		    {
    			err1=FillNetworkAddr(&sendsock,PCID);
    			if (err1==NETWORK_OK)
    			{
			    sendsock.COMMAND=STARCLONE_NETWORKJOIN_COMMAND_JOINREQUEST;
			    sendsock.PCID=PCID;
			    //send my income port and PCID
			    sprintf(sendsock.sendstr,SENDJOINREQUEST_STR,waitresp.connectport,MY_PCID,nickname);
			    err1=CallNetwork(NETWORK_SENDPACKET,&sendsock);
			    if (err1==NETWORK_OK)
			    {
				retstatus=glu_putmenu(glujoin,"rez\\glupokdis.bin",MYTBLSTR(MYINFO_TBL_SENDJOINREQUEST),
							fntadr,dlg,&Guest_WaitJoinResponce,&waitresp,1);
				switch(retstatus)
				{
				    case 1:
					glu_putmenu(glujoin,"rez\\glupok.bin",MYTBLSTR(MYINFO_TBL_HOSTFULL),fntadr,dlg);
					break;
				    case 2:
					glu_putmenu(glujoin,"rez\\glupok.bin",MYTBLSTR(MYINFO_TBL_HOSTDOWN),fntadr,dlg);
					break;
				    case 3:
				    case 4:
				    case 5:
				    case 6:
				    case 7:
				    case 8:
				    case 9:
				    case 10:
				    case 11:
				    case 12:
				    case 13:
				    case 14:
					//accept connection
					repeat=0;
					exitstatus=JOINGAME;
					break;
				    case CALLBACKCANCELFROMMENU2:
					printf("error cancel game, but I'm not joined\n");
					break;
				    case CALLBACKCANCELFROMMENU6:	//already banned
					glu_putmenu(glujoin,"rez\\glupok.bin",MYTBLSTR(MYINFO_TBL_ALREADYBANNED),fntadr,dlg);
					break;
				    case CALLBACKCANCELFROMMENU7:	//name exists
					glu_putmenu(glujoin,"rez\\glupok.bin",MYTBLSTR(MYINFO_TBL_NAMEEXISTS),fntadr,dlg);
					break;
				    default:
					printf("unknown join receive command\n");
					break;
				}
			    }//send joinpacket
			    else
				errtxt=MYINFO_TBL_SENDERROR;
			}//fillnetw
			else
			    errtxt=MYINFO_TBL_WRONGSERVERINFO;
		    }
		}
		else
		    errtxt=MYINFO_TBL_NEWGAMENETERR;
		if (err1!=NETWORK_OK||err2!=NETWORK_OK)
		{
		    glu_putmenu(glujoin,"rez\\glupok.bin",MYTBLSTR(errtxt),fntadr,dlg);
		}
		err1=CallNetwork(NETWORK_CLOSENETWORK,&sendsock);
		err1=CallNetwork(NETWORK_CLOSENETWORK,&waitresp);
		break;
	    case CANCELFROMMENU://ESC
	    case 19://cancel
		repeat=0;
		exitstatus=PREVIOUSMENU;
		break;
	    case 20:
		repeat=0;
		exitstatus=CREATEGAME;
		break;
	}
	}while(repeat);
	CallNetwork(NETWORK_CLOSENETWORK,&recvbcast);
	//flush the lists in listbox
	if (glujoin->menu[5].item.listbox->flist)
	    glujoin->menu[5].item.listbox->flist->FlushList();
	EmptyAllGamesList();
    }

    installmousemoveevent(&mymousemoveevent);
    MenuDisappear(items,NULL);

    uninstallmousemoveevent();
    mytimer.ClearMyTimerFunc();
    unloadmousecursors();

    UnloadDialogBin(glujoin);
    fontpcx.closePcx();
    SetTranspTable(NULL);
    wfree(menutranspcolors);
    wfree(dlg);
    backgnd.closePcx();
    return(exitstatus);
}

//==========================================
int glu_login(void)
{
    FILE *f;
    int strlenx,flags,i;
    int exitstatus=0,err;
    GRPFILE *dlg;
    PCX backgnd,*elempcx[3],fontpcx;
    char *menutranspcolors,*savedscr,*fntadr;
    char tempname[512];
    char pal[256*4];

    mpqloadfile(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],DLGGRP_STR),(char **)&dlg);

    backgnd.openMpqPcx(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],BACKGND_STR));
    backgnd.readPalFromPcx(pal,0);
    memcpy(GRP_vidmem,backgnd.GetPcxRawBytes(),backgnd.xsizePcx()*backgnd.ysizePcx());
//    backgnd.PutPcx(DELTASCREENX,DELTASCREENY2,PCX_EMPTYCOLOR1);
    palchange(pal,gameconf.videoconf.gamma,gameconf.videoconf.saturate);
    if (!gameconf.grmode.emulationmode)
	wscreenon();
    menutranspcolors=(char *)wmalloc(2*256*256);
    LoadTransPal(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],TRANS25_STR),
		    backgnd.GetRawPal768(),menutranspcolors,0.25f);
    LoadTransPal(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],TRANS50_STR),
		    backgnd.GetRawPal768(),menutranspcolors+256*256,0.5f);
    SetTranspTable(menutranspcolors);

    fontpcx.openMpqPcx(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],FNT_STR));
    fntadr=fontpcx.GetPcxRawBytes();//for glowtext
    fntadr[21]=fntadr[11];
    fntadr[22]=fntadr[12];

    MENUSTR *singammenu=LoadDialogBin("rez\\glulogin.bin",DIALOGBIN_MAINDIALOGS,IDFONT16);
    listboxlineitems(singammenu,7,11,20);

    menuspecialtables(singammenu,fntadr,dlg);
//    setdefaultbutton(singammenu,3);
    setmenuflags(singammenu,MENUFLAGS_ALWAYSDRAW);


    MENUSTR *nplayer=LoadDialogBin("rez\\glunewch.bin",DIALOGBIN_MAINDIALOGS,IDFONT16);
    menuspecialtables(nplayer,fntadr,dlg);
    setitemrelation(nplayer,1,ITEMRELATION_DISABLE,&nplayer->menu[3].item.editbox->length,0);
    setmenuflags(nplayer,MENUFLAGS_ALWAYSDRAW);
    changeeditboxparam(nplayer,3,"",30);
    AddPrevMenuShowing(nplayer,singammenu);

    mousetype=NORMALMOUSE;
    loadonecursor(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],ARROW_STR),mousetype);
    mytimer.SetMyTimerFunc(&menutimer,NULL);
    restrictmousecoords(MOUSEMODE1);
    installmousemoveevent(&mymousemoveevent);

    mylist ulist;
    listusers(USERS_DIRECTORY,&ulist);
    int nrofusers=ulist.GetMaxElements();
    ulist.DeallocList();

    MENUFIRSTDATA menushow[3];
    menushow[0].elemid=0;
    menushow[0].appearposition=MENUAPPEAR_FROMLEFT;
    menushow[1].elemid=1;
    menushow[1].appearposition=MENUAPPEAR_FROMBOTTOM;
    menushow[2].elemid=2;
    menushow[2].appearposition=MENUAPPEAR_FROMRIGHT;

    MENUAPPEAR *items;
    if (!nrofusers)
	items = MenuAppear(singammenu,3,menushow,nplayer);
    else
	items = MenuAppear(singammenu,3,menushow,NULL);

    int retstatus,retstatus2,repeat=1;
    do{
	listusers(USERS_DIRECTORY,&ulist);
	if (!nrofusers)
	    flags=MENUFLAGS_BYPASS;
	else
	    flags=MENUFLAGS_EMPTY;
	setlistbox_lists(singammenu,7,singammenu->menu[7].item.listbox->selectednr,&ulist,NULL);
	if (!ulist.GetMaxElements())
	{
	    setmenuitem_DISABLED(singammenu,3,TRUE);
	    setmenuitem_DISABLED(singammenu,6,TRUE);
	}
	else
	{
	    setmenuitem_DISABLED(singammenu,3,FALSE);
	    setmenuitem_DISABLED(singammenu,6,FALSE);
	}
	if (nrofusers)
	    retstatus=drawmenu(singammenu,flags);
	else
	{
	    retstatus=5;
	    nrofusers=1;
	}
//	doubleclick=retstatus&SELECTITEMDOUBLECLICK;
	retstatus&=~SELECTITEMDOUBLECLICK;
	switch(retstatus)
	{
	    case 7://dblclick on listmenu
	    case 3://ok
		if (singammenu->menu[7].item.listbox->selectednr==-1)
		    break;
		strncpy(nickname,(char *)ulist.GetElemNr(singammenu->menu[7].item.listbox->selectednr),sizeof(nickname)-1);
		setplayername(0,(char *)ulist.GetElemNr(singammenu->menu[7].item.listbox->selectednr));
		getplayerinfo(getplayername(NUMBGAMER));
		strcpy(tempname,USERS_DIRECTORY);
		strcat(tempname,nickname);
		strcat(tempname,STARCLONEEXT);
		if (loadandsetplayerinfo(tempname))
		{
		    glu_putmenu(singammenu,"rez\\glupok.bin",MYTBLSTR(MYINFO_TBL_CORRUPTCHAR),fntadr,dlg);
		    repeat = 1;
		    break;
		}
		repeat=0;
		exitstatus=0;//go player to select map
		break;
	    case CANCELFROMMENU://ESC
	    case 4://cancel
		repeat=0;
		exitstatus=2;//exit to main menu
		break;
	    case 5://newplayer
		do{
		    setdefaultbutton(nplayer,3);
		    changeeditboxtext(nplayer,3,"");
		    retstatus2=drawmenu(nplayer,MENUFLAGS_EMPTY);
		    err=0;
		    if (retstatus2==1||retstatus2==3)
		    {
			    //confirmed new player
			    strcpy(tempname,USERS_DIRECTORY);
			    strcat(tempname,geteditboxtext(nplayer,3));
			    strcat(tempname,STARCLONEEXT);
			    f=fopen(tempname,"r");
			    if (f)
			    {
				err=1;
			    }
			    else
			    {
				f=fopen(tempname,"w");
				if (f)
				{
				    CreateDefaultInfoForPlayer(f);
				    fclose(f);
				}
				else
				{
				    //can not create any character
				    glu_putmenu(singammenu,"rez\\glupok.bin",GLUALLSTR(GLUALL_TBL_ERRCREATECHAR),fntadr,dlg);
				    break;
				}
			    }
		    }
		    if (err)
		    {
			//error create player
			glu_putmenu(singammenu,"rez\\glupok.bin",GLUALLSTR(GLUALL_TBL_ALRDCHAR),fntadr,dlg);
		    }
		}while(err);
		break;
	    case 6://del character
		if (singammenu->menu[7].item.listbox->selectednr==-1)
		    break;
		strncpy(nickname,(char *)ulist.GetElemNr(singammenu->menu[7].item.listbox->selectednr),sizeof(nickname)-1);
	        strcpy(tempname,GLUALLSTR(GLUALL_TBL_DELETECHAR));
		retstatus2=glu_putmenu(singammenu,"rez\\glupokcancel.bin",tempname,fntadr,dlg);
		err=0;
		if (retstatus2==1)
		{
		    //confirmed delete player
		    //delete player savegames
		    strcpy(tempname,SAVES_DIRECTORY);
		    strcat(tempname,nickname);
		    rmdirrec(tempname);
		    
		    //delete player account
		    strcpy(tempname,USERS_DIRECTORY);
		    strcat(tempname,nickname);
		    strcat(tempname,STARCLONEEXT);
		    err=unlink(tempname);
		}
		if (err)
		{
		    glu_putmenu(singammenu,"rez\\glupok.bin",MYTBLSTR(MYINFO_TBL_ERRDELCHAR),fntadr,dlg);
		}
		break;
	}
	ulist.DeallocList();
    }while(repeat);


    installmousemoveevent(&mymousemoveevent);

    MenuDisappear(items,NULL);

    uninstallmousemoveevent();
    mytimer.ClearMyTimerFunc();
    unloadmousecursors();

    UnloadDialogBin(nplayer);
    UnloadDialogBin(singammenu);

    fontpcx.closePcx();
    SetTranspTable(NULL);
    wfree(menutranspcolors);
    backgnd.closePcx();
    wfree(dlg);
    return(exitstatus);
}
//==========================================
char SELMAP_OPENEDDIRICON[]="icons\\iopen.pcx";
char SELMAP_CLOSEDDIRICON[]="icons\\iclosed.pcx";
char SELMAP_CLOSEDSELDIRICON[]="icons\\iclosedsel.pcx";
char SELMAP_MAPSELICON[]="icons\\imapsel.pcx";
char SELMAP_MAPNOSELICON[]="icons\\imap.pcx";

char MPQ_SELMAP_OPENEDDIRICON[]="glue\\battle.net\\icons\\iopen.pcx";
char MPQ_SELMAP_CLOSEDDIRICON[]="glue\\battle.net\\icons\\iclosed.pcx";
char MPQ_SELMAP_CLOSEDSELDIRICON[]="glue\\battle.net\\icons\\iclosedsel.pcx";
char MPQ_SELMAP_MAPSELICON[]="glue\\battle.net\\icons\\imapsel.pcx";
char MPQ_SELMAP_MAPNOSELICON[]="glue\\battle.net\\icons\\imap.pcx";

char SELMAP_MAPINFO[]="glue\\gamesel\\pjoin.pcx";
char SELMAP_MAPLIST1[]="glue\\gamesel\\plistlrg.pcx";
//==========================================
int loaddlgicons(void)
{
    int err;
    err=dlgfilediriconspcx[0].openMpqPcx(SELMAP_OPENEDDIRICON);
    if (err!=OKPCX)
	return -1;
    err=dlgfilediriconspcx[1].openMpqPcx(SELMAP_CLOSEDDIRICON);
    if (err!=OKPCX)
	return -2;
    err=dlgfilediriconspcx[2].openMpqPcx(SELMAP_CLOSEDSELDIRICON);
    if (err!=OKPCX)
	return -3;
    err=dlgfilediriconspcx[3].openMpqPcx(SELMAP_MAPSELICON);
    if (err!=OKPCX)
	return -4;
    err=dlgfilediriconspcx[4].openMpqPcx(SELMAP_MAPNOSELICON);
    if (err!=OKPCX)
	return -5;
    return(0);
}
//==========================================
void unloaddlgicons(void)
{
    dlgfilediriconspcx[0].closePcx();
    dlgfilediriconspcx[1].closePcx();
    dlgfilediriconspcx[2].closePcx();
    dlgfilediriconspcx[3].closePcx();
    dlgfilediriconspcx[4].closePcx();
}
//==========================================
void GetLastDir(char *path,char *seldir)
{
    int i,len;
    len=strlen(path);
    for (i=len-1;i>=0;i--)
    {
	if (path[i]=='/')
	    break;
    }
    strcpy(seldir,&path[i+1]);
}
//==========================================
char test_nrofpl;
char test_iowner[MAXPLAYERS];
char test_owner[MAXPLAYERS];
char test_race[MAXPLAYERS];
//==========================================
void copytempowners(struct mapinfo *testmap)
{
    int i;
    starmap_forceslots(testmap,&force_slots,MAP_GAMETYPE_USEMAPSETTINGS);
    memcpy(test_owner,testmap->pl_owner,MAXPLAYERS);
    memcpy(test_iowner,testmap->pl_iowner,MAXPLAYERS);
    memcpy(test_race,testmap->pl_race,MAXPLAYERS);
    for (i=0;i<MAXPLAYERS;i++)
        if (test_race[i]>=OWNER_RESCUABLE)
    	test_race[i]=OWNER_RESCUABLE;
}
//==========================================
void preparegameconf_ums(void)
{
    int i,j,nrplayers=0;
    if (GAMETYPE==MAP_GAMETYPE_USEMAPSETTINGS)
    {
        int havehuman=0;
        for (i=0,j=0;i<PLAYEDPLAYERS;i++)
        {
	    if ((test_owner[i]==OWNER_HUMAN||test_owner[i]==OWNER_COMPUTER||test_iowner[i]==OWNER_NEUTRAL)&&
	        (j<test_nrofpl||!havehuman))
	    {
	        if (test_iowner[i]==OWNER_HUMAN)
	        {
		    havehuman++;
		    NUMBGAMER=i;
		}
		j++;
		nrplayers++;
		gameconf.pl_race[i]=test_race[i];
		if (gameconf.pl_race[i]==RACE_INDEPENDENT)//random race to specific race
		    gameconf.pl_race[i]=myrand(3);
		if (i==NUMBGAMER)
		    gameconf.pl_owner[i]=OWNER_HUMAN;
		else
		    gameconf.pl_owner[i]=OWNER_COMPUTER;
	    }
	    else
	    {
	        gameconf.pl_race[i]=test_race[i];
	        gameconf.pl_owner[i]=test_owner[i];
//	        gameconf.pl_race[i]=RACE_INACTIVE;
//	        gameconf.pl_owner[i]=OWNER_NONE;
	    }
	}
    }
}
//==========================================
void initselectmaplists(MENUSTR *allmenus)//,struct mapinfo *info)
{
    int i,j,find=0,forcenr;
    int gmtype;
    char playertypes[60];
    
    gmtype=getexpandbox_selecteditem(allmenus,19);
    strcpy(playertypes,GLUALLSTR(GLUALL_TBL_COMPUTERLIST));
    strcat(playertypes,"\n");
    strcat(playertypes,GLUALLSTR(GLUALL_TBL_CLOSEDLIST));

    switch(gmtype)
    {
	case MAP_GAMETYPE_MELEE:
	case MAP_GAMETYPE_FREEFORALL:
	    NUMBGAMER=0;
	    addexpanditem_lists(allmenus,21+0,0,nickname);
	    
	    setmenuitem_DISABLED(allmenus,21+0,FALSE);
    	    setmenuitem_VISIBLED(allmenus,21+0,TRUE);
	    changeexpanditemnr(allmenus,21+0,0);	//playertype=0
	    
	    setmenuitem_DISABLED(allmenus,29+0,FALSE);
    	    setmenuitem_VISIBLED(allmenus,29+0,TRUE);
    	    changeexpanditemnr(allmenus,29+0,3);	//racetype=0
	    for (i=1;i<test_nrofpl;i++)
	    {
		addexpanditem_lists(allmenus,21+i,0,playertypes);
		setmenuitem_DISABLED(allmenus,21+i,FALSE);
		setmenuitem_VISIBLED(allmenus,21+i,TRUE);
		if (!i)
		{
		    
		    setmenuitem_DISABLED(allmenus,29+i,FALSE);
		    setmenuitem_VISIBLED(allmenus,29+i,TRUE);
		}
		else
		{
		    if (getexpandbox_selecteditem(allmenus,21+i)==0)
		    {
			setmenuitem_DISABLED(allmenus,29+i,FALSE);
			setmenuitem_VISIBLED(allmenus,29+i,TRUE);
		    }
		}
		changeexpanditemnr(allmenus,21+i,0);	//playertype=0
		changeexpanditemnr(allmenus,29+i,3);	//racetype=0
	    }
	    for (i=test_nrofpl;i<8;i++)
	    {
		setmenuitem_VISIBLED(allmenus,21+i,FALSE);
		setmenuitem_VISIBLED(allmenus,29+i,FALSE);
	    }
	    break;
	case MAP_GAMETYPE_USEMAPSETTINGS:
	    for (i=0,j=0;i<PLAYEDPLAYERS;i++)
	    {
		if (j>=test_nrofpl)
		    break;
		addexpanditem_lists(allmenus,21+j,0,playertypes);
		if (test_owner[i]==OWNER_HUMAN)
		{
		    if (!find)
		    {
			find=1;
			NUMBGAMER=i;
			addexpanditem_lists(allmenus,21+j,0,nickname);
		    }
		    else
			addexpanditem_lists(allmenus,21+j,0,playertypes);
			
		    setmenuitem_VISIBLED(allmenus,21+j,TRUE);
		    setmenuitem_VISIBLED(allmenus,29+j,FALSE);
		    setmenuitem_DISABLED(allmenus,21+j,TRUE);
		    setmenuitem_DISABLED(allmenus,29+j,TRUE);
//		    changeexpanditemnr(allmenus,29+j,test_race[i]);
		    j++;
		}
		else if (test_owner[i]==OWNER_COMPUTER)
		{
		    setmenuitem_VISIBLED(allmenus,21+j,TRUE);
		    setmenuitem_VISIBLED(allmenus,29+j,FALSE);
//		    addexpanditem_lists(allmenus,21+j,0,playertypes);
//		    changeexpanditemnr(allmenus,29+j,test_race[i]);
		    setmenuitem_DISABLED(allmenus,21+j,TRUE);
		    setmenuitem_DISABLED(allmenus,29+j,TRUE);
		    j++;
		}
		else//other owners
		{
		    addexpanditem_lists(allmenus,21+j,1,playertypes);
		    setmenuitem_VISIBLED(allmenus,21+j,FALSE);
		    setmenuitem_VISIBLED(allmenus,29+j,FALSE);
		    setmenuitem_DISABLED(allmenus,21+j,TRUE);
		    setmenuitem_DISABLED(allmenus,29+j,TRUE);
		}
	    }
	    for (;j<PLAYEDPLAYERS;j++)
	    {
		setmenuitem_VISIBLED(allmenus,21+j,FALSE);
		setmenuitem_VISIBLED(allmenus,29+j,FALSE);
		setmenuitem_DISABLED(allmenus,21+j,TRUE);
		setmenuitem_DISABLED(allmenus,29+j,TRUE);
	    }
	    break;
    }
}
//==========================================
static unsigned char convpalformenumap[MAXTERRAINTYPES][256];
static unsigned char convpal_loaded[MAXTERRAINTYPES];
//==========================================
int getmapinfo(MENUSTR *allmenus,char *filename,int haveslots)
{
    char number[10];
    char *message;
    char temptext[60];
    char temptext2[60];
    unsigned char convpal[256];
    int offset,i;
    mapinfo *testmap=(mapinfo *) wmalloc(sizeof(mapinfo));
    memset(testmap,0,sizeof(mapinfo));
    int err=starmap_info(NULL,filename,testmap);
    if (!err)
    {
	copytempowners(testmap);
/*	starmap_forceslots(testmap,&force_slots,MAP_GAMETYPE_USEMAPSETTINGS);
	memcpy(test_owner,testmap->pl_owner,MAXPLAYERS);
	memcpy(test_iowner,testmap->pl_iowner,MAXPLAYERS);
	memcpy(test_race,testmap->pl_race,MAXPLAYERS);
	for (i=0;i<MAXPLAYERS;i++)
	    if (test_race[i]>=OWNER_RESCUABLE)
		test_race[i]=OWNER_RESCUABLE;
*/
	test_nrofpl=GetTotalStartLocations(testmap);
	message=getmapSTR(testmap,0);
	strncpy(MAPNAME,message,STARCLONE_NETWORKJOIN_MAXMAPNAMESYMB);
	MAPNAME[STARCLONE_NETWORKJOIN_MAXMAPNAMESYMB+1]=0;
	MAXPLRS=test_nrofpl;
	changetextitem(allmenus,8,message);
	message=getmapSTR(testmap,1);
	changetextitem(allmenus,9,message);
#ifdef STARMENUMAP_SHOWNEW
	sprintf(temptext2,"%s %s Slots:%d", GLUALLSTR(GLUALL_TBL_TILESETNAMES+testmap->terrain),
					    GLUALLSTR(GLUALL_TBL_MAPSIZEXY),test_nrofpl);
	sprintf(temptext,temptext2,testmap->map_width,testmap->map_height);
	MAPSIZEX=testmap->map_width;
	MAPSIZEY=testmap->map_height;
	changetextitem(allmenus,14,temptext);
	setmenuitem_DISABLED(allmenus,10,FALSE);
	setmenuitem_VISIBLED(allmenus,10,TRUE);
	if (allmenus->palette)
	{
	    if (!convpal_loaded[testmap->terrain])//check if already make conversion for this terrain type
	    {
		convpal_loaded[testmap->terrain]=1;
		ConvertColorsToNewPalette((unsigned char *)testmap->palette,
					  allmenus->palette,convpalformenumap[testmap->terrain],4);
	    }
	    for (i=0;i<MINIMAPW*MINIMAPW;i++)
		testmap->minimap[i]=convpalformenumap[testmap->terrain][testmap->minimap[i]];
	}
	for (i=0;i<MINIMAPW;i++)
	{
	    testmap->minimap[0+i]=FONTCOLOR(allmenus->fonttable,MWHITECOLORFONT,3);
	    testmap->minimap[MINIMAPW*(MINIMAPW-1)+i]=FONTCOLOR(allmenus->fonttable,MWHITECOLORFONT,3);
	    testmap->minimap[i*MINIMAPW+0]=FONTCOLOR(allmenus->fonttable,MWHITECOLORFONT,3);
	    testmap->minimap[i*MINIMAPW+MINIMAPW-1]=FONTCOLOR(allmenus->fonttable,MWHITECOLORFONT,3);
	}
	updateimageitem(allmenus,10,testmap->minimap);
//	memcpy(allmenus->menu[10].item.image->pcx->GetPcxRawBytes(),testmap->minimap,MINIMAPW*MINIMAPW);
#else
	test_nrofpl=GetTotalStartLocations(testmap);
	sprintf(temptext2,GLUALLSTR(GLUALL_TBL_MAPSIZEXY),testmap->map_width,testmap->map_height);
	MAPSIZEX=testmap->map_width;
	MAPSIZEY=testmap->map_height;
	sprintf(temptext ,GLUALLSTR(GLUALL_TBL_MAPSIZE),RIGHTALIGNSYMB,temptext2);
	changetextitem(allmenus,11,temptext);
	sprintf(temptext ,GLUALLSTR(GLUALL_TBL_TILESET),RIGHTALIGNSYMB,
			  GLUALLSTR(GLUALL_TBL_TILESETNAMES+testmap->terrain));
	changetextitem(allmenus,13,temptext);
	sprintf(temptext ,GLUALLSTR(GLUALL_TBL_NRPLAYERS),RIGHTALIGNSYMB,witoa(test_nrofpl,temptext2));
	changetextitem(allmenus,14,temptext);
	setmenuitem_DISABLED(allmenus,11,FALSE);
	setmenuitem_DISABLED(allmenus,13,FALSE);
	setmenuitem_VISIBLED(allmenus,11,TRUE);
	setmenuitem_VISIBLED(allmenus,13,TRUE);
#endif
    setmenuitem_DISABLED(allmenus,8,FALSE);
    setmenuitem_DISABLED(allmenus,9,FALSE);
    setmenuitem_DISABLED(allmenus,14,FALSE);
    setmenuitem_DISABLED(allmenus,15,FALSE);
    setmenuitem_DISABLED(allmenus,17,FALSE);
    setmenuitem_DISABLED(allmenus,19,FALSE);

    setmenuitem_VISIBLED(allmenus,8,TRUE);
    setmenuitem_VISIBLED(allmenus,9,TRUE);
    setmenuitem_VISIBLED(allmenus,14,TRUE);
    setmenuitem_VISIBLED(allmenus,15,TRUE);
    setmenuitem_VISIBLED(allmenus,17,TRUE);
    setmenuitem_VISIBLED(allmenus,19,TRUE);
    }
    else
    {
	//need parce for error
	test_nrofpl=0;
	changetextitem(allmenus,8,GLUALLSTR(GLUALL_TBL_ERRMAP));
	setmenuitem_DISABLED(allmenus,15,TRUE);
    }
    if (haveslots)
    {
	initselectmaplists(allmenus);//,testmap);
    }
    unload_starmapallocated(testmap);
    wfree(testmap);
    return(err);
}
//==========================================
void showinfofromlistitem(MENUSTR *allmenus,int menuitemnr,int listnr)
{
    int i,err,haveslots;
    char mapfile[1024];
    MENUPOS *menuitem=&allmenus->menu[menuitemnr];//listbox
    char *mes=(char *)menuitem->item.listbox->flist->GetElemNr(listnr);

    changetextitem(allmenus,8,NULL);
    changetextitem(allmenus,9,NULL);
    changetextitem(allmenus,11,NULL);
    changetextitem(allmenus,12,NULL);
    changetextitem(allmenus,13,NULL);
    changetextitem(allmenus,14,NULL);
    setmenuitem_DISABLED(allmenus,15,FALSE);
    err=1;
    haveslots=0;
    if (allmenus->elements==37)
        haveslots=1;
    if (mes)
    {
	if (mes[0]==COMMANDSYMB)
	{
	    if (mes[1]==ICONFORSELFILE||mes[1]==ICONFORNOSELFILE)
	    {
		getcwd(mapfile,sizeof(mapfile)-1);
		strncat(mapfile,"/",sizeof(mapfile)-1);
		strncat(mapfile,&mes[2],sizeof(mapfile)-1);
		err=getmapinfo(allmenus,mapfile,haveslots);
	    }
	}
    }
    if (err)
    {
	setmenuitem_VISIBLED(allmenus,9,FALSE);
	setmenuitem_VISIBLED(allmenus,10,FALSE);
	setmenuitem_VISIBLED(allmenus,11,FALSE);
	setmenuitem_VISIBLED(allmenus,12,FALSE);
	setmenuitem_VISIBLED(allmenus,13,FALSE);
	setmenuitem_VISIBLED(allmenus,14,FALSE);
	setmenuitem_VISIBLED(allmenus,17,FALSE);
	setmenuitem_VISIBLED(allmenus,19,FALSE);
	if (haveslots)
	{
	    for (i=0;i<8;i++)
	    {
		setmenuitem_VISIBLED(allmenus,21+i,FALSE);
		setmenuitem_VISIBLED(allmenus,29+i,FALSE);
	    }
	}
    }

}
//============================
signed char gamecounter=0;
#define SECONDSREMAINTOGO	6
//==========================================
int letsgamecounter(MENUSTR *allmenus,int id1,int id2)
{
	char txt1[30];
	
	if (gamecounter!=-2)
	{
    	    if (--gamecounter>=0)
    	    {
		switch(gamecounter)
		{
		    case 5:
		    case 4:
		    case 3:
		    case 2:
		    case 0:
			sprintf(txt1,MYTBLSTR(MYINFO_TBL_STARTINGIN),gamecounter,GLUALLSTR(GLUALL_TBL_TEXT_SECONDS));
			break;
		    case 1:
			sprintf(txt1,MYTBLSTR(MYINFO_TBL_STARTINGIN),gamecounter,GLUALLSTR(GLUALL_TBL_TEXT_SECOND));
			break;
		}
		changetextitem(allmenus,id1,txt1);
		setmenuitem_VISIBLED(allmenus,id1,TRUE);
		if (gamecounter>2)
		    setmenuitem_DISABLED(allmenus,id2,FALSE);
		else
		    setmenuitem_DISABLED(allmenus,id2,TRUE);
		Play_sfxdata_id(NULL,SFXDATA_COUNTDOWN,-1,0);
	    }
	    else
		return(1);
	}
	return(0);
}
//==========================================
int SelectMapCallbackFunc(MENUSTR *allmenus,void *data)
{
    static int update=0;
    if (menutimerupdate)
    {
	menutimerupdate=0;
	if ((++update%(60/TIMETOMAINMENUUPDATE))==1)
	{
#ifdef STARMENUMAP_SHOWNEW
	    if (letsgamecounter(allmenus,12,16))
#else
	    if (letsgamecounter(allmenus,10,16))
#endif
	    {
		update=0;
		return(CALLBACKCANCELFROMMENU9);//go go go - lets play
	    }
	}
    }
    return(0);
}
//==========================================
int selectmapmenu(void)
{
    int exitstatus=0,repeat=1,i,j,len,updatelistfiles,nrplayers;
    int retstatus,gmtype;
    char *menutranspcolors,*fntadr;
    GRPFILE *dlg;
    PCX backgnd,fontpcx;
    char pal[256*4];
    char selpath[512];
    char listboxstr[512];
    char allraces[60];
    char playertypes[60];
    char currentpath[1024];
    
    chdir(GAMEPATH);
    if (loaddlgicons())
	return -1;

    mpqloadfile(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],DLGGRP_STR),(char **)&dlg);
    backgnd.openMpqPcx(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],BACKGND_STR));
    backgnd.readPalFromPcx(pal,0);
    memcpy(GRP_vidmem,backgnd.GetPcxRawBytes(),backgnd.xsizePcx()*backgnd.ysizePcx());
//    backgnd.PutPcx(DELTASCREENX,DELTASCREENY2,PCX_EMPTYCOLOR1);
    palchange(pal,gameconf.videoconf.gamma,gameconf.videoconf.saturate);
    if (!gameconf.grmode.emulationmode)
	wscreenon();
    menutranspcolors=(char *)wmalloc(2*256*256);
    LoadTransPal(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],TRANS25_STR),
		    backgnd.GetRawPal768(),menutranspcolors,0.25f);
    LoadTransPal(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],TRANS25_STR),
		    backgnd.GetRawPal768(),menutranspcolors+256*256,0.5f);
    SetTranspTable(menutranspcolors);
    fontpcx.openMpqPcx(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],FNT_STR));
    fntadr=fontpcx.GetPcxRawBytes();//for glowtext
    fntadr[21]=fntadr[11];
    fntadr[22]=fntadr[12];

    MENUSTR *selmap=LoadDialogBin("rez\\glucustm.bin",DIALOGBIN_MAINDIALOGS,IDFONT16);
    listboxlineitems(selmap,6,6,20);
    menuspecialtables(selmap,fntadr,dlg);
    setmenuflags(selmap,MENUFLAGS_ALWAYSDRAW);

    mousetype=NORMALMOUSE;
    loadonecursor(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],ARROW_STR),mousetype);
    mytimer.SetMyTimerFunc(&menutimer,NULL);
    restrictmousecoords(MOUSEMODE1);
    installmousemoveevent(&mymousemoveevent);

    MENUFIRSTDATA menushow[5];
    menushow[0].elemid=0;
    menushow[0].appearposition=MENUAPPEAR_FROMLEFT;
    menushow[1].elemid=1;
    menushow[1].appearposition=MENUAPPEAR_FROMTOP;
    menushow[2].elemid=2;
    menushow[2].appearposition=MENUAPPEAR_FROMBOTTOM;
    menushow[3].elemid=3;
    menushow[3].appearposition=MENUAPPEAR_FROMBOTTOM;
    menushow[4].elemid=4;
    menushow[4].appearposition=MENUAPPEAR_FROMRIGHT;

    MENUAPPEAR *items = MenuAppear(selmap,5,menushow,NULL);

    chdir(GAMEMAPPATH);
    if (EXPANSIONSET)
	chdir(ADDGAMEPATH);
    addmenupalette(selmap,(unsigned char *)pal);
    setfontnr(selmap,8,IDFONT16);
    setfontnr(selmap,10,IDFONT16);
    setfontnr(selmap,11,IDFONT16);
    setfontnr(selmap,12,IDFONT16);
    setfontnr(selmap,13,IDFONT16);
    setfontnr(selmap,14,IDFONT16);

    setmenuitem_VISIBLED(selmap,18,FALSE);

    strcpy(allraces,MYTBLSTR(MYINFO_TBL_MELEE));
    strcat(allraces,"\n");
    strcat(allraces,MYTBLSTR(MYINFO_TBL_FREEFORALL));
    strcat(allraces,"\n");
    strcat(allraces,MYTBLSTR(MYINFO_TBL_USEMAPSETT));
    
    
    addexpanditem_lists(selmap,19,2,allraces);	//usemapsettings for default
    setmenuitem_VISIBLED(selmap,19,FALSE);
    NUMBGAMER=0;

    addexpanditem_lists(selmap,21,0,getplayername(NUMBGAMER));

    strcpy(allraces,NETWSTR(NETWORK_TBL_ZERGRACE));
    strcat(allraces,"\n");
    strcat(allraces,NETWSTR(NETWORK_TBL_TERRANRACE));
    strcat(allraces,"\n");
    strcat(allraces,NETWSTR(NETWORK_TBL_PROTOSSRACE));
    strcat(allraces,"\n");
    strcat(allraces,NETWSTR(NETWORK_TBL_RANDOMRACE));
    
    addexpanditem_lists(selmap,29,3,allraces);
    NUMBGAMER=0;

    strcpy(playertypes,GLUALLSTR(GLUALL_TBL_COMPUTERLIST));
    strcat(playertypes,"\n");
    strcat(playertypes,GLUALLSTR(GLUALL_TBL_CLOSEDLIST));
    
    for (i=1;i<PLAYEDPLAYERS;i++)
    {
	setmenuitem_VISIBLED(selmap,21+i,FALSE);
	setmenuitem_DISABLED(selmap,21+i,FALSE);

	addexpanditem_lists(selmap,21+i,0,playertypes);

	setmenuitem_VISIBLED(selmap,29+i,FALSE);
	setmenuitem_DISABLED(selmap,29+i,FALSE);
	addexpanditem_lists(selmap,29+i,3,allraces);
    }

#ifdef STARMENUMAP_SHOWNEW
    //change positions and sizes
    deltextitem(selmap,8,1);
    deltextitem(selmap,9,1);
    deltextitem(selmap,14,1);
    addtextitem(selmap,8,ISLABELCENTER,405,57,205,42,0,0,NULL,IDFONT16,20,MTEXTCOLOR1);//mapname
    addtextitem(selmap,9,ISLABELCENTER,405,226,205,86,0,0,NULL,IDFONT10,10,MTEXTCOLOR1);//mapinfo
    addtextitem(selmap,14,ISLABELCENTER,405,310,209,20,0,0,NULL,IDFONT16,20,MTEXTCOLOR1);//mapsize,etc
    delmenuitem(selmap,10);
    addimagearray(selmap,10,446,98,MINIMAPW,MINIMAPW);
    setmenuitem_DISABLED(selmap,10,FALSE);
    setmenuitem_VISIBLED(selmap,10,FALSE);
#endif
    setmenuflags(selmap,MENUFLAGS_ALWAYSDRAW);
    mylist filelist;
    updatelistfiles=1;
    gamecounter=-2;
//    AddMenu_SomeCallback(selmap,&SelectMapCallbackFunc,NULL);
    do{
	if (updatelistfiles)
	{
	    updatelistfiles=0;
	    listfiles(".",&filelist,".sc",GAMEMAPPATH,"",ADDICONSTOLIST);
	    selpath[0]=COMMANDSYMB;
	    selpath[1]=ICONFOROPENDIR;
	    getcwd(currentpath,sizeof(currentpath)-1);
	    GetLastDir(currentpath,&selpath[2]);
	    changetextitem(selmap,7,selpath);
	    setlistbox_lists(selmap,6,selmap->menu[6].item.listbox->selectednr,&filelist,&showinfofromlistitem);
	    if (selmap->menu[6].item.listbox->selectednr<=0)
	    {
		int elemnr=parselistfiles(&filelist);
		if (elemnr>=0)
		{
		    changelistbox_selectednr(selmap,6,elemnr);
//			changelistbox_fromto(selmap,6,elemnr);
		}
	    }    
	    if (!filelist.GetMaxElements())
	    {
		setmenuitem_DISABLED(selmap,15,TRUE);
	    }
	    else
	    {
		setmenuitem_DISABLED(selmap,15,FALSE);
	    }
	}
	retstatus=drawmenu(selmap,MENUFLAGS_EMPTY);
//	doubleclick=retstatus&SELECTITEMDOUBLECLICK;
	retstatus&=~SELECTITEMDOUBLECLICK;
	switch(retstatus)
	{
	    case 19:
		    initselectmaplists(selmap);
		    break;
	    case 15://ok
	    case 6://listbox doubleclick
//		gamecounter=SECONDSREMAINTOGO;
//		setmenuitem_DISABLED(selmap,15,TRUE);	//ok button now is disabled
//		break;
//	    case CALLBACKCANCELFROMMENU9:
	    	//play the game

		//check if ok button is not disabled
		if (!menuitem_ISDISABLED(selmap,15))	//ok button is 15
		if (selmap->menu[6].item.listbox->selectednr!=-1)
		{
		    GAMETYPE=getexpandbox_selecteditem(selmap,19);
		    strncpy(listboxstr,(char *)filelist.GetElemNr(selmap->menu[6].item.listbox->selectednr),sizeof(listboxstr)-1);
		    if (listboxstr[0]==COMMANDSYMB&&
			(listboxstr[1]==ICONFORCLOSESELDIR||listboxstr[1]==ICONFORCLOSENOSELDIR))
		    {
			len=strlen(GLUALLSTR(GLUALL_TBL_UPONELEVEL));
			if (!strncmp(listboxstr+3,GLUALLSTR(GLUALL_TBL_UPONELEVEL),len))
			{
			    chdir("..");
			}
			else
			{
			    int len=strlen(listboxstr);
			    listboxstr[len-1]=0;
			    chdir(&listboxstr[3]);
			}
			selmap->menu[6].item.listbox->selectednr=0;
		    }
		    else
		    {
			//mapselected, now we go to play game
			nrplayers=0;
			len=strlen(listboxstr);
			getcwd(SELECTMAP,sizeof(SELECTMAP)-1);
			strncat(SELECTMAP,"/",sizeof(SELECTMAP)-1);
			strncat(SELECTMAP,&listboxstr[2],sizeof(SELECTMAP)-1);
		    	if (GAMETYPE==MAP_GAMETYPE_USEMAPSETTINGS)
		    	{
			    preparegameconf_ums();
			}
			else
			{
			    for (i=0;i<PLAYEDPLAYERS;i++)
			    {
				if (selmap->menu[21+i].item.expandbox->selectednr==1||
				    !menuitem_ISVISIBLED(selmap,21+i))//closed or no drawed
				{
				    gameconf.pl_race[i]=RACE_INACTIVE;
				    gameconf.pl_owner[i]=OWNER_NONE;
				}
				else
				{
				    nrplayers++;
				    gameconf.pl_race[i]=selmap->menu[29+i].item.expandbox->selectednr;
				    if (gameconf.pl_race[i]==RACE_INDEPENDENT)//random race to specific race
					gameconf.pl_race[i]=myrand(3);
			    	    if (i==NUMBGAMER)
					gameconf.pl_owner[i]=OWNER_HUMAN;
				    else
					gameconf.pl_owner[i]=OWNER_COMPUTER;
				}
			    }
			    if (nrplayers<=1)
			    {
				glu_putmenu(selmap,"rez\\glupok.bin",GLUALLSTR(GLUALL_TBL_ERRNOCOMPUTERS),fntadr,dlg);
			        break;
			    }
			}
			NUMBGAMER=SortPlayers(NUMBGAMER);
			repeat=0;
			exitstatus=0;//playmap
		    }
		}
		updatelistfiles=1;
		break;
	    case CANCELFROMMENU://ESC
	    case 16://cancel
		repeat=0;
		exitstatus=1;//menu on level up
		break;
	    case 22:
	    case 23:
	    case 24:
	    case 25:
	    case 26:
	    case 27:
	    case 28:
		if (selmap->menu[retstatus].item.expandbox->selectednr==1) //closed
		{
		    setmenuitem_VISIBLED(selmap,retstatus+8,FALSE);
		}
		else
		{
		    setmenuitem_VISIBLED(selmap,retstatus+8,TRUE);
		}
		break;
	    default:
		repeat=1;
		break;
	}
	if (updatelistfiles)
	    filelist.DeallocList();
    }while(repeat);

    installmousemoveevent(&mymousemoveevent);
    MenuDisappear(items,NULL);

    uninstallmousemoveevent();
    mytimer.ClearMyTimerFunc();
    unloadmousecursors();


    UnloadDialogBin(selmap);
    SetTranspTable(NULL);
    wfree(menutranspcolors);
    fontpcx.closePcx();
    wfree(dlg);
    backgnd.closePcx();
    unloaddlgicons();
    chdir(GAMEPATH);
    return(exitstatus);
}
//==========================================
void speedchange(MENUSTR *allmenus,int nr,int horizpos)
{
    char speedtxt[30];
    sprintf(speedtxt,"%s%c%s",
		     GLUALLSTR(GLUALL_TBL_GAMESPEEDTXT),RIGHTALIGNSYMB,
		     GLUALLSTR(GLUALL_TBL_SPEED_SLOWEST+horizpos));
    changetextitem(allmenus,19,speedtxt);
}
//==========================================
int CheckForDublicateGame(MENUSTR *allmenus,void *data)
{
    static int waitcycle=0;
    int err,continuereadnetw,updatelists,nr=networkresend;
    NETWORK_INFO *info;
    Guest_CheckNetwork(NULL,data);
    if (nr)
    {
	if (waitcycle++>=NETWORKWAITFORFILLALLGAMES*60/TIMETORECVDATA)
	{
	    waitcycle=0;
	    return(CheckForGameName(GAMETITLE));
	}
    }
    return(0);
}
//==========================================
int glu_creat(FORCE_SLOTS *fslots)
{
    int exitstatus=0,repeat=1,i,len,error,repeatcheck,updatelistfiles,elemnr,errmap;
    int retstatus;
    char *menutranspcolors,*fntadr;
    GRPFILE *dlg;
    PCX backgnd,fontpcx;
    char pal[256*4];
    char selpath[512];
    char listboxstr[256];
    char currentpath[1024];
    NETWORK_INFO checkgame;

    chdir(GAMEPATH);

    if (loaddlgicons())
	return -1;

    memset(&checkgame,0,sizeof(NETWORK_INFO));

    mpqloadfile(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],DLGGRP_STR),(char **)&dlg);
    backgnd.openMpqPcx(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],BACKGND_STR));
    backgnd.readPalFromPcx(pal,0);
    memcpy(GRP_vidmem,backgnd.GetPcxRawBytes(),backgnd.xsizePcx()*backgnd.ysizePcx());
//    backgnd.PutPcx(DELTASCREENX,DELTASCREENY2,PCX_EMPTYCOLOR1);
    palchange(pal,gameconf.videoconf.gamma,gameconf.videoconf.saturate);
    if (!gameconf.grmode.emulationmode)
	wscreenon();
    menutranspcolors=(char *)wmalloc(2*256*256);
    LoadTransPal(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],TRANS25_STR),
		    backgnd.GetRawPal768(),menutranspcolors,0.25f);
    LoadTransPal(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],TRANS25_STR),
		    backgnd.GetRawPal768(),menutranspcolors+256*256,0.5f);
    SetTranspTable(menutranspcolors);
    fontpcx.openMpqPcx(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],FNT_STR));
    fntadr=fontpcx.GetPcxRawBytes();//for glowtext
    fntadr[21]=fntadr[11];
    fntadr[22]=fntadr[12];

    MENUSTR *glucreat=LoadDialogBin("rez\\glucreat.bin",DIALOGBIN_MAINDIALOGS,IDFONT16);
    listboxlineitems(glucreat,6,6,20);
    menuspecialtables(glucreat,fntadr,dlg);
    setmenuflags(glucreat,MENUFLAGS_ALWAYSDRAW);

    mousetype=NORMALMOUSE;
    loadonecursor(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],ARROW_STR),mousetype);
    mytimer.SetMyTimerFunc(&menutimerandnetwork,NULL);
    restrictmousecoords(MOUSEMODE1);
    installmousemoveevent(&mymousemoveevent);

    MENUFIRSTDATA menushow[4];
    menushow[0].elemid=0;
    menushow[0].appearposition=MENUAPPEAR_FROMLEFT;
    menushow[1].elemid=1;
    menushow[1].appearposition=MENUAPPEAR_FROMRIGHT;
    menushow[2].elemid=2;
    menushow[2].appearposition=MENUAPPEAR_FROMBOTTOM;
    menushow[3].elemid=3;
    menushow[3].appearposition=MENUAPPEAR_FROMRIGHT;

    MENUAPPEAR *items = MenuAppear(glucreat,4,menushow,NULL);

    chdir(GAMEMAPPATH);
    if (EXPANSIONSET)
	chdir(ADDGAMEPATH);
    addmenupalette(glucreat,(unsigned char *)pal);

    setmenuitem_VISIBLED(glucreat,4,FALSE);	//pslots.pcx
    setmenuitem_VISIBLED(glucreat,18,FALSE);	//subtype
    setmenuitem_VISIBLED(glucreat,19,TRUE);
    setmenuitem_DISABLED(glucreat,19,FALSE);
    setmenuitem_VISIBLED(glucreat,21,FALSE);
    setmenuitem_VISIBLED(glucreat,23,TRUE);
    setmenuitem_DISABLED(glucreat,23,FALSE);
    
    strcpy(listboxstr,MYTBLSTR(MYINFO_TBL_MELEE));
    strcat(listboxstr,"\n");
    strcat(listboxstr,MYTBLSTR(MYINFO_TBL_FREEFORALL));
    strcat(listboxstr,"\n");
    strcat(listboxstr,MYTBLSTR(MYINFO_TBL_USEMAPSETT));

    addexpanditem_lists(glucreat,20,0,listboxstr);

    addhorizbutton_params(glucreat,23,7,7,&speedchange);
    sethorizbuttonpos(glucreat,23,3);	//middle position - normal speed

#ifdef STARMENUMAP_SHOWNEW
    //change positions and sizes
    deltextitem(glucreat,8,1);
    deltextitem(glucreat,9,1);
    deltextitem(glucreat,14,1);
    addtextitem(glucreat,8,ISLABELCENTER,405,57,205,42,0,0,NULL,IDFONT16,20,MTEXTCOLOR1);//mapname
    addtextitem(glucreat,9,ISLABELCENTER,405,226,205,86,0,0,NULL,IDFONT10,10,MTEXTCOLOR1);//mapinfo
    addtextitem(glucreat,14,ISLABELCENTER,405,310,209,20,0,0,NULL,IDFONT16,20,MTEXTCOLOR1);//mapsize,etc
    delmenuitem(glucreat,10);
    addimagearray(glucreat,10,446,98,128,128);
    setmenuitem_DISABLED(glucreat,10,FALSE);
    setmenuitem_VISIBLED(glucreat,10,TRUE);
#endif

    mylist filelist;
    updatelistfiles=1;
    do{
	if (updatelistfiles)
	{
	    updatelistfiles=0;
	    listfiles(".",&filelist,".sc",GAMEMAPPATH,"",ADDICONSTOLIST);
	    selpath[0]=COMMANDSYMB;
	    selpath[1]=ICONFOROPENDIR;
	    getcwd(currentpath,sizeof(currentpath)-1);
	    GetLastDir(currentpath,&selpath[2]);
	    setlistbox_lists(glucreat,6,glucreat->menu[6].item.listbox->selectednr,&filelist,&showinfofromlistitem);
	    if (glucreat->menu[6].item.listbox->selectednr<=0)
	    {
		elemnr=parselistfiles(&filelist);
		if (elemnr>=0)
		{
		    changelistbox_selectednr(glucreat,6,elemnr);
		}
	    }    
	    changetextitem(glucreat,7,selpath);
	}
	retstatus=drawmenu(glucreat,MENUFLAGS_EMPTY);
//	doubleclick=retstatus&SELECTITEMDOUBLECLICK;
	retstatus&=~SELECTITEMDOUBLECLICK;
	switch(retstatus)
	{
	    case 15://ok
	    case 6://listbox doubleclick
		if (!(glucreat->menu[15].dialogbin_flags&DIALOGBIN_FLAGS_ITEMDISABLED))
		if (glucreat->menu[6].item.listbox->selectednr!=-1)
		{
		    strncpy(listboxstr,(char *)filelist.GetElemNr(glucreat->menu[6].item.listbox->selectednr),sizeof(listboxstr)-1);
		    if (listboxstr[0]==COMMANDSYMB&&
			(listboxstr[1]==ICONFORCLOSESELDIR||listboxstr[1]==ICONFORCLOSENOSELDIR))
		    {
			len=strlen(GLUALLSTR(GLUALL_TBL_UPONELEVEL));
			if (!strncmp(listboxstr+3,GLUALLSTR(GLUALL_TBL_UPONELEVEL),len))
			{
			    chdir("..");
			}
			else
			{
			    int len=strlen(listboxstr);
			    listboxstr[len-1]=0;
			    chdir(&listboxstr[3]);
			}
			glucreat->menu[6].item.listbox->selectednr=0;
		    }
		    else
		    {
			len=strlen(listboxstr);
			getcwd(SELECTMAP,sizeof(SELECTMAP)-1);
			strncat(SELECTMAP,"/",sizeof(SELECTMAP)-1);
			strncat(SELECTMAP,&listboxstr[2],sizeof(SELECTMAP)-1);
			GAMETYPE=getexpandbox_selecteditem(glucreat,20);
			GAMESPEED=gethorizvalue(glucreat,23);
			GAMETITLE=nickname;//getplayername(NUMBGAMER);
			strcpy(GAMETITLESTR,GAMETITLE);
			BASENAMEMAP=strstr(SELECTMAP,"maps")+4;
			CHECKSUMMMAP=mkchecksumm(SELECTMAP);
			FILESIZEMAP=FILElength(SELECTMAP);
			
			mapinfo *testmap=(mapinfo *) wmalloc(sizeof(mapinfo));
			memset(testmap,0,sizeof(mapinfo));

			errmap=starmap_info(NULL,SELECTMAP,testmap);
			if (!errmap)
			{
			    starmap_forceslots(testmap,fslots,GAMETYPE);
			}
		    	unload_starmapallocated(testmap);
			wfree(testmap);
			if (errmap)
			{
			    error=NETWORK_OK;
			    glu_putmenu(glucreat,"rez\\glupok.bin",MYTBLSTR(MYINFO_TBL_ERRLOADMAP),fntadr,dlg);
			    break;
			}
			chdir(GAMEPATH);
			do{
			    repeatcheck=0;
			    error=CallNetwork(NETWORK_INITNETWORK,&checkgame);
			    if (error==NETWORK_OK)
			    {
				error=CallNetwork(NETWORK_INITBROADCAST,&checkgame);
				if (!error)
				    error=CallNetwork(NETWORK_INITRECVIDENTIFICATION,&checkgame);
				if (error==NETWORK_OK)
				{
				    retstatus=glu_putmenu(glucreat,"rez\\glupokdis.bin",MYTBLSTR(MYINFO_TBL_CHKDUBLGAME),
						    fntadr,dlg,&CheckForDublicateGame,&checkgame,1);
				    error=CallNetwork(NETWORK_CLOSENETWORK,&checkgame);
				    EmptyAllGamesList();
				    if (retstatus==2)
    				    {
					retstatus=glu_editmenu(glucreat,"rez\\glueokcancel.bin",MYTBLSTR(MYINFO_TBL_GAMEALRDEXIST),fntadr,dlg,GAMETITLESTR,STARCLONE_NETWORKJOIN_MAXGAMETITLESYMB);
					switch(retstatus)
					{
					    case 1://ok
					    case 4:
					    	repeatcheck=1;
						GAMETITLE=GAMETITLESTR;
						break;
					    case 3://
					    case CANCELFROMMENU:
						break;
					}
				    }
				    else if (retstatus==3)
				    {
					glu_putmenu(glucreat,"rez\\glupok.bin",GLUALLSTR(GLUALL_TBL_MANYGAMES),fntadr,dlg);
				    }
				    else
				    {
					repeat=0;
					exitstatus=CREATEDGAME;
				    }
				}
			    }
			}while(repeatcheck);
			if (error!=NETWORK_OK)
			{
			    glu_putmenu(glucreat,"rez\\glupok.bin",MYTBLSTR(MYINFO_TBL_NEWGAMENETERR),fntadr,dlg);
			}
			chdir(GAMEMAPPATH);
			if (EXPANSIONSET)
			    chdir(ADDGAMEPATH);
		    }
		}
		updatelistfiles=1;
		break;
	    case CANCELFROMMENU://ESC
	    case 16://cancel
		repeat=0;
		exitstatus=QUITMISSION;
		break;
	}
	if (updatelistfiles)
	    filelist.DeallocList();
    }while(repeat);

    installmousemoveevent(&mymousemoveevent);
    MenuDisappear(items,NULL);

    uninstallmousemoveevent();
    mytimer.ClearMyTimerFunc();
    unloadmousecursors();

    UnloadDialogBin(glucreat);
    SetTranspTable(NULL);
    wfree(menutranspcolors);
    fontpcx.closePcx();
    wfree(dlg);
    backgnd.closePcx();
    unloaddlgicons();
    chdir(GAMEPATH);
    return(exitstatus);
}
//==========================================
int SendMyInfoToNetwork(MENUSTR *allmenus,void *data)
{
    int err;
    NETWORK_INFO *info;
    info=(NETWORK_INFO *) data;
    info->netgame.players=GetPlayersMapBits();
    info->netgame.slots=GetSlotsMapBits();
    info->netgame.TypeGame=GAMETYPE;
    info->netgame.SpeedGame=GAMESPEED;
    info->netgame.mapsizex=MAPSIZEX;
    info->netgame.mapsizey=MAPSIZEY;
    info->PCID=MY_PCID;
    strncpy(info->netgame.gametitle,GAMETITLE,STARCLONE_NETWORKJOIN_MAXGAMETITLESYMB);
    info->netgame.gametitle[STARCLONE_NETWORKJOIN_MAXGAMETITLESYMB]=0;
    strncpy(info->netgame.mapname,MAPNAME,STARCLONE_NETWORKJOIN_MAXMAPNAMESYMB+1);
    info->netgame.mapname[STARCLONE_NETWORKJOIN_MAXMAPNAMESYMB]=0;
    err=CallNetwork(NETWORK_SENDIDENTIFICATION,info);
    return(err);
}
//==========================================
int Master_JoinPackets(MENUSTR *allmenus,void *data)
{
    int cmd,err,err2,continuereadnetw,newplayers=0;
    NETWPLAYERINFO netwinfo;
    WAITJOIN_INFO *join;
    char txt1[100];
    join=(WAITJOIN_INFO *) data;
    do{
	err=CallNetwork(NETWORK_DATAREADY,join->sock2);
	if (err==NETWORK_OK_RECVDATA)
	{
		err=CallNetwork(NETWORK_RECVPACKET,join->sock2);	  	//get info from network
		memset(&netwinfo,0,sizeof(NETWPLAYERINFO));
    		err=Master_CheckJoinPacket(allmenus,join->sock1,join->sock2,&netwinfo);
    		switch(err)
    		{
		    case 0:
		    case 1:
		    case 2:
		    case 3:
		    case 4:
		    case 5:
		    case 6:
		    case 7:
		    case 8:
		    case 9:
		    case 10:
		    case 11://0-12 playernr
			//connect player to me at slot err
    			join->sock1->Addr.sin_family = AF_INET;
			join->sock1->Addr.sin_port = htons(netwinfo.useport);
		        join->sock1->Addr.sin_addr.s_addr = join->sock2->Addr.sin_addr.s_addr;
			join->sock1->COMMAND=STARCLONE_NETWORKJOIN_COMMAND_YOUJOINED;
			join->sock1->PCID=netwinfo.PCID;
			sprintf(join->sock1->sendstr,SENDJOINRESPONCE_STR,
						    MY_PCID,
						    join->sock1->netgame.TypeGame,
						    join->sock1->netgame.SpeedGame,
						    join->sock1->netgame.mapsizex,
						    join->sock1->netgame.mapsizey,
						    join->sock2->connectport,
						    join->sock1->netgame.gametitle,
						    join->sock1->netgame.mapname,
						    GetMapPlayers(),
						    err
						     );
			err2=CallNetwork(NETWORK_SENDPACKET,join->sock1);
			if (err2==NETWORK_OK)
			{
			    newplayers+=AddConnectedPlayer(allmenus,&netwinfo);
			    setslotinfo(allmenus,err,OWNERGAMER,netwinfo.playername,GAMEPLAYERSLOT,PLAYERRANDOMSLOT);
			    txt1[0]=MYELLOWCOLORFONT;
			    sprintf(txt1+1,NETWSTR(NETWORK_TBL_PLAYERJOIN),netwinfo.playername);
			    setplayername(netwinfo.playernr,netwinfo.playername);
			    SendTextToChatItem(allmenus,10,txt1);
			    SendTextToConnectedPlayers(SYSTEMCHAT,txt1,join->sock1,join->sock2);
			}
			break;
		    case CANNOTJOINME:
		    case CANNOTJOINMEBANNED:
		    case CANNOTJOINMENAMEEXISTS:
			switch(err)
			{
			    case CANNOTJOINME:
				cmd=STARCLONE_NETWORKJOIN_COMMAND_FULLSLOTS;
				break;
			    case CANNOTJOINMEBANNED:
				cmd=STARCLONE_NETWORKJOIN_COMMAND_ISBANNED;
				break;
			    case CANNOTJOINMENAMEEXISTS:
				cmd=STARCLONE_NETWORKJOIN_COMMAND_NAMEEXISTS;
				break;
			}
    			join->sock1->Addr.sin_family = AF_INET;
			join->sock1->Addr.sin_port = htons(netwinfo.useport);
		        join->sock1->Addr.sin_addr.s_addr = join->sock2->Addr.sin_addr.s_addr;
			join->sock1->COMMAND=cmd;
			join->sock1->PCID=netwinfo.PCID;
			err2=CallNetwork(NETWORK_SENDPACKET,join->sock1);
			break;
		    case IGNOREJOINTOME:
			printf("ignore from unknown\n");
			break;
		    case ABUSEMEFROMUNKNOWN:
			printf("abuse from unknown\n");
			//don't replay to unknown player
			break;
		    case REFRESHPLAYERSAFTER:
			newplayers++;
			break;
		    case NEEDUPLOADMAP:
			err=SendCommandToPlayer(allmenus,netwinfo.playernr,STARCLONE_NETWORKJOIN_COMMAND_ASKFORFILE,join->sock1,join->sock2);
			break;
		    case DOWNLOADPARTOK:
			break;
		}//switch err
    	}
    	else//no data
    	    break;
    }while(1);
    if (networkresend)
    {
	networkresend=0;
	newplayers+=UpdatePlayersInfo(allmenus,join->sock1,join->sock2);
	err=SendMyInfoToNetwork(allmenus,join->sock1);	//proceed to send my game to the network
	if (err!=NETWORK_OK)
	    return(CALLBACKCANCELFROMMENU4);
//???? need to check if some of players exit the game
//	if (newplayers&&gamecounter!=2)	//somebody join cancel the game
//	{
//	    
//	}    
	if (letsgamecounter(allmenus,21,7))
	    return(CALLBACKCANCELFROMMENU9);//go go go - lets play
    }
    if (newplayers)
    {
        SendCommandToAll(allmenus,STARCLONE_NETWORKJOIN_COMMAND_ALLPLAYERSINFO,join->sock1,join->sock2);
    }
    return(0);
}
//==========================================
int Guest_JoinPackets(MENUSTR *allmenus,void *data)
{
    static int waitcycle=0,waitcycledownload=0,readinfo=0,prevsize=0;
    int err,err2,continuereadnetw;
    WAITJOIN_INFO *join;
    NETWORK_INFO sendrespond;
    join=(WAITJOIN_INFO *) data;
    do{	
	err=CallNetwork(NETWORK_DATAREADY,join->sock2);
	if (err==NETWORK_OK_RECVDATA)
	{
	    err=CallNetwork(NETWORK_RECVPACKET,join->sock2);	  	//get info from network
    	    err2=Guest_CheckJoinPacket(allmenus,join->sock1,join->sock2);
    	    if (err2==REQUESTFILEPARTS)
    	    {
    	        RequestFileParts(allmenus,join->sock1,join->sock2);
    	    }
    	    else
    	    if (err2>=CALLBACKCANCELFROMMENU2)
    	    {
	        waitcycle=0;
    	        return(err2);
    	    }
    	    readinfo+=err2;
	}
	else//no data
	    break;
    }while(1);
    if (networkresend)
    {
	networkresend=0;
	if (MAPUPLOAD==2)//if upload launched
	{
	    if (prevsize==MAPLOADEDLENGTH)
	    {
		if (waitcycledownload++>=NETWORKWAITFORSERVER*60/TIMETORECVDATA)
		{
		    waitcycledownload=0;
		    MAPUPLOAD=1;	//resend info about give me a file because last packet is lost
		}
	    }
	    else
	    {
		prevsize=MAPLOADEDLENGTH;
		waitcycledownload=0;
	    }
	}
	SendInfoToMaster(STARCLONE_NETWORKJOIN_COMMAND_PLAYERINFO,"",join->sock1,join->sock2);
	if (MAPUPLOAD==1)
	{
	    MAPUPLOAD=2;//set to prevent dowuble upload, because upload is need to launched
	    prevsize=0;
	}
	if (!readinfo)
	{
	    if (waitcycle++>=NETWORKWAITFORSERVER*60/TIMETORECVDATA)
	    {
		waitcycle=0;
		return(CALLBACKCANCELFROMMENU);
	    }
	}
	readinfo=0;
//???? need to check if some of players exit the game
	if (letsgamecounter(allmenus,21,7))
	    return(CALLBACKCANCELFROMMENU9);//go go go - lets play
    }
    return(0);
}
//==========================================
void SendTextToChatItem(MENUSTR *gluchat,int nr,char *txt)
{
    char *chattxt;
    int txtoffset=0,nrsymb;
    do{
        nrsymb=visibletext(gluchat->menu[nr].fontnr,gluchat->menu[nr].item.listbox->sizexarea,txt+txtoffset);
        chattxt=(char *)wmalloc(nrsymb+1);
	strncpy(chattxt,txt+txtoffset,nrsymb);
	chattxt[nrsymb]=0;
	txtoffset+=nrsymb;
	listbox_addtext(gluchat,nr,chattxt);
//	gluchat->menu[nr].item.listbox->flist->AddList(chattxt);
    }while(txt[txtoffset]!=0);
//    setlistbox_lists(gluchat,nr,0,gluchat->menu[nr].item.listbox->flist);
    changelistbox_fromto(gluchat,nr,gluchat->menu[nr].item.listbox->flist->GetMaxElements()-1);
}
//==========================================
void createplayerslottabs(char *str,int withobserverslot)
{
    strcpy(str,NETWSTR(NETWORK_TBL_ZERGRACE));
    strcat(str,"\n");
    strcat(str,NETWSTR(NETWORK_TBL_TERRANRACE));
    strcat(str,"\n");
    strcat(str,NETWSTR(NETWORK_TBL_PROTOSSRACE));
    strcat(str,"\n");
    strcat(str,NETWSTR(NETWORK_TBL_RANDOMRACE));
    if (withobserverslot)
    {
	strcat(str,"\n");
	strcat(str,NETWSTR(NETWORK_TBL_OBSERVERRACE));
    }
}
//==========================================
int glu_chat(int masterjoin,int playernr,FORCE_SLOTS *fslots)
{
    int retstatus,retstatus2,exitstatus,i,repeat,elemnr,PCID,sellist,menuappear,connslot;
    int err1,err2,error;
    GRPFILE *dlg;
    PCX backgnd,fontpcx;
    MENUSTR *gluchat,*gluerr;
    NETWORK_INFO broadcast,waitconn;
    WAITJOIN_INFO bothsock;
    char *errtxt,*editboxtxt;
    char txt1[100];

    char *menutranspcolors,*fntadr;
    char pal[256*4];

    memset(&broadcast,0,sizeof(NETWORK_INFO));
    memset(&waitconn,0,sizeof(NETWORK_INFO));

    bothsock.sock1=&broadcast;
    bothsock.sock2=&waitconn;

    MAPUPLOAD=0;
    MAPLOADEDLENGTH=0;
    MAPVERIFIED=0;

    mpqloadfile(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],DLGGRP_STR),(char **)&dlg);
    backgnd.openMpqPcx(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],BACKGND_STR));
    backgnd.readPalFromPcx(pal,0);
//    Gimp_SavePal("palnl.gpl","starcraft-palnl",pal,4);
    memcpy(GRP_vidmem,backgnd.GetPcxRawBytes(),backgnd.xsizePcx()*backgnd.ysizePcx());
//    backgnd.PutPcx(DELTASCREENX,DELTASCREENY2,PCX_EMPTYCOLOR1);
    palchange(pal,gameconf.videoconf.gamma,gameconf.videoconf.saturate);
    if (!gameconf.grmode.emulationmode)
	wscreenon();
    menutranspcolors=(char *)wmalloc(2*256*256);
    LoadTransPal(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],TRANS25_STR),
		backgnd.GetRawPal768(),menutranspcolors,0.25f);
    LoadTransPal(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],TRANS50_STR),
		backgnd.GetRawPal768(),menutranspcolors+256*256,0.5f);
    SetTranspTable(menutranspcolors);

    fontpcx.openMpqPcx(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],FNT_STR));
    fntadr=fontpcx.GetPcxRawBytes();//for glowtext
    fntadr[21]=fntadr[11];
    fntadr[22]=fntadr[12];

    gluchat=LoadDialogBin("rez\\gluchat.bin",DIALOGBIN_MAINDIALOGS,IDFONT16);
    menuspecialtables(gluchat,fntadr,dlg);
    setmenuflags(gluchat,MENUFLAGS_ALWAYSDRAW);
    gamecounter=-2;

    if (masterjoin==OWNERGAMER)
    {
	menuappear=5;
	setmenuitem_VISIBLED(gluchat,6,TRUE);	//ok button
    }
    else
    {
	menuappear=4;
	setmenuitem_VISIBLED(gluchat,6,FALSE);	//ok button
	setmenuitem_VISIBLED(gluchat,4,FALSE);	//ok image
    }

    err1=CallNetwork(NETWORK_INITNETWORK,&broadcast);
    err1=CallNetwork(NETWORK_INITBROADCAST,&broadcast);

    err2=CallNetwork(NETWORK_INITNETWORK,&waitconn);
    err2=CallNetwork(NETWORK_INITRECVPACKET,&waitconn);
    if (masterjoin==OWNERGAMER)
    {
	broadcast.connectport=waitconn.connectport;
    }
    error=0;

    if (err1!=NETWORK_OK||err2!=NETWORK_OK)
    {
	error=-1;
	if (err2==NETWORK_ERROR_SOCKETBIND)
	    errtxt=MYTBLSTR(MYINFO_TBL_NETPORTINUSE);
    	else
	    errtxt=MYTBLSTR(MYINFO_TBL_NEWGAMENETERR);
	gluerr=LoadDialogBin("rez\\glupok.bin",DIALOGBIN_MAINDIALOGS,IDFONT16);
	menuspecialtables(gluerr,fntadr,dlg);
	setmenuflags(gluerr,MENUFLAGS_ALWAYSDRAW);
	AddPrevMenuShowing(gluerr,gluchat);
	changetextitem(gluerr,2,errtxt);
    }

    mousetype=NORMALMOUSE;
    loadonecursor(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],ARROW_STR),mousetype);
    mytimer.SetMyTimerFunc(&menutimerandnetwork,NULL);
    restrictmousecoords(MOUSEMODE1);
    installmousemoveevent(&mymousemoveevent);
        
    MENUFIRSTDATA menushow[5];
    menushow[0].elemid=0;
    menushow[0].appearposition=MENUAPPEAR_FROMBOTTOM;
    menushow[1].elemid=1;
    menushow[1].appearposition=MENUAPPEAR_FROMRIGHT;
    menushow[2].elemid=2;
    menushow[2].appearposition=MENUAPPEAR_FROMLEFT;
    menushow[3].elemid=3;
    menushow[3].appearposition=MENUAPPEAR_FROMBOTTOM;
    menushow[4].elemid=4;
    menushow[4].appearposition=MENUAPPEAR_FROMRIGHT;

    MENUAPPEAR *items;
    if (error<0)
	items = MenuAppear(gluchat,menuappear,menushow,gluerr);
    else
	items = MenuAppear(gluchat,menuappear,menushow,NULL);

    if (error<0)
    {
        drawmenu(gluerr,MENUFLAGS_EMPTY);
	UnloadDialogBin(gluerr);
    }
    else
    {
	mylist chatlist;
	if (masterjoin==OWNERGAMER)
	    CreateGameSlots(fslots->openedslots,NUMBGAMER);
	else
	    CreateGameSlots(MAXPLAYERS,NUMBGAMER);
	gluchat_initmenus(gluchat,masterjoin,playernr,fslots);
	repeat=1;
	if (masterjoin==OWNERGAMER)
	    AddMenu_SomeCallback(gluchat,&Master_JoinPackets,(void *)&bothsock);
	else
	    AddMenu_SomeCallback(gluchat,&Guest_JoinPackets,(void *)&bothsock);
	setlistbox_lists(gluchat,10,-1,&chatlist);
	do{
	    repeat=1;
    	    retstatus=drawmenu(gluchat,MENUFLAGS_EMPTY);
	    retstatus&=~SELECTITEMDOUBLECLICK;
	    switch(retstatus)
	    {
		case CALLBACKCANCELFROMMENU:	//servertimeout
			if (masterjoin==OWNERGAMER)
			{
			    glu_putmenu(gluchat,"rez\\glupok.bin",MYTBLSTR(MYINFO_TBL_SENDERROR),fntadr,dlg);
			}
			else
			{
			    SERVER_PCID=0;//clear server id to prevent abuse to send packets about game
			    glu_putmenu(gluchat,"rez\\glupok.bin",MYTBLSTR(MYINFO_TBL_SERVERTIMEOUT),fntadr,dlg);
			}
			repeat=0;
			exitstatus=EXITCREATEDGAME;
			break;
		case CALLBACKCANCELFROMMENU2:	//cancel game creation
			if (masterjoin==OWNERGAMER)
			    printf("error receive cancel game, but i'm master\n");
			else
			{
			    SERVER_PCID=0;//clear server id to prevent abuse to send packets about game
			    glu_putmenu(gluchat,"rez\\glupok.bin",MYTBLSTR(MYINFO_TBL_HOSTHASLEFT),fntadr,dlg);
			}
			repeat=0;
			exitstatus=EXITCREATEDGAME;
			break;
		case CALLBACKCANCELFROMMENU3:	//lets counter begin
		case 6://ok
			if (masterjoin==OWNERGAMER)
			{
			    //check if uploading map
			    if (CheckForPlayersReady())
			    {
				glu_putmenu(gluchat,"rez\\glupok.bin",GLUALLSTR(GLUALL_TBL_PLAYERNOTREADY),fntadr,dlg);
				break;
			    }
			    else
			    {
				Master_MakeMapConf();
				Master_CopyPlayersAddr();
				force_slots.CreatePlayersNr();
				if (force_slots.playedplayers<=1)
				{
				    glu_putmenu(gluchat,"rez\\glupok.bin",MYTBLSTR(MYINFO_TBL_ERRNOPLAYERS),fntadr,dlg);
				    break;
				}
				SendCommandToAll(gluchat,STARCLONE_NETWORKJOIN_COMMAND_PLAYGAME,bothsock.sock1,bothsock.sock2);
			    }
			}
			gamecounter=SECONDSREMAINTOGO;
			setmenuitem_DISABLED(gluchat,6,TRUE);	//ok button now is disabled
			break;
		case CALLBACKCANCELFROMMENU4:	//broadcast info game error
			SERVER_PCID=0;//clear server id to prevent abuse to send packets about game
			glu_putmenu(gluchat,"rez\\glupok.bin",MYTBLSTR(MYINFO_TBL_NETWORKERRORSEND),fntadr,dlg);
			repeat=0;
			exitstatus=EXITCREATEDGAME;
			break;
		case CALLBACKCANCELFROMMENU5:	//you bannned
			if (masterjoin==OWNERGAMER)
			    printf("error receive ban player, but i'm master\n");
			else
			{
			    SERVER_PCID=0;//clear server id to prevent abuse to send packets about game
			    glu_putmenu(gluchat,"rez\\glupok.bin",GLUALLSTR(GLUALL_TBL_YOUHAVEBEENBANNED),fntadr,dlg);
			}
			repeat=0;
			exitstatus=EXITCREATEDGAME;
			break;
		case CALLBACKCANCELFROMMENU8:	//downloadedmap is wrong
			if (masterjoin==OWNERGAMER)
			    printf("error receive downloaded map, i'm master\n");
			else
			{
			    SERVER_PCID=0;//clear server id to prevent abuse to send packets about game
			    glu_putmenu(gluchat,"rez\\glupok.bin",GLUALLSTR(GLUALL_TBL_ERRDOWNLOADMAP),fntadr,dlg);
			}
			repeat=0;
			exitstatus=EXITCREATEDGAME;
			break;
		case CALLBACKCANCELFROMMENU9:	//message from master to play the game
			if (masterjoin==OWNERGAMER)
			{
			    //need to remake those info because in time of start (in 5 sec)
			    //any body can exit or connect to the game
			    Master_MakeMapConf();
		    	    Master_CopyPlayersAddr();
			    force_slots.CreatePlayersNr();
			    RNDSEED=getseed();
//				printf("OWNER RNDSEED=%d\n",RNDSEED);
//			    SendCommandToAll(gluchat,STARCLONE_NETWORKJOIN_COMMAND_PLAYGAME,bothsock.sock1,bothsock.sock2);
			    NUMBGAMER=SortPlayers(NUMBGAMER);
			}
			else
			{
			    force_slots.CreatePlayersNr();
			    NUMBGAMER=SortPlayers(NUMBGAMER);
			    //can't be wrong
/*			    if (force_slots.playedplayers<=1)
			    {
				glu_putmenu(gluchat,"rez\\glupok.bin",GLUALLSTR(GLUALL_TBL_ERRNOPLAYERS),fntadr,dlg);
				break;
			    }
*/
			}
//			printf("RNDSEED=%d\n",RNDSEED);
			setseed(RNDSEED);
			repeat=0;
			exitstatus=PLAYNETWORKGAME;
			break;
		case CANCELFROMMENU://ESC
		case 7://ESC
			if (masterjoin==OWNERGAMER)
			    SendCommandToAll(gluchat,STARCLONE_NETWORKJOIN_COMMAND_CLOSEGAME,bothsock.sock1,bothsock.sock2);
			else
			    SendInfoToMaster(STARCLONE_NETWORKJOIN_COMMAND_LEAVEGAME,txt1,bothsock.sock1,bothsock.sock2);
			repeat=0;
			exitstatus=EXITCREATEDGAME;
			break;
		case 9:
			if (geteditboxtextsize(gluchat,9)!=0)
			{
			    editboxtxt=geteditboxtext(gluchat,9);
			    txt1[0]=MWHITECOLORFONT;
//			    txt1[0]=MYELLOWCOLORFONT;
			    strcpy(txt1+1,getplayername(NUMBGAMER));
//			    strcat(txt1,":" MWHITECOLORFONTCHAR " ");
			    strcat(txt1,": ");
			    strcat(txt1,editboxtxt);
			    SendTextToChatItem(gluchat,10,txt1);
			    changeeditboxtext(gluchat,9,"");
			    if (masterjoin==OWNERGAMER)
				SendTextToConnectedPlayers(NUMBGAMER,txt1,bothsock.sock1,bothsock.sock2);
		    	    else
				SendInfoToMaster(STARCLONE_NETWORKJOIN_COMMAND_MESSAGE,txt1,bothsock.sock1,bothsock.sock2);
			    //chat function
			}
			break;
		case 33:
		case 37:
		case 41:
		case 45:
		case 49:
		case 53:
		case 57:
		case 61:
		case 65:
		case 69:
		case 73:
		case 77:
//			if ((retstatus-33)/4==playernr)
//			    break;
			connslot=GetGameConnectedSlot((retstatus-33)/4);
			switch(connslot)
			{
			    case GAMEPLAYERSLOT:
				createplayerslottabs(txt1,1);	//with observerslot
				addexpanditem_lists(gluchat,retstatus+1,-1,txt1);
				if (getexpandbox_selecteditem(gluchat,retstatus)==1)//ban player
				{
				    SendCommandToPlayer(gluchat,(retstatus-33)/4,STARCLONE_NETWORKJOIN_COMMAND_BANPLAYER,bothsock.sock1,bothsock.sock2);
				    AddToBanList((retstatus-33)/4);
				}
				break;
			    case GAMEOPENSLOT:
			    case GAMECOMPUTERSLOT:
			    case GAMECLOSEDSLOT:
				if (getexpandbox_selecteditem(gluchat,retstatus)==GAMECLOSEDSLOT||
					getexpandbox_selecteditem(gluchat,retstatus)==GAMEOPENSLOT)//closed or open param
				    setmenuitem_VISIBLED(gluchat,retstatus+1,FALSE);
				else
				    setmenuitem_VISIBLED(gluchat,retstatus+1,TRUE);
				SetGameConnectedSlot((retstatus-33)/4,getexpandbox_selecteditem(gluchat,retstatus));
				if (getexpandbox_selecteditem(gluchat,retstatus)==GAMECOMPUTERSLOT)
				{
				    createplayerslottabs(txt1,0);	//no need observer slot
				    addexpanditem_lists(gluchat,retstatus+1,-1,txt1);
				}
				else
				{
				    createplayerslottabs(txt1,1);	//with observerslot
				    addexpanditem_lists(gluchat,retstatus+1,-1,txt1);
				}
				break;
			}
			break;
		case 34:
		case 38:
		case 42:
		case 46:
		case 50:
		case 54:
		case 58:
		case 62:
		case 66:
		case 70:
		case 74:
		case 78:
			SetGameValueSlot((retstatus-33)/4,getexpandbox_selecteditem(gluchat,retstatus));
			break;
	    }
	}while(repeat);
	chatlist.DeallocList();
    }
    if (masterjoin==OWNERGAMER)
    {
	EmptyConnectedPlayers();
    }
    err1=CallNetwork(NETWORK_SENDCLOSEIDENTIFICATION,&broadcast);
    CallNetwork(NETWORK_CLOSENETWORK,&broadcast);
    CallNetwork(NETWORK_CLOSENETWORK,&waitconn);

    installmousemoveevent(&mymousemoveevent);
    MenuDisappear(items,NULL);

    uninstallmousemoveevent();
    mytimer.ClearMyTimerFunc();
    unloadmousecursors();

    UnloadDialogBin(gluchat);
    fontpcx.closePcx();
    SetTranspTable(NULL);
    wfree(menutranspcolors);
    wfree(dlg);
    backgnd.closePcx();
    return(exitstatus);
}
//==========================================
void gluchat_initmenus(MENUSTR *menu,int masterjoin,int playernr,FORCE_SLOTS *fslots)
{
    int i,force;
    int playslot=0;
    char txt1[50];
    char txt2[50];
    
    createplayerslottabs(txt1,1);

    strcpy(txt2,GLUALLSTR(GLUALL_TBL_OPENLIST));
    strcat(txt2,"\n");
    strcat(txt2,GLUALLSTR(GLUALL_TBL_CLOSEDLIST));
    strcat(txt2,"\n");
    strcat(txt2,GLUALLSTR(GLUALL_TBL_COMPUTERLIST));
    setmenuitem_VISIBLED(menu,8,FALSE);
    for (i=0;i<MAXPLAYERS;i++)
    {
	menu->menu[i*4+32].hotxsize=16*8;
	changemenuitemtype(menu,i*4+32,ISEXPANDBOX,IDFONT10,MLISTCOLOR1);
	changemenuitemtype(menu,i*4+33,ISEXPANDBOX,IDFONT10,MLISTCOLOR1);
	changemenuitemtype(menu,i*4+34,ISEXPANDBOX,IDFONT10,MLISTCOLOR1);
	changemenuitemtype(menu,i*4+35,ISIMAGE,IDFONT8,MTEXTCOLOR1);
	addimagearray(menu,i*4+35,-1,-1,-1,-1);

	setmenuitem_VISIBLED(menu,i*4+32,FALSE);
	setmenuitem_VISIBLED(menu,i*4+33,FALSE);
	setmenuitem_VISIBLED(menu,i*4+34,FALSE);
	setmenuitem_VISIBLED(menu,i*4+35,FALSE);
	setmenuitem_DISABLED(menu,i*4+32,TRUE);
	setmenuitem_DISABLED(menu,i*4+33,TRUE);
	setmenuitem_DISABLED(menu,i*4+34,TRUE);
	setmenuitem_DISABLED(menu,i*4+35,TRUE);
	addexpanditem_lists(menu,i*4+33,0,txt2);
	addexpanditem_lists(menu,i*4+34,3,txt1);
	changeimageitem(menu,i*4+35,GLUCHAT_DOWNLOADPROGRESSICON);
    }
    
    setmenuitem_DISABLED(menu,25,TRUE);
    setmenuitem_DISABLED(menu,26,TRUE);
    changeeditboxparam(menu,9,"",MAXCHATSYMB);
    menu->menu[9].fontnr=IDFONT10;	//chat line
    setdefaultbutton(menu,9);
    menu->menu[10].fontnr=IDFONT10;	//chat list
    listboxlineitems(menu,10,7,14);
    menu->menu[10].colors4=MCHATCOLOR;

    for (i=0,force=0;i<MAXPLAYERS;i++)
    {
	if (fslots->usedslots1[i]!=0)
	{
	    if (fslots->usedslots1[i]==OWNER_FORCENAME)
	    {
		setmenuitem_VISIBLED(menu,i*4+32,TRUE);
		setmenuitem_DISABLED(menu,i*4+32,TRUE);
		addexpanditem_lists(menu,i*4+32,0,fslots->forcenames[force]);
		force++;
	    }
	    else
	    {
		setmenuitem_VISIBLED(menu,i*4+33,TRUE);
		if (i == playernr)
		{
		    setmenuitem_VISIBLED(menu,i*4+34,TRUE);
		}
		else
		{
		    setmenuitem_VISIBLED(menu,i*4+34,FALSE);
		}
		if (masterjoin==GUESTGAMER&&i!=playernr)//disabled all except me if join to game
		{
		    setmenuitem_DISABLED(menu,i*4+33,TRUE);
		    setmenuitem_DISABLED(menu,i*4+34,TRUE);
		}
		else
		{
		    setmenuitem_DISABLED(menu,i*4+33,FALSE);
		    setmenuitem_DISABLED(menu,i*4+34,FALSE);
		}
	    }
	}
    }
    changetextitem(menu,19,GAMETITLE);
    menu->menu[19].colors4=MTEXTCOLORWHITE;
//    changetextitem(menu,20,MYTBLSTR(MYINFO_TBL_USEMAPSETT));
    changetextitem(menu,20,MYTBLSTR(MYINFO_TBL_MELEE+GAMETYPE));
    menu->menu[20].colors4=MTEXTCOLORWHITE;
    changetextitem(menu,22,MAPNAME);
    menu->menu[22].colors4=MTEXTCOLORWHITE;
    sprintf(txt1,GLUALLSTR(GLUALL_TBL_MAPSIZEXY),MAPSIZEX,MAPSIZEY);
    changetextitem(menu,23,txt1);
    changetextitem(menu,24,GLUALLSTR(GLUALL_TBL_SPEED_SLOWEST+GAMESPEED));
    addexpanditem_lists(menu,playernr*4+33,0,getplayername(playernr));
    SetGameConnectedSlot(playernr,GAMEPLAYERSLOT);
}
//==========================================
char *myhelpmenutext[1]=
{
	GWHITECOLORFONTCHAR GAMENAME " Demo " GAMEVERSION
};
//==========================================
int glu_putmenu(MENUSTR *prevmenu,char *dialogbinfile,char *errormessage,char *fonttable,GRPFILE *dlg)
{
    return(glu_putmenu(prevmenu,dialogbinfile,errormessage,fonttable,dlg,NULL,NULL,1));
}
//==========================================
int glu_putmenu(MENUSTR *prevmenu,char *dialogbinfile,char *errormessage,char *fonttable,GRPFILE *dlg,int sounds)
{
    return(glu_putmenu(prevmenu,dialogbinfile,errormessage,fonttable,dlg,NULL,NULL,sounds));
}
//==========================================
int glu_putmenu(MENUSTR *prevmenu,char *dialogbinfile,char *errormessage,
		char *fonttable,GRPFILE *dlg,int (*callbackfunc)(MENUSTR *,void *info),
		void *somecallbackdata,int sounds)
{
    int ret;
    MENUSTR *menu=LoadDialogBin(dialogbinfile,DIALOGBIN_MAINDIALOGS,IDFONT16);
    if (!sounds)
	addsoundstomenu(menu,-1,-1);
    menuspecialtables(menu,fonttable,dlg);
    changetextitem(menu,2,errormessage);
    setmenuflags(menu,MENUFLAGS_ALWAYSDRAW);
    AddPrevMenuShowing(menu,prevmenu);
    if (callbackfunc)
	AddMenu_SomeCallback(menu,callbackfunc,somecallbackdata);
    ret=drawmenu(menu,MENUFLAGS_EMPTY);
    UnloadDialogBin(menu);
    return(ret);
}
//==========================================
#define SC_HISTORY_TBL		"rez\\gluhist.tbl"
//==========================================
int glu_selectmission(MENUSTR *prevmenu,int campaignnr,char *fonttable,GRPFILE *dlg)
{
    int ret,i,j,lenmiss,fromraceline,totallines,exitvalue,alldone,listboxvalue;
    char *namemission,*namemission2;
    unsigned char items[MAXMISSIONSVIDEOS];
    unsigned char mission_id;
    mylist missionlist;
    TBL	*history_tbl;
    MENUSTR *menu=LoadDialogBin("rez\\gluhist.bin",DIALOGBIN_MAINDIALOGS,IDFONT16);
    setimgtransparentcolors(menu,0,0,254);

    menuspecialtables(menu,fonttable,dlg);
    setdefaultbutton(menu,2);
    setmenuflags(menu,MENUFLAGS_ALWAYSDRAW);
    AddPrevMenuShowing(menu,prevmenu);

    history_tbl = new TBL;
    history_tbl->loadTBL(SC_HISTORY_TBL);

    listboxlineitems(menu,4,8,20);
    setlistbox_lists(menu,4,0,&missionlist);
    alldone = curplayer[0].missions[campaignnr].campaigndone;
    fromraceline = curplayer[0].missions[campaignnr].fromlineinhisttbl;
    totallines   = curplayer[0].missions[campaignnr].nrlinesinhisttbl;
    for (i=0,j=0;i<totallines;i++)
    {
	mission_id = curplayer[0].missions[campaignnr].seq_missions[i];
	//this is only textview(epilog,etc...)
	if ( ( mission_id & 0x3f ) == 0x3f )
	    continue;
	if (mission_id&0x80)	//done, so we can add to list
	{
	    items[j++]=i;
	    namemission2=history_tbl->get_TBL_STR(fromraceline+(mission_id & 0x3f)); //remove done & video bits
	    lenmiss=strlen(namemission2);
	    namemission=(char *)wmalloc(lenmiss+1);
	    strcpy(namemission,namemission2);
	    listbox_addtext(menu,4,namemission);
	}
    }
    if (alldone)
    {
	menu->menu[4].item.listbox->selectednr=0;
	changelistbox_fromto(menu,4,-100);
    }
    setmenuitem_VISIBLED(menu,4,TRUE);
    ret=drawmenu(menu,MENUFLAGS_EMPTY);
    switch(ret)
    {
	case CANCELFROMMENU://ESC
	case 3://ESC
	    exitvalue=-1;
	    break;
	case 2://ok
	case 4://listbox
	    listboxvalue = items[getlistbox_selecteditem(menu,4)];
	    exitvalue = (curplayer[0].missions[campaignnr].seq_missions[listboxvalue] & 0x7f) | (listboxvalue<<16);
	    break;
	default:
	    exitvalue=-1;
	    break;
    }
    UnloadDialogBin(menu);
    if (history_tbl)
    {
	delete history_tbl;
	history_tbl=NULL;
    }
    return(exitvalue);
}
//==========================================
int glu_editmenu(MENUSTR *prevmenu,char *dialogbinfile,char *errormessage,char *fonttable,GRPFILE *dlg,char *str,int maxstrlen)
{
    int ret;
    MENUSTR *menu=LoadDialogBin(dialogbinfile,DIALOGBIN_MAINDIALOGS,IDFONT16);
    menuspecialtables(menu,fonttable,dlg);
    changetextitem(menu,2,errormessage);
    changeeditboxparam(menu,4,str,maxstrlen);
    setdefaultbutton(menu,4);
    setmenuflags(menu,MENUFLAGS_ALWAYSDRAW);
    AddPrevMenuShowing(menu,prevmenu);
    ret=drawmenu(menu,MENUFLAGS_EMPTY);
    strcpy(str,geteditboxtext(menu,4));
    UnloadDialogBin(menu);
    return(ret);
}
//==========================================
void putokmenu(char *errormessage)
{
    MENUSTR *okmenu=LoadDialogBin("rez\\ok.bin",DIALOGBIN_GAMEDIALOGS,IDFONT16);
    menuspecialtables(okmenu,tfontgamp,gamedlggrp);
    changetextitem(okmenu,1,errormessage);
    ShowMenuEdge(okmenu);
    drawmenu(okmenu,MENUFLAGS_EMPTY);
    UnloadDialogBin(okmenu);
}
//==========================================
int putokcancelmenu(char *errormessage)
{
    int err;
    MENUSTR *okcancelmenu=LoadDialogBin("rez\\okcancel.bin",DIALOGBIN_GAMEDIALOGS,IDFONT16);
    menuspecialtables(okcancelmenu,tfontgamp,gamedlggrp);
    changetextitem(okcancelmenu,2,errormessage);
    ShowMenuEdge(okcancelmenu);
    err=drawmenu(okcancelmenu,MENUFLAGS_EMPTY);
    UnloadDialogBin(okcancelmenu);
    return(err);
}
//==========================================
void titlerefresh(void *param1,int param2)
{
    static int here=0,prev_tick;
    static PCX font;
    PCX title1,title2;
    int i,sx,sy,err2;
    if (here)
	return;
    MENUSTR *dialog=(MENUSTR *)param1;
    here=1;
    if (param2==MYTIMERFUNC_FIRSTTIMEENTER)//the first time
    {
	char newpal[256*4];
        title1.openMpqPcx(makefilename(GLUETITLE_NAME,GLUETITLE_OFFSET,0,0,TITLE_STR));

	sx=title1.xsizePcx();
	sy=title1.ysizePcx();
    	title1.readPalFromPcx(newpal,0);//readfourbytepalette
//	title1.PutScaledPcx(DELTASCREENX,DELTASCREENY2,PCX_EMPTYCOLOR1);
	title1.PutPcx(DELTASCREENX,DELTASCREENY2,PCX_EMPTYCOLOR1);
	title1.closePcx();

	font.openMpqPcx(makefilename(GLUETITLE_NAME,GLUETITLE_OFFSET,0,0,FNT_STR));
	menuspecialtables(dialog,font.GetPcxRawBytes(),NULL);
	checkanddrawmenu(dialog,-2,ITEM_RESTOREANDFREE);
	palchange(newpal,gameconf.videoconf.gamma,gameconf.videoconf.saturate);
	if (!gameconf.grmode.emulationmode)
	    wscreenon();
    }
    else
	if (param2==MYTIMERFUNC_LASTTIMEENTER)
	{
	    font.closePcx();
	    UnloadDialogBin(dialog);
	}
	else
	{
            if (tick_timer-prev_tick>=TIMETOSCROLLTITLE)
//            if (!(tick_timer%TIMETOSCROLLTITLE))
            {
		prev_tick=tick_timer;
		dialog->menu[3].dialogbin_flags^=DIALOGBIN_FLAGS_ITEMVISIBLED;
		checkanddrawmenu(dialog,-2,ITEM_RESTOREANDFREE);
//		wscreenonregion(dialog->menu[3].hotdeltax,dialog->menu[3].hotdeltay,
//			    	dialog->menu[3].hotxsize,dialog->menu[3].hotysize);
		wscreenon();
            }
	}
    here=0;
}
//==========================================
MENUSTR *showtitle(void)
{
    return(DialogBin_ShowPermanent("rez\\titledlg.bin",DIALOGBIN_GAMEDIALOGS,0,&titlerefresh));
}
//==========================================
void cleartitle(MENUSTR *title)
{
    DialogBin_ClosePermanent(title);
}
//==========================================
void MakeObserver(int playernr)
{
    OBJ *a;
    int i,pl;
    PLAYER[playernr].isobserverflag|=PLAYER_ISOBSERVER;
    for (pl=0;pl<force_slots.realplayers;pl++)
    {
        i=force_slots.playernr[pl];
        CreateAliance(&map,playernr,i,NEUTRALOBJ);
        CreateAliance(&map,i,playernr,NEUTRALOBJ);
    }
    minimapmenu->iteminfo[MINIMAPDIALOG_DIPLOMACYBUTTON].Flags|=DIALOGBIN_FLAGS_ITEMDISABLED;
    minimapmenu->iteminfo[MINIMAPDIALOG_MESSAGEBUTTON].Flags|=DIALOGBIN_FLAGS_ITEMDISABLED;
    //all players units are stopped from their work
    for (i=0;i<MaxObjects;i++)
    {
	a=objects[i];
	if (a->playernr==playernr)
	    moveobj(a,NULL,MODESTOP,NOSHOWERROR);
    }
}
//==========================================
void MakeDisconnect(int playernr)
{
    int i;
    MakeObserver(playernr);
    PLAYER[playernr].isobserverflag|=PLAYER_DISCONNECTED;
}
//==========================================

//statbtnp.bin		3x3 action buttons
//statdata.bin		56 elements (unitname,life,mana,constr,..)
//statport.bin		portrait button
//statres.bin		resources coords
//minimap.bin

//resdata.bin
//0-big uniticon,   1- unitname,   2-shield/life,    3 - mana/timeleft, 4-name status of unit (hal,paras,etc.)
//5-9 - icon of 5 constr units,    11 - ???,         12-(name of upg/constr/tech) 13 - constr/icon upgrade, 14 - upgrade %
//15-18 base build text,          19 - arrow between icons
//20-kills ,                      21-24 - unit upgrades icons,          25 - unit status
//
//
//
void testmenu(void)
{
    MENUSTR *testmenu=LoadDialogBin("rez\\statdata.bin",DIALOGBIN_GAMEDIALOGS,IDFONT10);
    menuspecialtables(testmenu,tfontgamp,gamedlggrp);
    ShowMenuEdge(testmenu);
    setmenuflags(testmenu,MENUFLAGS_ALWAYSDRAW);
//    setmenuitem_VISIBLED(testmenu,0,TRUE);
//    testmenu->menu[0].dialogbin_flags|=DIALOGBIN_FLAGS_NODLGGRP;
//    changeeditboxparam(testmenu,0,"",MAXCHATSYMB);
//    setdefaultbutton(testmenu,0);
    drawmenu(testmenu,MENUFLAGS_EMPTY);
    UnloadDialogBin(testmenu);
}

//==========================================
int messagingmenu(MENUDRAW *menudraw,MENUPARAMS *menuparams)
{
    static int firstradiobutton;
    int ret,i,j,pl,messagetype,items=0;
    char allcharset[PLAYEDPLAYERS*(MAXPLAYERNAMESYMBOLS+1)+1];
    allcharset[0]=0;
    if (!menudraw->menutodraw)
    {
        menudraw->menutodraw=LoadDialogBin("rez\\msgfltr.bin",DIALOGBIN_GAMEDIALOGS|DIALOGBIN_DONOTSAVEUNDERMENU,IDFONT16);
	if (!menudraw->menutodraw)
	    return(-1);
	menuspecialtables(menudraw->menutodraw,tfontgamp,gamedlggrp);
	firstradiobutton=MenuFindFirstElem(menudraw->menutodraw,ISRADIOBUTTON);
	for (pl=0,j=0;pl<force_slots.realplayers;pl++)
	{
	    i=force_slots.playernr[pl];
	    if (i != NUMBGAMER && gameconf.pl_owner[i] == OWNER_HUMAN)
	    {
		if (j)
		    strcat(allcharset,"\n");
		j++;
    		strcat(allcharset,getplayername(i));
		items++;
	    }
	}
	addexpanditem_lists(menudraw->menutodraw,5,getexpandbox_selecteditem(menudraw->menutodraw,5),allcharset);
	setmenuitem_DISABLED(menudraw->menutodraw, 0,TRUE);
	setmenuitem_DISABLED(menudraw->menutodraw, 5,TRUE);
	if (items)
	    setmenuitem_DISABLED(menudraw->menutodraw, 4,FALSE);
	else
	    setmenuitem_DISABLED(menudraw->menutodraw, 4,TRUE);
	setradiobuttonstate(menudraw->menutodraw,
		PLAYER[NUMBGAMER].sendmessageto-MESSAGE_SENDMESSAGETOEVERYONE+firstradiobutton);
	if (PLAYER[NUMBGAMER].sendmessageto==MESSAGE_SENDMESSAGETOPLAYER)
	{
	    changeexpanditemnr(menudraw->menutodraw,5,PLAYER[NUMBGAMER].messagetoplayernr);
	}
    }
    ret=drawmenu_ONETICK(menudraw->menutodraw);
    switch(ret)
    {
	    case 0://ok
		messagetype=getradiobuttonstate(menudraw->menutodraw,firstradiobutton)-firstradiobutton;
		PLAYER[NUMBGAMER].sendmessageto=messagetype+MESSAGE_SENDMESSAGETOEVERYONE;
		if (PLAYER[NUMBGAMER].sendmessageto==MESSAGE_SENDMESSAGETOPLAYER)
		{
		    PLAYER[NUMBGAMER].messagetoplayernr=getexpandbox_selecteditem(menudraw->menutodraw,5);
		}
		return(1);
		break;
	    case CANCELFROMMENU://ESC
	    case 1://cancel
		return(1);
		break;
	    case 2://to allies
	    case 3://to everyone
    		setmenuitem_DISABLED(menudraw->menutodraw,0,FALSE);
    		setmenuitem_DISABLED(menudraw->menutodraw,5,TRUE);
		break;
	    case 4://to player
    		setmenuitem_DISABLED(menudraw->menutodraw,0,FALSE);
    		setmenuitem_DISABLED(menudraw->menutodraw,5,FALSE);
		break;
    }
    return(0);
}
//==========================================
int diplomacymenu(MENUDRAW *menudraw,MENUPARAMS *menuparams)
{
    static unsigned char allies[MAXPLAYERS],shared[MAXPLAYERS]; 
    int i,j,pl,ret;
    char tempplayername[MAXPLAYERNAMESYMBOLS+7];	//commandsymb,cubesymbol,colorcubesymbol,sizex,sizey,space and 0-terminate string at end
    if (!menudraw->menutodraw)
    {
        memset(&allies[0],0,MAXPLAYERS);
	memset(&shared[0],0,MAXPLAYERS);
	menudraw->menutodraw=LoadDialogBin("rez\\allyfltr.bin",DIALOGBIN_GAMEDIALOGS|DIALOGBIN_DONOTSAVEUNDERMENU,IDFONT10);
	if (!menudraw->menutodraw)
	    return(-1);
	menuspecialtables(menudraw->menutodraw,tfontgamp,gamedlggrp);
	for (i=0;i<PLAYEDPLAYERS;i++)
	{
    	    setmenuitem_VISIBLED(menudraw->menutodraw,2+i,FALSE);
    	    setmenuitem_DISABLED(menudraw->menutodraw,2+i,TRUE);	//checkbox1
    	    setmenuitem_VISIBLED(menudraw->menutodraw,11+i,FALSE);
    	    setmenuitem_DISABLED(menudraw->menutodraw,11+i,TRUE);	//checkbox2
    	    setmenuitem_VISIBLED(menudraw->menutodraw,25+i,FALSE);
    	    setmenuitem_DISABLED(menudraw->menutodraw,25+i,TRUE);//playercolor and name
	}
	for (pl=0,j=0;pl<force_slots.realplayers;pl++)
	{
	    i=force_slots.playernr[pl];
	    if (i != NUMBGAMER && gameconf.pl_owner[i] == OWNER_HUMAN)
	    {
		allies[i]=map.pl_allied[NUMBGAMER][i];	//i make other player to be allied with me
		shared[i]=map.pl_vision[i][NUMBGAMER];	//i make my vision to other player
		setcheckboxstate(menudraw->menutodraw,2+j,allies[i]!=0);
		setcheckboxstate(menudraw->menutodraw,11+j,shared[i]!=0);
		tempplayername[0]=COMMANDSYMB;						//commandsymb
		tempplayername[1]=CUBESYMB;						//cubesymbol
		tempplayername[2]=16;							//cubesizex
		tempplayername[3]=16;							//cubesizey
		tempplayername[4]=tableforminimap[PLAYER[i].colorRACE];			//colorcube
		tempplayername[5]=' ';
		strcpy(tempplayername+4,getplayername(i));
		changetextitem(menudraw->menutodraw,25+j,tempplayername);
    	    	
    	    	setmenuitem_VISIBLED(menudraw->menutodraw,2+j,TRUE);
    	    	setmenuitem_DISABLED(menudraw->menutodraw,2+j,FALSE);	//checkbox1
    	    	setmenuitem_VISIBLED(menudraw->menutodraw,11+j,TRUE);
    	    	setmenuitem_DISABLED(menudraw->menutodraw,11+j,FALSE);	//checkbox2
    	    	setmenuitem_VISIBLED(menudraw->menutodraw,25+j,TRUE);
    	    	setmenuitem_DISABLED(menudraw->menutodraw,25+j,FALSE);//playercolor and name
		j++;
	    }
	}
	setcheckboxstate(menudraw->menutodraw,20,gameconf.allied_victory);//allied victory check box
	Play_sfxdata_id(NULL,SFXDATA_SOUNDSELECT,-1,0);
    }
    ret=drawmenu_ONETICK(menudraw->menutodraw);
    switch(ret)
    {
	    case 0://accept
		for (pl=0,j=0;pl<force_slots.realplayers;pl++)
		{
		    i=force_slots.playernr[pl];
		    if (i != NUMBGAMER)
		    {
			if (getcheckboxstate(menudraw->menutodraw,2+j))
			    CreateAliance(&map,NUMBGAMER,i,ALLIANCEOBJ);
			else
			    CreateAliance(&map,NUMBGAMER,i,ENEMYOBJ);
			if (getcheckboxstate(menudraw->menutodraw,11+j))
			    CreateVision(&map,i,NUMBGAMER,PLAYER_VISION);
			else
			    CreateVision(&map,i,NUMBGAMER,PLAYER_NOVISION);
			j++;
		    }
		}
		gameconf.allied_victory=getcheckboxstate(menudraw->menutodraw,20);//allied victory check box
		map.clearfog[NUMBGAMER]=1;
		bitsplayer=GetVisionBitsPlayer(NUMBGAMER);
		MAPREGENERATIONBIT=1;
		MAPUNITSREGENERATIONBIT=1;
		//???? send to players changes info
		return(1);
		break;
	    case 1:
	    case CANCELFROMMENU://ESC
		return(1);
		break;
    }
    return(0);
}
//==========================================
int WaitingUpdateTime(MENUSTR *allmenus,void *data)
{
    char timesec[10];
    int w,*pwaittimeremain;
    if (menutimerupdate)
    {
	menutimerupdate=0;
	pwaittimeremain=(int *)data;
	w=*pwaittimeremain;
	if (w)
	{
	    w--;
	}
	else
	    setmenuitem_DISABLED(allmenus,0,FALSE);
	*pwaittimeremain=w;
	sprintf(timesec,"0:%02u",w);
	changetextitem(allmenus,2,timesec);
    }
    return(0);
}
//==========================================
int WaitingPlayersMenu(MENUDRAW *menudraw,MENUPARAMS *menuparams)
{
    int ret,i,j,pl;
    int bitmaskplayers=menuparams->params_int1;
    int resettimer=menuparams->params_int1>>16;
    if (NETWORKGAME&&bitmaskplayers)
    {
	if (!menudraw->menutodraw)
	{
	    for (pl=0,j=0;pl<force_slots.realplayers;pl++)
	    {
		i=force_slots.playernr[pl];
		if (i != NUMBGAMER && gameconf.pl_owner[i] == OWNER_HUMAN)
		{
		    if (((bitmaskplayers>>i)&0x1)&&!(PLAYER[i].isobserverflag&PLAYER_DISCONNECTED))
    		    {
			j++;
			continue;
		    }
		}
		bitmaskplayers&=~(1<<i);
	    }
	    if (!j)
		return(-1);
	    PAUSEGAME=1;
	    if (resettimer)
		waittimeremain=60;
	    menudraw->menutodraw=LoadDialogBin("rez\\timeout.bin",DIALOGBIN_GAMEDIALOGS|DIALOGBIN_DONOTSAVEUNDERMENU,IDFONT16);
	    if (!menudraw->menutodraw)
		return(-1);
	    menuspecialtables(menudraw->menutodraw,tfontgamp,gamedlggrp);
	    setmenuitem_DISABLED(menudraw->menutodraw,0,TRUE);
	    for (i=0;i<MAXPLAYERS;i++)
		setmenuitem_VISIBLED(menudraw->menutodraw,3+i,FALSE);
	    for (pl=0,j=0;pl<force_slots.realplayers;pl++)
	    {
		i=force_slots.playernr[pl];
		if (((bitmaskplayers>>i)&0x1)&&!(PLAYER[i].isobserverflag&PLAYER_DISCONNECTED))
		{
		    changetextitem(menudraw->menutodraw,4+j,getplayername(i));
		    setmenuitem_VISIBLED(menudraw->menutodraw,4+j,TRUE);
		    j++;
		}
	    }
    	    AddMenu_SomeCallback(menudraw->menutodraw,&WaitingUpdateTime,&waittimeremain);
	    setmenuflags(menudraw->menutodraw,MENUFLAGS_ALWAYSDRAW);
	    if (!j)
	    {
		PAUSEGAME=0;
		return(-1);
	    }
	}
	ret=drawmenu_ONETICK(menudraw->menutodraw);
	switch(ret)
	{
	    case CANCELFROMMENU://ESC
		PAUSEGAME=0;
		return(1);
		break;
	    case 0://drop players
		for (pl=0,j=0;pl<force_slots.realplayers;pl++)
		{
		    i=force_slots.playernr[pl];
		    if ((bitmaskplayers>>i)&0x1)
		    {
		        MakeDisconnect(i);
		    }
		}
		PAUSEGAME=0;
		return(1);
		break;
	}
    }
    else
	return(-1);
    return(0);
}
//==========================================
//dialog rez\Ptextbox.bin - elements=7 groupbuttons=0
//elem 0 - (190x338) - (258x18) type=0 flags=00000008 text= [TextBox]
//elem 0 - (0x2) - (258x16) type=8 flags=00000010 text= [] txo=0 tyo=0 rax=0 ray=0
//elem 1 - (0x2) - (1x16) type=11 flags=00000000 text= [Message:] txo=0 tyo=0 rax=0 ray=0
//elem 2 - (0x2) - (1x16) type=11 flags=00000000 text= [To Allies:] txo=0 tyo=0 rax=0 ray=0
//elem 3 - (0x2) - (1x16) type=11 flags=00000000 text= [To All:] txo=0 tyo=0 rax=0 ray=0
//elem 4 - (0x2) - (1x16) type=11 flags=00000000 text= [] txo=0 tyo=0 rax=0 ray=0
//elem 5 - (0x0) - (1x1) type=9 flags=00000000 text= [To %s:] txo=0 tyo=0 rax=0 ray=0
//elem 6 - (0x2) - (258x16) type=10 flags=00000000 text= [] txo=0 tyo=0 rax=0 ray=0

int chatboxmenu(MENUDRAW *menudraw,MENUPARAMS *menuparams)
{
    int messageto=0,lenx,leny;
    char txt1[200];
    if (!menudraw->menutodraw)
    {
//	DEBUGMESSCR("%s\n",CHATBOXDIALOGFILE);
	CHATBOXDIALOGFILE[CHATBOXDIALOGFILE_OFFSET]=RACE_CHAR[gameconf.pl_race[NUMBGAMER]];
//	DEBUGMESSCR("%s\n",CHATBOXDIALOGFILE);
	menudraw->menutodraw=LoadDialogBin(CHATBOXDIALOGFILE,DIALOGBIN_GAMEDIALOGS|DIALOGBIN_DONOTSAVEUNDERMENU,IDFONT10);
	if (!menudraw->menutodraw)
	    return(-1);
	menuspecialtables(menudraw->menutodraw,tfontgamp,gamedlggrp);
	setmenuflags(menudraw->menutodraw,MENUFLAGS_ALWAYSDRAW);
	setmenuitem_VISIBLED(menudraw->menutodraw,0,TRUE);
	menudraw->menutodraw->menu[0].dialogbin_flags|=DIALOGBIN_FLAGS_NODLGGRP;
	changeeditboxparam(menudraw->menutodraw,0,"",MAXCHATSYMB);
	setdefaultbutton(menudraw->menutodraw,0);
	messageto=PLAYER[NUMBGAMER].sendmessageto;
	if (NETWORKGAME)
	    sprintf(txt1,MYTBLSTR(MYINFO_TBL_MESSAGETOALL+messageto),getplayername(PLAYER[NUMBGAMER].messagetoplayernr));
	else
	    strcpy(txt1,MYTBLSTR(MYINFO_TBL_MESSAGETOSYSTEM));
	getmessagesizex(IDFONT10,txt1,strlen(txt1),&lenx,&leny);
	changetextitem(menudraw->menutodraw,5,txt1);
	setmenuitem_VISIBLED(menudraw->menutodraw,5,TRUE);
	menudraw->menutodraw->menu[5].hotdeltay+=2;
	menudraw->menutodraw->menu[5].hotxsize=lenx;
	menudraw->menutodraw->menu[5].hotysize=leny;
	menudraw->menutodraw->menu[0].hotdeltax+=lenx;
	menudraw->menutodraw->menu[0].hotxsize-=lenx;
    }
    int ret=drawmenu_ONETICK(menudraw->menutodraw);
    switch(ret)
    {
	case CANCELFROMMENU:
	    return(1);
	    break;
	case 0://enter
	    //get string entered
	    if (geteditboxtextsize(menudraw->menutodraw,0)>0)
	    {
		txt1[0]=' ';
		txt1[1]=' ';
		txt1[2]=GYELLOWCOLORFONTCHAR[0];
		txt1[3]=0;
		strcat(txt1,getplayername(NUMBGAMER));
		strcat(txt1,": " GWHITECOLORFONTCHAR);
		strcat(txt1,geteditboxtext(menudraw->menutodraw,0));
		if (NETWORKGAME)
	    	    netplay.SendGame_NetworkText(txt1);
		else
		    ShowChatMessage(txt1);
	    }
	    return(1);
	    break;
    }
    return(0);
}
//==========================================
struct WGET_DOWNLOAD
{
    char filenr;
    char *filename;
    int  wgetprocid;
    long long fsize;
};
//==========================================
#define WGET_DOWNLOAD_COMPLETE		1000
#define WGET_DOWNLOAD_FILENOTFOUND	1001
#define WGET_FILESIZE1			"mpq1.info"
#define WGET_FILESIZE2			"mpq2.info"
//==========================================
int WaitingDownload(MENUSTR *allmenus,void *data)
{
//    static int timeout=0;
    int showx;
    WGET_DOWNLOAD *mpqfile;
    FILE *f=NULL;
    long long fsize;
    char txtlabel[100];
    if (menutimerupdate)
    {
	menutimerupdate=0;
/*	if (++timeout>10*60/TIMETOMAINMENUUPDATE)
	{
	    timeout=0;
    	    return WGET_DOWNLOAD_FILENOTFOUND;
	}
*/	mpqfile=(WGET_DOWNLOAD *)data;
	if (!mpqfile->fsize)
	{
	    //need to get filesize from wget outfile
	    switch(mpqfile->filenr)
	    {
		case 1:
		    f=fopen(WGET_FILESIZE2,"r");
		    break;
		case 2:
		    f=fopen(WGET_FILESIZE1,"r");
		    break;
	    }
	    if (!f)//file is not appeared yet
		return(0);
	    fscanf(f,"%s",txtlabel);
	    mpqfile->fsize=atoll(txtlabel);
	    if (!mpqfile->fsize)
	    {
		//file not found on download server
//		timeout=0;
		return WGET_DOWNLOAD_FILENOTFOUND;
	    }
	    fscanf(f,"%s",txtlabel);
	    mpqfile->wgetprocid=atoll(txtlabel);
	    fclose(f);
	}
	fsize=FILElength(mpqfile->filename);
	if (fsize<0)
	{
	    return(0);
	}
	if (fsize<mpqfile->fsize)
	{
	    showx=fsize*allmenus->menu[3].hotxsize/mpqfile->fsize;
	    if (showx<0)
		showx=0;
	    if (showx>allmenus->menu[3].hotxsize)
		showx=allmenus->menu[3].hotxsize;
	    setimageitem_visibilityxy(allmenus,3,showx,-1);
	    sprintf(txtlabel,MYTBLSTR(MYINFO_TBL_DOWNLOADINGTEXT),mpqfile->filename,
		    fsize/1024,mpqfile->fsize/1024,fsize*100/mpqfile->fsize);
	    changetextitem(allmenus,4,txtlabel);
	}
	else
	{
//	    timeout=0;
	    return WGET_DOWNLOAD_COMPLETE;
	}
    }
    return(0);
}
//==========================================
void wgetlauchdownloadfile(int nrmpq)
{
    switch(nrmpq)
    {
	case 1://broodat.mpq
	    unlink(WGET_FILESIZE2);
	    system("./download.broodat test &");
	    break;
	case 2://stardat.mpq
	    unlink(WGET_FILESIZE1);
	    system("./download.stardat test &");
	    break;
    }
}
//==========================================
int DownloadMpqMenu(int mpqcfgresult)
{
    int exitstatus=-1,ret,mpqtotal,mpqremain,i,j;
    MENUSTR *menu2,*menu3;
    PCX backgnd,fontpcx;
    char *files[3];
    char txtlabel[100];
    char newpal[256*4];
    char *fntadr;
    WGET_DOWNLOAD mpqfile;
    files[0]="";
    files[1]="";
    files[2]="";

    mouser[MOUSEMODE1].x1=0;
    mouser[MOUSEMODE1].y1=0;
    mouser[MOUSEMODE1].x2=GRP_screensizex-1;
    mouser[MOUSEMODE1].y2=GRP_screensizey-1;
    
    backgnd.openMpqPcx(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,DLNDMENU_STR[0],DLNDMENU_STR[1],BACKGND_STR));
    backgnd.readPalFromPcx(newpal,0);//readfourbytepalette
    memcpy(GRP_vidmem,backgnd.GetPcxRawBytes(),backgnd.xsizePcx()*backgnd.ysizePcx());
//    backgnd.PutPcx(DELTASCREENX,DELTASCREENY2,PCX_EMPTYCOLOR1);
    
    fontpcx.openMpqPcx(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,MAINMENU_STR[0],MAINMENU_STR[1],FNT_STR));
    fntadr=fontpcx.GetPcxRawBytes();//for glowtext
    fntadr[21]=fntadr[11];
    fntadr[22]=fntadr[12];
    palchange(newpal,gameconf.videoconf.gamma,gameconf.videoconf.saturate);
    if (!gameconf.grmode.emulationmode)
	wscreenon();
    loadonecursor(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,MAINMENU_STR[0],MAINMENU_STR[1],ARROW_STR),NORMALMOUSE);
    mytimer.SetMyTimerFunc(&mainmenutimer,NULL);
    restrictmousecoords(MOUSEMODE1);
    installmousemoveevent(&mymousemoveevent);

    mpqtotal=mpqcfgresult>>8;
    mpqremain=mpqcfgresult&0xff;
    
    for (i=0,j=0;i<mpqtotal;i++)
	if (!mpqprio[i].presence)
	{
	    files[j++]=mpqprio[i].mpqfilename;
	}
    
    sprintf(txtlabel,MYTBLSTR(MYINFO_TBL_MISSINGMPQS),files[0],files[1]);
    ret=glu_putmenu(NULL,"rez\\gluFirstRun.bin",txtlabel,fntadr,NULL,0);
    switch(ret)
    {
	    case 1:
		if (check_ifhavewget())
		{
		    for (i=0;i<mpqtotal;i++)
			if (!mpqprio[i].presence)
			{
			    wgetlauchdownloadfile(i);
			    mpqfile.filename=mpqprio[i].mpqfilename;
			    mpqfile.filenr=i;
			    mpqfile.wgetprocid=0;
			    mpqfile.fsize=0;
			    menu2=LoadDialogBin("rez\\gluDwnlMpq.bin",DIALOGBIN_MAINDIALOGS,IDFONT16);
			    addsoundstomenu(menu2,-1,-1);
			    menuspecialtables(menu2,fntadr,NULL);
			    setmenuflags(menu2,MENUFLAGS_ALWAYSDRAW);
			    changemenuitemtype(menu2,3,ISIMAGE,IDFONT8,MTEXTCOLOR1);
			    changeimageitem(menu2,3,MAINMENU_DOWNLOADPROGRESSICON);
			    setimageitem_visibilityxy(menu2,3,0,-1);
    			    AddMenu_SomeCallback(menu2,&WaitingDownload,&mpqfile);
			    sprintf(txtlabel,MYTBLSTR(MYINFO_TBL_STARTDOWNLOAD),mpqfile.filename);
			    changetextitem(menu2,4,txtlabel);
			    ret=drawmenu(menu2,MENUFLAGS_EMPTY);
			    UnloadDialogBin(menu2);
			    switch(ret)
			    {
				case 1://ok
				    break;
				case 2://cancel
				case CANCELFROMMENU://ESC
				    //need to kill 'wget' process 
				    if (mpqfile.wgetprocid)
					kill(mpqfile.wgetprocid,SIGKILL);
				    menu3=LoadDialogBin("rez\\gluDwnlErr.bin",DIALOGBIN_MAINDIALOGS,IDFONT16);
				    addsoundstomenu(menu3,-1,-1);
				    menuspecialtables(menu3,fntadr,NULL);
				    setmenuflags(menu3,MENUFLAGS_ALWAYSDRAW);
				    changetextitem(menu3,2,MYTBLSTR(MYINFO_TBL_DOWNLOADABORTED));
				    drawmenu(menu3,MENUFLAGS_EMPTY);
				    UnloadDialogBin(menu3);
				    i=mpqtotal;
				    exitstatus=-1;
				    break;
				case WGET_DOWNLOAD_COMPLETE:
//				    printf("%s file download competed.\n",mpqfile.filename);
				    mpqremain--;
				    break;
				case WGET_DOWNLOAD_FILENOTFOUND:
				    menu3=LoadDialogBin("rez\\gluDwnlErr.bin",DIALOGBIN_MAINDIALOGS,IDFONT16);
				    addsoundstomenu(menu3,-1,-1);
				    menuspecialtables(menu3,fntadr,NULL);
				    setmenuflags(menu3,MENUFLAGS_ALWAYSDRAW);
				    sprintf(txtlabel,MYTBLSTR(MYINFO_TBL_FILENOTFOUND),mpqfile.filename);
				    changetextitem(menu3,2,txtlabel);
				    drawmenu(menu3,MENUFLAGS_EMPTY);
				    UnloadDialogBin(menu3);
				    i=mpqtotal;
				    exitstatus=-1;
				    break;
			    }
			}
			if (!mpqremain)
			    exitstatus=(mpqtotal<<8)|mpqremain;
		}
		else
		{
		    menu3=LoadDialogBin("rez\\gluDwnlErr.bin",DIALOGBIN_MAINDIALOGS,IDFONT16);
		    addsoundstomenu(menu3,-1,-1);
		    menuspecialtables(menu3,fntadr,NULL);
		    setmenuflags(menu3,MENUFLAGS_ALWAYSDRAW);
		    changetextitem(menu3,2,MYTBLSTR(MYINFO_TBL_MISSINGWGET));
		    drawmenu(menu3,MENUFLAGS_EMPTY);
		    UnloadDialogBin(menu3);
		    exitstatus=-1;
		}
		break;
	    case 3:
	    case CANCELFROMMENU://ESC
		exitstatus=-1;
		break;
    }
    uninstallmousemoveevent();
    mytimer.ClearMyTimerFunc();
    unloadmousecursors();
    fontpcx.closePcx();
    backgnd.closePcx();
    unlink(WGET_FILESIZE2);
    unlink(WGET_FILESIZE1);
    return(exitstatus);
}
//==========================================
int PauseGame(int sendornot,int playernr)
{
    if (PLAYER[playernr].pausedgame-->0)
    {
	if (sendornot)
	    netplay.SendGame_PauseResume(1);
	netplay.SetPauseResumeMessage(NETWORK_TBL_SETPAUSE,playernr,PLAYER[playernr].pausedgame);
	return(1);
    }
    return(0);
}
//==========================================
int ResumeGame(int sendornot,int playernr)
{
    if (sendornot)
	netplay.SendGame_PauseResume(2);
    netplay.SetPauseResumeMessage(NETWORK_TBL_SETRESUME,playernr,PLAYER[playernr].pausedgame);
    return(1);
}
//==========================================
int putgamemenu(MENUDRAW *menudraw,MENUPARAMS *menuparams)
{
    if (!menudraw->menutodraw)
    {
	menudraw->menutodraw=LoadDialogBin("rez\\gamemenu.bin",DIALOGBIN_GAMEDIALOGS|DIALOGBIN_DONOTSAVEUNDERMENU,IDFONT16);
	if (!menudraw->menutodraw)
	    return(-1);
	menuspecialtables(menudraw->menutodraw,tfontgamp,gamedlggrp);
	if (NETWORKGAME)
	{
	    setmenuitem_VISIBLED(menudraw->menutodraw,2,FALSE);	//loadgame
	    if (PAUSEGAME)
	    {
		setmenuitem_VISIBLED(menudraw->menutodraw,3,FALSE);	//pausegame
		setmenuitem_VISIBLED(menudraw->menutodraw,4,TRUE);	//resumegame
	    }
	    else
	    {
		setmenuitem_VISIBLED(menudraw->menutodraw,3,TRUE);	//pausegame
		setmenuitem_VISIBLED(menudraw->menutodraw,4,FALSE);	//resumegame
	    }
	}
	else
	{
	    PAUSEGAME = 1;
	    setmenuitem_VISIBLED(menudraw->menutodraw,2,TRUE);	//loadgame
	    setmenuitem_VISIBLED(menudraw->menutodraw,3,FALSE);	//pausegame
	    setmenuitem_VISIBLED(menudraw->menutodraw,4,FALSE);	//resumeame
	}
	Play_sfxdata_id(NULL,SFXDATA_SOUNDSELECT,-1,0);
    }
    if (PLAYER[NUMBGAMER].pausedgame==0)
        setmenuitem_DISABLED(menudraw->menutodraw,3,TRUE);
    else
        setmenuitem_DISABLED(menudraw->menutodraw,3,FALSE);
    int ret=drawmenu_ONETICK(menudraw->menutodraw);
    switch(ret)
    {
	    case CANCELFROMMENU://ESC
	    case 0:
		if (!NETWORKGAME)
	    	    PAUSEGAME = 0;
		return(1);
		break;
	    case 1:
		showedmenu.prepareforshowmenu(&savegame,NULL);
		break;
	    case 2:
		showedmenu.prepareforshowmenu(&loadgame,NULL);
		break;
	    case 3://pause
		PAUSEGAME=1;
		if (NETWORKGAME)
		{
		    if (PauseGame(1,NUMBGAMER))
		    {
			setmenuitem_VISIBLED(menudraw->menutodraw,3,FALSE);	//pausegame
			setmenuitem_VISIBLED(menudraw->menutodraw,4,TRUE);	//resumegame
			mymousemoveevent(mouse_x,mouse_y);
		    }
		}
		break;
	    case 4://resume
		PAUSEGAME=0;
		if (NETWORKGAME)
		{
		    if (ResumeGame(1,NUMBGAMER))
		    {
			setmenuitem_VISIBLED(menudraw->menutodraw,3,TRUE);	//pausegame
			setmenuitem_VISIBLED(menudraw->menutodraw,4,FALSE);	//resumegame
			mymousemoveevent(mouse_x,mouse_y);
		    }
		}
		break;
    	    case 5:
		showedmenu.prepareforshowmenu(&optionsgame,NULL);
		break;
	    case 6:
		showedmenu.prepareforshowmenu(&puthelpfromgame,NULL);
		break;
	    case 7:
		showedmenu.prepareforshowmenu(&missionobjectives,NULL);
		break;
	    case 8:
		showedmenu.prepareforshowmenu(&abrtmenu,NULL);
		break;
    }
    return(0);
}
//==========================================
int missionobjectives(MENUDRAW *menudraw,MENUPARAMS *menuparams)
{
    int ret;
    if (!menudraw->menutodraw)
    {
        menudraw->menutodraw=LoadDialogBin("rez\\objctdlg.bin",DIALOGBIN_GAMEDIALOGS|DIALOGBIN_DONOTSAVEUNDERMENU,IDFONT16);
	if (!menudraw->menutodraw)
	    return(-1);
	menuspecialtables(menudraw->menutodraw,tfontgamp,gamedlggrp);
	changetextitem(menudraw->menutodraw,1,missionobj);
    }
    ret=drawmenu_ONETICK(menudraw->menutodraw);
    switch(ret)
    {
	    case CANCELFROMMENU://ESC
	    case 0:
		return(1);
		break;
    }
    return(0);
}
//==========================================
int wmission(MENUDRAW *menudraw,MENUPARAMS *menuparams)
{
    int ret,i;
    if (!menudraw->menutodraw)
    {
	menudraw->CloseMultiplesMenus(ALLMENUS.GetMaxElements());
        menudraw->menutodraw=LoadDialogBin("rez\\wmission.bin",DIALOGBIN_GAMEDIALOGS|DIALOGBIN_DONOTSAVEUNDERMENU,IDFONT16);
	if (!menudraw->menutodraw)
	    return(-1);
	if (!NETWORKGAME)
	    setmenuitem_VISIBLED(menudraw->menutodraw, 2,FALSE);
	menuspecialtables(menudraw->menutodraw,tfontgamp,gamedlggrp);
	Play_sfxdata_id(NULL,SFXDATA_SOUNDSELECT,-1,0);
    }
    map.flags |= STARMAP_FLAG_WINGAME;
    ret=drawmenu_ONETICK(menudraw->menutodraw);
    switch(ret)
    {
	    case 0:
		menustatus=QUITGAME;
		return(1);
		break;
	    case CANCELFROMMENU://ESC
	    case 2:
		for (i=0;i<MAXPLAYERS;i++)
		    if (player_aliance(NUMBGAMER,i)==ENEMYOBJ)
			MakeObserver(i);
		gamestatus=NOGAMESTATUS;	//so we can continue;
		PAUSEGAME=0;
		return(1);
		break;
    }
    return(0);
}
//==========================================
int lmission(MENUDRAW *menudraw,MENUPARAMS *menuparams)
{
    int ret;
    if (!menudraw->menutodraw)
    {
	menudraw->CloseMultiplesMenus(ALLMENUS.GetMaxElements());
        menudraw->menutodraw=LoadDialogBin("rez\\lmission.bin",DIALOGBIN_GAMEDIALOGS|DIALOGBIN_DONOTSAVEUNDERMENU,IDFONT16);
	if (!menudraw->menutodraw)
	    return(-1);
	menuspecialtables(menudraw->menutodraw,tfontgamp,gamedlggrp);
	if (!NETWORKGAME)
	    setmenuitem_VISIBLED(menudraw->menutodraw, 1,FALSE);
	Play_sfxdata_id(NULL,SFXDATA_SOUNDSELECT,-1,0);
    }
    ret=drawmenu_ONETICK(menudraw->menutodraw);
    switch(ret)
    {
	    case 0:
	    case CANCELFROMMENU://ESC
		menustatus=QUITGAME;
		return(1);
		break;
	    case 1:
		gamestatus=NOGAMESTATUS;	//so we can continue;
		PAUSEGAME=0;
		MakeObserver(NUMBGAMER);
		return(1);
		break;
    }
    return(0);
}
//==========================================
int abrtmenu(MENUDRAW *menudraw,MENUPARAMS *menuparams)
{
    int ret,exitmenu;
    if (!menudraw->menutodraw)
    {
        menudraw->menutodraw=LoadDialogBin("rez\\abrtmenu.bin",DIALOGBIN_GAMEDIALOGS|DIALOGBIN_DONOTSAVEUNDERMENU,IDFONT16);
	if (!menudraw->menutodraw)
	    return(-1);
	menuspecialtables(menudraw->menutodraw,tfontgamp,gamedlggrp);
    }	    
    ret=drawmenu_ONETICK(menudraw->menutodraw);
    switch(ret)
    {
	    case CANCELFROMMENU://ESC
	    case 0:
		menustatus=CONTINUEGAME;
		return(1);
	    case 1:
		showedmenu.prepareforshowmenu(&restartgame,NULL);
		break;
	    case 2:
		showedmenu.prepareforshowmenu(&leavegame,NULL);
		break;
	    case 3:
		showedmenu.prepareforshowmenu(&quitgame,NULL);
		break;
    }
    return(0);
}
//==========================================
int quitgame(MENUDRAW *menudraw,MENUPARAMS *menuparams)
{
    int ret;
    if (!menudraw->menutodraw)
    {
        menudraw->menutodraw=LoadDialogBin("rez\\quit.bin",DIALOGBIN_GAMEDIALOGS|DIALOGBIN_DONOTSAVEUNDERMENU,IDFONT16);
	if (!menudraw->menutodraw)
	    return(-1);
	menuspecialtables(menudraw->menutodraw,tfontgamp,gamedlggrp);
    }	    
    ret=drawmenu_ONETICK(menudraw->menutodraw);
    switch(ret)
    {
	    case 0:
		menudraw->CloseMultiplesMenus(ALLMENUS.GetMaxElements());
		menustatus=EXITGAME;
		return(1);
		break;
	    case 1:
	    case CANCELFROMMENU://ESC
		menustatus=CONTINUEGAME;
		return(1);
		break;
    }
    return(0);
}
//==========================================
int restartgame(MENUDRAW *menudraw,MENUPARAMS *menuparams)
{
    int ret;
    if (!menudraw->menutodraw)
    {
        menudraw->menutodraw=LoadDialogBin("rez\\restart.bin",DIALOGBIN_GAMEDIALOGS|DIALOGBIN_DONOTSAVEUNDERMENU,IDFONT16);
	if (!menudraw->menutodraw)
	    return(-1);
	menuspecialtables(menudraw->menutodraw,tfontgamp,gamedlggrp);
    }	    
    ret=drawmenu_ONETICK(menudraw->menutodraw);
    switch(ret)
    {
	    case 0://ok
		menudraw->CloseMultiplesMenus(ALLMENUS.GetMaxElements());
	        menustatus=RESTARTGAME;
		return(1);
		break;
	    case CANCELFROMMENU://ESC
	    case 1://cancel
		menustatus=CONTINUEGAME;
		return(1);
		break;
    }
    return(0);
}
//==========================================
int leavegame(MENUDRAW *menudraw,MENUPARAMS *menuparams)
{
    int ret;
    if (!menudraw->menutodraw)
    {
        menudraw->menutodraw=LoadDialogBin("rez\\quit2mnu.bin",DIALOGBIN_GAMEDIALOGS|DIALOGBIN_DONOTSAVEUNDERMENU,IDFONT16);
	if (!menudraw->menutodraw)
	    return(-1);
	menuspecialtables(menudraw->menutodraw,tfontgamp,gamedlggrp);
	//if no network or you are observer or you win the game but continue to play - not show 'become observer menu' 
	if (!NETWORKGAME||(PLAYER[NUMBGAMER].isobserverflag)|| (map.flags & STARMAP_FLAG_WINGAME))
    	    setmenuitem_VISIBLED(menudraw->menutodraw,1,FALSE);
	else
    	    setmenuitem_VISIBLED(menudraw->menutodraw,1,TRUE);
    }	    
    ret=drawmenu_ONETICK(menudraw->menutodraw);
    switch(ret)
    {
	    case 0://ok
		menudraw->CloseMultiplesMenus(ALLMENUS.GetMaxElements());
		menustatus=QUITMISSION;
		return(1);
		break;
	    case 1://become observer
		menustatus=BECOMEOBSERVER;
		return(1);
		break;
	    case CANCELFROMMENU://ESC
	    case 2://cancel
		menustatus=CONTINUEGAME;
		return(1);
		break;
    }
    return(0);
}
//==========================================
int puthelpfromgame(MENUDRAW *menudraw,MENUPARAMS *menuparams)
{
    int ret;
    if (!menudraw->menutodraw)
    {
        menudraw->menutodraw=LoadDialogBin("rez\\help.bin",DIALOGBIN_GAMEDIALOGS|DIALOGBIN_DONOTSAVEUNDERMENU,IDFONT16);
	if (!menudraw->menutodraw)
	    return(-1);
	menuspecialtables(menudraw->menutodraw,tfontgamp,gamedlggrp);
	changetextitem(menudraw->menutodraw,1,myhelpmenutext[0]);
    }	    
    ret=drawmenu_ONETICK(menudraw->menutodraw);
    if (ret>=0)
    {
	menustatus=CONTINUEGAME;
	return(1);
    }
    return(0);
}
//==========================================
int optionsgame(MENUDRAW *menudraw,MENUPARAMS *menuparams)
{
    int ret;
    if (!menudraw->menutodraw)
    {
        menudraw->menutodraw=LoadDialogBin("rez\\options.bin",DIALOGBIN_GAMEDIALOGS|DIALOGBIN_DONOTSAVEUNDERMENU,IDFONT16);
	if (!menudraw->menutodraw)
	    return(-1);
	menuspecialtables(menudraw->menutodraw,tfontgamp,gamedlggrp);
	if (!NETWORKGAME)
	    setmenuitem_VISIBLED(menudraw->menutodraw,4,FALSE);
    }	    
    ret=drawmenu_ONETICK(menudraw->menutodraw);
    switch(ret)
    {
	    case 1:
		showedmenu.prepareforshowmenu(&speedopt,NULL);
		break;
	    case 2:
		showedmenu.prepareforshowmenu(&audioopt,NULL);
		break;
	    case 3:
		showedmenu.prepareforshowmenu(&videoopt,NULL);
		break;
	    case 4:
		showedmenu.prepareforshowmenu(&networkopt,NULL);
		break;
	    case 0:
	    case CANCELFROMMENU://ESC
		menustatus=CONTINUEGAME;
		return(1);
		break;
    }
    return(0);
}
//==========================================
int networkopt(MENUDRAW *menudraw,MENUPARAMS *menuparams)
{
    static int firstradiobutton;
    int ret,networklatencybutton;
    if (!menudraw->menutodraw)
    {
        menudraw->menutodraw=LoadDialogBin("rez\\netdlg.bin",DIALOGBIN_GAMEDIALOGS|DIALOGBIN_DONOTSAVEUNDERMENU,IDFONT16);
	if (!menudraw->menutodraw)
	    return(-1);
	menuspecialtables(menudraw->menutodraw,tfontgamp,gamedlggrp);
	firstradiobutton=MenuFindFirstElem(menudraw->menutodraw,ISRADIOBUTTON);
	setradiobuttonstate(menudraw->menutodraw,gameconf.networkconf.networklatency+firstradiobutton);
    }	    
    ret=drawmenu_ONETICK(menudraw->menutodraw);
    switch(ret)
    {
	    case CANCELFROMMENU://ESC
	    case 1://cancel
		menustatus=CONTINUEGAME;
		return(1);
		break;
	    case 0://ok
		networklatencybutton=getradiobuttonstate(menudraw->menutodraw,firstradiobutton)-firstradiobutton;
		if (gameconf.networkconf.networklatency!=networklatencybutton)
		{
		    gameconf.networkconf.networklatency=networklatencybutton;
		    netplay.SendGame_NetworkSpeed(gameconf.networkconf.networklatency);
		    saveconf();
		    netplay.SetNetworkLatency(gameconf.networkconf.networklatency,NUMBGAMER);
		}
		menudraw->CloseMultiplesMenus(ALLMENUS.GetMaxElements());
		menustatus=CONTINUEGAME;
		return(1);
		break;
    }
    return(0);
}
//==========================================
int speedopt(MENUDRAW *menudraw,MENUPARAMS *menuparams)
{
    int ret;
    if (!menudraw->menutodraw)
    {
        menudraw->menutodraw=LoadDialogBin("rez\\spd_dlg.bin",DIALOGBIN_GAMEDIALOGS|DIALOGBIN_DONOTSAVEUNDERMENU,IDFONT16);
	if (!menudraw->menutodraw)
	    return(-1);
	menuspecialtables(menudraw->menutodraw,tfontgamp,gamedlggrp);
        addhorizbutton_params(menudraw->menutodraw,2,7,7,3,NULL);
        sethorizbuttonpos(menudraw->menutodraw,2,gameconf.speedconf.gamespeed);
        if (NETWORKGAME)
        {
    	    setmenuitem_DISABLED(menudraw->menutodraw, 2,TRUE);
	    setmenuitem_DISABLED(menudraw->menutodraw, 6,TRUE);
    	    setmenuitem_DISABLED(menudraw->menutodraw, 9,TRUE);
    	    setmenuitem_DISABLED(menudraw->menutodraw,12,TRUE);
	}
        addhorizbutton_params(menudraw->menutodraw,3,7,7,3,NULL);
        sethorizbuttonpos(menudraw->menutodraw,3,gameconf.speedconf.mousescroll);
        setmenuitem_DISABLED(menudraw->menutodraw, 3,TRUE);
        setmenuitem_DISABLED(menudraw->menutodraw, 6,TRUE);
        setmenuitem_DISABLED(menudraw->menutodraw, 9,TRUE);
        setmenuitem_DISABLED(menudraw->menutodraw,12,TRUE);
    
        addhorizbutton_params(menudraw->menutodraw,4,7,7,3,NULL);
        sethorizbuttonpos(menudraw->menutodraw,4,gameconf.speedconf.keyscroll);
        setmenuitem_DISABLED(menudraw->menutodraw, 4,TRUE);
        setmenuitem_DISABLED(menudraw->menutodraw, 7,TRUE);
        setmenuitem_DISABLED(menudraw->menutodraw,10,TRUE);
        setmenuitem_DISABLED(menudraw->menutodraw,13,TRUE);
        setcheckboxstate(menudraw->menutodraw,15,gameconf.speedconf.cputhrottling);
    }	    
    ret=drawmenu_ONETICK(menudraw->menutodraw);
    switch(ret)
    {
	    case 0:		//ok
		if (!NETWORKGAME)
		    gameconf.speedconf.gamespeed=gethorizvalue(menudraw->menutodraw,2);
		gameconf.speedconf.mousescroll=gethorizvalue(menudraw->menutodraw,3);
		gameconf.speedconf.keyscroll=gethorizvalue(menudraw->menutodraw,4);
		gameconf.speedconf.cputhrottling=getcheckboxstate(menudraw->menutodraw,15);
		saveconf();
		menustatus=CONTINUEGAME;
		return(1);
		break;
	    case CANCELFROMMENU://ESC
	    case 1:
		menustatus=CONTINUEGAME;
		return(1);
		break;
    }
    return(0);
}
//==========================================
int audioopt(MENUDRAW *menudraw,MENUPARAMS *menuparams)
{
    int ret;
    if (!menudraw->menutodraw)
    {
        menudraw->menutodraw=LoadDialogBin("rez\\snd_dlg.bin",DIALOGBIN_GAMEDIALOGS|DIALOGBIN_DONOTSAVEUNDERMENU,IDFONT16);
	if (!menudraw->menutodraw)
	    return(-1);
	menuspecialtables(menudraw->menutodraw,tfontgamp,gamedlggrp);
	setcheckboxstate(menudraw->menutodraw,4,gameconf.audioconf.unitspeach);
	setcheckboxstate(menudraw->menutodraw,5,gameconf.audioconf.advisorsounds);
        setcheckboxstate(menudraw->menutodraw,6,gameconf.audioconf.buildsounds);
        setcheckboxstate(menudraw->menutodraw,7,gameconf.audioconf.subtitles);
        addhorizbutton_params(menudraw->menutodraw,2,100,7,3,NULL);
        sethorizbuttonpos(menudraw->menutodraw,2,gameconf.audioconf.musicvolume);
        addhorizbutton_params(menudraw->menutodraw,3,100,7,3,NULL);
        sethorizbuttonpos(menudraw->menutodraw,3,gameconf.audioconf.soundvolume);
	if (gameconf.audioconf.audioflags&AUDIOFLAGS_MUSICDISABLE)
	{
	    setmenuitem_DISABLED(menudraw->menutodraw, 2,TRUE);//music volume horizbar
	    setmenuitem_DISABLED(menudraw->menutodraw, 8,TRUE);//music volume
	    setmenuitem_DISABLED(menudraw->menutodraw,10,TRUE);;//music volume min
	    setmenuitem_DISABLED(menudraw->menutodraw,12,TRUE);;//music volume max
	}
	if (gameconf.audioconf.audioflags&AUDIOFLAGS_SOUNDDISABLE)
	{
	    setmenuitem_DISABLED(menudraw->menutodraw, 3,TRUE);//sound volume horizbar
	    setmenuitem_DISABLED(menudraw->menutodraw, 9,TRUE);//sound volume
	    setmenuitem_DISABLED(menudraw->menutodraw,11,TRUE);;//sound volume min
	    setmenuitem_DISABLED(menudraw->menutodraw,13,TRUE);;//sound volume max
	}
    }	    
    ret=drawmenu_ONETICK(menudraw->menutodraw);
    switch(ret)
    {
	    case 0:		//ok
		gameconf.audioconf.musicvolume=gethorizvalue(menudraw->menutodraw,2);
		gameconf.audioconf.soundvolume=gethorizvalue(menudraw->menutodraw,3);
		volumeallsfx(gameconf.audioconf.soundvolume);
		gameconf.audioconf.unitspeach=getcheckboxstate(menudraw->menutodraw,4);
		gameconf.audioconf.advisorsounds=getcheckboxstate(menudraw->menutodraw,5);
		gameconf.audioconf.buildsounds=getcheckboxstate(menudraw->menutodraw,6);
		gameconf.audioconf.subtitles=getcheckboxstate(menudraw->menutodraw,7);
		if (gameconf.audioconf.musicvolume<5)
		    StopMusic(MUSIC_STOPWITHFADE);//stop play
		else    
		    GoPlayNextMusic();//play if not played,or not play if already playing
		if (audiostream)
		    audiostream->ChangeMusicVolume(gameconf.audioconf.musicvolume);
		saveconf();
		menustatus=CONTINUEGAME;
		return(1);
		break;
	    case CANCELFROMMENU://ESC
	    case 1:
		menustatus=CONTINUEGAME;
		return(1);
		break;
    }
    return(0);
}
//==========================================
void palchange(const char *pal,int gamma,int saturate)
{
    char newpal[256*4];
    memcpy(newpal,pal,256*4);
    palettegamma(newpal,pal,gamma-MAXGAMMA/2);
    palettemono(newpal,newpal,saturate);
    activatepalette(newpal);
//    DEBUGMESSCR("gamma=%d saturate=%d\n",gamma,saturate);
}
//==========================================
void gammachange(MENUSTR *allmenus,int itemnr,int value)
{
    gameconf.videoconf.gamma=gethorizvalue(allmenus,4);
    gameconf.videoconf.saturate=gethorizvalue(allmenus,12);
    palchange(map.palette,gameconf.videoconf.gamma,gameconf.videoconf.saturate);
}
//==========================================
int videoopt(MENUDRAW *menudraw,MENUPARAMS *menuparams)
{
    int ret;
    static GAMECONF tempconf;
    if (!menudraw->menutodraw)
    {
	memcpy(&tempconf,&gameconf,sizeof(tempconf));
        menudraw->menutodraw=LoadDialogBin("rez\\video.bin",DIALOGBIN_GAMEDIALOGS|DIALOGBIN_DONOTSAVEUNDERMENU,IDFONT16);
	if (!menudraw->menutodraw)
	    return(-1);
	menuspecialtables(menudraw->menutodraw,tfontgamp,gamedlggrp);
	setcheckboxstate(menudraw->menutodraw,7,tempconf.videoconf.animation);
	setradiobuttonstate(menudraw->menutodraw,tempconf.videoconf.portraits+8);

	setcheckboxstate(menudraw->menutodraw,15,tempconf.grmode.fullscreen);
	setcheckboxstate(menudraw->menutodraw,16,tempconf.videoconf.visiblemap);

	addhorizbutton_params(menudraw->menutodraw,4,MAXGAMMA,8,3,&gammachange);
	sethorizbuttonpos( menudraw->menutodraw,4,tempconf.videoconf.gamma);

	addhorizbutton_params(menudraw->menutodraw,12,MAXGAMMA,8,3,&gammachange);
	sethorizbuttonpos( menudraw->menutodraw,12,tempconf.videoconf.saturate);
    }	    
    ret=drawmenu_ONETICK(menudraw->menutodraw);
    switch(ret)
    {
	    case 1:		//ok
		gameconf.videoconf.animation=getcheckboxstate(menudraw->menutodraw,7);
		gameconf.videoconf.portraits=getradiobuttonstate(menudraw->menutodraw,8)-8;//get radio button item id selected
		gameconf.grmode.fullscreen=getcheckboxstate(menudraw->menutodraw,15);
		gameconf.videoconf.visiblemap=getcheckboxstate(menudraw->menutodraw,16);
		if (tempconf.grmode.fullscreen!=gameconf.grmode.fullscreen)
		{
		    gameconf.grmode.emulationmode=changemode(gameconf.grmode.x,gameconf.grmode.y,gameconf.grmode.s,gameconf.grmode.fullscreen,map.palette)-1;
		}
		saveconf();
		menustatus=CONTINUEGAME;
		return(1);
		break;
	    case CANCELFROMMENU://ESC
	    case 2:
		memcpy(&gameconf,&tempconf,sizeof(tempconf));
		palchange(map.palette,gameconf.videoconf.gamma,gameconf.videoconf.saturate);
		menustatus=CONTINUEGAME;
		return(1);
		break;
    }
    return(0);
}
//==========================================
static char fileforsaveload[512];
int loadgame(MENUDRAW *menudraw,MENUPARAMS *menuparams)
{
    int ret,elemnr;
    static mylist loadlist;
    static char LOADPATH[512];
    if (!menudraw->menutodraw)
    {
        menudraw->menutodraw=LoadDialogBin("rez\\loadgame.bin",DIALOGBIN_GAMEDIALOGS|DIALOGBIN_DONOTSAVEUNDERMENU,IDFONT16);
	if (!menudraw->menutodraw)
	    return(-1);
	listboxlineitems(menudraw->menutodraw,2,8,19);
	menuspecialtables(menudraw->menutodraw,tfontgamp,gamedlggrp);
	setitemrelation(menudraw->menutodraw,0,ITEMRELATION_DISABLE,&menudraw->menutodraw->menu[2].item.listbox->selectednr,-1);
	
	strcpy(LOADPATH,SAVES_DIRECTORY);
	mkdir(LOADPATH,0755);
	strcat(LOADPATH,nickname);
        strcat(LOADPATH,"/");
	mkdir(LOADPATH,0755);
        listfiles(LOADPATH,&loadlist,".sav",LOADPATH,".sav",NOADDDIRTOLIST);
        setlistbox_lists(menudraw->menutodraw,2,-1,&loadlist,NULL);
    }	    
    ret=drawmenu_ONETICK(menudraw->menutodraw);
    switch(ret)
    {
	case 0:
	case 2://listbox
		elemnr=menudraw->menutodraw->menu[2].item.listbox->selectednr;
		if (elemnr>=0)
		{
		    strcpy(fileforsaveload,LOADPATH);
		    strcat(fileforsaveload,(char *)menudraw->menutodraw->menu[2].item.listbox->flist->GetElemNr(elemnr));
		    if (strlen(fileforsaveload)!=0)
		    {
			strcat(fileforsaveload,".sav");
			loadgamefromfile();
			menustatus=CONTINUEGAME;
			return(0);
		    }
		}
//		loadlist.DeallocList();
//		return(1);
		break;
	case CANCELFROMMENU://ESC
	case 1://cancel
		menustatus=CONTINUEGAME;
		return(1);
		break;
    }
    return(0);
}
//==========================================
int loadgamefromfile_error1(MENUSTR *allmenus,int menuitem)
{
    menustatus=CONTINUEGAME;
}
//==========================================
void loadgamefromfile(void)
{
    MENUPARAMS *mp;
    FILE *f=fopen(fileforsaveload,"r");
//    if (f)
    if (0)
    {
	//some code of loading game.......
	fclose(f);
	menustatus=MAINMENUSTATUS_LOADGAMEOK;
    }
    else
    {
	menustatus=MAINMENUSTATUS_LOADGAMEFAILED;
	mp=new MENUPARAMS(&loadgamefromfile_error1,MYTBLSTR(MYINFO_TBL_LOADGAMEERR1));
	showedmenu.prepareforshowmenu(&xputokmenu,mp);
    }
}
//==========================================
int savegame_ondeleteaction(MENUSTR *allmenus,int menuitem)
{
    switch(menuitem)
    {
	case 0://confirm delete
	    unlink(fileforsaveload);//???? if error delete savegame
	    break;
	default:
	    break;//cancel delete
    }
    return(1);
}
//==========================================
int savegame(MENUDRAW *menudraw,MENUPARAMS *menuparams)
{
    int ret,elemnr;
    static mylist savelist;
    static char SAVEPATH[512];
    if (!menudraw->menutodraw)
    {
        menudraw->menutodraw=LoadDialogBin("rez\\savegame.bin",DIALOGBIN_GAMEDIALOGS|DIALOGBIN_DONOTSAVEUNDERMENU,IDFONT16);
	if (!menudraw->menutodraw)
	    return(-1);
	listboxlineitems(menudraw->menutodraw,4,7,19);
	menuspecialtables(menudraw->menutodraw,tfontgamp,gamedlggrp);
	setdefaultbutton(menudraw->menutodraw,0);
	changeeditboxparam(menudraw->menutodraw,0,"",30);
	setitemrelation(menudraw->menutodraw,1,ITEMRELATION_DISABLE,&menudraw->menutodraw->menu[0].item.editbox->length,0);
	setitemrelation(menudraw->menutodraw,2,ITEMRELATION_DISABLE,&menudraw->menutodraw->menu[4].item.listbox->selectednr,-1);

	strcpy(SAVEPATH,SAVES_DIRECTORY);
	mkdir(SAVEPATH,0755);
	strcat(SAVEPATH,nickname);
	strcat(SAVEPATH,"/");
	mkdir(SAVEPATH,0755);
        listfiles(SAVEPATH,&savelist,".sav",SAVEPATH,".sav",NOADDDIRTOLIST);
        setlistbox_lists(menudraw->menutodraw,4,-1,&savelist,NULL);
    }	    
    ret=drawmenu_ONETICK(menudraw->menutodraw);
    switch(ret)
    {
	case 0:	//enter or edit savename
	case 1:	//enter or edit savename
		if (geteditboxtextsize(menudraw->menutodraw,0)!=0)
		{
		    strcpy(fileforsaveload,SAVEPATH);
		    strcat(fileforsaveload,geteditboxtext(menudraw->menutodraw,0));
		    strcat(fileforsaveload,".sav");
		    if (savegametofile())
		    {
			menudraw->CloseMultiplesMenus(ALLMENUS.GetMaxElements()+1);
			menustatus=CONTINUEGAME;
			return(0);
		    }
		    else
		    {
			menustatus=CONTINUEGAME;
			return(0);
		    }
		}
		break;
	case 2://delete
		elemnr=menudraw->menutodraw->menu[4].item.listbox->selectednr;
		if (elemnr>=0)
		{
		    strcpy(fileforsaveload,SAVEPATH);
		    strcat(fileforsaveload,(char *)menudraw->menutodraw->menu[4].item.listbox->flist->GetElemNr(elemnr));
		    strcat(fileforsaveload,".sav");
		    sprintf(SAVELOADFILENAME,NETWSTR(NETWORK_TBL_DELSAVE),fileforsaveload);
		    MENUPARAMS *mp=new MENUPARAMS(&savegame_ondeleteaction,SAVELOADFILENAME);
		    showedmenu.prepareforshowmenu(&xputokcancelmenu,mp);
		    return(0);
		}
		menustatus=CONTINUEGAME;
		return(1);
	case CANCELFROMMENU://ESC
	case 3://cancel
		menustatus=CONTINUEGAME;
		return(1);
	case 4://listbox
		elemnr=menudraw->menutodraw->menu[4].item.listbox->selectednr;
		if (elemnr>=0)
		{
		    strcpy(fileforsaveload,(char *)menudraw->menutodraw->menu[4].item.listbox->flist->GetElemNr(elemnr));
		    changeeditboxtext(menudraw->menutodraw,0,fileforsaveload);
		}
		break;
    }
    return(0);
}
//==========================================
int savegametofile_replace(MENUSTR *allmenus,int menuitem)
{
    switch(menuitem)
    {
	case 0:
	    return(savegametofile_do());
	default:
	    return(1);
    }
}
//==========================================
int savegametofile_err1(MENUSTR *allmenus,int menuitem)
{
    menustatus=CONTINUEGAME;
}
//==========================================
void savegametofile_err2(void)
{
    MENUPARAMS *mp;
    menustatus=MAINMENUSTATUS_SAVEGAMEFAILED;
    sprintf(SAVELOADFILENAME,NETWSTR(NETWORK_TBL_ERRSAVE),fileforsaveload);
    mp=new MENUPARAMS(&savegametofile_err1,SAVELOADFILENAME);
//    mp=new MENUPARAMS(&savegametofile_err1,NETWSTR(NETWORK_TBL_ERRSAVE));
    showedmenu.prepareforshowmenu(&xputokmenu,mp);
}
//==========================================
int savegametofile_do(void)
{
    FILE *f;
    f=fopen(fileforsaveload,"w");
    if (f)
    {
        fwrite(fileforsaveload,1,strlen(fileforsaveload),f);
        fclose(f);
        return(1);
    }
    savegametofile_err2();
    return(0);
}
//==========================================
int savegametofile(void)
{
    MENUPARAMS *mp;
    FILE *f=fopen(fileforsaveload,"r");
    if (f)
    {
	fclose(f);
	menustatus=MAINMENUSTATUS_SAVEGAMEFAILED;
	sprintf(SAVELOADFILENAME,NETWSTR(NETWORK_TBL_REPLACESAVE),fileforsaveload);
	mp=new MENUPARAMS(&savegametofile_replace,SAVELOADFILENAME);
	
//	mp=new MENUPARAMS(&savegametofile_replace,NETWSTR(NETWORK_TBL_REPLACESAVE));
	showedmenu.prepareforshowmenu(&xputokcancelmenu,mp);
	return(0);
    }
    else
    {
	if (savegametofile_do())
	{
	    return(1);
	}
	else
	{
	    savegametofile_err2();
	    return(0);
	}
    }
}
//==========================================
int xputokmenu(MENUDRAW *menudraw,MENUPARAMS *menuparams)
{
    int ret;
    if (!menudraw->menutodraw)
    {
        menudraw->menutodraw=LoadDialogBin("rez\\ok.bin",DIALOGBIN_GAMEDIALOGS|DIALOGBIN_DONOTSAVEUNDERMENU,IDFONT16);
	if (!menudraw->menutodraw)
	    return(-1);
	menuspecialtables(menudraw->menutodraw,tfontgamp,gamedlggrp);
	changetextitem(menudraw->menutodraw,1,menuparams->params_pchar1);
    }
    ret=drawmenu_ONETICK(menudraw->menutodraw);
    if (ret>=0)
    {
	if (menuparams->onselectmenuitem)
	    if (menuparams->onselectmenuitem(menudraw->menutodraw,ret))
	    {
		menudraw->CloseMultiplesMenus(ALLMENUS.GetMaxElements()+1);
	    }
	return(0);//return 0 after closemultiplesmenus
    }
    return(0);
}
//==========================================
int xputokcancelmenu(MENUDRAW *menudraw,MENUPARAMS *menuparams)
{
    int ret;
    if (!menudraw->menutodraw)
    {
        menudraw->menutodraw=LoadDialogBin("rez\\okcancel.bin",DIALOGBIN_GAMEDIALOGS|DIALOGBIN_DONOTSAVEUNDERMENU,IDFONT16);
	if (!menudraw->menutodraw)
	    return(-1);
	menuspecialtables(menudraw->menutodraw,tfontgamp,gamedlggrp);
	changetextitem(menudraw->menutodraw,2,menuparams->params_pchar1);
    }
    ret=drawmenu_ONETICK(menudraw->menutodraw);
    if (ret>=0)
    {
	if (menuparams->onselectmenuitem)
	{
	    if (menuparams->onselectmenuitem(menudraw->menutodraw,ret))
	    {
		menudraw->CloseMultiplesMenus(ALLMENUS.GetMaxElements()+1);
	    }
	    return(0);//return 0 after closemultiplesmenus
	}
    }
    return(0);
}
//==========================================
void createtemptables(void)
{
    int i,j;
    PCX backgnd;
    char *menutranspcolors;

    menutranspcolors=(char *)wmalloc(256*256);
    
    //creating palcs 25% & 50% transparency
    backgnd.openMpqPcx(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,CAMPAIGN_STR[0],CAMPAIGN_STR[1],BACKGND_STR));
    LoadTransPal(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,CAMPAIGN_STR[0],CAMPAIGN_STR[1],TRANS25_STR),
		backgnd.GetRawPal768(),menutranspcolors,0.25f,1);
    mytimer.CallTimer(MYTIMER_ASINCHRONMODE);
//    DEBUGMESSCR("%d\n",tick_timer);
    LoadTransPal(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,CAMPAIGN_STR[0],CAMPAIGN_STR[1],TRANS50_STR),
		backgnd.GetRawPal768(),menutranspcolors,0.5f,1);
    mytimer.CallTimer(MYTIMER_ASINCHRONMODE);
//    DEBUGMESSCR("%d\n",tick_timer);
    backgnd.closePcx();

    //creating palnl 25% & 50% transparency
    backgnd.openMpqPcx(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],BACKGND_STR));
    LoadTransPal(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],TRANS25_STR),
		backgnd.GetRawPal768(),menutranspcolors,0.25f,1);
    mytimer.CallTimer(MYTIMER_ASINCHRONMODE);
//    DEBUGMESSCR("%d\n",tick_timer);
    LoadTransPal(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,SINGLEGAME_STR[0],SINGLEGAME_STR[1],TRANS50_STR),
		backgnd.GetRawPal768(),menutranspcolors,0.5f,1);
    mytimer.CallTimer(MYTIMER_ASINCHRONMODE);
//    DEBUGMESSCR("%d\n",tick_timer);
    backgnd.closePcx();

    //creating pal?? 25% & 50% transparency  first ? - race('P'|'T'|'Z') second ? - win|lose ('V'|'D')
    for (i=0;i<3;i++)
    {
	for (j=0;j<2;j++)
	{
	    backgnd.openMpqPcx(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,RACE_CHAR[i],LOSEWIN_STR[j],BACKGND_STR));
	    LoadTransPal(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,RACE_CHAR[i],LOSEWIN_STR[j],TRANS25_STR),
			backgnd.GetRawPal768(),menutranspcolors,0.25f,1);
	    mytimer.CallTimer(MYTIMER_ASINCHRONMODE);
//	    DEBUGMESSCR("%d\n",tick_timer);
	    LoadTransPal(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,RACE_CHAR[i],LOSEWIN_STR[j],TRANS50_STR),
			backgnd.GetRawPal768(),menutranspcolors,0.5f,1);
	    mytimer.CallTimer(MYTIMER_ASINCHRONMODE);
//	    DEBUGMESSCR("%d\n",tick_timer);
	    backgnd.closePcx();
	}
    }
    //creating palR? 25% & 50% transparency  ? - race('P'|'T'|'Z')
    for (i=0;i<3;i++)
    {
	backgnd.openMpqPcx(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,'R',RACE_CHAR[i],BACKGND_STR));
        LoadTransPal(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,'R',RACE_CHAR[i],TRANS25_STR),
		    backgnd.GetRawPal768(),menutranspcolors,0.25f);
	mytimer.CallTimer(MYTIMER_ASINCHRONMODE);
//        DEBUGMESSCR("%d\n",tick_timer);
	LoadTransPal(makefilename(GLUEPAL_NAME,GLUEPAL_OFFSET,'R',RACE_CHAR[i],TRANS50_STR),
		    backgnd.GetRawPal768(),menutranspcolors,0.5f);
	mytimer.CallTimer(MYTIMER_ASINCHRONMODE);
//	DEBUGMESSCR("%d\n",tick_timer);
	backgnd.closePcx();
    }
    wfree(menutranspcolors);
}
