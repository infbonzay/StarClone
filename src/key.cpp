
#include "market.h"
#include "tempobj.h"
#include "objs.h"
#include "gener.h"
#include "vars.h"
#include "man.h"
#include "Controller.h"
#include "key.h"

#ifdef WITHSDL
#include "sdl/keysdl.h"
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
void keyhandler(void)
{
#ifdef DEBUG
	OBJ *o = NULL;
	if (SelectedUnits.Count())
		o = (OBJ *)SelectedUnits.GetElem(0, NULL);
	if (mainController.KeyActive == F12)
	{
		menustatus = EXITGAME;
		return;
	}
	if (mainController.KeyActive == F11)
	{
		SHOWCELLS = 1 - SHOWCELLS;
	}
	if (mainController.KeyActive == NUMB1KEY)
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
	/*	  if (mainController.KeyActive==F1)
		{
			testmenu();
		}
		if (mainController.KeyActive==F2)
		{
			for (int i=0;i<MaxObjects;i++)
			{
				if (objects[i]->playernr==1)
					dieobj(objects[i]);
			}
		}
		if (mainController.KeyActive==F2)
		{
	//		WaitingPlayersMenu(0x0fff);
			showedmenu.prepareforshowmenu(&WaitingPlayersMenu,0xffff);
																//all 12 players (0000 1111 1111 1111)
	//																				0	f	 f	  f
		}
	*/
	if (mainController.KeyActive == F5)
	{
		if (o)
		{
			dieobj(o);
		}
	}
	if (mainController.KeyActive == F6)
	{
		if (o)
		{
			LowLevelDamage(NULL, o, WEAPONID_NUCLEARMISSILE, DAMAGE_INDEPENDENT, 25 << 8, 0, 0);
		}
	}
#endif
	if (mainController.KeyActive == F10)
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
	if (mainController.KeyActive == TABKEY)
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
	if (MENUACTIVE == 0 && (mainController.KeyActive == ENTERKEY || mainController.KeyActive == ENTERKEY2))
	{
		showedmenu.prepareforshowmenu(&chatboxmenu, NULL);
	}
	menustatus = CONTINUEGAME;
	return;
}
//====================================
