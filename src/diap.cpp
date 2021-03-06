#include <stdlib.h>
#include <grplib/gr8.h>
#include <grplib/usegrp.h>
#include <math.h>

#include "action.h"
#include "lists.h"

#include "auxil.h"
#include "mouse.h"
#include "gener.h"
#include "man.h"
#include "vars.h"
#include "putobj.h"
#include "mage.h"
#include "key.h"
#include "objs.h"
#include "load.h"
#include "maps.h"
#include "gr.h"
#include "gener.h"
#include "scripts.h"
#include "loadlo.h"
#include "audio.h"
#include "starmap.h"
#include "creep.h"
#include "unitaction.h"
#include "pylon.h"
#include "upgrade.h"
#include "ObjChecks.h"
#include "rand.h"
#include "construct.h"
#include "weapons.h"
#include "overlay.h"
#include "market.h"
#include "Controller.h"
#include "diap.h"

//=============================================
#define POSX(x) ((x+SIZESPRLANSHX-1)/SIZESPRLANSHX)
#define POSY(y) ((y+SIZESPRLANSHY-1)/SIZESPRLANSHY)
//=============================================
int curentproperties;
//=============================================
#define MINBUTTONDOWN  2
#define MINBUTTONSLEEP 2
#define BUTTONMESSAGEFONT IDFONT8
#define BORDERSIZE 2

#define NEED_MINERALS	0
#define NEED_GAS		1
#define NEED_MANA		2
#define NEED_PSI		3
#define NEED_MAX		4

