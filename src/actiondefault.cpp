
#include "auxil.h"
#include "maps.h"
#include "walk.h"
#include "upgrade.h"
#include "rand.h"
#include "unitaction.h"
#include "market.h"
#include "objinfo.h"
#include "gr.h"
#include "iscript.h"
#include "sigorder.h"
#include "images.h"
#include "objs.h"
#include "tbl.h"
#include "weapons.h"
#include "actiondefault.h"

//=================================
int NoneActionUnit(struct OBJ *a,MAIN_IMG *img)
{
    return(0);
}
//=================================
int GoliathTAction(struct OBJ *a,MAIN_IMG *img)
{
//    if (a->subunit->modemove == MODEMOVE)
//    {
//	ChangeSubUnitCoords(a,a->subunit);
//    }
    if (a->modemove==MODEATACK)
	return(0);
//    SetDestRotatePos(a,GetDestRotatePos(a->subunit));
    return(0);
//    if (!(a->prop & VARHAVEGOAL) && !(a->select[0] & VARROTATION))
//    {
//    }
//    return(0);
}
//=================================
int TankTAction(struct OBJ *a,MAIN_IMG *img)
{
//    if (a->subunit->modemove == MODEMOVE || )
//    {
//	ChangeSubUnitCoords(a,a->subunit);
//    }
    if (a->modemove==MODEATACK || a->modemove==MODETANKSIEGE || a->modemove==MODETANKNORMAL)
	return(0);
//    SetDestRotatePos(a,GetDestRotatePos(a->subunit));
    return(0);
}
//=================================
int CarrierAction(struct OBJ *a,MAIN_IMG *img)
{
    OBJstruct *b = loadobj(a->SC_Unit);
//    if (needactionatack(a,b))	//if need creation make it
    if (0)
    {
	WakeUpAllChilds(a,b);
    }
    return 0;
}
//=================================
int InterceptorsAction(struct OBJ *a,MAIN_IMG *img)
{
/*    OBJstruct *b = loadobj(a->SC_Unit);
    if (a->prop&VARINBASE)
	return 0;
    if (a->modemove!=MODERECHARGE)
    {
	if (b->timeflybeforerecharge)    //if exist timeflybeforerecharge
	{
	    a->timeflybeforerecharge--;
	    if (a->timeflybeforerecharge<=0)
	    {
		moveobj(a,a->myparent,MODERECHARGE,0,0,NOSHOWERROR,0);
		return 1;
	    }
	}
	if (!a->curentatacknr)
	{
            moveobj(a,a->myparent,MODERECHARGE,0,0,NOSHOWERROR,0);
	    return 1;
	}
	if (a->modemove == MODESTOP)
	{
	    int sx,sy,sxsource,sysource;
	    if (a->myparent)
	    {
		sxsource = GetOBJx(a->myparent);
		sysource = GetOBJy(a->myparent);
	    }
	    else
	    {
		//if parent die need to move around
//		sxsource = (int) a->prevx;
//		sysource = (int) a->prevy;
	    }
	    do{
	        sx = sxsource + myrand(-INTERCEPTORFLYRANGE,INTERCEPTORFLYRANGE);
	    }while(sx<0||sx>=MAXXMAP*SIZESPRLANSHX);
	    do{    
		sy = sysource + myrand(-INTERCEPTORFLYRANGE,INTERCEPTORFLYRANGE);
	    }while(sy<0||sy>MAXYMAP*SIZESPRLANSHY);
	    moveobj(a,NULL,MODEMOVE,sx,sy,NOSHOWERROR,0);
	    return 1;
	}
    }
    else
    {
	if (!a->myparent||a->myparent->playernr!=a->playernr)
	{
	    dieobj(a);
	    return 1;
	}
	else 
	    if (controldistancemode(GetOBJx(a),GetOBJy(a),GetOBJx(a->myparent),GetOBJy(a->myparent),a->modemove))
	    {
		ReturnedToBase(a,b);//after moved to base
		return 1;
	    }
    }
    return 0;
*/
}
//=================================
int CritterAction(struct OBJ *a,MAIN_IMG *img)
{
    int sx,sy;
    OBJstruct *b = loadobj(a->SC_Unit);
    if (GetUnitRace(a->SC_Unit)==NEUTRALRACE&&IsGroupMenFlag(a->SC_Unit))
    {
	if (a->modemove==MODESTOP||a->modemove==MODEHOLDPOS)
	{
	    if (a->whatbuildconstr==0)
	    {
		a->whatbuildconstr = TIMETOWAITBEFOREMOVECRITTER;
		do{
    		    sx = (int) a->startx + myrand(-CRITTERRANGE,CRITTERRANGE);
		}while(sx<0||sx>=MAXXMAP*SIZESPRLANSHX);
		do{
		    sy = (int) a->starty + myrand(-CRITTERRANGE,CRITTERRANGE);
		}while(sy<0||sy>MAXYMAP*SIZESPRLANSHY);
		moveobj(a,NULL,MODEMOVE,sx,sy,NOSHOWERROR,0);
		return 1;
	    }
	    else
		a->whatbuildconstr--;
	}
    }
    return 0;
}
//=================================
void SCVConstructAction(OBJ *a,MAIN_IMG *img)
{
    int x,y,mx,my;
    if (a->modemove == MODECONSTRUCT)
    {
        if (a->timerepair==0)
	{
	    //repair next part
	    a->timerepair = TIMETOREPAIRINONEPOSITION;

    	    mx = POSX(GetUnitWidthAndHeight(a->constrobj->SC_Unit,UNITDIM_WIDTH));	//razmer stroeshegosea zdania
    	    my = POSY(GetUnitWidthAndHeight(a->constrobj->SC_Unit,UNITDIM_HEIGHT));
    	    
//    	    do{
    		getborderbuild(mx,my,&x,&y);//uznati novye coord. stroiki
		x -= mx/2;
		y -= my/2;
		x++;
		y++;		//stsentrirovati coord. stroki po tsentru zdania
    		x += a->constrobj->xkart;
    		y += a->constrobj->ykart;
//	    }while(a->xkart == x && a->ykart == y);	//the same coordinates is bad idea, change it
		
	    initmoveaction(a,NULL,MODECONSTRUCTMOVE,0,0,x*32,y*32);//move to construct point build
	    SetOBJIScriptNr(a,ISCRIPTNR_WALKING,ISCRIPTNR_EXECUTE);			//set the same script to all obj images and subimages
	    a->modemove = MODECONSTRUCTMOVE;
	    AddModeMove(a,NULL,MODETURN180,0,0,0);//contibue construction
	    AddModeMove(a,NULL,MODECONSTRUCT,x,y,0);//continue construction
	}
	a->timerepair--;
    }
    else
    {
	if (a->modemove == MODESTAYANDREPAIR)
	{
	    if (!a->finalOBJ)
	    {
		moveobj(a,NULL,MODESTOP,0,0,NOSHOWERROR,0);
		return;
	    }
	    //check if repaired unit is move away, need to move to him
	    if (NEEDTOREPAIRREFRESHBIT)
	    {
		if ( GetDistanceTo(a->finalOBJ,GetOBJx(a),GetOBJy(a)) <= 16 )
		{
		}
		else
		{
		    moveobj(a,a->finalOBJ,MODEREPAIR,0,0,NOSHOWERROR,0);
		    return;
		}
	    }
	    if (!RepairUnit(a->playernr,a,a->finalOBJ))
	    {
		moveobj(a,NULL,MODESTOP,0,0,NOSHOWERROR,0);
		return;
	    }
	}
    }
}
//=================================
unsigned char SC_CARRY_GAS[3]={SC_CARRY_ZERG_GASORB1,SC_CARRY_TERRAN_GASORB1,SC_CARRY_PROTOSS_GASORB1};
unsigned char SC_CARRY_MIN[3]={SC_CARRY_MINERAL2,SC_CARRY_MINERAL2,SC_CARRY_MINERAL1};
//=================================
int WorkersAction(struct OBJ *a,MAIN_IMG *img)
{
    OBJ *newobj,*res;
    int constrerror,rescnt,resval;
    signed char xlo,ylo;
    switch(a->modemove)
    {
	case MODELANDING:
	    //only drone come here
    	    img->ydelta++;    
	    if (--a->deltavertpos == 0)
	    {
		//check if have resources and decrement
		if (!DecrUnitCost(a->SC_ConstrUnit,a->playernr))
		{
		    SIGOrder_DroneLiftOff(a);
		    SetOBJIScriptNr(a,ISCRIPTNR_LIFTOFF,ISCRIPTNR_EXECUTE);
		    return(0);
		}
		//check if can construct on this place
		if (!CheckIfCanBuild(a,a->SC_ConstrUnit,a->finalx>>8,a->finaly>>8,&constrerror))
		{
		    if (a->playernr == NUMBGAMER)
	    	    {
			activatesound(a,MODESOUNDERROR,2,NOSTOPCURRENTSOUNDS);
			putbuildplacemessage(-constrerror);
		    }
		    SIGOrder_DroneLiftOff(a);
		    SetOBJIScriptNr(a,ISCRIPTNR_LIFTOFF,ISCRIPTNR_EXECUTE);
		    return(0);
		}
		//if carry obj, carry obj is destroyed
		if (a->carryobj)
		{
		    dieobj_silently(a->carryobj);
		}
		//we landing, construct build
		SetObjWalk8(&map,a,CLEARWALK);
		a->modemove = MODESTOP;
            	a->SC_FromUnit = a->SC_Unit;			//save previous unit
            	a->templife = a->life;
            	a->tempshield = a->shield;
            	a->mainimage->DeleteMainImgAndChilds();

            	ChangeSC_Unit(a,a->playernr,a->SC_ConstrUnit,CHANGESC_UNIT_CONSTR);
            	CreateImageAndAddToList(a,a->finalx,a->finaly,0,NOLOIMAGE);
            	SetUnitPercentLife(a,1);
            	SetUnitPercentShield(a,1);
            	SetBeginSelfConstruct(a);
            	a->selfconstruct.currentconstrstep = CONSTR_STEP_ZERGBUILDLESS33;
            	SetOBJIScriptNr(a,ISCRIPTNR_INIT,ISCRIPTNR_SETONLY);
            	ChangeTypeOfProp(a,PROPDECONSTRUCT);
//		a->lastdamageoverlays = -1;
            	AddRemoveBloodFlameOverlays(a);		//remove any bloodflame if any
            	SetObjWalk8(&map,a,SETWALK);
            	if (a->SC_ConstrUnit == SC_EXTRACTOROBJ)
            	{
		    //geyser is occupied
	    	    SetOBJxy256(a,GetOBJx256(a->data.geyserdest.obj),GetOBJy256(a->data.geyserdest.obj));
            	    GeyserDisactivate(a,a->data.geyserdest.obj);
            	}
	    }
	    break;
	case MODELIFTOFF:
    	    //only drone come here
    	    img->ydelta--;    
	    if (--a->deltavertpos == 0)
	    {
		a->modemove = MODESTOP;
	    }
	    break;
	case MODEGATHERMINERAL:
	    if (--a->data.gather.gathertime == 0 )
	    {
		//gather complete, need to return
		a->finalOBJ = NULL;
		res = a->resourceobj;
		ReleaseResource(a);
		rescnt = decrresourceobj(res,NORMALGETRESOURCES);
		GetOBJXYSideOffsets(a,a->mainimage,IMAGE_OVERLAY_SPECIAL,&xlo,&ylo);

		newobj = createreschunk(a,GetOBJx(a)+xlo,GetOBJy(a)+ylo,SC_CARRY_MIN[GetUnitRace(a->SC_Unit)]);
		//carry obj setmode to carry, this parameter give to carryobj change they coords depend on scv/drone/probe
		newobj->modemove = MODECARRYME;
		newobj->prop |= VARCANTSELECT;

	    
		newobj->data.resourcechunk.restype = SC_CARRY_MINERAL1;
		newobj->data.resourcechunk.rescnt  = rescnt;

		newobj->mainimage->side = a->mainimage->side;
		newobj->mainimage->neededside = a->mainimage->side;

		moveobj(a,NULL,MODERETURNCARGO,0,0,NOSHOWERROR,0);
	    }
	    break;
	case MODEGATHERGAS:
	    if (--a->data.gather.gathertime == 0 )
	    {
		//gather complete, need to return
		a->finalOBJ = NULL;
		res = a->resourceobj;
		ReleaseResource(a);
		rescnt = decrresourceobj(res,NORMALGETRESOURCES);
		GetOBJXYSideOffsets(a,a->mainimage,IMAGE_OVERLAY_SPECIAL,&xlo,&ylo);

		newobj = createreschunk(a,GetOBJx(a)+xlo,GetOBJy(a)+ylo,SC_CARRY_GAS[GetUnitRace(a->SC_Unit)]);

		//carry obj setmode to carry, this parameter give to carryobj change they coords depend on scv/drone/probe
		newobj->modemove = MODECARRYME;
		newobj->prop |= VARCANTSELECT;

		a->mainimage->ShowChildsImgFlag();	//show worker
		a->mainimage->ShowImgFlag();
		a->prop &= ~VARNOTHERE;
	    
		newobj->data.resourcechunk.restype = SC_CARRY_GAS[0];
		newobj->data.resourcechunk.rescnt  = rescnt;

		newobj->mainimage->side = a->mainimage->side;
		newobj->mainimage->neededside = a->mainimage->side;

		moveobj(a,NULL,MODERETURNCARGO,0,0,NOSHOWERROR,0);
	    }
	    break;
	case MODEWAITHARVESTMINERAL:
	    if (--a->data.gather.waittoharvest == 0 )
	    {
		newobj = GetNearResource(a,SC_MINERAL1OBJ,&resval);
		if (newobj)
		    moveobj(a,newobj,MODEGATHER,0,0,NOSHOWERROR,0);
		else
		    if (resval)
			a->data.gather.waittoharvest = WAITTOHARVEST;
		    else
			moveobj(a,NULL,MODESTOP,0,0,NOSHOWERROR,0);
	    }
	    break;
	case MODEWAITHARVESTGAS:
	    if (--a->data.gather.waittoharvest == 0 )
	    {
		if (a->finalOBJ)
		    if (!a->finalOBJ->resourceobj)//if is not occupied gather
			moveobj(a,a->finalOBJ,MODEGATHERGAS,0,0,NOSHOWERROR,0);
		    else
			a->data.gather.waittoharvest = WAITTOHARVEST;
		else
		    moveobj(a,NULL,MODESTOP,0,0,NOSHOWERROR,0);
	    }
	    break;
	case MODEWAITRETURNCARGO:
	    if (--a->data.gather.waittoharvest == 0 )
	    {
		newobj = GetNearCenter(a);
		if (newobj)
		{
		    a->modemove = MODERETURNCARGO;
		    initmoveaction(a,newobj,MODERETURNCARGO,0,0,GetOBJx(newobj),GetOBJy(newobj));
		    AddModeMove(a,newobj,MODERETURNCARGO,GetOBJx(newobj),GetOBJy(newobj),NOSHOWERROR);
		}
		else
		{
		    a->data.gather.waittoharvest = WAITTOHARVEST;
		}
	    }
	    break;
	case MODESTOP:
	    if (a->carryobj)
	    {
		if (a->prop & VARAUTOMATRESOURCERET)
		    if (IsPickupUnit(a->carryobj->SC_Unit) && !IsInvincibleUnit(a->carryobj->SC_Unit))
		    {
			//if it is mineral or gas orb
			moveobj(a,NULL,MODERETURNCARGO,0,0,NOSHOWERROR,0);
		    }
		return(0);
	    }
	    else
	    {
		if (a->SC_Unit == SC_SCVOBJ && UnitDoAiAction(a->playernr))	//if scv and computer control try to repair if need something
		{
		    if (--a->data.gather.waitforrepair <= 0)
		    {
			a->data.gather.waitforrepair = WAITTICKBEFORESEARCHREPAIR;
	    		newobj = SearchOBJforOBJ(a,MODEREPAIR);
			if (newobj)
			{
			    moveobj(a,newobj,MODEREPAIR,0,0,NOSHOWERROR,0);
			    return(1);
			}
		    }
		    return(0);
		}
	    }
	    break;
    }
    if (a->SC_Unit == SC_SCVOBJ)
	SCVConstructAction(a,img);
    return(0);
}
//=================================
int AirBuildings(struct OBJ *a,MAIN_IMG *img)
{
    int j;
    MAIN_IMG *tempimg;
    if (a->modemove == MODELANDING)
    {
    	    //any flyed buildings come here
    	    //change ypos of build
    	    img->ypos += (1 << 8);
    	    //change ydelta of build shadow
	    if (img->childlists)
	    {
		img->childlists->EnumListInit();
		while(tempimg = (MAIN_IMG *) img->childlists->GetNextListElem(NULL))
		{
		    if (tempimg->flags & SC_IMAGE_FLAG_IMGUNDER)
			tempimg->ydelta--;
		}
	    }
	    if (--a->deltavertpos == 0)
	    {
    	        ChangeTypeOfProp(a,PROPNORMAL1);
		ForceKartChanges(a);
		SetObjWalk8(&map,a,SETWALK);
		a->modemove = MODESTOP;
		AttachNearestAddon(a);
		AddDustImages(a,IMAGE_OVERLAY_LANDDUST);
		a->prop &= ~VARDONOTAPPLYNEXTMOVE;
		ApplyNextModeMove(a);
    	    }
    }
    else if (a->modemove == MODELIFTOFF)
    {
    	    //any flyed buildings come here
    	    //change ypos of build
    	    img->ypos -= (1 << 8);
    	    //change ydelta of build shadow
	    if (img->childlists)
	    {
		img->childlists->EnumListInit();
		while(tempimg = (MAIN_IMG *) img->childlists->GetNextListElem(NULL))
		{
		    if (tempimg->flags & SC_IMAGE_FLAG_IMGUNDER)
		        tempimg->ydelta++;
		}
	    }
	    if (--a->deltavertpos == 0)
	    {
		a->modemove = MODESTOP;
		a->prop &= ~VARDONOTAPPLYNEXTMOVE;
		ApplyNextModeMove(a);
	    }
    }
}
//=================================
int LarvaeAction(struct OBJ *a,MAIN_IMG *img)
{
/*    int x,y;
    if (a->modemove==MODESTOP)
    {
	if (!a->deltaconstrx)
	{
	    a->deltaconstrx = NEXTLARVAEMOVETICK;
	    if (a->myparent)
	    {
		getcoordofnewunit(a->myparent,LARVAEOBJ,&x,&y);
		makemove(a,NULL,x,y,MODEMOVE,a->playernr);
	    }
	    else//if no parent move arround
	    {
		x = myrand(LARVAEMOVEDELTA-LARVAEMOVEDELTA/2);
		y = myrand(LARVAEMOVEDELTA-LARVAEMOVEDELTA/2);
		makemove(a,NULL,(int)a->prevx+x,
				(int)a->prevy+y,
				MODEMOVE,a->playernr);
	    }
	}
	else
	    a->deltaconstrx--;
    }
*/
//    if (!a->myparent)
	if (!map.creepflagplace[a->ykart*MAXXMAP+a->xkart])
	{
    	    dieobj(a);
	    return(1);
	}
    return 0;
}
//=================================
int HiveActionFirst(struct OBJ *a,MAIN_IMG *img)
{
    int nrchilds;
    nrchilds = getchilds(a);
    if (nrchilds != MAXLARVAEPRODUCE && !(a->prop&VARLARVAEONCONSTRUCT))
    {
        a->prop |= VARLARVAEONCONSTRUCT;
	MakeQueueAction(CREATELARVAEACTION,a,NULL,0,0,0,1);//make first larva without wait time
    }
    return 0;
}
//=================================
int HiveAction(struct OBJ *a,MAIN_IMG *img)
{
    int nrchilds;
    if (a->SC_Unit == SC_HATCHERYOBJ && IsOBJUnderConstruct(a))
	return(0);
    nrchilds = getchilds(a);
    if ( nrchilds!=MAXLARVAEPRODUCE && !(a->prop&VARLARVAEONCONSTRUCT))
    {
        a->prop |= VARLARVAEONCONSTRUCT;
	MakeQueueAction(CREATELARVAEACTION,a,NULL,0,0,0,LARVAEREFRESHTIME);
    }
    return 0;
}
//=================================
int MedicAction(struct OBJ *a,MAIN_IMG *img)
{
    struct OBJ *c;
    if (NEEDTOHEALBIT)
    {
	switch(a->modemove)
	{
	    case MODESTOP:
	    case MODEPATROL:
	    	if (a->mana >= (1<<8))
		{
		    c = SearchOBJforOBJ(a,MODEHEAL);
		    if (c)
		    {
			moveobj(a,c,MODEHEAL,0,0,NOSHOWERROR,0);
			return(1);
		    }
		}
		return(0);
	    case MODEHEAL:
		return(1);
	    case MODEHOLDPOS:
		return(1);
	}
    }
    if (a->modemove == MODEHEAL)
	return(1);
    return(0);
}
//=================================
int BatteryAction(struct OBJ *a,MAIN_IMG *img)
{
    OBJ *destobj = a->finalOBJ;
    unsigned short image_id,imgflags;
    if (a->modemove == MODERECHARGESHIELD)
    {
	if (!destobj || a->mana < (1<<8))
	{
	    a->finalOBJ = NULL;
	    SetOBJIScriptNr(a,ISCRIPTNR_WORKINGTOIDLE,ISCRIPTNR_EXECUTE);
	    a->modemove = MODESTOP;
	    return(1);
	}
	//show recharge shield image
	if (--a->data.battery.rechargetime<=0)
	{
	    a->data.battery.rechargetime = BATTERYRECHARGEWAITTIME;
	    if (GetDistanceBetweenUnits256(a,destobj) <= mageprop[a->castmagenr].mindistance)
	    {
		if (!CheckForSpecificChildsImageID(destobj->mainimage,IMAGEID_RECHARGESHIELDSMALL,IMAGEID_RECHARGESHIELDLARGE))
		{
		    image_id = IMAGEID_RECHARGESHIELDSMALL + GetUnitOverlaySize(destobj->SC_Unit);
		    imgflags = 0;
		    if (IsOnSkyOBJ(destobj))
			imgflags = SC_IMAGE_FLAG_AIRIMG;
		    new OVERLAY_IMG(destobj->mainimage,image_id,0,0,destobj->mainimage->elevationlevel+1,imgflags | SC_IMAGE_FLAG_IMGOVER,ISCRIPTNR_INIT);
		}
	    }
	    else
	    {
		//unit goes out of recharge range, so next unit charge
		if (!ApplyNextModeMove(a))
		{
		    SetOBJIScriptNr(a,ISCRIPTNR_WORKINGTOIDLE,ISCRIPTNR_EXECUTE);
		    a->modemove = MODESTOP;
		}
		return(1);
	    }
	    AddUnitShield(destobj,BATTERYRECHARGEFACTOR);
	    DecrMana(a,1);
	    if (destobj->shield == GetUnitMaxShield(destobj->SC_Unit))
	    {
		//rechange next unit if have in queue
		if (!ApplyNextModeMove(a))
		{
		    SetOBJIScriptNr(a,ISCRIPTNR_WORKINGTOIDLE,ISCRIPTNR_EXECUTE);
		    a->modemove = MODESTOP;
		}
	    }
	    return(1);
	}
    }
    return(0);
}
//=================================
int QueenAction(struct OBJ *a,MAIN_IMG *img)
{
    int x,y;
    struct OBJ *c;
    if (a->modemove == MODEINFEST)
    {
	if (a->data.queeninfest.actionbit)
	{
	    if (--a->data.queeninfest.remaintimetoinfest == 0)
	    {
		a->data.queeninfest.actionbit = 0;
		c = a->finalOBJ;
		//restore queen
		a->prop &= ~(VARNOTHERE | VARCANTSELECT);
		a->mainimage->EnableExecScript();
		a->mainimage->ShowChildsImgFlag();
    		a->mainimage->ShowImgFlag();
		if (c)
		{
		    x = GetOBJx(c);
		    y = GetOBJy(c);
		    dieobj_silently(c);
		    c = createobjfulllife(x,y,SC_INFCOMMCENTEROBJ,a->playernr);
		}
		moveobj(a,NULL,MODESTOP,0,0,NOSHOWERROR,0);
	    }
	    
	}
	return 1;
    }
    if (NEEDTOINFESTBIT)
    {
	switch(a->modemove)
	{
	    case MODESTOP:
	    	c = SearchOBJforOBJ(a,MODEINFEST);
		if (c)
		{
		    moveobj(a,c,MODEINFEST,0,0,NOSHOWERROR,0);
		    return(1);
		}
		return(0);
	    case MODEINFEST:
		return(0);
	    case MODEHOLDPOS:
		return(0);
	}
    }
    return 0;
}
//=================================
int TransportAction(struct OBJ *a,MAIN_IMG *img)
{
    OBJstruct *b = loadobj(a->SC_Unit);
    if (a && a->loaded)
    {
	if (--a->loaded->waittickstounload <= 0)
	{
	    a->loaded->waittickstounload = 0;
	    if (a->modemove == MODEUNLOADUNITS)
	    {
		UnLoadObjInObj(a,0,NEWXYCOORDS,0,0,0);
		if (!a->loaded)
		    ApplyNextModeMove(a);
	    }
	    else
	    { 
		if (a->loaded->nrunittounload < 0 )
		    return(1);
		else
		    UnLoadObjInObj(a,a->loaded->cellobj[a->loaded->nrunittounload],NEWXYCOORDS,0,0,0);
	    }
	    //if remain units in transport
	    if (a->loaded)
	    {
		a->loaded->nrunittounload = -1;
		if (a->SC_Unit == SC_BUNKEROBJ)
		    a->loaded->waittickstounload = 1;
		else
		    a->loaded->waittickstounload = TIMETOUNLOADUNIT;
	    }
	}
    }
    return(0);
}
//=================================
int SpiderMineAction(OBJ *a,MAIN_IMG *img)
{
/*    if (a->modemove==MODESTOP||a->destobj)
	if (!IsOBJBurrowed(a)&&(a->modemove!=MODEBURROW)&&(a->modemove!=MODEBURROW2))
	{
    	    moveobj(a,NULL,MODESTOP,0,0,NOSHOWERROR,0);
    	    moveobj(a,NULL,MODEBURROW,0,0,NOSHOWERROR,0);
	}
*/
    return(0);
}
//=================================
int ResActionUnit(OBJ *a,MAIN_IMG *img)
{
    signed char xlo,ylo;
    OBJ *worker = a->carryobj;
    if (a->modemove == MODEDIE)
	return(0);
    if (a->modemove != MODECARRYME || !worker)
    {
	//resource dropped or worker died, so the resource cluster die too
	dieobj(a);
    }
    else
    {
	//change coords of resources depend on worker + overlay special lo offset of worker
	GetOBJXYSideOffsets(worker,worker->mainimage,IMAGE_OVERLAY_SPECIAL,&xlo,&ylo);
	a->mainimage->xpos = GetOBJx256(worker)+(xlo<<8);
	a->mainimage->ypos = GetOBJy256(worker)+(ylo<<8);
	a->mainimage->side = worker->mainimage->side;
	a->mainimage->neededside = worker->mainimage->side;
    }
    return(0);
}
//=================================
//units 215-219 (flag,young chrysalis,psi emitter,data disc,khadarin crystal)
int CarryActionUnit(OBJ *a,MAIN_IMG *img)
{
    signed char xlo,ylo;
    OBJ *worker = a->carryobj;
    if (a->modemove == MODEDIE)
	return(0);
    if (a->modemove != MODECARRYME || !worker)
    {
	//unit leave and don't move
    }
    else
    {
	//change coords of resources depend on worker + overlay special lo offset of worker
	GetOBJXYSideOffsets(worker,worker->mainimage,IMAGE_OVERLAY_SPECIAL,&xlo,&ylo);
	a->mainimage->xpos = GetOBJx256(worker)+(xlo<<8);
	a->mainimage->ypos = GetOBJy256(worker)+(ylo<<8);
	a->mainimage->side = worker->mainimage->side;
	a->mainimage->neededside = worker->mainimage->side;
    }
    return(0);
}
//=================================
//228 units
int (*UnitActionDefaults[MAX_UNITS_ELEM])(struct OBJ *a,MAIN_IMG *img)=
{
    &NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&GoliathTAction,	//000-004
    &NoneActionUnit,&TankTAction   ,&WorkersAction, &NoneActionUnit,&NoneActionUnit,	//005-009
    &NoneActionUnit,&TransportAction,&NoneActionUnit,&SpiderMineAction,&NoneActionUnit,	//010-014
    &NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&GoliathTAction,&NoneActionUnit,	//015-019
    &NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&TankTAction,	//020-024
    &NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,	//025-029
    &NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&MedicAction,	//030-034
    &LarvaeAction,  &NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,	//035-039
    &NoneActionUnit,&WorkersAction, &TransportAction,&NoneActionUnit,&NoneActionUnit,	//040-044
    &QueenAction,   &NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,	//045-049
    &NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,	//050-054
    &NoneActionUnit,&NoneActionUnit,&TransportAction,&NoneActionUnit,&NoneActionUnit,	//055-059
    &NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&WorkersAction,	//060-064
    &NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&TransportAction,	//065-069
    &NoneActionUnit,&NoneActionUnit,&CarrierAction, &InterceptorsAction,&NoneActionUnit,//070-074
    &NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,	//075-079
    &NoneActionUnit,&NoneActionUnit,&CarrierAction, &NoneActionUnit,&NoneActionUnit,	//080-084
    &NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&CritterAction,	//085-089
    &CritterAction, &NoneActionUnit,&NoneActionUnit,&CritterAction, &CritterAction,	//090-094
    &CritterAction, &CritterAction ,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,	//095-099

    &NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,	//100-104
    &NoneActionUnit,&AirBuildings,  &NoneActionUnit,&NoneActionUnit,&NoneActionUnit,	//105-109
    &NoneActionUnit,&AirBuildings,  &NoneActionUnit,&AirBuildings,  AirBuildings,	//110-114
    &NoneActionUnit,&AirBuildings,  &NoneActionUnit,&NoneActionUnit,&NoneActionUnit,	//115-119
    &NoneActionUnit,&NoneActionUnit,&AirBuildings,  &NoneActionUnit,&NoneActionUnit,	//120-124
    &TransportAction,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,	//125-129
    &AirBuildings,  &HiveAction,    &HiveAction,    &HiveAction,    &NoneActionUnit,	//130-134
    &NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,	//135-139
    &NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,	//140-144
    &NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,	//145-149
    &NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,	//150-154
    &NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,	//155-159
    &NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,	//160-164
    &NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,	//165-169
    &NoneActionUnit,&NoneActionUnit,&BatteryAction, &NoneActionUnit,&NoneActionUnit,	//170-174
    &NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,	//175-179
    &NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,	//180-184
    &NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,	//185-189
    &NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,	//190-194
    &NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,	//195-199

    &NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,	//200-204
    &NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,	//205-209
    &NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,&NoneActionUnit,	//210-214
    &CarryActionUnit,&CarryActionUnit,&CarryActionUnit,&CarryActionUnit,&CarryActionUnit,//215-219
    &ResActionUnit, &ResActionUnit, &ResActionUnit, &ResActionUnit, &ResActionUnit,	//220-224
    &ResActionUnit, &ResActionUnit, &ResActionUnit                   			//225-227

};
//=================================
int AdditionalUnitProceed(OBJ *a,MAIN_IMG *img)
{
    if (a->atackcooldowntime)
	if (--a->atackcooldowntime == 0)
	{
	    if (a->modemove == MODEATACK)
	    {
		if (!a->finalOBJ || a->finalOBJ->modemove == MODEDIE)
		{
		    if (a->prop & VARMOVEINATACKMODE)
		    {
    			moveobj(a,NULL,MODEATACK,a->finalxatack,a->finalyatack,NOSHOWERROR|ATACKMOVEBIT,0);
		    }
		    else
		    {
			if (OneUnitSearchGoal(a,1))
			{
			    a->searchforatack_tick = MAXWAIT_SEARCHATACK_TICK;
			}
			else
			{
			    EndAtackAction(a);
			    SetModeMove(a,MODESTOP);
			    a->finalOBJ=NULL;
			}
		    }
		    return 0;
		}
		switch(AtackCoolDownEnds(a,a->finalOBJ,1,NOSHOWERROR))
		{
		    case MOVEOBJ_NOACT:
			EndAtackAction(a);
		        if (a->prop & VARMOVEINATACKMODE)
		        {
    			    moveobj(a,NULL,MODEATACK,a->finalxatack,a->finalyatack,NOSHOWERROR|ATACKMOVEBIT,0);
			}
			else
			{
			    SetModeMove(a,MODESTOP);
			    a->finalOBJ=NULL;
			}
			return(0);
		    case MOVEOBJ_WAITUNTIL:	//under disruption
			a->atackcooldowntime = GetAtackCoolDown(a,a->usedweapon_id);
			break;
		}
	    }
	}
    if ((a->prop & (VARNOTWORK | VARPOWEROFF))||GetMageAtr(&a->atrobj,ATRSTASIS)>0)
	return 0;
    if (a->prop & VARWAITDISTANCE)
    {
	if (NEEDTOMOVE1BIT)
	{
	    if (a->finalOBJ)
	    {
		if (GetDistanceBetweenUnits256(a,a->finalOBJ) <= (32<<8) )
		{
		    return(0);	//no move, repeat move iscript
		}
		if (a->modemove == MODEATACK)
		    ;//initmoveaction(a,a->finalOBJ,a->modemove,a->usedweapon_id,0,GetOBJx(a->finalOBJ),GetOBJy(a->finalOBJ));
		else
		    initmoveaction(a,a->finalOBJ,a->modemove,0,0,GetOBJx(a->finalOBJ),GetOBJy(a->finalOBJ));
	    }
	    else
	    {
		a->modemove = MODESTOP;
	    }
	    a->prop &= ~VARWAITDISTANCE;
	}
    }
//    if (IsReadyOBJ(a))		// i gorgot abount lair,hive,greater spire than underconstruction but can do additionaproperties
	return ((*UnitActionDefaults[a->SC_Unit])(a,img));
//    return(0);
}
//=================================
int AtackCoolDownEnds(OBJ *a,OBJ *destobj,int continueatack,int showerrorflag)
{
	int resval,errmes;
	//need to check if range is ok for atack again
	resval = IfCanCreateWeapon(a,destobj,&errmes,&a->usedweapon_id,CREATEWEAPON_IGNOREVISION);
	switch(resval)
	{
	    case CREATEDWEAPONSTATUS_UNDERDISRUPTION://atacker in disruption can't atack and no message to appear
		if (!a->atackcooldowntime)
		    a->atackcooldowntime = 1;
		return(MOVEOBJ_WAITUNTIL);
	    case CREATEDWEAPONSTATUS_DESTOUTOFRANGE://out of range
		//if sub unit - tell to base to move inthat direction
		if (IsSubUnit(a->SC_Unit))
		{
		    if (a->subunit->modemove != MODESTOP)
			return(MOVEOBJ_NOACT);
		    a->subunit->modemove = MODEATACK;
		    a->atackcooldowntime = 0;
		    initmoveaction(a->subunit,destobj,MODEATACK,a->usedweapon_id,0,GetOBJx(destobj),GetOBJy(destobj));
		    AddModeMove(a->subunit,destobj,MODEATACKREADY,0,0,NOSHOWERROR);
		}
		else
		{
		    if (!accesstomove(a,loadobj(a->SC_Unit),MODEMOVE,a->playernr) || a->in_transport)
		    {
			if (showerrorflag & SHOWERRORTEXT)
			{
		    	    //invalid target
		    	    Play_sfxdata_id(NULL,TARGETERROR[GetUnitRace(a->SC_Unit)],3,0);
		    	    showadvisortext(errmes);
			}
			return(MOVEOBJ_NOACT);
		    }
		    a->modemove = MODEATACK;
		    a->atackcooldowntime = 0;
		    initmoveaction(a,destobj,MODEATACK,a->usedweapon_id,0,GetOBJx(destobj),GetOBJy(destobj));
		    AddModeMove(a,destobj,MODEATACKREADY,0,0,NOSHOWERROR);
		}
		return(MOVEOBJ_CONTINUEJOB);
	    case CREATEDWEAPONSTATUS_DESTTOCLOSE://to close
	    case CREATEDWEAPONSTATUS_CANTATACKTHISUNIT:
		if (showerrorflag & SHOWERRORTEXT)
		{
		    //invalid target
		    Play_sfxdata_id(NULL,TARGETERROR[GetUnitRace(a->SC_Unit)],3,0);
		    showadvisortext(errmes);
		}
		return(MOVEOBJ_NOACT);
	    case CREATEDWEAPONSTATUS_ATACKSUCCESS:
		a->finalOBJ = destobj;
		SetAtackType(a,destobj);
		if (!a->atackcooldowntime)
		{
		    AtackAction(a,destobj,continueatack);
		    return(MOVEOBJ_DONE);
		}
		return(MOVEOBJ_NOACT);
	    case CREATEDWEAPONSTATUS_ATACKSUCCESSWITHROTATION:
		a->finalOBJ = destobj;
		a->finalx = GetOBJx256(destobj);
		a->finaly = GetOBJy256(destobj);
		SetAtackType(a,destobj);
		if (!a->atackcooldowntime)
		{
		    return(MOVEOBJ_DONE);
		}
		return(MOVEOBJ_CONTINUEJOB);
	    default:
		DEBUGMESSCR("error resval from ifcreateweapon\n");
		return(MOVEOBJ_NOACT);
	}
}
//=================================
void SetAtackType(OBJ *a,OBJ *destobj)
{
    if (IsOnSkyOBJ(destobj))
	a->prop |= VARATACKAIROBJ;
    else
        a->prop &= ~VARATACKAIROBJ;
}
//=================================
void SpecialAtackAction(OBJ *a,OBJ *destobj,int iscriptstate)
{
    a->mainimage->ForceSetIScript(255);
    SetOBJIScriptNr(a,iscriptstate,ISCRIPTNR_SETONLY);
}
//=================================
void AtackAction(OBJ *a,OBJ *destobj,int continueatack)
{
    a->mainimage->ForceSetIScript(255);
    if (continueatack)
    {
	if (a->prop & VARATACKAIROBJ)
	{
    	    SetOBJIScriptNr(a,ISCRIPTNR_AIRATTKRPT,ISCRIPTNR_SETONLY);
	}
	else
	{
    	    SetOBJIScriptNr(a,ISCRIPTNR_GNDATTKRPT,ISCRIPTNR_SETONLY);
	}
    }
    else
    {
	if (a->prop & VARATACKAIROBJ)
	{
    	    SetOBJIScriptNr(a,ISCRIPTNR_AIRATTKINIT,ISCRIPTNR_SETONLY);
	}
	else
	{
    	    SetOBJIScriptNr(a,ISCRIPTNR_GNDATTKINIT,ISCRIPTNR_SETONLY);
	}
    }
}
//=================================
void EndAtackAction(OBJ *a)
{
    a->mainimage->ForceSetIScript(255);
    if (a->prop & VARATACKAIROBJ)
    {
        SetOBJIScriptNr(a,ISCRIPTNR_AIRATTKTOIDLE,ISCRIPTNR_SETONLY);
    }
    else
    {
        SetOBJIScriptNr(a,ISCRIPTNR_GNDATTKTOIDLE,ISCRIPTNR_SETONLY);
    }
//    if (a->prop & VARMOVEINATACKMODE)
//    	moveobj(a,NULL,MODEATACK,a->finalxatack,a->finalyatack,NOSHOWERROR,0);
}
//=================================


