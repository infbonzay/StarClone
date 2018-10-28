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
#include "gener.h"
#include "mouse.h"

#ifdef WITHSDL
#	include "sdl/grsdl.h"
#	include "sdl/mousesdl.h"
#	include "sdl/keysdl.h"
#endif

#ifdef UNDERDOS
#	include "dos/handlers.h"
#endif

int 	factorscrollkey,mousehotpos;
int     mousemaxx,mousemaxy,mouse_b;
int     xmloc,ymloc,movieminikarta=NO,mousedoubleclick;
int     decrxx,decryy;
char    waitfordownrightbuton,waitfordownleftbuton;
char    patr,mouseclear,mloc;
unsigned short  mouseprop;
char     timemouse;

//==============================
void putdestination(struct OBJ *destobj,int xm,int ym,int modemove,int posibleconstr,int rightclick)
{
    int race;
    if (PLAYER[NUMBGAMER].isobserverflag)
	return;
    if (!highMouse->WaitToPressLeftButton)
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
	    	    highMouse->WaitToPressLeftButton=1;
		    SetVisualMapPosition((int) (((highMouse->PosX-Xkart-Xkartbeg)/factorx)-widthkart/2)*SIZESPRLANSHX,
					 (int) (((highMouse->PosY-Ykart-Ykartbeg)/factory)-hightkart/2)*SIZESPRLANSHY);
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

float scrollmapx(int border,float factor)
{
    static float ticks=0;
//    if (highMouse->cursorsCROLLON)
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
//    if (highMouse->cursorsCROLLON)
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
    highMouse->MouseType = NORMALMOUSE;
    if (MENUACTIVE)
	return;
    if (GAME)
    {
     c=0;
     if (!(mouse_b&WMLEFTKEY))
        waitfordownleftbuton=0;
     if (!waitfordownleftbuton)
     {
      if (!highMouse->WaitToPressLeftButton)
      {
	 buildconstr=SC_NOUNITNR;
         if ((mouse_b&WMLEFTKEY)&&(select_aria)&&!(karta_aria))
         {
             if (highMouse->PrevY<gameconf.grmode.y-YDECICONS)
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
                  selectMAN(highMouse->PrevX,highMouse->PrevY,highMouse->PosX,highMouse->PosY,1);
               else
                  selectMAN(highMouse->PrevX,highMouse->PrevY,highMouse->PosX,highMouse->PosY,0);
               patr=0;
            }
            highMouse->PrevX=highMouse->PosX;
            highMouse->PrevY=highMouse->PosY;
          }
     }
     else
     {
        if (waitfordownrightbuton&&(!(mouse_b&WMRIGHTKEY)))
        {
            waitfordownrightbuton=0;
            highMouse->WaitToPressLeftButton=0;
        }
        if (properties[8] == MODECANCEL_BB_AB && (mouse_b&WMRIGHTKEY))
        {
            waitfordownrightbuton=1;
        }
        if (highMouse->WaitToPressLeftButton==2)
           if (!(mouse_b & WMLEFTKEY))
              highMouse->WaitToPressLeftButton = false;
        waitfordownleftbuton=1;
     }
    }
    if (highMouse->WaitToPressLeftButton==1&&select_aria)
    {
      if (mouse_b&WMLEFTKEY)
      {
        highMouse->WaitToPressLeftButton=0;
        if (karta_aria)
           putdestination(highMouse->DestMouseOBJ,
                          (int)((highMouse->PosX-Xkart-Xkartbeg)/factorx)*SIZESPRLANSHX,
                          (int)((highMouse->PosY-Ykart-Ykartbeg)/factory)*SIZESPRLANSHY,
                          mouseprop,0,0);
        else
           putdestination(highMouse->DestMouseOBJ,highMouse->PosX+xk,highMouse->PosY+yk,mouseprop,posibleconstruct,0);
      }
    }

     highMouse->MouseOnBorder=0;
//     if (WMouseMoveRelativX&&WMouseMoveRelativY)
     {
        if ( highMouse->PosX <= BORDERMOUSE )
	    c|=1;
        else
    	    if ( highMouse->PosX>=mousemaxx - BORDERMOUSE )
		c|=2;
        if ( highMouse->PosY <= BORDERMOUSE  )
    	    c|=4;
        else
    	    if (highMouse->PosY>=mousemaxy- BORDERMOUSE )
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
                    highMouse->MouseOnBorder=1;
		    highMouse->MouseType = MOUSELEFTSCROLL;
		    if (!addscrx)
			addscrx = (int)scrollmapx(-1,SMOUTHMOUSE);
		    if (!addscry)
			addscry = (int)scrollmapy(0,SMOUTHMOUSE);
                }
                else
		    highMouse->MouseType = NORMALMOUSE;
                break;
    	    case 2:
                if (xk<(MAXXMAP-widthkart)*SIZESPRLANSHX)
                {
                    highMouse->MouseOnBorder=1;
		    highMouse->MouseType = MOUSERIGHTSCROLL;
		    if (!addscrx)
			addscrx = (int)scrollmapx(1,SMOUTHMOUSE);
		    if (!addscry)
			addscry = (int)scrollmapy(0,SMOUTHMOUSE);
                }
                else
		    highMouse->MouseType = NORMALMOUSE;
                break;
    	    case 4:
                if (yk)
                {
                    highMouse->MouseOnBorder=1;
		    highMouse->MouseType = MOUSEUPSCROLL;
		    if (!addscrx)
			addscrx = (int)scrollmapx(0,SMOUTHMOUSE);
		    if (!addscry)
			addscry = (int)scrollmapy(-1,SMOUTHMOUSE);
                }
                else
		    highMouse->MouseType = NORMALMOUSE;
                break;
    	    case 5:
                if (xk||yk)
                {
                    highMouse->MouseOnBorder=1;
		    highMouse->MouseType = MOUSELEFTUPSCROLL;
		    if (!addscrx)
			addscrx = (int)scrollmapx(-1,SMOUTHMOUSE);
		    if (!addscry)
			addscry = (int)scrollmapy(-1,SMOUTHMOUSE);
                }
                else
		    highMouse->MouseType = NORMALMOUSE;
                break;
    	    case 6:
                if (xk<(MAXXMAP-widthkart)*SIZESPRLANSHX||yk)
                {
                    highMouse->MouseOnBorder=1;
		    highMouse->MouseType = MOUSERIGHTUPSCROLL;
		    if (!addscrx)
			addscrx = (int)scrollmapx(1,SMOUTHMOUSE);
		    if (!addscry)
			addscry = (int)scrollmapy(-1,SMOUTHMOUSE);
                }
                else
		    highMouse->MouseType = NORMALMOUSE;
                break;
    	    case 8:
                if (yk<(MAXYMAP-hightkart)*SIZESPRLANSHY)
                {
                    highMouse->MouseOnBorder=1;
		    highMouse->MouseType = MOUSEDOWNSCROLL;
		    if (!addscrx)
			addscrx = (int)scrollmapx(0,SMOUTHMOUSE);
		    if (!addscry)
			addscry = (int)scrollmapy(1,SMOUTHMOUSE);
                }
                else
		    highMouse->MouseType = NORMALMOUSE;
                break;
    	    case 9:
                if (xk||yk<(MAXYMAP-hightkart)*SIZESPRLANSHY)
                {
                    highMouse->MouseOnBorder=1;
		    highMouse->MouseType = MOUSELEFTDOWNSCROLL;
		    if (!addscrx)
			addscrx = (int)scrollmapx(-1,SMOUTHMOUSE);
		    if (!addscry)
			addscry = (int)scrollmapy(1,SMOUTHMOUSE);
                }
                else
		    highMouse->MouseType = NORMALMOUSE;
                break;
    	    case 0xa:
                if (xk<(MAXXMAP-widthkart)*SIZESPRLANSHX||
                    yk<(MAXYMAP-hightkart)*SIZESPRLANSHY)
                {
                    highMouse->MouseOnBorder=1;
		    highMouse->MouseType = MOUSERIGHTDOWNSCROLL;
		    if (!addscrx)
			addscrx = (int)scrollmapx(1,SMOUTHMOUSE);
		    if (!addscry)
			addscry = (int)scrollmapy(1,SMOUTHMOUSE);
                }
                else
		    highMouse->MouseType = NORMALMOUSE;
                break;
    	}//if & switch
        oldc=c;
     }//if move events
    }//game
    if ((mouse_b==WMRIGHTKEY)&&(!mouseclear)&&select_aria&&GAME&&(!waitfordownrightbuton))
    {
	mouseclear=1;
	putdestination(highMouse->DestMouseOBJ,highMouse->PosX+xk,highMouse->PosY+yk,MODEMOVE,0,1);
    }
    if (patr&&movieminikarta!=YES)
    {
      if (highMouse->PosX != highMouse->PrevX || highMouse->PosY != highMouse->PrevY)
      {
        highMouse->MouseOnSelectionMode=1;
        addscrx=0;
        addscry=0;
        if (highMouse->PrevX > gameconf.grmode.x)
	    highMouse->PrevX = gameconf.grmode.x;
        if (highMouse->PrevY > gameconf.grmode.y)
	    highMouse->PrevY = gameconf.grmode.y;
      }
    }
    else
    {
	highMouse->MouseOnSelectionMode=0;
        if (!GAME)
	    highMouse->MouseType = NORMALMOUSE;
        if (mouse_b == WMLEFTKEY)
        {
	    highMouse->MouseType = NORMALMOUSE;
        }
    }
}



