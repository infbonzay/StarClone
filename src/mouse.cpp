#include <grplib/usegrp.h>
#include <grplib/gr8.h>

#include "const.h"
#include "auxil.h"
#include "audio.h"
#include "gr.h"
#include "key.h"
#include "vars.h"
#include "load.h"
#include "objs.h"
#include "objinfo.h"
#include "mpq.h"
#include "maps.h"
#include "lists.h"
#include "putobj.h"
#include "walk.h"
#include "stringfiles.h"
#include "mouse.h"

#ifdef WITHSDL
#	include "sdl/grsdl.h"
#	include "sdl/mousesdl.h"
#	include "sdl/keysdl.h"
#endif

#ifdef UNDERDOS
#	include "dos/handlers.h"
#endif

int 	grm,factorscrollkey,mousehotpos;
int     mouse_x,mouse_y,mousemaxx,mousemaxy,mouse_b;
int     xmloc,ymloc,movieminikarta=NO,mousedoubleclick;
struct  XY mouser[MAXMOUSEMODE];
int     patrate;
int     decrxx,decryy,mouseprevx,mouseprevy;
char    select_aria,karta_aria,mode_aria;
char    waitforleftbuton,waitfordownrightbuton,waitfordownleftbuton;
char    mouse,patr,mouseclear,mloc;
unsigned short  mouseprop;
char     timemouse;
struct OBJ *MouseOnOBJS[8];	//myunit,alianceunit,neutralunit,enemyunit,mybuild,aliancebuild,neutralbuild,enemybuild
struct OBJ *DestMouseOBJ;
char	DestMouseType,prevmouseobjtype;

char    curentREGIM;
int memmouseposx,memmouseposy;
int memmousepossizex,memmousepossizey;

//==============================
int restrictmousecoords(int REGIM)
{
    int mx,my;
    mx = mouse_x;
    my = mouse_y;
//    if (curentREGIM==REGIM)
//	return 1;
    curentREGIM=REGIM;
//    if (mousedetect)
    if (0)
    {
	lowMouse.GetPos();
	mouse_x += lowMouse.DeltaX;
        mouse_y += lowMouse.DeltaY;
        if (mouse_x>mouser[REGIM].x2)
    	    mouse_x=mouser[REGIM].x2;
        else
    	    if (mouse_x<mouser[REGIM].x1)
		mouse_x=mouser[REGIM].x1;
	if (mouse_y>mouser[REGIM].y2)
	    mouse_y=mouser[REGIM].y2;
        else
            if (mouse_y<mouser[REGIM].y1)
		mouse_y=mouser[REGIM].y1;
    }
    if ((mouse_x!=mx)||(mouse_y!=my))
	return 1;
    return 0;

}

