#include <grplib/usegrp.h>
#include <stdlib.h>
#include <math.h>

#include "unitaction.h"

#include "vars.h"
#include "man.h"
#include "putobj.h"
#include "diap.h"
#include "load.h"
#include "objs.h"
#include "gener.h"
#include "wmem.h"
#include "gr.h"
#include "lists.h"
#include "audio.h"
#include "objinfo.h"
#include "action.h"
#include "market.h"
#include "maps.h"
#include "overlay.h"
#include "flingy.h"
#include "pylon.h"
#include "images.h"
#include "weapons.h"
#include "sigorder.h"
#include "mage.h"
//=======================================
int GetMageAtr(ATROBJ *a,int atrnr)
{
    return(a->mageatribute[atrnr]);
}
//==============================================
void SetMageAtr(ATROBJ *a,int atrnr,int value)
{
    a->mageatribute[atrnr]=value;
}
//==============================================
int DeltaMageAtr(ATROBJ *a,int atrnr,int value)
{
    a->mageatribute[atrnr]+=value;
    return(a->mageatribute[atrnr]);
}
//==============================================
void mageattributedothings(OBJ *a)
{
    OBJ *a2;
    int x256,y256;
    int typemage,i,j,temp,damage,tickdamage,upgnr,upgrdamage;
    for (i=0;i<MAXMAGEATR;i++)
    {
	if (GetMageAtr(&a->atrobj,i)>0)
	{
	    typemage = magenrfrommageatr[i];
	    switch(i)
	    {
		case ATRPLAGUE:
		    if (ifcanworkatr_onme(a,i,typemage))
		    {
			LowLevelDamage(NULL,a,WEAPONID_PLAGUE,DAMAGE_IGNOREARMOR,plaguedamagepertick,0,0);
		    }
		    break;
		case ATRIRRADIATE:
		    //decrement health arround units if not burrowed
		    x256 = GetOBJx256(a);
		    y256 = GetOBJy256(a);
		    if (ifcanworkatr_onothers(a,i,typemage))
		    {
			if (a->in_transport)
			{
			    if (a->in_transport->loaded)
			    for (j=0;j<a->in_transport->loaded->nrofloadedunits;j++)
			    {
				a2=a->in_transport->loaded->loadedunits[j];
				if (a!=a2)
				{
				    if (IsOrganic(a2->SC_Unit))
					LowLevelDamage(NULL,a2,WEAPONID_IRRADIATE,DAMAGE_IGNOREARMOR,irradiatedamagepertick,0,0);
				}
			    }
			}    
			else
			{
			    if (!IsOBJBurrowed(a))
			    {
				for (j=0;j<MaxObjects;j++)
				{
				    a2=objects[j];
				    if ( a2 != a)
				    {
					if (IsOrganic(a2->SC_Unit) && !IsOBJBurrowed(a2))
					{
					    if (GetDistanceTo256(a2,x256,y256) <= mageprop[typemage].diapazone)
					    {
						LowLevelDamage(NULL,a2,WEAPONID_IRRADIATE,DAMAGE_IGNOREARMOR,irradiatedamagepertick,0,0);
					    }
					}
				    }
				}
			    }
			}
		    }
		    //decrementlife
		    if (ifcanworkatr_onme(a,i,typemage))
			LowLevelDamage(NULL,a,WEAPONID_IRRADIATE,DAMAGE_IGNOREARMOR,irradiatedamagepertick,0,0);
		    break;
	    }
	}
    }
}
//=======================================
void mageeffect(int typemage,struct OBJ *a,struct OBJ *whoobjputmage)
{
    int upgnr,upgdamage,totaldamage,mana;
    OBJ *temp1,*temp2;
    if (GetMageAtr(&a->atrobj,ATRSTASIS))
	return;
    switch(typemage)
    {

        case MODEMAELSTROM:
	case MODELOCKDOWN:
	    //need to stop construct
	    if (a->SC_Unit == SC_SCVOBJ)
		moveobj(a,NULL,MODEHALTCONSTRUCT,NOSHOWERROR);
	    moveobj(a,NULL,MODESTOP,NOSHOWERROR);
    	    a->prop |= VARNOTWORK;
	    break;
	case MODESTASISFIELD:
    	    a->prop |= (VARNOTWORK | VARNOTHERE);
	    break;
	default:
	    break;
    }
}
//=======================================
void restoreatribute(struct OBJ *a)
{
    if (GetMageAtr(&a->atrobj,ATRSTASIS)==0)
    {
	for (int i=0;i<MAXMAGEATR;i++)
	    switch(i)
	    {
		case ATRPARASITEFROM:
		    SetMageAtr(&a->atrobj,i,0);
		    break;
		case ATRBLIND:
		case ATRENSNARE:
//		case ATRMATRIX:
		case ATRLOCKDOWN:
		case ATRMAELSTROM:
		case ATRIRRADIATE:
		case ATRPLAGUE:
		case ATRCORROSIVEACID:
		    SetMageAtr(&a->atrobj,i,1);
		    break;
		default:
		    break;
	    }
    }
}
//=======================================
void ApplyCastedMage(OBJ *a,OBJ *casterobj,int typemage)
{
    char *adratr=(char *)&a->atrobj;
    mageeffect(typemage,a,casterobj);	//some action on unit who cast the mage and on destination unit.
    if (mageprop[typemage].atronobj>=0)
    {
	switch(typemage)
	{
	    case MODEPARASITE:
		addmage(a,ATRPARASITEFROM,( *(adratr) | (1<<(casterobj->playernr)) ));
		break;
	    case MODERECALL:
		a->atrobj.arbiterrecallobj = casterobj;
		a->atrobj.x256 = casterobj->finalx;
		a->atrobj.y256 = casterobj->finaly;
		a->atrobj.flags = 1;
		addmage(a,mageprop[typemage].atronobj,mageprop[typemage].timemageactive);
		break;
	    default:
		addmage(a,mageprop[typemage].atronobj,mageprop[typemage].timemageactive<<8);
	}
    }
}
//=======================================
int accessatronunit(struct OBJ *myobj,struct OBJ *destobj,unsigned char weapon_id)
{
    if (weapon_id == 255)
	return(1);
    if (!destobj)
	return 0;//deny
    return(WeaponCanApplyOnUnit(destobj,myobj->playernr,weapon_id));
}
//=======================================
int accesstomage(struct OBJ *a,struct OBJ *destobj,int mode)
{
    int castonterrain = 0;
    if (mageprop[mode].atr & ORDER_RIGHTWAY)
	return(1);
    if (mageprop[mode].atr & ORDER_IGNOREUNITDESTINATION)
	destobj = NULL;
    unsigned char weapon_id = mageprop[mode].createweapon;
    if (weapon_id != 255)
    {
	if (alldattbl.weapons_dat->TargetFlags[weapon_id] & WTF_ONTERRAIN)
	    castonterrain = 1;
    }
    else
	if (mageprop[mode].atr & ORDER_ONTERRAIN)
	    castonterrain = 1;
    if (destobj)
    {
	if (GetMageAtr(&destobj->atrobj,ATRSTASIS))
	    return 0;
	return (accessatronunit(a,destobj,weapon_id));
    }
    else
    {
	if (castonterrain)
	    return(1);
    }
    return(0);
}
//=======================================
void addmage(struct OBJ *a,int nmage,int val1)
{
    if (GetMageAtr(&a->atrobj,ATRSTASIS)==0)
    {
	SetMageAtr(&a->atrobj,nmage,val1);
	switch(nmage)
	{
	    case ATRMATRIX:
		a->dmatrix = ADDMATRIXHP;
		break;
	    case ATRSTASIS:
	    case ATRMAELSTROM:
		if (a->SC_Unit == SC_GHOSTOBJ)
		{
		    GhostNUKECancel(a,GHOSTCANCELNUKE);
		}
		break;
	}
    }
}
//=======================================
void DecrementOBJAtr(struct OBJ *a)
{
    OBJ *arbiterrecallobj;
    //ignore parasite atribute, lets begin from 1
    int prevsporecnt,newx256,newy256;
    for (int i=1;i<MAXMAGEATR;i++)
    {
	if ( (i == ATRSTASIS) || (GetMageAtr(&a->atrobj,ATRSTASIS) == 0) )
	{
    	    if ( GetMageAtr(&a->atrobj,i) < TERMINATEATRMAGE)
        	if (GetMageAtr(&a->atrobj,i) > 0)
        	{
		    if (i == ATRCORROSIVEACID)
		    {
			prevsporecnt = GetCorrosiveAcidValue(a);
            	    }
            	    if (DeltaMageAtr(&a->atrobj,i,-DELTAMANAONCYCLE)<=0)
		    {
			switch(i)
			{
    			    case ATRMAELSTROM:
			    case ATRLOCKDOWN:
            			SetMageAtr(&a->atrobj,i,0);
            			a->prop &= ~VARNOTWORK;
				break;
			    case ATRCORROSIVEACID:
            			SetMageAtr(&a->atrobj,i,0);
				DelImagesForAtribute(a,ATRCORROSIVEACID);
				break;
			    case ATRSTASIS:
            			SetMageAtr(&a->atrobj,i,0);
    				a->prop &= ~(VARNOTWORK | VARNOTHERE);
    				moveobj(a,NULL,MODESTOP,NOSHOWERROR);
				break;
			    case ATRMATRIX:
            			SetMageAtr(&a->atrobj,i,0);
				a->dmatrix=0;
				break;
			    case ATRRECALL:
				//recall to destination
				arbiterrecallobj = a->atrobj.arbiterrecallobj;
				if (a->atrobj.flags)
				{
				    if (arbiterrecallobj)	//arbiter with health
				    {
					newx256 = GetOBJx256(arbiterrecallobj) - a->atrobj.x256;
					newy256 = GetOBJy256(arbiterrecallobj) - a->atrobj.y256;
					if (CheckCoordinates(GetOBJx256(a)+newx256,GetOBJy256(a)+newy256,a->SC_Unit))
					    a->mainimage->SetDeltaCoords(a,newx256,newy256);
				    }
				    moveobj(a,NULL,MODESTOP,NOSHOWERROR);
				    a->atrobj.flags = 0;
            			    SetMageAtr(&a->atrobj,i,17 * DELTAMANAONCYCLE);
            			    //continue show recall overlays
            			    continue;
				}
				break;
			    default:
            			SetMageAtr(&a->atrobj,i,0);
				break;
			}
			DelImagesForAtribute(a,i);
		    }
		    else
		    {
			if (i == ATRCORROSIVEACID)
			{
			    ChangeSporeImage(a,prevsporecnt,GetCorrosiveAcidValue(a));
			}
		    }
        	}
	}
    }
    if (IsSpellCaster(a->SC_Unit) && !IsOBJUnderConstruct(a))
    {
	if (GetMageAtr(&a->atrobj,ATRSTASIS) == 0)
	{
    	    if ((a->prop & VARDECRMAGE) && GetMageAtr(&a->atrobj,ATRINVISIBLE) != WRAITHEPODARBITEROM)
    	    {
    		if (a->mana > 0)//decrement mage
        	{
		    DecrManaBITS(a,DELTAMANAONCYCLE);
    		}
		else
    		{
    		    addmage(a,ATRINVISIBLE,0);
    		    a->prop &= ~VARDECRMAGE;
		    ChangeTypeOfProp(a,PROPNORMAL1);
		}
	    }
	    else
	    {
		IncrManaBITS(a,DELTAMANAONCYCLE);
	    }
	}
    }
}
//==================================
//return nr to resource geted
int decrresourceobj(struct OBJ *a,int minus)
{
    int resret=0;
    switch(a->SC_Unit)
    {
	case SC_MINERAL1OBJ:
	case SC_MINERAL2OBJ:
	case SC_MINERAL3OBJ:	
	    if (minus >= a->data.resource.resource_count)
	    {
		resret = a->data.resource.resource_count;
		dieobj(a);
	    }
	    else
	    {
		a->data.resource.resource_count -= minus;
		resret = minus;
		ChangeMineralSprite(a);
	    }
	    break;
	case SC_EXTRACTOROBJ:
	case SC_ASSIMILATOROBJ:
	case SC_REFINERYOBJ:
	    if (minus >= a->data.resource.resource_count)
	    {
		//then message appear at first time reach to 0
		if (a->data.resource.resource_count != 0)
		{
		    Play_sfxdata_id(a,SFXDATA_GASDEPLETED,2,0);
	    	    infobar.addbarmessage(alldattbl.stattxt_tbl->get_TBL_STR(STATTXT_TBL_GEYSERDEPLETEDMES),
	    			    IDFONT10,GWHITECOLORFONT,INFO_DEFAULTTIME,BF_MALIGN);
		}
		a->data.resource.resource_count = 0;
		a->prop |= VARNOTSEEPROP;
		resret = DEPLEATEDGETGAZ;
	    }
	    else
	    {
		a->data.resource.resource_count -= minus;
		resret = minus;
	    }
	    break;
    }
    return resret;
}
//==================================
//                           0 1 2 3 4 5 6 7 8 9 a b c d e
char decloakatr[MAXMAGEATR]={1,1,1,0,0,1,1,0,0,1,1,1,1,1,0};
char  drawedatr[MAXMAGEATR]={0,0,1,0,0,1,1,0,0,1,1,1,1,1,1};
//==============================================
int existparasitebit(struct OBJ *a,int playernr)
{
    if (GetMageAtr(&a->atrobj,ATRPARASITEFROM) && (1<<playernr))
	return(1);
    return(0);
}
//==============================================
int existatrdecloak(struct OBJ *a)
{
    for (int i=0;i<MAXMAGEATR;i++)
    {
	if (GetMageAtr(&a->atrobj,i) > 0)
	    if (decloakatr[i])
	        return(1);
    }
    return(0);
}
//==============================================
void MakeMindControl(OBJ *a,int playernr,int ncolor)
{
    OBJ *a2;
    if (IfHaveSubUnit(a->SC_Unit))
	MakeMindControl(a->subunit,playernr,ncolor);
    if (a->SC_Unit==SC_PYLONOBJ)
	DelPylonArea(&map,a,a->playernr);
    else
    	CheckBuildForPower(&map,a,a->playernr);
    ChangeSupply(a->playernr,a->SC_Unit,MINUSFACTOR);
    ChangeMaxSupply(a->playernr,a->SC_Unit,MINUSFACTOR);
    a->playernr = playernr;
    a->color = ncolor;
    a->mainimage->imageusercolor = ncolor;
    if (a->myparent)			//now i have no parent
	delchild(a->myparent,a);	//
    if (a->childs)			//the childs so is my
    {
	for (int i=0;i<a->childs->nrofchildunits;i++)
	{
	    a2 = a->childs->parentof[i];
	    if (a2)
	    {
		//remove interceptor from parent if it is in base
		if (a2->SC_Unit == SC_INTERCEPTOROBJ && !(a2->prop & VARINBASE))
		{
		    delchild(a,a2);		
		}
		else
		{
		    if (a2->SC_Unit == SC_NUKEOBJ)
			PLAYER[a2->playernr].nukes--;
		    ChangeSupply(a->playernr,a2->SC_Unit,MINUSFACTOR);
		    a2->playernr = playernr;
	    	    a2->color = ncolor;
		    a2->mainimage->imageusercolor = ncolor;
		    ChangeSupply(a->playernr,a2->SC_Unit,PLUSFACTOR);
		    if (a2->SC_Unit == SC_NUKEOBJ)
			PLAYER[a2->playernr].nukes++;
		}
	    }
	}
    }
    if (a->loaded)			//the transportunits
    {
	for (int i=0;i<a->loaded->nrofloadedunits;i++)
	{
	    if (a->loaded->loadedunits[i])
	    {
		ChangeSupply(a->loaded->loadedunits[i]->playernr,a->loaded->loadedunits[i]->SC_Unit,MINUSFACTOR);
		a->loaded->loadedunits[i]->playernr = playernr;
		a->loaded->loadedunits[i]->color = ncolor;
		ChangeSupply(a->loaded->loadedunits[i]->playernr,a->loaded->loadedunits[i]->SC_Unit,PLUSFACTOR);
	    }
	}
    }
    ChangeSupply(a->playernr,a->SC_Unit,PLUSFACTOR);
    ChangeMaxSupply(a->playernr,a->SC_Unit,PLUSFACTOR);
    if (a->SC_Unit == SC_PYLONOBJ)
	AddPylonArea(&map,a,a->playernr);
    else
    	CheckBuildForPower(&map,a,a->playernr);
    if (a->doubleunit)
    {
	if (a->doubleunit->playernr!=playernr)
	    MakeMindControl(a->doubleunit,playernr,ncolor);
    }
//    makeoneobjseeopen(a,loadobj(a->SC_Unit));

//    moveobj(a,NULL,MODESTOP,NOSHOWERROR);
}
//==============================================
//return 1 if atribute can work on me
//==============================================
int ifcanworkatr_onothers(OBJ *a,int atr,int typemage)
{
    //if stasis do not do atribute things
    if (GetMageAtr(&a->atrobj,ATRSTASIS) > 0)
    	return(0);
    switch(atr)
    {
	case ATRIRRADIATE:
	    if (IsOBJBurrowed(a))
		return(0);
	    break;
    }
    return(1);
}
//==============================================
int ifcanworkatr_onme(OBJ *a,int atr,int typemage)
{
    //if stasis do not damage the unit
    switch(atr)
    {
	case ATRPLAGUE:
	    if ( GetMageAtr(&a->atrobj,ATRSTASIS) > 0)
		return(0);
	    break;
	case ATRIRRADIATE:
	    if (!IsOrganic(a->SC_Unit))
		return(0);
	    if (GetMageAtr(&a->atrobj,ATRSTASIS) > 0)
	        return(0);
	    break;
    }
    return(1);
}
//==============================================
void CastSpell(OBJ *casterobj)
{
    int needmana,finalx,finaly;
/*    needmana = getusemanaformodemove(casterobj->castmagenr);
    if (CheckForMana(casterobj,needmana)!=CHECKRES_OK)
    {
	playinfoadvisorsound(casterobj->playernr,GetUnitRace(casterobj->SC_Unit),ADVENERGY,PLAYADVISOR_TEXTANDSOUND);
	return;
    }
    DecrMana(casterobj,needmana);
*/    if (mageprop[casterobj->castmagenr].createweapon == 255)
    {
	CastSpellWithOutWeaponnr(casterobj,casterobj->castmagenr);
    }
    else
    {
	if (mageprop[casterobj->castmagenr].sound_id[SOUNDONCAST])
	    Play_sfxdata(GetOBJx(casterobj),GetOBJy(casterobj),mageprop[casterobj->castmagenr].sound_id[SOUNDONCAST],2);
	CreateWeaponID(casterobj,casterobj->finalOBJ,casterobj->finalx,casterobj->finaly,mageprop[casterobj->castmagenr].createweapon,1);
    }
}
//=======================================
void CastSpellWithOutWeaponnr(OBJ *casterobj,int castmagenr)
{
    OBJ *a,*destobj;
    int i,mindist,x256,y256;
    int oldsnd,leftsize,rightsize;
    if (mageprop[castmagenr].sound_id[SOUNDONHIT])
	Play_sfxdata(GetOBJx(casterobj),GetOBJy(casterobj),mageprop[castmagenr].sound_id[SOUNDONHIT],2);
    destobj = casterobj->finalOBJ;
    switch(castmagenr)
    {
	case MODEDEFENSIVEMATRIX:
	    if (destobj)
	    {
    		if (IsActiveUnit(destobj) && destobj != casterobj)
		{
		    AddOverlayAtrImages(destobj,castmagenr,IMAGEOVERLAY_DEPENDONMAINIMG);
		    ApplyCastedMage(destobj,casterobj,castmagenr);
		}
	    }
	    break;
	case MODERECALL:
	    mindist = mageprop[castmagenr].diapazone;
	    for (i=0;i<MaxObjects;i++)
	    {
		a = objects[i];
    		if (casterobj->playernr == a->playernr && IsActiveUnit(a) && !IsBuild(a->SC_Unit) && a != casterobj)
		{
		    if ( GetDistanceTo256(a,casterobj->finalx,casterobj->finaly) <= mindist )
		    {
			if (IsHallucination(a))
			{
			    dieobj(a);
			    continue;
			}
			AddOverlayAtrImages(a,castmagenr,IMAGEOVERLAY_NOTDEPENDONMAINIMG);
			ApplyCastedMage(a,casterobj,castmagenr);
		    }
		}
	    }
	    break;
	case MODEINFEST:
	    if (destobj)
	    {
		casterobj->data.queeninfest.actionbit = 1;
		casterobj->data.queeninfest.remaintimetoinfest = mageprop[castmagenr].timemageactive;
		casterobj->prop |= (VARNOTHERE | VARCANTSELECT);
		casterobj->mainimage->DisableExecScript();
		casterobj->mainimage->HideChildsImgFlag();
    		casterobj->mainimage->HideImgFlag();
		doselectedOBJbit(casterobj,casterobj->playernr,0);
		deselectobj(casterobj);
	    }
	    break;
	case MODEFEEDBACK:
	    if (destobj)
	    {
    		if (IsActiveUnit(destobj) && destobj != casterobj)
    		{
		    if (IsHallucination(destobj))
		    {
			dieobj(destobj);
			break;
		    }
    		    if (!IsSpellCaster(destobj->SC_Unit))
    			break;
		    LowLevelDamage(casterobj,destobj,WEAPONID_NUCLEARMISSILE,DAMAGE_INDEPENDENT,destobj->mana,0,0);
		    SetUnitMana(destobj,0);
		    AddOverlayAtrImages(destobj,castmagenr,IMAGEOVERLAY_NOTDEPENDONMAINIMG);
		}
	    }
	    break;
	case MODEHALLUCINATION:
	    if (destobj)
	    {
    		if (IsActiveUnit(destobj) && destobj != casterobj)
    		{
		    oldsnd = BlockSoundToPlay();
		    leftsize = GetUnitDimensions(destobj->SC_Unit,UNITDIM_LEFT);
		    rightsize = GetUnitDimensions(destobj->SC_Unit,UNITDIM_RIGHT);
            	    a = createobjfulllife(GetOBJx(destobj)-leftsize-rightsize,GetOBJy(destobj),destobj->SC_Unit,casterobj->playernr);
	    	    for (i=ATRPARASITEFROM;i<MAXMAGEATR;i++)
            	        SetMageAtr(&a->atrobj,i,0);
            	    SetHallucinationOBJ(a);
            	    a = createobjfulllife(GetOBJx(destobj)+leftsize+rightsize,GetOBJy(destobj),destobj->SC_Unit,casterobj->playernr);
	    	    for (i=ATRPARASITEFROM;i<MAXMAGEATR;i++)
            	        SetMageAtr(&a->atrobj,i,0);
            	    SetHallucinationOBJ(a);
		    RestoreSoundToPlay(oldsnd);
		    AddOverlayAtrImages(destobj,castmagenr,IMAGEOVERLAY_NOTDEPENDONMAINIMG);
		}
	    }
	    break;
    }
}
//=======================================
void calcsomespellinfo(void)
{
    int spellworkingcycles;
    unsigned char weapon_id;
    weapon_id = mageprop[MODEIRRADIATE].createweapon;
    if ( weapon_id != 255)
    {
	spellworkingcycles = (mageprop[MODEIRRADIATE].timemageactive << 8) / DELTAMANAONCYCLE;
	irradiatedamagepertick = (GetWeaponDamage(SC_SCIENCEVESSELOBJ,0,weapon_id) << 8) / spellworkingcycles;
    }
    weapon_id = mageprop[MODEPLAGUE].createweapon;
    if ( weapon_id != 255)
    {
	spellworkingcycles = (mageprop[MODEPLAGUE].timemageactive <<8) / DELTAMANAONCYCLE;
	plaguedamagepertick = (GetWeaponDamage(SC_DEFILEROBJ,0,weapon_id) << 8) / spellworkingcycles;
    }
}
//=======================================
