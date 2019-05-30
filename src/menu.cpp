
#include <stdlib.h>
#include <string.h>
#include <grplib/gr8.h>
#include <grplib/usegrp.h>
#include <grplib/palette.h>

#include "auxil.h"
#include "debug.h"
#include "gener.h"
#include "gr.h"
#include "audio.h"
#include "fonts.h"
#include "vars.h"
#include "key.h"
#include "maps.h"
#include "mytime.h"
#include "mpq.h"
#include "starmap.h"
#include "wmem.h"
#include "menu.h"
#include "pcx.h"
#include "portrait.h"

//#define DECORENABLE
//#define DECORTEXTENABLE

char scrollportrait_menu;
char fadeinout;
char *allscreen;
int menuonbar, ActiveMenuChanges;
MOUSEALLSPOTS *currentmousehottable;

//==========================================
void addnewmousehotpos(void(*func)(int), int maxspots)
{
	currentmousehottable = (MOUSEALLSPOTS *)
		wmalloc(sizeof(MOUSEALLSPOTS) + maxspots * sizeof(MOUSEHOTSPOT));
	currentmousehottable->callbackwork = 0;
	mousehotspot.AddList(currentmousehottable);
	currentmousehottable->maxhotspots = maxspots;
	currentmousehottable->curenthotspots = 0;
	currentmousehottable->mousemovecallback = func;
	highMouse->InstallMoveEvent(&mymousemoveevent);
}
//==========================================
void incrcallbackwork(void)
{
	if (currentmousehottable)
		currentmousehottable->callbackwork++;
}
//==========================================
void decrcallbackwork(void)
{
	if (currentmousehottable)
		currentmousehottable->callbackwork--;
}
//==========================================
void delnewmousehotpos(void)
{
	//delete current hotspot
	int lastelem;
	if (currentmousehottable)
		wfree(currentmousehottable);
	mousehotspot.DelList(currentmousehottable);
	lastelem = mousehotspot.Count();
	if (!lastelem)
	{
		currentmousehottable = NULL;
		highMouse->UninstallMoveEvent();
	}
	else
		currentmousehottable = (MOUSEALLSPOTS *)mousehotspot.GetElemNr(lastelem - 1);
}
//==========================================
void addmousehotpos(int x, int y, int sizex, int sizey, int *dialogflags)
{
	int curentspot;
	if (currentmousehottable)
	{
		curentspot = currentmousehottable->curenthotspots;
		if (curentspot < currentmousehottable->maxhotspots)
		{
			currentmousehottable->spot[curentspot].x = x;
			currentmousehottable->spot[curentspot].y = y;
			currentmousehottable->spot[curentspot].sizex = sizex;
			currentmousehottable->spot[curentspot].sizey = sizey;
			currentmousehottable->spot[curentspot].dialogflags = dialogflags;
			currentmousehottable->curenthotspots++;
		}
	}
}
//==========================================
//coming here from sdl main loop
//
void mymousemoveevent(int x, int y)
{
	int i, found = -1, flags;
	MOUSEHOTSPOT *temp;

	highMouse->FixRestrict(&x, &y);
	if (!currentmousehottable)
		return;
	if (!currentmousehottable->callbackwork)
		return;
	for (i = 0;i < currentmousehottable->curenthotspots;i++)
	{
		temp = &currentmousehottable->spot[i];
		if (temp->dialogflags)
		{
			flags = *temp->dialogflags;
			if (!(flags&DIALOGBIN_FLAGS_ITEMVISIBLED))
				continue;
			if (flags&DIALOGBIN_FLAGS_ITEMDISABLED)
				continue;
			if (!(flags&DIALOGBIN_FLAGS_KEYMOUSERESPONDEVENTS))
				continue;
		}
		if (x >= temp->x && y >= temp->y && x <= temp->x + temp->sizex && y <= temp->y + temp->sizey)
		{
			(*currentmousehottable->mousemovecallback)(i);
			found = i;
			break;
		}
	}
	if (found == -1)
		(*currentmousehottable->mousemovecallback)(NOSELECTMENUBAR);
}
//==========================================
void puttransptilebox(int x, int y, int sizex, int sizey, GRPFILE *grp)
{
	int boxnr, boxline, i, j, sizexgrp, sizeygrp;
	sizexgrp = grp->Picture[0].PixelPerLine;
	sizeygrp = grp->Picture[0].LinesPerPicture;
	sizex /= sizexgrp;
	sizey /= sizeygrp;
	for (i = 0;i < sizey;i++)
	{
		if (i == 0)
			boxline = 0;
		else
			if (i == sizey - 1)
				boxline = 2;
			else
				boxline = 1;
		for (j = 0;j < sizex;j++)
		{
			if (j == 0)
				boxnr = boxline * 3 + 0;
			else
				if (j == sizex - 1)
					boxnr = boxline * 3 + 2;
				else
					boxnr = boxline * 3 + 1;
			GRP_PutGrp(x + j * sizexgrp, y + i * sizeygrp, grp, TRANSPARENT, 255, 0, NULL, boxnr);
		}
	}
}
//==========================================
char *saveundermenu(MENUSTR *menu)
{
	return(savepartscreen(menu->x, menu->y, menu->xsize, menu->ysize));
}
//==========================================
void loadundermenu(MENUSTR *menu, char *undermenubuf)
{
	loadpartscreen(menu->x, menu->y, menu->xsize, menu->ysize, undermenubuf);
}
//==========================================
char *savepartscreen(int x, int y, int sizex, int sizey)
{
	char *scr = (char *)wmalloc(sizex*sizey);
	GRP_GetImage8(x, y, sizex, sizey, scr);
	return(scr);
}
//==========================================
void loadpartscreen(int x, int y, int sizex, int sizey, char *savedscreen)
{
	if (savedscreen)
	{
		GRP_PutImage8(x, y, sizex, sizey, savedscreen);
		wfree(savedscreen);
	}
}
//==========================================
void restorepartscreen(int x, int y, int sizex, int sizey, char *savedscreen)
{
	if (savedscreen)
	{
		GRP_PutImage8(x, y, sizex, sizey, savedscreen);
	}
}
//==========================================
void saveunderitem(MENUSTR *allmenus, int itemnr)
{
	MENUPOS *item;
	item = &allmenus->menu[itemnr];
	if (!item->savedscrunderitem)
		item->savedscrunderitem = savepartscreen(item->hotdeltax, item->hotdeltay, item->hotxsize, item->hotysize);
}
//==========================================
void loadunderitem(MENUSTR *allmenus, int itemnr)
{
	MENUPOS *item;
	item = &allmenus->menu[itemnr];
	loadpartscreen(item->hotdeltax, item->hotdeltay, item->hotxsize, item->hotysize, item->savedscrunderitem);
	item->savedscrunderitem = NULL;
}
//==========================================
void saveunderitems(MENUSTR *allmenus)
{
	int i;
	for (i = 0;i < allmenus->elements;i++)
	{
		saveunderitem(allmenus, i);
	}
}
//==========================================
void loadunderitems(MENUSTR *allmenus, int restoreandfree)
{
	MENUPOS *item;
	int i;

	for (i = allmenus->elements - 1;i >= 0;i--)
	{
		item = &allmenus->menu[i];
		if (restoreandfree || item->restoreatexit == ITEM_RESTOREANDFREE)
		{
			loadunderitem(allmenus, i);
		}
	}
}
//==========================================
void clearscrollportrait(void)
{
	if (scrollportrait)
	{
		scrollportrait = 0;
	}
}
//==========================================
void checkanddrawmenu(MENUSTR *allmenus, int ItemChanges, int saveunder)
{
	int i, showtype, inv;
	for (i = 0;i < allmenus->elements;i++)
	{
		if (allmenus->menu[i].relation.address[ITEMRELATION_DISABLE] != NULL)
		{
			if (*allmenus->menu[i].relation.address[ITEMRELATION_DISABLE] == allmenus->menu[i].relation.value[ITEMRELATION_DISABLE])
				setmenuitem_DISABLED(allmenus, i, TRUE);
			else
				setmenuitem_DISABLED(allmenus, i, FALSE);
		}
	}
	for (i = 0;i < allmenus->elements;i++)
	{
		if (menuitem_ISDISABLED(allmenus, i) || (ItemChanges == ITEMALLDISABLED && (allmenus->menu[i].itemtype == ISBUTTON || allmenus->menu[i].itemtype == ISSMKVIDEO)))
			showtype = ITEMSHOW_DISABLED;
		else
			if (ItemChanges == i)
				showtype = allmenus->vars.BarChanges.color;
			else
				showtype = ITEMSHOW_NOFOCUS;
		allmenus->menu[i].typeofshow = showtype;
	}
	if (saveunder == ITEM_RESTOREANDFREE)
	{
		loadunderitems(allmenus, ITEM_RESTOREANDFREE);
		saveunderitems(allmenus);
	}
	for (i = 0;i < allmenus->elements;i++)
	{
		inv = 0;
		if (!menuitem_ISVISIBLED(allmenus, i))
		{
			inv = 1;
		}
		else
		{
			if (allmenus->menu[i].relation.address[ITEMRELATION_INVISIBLE] != NULL)
				if (*allmenus->menu[i].relation.address[ITEMRELATION_INVISIBLE] == allmenus->menu[i].relation.value[ITEMRELATION_INVISIBLE])
				{
					inv = 1;
				}
		}
		if (!inv)
			drawmenuitem(allmenus, i);
		showitemborders(allmenus, i);
	}
}
//==========================================
void updateonemenuitem(MENUSTR *allmenus, int itemnr)
{
	MENUPOS *item = &allmenus->menu[itemnr];
	if (item->savedscrunderitem)
	{
		loadunderitem(allmenus, itemnr);
		saveunderitem(allmenus, itemnr);
	}
	drawmenuitem(allmenus, itemnr);
}
//==========================================
int actiondblclick(MENUSTR *allmenus, int downmenu)
{
	int itemselected;
	MENUPOS *item;
	LISTBOXBAR	*bar;
	int action;
	if (highMouse->DoubleClick)
	{
		//highMouse->DoubleClick = false;
		item = &allmenus->menu[downmenu];
		switch (item->itemtype)
		{
		case ISLISTBOX:
			action = 0;
			bar = item->item.listbox->bar;
			if (bar)
			{
				if (highMouse->PosX >= bar->xt_1)
					action = 1;
			}
			if (!action)
			{
				if (item->item.listbox->flist)
				{
					if (!item->item.listbox->pixelsbetweenlines)
						return(NOSELECTMENUBAR);
					itemselected = (highMouse->PosY - item->hotdeltay) / item->item.listbox->pixelsbetweenlines;
					if (itemselected >= item->item.listbox->lines)
						break;
					itemselected += item->item.listbox->from;
					if (itemselected < item->item.listbox->flist->Count())
					{
						item->item.listbox->selectednr = itemselected;
						if (allmenus->selectmenusnd)
							Play_sfxdata_id(NULL, allmenus->selectmenusnd, -1, 0);
						return(downmenu);
					}
				}
			}
			else
			{

			}
			break;
		}
	}
	return(NOSELECTMENUBAR);
}
//==========================================
#define MINBETWEENTHESAMECLICK 8
int actionmenuonpressmouse(MENUSTR *allmenus, int downmenu, int *selectedmenu)
{
	static long long prevpresstimer = 0;
	int i, j, itemselected, posx, retstatus, prevvalue, action;
	int expandboxflag;
	MENUPOS *item;
	LISTBOXBAR	*bar;
	char *savedbeforeshowlist;
	int sprnr, xsizegrp, ysizegrp, xsize, ysize, fromspr, typelistshow, color, selitem;
	item = &allmenus->menu[downmenu];
	GRPFILE *dlggrp = allmenus->dlggrp;
	switch (item->itemtype)
	{
	case ISLISTBOX:
		if (item->item.listbox->flist)
		{
			allmenus->defaultlistitem = downmenu;
			action = 0;
			bar = item->item.listbox->bar;
			if (bar)
			{
				if (highMouse->PosX >= bar->xt_1)
					action = 1;
			}
			if (action)
			{
				//need to check if pressed on buttons on bar or horizontalbar
				if (bar->showbutton == ITEMSHOW_CLICK)
				{
					action = 3;//then button bar is active before depressed
				}
				else
				{
					if (highMouse->PosY <= bar->yt_2)
						action = 1;
					else if (highMouse->PosY >= bar->yb_1)
						action = 2;
					else if (highMouse->PosY >= bar->buttonbary1&&highMouse->PosY <= bar->buttonbary2)
						action = 3;
					else
						action = 4;
				}
				switch (action)
				{
				case 1:
					if (tick_timer - prevpresstimer >= MINBETWEENTHESAMECLICK)
					{
						prevpresstimer = tick_timer;
						bar->showtypet = ITEMSHOW_CLICK;
						changelistbox_fromto(allmenus, downmenu, -1);
					}
					break;
				case 2:
					if (tick_timer - prevpresstimer >= MINBETWEENTHESAMECLICK)
					{
						prevpresstimer = tick_timer;
						bar->showtypeb = ITEMSHOW_CLICK;
						changelistbox_fromto(allmenus, downmenu, +1);
					}
					break;
				case 3:
					bar->showbutton = ITEMSHOW_CLICK;
					changelistbox_buttonbarpos(allmenus, downmenu, highMouse->PosY);
					break;
				case 4:
					if (tick_timer - prevpresstimer >= MINBETWEENTHESAMECLICK)
					{
						prevpresstimer = tick_timer;
						changelistbox_buttonbarposdelta(allmenus, downmenu, highMouse->PosY);
					}
					break;
				}
			}
			else
			{
				if (!item->item.listbox->pixelsbetweenlines)
					return(0);
				itemselected = (highMouse->PosY - item->hotdeltay) / item->item.listbox->pixelsbetweenlines;
				if (itemselected >= item->item.listbox->lines)
					break;
				itemselected += item->item.listbox->from;
				if (itemselected < item->item.listbox->flist->Count())
				{
					if (item->item.listbox->selectednr != itemselected)
					{
						item->item.listbox->selectednr = itemselected;
						if (item->item.listbox->onselectitem_callback != NULL)
							item->item.listbox->onselectitem_callback(allmenus, downmenu, itemselected);
						return(1);
					}
				}
			}
		}
		break;
	case ISEXPANDBOX:
		xsize = item->item.expandbox->array[ITEMSHOW_CLICK][0];
		ysize = item->item.expandbox->array[ITEMSHOW_CLICK][1];
		sprnr = item->item.expandbox->array[ITEMSHOW_CLICK][2];
		xsizegrp = dlggrp->Picture[sprnr].PixelPerLine;
		ysizegrp = dlggrp->Picture[sprnr].LinesPerPicture;
		typelistshow = item->item.expandbox->typelistshow;
		if (!item->item.expandbox->showlist)
		{
			if (allmenus->menuflags&MENUFLAGS_ONETICKDRAW)
			{
				expandboxflag = DIALOGBIN_DONOTSAVEUNDERMENU;
			}
			else
			{
				expandboxflag = 0;
				if (typelistshow == 1)
					savedbeforeshowlist = savepartscreen(item->hotdeltax, item->hotdeltay, item->hotxsize, ysize*ysizegrp);
				else
					savedbeforeshowlist = savepartscreen(item->hotdeltax, item->hotdeltay - (ysize*ysizegrp - 1), item->hotxsize, ysize*ysizegrp);
			}
			item->item.expandbox->showlist = createmenuinfo(item->hotdeltax, item->hotdeltay, 0, 0, item->item.expandbox->maxitem, expandboxflag);
			menuspecialtables(item->item.expandbox->showlist, allmenus->fonttable, allmenus->dlggrp);
			//this is need to call correct callback from listmenu
			item->item.expandbox->showlist->prevmenu = allmenus;
			for (i = 0;i < item->item.expandbox->maxitem;i++)
			{
				addtextitem(item->item.expandbox->showlist, i, ISLABELLEFT, 4, (i + 1)*ysizegrp*typelistshow, item->hotxsize, ysizegrp,
					0, 0, item->item.expandbox->items[i], item->fontnr, item->item.expandbox->rowsize, item->colors4);
				setmenuitem_VISIBLED(item->item.expandbox->showlist, i, TRUE);
				setmenuitem_DISABLED(item->item.expandbox->showlist, i, FALSE);
				setmenuitem_RESPONDEVENTS(item->item.expandbox->showlist, i, TRUE);
			}
		}
		fromspr = 2;
		for (i = 0;i < ysize;i++)
			for (j = 0;j < xsize;j++)
			{
				GRP_PutNoPacked(item->hotdeltax + j * xsizegrp, item->hotdeltay + i * ysizegrp*typelistshow,
					allmenus->dlggrp, item->item.expandbox->array[ITEMSHOW_CLICK][fromspr]);
				fromspr++;
			}

		sprnr = item->item.expandbox->arrowbottom[ITEMSHOW_CLICK];
		GRP_PutNoPacked(item->hotdeltax + item->hotxsize - 18, item->hotdeltay + 4, allmenus->dlggrp, sprnr);

		selitem = item->item.expandbox->selectednr;
		color = GETITEMCOLOR(item->colors4, ITEMSHOW_CLICK);
		putmessageinrectangleL(item->hotdeltax + 4, item->hotdeltay, item->hotxsize, item->hotysize, ysizegrp,
			item->item.expandbox->items[selitem], item->fontnr, color, allmenus->fonttable, allmenus->dlggrp, MESINRECT_COLORS, 0);
		if (allmenus->menuflags&MENUFLAGS_ONETICKDRAW)
		{
			allmenus->vars.expanditemnr = downmenu;
			retstatus = showlistmenu_ONETICK(item->item.expandbox->showlist);
		}
		else
		{
			retstatus = showlistmenu(item->item.expandbox->showlist);
			removeallmenuinfo(item->item.expandbox->showlist);
			item->item.expandbox->showlist = NULL;
			if (retstatus >= 0)
			{
				item->item.expandbox->selectednr = retstatus;
			}
			*selectedmenu = downmenu;
			if (typelistshow == 1)
				loadpartscreen(item->hotdeltax, item->hotdeltay, item->hotxsize, ysize*ysizegrp, savedbeforeshowlist);
			else
				loadpartscreen(item->hotdeltax, item->hotdeltay - (ysize*ysizegrp - 1), item->hotxsize, ysize*ysizegrp, savedbeforeshowlist);
		}
		return(1);
	case ISHORIZBUTTON:
		if (tick_timer - prevpresstimer >= MINBETWEENTHESAMECLICK)
		{
			prevpresstimer = tick_timer;
			posx = highMouse->PosX - (item->hotdeltax + (item->hotdeltax - item->item.horizbutton->sizexbar) / 2);
			prevvalue = gethorizvalue(allmenus, downmenu);
			sethorizbuttonposfrommouse(allmenus, downmenu, posx);
		}
		return(1);
	case ISSMKVIDEO:
		return(1);
	case ISIMAGE:
		return(1);
	}
	return(0);
}
//==========================================
void activemenuupdatevalues(MENUSTR *allmenus, int activeitem)
{

	if (fadeinout)
	{
		fadeinout = 0;
		for (int i = 0;i < allmenus->elements;i++)
		{
			if (i == activeitem)
			{
				allmenus->menu[i].showvalue += allmenus->incrementfactor;
				if (allmenus->menu[i].showvalue > MENUITEM_TIMEACTIVE_MAXVALUE)
					allmenus->menu[i].showvalue = MENUITEM_TIMEACTIVE_MAXVALUE;
			}
			else
			{
				allmenus->menu[i].showvalue -= allmenus->incrementfactor;
				if (allmenus->menu[i].showvalue < MENUITEM_TIMEACTIVE_MINVALUE)
					allmenus->menu[i].showvalue = MENUITEM_TIMEACTIVE_MINVALUE;
			}
		}
	}
}
//==========================================
void setmenuflags(MENUSTR *allmenus, unsigned int flags)
{
	allmenus->menuflags |= flags;
}
//==========================================
void calcmenuitemsizes(MENUSTR *allmenus)
{
	int i;
	for (i = 0;i < allmenus->elements;i++)
	{
		allmenus->menu[i].xtextpos = allmenus->menu[i].hotdeltax + allmenus->menu[i].xtextdelta;
		allmenus->menu[i].ytextpos = allmenus->menu[i].hotdeltay + allmenus->menu[i].ytextdelta;
	}
}
//==========================================
int drawmenu(MENUSTR *allmenus, int flags)
{
	int updateunder, exitcallback;
	int cursorblink = getcursorblinktype();
	int prevmenuonbar, downmenu, redrawbar, editboxchanges, needredraw, selectedmenu;
	int activeitemchange, activeitem, prevactiveitem = -1000;
	char *prev_entire_screen;
	MENUACTIVE = 1;
	if (allmenus->prevmenu)
	{
		prev_entire_screen = savepartscreen(0, 0, gameconf.grmode.x, gameconf.grmode.y);
		updateunder = ITEM_NOSAVELOADUNDER;
	}
	else
	{
		updateunder = ITEM_RESTOREANDFREE;
	}
	downmenu = NOSELECTMENUBAR;
	selectedmenu = NOSELECTMENUBAR;
	prevmenuonbar = menuonbar;
	menuonbar = NOSELECTMENUBAR;
	needredraw = 0;
	mainController.KeyActive = 0;
	mainController.EventsLoop();
	mainController.KeysBuffer->Flush();
	regmenu(allmenus->x, allmenus->y, allmenus, &getmouseonmenubar);//set menu hotcoord for mouse
	scrollportrait_menu = scrollportrait;
	clearscrollportrait();
	if (allmenus->prevmenu)//if exist prev menu need to show it only
	{
		checkanddrawmenu(allmenus->prevmenu, ITEMALLDISABLED, updateunder);
	}
	//	  saveunderitems(allmenus);
	mymousemoveevent(highMouse->PosX, highMouse->PosY);
	checkanddrawmenu(allmenus, ITEMNOONEACTIVE, updateunder);
	if (!(flags & MENUFLAGS_BYPASS))
	{
		highMouse->RefreshMouseType(map.MAPXGLOBAL, map.MAPYGLOBAL);
		do {
			mainController.EventsLoop();
			getmouseonitem(&activeitemchange, &activeitem);
			highMouse->SaveImageUnder();
			highMouse->DrawMouse();
			mainController.UpdateScreen();
			highMouse->LoadImageUnder();

			selectedmenu = menukeys(allmenus, &redrawbar, &needredraw);
			editboxchanges = editboxaction(allmenus);
			if (editboxchanges == 2)
			{
				needredraw = 1;
				selectedmenu = allmenus->DefaultTextBox;
			}
			if (selectedmenu >= 0)
			{
				//if some key unpressed
				if (selectedmenu == CANCELFROMMENU)
				{
					if (allmenus->selectmenusnd)
						Play_sfxdata_id(NULL, allmenus->selectmenusnd, -1, 0);
					break;
				}
				switch (allmenus->menu[selectedmenu].itemtype)
				{
				case ISBUTTON:
				case ISSMKVIDEO:
					if (allmenus->selectmenusnd)
						Play_sfxdata_id(NULL, allmenus->selectmenusnd, -1, 0);
					break;
				case ISCHECKBOX:
					changecheckboxstate(allmenus, selectedmenu);
					break;
				case ISRADIOBUTTON:
					setradiobuttonstate(allmenus, selectedmenu);
					break;
				}
				break;
			}
			if (redrawbar >= 0)
			{
				//pressed specific key
				activeitemchange = 1;
				allmenus->vars.BarChanges.bar = redrawbar;
				allmenus->vars.BarChanges.color = ITEMSHOW_CLICK;
			}
			else  //if from mouse action
			{
				if (activeitemchange) //move moved on other menu
				{
					allmenus->vars.BarChanges.bar = activeitem;
					allmenus->vars.BarChanges.color = ITEMSHOW_FOCUS;
					if (activeitem >= 0)
					{
						if (allmenus->menu[activeitem].itemtype == ISBUTTON ||
							(allmenus->menu[activeitem].itemtype == ISSMKVIDEO &&
								allmenus->menu[activeitem].item.smkvideo->smkbutton))
						{
							if (allmenus->mouseoversnd)
							{
								//prevent sound yhem mounse move on the same item
								if (prevactiveitem != activeitem)
								{
									prevactiveitem = activeitem;
									Play_sfxdata_id(NULL, allmenus->mouseoversnd, -1, 0);
								}
							}
						}
					}
					else
						prevactiveitem = -1000;
				}
				if (highMouse->GetButtonStatus() & WMLEFTKEY)
				{
					if (activeitem >= 0)
					{
						//							if (allmenus->menu[activeitem].dialogbin_flags&DIALOGBIN_FLAGS_KEYMOUSERESPONDEVENTS)
						//								setdefaultbutton(allmenus,activeitem);
						activeitemchange = 1;
						if (downmenu == NOSELECTMENUBAR)
							downmenu = activeitem;
						else
						{
							allmenus->vars.BarChanges.bar = activeitem;
							allmenus->vars.BarChanges.color = ITEMSHOW_CLICK;
						}
					}
					if (downmenu >= 0)
					{
						selectedmenu = actiondblclick(allmenus, downmenu);
						if (activeitem == downmenu)
							needredraw = actionmenuonpressmouse(allmenus, downmenu, &selectedmenu);
					}
				}
				else
				{
					if (downmenu != NOSELECTMENUBAR)
					{
						if (downmenu == activeitem)
						{
							needredraw = 1;
							switch (allmenus->menu[activeitem].itemtype)
							{
							case ISBUTTON:
							case ISSMKVIDEO:
								selectedmenu = activeitem;
								if (allmenus->selectmenusnd)
									Play_sfxdata_id(NULL, allmenus->selectmenusnd, -1, 0);
								break;
							case ISCHECKBOX:
								selectedmenu = activeitem;
								changecheckboxstate(allmenus, activeitem);
								break;
							case ISRADIOBUTTON:
								selectedmenu = activeitem;
								setradiobuttonstate(allmenus, activeitem);
								break;
							case ISLISTBOX:
								if (allmenus->menu[downmenu].item.listbox)
									if (allmenus->menu[downmenu].item.listbox->bar)
									{
										allmenus->menu[downmenu].item.listbox->bar->showtypet = ITEMSHOW_NOFOCUS;
										allmenus->menu[downmenu].item.listbox->bar->showtypeb = ITEMSHOW_NOFOCUS;
										allmenus->menu[downmenu].item.listbox->bar->showbutton = ITEMSHOW_NOFOCUS;
									}
								break;
							}
						}
						else
						{
							needredraw = 1;
							downmenu = NOSELECTMENUBAR;
						}
					}
				}
			}
			if (allmenus->DefaultTextBox >= 0)
			{
				if (allmenus->menu[allmenus->DefaultTextBox].itemtype == ISEDITBOX)
				{
					if (cursorblink != getcursorblinktype())
					{
						cursorblink = getcursorblinktype();
						updateonemenuitem(allmenus, allmenus->DefaultTextBox);
					}
				}
			}
			if (editboxchanges || needredraw || activeitemchange || (allmenus->menuflags&MENUFLAGS_ALWAYSDRAW))
			{
				scrollportrait_menu = scrollportrait;
				clearscrollportrait();
				if (allmenus->prevmenu)//if exist prev menu need to show it only
				{
					restorepartscreen(0, 0, gameconf.grmode.x, gameconf.grmode.y, prev_entire_screen);
					checkanddrawmenu(allmenus->prevmenu, ITEMALLDISABLED, updateunder);
					RunCallBackFuncs(allmenus->prevmenu);
				}
				checkanddrawmenu(allmenus, allmenus->vars.BarChanges.bar, updateunder);
				needredraw = 0;
				setmouseonitem(0);
			}
			activemenuupdatevalues(allmenus, activeitem);
			exitcallback = RunCallBackFuncs(allmenus);
			if (exitcallback)
			{
				selectedmenu = exitcallback;
				break;
			}
		} while (selectedmenu == NOSELECTMENUBAR);
	}//flags
	//highMouse->FlushPos();
	loadunderitems(allmenus, ITEM_ONLYFREE);
	unregmenucallbacks();
	mymousemoveevent(highMouse->PosX, highMouse->PosY);
	if (allmenus->prevmenu)
		loadpartscreen(0, 0, gameconf.grmode.x, gameconf.grmode.y, prev_entire_screen);
	MENUACTIVE = 0;
	return(selectedmenu);
}
//==========================================
int showlistmenu(MENUSTR *allmenus)
{
	int prevmenuonbar, downmenu, prevselmenu, ret, selectedmenu = NOSELECTMENUBAR;
	int activeitemchange, activeitem;
	BAR prevBar;
	prevBar.bar = allmenus->vars.BarChanges.bar;
	prevBar.color = allmenus->vars.BarChanges.color;
	downmenu = NOSELECTMENUBAR;
	prevselmenu = selectedmenu;
	prevmenuonbar = menuonbar;
	selectedmenu = NOSELECTMENUBAR;
	menuonbar = NOSELECTMENUBAR;
	mainController.EventsLoop();
	regmenu(allmenus->x, allmenus->y, allmenus, &getmouseonmenubar);						//set menu hotcoord for mouse
	saveunderitems(allmenus);
	mymousemoveevent(highMouse->PosX, highMouse->PosY);
	scrollportrait_menu = scrollportrait;
	clearscrollportrait();
	checkanddrawmenu(allmenus, ITEMNOONEACTIVE, ITEM_RESTOREANDFREE);
	highMouse->RefreshMouseType(map.MAPXGLOBAL, map.MAPYGLOBAL);
	do {
		getmouseonitem(&activeitemchange, &activeitem);
		mainController.EventsLoop();
		if (allmenus->prevmenu)
			RunCallBackFuncs(allmenus->prevmenu);
		highMouse->SaveImageUnder();
		highMouse->DrawMouse();
		mainController.UpdateScreen();
		highMouse->LoadImageUnder();

		allmenus->vars.BarChanges.bar = NOSELECTMENUBAR;
		if (activeitemchange) //move moved on other menu
		{
			allmenus->vars.BarChanges.bar = activeitem;
			allmenus->vars.BarChanges.color = ITEMSHOW_FOCUS;
		}
		if (highMouse->GetButtonStatus() & WMLEFTKEY)
		{
			if (activeitem >= 0)
			{
				activeitemchange = 1;
				if (downmenu == NOSELECTMENUBAR)
					downmenu = activeitem;
				else
				{
					allmenus->vars.BarChanges.bar = activeitem;
					allmenus->vars.BarChanges.color = ITEMSHOW_CLICK;
				}
			}
		}
		else
		{
			if (activeitem != NOSELECTMENUBAR)
				selectedmenu = activeitem;
			else
				selectedmenu = NOSELECTLIST;
		}
		if (activeitemchange)
		{
			scrollportrait_menu = scrollportrait;
			clearscrollportrait();
			checkanddrawmenu(allmenus, allmenus->vars.BarChanges.bar, ITEM_RESTOREANDFREE);
			setmouseonitem(0);
		}
	} while (selectedmenu == NOSELECTMENUBAR);
	//highMouse->FlushPos();
	loadunderitems(allmenus, ITEM_ONLYFREE);
	unregmenucallbacks();
	ret = selectedmenu;
	selectedmenu = prevselmenu;
	menuonbar = prevmenuonbar;
	allmenus->vars.BarChanges.bar = prevBar.bar;
	allmenus->vars.BarChanges.color = prevBar.color;
	return(ret);
}
//==========================================
void setmouseonitem(int activeitemchanges)//,int activeitem)
{
	ActiveMenuChanges = activeitemchanges;
}
//==========================================
void getmouseonitem(int *activeitemchanges, int *activeitem)
{
	*activeitemchanges = ActiveMenuChanges;
	*activeitem = menuonbar;
}
//==========================================
void getmouseonmenubar(int value)
{
	ActiveMenuChanges = 1;						//check mouse position every tick
	menuonbar = value;
}
//==========================================
void unregmenucallbacks(void)
{
	decrcallbackwork();
	delnewmousehotpos();
}
//==========================================
void regmenu(int x, int y, MENUSTR *allmenus, void(*func)(int))
{
	int i, elements = 0;
	if (allmenus->elements <= 0)
		return;
	//find non enabled buttons
	for (i = 0;i < allmenus->elements;i++)
	{
		elements++;
	}
	addnewmousehotpos(func, elements);
	calcmenuitemsizes(allmenus);
	for (i = 0;i < allmenus->elements;i++)
	{
		if (allmenus->menu[i].itemtype != ISSMKVIDEO)
		{
			addmousehotpos(allmenus->menu[i].hotdeltax, allmenus->menu[i].hotdeltay,
				allmenus->menu[i].hotxsize, allmenus->menu[i].hotysize, &allmenus->menu[i].dialogbin_flags);
		}
		else
		{
			addmousehotpos(allmenus->menu[i].hotdeltax, allmenus->menu[i].hotdeltay,
				allmenus->menu[i].textxsize, allmenus->menu[i].textysize, &allmenus->menu[i].dialogbin_flags);
		}
	}
	incrcallbackwork();
}
//==========================================
// the buttontype 0,1,2,3 (left,full,right,middle)
//						  0 - 121d,122d,123d,124e,125e,126e,127c,128c,129c
//						  1 - 112d,113d,114d,115e,116e,117e,118c,119c,120c
//						  2 - 103d,104d,105d,106e,107e,108e,109c,110c,111c
//						  3 - 123d,122d,123d,126e,125e,126e,129c,128c,129c
//letter d - disabled,e-enabled,c-clicked
//						buttontype-3,buttonstate-4(disable,nofocus,click,focus),butonpos-4(begin,middle,end)
//BUTTONPORTIONSIZE(xsize,ysize)( 8,28)
unsigned char BUTTONDECOR[4][4][3] = {
							{{121,122,123},{124,125,126},{127,128,129},{124,125,126}},
							{{112,113,114},{115,116,117},{118,119,120},{115,116,117}},
							{{103,104,105},{106,107,108},{109,110,111},{106,107,108}},
							{{103,122,123},{106,125,126},{109,128,129},{106,125,126}},
};
//==========================================
void drawmenuitem(MENUSTR *allmenus, int itemnr)
{
	switch (allmenus->menu[itemnr].itemtype)
	{
	case ISBUTTON:
		drawbuttonitem(allmenus, itemnr);
		break;
	case ISLISTBOX:
		drawlistboxitem(allmenus, itemnr);
		break;
	case ISEDITBOX:
		draweditboxitem(allmenus, itemnr);
		break;
	case ISLABELLEFT:
	case ISLABELCENTER:
	case ISLABELRIGHT:
		drawtextitem(allmenus, itemnr);
		break;
	case ISEXPANDBOX:
		drawexpanditem(allmenus, itemnr);
		break;
	case ISCHECKBOX:
		drawcheckboxitem(allmenus, itemnr);
		break;
	case ISRADIOBUTTON:
		drawradiobuttonitem(allmenus, itemnr);
		break;
	case ISHORIZBUTTON:
		drawhorizbutton(allmenus, itemnr);
		break;
	case ISSMKVIDEO:
		drawsmkvideo(allmenus, itemnr);
		break;
	case ISIMAGE:
		drawimageitem(allmenus, itemnr);
		break;
	}
	if (!(allmenus->menu[itemnr].dialogbin_flags & DIALOGBIN_FLAGS_NODLGGRP))
		drawdecorate(allmenus, itemnr);
}
//==========================================
int editboxaction(MENUSTR *allmenus)
{
	unsigned short keypressed;
	int activeitem;
	if (!mainController.KeysBuffer->IsEmpty())
	{
		//if keypressed and we have editbox add to edit string
		keypressed = mainController.KeysBuffer->PopElem();
		if (allmenus->DefaultTextBox >= 0)
		{
			MENUPOS *curitem = &allmenus->menu[allmenus->DefaultTextBox];
			if (curitem->itemtype != ISEDITBOX)
			{
				return(0);
			}
			int curlength = curitem->item.editbox->length;
			switch (keypressed)
			{
			case BACKSPACEKEY:
				if (curlength)
				{
					curlength--;
					curitem->item.editbox->editstr[curlength] = 0;
				}
				break;
			case ENTERKEY:
			case ENTERKEY2:
				activeitem = allmenus->defaultbutton;
				if (activeitem >= 0)
				{
					if (menuitem_ISDISABLED(allmenus, activeitem) ||
						!menuitem_ISVISIBLED(allmenus, activeitem))
					{
						break;
					}
				}
				if (curlength >= 0)
				{
					return(2);
				}
				break;
			default:
				if (keypressed < ' ' || keypressed > '~')
					return(0);
				if (curlength < curitem->item.editbox->maxsymbols)
				{
					curitem->item.editbox->editstr[curlength] = (unsigned char) keypressed;
					curitem->item.editbox->editstr[curlength + 1] = 0;
					curlength++;
				}
				break;
			}
			curitem->item.editbox->length = curlength;
			mainController.KeyActive = 0;
			return(1);
		}
	}
	return(0);
}
//==========================================
int menukeys(MENUSTR *allmenus, int *pressed, int *needredraw)
{

	int i = 0, tempkey, changelist = 0;
	static int prevkey = 0;
	*pressed = NOSELECTMENUBAR;
	if (mainController.KeyActive)
	{
		if (mainController.KeyActive == TABKEY) //need to parse all elements to need next active and decorated and responce to mouse&key events
		{
			for (i = allmenus->defaultbutton + 1;i < allmenus->elements;i++)
				if (!menuitem_ISDISABLED(allmenus, i) && menuitem_ISVISIBLED(allmenus, i))
					if (allmenus->menu[i].dialogbin_flags & DIALOGBIN_FLAGS_KEYMOUSERESPONDEVENTS)
					{
						setdefaultbutton(allmenus, i);
						if (needredraw)
							*needredraw = 1;
						mainController.KeyActive = 0;
						return NOSELECTMENUBAR;
					}
			for (i = 0;i < allmenus->defaultbutton;i++)
				if (!menuitem_ISDISABLED(allmenus, i) && menuitem_ISVISIBLED(allmenus, i))
					if (allmenus->menu[i].dialogbin_flags & DIALOGBIN_FLAGS_KEYMOUSERESPONDEVENTS)
					{
						setdefaultbutton(allmenus, i);
						mainController.KeyActive = 0;
						if (needredraw)
							*needredraw = 1;
						return NOSELECTMENUBAR;
					}
		}
		if (((allmenus->defaultbutton >= 0 && allmenus->menu[allmenus->defaultbutton].itemtype != ISEDITBOX) ||
			allmenus->defaultbutton < 0) ||
			(allmenus->menu[allmenus->defaultbutton].itemtype == ISEDITBOX && mainController.KeyActive == ESCAPEKEY))
		{
			prevkey = mainController.KeyActive;
			switch (mainController.KeyActive)
			{
			case ENTERKEY:
			case ENTERKEY2:
				if (allmenus->menu[allmenus->defaultbutton].itemtype == ISBUTTON ||
					allmenus->menu[allmenus->defaultbutton].itemtype == ISLISTBOX)
				{
					if (menuitem_ISENABLED(allmenus, allmenus->defaultbutton))
					{
						*pressed = allmenus->defaultbutton;
					}
					else
						prevkey = 0;
				}
				break;
			case TABKEY: //need to parse all elements to need next active and decorated and responce to mouse&key events
				for (i = allmenus->defaultbutton + 1;i < allmenus->elements;i++)
					if (!menuitem_ISDISABLED(allmenus, i) && menuitem_ISVISIBLED(allmenus, i))
						if (allmenus->menu[i].dialogbin_flags & DIALOGBIN_FLAGS_KEYMOUSERESPONDEVENTS)
						{
							setdefaultbutton(allmenus, i);
							return NOSELECTMENUBAR;
						}
				for (i = 0;i < allmenus->defaultbutton;i++)
					if (!menuitem_ISDISABLED(allmenus, i) && menuitem_ISVISIBLED(allmenus, i))
						if (allmenus->menu[i].dialogbin_flags & DIALOGBIN_FLAGS_KEYMOUSERESPONDEVENTS)
						{
							setdefaultbutton(allmenus, i);
							return NOSELECTMENUBAR;
						}
				break;
			case UPKEY:
				if (allmenus->defaultbutton >= 0)
					if (allmenus->menu[allmenus->defaultbutton].itemtype == ISLISTBOX)
					{
						if (allmenus->menu[allmenus->defaultbutton].item.listbox->bar)
							allmenus->menu[allmenus->defaultbutton].item.listbox->bar->showtypet = ITEMSHOW_CLICK;
						changelist = changelistbox_selectednr(allmenus, allmenus->defaultbutton, -1);
						if (changelist)
							*pressed = allmenus->defaultbutton;
						mainController.KeyActive = 0;
					}
				break;
			case DOWNKEY:
				if (allmenus->defaultbutton >= 0)
					if (allmenus->menu[allmenus->defaultbutton].itemtype == ISLISTBOX)
					{
						if (allmenus->menu[allmenus->defaultbutton].item.listbox->bar)
							allmenus->menu[allmenus->defaultbutton].item.listbox->bar->showtypeb = ITEMSHOW_CLICK;
						changelist = changelistbox_selectednr(allmenus, allmenus->defaultbutton, +1);
						if (changelist)
							*pressed = allmenus->defaultbutton;
						mainController.KeyActive = 0;
					}
				break;
			default:
				if (allmenus->DefaultTextBox >= 0)
					break;
				for (i = 0;i < allmenus->elements;i++)
					if (allmenus->menu[i].hotkey == mainController.KeyActive)
					{
						if (menuitem_ISENABLED(allmenus, i) && menuitem_ISVISIBLED(allmenus, i))
							*pressed = i;
						else
							prevkey = 0;
						break;
					}
				break;
			}
		}
	}
	else
	{
		if (prevkey)
		{
			tempkey = prevkey;
			prevkey = 0;
			switch (tempkey)
			{
			case ESCAPEKEY:
				return CANCELFROMMENU;
				break;
			case ENTERKEY:
			case ENTERKEY2:
				if (allmenus->menu[allmenus->defaultbutton].dialogbin_flags & DIALOGBIN_FLAGS_ITEMDISABLED)
					break;
				else
					if (!(allmenus->menu[allmenus->defaultbutton].dialogbin_flags & DIALOGBIN_FLAGS_ITEMVISIBLED))
						break;
				return (allmenus->defaultbutton);
			case UPKEY:
				if (allmenus->defaultbutton >= 0)
					if (allmenus->menu[allmenus->defaultbutton].itemtype == ISLISTBOX)
					{
						if (allmenus->menu[allmenus->defaultbutton].item.listbox->bar)
							allmenus->menu[allmenus->defaultbutton].item.listbox->bar->showtypet = ITEMSHOW_NOFOCUS;
					}
				if (allmenus->defaultlistitem >= 0)
				{
					changelist = changelistbox_selectednr(allmenus, allmenus->defaultlistitem, -1);
					if (changelist)
						*pressed = allmenus->defaultlistitem;
				}
				break;
			case DOWNKEY:
				if (allmenus->defaultbutton >= 0)
					if (allmenus->menu[allmenus->defaultbutton].itemtype == ISLISTBOX)
					{
						if (allmenus->menu[allmenus->defaultbutton].item.listbox->bar)
							allmenus->menu[allmenus->defaultbutton].item.listbox->bar->showtypeb = ITEMSHOW_NOFOCUS;
					}
				if (allmenus->defaultlistitem >= 0)
				{
					changelist = changelistbox_selectednr(allmenus, allmenus->defaultlistitem, +1);
					if (changelist)
						*pressed = allmenus->defaultlistitem;
				}
				break;
			case PAGEUPKEY:
				break;
			case PAGEDOWNKEY:
				break;
			default:
				if (allmenus->DefaultTextBox >= 0)
					break;
				for (i = 0;i < allmenus->elements;i++)
					if (menuitem_ISENABLED(allmenus, i) && menuitem_ISVISIBLED(allmenus, i))
						if (allmenus->menu[i].hotkey == tempkey)
							return i;
				break;
			}
		}
	}
	return NOSELECTMENUBAR;
}
//==========================================
void menuspecialtables(MENUSTR *menu, char *fonttable, GRPFILE *dlggrp)
{
	menu->fonttable = fonttable;
	menu->dlggrp = dlggrp;
}
//==========================================
MENUSTR *createmenuinfo(int xpos, int ypos, int xsize, int ysize, int nrofelems, int flags)
{
	MENUSTR *menu;
	menu = (MENUSTR *)wmalloc(sizeof(MENUSTR) + nrofelems * sizeof(MENUPOS));
	memset(menu, 0, sizeof(MENUSTR) + nrofelems * sizeof(MENUPOS));
	menu->elements = nrofelems;
	menuspecialtables(menu, NULL, NULL);
	setdefaultbutton(menu, NOSELECTMENUBAR);
	menu->defaultlistitem = -1;
	menu->x = xpos;
	menu->y = ypos;
	menu->xsize = xsize;
	menu->ysize = ysize;
	if (flags & DIALOGBIN_DONOTSAVEUNDERMENU)
	{
		setmenuflags(menu, MENUFLAGS_ONETICKDRAW);
	}
	else
	{
		if (xsize * ysize)
			menu->saveunder = savepartscreen(menu->x, menu->y, menu->xsize, menu->ysize);
	}
	setincrementvalue(menu, 48);
	return(menu);
}
//==========================================
void RemoveMenu_Callback(MENUSTR *allmenus)
{
	if (allmenus->CallBackInfo)
	{
		allmenus->CallBackInfo->CallBackFunc.FlushList();
		allmenus->CallBackInfo->CallBackData.FlushList();
		wfree(allmenus->CallBackInfo);
		allmenus->CallBackInfo = NULL;
	}
}
//==========================================
void AddMenu_SomeCallback(MENUSTR *allmenus, int(*func)(MENUSTR *, void *), void *somecallbackdata)
{
	if (!allmenus->CallBackInfo)
	{
		allmenus->CallBackInfo = (CALLBACKINFO *)wmalloc(sizeof(CALLBACKINFO));
		memset(allmenus->CallBackInfo, 0, sizeof(CALLBACKINFO));
	}
	allmenus->CallBackInfo->CallBackFunc.AddList((void *)func);
	allmenus->CallBackInfo->CallBackData.AddList(somecallbackdata);
}
//==========================================
void AddPrevMenuShowing(MENUSTR *allmenus, MENUSTR *prevmenu)
{
	allmenus->prevmenu = prevmenu;
}
//==========================================
void addsoundstomenu(MENUSTR *allmenus, short snd1, short snd2)
{
	allmenus->selectmenusnd = snd1;
	allmenus->mouseoversnd = snd2;
}
//==========================================
void delmenuitem(MENUSTR *allmenus, int i)
{
	if (allmenus->menu[i].savedscrunderitem)
		wfree(allmenus->menu[i].savedscrunderitem);
	switch (allmenus->menu[i].itemtype)
	{
	case ISBUTTON:
		delbuttonitem(allmenus, i, 1);
		break;
	case ISLISTBOX:
		dellistboxitem(allmenus, i, 1, MENUITEM_LISTBOX_DEALLOCLIST);
		break;
	case ISEDITBOX:
		deleditboxitem(allmenus, i, 1);
		break;
	case ISLABELLEFT:
	case ISLABELCENTER:
	case ISLABELRIGHT:
		deltextitem(allmenus, i, 1);
		break;
	case ISEXPANDBOX:
		delexpanditem(allmenus, i, 1);
		break;
	case ISCHECKBOX:
		delcheckboxitem(allmenus, i, 1);
		break;
	case ISRADIOBUTTON:
		delradiobuttonitem(allmenus, i, 1);
		break;
	case ISHORIZBUTTON:
		delhorizbuttonitem(allmenus, i, 1);
		break;
	case ISSMKVIDEO:
		delsmkbuttonitem(allmenus, i, 1);
		delsmkvideoitem(allmenus, i, 1);
		break;
	case ISIMAGE:
		delimageitem(allmenus, i, 1);
		break;
	}
}
//==========================================
void removeallmenuinfo(MENUSTR *allmenus)
{
	int i;
	for (i = allmenus->elements - 1;i >= 0;i--)
	{
		delmenuitem(allmenus, i);
	}
	if (!(allmenus->menuflags&MENUFLAGS_ONETICKDRAW))
		loadpartscreen(allmenus->x, allmenus->y, allmenus->xsize, allmenus->ysize, allmenus->saveunder);
	RemoveMenu_Callback(allmenus);
	wfree(allmenus);
}
//==========================================
int RunCallBackFuncs(MENUSTR *allmenus)
{
	int i, retval = 0, ret, max;
	int(*callbackadr)(MENUSTR *, void *);
	void *callbackdata;
	if (allmenus->CallBackInfo)
	{
		max = allmenus->CallBackInfo->CallBackFunc.Count();
		for (i = 0;i < max;i++)
		{
			callbackadr = (int(*)(MENUSTR *, void *))allmenus->CallBackInfo->CallBackFunc.GetElemNr(i);
			callbackdata = allmenus->CallBackInfo->CallBackData.GetElemNr(i);
			ret = (*callbackadr)(allmenus, callbackdata);
			if (!i)
				retval = ret;
		}
	}
	return(retval);
}
//==========================================
void restoreflag(MENUSTR *allmenus, int itemnr, int restoreflag)
{
	allmenus->menu[itemnr].restoreatexit = restoreflag;
}
//==========================================
void changemenunr(MENUSTR *allmenus, int newnrofitems)
{
	allmenus->elements = newnrofitems;
}
//==========================================
void setdefaultbutton(MENUSTR *allmenus, int nrofitem)
{
	allmenus->defaultbutton = nrofitem;
}
//==========================================
void SetDefaultTextBox(MENUSTR *allmenus, int nrofitem)
{
	allmenus->DefaultTextBox = nrofitem;
}
//==========================================
void setitemrelation(MENUSTR *allmenus, int itemnr, int relation_type, int *address, int addressvalue)
{
	allmenus->menu[itemnr].relation.address[relation_type] = address;
	allmenus->menu[itemnr].relation.value[relation_type] = addressvalue;
}
//==========================================
int getexpandbox_selecteditem(MENUSTR *allmenus, int nr)
{
	if (allmenus->menu[nr].itemtype == ISEXPANDBOX)
		return(allmenus->menu[nr].item.expandbox->selectednr);
	DEBUGMESSCR("leak detected!\n");
	return(-1);
}
//==========================================
void selectdecorobj(MENUSTR *allmenus, int nr, unsigned int colors4)
{
	//	  allmenus->menu[itemnr].dialogbin_flags|=respondevents;
	allmenus->menu[nr].decorcolors4 = colors4;
}
//==========================================
char BUTTONCOLORS[4][2] = {
						{0,0},			//disables colors4
						{3,1},			//nofocus colors4
						{2,2},			//click colors4
						{3,3}			//focus colors4
};
//==========================================
void setbuttonflags(MENUSTR *allmenus, int nr, int flags)
{
	if (allmenus->menu[nr].itemtype == ISBUTTON)
	{
		MENUPOS *menuitem = &allmenus->menu[nr];
		menuitem->item.button->buttonflags = flags;
	}
	else
		DEBUGMESSCR("leak detected!\n");
}
//==========================================
void parse_special_text(char *str, int len, int colors4, char buttoncolors[])
{
	int xormask = 0, j;
	for (j = 0;j < len;j++)
	{
		if (str[j] == '\r' || (str[j] < 8 && (!xormask)))
		{
			str[j] = GETITEMCOLOR(colors4, buttoncolors[0]);
			xormask ^= 1;
		}
		else
			if (str[j] == '\v' || (str[j] < 8 && xormask))
			{
				str[j] = GETITEMCOLOR(colors4, buttoncolors[1]);
				xormask ^= 1;
			}
	}
}
//==========================================
										// the buttontype 0,1,2	  0 - 103d,104d,105d,106e,107e,108e,109c,110c,111c
										//						  1 - 112d,113d,114d,115e,116e,117e,118c,119c,120c
										//						  2 - 121d,122d,123d,124e,125e,126e,127c,128c,129c
										//letter d - disabled,e-enabled,c-clicked
