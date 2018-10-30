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
#		include "sdl/grsdl.h"
#		include "sdl/mousesdl.h"
#		include "sdl/keysdl.h"
#endif

#ifdef UNDERDOS
#		include "dos/handlers.h"
#endif

int		factorscrollkey,mousehotpos;
int		mousemaxx,mousemaxy;
int		xmloc,ymloc,movieminikarta=NO,mousedoubleclick;
int		decrxx,decryy;
char	waitfordownrightbuton,waitfordownleftbuton;
char	patr,mouseclear,mloc;
unsigned short	mouseprop;
char	 timemouse;


//=================================
float scrollmapx(int border,float factor)
{
	static float ticks=0;
//	  if (highMouse->cursorsCROLLON)
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
//	  if (highMouse->cursorsCROLLON)
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
