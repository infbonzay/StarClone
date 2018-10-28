#include <grplib/usegrp.h>
#include <grplib/gr8.h>

#include "gr.h"
#include "stringfiles.h"
#include "auxil.h"
#include "debug.h"
#include "mouse.h"
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
		if (patrate)
		{
			MouseType = SELECTIONMOUSE;
		}
		else
		{
			if (!MouseOnBorder)
			{
				if (waitforleftbuton)
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
				}//waitforleftbuton
				else
				{
					if ((DestMouseOBJ)&&select_aria&&!patrate)
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
		putgrpspr(mouse_x+deltax,mouse_y+deltay,highMouse.cursors[MouseType].mousegrp,NORMAL,
				  0,0,NULL,highMouse.cursors[MouseType].curentposition);
	}
}
//==========================
void HighMouse::DrawSelectionArea(void)
{
    if (patrate)
    {
		wrectangle(COLORHR,mouseprevx,mouseprevy,mouse_x,mouse_y);
    }
}
//==========================

