#include <grplib/usegrp.h>
#include <grplib/gr8.h>

#include "gr.h"
#include "stringfiles.h"
#include "auxil.h"
#include "debug.h"
#include "vars.h"
#include "gener.h"
#include "objs.h"
#include "lists.h"
#include "maps.h"
#include "audio.h"
#include "iscript.h"
#include "walk.h"
#include "putobj.h"
#include "objinfo.h"
#include "key.h"
#include "mouse.h"

//==============================
HighMouse::HighMouse(void)
{
	PosX = 0;
	PosY = 0;
	PrevX = 0;
	PrevY = 0;
	MouseType = 0;
	PrevMouseType = 0;
	MoveFunc = NULL;
	ClickFunc = NULL;
	DblClickFunc = NULL;
	RestrictNr = 0;
	MouseOnBorder = false;
	MouseOnSelectionMode = false;
	WaitToPressLeftButton = false;
	MouseButtons = 0;
	MouseOnObjClear();
	lowMouse.Init();
	lowMouse.LowInMoveEvent(&MouseMoveEvent);
	lowMouse.LowInClickEvent(&MouseClickEvent);

}
//==============================
HighMouse::~HighMouse(void)
{
	lowMouse.LowUnClickEvent();
	lowMouse.LowUnMoveEvent();
}
//==============================
int HighMouse::LoadOneCursor(char *filename,int typemouse)
{
	if (mpqloadfile(filename,(char **)&cursors[typemouse].mousegrp))
	{
		if (loadgrp(filename,&cursors[typemouse].mousegrp))
			return -1;
	}
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
			freegrp(cursors[i].mousegrp);
			cursors[i].mousegrp = NULL;
		}
	}
}
//==============================
void HighMouse::DrawMouse(void)
{
	if (!MENUACTIVE)
	{
		if (highMouse->MouseOnSelectionMode)
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
				}//highMouse->WaitToPressLeftButton
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
	if (highMouse->cursors[MouseType].mousegrp)
	{
		int deltax = -highMouse->cursors[MouseType].mousegrp->SizeX/2;
		int deltay = -highMouse->cursors[MouseType].mousegrp->SizeY/2;
		putgrpspr(PosX+deltax,PosY+deltay,highMouse->cursors[MouseType].mousegrp,NORMAL,
				  0,0,NULL,highMouse->cursors[MouseType].curentposition);
	}
}
//==========================
void HighMouse::DrawSelectionArea(void)
{
	if (highMouse->MouseOnSelectionMode)
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
			DestMouseOBJ = highMouse->MouseOnOBJS[i];
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
		if (*y < highMouse->RestrictXY[RestrictNr].y1)
		{
			*y = highMouse->RestrictXY[RestrictNr].y1;
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
void HighMouse::InstallMoveEvent(MOUSEMOVEFUNC *eventFunc)
{
	MoveFunc = eventFunc;
}
//==========================
void HighMouse::UninstallMoveEvent(void)
{
	MoveFunc = NULL;
}
//==========================
void HighMouse::InstallClickEvent(MOUSECLICKFUNC *eventFunc)
{
	ClickFunc = eventFunc;
}
//==========================
void HighMouse::UninstallClickEvent(void)
{
	ClickFunc = NULL;
}
//==========================
void HighMouse::InstallDblClickEvent(MOUSEDBLCLICKFUNC *eventFunc)
{
	DblClickFunc = eventFunc;
}
//==========================
void HighMouse::UninstallDblClickEvent(void)
{
	DblClickFunc = NULL;
}
//==========================
void HighMouse::SetPos(int x,int y)
{
	lowMouse.SetPos(x,y);
	PosX = x;
	PosY = y;
}
//==========================
void HighMouse::DoRightClickAction(OBJ *destobj,int xm,int ym,int rightclick)
{
	int race;
    if (PLAYER[NUMBGAMER].isobserverflag)
		return;
	OBJ *o = NULL;
	if (SelectedUnits.totalelem)
		o = (OBJ *)SelectedUnits.GetElem(0,NULL);
	if (!highMouse->WaitToPressLeftButton)
	{
		if (Construct.SC_BuildUnit != SC_NOUNITNR)
		{
			if (Construct.CanDo > 0)
			{
				xm = Construct.PosX + map.MAPXGLOBAL;
				ym = Construct.PosY + map.MAPYGLOBAL;
				if (gameconf.audioconf.buildsounds)
				{
					if (IsBuild(Construct.SC_BuildUnit))
					{
						if (o)
						{
							race = GetUnitRace(o->SC_Unit);
							Play_sfxdata_id(o,sfx_buildplace[race],2,0);
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
						if (o)
						{
							if (IsWorkerUnit(o->SC_Unit))
							{
								activatesound(o,MODESOUNDERROR,2,NOSTOPCURRENTSOUNDS);
							}
							else
							{
								if (MouseModeMove == MODELANDING)
								{
									Construct.CanDo = TOBELAND_CANTLANDHERE;
								}
								Play_sfxdata_id(o,PUTLIFTDOWNERROR,2,0);
							}
						}
					}
					putbuildplacemessage(-Construct.CanDo);
					return;
				}
				else
				{
					highMouse->WaitToPressLeftButton = 1;
					SetVisualMapPosition((int) (((highMouse->PosX - screenMapInfo->SizeX32 - screenMapInfo->MinimapStartX) / factorx)-screenMapInfo->SizeX32/2)*SIZESPRLANSHX,
										 (int) (((highMouse->PosY - screenMapInfo->SizeY32 - screenMapInfo->MinimapStartY) / factory)-screenMapInfo->SizeY32/2)*SIZESPRLANSHY);
					return;
				}
			}
		}
		if (selectedobjmove(destobj,xm,ym,MouseModeMove,NUMBGAMER,SHOWERRORTEXT,rightclick))
		{
			destCursor->SetDestinationCursor(xm,ym);
			SetBlinkOBJ(destobj);
		}
	}
}
//==========================================
float HighMouse::ScrollMapX(int border,float factor)
{
	return 0;
}
//==========================================
float HighMouse::ScrollMapY(int border,float factor)
{
	return 0;
}
//==========================================
void HighMouse::RefreshMouseType(int xk,int yk)
{
	int borderBits;
	static bool selectionArea;
	static bool mouseClear;
	MouseType = NORMALMOUSE;
	if (MENUACTIVE)
		return;
	if (GAME)
	{
// 		if (DoubleClick && DestMouseOBJ)
// 		{
// 			selectMAN(PosX,PosX,PosX,PosY,0);
// 			DoubleClick = false;
// 			return;
// 		}
		borderBits = 0;
		if (!(GetButtonStatus() & WMLEFTKEY))
			WaitToReleaseLeftButton = 0;
		if (!WaitToReleaseLeftButton)
		{
			if (!WaitToPressLeftButton)
			{
				Construct.SC_BuildUnit = SC_NOUNITNR;
				if ((GetButtonStatus() & WMLEFTKEY) && (select_aria) && !(karta_aria))
				{
					if (PrevY < gameconf.grmode.y - YDECICONS)
						selectionArea = true;
					else
						selectionArea = false;
				}
				if (!GetButtonStatus())
				{
					mouseClear = false;
					if (selectionArea)
					{
						if (KEYPRESS(SHIFTLKEY) || KEYPRESS(SHIFTRKEY))
							selectMAN(PrevX,PrevY,PosX,PosY,1);
						else
							selectMAN(PrevX,PrevY,PosX,PosY,0);
						selectionArea = false;
					}
					PrevX = PosX;
					PrevY = PosY;
				}
			}
			else
			{
				if (WaitToReleaseRightButton && (!(GetButtonStatus() & WMRIGHTKEY)))
				{
					WaitToReleaseRightButton = 0;
					WaitToPressLeftButton = 0;
				}
				if (properties[8] == MODECANCEL_BB_AB && (GetButtonStatus()&WMRIGHTKEY))
				{
					WaitToReleaseRightButton = 1;
				}
				if (WaitToPressLeftButton == 2)
					if (!(GetButtonStatus() & WMLEFTKEY))
						WaitToPressLeftButton--;
				WaitToReleaseLeftButton = 1;
			}
		}
		if (WaitToPressLeftButton==1 && select_aria)
		{
			if (GetButtonStatus() & WMLEFTKEY)
			{
				WaitToPressLeftButton = 0;
				if (karta_aria)
					DoRightClickAction(	DestMouseOBJ,
										(int)((PosX-screenMapInfo->SizeX32-screenMapInfo->MinimapStartX)/factorx)*SIZESPRLANSHX,
										(int)((PosY-screenMapInfo->SizeY32-screenMapInfo->MinimapStartY)/factory)*SIZESPRLANSHY,
										0);
				else
					DoRightClickAction(	DestMouseOBJ,
										PosX + xk,
										PosY + yk,
										0);
			}
		}

		MouseOnBorder=0;
		if ( PosX <= BORDERMOUSE )
			borderBits |= 0x01;
		else
			if ( PosX >= gameconf.grmode.x - BORDERMOUSE )
				borderBits |= 0x02;
		if ( PosY <= BORDERMOUSE	 )
			borderBits |= 0x04;
		else
			if (PosY >= gameconf.grmode.y - BORDERMOUSE )
				borderBits |= 0x08;
		if (MOUSESCROLLON)
		{
			switch(borderBits)
			{
				case 0:
					if (!screenMapInfo->ScrollX)
						screenMapInfo->ScrollX = (int)ScrollMapX(0,SMOUTHMOUSE);
					if (!screenMapInfo->ScrollY)
						screenMapInfo->ScrollY = (int)ScrollMapY(0,SMOUTHMOUSE);
					break;
				case 1:
					if (xk)
					{
						MouseOnBorder=1;
						MouseType = MOUSELEFTSCROLL;
						if (!screenMapInfo->ScrollX)
							screenMapInfo->ScrollX = (int)ScrollMapX(-1,SMOUTHMOUSE);
						if (!screenMapInfo->ScrollY)
							screenMapInfo->ScrollY = (int)ScrollMapY(0,SMOUTHMOUSE);
					}
					else
						MouseType = NORMALMOUSE;
					break;
				case 2:
					if (xk<(MAXXMAP-screenMapInfo->SizeX32)*SIZESPRLANSHX)
					{
						MouseOnBorder=1;
						MouseType = MOUSERIGHTSCROLL;
						if (!screenMapInfo->ScrollX)
							screenMapInfo->ScrollX = (int)ScrollMapX(1,SMOUTHMOUSE);
						if (!screenMapInfo->ScrollY)
							screenMapInfo->ScrollY = (int)ScrollMapY(0,SMOUTHMOUSE);
					}
					else
						MouseType = NORMALMOUSE;
					break;
				case 4:
					if (yk)
					{
						MouseOnBorder=1;
						MouseType = MOUSEUPSCROLL;
						if (!screenMapInfo->ScrollX)
							screenMapInfo->ScrollX = (int)ScrollMapX(0,SMOUTHMOUSE);
						if (!screenMapInfo->ScrollY)
							screenMapInfo->ScrollY = (int)ScrollMapY(-1,SMOUTHMOUSE);
					}
					else
						MouseType = NORMALMOUSE;
					break;
				case 5:
					if (xk||yk)
					{
						MouseOnBorder=1;
						MouseType = MOUSELEFTUPSCROLL;
						if (!screenMapInfo->ScrollX)
							screenMapInfo->ScrollX = (int)ScrollMapX(-1,SMOUTHMOUSE);
						if (!screenMapInfo->ScrollY)
							screenMapInfo->ScrollY = (int)ScrollMapY(-1,SMOUTHMOUSE);
					}
					else
						MouseType = NORMALMOUSE;
					break;
				case 6:
					if (xk<(MAXXMAP-screenMapInfo->SizeX32)*SIZESPRLANSHX||yk)
					{
						MouseOnBorder=1;
						MouseType = MOUSERIGHTUPSCROLL;
						if (!screenMapInfo->ScrollX)
							screenMapInfo->ScrollX = (int)ScrollMapX(1,SMOUTHMOUSE);
						if (!screenMapInfo->ScrollY)
							screenMapInfo->ScrollY = (int)ScrollMapY(-1,SMOUTHMOUSE);
					}
					else
						MouseType = NORMALMOUSE;
					break;
				case 8:
					if (yk<(MAXYMAP-screenMapInfo->SizeY32)*SIZESPRLANSHY)
					{
						MouseOnBorder=1;
						MouseType = MOUSEDOWNSCROLL;
						if (!screenMapInfo->ScrollX)
							screenMapInfo->ScrollX = (int)ScrollMapX(0,SMOUTHMOUSE);
						if (!screenMapInfo->ScrollY)
							screenMapInfo->ScrollY = (int)ScrollMapY(1,SMOUTHMOUSE);
					}
					else
						MouseType = NORMALMOUSE;
					break;
				case 9:
					if (xk||yk<(MAXYMAP-screenMapInfo->SizeY32)*SIZESPRLANSHY)
					{
						MouseOnBorder=1;
						MouseType = MOUSELEFTDOWNSCROLL;
						if (!screenMapInfo->ScrollX)
							screenMapInfo->ScrollX = (int)ScrollMapX(-1,SMOUTHMOUSE);
						if (!screenMapInfo->ScrollY)
							screenMapInfo->ScrollY = (int)ScrollMapY(1,SMOUTHMOUSE);
					}
					else
						MouseType = NORMALMOUSE;
					break;
				case 0xa:
					if (xk<(MAXXMAP-screenMapInfo->SizeX32)*SIZESPRLANSHX||
						yk<(MAXYMAP-screenMapInfo->SizeY32)*SIZESPRLANSHY)
					{
						MouseOnBorder=1;
						MouseType = MOUSERIGHTDOWNSCROLL;
						if (!screenMapInfo->ScrollX)
							screenMapInfo->ScrollX = (int)ScrollMapX(1,SMOUTHMOUSE);
						if (!screenMapInfo->ScrollY)
							screenMapInfo->ScrollY = (int)ScrollMapY(1,SMOUTHMOUSE);
					}
					else
						MouseType = NORMALMOUSE;
					break;
			}//if & switch
		}

		if ((GetButtonStatus() == WMRIGHTKEY) && (!mouseClear) && select_aria && (!WaitToReleaseRightButton))
		{
			mouseClear = true;
			MouseModeMove = MODEMOVE;
			DoRightClickAction(DestMouseOBJ,PosX+xk,PosY+yk,1);
		}
	}//game

	if (selectionArea && !movieminikarta)
	{
		if (PosX != PrevX || PosY != PrevY)
		{
			MouseOnSelectionMode=1;
			//screenMapInfo->ScrollX = 0;
			//screenMapInfo->ScrollY = 0;
			if (PrevX > gameconf.grmode.x)
				PrevX = gameconf.grmode.x;
			if (PrevY > gameconf.grmode.y)
				PrevY = gameconf.grmode.y;
		}
	}
	else
	{
		MouseOnSelectionMode = 0;
		if (!GAME)
			MouseType = NORMALMOUSE;
		if (GetButtonStatus() == WMLEFTKEY)
		{
			MouseType = NORMALMOUSE;
		}
	}
}








//==========================================
DestCursor::DestCursor(void)
{
	Presence = false;
	MouseDestImg = NULL;
}
//==========================================
void DestCursor::ExecuteScript(void)
{
	if (MouseDestImg)
		iscriptinfo.ExecuteScript(MouseDestImg);
}
//==========================================
void DestCursor::SetDestinationCursor(int x,int y)
{
	Presence = true;
	PosX = x;
	PosY = y;
}
//==========================================
void DestCursor::DrawDestinationCursor(void)
{
	if (Presence)
	{
		Presence = false;
		if (!MouseDestImg)
		{
			MouseDestImg = new MAIN_IMG(IMAGEID_MOUSEDEST,
										PosX << 8,
										PosY << 8,
										MOUSEDESTELEVATION,0,0,0,0,
										SC_IMAGE_FLAG_NOCHECKFORFOG | SC_IMAGE_FLAG_AIRIMG,ISCRIPTNR_GNDATTKINIT);
		}
		else
		{
			MouseDestImg->SetPos(PosX << 8, PosY << 8);
			MouseDestImg->EnableExecScript();
			MouseDestImg->ShowImgFlag();
			MouseDestImg->SetIScriptNr(ISCRIPTNR_GNDATTKINIT);
		}
	}
	if (MouseDestImg)
		MouseDestImg->DrawImage();
}
//==========================================















//==========================================
void MouseMoveEvent(int x, int y)
{
	if (highMouse->MoveFunc)
		(highMouse->MoveFunc)(x,y);
}
//==========================================
void MouseClickEvent(bool type, int buttons)
{
	static long long prevtimer = 0;
	if (type)
	{
		highMouse->MouseButtons |= buttons;
	}
	else
	{
		highMouse->MouseButtons &= ~buttons;
	}
	if (highMouse->ClickFunc)
		(*highMouse->ClickFunc)(type, buttons);

	if (type && buttons & WMLEFTKEY)
	{
		if (tick_timer - prevtimer <= MOUSEDBLCLICKTIME)
		{
			highMouse->DoubleClick = true;
		}
		else
		{
			highMouse->DoubleClick = false;
		}
		prevtimer = tick_timer;
		if (highMouse->DblClickFunc)
			(*highMouse->DblClickFunc)();
	}
}
