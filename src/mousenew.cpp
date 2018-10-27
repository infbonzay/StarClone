#include <grplib/usegrp.h>
#include <grplib/gr8.h>

#include "defs.h"
#include "stringfiles.h"
#include "auxil.h"
#include "vars.h"
#include "objs.h"
#include "maps.h"
#include "putobj.h"
#include "audio.h"
#include "walk.h"
#include "lists.h"
#include "objinfo.h"
#include "gr.h"
#include "key.h"
#include "mousenew.h"

//==============================
HighMouse highMouse;
//==============================

int HighMouse::LoadOneCursor(char *filename,int typemouse)
{
    if (mpqloadfile(filename,(char **)&mouses[typemouse].mousegrp))
    {
		if (loadgrp(filename,&mouses[typemouse].mousegrp))
    	    return -1;
    	else
    	    mouses[typemouse].flag = 1;
    }
    else
        mouses[typemouse].flag = 2;

    mouses[typemouse].curentposition = 0;
    mouses[typemouse].maxpositions = mouses[typemouse].mousegrp->CountPictures;
    return 0;
}
//==============================
int HighMouse::LoadAllCursors(void)
{
    int i;
    for (i=0;i<TYPEOFCURSORS-1;i++)
    {
		if (LoadOneCursor(makefilename(GAMECURSOR_NAME,GAMECURSOR_OFFSET,'\\',0,allfilescursors[i]),i))
			return i+1;
    }
    if (LoadOneCursor(makefilename(GAMECURSOR_NAME,GAMECURSOR_OFFSET,0,0,ARROW_STR),i))
        return i+1;
    return 0;
}
//==============================
void HighMouse::UnloadCursors(void)
{
    for (int i=0;i<TYPEOFCURSORS;i++)
    {
		mouses[i].curentposition = 0;
		mouses[i].maxpositions = 0;
		if (mouses[i].mousegrp)
		{
			if (mouses[i].flag == 1)
				freegrp(mouses[i].mousegrp);
			else
				if (mouses[i].flag == 2)
					freegrp(mouses[i].mousegrp);
				else
				{
					printf("error mouse cursor source(grp or mpq)\n");
				}
			mouses[i].mousegrp=NULL;
		}
    }
}
//==============================
void HighMouse::SetMouseOn(OBJ *mouseonobj,int desttype)
{
    DestMouseOBJ = mouseonobj;
    DestMouseType = desttype;
}
//==============================
void HighMouse::DrawMouse(int menuActive, int mainArea, int minimapArea,int onBorder)
{
    int deltax,deltay,mousetype;
    if (!menuActive)
    {
		if (mainArea)
		{
			DestMouseType = SELECTIONMOUSE;
		}
		else
		{
			if (!onBorder)
			{
				if (WaitToPressLeftButton)
				{
					if (DestMouseOBJ)
					{
						switch(DestMouseType)
						{
							case MOUSEON_ENEMYUNIT:
							case MOUSEON_ENEMYBUILD:
								DestMouseType = TARGREDMOUSE;
							break;
							case MOUSEON_NEUTRALUNIT:
							case MOUSEON_NEUTRALBUILD:
								DestMouseType = TARGYELLOWMOUSE;
								break;
							case MOUSEON_ALLIANCEUNIT:
							case MOUSEON_ALLIANCEBUILD:
								DestMouseType = TARGYELLOWMOUSE;
								break;
							case MOUSEON_MYUNIT:
							case MOUSEON_MYBUILD:
								DestMouseType = TARGGREENMOUSE;
								break;
							default:
								DestMouseType = -1;
								return;
						}
					}
					else
					{
						DestMouseType = TARGGREENMOUSE;
					}
				}//waitforleftbuton
				else
				{
					if (DestMouseOBJ && mainArea && !minimapArea)
					{
						switch(DestMouseType)
						{
							case MOUSEON_ENEMYUNIT:
							case MOUSEON_ENEMYBUILD:
								DestMouseType = MOUSEONOBJRED;
							break;
							case MOUSEON_NEUTRALUNIT:
							case MOUSEON_NEUTRALBUILD:
								DestMouseType = MOUSEONOBJYELLOW;
							break;
							case MOUSEON_ALLIANCEUNIT:
							case MOUSEON_ALLIANCEBUILD:
								DestMouseType = MOUSEONOBJYELLOW;
							break;
							case MOUSEON_MYUNIT:
							case MOUSEON_MYBUILD:
								DestMouseType = MOUSEONOBJGREEN;
							break;
							default:
								DestMouseType = -1;
							return;
						}
					}
					else
					{
						DestMouseType = NORMALMOUSE;
					}
				}
			}//OnBorder
		}
    }
    if (mouses[DestMouseType].mousegrp)
    {
		deltax = -mouses[DestMouseType].mousegrp->SizeX/2;
		deltay = -mouses[DestMouseType].mousegrp->SizeY/2;
		putgrpspr(MouseX + deltax,MouseY + deltay,mouses[DestMouseType].mousegrp,NORMAL,
					0,0,NULL,mouses[DestMouseType].curentposition);
    }
}
//==========================
void HighMouse::DrawSelectedArea(int selectarea)
{
    if (selectarea)
    {
		wrectangle(COLORHR,PrevMouseX,PrevMouseY,MouseX,MouseY);
    }
}
//==========================
int HighMouse::CheckBorder(int x,int y,int x2,int y2)
{
    if (MouseX >= x && MouseY >= y && MouseX <= x2 && MouseY <= y2)
		return(1);
    else
		return(0);
}
//==========================
void HighMouse::PutDestination(OBJ *destobj,int xm,int ym,int modemove,int rightclick)
{
    if (PLAYER[NUMBGAMER].isobserverflag)
		return;
    if (!WaitToPressLeftButton)
    {
		if (Construct.SC_BuildUnit != SC_NOUNITNR)
		{
			if (Construct.CanDo > 0)
			{
				if (IsBuild(Construct.SC_BuildUnit))
				{
					xm = Construct.PosX + map.MAPXGLOBAL;
					ym = Construct.PosY + map.MAPYGLOBAL;
					if (gameconf.audioconf.buildsounds)
					{
						if (fordeselect[0])
						{
							int race = GetUnitRace(fordeselect[0]->SC_Unit);
							Play_sfxdata_id(fordeselect[0],sfx_buildplace[race],2,0);
						}
					}
				}
			}
			else
			{
				//cancel construct if wrong location
				if (Construct.CanDo < 0)
				{
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
								if (modemove == MODELANDING)
								{
									Construct.CanDo = TOBELAND_CANTLANDHERE;
								}
								Play_sfxdata_id(fordeselect[0],PUTLIFTDOWNERROR,2,0);
							}
						}
					}
					putbuildplacemessage(-Construct.CanDo);
					return;
				}
				else
				{
					WaitToPressLeftButton = 1;
					SetVisualMapPosition((int) (((MouseY - Xkart - Xkartbeg)/factorx) - widthkart/2) * SIZESPRLANSHX,
						 (int) (((MouseY - Ykart - Ykartbeg)/factory) - hightkart/2) * SIZESPRLANSHY);
					return;
				}
			}
		}
		if (selectedobjmove(destobj,xm,ym,modemove,NUMBGAMER,SHOWERRORTEXT,rightclick))
    	{
    	    DestCursor.Presence = 1;
    	    DestCursor.PosX = xm;
            DestCursor.PosY = ym;
			SetBlinkOBJ(destobj);
        }
    }
}
//=================================
void HighMouse::NextMousePict(void)
{
    for (int i=0;i<TYPEOFCURSORS;i++)
    {
		mouses[i].curentposition++;
		if (mouses[i].curentposition >= mouses[i].maxpositions)
			mouses[i].curentposition = 0;
    }
}
//=================================
float scrollmapx(int border,float factor)
{
    return 0;
}
/*    static float ticks=0;
//    if (MOUSESCROLLON)
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
*/
//=================================
float scrollmapy(int border,float factor)
{
    return 0;
}
/*    static float ticks=0;
//    if (MOUSESCROLLON)
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
*/
//=================================
void HighMouse::GetMouseType(int xk,int yk,int menuActive,int mainArea,int minimapArea)
{
    int borderBits;
    int activeSelection;
    int waitToPress;
    DestMouseType = NORMALMOUSE;
    if (menuActive)
		return;
    if (GAME)
    {
		borderBits = 0;
        if (!(MouseButtons & WMLEFTKEY))
        {
    	    WaitForReleaseLeftButton = 0;
        }
        if (!WaitForReleaseLeftButton)
        {
    	    if (!WaitToPressLeftButton)
    	    {
				Construct.SC_BuildUnit = SC_NOUNITNR;
                if ((MouseButtons & WMLEFTKEY) && (mainArea) && !(minimapArea))
                {
            	    if (PrevMouseY < gameconf.grmode.y - YDECICONS)
						activeSelection = 1;
                    else
                        activeSelection = 0;
                }
                if (!MouseButtons)
                {
                    waitToPress = 0;
                    if (activeSelection)
                    {
                        if (KEYPRESS(SHIFTLKEY) || KEYPRESS(SHIFTRKEY))
							selectMAN(PrevMouseX,PrevMouseY,MouseX,MouseY,1);
                        else
							selectMAN(PrevMouseX,PrevMouseY,MouseX,MouseY,0);
						activeSelection = 0;
                    }
                    PrevMouseX = MouseX;
                    PrevMouseY = MouseY;
                }
            }
            else
            {
                if (WaitForReleaseRightButton && (!(MouseButtons & WMRIGHTKEY)))
                {
                    WaitForReleaseRightButton = 0;
                    WaitForReleaseLeftButton = 0;
                }
                if (properties[8] == MODECANCEL_BB_AB && (MouseButtons&WMRIGHTKEY))
                {
                    WaitForReleaseRightButton = 1;
                }
                if (WaitToPressLeftButton == 2)
                   if (!(MouseButtons&WMLEFTKEY))
                      WaitToPressLeftButton--;
                WaitForReleaseLeftButton = 1;
            }
		}
		if (WaitToPressLeftButton == 1 && mainArea)
		{
			if (MouseButtons&WMLEFTKEY)
			{
				WaitToPressLeftButton = 0;
				if (minimapArea)
				{
					PutDestination(DestMouseOBJ,
							  (int)((MouseX - Xkart - Xkartbeg)/factorx)*SIZESPRLANSHX,
							  (int)((MouseY - Ykart - Ykartbeg)/factory)*SIZESPRLANSHY,
							  UnitCommandAction,0);
				}
				else
				{
					PutDestination(DestMouseOBJ,MouseX + xk,MouseY + yk,UnitCommandAction,0);
				}
			}
		}
		MouseBitBorder = 0;
		if ( MouseX <= BORDERMOUSE )
			borderBits |= 1;
		else
			if ( MouseX >= gameconf.grmode.x - BORDERMOUSE )
				borderBits |= 2;
		if ( MouseY <= BORDERMOUSE  )
			borderBits |= 4;
		else
			if (MouseY >= gameconf.grmode.y - BORDERMOUSE )
				borderBits |= 8;
		if (MOUSESCROLLON)
		{
			switch(borderBits)
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
						MouseBitBorder = 1;
						DestMouseType = MOUSELEFTSCROLL;
						if (!addscrx)
							addscrx = (int)scrollmapx(-1,SMOUTHMOUSE);
						if (!addscry)
							addscry = (int)scrollmapy(0,SMOUTHMOUSE);
                    }
                    else
						DestMouseType = NORMALMOUSE;
                    break;
    		    case 2:
                    if (xk<(MAXXMAP-widthkart)*SIZESPRLANSHX)
                    {
						MouseBitBorder = 1;
						DestMouseType = MOUSERIGHTSCROLL;
						if (!addscrx)
							addscrx = (int)scrollmapx(1,SMOUTHMOUSE);
						if (!addscry)
							addscry = (int)scrollmapy(0,SMOUTHMOUSE);
                    }
                    else
						DestMouseType = NORMALMOUSE;
                    break;
    		    case 4:
                    if (yk)
                    {
						MouseBitBorder = 1;
						DestMouseType = MOUSEUPSCROLL;
						if (!addscrx)
							addscrx = (int)scrollmapx(0,SMOUTHMOUSE);
						if (!addscry)
							addscry = (int)scrollmapy(-1,SMOUTHMOUSE);
                    }
                    else
						DestMouseType = NORMALMOUSE;
                    break;
    		    case 5:
                    if (xk||yk)
                    {
						MouseBitBorder = 1;
						DestMouseType = MOUSELEFTUPSCROLL;
						if (!addscrx)
							addscrx = (int)scrollmapx(-1,SMOUTHMOUSE);
						if (!addscry)
							addscry = (int)scrollmapy(-1,SMOUTHMOUSE);
                    }
                    else
						DestMouseType = NORMALMOUSE;
                    break;
    		    case 6:
                    if (xk<(MAXXMAP-widthkart)*SIZESPRLANSHX||yk)
                    {
						MouseBitBorder = 1;
						DestMouseType = MOUSERIGHTUPSCROLL;
						if (!addscrx)
							addscrx = (int)scrollmapx(1,SMOUTHMOUSE);
						if (!addscry)
							addscry = (int)scrollmapy(-1,SMOUTHMOUSE);
					}
					else
						DestMouseType = NORMALMOUSE;
					break;
    		    case 8:
                    if (yk<(MAXYMAP-hightkart)*SIZESPRLANSHY)
                    {
						MouseBitBorder = 1;
						DestMouseType = MOUSEDOWNSCROLL;
						if (!addscrx)
							addscrx = (int)scrollmapx(0,SMOUTHMOUSE);
						if (!addscry)
							addscry = (int)scrollmapy(1,SMOUTHMOUSE);
                    }
                    else
						DestMouseType = NORMALMOUSE;
                    break;
    		    case 9:
                    if (xk||yk<(MAXYMAP-hightkart)*SIZESPRLANSHY)
                    {
						MouseBitBorder = 1;
						DestMouseType = MOUSELEFTDOWNSCROLL;
						if (!addscrx)
							addscrx = (int)scrollmapx(-1,SMOUTHMOUSE);
						if (!addscry)
							addscry = (int)scrollmapy(1,SMOUTHMOUSE);
                    }
                    else
						DestMouseType = NORMALMOUSE;
                    break;
    		    case 0xa:
                    if (xk<(MAXXMAP-widthkart)*SIZESPRLANSHX ||
                        yk<(MAXYMAP-hightkart)*SIZESPRLANSHY)
                    {
						MouseBitBorder = 1;
						DestMouseType = MOUSERIGHTDOWNSCROLL;
						if (!addscrx)
							addscrx = (int)scrollmapx(1,SMOUTHMOUSE);
						if (!addscry)
							addscry = (int)scrollmapy(1,SMOUTHMOUSE);
                    }
                    else
						DestMouseType = NORMALMOUSE;
                    break;
			}//switch
		}//mouse
		MouseBitBorder = borderBits;
    }//game
    if ((MouseButtons == WMRIGHTKEY) && (!waitToPress) && mainArea && GAME && (!WaitForReleaseRightButton))
    {
		waitToPress = 1;
		PutDestination(DestMouseOBJ,MouseX + xk,MouseY + yk,MODEMOVE,1);
    }
    if (activeSelection && MoveOnMinimap != YES)
    {
		if (MouseX != PrevMouseX || MouseY != PrevMouseY)
    	{
    	    MouseBitSelection = 1;
    	    addscrx=0;
    	    addscry=0;
    	    if (PrevMouseX > gameconf.grmode.x)
				PrevMouseX = gameconf.grmode.x;
    	    if (PrevMouseX > gameconf.grmode.y)
				PrevMouseY = gameconf.grmode.y;
        }
    }
    else
    {
        MouseBitSelection = 0;
        if (!GAME)
        {
			DestMouseType = NORMALMOUSE;
        }
        if (MouseButtons == WMLEFTKEY)
        {
			DestMouseType = NORMALMOUSE;
        }
    }
}
//=================================
void HighMouse::LoadPixelsUnderMouse(void)
{
    CPutImage8( mousesavedpixels.posx,
		mousesavedpixels.posy,
		mousesavedpixels.sizex,
		mousesavedpixels.sizey,
		mousesavedpixels.buff);
}
//=================================
void HighMouse::SavePixelsUnderMouse(void)
{
    mousesavedpixels.posx = MouseX - MAXMOUSESIZEX / 2;
    mousesavedpixels.posy = MouseY - MAXMOUSESIZEY / 2;
    if (mousesavedpixels.posx < 0)
		mousesavedpixels.posx = 0;
    if (mousesavedpixels.posy < 0)
		mousesavedpixels.posy = 0;
    mousesavedpixels.sizex = MAXMOUSESIZEX;
    mousesavedpixels.sizey = MAXMOUSESIZEY;
    if (mousesavedpixels.posx + mousesavedpixels.sizex > GRP_wmaxx)
		mousesavedpixels.sizex = GRP_wmaxx - mousesavedpixels.posx + 1;
    if (mousesavedpixels.posy + mousesavedpixels.sizey > GRP_wmaxy)
		mousesavedpixels.sizey = GRP_wmaxy - mousesavedpixels.posy + 1;
    CPutImage8( mousesavedpixels.posx,
		mousesavedpixels.posy,
		mousesavedpixels.sizex,
		mousesavedpixels.sizey,
		mousesavedpixels.buff);
}
//==========================
void HighMouse::MouseOnObjClear(void)
{
    for (int i=0;i<MOUSEON_MAXVALUE;i++)
        MouseOnOBJS[i] = NULL;
    DestMouseOBJ = NULL;
    DestMouseType = -1;
    MouseObjType = MOUSEON_MAXVALUE;
}
//=================================
void HighMouse::GetMouseOnObj(void)
{
    for (int i=0;i<MOUSEON_MAXVALUE;i++)
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
