
#include <math.h>

#include "auxil.h"
#include "debug.h"
#include "mouse.h"
#include "man.h"
#include "vars.h"
#include "putobj.h"
#include "lists.h"
#include "mage.h"
#include "gener.h"
#include "gr.h"
#include "wmem.h"
#include "load.h"
#include "diap.h"
#include "maps.h"
#include "audio.h"
#include "creep.h"
#include "actiondefault.h"
#include "action.h"
#include "pylon.h"
#include "unitaction.h"
#include "flingy.h"
#include "walk.h"
#include "upgrade.h"
#include "objinfo.h"
#include "rand.h"
#include "pylon.h"
#include "tempobj.h"
#include "doodad.h"
#include "construct.h"
#include "scripts.h"
#include "market.h"
#include "portrait.h"
#include "objinfo.h"
#include "weapons.h"
#include "starmap.h"
#include "images.h"
#include "iscript.h"
#include "overlay.h"
#include "sigorder.h"
#include "objs.h"

#ifdef WITHSDL
       #include "sdl/grsdl.h"
       #include "sdl/keysdl.h"
#endif
#ifdef UNDERDOS
       #include "dos/handlers.h"
       #include "dos/keydos.h"
#endif

//===========================================
    struct OBJ *ObjsRevealMap[MAXOBJDEFSCR];
    int MaxObjsRevealMap;

//==========================================
//Unicitojiti OBJECT
void destroyobj(struct OBJ *a)
{
    struct OBJ *temp1,*temp2;
    struct OBJ *newobj;
    struct OBJstruct *newobjstr;
    if (a)
    {
	deselectobj(a);
	if (a->childs)
	{
	    for (int i=0;i<MAXCHILDS;i++)
	    {
		if (a->childs->parentof[i])
		{
		    if (a->childs->parentof[i]->modemove==MODERECHARGE)
			destroyobj(a->childs->parentof[i]);
		    if (a->childs->parentof[i])
		    {
			QueueDelObj(a->childs->parentof[i]);
			delchild(a,a->childs->parentof[i]);
		    }
		}
	    }
	    wfree(a->childs);	//destroy struct CHILDS
	    a->childs = NULL;
	}
//delete me from my parent list of childs
	struct OBJstruct *b=loadobj(a->SC_Unit);
	if (a->myparent)
	{
	    delchild(a->myparent,a);
	}
	if (a->loaded)
	{
	    wfree(a->loaded);
	    a->loaded = NULL;
	}
	
	weaponflingy.RemoveOBJFromFlingyobj(a);

	DeleteOldObjPointers(a);

	delobjlist(a);
	MinObjRegen=MaxObjects/MAPREGENERATION;
	if (MaxObjects%MAPREGENERATION)
	    MinObjRegen++;

	DelAllModeMoves(a);
	wfree(a);
	createobjregen();
    }
}
//==========================================
OBJ *CreateArchon(OBJ *a,OBJ *a2)
{
    OBJ *newobj;
    int midlife,midshield;
    switch(a->SC_Unit)
    {
	case SC_HIGHTEMPLAROBJ:
	    midlife=(int)(a->life+a2->life)*100/(GetUnitMaxLife(a->SC_Unit))/2;
	    midshield=(int)(a->shield+a2->shield)*100/GetUnitMaxShield(a->SC_Unit)/2;
	    newobj = createobjman(GetOBJx(a),GetOBJy(a),SC_ARCHONOBJ,a->playernr,TYPEMOVEATBEGIN,midshield,midlife,100);
	    break;
	case SC_DARKTEMPLAROBJ:
	    midlife=(int)(a->life+a2->life)*100/(GetUnitMaxLife(a->SC_Unit))/2;
	    midshield=(int)(a->shield+a2->shield)*100/GetUnitMaxShield(a->SC_Unit)/2;
	    newobj = createobjman(GetOBJx(a),GetOBJy(a),SC_DARKARCHONOBJ,a->playernr,TYPEMOVEATBEGIN,midshield,midlife,100);
	    break;
	default:
	    printf("error occured obj is not templars\n");
	    break;
    }
//    if (a->select[a->playernr]&VARSELECT||a2->select[a->playernr]&VARSELECT)
//	selectobj(newobj);
    dieobj_silently(a);
    dieobj_silently(a2);
    SetOBJIScriptNr(newobj,ISCRIPTNR_SPECIALSTATE1,ISCRIPTNR_EXECUTE);	//set warping archon
    SetBeginSelfConstruct(newobj);
    newobj->selfconstruct.currentconstrstep = CONSTR_STEP_WARPARCHONS;
    return(newobj);
}
//==========================================
struct OBJ *createobjfulllife(int x,int y,int SC_Unit,int player)
{
    return(createobjmanwithlife(x,y,SC_Unit,player,100,100,100));
}
//==========================================
//sozdati OBJECT
struct OBJ *createobjmanwithlife(int x,int y,int SC_Unit,int Pl,
				 int persshield,int perslife,int persenergy)
{
    OBJ *a;
    OBJ *a2=NULL;
    OBJstruct *b;
    switch(SC_Unit)
    {
	case SC_ASSIMILATOROBJ:
	case SC_EXTRACTOROBJ:
	case SC_REFINERYOBJ:
	    a2 = GetGEYSERfromMAP(x/SIZESPRLANSHX,y/SIZESPRLANSHY);
    	    if (!a2)
	    {
		a2 = CreateGeyserOnMap(x,y,SC_GEYSEROBJ,Pl,persenergy>>8,persenergy>>8,&map);
	    }
	    if (!a2)
	    {
		return(NULL);
	    }
	    break;
	case SC_MINERAL1OBJ:
	case SC_MINERAL2OBJ:
	case SC_MINERAL3OBJ:
	    if (persenergy > 0xff)
		return(CreateMineralsOnMap(x,y,SC_GEYSEROBJ,Pl,persenergy,persenergy>>8,&map));
	    break;
	case SC_GEYSEROBJ:
	    if (persenergy > 0xff)
	        return (CreateGeyserOnMap(x,y,SC_GEYSEROBJ,Pl,persenergy,persenergy>>8,&map));
	    break;
    }
    a = createobjman(x,y,SC_Unit,Pl,1);
    if (a2)
    {
	GeyserDisactivate(a,a2);
	a2->mainimage->HideChildsImgFlag();
	a2->mainimage->HideImgFlag();
    }
    b = loadobj(a->SC_Unit);
    AdditionalProperties(a,b);
    a->prop |= VARREADY;
    if (b)
    {
	SetUnitPercentLife(a,perslife);
	SetUnitPercentShield(a,persshield);
	if (IsSpellCaster(SC_Unit))
	    SetUnitPercentMana(a,persenergy);
    }
    ChangeTypeOfProp(a,b,PROPNORMAL1);

    a->prop |= VARKARTCHANGES;
    makeoneobjseeopen(a,b);
    calcfullinvandseeobj(a);

    if (b->creeptype!=NOCREEPBUILD)//if have creep, maximize the creep at once
    {
    	FillWithCreepNow(a,b,MAXCREEPTABLE);
    }
    ChangeSupply(a->playernr,SC_Unit,PLUSFACTOR);
    ChangeMaxSupply(a->playernr,SC_Unit,PLUSFACTOR);
    if (SC_Unit==SC_PYLONOBJ)
	AddPylonArea(&map,a,a->playernr);
    else
	CheckBuildForPower(&map,a,a->playernr);
    //if addon - need to atach mother build
    if (IsAddon(SC_Unit))
    {
	if (AttachAddonToBuild(a) == ADDON_ERROR_ATTACH)
	{
	    a->playernr = GREYNEUTRALCOLORPLAYER;
	}
    }
    else
    {
	AttachNearestAddon(a);
    }
//    a->lastdamageoverlays = -1;
    AddRemoveBloodFlameOverlays(a);
    add_unit_stat(&map,UNITSTAT_HAVE,a->playernr,a->SC_Unit);
    addremoveuniteffectfrommap(a,1,&map);
    return (a);
}
//==========================================
//sozdati OBJECT
struct OBJ *createobjman(int x,int y,int SC_Unit,int playernr)
{
    return(createobjman(x,y,SC_Unit,playernr,0));
}
//==========================================
//sozdati OBJECT
struct OBJ *createobjman(int x,int y,int SC_Unit,int playernr,int readyatbegin)
{
    return (createobjman(x,y,SC_Unit,playernr,readyatbegin,100,100,100));
}
//==========================================
struct OBJ *createobjman(int x,int y,int SC_Unit,int playernr,int readyatbegin,int persshield,int perslife,int energypers)
{
    return(createobjlowlevel(NULL,x,y,SC_Unit,playernr,readyatbegin,persshield,perslife,energypers));
}
//==========================================
struct OBJ *createreschunk(OBJ *workerobj,int x,int y,unsigned char SC_Unit)
{
    return(createobjlowlevel(workerobj,x,y,SC_Unit,GREYNEUTRALCOLORPLAYER,1,100,100,30));
}
//==========================================
//primary function to create an object
struct OBJ *createobjlowlevel(OBJ *workerobj,int x,int y,int SC_Unit,int playernr,int readyatbegin,int persshield,int perslife,int energypers)
{
    int i;
    OBJ *a;
    OBJ *a2=NULL;
    OBJstruct *b;
    if (SC_Unit==SC_ASSIMILATOROBJ || SC_Unit==SC_EXTRACTOROBJ || SC_Unit==SC_REFINERYOBJ)
    {
	a2=GetGEYSERfromMAP(x/SIZESPRLANSHX,y/SIZESPRLANSHY);
	if (!a2)
	    return(NULL);
    }

    a = (struct OBJ *)wmalloc(sizeof(struct OBJ ));
    if (!a)
	return(NULL);
    memset(a,0,sizeof(struct OBJ ));
    if (a2)
    {
	GeyserDisactivate(a,a2);
    }

    b=loadobj(SC_Unit);
    if (IsDoodadState(SC_Unit))
    {
	if (IsInvincibleUnit(SC_Unit))
	    playernr = GREYNEUTRALCOLORPLAYER;//doodad door,etc
    }
    else
    {
	if (IsGroupNeutralFlag(SC_Unit) && !IsNonNeutralFlag(SC_Unit))
	    playernr = GREYNEUTRALCOLORPLAYER;//disruptwave,darkswarm
	else
	    if (playernr>=PLAYEDPLAYERS&&playernr!=GREYNEUTRALCOLORPLAYER)
		playernr=GREYNEUTRALCOLORPLAYER;
    }	    

//    SetOrder(a,4,&SIGOrder_UnitInitComplete);

    a->startx = x;
    a->starty = y;
    a->lastdamageoverlays=-1;
    a->SC_Unit=SC_Unit;
    a->SC_ConstrUnit=SC_NOUNITNR;
    a->SC_FromUnit = b->SC_CreateFromUnit;
    if (a->SC_FromUnit != SC_NOUNITNR)
    {
	a->templife = GetUnitMaxLife(a->SC_FromUnit);
	a->tempshield = GetUnitMaxShield(a->SC_FromUnit);
    }
    a->creepbuildlist_ID=CREEPLISTEMPTY;
    addobjlist(a);
    MinObjRegen=MaxObjects/MAPREGENERATION;
    if (MaxObjects%MAPREGENERATION)
        MinObjRegen++;
    for (i=0;i<PLAYEDPLAYERS;i++)
        a->select[i]=b->UNITprop&(VARINVSEE|VARSEE|VAR_DX|VAR2UPDOWN);
    
    if (IsInvincibleUnit(a->SC_Unit))
	SetInvincibleOBJ(a,1);
    if (IsAirUnit(a->SC_Unit))
	SetOnSkyOBJ(a,1);
    a->color=PLAYER[playernr].colorRACE;
    a->playernr=playernr;
    if (IsSpellCaster(a->SC_Unit))//&&(GetMageAtr(&a->atrobj,ATRHALLUCINATION)==0))
    {
	SetUnitPercentMana(a,33);
    }
//    ChangeObjXY(a,x,y);

    a->timeleft=b->maxtimeleft;
    if (!IsBuild(a->SC_Unit))
    {
	SetUnitPercentLife(a,perslife);
//	if (IsShieldEnable(SC_Unit))
	    SetUnitPercentShield(a,persshield);
    }
    else
    {
	a->shield=STARTBUILDSHIELD;
	a->life=STARTBUILDLIFE;
    }
    createobjregen();
//    a->curentatacknr = b->maxbullets;
    a->timeflybeforerecharge = b->timeflybeforerecharge;
    a->sfxplay.sfxtypeplayed = -1;
    a->sfxplay.nrsfxplayed = -1;
    a->sfxplay.nrselectplayed = 0;
    if (b->UNITprop & VARSELECT4NOTSEEPROP)
	a->prop |= VARNOTSEEPROP;
    ChangeTypeOfProp(a,b,PROPDECONSTRUCT);
    if (workerobj)
    {
	//carry unit to worker
	workerobj->carryobj = a;
	a->carryobj = workerobj;
    }
    if (IsDoodadState(a->SC_Unit))
    {
	CreateImageAndAddToList(a,x<<8,y<<8,5);//5 is for doodadunit and no execute scripts
//	iscriptinfo.ExecuteScript(a->image);
    }
    else
    {
	CreateImageAndAddToList(a,x<<8,y<<8,readyatbegin);
//	iscriptinfo.ExecuteScript(a->image);
    }
    ForceKartChanges(a);
    if (a->xkart>=MAXXMAP||a->ykart>=MAXYMAP)
    {
	sprintf(forexit,"coords error UNIT=%d (mapx,mapy)=(%d,%d)\n",SC_Unit,a->xkart,a->ykart);
	gameend(forexit);
    }
    calcfullinvandseeobj(a);
    if (!IsOnSkyOBJ(a))
	SetObjWalk8(&map,a,SETWALK);
    a->createcycle=gamecycle;
    if (IsPermanentCloak(a->SC_Unit))
	SetMageAtr(&a->atrobj,ATRINVISIBLE,ATRMAGEINFINITE);
    if (IsDetector(a->SC_Unit))
	SetMageAtr(&a->atrobj,ATRDETECTOR,ATRMAGEINFINITE);
    if (GetMageAtr(&a->atrobj,ATRHALLUCINATION)==0)
    {
    	if (TRIG_ChangeStat)
	    if (IsBuild(a->SC_Unit))
    		PLAYER[playernr].statplayer.stat[STATPLAYER_BUILDSCONSTRUCTED]+=GetBuildUnitScore(a->SC_Unit);
	    else
    		PLAYER[playernr].statplayer.stat[STATPLAYER_UNITSPRODUCED]+=GetBuildUnitScore(a->SC_Unit);
    }
    a->atackernr=-1;
    a->whoatack=NULL;
    SetAtackTick(a);
    add_unit_stat(&map,UNITSTAT_PRODUCING,a->playernr,a->SC_Unit);
    SetModeMove(a,MODESTOP);
    if (IsPickupUnit(a->SC_Unit) && !a->carryobj)
    {
	//try to attach to near worker if any
	a2 = SearchUnitFunc(a->playernr,&IsWorkerUnit,x,y,24);
	if (a2)
	    PickupObj(a2,a);
    }
    return(a);
}
//=====================================
void GeyserDisactivate(OBJ *a,OBJ *geyser)
{
    a->replaceobj = geyser;
    geyser->replaceobj = a;
    geyser->prop |= (VARCANTSELECT | VARNOTWORK);
    a->data.resource.resource_count = geyser->data.resource.resource_count;
    a->data.resource.max_resource_count = geyser->data.resource.max_resource_count;
    
    geyser->mainimage->DisableExecScript();//disable all overlays exec scripts
    
}
//=====================================
void LoadSubUnitLoFile(OBJ *a,OBJstruct *b)
{
    unsigned char flingy_id;
    short sprites_id,images_id,overlay_id;
    flingy_id = alldattbl.units_dat->flingy_id[a->SC_Unit];
    sprites_id = alldattbl.flingy_dat->sprites_id[flingy_id];
    images_id = alldattbl.sprites_dat->images_id[sprites_id];
    GetLoadedImage(alldattbl.images_dat->Special_Overlay[images_id],(void **)&b->subunit_lo);
}
//=====================================
static int selectbuilds[4],selectunits[4];
//=====================================
//=====================================
void deselectallexcludeone(struct OBJ *a1)
{
    struct OBJ *a;
    for (int i=0;i<MaxObjects;i++)
    {
       a=objects[i];
       if (a != a1)
          if (ifselectedOBJbit(a,NUMBGAMER))
          {
             doselectedOBJbit(a,NUMBGAMER,0);
             deselectobj(a);
          }
    }
}
//=====================================
void deselectallexcludeonetypeobj(struct OBJ *a1)
{
    struct OBJ *a;
    for (int i=0;i<MaxObjects;i++)
    {
       a=objects[i];
       if (a->SC_Unit!=a1->SC_Unit)
          if (ifselectedOBJbit(a,NUMBGAMER))
          {
             doselectedOBJbit(a,NUMBGAMER,0);
             deselectobj(a);
          }
    }
}
//=====================================
void DetectIfAnyPylonOnSelected(void)
{
#ifdef DESENPYLONAREA
    unsigned char SC_Unit;
    pylonselected = 0;
    if (buildconstr != SC_NOUNITNR)
    {
	SC_Unit=buildconstr;
	if (GetUnitRace(SC_Unit)==PROTOSSRACE)
	    switch(SC_Unit)
	    {
		case SC_NEXUSOBJ:
		case SC_ASSIMILATOROBJ:
//		case SC_PYLONOBJ:
		    break;
		default:
		    pylonselected=1;
		    return;
	    }
    }
    for (int i=0;i<MAXSELECTMAN;i++)
        if (fordeselect[i]&&
            fordeselect[i]->SC_Unit==SC_PYLONOBJ&&
            fordeselect[i]->playernr==NUMBGAMER&&!IsOBJUnderConstruct(fordeselect[i]))
        {
	    pylonselected = 1;
	    break;
	}
#endif
}
//=====================================
void deselectvars(void)
{
     selectbuilds[0]=0;
     selectbuilds[1]=0;
     selectbuilds[2]=0;
     selectbuilds[3]=0;
     selectunits[0]=0;
     selectunits[1]=0;
     selectunits[2]=0;
     selectunits[3]=0;
}
//=====================================
void deselectallobj(int playernr)
{
    for (int i=0;i<MAXSELECTMAN;i++)
    {
	if (fordeselect[i])
        {
	    doselectedOBJbit(fordeselect[i],playernr,0);
    	    fordeselect[i]=NULL;
	}
    }
}
//=====================================
struct OBJ *getfirstselectedobj(struct OBJ *a[])
{
    for (int i=0;i<MAXSELECTMAN;i++)
    {
	if (fordeselect[i])
	    return fordeselect[i];
    }
    return NULL;
}
//=====================================
void controlbu(int typeunits,int typeb_or_u,int mode)
{
    struct OBJ *a;
    struct OBJstruct *b;
    int nrobj;
    nrobj=0;
    for (int i=0;i<MaxObjects;i++)
    {
       a=objects[i];
       b=loadobj(a->SC_Unit);
       if (ifselectedOBJbit(a,NUMBGAMER))
       {
             if ((player_aliance(NUMBGAMER,a->playernr)==typeunits)
                 &&IsBuild(a->SC_Unit)==typeb_or_u)
             {
                if (nrobj<MAXSELECTMAN)
                {
                   if (addselectobj(a)!=0)
                   {
                      nrobj++;
                      if (typeunits!=MYOBJ||typeb_or_u)
                      {
                         deselectallexcludeone(a);
                         break;
                      }
                      else
                         continue;
                   }
                }
            }
	    doselectedOBJbit(a,NUMBGAMER,0);
       }//if
    }//for
}
//========================================
void grupingobj(int mode)
{
    struct OBJ *a;
    for (int i=0;i<MaxObjects;i++)
    {
        a=objects[i];
        if (ifselectedOBJbit(a,NUMBGAMER))
        {
           if (IsBuild(a->SC_Unit))
           {
              selectbuilds[player_aliance(NUMBGAMER,a->playernr)]++;
           }
           else
              selectunits[player_aliance(NUMBGAMER,a->playernr)]++;
        }
    }
    //=========poreadok selectirovania
    //1. my units
    //2. aliance units
    //3. enemy unit
    //4. neutral unit
    //5. my build
    //6. aliance build
    //7. enemy build
    //8. neutral build
    if (selectunits[MYOBJ])
       controlbu(MYOBJ,0,mode);
    else
	if (selectunits[ALLIANCEOBJ])
    	    controlbu(ALLIANCEOBJ,0,mode);
        else
            if (selectunits[ENEMYOBJ])
        	controlbu(ENEMYOBJ,0,mode);
	    else
		if (selectunits[NEUTRALOBJ])
        	    controlbu(NEUTRALOBJ,0,mode);
		else
    		    if (selectbuilds[MYOBJ])
        		controlbu(MYOBJ,1,mode);
    		    else
            		if (selectbuilds[ALLIANCEOBJ])
                    	controlbu(ALLIANCEOBJ,1,mode);
                	else
            		    if (selectbuilds[ENEMYOBJ])
                	        controlbu(ENEMYOBJ,1,mode);
    			    else
    				if (selectbuilds[NEUTRALOBJ])
                		controlbu(NEUTRALOBJ,1,mode);
}
//=====================================
int foundonetypeobj(struct OBJ *a,struct OBJstruct *b)
{
    int i,u=0;
    struct OBJ *c;
    struct OBJstruct *c1;
    for (i=0;i<MaxObjects;i++)
    {
        c=objects[i];
        c1=loadobj(c->SC_Unit);
        if ((b==c1)&&(c->playernr==a->playernr))
            if (!GetUnitProp(c,NUMBGAMER,VARNOTDETECT))
            {
		doselectedOBJbit(c,NUMBGAMER,1);
        	u++;
            }
    }//for
    return(u);
}//end func
//=====================================
struct OBJ *founduniqueobj(int x1,int y1)
{
int i,nodoodad,x,y;
struct OBJ *a,*minobj=NULL;
int yn,distx,disty,mindist=0x7fffffff;
    for (i=0;i<MaxObjects;i++)
    {
        a=objects[i];
    	if (a->mainimage->flags & SC_IMAGE_FLAG_DISABLEDRAW)
    	    continue;
        yn=y1;
	nodoodad=1;
    	if (IsDoodadState(a->SC_Unit)&&(IsInvincibleUnit(a->SC_Unit) || GetDoodadState(a)==DOODAD_BOTTOM_STATE))
    	    nodoodad=0;
    	    if (IfCanClickOBJ(a->SC_Unit)  && nodoodad && (!a->carryobj || !IsPickupUnit(a->SC_Unit)))
        {
    	    x = GetOBJx(a);
    	    y = GetOBJy(a);
    	    if (x1 > x-GetUnitWidthAndHeight(a->SC_Unit,UNITDIM_WIDTH)/2  &&
		y1 > y-GetUnitWidthAndHeight(a->SC_Unit,UNITDIM_HEIGHT)/2 && 
		x1 < x+GetUnitWidthAndHeight(a->SC_Unit,UNITDIM_WIDTH)/2  && 
		y1 < y+GetUnitWidthAndHeight(a->SC_Unit,UNITDIM_HEIGHT)/2 )
	    {
		if (IfUnitIsSelectable(a))
        	{
        	    distx=abs(x-x1);
    		    disty=abs(y-yn);
        	    distx=(int) hypot(distx,disty);
        	    if (distx<mindist)
        	    {
            		mindist=distx;
            		minobj=a;
        	    }
        	}
    	    }
        }
    }//for
return(minobj);
}//end func
//=====================================
void ifselectedprobe(int countofselectunits)
{
    int i;
    for (i=0;i<MAXSELECTMAN;i++)
	if (fordeselect[i])
	{
	    if (IsWorkerUnit(fordeselect[i]->SC_Unit))
	    {
	        if (IsOBJBurrowed(fordeselect[i]))
		    ChangeTypeOfProp(fordeselect[i],PROPNORMAL2);
		else
		    if (!fordeselect[i]->constrobj)
		        ChangeTypeOfProp(fordeselect[i],PROPNORMAL1);
	    }
	}
}
//=====================================
void ifselectTEMPLARS(int player)
{
    if (selectobjcount()>=2)
    {
	warparchon = 1;
    }
    else
    {
	warparchon = 0;
    }
}
//=====================================
void ifselectTRANSPORTS(struct OBJ *a)
{
    if (a)
	if (GetSpaceProvided(a->SC_Unit))
	    AnalizeAllTransportUnits(a,transportplace,&transportplaceobj);
	else
	    transportplaceobj=NULL;
}
//=====================================
int IfUnitIsSelectable(OBJ *a)
{
    if (GetUnitProp(a,NUMBGAMER,VARNOTDETECT))
    {
	if (GetUnitProp(a,NUMBGAMER,VARINVSEE))
	    return(1);
    }
    else
    {
	if (GetUnitProp(a,NUMBGAMER,VARSEE))
	    return(1);
    }
    return(0);
}
//=====================================
#define MINHAP 2
void selectMAN(int x1,int y1,int x2,int y2,int mode)
{
    int i,yn1,yn2,u=0,oldu,nodoodad,x,y;
    struct OBJ *a,*firstobj=NULL,*speakOBJ;
    struct OBJstruct *b;
    deselectvars();
    if (!mode)
    {
	for (i=0;i<MAXSELECTMAN;i++)
    	    fordeselectcopy[i]=fordeselect[i];
	deselectallobj(NUMBGAMER);
    }
    if (x2<x1) 
        swap(&x1,&x2);
    if (y2<y1) 
        swap(&y1,&y2);
    x1=x1+map.MAPXGLOBAL;
    x2=x2+map.MAPXGLOBAL;
    y1=y1+map.MAPYGLOBAL;
    y2=y2+map.MAPYGLOBAL;
    int ctrl=(KEYPRESS(CTRLLKEY)|(KEYPRESS(CTRLRKEY)))||mousedoubleclick;
    if ((x2-x1<=MINHAP)&&(y2-y1<=MINHAP)||ctrl)
    {
	a=founduniqueobj((x1+x2)/2,(y1+y2)/2);
        firstobj = a;
        if (a)
        {
    	    if (IfCanClickOBJ(a->SC_Unit))
        	if (ctrl)
        	{
    		    b = loadobj(a->SC_Unit);
            	    u = foundonetypeobj(a,b);
        	}
        	else
        	{
            	    doselectedOBJbit(a,NUMBGAMER,1);
            	    u++;
        	}
        }
    }
    else
       for (i=0;i<MaxObjects;i++)
       {
            a=objects[i];
    	    if (a->mainimage->flags & SC_IMAGE_FLAG_DISABLEDRAW)
    		continue;
	    nodoodad=1;
    	    if (IsDoodadState(a->SC_Unit) && (IsInvincibleUnit(a->SC_Unit)||GetDoodadState(a)==DOODAD_BOTTOM_STATE))
    		nodoodad=0;
    	    if (IfCanClickOBJ(a->SC_Unit)  && nodoodad && (!a->carryobj || !IsPickupUnit(a->SC_Unit)))
            {
                yn1=y1;
                yn2=y2;
    		x = GetOBJx(a);
    		y = GetOBJy(a);
		if (x > x1-GetUnitWidthAndHeight(a->SC_Unit,UNITDIM_WIDTH)/2  && 
		    y > y1-GetUnitWidthAndHeight(a->SC_Unit,UNITDIM_HEIGHT)/2 && 
		    x < x2+GetUnitWidthAndHeight(a->SC_Unit,UNITDIM_WIDTH)/2  && 
		    y < y2+GetUnitWidthAndHeight(a->SC_Unit,UNITDIM_HEIGHT)/2)
		{
		    if (IfUnitIsSelectable(a))
		    {
            		doselectedOBJbit(a,NUMBGAMER,1);
                	u++;
		        if ((!firstobj)||(!IsBuild(a->SC_Unit)))
			    firstobj = a;
		    }
                }
            }
       }//for
    if (u)
	groupmove=0;
    if ((!mode)&&(!u))
    {
	for (i=0;i<MAXSELECTMAN;i++)
	{
    	    fordeselect[i]=fordeselectcopy[i];
    	    if (fordeselect[i])
            	doselectedOBJbit(fordeselect[i],NUMBGAMER,1);
	}
    }
    sortselectedunits(fordeselect,MAXSELECTMAN);
    grupingobj(mode);
    if (u)
	ifselectedprobe(u);
    ifselectTEMPLARS(NUMBGAMER);
    oldu=u;
    u=selectobjcount();
    if (u==1)
    {
	ifselectTRANSPORTS(getfirstselectedobj(fordeselect));
	ifselectedDAMAGEDBUILD(fordeselect[0]);
    }
    else
	transportplaceobj=NULL;
    firstobj=NULL;
    if (u)
    {
	for (i=0;i<MAXSELECTMAN;i++)
	    if (fordeselect[0])
	    {
		firstobj=fordeselect[0];
		break;
	    }
	if (firstobj&&oldu)
	{
	    if (firstobj->prop&VARREADY)
	    {
		if (firstobj->prop & VARPOWEROFF)
		{
		    if (gameconf.audioconf.buildsounds)
			if (firstobj&&firstobj->playernr==NUMBGAMER)
			    if (mapSEE(firstobj,NUMBGAMER)>1)
				Play_sfxdata_id(firstobj,sfx_powerdown[GetUnitRace(firstobj->SC_Unit)],2,0);
		}
		else
		{
		    speakOBJ = GetMaxRankOBJ(MAXSELECTMAN,fordeselect);
		    if (speakOBJ)
			activatesound(speakOBJ,MODESOUNDSELECT,0,NOSTOPCURRENTSOUNDS);
		}
	    }
	    else
		activatesound(firstobj,MODESOUNDCONSTRUCT,0,NOSTOPCURRENTSOUNDS);
	}
    }
}//end func
//========================================
void changedeselectobj(struct OBJ *a,struct OBJ *aa)
{
 int i;
 for (i=0;i<MAXSELECTMAN;i++)
     if (fordeselect[i]==a)
     {
        fordeselect[i]=aa;
	selectobj(aa);
        return;
     }
}
//========================================
int selectobjcount(void)
{
 int i,j=0;
 for (i=0;i<MAXSELECTMAN;i++)
     if (fordeselect[i])
        j++;
 return(j);
}
//==============================================
int selectobj(struct OBJ *a)
{
    int i;
    if (addselectobj(a)==1)
	for (i=0;i<MAXSELECTMAN;i++)
	    if (fordeselect[i]==a)
    	    {
    		doselectedOBJbit(fordeselect[i],NUMBGAMER,1);
		AddCircleImage(a);
    		return(1);
    	    }
    return(0);
}
//========================================
int deselectobj(struct OBJ *a)
{
    int i,found=-1,lastelem=-1;
    for (i=0;i<MAXSELECTMAN;i++)
	if (fordeselect[i]==a)
	    found = i;
        else
    	    if (fordeselect[i] && found!=-1)
    		lastelem=i;
    if (found != -1)
    {
	//return to advisor if deselecting unit
//	printf("deselection %x %x\n",test,fordeselect[found]);
	if (GetPortraitOBJ() == fordeselect[found])
	    ShowAdvisorPortrait();
	//deselect unit
	doselectedOBJbit(fordeselect[found],NUMBGAMER,0);
	if (lastelem!=-1)
	{
	    fordeselect[found]=fordeselect[lastelem];
	    fordeselect[lastelem]=NULL;
	}
	else
	    fordeselect[found]=NULL;
	DelCircleImage(a);
    }
    return(0);
}
//========================================
int addselectobj(struct OBJ *a)
{
    int i;
    if (a->prop&VARCANTSELECT)
	return 0;
    for (i=0;i<MAXSELECTMAN;i++)
	if (a==fordeselect[i])
    	    return(-1);
    for (i=0;i<MAXSELECTMAN;i++)
	if (!fordeselect[i])
        {
    	    fordeselect[i]=a;
    	    return(1);
        }
    return(0);
}
//========================================
//========================================
//========================================
void createobjregen(void)
{
    int i,nrobj=0,regen=0;
    struct OBJ *a;
    for (i=0;i<MAPREGENERATION;i++)
	diapazone[i].begin=-1;
    for (i=0;i<MaxObjects;i++)
    {
	a=objects[i];
        if (++nrobj>MinObjRegen)
        {
    	    nrobj=0;
            regen++;
        }
        a->whenregen=regen;
        if (diapazone[regen].begin==-1)
    	    diapazone[regen].begin=i;
    	diapazone[regen].end=i;
    }
    MaxRegen=regen;
}
//========================================
#define REGENERATESHIELD        5
#define REGENERATELIFE        	2
#define DEREGENERATELIFE       	10
#define DEREGENERATETEMPLIFE   	15
//#define DELTAMANAONCYCLE	7	//add mana in 1 cycle	in mage.h

