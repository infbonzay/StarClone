
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "man.h"
#include "Enumerate.h"
#include "mage.h"
#include "weapons.h"
#include "ObjChecks.h"
#include "objs.h"
#include "auxil.h"
#include "lists.h"
#include "iscript.h"
#include "vars.h"
#include "images.h"
#include "gener.h"
#include "flingy.h"

FLINGYLIST weaponflingy(3000);

//=====================================================
void FLINGYLIST::DeleteMarked(void)		//delete marked
{
	SC_FLINGY *flingy;
	int i, lastelem;
	if (!CountMarked)
		return;
	CountMarked = 0;
	lastelem = _Count - 1;
	for (i = lastelem;i >= 0;i--)
	{
		if (Marked[i])
		{
			flingy = elements[i];
			if (flingy->listdamaged)
			{
				flingy->listdamaged->FlushList();
				delete flingy->listdamaged;
			}
			delete flingy;
			if (i != lastelem)
			{
				elements[i] = elements[lastelem];
				elements[i]->flingylist_elemnr = i;
				Marked[i] = Marked[lastelem];
			}
			Marked[lastelem] = 0;
			_Count--;
			lastelem--;
		}
	}
}
//=========================================
void FLINGYLIST::Free(void)									//mark all,delete all,empty all
{
	SC_FLINGY *flingy;
	weaponflingy.EnumListInit();
	while ((flingy = weaponflingy.GetNextListElem(NULL)))
	{
		flingy->DelFlingy();
	}
	DeleteMarked();

	FreeAndEmptyAll();
}
//=========================================
void FLINGYLIST::RemoveOBJFromFlingyobj(OBJ *obj, OBJ *newatacker)
{
	SC_FLINGY *flingy;
	weaponflingy.EnumListInit();
	while ((flingy = weaponflingy.GetNextListElem(NULL)))
	{
		if (flingy->destobj == obj)
		{
			flingy->destobj = NULL;
		}
		if (flingy->atacker == obj)
		{
			flingy->atacker = newatacker;
		}
	}
}
//====================================================
void FLINGYLIST::MoveAllWeaponFlingy(void)
{
	SC_FLINGY *flingy;
	EnumListInit();
	while ((flingy = GetNextListElem(NULL)))
	{
		if (flingy->flags & FLINGY_FLAG_STAYANDWAITTIME)
			flingy->StayAndWaitTime();
		else
			flingy->MoveFlingy();
		if (flingy->timer)
		{
			if (--flingy->timer == 0)
			{
				flingy->DieFlingy();
			}
		}
	}
}
//=========================================
int SC_FLINGY::DieFlingy(void)
{
	OBJ *newdestobj;
	if (--nroftargets <= 1)
	{
		if (flags & FLINGY_FLAG_AFTERLAUNCHCHANGEELEVATION)
		{
			if (destobj)
				img->elevationlevel = destobj->mainimage->elevationlevel + 1;
			else
				img->elevationlevel += 2;
		}
		flags &= ~FLINGY_FLAG_ACCELERATIONBIT;
		img->SetIScriptNrAll(ISCRIPTNR_DEATH, ISCRIPTNR_EXECUTE);
		return(1);
	}
	else
	{
		//search for new atack target != destobj
		//search for new enemy target
		if (destobj)
		{
			newdestobj = SearchNewBounceOBJ(img->xpos >> 8, img->ypos >> 8, playernr, destobj, destobj2, alldattbl.weapons_dat->MaximumRange[weapon_id]);
		}
		else
			newdestobj = NULL;
		if (!newdestobj)
		{
			if (flags & FLINGY_FLAG_AFTERLAUNCHCHANGEELEVATION)
			{
				if (destobj)
					img->elevationlevel = destobj->mainimage->elevationlevel + 1;
				else
					img->elevationlevel += 2;
			}
			flags &= ~FLINGY_FLAG_ACCELERATIONBIT;
			img->SetIScriptNrAll(ISCRIPTNR_DEATH, ISCRIPTNR_EXECUTE);
			return(1);
		}
		else
		{
			img->iscriptnr = 255;
			img->SetIScriptNrAll(ISCRIPTNR_SPECIALSTATE1, ISCRIPTNR_EXECUTE);
			if (!destobj2)
				destobj2 = destobj;
			destobj = newdestobj;
			xdest256 = GetOBJx256(destobj);
			ydest256 = GetOBJy256(destobj);
			img->AllUnitDirection256(CalcDirection(img->xpos, img->ypos, xdest256, ydest256));
		}
	}
	return(0);
}
//=========================================
void SC_FLINGY::DelFlingy(void)
{
	img->flags |= SC_IMAGE_FLAG_MARKFORDELETE;
	weaponflingy.MarkForRemove(flingylist_elemnr);
	img = NULL;
}
//=========================================
void SC_FLINGY::StayAndWaitTime(void)
{
	OBJ *a;
	int i, mindist;
	mindist = mageprop[frommodemove].diapazone;
	Enumerate<OBJ *> EnumObj(&MaxObjects, objects);
	while( (a = EnumObj.GetNext()) )
//	for (i = 0;i < MaxObjects;i++)
	{
//		a = objects[i];
		if (IsActiveUnit(a))
		{
			if (GetDistances(xdest256, ydest256, GetOBJx256(a), GetOBJy256(a)) <= mindist)
			{
				if (IsHallucination(a))
				{
					dieobj(a);
					continue;
				}
				if (!WeaponCanApplyOnUnit(a, playernr, weapon_id))
					continue;
				a->psistormdamage = dmgpertick;
			}
		}
	}
}
//=========================================
void SC_FLINGY::MoveFlingy(void)
{
	int deltaz, deltax, deltay, rot;
	if (!(flags & FLINGY_FLAG_ACCELERATIONBIT))
	{
		return;
	}
	//accelerate speed
	if (alldattbl.flingy_dat->MoveControl[flingy_id] != FLINGYMOVECONTROL_ISCRIPT)
	{
		currentspeed += alldattbl.flingy_dat->Acceleration[flingy_id];
		if (currentspeed > alldattbl.flingy_dat->TopSpeed[flingy_id])
		{
			currentspeed = alldattbl.flingy_dat->TopSpeed[flingy_id];
			//			flags |= FLINGY_FLAG_REACHMAXSPEED;
		}
		if (flags & FLINGY_FLAG_MOVETOOBJ)
		{
			if (!destobj)
			{
				//destobj dies
				flags &= ~FLINGY_FLAG_MOVETOOBJ;
			}
			else
			{
				xdest256 = GetOBJx256(destobj);
				ydest256 = GetOBJy256(destobj);
			}
		}
		if (alldattbl.images_dat->Graphic_Turns[img->imageid])
		{
			img->UnitNeededDirection256(CalcDirection(img->xpos, img->ypos, xdest256, ydest256));
			rot = img->Rotation(alldattbl.flingy_dat->TurnRadius[flingy_id]);
			//			if (!rot)
			//				if (flags & FLINGY_FLAG_REACHMAXSPEED)
			//					currentspeed -= alldattbl.flingy_dat->Acceleration[flingy_id];
		}
		else
		{
			img->AllUnitDirection256(CalcDirection(img->xpos, img->ypos, xdest256, ydest256));
		}
	}
	//check if flingy come to destination
	deltax = xdest256 - img->xpos;
	deltay = ydest256 - img->ypos;
	deltaz = (int)hypot(deltax, deltay);
	//	  if (deltaz <= alldattbl.flingy_dat->TopSpeed[flingy_id]/2)
	if (deltaz - sizedestobj <= currentspeed / 2)
	{
		if (DieFlingy())
			return;
	}
	//do move flingy
	img->MoveInUnitDirection(NULL, img->side, currentspeed);
	if (++movedticks == 3)
	{
		if (flags & FLINGY_FLAG_AFTERLAUNCHCHANGEELEVATION)
		{
			flags &= ~FLINGY_FLAG_AFTERLAUNCHCHANGEELEVATION;
			if (destobj)
				img->elevationlevel = destobj->mainimage->elevationlevel + 1;
			else
				img->elevationlevel += 2;
		}
	}
}
//=========================================
SC_FLINGY::SC_FLINGY(OBJ *a, OBJ *dest, int xdest_256, int ydest_256, unsigned char flingyid)
{
	atacker = a;
	playernr = a->playernr;
	destobj = dest;
	destobj2 = NULL;
	xdest256 = xdest_256;
	ydest256 = ydest_256;
	flingy_id = flingyid;
	flags = 0;
	currentspeed = 0;
	movedticks = 0;
	nroftargets = 1;
	if (dest)
		//		sizedestobj = GetDestUnitHalfSize(dest->SC_Unit,GetOBJx(a)-GetOBJx(dest),GetOBJy(a)-GetOBJy(dest));
		sizedestobj = GetUnitWidthAndHeight(dest->SC_Unit, UNITDIM_WIDTH) / 3 * 256;
	else
		sizedestobj = 0;
	flags |= FLINGY_FLAG_ACCELERATIONBIT;
	if (dest)
		flags |= FLINGY_FLAG_MOVETOOBJ;
	listdamaged = NULL;
}
//=========================================
void CalcXYOffsets(int side,int deltax,int deltaycenter,signed char *xoffs,signed char *yoffs)
{
	int deltay = 0;
	short sinalfa,cosalfa;
	sinalfa = sintable256[side];
	cosalfa = costable256[side];
	*xoffs = -((deltax*cosalfa + deltay*sinalfa)/256);
	*yoffs = -((deltax*sinalfa + deltay*cosalfa)/256) - deltaycenter;
}
//===========================================
SC_FLINGY *CreateFlingyID(OBJ *a, OBJ *destobj, int xstart256, int ystart256, int xdest256, int ydest256,
	unsigned char weapon_id, unsigned char flingy_id, int deltaelevation)
{
	SC_FLINGY *retflingy;
	unsigned char launchspin;
	if (alldattbl.weapons_dat->DamageFactor[weapon_id] == 1)
	{
		return(CreateWeaponFlingy(a, destobj, xstart256, ystart256, xdest256, ydest256, weapon_id, flingy_id, 0, deltaelevation));
	}
	else
	{
		//create additional flingy
		launchspin = alldattbl.weapons_dat->LaunchSpin[weapon_id];
		retflingy = CreateWeaponFlingy(a, destobj, xstart256, ystart256, xdest256, ydest256, weapon_id, flingy_id, -launchspin, deltaelevation);
		CreateWeaponFlingy(a, destobj, xstart256, ystart256, xdest256, ydest256, weapon_id, flingy_id, +launchspin, deltaelevation);
		return(retflingy);
	}
}
//===========================================
SC_FLINGY *Create2FlingyID(OBJ *a, OBJ *destobj, int xstart256, int ystart256, int xdest256, int ydest256, int xdest256_2, int ydest256_2,
	unsigned char weapon_id, unsigned char flingy_id, int deltaelevation)
{
	SC_FLINGY *retflingy;
	unsigned char launchspin;
	//create additional flingy
	launchspin = alldattbl.weapons_dat->LaunchSpin[weapon_id];
	retflingy = CreateWeaponFlingy(a, NULL, xstart256, ystart256, xdest256, ydest256, weapon_id, flingy_id, -launchspin, deltaelevation);
	retflingy->destobj = destobj;
	retflingy = CreateWeaponFlingy(a, NULL, xstart256, ystart256, xdest256_2, ydest256_2, weapon_id, flingy_id, +launchspin, deltaelevation);
	retflingy->destobj = destobj;
	return(retflingy);
}
//===========================================
SC_FLINGY *CreateWeaponFlingy(OBJ *a, OBJ *destobj, int xstart256, int ystart256, int xdest256, int ydest256,
	unsigned char weapon_id, unsigned char flingy_id, signed char launchspin, int deltaelevation)
{
	unsigned char elevation, launchside;
	signed char weaponxoffs, weaponyoffs;
	short sprites_id, images_id;
	unsigned short flags = 0;
	SC_FLINGY *flingy;
	launchside = a->mainimage->side + launchspin;
	flingy = new SC_FLINGY(a, destobj, xdest256, ydest256, flingy_id);
	flingy->timer = alldattbl.weapons_dat->RemoveAfter[weapon_id];
	weaponflingy.Add(flingy);
	sprites_id = alldattbl.flingy_dat->sprites_id[flingy_id];
	if (!alldattbl.sprites_dat->is_visible[sprites_id])
		flags |= SC_IMAGE_FLAG_DISABLEDRAW;
	images_id = alldattbl.sprites_dat->images_id[sprites_id];
	CalcXYOffsets(launchside, alldattbl.weapons_dat->ForwardOffset[weapon_id], alldattbl.weapons_dat->UpwardOffset[weapon_id], &weaponxoffs, &weaponyoffs);
	if (destobj)
	{
		elevation = destobj->mainimage->elevationlevel;
		if (IsOnSkyOBJ(destobj))
			flags |= SC_IMAGE_FLAG_AIRIMG;
	}
	else
	{
		elevation = a->mainimage->elevationlevel;
		if (IsOnSkyOBJ(a))
			flags |= SC_IMAGE_FLAG_AIRIMG;
	}
	flingy->img = new MAIN_IMG(images_id, xstart256 + (weaponxoffs << 8), ystart256 + (weaponyoffs << 8), elevation + deltaelevation, 0, 0, launchside, a->color, flags, ISCRIPTNR_INIT);
	mainimageslist.Add(flingy->img);

	flingy->img->whocreate = SC_IMAGE_FLINGY_CREATOR;
	flingy->img->creator.flingycreator.flingy = flingy;
	flingy->weapon_id = weapon_id;
	flingy->frommodemove = a->castmagenr;
	flingy->SC_Unit = a->SC_Unit;
	if (deltaelevation == -1)
		flingy->flags |= FLINGY_FLAG_AFTERLAUNCHCHANGEELEVATION;
	return(flingy);
}
//===========================================