void desenproperties(int *localprop, char *selectableicons)
{
	static int oldmouseonicon = -1, plusfactor, keyupselectedicon = -1;
	int i, j, k, mbuttonpress, grpplayernr;
	int need[NEED_MAX];//mineral,gas,mana,psi
	int iconmessagecolor, needmana, err;
	int simbrace, race;
	MAGEP *mp;
	int upg, maxupg;
	int mouseonicon, selectedicon, retstatus;
	struct XY uu;
	char iconstr[150];
	char addstr[15];
	struct OBJ *obj, *obj2 = NULL;
	OBJ *o;
	for (i = 0;i < 3;i++)
		for (j = 0;j < 3;j++)
			if (localprop[i * 3 + j])
			{
				int sizex = znakgrp->Picture[mageprop[localprop[i * 3 + j]].icon_id].PixelPerLine;
				int sizey = znakgrp->Picture[mageprop[localprop[i * 3 + j]].icon_id].LinesPerPicture;
				uu.x1 = xicon[j] + XADDICONS + DELTASCREENX;
				uu.y1 = yicon[i] + YADDICONS - 2;
				grpplayernr = selectableicons[i * 3 + j];
				if ((oldmouseonicon != -1 && oldmouseonicon == i * 3 + j) ||
					(keyupselectedicon == i * 3 + j))
				{
					GRP_PutGrp(uu.x1 + 1, uu.y1 + 1, grpicons, NORMAL, 255, grpplayernr, NULL, 1);
					//					putgrp(uu.x1+1,uu.y1+1,grpicons,1,grpplayernr);
					uu.x1 += 1;
					uu.y1 += 1;
				}
				else
					GRP_PutGrp(uu.x1 + 1, uu.y1 + 1, grpicons, NORMAL, 255, grpplayernr, NULL, 0);
				//					putgrp(uu.x1+1,uu.y1+1,grpicons,0,grpplayernr);
				GRP_PutGrp(uu.x1 + (38 - sizex) / 2, uu.y1 + (36 - sizey) / 2,
					znakgrp, NORMAL, 255, grpplayernr, NULL,
					mageprop[localprop[i * 3 + j]].icon_id);

			}//if local
	if (!highMouse->MouseOnSelectionMode)
	{
		mouseonicon = -1;
		selectedicon = -1;
		mouseonconstrunit = -1;
		mouseontranspunit = -1;
		selecteddeconstrunit = -1;
		selectedunloadunit = -1;
		//		keyupselectedicon=-1;

		if (existinbuildconstr)
			for (i = 0;i < MAXCONSTRUCTINBUILD;i++)
			{
				uu.x1 = xy[0][i] + XUNITCONSTR;
				uu.y1 = xy[1][i] + YUNITCONSTR;
				uu.x2 = uu.x1 + 36;
				uu.y2 = uu.y1 + 36;
				if (highMouse->CheckForBorder(uu.x1, uu.y1, uu.x2, uu.y2))
					mouseonconstrunit = i;
			}
		if (transportplaceobj&&transportplaceobj->playernr == NUMBGAMER)
			if (highMouse->MouseHotPos >= MOUSEONTRANSP && highMouse->MouseHotPos < MOUSEONICONSMIN)
				if (transportplace[highMouse->MouseHotPos - MOUSEONTRANSP] != SC_NOUNITNR)
				{
					mouseontranspunit = highMouse->MouseHotPos - MOUSEONTRANSP;
				}
		if (!MENUACTIVE)		//prevent activate hotkey if some menu is active
		{
			for (i = 0;i < 3;i++)
				for (j = 0;j < 3;j++)
					if (localprop[i * 3 + j])
					{
						if (mainController.KeyActive && mageprop[localprop[i * 3 + j]].keyfororder != 255)  //check for hotkey
						{
							if (mageprop[localprop[i * 3 + j]].keyfororder == KEYPRESSEDTABLE(rus, mainController.KeyActive) - 1
								&&
								selectableicons[i * 3 + j] != FORGRAY)
							{
								keyupselectedicon = i * 3 + j;
								goto selectedicon;
							}
						}
						else
							if (keyupselectedicon != -1)
							{
								selectedicon = keyupselectedicon;
								keyupselectedicon = -1;
							}
					}
		}
selectedicon:
		if (mouseonicon == -1 && 
			highMouse->MouseHotPos >= MOUSEONICONSMIN && 
			highMouse->MouseHotPos <= MOUSEONICONSMAX)
		{
			mouseonicon = highMouse->MouseHotPos - MOUSEONICONSMIN;
			if (localprop[mouseonicon])
			{
				i = mouseonicon / 3;
				j = mouseonicon % 3;
				mp = &mageprop[localprop[i * 3 + j]];
				if (selectableicons[i * 3 + j] == FORGRAY)
				{
					iconmessagecolor = GGREYCOLORFONT;
					if (mp->type_id == ORDERS_UPGRADE)
					{
						upg = GetUpgradeTree(&map, NUMBGAMER, mp->obj_id);
						strcpy(iconstr, alldattbl.stattxt_tbl->get_TBL_STR(mp->stattxt_id_disable[upg]));
					}
					else
					{
						strcpy(iconstr, alldattbl.stattxt_tbl->get_TBL_STR(mp->stattxt_id_disable[0]));
					}
				}
				else
				{
					iconmessagecolor = GBLUECOLORFONT;
					switch (mp->type_id)
					{
					case ORDERS_NONE:
						iconstr[0] = 0;
						need[NEED_MINERALS] = 0;
						need[NEED_GAS] = 0;
						need[NEED_MANA] = 0;
						need[NEED_PSI] = 0;
						break;
					case ORDERS_MODEMOVE:
						strcpy(iconstr, alldattbl.stattxt_tbl->get_TBL_STR(mp->stattxt_id_enable) + 2);
						need[NEED_MINERALS] = 0;
						need[NEED_GAS] = 0;
						if (mp->obj_id != -1)
							need[NEED_MANA] = GetTechEnergyCost(mp->obj_id);
						else
							need[NEED_MANA] = 0;
						need[NEED_PSI] = 0;
						break;
					case ORDERS_UNIT:
						strcpy(iconstr, alldattbl.stattxt_tbl->get_TBL_STR(mp->stattxt_id_enable) + 2);
						GetCostUnit((SCUNIT) mp->obj_id, &need[NEED_MINERALS], &need[NEED_GAS]);
						need[NEED_MANA] = 0;
						if (mp->techid < MAX_TECHDATA_ELEM)
							need[NEED_MANA] = GetTechEnergyCost(mp->techid);
						//							if (mp->orderdatid<MAX_ORDERS_ELEM)
						//							{
						//								techdata_id=alldattbl.orders_dat->techdata_id[mp->orderdatid];
						//								if (techdata_id<MAX_TECHDATA_ELEM)
						//									need[NEED_MANA]=GetTechEnergyCost(techdata_id);
						//							}
						if (GetInEgg((SCUNIT) mp->obj_id))
							need[NEED_PSI] = GetSupplyUnit((SCUNIT) mp->obj_id);
						else
							need[NEED_PSI] = GetSupplyUnit((SCUNIT) mp->obj_id) / 2;
						break;
					case ORDERS_UPGRADE:
						upg = GetUpgradeTree(&map, NUMBGAMER, mp->obj_id);
						maxupg = GetMaxUpgradeTree(&map, NUMBGAMER, mp->obj_id);
						strcpy(iconstr, alldattbl.stattxt_tbl->get_TBL_STR(mp->stattxt_id_enable) + 2);
						if (upg < maxupg&&alldattbl.upgrades_dat->MaxRepeats[mp->obj_id] > 1)
						{
							strcat(iconstr, "\n");
							strcat(iconstr, alldattbl.stattxt_tbl->get_TBL_STR(STATTXT_TBL_NEXTLEVELTEXT));
							int len = strlen(iconstr);
							iconstr[len] = '1' + upg;
							iconstr[len + 1] = 0x0;//END STRING
						}
						GetCostUpgr(mp->obj_id, NUMBGAMER, &need[NEED_MINERALS], &need[NEED_GAS]);
						need[NEED_MANA] = 0;
						need[NEED_PSI] = 0;
						break;
					case ORDERS_TECHNOLOGY:
						strcpy(iconstr, alldattbl.stattxt_tbl->get_TBL_STR(mp->stattxt_id_enable) + 2);
						GetCostTech(mp->obj_id, &need[NEED_MINERALS], &need[NEED_GAS]);
						need[NEED_MANA] = 0;
						need[NEED_PSI] = 0;
						break;
					}
					int messagecr = 0;
					if (mp->atr&ORDER_IGNOREREQ)
					{
						need[NEED_MINERALS] = 0;
						need[NEED_GAS] = 0;
						need[NEED_MANA] = 0;
						need[NEED_PSI] = 0;
					}
					for (k = 0;k < 4;k++)
					{
						addstr[0] = 0;
						if (need[k] > 0)
						{
							if (!messagecr)
							{
								messagecr = 1;
								strcat(iconstr, "\n");
							}
							if (k == 3)
							{
								simbrace = mp->race;
								sprintf(addstr, "%c%c%d ", COMMANDSYMB, PSISYMBZ + simbrace, need[k]);
							}
							else
							{
								sprintf(addstr, "%c%c%d ", COMMANDSYMB, MINERALSYMB + k, need[k]);
							}
							strcat(iconstr, addstr);
						}
					}
				}
				//show 3x3 action(move,atack,etc) icons text
				putboxmessage(FONTCOLOR(tfontgamp, GBLUECOLORFONT, 2),
					FONTCOLOR(tfontgamp, GBLUECOLORFONT, 4),
					xicon[j] + XADDICONS + DELTASCREENX, yicon[i] + YADDICONS,
					BORDERSIZE, BUTTONMESSAGEFONT,
					iconstr, iconmessagecolor, tfontgamp, gamedlggrp);
				if (selectableicons[i * 3 + j] == FORGRAY)
					mouseonicon = -1;
			}
		}
		mbuttonpress = highMouse->GetButtonStatus() & WMLEFTKEY;
		if (mbuttonpress)
		{
			if (mouseonicon != -1)
			{
				if (localprop[mouseonicon])
					oldmouseonicon = mouseonicon;
			}
			if (mouseonconstrunit != -1)
			{
				oldmouseonconstrunit = mouseonconstrunit;
			}
			if (mouseontranspunit != -1)
			{
				oldmouseontranspunit = mouseontranspunit;
			}
		}
		else
		{
			if (mouseonicon == oldmouseonicon && oldmouseonicon != -1)
				selectedicon = oldmouseonicon;
			oldmouseonicon = -1;
			if (mouseonconstrunit == oldmouseonconstrunit &&
				oldmouseonconstrunit != -1)
				selecteddeconstrunit = oldmouseonconstrunit;
			oldmouseonconstrunit = -1;
			if (mouseontranspunit == oldmouseontranspunit &&
				oldmouseontranspunit != -1)
				selectedunloadunit = oldmouseontranspunit;
			oldmouseontranspunit = -1;
		}
		if (selectedicon != -1)
		{
			if (!SelectedUnits.Count())
			{
				highMouse->WaitToPressLeftButton = 0;
			}
			else
			{
				o = (OBJ *)SelectedUnits.GetElem(0, NULL);
				race = GetUnitRace(o->SC_Unit);
				plusfactor = 0;
				i = selectedicon / 3;
				j = selectedicon % 3;
				oldselectbuton = localprop[i * 3 + j];
				mp = &mageprop[oldselectbuton];
				if (mp->depend.maxdepend == 1 && mp->type_id == ORDERS_UNIT)
					switch (mp->depend.type_id[0])
					{
					case ORDERS_TECHNOLOGY:
						if (IsSpellCaster(o->SC_Unit))//if spellcaster check for mana too
						{
							if (!(mp->atr&ORDER_IGNOREREQ))
							{
								needmana = GetTechEnergyCost(mp->depend.obj_id[0]);
								err = CheckForMana(o, needmana);
								if (err == CHECKRES_MANA)
								{
									playinfoadvisorsound(NUMBGAMER, race, ADVENERGY, PLAYADVISOR_TEXTANDSOUND);
									return;
								}
							}
						}
						break;
					}
				if (mp->atr&ORDER_IGNOREREQ)
					retstatus = CHECKRES_OK;
				else
					retstatus = CheckForResource_typeid(o, NUMBGAMER, mp->type_id, mp->obj_id);
				if (retstatus != CHECKRES_OK)
				{
					playadvisorerr(NUMBGAMER, race, retstatus);
					return;
				}
				if (mp->atr & ORDER_RIGHTWAY)
				{
					if (oldselectbuton == MODECANCEL_BB_AB)
					{
						SelectedUnits.EnumListInit();
						while ((o = (OBJ *)SelectedUnits.GetNextListElem(NULL)))
						{
							if (IsWorkerUnit(o->SC_Unit))
								ChangeTypeOfProp(o, PROPNORMAL1);
						}
						groupmove = 0;
						highMouse->WaitToPressLeftButton = 0;
					}
					else
					{
						if (!probeconstruct(oldselectbuton))
						{
							if (localprop[i * 3 + j] != MODEWARPARCHON &&
								localprop[i * 3 + j] != MODEWARPDARCHON)
							{
								SelectedUnits.EnumListInit();
								while ((o = (OBJ *)SelectedUnits.GetNextListElem(NULL)))
								{
									//makemove(fordeselect[k],NULL,0,0,oldselectbutton,NUMBGAMER,SHOWERRORTEXT);
									DoOrder(NUMBGAMER, o, 0, 0,
										mp->type_id, mp->obj_id, mp->icon_id, mp->stattxt_id_enable,
										oldselectbuton, SHOWERRORTEXT);
								}
							}
							else
							{
								SelectedUnits.EnumListInit();
								while ((obj = (OBJ *)SelectedUnits.GetNextListElem(NULL)))
								{
									if (!obj2)
									{
										obj2 = obj;
										continue;
									}
									else
									{
										makemove(obj, obj2, 0, 0, localprop[i * 3 + j], obj->playernr, NOSHOWERROR);
										makemove(obj2, obj, 0, 0, localprop[i * 3 + j], obj2->playernr, NOSHOWERROR);
										//moveobj(obj,obj2,localprop[i*3+j],NOSHOWERROR);
										//moveobj(obj2,obj,localprop[i*3+j],NOSHOWERROR);
										obj2 = NULL;
									}
								}//for k
							}//else
						}//if probe construct
					}
				}
				else
				{
					cbuild = (OBJ *)SelectedUnits.GetElem(0, NULL);
					constrbuild(oldselectbuton);

					highMouse->MouseModeMove = oldselectbuton;
					groupmove = 0;
					highMouse->WaitToPressLeftButton = 2;
				}
			}
		}
		if (selecteddeconstrunit != -1)
		{
			OBJ *a;
			SelectedUnits.EnumListInit();
			while ((a = (OBJ *)SelectedUnits.GetNextListElem(NULL)))
			{
				if (IsOBJUnderConstruct(a) && selecteddeconstrunit == 0)
				{
					makemove(a, NULL, 0, 0, MODEESCAPE, NUMBGAMER, SHOWERRORTEXT);
					break;
				}
				if (selecteddeconstrunit < GetConstructNR(a))
				{
					makemove(a, NULL, 0, 0, MODEESCAPE1 + selecteddeconstrunit, NUMBGAMER, SHOWERRORTEXT);
					break;
				}
			}
		}
		if (selectedunloadunit != -1)
		{
			//x is nr of unloaded unit, y is 0
			makemove(transportplaceobj, NULL, selectedunloadunit, 0, MODEUNLOADUNITNR, transportplaceobj->playernr, NOSHOWERROR | XYNOTCOORDS);
		}
	}//if !patrate
}
//=============================================
static int CreateUnitProperties(struct OBJ *a, struct OBJstruct *b, int *prop, int playernr)
{
	int i, unitnr, underc, upgradenr, maxupgradenr, constraddon;
	for (i = 0;i < MAXUNITPROPERTIES;i++)
	{
		prop[i] = MODENON;
	}
	if (a->prop & VARNOTSEEPROP)
		return -1;//exit if exist any with no desen properties like interceptors
	constraddon = IfConstructAddon(a);
	underc = GetConstructNR(a);
	if (!constraddon)
	{
		if (underc)
			ChangeTypeOfProp(a, b, PROPDEUPGRADE);
		else
			if ((IsProducesUnits(a->SC_Unit) && !IsOnSkyOBJ(a) && !IsOBJUnderConstruct(a)) ||
				a->SC_Unit == SC_CARRIEROBJ || a->SC_Unit == SC_REAVEROBJ ||
				a->SC_Unit == SC_HERO_GANTRITHOROBJ || a->SC_Unit == SC_HERO_WARBRINGEROBJ)
				ChangeTypeOfProp(a, b, PROPNORMAL1);
	}
	for (i = 0;i < MAXUNITPROPERTIES;i++)
	{
		prop[i] = b->modemove[a->typeofprop][i];		//copy properties
		if (a->prop & (VARNOTWORK | VARPOWEROFF))
		{
			if (prop[i] != MODEESCAPE)
				prop[i] = MODENON;
		}
		else if (mageprop[prop[i]].type_id == ORDERS_UNIT)
		{
			unitnr = INBAY_GetCreatedUnits(a);
			if (unitnr + underc >= INBAY_GetMaxUnitsToBeCreated(a))
				prop[i] = MODENON;
			if (underc == MAXCONSTRUCTINBUILD)
				prop[i] = MODENON;
		}
		else if (prop[i] == MODECLOAKFIELD ||
			prop[i] == MODEPERSONNELCLOAK ||
			prop[i] == MODEDECLOAK)
		{
			if (GetMageAtr(&a->atrobj, ATRINVISIBLE) == WRAITHEPODARBITEROM || GetMageAtr(&a->atrobj, ATRINVISIBLE) == NEWRAITHEPODARBITEROM)
				prop[i] = MODENON;
		}
		else if (prop[i] == MODELOADUNITS && !IfCanTransport(a->SC_Unit, playernr))
		{
			prop[i] = MODENON;
		}
	}
	//if galiutination clear mage properties
	if (IsHallucination(a))
	{
		for (i = 0;i < MAXUNITPROPERTIES;i++)
			if (!(mageprop[prop[i]].atr & ORDER_SHOWHALLUCINATED))
				prop[i] = MODENON;
	}
	if (a->addonobj)
	{
		prop[6] = MODENON;		//addonicons
		prop[7] = MODENON;		//addonicons
	}
	if (underc&&IsBuild(a->SC_Unit))
	{
		prop[6] = MODENON;		//
		prop[7] = MODENON;		//addon position
	}
	for (i = 0;i < MAXUNITPROPERTIES;i++)
	{
		if (prop[i] == MODERETURNCARGO)
		{
			if (a->carryobj)
			{
				if (IsNonNeutralFlag(a->carryobj->SC_Unit))
					prop[i] = MODENON;
			}
			else
				prop[i] = MODENON;
		}
		else if (prop[i] == MODEUNLOADUNITS)
		{
			if (!a->loaded)
				prop[i] = MODENON;
		}
		else if ((a->SC_Unit == SC_VULTUREOBJ || a->SC_Unit == SC_HERO_JIMRAYNORVOBJ) && prop[i] == MODERELEASEMINE && !a->ammo)
		{
			prop[i] = MODENON;
		}
		else
		{
			if (prop[i] == MODEMAKENUKEBOMB)
			{
				if (a->ammo)
					prop[i] = MODENON;
			}
			else
			{
				switch (mageprop[prop[i]].type_id)
				{
				case ORDERS_NONE:
					break;
				case ORDERS_MODEMOVE:
					//check if need to deny prop
					if (mageprop[prop[i]].obj_id >= 0)
					{
						if (!GetMaxTechTree(&map, playernr, mageprop[prop[i]].obj_id))
							prop[i] = MODENON;//prevent use if not available(max tech tree=0)
					}
					break;
				case ORDERS_UNIT:
					if (mageprop[prop[i]].obj_id >= 0)
					{
						if (!GetUnitRestrictions(&map, playernr, (SCUNIT) mageprop[prop[i]].obj_id))
							prop[i] = MODENON;
					}
					break;
				case ORDERS_UPGRADE:
					upgradenr = GetTempUpgradeTree(&map, playernr, mageprop[prop[i]].obj_id);
					if (upgradenr)
						prop[i] = MODENON;
					else
					{
						upgradenr = GetUpgradeTree(&map, playernr, mageprop[prop[i]].obj_id);
						maxupgradenr = GetMaxUpgradeTree(&map, playernr, mageprop[prop[i]].obj_id);
						if (upgradenr >= maxupgradenr)
							prop[i] = MODENON;//prevent showing at max upgrade level reached
					}
					break;
				case ORDERS_TECHNOLOGY:
					upgradenr = GetTempTechTree(&map, playernr, mageprop[prop[i]].obj_id);
					if (upgradenr)
						prop[i] = MODENON;
					else
					{
						upgradenr = GetTechTree(&map, playernr, mageprop[prop[i]].obj_id);
						maxupgradenr = GetMaxTechTree(&map, playernr, mageprop[prop[i]].obj_id);
						if (upgradenr >= maxupgradenr)
							prop[i] = MODENON;//prevent showing at max tech level reached
					}
					break;
				}//switch
			}
		}
	}//for
	return 0;
}
//==============================================
//create prop[9] from selectedobjects for player nr.
int CreateMenuProperties(int *prop, char *selectableicons, int player)
{
	OBJ *o;
	int i, j, err, maxprop = 0, selobj, curenticon;
	int propunits[MAXSELECTMAN][MAXUNITPROPERTIES];
	struct OBJstruct *b;
	//if we select move/mage need to put only cancel mage/move button
	if (highMouse->WaitToPressLeftButton)
	{
		for (i = 0;i < MAXUNITPROPERTIES - 1;i++)
		{
			prop[i] = MODENON;
			selectableicons[i] = FORYELLOW;
		}
		prop[8] = MODECANCEL_BB_AB;
		selectableicons[8] = FORYELLOW;
		return 0;
	}
	//create all properties for selected units
	selobj = SelectedUnits.Count();
	if (selobj)
	{
		SelectedUnits.EnumListInit();
		while ((o = (OBJ *)SelectedUnits.GetNextListElem(NULL)))
		{
			if (!PLAYER[NUMBGAMER].isobserverflag 	&&
				player_aliance(o->playernr, player) == MYOBJ)
			{
				b = loadobj(o->SC_Unit);
				err = CreateUnitProperties(o, b, propunits[maxprop], player);
				if (!err)
					maxprop++;
				if (maxprop == 1)
				{
					curentproperties = o->modemove;
					curenticon = mageprop[curentproperties].icon_id;
					curentoperationobj = o;
				}
			}
		}
	}
	if (selobj == 1 && SelectedUnits.Count() && ((OBJ *)SelectedUnits.GetElem(0, NULL))->SC_Unit == SC_MEDICOBJ)
		propunits[0][2] = MODENON;				//do not show atack
	//combine all properties of units to have same one
	if (maxprop && selobj)
	{
		for (i = 0;i < MAXUNITPROPERTIES;i++)
		{
			prop[i] = propunits[0][i];
		}
		if (maxprop > 1)
		{
			for (i = 0;i < MAXUNITPROPERTIES;i++)
			{
				for (j = 1;j < maxprop;j++)
				{
					switch (prop[i])
					{
					case MODECLOAKFIELD:
					case MODEPERSONNELCLOAK:
					case MODEDECLOAK:
						if (propunits[j][i] != MODECLOAKFIELD &&
							propunits[j][i] != MODEPERSONNELCLOAK &&
							propunits[j][i] != MODEDECLOAK)
							prop[i] = MODENON;
						break;
					case MODEBURROW:
					case MODEBURROW2:
					case MODEUNBURROW:
						if (propunits[j][i] != MODEBURROW &&
							propunits[j][i] != MODEBURROW2 &&
							propunits[j][i] != MODEUNBURROW)
							prop[i] = MODENON;
						break;
					case MODETANKSIEGE:
					case MODETANKNORMAL:
						if (propunits[j][i] != MODETANKSIEGE &&
							propunits[j][i] != MODETANKNORMAL)
							prop[i] = MODENON;
						break;
					case MODELIFTOFF:
					case MODELANDING:
						if (propunits[j][i] != MODELIFTOFF &&
							propunits[j][i] != MODELANDING)
							prop[i] = MODENON;
						break;
					default:
						if (prop[i] != propunits[j][i])
							prop[i] = MODENON;
						break;
					}//switch
				}//forj
			}//fori
		}
		for (i = 0;i < MAXUNITPROPERTIES;i++)
		{
			o = (OBJ *)SelectedUnits.GetElem(0, NULL);
			if (o && prop[i] != MODENON)
			{
				if (!MageDepend(o, player, prop[i]))
				{
					selectableicons[i] = FORGRAY;
				}
				else
				{
					switch (prop[i])
					{
					case MODENUKELAUNCH:
						if (!PLAYER[player].nukes)
							selectableicons[i] = FORGRAY;
						else
							selectableicons[i] = FORYELLOW;
						break;
					case MODEWARPARCHON:
					case MODEWARPDARCHON:
						if (!warparchon)
							selectableicons[i] = FORGRAY;
						else
							selectableicons[i] = FORYELLOW;
						break;
					default:
						if (o->prop & VARHOLDPOSBIT)
						{
							if (prop[i] == MODEHOLDPOS)
								selectableicons[i] = FORWHITE;
							else
								selectableicons[i] = FORYELLOW;
						}
						else
						{
							if (mageprop[prop[i]].icon_id == curenticon)
								selectableicons[i] = FORWHITE;
							else
								selectableicons[i] = FORYELLOW;
						}
						break;
					}
				}
			}
		}
	}//maxprop
	else		//if not units put no properties
	{
		for (i = 0;i < MAXUNITPROPERTIES;i++)
		{
			prop[i] = MODENON;
		}
	}
	return 0;
}
//==============================================
int ChangeTypeOfProp(struct OBJ *a,struct OBJstruct *b,int typeofprop)
{
	if (a->typeofprop == PROPEMPTY)//if set this orders line, it is for infinite and deny to change
		return(-1);
	if (typeofprop < b->maxlineprop)
	{
		ChangeUnitIconProp(a,typeofprop);
		return 0;
	}
	return -1;
}
//=============================================
int ChangeTypeOfProp(struct OBJ *a,int typeofprop)
{
	return(ChangeTypeOfProp(a,loadobj(a->SC_Unit),typeofprop));
}
//=============================================
int ExistProp(struct OBJstruct *b,int verifymode)
{
	int i,j;
	for (i=0;i<MAXLINESPROPERTIES;i++)
		for (j=0;j<MAXUNITPROPERTIES;j++)
			if (b->modemove[i][j]==verifymode)
				return 1;
	return 0;
}
//================================================================

