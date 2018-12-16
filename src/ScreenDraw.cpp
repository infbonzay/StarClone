
#include "auxil.h"
#include "man.h"
#include "vars.h"
#include "gener.h"
#include "maps.h"
#include "fonts.h"
#include "mytime.h"

#include "ScreenDraw.h"

//==========================================================
void ScreenDraw::AddRegion(int regionNr, int x, int y, int sx, int sy)
{
	Regions[regionNr].x = x;
	Regions[regionNr].y = y;
	Regions[regionNr].w = sx;
	Regions[regionNr].h = sy;
}
//==========================================================
void ScreenDraw::AddRegion(int regionNr, SCREEN_REGION *Region)
{
	Regions[regionNr].x = Region->x;
	Regions[regionNr].y = Region->y;
	Regions[regionNr].w = Region->w;
	Regions[regionNr].h = Region->h;
}
//==========================================================
void ScreenDraw::CloneRegion(int toRegionNr, int fromRegionNr)
{
	AddRegion(toRegionNr, &Regions[fromRegionNr]);
}
//==========================================================
void ScreenDraw::TopMessage(void)
{
	static int frames, fps, prevsec, clc;
	static long prevgamecycle;
	int cursec;
	char s[300];
	char ss[50];
	s[0] = 0;
	OBJ *o = NULL;
	if (SelectedUnits.Count())
	{
		o = (OBJ *)SelectedUnits.GetElem(0, NULL);
	}
	frames++;
	cursec = mytimer.GetTimeParsed();
	if (cursec != prevsec)
	{
		prevsec = cursec;
		clc = gamecycle - prevgamecycle;
		prevgamecycle = gamecycle;
		fps = frames;
		frames = 0;
	}
	strcat(s, "FPS/CYCLES:");
	itoa(fps, ss, 10);
	strcat(s, ss);
	strcat(s, "/");
	itoa(clc, ss, 10);
	strcat(s, ss);

	strcat(s, " GCYCLE:");
	itoa(gamecycle, ss, 10);
	strcat(s, ss);
	strcat(s, " maxobjs:");
	itoa(MaxObjects, ss, 10);
	strcat(s, ss);
	/*	  if (fordeselect[0])
		{
		sprintf(ss," %d,%d %d,%d",GetOBJx(fordeselect[0]),GetOBJy(fordeselect[0]),fordeselect[0]->finalx>>8,fordeselect[0]->finaly>>8);
		strcat(s,ss);
		}
	*/
	/*	  if (fordeselect[0])
		{
		sprintf(ss," 0x%x",(int)fordeselect[0]);
		strcat(s,ss);
		}
		if (fordeselect[0])
		{
		sprintf(ss," %d",fordeselect[0]->mainimage->side);
		strcat(s,ss);
		}
	*/
	strcat(s, " hot:");
	itoa(mousehotpos, ss, 10);
	strcat(s, ss);

	strcat(s, " pl:");
	itoa(NUMBGAMER, ss, 10);
	strcat(s, ss);
	strcat(s, "/");
	itoa(PLAYEDPLAYERS, ss, 10);
	strcat(s, ss);

	if (o)
	{
		strcat(s, " objpl:");
		itoa(o->playernr, ss, 10);
		strcat(s, ss);
		strcat(s, " move:");
		itoa(o->modemove, ss, 10);
		strcat(s, ss);
		//	strcat(s,"/");
		//	itoa(o->atackcooldowntime,ss,10);
		//	strcat(s,ss);
	}
	/*	  if (o)
		{
			strcat(s,"x=");
			itoa(o->xkart,ss,10);
			strcat(s,ss);
			strcat(s,"y=");
			itoa(o->ykart,ss,10);
			strcat(s,ss);
			strcat(s," x=");
			itoa(o->sourcex,ss,10);
			strcat(s,ss);
			strcat(s,"y=");
			itoa(o->sourcey,ss,10);
			strcat(s,ss);
		}
	*/
	putmessage(0, 0, IDFONT16, s, GGREENCOLORFONT, tfontgamp, gamedlggrp);
}
//===================================================
void ScreenDraw::UpdateScreen(void)
{
	if (map.palettechanges)
	{
		palchange(map.palette, gameconf.videoconf.gamma, gameconf.videoconf.saturate);
		map.palettechanges = 0;
		if (!(gameconf.grmode.flags & DISPLAYFLAGS_EMULATIONMODE))
			mainController.UpdateScreenRegions(UsedRegions, Regions);
	}
	else
		mainController.UpdateScreenRegions(UsedRegions, Regions);
	//	  printf("totalimgs=%d drawedimgs=%d\n",totalimgs,drawedimgs);		//to check for leaks
	//	  printf("windowactive=%d\n",gameconf.grmode.windowactive);
}