void addbuttonitem(MENUSTR *allmenus, int nr, int hotx, int hoty,
	int hotsizex, int hotsizey, int texty, char hotkey,
	int fontnr, char *text, unsigned int colors4, int buttonsize, int buttontype)
{
	int textsizex, i, len;
	MENUPOS *menuitem = &allmenus->menu[nr];
	if (menuitem->item.button)
	{
		printf("lost detected %d item\n", nr);
	}
	menuitem->item.button = (BUTTON *)wmalloc(sizeof(BUTTON));
	memset(menuitem->item.button, 0, sizeof(BUTTON));
	menuitem->itemtype = ISBUTTON;
	menuitem->colors4 = colors4;
	menuitem->hotdeltax = hotx + allmenus->x;
	menuitem->hotdeltay = hoty + allmenus->y;
	menuitem->hotxsize = hotsizex;
	menuitem->hotysize = hotsizey;
	getmessagelen(fontnr, text, &textsizex, NULL);
	menuitem->xtextdelta = (menuitem->hotxsize - textsizex) / 2;
	menuitem->ytextdelta = texty;
	menuitem->fontnr = fontnr;
	menuitem->hotkey = hotkey;

	menuitem->item.button->buttonsize = buttonsize;
	menuitem->item.button->buttontype = buttontype;
	//	  menuitem->item.button->buttonflags=BF_TEXTSIMPLE;
	len = strlen(text);
	for (i = 0;i < ITEMSHOW_MAXPOS;i++)
	{
		menuitem->item.button->menustr.text[i] = (char *)wmalloc(len + 1);
		strcpy(menuitem->item.button->menustr.text[i], text);
		parse_special_text(menuitem->item.button->menustr.text[i], len, colors4, BUTTONCOLORS[i]);
	}
	if (menuitem->savedscrunderitem)
	{
		loadunderitem(allmenus, nr);
		saveunderitem(allmenus, nr);
	}
	//	  setmenuitem_DISABLED(allmenus,nr,FALSE);
	restoreflag(allmenus, nr, ITEM_ONLYFREE);
}
//==========================================
void drawbuttonitem(MENUSTR *allmenus, int itemnr)
{
	int buttonsize, sprnr0, sprnr1, sprnr2, sizexgrp, i, buttontype;
	GRPFILE *dlggrp = allmenus->dlggrp;
	MENUPOS *menuitem = &allmenus->menu[itemnr];
	char *fonttable = allmenus->fonttable;
	int showtype = menuitem->typeofshow;
	int colors4 = menuitem->colors4;
	int glowoffset;

	buttonsize = menuitem->item.button->buttonsize;
	if (buttonsize > 0)//have button decoration
	{
		buttontype = menuitem->item.button->buttontype;
		sprnr0 = BUTTONDECOR[buttontype][showtype][0];
		sprnr1 = BUTTONDECOR[buttontype][showtype][1];
		sprnr2 = BUTTONDECOR[buttontype][showtype][2];
		sizexgrp = dlggrp->Picture[sprnr0].PixelPerLine;
		i = 0;
		GRP_PutNoPacked(menuitem->hotdeltax + i * sizexgrp, menuitem->hotdeltay, dlggrp, sprnr0);
		i++;
		for (;i < buttonsize - 1;i++)
			GRP_PutNoPacked(menuitem->hotdeltax + i * sizexgrp, menuitem->hotdeltay, dlggrp, sprnr1);
		GRP_PutNoPacked(menuitem->hotdeltax + i * sizexgrp, menuitem->hotdeltay, dlggrp, sprnr2);
	}
	if (showtype == ITEMSHOW_FOCUS)
	{
		if (menuitem->item.button->buttonflags&BF_TEXTGLOWED)
		{
			glowoffset = menuitem->showvalue / 128;
			putglowtext(menuitem->xtextpos,
				menuitem->ytextpos,
				menuitem->fontnr,
				menuitem->item.button->menustr.text[ITEMSHOW_FOCUS],
				GETITEMCOLOR(colors4, ITEMSHOW_FOCUS), fonttable, 1 - glowoffset);
			return;
		}
	}
	putmessage(menuitem->xtextpos,
		menuitem->ytextpos,
		menuitem->fontnr,
		menuitem->item.button->menustr.text[showtype],
		GETITEMCOLOR(colors4, showtype), fonttable, dlggrp);
}
//==========================================
void delbuttonitem(MENUSTR *allmenus, int nr, int end)
{
	for (int j = 0;j < ITEMSHOW_MAXPOS;j++)
		if (allmenus->menu[nr].item.button->menustr.text[j])
		{
			wfree(allmenus->menu[nr].item.button->menustr.text[j]);
			allmenus->menu[nr].item.button->menustr.text[j] = NULL;
		}
	if (end && allmenus->menu[nr].item.button)
	{
		wfree(allmenus->menu[nr].item.button);
		allmenus->menu[nr].item.button = NULL;
	}
}
//==========================================
//==========================================
char *geteditboxtext(MENUSTR *allmenus, int nr)
{
	MENUPOS *menuitem = &allmenus->menu[nr];
	if (menuitem->itemtype == ISEDITBOX)
	{
		return(menuitem->item.editbox->editstr);
	}
	else
		DEBUGMESSCR("leak detected!\n");
	return(NULL);
}
//==========================================
int geteditboxtextsize(MENUSTR *allmenus, int nr)
{
	MENUPOS *menuitem = &allmenus->menu[nr];
	if (menuitem->itemtype == ISEDITBOX)
	{
		return(menuitem->item.editbox->length);
	}
	else
		DEBUGMESSCR("leak detected!\n");
	return(0);
}
//==========================================
void changeeditboxtext(MENUSTR *allmenus, int nr, const char *newstr)
{
	MENUPOS *menuitem = &allmenus->menu[nr];
	if (menuitem->itemtype == ISEDITBOX)
	{
		strncpy(menuitem->item.editbox->editstr, newstr, menuitem->item.editbox->maxsymbols);
		menuitem->item.editbox->cursorpos = strlen(newstr);
		menuitem->item.editbox->length = menuitem->item.editbox->cursorpos;
	}
	else
		DEBUGMESSCR("leak detected!\n");

}
//==========================================
void changeeditboxparam(MENUSTR *allmenus, int nr, const char *beginstr, int maxsizestr)
{
	MENUPOS *menuitem = &allmenus->menu[nr];
	if (menuitem->itemtype == ISEDITBOX)
	{
		deleditboxitem(allmenus, nr, 0);
		menuitem->item.editbox->editstr = (char *)wmalloc(maxsizestr + 1);
		menuitem->item.editbox->maxsymbols = maxsizestr;
		menuitem->item.editbox->cursorpos = strlen(beginstr);
		menuitem->item.editbox->length = menuitem->item.editbox->cursorpos;
		strcpy(menuitem->item.editbox->editstr, beginstr);
	}
}