int RegenerateLife(struct OBJ *a,struct OBJstruct *b,int regenshield,int regenlife)
{
    int upgradevalue,lifechange=0;
    if (IsOBJUnderConstruct(a))
	return(0);
    if (IsRegenerate(a->SC_Unit)&&regenlife)
    {
	if (a->life<GetUnitMaxLife(a->SC_Unit))
	{
	    if (IsOBJBurrowed(a))
		AddUnitLife(a,REGENERATELIFE+1);
	    else
		AddUnitLife(a,REGENERATELIFE);
	    lifechange=1;
	}
    }
    else
    {
	if (IsShieldEnable(a->SC_Unit) && regenshield)
	{
	    if (a->shield<GetUnitMaxShield(a->SC_Unit))
	    {
		AddUnitShield(a,REGENERATESHIELD);
	    }
	}
	else
	{
	    if (IsBuild(a->SC_Unit)&&GetUnitRace(a->SC_Unit)==TERRANRACE)
		if (a->life*3<(GetUnitMaxLife(a->SC_Unit)))
		{
		    a->life-=DEREGENERATELIFE;
		    lifechange=1;
		    if (a->life<=0)
		        dieobj(a);
		}
	}
    }
    return(lifechange);
}
//========================================
int DecrementLifeMage(struct OBJ *a,struct OBJstruct *b)
{
    int x,y,sx,sy,nrmage;
    if (a->timeleft>0)
    {
	if (--a->timeleft<=0)
	{
    	    dieobj(a);
	    return -1;
	}
    }
    return 0;
}
//========================================
void allobjdecrmtimemage(void)
{
    int i;
    struct OBJ *a;
    struct OBJstruct *b;
    for (i=0;i<MaxObjects;i++)
    {
    	a = objects[i];
    	b = loadobj(a->SC_Unit);
	DecrementOBJAtr(a);
    }
}
//========================================
int ApplyDamageToUnit(struct OBJ *a,struct OBJstruct *b)
{
    unsigned short image_id,imgflags;
    int lifechange;
    MAIN_IMG *img;
    OVERLAY_IMG *newimg;
    
    lifechange=0;
    if (a->psistormdamage)
    {
	if (GetMageAtr(&a->atrobj,ATRHALLUCINATION)>0)
	{
	    dieobj(a);
	    return(0);
	}
	LowLevelDamage(NULL,a,b,WEAPONID_PSISTORM,DAMAGE_IGNOREARMOR,a->psistormdamage,0,DAMAGE_SPLASH|DAMAGE_NOSHOWHIT);
	a->psistormdamage=0;
    }
    if (a->dmatrixdamage)
    {
	//show dmatrixpicture;
	img = a->mainimage;
	image_id = IMAGEID_DEFENSIVEMATRIXHIT1 + GetUnitOverlaySize(a->SC_Unit);
	//permits only one image of damage to matrixshield
	if (!CheckForSpecificChildsImageID(img,image_id,image_id))
	{
	    //show damage to defensive matrix
	    imgflags = 0;
	    if (IsOnSkyOBJ(a))
		imgflags = SC_IMAGE_FLAG_AIRIMG;
	    newimg = new OVERLAY_IMG(img,image_id,0,0,a->mainimage->elevationlevel+1,imgflags | SC_IMAGE_FLAG_IMGOVER,ISCRIPTNR_INIT);
//	    iscriptinfo.ExecuteScript(newimg);
	}
    }
    if (a->shielddamage>0)
    {
	a->shield-=a->shielddamage;
    }
    if (a->lifedamage>0)
    {
	a->life -= a->lifedamage;
	lifechange=1;
	if (a->life<=0)
	    dieobj(a);
    }
    a->lifedamage=0;
    a->shielddamage=0;
    a->dmatrixdamage=0;
    return(lifechange);
}
//========================================
void allobj_dieheal(void)
{
    int i,regenshield,regenlife,lifechange;
    struct OBJ *a;
    struct OBJstruct *b;
    for (i=0;i<MaxObjects;i++)
    {
        a = objects[i];
        if (a->modemove == MODEDIE)
    	    continue;
	b = loadobj(a->SC_Unit);
	regenshield=(a->shielddamage==0);
	regenlife=(a->lifedamage==0);
	lifechange = ApplyDamageToUnit(a,b);
        if (a->modemove == MODEDIE)
    	    continue;
	mageattributedothings(a,b);
    	if (!IsOBJUnderConstruct(a))
    	{
	    //regenerate life/shield if not ignore shield/life
	    lifechange += RegenerateLife(a,b,regenshield,regenlife);
	    DecrementLifeMage(a,b);
	}
	if (lifechange)
	{
//	    a->lastdamageoverlays = -1;
	    AddRemoveBloodFlameOverlays(a);
	}
    }
}
//========================================
void allobjconstr(void)
{
    int i;
    struct OBJ *a;
    struct OBJstruct *b;
    for (i=0;i<MaxObjects;i++)
    {
	a=objects[i];
        b=loadobj(a->SC_Unit);
	TickUnderConstruct(a);	//perform construct build
    	if (!(a->prop&VARNOTWORK))
    	    workingbuilds(a);
    }
}
//=============================================
#define FACTORINV 3
int makeinvisibles(struct OBJ *a)
{
    int i,deltafactor,distance;
    if (!IsOBJBurrowed(a))
    {
	i = a->mainimage->invisiblecolors;
        if (GetMageAtr(&a->atrobj,ATRINVISIBLE)==0) //nevidimii obj
	    deltafactor=-FACTORINV;
	else
	    deltafactor=FACTORINV;
	i += deltafactor;
	if (i<0)
	    i=MININVISIBLECOLOR;
	else
	    if (i>MAXINVISIBLECOLOR)
    		i=MAXINVISIBLECOLOR;
	if (deltafactor>0)
	{
	    if (!IsCloaked(a))
    	    {
	        //need to play cloak sound
		SetCloakedFlag(a,1);
		if (mapSEE(a->xkart,a->ykart)>1)
		{
		    distance = (int) hypot(GetOBJx(a)-map.MAPXGLOBAL,GetOBJy(a)-map.MAPYGLOBAL);
		    Play_sfxdata_id(NULL,SFXDATA_CLOAKIN,2,distance);
	        }
	    }
	}
    	else
	    if (IsCloaked(a))
    	    {
		//need to play decloak sound
	    	SetCloakedFlag(a,0);
		if (mapSEE(a->xkart,a->ykart)>1)
		{
		    distance = (int) hypot(GetOBJx(a)-map.MAPXGLOBAL,GetOBJy(a)-map.MAPYGLOBAL);
		    Play_sfxdata_id(NULL,SFXDATA_CLOAKOUT,2,distance);
		}
	    }
	a->mainimage->invisiblecolors=i;
	return(i);
    }
    return(a->mainimage->invisiblecolors);
}
//========================================
void invisiblestick(void)
{
    int i,grpmethod;
    OBJ *a;
    for (i=0;i<MaxObjects;i++)
    {
        a = objects[i];
        if (a->modemove == MODEDIE || IsInvincibleUnit(a->SC_Unit))
    	    continue;
	if (IsDoodadState(a->SC_Unit))// || IsBuild(a->SC_Unit))
	    continue;
        if (GetMageAtr(&a->atrobj,ATRSTASIS)==0 )
        {
    	    if (makeinvisibles(a))
    	    {
		if (GetUnitProp(a,NUMBGAMER,VARINVSEE))
		{
		    //invisible detected
		    if (IsOBJBurrowed(a))
		        a->mainimage->newgrpmethod = DRAWBURROWED;
		    else
		        a->mainimage->newgrpmethod = TRANSPARENT;
		}
		else
		{
		    //invisible notdetected
		    a->mainimage->newgrpmethod = DISTORTION;
		}
    	    }
    	    else
    	    {
    		//unit become full visible,change image grpmethod
	    	SetCloakedFlag(a,0);
		a->mainimage->newgrpmethod = NORMAL;
    	    }
    	}
    }
}
//=============================================
void FixSubUnitCoords(OBJ *a,OBJstruct *b)
{
/*    int side,mirror;
    signed char *adrlo,lox,loy;
    if (alldattbl.units_dat->Subunit1[a->SC_Unit]<MAX_UNITS_ELEM)
    {
	if (b->subunit_lo)
	{
	    if (a->storonasveta>=SPRITE_SIDES)
	    {
		side=MAXFACEDIRECTIONS-a->storonasveta;
		mirror=1;
	    }
	    else
	    {
		side=a->storonasveta;
		mirror=0;
	    }
	    adrlo = GetLoXY(b->subunit_lo,side,0);
	    if (mirror)
		lox = -adrlo[0];
	    else
		lox = adrlo[0];
	    loy = adrlo[1];
	    a->subunit->sourcex = a->sourcex + lox;
	    a->subunit->sourcey = a->sourcey + loy;
	    ForceKartChanges(a->subunit);
	}
    }
*/
}
//==================================
void CreateImageAndAddToList(OBJ *a,int x256,int y256,int readyatbegin)
{
    unsigned char initscriptnr;
    unsigned short constr_id=0;
    MAIN_IMG *img;
    int groundair,x,y;
    x = x256 >> 8;
    y = y256 >> 8;
    initscriptnr = ISCRIPTNR_INIT;
    switch(readyatbegin)
    {
	case 0:
	    if (IsBuild(a->SC_Unit))
	    {
		//prevent geyser to produce smokes
		if (a->replaceobj)
		    a->replaceobj->mainimage->DisableExecScript();//disable all overlays exec scripts
		if (IsProtossRace(a->SC_Unit))
		{
		    constr_id = IMAGEID_WARPANCHOR;
		    Play_sfxdata(x,y,SFXDATA_PROTOSSBUILDWARPBEGIN,2);
		}
		else
		{
		    if (a->replaceobj)
		    {
//    			a->replaceobj->mainimage->HideChildsImgFlag();
//    			a->replaceobj->mainimage->HideImgFlag();
		    }
		    constr_id = alldattbl.units_dat->ConstructionAnimation[a->SC_Unit];
		}
	    }
	    else
		constr_id = alldattbl.units_dat->ConstructionAnimation[a->SC_Unit];
	    break;
	case 1:
	case 2:
	case 3:
	    constr_id = 0;
	    break;
	case 4://zerg unit birth
	    break;
	case 5:
	    initscriptnr = ISCRIPTNR_UNKNOWN;//for doodad unit
	    break;
    }
    groundair = IsOnSkyOBJ(a);
    img = OBJCreateImage(a,x256,y256,initscriptnr,groundair,constr_id);
    mainimageslist.AddElem(img);
    a->mainimage = img;
    if (IsBuild(a->SC_Unit))
    {
        switch(readyatbegin)
        {
    	    case 0:
	        SetBeginSelfConstruct(a);		//need to be constructed
		if (IsProtossRace(a->SC_Unit))
		    a->selfconstruct.currentconstrstep = CONSTR_STEP_PROTOSSBUILD;
		else if (IsZergRace(a->SC_Unit))
		    a->selfconstruct.currentconstrstep = CONSTR_STEP_ZERGBUILDLESS33;
		else if (IsTerranRace(a->SC_Unit))
		    a->selfconstruct.currentconstrstep = CONSTR_STEP_TERRANBUILDLESS25;
	        break;
    	    case 1:
    		img->SetIScriptNr(ISCRIPTNR_BUILT);	//complete construction
		iscriptinfo.ExecuteScript(img);
    		break;
    	    case 2:
    		img->SetIScriptNr(ISCRIPTNR_ALMOSTBUILT);//begin appear at end of construction
    		break;
    	    case 3:
    		break;
    	}
    }
}
//==================================
void ChangeUnitAndImagesAssociated(OBJ *a,int SC_NewUnit)
{
    int x256,y256;
    x256 = GetOBJx256(a);
    y256 = GetOBJy256(a);
    a->mainimage->DeleteMainImgAndChilds();
    a->mainimage = NULL;
    a->SC_Unit = SC_NewUnit;
    CreateImageAndAddToList(a,x256,y256,2);
}
//==================================
void ReleaseResource(OBJ *a)
{
    if (a->resourceobj)
    {
	a->resourceobj->resourceobj = NULL;
	a->resourceobj = NULL;
    }
}
//==================================
int moveobj_addonmode(struct OBJ *a,struct OBJ *destobj,int mode,int x,int y,int showerrorflag)
{
    OBJ *newobj;
    int deltax,deltay;
    AttachAddonDeltaCoords(a->SC_Unit,mode,&deltax,&deltay);
    if ( x - GetOBJx(a) == deltax && y - GetOBJy(a) == deltay)
    {
	//the near position
	DoOrder(a->playernr,a,x,y,mageprop[mode].type_id,mageprop[mode].obj_id,
				mageprop[mode].obj_id,mageprop[mode].stattxt_id_enable,mode,showerrorflag);
	newobj = createobjman(x,y,mageprop[mode].obj_id,a->playernr);
	newobj->life=STARTBUILDLIFE;
	newobj->shield=STARTBUILDSHIELD;
	a->constrobj = newobj;
	newobj->constrobj=a;
	a->addonobj = newobj;
	newobj->addonobj = a;
	ChangeTypeOfProp(a,PROPDECONSTRUCT);
	return(MOVEOBJ_DONE);
    }
    else
    {
	if (IfHaveNextModeMove(a))
	{
	    moveobj(a,NULL,MODEMOVE,x - deltax,y - AIRUNITS_START_YPOS - deltay,showerrorflag,0);	//move to land destination
	    AddModeMove(a,NULL,MODELANDING,x - deltax,y - deltay,showerrorflag);			//land to destination
	    AddModeMove(a,NULL,mode,x,y,showerrorflag);							//create addon after
	}
	else
	{
	    moveobj(a,NULL,MODELIFTOFF,x - deltax,y - deltay,showerrorflag,0);	//liftoff
	    AddModeMove(a,NULL,MODEMOVE,x - deltax,y - AIRUNITS_START_YPOS - deltay,showerrorflag);	//move to land destination
	    AddModeMove(a,NULL,MODELANDING,x - deltax,y - deltay,showerrorflag);			//land to destination
	    AddModeMove(a,NULL,mode,x,y,showerrorflag);							//create addon after
	}
//	printf("deltax=%d deltay=%d\n",x-GetOBJx(a),y-GetOBJy(a));
	//far position
    }

}
//==================================
int moveobj_buildmode(struct OBJ *a,struct OBJ *destobj,int mode,int x,int y,int showerrorflag)
{
    OBJ *newobj;
    int mcost,gcost,x256,y256,constrerror;
    unsigned char obj_id;
    obj_id = mageprop[mode].obj_id;
    GetCostUnit(obj_id,&mcost,&gcost);
    if (IsBuild(a->SC_Unit))
    {
        if (a->SC_Unit == obj_id)
        {
	    newobj = createobjman(x,y,SC_NYDUSCANALOBJ,a->playernr);
	    newobj->life = GetUnitMaxLife(a->SC_Unit);
	    ChangeTypeOfProp(newobj,PROPEMPTY);
	    ChangeTypeOfProp(a,PROPEMPTY);
	    a->doubleunit = newobj;
	    newobj->doubleunit = a;
        }
        else
        {
	    //build make build (usually zerg building upgrade)
    	    a->SC_ConstrUnit = obj_id;				//construct unit
	    a->SC_FromUnit = a->SC_Unit;			//save previous unit
	    a->templife = a->life;
	    a->tempshield = a->shield;
	    x256 = GetOBJx256(a);
	    y256 = GetOBJy256(a);
	    a->mainimage->DeleteMainImgAndChilds();

	    ChangeSC_Unit(a,a->playernr,obj_id,CHANGESC_UNIT_CONSTR);
	    CreateImageAndAddToList(a,x256,y256,0);
	    SetUnitPercentLife(a,100);
	    SetUnitPercentShield(a,100);
	    ChangeResourcePlayer(a->playernr,MINUSFACTOR,mcost,gcost);
	    SetBeginSelfConstruct(a);
	    a->selfconstruct.currentconstrstep = CONSTR_STEP_ZERGBUILDLESS99;
	    SetOBJIScriptNr(a,ISCRIPTNR_SPECIALSTATE2,ISCRIPTNR_SETONLY);
	    ChangeTypeOfProp(a,PROPDECONSTRUCT);
//	    a->lastdamageoverlays = -1;
	    AddRemoveBloodFlameOverlays(a);		//remove any bloodflame if any
    	    return(MOVEOBJ_DONE);
	}
    }
    else
    {
	//unit make build
	if (a->carryobj && IsNonNeutralFlag(a->carryobj->SC_Unit))
	    return(MOVEOBJ_NOACT);
	if (GetDistanceTo(a,x,y) > mageprop[mode].mindistance/256 + 10)
	{
	    ChangeTypeOfProp(a,PROPNORMAL1);
	    a->modemove = mode;
	    //move to destination
	    initmoveaction(a,destobj,mode,0,0,x,y);
	    if (destobj)
		a->data.geyserdest.obj = destobj;
	    AddModeMove(a,NULL,mode,x,y,showerrorflag);//create build after
	    
	}
	else
	{
	    //change x,y to correct geyser coords
	    if ( obj_id == SC_EXTRACTOROBJ || 
		 obj_id  == SC_REFINERYOBJ || 
		 obj_id  == SC_ASSIMILATOROBJ )
	    {
		//geyser under worker
		destobj = a->data.geyserdest.obj;
		if (!destobj)
		{
		    DEBUGMESSCR("geyser is NULL\n");
		    moveobj(a,NULL,MODESTOP,0,0,NOSHOWERROR,0);
		    return(MOVEOBJ_NOACT);
		}
	    }
//	    a->finalOBJ = destobj;
	    //check if can construct on this place
	    if (!CheckIfCanBuild(a,obj_id,x,y,&constrerror))
	    {
		moveobj(a,NULL,MODESTOP,0,0,NOSHOWERROR,0);
		if (a->playernr == NUMBGAMER && showerrorflag)
	    	{
		    activatesound(a,MODESOUNDERROR,2,NOSTOPCURRENTSOUNDS);
		    putbuildplacemessage(-constrerror);
		}
		return(MOVEOBJ_NOACT);
	    }
	    //createbuild
	    //check ass/ref/ext
	    switch(a->SC_Unit)
	    {
		case SC_PROBEOBJ:
		    if (!DecrUnitCost(obj_id,a->playernr))
		    {
			moveobj(a,NULL,MODESTOP,0,0,NOSHOWERROR,0);
			return(MOVEOBJ_NOACT);
		    }
		    createobjman(x,y,obj_id,a->playernr,0,1,1,33);
		    break;
		case SC_DRONEOBJ:
		    //doliftdown
		    SetModeMove(a,MODELANDING);
		    SetOBJIScriptNr(a,ISCRIPTNR_LANDING,ISCRIPTNR_EXECUTE);
		    a->deltavertpos = DRONE_START_YPOS;
    	    	    a->SC_ConstrUnit = obj_id;
    	    	    a->mainimage->UnitNeededDeltaDirection256(+128);
		    break;
		case SC_SCVOBJ:
		    if (!DecrUnitCost(obj_id,a->playernr))
		    {
			moveobj(a,NULL,MODESTOP,0,0,NOSHOWERROR,0);
			return(MOVEOBJ_NOACT);
		    }
		    //create build
		    newobj = createobjman(x,y,obj_id,a->playernr,0,1,1,33);
		    //lets begin construct
		    ContinueSCVConstruct(a,newobj);
		    SetModeMove(a,MODECONSTRUCT);
		    break;
	    }
	}
	return(MOVEOBJ_DONE);
    }
    return(MOVEOBJ_NOACT);
}
//==================================
int moveobj_unitmode(struct OBJ *a,struct OBJ *destobj,int mode,int x,int y,int showerrorflag)
{
    int needmana;
    OBJ *newobj;
    unsigned char obj_id;
    obj_id = mageprop[mode].obj_id;
    switch(obj_id)
    {
	case SC_SCANNERSWEEP:
	    if (MageDepend(a,a->playernr,mode))
	    {
		needmana = GetTechEnergyCost(mageprop[mode].depend.obj_id[0]);
		DecrMana(a,needmana);
		newobj = createobjfulllife(x,y,obj_id,a->playernr);
		makeoneobjseeopen(newobj,loadobj(newobj->SC_Unit));
	        //if scaner apear on closed area we don not hear 'music from sweepscan' so we need to
		//activate it again and prevent to hear it doubled if not
		activatesound(newobj,MODESOUNDREADY,1,NOSTOPCURRENTSOUNDS);
		SetOrder(a,1,&SIGOrder_ScannerSweepEnd);
	    }
	    break;
	case SC_LURKEROBJ:
	    UnitConstrUnit(a,obj_id,-1,-1);
	    break;
	default:
    	    DEBUGMESSCR("unit %d make unit %d, not develop\n",a->SC_Unit,obj_id);
    	    break;
    }
    return(MOVEOBJ_DONE);
}
//==================================
short TARGETERROR[3]={SFXDATA_ERRORTARGET_ZERG,SFXDATA_ERRORTARGET_TERRAN,SFXDATA_ERRORTARGET_PROTOSS};
//==================================
int moveobj(struct OBJ *a,struct OBJ *destobj,int mode,int x,int y,int showerrorflag,int shiftbit)
{
    OBJ *newobj;
    unsigned char flingy_id,SC_res_type,inv,useweapon_id;
    int needmana,type_id,obj_id,ret,slotnr,deltaz,constrerror,resval;
    int i,tempvar,errmes,state,openstate;
    int (*comparefunc)(int *,int );					//for doodad
    
    //prevent to move if is uninterrupted mode
    if (a->modemove == MODEDIE)
    {
	DEBUGMESSCR("moveobj <%s> than died\n",mageprop[mode].namemage);
        return(MOVEOBJ_NOACT);
    }
    if (mageprop[a->modemove].atr & ORDER_CANNOTBEINTERRUPTED)
        return(MOVEOBJ_NOACT);
    //add to list of moves if have flag and have some in listmove
    if (shiftbit && mageprop[mode].atr & ORDER_CANBEQUEUED)
    {
	if (a->modemove != MODESTOP)
	{
	    AddModeMove(a,destobj,mode,x,y,showerrorflag);
	    return(MOVEOBJ_DONE);
	}
    }
    //release any resource field
    if (mode != MODEDIE)
	ReleaseResource(a);
    OBJstruct *b = loadobj(a->SC_Unit);
    a->prop &= ~( VARPATROLFLAG | VARHOLDPOSBIT );
    if (a->mainimage->flags & SC_IMAGE_FLAG_CANTBREAKCODE)
	if (mode != MODEDIE)
	{
	    AddModeMove(a,destobj,mode,x,y,NOSHOWERROR);							//create addon after
	    return(MOVEOBJ_DONE);
	}    
//    if (a->mainimage->flags & SC_IMAGE_FLAG_CANTBREAKCODE && mode != MODEDIE)
//        return(MOVEOBJ_NOACT);
    if (!(showerrorflag & ATACKMOVEBIT))
        a->prop &= ~VARMOVEINATACKMODE;
    if (mageprop[mode].type_id==ORDERS_UNIT)
    {
	if (IsAddon(mageprop[mode].obj_id))
	{
	    return(moveobj_addonmode(a,destobj,mode,x,y,showerrorflag));
	}
	else if (IsBuild(mageprop[mode].obj_id))
	{
	    return(moveobj_buildmode(a,destobj,mode,x,y,showerrorflag));
	}
	else
	{
	    return(moveobj_unitmode(a,destobj,mode,x,y,showerrorflag));
	}
    }
    else
    switch(mode)
    {
        case MODENUKESPECIALSTATE1:
	    a->castmagenr = mode;
	    a->modemove = mode;
	    SetOrder(a,1,NULL);
	    SetOBJIScriptNr(a,ISCRIPTNR_SPECIALSTATE1,ISCRIPTNR_EXECUTE);
    	    break;
        case MODENUKEWAITFORLANDING:
    	    a->modemove = mode;
    	    SetOBJxy256(a,x<<8,y<<8);
    	    SetOrder(a,2,&SIGOrder_NukeLanding);
    	    //hide nuke from players
    	    a->mainimage->HideChildsImgFlag();
    	    a->mainimage->HideImgFlag();
	    //play "nuclear launch detected"
	    playinfoadvisorsound(a->playernr,gameconf.pl_race[NUMBGAMER],ADVNUCLEARDETECT,PLAYADVISOR_TEXTANDSOUND);
	    
	    SetOBJIScriptNr(a,ISCRIPTNR_WARPIN,ISCRIPTNR_EXECUTE);
    	    break;
        case MODECANCELNUKE:
	    GhostNUKECancel(a,GHOSTCANCELNUKE);
	    if (a->prop & VARINCRMAGE)	//if decreasing mage (ghost is cloaked)
	        ChangeTypeOfProp(a,b,PROPNORMAL2);
	    else
	        ChangeTypeOfProp(a,b,PROPNORMAL1);
	    break;
	case MODEGHOSTSEATNUKEPOS:
	    if (GhostNUKEAction(a) == 0)
	    {
		ChangeTypeOfProp(a,PROPNUKECANCEL);
		SetOBJIScriptNr(a,ISCRIPTNR_SPECIALSTATE1,ISCRIPTNR_EXECUTE);
	    }
	    else
	    {
		//error to launch nuke
	    }
	    break;
	case MODENUKELAUNCH:
	    a->castmagenr = mode;
	    a->modemove = mode;
	    //move to destination
	    initmoveaction(a,destobj,mode,0,0,x,y);
	    AddModeMove(a,destobj,MODEGHOSTSEATNUKEPOS,x,y,showerrorflag);
	    break;
	case MODEINFEST:
	    if (destobj)
	    {
		//queen search command center by self
		if (GetDistanceTo(destobj,GetOBJx(a),GetOBJy(a)) <= 0 )
		{
		    CastSpellWithOutWeaponnr(a,a->castmagenr);
		}
		else
		{
		    a->castmagenr = mode;
		    a->modemove = mode;
		    //move to destination
		    initmoveaction(a,destobj,mode,0,0,x,y);
		    AddModeMove(a,destobj,MODEINFEST,x,y,showerrorflag);//cast mage after
		}
	    }
	    break;	
	case MODECASTSPELL:
	    SetOrder(a,1,&SIGOrder_AfterCastMage);
	    SetOBJIScriptNr(a,ISCRIPTNR_CASTSPELL,ISCRIPTNR_EXECUTE);
	    break;
	case MODEPSIONICSTORM:
	case MODEHALLUCINATION:
	case MODEMAELSTROM:
	case MODEMINDCONTROL:
	case MODEFEEDBACK:
	case MODERECALL:
	case MODEDISRUPTION:
	case MODESTASISFIELD:
	case MODEPLAGUE:
	case MODECONSUME:
	case MODECONSUME_HERO:
	case MODEDARKSWARM:
	case MODEPARASITE:
	case MODEENSNARE:
	case MODEBROODLING:
//	case MODEYAMATOGUN:
	case MODELOCKDOWN:
	case MODEOPTICALFLARE:
	case MODERESTORATION:
	case MODEDEFENSIVEMATRIX:
	case MODEIRRADIATE:
	case MODEEMPSHOCKWAVE:
	    a->castmagenr = mode;
	    a->modemove = mode;
	    //move to destination
	    initmoveaction(a,destobj,mode,0,0,x,y);
	    AddModeMove(a,destobj,MODECASTSPELL,x,y,showerrorflag);//cast mage after
	    break;
	case MODEYAMATOGUN:
	    a->castmagenr = mode;
	    a->modemove = mode;
	    //move to destination
	    initmoveaction(a,destobj,mode,0,0,x,y);
	    AddModeMove(a,destobj,MODECASTSPELL,x,y,showerrorflag);//cast mage after
//	    a->finalOBJ = NULL;		
	    a->prop &= ~VARMOVEOBJACT;	//this is prevent move to destobj at next tick, that create engine glowing then do yamatogun
	    break;
	case MODEWAITHARVESTMINERAL:
	    a->modemove = mode;
	    a->data.gather.waittoharvest = WAITTOHARVEST;
	    break;
	case MODEWAITHARVESTGAS:
	    a->modemove = mode;
	    a->data.gather.waittoharvest = WAITTOHARVEST;
	    break;
	case MODEWAITRETURNCARGO:
	    a->modemove = mode;
	    a->data.gather.waittoharvest = WAITTOHARVEST;
	    break;
	case MODEGIVERESOURCETOBASE:
	    newobj = a->carryobj;
	    if (newobj->data.resourcechunk.restype == SC_CARRY_MINERAL1)
	    {
		SC_res_type = SC_MINERAL1OBJ;
		ChangeResourcePlayer(a->playernr,PLUSFACTOR,newobj->data.resourcechunk.rescnt,0);
    		if (TRIG_ChangeStat)
		    PLAYER[a->playernr].statplayer.stat[STATPLAYER_MINERALSMINED] += newobj->data.resourcechunk.rescnt;
	    }
	    else
	    {
    		if (newobj->data.resourcechunk.restype == SC_CARRY_GAS[0])
    		{
		    SC_res_type = SC_ASSIMILATOROBJ;
		    ChangeResourcePlayer(a->playernr,PLUSFACTOR,0,newobj->data.resourcechunk.rescnt);
    		    if (TRIG_ChangeStat)
			PLAYER[a->playernr].statplayer.stat[STATPLAYER_GASMINED] += newobj->data.resourcechunk.rescnt;
		}
		else
		    break;
	    }
	    changegoods = 1;
	    
	    dieobj_silently(newobj);
//	    ChangeTypeOfProp(a,b,PROPNORMAL1);
	    if ( !a->prevresourceobj)
	    {
	        newobj = GetNearResource(a,SC_res_type,&resval);
	        if (newobj)
		    moveobj(a,newobj,MODEGATHER,0,0,NOSHOWERROR,0);
		else
	    	    if (resval)
	    		if (SC_res_type = SC_MINERAL1OBJ)
	    		    moveobj(a,NULL,MODEWAITHARVESTMINERAL,0,0,NOSHOWERROR,0);
	    		else
	    		    moveobj(a,NULL,MODEWAITHARVESTGAS,0,0,NOSHOWERROR,0);
	    	    else
	    		moveobj(a,NULL,MODESTOP,0,0,NOSHOWERROR,0);
	    }
	    else
	        moveobj(a,a->prevresourceobj,MODEGATHER,0,0,NOSHOWERROR,0);
	    break;
	case MODERETURNCARGO:
	    a->modemove = mode;
	    SetOBJIScriptNr(a,ISCRIPTNR_WALKINGTOIDLE,ISCRIPTNR_EXECUTE);
	    newobj = GetNearCenter(a);
	    if (newobj)
	    {
		a->modemove = mode;
		initmoveaction(a,newobj,mode,0,0,x,y);
		AddModeMove(a,newobj,MODEGIVERESOURCETOBASE,x,y,showerrorflag);
	    }
	    else
	    {
		//set to wait to appear base build
		moveobj(a,NULL,MODEWAITRETURNCARGO,0,0,showerrorflag,0);
	    }
	    return(MOVEOBJ_DONE);
	    break;
	case MODEGATHERMINERAL:
	    //check if mineral is non-occuied
	    if (destobj)
	    {
		if (!destobj->resourceobj)		//somebody harvest from this resource?
		{
		    a->modemove = mode;
		    destobj->resourceobj = a;		//resources now is busy by unit
		    a->resourceobj = destobj;		//unit harvest from this resource
		    a->prevresourceobj = destobj;	//doublicate unit harvest from this resource

		    a->data.gather.gathertime = TIMETOGETMINERALS;
		    SetOBJIScriptNr(a,ISCRIPTNR_ALMOSTBUILT,ISCRIPTNR_EXECUTE);
		    return(MOVEOBJ_DONE);
		}
	    }
	    //search for other mineral field
	    a->resourceobj = NULL;
	    a->prevresourceobj = NULL;
	    newobj = GetNearResource(a,SC_MINERAL1OBJ,&resval);
	    if (newobj)
	        moveobj(a,newobj,MODEGATHER,0,0,showerrorflag,0);
	    else
		if (resval)
	    	    moveobj(a,NULL,MODEWAITHARVESTMINERAL,0,0,showerrorflag,0);
		else
	    	    moveobj(a,NULL,MODESTOP,0,0,showerrorflag,0);
	    break;
	case MODEGATHERGAS:
	    if (destobj)
	    {
		if (IsOBJUnderConstruct(destobj))
		{
	    	    moveobj(a,NULL,MODESTOP,0,0,showerrorflag,0);
		    return(MOVEOBJ_NOACT);
		}
		if (!destobj->resourceobj)		//somebody harvest from this resource?
		{
		    a->modemove = mode;
		    destobj->resourceobj = a;		//resources now is busy by unit
		    a->resourceobj = destobj;		//unit harvest from this resource
		    a->prevresourceobj = destobj;	//doublicate unit harvest from this resource
		    a->data.gather.gathertime = TIMETOGETGAS;
		    //need to disapear unit
		    a->mainimage->HideChildsImgFlag();
		    a->mainimage->HideImgFlag();
		    a->mainimage->side += 128;		//razvorot na 180 grad
		    a->prop |= VARNOTHERE;
		    deselectobj(a);
		    return(MOVEOBJ_DONE);
		}
		else
		    moveobj(a,NULL,MODEWAITHARVESTGAS,0,0,showerrorflag,0);
	    }
	    else
	        moveobj(a,NULL,MODESTOP,0,0,showerrorflag,0);
	    break;
	case MODEGATHER:
	    if (destobj)
	    {
		if (a->carryobj)
		{
		    moveobj(a,destobj,MODEMOVE,0,0,NOSHOWERROR,0);
		}
		else
		{
		    a->modemove = mode;
		    a->prop |= VARAUTOMATRESOURCERET;
		    initmoveaction(a,destobj,mode,0,0,x,y);
		    if (IsNonNeutralFlag(destobj->SC_Unit))	//dest unit is extractor or refinery or assimilator
			AddModeMove(a,destobj,MODEGATHERGAS,x,y,showerrorflag);
		    else
			AddModeMove(a,destobj,MODEGATHERMINERAL,x,y,showerrorflag);
		}
	    }
	    break;
	case MODESETRALLYPOINT:
	    a->finalx = x;
	    a->finaly = y;
	    break;
	case MODEREPAIR:
	    if (a->carryobj && IsNonNeutralFlag(a->carryobj->SC_Unit))
		return(MOVEOBJ_NOACT);
	    if (destobj)
	    {
		//continue construction
		if (IsOBJUnderConstruct(destobj))
		{
		    moveobj(a,NULL,MODEMOVE,GetOBJx(destobj),GetOBJy(destobj),NOSHOWERROR,0);
		    AddModeMove(a,destobj,MODECONSTRUCT,0,0,NOSHOWERROR);
		}
		else
		{
		    if (GetUnitMaxLife(destobj->SC_Unit) == destobj->life)
		    {
			showadvisortext(STATTXT_UNABLEREPAIRTARGET);
			return(MOVEOBJ_NOACT);
		    }
		    a->modemove = mode;
		    initmoveaction(a,destobj,MODEREPAIR,0,0,x,y);
		    AddModeMove(a,destobj,MODESTAYANDREPAIR,0,0,NOSHOWERROR);
		}
	    }
	    break;
	case MODESTAYANDREPAIR:
	    if ( GetDistanceTo(destobj,GetOBJx(a),GetOBJy(a)) <= 16 )
	    {
		a->modemove = mode;
//		SetOBJIScriptNr(a,ISCRIPTNR_GNDATTKINIT,ISCRIPTNR_EXECUTE);
		SetOBJIScriptNr(a,ISCRIPTNR_ALMOSTBUILT,ISCRIPTNR_EXECUTE);
	    }
	    else
	    {
		initmoveaction(a,destobj,MODEREPAIR,0,0,x,y);
		AddModeMove(a,destobj,MODESTAYANDREPAIR,0,0,NOSHOWERROR);
	    }
	    break;
	case MODETURN180:
    	    a->modemove = mode;
    	    a->mainimage->UnitNeededDeltaDirection256(+128);
	    break;
	case MODEHALTCONSTRUCT:
	    if (a->constrobj)
	    {
		a->constrobj->constrobj = NULL;
		a->constrobj = NULL;
		moveobj(a,NULL,MODESTOP,0,0,NOSHOWERROR,0);
		ChangeTypeOfProp(a,b,PROPNORMAL1);
	    }
	    break;
	case MODECONSTRUCT:
    	    a->modemove = mode;
    	    if (!a->constrobj)
    	    {
	        ContinueSCVConstruct(a,destobj);
	        break;
    	    }
	    SetOBJIScriptNr(a,ISCRIPTNR_ALMOSTBUILT,ISCRIPTNR_EXECUTE);
	    break;
	case MODEHOLDPOS:
	    if (a->movelist)
		a->movelist->EmptyElemFifo();
	    a->prop |= VARHOLDPOSBIT;
	    if (a->modemove != mode)
	    {
		//engines off
		a->prop &= ~(VARMOVEACT | VARMOVEOBJACT | VARACCELERATIONBIT | VARPATROLFLAG);
		if (a->modemove == MODEATACK)
		{
		    EndAtackAction(a);
		}
		else
		{
		    SetOBJIScriptNr(a,ISCRIPTNR_WALKINGTOIDLE,ISCRIPTNR_EXECUTE);
		}
		SetModeMove(a,mode);
	    }
	    break;
	case MODESTOP:
//	    a->finalOBJ=NULL;
	    if (a->movelist)
		a->movelist->EmptyElemFifo();
	    if (a->modemove != mode)
	    {
		a->prop &= ~(VARMOVEACT | VARMOVEOBJACT | VARACCELERATIONBIT | VARPATROLFLAG);
		//engines off
		if (a->modemove == MODEATACK)
		{
		    EndAtackAction(a);
	    	}
		else
		{
		    SetOBJIScriptNr(a,ISCRIPTNR_WALKINGTOIDLE,ISCRIPTNR_EXECUTE);
		}
		SetModeMove(a,mode);
	    }
	    break;
	case MODEPATROL:
	    SetModeMove(a,mode);
	    initmoveaction(a,destobj,mode,0,0,x,y);
	    break;
	case MODEACTIVATE:
	    if (destobj)
	    {
		a->modemove = mode;
		initmoveaction(a,NULL,mode,0,0,GetOBJx(destobj),GetOBJy(destobj));
	    }
	    break;
	case MODEMOVE:
	    a->modemove = mode;
	    if (destobj)
	    {
		CheckIfGotoTransport(a,destobj);
		CheckIfGotoBattery(a,destobj);
		if (!a->currentspeed)
		{
		    flingy_id = alldattbl.units_dat->flingy_id[a->SC_Unit];
		    if (alldattbl.flingy_dat->MoveControl[flingy_id] == FLINGYMOVECONTROL_FLINGY  || 
			alldattbl.flingy_dat->MoveControl[flingy_id] == FLINGYMOVECONTROL_WEAPON   )
		    {
			a->prop |= VARMOVEOBJACT;
			a->finalOBJ = destobj;
		    }
		}
	    }
	    initmoveaction(a,destobj,mode,0,0,x,y);
	    break;
	case MODETANKSIEGE:
	    if (!MageDepend(a,a->playernr,mode))
	    {
	        return(MOVEOBJ_NOACT);
	    }
	    if (a->modemove == MODETANKSIEGE || a->modemove == MODETANKNORMAL)
	        return(MOVEOBJ_NOACT);
	    SetModeMove(a,mode);
	    inv = a->mainimage->invisiblecolors;
	    SetOrder(a,1,&SIGOrder_Tank_AfterSiegeCmd);
	    if (a->SC_Unit == SC_TANKNORMALOBJ)
		ChangeUnitAndImagesAssociated(a,SC_TANKSIEGEOBJ);
	    else
		if (a->SC_Unit == SC_HERO_EDMUNDDUKETMOBJ)
		    ChangeUnitAndImagesAssociated(a,SC_HERO_EDMUNDDUKESMOBJ);
	    a->mainimage->invisiblecolors = inv;
	    break;
	case MODETANKNORMAL:
	    if (!MageDepend(a,a->playernr,mode))
	    {
	        return(MOVEOBJ_NOACT);
	    }
	    if (a->modemove == MODETANKSIEGE || a->modemove == MODETANKNORMAL)
	        return(MOVEOBJ_NOACT);
	    SetModeMove(a,mode);
	    SetOrder(a,1,&SIGOrder_Tank_AfterNormalCmd);
	    SetOBJIScriptNr(a,ISCRIPTNR_SPECIALSTATE2,ISCRIPTNR_SETONLY);
	    break;
	case MODEBURROW:
	case MODEBURROW2:
	    if (!MageDepend(a,a->playernr,mode))
	    {
	        return(MOVEOBJ_NOACT);
	    }
	    if (a->modemove == mode)
	        return(MOVEOBJ_NOACT);
	    if (!SetOBJIScriptNr(a,ISCRIPTNR_BURROW,ISCRIPTNR_SETONLY))
	    {
		if (a->SC_Unit == SC_LURKEROBJ)
		    a->mainimage->UnitNeededDirection256(9*8);
		else
		    a->mainimage->UnitNeededDirection256(12*8);
		//special action for drone
		if (a->SC_Unit == SC_DRONEOBJ)
		{
		    a->mainimage->flags |= SC_IMAGE_FLAG_ISCRIPTROTATION;
		    a->modemove=MODESTOP;
		    a->prop &=~VARACCELERATIONBIT;
		    a->currentspeed=0;
		}
		SetOrder(a,4,&SIGOrder_AfterBurrow);
		SetModeMove(a,mode);
		if (mode == MODEBURROW || (mode == MODEBURROW2 && a->SC_Unit !=SC_LURKEROBJ ))
		    Play_sfxdata(a->mainimage->xpos>>8,a->mainimage->ypos>>8,SFXDATA_BURROWDOWN,2);
	    }
	    break;
	case MODEUNBURROW:
	    if (!MageDepend(a,a->playernr,mode))
	    {
	        return(MOVEOBJ_NOACT);
	    }
	    if (a->modemove == mode)
	        return(MOVEOBJ_NOACT);
	    if (!SetOBJIScriptNr(a,ISCRIPTNR_UNBURROW,ISCRIPTNR_SETONLY))
	    {
		SetBurrowFlag(a,0);
		SetMageAtr(&a->atrobj,ATRINVISIBLE,0);
		a->mainimage->invisiblecolors = MININVISIBLECOLOR;
		SetOrder(a,4,&SIGOrder_AfterUnburrow);
		SetModeMove(a,mode);
	    }
	    break;
	case MODEDIE:
            doselectedOBJbit(a,NUMBGAMER,0);
	    deselectobj(a);
            a->blinkvalue = 0;
	    if (a->movelist)
		a->movelist->EmptyElemFifo();
	    a->modemove = mode;
	    if (GetMageAtr(&a->atrobj,ATRHALLUCINATION)!=0)
	    {
		a->mainimage->SetGrpFunc(IMAGEID_HALLUCINATIONDIE1);
		Play_sfxdata(a->mainimage->xpos>>8,a->mainimage->ypos>>8,SFXDATA_HALLUCINATIONDIE,2);
		a->mainimage->offsetcmdinbuf = 0;
		a->mainimage->SetIScriptNr(ISCRIPTNR_INIT);
	    }
	    a->mainimage->newgrpmethod = NORMAL;
	    SetOBJIScriptNr(a,ISCRIPTNR_DEATH,ISCRIPTNR_SETONLY);
	    break;
	case MODELANDING:
	    deltaz = hypot(GetOBJx(a)-x,GetOBJy(a)-y+AIRUNITS_START_YPOS);
	    if ( deltaz <= 1)
	    {
		if (!CheckIfCanLand(a,x,y,&constrerror))
		{
		    moveobj(a,NULL,MODESTOP,0,0,NOSHOWERROR,0);
		    if (a->playernr == NUMBGAMER && showerrorflag)
	    	    {
			Play_sfxdata_id(fordeselect[0],PUTLIFTDOWNERROR,2,0);
			putbuildplacemessage(-constrerror);
		    }
		    return(MOVEOBJ_NOACT);
		}
		a->modemove = mode;
		a->deltavertpos = y - GetOBJy(a);

		a->mainimage->xpos = x<<8;

		SetOrder(a,16,&SIGOrder_Landing);
		SetOBJIScriptNr(a,ISCRIPTNR_LANDING,ISCRIPTNR_EXECUTE);
		a->finalx = 0;//erase rallypoint
		a->finaly = 0;
	    }
	    else
	    {
		moveobj(a,NULL,MODEMOVE,x,y-AIRUNITS_START_YPOS,NOSHOWERROR,0);
		AddModeMove(a,NULL,mode,x,y,showerrorflag);
	    }
	    break;
	case MODELIFTOFF:
	    a->modemove = mode;
	    AddDustImages(a,IMAGE_OVERLAY_LIFTOFF);
	    SetOrder(a,16,&SIGOrder_LiftOff);
	    SetOBJIScriptNr(a,ISCRIPTNR_LIFTOFF,ISCRIPTNR_SETONLY);
	    DettachAddon(a);
	    a->deltavertpos = AIRUNITS_START_YPOS;
	    break;
        case MODECLOAKFIELD:
        case MODEPERSONNELCLOAK:
	    if (accesstomove(a,b,mode,a->playernr))
	    {
		if (!MageDepend(a,a->playernr,mode))
		{
	    	    return(MOVEOBJ_NOACT);
		}
		needmana = getusemanaformodemove(mode);
		if (CheckForMana(a,needmana)!=CHECKRES_OK)
		{
		    playinfoadvisorsound(a->playernr,GetUnitRace(a->SC_Unit),ADVENERGY,PLAYADVISOR_TEXTANDSOUND);
		    return(MOVEOBJ_NOACT);
		}
		DecrMana(a,needmana);
        	addmage(a,ATRINVISIBLE,a->mana);
        	a->prop |= VARINCRMAGE;
		ChangeTypeOfProp(a,b,PROPNORMAL2);
            }
	    return(MOVEOBJ_DONE);
	case MODEDECLOAK:
            addmage(a,ATRINVISIBLE,0);
            a->prop &= ~VARINCRMAGE;
	    ChangeTypeOfProp(a,b,PROPNORMAL1);
	    return(MOVEOBJ_DONE);
        case MODEESCAPE1:	//escape slots in construct mode
        case MODEESCAPE2:
        case MODEESCAPE3:
        case MODEESCAPE4:
        case MODEESCAPE5:
escapeconstrslots:
	    slotnr = mode - MODEESCAPE1;
//            if (!(a->prop & VARNOTWORK))
	    {
		    SlotReturnResources(a,a->playernr,slotnr);
		    ret = DelConstruct(a,slotnr,&type_id,&obj_id);
		    if (ret)
		    {
			switch(type_id)
			{
			    case ORDERS_UNIT:
				ChangeSupply(a->playernr,obj_id,MINUSFACTOR);
				if (IsAddon(obj_id))
				{
				    a->addonobj->addonobj = NULL;
				    dieobj(a->addonobj);
				    a->addonobj = NULL;
				}
				break;
			    case ORDERS_UPGRADE:
				break;
			    case ORDERS_TECHNOLOGY:
				break;
			    default:
				break;

			}
		    }
	    }
	    break;
	case MODEESCAPE:
	    if (IsOBJUnderConstruct(a))
	    {
		SetObjWalk8(&map,a,CLEARWALK);
		switch(GetUnitRace(a->SC_Unit))
		{
		    case ZERGRACE:
			ReturnResources(a->SC_Unit,a->playernr,100);
		        switch(a->SC_FromUnit)
		        {
			    case SC_DRONEOBJ:
				SIGOrder_CancelMorthFromDrone(a);
				break;
		    	    case SC_LARVAEOBJ:
			        dieobj(a);
				break;
		    	    case SC_HYDRALISKOBJ:
		    		SetOrder(a,4,SIGOrder_CancelMorthFromHydralisk);
		    		SetOBJIScriptNr(a,ISCRIPTNR_SPECIALSTATE2,ISCRIPTNR_EXECUTE);		//egg deconstruct script
				break;
		    	    case SC_MUTALISKOBJ:
		    		SetOrder(a,4,SIGOrder_CancelMorthFromMutalisk);
		    		SetOBJIScriptNr(a,ISCRIPTNR_SPECIALSTATE2,ISCRIPTNR_EXECUTE);		//cocoon deconstruct script
				break;
			    case SC_HATCHERYOBJ:
			    case SC_LAIROBJ:
			    case SC_SPIREOBJ:
			    case SC_CREEPCOLONYOBJ:
				SIGOrder_CancelMorthFromBuild(a);
				break;
			}
			break;
		    case TERRANRACE:
			ReturnResources(a->SC_Unit,a->playernr,a->selfconstruct.timeremained * 100 / a->selfconstruct.timemax);
		        if (IsAddon(a->SC_Unit))
		    	    delADVobj(a);
			else	
			    dieobj(a);
			break;
		    case PROTOSSRACE:
			if (a->SC_Unit == SC_ARCHONOBJ || a->SC_Unit == SC_DARKARCHONOBJ)
			    return(MOVEOBJ_NOACT);
			ReturnResources(a->SC_Unit,a->playernr,100);
			dieobj(a);
		        break;
		}
	    }
	    else
	    {
		if (a->construct)
		{
		    mode = a->construct->nrofelem + MODEESCAPE1 - 1;
		    goto escapeconstrslots;
		}
	    }
	    break;
	case MODEPOWERON:
	    a->prop &= ~(VARNOTWORK | VARPOWEROFF);
	    if (GetConstructNR(a))
		SetOBJIScriptNr(a,ISCRIPTNR_ISWORKING,ISCRIPTNR_EXECUTE);
	    break;
	case MODEPOWEROFF:
	    a->prop |= (VARNOTWORK | VARPOWEROFF);
	    if (a->SC_Unit == SC_PHOTONCANONOBJ && a->modemove == MODEATACK)
	    {
		EndAtackAction(a);
		SetModeMove(a,MODESTOP);
	    }
	    else
		if (a->SC_Unit == SC_BATTERYOBJ || GetConstructNR(a))
		    SetOBJIScriptNr(a,ISCRIPTNR_WORKINGTOIDLE,ISCRIPTNR_EXECUTE);
	    break;
	case MODESTIMPACK1:
	case MODESTIMPACK2:
	    if (a->life <= STIMPACKMINLIFE)
	        return(MOVEOBJ_NOACT);
    	    addmage(a,ATRSTIMPACK,STIMPACKTIME);
	    a->life -= STIMPACKMINLIFE;
	    if (mageprop[mode].sound_id[SOUNDONCAST])
		Play_sfxdata(GetOBJx(a),GetOBJy(a),mageprop[mode].sound_id[SOUNDONCAST],2);
	    break;
	case MODEENTERTRANSPORT:
	    if (destobj)
	    {
		//check is too far to transport
		if ( GetDistanceTo(destobj,GetOBJx(a),GetOBJy(a)) <= 48 )
		{
		    if (a->playernr == destobj->playernr)
		    {
			if (destobj->SC_Unit == SC_BUNKEROBJ)
		    	    LoadObjInObj(a,b,destobj,1,4);
			else
			    LoadObjInObj(a,b,destobj,4,8);
		    }
		}
		else
		{
		    moveobj(a,destobj,MODEMOVE,0,0,NOSHOWERROR,0);
		}
	    }
	    break;
	case MODEWARPINIT:
	    CreateArchon(a,destobj);
	    break;
	case MODEWARPARCHON:
	case MODEWARPDARCHON:
	    if (destobj)
	    {
		a->modemove = mode;
		//move to destination
		initmoveaction(a,destobj,mode,0,0,x,y);
		AddModeMove(a,destobj,MODEWARPINIT,x,y,showerrorflag);//cast mage after
//		a->prop |= VARMOVETHROWUNITS|VARSUICIDE;
//		destobj->prop |= VARMOVETHROWUNITS|VARSUICIDE;
	    }
	    return(MOVEOBJ_DONE);
	case MODEUNLOADALLUNITS:
	    if (a->loaded)
	    {
		tempvar = a->loaded->nrofloadedunits;
		for (i=0;i<tempvar;i++)
		    UnLoadObjInObj(a,0,NEWXYCOORDS,0,0,0);
		return(MOVEOBJ_DONE);
	    }
	    return(MOVEOBJ_NOACT);
	    break;
	case MODEHEAL:
	    if (destobj == NULL)
	    {
		moveobj(a,NULL,MODESTOP,0,0,NOSHOWERROR,0);
		return(MOVEOBJ_NOACT);
	    }
	    if (ishealingobj(destobj))
	    {
		//check is too far to transport
		a->modemove = mode;
		a->castmagenr = mode;
		a->finalOBJ = destobj;
		if (GetDistanceBetweenUnits256(a,destobj) <= mageprop[mode].mindistance+(10<<8) )
		{
		    SetOrder(a,1,&SIGOrder_AfterCastHeal);
		    SetOBJIScriptNr(a,ISCRIPTNR_SPECIALSTATE1,ISCRIPTNR_EXECUTE);	//cast heal
		}
		else
		{
		    initmoveaction(a,destobj,mode,0,0,GetOBJx(destobj),GetOBJy(destobj));
		    AddModeMove(a,destobj,MODEHEAL,GetOBJx(destobj),GetOBJy(destobj),NOSHOWERROR);//cast mage after
		}
	    }
	    break;
	case MODERECHARGESHIELD:
	    if (destobj)
	    {
		if (!isrechargebleobj(destobj))
		    return(MOVEOBJ_NOACT);
		a->finalOBJ = destobj;
		if (GetDistanceBetweenUnits256(a,destobj) <= mageprop[mode].mindistance)
		{
		    a->data.battery.rechargetime = BATTERYRECHARGEWAITTIME;
		    a->modemove = mode;
		    a->castmagenr = mode;
		    SetOBJIScriptNr(a,ISCRIPTNR_ISWORKING,ISCRIPTNR_EXECUTE);	//cast recharge shield
		}
		else
		{
		    //unit moves to shield battery?!
		}
	    }
	    break;
	case MODEUNLOADUNITNR:
//	    DEBUGMESSCR("unload %d\n",x);
	    if (a->modemove != MODEUNLOADUNITS)	//prevent unload unit nr then unload all units
	    {
		if (a->loaded)
		{
		    a->loaded->nrunittounload = x;
		}
	    }
	    break;
	case MODEUNLOADUNITS:
	    if (destobj)
	    {
		x = GetOBJx(destobj);
		y = GetOBJy(destobj);
		destobj = NULL;
	    }
	    if (GetDistanceTo(a,x,y) <= 10)
	    {
		a->modemove = mode;
	    }
	    else
	    {
		a->modemove = MODEMOVE;
		initmoveaction(a,NULL,MODEMOVE,0,0,x,y);
		AddModeMove(a,NULL,MODEUNLOADUNITS,x,y,NOSHOWERROR);
	    }
	    break;
	case MODEATACKREADY:
	    if (destobj)
	    {
		a->finalOBJ = destobj;
		SetAtackType(a,destobj);
		switch(a->SC_Unit)
		{
		    case SC_VULTUREMINEOBJ:
			SetOrder(a,1,&SIGOrder_NULL);
			a->castmagenr = MODEMINEEXPLODE;
    			SpecialAtackAction(a,destobj,ISCRIPTNR_SPECIALSTATE1);
			break;
		    default:
			AtackAction(a,destobj,0);
			break;
		}
	    }
	    else
		ApplyNextModeMove(a);
	    break;
	case MODEATACK:
	    if (destobj)
	    {
		if (!(showerrorflag & ATACKMOVEBIT))
		    a->prop &= ~VARMOVEINATACKMODE;
		ret = AtackCoolDownEnds(a,destobj,0,showerrorflag);
		if (ret == MOVEOBJ_DONE)
		{
//		    initmoveaction(a,destobj,MODEATACK,0,0,x,y);
		    a->modemove = MODEATACK;
		    a->finalOBJ = destobj;
		    SetAtackType(a,destobj);
		    AtackAction(a,destobj,0);
		}
	    }
	    else
	    {
		//atack to ground, check if object can move
		if (accesstomove(a,loadobj(a->SC_Unit),MODEMOVE,a->playernr))
		{
//		    DEBUGMESSCR("atack to ground is under develop\n");
		    a->prop |= VARMOVEINATACKMODE;
		    a->searchforatack_tick = 0;//fast search
		    a->modemove = MODEATACK;
		    a->finalxatack = x;
		    a->finalyatack = y;
		    initmoveaction(a,NULL,MODEATACK,0,0,x,y);
		}
		else
		    return(MOVEOBJ_NOACT);
	    }
	    break;
	case MODERELEASEMINENOW:
	    if (a->mines--)
		createobjfulllife(x,y,SC_VULTUREMINEOBJ,a->playernr);
	    ApplyNextModeMove(a);
	    break;
	case MODERELEASEMINE:
	    initmoveaction(a,NULL,mode,0,0,x,y);
	    AddModeMove(a,destobj,MODERELEASEMINENOW,x,y,NOSHOWERROR);
	    break;
	case MODEDOODADCHANGESTATE:
	    state = GetDoodadState(a);
	    openstate = x;
	    if (a->SC_Unit == SC_LEFTUPPERLEVELDOOROBJ || 
		a->SC_Unit == SC_RIGHTUPPERLEVELDOOROBJ || 
		a->SC_Unit == SC_LEFTPITDOOROBJ || 
		a->SC_Unit == SC_RIGHTPITDOOROBJ )
	    {
	        comparefunc = DoorConditionFunctions[openstate];
	    }
	    else
	    {
	        comparefunc = DoorConditionFunctions[openstate];
    	    }
    	    (*comparefunc)(&state,-1);
	    DoodadChangeState(a,state);
	    break;
	default:
	    DEBUGMESSCR("mode=%d(%s)not developed\n",mode,mageprop[mode].namemage);
	    break;
    }
    return(MOVEOBJ_DONE);
}
//==========================================
void CheckIfGotoBattery(OBJ *a,OBJ *destobj)
{
    switch(destobj->SC_Unit)
    {
	case SC_BATTERYOBJ:
	    break;
    }
}
//==========================================
void CheckIfGotoTransport(OBJ *a,OBJ *destobj)
{
    //check if can enter tarnsport
    if (GetSpaceRequired(a->SC_Unit) == 255)
	return;
    if (GetMageAtr(&destobj->atrobj,ATRHALLUCINATION))
	return;
    switch(destobj->SC_Unit)
    {
	case SC_OVERLORDOBJ:
	    //check if have transport ability
	    if (!GetUpgradeTree(&map,destobj->playernr,UPGRADES_DAT_OVERLORDTRANSP))
		return;
	case SC_HERO_YGGDRASILLOBJ:
	case SC_DROPSHIPOBJ:
	case SC_SHUTTLEOBJ:
	    AddModeMove(a,destobj,MODEENTERTRANSPORT,0,0,NOSHOWERROR);
	case SC_BUNKEROBJ:
	    if (IsReadyOBJ(destobj))
	        AddModeMove(a,destobj,MODEENTERTRANSPORT,0,0,NOSHOWERROR);
	    break;
    }
}
//==========================================
void SlotReturnResources(OBJ *a,int playernr,int slotnr)
{
    int factorreturn,mcost,gcost,type_id,obj_id;
    if (!a->construct)
        return;
    if (!slotnr)
    {
	factorreturn = a->construct->timeremained * 100 / a->construct->timemax;
    }
    else
        factorreturn=100;
    if (GetConstruct(a,slotnr,&type_id,&obj_id,NULL,NULL,NULL,NULL))
    {
	switch(type_id)
	{
	    case ORDERS_UNIT:
    		GetCostUnit(obj_id,&mcost,&gcost);
		break;
	    case ORDERS_UPGRADE:
	        GetCostUpgr(obj_id,a->playernr,&mcost,&gcost);
		break;
	    case ORDERS_TECHNOLOGY:
		GetCostTech(obj_id,&mcost,&gcost);
		break;
	    default:
		mcost=0;
		gcost=0;

	}
    }
    ChangeResourcePlayer(playernr,PLUSFACTOR,mcost*factorreturn/100,gcost*factorreturn/100);
}
//==========================================
void ReturnResources(unsigned char SC_Unit,int playernr,int factorreturn)
{
    int mcost,gcost;
    GetCostUnit(SC_Unit,&mcost,&gcost);
    ChangeResourcePlayer(playernr,PLUSFACTOR,mcost*factorreturn/100,gcost*factorreturn/100);
}
//==========================================
void delADVobj(struct OBJ *a)
{
    if (a->addonobj)
    {
	ConstructUnitsReleaseSupply(a->addonobj);
	DestroyConstrStruct(a->addonobj);
    }
    dieobj(a);
}
//=================================
int accesstomove(struct OBJ *a,struct OBJstruct *b,int mode,int player)
{
    int j,prop;
    if (GetMageAtr(&a->atrobj,ATRHALLUCINATION)>0)
	if (!(mageprop[mode].atr & ORDER_SHOWHALLUCINATED))
	    return(0);

    if (a->prop & (VARNOTWORK|VARPOWEROFF))
	return(0);
    if (IsSubUnit(a->SC_Unit))
    {
	a = a->subunit;
	b = loadobj(a->SC_Unit);
    }
    for (j=0;j<MAXUNITPROPERTIES;j++)
    {
	prop=b->modemove[a->typeofprop][j];
	if (a->playernr==player)
	{
	    if (prop==mode)
		return(1);
	    else
		if ((mode==MODEBURROW&&prop==MODEBURROW2)||(mode==MODEBURROW2&&prop==MODEBURROW))
		    return(1);
	}
    }
    switch(a->SC_Unit)
    {
	case SC_DRONEOBJ:
	    if (IsOBJBurrowed(a))
	        return(0);
	    //proceed next like a other probe or scv
	case SC_PROBEOBJ:
	case SC_SCVOBJ:
	    if ((a->typeofprop != PROPNORMAL1) && a->modemove<=MODELASTSIMPLEMOVE)
		    return 1;
	    break;
	case SC_LARVAEOBJ:
	    for (j=0;j<MAXUNITPROPERTIES;j++)
    		if (b->modemove[PROPNORMAL2][j]==mode&&a->playernr==player)
		    return(1);
	    break;
    }
    return(0);
}
//==================================
//return 1 if i make move form right button of mouse
int movefrommouse(struct OBJ *a)
{
    switch(a->SC_Unit)
    {
	case SC_LARVAEOBJ:
	    return 0;
	    break;
	case SC_PROBEOBJ:
	case SC_DRONEOBJ:
	case SC_SCVOBJ:
	    if ((a->typeofprop == PROPBUILDSIMPLE || a->typeofprop == PROPBUILDADVANCED)  &&
		 !buildconstr)
	    {
	        ChangeTypeOfProp(a,PROPNORMAL1);
	        return 0;
	    }
	    break;
    }    
    return 1;
}
//==================================
int makemove(struct OBJ *a,struct OBJ *destobj,int locx,int locy,int mode,int player,int showerrorflag)
{
    struct OBJstruct *b;
    b=loadobj(a->SC_Unit);
    if (mode >= MODEACTIVATE && mode <= MODEESCAPE5)
	;
    else
    {
    	if (!accesstomove(a,b,mode,player))
	    return(0);
	if (!accesstomage(a,destobj,mode))
	{
	    if (player==NUMBGAMER)
		if (gameconf.audioconf.advisorsounds)
		    Play_sfxdata_id(a,PUTMAGEERROR,1,0);
    	    return(0);
	}
    }
    if (a->movelist)
	a->movelist->EmptyElemFifo();
    if ((IsOnSkyOBJ(a))||(mode==MODEESCAPE))
	moveobj(a,destobj,mode,locx,locy,showerrorflag,0);//airobj
    else
	moveobj(a,destobj,mode,locx,locy,showerrorflag,0);//terraobj
    return(1);
}
//==================================
int selectedobjmove(struct OBJ *destobj,int locx,int locy,int mode,int player,int showerrorflag,int rightclick)
{
    int minx,miny,maxx,maxy,midx=0,midy=0,deltax=0,deltay=0,flags=SHOWERRORTEXT;
    int i,k=0,maxmoveobj=0;
    struct OBJ *speakOBJ;
    struct OBJ *moveobjects[12];
       for (i=0;i<MAXSELECTMAN;i++)
         if (fordeselect[i]
		    &&
	     (!(fordeselect[i]->prop&VARNOTSEEPROP))
	    	    &&
	    !(fordeselect[i]->prop&VARNOTWORK)
	    )
        {
              //elsi move na sebea
            if (fordeselect[i]==destobj)
            {
//        	if (!(mageprop[mode].atr&VARMAKEONMYSELF))
//            	    continue;
//        	else
            	    if (mode != MODELANDING)
		    {
                	locx = GetOBJx(destobj);
                	locy = GetOBJy(destobj);
                	destobj=NULL;
                    }
    	    }
    	    if (movefrommouse(fordeselect[i]))
	    {
		moveobjects[maxmoveobj++]=fordeselect[i];
	    }
        }
        minx=256*32;
        miny=256*32;
        maxx=0;
        maxy=0;
    	for (i=0;i<maxmoveobj;i++)
	{
	    if (GetOBJx(moveobjects[i]) < minx)
		minx = GetOBJx(moveobjects[i]);
	    if (GetOBJy(moveobjects[i]) < miny)
		miny = GetOBJy(moveobjects[i]);
	    if (GetOBJx(moveobjects[i]) > maxx)
		maxx = GetOBJx(moveobjects[i]);
	    if (GetOBJy(moveobjects[i]) > maxy)
		maxy = GetOBJy(moveobjects[i]);
	}
	if (maxx-minx<=128 && maxy-miny<=128)
	{
	    //move in group
	    if (locx < minx || locy < miny || locx > maxx || locy > maxy)
	    {
		midx=(maxx-minx)/2+minx;
	        midy=(maxy-miny)/2+miny;
	    }
	}
	if (mode == MODEATACK)
	    flags |= ATACKMOVEBIT;
    	for (i=0;i<maxmoveobj;i++)
    	{
    	    if (midx)
    	    {
    		deltax = GetOBJx(moveobjects[i]) - midx;
    	        deltay = GetOBJy(moveobjects[i]) - midy;
    	    }
	    moveobjects[i]->prop &= ~VARAUTOMATRESOURCERET;
	    if (rightclick && destobj)
		mode = GetDefaultModeForRightClick(moveobjects[i],destobj,player);
	    if (mode != MODENON)
		k |= makemove(moveobjects[i],destobj,locx+deltax,locy+deltay,mode,player,flags);
        }
	speakOBJ = GetMaxRankOBJ(maxmoveobj,moveobjects);
	if (speakOBJ)
	    activatesound(speakOBJ,MODESOUNDACTION,0,NOSTOPCURRENTSOUNDS);
        return(k);
}
//=================================
int GetDefaultModeForRightClick(OBJ *a,OBJ *destobj,int playernr)
{
	int isenemy,modemove;
	isenemy = player_aliance(playernr,destobj->playernr);
	switch(alldattbl.units_dat->RightClickAction[a->SC_Unit])
	{
	    case UNIT_AIACTION_RIGHTCLICK_NOCMD_AUTOATACK:		//0
	    case UNIT_AIACTION_RIGHTCLICK_NOMOVE_NORMALATACK:		//3
		if (isenemy == ENEMYOBJ)
		    modemove = MODEATACK;
		else
		    modemove = MODENON;
		break;
	    case UNIT_AIACTION_RIGHTCLICK_NORMALMOVE_NORMALATACK:	//1
		if (isenemy == ENEMYOBJ)
		    modemove = MODEATACK;
		else
		    modemove = MODEMOVE;
		break;
	    case UNIT_AIACTION_RIGHTCLICK_NORMALMOVE_NOATACK:		//2
		modemove = MODEMOVE;
		break;
	    case UNIT_AIACTION_RIGHTCLICK_HARVEST:			//4
		if (isenemy == ENEMYOBJ)
		{
		    modemove = MODEATACK;
		}
		else
		{
		    if (IsResourceContainer(destobj->SC_Unit) && !a->carryobj)
		    {
			if (destobj->SC_Unit == SC_GEYSEROBJ)
			{
			    //show cannot gather from geyser
			    activatesound(a,MODESOUNDERROR,0,STOPCURRENTSOUNDS);
			    playinfoadvisorsound(playernr,GetUnitRace(a->SC_Unit),ADVGEYSERNOTREADY,PLAYADVISOR_ONLYTEXT);
			    modemove = MODENON;
			}
			else
			{
			    modemove = MODEGATHER;
			}
		    }
		    else
		    {
			if (a->carryobj && IsResourceDepot(destobj->SC_Unit))
			{
		    	    modemove = MODERETURNCARGO;
			}
			else
			{
			    modemove = MODEMOVE;
			}
		    }
		}
		break;
	    case UNIT_AIACTION_RIGHTCLICK_HARVEST_REPAIR:		//5
		if (isenemy == ENEMYOBJ)
		{
		    modemove = MODEATACK;
		}
		else
		{
		    if (IsResourceContainer(destobj->SC_Unit) && !a->carryobj)
		    {
			if (destobj->SC_Unit == SC_GEYSEROBJ)
			{
			    //show cannot gather from geyser
			    activatesound(a,MODESOUNDERROR,0,STOPCURRENTSOUNDS);
			    playinfoadvisorsound(playernr,GetUnitRace(a->SC_Unit),ADVGEYSERNOTREADY,PLAYADVISOR_ONLYTEXT);
			    modemove = MODENON;
			}
			else
			{
			    modemove = MODEGATHER;
			}
		    }
		    else	//check if need to repair
		    {
			if (a->carryobj && IsResourceDepot(destobj->SC_Unit))
			{
			    modemove = MODERETURNCARGO;
			}
			else
			{
			    if (IsMechanical(destobj->SC_Unit) && 
				IsTerranRace(destobj->SC_Unit) && 
				destobj->life != GetUnitMaxLife(destobj->SC_Unit))
			    {
				    modemove = MODEREPAIR;
			    }
			    else
			    {
				modemove = MODEMOVE;
			    }
			}
		    }
		}
		break;
	    case UNIT_AIACTION_RIGHTCLICK_NOTHING:			//6
		modemove = MODENON;
		break;
	    default:
		modemove = MODENON;
		break;
	}
	if (IsInvincibleOBJ(destobj))
	{
	    if (destobj->SC_Unit >= SC_ZERGBEACONOBJ && destobj->SC_Unit <= SC_PROTOSSFLAGBEACONOBJ)
		modemove = MODEACTIVATE;
	    else
		if (modemove == MODEATACK)
		    modemove = MODEMOVE;
	}
	return(modemove);
}
//=================================
struct OBJ* GetMaxRankOBJ(int maxobjs,OBJ *objs[])
{
    int maxrank=-1,unitrank,i;
    struct OBJ *maxrankOBJ=NULL;
    for (i=0;i<maxobjs;i++)
    {
	if (objs[i])
	{
	    unitrank = GetUnitSublabel(objs[i]->SC_Unit);
	    if (unitrank>maxrank)
	    {
	        maxrank=unitrank;
	        maxrankOBJ = objs[i];
	    }
        }
    }
    return(maxrankOBJ);
}
//=================================
void sortselectedunits(struct OBJ *a[],int maxcount)
{
    int i,j,size1,size2;
    for (i=0;i<maxcount-1;i++)
    {
	if (a[i])
	    size1 = GetSpaceRequired(a[i]->SC_Unit);
	else
	    size1 = 8;
	for (j=i+1;j<maxcount;j++)
	{
	    if (a[j])
		size2 = GetSpaceRequired(a[j]->SC_Unit);
	    else
		size2 = 8;
	    if (size1>size2)
	    {
		size1 = size2;
		swapOBJ(&a[i],&a[j]);
	    }
	}
    }
}
//=================================
void transportanalizeobj(struct OBJ *a)
{
    int weightobj,i,j,pos=0;
    struct OBJ *c;
    memset(&a->loaded->cellobj,-1,sizeof(a->loaded->cellobj));

    for (i=0;i<a->loaded->nrofloadedunits;i++)
    {
	c = a->loaded->loadedunits[i];
	weightobj = GetSpaceRequired(c->SC_Unit);
	if (weightobj>1)
	    if (pos&1)
		pos++;
	for (j=0;j<weightobj;j++)
	    a->loaded->cellobj[pos+j] = i;
	pos+=weightobj;
    }
}
//=================================
int unloadcoordxy[4][2]={
			    {-15,-10},
			    { 15,-10},
			    {-15, 20},
			    { 15, 20}
			};
