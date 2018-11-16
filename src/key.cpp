
#include <string.h>

#include "load.h"

#include "auxil.h"
#include "audio.h"
#include "doodad.h"
#include "gr.h"
#include "vars.h"
#include "gener.h"
#include "mytime.h"
#include "objs.h"
#include "gamemenu.h"
#include "maps.h"
#include "fog.h"
#include "mage.h"
#include "tempobj.h"
#include "messages.h"
#include "market.h"
#include "weapons.h"
#include "action.h"
#include "images.h"
#include "portrait.h"


#include "key.h"

#ifdef WITHSDL
#include "sdl/grsdl.h"
#include "sdl/keysdl.h"
#endif
#ifdef UNDERDOS
#include "dos/handlers.h"
#include "dos/keydos.h"
#endif

#define	 TIMEFORKEY	 4
#define	 KEYSLAY	 2

//================================
void keyscroll(void)
{
	/*	if (KEYPRESS(LEFTKEY))
			screenMapInfo->ScrollX-=factorscrollkey;
		if (KEYPRESS(RIGHTKEY))
			screenMapInfo->ScrollX+=factorscrollkey;
		if (KEYPRESS(UPKEY))
			screenMapInfo->ScrollY-=factorscrollkey;
		if (KEYPRESS(DOWNKEY))
			screenMapInfo->ScrollY+=factorscrollkey;
	*/
}
//==================================
void keyup(void)
{
	do {
	} while (!keyactive);
}
//===================================================
void keydown(void)
{
	do {
	} while (keyactive);
}
//====================================
void pausekey(void)
{
	keydown();
	keyup();
}
//==================================
int readkey(void)
{
	static int timeoldkey = 0;
	int a = 0;
	if (tick_timer > timeoldkey)
	{
		if (keyactive)
		{
			a = lastkey;
			timeoldkey = tick_timer + KEYSLAY;
		}
	}
	return(a);
}
//====================================
int strcmpw(char *a, char *b)
{
	int i, l = strlen(b);
	for (i = 1;i <= l;i++)
		if (*a++ != *b++) return(0);
	return(1);
}
//====================================
void keyhandler(void)
{
#ifdef DEBUG
	OBJ *o = NULL;
	if (SelectedUnits.totalelem)
		o = (OBJ *)SelectedUnits.GetElem(0, NULL);
	if (KEYPRESS(F12))
	{
		//		logend();
		menustatus = EXITGAME;
		return;
	}
	if (KEYPRESS(F11))
	{
		SHOWCELLS = 1 - SHOWCELLS;
	}
	if (KEYPRESS(ALTKEY))
	{
	}
	if (curentreadkey == NUMB1KEY)
	{
		for (int g = 0;g < PLAYEDPLAYERS;g++)
			deselectallobj(g);
		do {
			if (++NUMBGAMER >= PLAYEDPLAYERS)
				NUMBGAMER = 0;
		} while (IfPlayerHaveStartLocation(&map, NUMBGAMER) == -1);
		changegoods = 0;
		map.clearfog[NUMBGAMER] = 1;
		bitsplayer = GetVisionBitsPlayer(NUMBGAMER);
		MAPREGENERATIONBIT = 1;
		MAPUNITSREGENERATIONBIT = 1;
	}
	/*	  if (keyactive==F1)
		{
			testmenu();
		}
		if (keyactive==F2)
		{
			for (int i=0;i<MaxObjects;i++)
			{
				if (objects[i]->playernr==1)
					dieobj(objects[i]);
			}
		}
		if (keyactive==F2)
		{
	//		WaitingPlayersMenu(0x0fff);
			showedmenu.prepareforshowmenu(&WaitingPlayersMenu,0xffff);
																//all 12 players (0000 1111 1111 1111)
	//																				0	f	 f	  f
		}
	*/
	if (curentreadkey == F5)
	{
		if (o)
		{
			dieobj(o);
		}
	}
	if (curentreadkey == F6)
	{
		if (o)
		{
			LowLevelDamage(NULL, o, WEAPONID_NUCLEARMISSILE, DAMAGE_INDEPENDENT, 25 << 8, 0, 0);
		}
	}
#endif
	if (keyactive == F10)
	{
		f10_menu |= GAMEBUTTON_KEYPRESS;
		f10_menu &= ~GAMEBUTTON_KEYRELEASE;
	}
	else
		if (f10_menu & GAMEBUTTON_KEYPRESS)
		{
			f10_menu &= ~GAMEBUTTON_KEYPRESS;
			f10_menu |= GAMEBUTTON_KEYRELEASE;
		}
	if (keyactive == TABKEY)
	{
		terr_menu |= GAMEBUTTON_KEYPRESS;
		terr_menu &= ~GAMEBUTTON_KEYRELEASE;
	}
	else
		if (terr_menu & GAMEBUTTON_KEYPRESS)
		{
			terr_menu &= ~GAMEBUTTON_KEYPRESS;
			terr_menu |= GAMEBUTTON_KEYRELEASE;
		}
	if (MENUACTIVE == 0 && (keyactive == ENTERKEY || keyactive == ENTERKEY2))
	{
		showedmenu.prepareforshowmenu(&chatboxmenu, NULL);
	}
	menustatus = CONTINUEGAME;
	return;
}
//====================================