//==========================================
void addeditboxitem(MENUSTR *allmenus, int nr, int hotx, int hoty,
	int hotsizex, int hotsizey, int textx, int texty,
	int fontnr, const char *beginstr, int maxsizestr, unsigned int colors4)
{
	MENUPOS *menuitem = &allmenus->menu[nr];
	menuitem->item.editbox = (EDITBOX *)wmalloc(sizeof(EDITBOX));
	memset(menuitem->item.editbox, 0, sizeof(EDITBOX));
	menuitem->itemtype = ISEDITBOX;
	menuitem->colors4 = colors4;
	menuitem->hotdeltax = hotx + allmenus->x;
	menuitem->hotdeltay = hoty + allmenus->y;
	menuitem->hotxsize = hotsizex;
	menuitem->hotysize = hotsizey;
	menuitem->xtextdelta = textx;
	menuitem->ytextdelta = texty;
	menuitem->fontnr = fontnr;
	menuitem->hotkey = 255;
	changeeditboxparam(allmenus, nr, beginstr, maxsizestr);
	if (menuitem->savedscrunderitem)
	{
		loadunderitem(allmenus, nr);
		saveunderitem(allmenus, nr);
	}
	//	  setmenuitem_DISABLED(allmenus,nr,FALSE);
	restoreflag(allmenus, nr, ITEM_RESTOREANDFREE);
}
//==========================================
void draweditboxitem(MENUSTR *allmenus, int itemnr)
{
	int vis, len;
	char tempstr[200];
	MENUPOS *menuitem = &allmenus->menu[itemnr];
	strcpy(tempstr, menuitem->item.editbox->editstr);
	vis = visiblerighttext(menuitem->fontnr, menuitem->hotxsize, tempstr);
	len = strlen(tempstr);
	if (allmenus->DefaultTextBox == itemnr && getcursorblinktype())
		strcat(tempstr, "|");
	putmessage(menuitem->xtextpos, menuitem->ytextpos,
		menuitem->fontnr, tempstr + len - vis, GETITEMCOLOR(menuitem->colors4, menuitem->typeofshow),
		allmenus->fonttable, allmenus->dlggrp);
}
//==========================================
void deleditboxitem(MENUSTR *allmenus, int nr, int end)
{
	if (allmenus->menu[nr].item.editbox->editstr)
	{
		wfree(allmenus->menu[nr].item.editbox->editstr);
		allmenus->menu[nr].item.editbox->editstr = NULL;
	}
	if (end&&allmenus->menu[nr].item.editbox)
	{
		wfree(allmenus->menu[nr].item.editbox);
		allmenus->menu[nr].item.editbox = NULL;
	}
}
//==========================================
//==========================================
void changetextitem(MENUSTR *allmenus, int nr, const char *textstr)
{
	MENUPOS *menuitem = &allmenus->menu[nr];
	if (menuitem->itemtype == ISLABELLEFT || menuitem->itemtype == ISLABELCENTER || menuitem->itemtype == ISLABELRIGHT)
	{
		deltextitem(allmenus, nr, 0);
		if (textstr != NULL)
		{
			menuitem->item.textitem->textstr = (char *)wmalloc(strlen(textstr) + 1);
			strcpy(menuitem->item.textitem->textstr, textstr);
		}
	}
	else
		DEBUGMESSCR("leak detected!\n");
}
//==========================================
void changetextskiplinepixels(MENUSTR *allmenus, int nr, int skiplinepixels)
{
	MENUPOS *menuitem = &allmenus->menu[nr];
	menuitem->item.textitem->skiplinepixels = skiplinepixels;
}
//==========================================
void addtextitem(MENUSTR *allmenus, int nr, int labelid, int hotx, int hoty, int hotsizex, int hotsizey, int textx, int texty,
	char *textstr, int fontnr, int rowsize, unsigned int colors4)
{
	char *txtstr = NULL;
	MENUPOS *menuitem = &allmenus->menu[nr];
	if (menuitem->item.textitem)
	{
		printf("lost detected %d item\n", nr);
	}
	menuitem->item.textitem = (struct TEXTLABEL *)wmalloc(sizeof(struct TEXTLABEL));
	memset(menuitem->item.textitem, 0, sizeof(struct TEXTLABEL));
	menuitem->itemtype = labelid;
	menuitem->colors4 = colors4;
	menuitem->hotdeltax = hotx + allmenus->x;
	menuitem->hotdeltay = hoty + allmenus->y;
	menuitem->hotxsize = hotsizex;
	menuitem->hotysize = hotsizey;
	menuitem->xtextdelta = textx;
	menuitem->ytextdelta = texty;
	menuitem->fontnr = fontnr;
	menuitem->hotkey = 255;
	if (textstr)
		if (textstr[0])
			txtstr = textstr;
	changetextitem(allmenus, nr, txtstr);
	menuitem->item.textitem->rowsize = rowsize;
	menuitem->item.textitem->skiplinepixels = 0;

	if (menuitem->savedscrunderitem)
	{
		loadunderitem(allmenus, nr);
		saveunderitem(allmenus, nr);
	}
	restoreflag(allmenus, nr, ITEM_RESTOREANDFREE);
}
//==========================================
void drawtextitem(MENUSTR *allmenus, int itemnr)
{
	int color;
	GRPFILE *dlggrp = allmenus->dlggrp;
	MENUPOS *menuitem = &allmenus->menu[itemnr];
	char *fonttable = allmenus->fonttable;
	int showtype = menuitem->typeofshow;
	int colors4 = menuitem->colors4;

	if (menuitem->item.textitem->textstr)
	{
		if (menuitem->item.textitem->textstr[0] == '%')
		{
			drawtextpercent(allmenus, itemnr, menuitem->item.textitem->textstr + 1, menuitem->fontnr, colors4, MBLUECOLORFONT);
		}
		else
		{
			color = GETITEMCOLOR(colors4, showtype);
			switch (menuitem->itemtype)
			{
			case ISLABELLEFT:
				putmessageinrectangleL(menuitem->hotdeltax + menuitem->xtextdelta, menuitem->hotdeltay + menuitem->ytextdelta,
					menuitem->hotxsize, menuitem->hotysize, menuitem->item.textitem->rowsize,
					menuitem->item.textitem->textstr, menuitem->fontnr, color, fonttable, dlggrp, showtype != ITEMSHOW_DISABLED,
					menuitem->item.textitem->skiplinepixels);
				break;
			case ISLABELCENTER:
				putmessageinrectangleM(menuitem->hotdeltax + menuitem->xtextdelta, menuitem->hotdeltay + menuitem->ytextdelta,
					menuitem->hotxsize, menuitem->hotysize, menuitem->item.textitem->rowsize,
					menuitem->item.textitem->textstr, menuitem->fontnr, color, fonttable, dlggrp, showtype != ITEMSHOW_DISABLED);
				break;
			case ISLABELRIGHT:
				putmessageinrectangleR(menuitem->hotdeltax, menuitem->hotdeltay + menuitem->ytextdelta,
					menuitem->hotxsize - menuitem->xtextdelta, menuitem->hotysize, menuitem->item.textitem->rowsize,
					menuitem->item.textitem->textstr, menuitem->fontnr, color, fonttable, dlggrp, showtype != ITEMSHOW_DISABLED);
				break;
			}
		}
	}
}
//==========================================
void deltextitem(MENUSTR *allmenus, int nr, int end)
{
	if (allmenus->menu[nr].item.textitem->textstr)
	{
		wfree(allmenus->menu[nr].item.textitem->textstr);
		allmenus->menu[nr].item.textitem->textstr = NULL;
	}
	if (end&&allmenus->menu[nr].item.textitem)
	{
		wfree(allmenus->menu[nr].item.textitem);
		allmenus->menu[nr].item.textitem = NULL;
	}
}
//==========================================
int getexpanditems(MENUSTR *allmenus, int nr, char *charset)
{
	int ret = 0, i, len;
	MENUPOS *menuitem = &allmenus->menu[nr];
	if (menuitem->itemtype == ISEXPANDBOX)
	{
		len = strlen(charset);
		for (i = 0;i < len;i++)
		{
			if (charset[i] == '\n')
				ret++;
		}
		ret++;
	}
	else
		DEBUGMESSCR("leak detected!\n");

	return(ret);
}
//==========================================
void changeexpanditemnr(MENUSTR *allmenus, int nr, int selectednr)
{
	MENUPOS *menuitem = &allmenus->menu[nr];
	if (menuitem->itemtype == ISEXPANDBOX)
	{
		menuitem->item.expandbox->selectednr = selectednr;
	}
	else
		DEBUGMESSCR("leak detected!\n");
}
//==========================================
void addexpanditem_lists(MENUSTR *allmenus, int nr, int selectednr, char *charset)
{
	int from, i, j, len, itemnr, barxsize, barysize, items;
	MENUPOS *menuitem = &allmenus->menu[nr];
	if (menuitem->itemtype == ISEXPANDBOX)
	{
		delexpanditem(allmenus, nr, 0);
		items = getexpanditems(allmenus, nr, charset);

		if (selectednr >= items)
			selectednr = 0;
		if (selectednr != -1)
			menuitem->item.expandbox->selectednr = selectednr;
		menuitem->item.expandbox->maxitem = items;

		len = strlen(charset);
		menuitem->item.expandbox->items = (char **)wmalloc(items * sizeof(char *));
		menuitem->item.expandbox->items[0] = (char *)wmalloc(len + 1);
		j = 0;
		from = 0;
		itemnr = 0;
		for (;j < len;j++)
		{
			menuitem->item.expandbox->items[0][j] = charset[j];
			if (charset[j] == '\n')
			{
				menuitem->item.expandbox->items[0][j] = 0;
				if (itemnr)
					menuitem->item.expandbox->items[itemnr] = &menuitem->item.expandbox->items[0][from];
				itemnr++;
				from = j + 1;
			}
		}
		menuitem->item.expandbox->items[itemnr] = &menuitem->item.expandbox->items[0][from];
		menuitem->item.expandbox->items[0][j] = 0;

		menuitem->item.expandbox->arrowbottom[ITEMSHOW_DISABLED] = 52;
		menuitem->item.expandbox->arrowbottom[ITEMSHOW_NOFOCUS] = 50;
		menuitem->item.expandbox->arrowbottom[ITEMSHOW_CLICK] = 50;
		menuitem->item.expandbox->arrowbottom[ITEMSHOW_FOCUS] = 51;

		barxsize = menuitem->hotxsize / allmenus->dlggrp->Picture[53].PixelPerLine;
		barysize = menuitem->hotysize / allmenus->dlggrp->Picture[53].LinesPerPicture;
		menuitem->hotxsize = barxsize * allmenus->dlggrp->Picture[53].PixelPerLine;
		menuitem->hotysize = barysize * allmenus->dlggrp->Picture[53].LinesPerPicture;

		menuitem->item.expandbox->array[ITEMSHOW_DISABLED] = (char *)wmalloc(barxsize + 3);//disabled array
		menuitem->item.expandbox->array[ITEMSHOW_NOFOCUS] = (char *)wmalloc(barxsize + 3);//nofocus array
		menuitem->item.expandbox->array[ITEMSHOW_CLICK] = (char *)wmalloc(barxsize*(barysize + items) + 3);//click array
		menuitem->item.expandbox->array[ITEMSHOW_FOCUS] = (char *)wmalloc(barxsize + 3);//focus array

		menuitem->item.expandbox->array[ITEMSHOW_DISABLED][0] = barxsize; //xsize
		menuitem->item.expandbox->array[ITEMSHOW_DISABLED][1] = 1;				//ysize
		menuitem->item.expandbox->array[ITEMSHOW_DISABLED][2] = 53;
		for (j = 3;j < barxsize + 1;j++)
			menuitem->item.expandbox->array[ITEMSHOW_DISABLED][j] = 54;
		menuitem->item.expandbox->array[ITEMSHOW_DISABLED][j++] = 55;
		menuitem->item.expandbox->array[ITEMSHOW_DISABLED][j] = 0;

		menuitem->item.expandbox->array[ITEMSHOW_NOFOCUS][0] = barxsize;	//xsize
		menuitem->item.expandbox->array[ITEMSHOW_NOFOCUS][1] = 1;			//ysize
		menuitem->item.expandbox->array[ITEMSHOW_NOFOCUS][2] = 53;
		for (j = 3;j < barxsize + 1;j++)
			menuitem->item.expandbox->array[ITEMSHOW_NOFOCUS][j] = 54;
		menuitem->item.expandbox->array[ITEMSHOW_NOFOCUS][j++] = 55;
		menuitem->item.expandbox->array[ITEMSHOW_NOFOCUS][j] = 0;

		menuitem->item.expandbox->array[ITEMSHOW_FOCUS][0] = barxsize;			//xsize
		menuitem->item.expandbox->array[ITEMSHOW_FOCUS][1] = 1;			//ysize
		menuitem->item.expandbox->array[ITEMSHOW_FOCUS][2] = 56;
		for (j = 3;j < barxsize + 1;j++)
			menuitem->item.expandbox->array[ITEMSHOW_FOCUS][j] = 57;
		menuitem->item.expandbox->array[ITEMSHOW_FOCUS][j++] = 58;
		menuitem->item.expandbox->array[ITEMSHOW_FOCUS][j] = 0;

		if (menuitem->item.expandbox->hoty + (barysize + items)*menuitem->hotysize <= GRP_wmaxy)				//if list greater than maxy drawpixels
			menuitem->item.expandbox->typelistshow = 1;
		else
			menuitem->item.expandbox->typelistshow = -1;
		menuitem->item.expandbox->array[ITEMSHOW_CLICK][0] = barxsize;			//xsize
		menuitem->item.expandbox->array[ITEMSHOW_CLICK][1] = barysize + items;		//ysize
		from = 2;
		if (menuitem->item.expandbox->typelistshow == 1)
		{
			menuitem->item.expandbox->array[ITEMSHOW_CLICK][from++] = 47;
			for (j = 1;j < barxsize - 1;j++)
				menuitem->item.expandbox->array[ITEMSHOW_CLICK][from++] = 48;
			menuitem->item.expandbox->array[ITEMSHOW_CLICK][from++] = 49;
			for (i = 1;i < barysize + items - 1;i++)
			{
				menuitem->item.expandbox->array[ITEMSHOW_CLICK][from++] = 38;
				for (j = 1;j < barxsize - 1;j++)
					menuitem->item.expandbox->array[ITEMSHOW_CLICK][from++] = 39;
				menuitem->item.expandbox->array[ITEMSHOW_CLICK][from++] = 40;
			}
			menuitem->item.expandbox->array[ITEMSHOW_CLICK][from++] = 41;
			for (j = 1;j < barxsize - 1;j++)
				menuitem->item.expandbox->array[ITEMSHOW_CLICK][from++] = 42;
			menuitem->item.expandbox->array[ITEMSHOW_CLICK][from++] = 43;
		}
		else
		{
			menuitem->item.expandbox->array[ITEMSHOW_CLICK][from++] = 44;
			for (j = 1;j < barxsize - 1;j++)
				menuitem->item.expandbox->array[ITEMSHOW_CLICK][from++] = 45;
			menuitem->item.expandbox->array[ITEMSHOW_CLICK][from++] = 46;
			for (i = 1;i < barysize + items - 1;i++)
			{
				menuitem->item.expandbox->array[ITEMSHOW_CLICK][from++] = 38;
				for (j = 1;j < barxsize - 1;j++)
					menuitem->item.expandbox->array[ITEMSHOW_CLICK][from++] = 39;
				menuitem->item.expandbox->array[ITEMSHOW_CLICK][from++] = 40;
			}
			menuitem->item.expandbox->array[ITEMSHOW_CLICK][from++] = 35;
			for (j = 1;j < barxsize - 1;j++)
				menuitem->item.expandbox->array[ITEMSHOW_CLICK][from++] = 36;
			menuitem->item.expandbox->array[ITEMSHOW_CLICK][from++] = 37;
		}
		menuitem->item.expandbox->array[ITEMSHOW_CLICK][from++] = 0;
	}
	else
		DEBUGMESSCR("leak detected!\n");

}
//==========================================
void addexpanditem(MENUSTR *allmenus, int nr, int selectednr, int fontnr, int rowsize, unsigned int colors4)
{
	MENUPOS *menuitem = &allmenus->menu[nr];
	menuitem->item.expandbox = (EXPANDBOX *)wmalloc(sizeof(EXPANDBOX));
	memset(menuitem->item.expandbox, 0, sizeof(EXPANDBOX));
	menuitem->itemtype = ISEXPANDBOX;
	menuitem->colors4 = colors4;
	menuitem->item.expandbox->hoty = menuitem->hotdeltay - allmenus->y;
	menuitem->fontnr = fontnr;
	menuitem->hotkey = 255;
	menuitem->item.expandbox->selectednr = selectednr;
	menuitem->item.expandbox->maxitem = 0;
	menuitem->item.expandbox->rowsize = rowsize;
	if (menuitem->savedscrunderitem)
	{
		loadunderitem(allmenus, nr);
		saveunderitem(allmenus, nr);
	}
	restoreflag(allmenus, nr, ITEM_RESTOREANDFREE);
}
//==========================================
void addexpanditem(MENUSTR *allmenus, int nr, int hotx, int hoty, int hotsizex, int hotsizey, int selectednr,
	int fontnr, int rowsize, unsigned int colors4)
{
	MENUPOS *menuitem = &allmenus->menu[nr];
	menuitem->item.expandbox = (EXPANDBOX *)wmalloc(sizeof(EXPANDBOX));
	memset(menuitem->item.expandbox, 0, sizeof(EXPANDBOX));
	menuitem->itemtype = ISEXPANDBOX;
	menuitem->colors4 = colors4;
	menuitem->item.expandbox->hoty = hoty;
	menuitem->hotdeltax = hotx + allmenus->x;
	menuitem->hotdeltay = hoty + allmenus->y;
	menuitem->hotxsize = hotsizex;
	menuitem->hotysize = hotsizey;
	menuitem->xtextdelta = 0;
	menuitem->ytextdelta = 0;
	menuitem->fontnr = fontnr;
	menuitem->hotkey = 255;
	menuitem->item.expandbox->selectednr = selectednr;
	menuitem->item.expandbox->maxitem = 0;
	menuitem->item.expandbox->rowsize = rowsize;
	if (menuitem->savedscrunderitem)
	{
		loadunderitem(allmenus, nr);
		saveunderitem(allmenus, nr);
	}
	//	  setmenuitem_DISABLED(allmenus,nr,FALSE);
	restoreflag(allmenus, nr, ITEM_RESTOREANDFREE);
}
//==========================================
void drawexpanditem(MENUSTR *allmenus, int itemnr)
{
	int i, j, color, x, y, xsize, ysize, fromspr, sprnr0;
	GRPFILE *dlggrp = allmenus->dlggrp;
	MENUPOS *menuitem = &allmenus->menu[itemnr];
	char *mes;
	char *fonttable = allmenus->fonttable;
	int showtype = menuitem->typeofshow;
	int colors4 = menuitem->colors4;

	//	  if (showtype!=ITEMSHOW_CLICK)
	//	  {
	//	  if (menuitem->item.expandbox->array)
	//	  {
	if (menuitem->item.expandbox->array[showtype])
	{
		x = menuitem->item.expandbox->array[showtype][0];
		y = menuitem->item.expandbox->array[showtype][1];
		fromspr = 2;
		sprnr0 = menuitem->item.expandbox->array[showtype][fromspr];
		xsize = dlggrp->Picture[sprnr0].PixelPerLine;
		ysize = dlggrp->Picture[sprnr0].LinesPerPicture;
		for (i = 0;i < y;i++)
		{
			for (j = 0;j < x;j++)
			{
				GRP_PutNoPacked(menuitem->hotdeltax + j * xsize, menuitem->hotdeltay + i * ysize,
					dlggrp, menuitem->item.expandbox->array[showtype][fromspr]);
				fromspr++;
			}
		}
		sprnr0 = menuitem->item.expandbox->arrowbottom[showtype];
		GRP_PutNoPacked(menuitem->hotdeltax + menuitem->hotxsize - 18, menuitem->hotdeltay + 4, dlggrp, sprnr0);
		mes = menuitem->item.expandbox->items[menuitem->item.expandbox->selectednr];
		//			color=GETITEMCOLOR(colors4,showtype);
		color = GETITEMCOLOR(colors4, ITEMSHOW_NOFOCUS);
		putmessage(menuitem->hotdeltax + 4, menuitem->hotdeltay, menuitem->fontnr, mes, color, fonttable, dlggrp);
	}
	//	  }
	//	  }
	//	  else
	//		mes[0]=0;
}
//==========================================
void delexpanditem(MENUSTR *allmenus, int nr, int end)
{
	for (int j = 0;j < 4;j++)
	{
		if (allmenus->menu[nr].item.expandbox->array[j])
		{
			wfree(allmenus->menu[nr].item.expandbox->array[j]);
			allmenus->menu[nr].item.expandbox->array[j] = NULL;
		}
	}
	if (allmenus->menu[nr].item.expandbox->items)
	{
		if (allmenus->menu[nr].item.expandbox->items[0])
		{
			wfree(allmenus->menu[nr].item.expandbox->items[0]);
		}
		wfree(allmenus->menu[nr].item.expandbox->items);
		allmenus->menu[nr].item.expandbox->items = NULL;
	}
	if (end&&allmenus->menu[nr].item.expandbox)
	{
		wfree(allmenus->menu[nr].item.expandbox);
		allmenus->menu[nr].item.expandbox = NULL;
	}
}
//==========================================
void addcheckboxitem(MENUSTR *allmenus, int nr, int hotx, int hoty, int hotsizex, int hotsizey, int textx, int texty, char hotkey,
	char *textstr, int fontnr, unsigned int colors4, int state)
{
	int i, len;
	MENUPOS *menuitem = &allmenus->menu[nr];
	menuitem->item.checkbox = (CHECKBOX *)wmalloc(sizeof(CHECKBOX));
	memset(menuitem->item.checkbox, 0, sizeof(CHECKBOX));
	menuitem->itemtype = ISCHECKBOX;
	menuitem->colors4 = colors4;
	menuitem->hotdeltax = hotx + allmenus->x;
	menuitem->hotdeltay = hoty + allmenus->y;
	menuitem->hotxsize = hotsizex;
	menuitem->hotysize = hotsizey;
	menuitem->xtextdelta = textx;
	menuitem->ytextdelta = texty;
	menuitem->fontnr = fontnr;
	menuitem->hotkey = hotkey;
	if (textstr != NULL)
	{
		len = strlen(textstr);
		for (i = 0;i < ITEMSHOW_MAXPOS;i++)
		{
			menuitem->item.checkbox->textstr.text[i] = (char *)wmalloc(len + 1);
			strcpy(menuitem->item.checkbox->textstr.text[i], textstr);
			parse_special_text(menuitem->item.checkbox->textstr.text[i], len, colors4, BUTTONCOLORS[i]);
		}

	}
	if (menuitem->savedscrunderitem)
	{
		loadunderitem(allmenus, nr);
		saveunderitem(allmenus, nr);
	}
	//	  setmenuitem_DISABLED(allmenus,nr,FALSE);
	restoreflag(allmenus, nr, ITEM_RESTOREANDFREE);
	menuitem->item.checkbox->state = state;
}
//==========================================
void changecheckboxstate(MENUSTR *allmenus, int nr)
{
	if (allmenus->menu[nr].itemtype == ISCHECKBOX)
		allmenus->menu[nr].item.checkbox->state ^= 1;
	else
		DEBUGMESSCR("leak detected!\n");
}
//==========================================
void setcheckboxstate(MENUSTR *allmenus, int nr, int value)
{
	if (allmenus->menu[nr].itemtype == ISCHECKBOX)
		allmenus->menu[nr].item.checkbox->state = value;
	else
		DEBUGMESSCR("leak detected!\n");
}
//==========================================
int getcheckboxstate(MENUSTR *allmenus, int nr)
{
	if (allmenus->menu[nr].itemtype == ISCHECKBOX)
		return(allmenus->menu[nr].item.checkbox->state);
	else
		DEBUGMESSCR("leak detected!\n");
	return(0);
}
//==========================================
char CHECKBOXTYPES[2][4] = {
						{11,12,15,12},
						{11,14,13,14}
};
//==========================================
void drawcheckboxitem(MENUSTR *allmenus, int itemnr)
{
	int leny, color;
	GRPFILE *dlggrp = allmenus->dlggrp;
	MENUPOS *menuitem = &allmenus->menu[itemnr];
	int showtype = menuitem->typeofshow;

	GRP_PutNoPacked(menuitem->hotdeltax, menuitem->hotdeltay, dlggrp, CHECKBOXTYPES[menuitem->item.checkbox->state][showtype]);
	color = GETITEMCOLOR(menuitem->colors4, showtype);
	getmessagelen(menuitem->fontnr, menuitem->item.checkbox->textstr.text[showtype], NULL, &leny);
	putmessageinrectangleL(menuitem->hotdeltax + menuitem->xtextdelta, menuitem->hotdeltay + menuitem->ytextdelta,
		menuitem->hotxsize, menuitem->hotysize, leny,
		menuitem->item.checkbox->textstr.text[showtype], menuitem->fontnr,
		color, allmenus->fonttable, allmenus->dlggrp, showtype != ITEMSHOW_DISABLED, 0);
}
//==========================================
void delcheckboxitem(MENUSTR *allmenus, int nr, int end)
{
	int i;
	for (i = 0;i < 4;i++)
	{
		if (allmenus->menu[nr].item.checkbox->textstr.text[i])
		{
			wfree(allmenus->menu[nr].item.checkbox->textstr.text[i]);
			allmenus->menu[nr].item.checkbox->textstr.text[i] = NULL;
		}
	}
	if (end&&allmenus->menu[nr].item.checkbox)
	{
		wfree(allmenus->menu[nr].item.checkbox);
		allmenus->menu[nr].item.checkbox = NULL;
	}
}
//==========================================
int addradiobuttonimg(MENUSTR *allmenus, int nr, PCX *pcx1, PCX *pcx2, int deltax, int deltay)
{
	MENUPOS *menuitem = &allmenus->menu[nr];
	if (menuitem->itemtype == ISRADIOBUTTON)
	{
		menuitem->item.radiobutton->pcx1 = pcx1;
		menuitem->item.radiobutton->pcx2 = pcx2;
		menuitem->item.radiobutton->deltax = deltax;
		menuitem->item.radiobutton->deltay = deltay;
	}
	else
		DEBUGMESSCR("leak detected!\n");
	return(0);
}
//==========================================
int addradiobuttonitem(MENUSTR *allmenus, int nr, int hotx, int hoty, int hotsizex, int hotsizey, int textx, int texty, char hotkey,
	char *textstr, int fontnr, unsigned int colors4, int firstinstack)
{
	int nextitem, i, len, curitem = -1;
	MENUPOS *menuitem = &allmenus->menu[nr];
	menuitem->item.radiobutton = (RADIOBUTTON *)wmalloc(sizeof(RADIOBUTTON));
	memset(menuitem->item.radiobutton, 0, sizeof(RADIOBUTTON));
	menuitem->itemtype = ISRADIOBUTTON;
	menuitem->colors4 = colors4;
	menuitem->hotdeltax = hotx + allmenus->x;
	menuitem->hotdeltay = hoty + allmenus->y;
	menuitem->hotxsize = hotsizex;
	menuitem->hotysize = hotsizey;
	menuitem->xtextdelta = textx;
	menuitem->ytextdelta = texty;
	menuitem->fontnr = fontnr;
	menuitem->hotkey = hotkey;
	if (textstr != NULL)
	{
		len = strlen(textstr);
		for (i = 0;i < ITEMSHOW_MAXPOS;i++)
		{
			menuitem->item.radiobutton->textstr.text[i] = (char *)wmalloc(len + 1);
			strcpy(menuitem->item.radiobutton->textstr.text[i], textstr);
			parse_special_text(menuitem->item.radiobutton->textstr.text[i], len, colors4, BUTTONCOLORS[i]);
		}
	}
	if (menuitem->savedscrunderitem)
	{
		loadunderitem(allmenus, nr);
		saveunderitem(allmenus, nr);
	}
	//	  setmenuitem_DISABLED(allmenus,nr,FALSE);
	restoreflag(allmenus, nr, ITEM_RESTOREANDFREE);

	menuitem->item.radiobutton->state = 0;
	menuitem->item.radiobutton->firstinstack = firstinstack;
	menuitem->item.radiobutton->nextradiobutton = -1;
	nextitem = firstinstack;
	while (nextitem != -1)
	{
		curitem = nextitem;
		if (allmenus->menu[curitem].itemtype != ISRADIOBUTTON)
		{
			menuitem->itemtype = ISNULL;
			return(-1);
		}
		nextitem = allmenus->menu[curitem].item.radiobutton->nextradiobutton;
	}
	if (curitem != nr)
		allmenus->menu[curitem].item.radiobutton->nextradiobutton = nr;
	return(0);
}
//==========================================
char RADIOBUTTONTYPES[2][4] = {
							{6,7,10,7},
							{6,9,10,9}
};
//==========================================
void drawradiobuttonitem(MENUSTR *allmenus, int itemnr)
{
	int leny, color, deltax = 0;
	GRPFILE *dlggrp = allmenus->dlggrp;
	MENUPOS *menuitem = &allmenus->menu[itemnr];
	PCX *usedpcx;
	int showtype = menuitem->typeofshow;
	int state = menuitem->item.radiobutton->state;
	if (state)
	{
		showtype = ITEMSHOW_FOCUS;
		usedpcx = menuitem->item.radiobutton->pcx1;
	}
	else
	{
		usedpcx = menuitem->item.radiobutton->pcx2;
	}
	if (usedpcx)
	{
		usedpcx->PutPcx(menuitem->hotdeltax + menuitem->item.radiobutton->deltax,
			menuitem->hotdeltay + menuitem->item.radiobutton->deltay, PCX_EMPTYCOLOR2);
	}
	if (dlggrp)
	{
		if (!(menuitem->dialogbin_flags&DIALOGBIN_FLAGS_NODLGGRP))
		{
			GRP_PutNoPacked(menuitem->hotdeltax, menuitem->hotdeltay, dlggrp, RADIOBUTTONTYPES[state][menuitem->typeofshow]);
			deltax = 22;
		}
	}
	color = GETITEMCOLOR(menuitem->colors4, showtype);
	getmessagelen(menuitem->fontnr, menuitem->item.radiobutton->textstr.text[showtype], NULL, &leny);

	putmessageinrectangleL(menuitem->hotdeltax + deltax + menuitem->xtextdelta, menuitem->hotdeltay + menuitem->ytextdelta,
		menuitem->hotxsize, menuitem->hotysize, leny,
		menuitem->item.radiobutton->textstr.text[showtype],
		menuitem->fontnr, color, allmenus->fonttable, allmenus->dlggrp, showtype != ITEMSHOW_DISABLED, 0);

}
//==========================================
void delradiobuttonitem(MENUSTR *allmenus, int nr, int end)
{
	int i;
	for (i = 0;i < 4;i++)
		if (allmenus->menu[nr].item.radiobutton->textstr.text[i])
		{
			wfree(allmenus->menu[nr].item.radiobutton->textstr.text[i]);
			allmenus->menu[nr].item.radiobutton->textstr.text[i] = NULL;
		}
	if (end&&allmenus->menu[nr].item.radiobutton)
	{
		wfree(allmenus->menu[nr].item.radiobutton);
		allmenus->menu[nr].item.radiobutton = NULL;
	}

}
//==========================================
void setradiobuttonstate(MENUSTR *allmenus, int itemnr)
{
	int nextitem, curitem;
	if (allmenus->menu[itemnr].itemtype == ISRADIOBUTTON)
	{
		nextitem = allmenus->menu[itemnr].item.radiobutton->firstinstack;
		curitem = itemnr;
		while (nextitem != -1)
		{
			allmenus->menu[nextitem].item.radiobutton->state = 0;
			nextitem = allmenus->menu[nextitem].item.radiobutton->nextradiobutton;
		}
		allmenus->menu[curitem].item.radiobutton->state = 1;
	}
	else
		DEBUGMESSCR("leak detected!\n");
}
//==========================================
int getradiobuttonstate(MENUSTR *allmenus, int itemnr)
{
	int nextitem, curitem;
	if (allmenus->menu[itemnr].itemtype == ISRADIOBUTTON)
	{
		nextitem = allmenus->menu[itemnr].item.radiobutton->firstinstack;
		curitem = itemnr;
		while (nextitem != -1)
		{
			if (allmenus->menu[nextitem].item.radiobutton->state)
				return(nextitem);
			nextitem = allmenus->menu[nextitem].item.radiobutton->nextradiobutton;
		}
	}
	else
		DEBUGMESSCR("leak detected!\n");
	return(-1);
}
//==========================================
char HORIZBAR[4][3] = {
			{91,92,93},
			{94,95,96},
			{94,95,96},
			{94,95,96}
};
char HORIZHOLE[4] = { 97,98,98,98 };
char HORIZBUTTON[4] = { 99,100,102,101 };
//==========================================
#define HORIZBUTTONSIZE			5
#define HORIZLINESIZE			8
void addhorizbutton_params(MENUSTR *allmenus, int nr, int maxpos, int nrofholes, int spacebetweenholes,
	void(*func)(MENUSTR *allmenus, int itemnr, int horizpos))
{
	int sizexelem, newsizepix;
	MENUPOS *menuitem = &allmenus->menu[nr];
	if (menuitem->itemtype == ISHORIZBUTTON)
	{
		sizexelem = (spacebetweenholes + 1)*(nrofholes - 1) + 1 + 2;				//size in elements count
		newsizepix = (spacebetweenholes*HORIZLINESIZE + HORIZBUTTONSIZE)*(nrofholes - 1) + HORIZBUTTONSIZE + 2 * HORIZLINESIZE;
		menuitem->item.horizbutton->sizexelem = sizexelem;
		menuitem->item.horizbutton->sizexbar = newsizepix;
		addhorizbutton_params(allmenus, nr, maxpos, nrofholes, func);
	}
	else
		DEBUGMESSCR("leak detected!\n");

}
//==========================================
void addhorizbutton_params(MENUSTR *allmenus, int nr, int maxpos, int nrofholes,
	void(*func)(MENUSTR *allmenus, int itemnr, int horizpos))
{
	int sizexelem, xvaluesdelta = 0;
	int i, j, factor;
	char tempbar[200];
	MENUPOS *menuitem = &allmenus->menu[nr];
	if (menuitem->itemtype == ISHORIZBUTTON)
	{
		delhorizbuttonitem(allmenus, nr, 0);

		menuitem->item.horizbutton->onselectitem_callback = func;
		memset(tempbar, 0, sizeof(tempbar));

		menuitem->item.horizbutton->xvalues = (int *)wmalloc(nrofholes * sizeof(int));
		memset(menuitem->item.horizbutton->xvalues, 0, nrofholes * sizeof(int));
		if (maxpos <= nrofholes)
			menuitem->item.horizbutton->exactposition = 1;
		menuitem->item.horizbutton->metric = maxpos;
		menuitem->item.horizbutton->minxvalue = 5;
		menuitem->item.horizbutton->maxxvalue = menuitem->item.horizbutton->sizexbar - allmenus->dlggrp->Picture[HORIZBUTTON[1]].PixelPerLine - 4;
		menuitem->item.horizbutton->barxposition = menuitem->item.horizbutton->minxvalue;

		factor = (menuitem->item.horizbutton->sizexbar - 2 * (5 + 8) - 5 * (nrofholes - 2)) / (nrofholes - 1) / 8;
		j = (menuitem->item.horizbutton->sizexbar - 2 * (5 + 8) - 5 * (nrofholes - 2) - 8 * (nrofholes - 1)*factor) / 8;

		factor++;
		int hole = 0;
		tempbar[0] = 0;
		tempbar[1] = 1;
		xvaluesdelta += 5;
		menuitem->item.horizbutton->xvalues[hole++] = xvaluesdelta;
		for (i = 1 + factor;nrofholes > 2;nrofholes--, i += factor)
		{
			tempbar[i] = 1;
			xvaluesdelta += 5 + (factor - 1) * 8;
			menuitem->item.horizbutton->xvalues[hole++] = xvaluesdelta;
		}
		i += j;
		xvaluesdelta += j * 8;
		tempbar[i++] = 1;
		tempbar[i++] = 0;
		xvaluesdelta += 5 + (factor - 1) * 8;
		menuitem->item.horizbutton->xvalues[hole++] = xvaluesdelta;
		sizexelem = i;
		menuitem->item.horizbutton->sizexelem = sizexelem;
		menuitem->item.horizbutton->allsprbar = (char *)wmalloc(sizexelem);
		memcpy(menuitem->item.horizbutton->allsprbar, tempbar, sizexelem);
	}
	else
		DEBUGMESSCR("leak detected!\n");

}
//==========================================
void addhorizbutton(MENUSTR *allmenus, int nr, int hotx, int hoty, int hotsizex, int hotsizey, unsigned int colors4, int maxpos, int nrofholes,
	void(*func)(MENUSTR *allmenus, int itemnr, int horizpos))
{
	MENUPOS *menuitem = &allmenus->menu[nr];
	menuitem->item.horizbutton = (struct HORIZBUTTON *)wmalloc(sizeof(struct HORIZBUTTON));
	memset(menuitem->item.horizbutton, 0, sizeof(struct HORIZBUTTON));
	menuitem->itemtype = ISHORIZBUTTON;
	menuitem->colors4 = colors4;
	menuitem->hotdeltax = hotx + allmenus->x;
	menuitem->hotdeltay = hoty + allmenus->y;
	menuitem->hotxsize = hotsizex;
	menuitem->hotysize = hotsizey;
	menuitem->item.horizbutton->sizexbar = hotsizex;//defaultvalue
	menuitem->hotkey = 255;
	if (maxpos&&nrofholes)//if empty do not create params
		addhorizbutton_params(allmenus, nr, maxpos, nrofholes, func);
	if (menuitem->savedscrunderitem)
	{
		loadunderitem(allmenus, nr);
		saveunderitem(allmenus, nr);
	}
	//	  setmenuitem_DISABLED(allmenus,nr,FALSE);
	restoreflag(allmenus, nr, ITEM_RESTOREANDFREE);
}
//==========================================
void sethorizbuttonpos(MENUSTR *allmenus, int itemnr, int value)
{
	sethorizbuttonpos(allmenus, itemnr, value, false);
}
//==========================================
void sethorizbuttonpos(MENUSTR *allmenus, int itemnr, int value,int ignorecallback)
{
	int value2;
	if (allmenus->menu[itemnr].itemtype == ISHORIZBUTTON)
	{
		MENUPOS *menuitem = &allmenus->menu[itemnr];
		if (value < 0)
			value = 0;
		else
			if (value >= allmenus->menu[itemnr].item.horizbutton->metric)
				value = allmenus->menu[itemnr].item.horizbutton->metric - 1;
		if (menuitem->item.horizbutton->exactposition)
			value2 = value * menuitem->item.horizbutton->sizexbar / (allmenus->menu[itemnr].item.horizbutton->metric - 1) + 1;
		else
			value2 = value * menuitem->item.horizbutton->sizexbar / allmenus->menu[itemnr].item.horizbutton->metric;
		sethorizbuttonposfrommouse(allmenus, itemnr, value2, ignorecallback);
		menuitem->item.horizbutton->value = value;
	}
	else
		DEBUGMESSCR("leak detected!\n");
}
//==========================================
int gethorizvalue(MENUSTR *allmenus, int nr)
{
	if (allmenus->menu[nr].itemtype == ISHORIZBUTTON)
	{
		MENUPOS *menuitem = &allmenus->menu[nr];
		return(menuitem->item.horizbutton->value);
	}
	else
		DEBUGMESSCR("leak detected!\n");
	return(0);
}
//==========================================
void sethorizbuttonposfrommouse(MENUSTR *allmenus, int itemnr, int value)
{
	sethorizbuttonposfrommouse(allmenus, itemnr, value, false);
}
//==========================================
void sethorizbuttonposfrommouse(MENUSTR *allmenus, int itemnr, int value, int ignorecallback)
{
	int i;
	if (allmenus->menu[itemnr].itemtype == ISHORIZBUTTON)
	{
		MENUPOS *menuitem = &allmenus->menu[itemnr];
		if (menuitem->item.horizbutton->exactposition)
		{
			value -= 5;
			i = 0;
			if (value >= menuitem->item.horizbutton->xvalues[0])
			{
				for (i = 0;i < menuitem->item.horizbutton->metric;i++)
				{
					if (value < menuitem->item.horizbutton->xvalues[i])
						break;
				}
				if (i < menuitem->item.horizbutton->metric)
				{
					int delta1 = menuitem->item.horizbutton->xvalues[i] - value;
					int delta2 = value - menuitem->item.horizbutton->xvalues[i - 1];
					if (delta2 < delta1)
						i--;
				}
				else
					i--;
			}
			value = menuitem->item.horizbutton->xvalues[i];
			menuitem->item.horizbutton->value = i;
			menuitem->item.horizbutton->barxposition = value;
		}
		else
		{
			value -= 5;
			if (value < menuitem->item.horizbutton->minxvalue)
				value = menuitem->item.horizbutton->minxvalue;
			else
				if (value >= menuitem->item.horizbutton->maxxvalue)
					value = menuitem->item.horizbutton->maxxvalue - 1;
			menuitem->item.horizbutton->barxposition = value;
			menuitem->item.horizbutton->value = menuitem->item.horizbutton->barxposition*menuitem->item.horizbutton->metric / menuitem->item.horizbutton->maxxvalue;
		}
		if (menuitem->item.horizbutton->onselectitem_callback != NULL && !ignorecallback)
			menuitem->item.horizbutton->onselectitem_callback(allmenus, itemnr, menuitem->item.horizbutton->value);
	}
	else
		DEBUGMESSCR("leak detected!\n");
}
//==========================================
void drawhorizbutton(MENUSTR *allmenus, int itemnr)
{
	int i, sprnr, addx = 0;
	GRPFILE *dlggrp = allmenus->dlggrp;
	MENUPOS *menuitem = &allmenus->menu[itemnr];
	int showtype = menuitem->typeofshow;
	int deltax = (menuitem->hotdeltax - menuitem->item.horizbutton->sizexbar) / 2;
	for (i = 0;i < menuitem->item.horizbutton->sizexelem;i++)
	{
		if (!menuitem->item.horizbutton->allsprbar[i])
		{
			if (i == 0)
				sprnr = 0;
			else
				if (i == menuitem->item.horizbutton->sizexelem - 1)
					sprnr = 2;
				else
					sprnr = 1;
			sprnr = HORIZBAR[showtype][sprnr];
		}
		else
		{
			sprnr = HORIZHOLE[showtype];
		}
		GRP_PutNoPacked(menuitem->hotdeltax + deltax + addx, menuitem->hotdeltay, dlggrp, sprnr);
		addx += dlggrp->Picture[sprnr].PixelPerLine;
	}
	GRP_PutNoPacked(menuitem->hotdeltax + deltax + menuitem->item.horizbutton->barxposition, menuitem->hotdeltay, dlggrp, HORIZBUTTON[showtype]);
	//	  printf("%x\n",dlggrp->Picture[sprnr].OffsetForLine);
}
//==========================================
void delhorizbuttonitem(MENUSTR *allmenus, int nr, int end)
{
	if (allmenus->menu[nr].item.horizbutton->allsprbar)
	{
		wfree(allmenus->menu[nr].item.horizbutton->allsprbar);
		allmenus->menu[nr].item.horizbutton->allsprbar = NULL;
	}
	if (allmenus->menu[nr].item.horizbutton->xvalues)
	{
		wfree(allmenus->menu[nr].item.horizbutton->xvalues);
		allmenus->menu[nr].item.horizbutton->xvalues = NULL;
	}
	if (end&&allmenus->menu[nr].item.horizbutton)
	{
		wfree(allmenus->menu[nr].item.horizbutton);
		allmenus->menu[nr].item.horizbutton = NULL;
	}
}
//==========================================
void addsmkvideo(MENUSTR *allmenus, int nr, int hotx, int hoty, int sizex, int sizey, int nrofsmks, SMK_ARRAY *arrayofsmks)
{
	int smknrframes, i;
	HANDLE hmpq;
	smk_t *tempsmk;
	MENUPOS *menuitem = &allmenus->menu[nr];
	menuitem->item.smkvideo = (SMKVIDEO *)wmalloc(sizeof(SMKVIDEO));
	memset(menuitem->item.smkvideo, 0, sizeof(SMKVIDEO));
	menuitem->itemtype = ISSMKVIDEO;
	menuitem->hotdeltax = hotx + allmenus->x;
	menuitem->hotdeltay = hoty + allmenus->y;
	menuitem->hotxsize = sizex;
	menuitem->hotysize = sizey;
	menuitem->item.smkvideo->smkbutton = NULL;
	if (nrofsmks)
	{
		menuitem->item.smkvideo->smks = (ALLSMKS *)wmalloc(sizeof(ALLSMKS) + sizeof(SMKONEITEM)*nrofsmks);
		memset(menuitem->item.smkvideo->smks, 0, sizeof(ALLSMKS) + sizeof(SMKONEITEM)*nrofsmks);
		menuitem->item.smkvideo->smks->totalsmks = nrofsmks;
		for (i = 0;i < nrofsmks;i++)
		{

			if (arrayofsmks[i].smkfile)
			{
				hmpq = MpqsFindFile(arrayofsmks[i].smkfile);
				menuitem->item.smkvideo->hmpq = hmpq;
				tempsmk = smk_open_file(hmpq, arrayofsmks[i].smkfile, SMK_MODE_MEMORY);
				//				printf("loaded %s ring_frame=%d\n",arrayofsmks[i].smkfile,tempsmk->ring_frame);
				if (!tempsmk)
					return;
				smk_enable_video(tempsmk, 1);
				smk_first(tempsmk, SMK_FULL_DECODE);
				smknrframes = tempsmk->f + tempsmk->ring_frame;
				menuitem->item.smkvideo->smks->smkid[i].smk = tempsmk;
				smk_info_all(tempsmk, &menuitem->item.smkvideo->smks->smkid[i].curpos, &menuitem->item.smkvideo->smks->smkid[i].maxpos, NULL);
				menuitem->item.smkvideo->smks->smkid[i].posx = arrayofsmks[i].posx_smk + allmenus->x;
				menuitem->item.smkvideo->smks->smkid[i].posy = arrayofsmks[i].posy_smk + allmenus->y;
				if (menuitem->dialogbin_smk_flags[i] & DIALOGBIN_SMKFLAGS_ADDED_UNPACKALLFRAMES)
				{
					menuitem->item.smkvideo->smks->smkid[i].smkallframes = (unsigned char **)wmalloc(smknrframes * sizeof(unsigned char *));
					memset(menuitem->item.smkvideo->smks->smkid[i].smkallframes, 0, smknrframes * sizeof(unsigned char *));
					menuitem->item.smkvideo->smks->smkid[i].smknrframes = smknrframes;
				}
				menuitem->item.smkvideo->smks->smkid[i].smkfile = (char *)wmalloc(strlen(arrayofsmks[i].smkfile) + 1);
				strcpy(menuitem->item.smkvideo->smks->smkid[i].smkfile, arrayofsmks[i].smkfile);
			}
		}
	}
	if (menuitem->savedscrunderitem)
	{
		loadunderitem(allmenus, nr);
		saveunderitem(allmenus, nr);
	}
	//	  setmenuitem_DISABLED(allmenus,nr,FALSE);
	restoreflag(allmenus, nr, ITEM_RESTOREANDFREE);
}
//==========================================
void addbuttontosmk(MENUSTR *allmenus, int nr, int xpos, int ypos, int smksizex, int smksizey, int dialogbin_flags, char hotkey,
	int fontnr, char *text, unsigned int colors4,
	int buttonsize, int buttontype, int buttonflags)
{
	int i, len, len2, smkposx, smkposy, sizextext, sizeytext;
	char *poschar;
	MENUPOS *menuitem = &allmenus->menu[nr];
	menuitem->colors4 = colors4;
	menuitem->fontnr = fontnr;
	menuitem->hotkey = hotkey;
	smkposx = menuitem->hotdeltax;
	smkposy = menuitem->hotdeltax;
	menuitem->textxsize = smksizex;
	menuitem->textysize = smksizey;
	getmessagelen(fontnr, text, &sizextext, &sizeytext);

	menuitem->item.smkvideo->smkbutton = (struct BUTTON *)wmalloc(sizeof(BUTTON));
	memset(menuitem->item.smkvideo->smkbutton, 0, sizeof(BUTTON));

	len = strlen(text);
	len2 = strchrnul(text, '\n') - text;	//TODO
	for (i = 0;i < ITEMSHOW_MAXPOS;i++)
	{
		menuitem->item.smkvideo->smkbutton->menustr.text[i] = (char *)wmalloc(len + 1);
		strcpy(menuitem->item.smkvideo->smkbutton->menustr.text[i], text);
		if (i == ITEMSHOW_DISABLED)
			parse_special_text(menuitem->item.smkvideo->smkbutton->menustr.text[i], len, colors4, BUTTONCOLORS[i]);
		else
			parse_special_text(menuitem->item.smkvideo->smkbutton->menustr.text[i], len2, colors4, BUTTONCOLORS[i]);
	}
	if (dialogbin_flags & DIALOGBIN_FLAGS_SMKBUTTON_HORIZLEFT)
	{
		//		xpos=0;
		menuitem->item.smkvideo->buttonalign = 0;
	}
	else
		if (dialogbin_flags & DIALOGBIN_FLAGS_SMKBUTTON_HORIZCENTER)
		{
			//			xpos=xpos+(smksizex-sizextext)/2;
			menuitem->item.smkvideo->buttonalign = 1;
		}
		else
			if (dialogbin_flags & DIALOGBIN_FLAGS_SMKBUTTON_HORIZRIGHT)
			{
				xpos = xpos + smksizex - sizextext;
				menuitem->item.smkvideo->buttonalign = 2;
			}
			else
				menuitem->item.smkvideo->buttonalign = 3;
	if (dialogbin_flags & DIALOGBIN_FLAGS_SMKBUTTON_VERTTOP)
	{
		//		ypos=0;
	}
	else
		if (dialogbin_flags & DIALOGBIN_FLAGS_SMKBUTTON_VERTMIDDLE)
		{
			if (!menuitem->item.smkvideo->smks)			//vertical align the text in button if not smkvideo in item
				ypos = ypos + (smksizey - sizeytext) / 2;
		}
		else
			if (dialogbin_flags & DIALOGBIN_FLAGS_SMKBUTTON_VERTBOTTOM)
			{
				ypos = ypos + smksizey - sizeytext;
			}

	menuitem->item.smkvideo->smkbuttonxpos = xpos;
	menuitem->item.smkvideo->smkbuttonypos = ypos;
	menuitem->item.smkvideo->smkbutton->buttonsize = buttonsize;
	menuitem->item.smkvideo->smkbutton->buttontype = buttontype;
	menuitem->item.smkvideo->smkbutton->buttonflags = buttonflags;
}
//==========================================
/*void fadebuffinout(int fadeparam,unsigned char *vidbuff,int sizevidbuff,char *shadowbuff)
{
	fadeparam=fadeparam*FADECOUNTS/256;
	fadeparam=SMKFADETABLE[fadeparam]*256;
	shadowbuff+=fadeparam;
	do
	{
		if ((*vidbuff)!=0)
			*vidbuff=shadowbuff[*vidbuff];
		vidbuff++;
	}while(--sizevidbuff);
}
*/
//==========================================
void drawsmkvideo(MENUSTR *allmenus, int itemnr)
{
	int i, showsmk, smkfull;
	smk_t *smk;
	MENUPOS *menuitem = &allmenus->menu[itemnr];
	unsigned int w, h;
	unsigned char *vidbuff;
	if (menuitem->item.smkvideo->smks)
	{
		for (i = 0;i < menuitem->item.smkvideo->smks->totalsmks;i++)
		{
			showsmk = 0;
			smkfull = 0;
			smk = menuitem->item.smkvideo->smks->smkid[i].smk;
			if ((smk) && !(menuitem->dialogbin_smk_flags[i] & DIALOGBIN_SMKFLAGS_NODRAW))
			{
				if (menuitem->dialogbin_smk_flags[i] & DIALOGBIN_SMKFLAGS_SHOWONMOUSEOVER)
				{
					if (menuitem->typeofshow == ITEMSHOW_FOCUS || menuitem->typeofshow == ITEMSHOW_CLICK)
						showsmk = 1;
				}
				else
					showsmk = 1;
				smk_info_video(smk, &w, &h, NULL);
				if (menuitem->dialogbin_smk_flags[i] & DIALOGBIN_SMKFLAGS_ADDED_UNPACKALLFRAMES)
				{
					vidbuff = menuitem->item.smkvideo->smks->smkid[i].smkallframes[smk->cur_frame];
					if (!vidbuff)
					{
						vidbuff = smk_get_video(smk);
						menuitem->item.smkvideo->smks->smkid[i].smkallframes[smk->cur_frame] = (unsigned char *)wmalloc(w*h);
						memcpy(menuitem->item.smkvideo->smks->smkid[i].smkallframes[smk->cur_frame], vidbuff, w*h);
						smkfull = 1;
					}
				}
				else
					vidbuff = smk_get_video(smk);
				if (showsmk)
				{
					if (menuitem->dialogbin_smk_flags[i] & DIALOGBIN_SMKFLAGS_FADEMOUSEOVER)
					{
						puttranslucencyrow(menuitem->item.smkvideo->smks->smkid[i].posx,
										   menuitem->item.smkvideo->smks->smkid[i].posy,
										   w, h, vidbuff, menuitem->showvalue / 127 + 1);
					}
					else
					{
						putrow(menuitem->item.smkvideo->smks->smkid[i].posx,
								menuitem->item.smkvideo->smks->smkid[i].posy,
								w, h, vidbuff);
					}
				}
				if (scrollportrait_menu)
				{
					if (menuitem->dialogbin_smk_flags[i] & DIALOGBIN_SMKFLAGS_ADDED_UNPACKALLFRAMES)
					{
						if (menuitem->dialogbin_smk_flags[i] & DIALOGBIN_SMKFLAGS_ADDED_UNPACKREADY)
						{
							if (smk_next(smk, SMK_SKIP_DECODE) == SMK_DONE)
								smk_first(smk, SMK_SKIP_DECODE);
						}
						else
						{
							if (smk_next(smk, SMK_FULL_DECODE) == SMK_LAST)
							{
								menuitem->dialogbin_smk_flags[i] |= DIALOGBIN_SMKFLAGS_ADDED_UNPACKREADY;
								if (!(menuitem->dialogbin_smk_flags[i] & DIALOGBIN_SMKFLAGS_LOOPANIMATION))
								{
									menuitem->dialogbin_smk_flags[i] |= DIALOGBIN_SMKFLAGS_NODRAW;
								}
							}
						}
					}
					else
						if (smk_next(smk, SMK_FULL_DECODE) == SMK_DONE)
						{
							if (menuitem->dialogbin_smk_flags[i] & DIALOGBIN_SMKFLAGS_LOOPANIMATION)
								smk_first(smk, SMK_FULL_DECODE);
							else
								menuitem->dialogbin_smk_flags[i] |= DIALOGBIN_SMKFLAGS_NODRAW;
						}
				}
			}
		}
	}
	drawsmkbuttonitem(allmenus, itemnr, menuitem->item.smkvideo->smkbutton,
		menuitem->item.smkvideo->smkbuttonxpos,
		menuitem->item.smkvideo->smkbuttonypos);
}
//==========================================
void delsmkvideoitem(MENUSTR *allmenus, int nr, int end)
{
	int j, k;
	//wfree memory with unpacked smk frames
	if (allmenus->menu[nr].item.smkvideo->smks)
	{
		for (j = 0;j < allmenus->menu[nr].item.smkvideo->smks->totalsmks;j++)
		{
			if (allmenus->menu[nr].item.smkvideo->smks->smkid[j].smkfile)
				wfree(allmenus->menu[nr].item.smkvideo->smks->smkid[j].smkfile);
			if (allmenus->menu[nr].item.smkvideo->smks->smkid[j].smk)
				smk_close(allmenus->menu[nr].item.smkvideo->smks->smkid[j].smk);
			for (k = 0;k < allmenus->menu[nr].item.smkvideo->smks->smkid[j].smknrframes;k++)
				if (allmenus->menu[nr].item.smkvideo->smks->smkid[j].smkallframes[k])
					wfree(allmenus->menu[nr].item.smkvideo->smks->smkid[j].smkallframes[k]);
			if (allmenus->menu[nr].item.smkvideo->smks->smkid[j].smknrframes)
				wfree(allmenus->menu[nr].item.smkvideo->smks->smkid[j].smkallframes);
		}
	}
	if (allmenus->menu[nr].item.smkvideo->smks)
	{
		wfree(allmenus->menu[nr].item.smkvideo->smks);
		allmenus->menu[nr].item.smkvideo->smks = NULL;
	}
	if (end&&allmenus->menu[nr].item.smkvideo)
	{
		wfree(allmenus->menu[nr].item.smkvideo);
		allmenus->menu[nr].item.smkvideo = NULL;
	}
}
//==========================================
void drawsmkbuttonitem(MENUSTR *allmenus, int itemnr, BUTTON *button, int xpos, int ypos)
{
	int buttonsize, sprnr0, sprnr1, sprnr2, sizexgrp, i, buttontype, glowoffset;
	if (!button)
		return;
	GRPFILE *dlggrp = allmenus->dlggrp;
	MENUPOS *menuitem = &allmenus->menu[itemnr];
	char *fonttable = allmenus->fonttable;
	int showtype = menuitem->typeofshow;
	int colors4 = menuitem->colors4;
	int rowsize;
	getmaxsymbolsize(menuitem->fontnr, NULL, &rowsize);


	buttonsize = button->buttonsize;
	if (buttonsize > 0)//have button decoration
	{
		buttontype = button->buttontype;
		sprnr0 = BUTTONDECOR[buttontype][showtype][0];
		sprnr1 = BUTTONDECOR[buttontype][showtype][1];
		sprnr2 = BUTTONDECOR[buttontype][showtype][2];
		sizexgrp = dlggrp->Picture[sprnr0].PixelPerLine;
		i = 0;
		GRP_PutNoPacked(menuitem->hotdeltax + i * sizexgrp, menuitem->hotdeltay, dlggrp, sprnr0);
		i++;
		for (;i < buttonsize - 1;i++)
			GRP_PutNoPacked(menuitem->hotdeltax + i * sizexgrp, menuitem->hotdeltay, dlggrp, sprnr1);
		GRP_PutNoPacked(menuitem->hotdeltax + i * sizexgrp, menuitem->hotdeltay, dlggrp, sprnr2);
	}
	if (showtype == ITEMSHOW_FOCUS)
	{
		if (menuitem->item.smkvideo->smkbutton->buttonflags&BF_TEXTGLOWED)
		{
			glowoffset = menuitem->showvalue / 128;
			switch (menuitem->item.smkvideo->buttonalign)
			{
			case 0:
				putsmkbuttonmesL(menuitem->hotdeltax + xpos, menuitem->hotdeltay + ypos, rowsize,
					button->menustr.text[showtype], menuitem->fontnr, GETITEMCOLOR(colors4, showtype),
					fonttable, 1 - glowoffset);
				break;
			case 1:
				putsmkbuttonmesM(menuitem->hotdeltax + menuitem->item.smkvideo->smkbuttonxpos,
					menuitem->hotdeltay + menuitem->item.smkvideo->smkbuttonypos,
					menuitem->textxsize - menuitem->item.smkvideo->smkbuttonxpos, rowsize,
					button->menustr.text[showtype], menuitem->fontnr, GETITEMCOLOR(colors4, showtype),
					fonttable, 1 - glowoffset);
				break;
			case 2:
				putsmkbuttonmesR(menuitem->hotdeltax + xpos, menuitem->hotdeltay + ypos, menuitem->hotxsize, rowsize,
					button->menustr.text[showtype], menuitem->fontnr, GETITEMCOLOR(colors4, showtype),
					fonttable, 1 - glowoffset);
				break;
			case 3:
				putsmkbuttonmes(menuitem->hotdeltax + xpos, menuitem->hotdeltay + ypos, rowsize,
					button->menustr.text[showtype], menuitem->fontnr, GETITEMCOLOR(colors4, showtype),
					fonttable, 1 - glowoffset);
				break;
			}
			return;
		}
	}
	glowoffset = 1;
	switch (menuitem->item.smkvideo->buttonalign)
	{
	case 0:
		putsmkbuttonmesL(menuitem->hotdeltax + xpos, menuitem->hotdeltay + ypos, rowsize,
			button->menustr.text[showtype], menuitem->fontnr, GETITEMCOLOR(colors4, showtype),
			fonttable, 1 - glowoffset);
		break;
	case 1:
		putsmkbuttonmesM(menuitem->hotdeltax + menuitem->item.smkvideo->smkbuttonxpos,
			menuitem->hotdeltay + menuitem->item.smkvideo->smkbuttonypos,
			menuitem->textxsize - menuitem->item.smkvideo->smkbuttonxpos, rowsize,
			button->menustr.text[showtype], menuitem->fontnr, GETITEMCOLOR(colors4, showtype),
			fonttable, 1 - glowoffset);
		break;
	case 2:
		putsmkbuttonmesR(menuitem->hotdeltax + xpos, menuitem->hotdeltay + ypos, menuitem->hotxsize, rowsize,
			button->menustr.text[showtype], menuitem->fontnr, GETITEMCOLOR(colors4, showtype),
			fonttable, 1 - glowoffset);
		break;
	case 3:
		putsmkbuttonmes(menuitem->hotdeltax + xpos, menuitem->hotdeltay + ypos, rowsize,
			button->menustr.text[showtype], menuitem->fontnr, GETITEMCOLOR(colors4, showtype),
			fonttable, 1 - glowoffset);
		break;
	}
}
//==========================================
void delsmkbuttonitem(MENUSTR *allmenus, int nr, int end)
{
	int j;
	if (allmenus->menu[nr].item.smkvideo->smkbutton)
	{
		for (j = 0;j < ITEMSHOW_MAXPOS;j++)
			if (allmenus->menu[nr].item.smkvideo->smkbutton->menustr.text[j])
				wfree(allmenus->menu[nr].item.smkvideo->smkbutton->menustr.text[j]);
		wfree(allmenus->menu[nr].item.smkvideo->smkbutton);
		allmenus->menu[nr].item.smkvideo->smkbutton = NULL;
	}
}
//==========================================
void setsmkflags(MENUSTR *allmenus, int itemnr, int smknr, unsigned char smkflags)
{
	MENUPOS *menuitem = &allmenus->menu[itemnr];
	menuitem->dialogbin_smk_flags[smknr] = smkflags;
}
//==========================================
void updateimageitem(MENUSTR *allmenus, int nr, char *bufpixels)
{
	MENUPOS *menuitem = &allmenus->menu[nr];
	if (menuitem->itemtype == ISIMAGE)
	{
		memcpy(menuitem->item.image->pcx->GetPcxRawBytes(), bufpixels, menuitem->item.image->pcx->sizePcx());
	}
	else
		DEBUGMESSCR("leak detected!\n");
}
//==========================================
void changeimageitem(MENUSTR *allmenus, int nr, const char *pcxfile)
{
	MENUPOS *menuitem = &allmenus->menu[nr];
	if (menuitem->itemtype == ISIMAGE)
	{
		PCX *pcx = (PCX *)wmalloc(sizeof(PCX));
		HANDLE hmpq = MpqsFindFile(pcxfile);

		int err = pcx->openMpqPcx(pcxfile);
		if (err != OKPCX)
		{
			wfree(pcx);
			return;
		}
		if (!menuitem->item.image)
		{
			addimageitem(allmenus, nr);
		}
		else
			delimageitem(allmenus, nr, 0);
		menuitem->item.image->hmpq = hmpq;
		int len = strlen(pcxfile);
		menuitem->item.image->pcxfilename = (char *)wmalloc(len + 1);
		strcpy(menuitem->item.image->pcxfilename, pcxfile);
		menuitem->item.image->pcx = pcx;
		menuitem->hotxsize = (short) pcx->xsizePcx();
		menuitem->hotysize = (short) pcx->ysizePcx();
		setimageitem_visibilityxy(allmenus, nr, menuitem->hotxsize, menuitem->hotysize);
	}
	else
		DEBUGMESSCR("leak detected!\n");

}
//==========================================
void addimageitem(MENUSTR *allmenus, int nr)
{
	MENUPOS *menuitem = &allmenus->menu[nr];

	menuitem->item.image = (IMAGE *)wmalloc(sizeof(IMAGE));
	memset(menuitem->item.image, 0, sizeof(IMAGE));
	menuitem->itemtype = ISIMAGE;
	menuitem->item.image->pcx = NULL;
	menuitem->item.image->flags = 0;

	setimgtransparentcolors(allmenus, nr, 255, 255);
	setimgtranslucencyvalue(allmenus, nr, PCXSHOW_25);

	if (menuitem->savedscrunderitem)
	{
		loadunderitem(allmenus, nr);
		saveunderitem(allmenus, nr);
	}
	restoreflag(allmenus, nr, ITEM_RESTOREANDFREE);
}
//==========================================
void addimageitem(MENUSTR *allmenus, int nr, int hotx, int hoty, int sizex, int sizey, char *pcxfile)
{
	MENUPOS *menuitem = &allmenus->menu[nr];

	menuitem->item.image = (IMAGE *)wmalloc(sizeof(IMAGE));
	memset(menuitem->item.image, 0, sizeof(IMAGE));
	menuitem->itemtype = ISIMAGE;
	if (hotx != -1 && hoty != -1)
	{
		menuitem->hotdeltax = hotx + allmenus->x;
		menuitem->hotdeltay = hoty + allmenus->y;
	}
	if (sizex != -1 && sizey != -1)
	{
		menuitem->hotxsize = sizex;
		menuitem->hotysize = sizey;
	}
	menuitem->item.image->pcx = NULL;
	menuitem->item.image->flags = 0;
	if (pcxfile&&pcxfile[0])
	{
		changeimageitem(allmenus, nr, pcxfile);
	}
	setimgtransparentcolors(allmenus, nr, 255, 255);
	setimgtranslucencyvalue(allmenus, nr, PCXSHOW_25);
	setimageitem_visibilityxy(allmenus, nr, menuitem->hotxsize, menuitem->hotysize);

	if (menuitem->savedscrunderitem)
	{
		loadunderitem(allmenus, nr);
		saveunderitem(allmenus, nr);
	}
	//	  setmenuitem_DISABLED(allmenus,nr,FALSE);
	restoreflag(allmenus, nr, ITEM_RESTOREANDFREE);
}
//==========================================
void addimagearray(MENUSTR *allmenus, int nr, int hotx, int hoty, int sizex, int sizey)
{
	MENUPOS *menuitem = &allmenus->menu[nr];
	menuitem->item.image = (IMAGE *)wmalloc(sizeof(IMAGE));
	memset(menuitem->item.image, 0, sizeof(IMAGE));
	menuitem->itemtype = ISIMAGE;
	if (hotx != -1 && hoty != -1)
	{
		menuitem->hotdeltax = hotx + allmenus->x;
		menuitem->hotdeltay = hoty + allmenus->y;
	}
	if (sizex != -1 && sizey != -1)
	{
		menuitem->hotxsize = sizex;
		menuitem->hotysize = sizey;
	}
	PCX *pcx = (PCX *)wmalloc(sizeof(PCX));
	memset(pcx, 0, sizeof(PCX));
	pcx->CreatePcx(menuitem->hotxsize, menuitem->hotysize);//create randomly array for pcx
	menuitem->item.image->hmpq = NULL;
	menuitem->item.image->pcx = pcx;
	menuitem->item.image->pcxfilename = NULL;
	setimgtransparentcolors(allmenus, nr, 255, 255);
	setimgtranslucencyvalue(allmenus, nr, PCXSHOW_25);
	setimageitem_visibilityxy(allmenus, nr, menuitem->hotxsize, menuitem->hotysize);
	if (menuitem->savedscrunderitem)
	{
		loadunderitem(allmenus, nr);
		saveunderitem(allmenus, nr);
	}
	//	  setmenuitem_DISABLED(allmenus,nr,FALSE);
	restoreflag(allmenus, nr, ITEM_RESTOREANDFREE);
}
//==========================================
void changegrpitem(MENUSTR *allmenus, int nr, GRPFILE *grp, int grpnr)
{
	delimageitem(allmenus, nr, 0);
	MENUPOS *menuitem = &allmenus->menu[nr];
	menuitem->item.image->grp = grp;
	menuitem->item.image->grpnr = grpnr;
	menuitem->item.image->flags |= IMAGEFLAGS_GRPIMAGE;
}
//==========================================
void setimgtranslucencyvalue(MENUSTR *allmenus, int nr, unsigned char trans)
{
	if (allmenus->menu[nr].itemtype == ISIMAGE)
		allmenus->menu[nr].item.image->transvalue = trans;
	else
		DEBUGMESSCR("leak detected!\n");
}
//==========================================
void setimgtransparentcolors(MENUSTR *allmenus, int nr, unsigned char color1, unsigned char color2)
{
	if (allmenus->menu[nr].itemtype == ISIMAGE)
	{
		allmenus->menu[nr].item.image->color1 = color1;
		allmenus->menu[nr].item.image->color2 = color2;
	}
	else
		DEBUGMESSCR("leak detected!\n");
}
//==========================================
void setimageitem_visibilityxy(MENUSTR *allmenus, int nr, int visiblex, int visibley)
{
	MENUPOS *menuitem = &allmenus->menu[nr];
	if (menuitem->itemtype == ISIMAGE)
	{
		if (visiblex >= 0)
			menuitem->item.image->visiblex = visiblex;
		if (visibley >= 0)
			menuitem->item.image->visibley = visibley;
	}
	else
		DEBUGMESSCR("leak detected!\n");
}
//==========================================
void drawimageitem(MENUSTR *allmenus, int itemnr)
{
	MENUPOS *menuitem = &allmenus->menu[itemnr];
	PCX *pcx = menuitem->item.image->pcx;
	if (pcx)
		pcx->PutPcx(menuitem->hotdeltax, menuitem->hotdeltay,
			menuitem->item.image->visiblex,
			menuitem->item.image->color1, menuitem->item.image->color2,
			menuitem->item.image->transvalue);
	GRPFILE *grp = menuitem->item.image->grp;
	if (grp)
	{
		int imgnr = menuitem->item.image->grpnr;
		if (menuitem->item.image->flags&IMAGEFLAGS_BOTTOMALIGN)
			//top-align
			GRP_PutNoPacked(menuitem->hotdeltax, menuitem->hotdeltay, grp, imgnr);
		else //botom-aling
			GRP_PutNoPacked(menuitem->hotdeltax,
				menuitem->hotdeltay + menuitem->hotysize - grp->Picture[imgnr].LinesPerPicture,
				grp, imgnr);
	}
}
//==========================================
void delimageitem(MENUSTR *allmenus, int nr, int end)
{
	if (allmenus->menu[nr].item.image->pcx)
	{
		allmenus->menu[nr].item.image->pcx->closePcx();
		wfree(allmenus->menu[nr].item.image->pcx);
		allmenus->menu[nr].item.image->pcx = NULL;
	}
	if (allmenus->menu[nr].item.image->pcxfilename)
	{
		wfree(allmenus->menu[nr].item.image->pcxfilename);
		allmenus->menu[nr].item.image->pcxfilename = NULL;
	}
	if (end && allmenus->menu[nr].item.image)
	{
		wfree(allmenus->menu[nr].item.image);
		allmenus->menu[nr].item.image = NULL;
	}
}
//==========================================
void drawdecorate(MENUSTR *allmenus, int itemnr)
{
	int color;
	MENUPOS *menuitem = &allmenus->menu[itemnr];
	int showtype = menuitem->typeofshow;
	if (menuitem->dialogbin_flags&DIALOGBIN_FLAGS_KEYMOUSERESPONDEVENTS)
	{
		if (allmenus->defaultbutton == itemnr)
		{
			showtype = ITEMSHOW_CLICK;
		}
		color = GETITEMCOLOR(menuitem->decorcolors4, showtype);
		if (color)
		{
			int x2 = menuitem->hotdeltax + menuitem->hotxsize;
			int y2 = menuitem->hotdeltay + menuitem->hotysize;
			wrectangle2(color, menuitem->hotdeltax, menuitem->hotdeltay, x2, y2);
		}
	}
}
//==========================================
void setincrementvalue(MENUSTR *allmenus, unsigned char value)
{
	allmenus->incrementfactor = value;
}
//==========================================
void addmenupalette(MENUSTR *allmenus, unsigned char *palette)
{
	allmenus->palette = palette;
}
//==========================================
int MenuFindFirstElem(MENUSTR *allmenus, int typeelem)
{
	int i;
	for (i = 0;i < allmenus->elements;i++)
	{
		if (allmenus->menu[i].itemtype == typeelem)
			return(i);
	}
	return(-1);
}
//==========================================
void setitemflags(MENUSTR *allmenus, int itemnr, int	dialogbin_flags)
{
	allmenus->menu[itemnr].dialogbin_flags = dialogbin_flags;
}
//==========================================
void setfontnr(MENUSTR *allmenus, int itemnr, int fontid)
{
	allmenus->menu[itemnr].fontnr = fontid;
}
//==========================================
int menuitem_ISVISIBLED(MENUSTR *allmenus, int itemnr)
{
	return(allmenus->menu[itemnr].dialogbin_flags&DIALOGBIN_FLAGS_ITEMVISIBLED);
}
//==========================================
int menuitem_ISDISABLED(MENUSTR *allmenus, int itemnr)
{
	return(allmenus->menu[itemnr].dialogbin_flags&DIALOGBIN_FLAGS_ITEMDISABLED);
}
//==========================================
int menuitem_ISENABLED(MENUSTR *allmenus, int itemnr)
{
	return(!(allmenus->menu[itemnr].dialogbin_flags&DIALOGBIN_FLAGS_ITEMDISABLED));
}
//==========================================
void setmenuitem_VISIBLED(MENUSTR *allmenus, int itemnr, int mybool)
{
	if (mybool == TRUE)
		allmenus->menu[itemnr].dialogbin_flags |= DIALOGBIN_FLAGS_ITEMVISIBLED;
	else
		allmenus->menu[itemnr].dialogbin_flags &= ~DIALOGBIN_FLAGS_ITEMVISIBLED;
}
//==========================================
void setmenuitem_DISABLED(MENUSTR *allmenus, int itemnr, int mybool)
{
	if (mybool == TRUE)
		allmenus->menu[itemnr].dialogbin_flags |= DIALOGBIN_FLAGS_ITEMDISABLED;
	else
		allmenus->menu[itemnr].dialogbin_flags &= ~DIALOGBIN_FLAGS_ITEMDISABLED;
}
//==========================================
void setmenuitem_RESPONDEVENTS(MENUSTR *allmenus, int itemnr, int mybool)
{
	if (mybool == TRUE)
		allmenus->menu[itemnr].dialogbin_flags |= DIALOGBIN_FLAGS_KEYMOUSERESPONDEVENTS;
	else
		allmenus->menu[itemnr].dialogbin_flags &= ~DIALOGBIN_FLAGS_KEYMOUSERESPONDEVENTS;
}
//==========================================
void showitemborders(MENUSTR *allmenus, int itemnr)
{
#ifdef DECORENABLE
	int color;
	MENUPOS *menuitem = &allmenus->menu[itemnr];
	int showtype = menuitem->typeofshow;
	color = 0xff;
	int x2 = menuitem->hotdeltax + menuitem->hotxsize;
	int y2 = menuitem->hotdeltay + menuitem->hotysize;
	wrectangle2(color, menuitem->hotdeltax, menuitem->hotdeltay, x2, y2);
#ifdef DECORTEXTENABLE
	char strr[20];
	sprintf(strr, "%d", itemnr);
	putmessage(menuitem->hotdeltax + 3, menuitem->hotdeltay + 3, IDFONT8, strr, 4, allmenus->fonttable, allmenus->dlggrp);
#endif
#endif
}
//==========================================
void showprogress(int x, int y, int sizex, int percent, int colornr, GRPFILE *grp)
{
	int i, sprsize, sprnr, deltax, showpix, lastshowpix;
	sprsize = sizex / 16 - 1;
	colornr *= 6;
	deltax = 0;
	lastshowpix = sizex * percent / 100;
	//at first show the borders
	for (i = 0;i <= sprsize;i++)
	{
		if (!i)
		{
			sprnr = 9 + colornr;
		}
		else if (i == sprsize)
		{
			sprnr = 11 + colornr;
		}
		else
		{
			sprnr = 10 + colornr;
		}
		GRP_PutNoPacked(x + deltax, y, grp, sprnr);
		deltax += 16;
	}
	deltax = 0;
	for (i = 0;i <= sprsize;i++)
	{
		if (!i)
		{
			sprnr = 6 + colornr;
		}
		else if (i == sprsize)
		{
			sprnr = 8 + colornr;
		}
		else
		{
			sprnr = 7 + colornr;
		}
		showpix = 16;
		if (lastshowpix < showpix)
			showpix = lastshowpix;
		putgrp_nopacked2(x + deltax, y, showpix, 256, grp, sprnr);
		deltax += 16;
		lastshowpix -= 16;
		if (lastshowpix <= 0)
			break;
	}
}
//==========================================
char tempmes[30];
void drawtextpercent(MENUSTR *allmenus, int nr, char *mes, int fontnr, int colorgrp, int colortext)
{
	int min, total, percent;
	char *from2;
	strcpy(tempmes, mes);

	min = atoi(tempmes);
	from2 = strchr(tempmes, ' ');
	from2[0] = 0;
	total = atoi(from2 + 1);
	if (!total)
		total = 1;
	percent = min * 100 / total;
	if (percent > 100)
		percent = 100;
	if (percent < 0)
		percent = 0;
	showprogress(allmenus->menu[nr].hotdeltax, allmenus->menu[nr].hotdeltay,
		allmenus->menu[nr].hotxsize, percent, colorgrp, allmenus->dlggrp);
	putmessageinrectangleM(allmenus->menu[nr].hotdeltax, allmenus->menu[nr].hotdeltay + 2,
		allmenus->menu[nr].hotxsize, allmenus->menu[nr].hotysize, allmenus->menu[nr].hotysize,
		tempmes, fontnr, colortext, allmenus->fonttable, allmenus->dlggrp, 1);
}
//==========================================
void  addlistboxitem(MENUSTR *allmenus, int nr, int hotx, int hoty,
	int hotsizex, int hotsizey, int textx, int texty,
	int fontnr, unsigned int colors4, int decorcolor)
{
	MENUPOS *menuitem = &allmenus->menu[nr];
	menuitem->item.listbox = (LISTBOX *)wmalloc(sizeof(LISTBOX));
	memset(menuitem->item.listbox, 0, sizeof(LISTBOX));
	menuitem->itemtype = ISLISTBOX;
	menuitem->colors4 = colors4;
	menuitem->hotdeltax = hotx + allmenus->x;
	menuitem->hotdeltay = hoty + allmenus->y;
	menuitem->hotxsize = hotsizex;
	menuitem->hotysize = hotsizey;
	menuitem->xtextdelta = textx;
	menuitem->ytextdelta = texty;
	menuitem->fontnr = fontnr;
	menuitem->hotkey = 255;
	menuitem->item.listbox->flist = NULL;
	menuitem->item.listbox->decorcolor = decorcolor;
	menuitem->item.listbox->selectednr = -1;
	menuitem->item.listbox->lines = 0;
	menuitem->item.listbox->onselectitem_callback = NULL;
	menuitem->item.listbox->bar = NULL;
	menuitem->item.listbox->sizexarea = hotsizex;
	if (menuitem->savedscrunderitem)
	{
		loadunderitem(allmenus, nr);
		saveunderitem(allmenus, nr);
	}
	setmenuitem_DISABLED(allmenus, nr, TRUE);
	restoreflag(allmenus, nr, ITEM_ONLYFREE);
	return;
}
//==========================================
void listboxlineitems(MENUSTR *allmenus, int nr, int lines, int pixelsbetweenlines)
{
	if (allmenus->menu[nr].itemtype == ISLISTBOX)
	{
		allmenus->menu[nr].item.listbox->lines = lines;
		allmenus->menu[nr].item.listbox->pixelsbetweenlines = pixelsbetweenlines;
	}
	else
		DEBUGMESSCR("leak detected!\n");
}
//==========================================
int getlistbox_selecteditem(MENUSTR *allmenus, int nr)
{
	if (allmenus->menu[nr].itemtype == ISLISTBOX)
	{
		MENUPOS *menuitem = &allmenus->menu[nr];
		if (menuitem->item.listbox)
			if (menuitem->item.listbox->flist)
				return(menuitem->item.listbox->selectednr);
	}
	else
		DEBUGMESSCR("leak detected!\n");
	return(-1);
}
//==========================================
char *getlistbox_selecteditemname(MENUSTR *allmenus, int nr)
{
	if (allmenus->menu[nr].itemtype == ISLISTBOX)
	{
		MENUPOS *menuitem = &allmenus->menu[nr];
		if (menuitem->item.listbox)
			if (menuitem->item.listbox->flist)
			{
				if (menuitem->item.listbox->selectednr != -1)
					return((char *)menuitem->item.listbox->flist->GetElemNr(menuitem->item.listbox->selectednr));
			}
	}
	else
		DEBUGMESSCR("leak detected!\n");
	return(NULL);
}
//==========================================
#define DLGGRP_SIZEX(grpnr) allmenus->dlggrp->Picture[grpnr].PixelPerLine
#define DLGGRP_SIZEY(grpnr) allmenus->dlggrp->Picture[grpnr].LinesPerPicture