//=================================
void LoadObjInObj(struct OBJ *a,struct OBJstruct *b,struct OBJ *c,
		 int maxweighttoenter,int maxweight)
{
    int weightobj;
    if (!c->loaded)
    {
	c->loaded = (struct LOADED *) wmalloc(sizeof(struct LOADED));
	memset(c->loaded,0,sizeof(struct LOADED));
	c->loaded->nrunittounload = -1;
    }
    weightobj = GetSpaceRequired(a->SC_Unit);
    if (weightobj<=maxweighttoenter&&
	c->loaded->weight+weightobj<=maxweight)
    {
	c->loaded->weight += weightobj;
	c->loaded->loadedunits[c->loaded->nrofloadedunits] = a;
	if (c->SC_Unit == SC_BUNKEROBJ)
	{
	    //if transport is bunker need to chenge coordinates to centered in bunker
    	    SetOBJxy256(a,GetOBJx256(c) + (unloadcoordxy[c->loaded->nrofloadedunits][0]<<8),
    			  GetOBJy256(c) + (unloadcoordxy[c->loaded->nrofloadedunits][1]<<8));
	    ForceKartChanges(a);
	}
	c->loaded->nrofloadedunits++;
	a->in_transport = c;
	moveobj(a,NULL,MODESTOP,0,0,NOSHOWERROR,0);
	Play_sfxdata(GetOBJx(c),GetOBJy(c),SFXDATA_ZINTRANSPORT+GetUnitRace(c->SC_Unit),2);
	//disable drawing in transport unit
	a->prop |= (VARINTRANSPORT | VARCANTSELECT | VARNOTHERE);
        
        a->mainimage->HideChildsImgFlag();
        a->mainimage->HideImgFlag();
	
	sortselectedunits(c->loaded->loadedunits,8);
	transportanalizeobj(c);
	doselectedOBJbit(a,a->playernr,0);
	deselectobj(a);
	if (selectobjcount()==1)
	{
	    ifselectTRANSPORTS(getfirstselectedobj(fordeselect));
	}
	DeleteOldObjPointers(a);
    }
    //if selected transport need to refresh cargo
    if (transportplaceobj==c)
    {
	AnalizeAllTransportUnits(c,transportplace,&transportplaceobj);
    }
}
//=================================
int GetOBJNrInTransport(OBJ *c,OBJ *a)
{
    int i;
    for (i=0;i<c->loaded->nrofloadedunits;i++)
	if (c->loaded->loadedunits[i] == a)
	    return(i);
    DEBUGMESSCR("BAD TRANSPORT UNIT\ncontinue run is bad way\n");
    return(-1);
}
//=================================
void FreeOBJFromTransport(OBJ *c,OBJ *a,int nrofunloadobj)
{
    int weightobj;
    a->in_transport = NULL;
    weightobj = GetSpaceRequired(a->SC_Unit);
    c->loaded->weight -= weightobj;
    c->loaded->nrofloadedunits--;
    if (c->loaded->nrofloadedunits)
    {
        c->loaded->loadedunits[nrofunloadobj] = c->loaded->loadedunits[c->loaded->nrofloadedunits];
    }
    c->loaded->loadedunits[c->loaded->nrofloadedunits] = NULL;
    if (!c->loaded->nrofloadedunits)
    {
        wfree(c->loaded);
        c->loaded=NULL;
    }
    if (c->loaded)
    {
        sortselectedunits(c->loaded->loadedunits,8);
        transportanalizeobj(c);
    }
    if (transportplaceobj==c)
    {
        AnalizeAllTransportUnits(c,transportplace,&transportplaceobj);
    }
}
//=================================
void UnLoadObjInObj(struct OBJ *c,int nrofunloadobj,int typeofnewxycoords,int deltax,int deltay,int silentlyflag)
{
    if (c->loaded)
    {
	int x=-1,y=-1;
	struct OBJ *a;
	a = c->loaded->loadedunits[nrofunloadobj];
	if (!a)
	    return;
	switch(typeofnewxycoords)
	{
	    case NEWXYCOORDS:
		getcoordofnewunit(c,a->SC_Unit,&x,&y);
		if (x==-1||y==-1)
		    return;//cannot unload unit in unloaded space
		break;
	    case NOCHANGEXYCOORDS:
		x = GetOBJx(a);
		y = GetOBJy(a);
		break;
	}
	FreeOBJFromTransport(c,a,nrofunloadobj);
	a->prop &= ~(VARINTRANSPORT | VARCANTSELECT | VARNOTHERE);
        
        a->mainimage->ShowChildsImgFlag();
        a->mainimage->ShowImgFlag();

	moveobj(a,NULL,MODESTOP,0,0,NOSHOWERROR,0);

	if (typeofnewxycoords == NEWXYCOORDS)
	{
	    ChangeObjXY(a,x,y);
	    ForceKartChanges(a);
	}
	if (!silentlyflag)
	    Play_sfxdata(GetOBJx(c),GetOBJy(c),SFXDATA_ZOUTTRANSPORT+GetUnitRace(c->SC_Unit),2);

	if (!IsOnSkyOBJ(a))
	    SetObjWalk8(&map,a,SETWALK);
    }
}
//=================================
void dieobj_silently(OBJ *a)
{
    a->mainimage->HideChildsImgFlag();
    a->mainimage->HideImgFlag();
    a->mainimage->EnableExecScript();

    a->prop |= VARDIESILENTLY;
    dieobj(a);
}
//=================================
void dieobj(struct OBJ *a)
{
    unsigned char SC_Unit;
    struct OBJstruct *b,*b1;
    int i,needtodestroyonexit,inegg,damage,oldsnd,changesupply=0;
    struct OBJ *a1,*newobj;

    if (a->prop & VARMARKFORDESTROY)
	return;		//already died
    if (a->in_transport)
    {
	FreeOBJFromTransport(a->in_transport,a,GetOBJNrInTransport(a->in_transport,a));
	dieobj_silently(a);
	return;
    }
    addremoveuniteffectfrommap(a,-1,&map);

    a->currentspeed = 0;
    a->prop &= ~VARACCELERATIONBIT;
    
    moveobj(a,NULL,MODEDIE,0,0,NOSHOWERROR,0);

    needtodestroyonexit=0;
    deselectobj(a);
    SC_Unit=a->SC_Unit;
    b = loadobj(SC_Unit);
    switch(SC_Unit)
    {
	case SC_ASSIMILATOROBJ:
	case SC_EXTRACTOROBJ:
	case SC_REFINERYOBJ:
	    newobj = a->resourceobj;
	    if (newobj)		//somebody extarct gas from here
	    {
		newobj->resourceobj = NULL;
		newobj->prevresourceobj = NULL;
		a->resourceobj = NULL;
		a->prevresourceobj = NULL;
		//restore visibility for unit
		newobj->finalOBJ = NULL;
		newobj->mainimage->ShowChildsImgFlag();
		newobj->mainimage->ShowImgFlag();
		newobj->prop &= ~VARNOTHERE;
		moveobj(newobj,NULL,MODESTOP,0,0,NOSHOWERROR,0);
	    }
	    break;
	case SC_NUCLEARSILOOBJ:
	    if (getchilds(a))
	    {
		a1 = a->childs->parentof[0];
		delchild(a,a1);
		dieobj_silently(a1);
		PLAYER[a->playernr].nukes--;
//		ChangeSupply(a->playernr,SC_NUKEOBJ,MINUSFACTOR);
	    }
	    break;
	case SC_GHOSTOBJ:
	    //remove nuke with trail if need
	    if (a->doubleunit)
		GhostNUKECancel(a,GHOSTCANCELNUKE);
	break;
    }
    if (a->loaded)
    {
	int maxloadedunits = a->loaded->nrofloadedunits;
	if (SC_Unit!=SC_BUNKEROBJ)
	{
	    for (i=0;i<maxloadedunits;i++)
	    {
//		ChangeSupply(a->loaded->loadedunits[i]->playernr,a->loaded->loadedunits[i]->SC_Unit,MINUSFACTOR);
		dieobj_silently(a->loaded->loadedunits[i]);
	    }
	}
	else
	{
//	    oldsnd = BlockSoundToPlay();
	    for (i=0;i<maxloadedunits;i++)
		UnLoadObjInObj(a,0,NOCHANGEXYCOORDS,0,0,1);
//	    RestoreSoundToPlay(oldsnd);
	}
    }
    if (GetConstructNR(a))
    {
	ConstructUnitsReleaseSupply(a);
	DestroyConstrStruct(a);
    }
    if (a->addonobj)
    {
	if (IsAddon(a->addonobj->SC_Unit))
	{
	    if (IsOBJUnderConstruct(a->addonobj))
	    {
		dieobj(a->addonobj);
	    }
	    else
	    {
		a->addonobj->playernr = GREYNEUTRALCOLORPLAYER;
	    }
	}
	else
	{
	    moveobj(a->addonobj,NULL,MODEESCAPE1,0,0,NOSHOWERROR,0);//cancel construct addon
	}
	if (a->addonobj)
	{
	    DettachAddon(a);
	}
    }
    if (a->resourceobj)
    {
	a->resourceobj->resourceobj = NULL;
	a->resourceobj->prevresourceobj = NULL;
	a->resourceobj = NULL;
	a->prevresourceobj = NULL;
    }
    if (a->replaceobj)
    {
	a->replaceobj->prop &= ~(VARCANTSELECT|VARNOTWORK);
        a->replaceobj->mainimage->ShowChildsImgFlag();
        a->replaceobj->mainimage->ShowImgFlag();
	a->replaceobj->mainimage->EnableExecScript();
	a->replaceobj->data.resource.resource_count = a->data.resource.resource_count;
	if (a->replaceobj->replaceobj)
	    a->replaceobj->replaceobj = NULL;
	a->replaceobj = NULL;
    }
    if (a->doubleunit)
    {
	a->doubleunit->doubleunit=NULL;
	dieobj(a->doubleunit);
	a->doubleunit=NULL;
    }
    if (a->constrobj)
    {
	FinishMainUnitConstruct(a);
    }
    if (a->myparent&&a->myparent->childs)
    {
	delchild(a->myparent,a);
    }
//    a->invisiblecolors=MININVISIBLECOLOR;
    SetCloakedFlag(a,0);
    SetMageAtr(&a->atrobj,ATRINVISIBLE,0);
    SetMageAtr(&a->atrobj,ATRDETECTOR,0);

    if (SC_Unit == SC_EGGOBJ || SC_Unit == SC_MUTALISKCOCOONOBJ || SC_Unit == SC_LURKEREGGOBJ)
	SC_Unit = a->SC_ConstrUnit;
    if (SC_Unit != SC_NOUNITNR)
    {
	if (GetMageAtr(&a->atrobj,ATRHALLUCINATION)==0)
	{
	    if (IsReadyOBJ(a))
	    {
		changesupply=1;
	    }
	    else
	    {
		if (!IsBuild(SC_Unit))
		    changesupply=1;
		else
		    if (a->SC_FromUnit == SC_HIVEOBJ || a->SC_FromUnit == SC_LAIROBJ)	//hive or lair
			changesupply=1;
	    }
	    if (changesupply)
	    {
	        inegg = GetInEgg(a->SC_ConstrUnit);
		for (i=0;i<=inegg;i++)
		{
	    	    ChangeSupply(a->playernr,SC_Unit,MINUSFACTOR);
		    ChangeMaxSupply(a->playernr,SC_Unit,MINUSFACTOR);
		}
	    }
	}
    }
//    a->prop&=~VARBEGINFIREPARSED;
    RemoveFromDestination(a);
    
//    if (needtodestroyonexit)
//	dieobj_silently(a);

    if ( SC_Unit == SC_PYLONOBJ && !IsOBJUnderConstruct(a))
	DelPylonArea(&map,a,a->playernr);
    if (IsResource(a,SC_MINERALSOBJ))
    {
        int xp = (GetOBJx(a) - GetUnitWidthAndHeight(a->SC_Unit,UNITDIM_WIDTH)/2) / SIZESPRLANSHX-3;
        int yp = (GetOBJy(a) - GetUnitWidthAndHeight(a->SC_Unit,UNITDIM_HEIGHT)/2) / SIZESPRLANSHY-3;
	DelMarkOn32(xp,yp,8,7,map.constrmap);
    }
    if (!IsOnSkyOBJ(a))
        SetObjWalk8(&map,a,CLEARWALK);
    if (cbuild==a)
    {
	buildconstr=0;
	cbuild=NULL;
    }
    if (GetMageAtr(&a->atrobj,ATRHALLUCINATION)==0)
    {
	if (a->atackernr!=-1)
	{
    	    if (TRIG_ChangeStat)
		if (IsBuild(a->SC_Unit))
		{
    		    PLAYER[a->playernr].statplayer.stat[STATPLAYER_BUILDSLOST]+=GetBuildUnitScore(a->SC_Unit);
    		    PLAYER[a->atackernr].statplayer.stat[STATPLAYER_BUILDSRISED]+=GetDestroyUnitScore(a->SC_Unit);
		}
		else
		{
    		    PLAYER[a->playernr].statplayer.stat[STATPLAYER_UNITSLOST]+=GetBuildUnitScore(a->SC_Unit);
    		    PLAYER[a->atackernr].statplayer.stat[STATPLAYER_UNITSKILLED]+=GetDestroyUnitScore(a->SC_Unit);
    		}
    	    if (a->whoatack)
    		a->whoatack->kills++;
    	}
    	add_unit_stat(&map,UNITSTAT_DEATH,a->playernr,a->SC_Unit);
    	
//    	sub_unit_stat(&map,UNITSTAT_PRODUCING,a->playernr,a->SC_Unit);
        if (IsReadyOBJ(a))
        {
//    	    sub_unit_stat(&map,UNITSTAT_HAVE,a->playernr,a->SC_Unit);
    	}
    }
    if (a->carryobj)
    {
	if (IsPickupUnit(a->carryobj->SC_Unit))
	{
	    //pickup unit will stopped
	    a->carryobj->modemove = MODESTOP;
	    a->carryobj->prop &= ~VARCANTSELECT;

	}
	a->carryobj->carryobj = NULL;
	a->carryobj = NULL;
    }
    if (a->subunit && !IsSubUnit(a->SC_Unit))
    {
	dieobj(a->subunit);
    }
    RemoveBloodFlameOverlays(a,0);
    if (b->creeptype!=NOCREEPBUILD)
	if (!IsOBJUnderConstruct(a)||(a->SC_FromUnit!=SC_NOUNITNR&&a->SC_FromUnit!=SC_DRONEOBJ))
	{
	    EmptyCreepTimed(a,b);
	}

}
//=================================
void WakeUpChild(struct OBJ *a)
{
    if (a)
    {
	a->prop &= ~VARNOTHERE;
	SetModeMove(a,MODESTOP);
	a->finalOBJ = NULL;
	a->prop &= ~VARINBASE;
    }
}
//=================================
void WakeUpAllChilds(struct OBJ *a,struct OBJstruct *b)
{
    //set childs to be unlocked for fly,atack,etc.
    if (a->childs)
	for (int i=0;i<MAXCHILDS;i++)
	    if (a->childs->parentof[i])
		if (a->childs->parentof[i]->prop&VARINBASE)
		    if (a->childs->parentof[i]->playernr==a->playernr)
			WakeUpChild(a->childs->parentof[i]);
		    else
			dieobj_silently(a->childs->parentof[i]);
}
//=================================
void ReturnedToBase(struct OBJ *a,struct OBJstruct *b)//after moved to base
{
    if (a->myparent->childs)
    {
	//object now in base need wait factor
	a->prop |= VARNOTHERE;
	MakeQueueAction(NONDETECTEDACTION,a,NULL,
			0,0,0,b->timerechargeonbase);
    }
    else
	savelog("return non-child unit to base",0);
}
//=================================
void coordcalc(struct OBJ *a,struct OBJstruct *b,int destx,int desty)
{
}
//=================================
void makeallblinking(void)
{
    int i;
    OBJ *a;
    for (i=0;i<MaxObjects;i++)
    {
	a=objects[i];
        if (a->blinkvalue)
    	    a->blinkvalue--;
    }
}
//=================================
void SetBlinkOBJ(struct OBJ *destobj)
{
    if (destobj)
	destobj->blinkvalue=BLINKTIME;
}
//=================================
int getcoordfrompatrate(int sx,int sy,int *retx,int *rety)
{
    //				a = 0
    //		 bbbbbb		b = 1
    //		c      a	c = 2
    //		c      a	d = 4
    //		 dddddd
    //
    int side=myrand(4);//get random side 0-3
    switch(side)
    {
	case 0:		//side a
	    *retx = sx-1;
	    *rety = myrand(sy);
	    break;
	case 1:		//side b
	    *retx = myrand(sx);
	    *rety = 0;
	    break;
	case 2:		//side c
	    *retx = 0;
	    *rety = myrand(sy);
	    break;
	case 3:		//side d
	    *retx = myrand(sx);
	    *rety = sy-1;
	    break;
    }
    return side;
}
//============================================
void ChangeObjXY(struct OBJ *a,int x,int y)
{
    if (x<0)
	x=SIZESPRLANSHX/2;
    if (x>MAXXMAP*SIZESPRLANSHX)
	x=MAXXMAP*SIZESPRLANSHX-SIZESPRLANSHX/2;
    if (y<0)
	y=SIZESPRLANSHY/2;
    if (y>MAXYMAP*SIZESPRLANSHY)
	y=MAXYMAP*SIZESPRLANSHY-SIZESPRLANSHY/2;
    SetOBJxy256(a,x<<8,y<<8);
}
//==========================================
void ChangeObjXYDelta(struct OBJ *a,int xdelta,int ydelta)
{
    ChangeObjXY(a,GetOBJx(a)+xdelta,GetOBJy(a)+ydelta);
}
//==========================================
void DeleteOldObjPointers(struct OBJ *a)
{
    int i;
    struct OBJ *b,*b2;
    struct OBJstruct *astr=loadobj(a->SC_Unit);
    struct MAGEARRAY *c;
    //find pointers in all objects
    for (i=0;i<MaxObjects;i++)
    {
	b = objects[i];
	if (b->finalOBJ == a)
	{
	    b->finalOBJ = NULL;
//	    b->prop &= ~VARMOVEOBJACT;
	}
	if (b->carryobj == a)
	    b->carryobj = NULL;
//	if (a->aftercooldownaction.destobj == a)
//	    a->aftercooldownaction.destobj = NULL;
	if (b->atrobj.arbiterrecallobj == a)
	    //found an recalled obj without arbiter - master of this mage
	    b->atrobj.arbiterrecallobj = NULL;
	// if have child units verify if child dead
	if (b->childs)
	{
	    for (int j=0;j<getchilds(b);j++)
	    {
		if (b->childs->parentof[j] == a)
		    b->childs->parentof[j] = NULL;
	    }
	}
	if (b->resourceobj == a)
	    b->resourceobj = NULL;
	if (b->prevresourceobj == a)
	    b->prevresourceobj = NULL;
/*	if (IsResourceContainer(a->SC_Unit))
	{
	    if (a->resourceobj == b)
	    {
		b->resourceobj = NULL;
		b->prevresourceobj = NULL;
	    }
	    if (a == b->destobj || (b->resourceobj == a && a!=b->destobj))
	    {
	        b2 = GetNearResource(b,a->SC_Unit);
		if (!b2)
		    moveobj(b,NULL,MODESTOP,0,0,NOSHOWERROR,0);
		else
	    	    moveobj(b,b2,MODEMOVE,0,0,NOSHOWERROR,0);
	    }
	}	
*/	if (b->in_transport == a)
	    b->in_transport = NULL;
	if (b->whoatack == a)
	    b->whoatack = NULL;
	if (b->finalOBJ == a)
	{
	    moveobj(b,NULL,MODESTOP,0,0,NOSHOWERROR,0);
	    b->finalOBJ = NULL;
	}
	if (b->constrobj == a)
	    b->constrobj = NULL;
	c = firstmage;
	while(c)
	{
	    if (c->whoobjputmage == a)
		c->whoobjputmage = NULL;
	    if (c->destobj == a)
		c->destobj = NULL;
	    if (c->onwhoobj == a)
		c->onwhoobj = NULL;
	    c = c->nextmage;
	}
	DelOBJFromModeList(b,a);	//search in obj b ref to obj a
    }
    for (i=0;i<MAXSELECTMAN;i++)
	if (fordeselect[i]==a)
	    fordeselect[i] = NULL;
    //find pointers in sound channels
    FreeChannelWithObj(a);
    QueueDelObj(a);
//.RemoveOBJFromEffects(a);//done in destroyobj
}
//==================================
int QueueDelObj(struct OBJ *obj)
{
    int i,ret=0;
    struct QUEUEACTION *act;
    for (i=0;i<QueueGame.GetQueueElements();i++)
    {
	act = (struct QUEUEACTION *)(QueueGame.GetElemNr(i)->ID);
	if (act->destobj==obj||act->obj==obj)
	{
	    QueueAction(act,DESTROYQUEUE);
	    QueueGame.DelQueue(i--,1);
	    ret++;
	}
    }
    return ret;
}
//==========================================
void RemoveFromDestination(OBJ *a)
{
    int i,resval;
    OBJ *a1,*tempobj;
    if (IsUnitBaseBuild(a) == 1)
    {
	for (i=0;i<MaxObjects;i++)
	{
	    a1 = objects[i];
	    if (IsWorkerUnit(a1->SC_Unit))
		if (a1->finalOBJ == a)
		{
		    a1->finalOBJ = NULL;
		}
	}
    }
    if (IsResource(a,a->SC_Unit))
    {
	for (i=0;i<MaxObjects;i++)
	{
	    a1 = objects[i];
	    if (IsWorkerUnit(a1->SC_Unit))
		if (a1->finalOBJ == a)
		{
		    a1->finalOBJ = NULL;
		    if (a->movelist)
			a->movelist->EmptyElemFifo();
		    if (IsNonNeutralFlag(a->SC_Unit))	//unit is extractor or refinery or assimilator
		    {
			//if gas deposit is destroyed unit stopped
			moveobj(a1,NULL,MODESTOP,0,0,NOSHOWERROR,0);
		    }
		    else
		    {
			//if mineral field (destroyed or dissapeared), unit stopped
			tempobj = GetNearResource(a1,a->SC_Unit,&resval);
			if (tempobj)
			    moveobj(a1,tempobj,MODEGATHER,0,0,NOSHOWERROR,0);
			else
			    if (resval)
				moveobj(a1,NULL,MODEWAITHARVESTMINERAL,0,0,NOSHOWERROR,0);
			    else
				moveobj(a1,NULL,MODESTOP,0,0,NOSHOWERROR,0);
		    }
		}
	}
    }
}
//==================================
void AddRefreshedOBJ(OBJ *a)
{
    ObjsRevealMap[MaxObjsRevealMap++]=a;
    if (MaxObjsRevealMap == MAXOBJDEFSCR)
    {
	printf("MAXOBJECTS REACHED = %d\n",MAXOBJDEFSCR);
	exit(-1);
    }
}
//==================================
void FinishMainUnitConstruct(OBJ *a)
{
    if (a->constrobj->SC_Unit == SC_SCVOBJ)
    {
	a->constrobj->prop &= ~VARMOVETHROWUNITS;
	if (a->constrobj->movelist)
	    a->constrobj->movelist->EmptyElemFifo();
	if (!ApplyNextModeMove(a->constrobj))
	{
	    ChangeTypeOfProp(a->constrobj,PROPNORMAL1);
	}
    }
    a->constrobj->constrobj = NULL;
    a->constrobj = NULL;
    ChangeTypeOfProp(a,PROPNORMAL1);
}
//==================================
void ContinueSCVConstruct(OBJ *a,OBJ *aa)//a-mainunit,b-constructedunit
{
    a->constrobj = aa;
    aa->constrobj = a;
    a->prop |= VARMOVETHROWUNITS;
    a->timerepair = 0;
    ChangeTypeOfProp(a,PROPDECONSTRUCT);
}
//==================================
void applyrescuableunits(void)
{
    int i,haverescued=0;
    OBJ *a,*c;
    OBJstruct *b;
    if (NEEDTOREPAIRREFRESHBIT)
    {
    	for (i=0;i<MaxObjects;i++)
	{
    	    a=objects[i];
    	    b=loadobj(a->SC_Unit);
	    //if rescuable unit/player need to apply to seeit player
	    if (map.pl_iowner[a->playernr]==OWNER_RESCUABLE)
	    {
//		printf("rescuable=%s\n",getOBJname(a->SC_Unit));
	    	c = SearchOBJforOBJ(a,MODERESCUABLE);
		if (c)
		{
		    MakeMindControl(a,NUMBGAMER,a->color);
		    SetBlinkOBJ(a);
		    if (++haverescued>2)
			break;
		}
	    }
	}
	if (haverescued)
	{
//	    CheckProtossBuildsForPower(&map);
	    play_race_rescue(gameconf.pl_race[NUMBGAMER],4,0);
	}
    }
}
//==================================
void SetInvisibleUnit(struct OBJ *a)
{
    a->mainimage->invisiblecolors = 255;
    SetMageAtr(&a->atrobj,ATRINVISIBLE,ATRMAGEINFINITE);
    SetCloakedFlag(a,1);
}
//==================================
void setpropertiestounit(struct OBJ *a,int special_props,int state_flags)
{
    int mask,mode;
    OVERLAY_IMG *tempimg;
    OBJstruct *b=loadobj(a->SC_Unit);
    if (state_flags&UNITONMAP_STATEFLAGS_CLOAK)
	if (special_props&UNITONMAP_STATEFLAGS_CLOAK)
	{
	    //check if cloak is evolved
	    mask=0;
	    if (IsHeroUnit(a->SC_Unit))
		mask=1;
	    else
	    {
		if (a->SC_Unit == SC_GHOSTOBJ)
		    mode = MODEPERSONNELCLOAK;
		else
		    if (a->SC_Unit == SC_WRAITHOBJ)
			mode = MODECLOAKFIELD;
		    else
		    {
			SetInvisibleUnit(a);
			mode = MODENON;
		    }
		if (mode != MODENON)
		    moveobj(a,NULL,mode,0,0,NOSHOWERROR|ATACKMOVEBIT,0);
	    }
	}
    if (state_flags&UNITONMAP_STATEFLAGS_BURROW)
	if (special_props&UNITONMAP_STATEFLAGS_BURROW)
	{
	    //check if burrow is evolved
	    mask=0;
	    if (IsHeroUnit(a->SC_Unit))
		mask=1;
	    else
		if (a->SC_Unit==SC_LURKEROBJ || a->SC_Unit==SC_VULTUREMINEOBJ)
		    mask=1;
	        else
		    mask = GetTechTree(&map,a->playernr,TECHDATA_DAT_BURROWINGTECH);
	    if (mask && ChangeTypeOfProp(a,b,PROPNORMAL2)!=-1)
	    {
		SetBurrowFlag(a,1);
		SetMageAtr(&a->atrobj,ATRINVISIBLE,WRAITHEPODARBITEROM);
        	a->mainimage->invisiblecolors = MAXINVISIBLECOLOR;
        	SetOBJIScriptNr(a,ISCRIPTNR_SPECIALSTATE2,ISCRIPTNR_EXECUTE);	//burrowing state
    	    }
	}
    if (state_flags&UNITONMAP_STATEFLAGS_HALLUCINATION)
	if (special_props&UNITONMAP_STATEFLAGS_HALLUCINATION)
	{
	    SetHallucinationOBJ(a);
	}
    if (state_flags&UNITONMAP_STATEFLAGS_BUILDINTRANSIT)
	if (special_props&UNITONMAP_STATEFLAGS_BUILDINTRANSIT)
	{
	    SIGOrder_LiftOff(a);
	    a->mainimage->childlists->EnumListInit();
	    while(tempimg = (OVERLAY_IMG *) a->mainimage->childlists->GetNextListElem(NULL))
	    {
	        if (tempimg->flags & SC_IMAGE_FLAG_IMGUNDER)	//if this is shadow image
	        {
		    tempimg->ydelta = AIRUNITS_START_YPOS;
		    break;
		}
	    }
    	    SetOBJIScriptNr(a,ISCRIPTNR_SPECIALSTATE1,ISCRIPTNR_EXECUTE);	//air state
	}
    if (state_flags&UNITONMAP_STATEFLAGS_INVINCIBLE)
	if (special_props&UNITONMAP_STATEFLAGS_INVINCIBLE)
	{
	    SetInvincibleOBJ(a,1);
	}
}
//==================================
struct OBJ *createunitwithproperties(int xpos,int ypos,int unit_id,int playernr,
				int special_prop,int special_prop2,int state_flags,
				int hp,int sp,int ep,int resnr,int unitsinhanger)
{
    OBJ *a;
    int hp_perc,sp_perc,ep_perc;
    if (special_prop2 & UNITONMAP_HPISVALID)
        hp_perc = hp;
    else
	hp_perc = 100;
    if (special_prop2 & UNITONMAP_SPISVALID)
	sp_perc = sp;
    else
	sp_perc = 100;
    if (special_prop2 & UNITONMAP_EPISVALID)
	ep_perc = ep;
    else
	ep_perc = 100;
    if (resnr)	//geyser or mineralfield
	ep_perc = resnr << 8;
    a = createobjmanwithlife(xpos,ypos,unit_id,playernr,sp_perc,hp_perc,ep_perc);
    setpropertiestounit(a,special_prop,state_flags);
    return(a);
}
//==================================
int SearchObjInArea(struct OBJ *a,struct XY *area)
{
    if (GetOBJx(a) < area->x1 || GetOBJx(a) > area->x2 || GetOBJy(a) < area->y1 || GetOBJy(a) > area->y2 )
	return(0);
    return(1);
}
//==================================
void MakeCoordsOfRes(struct OBJ *a,struct OBJstruct *b,int *resposx,int *resposy)
{
/*    int storonasv;
    signed char xoffs,yoffs;
    signed char *adrxyoffs;

    if (a->mirror[0])
	storonasv = (b->maxsides-1)*2 - GetRotatePos(a);
    else
        storonasv = GetRotatePos(a);
    adrxyoffs = GetLoXY(
	    		b->allmove[TYPEMODEGATHERMINERALS][1].loinf.lo,
			storonasv+b->allmove[TYPEMODEGATHERMINERALS][1].loinf.fromloline,
			b->allmove[TYPEMODEGATHERMINERALS][1].loinf.fromlooffset);
    if (adrxyoffs)
    {
	if (a->mirror[0])
    	    xoffs = -adrxyoffs[0];
    	else
    	    xoffs = adrxyoffs[0];
        yoffs = adrxyoffs[1];
    }	
    else
    {
        xoffs = 0;
        yoffs = 0;  
    }
    *resposx=a->sourcex+xoffs;
    *resposy=a->sourcey+yoffs;
*/
}
//=================================
#define MAXWAIT_SEARCHATACK_TICK	(MAXGAMECYCLESPERSECOND*1)
//=================================
void SetAtackTick(OBJ *a)
{
    static unsigned char atacktick = 0;
    a->searchforatack_tick = atacktick++;
    if ( atacktick >= MAXWAIT_SEARCHATACK_TICK )
	atacktick = 0;
}
//=================================
#define UNITATACKFUNCTYPE_EVERYUNITS		0
#define UNITATACKFUNCTYPE_DOODADS		1
#define UNITATACKFUNCTYPE_BUNKERS		2
#define UNITATACKFUNCTYPE_REAVERS		3
#define UNITATACKFUNCTYPE_CARRIERS		4
#define UNITATACKFUNCTYPE_VULTUREMINES		5
#define UNITATACKFUNCTYPE_UNITWITHSUBUNIT	6
//=================================
int GetOBJAtackWithoutWeapons(int SC_Unit)
{
    switch(SC_Unit)
    {
	case SC_BUNKEROBJ:
	    return(UNITATACKFUNCTYPE_BUNKERS);
	case SC_REAVEROBJ:
	case SC_HERO_WARBRINGEROBJ:
	    return(UNITATACKFUNCTYPE_REAVERS);
	case SC_CARRIEROBJ:
	case SC_HERO_GANTRITHOROBJ:
	    return(UNITATACKFUNCTYPE_CARRIERS);
	case SC_VULTUREMINEOBJ:
	    return(UNITATACKFUNCTYPE_VULTUREMINES);
//	case SC_GOLIATHTURRETOBJ:
//	    return(UNITATACKFUNCTYPE_UNITWITHSUBUNIT);
	default:
	    if (IsDoodadState(SC_Unit)&&!IsInvincibleUnit(SC_Unit))
	    {
		//doodad traps
		return(UNITATACKFUNCTYPE_DOODADS);
	    }
	    return(UNITATACKFUNCTYPE_EVERYUNITS);
    }
}
//=================================
struct OBJ* EveryUnitAtackFunc(OBJ *a,OBJstruct *b,unsigned char weaponmask,unsigned char groundweapon,unsigned char airweapon)
{
    OBJ *a2;
    if (weaponmask)
    {
        a2 = FindObjForAtack(a,b,weaponmask,groundweapon,airweapon,NULL);
	unitprepareforatack(a,a2);
    }
    return(a2);
}
//=================================
struct OBJ* BunkerAtackFunc(OBJ *aa,OBJstruct *bb,unsigned char wm,unsigned char gw,unsigned char aw)
{
	unsigned char weaponmask,groundweapon,airweapon,SC_Unit;
	OBJ *a,*a2;
	OBJstruct *b;
	if (aa->loaded)
	{
	    for (int i=0;i<aa->loaded->nrofloadedunits;i++)
	    {
		a = aa->loaded->loadedunits[i];
    		if (!a)
    		    continue;
    		SC_Unit = a->SC_Unit;
		b = loadobj(SC_Unit);
		weaponmask=0;
		groundweapon = alldattbl.units_dat->GroundWeapon[SC_Unit];
		if (groundweapon<MAX_WEAPONS_ELEM)
		    weaponmask |= 1;
		airweapon = alldattbl.units_dat->AirWeapon[SC_Unit];
		if (airweapon<MAX_WEAPONS_ELEM)
		    weaponmask |= 2;
		if (weaponmask)
		{
		    a2 = FindObjForAtack(a,b,weaponmask,groundweapon,airweapon,NULL);
		    unitprepareforatack(a,a2);
		}
	    }
	}
	return(NULL);
}
//=================================
int CheckMineSpecialUnit(int SC_Unit)
{
    return(IsBuild(SC_Unit));
}
//=================================
struct OBJ* VultureMineUnitAtackFunc(OBJ *a,OBJstruct *b,unsigned char weaponmask,unsigned char groundweapon,unsigned char airweapon)
{
    OBJ *a2;
    if (weaponmask)
    {
        a2 = FindObjForAtack(a,b,weaponmask,groundweapon,airweapon,&CheckMineSpecialUnit);
	unitprepareforatack(a,a2);
    }
    return(a2);
}
//=================================
struct OBJ* DoodadTrapAtackFunc(OBJ *a,OBJstruct *b,unsigned char weaponmask,unsigned char groundweapon,unsigned char airweapon)
{
    OBJ *a2;
    if ((a->prop & VARAUTOMATICDOODAD) || GetDoodadState(a) == DOODAD_TOP_STATE)
	a2 = FindObjForAtack(a,b,weaponmask,groundweapon,airweapon,NULL);
    else
	a2 = NULL;
    trapprepareforatack(a,a2);
    return(a2);
}
//=================================
struct OBJ* UnitWithSubUnitAtackFunc(OBJ *a,OBJstruct *b,unsigned char weaponmask,unsigned char groundweapon,unsigned char airweapon)
{
    OBJ *a2;
    if (weaponmask)
    {
        a2 = FindObjForAtack(a,b,weaponmask,groundweapon,airweapon,NULL);
	unitprepareforatack(a,a2);
    }
    return(a2);
}
//=================================
struct OBJ* (*Atack_IDFunc[7])(OBJ *a,OBJstruct *b,unsigned char weaponmask,unsigned char groundweapon,unsigned char airweapon)=
			{
			    &EveryUnitAtackFunc,
			    &DoodadTrapAtackFunc,
			    &BunkerAtackFunc,
			    &EveryUnitAtackFunc,
			    &EveryUnitAtackFunc,
			    &VultureMineUnitAtackFunc,
			    &UnitWithSubUnitAtackFunc
			};
