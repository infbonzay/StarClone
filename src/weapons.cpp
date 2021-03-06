
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <grplib/grp.h>
#include <math.h>

#include "audio.h"
#include "objs.h"

#include "vars.h"
#include "auxil.h"
#include "scripts.h"
#include "action.h"
#include "gr.h"
#include "maps.h"
#include "mpq.h"
#include "mylist.h"
#include "lists.h"
#include "ObjChecks.h"
#include "market.h"
#include "flingy.h"
#include "load.h"
#include "putobj.h"
#include "mage.h"
#include "overlay.h"
#include "rand.h"
#include "stringfiles.h"
#include "images.h"
#include "iscript.h"
#include "gener.h"
#include "man.h"
#include "Enumerate.h"
#include "weapons.h"

WEAPONS_UNIT weapons[MAX_WEAPONS_ELEM];
//===========================================
void unloadweapons(void)
{
}
//===========================================
int loadweapons(void)
{
	int whatobj, i, j;
	int flags;
	unsigned char trailrefresh, rangeupgnr, rangeupgaddfactor;
	int nrofweapons, maxnrforread[100];
	static char strid[200], strvalue[200];
	FILE *f = fopen(fileweaponsconf, "rb");
	if (!f)
	{
		return(-1);
	}
	while (!feof(f))
	{
		strcpy(strid, "");
		strcpy(strvalue, "");
		fscanf(f, "%s \n", strid);
		strupr(strid);
		flags = 0;
		fscanf(f, "%s \n", strvalue);//read '='
		fscanf(f, "%s \n", strvalue);//read flags
		for (j = 0;j < sizeof(rezwp) / sizeof(char *);j++)
		{
			if (!strcmp(strid, rezwp[j]))
			{
				switch (j)
				{
				case 0://[weapon] = nr_sc_weapon1-,: ...
					nrofweapons = getallnumbers(strvalue, maxnrforread, 100);
					trailrefresh = 255;
					rangeupgnr = 255;
					rangeupgaddfactor = 0;
					//printf("%s = %s\n",strid,strvalue);
					goto nextweaponcmd;
					break;
				case 1://RANGEUPGNR upgrade_id increase_range_by_this_value
					rangeupgnr = atoi(strvalue);
					fscanf(f, "%s \n", strvalue);
					rangeupgaddfactor = atoi(strvalue);
					for (i = 0;i < nrofweapons;i++)
					{
						whatobj = maxnrforread[i];
						weapons[whatobj].rangeupgnr = rangeupgnr;
						weapons[whatobj].rangeupgaddfactor = rangeupgaddfactor;
					}
					goto nextweaponcmd;
					break;
				}
			}
		}
		if (j >= sizeof(rezwp) / sizeof(char *))
		{
			if (strid[0] == '#')
				continue;
			DEBUGMESSCR("WEAPON:error [%s] is not WEAPON DIRECTIVE\n", strid);
			fclose(f);
			return(-2);
		}
nextweaponcmd:
	;
	}
	fclose(f);
	return(0);
}
//===========================================
#define WEAPON_UNIT_ANY			0
#define WEAPON_UNIT_CARRIER		1
int unitweapon_retstatus[2] = { CREATEDWEAPONSTATUS_ATACKSUCCESS,CREATEDWEAPONSTATUS_LAUNCHINTERCEPTORS };
//===========================================
int IfCanCreateWeapon(OBJ *atacker, OBJ *destobj, int *errmes, unsigned char *weapon_id, int flags)
{
	SCUNIT SC_Unit = atacker->SC_Unit;
	SCUNIT Subunit1;
	int rangebad = 0, weaponunitid;
	signed char deltaside;
	unsigned char groundweapon_id, airweapon_id, usedweapon_id, atackangle, neededside;
	if (destobj)
	{
		if (!IsActiveUnitForAtack(destobj,flags & CREATEWEAPON_ISINTRANSPORT))
		{
			if (errmes)
				*errmes = 875;
			return(CREATEDWEAPONSTATUS_CANTATACKTHISUNIT);
		}
		Subunit1 = (SCUNIT) alldattbl.units_dat->Subunit1[atacker->SC_Unit];
		if (Subunit1 < MAX_UNITS_ELEM)
			SC_Unit = Subunit1;
		groundweapon_id = alldattbl.units_dat->GroundWeapon[SC_Unit];
		airweapon_id = alldattbl.units_dat->AirWeapon[SC_Unit];
		switch (atacker->SC_Unit)
		{
		case SC_REAVEROBJ:
		case SC_HERO_WARBRINGEROBJ:
			if (IsOnSkyOBJ(destobj))
			{
				if (errmes)
					*errmes = 875;
				return(CREATEDWEAPONSTATUS_CANTATACKTHISUNIT);
			}
			if (!atacker->ammo)
			{
				if (errmes)
					*errmes = 878;
				return(CREATEDWEAPONSTATUS_NOTAMMO);
			}
			if (GetDistanceBetweenUnits256(atacker, destobj) <= (GetTargetAcquisitionRange(atacker->SC_Unit) << 13))		//256 * 32 (<<13)
				rangebad = 0;
			else
				rangebad = 1;
			weaponunitid = WEAPON_UNIT_ANY;
			usedweapon_id = WEAPONID_REAVERRANGE;
			weapon_id = NULL;
			atackangle = alldattbl.weapons_dat->AttackAngle[WEAPONID_SCARAB];
			break;
		case SC_CARRIEROBJ:
		case SC_HERO_GANTRITHOROBJ:
			if (GetDistanceBetweenUnits256(atacker, destobj) <= (GetTargetAcquisitionRange(atacker->SC_Unit) << 13))
				rangebad = 0;
			else
				rangebad = 1;
			weaponunitid = WEAPON_UNIT_CARRIER;
			//usedweapon_id = airweapon_id;
			usedweapon_id = WEAPONID_CARRIERRANGE;
			atackangle = 0;
			atacker->prop &= ~VARNEEDTOLAUNCHINTERCEPTORS;
			break;
		case SC_INTERCEPTOROBJ:
			if (atacker->myparent)			//ignore distance between interceptorparent(carrier) and atack destination because carier was die
			{
				if (GetDistanceBetweenUnits256(atacker, atacker->myparent) > (8 + 6) * 32 * 256)
					return(CREATEDWEAPONSTATUS_INTERCEPTORFARAWAY);
			}
		default:
			if (!IsOnSkyOBJ(destobj))
			{
				if (groundweapon_id < MAX_WEAPONS_ELEM)
				{
					usedweapon_id = groundweapon_id;
				}
				else
				{
					if (errmes)
						//*errmes=alldattbl.weapons_dat->TargetErrorMessage[groundweapon_id];
						*errmes = 875;
					return(CREATEDWEAPONSTATUS_CANTATACKTHISUNIT);
				}
			}
			else
			{
				if (IsOnSkyOBJ(destobj))
				{
					if (airweapon_id < MAX_WEAPONS_ELEM)
					{
						usedweapon_id = airweapon_id;
					}
					else
					{
						if (errmes)
							*errmes = 875;
						return(CREATEDWEAPONSTATUS_CANTATACKTHISUNIT);
					}
				}
			}
			if (destobj || atacker)
			{
				//check visibility
				if (destobj &&
					(OBJ_VAR_CHK(destobj, obj_notdetect, atacker->playernr) ||
					(!OBJ_VAR_CHK(destobj, obj_see, atacker->playernr))) )
				{
					if (errmes)
						*errmes = 875;//invalid target
					return(CREATEDWEAPONSTATUS_CANTATACKTHISUNIT);
				}
				else
				{
					if (Subunit1 < MAX_UNITS_ELEM)
					{
						//subunit atack
						if (weapon_id)
							*weapon_id = usedweapon_id;
						return(CREATEDWEAPONSTATUS_DESTOUTOFRANGE);
					}
					else
					{
						rangebad = CheckWeaponRange(atacker, destobj, usedweapon_id, atacker->playernr);
					}
				}
			}
			weaponunitid = WEAPON_UNIT_ANY;
			atackangle = alldattbl.weapons_dat->AttackAngle[usedweapon_id];
			break;
		}
		if (weapon_id)
			*weapon_id = usedweapon_id;
		switch (rangebad)
		{
		case 0://range ok
			if (!IsOnSkyOBJ(atacker))
			{
				if (mapEFFECT(atacker->xkart, atacker->ykart, DISRUPTEFFECT))//unit under disruption
				{
					return(CREATEDWEAPONSTATUS_UNDERDISRUPTION);//return 'under disruption'
				}
			}
			if (UnitIgnoreInvisibles(SC_Unit) || !OBJ_VAR_CHK(destobj, obj_notdetect, atacker->playernr))
			{
				if (atackangle)
				{
					neededside = CalcDirection(GetOBJx256(atacker), GetOBJy256(atacker), GetOBJx256(destobj), GetOBJy256(destobj));
					deltaside = neededside - atacker->mainimage->side;
					if (abs(deltaside) > atackangle)
					{
						//need to rotate before createweapon
						atacker->mainimage->UnitNeededDirection256(neededside);
						if (!atacker->atackcooldowntime)
						{
							atacker->atackcooldowntime = 2; //to come here again from additionalunitproceed
							atacker->mainimage->waitticks = 1;
						}
						return(CREATEDWEAPONSTATUS_ATACKSUCCESSWITHROTATION);
					}
					atacker->mainimage->AllUnitDirection256(neededside);
				}
				return(unitweapon_retstatus[weaponunitid]);
			}
			break;
		case 1:
			if (errmes)
				*errmes = 887;//out of range
			break;
		case 2:
			if (errmes)
				*errmes = 888;//range is too close
			return(CREATEDWEAPONSTATUS_DESTTOCLOSE);
			break;
		}
		return(CREATEDWEAPONSTATUS_DESTOUTOFRANGE);
	}
	if (errmes)
		*errmes = 875;//invalid target
	return(CREATEDWEAPONSTATUS_CANTATACKTHISUNIT);
}
//===========================================
int IsSplashDamage(unsigned char weapon_id)
{
	return( alldattbl.weapons_dat->InnerSplash[weapon_id] +
			alldattbl.weapons_dat->MediumSplash[weapon_id] +
			alldattbl.weapons_dat->OuterSplash[weapon_id] );
}
//=======================================
void WeaponPlaySFX(OBJ *a, int sfxdata_id, int distance, int x, int y)
{
	if (a)
		distance = DistanceForAudio(GetOBJx(a), GetOBJy(a));
	else
		distance = DistanceForAudio(x, y);
	Play_sfxdata_id(a, sfxdata_id, -1, distance);
}
//=======================================
//int CalcMaxRangeCoordsXY(int deltax,int deltay,int *destx,int *desty,unsigned char weapon_id,unsigned char flingy_id)
int CalcMaxRangeCoordsXY(int deltax, int deltay, int *destx, int *desty, int maxdist)
{
	int deltaz;
	deltaz = (int)hypot(deltax >> 8, deltay >> 8);
	if (!deltaz)
	{
		deltaz = 1;
		deltax = 1;
		deltay = 0;
	}
	*destx = maxdist * deltax / deltaz;
	*desty = maxdist * deltay / deltaz;
	return(0);
}
//=================================
//return 1 if selfdestruct
int CreateWeaponID(OBJ *a, OBJ *destobj, int xdest256, int ydest256, unsigned char weapon_id, int createflingy)
{
	SC_FLINGY *flingy;
	unsigned char flingy_id;
	unsigned short images_id, sprites_id;
	int xdest256_2, ydest256_2, maxrangex, maxrangey, elevationdelta, maxdist;
	a->usedweapon_id = weapon_id;
	if (createflingy)
	{
		flingy_id = alldattbl.weapons_dat->flingy_id[weapon_id];
		sprites_id = alldattbl.flingy_dat->sprites_id[flingy_id];
		images_id = alldattbl.sprites_dat->images_id[sprites_id];
		elevationdelta = GetDeltaWeaponElevationLevel(weapon_id);
		switch (alldattbl.weapons_dat->WeaponBehavior[weapon_id])
		{
		case WB_FLY_DONT_FOLLOW_TARGET:				//0
			if (destobj)
			{
				xdest256 = GetOBJx256(destobj);
				ydest256 = GetOBJy256(destobj);
			}
			flingy = CreateFlingyID(a, NULL, GetOBJx256(a), GetOBJy256(a), xdest256, ydest256, weapon_id, flingy_id, elevationdelta);
			flingy->destobj = destobj;
			break;
		case WB_FLY_AND_FOLLOW_TARGET:				//1
			if (destobj)
			{
				xdest256 = GetOBJx256(destobj);
				ydest256 = GetOBJy256(destobj);
			}
			CreateFlingyID(a, destobj, GetOBJx256(a), GetOBJy256(a), xdest256, ydest256, weapon_id, flingy_id, elevationdelta);
			break;
		case WB_APPEAR_ON_TARGET_UNIT:				//2
			if (destobj)
				CreateFlingyID(a, destobj, GetOBJx256(destobj), GetOBJy256(destobj), GetOBJx256(destobj), GetOBJy256(destobj), weapon_id, flingy_id, elevationdelta);
			break;
		case WB_PERSIST_ON_TARGET_SITE:				//3
			//psionic storm,plague
			flingy = CreateFlingyID(a, NULL, xdest256, ydest256, xdest256, ydest256, weapon_id, flingy_id, elevationdelta);
			iscriptinfo.ExecuteScript(flingy->img);
			flingy->flags |= FLINGY_FLAG_STAYANDWAITTIME;
			flingy->timer = alldattbl.weapons_dat->RemoveAfter[weapon_id];
			//special damage for psionic storm
			flingy->dmgpertick = alldattbl.weapons_dat->DamageAmount[weapon_id] * 8 * 256 / alldattbl.weapons_dat->RemoveAfter[weapon_id];
			flingy->img->SetIScriptNrAll(ISCRIPTNR_SPECIALSTATE2, ISCRIPTNR_EXECUTE);
			break;
		case WB_APPEAR_ON_TARGET_SITE:				//4
			flingy = CreateFlingyID(a, NULL, xdest256, ydest256, xdest256, ydest256, weapon_id, flingy_id, elevationdelta);
			break;
		case WB_APPEAR_ON_ATACKER:					//5
			flingy = CreateFlingyID(a, NULL, xdest256, ydest256, xdest256, ydest256, weapon_id, flingy_id, elevationdelta);
			flingy->destobj = destobj;
			break;
		case WB_ATACK_AND_SELF_DESTRUCT:			//6
			if (destobj)
			{
				CreateFlingyID(a, destobj, GetOBJx256(destobj), GetOBJy256(destobj), GetOBJx256(destobj), GetOBJy256(destobj), weapon_id, flingy_id, elevationdelta);
			}
			dieobj_silently(a);
			return(1);
			break;
		case WB_BOUNCE:								//7
			if (destobj)
			{
				flingy = CreateFlingyID(a, destobj, GetOBJx256(a), GetOBJy256(a), GetOBJx256(destobj), GetOBJy256(destobj), weapon_id, flingy_id, elevationdelta);
				flingy->flags |= FLINGY_FLAG_BOUNCEATACK;
				flingy->nroftargets = 4;
			}
			break;
#define MAXRADIUS	(2*32)
		case WB_ATACK_3X3_AREA:						//8
			if (destobj)
			{
				xdest256 = GetOBJx256(destobj);
				ydest256 = GetOBJy256(destobj);
			}
			xdest256 = xdest256 + myrand((-MAXRADIUS/2) * 256, (MAXRADIUS/2-1) * 256);			//3x3 area
			ydest256 = ydest256 + myrand((-MAXRADIUS/2) * 256, (MAXRADIUS/2-1) * 256);

			xdest256_2 = xdest256 + myrand((-MAXRADIUS/2) * 256, (MAXRADIUS/2-1) * 256);;		//3x3 area
			ydest256_2 = ydest256 + myrand((-MAXRADIUS/2) * 256, (MAXRADIUS/2-1) * 256);;

			//xdest256 += myrand(-48 * 256, 47 * 256);					//3x3 area
			//ydest256 += myrand(-48 * 256, 47 * 256);

			flingy = Create2FlingyID(a, destobj, GetOBJx256(a), GetOBJy256(a), xdest256, ydest256, xdest256_2, ydest256_2, weapon_id, flingy_id, elevationdelta);
			break;
		case WB_GOTO_MAX_RANGE:						//9
			maxdist = alldattbl.weapons_dat->MaximumRange[weapon_id];// + alldattbl.flingy_dat->Acceleration[flingy_id]/256;
			CalcMaxRangeCoordsXY(xdest256 - GetOBJx256(a), ydest256 - GetOBJy256(a), &maxrangex, &maxrangey, maxdist);

			xdest256 = GetOBJx256(a) + maxrangex;
			ydest256 = GetOBJy256(a) + maxrangey;

			flingy = CreateFlingyID(a, NULL, GetOBJx256(a), GetOBJy256(a), xdest256, ydest256, weapon_id, flingy_id, elevationdelta);
			flingy->destobj = destobj;
			flingy->listdamaged = new mylist();
			break;
		default:
			DEBUGMESSCR("weapon with this behaviour not developed yet\n");
			break;
		}
	}
	else
	{
		//do damage only
		WeaponDoDamage(a, destobj, GetOBJx256(a), GetOBJy256(a), a->SC_Unit, a->playernr, weapon_id, -1, 0, NULL);
	}
	return(0);
}
//===========================================
void GetOBJXYSideOffsets(OBJ *a, MAIN_IMG *img, int overlaytype, signed char *xlo, signed char *ylo)
{
	unsigned short imagelo_id;
	signed char *adrxyoffs;
	LOFILE *lo;
	unsigned char objside;
	imagelo_id = GetIDFromOverlayLayer(img->imageid, overlaytype);
	if (imagelo_id && GetLoadedImage(imagelo_id, (void **)&lo) >= 0)
	{
		if (img->side & 0x80)
		{
			objside = (256 - img->side) / 8;
			adrxyoffs = GetLoXY(lo, 0 + objside, 0);
			*xlo = -adrxyoffs[0];
		}
		else
		{
			objside = img->side / 8;
			adrxyoffs = GetLoXY(lo, 0 + objside, 0);
			*xlo = adrxyoffs[0];
		}
		*ylo = adrxyoffs[1];
	}
	else
	{
		*xlo = 0;
		*ylo = 0;
	}
}
//===========================================
//						     0-100%,1-11%,2-33%,3-100%
const int BounceDamage[5] = { 256, 28, 89, 256 };
//===========================================
void WeaponDoDamage(OBJ *atacker, OBJ *destobj, int x256, int y256, SCUNIT SC_Unit,
	int playernr, unsigned char weapon_id, int castmagenr, int targetnr, mylist *listdamaged)
{
	MAIN_IMG *newimg;
	OBJ *o;
	int mindist[3], i, realdamage, damage, distance256, prevsporecnt, corrvalue;
	switch (alldattbl.weapons_dat->WeaponEffect[weapon_id])
	{
	case WEFFECT_NONE:
		break;
	case WEFFECT_MAELSTROM:
		newimg = new MAIN_IMG(IMAGEID_MAELSTROMHIT, x256, y256, atacker->mainimage->elevationlevel + 1, 0, 0, 0, 0, 0, ISCRIPTNR_INIT);
		newimg->whocreate = SC_IMAGE_SELF_CREATOR;
		mainimageslist.Add(newimg);

		if (castmagenr != -1)
			mindist[0] = mageprop[castmagenr].diapazone;
		else
			mindist[0] = alldattbl.weapons_dat->InnerSplash[weapon_id] * 256;
		if (atacker)
			if (mageprop[castmagenr].sound_id[SOUNDONHIT])
				Play_sfxdata(GetOBJx(atacker), GetOBJy(atacker), mageprop[castmagenr].sound_id[SOUNDONHIT], 2);
		{
			Enumerate<OBJ *> EnumObj(&MaxObjects, objects);
			while( (o = EnumObj.GetNext()) )
			{
				if (IsActiveUnit(o) && o != atacker)
				{
					if (GetDistanceTo256(o, x256, y256) <= mindist[0])
					{
						if (IsHallucination(o))
						{
							dieobj(o);
							continue;
						}
						if (!WeaponCanApplyOnUnit(o, playernr, weapon_id))
							continue;
						AddOverlayAtrImages(o, castmagenr, IMAGEOVERLAY_DEPENDONMAINIMG);
						ApplyCastedMage(o, atacker, castmagenr);
					}
				}
			}
		}
		break;
	case WEFFECT_MINDCONTROL:
		if (destobj)
		{
			if (destobj->modemove == MODEDIE)
				return;
			if (IsActiveUnit(destobj) && destobj != atacker && playernr != destobj->playernr)
			{
				if (IsHallucination(destobj))
				{
					dieobj(destobj);
					break;
				}
				if (WeaponCanApplyOnUnit(destobj, playernr, weapon_id))
				{
					if (mageprop[castmagenr].sound_id[SOUNDONHIT])
						Play_sfxdata(GetOBJx(destobj), GetOBJy(destobj), mageprop[castmagenr].sound_id[SOUNDONHIT], 2);
					AddOverlayAtrImages(destobj, castmagenr, IMAGEOVERLAY_NOTDEPENDONMAINIMG);
					MakeMindControl(destobj, playernr, PLAYER[playernr].colorRACE);
					if (atacker)
						SetUnitShield(atacker, 0);
				}
			}
		}
		break;
	case WEFFECT_DISRUPTIONWEB:
		o = createobjfulllife(x256 >> 8, y256 >> 8, SC_MISC_DISRUPTIONWEB, GREYNEUTRALCOLORPLAYER);
		o->timeleft = mageprop[castmagenr].timemageactive;
		break;
	case WEFFECT_DARKSWARM:
		o = createobjfulllife(x256 >> 8, y256 >> 8, SC_MISC_DARKSWARM, GREYNEUTRALCOLORPLAYER);
		o->timeleft = mageprop[castmagenr].timemageactive;
		break;
	case WEFFECT_CONSUME:
		//have iscript sound
		if (destobj && atacker)
		{
			if (destobj->modemove == MODEDIE)
				return;
			if (IsActiveUnit(destobj) && destobj != atacker)
			{
				if (IsHallucination(destobj))
				{
					dieobj(destobj);
					break;
				}
				if (WeaponCanApplyOnUnit(destobj, playernr, weapon_id))
				{
					dieobj(destobj);
					IncrMana(atacker, 50);
				}
			}
		}
		break;
	case WEFFECT_PLAGUE:
		//have iscript sound
		if (castmagenr != -1)
			mindist[0] = mageprop[castmagenr].diapazone;
		else
			mindist[0] = alldattbl.weapons_dat->InnerSplash[weapon_id] * 256;
		{
			Enumerate<OBJ *> EnumObj(&MaxObjects, objects);
			while( (o = EnumObj.GetNext()) )
			{
				if (IsActiveUnit(o) && o != atacker)
				{
					if (GetDistanceTo256(o, x256, y256) <= mindist[0])
					{
						if (IsHallucination(o))
						{
							dieobj(o);
							continue;
						}
						if (!WeaponCanApplyOnUnit(o, playernr, weapon_id))
							continue;
						AddOverlayAtrImages(o, castmagenr, IMAGEOVERLAY_DEPENDONMAINIMG);
						ApplyCastedMage(o, atacker, castmagenr);
					}
				}
			}
		}
		break;
	case WEFFECT_BROODLINGS:
		//if queen is have hitpoints
		if (atacker && atacker->health && destobj)
		{
			if (destobj->modemove == MODEDIE)
				return;
			if (IsHallucination(destobj))
			{
				dieobj(destobj);
				break;
			}
			if (WeaponCanApplyOnUnit(destobj, playernr, weapon_id))
			{
				if (mageprop[castmagenr].sound_id[SOUNDONHIT])
					Play_sfxdata(GetOBJx(destobj), GetOBJy(destobj), mageprop[castmagenr].sound_id[SOUNDONHIT], 2);
				createobjfulllife(GetOBJx(destobj) - 10, GetOBJy(destobj) - 1, SC_BROODLINGOBJ, playernr);
				createobjfulllife(GetOBJx(destobj) + 10, GetOBJy(destobj) - 1, SC_BROODLINGOBJ, playernr);
				dieobj(destobj);
			}
		}
		break;
	case WEFFECT_ENSNARE:
		//show ensnare cloud
		//have iscript sound

		newimg = new MAIN_IMG(IMAGEID_ENSNARECLOUD, x256, y256, atacker->mainimage->elevationlevel + 1, 0, 0, 0, 0,
			SC_IMAGE_FLAG_AIRIMG, ISCRIPTNR_INIT);
		newimg->whocreate = SC_IMAGE_SELF_CREATOR;
		mainimageslist.Add(newimg);

		if (castmagenr != -1)
			mindist[0] = mageprop[castmagenr].diapazone;
		else
			mindist[0] = alldattbl.weapons_dat->InnerSplash[weapon_id] * 256;
		{
			Enumerate<OBJ *> EnumObj(&MaxObjects, objects);
			while( (o = EnumObj.GetNext()) )
			{
				if (IsActiveUnit(o) && o != atacker)
				{
					if (GetDistanceTo256(o, x256, y256) <= mindist[0])
					{
						if (IsHallucination(o))
						{
							dieobj(o);
							continue;
						}
						if (!WeaponCanApplyOnUnit(o, playernr, weapon_id))
							continue;
						AddOverlayAtrImages(o, castmagenr, IMAGEOVERLAY_DEPENDONMAINIMG);
						ApplyCastedMage(o, atacker, castmagenr);
					}
				}
			}
		}
		break;
	case WEFFECT_NUCLEARMISSILE:
		//damage all obj in the area of effect(except bunker's units)
		mindist[0] = alldattbl.weapons_dat->InnerSplash[weapon_id] * 256;
		mindist[1] = alldattbl.weapons_dat->MediumSplash[weapon_id] * 256;
		mindist[2] = alldattbl.weapons_dat->OuterSplash[weapon_id] * 256;
		damage = GetWeaponDamage(atacker->SC_Unit, playernr, weapon_id) << 8;
		//correct nuke explotion position
		x256 = GetOBJx256(atacker);
		y256 = GetOBJy256(atacker);
		{
			Enumerate<OBJ *> EnumObj(&MaxObjects, objects);
			while( (o = EnumObj.GetNext()) )
			{
				if (IsActiveUnit(o))
				{
					//distance256 = GetDistanceTo256(o,x256,y256);
					distance256 = GetDistances(GetOBJx256(o), GetOBJy256(o), x256, y256);
					if (distance256 <= mindist[0])
						realdamage = damage;
					else
						if (distance256 <= mindist[1])
							realdamage = damage / 2;
						else
							if (distance256 <= mindist[2])
								realdamage = damage / 4;
							else
								continue;//out of damage range
					if (!WeaponCanApplyOnUnit(o, playernr, weapon_id))
						continue;
					if (GetUnitMaxHitPoints(o->SC_Unit) * 2 / 3 > realdamage)				//damage or 2/3 of unit's life+shield, depend who bigger
						realdamage = GetUnitMaxHealth(o->SC_Unit) * 2 / 3;
					LowLevelDamage(atacker, o, weapon_id,
						alldattbl.weapons_dat->WeaponType[weapon_id],
						realdamage, 0, 0);
				}
			}
			dieobj_silently(atacker);
		}
		break;
	case WEFFECT_YAMATOGUN:
		if (destobj)
		{
			if (destobj->modemove == MODEDIE)
				return;
			if (IsActiveUnit(destobj))
			{
				if (IsHallucination(destobj))
				{
					dieobj(destobj);
					break;
				}
				if (WeaponCanApplyOnUnit(destobj, playernr, weapon_id))
					LowLevelDamage(atacker, destobj,
						weapon_id,
						alldattbl.weapons_dat->WeaponType[weapon_id],
						GetWeaponDamage(SC_Unit, playernr, weapon_id) << 8,
						0, 0);
			}
		}
		break;
	case WEFFECT_RESTORATION:
		if (destobj)
		{
			if (destobj->modemove == MODEDIE)
				return;
			if (IsActiveUnit(destobj) && destobj != atacker)
			{
				if (IsHallucination(destobj))
				{
					dieobj(destobj);
					break;
				}
				if (WeaponCanApplyOnUnit(destobj, playernr, weapon_id))
				{
					if (mageprop[castmagenr].sound_id[SOUNDONHIT])
						Play_sfxdata(GetOBJx(destobj), GetOBJy(destobj), mageprop[castmagenr].sound_id[SOUNDONHIT], 2);
					restoreatribute(destobj);
					AddOverlayAtrImages(destobj, castmagenr, IMAGEOVERLAY_NOTDEPENDONMAINIMG);
				}
			}
		}
		break;
	case WEFFECT_STASISFIELD:
		//have iscript sound
		newimg = new MAIN_IMG(IMAGEID_STASISFIELDHIT, x256, y256, atacker->mainimage->elevationlevel + 1, 0, 0, 0, 0,
			SC_IMAGE_FLAG_AIRIMG, ISCRIPTNR_INIT);
		newimg->whocreate = SC_IMAGE_SELF_CREATOR;
		mainimageslist.Add(newimg);

		if (castmagenr != -1)
			mindist[0] = mageprop[castmagenr].diapazone;
		else
			mindist[0] = alldattbl.weapons_dat->InnerSplash[weapon_id] * 256;
		{
			Enumerate<OBJ *> EnumObj(&MaxObjects, objects);
			while( (o = EnumObj.GetNext()) )
			{
				if (IsActiveUnit(o) && o != atacker)
				{
					if (GetDistanceTo256(o, x256, y256) <= mindist[0])
					{
						if (IsHallucination(o))
						{
							dieobj(o);
							continue;
						}
						if (!WeaponCanApplyOnUnit(o, playernr, weapon_id))
							continue;
						AddOverlayAtrImages(o, castmagenr, IMAGEOVERLAY_DEPENDONMAINIMG);
						ApplyCastedMage(o, atacker, castmagenr);
					}
				}
			}
		}
		break;
	case WEFFECT_IRRADIATE:
	case WEFFECT_LOCKDOWN:
		if (destobj)
		{
			if (destobj->modemove == MODEDIE)
				return;
			if (IsActiveUnit(destobj) && destobj != atacker)
			{
				if (IsHallucination(destobj))
				{
					dieobj(destobj);
					break;
				}
				if (WeaponCanApplyOnUnit(destobj, playernr, weapon_id))
				{
					if (mageprop[castmagenr].sound_id[SOUNDONHIT])
						Play_sfxdata(GetOBJx(destobj), GetOBJy(destobj), mageprop[castmagenr].sound_id[SOUNDONHIT], 2);
					AddOverlayAtrImages(destobj, castmagenr, IMAGEOVERLAY_DEPENDONMAINIMG);
					ApplyCastedMage(destobj, atacker, castmagenr);
				}
			}
		}
		break;
	case WEFFECT_OPTICALFLARE:
	case WEFFECT_PARASITE:
		if (destobj)
		{
			if (destobj->modemove == MODEDIE)
				return;
			if (IsActiveUnit(destobj) && destobj != atacker)
			{
				if (IsHallucination(destobj))
				{
					dieobj(destobj);
					break;
				}
				if (WeaponCanApplyOnUnit(destobj, playernr, weapon_id))
				{
					if (mageprop[castmagenr].sound_id[SOUNDONHIT])
						Play_sfxdata(GetOBJx(destobj), GetOBJy(destobj), mageprop[castmagenr].sound_id[SOUNDONHIT], 2);
					AddOverlayAtrImages(destobj, castmagenr, IMAGEOVERLAY_NOTDEPENDONMAINIMG);
					ApplyCastedMage(destobj, atacker, castmagenr);
				}
			}
		}
		break;
	case WEFFECT_EMPSHOCKWAVE:
		if (castmagenr != -1)
			mindist[0] = mageprop[castmagenr].diapazone;
		else
			mindist[0] = alldattbl.weapons_dat->InnerSplash[weapon_id] * 256;
		{
			Enumerate<OBJ *> EnumObj(&MaxObjects, objects);
			while( (o = EnumObj.GetNext()) )
			{
				if (IsActiveUnit(o) && o != atacker)
				{
					if (GetDistanceTo256(o, x256, y256) <= mindist[0])
					{
						if (IsHallucination(o))
						{
							dieobj(o);
							continue;
						}
						if (!WeaponCanApplyOnUnit(o, playernr, weapon_id))
							continue;
						SetUnitShield(o, 0);
						SetUnitMana(o, 0);
					}
				}
			}
		}
		break;
	case WEFFECT_NORMALHIT:
		if (destobj)
		{
			if (destobj->modemove == MODEDIE)
				return;
			realdamage = GetWeaponDamage(SC_Unit, playernr, weapon_id) << 8;
			realdamage = realdamage * BounceDamage[targetnr] / 256;
			LowLevelDamage(atacker, destobj, weapon_id,
				alldattbl.weapons_dat->WeaponType[weapon_id],
				realdamage, 0, 0);
		}
		break;
	case WEFFECT_SPLASHENEMY:
	case WEFFECT_SPLASHENEMYAIR:
		if (atacker && IsHallucination(atacker))
			break;
		mindist[0] = alldattbl.weapons_dat->InnerSplash[weapon_id] * 256;
		mindist[1] = alldattbl.weapons_dat->MediumSplash[weapon_id] * 256;
		mindist[2] = alldattbl.weapons_dat->OuterSplash[weapon_id] * 256;
		damage = GetWeaponDamage(SC_Unit, playernr, weapon_id) << 8;
		{
			Enumerate<OBJ *> EnumObj(&MaxObjects, objects);
			while( (o = EnumObj.GetNext()) )
			{
				if (IsActiveUnit(o) && (player_aliance(playernr, o->playernr) != MYOBJ || o == destobj))
				{
					distance256 = GetDistanceTo256(o, x256, y256);
					if (distance256 <= mindist[0])
						realdamage = damage;
					else
						if (distance256 <= mindist[1])
							realdamage = damage / 2;
						else
							if (distance256 <= mindist[2])
								realdamage = damage / 4;
							else
								continue;//out of damage range
					if (!WeaponCanApplyOnUnit(o, playernr, weapon_id))
						continue;
					if (listdamaged)
					{
						if (listdamaged->HaveElem(o))
							continue;
						listdamaged->AddList(o);
					}
					LowLevelDamage(atacker, o, weapon_id,
						alldattbl.weapons_dat->WeaponType[weapon_id],
						realdamage, 0, 0);
				}
			}
		}
		break;
	case WEFFECT_SPLASHALL:
		if (atacker && IsHallucination(atacker))
			break;
		mindist[0] = alldattbl.weapons_dat->InnerSplash[weapon_id] * 256;
		mindist[1] = alldattbl.weapons_dat->MediumSplash[weapon_id] * 256;
		mindist[2] = alldattbl.weapons_dat->OuterSplash[weapon_id] * 256;
		damage = GetWeaponDamage(SC_Unit, playernr, weapon_id) << 8;
		{
			Enumerate<OBJ *> EnumObj(&MaxObjects, objects);
			while( (o = EnumObj.GetNext()) )
			{
				if (IsActiveUnit(o))
				{
					distance256 = GetDistanceTo256(o, x256, y256);
					if (distance256 < mindist[0])
						realdamage = damage;
					else
						if (distance256 < mindist[1])
							realdamage = damage / 2;
						else
							if (distance256 < mindist[2])
								realdamage = damage / 4;
							else
								continue;//out of damage range
					if (!WeaponCanApplyOnUnit(o, playernr, weapon_id))
						continue;
					if (listdamaged)
					{
						if (listdamaged->HaveElem(o))
							continue;
						listdamaged->AddList(o);
					}
					LowLevelDamage(atacker, o, weapon_id,
						alldattbl.weapons_dat->WeaponType[weapon_id],
						realdamage, 0, 0);
				}
			}
		}
		break;
	case WEFFECT_CORROSIVEACID:
		if (!destobj)
			break;
		if (atacker && IsHallucination(atacker))
			break;
		realdamage = GetWeaponDamage(SC_Unit, playernr, weapon_id) << 8;
		realdamage = realdamage * BounceDamage[targetnr] / 256;
		LowLevelDamage(atacker, destobj, weapon_id,
			alldattbl.weapons_dat->WeaponType[weapon_id],
			realdamage, 0, 0);
		{
			Enumerate<OBJ *> EnumObj(&MaxObjects, objects);
			while( (o = EnumObj.GetNext()) )
			{
				if (IsOnSkyOBJ(o) && ((player_aliance(playernr, o->playernr) == ENEMYOBJ) || o == destobj))
				{
					if (WeaponCanApplyOnUnit(o, playernr, weapon_id))
					{
						if (IsHallucination(o))
						{
							dieobj(o);
							continue;
						}
						if (GetDistanceTo256(o, x256, y256) <= CORROSIVEACIDSPLASHRADIUS)
						{
							prevsporecnt = GetCorrosiveAcidValue(o);
							corrvalue = GetMageAtr(&o->atrobj, ATRCORROSIVEACID) + CORROSIVEACIDINCREMENTTICKS;
							if (o == destobj)
								corrvalue += CORROSIVEACIDINCREMENTTICKS;
							if (corrvalue > CORROSIVEACIDMAXATRVAL)
								corrvalue = CORROSIVEACIDMAXATRVAL;
							SetMageAtr(&o->atrobj, ATRCORROSIVEACID, corrvalue);
							if (!prevsporecnt)
								InitSporeImage(o, corrvalue);
							else
								ChangeSporeImage(o, prevsporecnt, GetCorrosiveAcidValue(o));
						}
					}
				}
			}
		}
		break;
	default:
		DEBUGMESSCR("damage not developed\n");
		break;
	}
}
//===========================================
void AddOverlayAtrImages(OBJ *destobj, int castmagenr, int independentofmain)
{
	int canshowatrimage;
	signed char atrnr;
	atrnr = mageprop[castmagenr].atronobj;
	if (atrnr != -1 && GetMageAtr(&destobj->atrobj, atrnr))		//if no have the same mage(images overlay) on unit
		canshowatrimage = 0;
	else
		canshowatrimage = 1;
	if (canshowatrimage)
	{
		AddImagesForAtribute(destobj, atrnr, mageprop[castmagenr].images_id[GetUnitOverlaySize(destobj->SC_Unit)],
			independentofmain);
	}
}
//===========================================
int GetDeltaWeaponElevationLevel(unsigned char weapon_id)
{
	switch (alldattbl.weapons_dat->WeaponEffect[weapon_id])
	{
	case WEFFECT_NONE:
	case WEFFECT_MAELSTROM:
	case WEFFECT_MINDCONTROL:
	case WEFFECT_DISRUPTIONWEB:
	case WEFFECT_DARKSWARM:
	case WEFFECT_CONSUME:
	case WEFFECT_PLAGUE:
	case WEFFECT_NUCLEARMISSILE:
	case WEFFECT_RESTORATION:
	case WEFFECT_STASISFIELD:
	case WEFFECT_IRRADIATE:
	case WEFFECT_LOCKDOWN:
	case WEFFECT_OPTICALFLARE:
	case WEFFECT_NORMALHIT:
	case WEFFECT_SPLASHALL:
	case WEFFECT_SPLASHENEMY:
	case WEFFECT_SPLASHENEMYAIR:
	case WEFFECT_CORROSIVEACID:
		return(+1);
	case WEFFECT_BROODLINGS:
	case WEFFECT_ENSNARE:
	case WEFFECT_YAMATOGUN:
	case WEFFECT_PARASITE:
	case WEFFECT_EMPSHOCKWAVE:
		return(-1);
	default:
		return(0);
	}
}
//===========================================
int WeaponCanApplyOnUnit(OBJ *a, int playernr, unsigned char weapon_id)
{
	unsigned short targetflags = alldattbl.weapons_dat->TargetFlags[weapon_id];
	SCUNIT SC_Unit = a->SC_Unit;
	if ((targetflags & WTF_NONBUILD) && IsBuild(SC_Unit))
		return(0);
	if ((targetflags & WTF_NONROBOTIC) && IsRobotic(SC_Unit))
		return(0);
	if ((targetflags & WTF_MECHANICALORGANIC) && !(IsMechanical(SC_Unit) | IsOrganic(SC_Unit)))
		return(0);
	if ((targetflags & WTF_MECHANICAL) && !IsMechanical(SC_Unit))
		return(0);
	if ((targetflags & WTF_ORGANIC) && !IsOrganic(SC_Unit))
		return(0);
	if ((targetflags & WTF_OWN) && ( a->playernr != playernr || !IsGroupMenFlag(SC_Unit)) )
		return(0);
	if ((targetflags & WTF_STARCLONE_ONSPELLCASTER) && !IsSpellCaster(SC_Unit))
		return(0);
	if ((targetflags & WTF_AIR) && IsAirUnit(SC_Unit))
		return(1);
	if ((targetflags & WTF_GROUND) && !IsAirUnit(SC_Unit))
		return(1);
	return(0);
}
//===========================================