char LISTBOX_TOPBUTTON[4] = { 16,17,18,17 };		//disabled,nofocus,click,focus
char LISTBOX_BOTTOMBUTTON[4] = { 19,20,21,20 };		//disabled,nofocus,click,focus

#define LISTBOX_BUTTONBAR		28
#define LISTBOX_TOPBAR			29
#define LISTBOX_MIDDLEBAR		30
#define LISTBOX_BOTTOMBAR		31
//==========================================
void calclistboxvertbar(MENUSTR *allmenus, int nr)
{
	int y1, sy, syelem;
	LISTBOXBAR *bar;
	MENUPOS *menuitem = &allmenus->menu[nr];
	if (!menuitem->item.listbox->bar)
	{
		bar = (LISTBOXBAR *)wmalloc(sizeof(LISTBOXBAR));
		menuitem->item.listbox->bar = bar;
	}
	else
	{
		bar = menuitem->item.listbox->bar;
	}
	memset(bar, 0, sizeof(LISTBOXBAR));

	bar->xt_1 = menuitem->hotdeltax + menuitem->item.listbox->sizexarea;
	bar->xt_2 = menuitem->hotdeltax + menuitem->item.listbox->sizexarea + DLGGRP_SIZEX(LISTBOX_TOPBUTTON[ITEMSHOW_NOFOCUS]) - 1;
	y1 = menuitem->hotdeltay;
	bar->yt_1 = y1;
	y1 += DLGGRP_SIZEY(LISTBOX_TOPBUTTON[ITEMSHOW_NOFOCUS]);
	bar->yt_2 = y1 - 1;
	y1 += 5;
	bar->yh_1 = y1;
	y1 += DLGGRP_SIZEY(LISTBOX_TOPBAR);

	sy = menuitem->hotysize;
	sy -= DLGGRP_SIZEY(LISTBOX_TOPBUTTON[ITEMSHOW_NOFOCUS]) + 5 +
		DLGGRP_SIZEY(LISTBOX_TOPBAR) +
		DLGGRP_SIZEY(LISTBOX_BOTTOMBAR) +
		DLGGRP_SIZEY(LISTBOX_BOTTOMBUTTON[ITEMSHOW_NOFOCUS]) + 5;
	syelem = (sy + DLGGRP_SIZEY(LISTBOX_MIDDLEBAR) - 1) / DLGGRP_SIZEY(LISTBOX_MIDDLEBAR);
	bar->elemtodraw = syelem;

	y1 = menuitem->hotdeltay + menuitem->hotysize;
	bar->yb_2 = y1 - 1;
	y1 -= DLGGRP_SIZEY(LISTBOX_BOTTOMBUTTON[ITEMSHOW_NOFOCUS]);
	bar->yb_1 = y1;
	y1 -= 5;
	bar->yh_2 = y1 - 1;
	y1 -= DLGGRP_SIZEY(LISTBOX_BOTTOMBAR);

	bar->yh_last = y1;

	bar->showtypet = ITEMSHOW_NOFOCUS;
	bar->showtypeb = ITEMSHOW_NOFOCUS;


	bar->buttonbarymax = bar->yh_2 - DLGGRP_SIZEY(LISTBOX_BUTTONBAR) - 1;

	calclistboxbuttonbarpos(allmenus, nr);

}
//==========================================
void calclistboxbuttonbarpos(MENUSTR *allmenus, int nr)
{
	int buttonbariter, curelem, maxelem;
	LISTBOXBAR *bar;
	MENUPOS *menuitem = &allmenus->menu[nr];
	bar = menuitem->item.listbox->bar;
	if (bar)
	{
		maxelem = menuitem->item.listbox->flist->Count() - menuitem->item.listbox->lines;
		if (maxelem <= 0)
		{
			wfree(menuitem->item.listbox->bar);
			menuitem->item.listbox->bar = NULL;
			return;
		}
		buttonbariter = bar->buttonbarymax - bar->yh_1;
		curelem = menuitem->item.listbox->from;
		//		changelistbox_fromto(allmenus,nr,0);
		bar->buttonbary1 = curelem * buttonbariter / maxelem + bar->yh_1;
		bar->buttonbary2 = bar->buttonbary1 + DLGGRP_SIZEY(LISTBOX_BUTTONBAR) - 1;
	}
}
//==========================================
void listbox_addtext(MENUSTR *allmenus, int nr, char *text)
{
	int listelements = 0;
	MENUPOS *menuitem = &allmenus->menu[nr];
	mylist *listbox;
	if (menuitem->itemtype == ISLISTBOX)
	{
		listbox = menuitem->item.listbox->flist;
		if (listbox)
		{
			listbox->AddList(text);
			listelements = listbox->Count();
			menuitem->item.listbox->selectednr = listelements - 1;
			if (listelements > menuitem->item.listbox->lines)
			{
				menuitem->item.listbox->sizexarea = menuitem->hotxsize - DLGGRP_SIZEX(LISTBOX_MIDDLEBAR);
				calclistboxvertbar(allmenus, nr);
			}
			else
			{
				menuitem->item.listbox->sizexarea = menuitem->hotxsize;
			}
		}
		if (listelements)
		{
			changelistbox_selectednr(allmenus, nr, 0);
			changelistbox_fromto(allmenus, nr, 0);
			setmenuitem_DISABLED(allmenus, nr, FALSE);
		}
		else
		{
			changelistbox_selectednr(allmenus, nr, 0);
			setmenuitem_DISABLED(allmenus, nr, TRUE);
		}
	}
	else
		DEBUGMESSCR("leak detected!\n");
}
//==========================================
void setlistbox_lists(MENUSTR *allmenus, int nr, int selectednr, mylist *listbox)
{
	int listelements = 0, prevlistelements = 0;
	MENUPOS *menuitem = &allmenus->menu[nr];
	if (menuitem->itemtype == ISLISTBOX)
	{
		if (listbox)
		{
			if (!menuitem->item.listbox->pixelsbetweenlines)
			{
				DEBUGMESSCR("missing listboxlineitems(MENU, %d , lines, pixelsbetweenlines);\n" \
					"not initialized for item %d (lines & pixelsbetweenlines equals with zero)\n", nr, nr);
			}
			prevlistelements = listbox->Count();
			if (menuitem->item.listbox->flist != listbox)
				dellistboxitem(allmenus, nr, 0, MENUITEM_LISTBOX_DEALLOCLIST);
			listelements = listbox->Count();
			menuitem->item.listbox->flist = listbox;
			if ((prevlistelements != listelements || menuitem->item.listbox->selectednr != selectednr) && listelements == 0)
				menuitem->item.listbox->selectednr = 0;
			else
				menuitem->item.listbox->selectednr = selectednr;
			if (listelements > menuitem->item.listbox->lines)
			{
				menuitem->item.listbox->sizexarea = menuitem->hotxsize - DLGGRP_SIZEX(LISTBOX_MIDDLEBAR);
				calclistboxvertbar(allmenus, nr);
			}
			else
			{
				menuitem->item.listbox->sizexarea = menuitem->hotxsize;
			}
		}
		if (listelements)
		{
			changelistbox_selectednr(allmenus, nr, 0);
			changelistbox_fromto(allmenus, nr, 0);
			setmenuitem_DISABLED(allmenus, nr, FALSE);
		}
		else
		{
			changelistbox_selectednr(allmenus, nr, 0);
			setmenuitem_DISABLED(allmenus, nr, TRUE);
		}
	}
	else
		DEBUGMESSCR("leak detected!\n");
}
//==========================================
void setlistbox_lists(MENUSTR *allmenus, int nr, int selectednr, mylist *listbox,
	void(*func)(MENUSTR *allmenus, int menuitemnr, int listnr))
{
	MENUPOS *menuitem = &allmenus->menu[nr];
	if (menuitem->itemtype == ISLISTBOX)
	{
		menuitem->item.listbox->onselectitem_callback = func;
		if (listbox)
		{
			setlistbox_lists(allmenus, nr, selectednr, listbox);
		}
	}
	else
		DEBUGMESSCR("leak detected!\n");
}
//==========================================
void drawlistboxitem(MENUSTR *allmenus, int itemnr)
{
	mylist *listbox;
	char *mes;
	LISTBOXBAR *bar;
	int x1, y1, x2, y2;
	int i, jj, j, from, to, selitem, color;
	GRPFILE *dlggrp = allmenus->dlggrp;
	MENUPOS *menuitem = &allmenus->menu[itemnr];
	char *fonttable = allmenus->fonttable;
	int showtype = menuitem->typeofshow;
	int colors4 = menuitem->colors4;

	listbox = menuitem->item.listbox->flist;
	from = menuitem->item.listbox->from;
	to = menuitem->item.listbox->to;
	//draw items
	for (jj = 0, j = from;j <= to;jj++, j++)
	{
		if (listbox&&listbox->Count())
			mes = (char *)listbox->GetElemNr(j);
		else
			break;
		selitem = 0;
		if (menuitem_ISDISABLED(allmenus, itemnr) || showtype == ITEMSHOW_DISABLED)
			color = GETITEMCOLOR(colors4, ITEMSHOW_DISABLED);
		else
			if (j == menuitem->item.listbox->selectednr)
			{
				if (mes[0] == COMMANDSYMB)
				{
					if (mes[1] == ICONFORNOSELFILE)
					{
						mes[1] = ICONFORSELFILE;
					}
					else
						if (mes[1] == ICONFORCLOSENOSELDIR)
							mes[1] = ICONFORCLOSESELDIR;
				}
				color = GETITEMCOLOR(colors4, ITEMSHOW_CLICK);
				selitem = 1;
			}
			else
			{
				if (mes[0] == COMMANDSYMB)
				{
					if (mes[1] == ICONFORSELFILE)
						mes[1] = ICONFORNOSELFILE;
					else
						if (mes[1] == ICONFORCLOSESELDIR)
							mes[1] = ICONFORCLOSENOSELDIR;
				}
				color = GETITEMCOLOR(colors4, ITEMSHOW_NOFOCUS);
			}
		putmessage(menuitem->xtextpos,
			menuitem->ytextpos + jj * menuitem->item.listbox->pixelsbetweenlines,
			menuitem->fontnr, mes, color, fonttable, dlggrp);
		if (selitem&&menuitem->item.listbox->decorcolor)
		{
			x1 = menuitem->hotdeltax + 1;
			y1 = menuitem->ytextpos + jj * menuitem->item.listbox->pixelsbetweenlines + 1;
			x2 = menuitem->hotdeltax + menuitem->item.listbox->sizexarea - 2;
			y2 = y1 + menuitem->item.listbox->pixelsbetweenlines - 3;
			wrectangle(menuitem->item.listbox->decorcolor, x1, y1, x2, y2);
		}
	}
	//draw right scroll bar if any
	bar = menuitem->item.listbox->bar;
	if (bar)
	{
		bar = menuitem->item.listbox->bar;
		GRP_PutNoPacked(bar->xt_1, bar->yt_1, allmenus->dlggrp, LISTBOX_TOPBUTTON[bar->showtypet]);
		GRP_PutNoPacked(bar->xt_1, bar->yh_1, allmenus->dlggrp, LISTBOX_TOPBAR);
		y1 = bar->yh_1 + DLGGRP_SIZEY(LISTBOX_TOPBAR);
		for (i = 0;i < bar->elemtodraw;i++)
		{
			GRP_PutNoPacked(bar->xt_1, y1, allmenus->dlggrp, LISTBOX_MIDDLEBAR);
			y1 += DLGGRP_SIZEY(LISTBOX_MIDDLEBAR);
		}
		GRP_PutNoPacked(bar->xt_1, bar->yh_last, allmenus->dlggrp, LISTBOX_BOTTOMBAR);
		GRP_PutNoPacked(bar->xt_1, bar->yb_1, allmenus->dlggrp, LISTBOX_BOTTOMBUTTON[bar->showtypeb]);
		GRP_PutNoPacked(bar->xt_1, bar->buttonbary1, allmenus->dlggrp, LISTBOX_BUTTONBAR);
	}
}
//==========================================
int changelistbox_selectednr(MENUSTR *allmenus, int itemnr, int delta)
{
	int changes = 0, maxelem, prevsel;
	MENUPOS *menuitem = &allmenus->menu[itemnr];
	if (menuitem->itemtype == ISLISTBOX)
	{
		if (!menuitem->item.listbox->flist)
			return 0;
		prevsel = menuitem->item.listbox->selectednr;
		menuitem->item.listbox->selectednr += delta;
		if (delta <= 0)
		{
			if (menuitem->item.listbox->selectednr < 0)
				menuitem->item.listbox->selectednr = 0;
			if (menuitem->item.listbox->selectednr < menuitem->item.listbox->from)
			{
				changelistbox_fromto(allmenus, itemnr, -1);
			}
		}
		if (delta >= 0)
		{
			maxelem = menuitem->item.listbox->flist->Count();
			if (menuitem->item.listbox->selectednr >= maxelem)
				menuitem->item.listbox->selectednr = maxelem - 1;
			if (menuitem->item.listbox->selectednr > menuitem->item.listbox->to)
			{
				changelistbox_fromto(allmenus, itemnr, +1);
			}
		}
		if (prevsel != menuitem->item.listbox->selectednr || !delta)
		{
			changes = 1;
		}
		if (changes&&prevsel >= 0)
		{
			if (menuitem->item.listbox->onselectitem_callback != NULL)
				menuitem->item.listbox->onselectitem_callback(allmenus, itemnr, menuitem->item.listbox->selectednr);
		}
	}
	else
		DEBUGMESSCR("leak detected!\n");
	return(changes);
}
//==========================================
void changelistbox_fromto(MENUSTR *allmenus, int itemnr, int delta)
{
	int maxelem;
	MENUPOS *menuitem;
	menuitem = &allmenus->menu[itemnr];
	if (menuitem->itemtype == ISLISTBOX)
	{
		if (!menuitem->item.listbox->flist)
			return;
		maxelem = menuitem->item.listbox->flist->Count();
		menuitem->item.listbox->from += delta;
		if (delta <= 0)
		{
			if (menuitem->item.listbox->from < 0)
				menuitem->item.listbox->from = 0;
		}
		if (delta >= 0)
		{
			if (menuitem->item.listbox->from + menuitem->item.listbox->lines > maxelem)
			{
				menuitem->item.listbox->from = maxelem - menuitem->item.listbox->lines;
				if (menuitem->item.listbox->from < 0)
					menuitem->item.listbox->from = 0;
			}
		}
		menuitem->item.listbox->to = menuitem->item.listbox->from + menuitem->item.listbox->lines - 1;
		if (menuitem->item.listbox->to >= maxelem)
			menuitem->item.listbox->to = maxelem - 1;
		calclistboxbuttonbarpos(allmenus, itemnr);
	}
	else
		DEBUGMESSCR("leak detected!\n");
}
//==========================================
void dellistboxitem(MENUSTR *allmenus, int nr, int end, int typeofdel)//0-flushlist 1-dealloclist
{
	MENUPOS *menuitem = &allmenus->menu[nr];
	if (menuitem->item.listbox)
	{
		if (menuitem->item.listbox->flist)
		{
			switch (typeofdel)
			{
			case MENUITEM_LISTBOX_FLUSHLIST:
				menuitem->item.listbox->flist->FlushList();
				break;
			case MENUITEM_LISTBOX_DEALLOCLIST:
				menuitem->item.listbox->flist->DeallocList();
				break;
			}
			menuitem->item.listbox->flist = NULL;
		}
		if (menuitem->item.listbox->bar)
		{
			wfree(menuitem->item.listbox->bar);
			menuitem->item.listbox->bar = NULL;
		}
		if (end)
		{
			wfree(menuitem->item.listbox);
			menuitem->item.listbox = NULL;
		}
	}
}
//==========================================
void changelistbox_buttonbarposdelta(MENUSTR *allmenus, int nr, int mouseypos)
{
	MENUPOS *menuitem = &allmenus->menu[nr];
	LISTBOXBAR *bar;
	if (menuitem->itemtype == ISLISTBOX)
	{
		if (menuitem->item.listbox)
		{
			bar = menuitem->item.listbox->bar;
			if (bar)
			{
				if (highMouse->PosY < bar->buttonbary1)
				{
					changelistbox_buttonbarpos(allmenus, nr, bar->buttonbary1 - DLGGRP_SIZEY(LISTBOX_BUTTONBAR) * 3 / 2);
				}
				else
				{
					changelistbox_buttonbarpos(allmenus, nr, bar->buttonbary1 + DLGGRP_SIZEY(LISTBOX_BUTTONBAR) * 2);
				}
			}
		}
	}
	else
		DEBUGMESSCR("leak detected!\n");
}
//==========================================
void changelistbox_buttonbarpos(MENUSTR *allmenus, int nr, int mouseypos)
{
	int posy, maxelem, from;
	MENUPOS *menuitem = &allmenus->menu[nr];
	LISTBOXBAR *bar;
	if (menuitem->itemtype == ISLISTBOX)
	{
		if (menuitem->item.listbox)
		{
			bar = menuitem->item.listbox->bar;
			if (bar)
			{
				posy = mouseypos - bar->yh_1;
				maxelem = menuitem->item.listbox->flist->Count() - menuitem->item.listbox->lines;
				from = maxelem * (posy - DLGGRP_SIZEY(LISTBOX_BUTTONBAR) / 2) / (bar->buttonbarymax - bar->yh_1);
				if (from > maxelem)
					from = maxelem;
				if (from < 0)
					from = 0;
				menuitem->item.listbox->from = from;
				menuitem->item.listbox->to = from + menuitem->item.listbox->lines - 1;
				bar->buttonbary1 = posy + bar->yh_1 - DLGGRP_SIZEY(LISTBOX_BUTTONBAR) / 2;
				if (bar->buttonbary1 < bar->yh_1)
					bar->buttonbary1 = bar->yh_1;
				if (bar->buttonbary1 > bar->buttonbarymax)
					bar->buttonbary1 = bar->buttonbarymax;
				bar->buttonbary2 = bar->buttonbary1 + DLGGRP_SIZEY(LISTBOX_BUTTONBAR) - 1;
			}
		}
	}
	else
		DEBUGMESSCR("leak detected!\n");
}
//==========================================
void changemenuitemtype(MENUSTR *allmenus, int nr, int newitemtype, int fontnr, int colors4)
{
	MENUPOS *menuitem = &allmenus->menu[nr];
	int previtemtype = menuitem->itemtype;
	delmenuitem(allmenus, nr);	//delete only added information
	menuitem->itemtype = newitemtype;
	if (previtemtype == ISLABELLEFT || previtemtype == ISLABELCENTER || previtemtype == ISLABELRIGHT)
		menuitem->hotxsize--;
	switch (newitemtype)
	{
	case ISEXPANDBOX:
		addexpanditem(allmenus, nr, 0, fontnr, allmenus->menu[nr].hotysize, colors4);
		allmenus->menu[nr].dialogbin_flags |= DIALOGBIN_FLAGS_KEYMOUSERESPONDEVENTS;
		break;
	case ISIMAGE:
		//				addimageitem(allmenus,nr);
		break;
	}
}
/*			case ISBUTTON:
				menuitem->item.button=(struct BUTTON *)wmalloc(sizeof(struct BUTTON));
				memset(menuitem->item.button,0,sizeof(struct BUTTON));
				break;
			case ISLISTBOX:
				menuitem->item.listbox=(struct LISTBOX *)wmalloc(sizeof(struct LISTBOX));
				memset(menuitem->item.listbox,0,sizeof(struct LISTBOX));
				break;
			case ISEDITBOX:
				menuitem->item.editbox=(struct EDITBOX *)wmalloc(sizeof(struct EDITBOX));
				memset(menuitem->item.editbox,0,sizeof(struct EDITBOX));
				break;
			case ISTEXTITEM:
				menuitem->item.textitem=(struct TEXTLINE *)wmalloc(sizeof(struct TEXTLINE));
				memset(menuitem->item.textitem,0,sizeof(struct TEXTLINE));
				break;
			case ISEXPANDBOX:
				menuitem->item.expandbox=(struct EXPANDBOX *)wmalloc(sizeof(struct EXPANDBOX));
				memset(menuitem->item.expandbox,0,sizeof(struct EXPANDBOX));
				break;
			case ISCHECKBOX:
				menuitem->item.checkbox=(struct CHECKBOX *)wmalloc(sizeof(struct CHECKBOX));
				memset(menuitem->item.checkbox,0,sizeof(struct CHECKBOX));
				break;
			case ISRADIOBUTTON:
				menuitem->item.radiobutton=(RADIOBUTTON *)wmalloc(sizeof(struct RADIOBUTTON));
				memset(menuitem->item.radiobutton,0,sizeof(struct RADIOBUTTON));
				break;
			case ISHORIZBUTTON:
				menuitem->item.horizbutton=(struct HORIZBUTTON *)wmalloc(sizeof(struct HORIZBUTTON));
				memset(menuitem->item.horizbutton,0,sizeof(struct HORIZBUTTON));
				break;
			case ISSMKVIDEO:
				menuitem->item.smkvideo=(struct SMKVIDEO *)wmalloc(sizeof(struct SMKVIDEO));
				memset(menuitem->item.smkvideo,0,sizeof(struct SMKVIDEO));
				break;
			case ISIMAGE:
				menuitem->item.image=(struct IMAGE *)wmalloc(sizeof(struct IMAGE));
				memset(menuitem->item.image,0,sizeof(struct IMAGE));
				break;
*/