//==========================
int mouseborder2(int x1,int y1,int x2,int y2)
{
    if(mouse_x>=x1&&mouse_y>=y1&&mouse_x<=x2&&mouse_y<=y2)
	return(1);
    else
	return(0);
}
//==========================
int mouseborder(struct XY *m)
{
    return(mouseborder2(m->x1,m->y1,m->x2,m->y2));
}
//==========================
/*void desenlocation(void)
{
    if (mloc)
    {
	int type=DESTINATIONMOUSE;
	int deltax=-highMouse.cursors[type].mousegrp->SizeX/2;
	int deltay=-highMouse.cursors[type].mousegrp->SizeY/2;
	putgrpspr(xmloc+deltax-map.MAPXGLOBAL,ymloc+deltay-map.MAPYGLOBAL,highMouse.cursors[type].mousegrp,NORMAL,
              0,0,NULL,highMouse.cursors[type].curentposition);
    }
}
*/
//=================================
void putdestination(struct OBJ *destobj,int xm,int ym,int modemove,int posibleconstr,int rightclick)
{
    int race;
    if (PLAYER[NUMBGAMER].isobserverflag)
	return;
    if (!waitforleftbuton)
    {
	if (buildconstr != SC_NOUNITNR)
	{
	    if (posibleconstr>0)
	    {
		xm = xpointconstr+map.MAPXGLOBAL;
		ym = ypointconstr+map.MAPYGLOBAL;
		if (gameconf.audioconf.buildsounds)
		    if (IsBuild(buildconstr))
			if (fordeselect[0])
			{
			    race = GetUnitRace(fordeselect[0]->SC_Unit);
			    Play_sfxdata_id(fordeselect[0],sfx_buildplace[race],2,0);
			}
	    }
	    else
	    {
	    	//cancel construct if wrong location
		if (posibleconstr<0)
		{
errbuildonminimap:
		    if (gameconf.audioconf.buildsounds)
		    {
			if (fordeselect[0])
			{
			    if (IsWorkerUnit(fordeselect[0]->SC_Unit))
			    {
				activatesound(fordeselect[0],MODESOUNDERROR,2,NOSTOPCURRENTSOUNDS);
			    }
			    else
			    {
				if (modemove==MODELANDING)
				{
				    posibleconstr=TOBELAND_CANTLANDHERE;
				}
				Play_sfxdata_id(fordeselect[0],PUTLIFTDOWNERROR,2,0);
			    }
			}
		    }
		    putbuildplacemessage(-posibleconstr);
	    	    return;
	    	}
	    	else
	    	{
	    	    waitforleftbuton=1;
		    SetVisualMapPosition((int) (((mouse_x-Xkart-Xkartbeg)/factorx)-widthkart/2)*SIZESPRLANSHX,
					 (int) (((mouse_y-Ykart-Ykartbeg)/factory)-hightkart/2)*SIZESPRLANSHY);
	    	    return;
//	    	    if (IsBuild(buildconstr))
//	    		goto errbuildonminimap;
	    	}
	    }
	}
	if (selectedobjmove(destobj,xm,ym,modemove,NUMBGAMER,SHOWERRORTEXT,rightclick))
    	{
    	    //
    	    mloc=1;
    	    xmloc=xm;
            ymloc=ym;
	    SetBlinkOBJ(destobj);
        }
    }
}
//=================================
void nextmousespos(void)
{
    for (int i=0;i<TYPEOFCURSORS;i++)
    {
	highMouse.cursors[i].curentposition++;
	if (highMouse.cursors[i].curentposition>=highMouse.cursors[i].maxpositions)
	    highMouse.cursors[i].curentposition = 0;
    }
}
//=================================
float scrollmapx(int border,float factor)
{
    static float ticks=0;
//    if (highMouse.cursorsCROLLON)
	if (!border)
	{
	    if (ticks>0)
	    {
		ticks--;
		if (ticks<0)
		    ticks=0;
	    }
    	    else
		if (ticks<0)
		{
		    ticks++;
		    if (ticks>0)
			ticks=0;
		}
	}
        else
	{
	    ticks+=border;
	    if (ticks>MAXSCROLLTICKS)
		ticks=MAXSCROLLTICKS;
	    if (ticks<-MAXSCROLLTICKS)
		ticks=-MAXSCROLLTICKS;
	}
    return(ticks*factor);
}
//=================================
float scrollmapy(int border,float factor)
{
    static float ticks=0;
//    if (highMouse.cursorsCROLLON)
	if (!border)
	{
	    if (ticks>0)
	    {
		ticks--;
		if (ticks<0)
		    ticks=0;
	    }
	    else
		if (ticks<0)
		{
		    ticks++;
		    if (ticks>0)
			ticks=0;
		}
	}
	else
	{
	    ticks+=border;
	    if (ticks>MAXSCROLLTICKS)
		ticks=MAXSCROLLTICKS;
	    if (ticks<-MAXSCROLLTICKS)
		ticks=-MAXSCROLLTICKS;
	}
    return(ticks*factor);
}
//=================================
//=================================
void getmousetype(int xk,int yk)
{
    static int oldc=0;
    int c;
    highMouse.MouseType = NORMALMOUSE;
    if (MENUACTIVE)
	return;
    if (GAME)
    {
     c=0;
     if (!(mouse_b&WMLEFTKEY))
        waitfordownleftbuton=0;
     if (!waitfordownleftbuton)
     {
      if (!waitforleftbuton)
      {
	 buildconstr=SC_NOUNITNR;
         if ((mouse_b&WMLEFTKEY)&&(select_aria)&&!(karta_aria))
         {
             if (mouseprevy<gameconf.grmode.y-YDECICONS)
                patr=1;
             else
                patr=0;
         }
         if (!mouse_b)
         {
            mouseclear=0;
            if (patr)
            {
               if (KEYPRESS(SHIFTLKEY)||KEYPRESS(SHIFTRKEY))
                  selectMAN(mouseprevx,mouseprevy,mouse_x,mouse_y,1);
               else
                  selectMAN(mouseprevx,mouseprevy,mouse_x,mouse_y,0);
               patr=0;
            }
            mouseprevx=mouse_x;
            mouseprevy=mouse_y;
          }
     }
     else
     {
        if (waitfordownrightbuton&&(!(mouse_b&WMRIGHTKEY)))
        {
            waitfordownrightbuton=0;
            waitforleftbuton=0;
        }
        if (properties[8] == MODECANCEL_BB_AB && (mouse_b&WMRIGHTKEY))
        {
            waitfordownrightbuton=1;
        }
        if (waitforleftbuton==2)
           if (!(mouse_b&WMLEFTKEY))
              waitforleftbuton--;
        waitfordownleftbuton=1;
     }
    }
    if (waitforleftbuton==1&&select_aria)
    {
      if (mouse_b&WMLEFTKEY)
      {
        waitforleftbuton=0;
        if (karta_aria)
           putdestination(DestMouseOBJ,
                          (int)((mouse_x-Xkart-Xkartbeg)/factorx)*SIZESPRLANSHX,
                          (int)((mouse_y-Ykart-Ykartbeg)/factory)*SIZESPRLANSHY,
                          mouseprop,0,0);
        else
           putdestination(DestMouseOBJ,mouse_x+xk,mouse_y+yk,mouseprop,posibleconstruct,0);
      }
    }

     grm=0;
//     if (WMouseMoveRelativX&&WMouseMoveRelativY)
     {
        if ( mouse_x <= BORDERMOUSE )
	    c|=1;
        else
    	    if ( mouse_x>=mousemaxx - BORDERMOUSE )
		c|=2;
        if ( mouse_y <= BORDERMOUSE  )
    	    c|=4;
        else
    	    if (mouse_y>=mousemaxy- BORDERMOUSE )
		c|=8;
	if (MOUSESCROLLON)
        switch(c)
        {
    	    case 0:
		    if (!addscrx)
			addscrx = (int)scrollmapx(0,SMOUTHMOUSE);
		    if (!addscry)
			addscry = (int)scrollmapy(0,SMOUTHMOUSE);
                break;
    	    case 1:
                if (xk)
                {
                    grm=1;
		    highMouse.MouseType = MOUSELEFTSCROLL;
		    if (!addscrx)
			addscrx = (int)scrollmapx(-1,SMOUTHMOUSE);
		    if (!addscry)
			addscry = (int)scrollmapy(0,SMOUTHMOUSE);
                }
                else
		    highMouse.MouseType = NORMALMOUSE;
                break;
    	    case 2:
                if (xk<(MAXXMAP-widthkart)*SIZESPRLANSHX)
                {
                    grm=1;
		    highMouse.MouseType = MOUSERIGHTSCROLL;
		    if (!addscrx)
			addscrx = (int)scrollmapx(1,SMOUTHMOUSE);
		    if (!addscry)
			addscry = (int)scrollmapy(0,SMOUTHMOUSE);
                }
                else
		    highMouse.MouseType = NORMALMOUSE;
                break;
    	    case 4:
                if (yk)
                {
                    grm=1;
		    highMouse.MouseType = MOUSEUPSCROLL;
		    if (!addscrx)
			addscrx = (int)scrollmapx(0,SMOUTHMOUSE);
		    if (!addscry)
			addscry = (int)scrollmapy(-1,SMOUTHMOUSE);
                }
                else
		    highMouse.MouseType = NORMALMOUSE;
                break;
    	    case 5:
                if (xk||yk)
                {
                    grm=1;
		    highMouse.MouseType = MOUSELEFTUPSCROLL;
		    if (!addscrx)
			addscrx = (int)scrollmapx(-1,SMOUTHMOUSE);
		    if (!addscry)
			addscry = (int)scrollmapy(-1,SMOUTHMOUSE);
                }
                else
		    highMouse.MouseType = NORMALMOUSE;
                break;
    	    case 6:
                if (xk<(MAXXMAP-widthkart)*SIZESPRLANSHX||yk)
                {
                    grm=1;
		    highMouse.MouseType = MOUSERIGHTUPSCROLL;
		    if (!addscrx)
			addscrx = (int)scrollmapx(1,SMOUTHMOUSE);
		    if (!addscry)
			addscry = (int)scrollmapy(-1,SMOUTHMOUSE);
                }
                else
		    highMouse.MouseType = NORMALMOUSE;
                break;
    	    case 8:
                if (yk<(MAXYMAP-hightkart)*SIZESPRLANSHY)
                {
                    grm=1;
		    highMouse.MouseType = MOUSEDOWNSCROLL;
		    if (!addscrx)
			addscrx = (int)scrollmapx(0,SMOUTHMOUSE);
		    if (!addscry)
			addscry = (int)scrollmapy(1,SMOUTHMOUSE);
                }
                else
		    highMouse.MouseType = NORMALMOUSE;
                break;
    	    case 9:
                if (xk||yk<(MAXYMAP-hightkart)*SIZESPRLANSHY)
                {
                    grm=1;
		    highMouse.MouseType = MOUSELEFTDOWNSCROLL;
		    if (!addscrx)
			addscrx = (int)scrollmapx(-1,SMOUTHMOUSE);
		    if (!addscry)
			addscry = (int)scrollmapy(1,SMOUTHMOUSE);
                }
                else
		    highMouse.MouseType = NORMALMOUSE;
                break;
    	    case 0xa:
                if (xk<(MAXXMAP-widthkart)*SIZESPRLANSHX||
                    yk<(MAXYMAP-hightkart)*SIZESPRLANSHY)
                {
                    grm=1;
		    highMouse.MouseType = MOUSERIGHTDOWNSCROLL;
		    if (!addscrx)
			addscrx = (int)scrollmapx(1,SMOUTHMOUSE);
		    if (!addscry)
			addscry = (int)scrollmapy(1,SMOUTHMOUSE);
                }
                else
		    highMouse.MouseType = NORMALMOUSE;
                break;
    	}//if & switch
        oldc=c;
     }//if move events
    }//game
    if ((mouse_b==WMRIGHTKEY)&&(!mouseclear)&&select_aria&&GAME&&(!waitfordownrightbuton))
    {
	mouseclear=1;
	putdestination(DestMouseOBJ,mouse_x+xk,mouse_y+yk,MODEMOVE,0,1);
    }
    if (patr&&movieminikarta!=YES)
    {
      if (mouse_x!=mouseprevx||mouse_y!=mouseprevy)
      {
        patrate=1;
        addscrx=0;
        addscry=0;
        if (mouseprevx > gameconf.grmode.x)
	    mouseprevx = gameconf.grmode.x;
        if (mouseprevy > gameconf.grmode.y)
	    mouseprevy = gameconf.grmode.y;
      }
    }
    else
    {
	patrate=0;
        if (!GAME)
	    highMouse.MouseType = NORMALMOUSE;
        if (mouse_b == WMLEFTKEY)
        {
	    highMouse.MouseType = NORMALMOUSE;
        }
    }
}
//==========================
//static char *adrmouse;
/*void saveundermouse(void)
{
    __type=NORMALMOUSE;
    memmouseposx = mouse_x-highMouse.cursors[__type].mousegrp->SizeX/2;
    if (memmouseposx<0)
	memmouseposx=0;
    memmouseposy = mouse_y-highMouse.cursors[__type].mousegrp->SizeY/2;
    if (memmouseposy<0)
	memmouseposy=0;
    memmousepossizex = highMouse.cursors[__type].mousegrp->SizeX*3/2;
    if (memmouseposx+memmousepossizex>=GRP_wmaxx)
	memmousepossizex = GRP_wmaxx-memmouseposx+1;
    memmousepossizey = highMouse.cursors[__type].mousegrp->SizeY*3/2;
    if (memmouseposy+memmousepossizey>=GRP_wmaxy)
	memmousepossizey = GRP_wmaxy-memmouseposy+1;
    __adrmouse = (char *)wmalloc(memmousepossizex*memmousepossizey);
    CGetImage8(memmouseposx,memmouseposy,
	       memmousepossizex,memmousepossizey,__adrmouse);

}
//==========================
void loadundermouse(void)
{
    CPutImage8(memmouseposx,memmouseposy,
	       memmousepossizex,memmousepossizey,__adrmouse);
    wfree(__adrmouse);
}
//==========================
*/
static char pixundermouse[64*64];
void saveundermouse(void)
{
    memmouseposx = mouse_x-32;
    memmouseposy = mouse_y-32;
    if (memmouseposx<0)
	memmouseposx=0;
    if (memmouseposy<0)
	memmouseposy=0;
    memmousepossizex = 64;
    memmousepossizey = 64;
    if (memmouseposx+memmousepossizex>GRP_wmaxx)
	memmousepossizex = GRP_wmaxx-memmouseposx+1;
    if (memmouseposy+memmousepossizey>GRP_wmaxy)
	memmousepossizey = GRP_wmaxy-memmouseposy+1;
    CGetImage8(memmouseposx,memmouseposy,memmousepossizex,memmousepossizey,pixundermouse);
}
//==========================
void loadundermouse(void)
{
    CPutImage8(memmouseposx,memmouseposy,memmousepossizex,memmousepossizey,pixundermouse);
}
//==========================
void MouseOnObjClear(void)
{
    int i;
    for (i=0;i<MOUSEON_MAXVALUE;i++)
        MouseOnOBJS[i]=NULL;
    DestMouseOBJ=NULL;
    DestMouseType=-1;
    prevmouseobjtype=MOUSEON_MAXVALUE;
}
//=================================
void GetMouseOnObj(void)
{
    int i;
    for (i=0;i<MOUSEON_MAXVALUE;i++)
    {
        if (MouseOnOBJS[i])
        {
    	    DestMouseOBJ = MouseOnOBJS[i];
    	    DestMouseType = i;
    	    break;
    	}
    }
}
//==================================

