
#include "objs.h"
#include "scripts.h"
#include "load.h"

#include "auxil.h"
#include "gener.h"
#include "diap.h"
#include "maps.h"
#include "upgrade.h"
#include "lists.h"
#include "walk.h"
#include "rand.h"
#include "unitaction.h"
#include "actiondefault.h"
#include "objinfo.h"
#include "market.h"
#include "weapons.h"
#include "iscript.h"
#include "action.h"

//====================================
void PickupObj(OBJ *a,OBJ *pickupobj)
{

    pickupobj->mainimage->elevationlevel = a->mainimage->elevationlevel+1;
    a->carryobj = pickupobj;
    a->carryobj->carryobj=a;
    a->finalOBJ = NULL;
    
    pickupobj->modemove = MODECARRYME;
    pickupobj->prop |= VARCANTSELECT;
	    
    pickupobj->mainimage->side = a->mainimage->side;
    pickupobj->mainimage->neededside = a->mainimage->side;
    //!!!! need to destroy images and send init message
}
//=================================
//callback function for queue
//=================================
void CommandQueueAction(void *IDQueueAction)
{

}
//=================================
void QueueAction(void *IDQueueAction,int DestroyQueue)
{
    int *regen,i,randd;
    OBJ *temp,*actobj,*actdestobj;
    OBJstruct *b;
    int x,y;
    struct QUEUEACTION *act = (struct QUEUEACTION *)IDQueueAction;
    if (DestroyQueue!=DESTROYQUEUE)
    {
	actobj=(OBJ*)act->obj;
	actdestobj=(OBJ*)act->destobj;
	switch(act->actiontype)
	{
	case CREATELARVAEACTION:
	    if (actobj)
	    {
		getcoordofnewunit(actobj,SC_LARVAEOBJ,&x,&y);
		temp = createobjfulllife(x,y,SC_LARVAEOBJ,actobj->playernr);
		addchild(actobj,temp);
		temp->finalx = actobj->finalx;
		temp->finaly = actobj->finaly;
		actobj->prop &= ~VARLARVAEONCONSTRUCT;
	    }
	    break;
	case WAITSOMETIMEACTION:
	    actobj=(OBJ*)act->obj;
	    actdestobj=(OBJ*)act->destobj;
	    ApplyNextModeMove(actobj);
	    break;
	case SELFCREATEACTION:
	    MakeQueueAction(SELFCREATEACTION,actobj,actdestobj,act->param0,act->param1,act->param2,act->param0);
	    regen=(int *)actdestobj;
	    *regen=1;
	    break;
	}
    }
    wfree(act);
}
//=================================
//put obj action in queue
//=================================
void MakeQueueAction(int actiontype,void *a,void *destobj,
		    int param0,
		    int param1,int param2,int afterexpiriedticks)
{
    OBJ *actobj=NULL,*actdestobj=NULL;
    struct QUEUEACTION *act = (struct QUEUEACTION *)wmalloc(sizeof(struct QUEUEACTION ));
    memset(act,0,sizeof(struct QUEUEACTION ));
    act->actiontype = actiontype;
    actobj = (OBJ*)a;
    actdestobj = (OBJ*)destobj;
    act->param0 = param0;
    act->param1 = param1;
    act->param2 = param2;
/*    switch(actiontype)
    {
	case MAGETIMEDACTION:
	case SELFCREATEACTIONION:
	case TIMETOWAITNUKE:
	default:
	    break;
    }
*/
    act->obj=actobj;
    act->destobj=actdestobj;
    if (QueueGame.AddToQueue(act,afterexpiriedticks)==QUEUEFULL)
    {
	wfree(act);
    }
}
//=================================
unsigned char GetMotherAddon(SCUNIT SC_Unit)
{
    switch(SC_Unit)
    {
	case SC_COMSATSTATIONOBJ:
	case SC_NUCLEARSILOOBJ:
    	     return(SC_COMMCENTEROBJ);
        case SC_MACHINESHOPOBJ:
	    return(SC_FACTORYOBJ);
        case SC_CONTROLTOWEROBJ:
	    return(SC_STARPORTOBJ);
    	case SC_COVERTOPSOBJ:
	case SC_PHYSICSLABOBJ:
    	    return(SC_SCIENCEFACILITYOBJ);
	default:
	    return(0);
    }
}
//=================================
void GetAddonType(SCUNIT SC_Unit,unsigned char *addontype1,unsigned char *addontype2)
{
    switch(SC_Unit)
    {
	case SC_COMMCENTEROBJ:
    	    *addontype1=SC_COMSATSTATIONOBJ;
    	    *addontype2=SC_NUCLEARSILOOBJ;
	    break;
        case SC_FACTORYOBJ:
    	    *addontype1=SC_MACHINESHOPOBJ;
    	    *addontype2=0;
	    break;
        case SC_STARPORTOBJ:
    	    *addontype1=SC_CONTROLTOWEROBJ;
    	    *addontype2=0;
	    break;
        case SC_SCIENCEFACILITYOBJ:
    	    *addontype1=SC_COVERTOPSOBJ;
    	    *addontype2=SC_PHYSICSLABOBJ;
	    break;
	default:
    	    *addontype1 = 0;
    	    *addontype2 = 0;
	    break;
    }
}
//=================================
void AttachAddonDeltaCoords(unsigned char base_SC_Unit,unsigned char addon_SC_Unit,int *deltax,int *deltay)
{
    *deltax = - GetUnitBoxWidth(base_SC_Unit)/2  + GetDeltaAddonHorizontal(addon_SC_Unit) + GetUnitBoxWidth(addon_SC_Unit)/2;
    *deltay = - GetUnitBoxHeight(base_SC_Unit)/2 + GetDeltaAddonVertical(addon_SC_Unit)   + GetUnitBoxHeight(addon_SC_Unit)/2;
}
//=================================
int AttachAddonToBuild(OBJ *a)
{
    int nukes,deltax,deltay;
    OBJ *a2;
    unsigned char motherbuild = GetMotherAddon(a->SC_Unit);
    if (!motherbuild)
	return(ADDON_ERROR_ATTACH);

    AttachAddonDeltaCoords(motherbuild,a->SC_Unit,&deltax,&deltay);
    a2 = SearchForObjInXY(GetOBJx(a)-deltax,GetOBJy(a)-deltay,&motherbuild,1);
    if (a2)
    {
	a->playernr = a2->playernr;
	a->color = a2->color;
	if (a->SC_Unit == SC_NUCLEARSILOOBJ)
	{
	    nukes = getchilds(a);
	    if (nukes)
	    {
		PLAYER[a->playernr].nukes += nukes;
		ChangeSupply(a->playernr,SC_NUKEOBJ,PLUSFACTOR);
	    }
	}
	a2->addonobj = a;
	a->addonobj = a2;
	SetOBJIScriptNr(a,ISCRIPTNR_LANDING,ISCRIPTNR_EXECUTE);	//connect script
	return(ADDON_SUCCESS_ATTACH);
    }
    else
    {
	return(ADDON_ERROR_ATTACH);
    }
}
//=================================
int AttachNearestAddon(OBJ *a)
{
    int nukes,deltax,deltay;
    unsigned char addontype[2];
    OBJ *a2;
    GetAddonType(a->SC_Unit,&addontype[0],&addontype[1]);
    if (!addontype[0])
	return(ADDON_ERROR_ATTACH);

    AttachAddonDeltaCoords(a->SC_Unit,addontype[0],&deltax,&deltay);

    a2 = SearchForObjInXY(GetOBJx(a)+deltax,GetOBJy(a)+deltay,addontype,2);
    if (a2)
    {
	a2->playernr = a->playernr;
	a2->color = a->color;
	if (a2->SC_Unit == SC_NUCLEARSILOOBJ)
	{
	    nukes=getchilds(a2);
	    if (nukes)
	    {
		PLAYER[a2->playernr].nukes += nukes;
		ChangeSupply(a2->playernr,SC_NUKEOBJ,PLUSFACTOR);
	    }
	}
	a->addonobj = a2;
	a2->addonobj = a;
	SetOBJIScriptNr(a2,ISCRIPTNR_LANDING,ISCRIPTNR_SETONLY);	//connect script
	return(ADDON_SUCCESS_ATTACH);
    }
    else
    {
	return(ADDON_ERROR_ATTACH);
    }
}
//=================================
void DettachAddonFromBuild(OBJ *a)
{
    int nukes;
    if (a->SC_Unit == SC_NUCLEARSILOOBJ)
    {
	nukes = getchilds(a);
	if (nukes)
	{
	    PLAYER[a->playernr].nukes -= nukes;
	    ChangeSupply(a->playernr,SC_NUKEOBJ,MINUSFACTOR);
	}
    }
    a->playernr = GREYNEUTRALCOLORPLAYER;
}
//=================================
void DettachAddon(OBJ *a)
{
    if ( a->addonobj )
    {
	if (IsAddon(a->SC_Unit))
	{
	    DettachAddonFromBuild(a);
	    SetOBJIScriptNr(a,ISCRIPTNR_LIFTOFF,ISCRIPTNR_SETONLY);		//dettach script
	}
	else
	{
	    DettachAddonFromBuild(a->addonobj);
	    SetOBJIScriptNr(a->addonobj,ISCRIPTNR_LIFTOFF,ISCRIPTNR_SETONLY);	//dettach script
	}
	a->addonobj->addonobj = NULL;
	a->addonobj = NULL;
    }
}
//=================================
//=================================
//return 0 - if not satisfy all dependency
//=================================
int MageDepend(OBJ *a,int playernr,int modemovetype)
{
    int i;
    if (CODEFORWITHOUTTECHTREE)
	return(1);
    if (IsHeroUnit(a->SC_Unit))
	return(1);
    int maxdepend=mageprop[modemovetype].depend.maxdepend;
    //if modemove id upgrade we need to check all dependency
    if (mageprop[modemovetype].type_id==ORDERS_UPGRADE)
    {
	i=GetUpgradeTree(&map,playernr,mageprop[modemovetype].obj_id);
	switch(mageprop[modemovetype].depend.type_id[i])
	{
	    case ORDERS_NONE:
	        break;
	    case ORDERS_UNIT:	//check depend on exist someunit
		if (mageprop[modemovetype].depend.obj_id[i]==SC_NOUNITNR)//no unit dependancy
		    break;
		if (!FindSC_UnitType(a,playernr,mageprop[modemovetype].depend.obj_id[i],SC_NOUNITNR))
    	    	    return(0);
		break;
	    case ORDERS_UPGRADE:	//check depend on upgrade
		    if (!GetUpgradeTree(&map,playernr,mageprop[modemovetype].depend.obj_id[i]))
			return(0);
		    break;
	    case ORDERS_TECHNOLOGY:	//check depend on technology
		    if (!GetTechTree(&map,playernr,mageprop[modemovetype].depend.obj_id[i]))
			return(0);
		    break;
	}
	return(1);
    }
    for (i=0;i<maxdepend;i++)
    {
        switch(mageprop[modemovetype].depend.type_id[i])
        {
	    case ORDERS_NONE:
		break;
	    case ORDERS_UNIT:	//check depend on exist someunit
		if (mageprop[modemovetype].depend.obj_id[i]==SC_NOUNITNR)	//no unit dependancy
		    break;
		//if addon check only for addon build
    		if (IsAddon(mageprop[modemovetype].depend.obj_id[i]))
		{
		    if (a->addonobj)
		    {
			if (a->addonobj->SC_Unit==mageprop[modemovetype].depend.obj_id[i])
			    break;
		    }
	    	    return(0);
	        }
		else
    		    if (mageprop[modemovetype].depend.obj_id2[i]==SC_NOUNITNR)
		    {
			//check only one build
			if (!FindSC_UnitType(a,playernr,mageprop[modemovetype].depend.obj_id[i],SC_NOUNITNR))
    			    return(0);
		    }
		    else
		    {
			//check build with addon
			if (!FindSC_UnitType(a,playernr,mageprop[modemovetype].depend.obj_id[i],
			    mageprop[modemovetype].depend.obj_id2[i]))
    			    return(0);
		    }
		break;
	    case ORDERS_UPGRADE:	//check depend on upgrade
		if (!GetUpgradeTree(&map,playernr,mageprop[modemovetype].depend.obj_id[i]))
		    return(0);
		break;
	    case ORDERS_TECHNOLOGY:	//check depend on technology
		if (!GetTechTree(&map,playernr,mageprop[modemovetype].depend.obj_id[i]))
		    return(0);
	    break;
        }
    }
    return(1);
}
//=================================
AROUNDBUILD unitaroundbuild[MAXAROUNDARRAY];
//=================================
int GetUnitConstrMinXY(int pos,int *x,int *y)
{
    *x=unitaroundbuild[pos].deltax;
    *y=unitaroundbuild[pos].deltay;
    return(unitaroundbuild[pos].method);
}
//=================================
void SetUnitConstrMinXY(int pos,int x,int y,int method)
{
    unitaroundbuild[pos].deltax=x;
    unitaroundbuild[pos].deltay=y;
    unitaroundbuild[pos].method=method;
}
//=================================
int methoddecrxy[4][2]={{1,0},{0,-1},{-1,0},{0,1}};
//=================================
//sizexb=(128/8-L-R)*2
//sizeyb=(96/8-U-D)*2
#define LBUILD	1
#define RBUILD	1
#define UBUILD	1
#define DBUILD	1
#define SIZEXB (((128/8-LBUILD-RBUILD)*2))	
#define SIZEYB (((96/8-UBUILD-DBUILD)*2))
//=================================
void getcoordofnewunit(struct OBJ *a,int nrunit,int *x,int *y)
{
    int xbeg,ybeg,i,xx,yy,beginpos,deltax,deltay,method,x8pos,y8pos;
    int fromoffset=0;
    if (IsAirUnit(nrunit))
    {
	*x = GetOBJx(a);
	*y = GetOBJy(a);
    }    
    else
    {
	xbeg = GetOBJx(a)/8;
	ybeg = GetOBJy(a)/8;
	if (IsBuild(a->SC_Unit))
	    fromoffset=SIZEXB*SIZEYB+14;
	for (i=fromoffset;i<MAXAROUNDARRAY;i++)
	{
	    method=GetUnitConstrMinXY(i,&xx,&yy);
	    x8pos=xbeg+xx;
	    y8pos=ybeg+yy;
    	    if (GetMapWalk8(x8pos,y8pos,GetUnitWidthAndHeight(nrunit,UNITDIM_WIDTH)/8,GetUnitWidthAndHeight(nrunit,UNITDIM_HEIGHT)/8))
	    {
		*x=x8pos*8+GetUnitWidthAndHeight(nrunit,UNITDIM_WIDTH)/2;
		*y=y8pos*8+GetUnitWidthAndHeight(nrunit,UNITDIM_HEIGHT)/2;
		return;
	    }
	}
    }
}
//=================================
void getcoordofnewunit(int nrunit,int *x,int *y,struct XY *area)
{
    int xbeg,ybeg,i,xx,yy,beginpos,deltax,deltay,method,x8pos,y8pos;
    OBJstruct *b2=loadobj(nrunit);
    if (IsAirUnit(nrunit))
    {
	CenterXYArea(area,x,y);
    }
    else
    {
	CenterXYArea(area,x,y);
	xbeg=*x/8;
	ybeg=*y/8;
	for (i=0;i<MAXAROUNDARRAY;i++)
	{
	    method=GetUnitConstrMinXY(i,&xx,&yy);
	    x8pos=xbeg+xx;
	    y8pos=ybeg+yy;
    	    if (GetMapWalk8(x8pos,y8pos,GetUnitWidthAndHeight(nrunit,UNITDIM_WIDTH)/8,GetUnitWidthAndHeight(nrunit,UNITDIM_HEIGHT)/8))
	    {
		*x=x8pos*8;
		*y=y8pos*8;
		return;
	    }
	}
    }
}
//=================================
void SetAroundTable(void)
{
    int i,j,posx,posy,prevx,prevy,method=0,prevmethod,posintable=0,pos=2;
    int deltax,deltay;
    int usedarraybuild[AROUNDTABLEY][AROUNDTABLEX];
    memset(usedarraybuild,0,sizeof(usedarraybuild));
    
/*    for (i=0;i<SIZEYB;i++)
    {
	for (j=0;j<SIZEXB;j++)
	{
	    if (i==0||i==SIZEYB-1)
		if (j==0||j==SIZEXB-1)
		    continue;
	    usedarraybuild[(AROUNDTABLEY-SIZEYB)/2+i][(AROUNDTABLEX-SIZEXB)/2+j]=1;
	}
    }

    posx=(AROUNDTABLEX-SIZEXB)/2-1;
    posy=(AROUNDTABLEY+SIZEYB)/2-DBUILD*2;
*/
    posx=AROUNDTABLEX/2-1;
    posy=AROUNDTABLEY/2;
    do{
	deltax=methoddecrxy[method][0];
	deltay=methoddecrxy[method][1];
	prevx=posx;
	prevy=posy;
	posx+=deltax;
	posy+=deltay;
	if (posx>=AROUNDTABLEX||posy>=AROUNDTABLEY||posx<0||posy<0)
	    break;
	if (!usedarraybuild[posy][posx])
	{
	    usedarraybuild[posy][posx]=pos++;
	    SetUnitConstrMinXY(posintable++,(posx-AROUNDTABLEX/2)/2,(posy-AROUNDTABLEY/2)/2,method);
	    if (++method>3)
		method=0;
	}
	else
	{
	    if (--method<0)
		method=3;
	    posx=prevx;
	    posy=prevy;
	}
    }while(1);
/*    FILE *f=fopen("txt.build","wb");
    unsigned short indextile32;
    for (i=0;i<AROUNDTABLEY;i++)
    {
        for (j=0;j<AROUNDTABLEX;j++)
            fprintf(f,"-%04d-",usedarraybuild[i][j]);
        fprintf(f,"\n");
    }
    fclose(f);
*/
}
//=================================
#define NUKEDOT_ELEVATIONLEVEL	5
int CreateNuke(OBJ *a,int x,int y)
{
    MAIN_IMG *newimg;
    OBJ *nuke;
    int i,nukestartx,nukestarty;
    for (i=0;i<MaxObjects;i++)
    {
	if (objects[i]->SC_Unit == SC_NUCLEARSILOOBJ 	&& 
	    objects[i]->playernr == a->playernr		&&
	    !(objects[i]->prop & VARPOWEROFF)) 
	{
	    if (getchilds(objects[i]))//check if we have bomb in nukesilo
	    {
		//nuke already in nuclear silo
		nuke = objects[i]->childs->parentof[0];
		nukestartx = GetOBJx(objects[i]);
		nukestarty = GetOBJy(objects[i]);
		SetOBJxy256(nuke,nukestartx<<8,(nukestarty-INITIAL_NUKE_YPOS)<<8);
//		nuke->playernr = objects[i]->playernr;
		nuke->playernr = GREYNEUTRALCOLORPLAYER;
		nuke->prop &= ~(VARNOTHERE | VARINBASE);
		nuke->finalOBJ = NULL;
		nuke->prop |= (VARREADY | VARCANTSELECT);
		nuke->mainimage->EnableExecScript();
	        nuke->mainimage->ShowChildsImgFlag();
    		nuke->mainimage->ShowImgFlag();
		//sound nuke launch from silo
		WeaponPlaySFX(nuke,SFXDATA_LAUNCHNUKE,0,GetOBJx(nuke),GetOBJy(nuke));
		moveobj(nuke,NULL,MODEMOVE,nukestartx,nukestarty-NUKE_MAXAIR_YPOS,NOSHOWERROR);
		AddModeMove(nuke,NULL,MODENUKEWAITFORLANDING,x,y-NUKE_MAXAIR_YPOS,0);
		SetOnSkyOBJ(nuke,1);
		a->mainimage->flags |= SC_IMAGE_FLAG_AIRIMG;
		nuke->doubleunit = a;
		a->doubleunit = nuke;
		a->memoryformage=0;
		delchild(objects[i],objects[i]->childs->parentof[0]);
		PLAYER[a->playernr].nukes--;
		ChangeSupply(a->playernr,SC_NUKEOBJ,MINUSFACTOR);

		//play nuclear launch detected
//		playinfoadvisorsound(a->playernr,gameconf.pl_race[NUMBGAMER],ADVNUCLEARDETECT,PLAYADVISOR_TEXTANDSOUND);
		//create nuke target dot

		newimg = new MAIN_IMG(IMAGEID_NUKE_TARGET_DOT,x<<8,y<<8,NUKEDOT_ELEVATIONLEVEL,0,0,0,0,0,ISCRIPTNR_INIT);
		newimg->whocreate = SC_IMAGE_SELF_CREATOR;
		mainimageslist.AddElem(newimg);
		nuke->data.nukehitdot.hitdot = newimg;
		return(0);
	    }
	}
    }
    return(-1);
}
//=================================
void addremoveuniteffectfrommap(OBJ *a,int plusminus,mapinfo *testmap)
{
    int i,j,x,y,sx1,sy1,sx2,sy2,mageobjnr;
    x = GetOBJx(a);
    y = GetOBJy(a);
    switch(a->SC_Unit)
    {
        case SC_MISC_DISRUPTIONWEB:
    	    mageobjnr = DISRUPTEFFECT;
    	    sx1 = x/32 - 2;
    	    sx2 = x/32 + 1;		
    	    if (x%32 > 16)
    	    {
    		sx2++;			//the position is more to the right, need to change map position +1 to right
    		sx1++;
    	    }
    	    sy2 = y/32 + 1;
    	    sy1 = y/32 - 1;
    	    break;
        case SC_MISC_DARKSWARM:
    	    mageobjnr = DSWARMEFFECT;
    	    sx1 = x/32 - 3;
    	    sx2 = x/32 + 2;		
    	    if (x%32 > 16)
    	    {
    		sx2++;			//the position is more to the right, need to change map position +1 to right
    		sx1++;
    	    }
    	    sy1 = y/32 - 3;
    	    sy2 = y/32 + 2;		
    	    if (y%32 > 16)
    	    {
    		sy2++;			//the position is more to the bottom, need to change map position +1 to bottom
    		sy1++;
    	    }
    	    break;
        default:
	    return;
    }
    for (i=sy1;i<=sy2;i++)
    {
	if (i>=0 && i<MAXYMAP)
	    for (j=sx1;j<=sx2;j++)
	    {
		if (j>=0 && j<MAXXMAP)
		    testmap->mapbits.mageeffect[mageobjnr][i*MAXXMAP+j] += plusminus;//add|remove one effect
	    }
    }
}
//=================================
//SCV repair tick goes here
//return 0 - if complete repairing or not neede repair
//=================================
int RepairUnit(int playernr,OBJ *scv,OBJ *a)
{
    OBJstruct *b;
    if (a->health < GetUnitMaxHealth(a->SC_Unit))
    {
	b=loadobj(a->SC_Unit);
	//check if have enough resources for repair
	if (CheckResourcePlayerBITS(playernr,b->repairmin * CODEFORQUICKMAKE,b->repairgas * CODEFORQUICKMAKE) == CHECKRES_OK)
	{
	    a->health += b->repairlife * CODEFORQUICKMAKE;
	    AddRemoveBloodFlameOverlays(a);	//change overlay blood|flame
	    
	    ChangeResourcePlayerBITS(playernr,MINUSFACTOR,b->repairmin * CODEFORQUICKMAKE,b->repairgas * CODEFORQUICKMAKE);
	    if (a->health < GetUnitMaxHealth(a->SC_Unit))
		return(1);
	    a->health = GetUnitMaxHealth(a->SC_Unit);
	}
    }
    return(0);
}
//=================================