//=================================
struct OBJ* OneUnitSearchGoal(OBJ *a,OBJstruct *b,int ignoremodes)
{
    unsigned char weaponmask,groundweapon,airweapon,SC_Unit,Subunit1,unitatack_id;
    //neutral units cannot automatic atack
    if (IsAtackerActiveUnit(a)&& map.pl_owner[a->playernr] == OWNER_HUMAN || map.pl_owner[a->playernr] == OWNER_COMPUTER)
    {
	if (ignoremodes || a->modemove == MODESTOP || a->modemove == MODEHOLDPOS || a->modemove == MODEPATROL || (a->prop & VARMOVEINATACKMODE)||
	    (IsDoodadState(a->SC_Unit)&&!IsInvincibleUnit(a->SC_Unit)&&(a->modemove==MODEDOODADGOTOP || a->modemove==MODEDOODADGOBOTTOM)))
	    {
		//zero if usual unit,     bunker,reaver,carrie have a special unitatack_id
		SC_Unit = a->SC_Unit;
		unitatack_id = GetOBJAtackWithoutWeapons(SC_Unit);
		switch(unitatack_id)
		{
		    case UNITATACKFUNCTYPE_UNITWITHSUBUNIT:	//unitwithsubunits
			SC_Unit = alldattbl.units_dat->Subunit1[SC_Unit];
		    case UNITATACKFUNCTYPE_EVERYUNITS:		//all units that not have a scpecial functions
		    case UNITATACKFUNCTYPE_DOODADS:		//doodadunits
		    case UNITATACKFUNCTYPE_VULTUREMINES:	//vulturemine
			weaponmask=0;
			groundweapon = alldattbl.units_dat->GroundWeapon[SC_Unit];
			if (groundweapon<MAX_WEAPONS_ELEM)
			    weaponmask |= 1;
			airweapon = alldattbl.units_dat->AirWeapon[SC_Unit];
			if (airweapon<MAX_WEAPONS_ELEM)
			    weaponmask |= 2;
			break;
		    case UNITATACKFUNCTYPE_BUNKERS://bunker
		    case UNITATACKFUNCTYPE_REAVERS://reaver
		    case UNITATACKFUNCTYPE_CARRIERS://carrier
			break;
		}
		return ((*Atack_IDFunc[unitatack_id])(a,b,weaponmask,groundweapon,airweapon));
	    }
    }
    return(NULL);
}
//=================================
void SearchForAtacks(void)
{
    int i;
    OBJ *a;
    OBJstruct *b;
    for (i=0;i<MaxObjects;i++)
    {
	a = objects[i];
        b = loadobj(a->SC_Unit);
	if (!a->finalOBJ)
	    if (a->searchforatack_tick--==0)
	    {
		a->searchforatack_tick = MAXWAIT_SEARCHATACK_TICK;
		OneUnitSearchGoal(a,b,0);
	    } 
    }
}
//=================================
int UnitIgnoreInvisibles(int SC_Unit)
{
    if (SC_Unit == SC_VULTUREMINEOBJ)
	return(1);
    else
	return(0);
}
//=================================
OBJ *FindObjForAtack(OBJ *a,OBJstruct *b,
		     unsigned char weaponmask,
		     unsigned char groundweapon,
		     unsigned char airweapon,
		     int (*checkspecialfunc)(int))
{
    int deltaz,i,j,minrange,maxrange,finddistance,mindeltaz[3],weapon_id,wmask;
    OBJ *a2,*findobj[3];
    findobj[NOATACKER]=NULL;			//no atacker
    mindeltaz[NOATACKER] = 256*256*256;

    findobj[GROUNDATACKER]=NULL;		//groundatacker
    mindeltaz[GROUNDATACKER] = 256*256*256;

    findobj[AIRATACKER]=NULL;			//airatacker
    mindeltaz[AIRATACKER] = 256*256*256;
    for (i=0;i<MaxObjects;i++)
    {
	a2 = objects[i];
	if (checkspecialfunc)
	    if (checkspecialfunc(a2->SC_Unit))
		continue;
	if (UnitIgnoreInvisibles(a->SC_Unit) || !GetUnitProp(a2,a->playernr,VARNOTDETECT))
	{
	    //we see this object
	    if (player_aliance(a->playernr,a2->playernr)==ENEMYOBJ && GetUnitProp(a2,a->playernr,VARSEE))
	    {
		weapon_id = MAX_WEAPONS_ELEM;
		if ( (weaponmask & 1) && !IsOnSkyOBJ(a2) )
		    weapon_id = groundweapon;
		if ( (weaponmask & 2) && IsOnSkyOBJ(a2) )
		    weapon_id = airweapon;
		if (weapon_id < MAX_WEAPONS_ELEM)
		{
		    if (IsActiveUnitForAtack(a2))
		    {
			//this is enemy, need to check detect vision radius
			minrange = 0;
		        maxrange = GetTargetAcquisitionRange(a->SC_Unit);
			if (!maxrange)
			{
		    	    minrange=alldattbl.weapons_dat->MinimumRange[weapon_id];
			    maxrange = GetRangeWeaponInPixels(a,weapon_id,a->playernr);
			}
			else
			    maxrange*=SIZESPRLANSHX;
			deltaz = GetDistanceBetweenUnits256(a,a2)/256;
			wmask = UnitWeaponMask(a2->SC_Unit);
			for (j=0;wmask;j++,wmask>>=1)
			{
			    if (wmask & 1)
				if (deltaz < mindeltaz[j] && (maxrange>=deltaz) && (minrange<=deltaz))
				{
				    mindeltaz[j] = deltaz;
				    findobj[j] = a2;
				}
			}
		    }//if unit can atack
		}//if have weapon
	    }//if player aliance
	}//if not detect
    }
    if (IsGroundUnit(a->SC_Unit))
    {
	//atack ground unit if have
	if (findobj[GROUNDATACKER])		//if have ground atacker, atack him
	    return(findobj[GROUNDATACKER]);
	if (findobj[AIRATACKER])	//if have air atacker, atack him
	    return(findobj[AIRATACKER]);
    }
    else
    {
	//atack air unit if have
	if (findobj[AIRATACKER])	//if have air atacker, atack him
	    return(findobj[AIRATACKER]);
	if (findobj[GROUNDATACKER])		//if have ground atacker, atack him
	    return(findobj[GROUNDATACKER]);
    }
    return(findobj[NOATACKER]);		//return noatacker unit, if have
}
//=================================
void unitprepareforatack(OBJ *a,OBJ *a2)
{
    int aiaction=0,doatack=0,status;
    unsigned char groundweapon_id,NEWSC_Unit;
    OBJstruct *b;
    if (UnitDoAiAction(a->playernr) || a->SC_Unit == SC_VULTUREMINEOBJ)
	aiaction=1;
    if (a2)//have unit to atack
    {
	switch(a->SC_Unit)
	{
	    case SC_LURKEROBJ:
		if (IsOBJBurrowed(a))
		{
		    doatack=1;
		}
		else
		{
		    if (aiaction)
		    {
			moveobj(a,NULL,MODEBURROW2,0,0,NOSHOWERROR,0);
		    }
		}
		break;
	    case SC_TANKNORMALOBJ:
	    case SC_HERO_EDMUNDDUKETMOBJ:
		if (aiaction)
		{
		    if (MageDepend(a,a->playernr,MODETANKSIEGE))
		    {
			if (accesstomove(a,loadobj(a->SC_Unit),MODETANKSIEGE,a->playernr))//if can go siegemode
			{
			    if (a->SC_Unit == SC_TANKNORMALOBJ)
				NEWSC_Unit = SC_TANKSIEGEOBJ;
			    else
				if (a->SC_Unit == SC_HERO_EDMUNDDUKETMOBJ)
				    NEWSC_Unit = SC_HERO_EDMUNDDUKESMOBJ;
				else
				    break;
			    groundweapon_id = GetNewWeaponType(a,a2,NEWSC_Unit);
			    if (groundweapon_id < MAX_WEAPONS_ELEM)
			    {
				status = CheckWeaponRange(a,a2,groundweapon_id,a->playernr,0);
				if (status==0)	//siege weapon in range, we can go siege
				    moveobj(a,NULL,MODETANKSIEGE,0,0,NOSHOWERROR,0);
				else
				    //to close obj, do normal atack
				    doatack=1;
			    }
			    break;
			}
		    }
		}
		doatack=1;
		break;
	    default:
		if (!IsOBJBurrowed(a))
		{
		    doatack=1;
		}
		else
		{
		    if (aiaction)
		    {
			moveobj(a,NULL,MODEUNBURROW,0,0,NOSHOWERROR,0);
			AddModeMove(a,a2,MODEATACK,0,0,NOSHOWERROR);
		    }	
		}
		break;
	}
	if (doatack)
	{
	    moveobj(a,a2,MODEATACK,0,0,NOSHOWERROR|ATACKMOVEBIT,0);
	}
    }
    else//no body to atack
    {
	if (aiaction)
	{
	    switch(a->SC_Unit)
	    {
		case SC_VULTUREMINEOBJ:		//mine burrow of not have atack goal
		    if (!IsOBJBurrowed(a))
			moveobj(a,NULL,MODEBURROW2,0,0,NOSHOWERROR,0);
		    break;
		default:
/*		    if (!IsOBJBurrowed(a))
		    {
			b=loadobj(a->SC_Unit);
			if (accesstomove(a,b,MODEBURROW,a->playernr))
			{
			    moveobj(a,NULL,MODEBURROW,0,0,NOSHOWERROR,0);
			}
		    }
*/
		    break;
	    }
	}
    }
}
//=================================
void trapprepareforatack(OBJ *a,OBJ *a2)
{
    if (a2)
    {
	if (GetDoodadState(a)==DOODAD_BOTTOM_STATE)
	{
	    //need show trap & atack
	    DoodadChangeState(a,DOODAD_TOP_STATE);
	}
	else
	{
	    if (a->movelist)
	        a->movelist->EmptyElemFifo();
	    moveobj(a,a2,MODEATACK,0,0,NOSHOWERROR,0);
	}
    }
    else
    {
	a->finalOBJ = NULL;
	if (a->prop & VARAUTOMATICDOODAD)
	{
	    if (GetDoodadState(a)==DOODAD_TOP_STATE)
	    {
		//need hide trap
		DoodadChangeState(a,DOODAD_BOTTOM_STATE);
	    }
	}
    }
}
//#define DONOTATACKBACKTEST
//=================================
void atackback(OBJ *firstatacker,OBJ *destobj,int directiondamage)
{
#ifdef DONOTATACKBACKTEST
    return;
#endif
    int err,aiaction=0,notdetect,aiactionfailed=1,actionfailed=1;
    if (!firstatacker || !destobj)
	return;
    if (player_aliance(destobj->playernr,firstatacker->playernr) != ENEMYOBJ)
	return;
    if (IsSubUnit(firstatacker->SC_Unit))
	firstatacker = firstatacker->subunit;	//take base as atacker
    if (UnitDoAiAction(destobj->playernr))
	aiaction = 1;
    if (aiaction)
    {
	aiactionfailed = tryaiaction(destobj,firstatacker,directiondamage);
    }
    if (aiactionfailed && (destobj->modemove==MODESTOP || destobj->modemove==MODEHOLDPOS ))
    {
	notdetect = GetUnitProp(firstatacker,destobj->playernr,VARNOTDETECT);
	if (!aiaction)
	{
	    actionfailed = tryunitaction(destobj,firstatacker);
	}
	if (actionfailed)
	{
	    if (aiaction && notdetect)
	    {
		TryToScanArea(destobj,firstatacker);
	    }
	    if (!notdetect)
		err = IfCanCreateWeapon(destobj,firstatacker,NULL,NULL,CREATEWEAPON_IGNOREVISION);
	    if (notdetect || err == CREATEDWEAPONSTATUS_CANTATACKTHISUNIT)
	    {
		//cannot atackback
		if (accesstomove(destobj,loadobj(destobj->SC_Unit),MODEMOVE,destobj->playernr))
		    if (destobj->modemove!=MODEHOLDPOS)
			moveaway(destobj,directiondamage,MODEATACK,0);
	    }
	    else
	    {
		//try to atackback
		if (firstatacker->prop & VARINTRANSPORT)
		{
		    if (destobj->SC_Unit == SC_VULTUREMINEOBJ && 
			firstatacker->in_transport && 
			IsBuild(firstatacker->in_transport->SC_Unit))
		    {
		    }
		    else
			moveobj(destobj,firstatacker->in_transport,MODEATACK,0,0,NOSHOWERROR|ATACKMOVEBIT,0);	//atack bunker back
		}
		else
		{
		    if (IsOBJBurrowed(destobj))
		    {
			moveobj(destobj,firstatacker,MODEUNBURROW,0,0,NOSHOWERROR|ATACKMOVEBIT,0);
			AddModeMove(destobj,firstatacker,MODEATACK,0,0,NOSHOWERROR);
		    }
		    else
		    {
			moveobj(destobj,firstatacker,MODEATACK,0,0,NOSHOWERROR|ATACKMOVEBIT,0);
		    }
		}
	    }
	}
    }
    TellOtherUnitsAboutAtacking(destobj,firstatacker);
}
//=================================
void moveaway(OBJ *a,int directiondamage,int modeofmove,int addtoqueue)
{
/*    int dx,dy;
    int side = directiondamage + MAXFACEDIRECTIONS/2;
    if (side>=MAXFACEDIRECTIONS)
	side-=MAXFACEDIRECTIONS;
    dx = inertion[side][0]*SIZESPRLANSHX*5;
    dy = inertion[side][1]*SIZESPRLANSHY*5;
    if (addtoqueue)
	;//???? AddNextModeMove(a,NULL,modeofmove,a->sourcex+dx,a->sourcey+dy);
    else
	moveobj(a,NULL,modeofmove,GetOBJx(a)+dx,GetOBJy(a)+dy,NOSHOWERROR,0);
*/
}
//=================================
int tryaiaction(OBJ *a,OBJ *atacker,int directiondamage)
{
    int err;
    switch(a->SC_Unit)
    {
	case SC_TANKSIEGEOBJ:
	case SC_HERO_EDMUNDDUKESMOBJ:
	    //check for distance to unit to check if we need to go normal tank mode
	    err = IfCanCreateWeapon(a,atacker,NULL,NULL,0);
	    if ( err == CREATEDWEAPONSTATUS_DESTTOCLOSE)
		moveobj(a,NULL,MODETANKNORMAL,0,0,NOSHOWERROR,0);
	    return(0);
	case SC_LURKEROBJ:
	    err = IfCanCreateWeapon(a,atacker,NULL,NULL,0);
	    switch(err)
	    {
		//cannot atack ignore this atacker
		case CREATEDWEAPONSTATUS_CANTATACKTHISUNIT:
		case CREATEDWEAPONSTATUS_UNDERDISRUPTION:
		    if (IsOBJBurrowed(a))
		    {
			//if cannot atack -> unburrow & get out of here
			moveobj(a,NULL,MODEUNBURROW,0,0,NOSHOWERROR,0);
			moveaway(a,directiondamage,MODEMOVE,1);//add this command to queue
		    }
		    return(0);
		case CREATEDWEAPONSTATUS_ATACKSUCCESS:
		case CREATEDWEAPONSTATUS_ATACKSUCCESSWITHROTATION:
		//we in position, do not unburrow
		    if (!IsOBJBurrowed(a))
			moveobj(a,NULL,MODEBURROW2,0,0,NOSHOWERROR,0);
		    return(1);
		case CREATEDWEAPONSTATUS_DESTOUTOFRANGE:
		case CREATEDWEAPONSTATUS_DESTTOCLOSE://never
		    if (IsOBJBurrowed(a))
		    {
			moveobj(a,NULL,MODEUNBURROW,0,0,NOSHOWERROR,0);
		    }
		    else
		    {
			moveobj(a,atacker,MODEATACK,0,0,NOSHOWERROR,0);
		    }
		    return(1);
	    }
	    return(0);//never
    }
    if (IsCloakable(a->SC_Unit))
    {
	if (accesstomove(a,loadobj(a->SC_Unit),MODECLOAKFIELD,a->playernr))
	{
	    //cloakunit
	    if (!GetMageAtr(&a->atrobj,ATRINVISIBLE))
	    {
		moveobj(a,NULL,MODECLOAKFIELD,0,0,NOSHOWERROR,0);
    	    }
	    return(0);
	}
	else
	    if (accesstomove(a,loadobj(a->SC_Unit),MODEPERSONNELCLOAK,a->playernr))
	    {
		//cloakunit
		if (!GetMageAtr(&a->atrobj,ATRINVISIBLE))
		{
		    moveobj(a,NULL,MODEPERSONNELCLOAK,0,0,NOSHOWERROR,0);
		}
		return(0);
	    }
    }
    return(1);//failed ai
}
//=================================
int tryunitaction(OBJ *a,OBJ *atacker)
{
    int err;
    switch(a->SC_Unit)
    {
	case SC_VULTUREMINEOBJ:
	    return(1);
	case SC_LURKEROBJ:
	    if (IsOBJBurrowed(a))
	    {
//		moveobj(a,NULL,MODEUNBURROW,0,0,NOSHOWERROR,0);
	    }
	    else
	    {
		moveobj(a,NULL,MODEBURROW2,0,0,NOSHOWERROR,0);
	    }
	    return(0);
    }
    if (IsOBJBurrowed(a))
    {
	moveobj(a,NULL,MODEUNBURROW,0,0,NOSHOWERROR,0);
	return(0);
    }
    return(1);
}
//=================================
void TryToScanArea(OBJ *a,OBJ *scanobjarea)
{
    int needmana;
    OBJ *a2;
    if (!a || !scanobjarea)
	return;
    //if not build we will not scan the region
    if (!IsBuild(a->SC_Unit))
    {
	needmana = GetTechEnergyCost(mageprop[MODESCANREGION].depend.obj_id[0]);
	a2 = SearchUnit(a->playernr,SC_COMSATSTATIONOBJ,-1,-1,needmana << 8);
	if (a2)
	{
	    //scan the region
	    moveobj(a2,NULL,MODESCANREGION,GetOBJx(scanobjarea),GetOBJy(scanobjarea),NOSHOWERROR,0);
	    return;
	}
    }
    a2 = SearchUnit(a->playernr,SC_OVERLORDOBJ,-1,-1,-1);
    if (!a2)
	a2 = SearchUnit(a->playernr,SC_SCIENCEVESSELOBJ,-1,-1,-1);
	if (!a2)
	    a2 = SearchUnit(a->playernr,SC_OBSERVEROBJ,-1,-1,-1);
    if (a2)
	//send observer obj to the area
	moveobj(a2,NULL,MODEMOVE,GetOBJx(scanobjarea),GetOBJy(scanobjarea),NOSHOWERROR,0);
}
//=================================
void TellOtherUnitsAboutAtacking(OBJ *a,OBJ *atacker)
{
    int i,allstat;
    OBJ *a2;
    for (i=0;i<MaxObjects;i++)
    {
	a2 = objects[i];
	allstat = player_aliance(a->playernr,a2->playernr);
	if (allstat == MYOBJ || allstat == ALLIANCEOBJ )
	if (a2->modemove == MODESTOP && a2 != a)
	{
	    if (IsAtackerActiveUnit(a2))
	    {
		if (controldistanceunit(GetOBJx(a),GetOBJy(a),GetOBJx(a2),GetOBJy(a2),MINREACTIONDISTANCEFORATACKING))
		{
		    //tell other unit to atack my atacker
		    if (IsOBJBurrowed(a2) && a2->SC_Unit != SC_LURKEROBJ)
		    {
			moveobj(a2,NULL,MODEUNBURROW,0,0,NOSHOWERROR,0);
		    }
		    else
		    {
			moveobj(a2,atacker,MODEATACK,0,0,NOSHOWERROR,0);
		    }
		}
	    }
	}
    }
}
//=================================
void ShowCircleAndBar(OBJ *a)
{
    OBJstruct *b;
    int showcircle=0,fromy=0,maxshield;
    int manacnt,timeleft;
    int x,y;
    unsigned short flingy_id,sprites_id,sizexhealthbar;
//    if (a->modemove == MODEDIE)
//	return;
    if (a->blinkvalue)
    {
	if (a->blinkvalue & BLINKTICKS)
	    showcircle=1;
    }
//    else
    {
	if (ifselectedOBJbit(a,NUMBGAMER))
	    showcircle=1;
    }
    if (showcircle)
    {
	b = loadobj(a->SC_Unit);
        x = GetOBJx(a);
        y = GetOBJy(a);
        fromy=desencircle(x-map.MAPXGLOBAL,
                          y-map.MAPYGLOBAL,
                          player_aliance(NUMBGAMER,a->playernr),a,b);

	if (!IsInvincibleOBJ(a))
	{
	    flingy_id = alldattbl.units_dat->flingy_id[a->SC_Unit];
	    sprites_id = alldattbl.flingy_dat->sprites_id[flingy_id];
	    sizexhealthbar=alldattbl.sprites_dat->health_bar[sprites_id-130];
	    if (IsSpellCaster(a->SC_Unit))
		manacnt=GetUnitMaxMana(a->SC_Unit,a->playernr);
	    else
		manacnt=0;
	    if (sizexhealthbar)
	    {
    		if (IsShieldEnable(a->SC_Unit))
    	    	    maxshield = GetUnitMaxShield(a->SC_Unit);
    	    	else
    	    	    maxshield = 0;
		if (a->playernr==NUMBGAMER)
		{
		    if (GetMageAtr(&a->atrobj,ATRHALLUCINATION))
        		timeleft = mageprop[MODEHALLUCINATION].timemageactive;
    		    else
        		timeleft = b->maxtimeleft;
    	        }
    	        else
    	    	timeleft=0;
    	        desenlife(x-map.MAPXGLOBAL-sizexhealthbar/2,fromy,sizexhealthbar,
            		    player_aliance(NUMBGAMER,a->playernr),a->life,a->shield,a->mana,
		    	    a->timeleft,GetUnitMaxLife(a->SC_Unit),maxshield,manacnt,timeleft);
	    }
	}
    }
}
//=================================
int ifselectedOBJbit(OBJ *a,int playernr)
{
    return(a->selectedbits & (1 << playernr));
}
//=================================
void doselectedOBJbit(OBJ *a,int playernr,int flag)
{
    if (flag)
	a->selectedbits |= (1 << playernr);
    else
	a->selectedbits &= ~(1 << playernr);
}
//=================================
void SetHallucinationOBJ(OBJ *a)
{
    MAIN_IMG *img,*tempimg;
    int i;
    a->mana=0;
    a->timeleft = mageprop[MODEHALLUCINATION].timemageactive;
    for (int i=ATRPARASITEFROM;i<MAXMAGEATR;i++)
        SetMageAtr(&a->atrobj,i,0);
    SetMageAtr(&a->atrobj,ATRHALLUCINATION,ATRMAGEINFINITE);
    if (a->playernr == NUMBGAMER)
    {
	a->mainimage->imageusercolor = TOBLUE;
    }
    //hallucination dont need food, so decrease supply
    ChangeSupply(a->playernr,a->SC_Unit,MINUSFACTOR);
    ChangeMaxSupply(a->playernr,a->SC_Unit,MINUSFACTOR);
    
}
//=================================
void initmoveaction(OBJ *a,OBJ *destobj,int mode,int startx,int starty,int x,int y)
{
    int deltax,deltay,deltaz;
    unsigned char flingy_id;
    flingy_id = alldattbl.units_dat->flingy_id[a->SC_Unit];
    if (mageprop[mode].createweapon != 255)
    {
	a->modemoveminusdistance = GetRangeWeaponInPixels(a,mageprop[mode].createweapon,a->playernr)<<8;
    }
    else
    {
	switch(mode)
	{
	    case MODEATACK:
		if (destobj)
		    a->modemoveminusdistance = GetRangeWeaponInPixels(a,startx,a->playernr)<<8;	//move minus range of weapos
		else
		    a->modemoveminusdistance = 0;						//move in atackmode to dest on ground
		break;
	    case MODENUKELAUNCH:
		a->modemoveminusdistance = GetRangeWeaponInPixels(a,alldattbl.units_dat->GroundWeapon[a->SC_Unit],a->playernr)<<8;
		break;
	    case MODEPATROL:
		if (destobj)
		{
		    //do only patrol to terain not unit
		    x = GetOBJx(destobj);
		    y = GetOBJy(destobj);
		    destobj = NULL;
		}
		if (startx==0)
		{
		    a->startx = GetOBJx(a);
		    a->starty = GetOBJy(a);
		}
		else
		{
		    a->startx = startx;		//from patrol command
		    a->starty = starty;
		}
		a->prop |= VARPATROLFLAG;
		break;
	    default:
		a->modemoveminusdistance = mageprop[mode].mindistance;
		break;
	    
	}
    }
    switch(alldattbl.flingy_dat->MoveControl[flingy_id])
    {
	case FLINGYMOVECONTROL_ISCRIPT:
	    a->finalx   = x<<8;
	    a->finaly   = y<<8;
	    a->prop |= VARMOVEACT;
	    if (destobj)
	    {
		a->finalOBJ = destobj;
		a->prop |= VARMOVEOBJACT;
	    }
	    else
	    {
		a->finalOBJ = NULL;
		a->prop &= ~VARMOVEOBJACT;
	    }	
	    PathFinding_InitType1(a,GetOBJx256(a),GetOBJy256(a),a->finalx,a->finaly);
	    SetOBJIScriptNr(a,ISCRIPTNR_WALKING,ISCRIPTNR_SETONLY);
	    break;
	case FLINGYMOVECONTROL_FLINGY:
	case FLINGYMOVECONTROL_WEAPON:
	    a->prop |= VARACCELERATIONBIT;
	    a->prop |= VARMOVEACT;
	    if (destobj)
	    {
		a->finalOBJ = destobj;
		a->prop |= VARMOVEOBJACT;
		a->finalx   = GetOBJx256(destobj);
		a->finaly   = GetOBJy256(destobj);
	    }
	    else
	    {
		a->finalOBJ = NULL;
		a->prop &= ~VARMOVEOBJACT;
		a->finalx   = x<<8;
		a->finaly   = y<<8;
	    }	
	    PathFinding_InitType2(a,GetOBJx256(a),GetOBJy256(a),a->finalx,a->finaly,flingy_id);
	    SetOBJIScriptNr(a,ISCRIPTNR_WALKING,ISCRIPTNR_EXECUTE);
	    break;
    }
}
//=================================
void InitStopAfterMove(OBJ *a)
{
    //engines off
    a->prop &= ~(VARACCELERATIONBIT | VARMOVEACT);
    SetOBJIScriptNr(a,ISCRIPTNR_WALKINGTOIDLE,ISCRIPTNR_EXECUTE);
}
//=======================================
//for iscriptbin version
void PathFinding_InitType1(OBJ *a,int initx,int inity,int destx,int desty)
{
    a->mainimage->UnitNeededDirection256(CalcDirection1(initx,inity,destx,desty));
}
//=================================
int PathFinding_MovePortionType1(OBJ *a,MAIN_IMG *img,int deltamove)
{
    int deltaz,deltax,deltay,destx,desty,widthsumm;
    unsigned char direction;
    if (a->prop & VARMOVEOBJACT)
    {
	// if move to a object
	if (!a->finalOBJ)	//destobj did not exist(died?)
	{
	    InitStopAfterMove(a);
	    a->prop &= ~VARMOVEOBJACT;
	    //apply next mode from list
	    if (!(a->prop & VARHOLDPOSBIT))
	    {
		if (ApplyNextModeMove(a))
		    return(2);
	    }
	    return(2);		//change iscriptnr
	}
	destx = GetOBJx256(a->finalOBJ);
	desty = GetOBJy256(a->finalOBJ);
	img->UnitNeededDirection256(CalcDirection1(img->xpos,img->ypos,destx,desty));

	deltax = destx - img->xpos;
	deltay = desty - img->ypos;
	deltaz = (int) hypot(deltax,deltay);
	
	widthsumm = GetWidthSummOfUnits(a->SC_Unit,a->finalOBJ->SC_Unit,deltax,deltay);
	if (deltaz - widthsumm - a->modemoveminusdistance <= deltamove)
	{
	    if (ApplyNextModeMove(a))
	        return(2);
//	    return(0);
//here unit stopped and wait for change distance to move again
	    a->prop |= VARWAITDISTANCE;
	    SetOBJIScriptNr(a,ISCRIPTNR_WALKINGTOIDLE,ISCRIPTNR_EXECUTE);
	    return(2);
	}
    }
    else
    {
	//if move to a terrain
	deltax = a->finalx - img->xpos;
	deltay = a->finaly - img->ypos;
	deltaz = (int)hypot(deltax,deltay);

	if (deltaz - a->modemoveminusdistance <= deltamove)
	{
	    //we need to stop action
	    if (a->prop & VARPATROLFLAG)
	    {
		initmoveaction(a,NULL,MODEPATROL,a->finalx>>8,a->finaly>>8,a->startx,a->starty);
		return(0);	//not move but initiate other move
	    }
	    else
	    {
		InitStopAfterMove(a);
		//apply next mode from list
		if (!a->finalOBJ)
		    if (!(a->prop & VARHOLDPOSBIT))
		    {
			if (ApplyNextModeMove(a))
			    return(2);
		    }
		return(2);		//change iscriptnr
	    }
	}
    }
    deltax = (long long)deltamove*deltax/deltaz;
    deltay = (long long)deltamove*deltay/deltaz;
    img->SetDeltaCoords(a,deltax,deltay);
    return(1);//can pass
}
//=================================
//for flingy movecontrol version
void PathFinding_InitType2(OBJ *a,int initx,int inity,int destx,int desty,unsigned char flingy_id)
{
    int deltaz;
    float newhdist;
    PathFinding_InitType1(a,initx,inity,destx,desty);
    if (a->mainimage->side == a->mainimage->neededside)
    {
	//if the same direction, calculate move vars
	CalcDestVars(a,a->finalOBJ,initx,inity,destx,desty,flingy_id);
    }
}
//=================================
int PathFinding_MovePortionType2(OBJ *a,MAIN_IMG *img,unsigned char flingy_id,int deltamove)
{
    int deltaz,deltax,deltay,destx,desty,widthsumm;
    unsigned char direction;
    if (a->prop & VARMOVEOBJACT)
    {
	if (!a->finalOBJ)	//destobj did not exist(died?)
	{
	    InitStopAfterMove(a);
	    a->prop &= ~VARMOVEOBJACT;
	    return(2);		//change iscriptnr
	}
	destx = GetOBJx256(a->finalOBJ);
	desty = GetOBJy256(a->finalOBJ);
	if (a->prop &  VARACCELERATIONBIT)
	{
	    img->UnitNeededDirection256(CalcDirection1(img->xpos,img->ypos,destx,desty));
	    a->finalx = destx;
	    a->finaly = desty;
	}
	else
	{
	    img->MoveInUnitDirection(a,img->side,deltamove);
	    return(1);
	}
	deltax = destx - img->xpos;
	deltay = desty - img->ypos;
	deltaz = (int) hypot(deltax,deltay);
	
	widthsumm = GetWidthSummOfUnits(a->SC_Unit,a->finalOBJ->SC_Unit,deltax,deltay);
	if (deltaz - widthsumm - a->modemoveminusdistance <= a->haltdistance)
	{
	    InitStopAfterMove(a);
	    a->prop |= VARWAITDISTANCE;
	}
    }
    else
    {
	//if move to a terrain
	deltax = a->finalx - img->xpos;
	deltay = a->finaly - img->ypos;
	deltaz = (int)hypot(deltax,deltay);
	if (a->prop & VARACCELERATIONBIT)
	{
	    if (deltaz - a->modemoveminusdistance <= a->haltdistance)
	    {
		InitStopAfterMove(a);
	    }
	}
	else
	{
	    //do inertion move
	    img->MoveInUnitDirection(a,img->side,a->currentspeed);//NOTE: used a->currentspeed (no used 'deltamove' for correct inertion)
	    return(1);
	}
    }
    deltax = (long long)deltamove*deltax/deltaz;
    deltay = (long long)deltamove*deltay/deltaz;
    img->SetDeltaCoords(a,deltax,deltay);
    return(1);//can pass
}
//=================================
int CalcDestVars(OBJ *a,OBJ *destobj,int initx,int inity,int destx,int desty,unsigned char flingy_id)
{
    int haltdistance,topspeed,deltaz;
    topspeed = alldattbl.flingy_dat->TopSpeed[flingy_id];
    a->deacceleration = alldattbl.flingy_dat->Acceleration[flingy_id];
    if (alldattbl.flingy_dat->MoveControl[flingy_id] == FLINGYMOVECONTROL_WEAPON)
    {
        haltdistance = topspeed;
    }
    else
    {
	haltdistance = sqr(topspeed)/(2 * alldattbl.flingy_dat->Acceleration[flingy_id]);
//	haltdistance = CalcTotalDistance(topspeed,0,alldattbl.flingy_dat->Acceleration[flingy_id]);
	if (destobj)
	    deltaz = GetDistanceTo256(destobj,initx,inity)/2;
	else
	    deltaz = hypot(destx-initx,desty-inity)/2;
	if (deltaz < haltdistance)
	{
//	    haltdistance = CalcTotalDistance(topspeed-a->currentspeed,0,alldattbl.flingy_dat->Acceleration[flingy_id]);
//	    if (deltaz >= haltdistance)
//	    {
		haltdistance = deltaz / 2;
		a->deacceleration *= 2;
//	    }
	}
    }
    a->topspeed = topspeed;
    a->haltdistance = haltdistance;
    return (haltdistance<=256);
}
//=================================
int moveaction(MAIN_IMG *img,int deltamove)
{
    int status;
    OBJ *a;
    if (img->whocreate != SC_IMAGE_OBJ_CREATOR)
    {
//	DEBUGMESSCR("something wrong with this img, no OBJ asociated\n");	//goliath turret
	return(0);
    }
    a = img->creator.objcreator.obj;
    return(PathFinding_MovePortionType1(a,img,GetSpeed(a,deltamove)));
}
//=================================
void ForceKartChanges(OBJ *a)
{
    a->xkart = (GetOBJx256(a)>>8)/32;
    a->ykart = (GetOBJy256(a)>>8)/32;
    a->prop |= VARKARTCHANGES;
    AddRefreshedOBJ(a);
}
//=================================
void ForceKartChanges(OBJ *a,int xpos,int ypos)
{
    a->xkart = (xpos>>8) / 32;
    a->ykart = (ypos>>8) / 32;
    a->prop |= VARKARTCHANGES;
    AddRefreshedOBJ(a);
}
//=================================
void CheckForKartChanges(OBJ *a,int xpos,int ypos)
{
    unsigned char newxkart,newykart;
    newxkart = (xpos>>8) / 32;
    newykart = (ypos>>8) / 32;
    if (a->xkart != newxkart || a->ykart != newykart)
    {
	a->xkart = newxkart;
	a->ykart = newykart;
	a->prop |= VARKARTCHANGES;
	AddRefreshedOBJ(a);
//	DEBUGMESSCR("kart changes %dx%d\n",a->xkart,a->ykart);
    }
}
//=================================
//deltax,deltay - delta between movedunit & destination unit
int GetWidthSummOfUnits(unsigned char SC_Unit1,unsigned char SC_Unit2,int deltax,int deltay)
{
    if (abs(deltax) > abs(deltay))
        return ((GetUnitWidthAndHeight(SC_Unit1,UNITDIM_WIDTH)/2 + GetUnitWidthAndHeight(SC_Unit2,UNITDIM_WIDTH)/2)<<8);
    else
	return ((GetUnitWidthAndHeight(SC_Unit1,UNITDIM_HEIGHT)/2 + GetUnitWidthAndHeight(SC_Unit2,UNITDIM_HEIGHT)/2)<<8);
}
//=================================
int GetDestUnitHalfSize(unsigned char SC_Unit,int deltax,int deltay)
{
    if (abs(deltax) > abs(deltay))
        return (GetUnitWidthAndHeight(SC_Unit,UNITDIM_WIDTH)/2);
    else
	return (GetUnitWidthAndHeight(SC_Unit,UNITDIM_HEIGHT)/2);
}
//=================================
void AdditionalUnitsProceed(void)
{
    int i;
    for (i=0;i<MaxObjects;i++)
    {
	if (objects[i]->modemove != MODEDIE)
	    AdditionalUnitProceed(objects[i],objects[i]->mainimage);
    }
}
//=================================
void AllFlingyControlOBJMoving(void)
{
    int i,rot,deltax,deltay,deltaz,widthsumm,curspeed;
    signed char incfactor;
    unsigned char flingy_id,side1,side2;
    MAIN_IMG *img;
    OBJ *a;
    for (i=0;i<MaxObjects;i++)
    {
	a = objects[i];
	img = a->mainimage;
	flingy_id = alldattbl.units_dat->flingy_id[a->SC_Unit];
	if (img->flags & SC_IMAGE_FLAG_NEEDROTATIONTODIRECTION)
	{
	    if (!(img->flags & SC_IMAGE_FLAG_ISCRIPTROTATION))
	    {
		side1 = img->side;
		rot = img->Rotation(alldattbl.flingy_dat->TurnRadius[flingy_id]);
		if (!rot)
		{
	    	    img->MoveInUnitDirection(a,side1,GetSpeed(a,a->currentspeed));
		    img->UnitNeededDirection256(CalcDirection1(GetOBJx256(a),GetOBJy256(a),a->finalx,a->finaly));
		    continue;
		}
		if (a->modemove == MODETURN180)
		{
		    ApplyNextModeMove(a);
		    continue;
		}
		if (CalcDestVars(a,a->finalOBJ,GetOBJx256(a),GetOBJy256(a),a->finalx,a->finaly,flingy_id))
		{
		    //need to stop engines    
		    InitStopAfterMove(a);
		}
	    }
	}
	if (alldattbl.flingy_dat->MoveControl[flingy_id] == FLINGYMOVECONTROL_FLINGY  ||
	    alldattbl.flingy_dat->MoveControl[flingy_id] == FLINGYMOVECONTROL_WEAPON   )
	{
	    if (a->prop &  VARACCELERATIONBIT)
	    {
		//accelerate speed
		a->currentspeed += alldattbl.flingy_dat->Acceleration[flingy_id];
		if (a->currentspeed > a->topspeed)
		{
		    //reach topspeed
		    a->currentspeed = a->topspeed;
		}
	    }
	    else
	    {
		if (!a->currentspeed)
		{
		    if (a->movelist && a->movelist->GetUsedElem())
			ApplyNextModeMove(a);
		    if (a->finalOBJ && IsPickupUnit(a->finalOBJ->SC_Unit) && IsWorkerUnit(a->SC_Unit) &&  !a->carryobj )
		    {
			PickupObj(a,a->finalOBJ);
		    }
		    continue;
		}
		if (alldattbl.flingy_dat->MoveControl[flingy_id] == FLINGYMOVECONTROL_WEAPON)
		{
		    a->currentspeed = a->currentspeed / 2;
		}
		else
		{
		    a->currentspeed -= a->deacceleration;
		}
		if (a->currentspeed <= 0)
		{
			//stop move
			a->currentspeed = 0;
			if (a->prop & VARPATROLFLAG)
			{
			    initmoveaction(a,NULL,MODEPATROL,a->finalx>>8,a->finaly>>8,a->startx,a->starty);
			}
			else
			{
			    //set next move from list
			    if (!a->finalOBJ)
			    {
				if (!(a->prop & VARHOLDPOSBIT))
				{
				    ApplyNextModeMove(a);
				}
			    }
			    else
			    {
				ApplyNextModeMove(a);
			    }
			}
			continue;
		}
	    }
	    curspeed = GetSpeed(a,a->currentspeed);
	    PathFinding_MovePortionType2(a,img,flingy_id,GetSpeed(a,a->currentspeed));
//	    printf("speed=%d ensnaired=%d\n",a->currentspeed,curspeed);
	}
    }
}
//=================================
int IfHaveDistanceForMove(OBJ *a,MAIN_IMG *img,OBJ *destobj,int mindistance)
{
    unsigned char flingy_id;
    int deltax,deltay,deltaz,widthsumm;
    deltax = GetOBJx256(destobj) - img->xpos;
    deltay = GetOBJy256(destobj) - img->ypos;
    deltaz = (int) hypot(deltax,deltay);
    widthsumm = GetWidthSummOfUnits(a->SC_Unit,destobj->SC_Unit,deltax,deltay);
    return ( (deltaz - widthsumm ) > mindistance);
}
//==================================
void SetModeMove(OBJ *a,int mode)
{
    a->modemove = mode;
    if (a->movelist)
	a->movelist->EmptyElemFifo();
}
//==================================
void AddModeMove(OBJ *a,OBJ *destobj,int mode,int x,int y,int showmesflag)
{
    ONEMODEMOVE *onemodemove;
    if (!a->movelist)
    {
	a->movelist = new myfifo(5);
    }
    if (!a->movelist->GetFreeElem())
    {
	DEBUGMESSCR("list of mode moves is full\n");
	return;
    }
    onemodemove = (ONEMODEMOVE *) a->movelist->AddElem(sizeof(ONEMODEMOVE));
    if (!onemodemove)
    {
	DEBUGMESSCR("cannot add to list of mode moves :( \n");
	return;
    }
    onemodemove->destobj = destobj;
    onemodemove->modemove = mode;
    onemodemove->destx = x;
    onemodemove->desty = y;
    onemodemove->showerrflag = NOSHOWERROR;//showmesflag;
}
//==================================
int IfHaveNextModeMove(OBJ *a)
{
    if (!a->movelist)
	return(0);
    return(a->movelist->GetUsedElem());
}
//==================================
int ApplyNextModeMove(OBJ *a)
{
    int dx,dy,modemove,showerrmes;
    OBJ *destobj;
    ONEMODEMOVE *onemodemove;
//    if (a->modemove == MODEDIE)
//	return(0);
    do{
	//check if we have move in lists, if have do that move
	if (!a->movelist || !a->movelist->GetUsedElem())
	{
	    if (!a->finalOBJ )
		return(moveobj(a,NULL,MODESTOP,0,0,NOSHOWERROR,0));
//	    else
//		a->modemove = MODESTOP;
	    return(0);
	}
	onemodemove = (ONEMODEMOVE *) a->movelist->GetCurElem();
	modemove = onemodemove->modemove;
	dx = onemodemove->destx;
	dy = onemodemove->desty;
	destobj = onemodemove->destobj;
	showerrmes = onemodemove->showerrflag;
	a->movelist->DelCurElem();
    }while(modemove == MODENON);
    moveobj(a,destobj,modemove,dx,dy,showerrmes|ATACKMOVEBIT,0);
    return(1);
}
//==================================
void DelOBJFromModeList(struct OBJ *a,struct OBJ *searchobj)
{
    int i;
    ONEMODEMOVE *onemodemove;
    if (a->movelist)
    {
	if (a->movelist->GetUsedElem())
	{
	    for (i=0;i<a->movelist->totalelem;i++)
	    {
		onemodemove = (ONEMODEMOVE *)a->movelist->elements[i];
		if (onemodemove && onemodemove->destobj == searchobj)
		{
		    //emtpy modemove
		    onemodemove->modemove = MODENON;
		    onemodemove->destobj = NULL;
		}
	    }
	}
    }
}
//==============================================
void DelAllModeMoves(struct OBJ *a)
{
    if (a->movelist)
    {
	a->movelist->EmptyElemFifo();
	delete a->movelist;
	a->movelist = NULL;
    }
}
//==============================================


