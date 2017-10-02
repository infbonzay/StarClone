
#include <math.h>
#include <grplib/gr8.h>
#include <grplib/usegrp.h>

#include "sdl/grsdl.h"
#include "sdl/keysdl.h"

#include "auxil.h"
#include "man.h"
#include "key.h"
#include "vars.h"
#include "debug.h"
#include "doodad.h"
#include "gr.h"
#include "gener.h"
#include "maps.h"
#include "mouse.h"
#include "wmem.h"
#include "load.h"
#include "objs.h"
#include "mage.h"
#include "lists.h"
#include "loadlo.h"
#include "audio.h"
#include "diap.h"
#include "rules.h"
#include "hod.h"
#include "walk.h"
#include "upgrade.h"
#include "objinfo.h"
#include "action.h"
#include "construct.h"
#include "scripts.h"
#include "market.h"
#include "mpq.h"
#include "weapons.h"
#include "overlay.h"
#include "mytime.h"
#include "images.h"
#include "sigorder.h"
#include "putobj.h"


//=====================================
int getlifepercent(int number,int maxnumber)
{
    int i;
    for (i=5;i>0;i--)
	if (number>=(maxnumber*i/6))
	    return i;
    return 0;
}
//========================================
//risovati u objecta jizni,shield,magia
void desenlife(int x,int y,
               int sizex,int statusplayer,
               int health,int shield,
               int mage,int timelife,
               int  maxhealth,int maxshield,
               int maxmage,int maxtimelife)
{
    int color;
    int kubik,maxkubik;
    if (maxshield)
    {
	createmaxkubikSH(shield,maxshield,sizex);
        desenhealth2(x,y,BLUEHPBAR,kubik,maxkubik);
        y+=SIZEYKUBIK-1;
    }
    switch(getlifepercent(health,maxhealth))
    {
	case 0:
	case 1:
	    color=REDHPBAR;
	    break;
	case 2:
	case 3:
	    color=YELLOWHPBAR;
	    break;
	case 4:
	case 5:
	    color=GREENHPBAR;
	    break;
    }
    createmaxkubik(health,maxhealth,sizex);
    if (maxshield)
    {
	desenhealth2(x,y,color,kubik,maxkubik);
	y+=SIZEYKUBIK+1;
    }
    else
    {
	desenhealth(x,y,color,kubik,maxkubik);
	y+=SIZEYKUBIK+2;
    }
    if ((statusplayer==MYOBJ)&&(maxmage))
    {
        createmaxkubik(mage,maxmage,sizex);
    	desenhealth(x,y,VIOLETHPBAR,kubik,maxkubik);
        y+=SIZEYKUBIK+1;
    }
    if ((statusplayer==MYOBJ)&&maxtimelife)
    {
        createmaxkubik(timelife,maxtimelife,sizex);
    	desenhealth(x,y,VIOLETHPBAR,kubik,maxkubik);
    }
}
//========================================
//risovati odno iz component (jizni|shield|magia)
void desenhealth(int x,int y,int kubik,int count,int max)
{
    int i;
    horizline(KUBIK[GREYHPBAR][0],x,x+max*(SIZEXKUBIK),y+SIZEYKUBIK);
    for (i=0;i<count;i++)
    {
	CPutImage8(x,y,SIZEXKUBIK,SIZEYKUBIK,KUBIK[kubik]);
	x+=SIZEXKUBIK;
    }
    for (;i<max;i++)
    {
	CPutImage8(x,y,SIZEXKUBIK,SIZEYKUBIK,KUBIK[GREYHPBAR]);
	x+=SIZEXKUBIK;
    }
    vertline(KUBIK[GREYHPBAR][0],x,y,y+SIZEYKUBIK);
}
//========================================
void desenhealth2(int x,int y,int kubik,int count,int max)
{
    int i;
    horizline(PKUBIK[GREYHPBAR][0],x,x+max*(SIZEXKUBIK),y+SIZEYKUBIK-1);
    for (i=0;i<count;i++)
    {
	CPutImage8(x,y,SIZEXKUBIK,SIZEYKUBIK-1,PKUBIK[kubik]);
	x+=SIZEXKUBIK;
    }
    for (;i<max;i++)
    {
	CPutImage8(x,y,SIZEXKUBIK,SIZEYKUBIK-1,PKUBIK[GREYHPBAR]);
	x+=SIZEXKUBIK;
    }
    vertline(PKUBIK[GREYHPBAR][0],x,y,y+SIZEYKUBIK-1);
}

/* a mechanism is needed to support multiple decoration schemes */
/* currently we do not care to create one. This might be done as
 * part of a theme engine - which would be the preferred way */

/* this value can be modified on the fly */
unsigned char selected_unit_decoration[4] = 
{ 
  REDCIRCLE, 
  YELLOWCIRCLE, 
  YELLOWCIRCLE, 
  GREENCIRCLE
};