//==========================================
MENUPARAMS::MENUPARAMS(int(*func)(MENUSTR *allmenus, int menuitem), int param1)
{
	onselectmenuitem = func;
	params_int1 = param1;
}
//==========================================
MENUPARAMS::MENUPARAMS(int(*func)(MENUSTR *allmenus, int menuitem), char *param1)
{
	onselectmenuitem = func;
	params_pchar1 = param1;
}
//==========================================
int MENUDRAW::IfCanShowMenu(void)
{
	return(activatedmenu != NULL);
}
//==========================================
int MENUDRAW::ShowMenu(void)
{
	return((*activatedmenu)(this, menuparams));
}
//==========================================
mylist ALLMENUS;
//==========================================
void MENUDRAW::Init(void)
{
	activatedmenu = NULL;
	menuparams = NULL;
	nextactivatedmenu = NULL;
	nextmenuparams = NULL;
	menutodraw = NULL;
}
//==========================================
void MENUDRAW::prepareforshowmenu(int(*menufunc)(MENUDRAW *menudraw, MENUPARAMS *menuparams), MENUPARAMS *params)
{
	MENUDRAW *prevdraw;
	mainController.KeyActive = 0;
	if (activatedmenu)
	{
		prevdraw = (MENUDRAW *)wmalloc(sizeof(MENUDRAW));
		memset(prevdraw, 0, sizeof(MENUDRAW));
		prevdraw->activatedmenu = activatedmenu;
		prevdraw->menuparams = menuparams;
		prevdraw->menutodraw = menutodraw;
		ALLMENUS.AddList(prevdraw);

		nextactivatedmenu = menufunc;
		nextmenuparams = params;
	}
	else
	{
		activatedmenu = menufunc;
		menuparams = params;
		menutodraw = NULL;
	}
}
//==========================================
void MENUDRAW::CloseMultiplesMenus(int nrofrecursivemenus)
{
	int i;
	for (i = 0;i < nrofrecursivemenus;i++)
	{
		MENUSTR *allmenus = menutodraw;
		if (allmenus)
		{
			if (allmenus->mainmenuflags&DIALOGBIN_FLAGS_ENTERSEMAPHORE)
			{
				allmenus->mainmenuflags &= ~DIALOGBIN_FLAGS_ENTERSEMAPHORE;
				EndDrawMenu();
				unregmenucallbacks();
				if (--MENUACTIVE == 0)
					if (PAUSEGAME)
						if (!NETWORKGAME)
							PAUSEGAME = 0;
				EndShowMenu();
			}
		}
	}
	mainController.KeyActive = 0;
}
//==========================================
void MENUDRAW::EndDrawMenu(void)
{
	if (menutodraw)
	{
		if (menutodraw->saveunder)
		{
			loadundermenu(menutodraw, menutodraw->saveunder);
		}
		menutodraw->saveunder = NULL;
		if (nextactivatedmenu)
		{
			activatedmenu = nextactivatedmenu;
			menuparams = nextmenuparams;
			menutodraw = NULL;
			nextactivatedmenu = NULL;
			nextmenuparams = NULL;
		}
	}
}
//==========================================
void MENUDRAW::EndShowMenu(void)
{
	MENUDRAW *prevdrawmenu;
	int lastelem;
	if (activatedmenu)
	{
		if (menutodraw)
			UnloadDialogBin(menutodraw);
		lastelem = ALLMENUS.GetLastElemNr();
		if (lastelem == -1)
		{
			menutodraw = NULL;
			activatedmenu = NULL;
			if (menuparams)
				delete menuparams;
			menuparams = NULL;
		}
		else
		{
			prevdrawmenu = (MENUDRAW *)ALLMENUS.GetElemNr(lastelem);
			menutodraw = prevdrawmenu->menutodraw;
			activatedmenu = prevdrawmenu->activatedmenu;
			if (menuparams)
				delete menuparams;
			menuparams = prevdrawmenu->menuparams;
			ALLMENUS.DelList(lastelem);
			wfree(prevdrawmenu);
		}
		mymousemoveevent(highMouse->PosX, highMouse->PosY);
	}
}
//==========================================
int drawmenu_ONETICK(MENUSTR *allmenus)
{
	int exitcallback, redrawbar, expitem;
	MENUSTR *showlist;

	allmenus->saveunder = saveundermenu(allmenus);
	ShowMenuEdge(allmenus);

	if (!(allmenus->mainmenuflags&DIALOGBIN_FLAGS_ENTERSEMAPHORE))
	{
		mainController.KeysBuffer->Flush();
		MENUACTIVE++;
		allmenus->mainmenuflags |= DIALOGBIN_FLAGS_ENTERSEMAPHORE;
		allmenus->vars.selectedmenu = NOSELECTMENUBAR;
		allmenus->vars.downmenu = NOSELECTMENUBAR;
		allmenus->vars.prevmenuonbar = menuonbar;
		menuonbar = NOSELECTMENUBAR;
		allmenus->vars.prevactiveitem = -1000;
		allmenus->vars.BarChanges.bar = ITEMNOONEACTIVE;
		allmenus->vars.BarChanges.color = 0;
		regmenu(allmenus->x, allmenus->y, allmenus, &getmouseonmenubar);//set menu hotcoord for mouse
		allmenus->vars.expanditemnr = -1;			//save id of selected expanditem for future release
		mymousemoveevent(highMouse->PosX, highMouse->PosY);
	}
	checkanddrawmenu(allmenus, allmenus->vars.BarChanges.bar, ITEM_NOSAVELOADUNDER);
	do {
		exitcallback = RunCallBackFuncs(allmenus);
		if (exitcallback)
		{
			allmenus->vars.selectedmenu = exitcallback;
			break;
		}
		if (allmenus->vars.expanditemnr < 0)
			getmouseonitem(&allmenus->vars.activeitemchange, &allmenus->vars.activeitem);
		allmenus->vars.selectedmenu = menukeys(allmenus, &redrawbar, NULL);//&needredraw);
		if (editboxaction(allmenus) == 2)
		{
			allmenus->vars.selectedmenu = allmenus->DefaultTextBox;
		}
		if (allmenus->vars.selectedmenu >= 0)
		{
			//if some key unpressed
			if (allmenus->vars.selectedmenu == CANCELFROMMENU)
			{
				if (allmenus->selectmenusnd)
					Play_sfxdata_id(NULL, allmenus->selectmenusnd, -1, 0);
				break;
			}
			switch (allmenus->menu[allmenus->vars.selectedmenu].itemtype)
			{
			case ISBUTTON:
			case ISSMKVIDEO:
				if (allmenus->selectmenusnd)
					Play_sfxdata_id(NULL, allmenus->selectmenusnd, -1, 0);
				break;
			case ISCHECKBOX:
				changecheckboxstate(allmenus, allmenus->vars.selectedmenu);
				break;
			case ISRADIOBUTTON:
				setradiobuttonstate(allmenus, allmenus->vars.selectedmenu);
				break;
			}
			break;
		}
		if (redrawbar >= 0)
		{
			//pressed specific key
			allmenus->vars.activeitemchange = 1;
			allmenus->vars.BarChanges.bar = redrawbar;
			allmenus->vars.BarChanges.color = ITEMSHOW_CLICK;
		}
		else  //if from mouse action
		{
			if (allmenus->vars.activeitemchange) //move moved on other menu
			{
				allmenus->vars.BarChanges.bar = allmenus->vars.activeitem;
				allmenus->vars.BarChanges.color = ITEMSHOW_FOCUS;
				if (allmenus->vars.activeitem >= 0)
				{
					if (allmenus->menu[allmenus->vars.activeitem].itemtype == ISBUTTON ||
						(allmenus->menu[allmenus->vars.activeitem].itemtype == ISSMKVIDEO &&
							allmenus->menu[allmenus->vars.activeitem].item.smkvideo->smkbutton))
					{
						if (allmenus->mouseoversnd)
						{
							//prevent sound yhem mounse move on the same item
							if (allmenus->vars.prevactiveitem != allmenus->vars.activeitem)
							{
								allmenus->vars.prevactiveitem = allmenus->vars.activeitem;
								Play_sfxdata_id(NULL, allmenus->mouseoversnd, -1, 0);
							}
						}
					}
				}
				else
					allmenus->vars.prevactiveitem = -1000;
			}
			if (highMouse->GetButtonStatus() & WMLEFTKEY)
			{
				if (allmenus->vars.expanditemnr >= 0)
				{
					actionmenuonpressmouse(allmenus, allmenus->vars.expanditemnr, &allmenus->vars.selectedmenu);
				}
				else
				{
					if (allmenus->vars.activeitem >= 0)
					{
						allmenus->vars.activeitemchange = 1;
						if (allmenus->vars.downmenu == NOSELECTMENUBAR)
							allmenus->vars.downmenu = allmenus->vars.activeitem;
						else
						{
							allmenus->vars.BarChanges.bar = allmenus->vars.activeitem;
							allmenus->vars.BarChanges.color = ITEMSHOW_CLICK;
						}
					}
					if (allmenus->vars.downmenu >= 0)
					{
						allmenus->vars.selectedmenu = actiondblclick(allmenus, allmenus->vars.downmenu);
						if (allmenus->vars.activeitem == allmenus->vars.downmenu)
						{
							actionmenuonpressmouse(allmenus, allmenus->vars.downmenu, &allmenus->vars.selectedmenu);
						}
					}
				}
			}
			else
			{
				if (allmenus->vars.expanditemnr >= 0)
				{
					expitem = allmenus->vars.expanditemnr;
					showlist = allmenus->menu[expitem].item.expandbox->showlist;
					showlistmenu_LASTTICK(showlist);
					allmenus->vars.selectedmenu = expitem;
					if (showlist->vars.BarChanges.bar >= 0)
					{
						allmenus->menu[expitem].item.expandbox->selectednr = showlist->vars.BarChanges.bar;
					}
					removeallmenuinfo(showlist);
					allmenus->menu[expitem].item.expandbox->showlist = NULL;
					allmenus->vars.expanditemnr = -1;
					mymousemoveevent(highMouse->PosX, highMouse->PosY);
				}
				else
					if (allmenus->vars.downmenu != NOSELECTMENUBAR)
					{
						if (allmenus->vars.downmenu == allmenus->vars.activeitem)
						{
							switch (allmenus->menu[allmenus->vars.activeitem].itemtype)
							{
							case ISBUTTON:
							case ISSMKVIDEO:
								allmenus->vars.selectedmenu = allmenus->vars.activeitem;
								if (allmenus->selectmenusnd)
									Play_sfxdata_id(NULL, allmenus->selectmenusnd, -1, 0);
								break;
							case ISCHECKBOX:
								allmenus->vars.selectedmenu = allmenus->vars.activeitem;
								changecheckboxstate(allmenus, allmenus->vars.activeitem);
								break;
							case ISRADIOBUTTON:
								allmenus->vars.selectedmenu = allmenus->vars.activeitem;
								setradiobuttonstate(allmenus, allmenus->vars.activeitem);
								break;
							case ISLISTBOX:
								if (allmenus->menu[allmenus->vars.downmenu].item.listbox)
									if (allmenus->menu[allmenus->vars.downmenu].item.listbox->bar)
									{
										allmenus->menu[allmenus->vars.downmenu].item.listbox->bar->showtypet = ITEMSHOW_NOFOCUS;
										allmenus->menu[allmenus->vars.downmenu].item.listbox->bar->showtypeb = ITEMSHOW_NOFOCUS;
										allmenus->menu[allmenus->vars.downmenu].item.listbox->bar->showbutton = ITEMSHOW_NOFOCUS;
									}
								break;
							}
							allmenus->vars.downmenu = NOSELECTMENUBAR;
						}
						else
						{
							allmenus->vars.downmenu = NOSELECTMENUBAR;
						}
					}
			}
		}
	} while (0);
	return(allmenus->vars.selectedmenu);
}
//==========================================
int drawmenu_LASTTICK(MENUDRAW *menudraw)
{
	int retmenu = 0;
	MENUSTR *allmenus = menudraw->menutodraw;
	if (allmenus)
	{
		if (allmenus->mainmenuflags&DIALOGBIN_FLAGS_ENTERSEMAPHORE)
		{
			allmenus->mainmenuflags &= ~DIALOGBIN_FLAGS_ENTERSEMAPHORE;
			menudraw->EndDrawMenu();
			unregmenucallbacks();
			MENUACTIVE--;
			menudraw->EndShowMenu();
			if (menudraw->menutodraw)
			{
				//if we have parent of this menu, lets show parent menu
				retmenu = showedmenu.ShowMenu();
			}
			mainController.KeyActive = 0;
		}
	}
	return(retmenu);
}
//==========================================
int showlistmenu_ONETICK(MENUSTR *allmenus)
{
	int exitcallback;
	if (!(allmenus->mainmenuflags&DIALOGBIN_FLAGS_ENTERSEMAPHORE))
	{
		MENUACTIVE++;
		allmenus->mainmenuflags |= DIALOGBIN_FLAGS_ENTERSEMAPHORE;
		allmenus->vars.selectedmenu = NOSELECTMENUBAR;
		allmenus->vars.downmenu = NOSELECTMENUBAR;
		allmenus->vars.prevmenuonbar = menuonbar;
		menuonbar = NOSELECTMENUBAR;
		allmenus->vars.prevactiveitem = -1000;
		allmenus->vars.BarChanges.bar = ITEMNOONEACTIVE;
		allmenus->vars.BarChanges.color = 0;
		regmenu(allmenus->x, allmenus->y, allmenus, &getmouseonmenubar);//set menu hotcoord for mouse
	}
	checkanddrawmenu(allmenus, allmenus->vars.BarChanges.bar, ITEM_NOSAVELOADUNDER);
	do {
		getmouseonitem(&allmenus->vars.activeitemchange, &allmenus->vars.activeitem);
		exitcallback = RunCallBackFuncs(allmenus->prevmenu);
		if (exitcallback)
		{
			allmenus->prevmenu->vars.selectedmenu = exitcallback;
			allmenus->vars.selectedmenu = NOSELECTMENUBAR;
			break;
		}
		if (allmenus->vars.activeitemchange) //move moved on other menu
		{
			allmenus->vars.BarChanges.bar = allmenus->vars.activeitem;
			allmenus->vars.BarChanges.color = ITEMSHOW_FOCUS;
		}
		if (highMouse->GetButtonStatus() & WMLEFTKEY)
		{
			if (allmenus->vars.activeitem >= 0)
			{
				allmenus->vars.activeitemchange = 1;
				if (allmenus->vars.downmenu == NOSELECTMENUBAR)
					allmenus->vars.downmenu = allmenus->vars.activeitem;
				else
				{
					allmenus->vars.BarChanges.bar = allmenus->vars.activeitem;
					allmenus->vars.BarChanges.color = ITEMSHOW_CLICK;
				}
			}
		}
	} while (0);
	return(allmenus->vars.selectedmenu);
}
//==========================================
void showlistmenu_LASTTICK(MENUSTR *listmenus)
{
	if (listmenus->mainmenuflags&DIALOGBIN_FLAGS_ENTERSEMAPHORE)
	{
		listmenus->mainmenuflags &= ~DIALOGBIN_FLAGS_ENTERSEMAPHORE;
		if (listmenus->saveunder)
		{
			loadundermenu(listmenus, listmenus->saveunder);
			listmenus->saveunder = NULL;
		}
		unregmenucallbacks();
		MENUACTIVE--;
	}
}
//==========================================
