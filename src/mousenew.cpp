#include <grplib/usegrp.h>
#include <grplib/gr8.h>

#include "gr.h"
#include "stringfiles.h"
#include "auxil.h"
#include "debug.h"
#include "gener.h"
#include "vars.h"
#include "mousenew.h"

//==============================
HighMouse highMouse;
//==============================

int HighMouse::LoadOneCursor(char *filename,int typemouse)
{
    if (mpqloadfile(filename,(char **)&cursors[typemouse].mousegrp))
    {
		if (loadgrp(filename,&cursors[typemouse].mousegrp))
    	    return -1;
    	else
    	    cursors[typemouse].flag = 1;
    }
    else
        cursors[typemouse].flag = 2;

    cursors[typemouse].curentposition = 0;
    cursors[typemouse].maxpositions = cursors[typemouse].mousegrp->CountPictures;
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
		cursors[i].curentposition = 0;
		cursors[i].maxpositions = 0;
		if (cursors[i].mousegrp)
		{
			if (cursors[i].flag == 1)
				freegrp(cursors[i].mousegrp);
			else
			{
				if (cursors[i].flag == 2)
				{
					freegrp(cursors[i].mousegrp);
				}
				else
				{
					DEBUGMESSCR("error mouse cursor source(grp or mpq)\n");
				}
			}
			cursors[i].mousegrp=NULL;
		}
    }
}
//==============================
void HighMouse::DrawMouse(void)
{
	if (!MENUACTIVE)
	{
		if (highMouse.MouseOnSelectionMode)
		{
			MouseType = SELECTIONMOUSE;
		}
		else
		{
			if (!MouseOnBorder)
			{
				if (WaitToPressLeftButton)
				{
					if (DestMouseOBJ)
					{
						switch(DestMouseType)
						{
							case MOUSEON_ENEMYUNIT:
							case MOUSEON_ENEMYBUILD:
								MouseType = TARGREDMOUSE;
								break;
							case MOUSEON_NEUTRALUNIT:
							case MOUSEON_NEUTRALBUILD:
								MouseType = TARGYELLOWMOUSE;
								break;
							case MOUSEON_ALLIANCEUNIT:
							case MOUSEON_ALLIANCEBUILD:
								MouseType = TARGYELLOWMOUSE;
								break;
							case MOUSEON_MYUNIT:
							case MOUSEON_MYBUILD:
								MouseType = TARGGREENMOUSE;
								break;
							default:
								MouseType = -1;
								break;
						}
					}
					else
						MouseType = TARGGREENMOUSE;
				}//highMouse.WaitToPressLeftButton
				else
				{
					if ((DestMouseOBJ) && select_aria && !MouseOnSelectionMode)
					{
						switch(DestMouseType)
						{
							case MOUSEON_ENEMYUNIT:
							case MOUSEON_ENEMYBUILD:
								MouseType = MOUSEONOBJRED;
								break;
							case MOUSEON_NEUTRALUNIT:
							case MOUSEON_NEUTRALBUILD:
								MouseType = MOUSEONOBJYELLOW;
								break;
							case MOUSEON_ALLIANCEUNIT:
							case MOUSEON_ALLIANCEBUILD:
								MouseType = MOUSEONOBJYELLOW;
								break;
							case MOUSEON_MYUNIT:
							case MOUSEON_MYBUILD:
								MouseType = MOUSEONOBJGREEN;
								break;
							default:
								MouseType = -1;
								break;
						}
					}
					else
					{
						MouseType = NORMALMOUSE;
					}
				}
			}
		}
	}
	if (MouseType < 0 )
	{
		DEBUGMESSCR("MouseType < 0 ");
		return;
	}
	if (highMouse.cursors[MouseType].mousegrp)
	{
		int deltax = -highMouse.cursors[MouseType].mousegrp->SizeX/2;
		int deltay = -highMouse.cursors[MouseType].mousegrp->SizeY/2;
		putgrpspr(PosX+deltax,PosY+deltay,highMouse.cursors[MouseType].mousegrp,NORMAL,
				  0,0,NULL,highMouse.cursors[MouseType].curentposition);
	}
}
//==========================
void HighMouse::DrawSelectionArea(void)
{
    if (highMouse.MouseOnSelectionMode)
    {
		wrectangle(COLORHR,PrevX,PrevY,PosX,PosY);
    }
}
//=================================
void HighMouse::ScrollMouse(void)
{
    for (int i=0;i<TYPEOFCURSORS;i++)
    {
		cursors[i].curentposition++;
		if (cursors[i].curentposition >= cursors[i].maxpositions)
			cursors[i].curentposition = 0;
    }
}
//=================================
void HighMouse::MouseOnObjClear(void)
{
    for (int i=0;i<MOUSEON_MAXVALUE;i++)
        MouseOnOBJS[i] = NULL;
    DestMouseOBJ = NULL;
    DestMouseType = MOUSEON_NONE;
    PrevMouseType = MOUSEON_MAXVALUE;
}
//=================================
void HighMouse::GetMouseOnObj(void)
{
    for (int i=0;i<MOUSEON_MAXVALUE;i++)
    {
        if (MouseOnOBJS[i])
        {
    	    DestMouseOBJ = highMouse.MouseOnOBJS[i];
    	    DestMouseType = i;
    	    break;
    	}
    }
}
//==================================
void HighMouse::SaveImageUnder(void)
{
    SavedUnder.PosX = PosX - MAXMOUSESIZEX/2;
    SavedUnder.PosY = PosY - MAXMOUSESIZEY/2;
    if (SavedUnder.PosX < 0)
		SavedUnder.PosX = 0;
    if (SavedUnder.PosY < 0)
		SavedUnder.PosY = 0;
    SavedUnder.SizeX = MAXMOUSESIZEX;
    SavedUnder.SizeY = MAXMOUSESIZEY;
    if (SavedUnder.PosX + SavedUnder.SizeX > GRP_wmaxx)
		SavedUnder.SizeX = GRP_wmaxx - SavedUnder.PosX + 1;
    if (SavedUnder.PosY + SavedUnder.SizeY > GRP_wmaxy)
		SavedUnder.SizeY = GRP_wmaxy - SavedUnder.PosY + 1;
    CGetImage8(SavedUnder.PosX,SavedUnder.PosY,SavedUnder.SizeX,SavedUnder.SizeY,SavedUnder.SavedPixels);
}
//==========================
void HighMouse::LoadImageUnder(void)
{
    CPutImage8(SavedUnder.PosX,SavedUnder.PosY,SavedUnder.SizeX,SavedUnder.SizeY,SavedUnder.SavedPixels);
}
//==========================
bool HighMouse::CheckForBorder(int x1,int y1,int x2,int y2)
{
    if(PosX >= x1 && PosY >= y1 && PosX <= x2 && PosY <= y2)
		return(1);
    else
		return(0);
}
//==========================
void HighMouse::SetRestrictCoords(int restrictNr)
{
    RestrictNr = restrictNr;
}
//==========================================
void HighMouse::SetRestrictCoords(int restrictNr,int x1,int y1,int x2,int y2)
{
    RestrictXY[restrictNr].x1 = x1;
    RestrictXY[restrictNr].y1 = y1;
    RestrictXY[restrictNr].x2 = x2;
    RestrictXY[restrictNr].y2 = y2;
}
//==========================================
void HighMouse::FixRestrict(int *x,int *y)
{
	bool change = false;
	if (*x > RestrictXY[RestrictNr].x2)
    {
        *x = RestrictXY[RestrictNr].x2;
        change = true;
    }
    else
    {
        if (*x < RestrictXY[RestrictNr].x1)
		{
			*x = RestrictXY[RestrictNr].x1;
			change = true;
		}
	}
    if (*y > RestrictXY[RestrictNr].y2)
    {
		*y = RestrictXY[RestrictNr].y2;
        change = true;
    }
    else
    {
        if (*y < highMouse.RestrictXY[RestrictNr].y1)
		{
    	    *y = highMouse.RestrictXY[RestrictNr].y1;
			change = true;
		}
	}
	PosX = *x;
	PosY = *y;
    if (change)
    {
		lowMouse.SetPos(PosX,PosY);
	}
}
//==========================================