//===================================================
int showcircle(int x,int y,GRPFILE *grp,int color)
{
    x-=grp->SizeX/2;
    y-=grp->SizeY/2;
    GRP_gr_gamernr=color;
    putgrp(x,y-5,grp,0);
    return(y+grp->Picture[0].LinesPerPicture+grp->Picture[0].SkipUp);//+5);
}
//===================================================
int desencircle(int x,int y,int st,struct OBJ *a,struct OBJstruct *b)
{
    int color,flingy_id,sprites_id,images_tbl;
    int nrcircle,circleoffset;
    y += 10;
/*    if (IsInvincibleUnit(a->SC_Unit)
	color = YELLOWCIRCLE;
    else
*/
    color = selected_unit_decoration[st];

    if (a->SC_Unit!=SC_NOUNITNR)
    {
        flingy_id=alldattbl.units_dat->flingy_id[a->SC_Unit];
	sprites_id=alldattbl.flingy_dat->sprites_id[flingy_id];
	nrcircle=alldattbl.sprites_dat->sel_circle_image[sprites_id-130];
	if (IsOBJBurrowed(a))
	{
	    if (a->SC_Unit==SC_LURKEROBJ)
		nrcircle=2;
	    else
		nrcircle=0;
	}
	circleoffset=alldattbl.sprites_dat->sel_circle_offset[sprites_id-130];
	if (!fullc[nrcircle])
	{
	    images_tbl=alldattbl.images_dat->images_tbl[nrcircle+IMAGES_DAT_FIRSTCIRCLENORMAL];
	    GetLoadedImage(2,images_tbl,(void **)&fullc[nrcircle]);
	}
	if (NETWORKGAME&&!puncc[nrcircle])
	{
	    images_tbl=alldattbl.images_dat->images_tbl[nrcircle+IMAGES_DAT_FIRSTCIRCLEDASHED];
	    GetLoadedImage(2,images_tbl,(void **)&puncc[nrcircle]);
	}
    }
    return(showcircle(x,y+circleoffset-5,fullc[nrcircle],color));
}
//==================================
void ifselectedDAMAGEDBUILD(OBJ *a)
{
    if (a)
    if (IsMechanical(a->SC_Unit) && IsReadyOBJ(a) && IsBuild(a->SC_Unit))
    {
	switch(DamageFactor(a->health,GetUnitMaxHealth(a->SC_Unit)))
	{
	    case 0:
		break;
	    case 1:
	    case 2:
	    case 3:
		Play_sfxdata_id(a,SFXDATA_BUILDFIRE1,2,0);
		break;
	    case 4:
	    case 5:
	    case 6:
		Play_sfxdata_id(a,SFXDATA_BUILDFIRE2,2,0);
		break;
	}
    }
}
//==============================================
//return 
//	0 - close & notsee
//	1 - open  & notsee
//	2 - open  & see
int GetMapFogUnderOBJ(OBJ *a,OBJstruct *b)
{
    if (mapSEE(a->xkart,a->ykart)>1)
	return(2);
    else
	if (mapOPEN(a->xkart,a->ykart)>1)
	    return(1);
        else
	    return(0);
}
//==============================================
void NewUnitPlace(OBJ *a,SCUNIT SC_Unit,int *first_xpos,int *first_ypos)
{
//    *first_xpos=a->sourcex+64;
//    *first_ypos=a->sourcey+64;
    getcoordofnewunit(a,SC_Unit,first_xpos,first_ypos);
}
//=====================
OBJ *CreateUnitInUnit(struct OBJ *a,int obj_id,int playsound,int first_xpos,int first_ypos)
{
    struct OBJ *temp;
    if (!first_xpos||!first_ypos)
    {
	NewUnitPlace(a,obj_id,&first_xpos,&first_ypos);
    }
    switch(obj_id)
    {
	case SC_SCARABOBJ:
	    a->ammo++;
	    break;
	case SC_INTERCEPTOROBJ:
	case SC_NUKEOBJ:
	    temp = createobjman(first_xpos,first_ypos,obj_id,a->playernr);
    	    addchild(a,temp);
	    temp->prop |= VARNOTHERE;
	    temp->finalOBJ = NULL;
	    temp->prop |= VARINBASE;
	    temp->modemove = MODERECHARGE;
	
	    temp->mainimage->DisableExecScript();
    	    temp->mainimage->HideChildsImgFlag();
	    temp->mainimage->HideImgFlag();
	    temp->mainimage->elevationlevel = a->mainimage->elevationlevel-1;
	    temp->prop |= VARREADY;

	    if (obj_id == SC_NUKEOBJ)
	    {
		PLAYER[a->playernr].nukes++;
		if (playsound)
		    playinfoadvisorsound(a->playernr,TERRANRACE,ADVNUCLREADY,PLAYADVISOR_TEXTANDSOUND);
	    }
	    else
	    { 
		if (obj_id == SC_INTERCEPTOROBJ)
		{
		    SetOrder(temp,1,&SIGOrder_InterceptorAfterAtack);
		}
	    }
	    break;
	default:
	    temp = createobjman(first_xpos,first_ypos,obj_id,a->playernr);
	    temp->finalx = a->finalx;
	    temp->finaly = a->finaly;
	    SIGOrder_UnitInitComplete(temp);
	    break;
    }
    return(temp);
}
//=====================
//process to construct unit in builds
void workingbuilds(struct OBJ *a)
{
    int i,emptyafterconstr=0;
    int type_id,obj_id;
    unsigned char upgrace,techrace;
    struct OBJ *temp;
    struct OBJstruct *b,*b2;
    b=loadobj(a->SC_Unit);
    if ( a->prop & VARPOWEROFF )
	return;
    //no constructions
    if ( !GetConstructNR(a))
	return;
    type_id = TickBuildMakeConstruct(a,&obj_id);
    switch(type_id)
    {
	case ORDERS_NONE:
	    return;
	case ORDERS_MODEMOVE:
	    return;
	case ORDERS_UNIT:
	    if (IsBuild(obj_id) && IsAddon(obj_id))
	    {
	    }
	    else
	    {
		CreateUnitInUnit(a,obj_id,1,0,0);
    	    }
	    break;
	case ORDERS_UPGRADE:
	    upgrace=alldattbl.upgrades_dat->Race[obj_id];
	    AddUpgradeTree(&map,a->playernr,obj_id);
	    playinfoadvisorsound(a->playernr,upgrace,ADVRESEARCH2,PLAYADVISOR_TEXTANDSOUND);
    	    break;
	case ORDERS_TECHNOLOGY:
	    techrace=alldattbl.techdata_dat->Race[obj_id];
	    AddTechTree(&map,a->playernr,obj_id);
	    playinfoadvisorsound(a->playernr,techrace,ADVRESEARCH1,PLAYADVISOR_TEXTANDSOUND);
	    break;
    }
}
//==============================================
//==============================================
mylist minimap_showedobjs;
//==============================================
void minimap_showobjs(void)
{
    int i,max;
    OBJ *a;
    minimap_showedobjs.EnumListInit();
    while(a=(OBJ *)minimap_showedobjs.GetNextListElem())
    {
	minimap_showobj(a);
    }
    minimap_showedobjs.FlushList();		//clear all obj drawed on minimap for prepare a new one
}
//==============================================
void putobjsonminimap(void)
{
    int i;
    struct OBJ *a;
//    struct OBJstruct *b;
    if ((MAPUNITSREGENERATIONBIT)&&(MAPDEF&UNITS))		//if Desenterrain && time to regeneration minimap
    {
	for (i=0;i<MaxObjects;i++)
	{
    	    a=objects[i];
    	    if (IfCanClickOBJ(a->SC_Unit))
    	    {
    		if (!(a->prop&(VARINBASE|VARNOTHERE|VARINTRANSPORT)))
		if (!OBJ_VAR_CHK(a,obj_notdetect,NUMBGAMER))
		{
			if (mapSEE(a->xkart,a->ykart)>=1)
			{
			    minimap_showedobjs.AddList(a);
			}
		}
	    }
	}
    }
}
//==============================================
short unitlimitmes[3][4]=
{
    {820,814,817,823},
    {821,815,818,824},
    {822,816,819,825}
};
//==============================================
void ShowBaseBuildMessage(int XWINPOS,int YWINPOS,OBJ *a,OBJstruct *b)
{
    char mes[50];
    if (a->playernr!=NUMBGAMER||GetConstructNR(a))
	return;
    int base = IsUnitBaseBuild(a);
    if (!IsOBJUnderConstruct(a)&&(!GetMageAtr(&a->atrobj,ATRHALLUCINATION))&&
	(base==1 || base==2 || IsPsiUnit(a->SC_Unit)) && (!a->loaded))
    {
	int race=GetUnitRace(a->SC_Unit);
	sprintf(mes,"%s%d",alldattbl.stattxt_tbl->get_TBL_STR(unitlimitmes[race][0]),PLAYER[a->playernr].curentpsi[race]/2);
	putrowtext(XWINPOS+143,YWINPOS+30,111,IDFONT8,TEXT_CENTERALIGN,mes,GBLUECOLORFONT,tfontgamp);
	sprintf(mes,"%s%d",alldattbl.stattxt_tbl->get_TBL_STR(unitlimitmes[race][1]),GetSupplyProvided(a->SC_Unit)/2);
	putrowtext(XWINPOS+143,YWINPOS+44,111,IDFONT8,TEXT_CENTERALIGN,mes,GBLUECOLORFONT,tfontgamp);
	sprintf(mes,"%s%d",alldattbl.stattxt_tbl->get_TBL_STR(unitlimitmes[race][2]),PLAYER[a->playernr].maxcurentpsi[race]/2);
	putrowtext(XWINPOS+143,YWINPOS+58,111,IDFONT8,TEXT_CENTERALIGN,mes,GBLUECOLORFONT,tfontgamp);
	sprintf(mes,"%s%d",alldattbl.stattxt_tbl->get_TBL_STR(unitlimitmes[race][3]),MAXPSI/2);
	putrowtext(XWINPOS+143,YWINPOS+72,111,IDFONT8,TEXT_CENTERALIGN,mes,GBLUECOLORFONT,tfontgamp);
    }
}
//==============================================
void PutUpgrdMes(int xpos,int ypos,char *iconmes,char *message,int param1,int param2,char *finaltext)
{
    char mes[100];
    if (iconmes)
	if (param2<=0)
	    sprintf(mes,"%s\n%s %d %s",iconmes,message,param1,finaltext);
	else
	    sprintf(mes,"%s\n%s %d+%d %s",iconmes,message,param1,param2,finaltext);
    else
	sprintf(mes,"%s: %d",message,param1);
    putboxmessage(  FONTCOLOR(tfontgamp,GBLUECOLORFONT,2),
            	    FONTCOLOR(tfontgamp,GBLUECOLORFONT,4),
            	    xpos,
	    	    ypos,
            	    2,IDFONT8,mes,GBLUECOLORFONT,tfontgamp,gamedlggrp);
}
//==============================================
void ShowUpgradesInBar(int XWINPOS,int YWINPOS,OBJ *a,OBJstruct *b)
{
    int xpos=XWINPOS+104,ypos=YWINPOS+51;
    unsigned short stattxt_id;
    unsigned char  nrofhits,tech_id;
    SCUNIT Subunit1;
    int upgradenr,upgtot,mousehot=MOUSEONUPGRDICONS;
    int upgrade_id,weapon_id1,weapon_id2;
    int atack1,atack2,armor,sharmor,icon,icon2;
    int MESxpos=-1,MESypos,MESparam1,MESparam2;
    int newunit;
    char *MESstr,*MESicon,*tempfinal="",*MESfinal="";
    char *mes1,*mes2;

    char sname[3]={0,0,0};
    if (IsOBJUnderConstruct(a))//&&a->SC_Unit!=SC_EGGOBJ&&a->SC_Unit!=SC_LURKEREGGOBJ)
	return;
    if (GetConstructNR(a)||a->loaded)
	return;
    SCUNIT SC_Unit=a->SC_Unit;
    upgrade_id=alldattbl.units_dat->ArmorUpgrade[SC_Unit];
    if (upgrade_id<MAX_UPGRADES_ELEM)
    {
	icon=alldattbl.upgrades_dat->Icon[upgrade_id];
	if (icon>0)
	{
	    armor=GetArmor(a,b->armorupgnr,&upgradenr,&upgtot);
	    putgrpspr(xpos+2,ypos+2,znakgrp,NORMAL,255,FORBLUE,NULL,icon);
	    putgrp(xpos,ypos,grpicons,12);
	    sname[0]='0'+upgradenr;
	    putmessage(xpos+26,ypos+24,IDFONT8,sname,GBLUECOLORFONT,tfontgamp,gamedlggrp);
	    if (mousehotpos==mousehot)
	    {
		MESxpos=xpos+36;
		MESypos=ypos+28;
		stattxt_id=alldattbl.upgrades_dat->Label[upgrade_id];
		MESicon=alldattbl.stattxt_tbl->get_TBL_STR(stattxt_id-1);
		MESstr=alldattbl.stattxt_tbl->get_TBL_STR(STATTXT_TBL_ARMORTEXT);
		MESparam1=armor;
		MESparam2=upgtot;
	    }
	    xpos+=36;
	    mousehot+=2;
	}
    }
    if (alldattbl.units_dat->ShieldEnable[SC_Unit])
    {
	sharmor=GetShieldArmor(a,&upgradenr);
	icon=alldattbl.upgrades_dat->Icon[UPGRADES_DAT_PLASMASHIELDICON];
	putgrpspr(xpos+2,ypos+2,znakgrp,NORMAL,255,FORBLUE,NULL,icon);
	putgrp(xpos,ypos,grpicons,12);
	sname[0]='0'+upgradenr;
	putmessage(xpos+26,ypos+24,IDFONT8,sname,GBLUECOLORFONT,tfontgamp,gamedlggrp);
	if (mousehotpos==mousehot)
	{
	    MESxpos=xpos+36;
	    MESypos=ypos+28;
	    stattxt_id=alldattbl.upgrades_dat->Label[UPGRADES_DAT_PLASMASHIELDICON];
	    MESicon=alldattbl.stattxt_tbl->get_TBL_STR(stattxt_id-1);
	    MESstr=alldattbl.stattxt_tbl->get_TBL_STR(STATTXT_TBL_SHIELDSTEXT);
	    MESparam1=sharmor;
	    MESparam2=-1;
	}
	xpos+=36;
	mousehot+=2;
    }
    Subunit1=alldattbl.units_dat->Subunit1[SC_Unit];
    if (Subunit1<MAX_UNITS_ELEM)	
	SC_Unit=Subunit1;
    weapon_id1=alldattbl.units_dat->GroundWeapon[SC_Unit];
    nrofhits=alldattbl.units_dat->MaxGroundHits[SC_Unit];
    if (nrofhits>2)
	nrofhits=nrofhits&0x02;
    if (weapon_id1<MAX_WEAPONS_ELEM)
    {
	icon=alldattbl.weapons_dat->Icon[weapon_id1];
//	if (nrofhits>2)
//	    nrofhits=2;//goundatack less or equal then 2
	atack1=GetInfoWeapon(a->SC_Unit,a->playernr,nrofhits,weapon_id1,&upgradenr,&upgtot,&tempfinal);
	putgrpspr(xpos+2,ypos+2,znakgrp,NORMAL,255,FORBLUE,NULL,icon);
	putgrp(xpos,ypos,grpicons,12);
	sname[0]='0'+upgradenr;
	putmessage(xpos+26,ypos+24,IDFONT8,sname,GBLUECOLORFONT,tfontgamp,gamedlggrp);
	if (mousehotpos==mousehot)
	{
		MESxpos=xpos+36;
	    	MESypos=ypos+28;
		stattxt_id=alldattbl.weapons_dat->Label[weapon_id1];
		MESicon=alldattbl.stattxt_tbl->get_TBL_STR(stattxt_id-1);
		MESstr=alldattbl.stattxt_tbl->get_TBL_STR(STATTXT_TBL_DAMAGETEXT);
		MESparam1=atack1;
		MESparam2=upgtot;
	}
	xpos+=36;
    	mousehot+=2;
    }
    weapon_id2=alldattbl.units_dat->AirWeapon[SC_Unit];
    nrofhits=alldattbl.units_dat->MaxAirHits[SC_Unit];
    if (weapon_id2<MAX_WEAPONS_ELEM&&weapon_id1!=weapon_id2)
    {
	icon=alldattbl.weapons_dat->Icon[weapon_id2];
	atack2=GetInfoWeapon(a->SC_Unit,a->playernr,nrofhits,weapon_id2,&upgradenr,&upgtot,&tempfinal);
	putgrpspr(xpos+2,ypos+2,znakgrp,NORMAL,255,FORBLUE,NULL,icon);
	putgrp(xpos,ypos,grpicons,12);
	sname[0]='0'+upgradenr;
	putmessage(xpos+26,ypos+24,IDFONT8,sname,GBLUECOLORFONT,tfontgamp,gamedlggrp);
	if (mousehotpos==mousehot)
	{
	    MESxpos=xpos+36;
	    MESypos=ypos+28;
	    stattxt_id=alldattbl.weapons_dat->Label[weapon_id2];
	    MESicon=alldattbl.stattxt_tbl->get_TBL_STR(stattxt_id-1);
	    MESstr=alldattbl.stattxt_tbl->get_TBL_STR(STATTXT_TBL_DAMAGETEXT);
	    MESfinal=tempfinal;
	    MESparam1=atack2;
	    MESparam2=upgtot;
	}
	xpos+=36;
	mousehot+=2;
    }
    upgtot=GetNrSomeObjects(a,&newunit,&mes1);
    if (upgtot>=0)
    {
	if (upgtot<10)
	    sname[0]='0'+upgtot;
	else
	{
	    sname[0]='0'+upgtot/10;
	    sname[1]='0'+upgtot%10;
	}
	weapon_id1=alldattbl.units_dat->GroundWeapon[newunit];
	nrofhits=alldattbl.units_dat->MaxGroundHits[newunit];
	icon=newunit;
	putgrpspr(xpos+2,ypos+2,znakgrp,NORMAL,255,FORBLUE,NULL,icon);
	putgrp(xpos,ypos,grpicons,12);
	putmessage(xpos+26,ypos+24,IDFONT8,sname,GBLUECOLORFONT,tfontgamp,gamedlggrp);
	if (mousehotpos==mousehot)
	{
	    MESxpos=xpos+36;
	    MESypos=ypos+28;
	    if (weapon_id1<MAX_WEAPONS_ELEM)
	    {
		atack1=GetInfoWeapon(a->SC_Unit,a->playernr,nrofhits,weapon_id1,&upgradenr,&upgtot,&tempfinal);
		MESicon=mes1;
		MESstr=alldattbl.stattxt_tbl->get_TBL_STR(STATTXT_TBL_DAMAGETEXT);
		MESparam1=atack1;
		MESparam2=upgtot;
	    }
	    else
	    {
		MESicon=NULL;
		MESstr=mes1;
		MESparam1=upgtot;
		MESparam2=0;
	    }
	}
    }
    if (MESxpos!=-1)
	PutUpgrdMes(MESxpos,MESypos,MESicon,MESstr,MESparam1,MESparam2,MESfinal);
}
//==============================================
void printobjparam(void)
{
    char *tblname;
    char tblcolor;
    char sname[400];
    int snamecolor;
    struct XY uu;
    int XWINPOS=138+DELTASCREENX;	//depend on screensize
    int YWINPOS=388+DELTASCREENY;	//
    int i,addxforlife=0,xx,yy,xout=XOUT+10,yout=YOUT,maxtimeleft,objtype;
//    int i,addxforlife=0,xx,yy,xout=XUNITBAR,yout=YUNITBAR,maxtimeleft,objtype;
    int icon,objcount,shield_pers,xsize;
    unsigned int life_pers;
    int nrofconstrslots,type_id,obj_id,icon_nr,stattxt_nr,stattxt_nr2,race,build,constrelem;
    int flingy_id,sprites_id,sizexhealthbar;
    int constrcomplete,underconstruct=0;
    int shields,lifes,shieldl;
    int sizex,sizey;
    SCUNIT SC_Unit;
    char ss[100],ss1[100];
    char *ss2;
    struct OBJ *a;
    struct OBJstruct *b,*tempobjstr;
    existinbuildconstr = 0;
    objcount = selectobjcount();
    if (objcount==1)
    {
        for (i=0;i<MAXSELECTMAN;i++)
             if (fordeselect[i])
             {
                a=fordeselect[i];
//		if (a->prop&VARCANTSELECT)
//		    return;
		SC_Unit=a->SC_Unit;
                b=loadobj(SC_Unit);
                break;
             }
	constrelem=GetConstructNR(a);
        ss[0]=0;
    	tblname=getOBJname(SC_Unit);
	putrowtext(XWINPOS+102,YWINPOS+0,152,IDFONT10,TEXT_CENTERALIGN,tblname,GBLUECOLORFONT,tfontgamp);

	tblcolor=GBLUECOLORFONT;
	tblname=NULL;
	sname[0]=0;
	if (a->prop&VARREADY)
	{
	    //is resourcebuild or geyser and depleted show this message
	    if ( allow_display_mineralstatus( a, NUMBGAMER ) && 
		    ( IsResource(a,SC_GEYSERBUILDSOBJ) || IsResource(a,SC_GEYSEROBJ) )&& IfGeyserIsDepleted(a) )
	    {
		tblname=alldattbl.stattxt_tbl->get_TBL_STR(STATTXT_TBL_DEPLETEDTEXT);
		tblcolor=GWHITECOLORFONT;
	    }
	    else 
		if( allow_display_transportcontents( a, NUMBGAMER ) && a->loaded )
		    drawunitsintransport(XWINPOS,YWINPOS,a);
		else 
		    if( allow_display_powerstatus( a, NUMBGAMER ) && (a->prop & VARPOWEROFF) )
		    {
    			tblname=alldattbl.stattxt_tbl->get_TBL_STR(STATTXT_TBL_UNPOWEREDTEXT);
			tblcolor=GREDCOLORFONT;
		    }
		else 
		    if( allow_display_unitstatus( a, NUMBGAMER ) )
		    {
			if (fordeselect[0])
		        for( int attr=0; attr<MAXMAGEATR; attr++ )
		        {
			    //don't display hallucination on enemy unit
			    if (attr==ATRHALLUCINATION&&player_aliance(fordeselect[0]->playernr,NUMBGAMER)==ENEMYOBJ)
			        continue;
			    if(mage_descriptions_hod[attr]==-1)
				continue;
			    if(GetMageAtr(&a->atrobj,attr)==0 )
				continue;
    			    tblname=alldattbl.stattxt_tbl->get_TBL_STR(mage_descriptions_hod[attr]);
			    tblcolor=GYELLOWCOLORFONT;
			    if (attr==ATRCORROSIVEACID)
			    {
				sprintf(sname,"%s:%d",tblname,GetCorrosiveAcidValue(fordeselect[0]));
				tblname=sname;
			    }
			    break;
			}
		    }
	}
	if (tblname)
	{
	    if (!constrelem)
		//show status unit(observer,parasite,haluc,etc.)
		putrowtext(XWINPOS+102,YWINPOS+21,152,IDFONT10,TEXT_CENTERALIGN,tblname,tblcolor,tfontgamp);
	    else
	    {
		getmessagelen(IDFONT10,tblname,&xsize,NULL);
		putmessage((XOUT+410-xsize)/2,YOUT-4+4,IDFONT10,tblname,GWHITECOLORFONT,tfontgamp,gamedlggrp);
	    }
	}
        sname[0]=0;
//	if (!IsInvincibleOBJ(a))
	{
	    if (!IsShieldEnable(SC_Unit))
	    {
		shield_pers = 0;
	    }
	    else
	    {
		shield_pers = (int) a->shield*100/GetUnitMaxShield(SC_Unit);
	    }
	    objtype = 0;
	    life_pers = (unsigned int) a->health*100/GetUnitMaxHealth(SC_Unit);
	    drawunitinbar(XWINPOS+30,YWINPOS+0,4,0,NORAMKA,shield_pers,life_pers,grpicons,grpwire3,SC_Unit);
	}
	if (IsResourceContainer(SC_Unit)&&(!IsOBJUnderConstruct(a)) && !IfGeyserIsDepleted(a))
	{
	//show resource nr
	    strcpy(ss,alldattbl.stattxt_tbl->get_TBL_STR(b->additionaltext));
	    itoa(a->data.resource.resource_count,ss1,10);
    	    strcat(ss,ss1);
	    putrowtext(XWINPOS+125,YWINPOS+39,107,IDFONT8,TEXT_CENTERALIGN,ss,GWHITECOLORFONT,tfontgamp);
	}
	if (!IsInvincibleOBJ(a))
	{
	    if (!IsResourceContainer(SC_Unit)||IsGroupBuildFlag(SC_Unit))
	    {
		flingy_id=alldattbl.units_dat->flingy_id[SC_Unit];
		sprites_id=alldattbl.flingy_dat->sprites_id[flingy_id];
		sizexhealthbar=alldattbl.sprites_dat->health_bar[sprites_id-130];
		if (sizexhealthbar)
		{
	    	    shields=0;
	    	    shieldl=0;
	    	    if (IsShieldEnable(a->SC_Unit))
    	    	    {
			ss[0]=GBLUECOLORFONT;
			ss[1]=0;
        		itoa(a->shield>>8,ss1,10);
        		strcat(ss,ss1);
        		strcat(ss,"/");
        		itoa(GetUnitMaxShield(a->SC_Unit)>>8,ss1,10);
        		strcat(ss,ss1);
			shieldl = getmessagelen(IDFONT8,ss,&shields,NULL);
        		strcat(ss," "GGREENCOLORFONTCHAR);
	    	    }
	    	    else
	    	    {
			ss[0]=0;
		    }
	    	    itoa((a->health+0xff)>>8,ss1,10);
    	    	    strcat(ss,ss1);
    	    	    strcat(ss,"/");
    	    	    itoa((GetUnitMaxHealth(a->SC_Unit)+0xff)>>8,ss1,10);
    	    	    strcat(ss,ss1);
		    putrowtext(XWINPOS+22,YWINPOS+64,80,IDFONT8,TEXT_CENTERALIGN,ss,GGREENCOLORFONT,tfontgamp);
		}
	    }
	}
	ShowBaseBuildMessage(XWINPOS,YWINPOS,a,b);
	ShowUpgradesInBar(XWINPOS,YWINPOS,a,b);
	addxforlife=0;
    	ss[0]=0;
        if (player_aliance(NUMBGAMER,a->playernr)==MYOBJ&&IsSpellCaster(a->SC_Unit))
        {
            itoa(a->mana>>8,ss1,10);
            strcat(ss,ss1);
            strcat(ss,"/");
            itoa(GetUnitMaxMana(a->SC_Unit,a->playernr)>>8,ss1,10);
            strcat(ss,ss1);
            strcat(ss,"  ");
    	}
        if (player_aliance(NUMBGAMER,a->playernr)==MYOBJ&&a->timeleft)
        {
            if (GetMageAtr(&a->atrobj,ATRHALLUCINATION)>0)
        	maxtimeleft=mageprop[MODEHALLUCINATION].timemageactive;
            else
        	maxtimeleft=b->maxtimeleft;
            itoa(a->timeleft/NORMALSPEED_TICKS,ss1,10);
	    strcat(ss,ss1);
            strcat(ss,"/");
            itoa(maxtimeleft/NORMALSPEED_TICKS,ss1,10);
            strcat(ss,ss1);
        }
	if (ss[0])
	{
//	    getmessagelen(IDFONT8,ss,&xsize,NULL);
//	    putmessage((XOUT+164-xsize)/2,
//			yout+65,IDFONT8,ss,GWHITECOLORFONT,tfontgamp,gamedlggrp);
	    putrowtext(XWINPOS+22,YWINPOS+76,80,IDFONT8,TEXT_CENTERALIGN,ss,GWHITECOLORFONT,tfontgamp);
        }
	if (IsOBJUnderConstruct(a))
	    underconstruct=1;
	if (underconstruct)
	{
	    nrofconstrslots=1;
	}
	else
	{
	    nrofconstrslots=5;
	    if (GetConstruct(a,0,&type_id,&obj_id,NULL,NULL,NULL,NULL))
	    {
		if (type_id==ORDERS_UNIT)
		{
		    if (IsBuild(obj_id)||obj_id==SC_NUKEOBJ)
			nrofconstrslots=1;
		}
		else
		    nrofconstrslots=1;
	    }
	    else
		nrofconstrslots=0;
	}
        if (nrofconstrslots==1&&player_aliance(NUMBGAMER,a->playernr)==MYOBJ)
        {
            xx=XOUT+40;
            yy=YOUT+25;
    	    putgrp(XUNITCONSTR,YUNITCONSTR,grpicons,2);
	    existinbuildconstr = 1;
	    if (underconstruct)
	    {
		type_id=ORDERS_UNIT;
		if (SC_Unit==SC_EGGOBJ || SC_Unit==SC_LURKEREGGOBJ|| SC_Unit==SC_MUTALISKCOCOONOBJ)
		    SC_Unit=a->SC_ConstrUnit;
		obj_id=SC_Unit;
		icon_nr=SC_Unit;
		stattxt_nr=SC_Unit;
		constrcomplete=GetSelfConstrComplete(a,buildfullraw.xsizePcx());
	    }
	    else
	    {
		GetConstruct(a,0,&type_id,&obj_id,NULL,NULL,&icon_nr,&stattxt_nr);
		constrcomplete=GetConstrComplete(a,buildfullraw.xsizePcx());
	    }
	    sizex=znakgrp->Picture[icon_nr].PixelPerLine;
	    sizey=znakgrp->Picture[icon_nr].LinesPerPicture;
	    if (sizex<32)
	        sizex=(32-sizex)/2;
	    else
	        sizex=0;
	    if (sizey<32)
	        sizey=(32-sizey)/2;
	    else
	        sizey=0;
	    //show cancelled unit in bar in time of construction
    	    if (oldmouseonconstrunit!=-1&&(!oldmouseonconstrunit))
		putgrpspr(xy[0][0]+XUNITCONSTR+1+2+sizex,xy[1][0]+YUNITCONSTR+1+2+sizey,
			znakgrp,NORMAL,255,FORYELLOW,NULL,icon_nr);
	    else
	        putgrpspr(xy[0][0]+XUNITCONSTR+sizex+2,xy[1][0]+YUNITCONSTR+sizey+2,
			znakgrp,NORMAL,255,FORYELLOW,NULL,icon_nr);
            putspr(xx,yy,
		    buildemptyraw.xsizePcx(),
                    buildemptyraw.ysizePcx(),
		    buildemptyraw.xsizePcx(),
		    buildemptyraw.GetPcxRawBytes());
    	    putspr(xx,yy,constrcomplete,
        	    buildfullraw.ysizePcx(),
		    buildfullraw.xsizePcx(),
		    buildfullraw.GetPcxRawBytes());
//	    tempobjstr=alldattbl.stattxt_tbl->get_TBL_STR(stattxt_nr)+2;
	    switch(type_id)
	    {
		case ORDERS_UNIT:
		    race=GetUnitRace(obj_id);
        	    build=IsBuild(obj_id);
		    stattxt_nr2=ub_constr[race][build];
        	    break;
		case ORDERS_UPGRADE:
		    race=alldattbl.upgrades_dat->Race[obj_id];
		    stattxt_nr2=ub_constr[race][2];
        	    break;
		case ORDERS_TECHNOLOGY:
		    race=alldattbl.techdata_dat->Race[obj_id];
		    stattxt_nr2=ub_constr[race][3];
        	    break;
            }
    	    ss2=alldattbl.stattxt_tbl->get_TBL_STR(stattxt_nr2);
	    //put message under progress of upgrade/research/constr
	    getmessagelen(IDFONT8,ss2,&xsize,NULL);
	    putmessage(xx+55-xsize/2,yy-15,IDFONT8,ss2,GBLUECOLORFONT,tfontgamp,gamedlggrp);
        }
        if (nrofconstrslots==5&&(player_aliance(NUMBGAMER,a->playernr)==MYOBJ))
        {
	    existinbuildconstr = 1;
	    GRP_gr_gamernr = FORYELLOW;
            for (i=0;i<MAXCONSTRUCTINBUILD;i++)
	    {
		switch(i)
		{
		    case 0:
			putgrp( xy[0][i]+XUNITCONSTR,
				xy[1][i]+YUNITCONSTR,grpicons,2);
			if (constrelem>1)
			    putgrp( xy[0][1]+XUNITCONSTR,
				    xy[1][1]+YUNITCONSTR-3,grpicons,11);
			break;
		    case 1:
		    case 2:
		    case 3:
			if (constrelem>i+1)
			    putgrp( xy[0][i]+XUNITCONSTR,xy[1][i]+YUNITCONSTR,grpicons,4);
			else
			    putgrp( xy[0][i]+XUNITCONSTR,xy[1][i]+YUNITCONSTR,grpicons,2);
			break;
		    case 4:
			putgrp( xy[0][i]+XUNITCONSTR,xy[1][i]+YUNITCONSTR,grpicons,2);
			break;		    
		}
                if (constrelem<=i)
		    putgrp( xy[0][i]+XUNITCONSTR+8,xy[1][i]+YUNITCONSTR+10,grpicons,i+6);
            }
	    for (i=0;i<constrelem;i++)
	    {
		GetConstruct(a,i,NULL,NULL,NULL,NULL,&icon_nr,&stattxt_nr);
		sizex=znakgrp->Picture[icon_nr].PixelPerLine;
		sizey=znakgrp->Picture[icon_nr].LinesPerPicture;
		if (sizex<32)
		    sizex=(32-sizex)/2;
		else
		    sizex=0;
		if (sizey<32)
		    sizey=(32-sizey)/2;
		else
		    sizey=0;
		if (oldmouseonconstrunit!=-1&&oldmouseonconstrunit==i)
		{
            	    putgrp( xy[0][i]+XUNITCONSTR+1+sizex+2,
			    xy[1][i]+YUNITCONSTR+1+sizey+2,
                    	    znakgrp,icon_nr);
		}
		else
		{
            	    putgrp( xy[0][i]+XUNITCONSTR+sizex+2,
			    xy[1][i]+YUNITCONSTR+sizey+2,
                    	    znakgrp,icon_nr);
		}
            }
	    xx=XOUT+40;
            yy=YOUT+25;
            putspr(xx,yy,
		    buildemptyraw.xsizePcx(),
                    buildemptyraw.ysizePcx(),
		    buildemptyraw.xsizePcx(),
		    buildemptyraw.GetPcxRawBytes());
            putspr(xx,yy,GetConstrComplete(a,buildfullraw.xsizePcx()),
        	    buildfullraw.ysizePcx(),
		    buildfullraw.xsizePcx(),
		    buildfullraw.GetPcxRawBytes());
	    switch(type_id)
	    {
		case ORDERS_UNIT:
		    race=GetUnitRace(obj_id);
        	    build=IsBuild(obj_id);
		    stattxt_nr2=ub_constr[race][build];
        	    break;
		case ORDERS_UPGRADE:
		    race=alldattbl.upgrades_dat->Race[obj_id];
		    stattxt_nr2=ub_constr[race][2];
        	    break;
		case ORDERS_TECHNOLOGY:
		    race=alldattbl.techdata_dat->Race[obj_id];
		    stattxt_nr2=ub_constr[race][3];
        	    break;
            }
    	    ss2=alldattbl.stattxt_tbl->get_TBL_STR(stattxt_nr2);
	    getmessagelen(IDFONT8,ss2,&xsize,NULL);
    	    putmessage(xx+55-xsize/2,yy-15,IDFONT8,ss2,GBLUECOLORFONT,tfontgamp,gamedlggrp);
    	}
	if (mouseonconstrunit!=-1)
	{
	    if (mouseonconstrunit<GetConstructNR(a)||(underconstruct&&mouseonconstrunit==0))
	    {
		if (underconstruct)
		{
		    type_id=ORDERS_UNIT;
		    if (SC_Unit==SC_EGGOBJ || SC_Unit==SC_LURKEREGGOBJ|| SC_Unit==SC_MUTALISKCOCOONOBJ)
			SC_Unit=a->SC_ConstrUnit;
		    obj_id=SC_Unit;
		    icon_nr=SC_Unit;
		}
		else
		{
		    GetConstruct(a,mouseonconstrunit,&type_id,&obj_id,NULL,NULL,&icon_nr,NULL);
		}
		switch(type_id)
		{
		    case ORDERS_UNIT:
			race=GetUnitRace(obj_id);
        		build=IsBuild(obj_id);
			stattxt_nr2=ub_deconstr[race][build];
	    		sprintf(ss,alldattbl.stattxt_tbl->get_TBL_STR(stattxt_nr2),getOBJname(obj_id));
        		break;
		    case ORDERS_UPGRADE:
			race=alldattbl.upgrades_dat->Race[obj_id];
			stattxt_nr=alldattbl.upgrades_dat->Label[obj_id]-1;
			stattxt_nr2=ub_deconstr[race][2];
	    		sprintf(ss,"%s\n(%s)",alldattbl.stattxt_tbl->get_TBL_STR(stattxt_nr2),alldattbl.stattxt_tbl->get_TBL_STR(stattxt_nr));
        		break;
		    case ORDERS_TECHNOLOGY:
			race=alldattbl.techdata_dat->Race[obj_id];
			stattxt_nr=alldattbl.techdata_dat->Label[obj_id]-1;
			stattxt_nr2=ub_deconstr[race][3];
	    		sprintf(ss,"%s\n(%s)",alldattbl.stattxt_tbl->get_TBL_STR(stattxt_nr2),alldattbl.stattxt_tbl->get_TBL_STR(stattxt_nr));
        		break;
        	}
		//putmessage under icon of resear/upgr/constr
    		putboxmessage(  FONTCOLOR(tfontgamp,GBLUECOLORFONT,2),
                        	FONTCOLOR(tfontgamp,GBLUECOLORFONT,4),
                    		XUNITCONSTR+xy[0][mouseonconstrunit]+36,
				YUNITCONSTR+xy[1][mouseonconstrunit]+19,
                        	2,IDFONT8,ss,GBLUECOLORFONT,tfontgamp,gamedlggrp);
	    }

	}//if mouseonconstrunit

	if (mouseontranspunit!=-1)
	{
	    b = loadobj(transportplace[mouseontranspunit]);
    	    tblname=getOBJname(transportplace[mouseontranspunit]);
//	    strcat(ss,tblname);
    	    sprintf(ss,alldattbl.stattxt_tbl->get_TBL_STR(STATTXT_TBL_UNLOAD_UNIT),tblname);
	    putboxmessage(  FONTCOLOR(tfontgamp,GBLUECOLORFONT,2),
                                FONTCOLOR(tfontgamp,GBLUECOLORFONT,4),
                                XWINPOS+104+mouseontranspunit/2*36+36,
				YWINPOS+10+(mouseontranspunit&1)*36+18,
                                2,IDFONT8,ss,GBLUECOLORFONT,tfontgamp,gamedlggrp);
	}//if mouseonconstrunit
    }//else objcount
    else//objcount!=1
    {
	drawallunitsinbar(XWINPOS,YWINPOS,fordeselect,selectobjcount());
    }
}
//==============================================
//find from modemove what line need to be desened
void FindLineOfProp(struct OBJ *a,struct OBJstruct *b)
{
    if (a->modemove == MODESTOP || a->prop & VARHOLDPOSBIT)
    {
	    if (a->prop & VARREADY)
	    {
		if ( IsOnSkyOBJ(a) && (IsBuild(a->SC_Unit)) )
		    ChangeTypeOfProp(a,b,PROPONAIR);
		else
		    if (IsOBJBurrowed(a)||
			    (GetMageAtr(&a->atrobj,ATRINVISIBLE)>0&&
			    (ExistProp(b,MODECLOAKFIELD)||ExistProp(b,MODEPERSONNELCLOAK))))
			ChangeTypeOfProp(a,b,PROPNORMAL2);
		    else
			if (a->typeofprop!=PROPBUILDSIMPLE&&
			    a->typeofprop!=PROPBUILDADVANCED)
			    ChangeTypeOfProp(a,b,PROPNORMAL1);
	    }
	    else
		ChangeTypeOfProp(a,b,PROPDECONSTRUCT);
    }
}
//=====================================
void drawunitinbar( int x,int y,int weight,int type,int ramka,
		    int shield_pers,int life_pers,
		    GRPFILE *grp1,GRPFILE *grp2,int iconnr2)
{
    int iconnr,life,shield;
    if (ramka)
    {
	iconnr = icontype[weight-1]+type*6+1;
	putgrpspr(x,y,grp1,NORMAL,255,0,NULL,iconnr);
    }
    life = LIFE1+life_pers*(LIFEMAXPOS-1)/100;
    shield = shield_pers*(SHIELDMAXPOS-1)/100;
    SetPlayerColors(life,1,0xc0,2,&shield_pos[shield]);
    if (weight==4)
	putgrpspr(x+3,y+3,grp2,NORMAL,255,life,NULL,iconnr2);
    else
	putgrpspr(x,y,grp2,NORMAL,255,life,NULL,iconnr2);
}
//=====================================
void drawunitsintransport(int XWINPOS,int YWINPOS,struct OBJ *a)
{
    int xpos=XWINPOS+104,ypos=YWINPOS+14;
    int xsize,ysize,oldxsize;
    int addx=0,addy=0,shield_pers,life_pers;
    int weightobj,i,pos=0,possize;
    int objtype;//objtype=0-darkblue,1-white,2-green,3-lightblue
    struct OBJ *a1;
    for (i=0;i<a->loaded->nrofloadedunits;i++)
    {
	a1 = a->loaded->loadedunits[i];
	weightobj = GetSpaceRequired(a1->SC_Unit);
	switch(weightobj)
	{
    	    case 4:
		xsize = 72;
		ysize = 72;
		possize = 4;
		break;
	    case 2:
		xsize = 36;
		ysize = 72;
		possize = 2;
		break;
	    case 1:
		xsize = 36;
		ysize = 36;
		possize = 1;
		break;
	}
	if (ypos+ysize>YUNITTRANSP+72)
	{
	    xpos += oldxsize;
	    ypos = YWINPOS+14;
	}
    	if (!IsShieldEnable(a1->SC_Unit))
	{
	    shield_pers = 0;
	}
	else
	{
	    shield_pers = (int) (a1->shield*100/GetUnitMaxShield(a1->SC_Unit));
	}
	pos = ((xpos-(XWINPOS+104))/36)*2;
	if ((ypos-(YWINPOS+14))/36)
	    pos++;
	if (oldmouseontranspunit>=pos&&
	    oldmouseontranspunit<pos+possize)
	{
	    addx=2;
	    addy=2;
	}
	else
	{
	    addx=0;
	    addy=0;
	}
	if (IsHeroUnit(a1->SC_Unit))
	    objtype=1;
	else
	    if (GetMageAtr(&a1->atrobj,ATRHALLUCINATION)==0)
		if (GetMageAtr(&a1->atrobj,ATRBLIND)==0)
		    if (GetMageAtr(&a1->atrobj,ATRPARASITEFROM)==0)
			objtype = 0;
		    else
			objtype = 2;
		else
		    objtype = 1;
	    else
		objtype = 3;
	life_pers = (int) (a1->health*100/GetUnitMaxHealth(a1->SC_Unit));
	drawunitinbar(xpos+addx,ypos+addy-1,weightobj,objtype,WITHRAMKA,
		      shield_pers,life_pers,
		      grpicons,grpwire2,a1->SC_Unit);
	ypos += ysize;
	oldxsize = xsize;
    }
}
//=====================================
void putnameonselectedunits(struct OBJ *a[],int count,int XWINPOS,int YWINPOS)
{
    char *tblname;
    int xpos,ypos,xsize,ysize,i,oldxsize;
    struct OBJ *a1;
    struct XY uu;
    if (patrate)
	return;
    xpos=XWINPOS+30;
    ypos=YWINPOS+14;
    for (i=0;i<MAXSELECTMAN;i++)
    {
    	    a1 = a[i];
    	    if (!a1)
		continue;
	    xsize = 36;
	    ysize = 36;
	    if (ypos+ysize>YWINPOS+14+36*2)
	    {
		xpos += oldxsize;
		ypos = YWINPOS+14;
	    }
	    if (mousehotpos==i+MOUSEONSTATUNIT)
	    {
    		tblname=alldattbl.stattxt_tbl->get_TBL_STR(STATTXT_TBL_HELPSELECTUNITS);
            	putboxmessage(  FONTCOLOR(tfontgamp,GBLUECOLORFONT,2),
                        	FONTCOLOR(tfontgamp,GBLUECOLORFONT,4),
                            	xpos,ypos,2,IDFONT8,tblname,GBLUECOLORFONT,tfontgamp,gamedlggrp);
	    }
	    ypos += ysize;
	    oldxsize = xsize;
    }
}
//=====================================
void drawallunitsinbar(int XWINPOS,int YWINPOS,struct OBJ *a[],int count)
{
    static int selectunit=-1;
    int addx,addy;
//    int xpos=XUNITBAR,ypos=YUNITBAR;
    int xpos=XWINPOS+30,ypos=YWINPOS+14;
    int xsize,ysize,oldxsize;

    int i,shield_pers,life_pers,mbuttonpress,shiftpress,ctrlpress;
    int objtype;//objtype=0-darkblue,1-white,2-green,3-lightblue
    struct OBJ *a1;
    if (count)
    {
	mbuttonpress=mouse_b&WMLEFTKEY;
	shiftpress=KEYPRESS(SHIFTLKEY)|KEYPRESS(SHIFTRKEY);
	ctrlpress=KEYPRESS(CTRLLKEY)|KEYPRESS(CTRLRKEY);
	for (i=0;i<MAXSELECTMAN;i++)
        {
    	    a1 = a[i];
    	    if (!a1)
		continue;
	    xsize = 36;
	    ysize = 36;
	    if (ypos+ysize>YWINPOS+14+36*2)
	    {
		xpos += oldxsize;
		ypos = YWINPOS+14;
	    }
    	    if (!IsShieldEnable(a1->SC_Unit))
	    {
		shield_pers = 0;
	    }
	    else
	    {
		shield_pers = (int) (a1->shield*100/GetUnitMaxShield(a1->SC_Unit));
	    }
	    if (IsHeroUnit(a1->SC_Unit))
		objtype=1;
	    else
	        if (GetMageAtr(&a1->atrobj,ATRHALLUCINATION)==0)
		    if (GetMageAtr(&a1->atrobj,ATRBLIND)==0)
			if (GetMageAtr(&a1->atrobj,ATRPARASITEFROM)==0)
			    objtype = 0;
			else
			    objtype = 2;
		    else
			objtype = 1;
		else
		    objtype = 3;
	    life_pers = (int) (a1->health*100/GetUnitMaxHealth(a1->SC_Unit));
	    if (mbuttonpress&&mousehotpos-MOUSEONSTATUNIT==i)
	    {
		addx=2;
		addy=2;
		selectunit=i;
	    }
	    else
	    {
		addx=0;
		addy=0;
	    }
	    if (!mbuttonpress)
		if (selectunit!=-1)
		{
		    if (!shiftpress)
			if (ctrlpress)
			    deselectallexcludeonetypeobj(fordeselect[selectunit]);
			else
			    deselectallexcludeone(fordeselect[selectunit]);
		    else
			deselectobj(fordeselect[selectunit]);
		    selectunit=-1;
		    break;
		}
	    drawunitinbar(xpos+addx,ypos+addy,1,objtype,WITHRAMKA,
			shield_pers,life_pers,grpicons,grpwire1,a1->SC_Unit);
	    ypos += ysize;
	    oldxsize = xsize;
	    putnameonselectedunits(a,count,XWINPOS,YWINPOS);
	}
    }
}
//=====================================
//0246
//1357
//=====================================
void AnalizeAllTransportUnits(struct OBJ *a,unsigned char *retarray,struct OBJ **trobj)
{
    int pos=0,i,j,weightobj;
    struct OBJ *a1;
    for (i=0;i<8;i++)
	retarray[i]=SC_NOUNITNR;
    *trobj=NULL;
    if (a)
    if (a->loaded)
    {
	*trobj=a;
	for (i=0;i<a->loaded->nrofloadedunits;i++)
	{
	    a1 = a->loaded->loadedunits[i];
	    weightobj = GetSpaceRequired(a1->SC_Unit);
	    switch(weightobj)
	    {
		case 4:
		case 2:
		    if (pos&1)
			pos++;
		    break;
		case 1:
		    break;
	    }
	    for (j=0;j<weightobj;j++)
		retarray[j+pos] = a1->SC_Unit;
	    pos+=weightobj;
	}
    }
}
//=====================================
//show the place were we can put build (geyser,...)
//=====================================
void putbuildneededtodraw(struct OBJ *a)
{
    int deltax,deltay;
    int xb,yb,xe,ye,i,myobj=0;
    unsigned char motherbuild=SC_NOUNITNR;
    unsigned char bf;
    switch(buildconstr)
    {
	case SC_EXTRACTOROBJ:
	case SC_ASSIMILATOROBJ:
	case SC_REFINERYOBJ:
	    if (mapOPEN(a,NUMBGAMER)<=1 || IsOnSkyOBJ(a))
		return;
	    if (a->SC_Unit == SC_GEYSEROBJ && !(a->replaceobj))
	    {
		xb = GetOBJx(a) - GetUnitWidthAndHeight(buildconstr,UNITDIM_WIDTH)/2-map.MAPXGLOBAL;
		yb = GetOBJy(a) - GetUnitWidthAndHeight(buildconstr,UNITDIM_HEIGHT)/2-map.MAPYGLOBAL;
		
		xe = xb + GetUnitWidthAndHeight(buildconstr,UNITDIM_WIDTH);
		ye = yb + GetUnitWidthAndHeight(buildconstr,UNITDIM_HEIGHT);
		wrectangle(YELLOWCOLOR,xb-1,yb-1,xe,ye);
		DrawRawMainImageOBJ((xb + xe) / 2,
				    (yb + ye) / 2,
				    buildconstr,PLAYER[NUMBGAMER].colorRACE,TRANSPARENT,MAXINVISIBLECOLOR);
	    }
	    break;
	case SC_COMSATSTATIONOBJ:
	case SC_NUCLEARSILOOBJ:
	case SC_CONTROLTOWEROBJ:
	case SC_COVERTOPSOBJ:
	case SC_PHYSICSLABOBJ:
	case SC_MACHINESHOPOBJ:
	    if (mapOPEN(a,NUMBGAMER)<=1 || IsOnSkyOBJ(a))
		return;
    	    motherbuild = GetMotherAddon(buildconstr);
	    if (a->SC_Unit == motherbuild && !a->addonobj && !IsOBJUnderConstruct(a)&&(a->playernr==NUMBGAMER)&&!(a->prop&VARNOTWORK))
	    {
    		AttachAddonDeltaCoords(motherbuild,buildconstr,&deltax,&deltay);
		
		xb = GetOBJx(a) + deltax - GetUnitWidthAndHeight(buildconstr,UNITDIM_WIDTH)/2-map.MAPXGLOBAL;
		yb = GetOBJy(a) + deltay - GetUnitWidthAndHeight(buildconstr,UNITDIM_HEIGHT)/2-map.MAPYGLOBAL;
		
		xe = xb + GetUnitWidthAndHeight(buildconstr,UNITDIM_WIDTH);
		ye = yb + GetUnitWidthAndHeight(buildconstr,UNITDIM_HEIGHT);
		
		wrectangle(YELLOWCOLOR,xb-1,yb-1,xe,ye);

		DrawRawMainImageOBJ((xb + xe) / 2,
				    (yb + ye) / 2,
				    buildconstr,PLAYER[NUMBGAMER].colorRACE,TRANSPARENT,MAXINVISIBLECOLOR);
	    }
	    break;
	case SC_COMMCENTEROBJ:
	case SC_FACTORYOBJ:
	case SC_STARPORTOBJ:
	case SC_SCIENCEFACILITYOBJ:
    	    motherbuild = GetMotherAddon(a->SC_Unit);
    	    if (motherbuild != buildconstr)
    		return;
	    if (mapOPEN(a,NUMBGAMER)<=1 || IsOnSkyOBJ(a))
		return;
	    if (!a->addonobj && !IsOBJUnderConstruct(a))
	    {
    		AttachAddonDeltaCoords(motherbuild,a->SC_Unit,&deltax,&deltay);
		
		xb = GetOBJx(a) - deltax - GetUnitWidthAndHeight(buildconstr,UNITDIM_WIDTH)/2-map.MAPXGLOBAL;
		yb = GetOBJy(a) - deltay - GetUnitWidthAndHeight(buildconstr,UNITDIM_HEIGHT)/2-map.MAPYGLOBAL;
		
		xe = xb + GetUnitWidthAndHeight(buildconstr,UNITDIM_WIDTH);
		ye = yb + GetUnitWidthAndHeight(buildconstr,UNITDIM_HEIGHT);
		
		wrectangle(YELLOWCOLOR,xb-1,yb-1,xe,ye);

		DrawRawMainImageOBJ((xb + xe) / 2,
				    (yb + ye) / 2,
				    buildconstr,PLAYER[NUMBGAMER].colorRACE,TRANSPARENT,MAXINVISIBLECOLOR);
	    }
	    break;
    }
}
//===================================================
//colors in palette
#define ENABLECONSTRUCT  158
#define DISABLECONSTRUCT 111
//=============================================
//put picture of build if we have to construct build and click on icon of
//probe drone or SCV
//return posibleconstruct
// 0 - noconstr
// >0- construct
// <0- error on constr
//
int  desenbuildifconstr(void)
{
    unsigned char motherbuild=SC_NOUNITNR;
    int deltax,deltay;
    unsigned short returnedarray[MAXBUILDSPRSIZE*MAXBUILDSPRSIZE];	//array of x,y sizez 1-noposibleconstruct
				//		     0-posibleconstruct
    int posibleconstr=0,posibleconstr2=0,constrerror=0;
    int x0,y0,x,y,xkk,ykk,i,j;
    int xk,yk;
    int xmap,ymap;
    xk=map.MAPXGLOBAL;
    yk=map.MAPYGLOBAL;
    struct XY a;
    struct OBJstruct *b,*b2;
    int posibletry;
    if (buildconstr==SC_NOUNITNR)
	return(posibleconstr);
    if (IsBuild(buildconstr))
    {
	posibleconstr=1;
	b=loadobj(buildconstr);
        x=POSX(GetUnitWidthAndHeight(buildconstr,UNITDIM_WIDTH));
        y=POSY(GetUnitWidthAndHeight(buildconstr,UNITDIM_HEIGHT));
        a.x1=(mouse_x/SIZESPRLANSHX)*SIZESPRLANSHX-xk%SIZESPRLANSHX;
        a.y1=(mouse_y/SIZESPRLANSHY)*SIZESPRLANSHY-yk%SIZESPRLANSHY;
        xpointconstr = a.x1+SIZESPRLANSHX*x/2;
        ypointconstr = a.y1+SIZESPRLANSHY*y/2;
	if (IsAddon(buildconstr))
	{
    	    motherbuild = GetMotherAddon(buildconstr);
            //put pred-addon build picture
    	    AttachAddonDeltaCoords(motherbuild,buildconstr,&deltax,&deltay);
    	    DrawRawMainImageOBJ(xpointconstr - deltax,ypointconstr - deltay, motherbuild, PLAYER[NUMBGAMER].colorRACE,NORMAL,MININVISIBLECOLOR);
        }
    	DrawRawMainImageOBJ(xpointconstr,ypointconstr,buildconstr, PLAYER[NUMBGAMER].colorRACE,NORMAL,MININVISIBLECOLOR);
        //put build picture
    	x0=(mouse_x/SIZESPRLANSHX)*SIZESPRLANSHX-xk%SIZESPRLANSHX;
    	y0=(mouse_y/SIZESPRLANSHY)*SIZESPRLANSHY-yk%SIZESPRLANSHX;
	xmap=(x0+xk)/SIZESPRLANSHX;
	ymap=(y0+yk)/SIZESPRLANSHY;
    	if (mapOPEN(xmap,ymap)<=1)//check if open this part of map
    	{
	    memset(returnedarray,0,sizeof(returnedarray));
	    posibletry = 0;
	    posibleconstr=TOBECONSTRUCT_NOTOPENMAP;
    	}
    	else
    	{
    	    if (mapSEE(xmap,ymap)<=1)//check if see this part of map
    	    {
		memset(returnedarray,0,sizeof(returnedarray));
		posibletry = 0;
		posibleconstr=TOBECONSTRUCT_NOTSEEMAP;
    	    }
    	    else
    	    {
    		posibletry=CanBuildOnPlace((x0+xk)/32,(y0+yk)/32,x,y,returnedarray,NULL,buildconstr,NUMBGAMER,&constrerror);
		posibleconstr &= posibletry;
		if (posibleconstr==0)
		    posibleconstr=constrerror;
	    }
        }
        for (i=0;i<y;i++)
        {
    	    a.y1=y0+i*SIZESPRLANSHY;
    	    a.y2=a.y1+SIZESPRLANSHY-1;
            ykk=(a.y1+yk)/SIZESPRLANSHY;
            for (j=0;j<x;j++)
            {
        	a.x1=x0+j*SIZESPRLANSHX;
                a.x2=a.x1+SIZESPRLANSHX-1;
                xkk=(a.x1+xk)/SIZESPRLANSHX;
        	if (returnedarray[i*x+j]==0xffff)
        	    wsetimage8x(ENABLECONSTRUCT,&a);
        	else
            	    wsetimage8x(DISABLECONSTRUCT,&a);
            }
	}
	if (posibleconstr<0)
	    posibleconstr2=posibleconstr;
        if (motherbuild != SC_NOUNITNR)
        {
    	    //put masterbuild
    	    x=4;
    	    y=3;
    	    x0=(mouse_x/SIZESPRLANSHX)*SIZESPRLANSHX-xk%SIZESPRLANSHX-SIZESPRLANSHX*4;
	    y0=(mouse_y/SIZESPRLANSHY)*SIZESPRLANSHY-yk%SIZESPRLANSHY-SIZESPRLANSHY*1;
	    posibletry = CanBuildOnPlaceWithAddon((x0+xk)/32,(y0+yk)/32,
				GetOBJx(cbuild)/32-x/2,GetOBJy(cbuild)/32-y/2,
				x,y,returnedarray,motherbuild,NUMBGAMER,&constrerror);
	    posibleconstr &= posibletry;
	    if (posibleconstr==0)
		posibleconstr=constrerror;
    	    for (i=0;i<y;i++)
    	    {
    		a.y1=y0+i*SIZESPRLANSHY;
    		a.y2=a.y1+SIZESPRLANSHY-1;
            	ykk=(a.y1+yk)/SIZESPRLANSHY;
        	for (j=0;j<x;j++)
        	{
        	    a.x1=x0+j*SIZESPRLANSHX;
            	    a.x2=a.x1+SIZESPRLANSHX-1;
            	    xkk=(a.x1+xk)/SIZESPRLANSHX;
        	    if (returnedarray[i*x+j]==0xffff)
        		wsetimage8x(ENABLECONSTRUCT,&a);
        	    else
            		wsetimage8x(DISABLECONSTRUCT,&a);
        	}//forj
	    }//fori
    	}//if masterbuild
    }//if buildconstruct
    else
    {
	posibleconstr=1;
        xpointconstr = mouse_x;
        ypointconstr = mouse_y;
    }
    if (posibleconstr2<0)
	return(posibleconstr2);
    return(posibleconstr);
}
//=============================================
#define INCREMENTVALUE (10<<8)
int showgoods_drawminerals,showgoods_drawgas;
int showgoods_drawpsi[4],showgoods_drawmaxpsi[4];
//=============================================
void clearshowgoods(void)
{
    showgoods_drawminerals=0;
    showgoods_drawgas=0;
    showgoods_drawpsi[0]=0;
    showgoods_drawpsi[1]=0;
    showgoods_drawpsi[2]=0;
    showgoods_drawpsi[3]=0;
    showgoods_drawmaxpsi[0]=0;
    showgoods_drawmaxpsi[1]=0;
    showgoods_drawmaxpsi[2]=0;
    showgoods_drawmaxpsi[3]=0;
}
//=============================================
void showgoods(void)
{
    int offspsi,i,decrxcoord=0,race,racepos=1;
    int drawrace[4],drawcolor[3];
    static char minerals[20],gas[20],psi[40];
    if (showgoods_drawminerals<PLAYER[NUMBGAMER].minerals)
    {
	showgoods_drawminerals+=INCREMENTVALUE;
	if (showgoods_drawminerals>PLAYER[NUMBGAMER].minerals)
	    showgoods_drawminerals=PLAYER[NUMBGAMER].minerals;
    }
    else
        if (showgoods_drawminerals>PLAYER[NUMBGAMER].minerals)
	{
	    showgoods_drawminerals-=INCREMENTVALUE;
	    if (showgoods_drawminerals<PLAYER[NUMBGAMER].minerals)
		showgoods_drawminerals=PLAYER[NUMBGAMER].minerals;
	}
    sprintf(minerals,"%c%c %d",COMMANDSYMB,MINERALSYMB,showgoods_drawminerals>>8);
    if (showgoods_drawgas<PLAYER[NUMBGAMER].gas)
    {
	showgoods_drawgas+=INCREMENTVALUE;
	if (showgoods_drawgas>PLAYER[NUMBGAMER].gas)
	    showgoods_drawgas=PLAYER[NUMBGAMER].gas;
    }
    else
	if (showgoods_drawgas>PLAYER[NUMBGAMER].gas)
	{
	    showgoods_drawgas-=INCREMENTVALUE;
	    if (showgoods_drawgas<PLAYER[NUMBGAMER].gas)
		showgoods_drawgas=PLAYER[NUMBGAMER].gas;
	}
    sprintf(gas,"%c%c %d",COMMANDSYMB,GASSYMB,showgoods_drawgas>>8);

    race=gameconf.pl_race[NUMBGAMER];
    if (race >= MAXRACES)
	race=0;
    showgoods_drawmaxpsi[0]=PLAYER[NUMBGAMER].maxcurentpsi[race];
    if (showgoods_drawmaxpsi[0]>MAXPSI)
        showgoods_drawmaxpsi[0]=MAXPSI;    
    showgoods_drawpsi[0]=PLAYER[NUMBGAMER].curentpsi[race];
    drawrace[0]=race;
    for (race=0;race<MAXRACES;race++)
    {
	if (race!=gameconf.pl_race[NUMBGAMER])
	{
	    showgoods_drawmaxpsi[racepos]=PLAYER[NUMBGAMER].maxcurentpsi[race];
	    if (showgoods_drawmaxpsi[racepos]>MAXPSI)
	        showgoods_drawmaxpsi[racepos]=MAXPSI;    
	    showgoods_drawpsi[racepos]=PLAYER[NUMBGAMER].curentpsi[race];
	    drawrace[racepos]=race;
	    racepos++;
	}
	if (showgoods_drawpsi[race]/2 > showgoods_drawmaxpsi[race]/2 && !CODEFORPSIUNLIMIT)
	    drawcolor[race]=GREDCOLORFONT;
	else
	    drawcolor[race]=GGREENCOLORFONT;
    }
    offspsi=0;
    psi[0]=0;
    for (i=0;i<3;i++,offspsi=strlen(psi))
    {
	if (showgoods_drawmaxpsi[i])//+showgoods_drawpsi[i])
	{
	    sprintf(psi+offspsi,"%c%c%c%d\x07/%d ",COMMANDSYMB,PSISYMBZ+drawrace[i],drawcolor[i],showgoods_drawpsi[i]/2,showgoods_drawmaxpsi[i]/2);
//	    sprintf(psi+offspsi,"%c%c%c%d\x07/%d ",COMMANDSYMB,PSISYMBZ+drawrace[i],drawcolor[i],showgoods_drawpsi[i],showgoods_drawmaxpsi[i]);
	    decrxcoord-=60;
	}
    }
    decrxcoord+=60;
/*    
    if (showgoods_drawmaxpsi[1]+showgoods_drawpsi[1])
    {
	    if (showgoods_drawmaxpsi[2]+showgoods_drawpsi[2])
	    {
		//put 3 psies
		sprintf(psi,"%c%c%c%d\x07/%d %c%c%c%d\x07/%d %c%c%c%d\x07/%d",
			    COMMANDSYMB,PSISYMBZ+drawrace[0],drawcolor[0],showgoods_drawpsi[0]/2,showgoods_drawmaxpsi[0]/2,
			    COMMANDSYMB,PSISYMBZ+drawrace[1],drawcolor[1],showgoods_drawpsi[1]/2,showgoods_drawmaxpsi[1]/2,
			    COMMANDSYMB,PSISYMBZ+drawrace[2],drawcolor[2],showgoods_drawpsi[2]/2,showgoods_drawmaxpsi[2]/2);
    		decrxcoord=120;
	    }
	    else
	    {
		//put 2 psies
		sprintf(psi,"%c%c%c%d\x07/%d %c%c%c%d\x07/%d",
			COMMANDSYMB,PSISYMBZ+drawrace[0],drawcolor[0],showgoods_drawpsi[0]/2,showgoods_drawmaxpsi[0]/2,
			COMMANDSYMB,PSISYMBZ+drawrace[1],drawcolor[1],showgoods_drawpsi[1]/2,showgoods_drawmaxpsi[1]/2);
    		decrxcoord=60;
	    }
    }
    else
    {
	if (showgoods_drawmaxpsi[2]+showgoods_drawpsi[2])
	{
	    //put 2 psies
	    sprintf(psi,"%c%c%c%d\x07/%d %c%c%c%d\x07/%d",
			COMMANDSYMB,PSISYMBZ+drawrace[0],drawcolor[0],showgoods_drawpsi[0]/2,showgoods_drawmaxpsi[0]/2,
			COMMANDSYMB,PSISYMBZ+drawrace[2],drawcolor[2],showgoods_drawpsi[2]/2,showgoods_drawmaxpsi[2]/2);
    	    decrxcoord=60;
	}
	else
	{
        //put one psi
	    sprintf(psi,"%c%c%c%d\x07/%d",COMMANDSYMB,PSISYMBZ+drawrace[0],drawcolor[0],showgoods_drawpsi[0]/2,showgoods_drawmaxpsi[0]/2);
	    decrxcoord=0;
	}
    }    
*/
    putmessage(460+decrxcoord,15,IDFONT12,minerals,GGREENCOLORFONT,tfontgamp,gamedlggrp);
    putmessage(520+decrxcoord,15,IDFONT12,gas,GGREENCOLORFONT,tfontgamp,gamedlggrp);
    putmessage(575+decrxcoord,15,IDFONT12,psi,GGREENCOLORFONT,tfontgamp,gamedlggrp);
}
//=============================
void showadvisortext(int stat_txt_nr)
{
    if (stat_txt_nr!=-1)
	if (gameconf.audioconf.subtitles)
	    infobar.addbarmessage(alldattbl.stattxt_tbl->get_TBL_STR(stat_txt_nr),
				    IDFONT10,GWHITECOLORFONT,INFO_DEFAULTTIME,BF_MALIGN);
}
//====================================================
short constrbuildstate[9]=
{
    //0,TOBECONSTRUCT_NOTOPENMAP,TOBECONSTRUCT_NOTSEEMAP,TOBECONSTRUCT_NOTGEYSER
    //TOBECONSTRUCT_NEARRESOURCE,TOBECONSTRUCT_WRONGPLACE,TOBECONSTRUCT_NOTCREEP
    //TOBECONSTRUCT_NOTPYLONAREA,TOBELAND_CANTLANDHERE
    0,856,857,858,851,855,859,860,862
};
//====================================================
void putbuildplacemessage(int errorstate)
{
    if (errorstate>0)
	if (gameconf.audioconf.subtitles)
    	    infobar.addbarmessage(alldattbl.stattxt_tbl->get_TBL_STR(constrbuildstate[errorstate]),
    				    IDFONT10,GWHITECOLORFONT,INFO_DEFAULTTIME,BF_MALIGN);
}
//====================================================
void DrawBuildPlace(void)
{
    int i;
    OBJ *a;
    if (buildconstr)
    {
	for (i=0;i<MaxObjects;i++)
	{
	    a = objects[i];
	    if (IsBuild(a->SC_Unit))
		putbuildneededtodraw(a);
	}
    }
}

