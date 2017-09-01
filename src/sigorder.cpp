
#include "debug.h"
#include "man.h"
#include "auxil.h"
#include "audio.h"
#include "doodad.h"
#include "actiondefault.h"
#include "objs.h"
#include "creep.h"
#include "images.h"
#include "iscript.h"
#include "load.h"
#include "lists.h"
#include "maps.h"
#include "pylon.h"
#include "diap.h"
#include "action.h"
#include "overlay.h"
#include "market.h"
#include "walk.h"
#include "objinfo.h"
#include "sigorder.h"

//==================================
void AfterConstruct(OBJ *a)
{
    if (a->SC_FromUnit != SC_NOUNITNR && a->SC_FromUnit != SC_DRONEOBJ)
    {
        ChangeSupply(a->playernr,a->SC_FromUnit,MINUSFACTOR);
	ChangeMaxSupply(a->playernr,a->SC_FromUnit,MINUSFACTOR);
    }
    ChangeSupply(a->playernr,a->SC_Unit,PLUSFACTOR);
    ChangeMaxSupply(a->playernr,a->SC_Unit,PLUSFACTOR);
}
//==================================
int SIGOrder_NULL(OBJ *a)
{
    DEBUGMESSCR("NULL order\n");
}
//==================================
void SetOrder(OBJ *a,int signal,int (*func)(OBJ *a))
{
    a->OrderFunc = func;
}
//==================================
int  SIGOrder_ZergBuildFinishConstruct(OBJ *a)
{
    a->prop |= VARREADY;
    ChangeTypeOfProp(a,PROPNORMAL1);
    ChangeUnitAndImagesAssociated(a,a->SC_Unit);
    SetOrder(a,0,NULL);
    Play_sfxdata(GetOBJx(a),GetOBJy(a),SFXDATA_ZERGBUILDWARPEND,2);
    if (a->SC_FromUnit==SC_DRONEOBJ||(a->doubleunit&&a->doubleunit->SC_Unit==SC_NYDUSCANALOBJ))
    {
	FillWithCreepNow(a,loadobj(a->SC_Unit),BEGINCREEPTABLE);
    }
    a->lastdamageoverlays = -1;
    AddRemoveBloodFlameOverlays(a);
    AfterConstruct(a);
    return(1);
}
//==================================
int  SIGOrder_TerranBuildLastConstructSprite(OBJ *a)
{
    OVERLAY_IMG *img;
    //because is lanching from construction and iscript execution for all scripts is done
    //we need to execute iscriot and shadow layer iscript forced twicely
    ChangeUnitAndImagesAssociated(a,a->SC_Unit);

    iscriptinfo.ExecuteScript(a->mainimage);

    a->mainimage->childlists->EnumListInit();
    while(img = (OVERLAY_IMG *) a->mainimage->childlists->GetNextListElem(NULL))
    {
	if (img->flags & SC_IMAGE_FLAG_IMGUNDER)
	    iscriptinfo.ExecuteScript(img);
    }
    return(0);
}
//==================================
int  SIGOrder_TerranBuildFinishConstruct(OBJ *a)
{
    a->prop |= VARREADY;
    FinishMainUnitConstruct(a);
    if (IsAddon(a->SC_Unit))
    {
	AttachAddonToBuild(a);				//send landing cmd(attach connect)
//	SetOBJIScriptNr(a,ISCRIPTNR_BUILT);		//send build complete
    }
    else
    {
	AttachNearestAddon(a);
    }
    if (a->playernr==NUMBGAMER)
    {
	if (IsAddon(a->SC_Unit))
	{
	    playinfoadvisorsound(a->playernr,TERRANRACE,ADVADDONCOMPL,PLAYADVISOR_TEXTANDSOUND);
	}
	else
	{
	    Play_sfxdata(GetOBJx(a),GetOBJy(a),SFXDATA_TERRANBUILDWARPEND,2);
	}
    }
//    a->lastdamageoverlays = -1;
    AddRemoveBloodFlameOverlays(a);
    AfterConstruct(a);
    return(0);
}
//==================================
int  SIGOrder_ProtossBuildShowTexture(OBJ *a)
{
    int x256,y256;
    MAIN_IMG *img;
    if (a->replaceobj)
    {
        a->replaceobj->mainimage->HideChildsImgFlag();
        a->replaceobj->mainimage->HideImgFlag();
    }
    img = a->mainimage;
    x256 = img->xpos;
    y256 = img->ypos;
    img->DeleteMainImgAndChilds();
    //here a->image was deleted and created another one
    CreateImageAndAddToList(a,x256,y256,3,NOLOIMAGE);
    img = a->mainimage;
    img->iscriptid = alldattbl.images_dat->Iscript_ID[IMAGEID_TEXTURE];
    img->offsetcmdinbuf = 0;
    img->SetIScriptNr(ISCRIPTNR_INIT);
    img->iscriptid = alldattbl.images_dat->Iscript_ID[img->imageid];
    Play_sfxdata(GetOBJx(a),GetOBJy(a),SFXDATA_PROTOSSBUILDWARPEND,2);
    SetOrder(a,1,&SIGOrder_ProtossBuildWhiteAppear);
    return(1);
}
//==================================
int  SIGOrder_ProtossBuildWhiteAppear(OBJ *a)
{
    int nrofwarpimg;
    OVERLAY_IMG *whitewarpimg;
    MAIN_IMG *img;
    img = a->mainimage;
    Images_FUNC(img,
		alldattbl.images_dat->Draw_Function[img->imageid],
		alldattbl.images_dat->Remapping[img->imageid],&img->remaptable,&img->grpmethod);
    img->framenr=0;
    img->SetIScriptNr(ISCRIPTNR_BUILT);
    iscriptinfo.ExecuteScript(img);
    img->SetIScriptNr(ISCRIPTNR_WARPIN);
    iscriptinfo.ExecuteScript(img);
    nrofwarpimg = img->childlists->GetMaxElements();
    whitewarpimg = (OVERLAY_IMG *) img->childlists->GetElem(nrofwarpimg-1,NULL);
    if (whitewarpimg)
    {
	whitewarpimg->protossbuildwhite = 0;
    }
    else
    {
	DEBUGMESSCR("bad news about warp white build img\n");
    }
    OBJstruct *b = loadobj(a->SC_Unit);
    SetOrder(a,0,NULL);
    return(1);
}
//==================================
int  SIGOrder_ProtossBuildFinishConstruct(OBJ *a)
{
    a->prop |= VARREADY;
    ChangeTypeOfProp(a,PROPNORMAL1);
//    a->lastdamageoverlays = -1;
    AddRemoveBloodFlameOverlays(a);
    AfterConstruct(a);
    if (a->SC_Unit==SC_PYLONOBJ)
    	AddPylonArea(&map,a,a->playernr);
    else
	CheckBuildForPower(&map,a,a->playernr);
    return(0);
}
//==================================
int SIGOrder_Tank_EndRotationBeforeSiegeMode(OBJ *a)
{
    SetOrder(a,0,NULL);
    //base and turret rotated to special position
    if (a->mainimage->side == TANKSIEGESIDE && a->subunit->mainimage->side == TANKSIEGESIDE)
    {
	if (IsSubUnit(a->SC_Unit))
	    ApplyNextModeMove(a->subunit);
	else
	    ApplyNextModeMove(a);
    }
}
//==================================
int SIGOrder_Tank_EndRotationBeforeTankMode(OBJ *a)
{
    SetOrder(a,0,NULL);
    //turret rotated to special position
    if (a->mainimage->side == TANKNORMALSIDE)
    {
	if (IsSubUnit(a->SC_Unit))
	    ApplyNextModeMove(a->subunit);
	else
	    ApplyNextModeMove(a);
    }
}
//==================================
int  SIGOrder_Tank_AfterSiegeCmd(OBJ *a)
{
    a->modemove = MODESTOP;
    a->subunit->modemove = MODESTOP;
    SetOrder(a,0,NULL);
    return(0);
}
//==================================
int  SIGOrder_Tank_AfterNormalCmd(OBJ *a)
{
    unsigned char inv = a->mainimage->invisiblecolors;
    if (a->SC_Unit == SC_TANKSIEGEOBJ)
    {
	ChangeUnitSubUnitAndImagesAssociated(a,SC_TANKNORMALOBJ);
    }
    else
	if (a->SC_Unit == SC_HERO_EDMUNDDUKESMOBJ)
	{
	    ChangeUnitSubUnitAndImagesAssociated(a,SC_HERO_EDMUNDDUKETMOBJ);
	}
    a->modemove = MODESTOP;
    a->subunit->modemove = MODESTOP;
    SetOrder(a,0,NULL);
    a->mainimage->invisiblecolors = inv;
    a->subunit->mainimage->invisiblecolors = inv;
    return(1);
}
//==================================
int  SIGOrder_AfterBurrow(OBJ *a)
{
    ChangeTypeOfProp(a,PROPNORMAL2);

    SetBurrowFlag(a,1);
    SetMageAtr(&a->atrobj,ATRINVISIBLE,WRAITHEPODARBITEROM);
    a->mainimage->invisiblecolors = MAXINVISIBLECOLOR;

    a->mainimage->UnitDirection256(12*8);
    SetOrder(a,0,NULL);
    a->modemove = MODESTOP;		//to prevent moveobj with MODESTOP after burrowed
    return(0);    
}
//==================================
int SIGOrder_AfterUnburrow(OBJ *a)
{
    ChangeTypeOfProp(a,PROPNORMAL1);
//    SetBurrowFlag(a,0);
//    SetMageAtr(&a->atrobj,ATRINVISIBLE,0);
//    a->mainimage->invisiblecolors = MININVISIBLECOLOR;
    if (a->SC_Unit != SC_VULTUREMINEOBJ)
	Play_sfxdata(a->mainimage->xpos>>8,a->mainimage->ypos>>8,SFXDATA_BURROWUP,2);
    SetOrder(a,0,NULL);
    if (IfHaveNextModeMove(a))
    {
	ApplyNextModeMove(a);
	return(0);
    }
    else
    {
	a->modemove = MODESTOP;		//to prevent obj to stay with unburrow mode
    }
    return(0);
}
//==================================
int  SIGOrder_LiftOff(OBJ *a)
{
    OVERLAY_IMG *tempimg;
    SetOnSkyOBJ(a,1);
    SetObjWalk8(&map,a,CLEARWALK);
    SetOrder(a,0,NULL);
    ChangeTypeOfProp(a,PROPONAIR);
    a->mainimage->flags |= SC_IMAGE_FLAG_AIRIMG;
    a->mainimage->elevationlevel += ADDBUILD_AIR_ELEVATIONLEVEL;
    if (a->mainimage->childlists)
    {
	a->mainimage->childlists->EnumListInit();
	while(tempimg = (OVERLAY_IMG *) a->mainimage->childlists->GetNextListElem(NULL))
	{
    	    if (tempimg->flags & SC_IMAGE_FLAG_IMGOVER)
    	    {
		tempimg->flags |= SC_IMAGE_FLAG_AIRIMG;
		tempimg->elevationlevel += ADDBUILD_AIR_ELEVATIONLEVEL;
	    }
	}
    }
    return(0);
}
//==================================
int  SIGOrder_Landing(OBJ *a)
{
    OVERLAY_IMG *tempimg;
    SetOnSkyOBJ(a,0);
    SetOrder(a,0,NULL);
    a->mainimage->flags &= ~SC_IMAGE_FLAG_AIRIMG;
    a->mainimage->elevationlevel -= ADDBUILD_AIR_ELEVATIONLEVEL;
    
    if (a->mainimage->childlists)
    {
	a->mainimage->childlists->EnumListInit();
	while(tempimg = (OVERLAY_IMG *) a->mainimage->childlists->GetNextListElem(NULL))
	{
    	    if (tempimg->flags & SC_IMAGE_FLAG_IMGOVER)
    	    {
		tempimg->flags &= ~SC_IMAGE_FLAG_AIRIMG;
		tempimg->elevationlevel -= ADDBUILD_AIR_ELEVATIONLEVEL;
	    }
	}
    }
    return(0);
}
//==================================
int  SIGOrder_ScannerSweepEnd(OBJ *a)
{
    a->timeleft = SCANNERSWEEPTIMELEFT;
    return(0);
}
//==================================
int  SIGOrder_ZergConstrFromLarvaeEnd(OBJ *a)
{
    SetOrder(a,4,&SIGOrder_ZergEggBirth);
    SetOBJIScriptNr(a,ISCRIPTNR_SPECIALSTATE1,ISCRIPTNR_EXECUTE);		//egg construct script
    return(1);
}
//==================================
int  SIGOrder_ZergEggBirth(OBJ *a)
{
    int x256,y256;
    x256 = GetOBJx256(a);
    y256 = GetOBJy256(a);
    a->mainimage->DeleteMainImgAndChilds();
//    a->SC_Unit = a->SC_ConstrUnit;
    ChangeSC_Unit(a,a->playernr,a->SC_ConstrUnit,CHANGESC_UNIT_READY);
    CreateImageAndAddToList(a,x256,y256,0,NOLOIMAGE);
    SetUnitPercentLife(a,100);
    SetUnitPercentShield(a,100);
    SetOrder(a,4,&SIGOrder_ZergUnitBirth);
    SetOBJIScriptNr(a,ISCRIPTNR_SPECIALSTATE1,ISCRIPTNR_EXECUTE);		//zerg birth script
    activatesound(a,MODESOUNDREADY,2,NOSTOPCURRENTSOUNDS);
    return(1);
}
//==================================
int  SIGOrder_ZergUnitBirth(OBJ *a)
{
    OBJ *a2;
    LOFILE *lo;
    int x,y;
    unsigned short birth_id;
    signed char *adrxyoffs;
    x = GetOBJx(a);
    y = GetOBJy(a);
//    if (GetSupplyProvided(a->SC_Unit)>0)
//    {
//	ChangeMaxSupply(a->playernr,a->SC_Unit,PLUSFACTOR);
//    }
    if (GetInEgg(a->SC_Unit))
    {
	//change first unit position
	birth_id = alldattbl.units_dat->ConstructionAnimation[a->SC_Unit];
	GetLoadedImage( alldattbl.images_dat->Landing_Dust_Overlay[birth_id],(void **)&lo);
	adrxyoffs = GetLoXY(lo,a->mainimage->framenr,0);
	a2 = createobjfulllife(x+adrxyoffs[2],y+adrxyoffs[3],a->SC_Unit,a->playernr);
	if (a->SC_Unit == SC_ZERGLINGOBJ)
	    a2->mainimage->side += 8 * 8;
	else//here scourge
	    a2->mainimage->side += 3 * 8;
        x += adrxyoffs[0];
        y += adrxyoffs[1];
    }
    else
    {
	if (IsAirUnit(a->SC_Unit))
	{
	    if (IsGroundUnit(a->SC_FromUnit))
	    {
		SetOnSkyOBJ(a,1);
		y -= AIRUNITS_START_YPOS;
	    }
	}
	else
	{
	    if (a->SC_Unit == SC_DRONEOBJ)
    		y -= DRONE_START_YPOS;
	}
    }
    a->mainimage->DeleteMainImgAndChilds();
    CreateImageAndAddToList(a,x<<8,y<<8,4,NOLOIMAGE);
    a->prop |= VARREADY;
    return(0);
}
//==================================
int  SIGOrder_CancelMorthFromHydralisk(OBJ *a)
{
    int x256,y256;
    x256 = GetOBJx256(a);
    y256 = GetOBJy256(a);
    SetOrder(a,0,NULL);
    a->life = a->templife;
    a->shield = a->tempshield;
    a->prop |= VARREADY;
    a->mainimage->DeleteMainImgAndChilds();
    a->SC_Unit = a->SC_FromUnit;
    CreateImageAndAddToList(a,x256,y256,2,NOLOIMAGE);
    return(1);
}
//==================================
int  SIGOrder_CancelMorthFromDrone(OBJ *a)
{
    int x256,y256;
    if (a->replaceobj)
    {
	a->replaceobj->prop &= ~(VARCANTSELECT|VARNOTWORK);
        a->replaceobj->mainimage->ShowChildsImgFlag();
        a->replaceobj->mainimage->ShowImgFlag();
	a->replaceobj->mainimage->EnableExecScript();
	if (a->replaceobj->replaceobj)
	    a->replaceobj->replaceobj = NULL;
	a->replaceobj = NULL;
    }
    SetOrder(a,0,SIGOrder_DroneLiftOff);
    x256 = GetOBJx256(a);
    y256 = GetOBJy256(a);
    SetObjWalk8(&map,a,CLEARWALK);

    a->mainimage->DeleteMainImgAndChilds();
    a->prop |= VARREADY;
    CreateImageAndAddToList(a,x256,y256,2,NOLOIMAGE);
    iscriptinfo.ExecuteScript(a->mainimage);	//execute almostbuild
    a->SC_ConstrUnit = a->SC_FromUnit;
    a->SC_FromUnit = a->SC_Unit;
    
    ChangeSC_Unit(a,a->playernr,a->SC_ConstrUnit,CHANGESC_UNIT_CONSTR);
    a->prop |= VARREADY;
    a->mainimage->DeleteChilds(SC_IMAGE_FLAG_IMGUNDER);
    a->mainimage->DeleteMainImg();
    CreateImageAndAddToList(a,x256,y256,2,NOLOIMAGE);//a->mainimage remain in mainimageslist 

    a->life = a->templife;
    a->shield = a->tempshield;
    RemoveBloodFlameOverlays(a,0);
    SetOBJIScriptNr(a,ISCRIPTNR_LIFTOFF,ISCRIPTNR_EXECUTE);
    Play_sfxdata(GetOBJx(a),GetOBJy(a),SFXDATA_ZERGBUILDWARPCANCEL,2);
    SetObjWalk8(&map,a,SETWALK);
    return(1);
}
//==================================
int  SIGOrder_DroneLiftOff(OBJ *a)
{
    ChangeTypeOfProp(a,PROPNORMAL1);
    SetModeMove(a,MODELIFTOFF);
    a->mainimage->ydelta = DRONE_START_YPOS;
    a->deltavertpos = DRONE_START_YPOS;
    return(0);
}
//==================================
int  SIGOrder_CancelMorthFromBuild(OBJ *a)
{
    int x256,y256;
    x256 = GetOBJx256(a);
    y256 = GetOBJy256(a);

    SetOrder(a,0,NULL);

    a->mainimage->DeleteMainImgAndChilds();
    ChangeSC_Unit(a,a->playernr,a->SC_Unit,CHANGESC_UNIT_READY);
    CreateImageAndAddToList(a,x256,y256,2,NOLOIMAGE);
    iscriptinfo.ExecuteScript(a->mainimage);	//execute almostbuild

    ChangeSC_Unit(a,a->playernr,a->SC_FromUnit,CHANGESC_UNIT_READY);
    a->mainimage->DeleteChilds(SC_IMAGE_FLAG_IMGUNDER);
    a->mainimage->DeleteMainImg();
    CreateImageAndAddToList(a,x256,y256,2,NOLOIMAGE);

    a->life = a->templife;
    a->shield = a->tempshield;
    a->lastdamageoverlays = -1;
    AddRemoveBloodFlameOverlays(a);
    ChangeTypeOfProp(a,PROPNORMAL1);
    Play_sfxdata(GetOBJx(a),GetOBJy(a),SFXDATA_ZERGBUILDWARPCANCEL,2);
    return(1);
}
//==================================
int  SIGOrder_CancelMorthFromMutalisk(OBJ *a)
{
    int x256,y256;
    x256 = GetOBJx256(a);
    y256 = GetOBJy256(a);

    SetOrder(a,0,NULL);

    if (a->life > GetUnitMaxLife(a->SC_FromUnit))
	a->life = GetUnitMaxLife(a->SC_FromUnit);
    if (a->shield > a->tempshield)
	a->shield = GetUnitMaxShield(a->SC_FromUnit);
    a->prop |= VARREADY;
    a->mainimage->DeleteMainImgAndChilds();
    a->SC_Unit = a->SC_FromUnit;
    CreateImageAndAddToList(a,x256,y256,2,NOLOIMAGE);

    return(1);
}
//==================================
int  SIGOrder_DoodadAfterTop(OBJ *a)
{
    SetOrder(a,0,NULL);
    if (!IsInvincibleUnit(a->SC_Unit))
    {
        OBJSetInvisibleColors(a,MININVISIBLECOLOR);
        SetMageAtr(&a->atrobj,ATRINVISIBLE,0);
    }
    SetDoodadState(a,DOODAD_TOP_STATE);
    SetDoodadMoveDirection(a,DOODAD_MOVE_NONE);
    ApplyNextModeMove(a);
    return(0);
}
//==================================
int  SIGOrder_DoodadAfterBottom(OBJ *a)
{
    SetOrder(a,0,NULL);
    if (!IsInvincibleUnit(a->SC_Unit))
    {
        OBJSetInvisibleColors(a,MAXINVISIBLECOLOR);
        SetMageAtr(&a->atrobj,ATRINVISIBLE,ATRMAGEINFINITE);
    }
    SetDoodadState(a,DOODAD_BOTTOM_STATE);
    SetDoodadMoveDirection(a,DOODAD_MOVE_NONE);
    ApplyNextModeMove(a);
    return(0);
}
//==================================
int  SIGOrder_UnitInitComplete(OBJ *a)
{
    SetOrder(a,0,NULL);
    ChangeTypeOfProp(a,PROPNORMAL1);
    a->prop |= VARREADY;
    activatesound(a,MODESOUNDREADY,2,NOSTOPCURRENTSOUNDS);
    return(0);
}
//==================================
int  SIGOrder_AfterCastMage(OBJ *a)
{
    SetOrder(a,0,NULL);
    a->finalOBJ = NULL;
    if (ApplyNextModeMove(a))
	return(1);
    return(0);
}
//==================================
int  SIGOrder_NukeLanding(OBJ *a)
{
    OBJ *ghostobj;
    SetOrder(a,0,NULL);
    a->mainimage->side = 128;	//set nuke down position
    a->currentspeed = 0;
    //show nuke for all players
    a->mainimage->ShowChildsImgFlag();
    a->mainimage->ShowImgFlag();
    //ghost leave wait state
    ghostobj = a->doubleunit;
    GhostNUKECancel(ghostobj,GHOSTRELEASENUKE);
    if (a->prop & VARINCRMAGE)	//decreasing mage (now is cloaked)
        ChangeTypeOfProp(ghostobj,PROPNORMAL2);
    else
        ChangeTypeOfProp(ghostobj,PROPNORMAL1);//or normal2
    //nuke move down
    moveobj(a,NULL,MODEMOVE,GetOBJx(a),GetOBJy(a)+NUKE_MAXAIR_YPOS+INITIAL_NUKE_YPOS,NOSHOWERROR,0);
    //after, do special state(create weapon, that do damage)
    AddModeMove(a,NULL,MODENUKESPECIALSTATE1,0,0,0);
    return(1);
}
//==================================
int  SIGOrder_AfterCastHeal(OBJ *a)
{
    if (a->finalOBJ)
    {
	//heal unit here
	if (a->mana >= (1<<8))
	{
	    AddUnitLife(a->finalOBJ,MEDICHEALFACTOR);
	    DecrMana(a,1);
	    //check if unitlife is max do search next to heal
	    if (a->finalOBJ->life == GetUnitMaxLife(a->finalOBJ->SC_Unit))
	    {
//		moveobj(a,NULL,MODESTOP,0,0,NOSHOWERROR,0);
//		return(1);
		//try to hear other unit
		a->modemove = MODEPATROL;
		if (!MedicAction(a,a->mainimage))
		    moveobj(a,NULL,MODESTOP,0,0,NOSHOWERROR,0);
		return(1);
	    }
	}
	if (GetDistanceBetweenUnits256(a,a->finalOBJ) <= mageprop[a->castmagenr].mindistance+(10<<8) )
	{
	    a->mainimage->UnitNeededDirection256(CalcDirection1(GetOBJx256(a),GetOBJy256(a),GetOBJx256(a->finalOBJ),GetOBJy256(a->finalOBJ)));
	    //check if need rotation
	    if (a->mainimage->flags & SC_IMAGE_FLAG_NEEDROTATIONTODIRECTION)
	    {
		a->mainimage->Rotation(alldattbl.flingy_dat->TurnRadius[alldattbl.units_dat->flingy_id[a->SC_Unit]]);
	    }
	}
	else
	{
	    //unit goes out of heal range
	    moveobj(a,a->finalOBJ,MODEHEAL,0,0,NOSHOWERROR,0);
	    return(1);
	}
    }
    else
    {
        moveobj(a,NULL,MODESTOP,0,0,NOSHOWERROR,0);
        return(1);
    }
    return(0);
}
//==================================
