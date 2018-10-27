#include <grplib/usegrp.h>
#include <grplib/gr8.h>

#include "stringfiles.h"
#include "auxil.h"
#include "debug.h"
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
				if (cursors[i].flag == 2)
					freegrp(cursors[i].mousegrp);
				else
				{
					DEBUGMESSCR("error mouse cursor source(grp or mpq)\n");
				}
			cursors[i].mousegrp=NULL;
		}
    }
}
//==============================
