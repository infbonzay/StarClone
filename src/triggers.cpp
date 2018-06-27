
#include <math.h>

#include "mage.h"
#include "audio.h"
#include "putobj.h"
#include "load.h"
#include "auxil.h"
#include "doodad.h"
#include "player.h"
#include "maps.h"
#include "gener.h"
#include "starmap.h"
#include "messages.h"
#include "lists.h"
#include "vars.h"
#include "objs.h"
#include "portrait.h"
#include "rand.h"
#include "action.h"
#include "gr.h"
#include "objinfo.h"
#include "pylon.h"
#include "debug.h"
#include "market.h"
#include "images.h"
#include "weapons.h"
#include "music.h"
#include "triggers.h"

//#define		TRIG_DEBUG
int		TRIG_pause;
int		TRIG_MusicQuieter;
int		*ALLTRIG_pause;
SCUNIT		TRIG_Portrait;
char		TRIG_MusicVolume;
char 		TRIG_ChangeStat;
char 		TRIG_preserve;
char 		TRIG_leaderboardcomputerplayers;
char		TRIG_active;
char		TRIG_quit;
char		TRIG_snd;
char		TRIG_stopalltriggers;
char		TRIG_inittriggers;
char		*TRIG_commentstr;

//=================================================
void Remove_Triggers(mapinfo *info)
{
    if (info)
    {
	stopallsounds();
    	if (ALLTRIG_pause)
	    wfree(ALLTRIG_pause);
	ALLTRIG_pause=NULL;
	if (info->TRIGS)
	{
	    wfree(info->TRIGS);
	    info->TRIGS=NULL;
	    info->TRIGS_NR=0;
	}
    }
    TRIG_stopalltriggers = 1;
    TRIG_inittriggers = 0;
    TRIG_MusicQuieter = 0;
    TRIG_Portrait = SC_NOUNITNR;
}
//=================================================
int AllGroups_Prepare(mapinfo *info,MAP_TRIGS *temptrg)
{
    int i,j;
    int actiononplayers=0;
    for (i=0;i<TRIG_MAX_GROUPPLAYERS;i++)
    {
	if (temptrg->groupinfo.grouplist[i])
	{
	    if (temptrg->groupinfo.grouplist[i])
    	        actiononplayers|=OneGroup_Prepare(info,i,1<<NUMBGAMER);
	}
    }
    return(actiononplayers);
}
//=================================================
int OneGroup_Prepare(mapinfo *info,int i,int actionfromplayers)
{
    int j,actiononplayers=0;
    switch(i)
    {
		case TRG_PLAYERGROUP_PLAYER0:
		case TRG_PLAYERGROUP_PLAYER1:
		case TRG_PLAYERGROUP_PLAYER2:
		case TRG_PLAYERGROUP_PLAYER3:
		case TRG_PLAYERGROUP_PLAYER4:
		case TRG_PLAYERGROUP_PLAYER5:
		case TRG_PLAYERGROUP_PLAYER6:
		case TRG_PLAYERGROUP_PLAYER7:
		case TRG_PLAYERGROUP_PLAYER8:
		case TRG_PLAYERGROUP_PLAYER9:
		case TRG_PLAYERGROUP_PLAYER10:
		case TRG_PLAYERGROUP_PLAYER11:
		    actiononplayers|=1<<(i-TRG_PLAYERGROUP_PLAYER0);
		    break;
		case TRG_PLAYERGROUP_UNKNOWN1:
		    break;
		case TRG_PLAYERGROUP_CURRENTPLAYER:
		    actiononplayers=actionfromplayers;
		    break;
		case TRG_PLAYERGROUP_FOES:		//enemies
		    for (j=0;j<PLAYEDPLAYERS;j++)
		        if (player_aliance(NUMBGAMER,j)==ENEMYOBJ)
			    actiononplayers|=1<<j;
		    break;
		case TRG_PLAYERGROUP_ALLIES:
		    for (j=0;j<PLAYEDPLAYERS;j++)
		        if (player_aliance(NUMBGAMER,j)==MYOBJ||player_aliance(NUMBGAMER,j)==ALLIANCEOBJ)
			    actiononplayers|=1<<j;
		    break;
		case TRG_PLAYERGROUP_NEUTRALPLAYERS:
		    for (j=0;j<PLAYEDPLAYERS;j++)
		        if (player_aliance(NUMBGAMER,j)==NEUTRALOBJ)
			    actiononplayers|=1<<j;
		    break;
		case TRG_PLAYERGROUP_ALLPLAYERS:
		    actiononplayers=TRIGGER_ALLPLAYERSBITMASK;	//12 players bitmask
		    break;
		case TRG_PLAYERGROUP_FORCE1:	
		case TRG_PLAYERGROUP_FORCE2:
		case TRG_PLAYERGROUP_FORCE3:
		case TRG_PLAYERGROUP_FORCE4:
		    if (force_slots.forces != 1)//apply this if we have at least 2 forces (UMS && forces added in staredit)
		    {
			for (j=0;j<PLAYEDPLAYERS;j++)
			    if (info->pl_force.force_nr[j]==i-TRG_PLAYERGROUP_FORCE1)
			    actiononplayers|=1<<j;
		    }
		    break;
		case TRG_PLAYERGROUP_UNKNOWN2:
		case TRG_PLAYERGROUP_UNKNOWN3:
		case TRG_PLAYERGROUP_UNKNOWN4:
		case TRG_PLAYERGROUP_UNKNOWN5:
		    break;
		case TRG_PLAYERGROUP_NONALLIEDVICTORY:
		    for (j=0;j<PLAYEDPLAYERS;j++)
		        if (player_aliance(NUMBGAMER,j)==ENEMYOBJ||player_aliance(NUMBGAMER,j)==NEUTRALOBJ)
			    actiononplayers|=1<<j;
		    break;
		default:
		    break;
    }
    return(actiononplayers);
}
//=================================================
int Condition_AtLeast(int *var1,int var2)	//kak minimum var2 units
{
    return(*var1>=var2);
}
//=================================================
int Condition_AtMost(int *var1,int var2)		//kak maximum 0 units
{
    return(*var1<=var2);
}
//=================================================
int Condition_Exact(int *var1,int var2)
{
    return(*var1==var2);
}
//=================================================
int Condition_IsSet(int *var1,int var2)
{
    return(*((char *)var1));
}
//=================================================
int Condition_IsCleared(int *var1,int var2)
{
    return( *( (char *)var1 )==0);
}
//=================================================
int Switch_Set(int *var1,int var2)
{
    *((char *)var1)=1;
}
//=================================================
int Switch_Clear(int *var1,int var2)
{
    *((char *)var1)=0;
}
//=================================================
int Switch_Toggle(int *var1,int var2)
{
    *((char *)var1)^=1;
}
//=================================================
int Function_Set(int *var1,int var2)
{
    *var1=var2;
}
//=================================================
int Function_Add(int *var1,int var2)
{
    *var1+=var2;
}
//=================================================
int Function_Sub(int *var1,int var2)
{
    *var1-=var2;
}
//=================================================
int Switch_Randomize(int *var1,int var2)
{
    *((char *)var1)=myrand(2);
}
//=================================================
int (*ConditionFunctions[TRG_MAXTYPEFUNC])(int *var1,int var2)=
		    {
			&Condition_AtLeast,
			&Condition_AtMost,
			&Condition_IsSet,
			&Condition_IsCleared,
			&Switch_Set,
			&Switch_Clear,
			&Switch_Toggle,
			&Function_Set,
			&Function_Add,
			&Function_Sub,
			&Condition_Exact,
			&Switch_Randomize
		    };
//=================================================
int (*DoorConditionFunctions[TRG_MAXTYPEFUNC])(int *var1,int var2)=
		    {
			&Condition_AtLeast,
			&Condition_AtMost,
			&Condition_IsSet,
			&Condition_IsCleared,
			&Switch_Clear,
			&Switch_Set,
			&Switch_Toggle,
			&Function_Set,
			&Function_Add,
			&Function_Sub,
			&Condition_Exact,
			&Switch_Randomize
		    };
//=================================================
int UNITNRFORCHECK;
int CheckForUnit_UnitNr(SCUNIT SC_Unit)
{
    return(UNITNRFORCHECK==SC_Unit);
}
//=================================================
int CheckForUnit_EveryUnit(SCUNIT SC_Unit)
{
    return(1);
}
//=================================================
char readyfunc[5]={1,1,0,0,0};
int (*CheckUnit[5])(SCUNIT)=
		    {
			&CheckForUnit_UnitNr,				//unit  nr			//228
			&CheckForUnit_EveryUnit,			//every unit			//229
			&IsGroupMenFlag,				//check for unit		//230
			&IsGroupBuildFlag,				//check for build		//231
			&IsGroupFactoryFlag,				//check for factory		//232
		    };
//=================================================
int CheckForUnit(int (*ConditionFunction)(int *, int),
		 int actiononplayers,int unitid,int cntunits,OBJ **retlast,struct XY *searcharea)
{
    return(CheckForUnit(ConditionFunction,actiononplayers,unitid,cntunits,retlast,searcharea,0));
}
//=================================================
int CheckForUnit(int (*ConditionFunction)(int *, int),
		 int actiononplayers,int unitid,int cntunits,OBJ **retlast,struct XY *searcharea,int modemove)
{
    int i,nrofunits=0,checkready=0;
    int (*UnitTypeFunc) (SCUNIT);
    struct OBJ *a,*last=NULL;
//    mylistsimple *objlist = new mylistsimple(MaxObjects);
    if (unitid>=MAX_UNITS_ELEM)					
    {								
	UnitTypeFunc=CheckUnit[unitid-MAX_UNITS_ELEM];		//>228 (0xe5-0xe8)
	checkready=readyfunc[unitid-MAX_UNITS_ELEM];
    }								
    else							
    {								
	UnitTypeFunc=CheckUnit[0];
	checkready=readyfunc[0];
	UNITNRFORCHECK=unitid;
    }
    for (i=0;i<MaxObjects;i++)
    {
	a = objects[i];
	if (modemove)// && a->modemove != MODEDIE)
	    if (a->modemove != modemove)
		continue;
	if (actiononplayers & (1<<a->playernr))
	{
		if (checkready && !IsReadyOBJ(a))
		    continue;
		else if ( (*UnitTypeFunc)(a->SC_Unit))
		{
		    if (searcharea)
			if (!SearchObjInArea(a,searcharea))
			    continue;
		    nrofunits++;
		    if (retlast)
		    {
			SetTriggeredUnitState(a, SETSTATE);
//			objlist->AddElem(a);
			last = a;
			if (--cntunits==0)
			    break;
		    }
		}
	}
    }
    if (retlast)
	*retlast=last;
    if (ConditionFunction)
	return((*ConditionFunction)(&nrofunits,cntunits));
    else
	return(nrofunits);
}
//=================================================
int CheckForUnitStat(int (*ConditionFunction)(int *, int),int actiononplayers,int unitid,int cntunits,int stattype)
{
    int j,mask,nrofunits=0;
    if (stattype==UNITSTAT_HAVE)
    {
	if (unitid>=MAX_UNITS_ELEM)				
	{
	    stattype=UNITSTAT_PRODUCING;//because last construction unit do not do defeat game
	}
    }								
    for (j=0,mask=1;j<MAXPLAYERS;j++,mask<<=1)
    {
	if (actiononplayers&mask)
	{
	    nrofunits+=map.units_stat.stattype[stattype][j][unitid];
	}
    }
    return((*ConditionFunction)(&nrofunits,cntunits));
}
//=================================================
int CheckForOre(int (*ConditionFunction)(int *, int),int playernr,int cnt)
{
    return((*ConditionFunction)(&PLAYER[playernr].minerals,cnt<<8));
}
//=================================================
int CheckForGas(int (*ConditionFunction)(int *, int),int playernr,int cnt)
{
    return((*ConditionFunction)(&PLAYER[playernr].gas,cnt<<8));
}
//=================================================
int CheckForOreAndGas(int (*ConditionFunction)(int *, int),int playernr,int cnt)
{
    return((*ConditionFunction)(&PLAYER[playernr].minerals,cnt<<8)&&(*ConditionFunction)(&PLAYER[playernr].gas,cnt<<8));
}
//=================================================
int (*CheckResFunctions[3])(int (*ConditionFunction)(int *, int), int, int)=
		    {
			&CheckForOre,
			&CheckForGas,
			&CheckForOreAndGas
		    };
//=================================================
int AddForOre(int (*ConditionFunction)(int *, int),int actiononplayers,int cnt)
{
    for (int i=0;i<MAXPLAYERS;i++)
	if (actiononplayers&(1<<i))
	{
	    (*ConditionFunction)(&PLAYER[i].minerals,cnt<<8);
	    if (PLAYER[i].minerals<0)
		PLAYER[i].minerals=0;
	}
}
//=================================================
int AddForGas(int (*ConditionFunction)(int *, int),int actiononplayers,int cnt)
{
    for (int i=0;i<MAXPLAYERS;i++)
	if (actiononplayers&(1<<i))
	{
	    (*ConditionFunction)(&PLAYER[i].gas,cnt<<8);
	    if (PLAYER[i].gas<0)
		PLAYER[i].gas=0;
	}
}
//=================================================
int AddForOreAndGas(int (*ConditionFunction)(int *, int),int actiononplayers,int cnt)
{
    for (int i=0;i<MAXPLAYERS;i++)
	if (actiononplayers&(1<<i))
	{
	    (*ConditionFunction)(&PLAYER[i].gas,cnt<<8);
	    if (PLAYER[i].minerals<0)
		PLAYER[i].minerals=0;
	    (*ConditionFunction)(&PLAYER[i].minerals,cnt<<8);
	    if (PLAYER[i].gas<0)
		PLAYER[i].gas=0;
	}
}
//=================================================
int (*AddResFunctions[3])(int (*ConditionFunction)(int *, int), int, int)=
		    {
			&AddForOre,
			&AddForGas,
			&AddForOreAndGas
		    };
//=================================================
//=================================================
//=================================================
//=================================================
//=================================================
//=================================================
void Triggers_ReduceMusicVolume(void)
{
    Triggers_SetMusicVolume(gameconf.audioconf.musicvolume/3);
}
//=================================================
void Triggers_RestoreMusicVolume(void)
{
    Triggers_SetMusicVolume(TRIG_MusicVolume);
}
//=================================================
void Triggers_SetMusicVolume(int volume)
{
    TRIG_MusicVolume = gameconf.audioconf.musicvolume;
    if (audiostream)
    {
    	audiostream->ChangeMusicVolume(volume);
    }
}
//=================================================
void Triggers_ReduceSoundVolume(int exceptchannelid)
{
    setreducevolumeallsfx(0,exceptchannelid);
}
//=================================================
void Triggers_RestoreSoundVolume(void)
{
    setreducevolumeallsfx(1,0);
}
//=================================================
void Init_Triggers_Variables(int cnttrg)
{
    TRIG_Portrait = SC_NOUNITNR;
    TRIG_MusicQuieter = 0;
    TRIG_stopalltriggers = 0;
    TRIG_active=0;
    TRIG_snd=0;
    TRIG_quit=0;
    TRIG_pause=0;
    TRIG_ChangeStat=0;
    TRIG_preserve=0;
    TRIG_commentstr=NULL;
    TRIG_leaderboardcomputerplayers=1;
    ALLTRIG_pause = (int *) wmalloc(cnttrg * sizeof (int ));
    memset(ALLTRIG_pause,0,cnttrg * sizeof (int )); 
}
//=================================================
void First_Triggers_Prepare(mapinfo *info,int cnttrg,MAP_TRIGS *trigs)
{
    int actiononplayers;
    int i,j;
    MAP_TRIGS	*trg;
    MAP_TRIGS	*temptrg;
    MAP_TRIGS	(*alltrigs)[]=(MAP_TRIGS (*)[]) trigs;
    //allocation memory for all triggers pause 1 int (4 bytes) for each trigger;
    for (i=0;i<cnttrg;i++)
    {
	temptrg=&(*alltrigs)[i];
	temptrg->groupinfo.playersmask=AllGroups_Prepare(info,temptrg);
    }
    Init_Triggers_Variables(cnttrg);
}
//=================================================
void Triggers_Parce(mapinfo *info,int cnttrg,MAP_TRIGS *trigs,int deltatick)
{
    int i,j,mask;
    MAP_TRIGS	*trg;
    MAP_TRIGS	*temptrg;
    MAP_TRIGS	(*alltrigs)[]=(MAP_TRIGS (*)[]) trigs;
    TRIG_active=1;
    TRIG_inittriggers = 1;

    if (TRIG_MusicQuieter)
    {
	TRIG_MusicQuieter -= deltatick;
	if (TRIG_MusicQuieter <= 0)
	{
	    //restore music volume
	    TRIG_MusicQuieter = 0;
	    Triggers_RestoreMusicVolume();
	    Triggers_RestoreSoundVolume();
	    //check if the same portrait -> show main portrait
	    staticport.UnHoldPortrait();
	    if (staticport.GetShowedPortrait() == TRIG_Portrait)
	    {
		//show main portrait
		ShowAdvisorPortrait();
	    }
	}
    }
    for (i=0;i<cnttrg;i++)
    {
	temptrg=&(*alltrigs)[i];
        if (temptrg->groupinfo.playersmask)
        {
	    for (j=0,mask=1;j<MAXPLAYERS;j++,mask<<=1)
	    {
    		if (temptrg->groupinfo.playersmask & mask)
    		{
    		    if (Condition_Prepare(info,temptrg,i,j,mask,deltatick))
	    		temptrg->groupinfo.playersmask &= ~mask;//prevent enter again if all triggers parced
	    	    if (TRIG_quit)
	    	    {
	    		return;
	    	    }
	    	    if (TRIG_pause)
	    	    {
	    		TRIG_pause=0;
	    		break;
	    	    }
	    	    if (!temptrg->groupinfo.playersmask)
	    		break;
		}
	    }
	}
    }
    TRIG_active=0;
}
//=================================================
int Condition_Prepare(mapinfo *info,MAP_TRIGS *temptrg,int trig_nr,int playernr,int playernrmask,int deltatick)
{
    int i,j,mask,allcond,applycond,playergrp,nrofunits,nrofunits2,totalcond,conddone,nrofresources,locnr;
    int switchnr,switchfunc,tick,ownactiononplayers,opponents,remainopponents;

    int (*ResTypeFunc)(int (*)(int *, int),int playernr,int cnt);
    unsigned char unitnr;
    int (*comparefunc)(int *,int );
    char *txtstr;
    allcond=0;
    conddone=0;
    totalcond=TRIG_MAX_CONDITIONS;
    if (ALLTRIG_pause[trig_nr])
    {
	ALLTRIG_pause[trig_nr] -= deltatick;
	if (ALLTRIG_pause[trig_nr] > 0)
	    return(0);
	ALLTRIG_pause[trig_nr] = 0;
    }
    if (temptrg->condition[0].TriggerStateChecked&playernrmask)
    {
	conddone=TRIG_MAX_CONDITIONS;
    }
    else
    {
	for (i=0;i<TRIG_MAX_CONDITIONS;i++)
	{
	    if (temptrg->condition[i].conditiontype==TRG_CONDITIONTYPE_NONE)
	    {
		totalcond=i;
		break;
	    }
	    if (temptrg->condition[i].SwitchState&TRIGGERCONDITIONSTATE_DISABLED)
	    {
		applycond=1;
	    }
	    else
	    {
		applycond=0;
		comparefunc=ConditionFunctions[temptrg->condition[i].subcond];
		switch(temptrg->condition[i].conditiontype)
		{
		case TRG_CONDITIONTYPE_COUNTDOWNTIMER://1
		    applycond=(*comparefunc)(&info->CountDownTimer,temptrg->condition[i].seconds*MAXGAMECYCLESPERSECOND);
		    break;
		case TRG_CONDITIONTYPE_COMMAND://2
		    ownactiononplayers=OneGroup_Prepare(info,temptrg->condition[i].actiononplayers,playernrmask);
		    unitnr=temptrg->condition[i].unittype;
		    nrofunits=temptrg->condition[i].seconds;
		    applycond=CheckForUnit(comparefunc,ownactiononplayers,unitnr,nrofunits,NULL,NULL);
		    break;
		case TRG_CONDITIONTYPE_BRING://3
		    ownactiononplayers=OneGroup_Prepare(info,temptrg->condition[i].actiononplayers,playernrmask);
		    unitnr=temptrg->condition[i].unittype;
		    nrofunits=temptrg->condition[i].seconds;
		    locnr=temptrg->condition[i].locationnr-1;
//		    txtstr=getmapSTR(info,info->gamelocations[locnr].stringID-1);
		    applycond = CheckForUnit(comparefunc,ownactiononplayers,unitnr,nrofunits,NULL,&info->gamelocations[locnr].coords);
		    break;
		case TRG_CONDITIONTYPE_ACCUMULATE://4
		    ownactiononplayers=OneGroup_Prepare(info,temptrg->condition[i].actiononplayers,playernrmask);
		    ResTypeFunc=CheckResFunctions[temptrg->condition[i].ResourceType];
		    nrofresources=temptrg->condition[i].seconds;
		    for (j=0,mask=1;j<MAXPLAYERS;j++,mask<<=1)
		    {
			if (ownactiononplayers&mask)
			{
			    applycond=(*ResTypeFunc)(comparefunc,j,nrofresources);
			    if (!applycond)
				return(0);
			    ownactiononplayers&=~mask;
			    if (!ownactiononplayers)
				break;
			}
		    }
		    break;
		case TRG_CONDITIONTYPE_KILL://5
		    nrofunits=0;
		    unitnr=temptrg->condition[i].unittype;
		    nrofunits2=temptrg->condition[i].seconds;
		    ownactiononplayers=OneGroup_Prepare(info,temptrg->condition[i].actiononplayers,playernrmask);
		    for (j=0,mask=1;j<MAXPLAYERS;j++,mask<<=1)
			if (!(ownactiononplayers&mask))
			    nrofunits += info->units_stat.stattype[UNITSTAT_DEATH][j][unitnr];
		    applycond=(*comparefunc)((int *)&nrofunits,nrofunits2);
		    break;
		case TRG_CONDITIONTYPE_COMMANDTHEMOSTAT://7
		    unitnr=temptrg->condition[i].unittype;
		    locnr=temptrg->condition[i].locationnr-1;
		    comparefunc=ConditionFunctions[TRG_TYPEFUNC_ATLEAST];
//		    txtstr=getmapSTR(info,info->gamelocations[locnr].stringID-1);
		    applycond=CheckForUnit(comparefunc,playernrmask,unitnr,0,NULL,&info->gamelocations[locnr].coords);
		    break;
		case TRG_CONDITIONTYPE_SWITCH://11
		    switchnr=temptrg->condition[i].ResourceType;
		    applycond=(*comparefunc)((int *)&info->Switch[switchnr],-1);
		    break;
		case TRG_CONDITIONTYPE_ELAPSEDTIME://12
		    tick=mytimer.GetCurrentGameTime();
		    applycond=(*comparefunc)(&tick,temptrg->condition[i].seconds);
		    break;
//		case TRG_CONDITIONTYPE_MISSIONBRIEFING://13
//		    applycond=1;
//		    break;
		case TRG_CONDITIONTYPE_OPPONENTS://14
		    nrofunits = 0;
		    opponents = temptrg->condition[i].seconds;
		    ownactiononplayers = OneGroup_Prepare(info,temptrg->condition[i].actiononplayers,playernrmask);
		    for (j=0,mask=1;j<PLAYEDPLAYERS;j++,mask<<=1)
		    {
			if (!(ownactiononplayers & mask))
			{
			    remainopponents = 0;
			    for (i=j+1;i<PLAYEDPLAYERS;i++)
		    		if (player_aliance(j,i) == ENEMYOBJ)
				    remainopponents++;
			    applycond = (*comparefunc)(&remainopponents,opponents);
			    if (!applycond)
				return(0);
			}
		    }
		    break;
		case TRG_CONDITIONTYPE_DEATHS://15
		    ownactiononplayers=OneGroup_Prepare(info,temptrg->condition[i].actiononplayers,playernrmask);
		    unitnr=temptrg->condition[i].unittype;
		    nrofunits=temptrg->condition[i].seconds;
		    applycond=CheckForUnitStat(comparefunc,ownactiononplayers,unitnr,nrofunits,UNITSTAT_DEATH);
		    break;
		case TRG_CONDITIONTYPE_COMMANDTHELEASTAT://17
		    unitnr=temptrg->condition[i].unittype;
		    locnr=temptrg->condition[i].locationnr-1;
		    comparefunc=ConditionFunctions[TRG_TYPEFUNC_ATMOST];
//		    txtstr=getmapSTR(info,info->gamelocations[locnr].stringID-1);
		    applycond=CheckForUnit(comparefunc,playernrmask,unitnr,0,NULL,&info->gamelocations[locnr].coords);
		    break;
		case TRG_CONDITIONTYPE_SCORE://21
		    ownactiononplayers=OneGroup_Prepare(info,temptrg->condition[i].actiononplayers,playernrmask);
		    unitnr=temptrg->condition[i].ResourceType;
		    nrofunits=0;
		    for (j=0,mask=1;j<MAXPLAYERS;j++,mask<<=1)
		    {
			if (ownactiononplayers&mask)
			{
			    applycond=(*comparefunc)(&info->units_stat.score[j][unitnr],temptrg->condition[i].seconds);
			    if (!applycond)
				return(0);
			    ownactiononplayers&=~mask;
			    if (!ownactiononplayers)
				break;
			}
		    }
		    break;
		case TRG_CONDITIONTYPE_ALWAYS://22
		    applycond=1;
		    break;
		case TRG_CONDITIONTYPE_NEVER://23
		    applycond=0;
		    break;
		default:
//		    applycond=1;
		    printf("conditiontype %d for trigger unknown\n",temptrg->condition[i].conditiontype);
		    break;
		}//switch
	    }//else
	    if (!applycond)
		return(0);
	    conddone+=applycond;
	}//for
    }
    if (conddone>=totalcond)
    {
	TRIG_preserve=0;
	temptrg->condition[0].TriggerStateChecked|=playernrmask;
        allcond=Action_Prepare(info,temptrg,trig_nr,playernr,playernrmask);
	if (TRIG_preserve)
	{
	    temptrg->condition[0].TriggerStateChecked&=~playernrmask;
	    allcond=0;
	    //make all action on this triggers have initial state
	    for (j=0;j<TRIG_MAX_ACTIONS;j++)
	    {
		if (temptrg->action[j].actiontype==TRG_ACTIONTYPE_NONE)
		    break;
		temptrg->action[j].TriggerStateDone&=~playernrmask;
	    }
	}
	if (TRIG_pause)
	{
	    ALLTRIG_pause[trig_nr] = TRIG_pause;
	}
    }
    return(allcond);
}
//=================================================
int Action_Prepare(mapinfo *info,MAP_TRIGS *temptrg,int trig_nr,int playernr,int playernrmask)
{
    int min,gas,settype,err,i,j,jj,mask,mask2,triggcnt,triggset,locnr,unitnr,sx,sy;
    int textid,waveid,soundid,funcdelta,deltapaused,waittime,switchnr,nrofunits;
    int searchloc,ownedactiononplayers,xobj,yobj,xobj2,yobj2,grpposx,grpposy,state,propnr;
    int haverescued,openstate,movestate,deltax,deltay,oldsnd;

    char *txtstr;
    struct triggerunit *triggerunitprop;

    int (*comparefunc)(int *,int );
    int (*ResTypeFunc)(int (*)(int *, int),int playernr,int cnt);
    LEADERBOARD *leaderboard;
    OBJ *newobj;
    OBJstruct *b;
    err=0;
    triggcnt=0;
#ifdef TRIG_DEBUG	
	//show comment trigger first
	for (i=0;i<TRIG_MAX_ACTIONS;i++)
	{
	    if (temptrg->action[i].actiontype==TRG_ACTIONTYPE_NONE)
		break;
	    if (temptrg->action[i].actiontype==TRG_ACTIONTYPE_COMMENT)
	    {
	        TRIG_commentstr=getmapSTR(info,temptrg->action[i].stringID-1);
		DEBUGMESSCR("COMMENT[%s]\n",TRIG_commentstr);
		break;
	    }
	
	}
#endif
    for (i=0;i<TRIG_MAX_ACTIONS;i++)
    {
	    triggset=0;
	    if ((temptrg->action[i].TriggerStateDone&playernrmask)||
    		(temptrg->action[i].TriggerEntryFlags&TRIGGERENTRYFLAG_STATEDISABLED))
	    {
	        triggset=1;
		triggcnt+=triggset;
		continue;	//ignore this trigger
	    }
//	    printf("action type=%d\n",temptrg->action[i].actiontype);
	    switch(temptrg->action[i].actiontype)
	    {
		case TRG_ACTIONTYPE_NONE://0
		    triggcnt+=TRIG_MAX_ACTIONS-i;
//		    PAUSEINTRIG=0;		//end of actions remove pause if have
		    return(triggcnt==TRIG_MAX_ACTIONS);
		case TRG_ACTIONTYPE_VICTORY://1
		    PAUSEINTRIG=0;
		    gamestatus=WINGAME;
		    gamestatusremainticks = WAITGAMETICKSBEFORESHOWGAMESTATUSMESSAGE;
		    TRIG_stopalltriggers = 1;
		    TRIG_quit=1;
		    triggset=1;
		    return(1);
		    break;
		case TRG_ACTIONTYPE_DEFEAT://2
		    PAUSEINTRIG=0;
		    if (playernr==NUMBGAMER)
		    {
			gamestatus=DEFEATGAME;
			gamestatusremainticks = WAITGAMETICKSBEFORESHOWGAMESTATUSMESSAGE;
			TRIG_stopalltriggers = 1;
		    }
		    else
		    {
			MakeObserver(playernr);
		    }
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_PRESERVETRIGGER://3
		    triggset=1;
		    TRIG_preserve=1;
		    break;
		case TRG_ACTIONTYPE_WAIT://4
		    TRIG_pause=temptrg->action[i].pauseatime;
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_PAUSEGAME://5
		    if (!NETWORKGAME)
		    {
			info->flags |= STARMAP_FLAG_COUNTSTOPED;
			PAUSEINTRIG=1;
		    }
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_UNPAUSEGAME://6
		    if (!NETWORKGAME)
		    {
			info->flags &= ~STARMAP_FLAG_COUNTSTOPED;
			PAUSEINTRIG=0;
		    }
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_TRANSMISSION://7
		    deltapaused=temptrg->action[i].rescount;
		    comparefunc=ConditionFunctions[temptrg->action[i].subaction];
		    locnr=temptrg->action[i].locationnr-1;
//		    txtstr=getmapSTR(info,temptrg->action[i].stringID-1);
		    waveid=temptrg->action[i].waveID-1;
		    FULLFILENAME[0]=0;
		    if (prefix_campaignpath)
		    {
			strcpy(FULLFILENAME,prefix_campaignpath);
			strcat(FULLFILENAME,"\\");
		    }
		    strcat(FULLFILENAME,getmapSTR(info,waveid));
		    soundid = loadandplaywav(info->mpqid,NULL,FULLFILENAME,1,0);
//		    DEBUGMESSCR("sountid=%d\n",soundid);
//		    waittime=getchannelplaylength(soundid);
		    TRIG_pause = temptrg->action[i].pauseatime;
		    (*comparefunc)(&TRIG_pause,deltapaused);
		    TRIG_pause = TRIG_pause*timewaitticks[gameconf.speedconf.gamespeed]/MAXGAMECYCLESPERSECOND;
		    waittime=TRIG_pause;

		    if (waittime<TRIGGER_DISPLAYMESSAGETIME)
			waittime=TRIGGER_DISPLAYMESSAGETIME;
		    unitnr=temptrg->action[i].unitorrestype;

		    //show transmission smk portrait
		    TRIG_Portrait = unitnr;
		    SetPortrait(unitnr,SMKTALK,NOSOUNDFILENR,TRIG_pause);
		    if (!TRIG_MusicQuieter)
		    {
			Triggers_ReduceMusicVolume();
			Triggers_ReduceSoundVolume(soundid);
			staticport.HoldPortrait();
		    }
		    TRIG_MusicQuieter += TRIG_pause;
		    //blink transmission unit
//		    ownedactiononplayers=OneGroup_Prepare(info,temptrg->action[i].actiononplayers,playernrmask);
//		    nrofunits = 1;
		    newobj=NULL;
//		    nrofunits=CheckForUnit(NULL,ownedactiononplayers,unitnr,nrofunits,&newobj,&info->gamelocations[locnr].coords);
		    nrofunits=CheckForUnit(NULL,TRIGGER_ALLPLAYERSBITMASK,unitnr,1,&newobj,&info->gamelocations[locnr].coords);
		    if (newobj)
		    {
			if (GetTriggeredUnitState(newobj))
		    	    SetTriggeredUnitState(newobj, CLEARSTATE);
			SetBlinkOBJ(newobj);
		    }

		    //show transmission text
		    if (playernr==NUMBGAMER && temptrg->action[i].stringID)
			chatbar.addbarmessage(getmapSTR(info,temptrg->action[i].stringID-1),IDFONT10,GWHITECOLORFONT,waittime,BF_ALLOCBUF|BF_FORCEADD);
//		    printf("transmission pause=%d waivelength=%d\n",temptrg->action[i].pauseatime,waittime);
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_PLAYWAV://8
		    waveid=temptrg->action[i].waveID-1;
		    FULLFILENAME[0]=0;
		    if (prefix_campaignpath)
		    {
			strcpy(FULLFILENAME,prefix_campaignpath);
			strcat(FULLFILENAME,"\\");
		    }
		    strcat(FULLFILENAME,getmapSTR(info,waveid));
		    loadandplaywav(info->mpqid,NULL,FULLFILENAME,1,0);
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_DISPLAYTEXTMESSAGE://9
		    if (playernr==NUMBGAMER)
		    {
			if (gameconf.audioconf.subtitles || (temptrg->action[i].TriggerEntryFlags & TRIGGERENTRYFLAG_ALWAYSDISPLAY))
			{
			    chatbar.addbarmessage(getmapSTR(info,temptrg->action[i].stringID-1),
						IDFONT10,GWHITECOLORFONT,TRIGGER_DISPLAYMESSAGETIME,BF_ALLOCBUF|BF_FORCEADD);
			}
		    }
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_CENTERVIEW://10
		    locnr=temptrg->action[i].locationnr-1;
		    CenterXYArea(&info->gamelocations[locnr].coords,&sx,&sy);
//		    SetVisualMapPositionCenter(sx,sy);
		    MoveVisualMapPositionCenter(sx,sy);
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_CREATEUNITWITHPROPERTIES://11
		    if (temptrg->action[i].TriggerEntryFlags&TRIGGERENTRYFLAG_MANYUNITS)
		    {
			nrofunits=temptrg->action[i].subaction;
			if (!nrofunits)
			    nrofunits=1;
		    }
		    else
			nrofunits=1;
		    propnr=temptrg->action[i].rescount-1;
		    if (propnr<0)
			goto creationwithoutproperties;
		    triggerunitprop=&info->triggerunitsprop[propnr];
		    unitnr=temptrg->action[i].unitorrestype;
		    b=loadobj(unitnr);
		    locnr=temptrg->action[i].locationnr-1;
		    ownedactiononplayers=OneGroup_Prepare(info,temptrg->action[i].actiononplayers,playernrmask);
		    deltax=nrofunits;
		    for (j=0,mask=1;j<MAXPLAYERS;j++,mask<<=1)
		    {
		        if (ownedactiononplayers&mask)
		        {
			    while(nrofunits--)
			    {
				MAPREGENERATIONBIT=1;
				MAPUNITSREGENERATIONBIT=1;
//				CenterXYArea(&info->gamelocations[locnr].coords,&xobj,&yobj);
			        getcoordofnewunit(unitnr,&xobj,&yobj,&info->gamelocations[locnr].coords);
				newobj = createunitwithproperties(xobj,yobj,unitnr,j,
					     triggerunitprop->special_props,triggerunitprop->special_prop2,triggerunitprop->state_flags,
					     triggerunitprop->hitpoints,triggerunitprop->shieldpoints,triggerunitprop->energypoints,
					     triggerunitprop->resource_count,triggerunitprop->units_in_hangar);


				makeoneobjseeopen(newobj,loadobj(newobj->SC_Unit));
//				makeprop(newobj);		//make visual/working/stasis properties;
			    }
			    nrofunits=deltax;
			    ownedactiononplayers&=~mask;
			    if (!ownedactiononplayers)
				break;
			}
		    }
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_SETMISSIONOBJECTIVES://12
		    fill_missionobjectives(getmapSTR(info,temptrg->action[i].stringID-1));
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_SETSWITCH://13
		    comparefunc=ConditionFunctions[temptrg->action[i].subaction];	//action applyed to 
		    switchnr=temptrg->action[i].rescount;
		    (*comparefunc)((int *)&info->Switch[switchnr],-1);
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_SETCOUNTDOWNTIMER://14
		    comparefunc=ConditionFunctions[temptrg->action[i].subaction];	//action applyed to 
		    (*comparefunc)(&info->CountDownTimer,temptrg->action[i].pauseatime*MAXGAMECYCLESPERSECOND);
		    info->flags |= STARMAP_FLAG_HAVECOUNTDOWN;
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_RUNAISCRIPT://15
		    TRG_RunAIScript(info,(temptrg->action[i].scriptnr<<16) | temptrg->action[i].rescount,playernr,playernrmask);
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_RUNAISCRIPTATLOCATION://16
		    locnr=temptrg->action[i].locationnr-1;
		    TRG_RunAIScriptAtLocation(info,(temptrg->action[i].scriptnr<<16) | temptrg->action[i].rescount,playernr,playernrmask,locnr);
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_LEADERBOARDCONTROL://17
		case TRG_ACTIONTYPE_LEADERBOARDRESOURCES://19
		case TRG_ACTIONTYPE_LEADERBOARDPOINTS://21
		    unitnr=temptrg->action[i].unitorrestype;
		    nrofunits=temptrg->action[i].rescount;
		    leaderboard=AddLeaderBoard(0,temptrg->action[i].actiontype,info,playernrmask,unitnr,nrofunits,temptrg->action[i].stringID-1);
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_KILLUNIT://22
		    if (temptrg->action[i].TriggerEntryFlags&TRIGGERENTRYFLAG_MANYUNITS)
		    {
			nrofunits=temptrg->action[i].subaction;
			if (!nrofunits)
			    nrofunits=MaxObjects;
		    }
		    else
			nrofunits=MaxObjects;
		    unitnr=temptrg->action[i].unitorrestype;
		    ownedactiononplayers=OneGroup_Prepare(info,temptrg->action[i].actiononplayers,playernrmask);
		    newobj=NULL;
		    nrofunits = CheckForUnit(NULL,ownedactiononplayers,unitnr,nrofunits,&newobj,NULL);
		    if (nrofunits)
		    {
			for (j=0;j<MaxObjects;j++)
			{
			    if (GetTriggeredUnitState(objects[j]))
			    {
				SetTriggeredUnitState(objects[j], CLEARSTATE);
				oldsnd = UnBlockSoundToPlay();
				dieobj(objects[j]);
				RestoreSoundToPlay(oldsnd);
				if (--nrofunits==0)
				    break;
			    }
			}
		    }
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_KILLUNITATLOCATION://23
		    if (temptrg->action[i].TriggerEntryFlags&TRIGGERENTRYFLAG_MANYUNITS)
		    {
			nrofunits=temptrg->action[i].subaction;
			if (!nrofunits)
			    nrofunits=MaxObjects;
		    }
		    else
			nrofunits=MaxObjects;
		    unitnr=temptrg->action[i].unitorrestype;
		    locnr=temptrg->action[i].locationnr-1;
		    ownedactiononplayers=OneGroup_Prepare(info,temptrg->action[i].actiononplayers,playernrmask);
		    newobj=NULL;
		    nrofunits=CheckForUnit(NULL,ownedactiononplayers,unitnr,nrofunits,&newobj,&info->gamelocations[locnr].coords);
		    if (nrofunits)
		    {
			for (j=0;j<MaxObjects;j++)
			{
			    if (GetTriggeredUnitState(objects[j]))
			    {
				//done by previous trigger
				SetTriggeredUnitState(objects[j], CLEARSTATE);
				oldsnd = UnBlockSoundToPlay();
				dieobj(objects[j]);
				RestoreSoundToPlay(oldsnd);
				if (--nrofunits==0)
				    break;
			    }
			}
		    }
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_REMOVEUNIT://24
		    TRIG_ChangeStat=0;
		    if (temptrg->action[i].TriggerEntryFlags&TRIGGERENTRYFLAG_MANYUNITS)
		    {
			nrofunits=temptrg->action[i].subaction;
			if (!nrofunits)
			    nrofunits=MaxObjects;
		    }
		    else
			nrofunits=MaxObjects;
		    unitnr=temptrg->action[i].unitorrestype;
		    ownedactiononplayers=OneGroup_Prepare(info,temptrg->action[i].actiononplayers,playernrmask);
		    newobj=NULL;
		    nrofunits=CheckForUnit(NULL,ownedactiononplayers,unitnr,nrofunits,&newobj,NULL);
		    if (nrofunits)
		    {
			for (j=0;j<MaxObjects;j++)
			{
			    if (GetTriggeredUnitState(objects[j]))
			    {
				SetTriggeredUnitState(objects[j], CLEARSTATE);
			        oldsnd=BlockSoundToPlay();
				dieobj_silently(objects[j]);
				RestoreSoundToPlay(oldsnd);
				if (--nrofunits==0)
				    break;
			    }
			}
		    }
		    TRIG_ChangeStat=1;
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_REMOVEUNITATLOCATION://25
		    TRIG_ChangeStat=0;
		    if (temptrg->action[i].TriggerEntryFlags&TRIGGERENTRYFLAG_MANYUNITS)
		    {
			nrofunits=temptrg->action[i].subaction;
			if (!nrofunits)
			    nrofunits=MaxObjects;
		    }
		    else
			nrofunits=MaxObjects;
		    unitnr=temptrg->action[i].unitorrestype;
		    locnr=temptrg->action[i].locationnr-1;
		    ownedactiononplayers=OneGroup_Prepare(info,temptrg->action[i].actiononplayers,playernrmask);
		    newobj=NULL;
		    nrofunits=CheckForUnit(NULL,ownedactiononplayers,unitnr,nrofunits,&newobj,&info->gamelocations[locnr].coords);
		    if (nrofunits)
		    {
			for (j=0;j<MaxObjects;j++)
			{
			    if (GetTriggeredUnitState(objects[j]))
			    {
				//done by previous trigger
				SetTriggeredUnitState(objects[j], CLEARSTATE);
			        oldsnd=BlockSoundToPlay();
				dieobj_silently(objects[j]);
				RestoreSoundToPlay(oldsnd);
				if (--nrofunits==0)
				    break;
			    }
			}
		    }
		    TRIG_ChangeStat=1;
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_SETRESOURCES://26
		    ownedactiononplayers=OneGroup_Prepare(info,temptrg->action[i].actiononplayers,playernrmask);
		    comparefunc=ConditionFunctions[temptrg->action[i].subaction];	//action applyed to 
		    ResTypeFunc=AddResFunctions[temptrg->action[i].unitorrestype];
		    (*ResTypeFunc)(comparefunc,ownedactiononplayers,temptrg->action[i].rescount);
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_SETSCORE://27
		    ownedactiononplayers=OneGroup_Prepare(info,temptrg->action[i].actiononplayers,playernrmask);
		    comparefunc=ConditionFunctions[temptrg->action[i].subaction];
		    state=temptrg->action[i].unitorrestype;		//type of score
		    for (j=0,mask=1;j<MAXPLAYERS;j++,mask<<=1)
		    {
			if (ownedactiononplayers&mask)
			    (*comparefunc)(&info->units_stat.score[j][state],temptrg->action[i].rescount);
		    }
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_MINIMAPPING://28
		    //show minimap ping only for current player
		    if (playernr == NUMBGAMER)
		    {
			locnr = temptrg->action[i].locationnr-1;
			CenterXYArea(&info->gamelocations[locnr].coords,&xobj,&yobj);

			getminimapcoord(info,xobj,yobj,&grpposx,&grpposy);

			ShowBlink(grpposx,grpposy,5);
		    }

		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_MUTEUNITSPEACH://30
		    BlockSoundToPlay();
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_UNMUTEUNITSPEACH://31
		    UnBlockSoundToPlay();
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_LEADERBOARDCOMPUTERPLAYERS://32
		    comparefunc=ConditionFunctions[temptrg->action[i].subaction];
		    (*comparefunc)((int *)&TRIG_leaderboardcomputerplayers,-1);
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_LEADERBOARDGOALKILLS://36
		    unitnr=temptrg->action[i].unitorrestype;
		    nrofunits=temptrg->action[i].rescount;
		    leaderboard=AddLeaderBoard(1,temptrg->action[i].actiontype,info,playernrmask,unitnr,nrofunits,temptrg->action[i].stringID-1);
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_LEADERBOARDGOALPOINTS://37
		    unitnr=temptrg->action[i].unitorrestype;
		    nrofunits=temptrg->action[i].rescount;
		    leaderboard=AddLeaderBoard(1,temptrg->action[i].actiontype,info,playernrmask,unitnr,nrofunits,temptrg->action[i].stringID-1);
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_MOVELOCATION://38
		    searchloc=temptrg->action[i].locationnr-1;
//		    txtstr=getmapSTR(info,info->gamelocations[searchloc].stringID-1);
		    ownedactiononplayers=OneGroup_Prepare(info,temptrg->action[i].actiononplayers,playernrmask);
		    locnr=temptrg->action[i].rescount-1;
//		    txtstr=getmapSTR(info,info->gamelocations[locnr].stringID-1);
		    unitnr=temptrg->action[i].unitorrestype;
		    newobj=NULL;
		    CheckForUnit(NULL,ownedactiononplayers,unitnr,1,&newobj,&info->gamelocations[searchloc].coords);
		    if (newobj)
		    {
			SetTriggeredUnitState(newobj, CLEARSTATE);
			CenterXYArea(&info->gamelocations[locnr].coords,&sx,&sy);
			//deltax deltay delta moved location
			deltax = GetOBJx(newobj) - sx;
			deltay = GetOBJy(newobj) - sy;
			info->gamelocations[locnr].coords.x1 += deltax;
			info->gamelocations[locnr].coords.y1 += deltay;
			info->gamelocations[locnr].coords.x2 += deltax;
			info->gamelocations[locnr].coords.y2 += deltay;
		    }
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_MOVEUNITS://39
		    locnr=temptrg->action[i].locationnr-1;
		    searchloc=temptrg->action[i].rescount-1;
		    ownedactiononplayers=OneGroup_Prepare(info,temptrg->action[i].actiononplayers,playernrmask);
		    unitnr=temptrg->action[i].unitorrestype;
		    if (temptrg->action[i].TriggerEntryFlags&TRIGGERENTRYFLAG_MANYUNITS)
		    {
			nrofunits=temptrg->action[i].subaction;
			if (!nrofunits)
			    nrofunits=MaxObjects;
		    }
		    else
			nrofunits=MaxObjects;
		    newobj=NULL;
		    nrofunits=CheckForUnit(NULL,ownedactiononplayers,unitnr,nrofunits,&newobj,&info->gamelocations[locnr].coords);
		    if (nrofunits)
		    {
			for (j=0;j<MaxObjects;j++)
			{
			    if (GetTriggeredUnitState(objects[j]))
			    {
				//done by previous trigger
				SetTriggeredUnitState(objects[j], CLEARSTATE);
				b=loadobj(objects[j]->SC_Unit);
				//teleport to other location
//				CenterXYArea(&info->gamelocations[searchloc].coords,&xobj,&yobj);
				getcoordofnewunit(objects[j]->SC_Unit,&xobj,&yobj,&info->gamelocations[searchloc].coords);
				ChangeObjXY(objects[j], xobj,yobj);

				ForceKartChanges(objects[j]);
				moveobj(objects[j],NULL,MODESTOP,NOSHOWERROR);
				if (--nrofunits==0)
				    break;
			    }
			}
		    }
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_SETNEXTSCENARIO://41
		    triggset=1;
		    txtstr=getmapSTR(info,temptrg->action[i].stringID-1);
		    getmissionidformmissionname(txtstr,&nextscenario);
		    break;
		case TRG_ACTIONTYPE_SETDOODADSTATE://42
		    if (temptrg->action[i].TriggerEntryFlags & TRIGGERENTRYFLAG_MANYUNITS)
			openstate = temptrg->action[i].subaction;
		    else
			if (temptrg->action[i].subaction == TRG_TYPEFUNC_SWITCHSET   ||
			    temptrg->action[i].subaction == TRG_TYPEFUNC_SWITCHCLEAR || 
			    temptrg->action[i].subaction == TRG_TYPEFUNC_SWITCHTOGGLE )
			    openstate = temptrg->action[i].subaction;
			else
			    openstate = TRG_TYPEFUNC_SWITCHTOGGLE;
		    comparefunc=ConditionFunctions[openstate];
		    searchloc=temptrg->action[i].locationnr-1;
		    ownedactiononplayers=OneGroup_Prepare(info,temptrg->action[i].actiononplayers,playernrmask);
		    unitnr=temptrg->action[i].unitorrestype;
		    newobj=NULL;
		    CheckForUnit(NULL,ownedactiononplayers,unitnr,1,&newobj,&info->gamelocations[searchloc].coords);
		    if (newobj)
		    {
			SetTriggeredUnitState(newobj, CLEARSTATE);
			movestate = GetDoodadMoveDirection(newobj);
			if (movestate == DOODAD_MOVE_NONE)
			    moveobj(newobj,NULL,MODEDOODADCHANGESTATE,openstate,0,NOSHOWERROR|XYNOTCOORDS);
			else
			    AddModeMove(newobj,NULL,MODEDOODADCHANGESTATE,openstate,0,NOSHOWERROR|XYNOTCOORDS);
		    }
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_SETINVINCIBILITY://43
		    openstate = temptrg->action[i].subaction;
		    searchloc=temptrg->action[i].locationnr-1;
		    ownedactiononplayers=OneGroup_Prepare(info,temptrg->action[i].actiononplayers,playernrmask);
		    unitnr=temptrg->action[i].unitorrestype;
		    newobj=NULL;
		    nrofunits=CheckForUnit(NULL,ownedactiononplayers,unitnr,MaxObjects,&newobj,&info->gamelocations[searchloc].coords);
		    if (nrofunits)
		    {
			for (j=0;j<MaxObjects;j++)
			{
			    if (GetTriggeredUnitState(objects[j]))
			    {
				//done by previous trigger
				SetTriggeredUnitState(objects[j], CLEARSTATE);
				switch(openstate)
				{
				    case TRG_TYPEFUNC_SWITCHSET:
					SetInvincibleOBJ(objects[j],1);
					break;
				    case TRG_TYPEFUNC_SWITCHCLEAR:
					SetInvincibleOBJ(objects[j],0);
					break;
				    case TRG_TYPEFUNC_SWITCHTOGGLE:
					objects[j]->prop^=VARINVINCIBILITY;
					break;
				}
				if (--nrofunits==0)
				    break;
			    }
			}
		    }
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_CREATEUNIT://44
		    if (temptrg->action[i].TriggerEntryFlags&TRIGGERENTRYFLAG_MANYUNITS)
		    {
			nrofunits=temptrg->action[i].subaction;
			if (!nrofunits)
			    nrofunits=1;
		    }
		    else
			nrofunits=1;
creationwithoutproperties:
		    unitnr=temptrg->action[i].unitorrestype;
		    b=loadobj(unitnr);
		    locnr=temptrg->action[i].locationnr-1;
		    ownedactiononplayers=OneGroup_Prepare(info,temptrg->action[i].actiononplayers,playernrmask);
		    deltax=nrofunits;
		    for (j=0,mask=1;j<MAXPLAYERS;j++,mask<<=1)
		    {
		        if (ownedactiononplayers&mask)
		        {
			    while(nrofunits--)
			    {
				MAPREGENERATIONBIT=1;
				MAPUNITSREGENERATIONBIT=1;
//				CenterXYArea(&info->gamelocations[locnr].coords,&xobj,&yobj);
			        getcoordofnewunit(unitnr,&xobj,&yobj,&info->gamelocations[locnr].coords);
		    		newobj=createobjfulllife(xobj,yobj,unitnr,j);
				makeoneobjseeopen(newobj,loadobj(newobj->SC_Unit));
//				maketypeofmove(newobj,loadobj(newobj->SC_Unit));
			    }
			    nrofunits=deltax;
			    ownedactiononplayers&=~mask;
			    if (!ownedactiononplayers)
				break;
			}
		    }
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_ORDER://46
		    deltax=0;
		    deltay=0;
		    unitnr=temptrg->action[i].unitorrestype;
		    nrofunits=MaxObjects;
		    ownedactiononplayers=OneGroup_Prepare(info,temptrg->action[i].actiononplayers,playernrmask);
		    locnr=temptrg->action[i].locationnr-1;
//		    txtstr=getmapSTR(info,info->gamelocations[locnr].stringID-1);
		    searchloc=temptrg->action[i].rescount-1;
//		    txtstr=getmapSTR(info,info->gamelocations[searchloc].stringID-1);
		    CenterXYArea(&info->gamelocations[locnr].coords,&xobj,&yobj);
		    CenterXYArea(&info->gamelocations[searchloc].coords,&xobj2,&yobj2);
		    switch(temptrg->action[i].subaction)
		    {
			case 0:
			    state=MODEMOVE;
			    break;
			case 1:
			    state=MODEPATROL;
			    break;
			case 2:
			    state=MODEATACK;
			    break;
		    }
		    newobj=NULL;
		    nrofunits = CheckForUnit(NULL,ownedactiononplayers,unitnr,nrofunits,&newobj,&info->gamelocations[locnr].coords);
//		    nrofunits = CheckForUnit(NULL,ownedactiononplayers,unitnr,nrofunits,&newobj,&info->gamelocations[locnr].coords,MODESTOP);
		    if (nrofunits)
		    {
			for (j=0;j<MaxObjects;j++)
			{
			    if (GetTriggeredUnitState(objects[j]))
			    {
				//done by previous trigger
				SetTriggeredUnitState(objects[j], CLEARSTATE);
				//order unburrow if needed, after then move,atack,...
				deltax=0;
				deltay=0;
				if (IsAirUnit(objects[j]->SC_Unit))
				    deltay += 16;
				switch(objects[j]->SC_Unit)
				{
				    case SC_TANKSIEGEOBJ:
				    case SC_HERO_EDMUNDDUKESMOBJ:
					err = moveobj(objects[j],NULL,MODETANKNORMAL,NOSHOWERROR);
					if (err == MOVEOBJ_DONE)
					    AddModeMove(objects[j],NULL,state,xobj2+deltax,yobj2+deltay,NOSHOWERROR);
					break;
				    default:
					if (IsOBJBurrowed(objects[j]))
					{
					    err = moveobj(objects[j],NULL,MODEUNBURROW,NOSHOWERROR);
					    if (err == MOVEOBJ_DONE)
						AddModeMove(objects[j],NULL,state,xobj2+deltax,yobj2+deltay,NOSHOWERROR);
					}
					else
					{
					    moveobj(objects[j],NULL,state,xobj2+deltax,yobj2+deltay,NOSHOWERROR);
					}
					break;
				}
				if (--nrofunits==0)
				    break;
			    }
			}
		    }
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_COMMENT://47
		    TRIG_commentstr=getmapSTR(info,temptrg->action[i].stringID-1);
//		    DEBUGMESSCR("COMMENT[%s]\n",TRIG_commentstr);
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_GIVEUNITSTOPLAYER://48
		    unitnr=temptrg->action[i].unitorrestype;
		    if (temptrg->action[i].TriggerEntryFlags&TRIGGERENTRYFLAG_MANYUNITS)
		    {
			nrofunits=temptrg->action[i].subaction;
			if (!nrofunits)
			    nrofunits=MaxObjects;
		    }
		    else
			nrofunits=MaxObjects;
		    locnr=temptrg->action[i].locationnr-1;
		    ownedactiononplayers=OneGroup_Prepare(info,temptrg->action[i].rescount,playernrmask);
		    for (j=0,mask=1;j<MAXPLAYERS;j++,mask<<=1)
			if (ownedactiononplayers&mask)
			{
			    playernr=j;
			    break;
			}
		    ownedactiononplayers=OneGroup_Prepare(info,temptrg->action[i].actiononplayers,playernrmask);
		    newobj=NULL;
		    nrofunits=CheckForUnit(NULL,ownedactiononplayers,unitnr,nrofunits,&newobj,&info->gamelocations[locnr].coords);
		    if (nrofunits)
		    {
			for (j=0;j<MaxObjects;j++)
			{
			    if (GetTriggeredUnitState(objects[j]))
			    {
				//done by previous trigger
				SetTriggeredUnitState(objects[j], CLEARSTATE);
				MakeMindControl(objects[j],playernr,PLAYER[playernr].colorRACE);
				moveobj(objects[j],NULL,MODESTOP,NOSHOWERROR);
		    		SetBlinkOBJ(objects[j]);
				if (--nrofunits==0)
				    break;
    			    }
    			}
    		    }
    		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_MODIFYUNITHITPOINT://49
		    unitnr=temptrg->action[i].unitorrestype;
		    if (temptrg->action[i].TriggerEntryFlags&TRIGGERENTRYFLAG_MANYUNITS)
		    {
			nrofunits=temptrg->action[i].subaction;
			if (!nrofunits)
			    nrofunits=MaxObjects;
		    }
		    else
			nrofunits=MaxObjects;
		    locnr=temptrg->action[i].locationnr-1;
		    ownedactiononplayers=OneGroup_Prepare(info,temptrg->action[i].actiononplayers,playernrmask);
    		    state=temptrg->action[i].rescount;
		    nrofunits=CheckForUnit(NULL,ownedactiononplayers,unitnr,nrofunits,&newobj,&info->gamelocations[locnr].coords);
		    if (nrofunits)
		    {
			for (j=0;j<MaxObjects;j++)
			{
			    if (GetTriggeredUnitState(objects[j]))
			    {
				//done by previous trigger
				SetTriggeredUnitState(objects[j], CLEARSTATE);
				SetUnitPercentHealth(objects[j],state);
				//need to add fireblood images
				AddRemoveBloodFlameOverlays(objects[j]);
				if (--nrofunits==0)
				    break;
			    }
			}
		    }
    		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_MODIFYUNITENERGY://50
		    unitnr=temptrg->action[i].unitorrestype;
		    if (temptrg->action[i].TriggerEntryFlags&TRIGGERENTRYFLAG_MANYUNITS)
		    {
			nrofunits=temptrg->action[i].subaction;
			if (!nrofunits)
			    nrofunits=MaxObjects;
		    }
		    else
			nrofunits=MaxObjects;
		    locnr=temptrg->action[i].locationnr-1;
		    ownedactiononplayers=OneGroup_Prepare(info,temptrg->action[i].actiononplayers,playernrmask);
    		    state=temptrg->action[i].rescount;
		    nrofunits=CheckForUnit(NULL,ownedactiononplayers,unitnr,nrofunits,&newobj,&info->gamelocations[locnr].coords);
		    if (nrofunits)
		    {
			for (j=0;j<MaxObjects;j++)
			{
			    if (GetTriggeredUnitState(objects[j]))
			    {
				//done by previous trigger
				SetTriggeredUnitState(objects[j], CLEARSTATE);
				SetUnitPercentMana(objects[j],state);
				if (--nrofunits==0)
				    break;
			    }
			}
		    }
    		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_MODIFYUNITSHIELDPOINTS://51
		    unitnr=temptrg->action[i].unitorrestype;
		    if (temptrg->action[i].TriggerEntryFlags&TRIGGERENTRYFLAG_MANYUNITS)
		    {
			nrofunits=temptrg->action[i].subaction;
			if (!nrofunits)
			    nrofunits=MaxObjects;
		    }
		    else
			nrofunits=MaxObjects;
		    locnr=temptrg->action[i].locationnr-1;
		    ownedactiononplayers=OneGroup_Prepare(info,temptrg->action[i].actiononplayers,playernrmask);
    		    state=temptrg->action[i].rescount;
		    nrofunits=CheckForUnit(NULL,ownedactiononplayers,unitnr,nrofunits,&newobj,&info->gamelocations[locnr].coords);
		    if (nrofunits)
		    {
			for (j=0;j<MaxObjects;j++)
			{
			    if (GetTriggeredUnitState(objects[j]))
			    {
				//done by previous trigger
				SetTriggeredUnitState(objects[j], CLEARSTATE);
				SetUnitPercentShield(objects[j],state);
				if (--nrofunits==0)
				    break;
			    }
			}
		    }
    		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_PAUSETIMER://54
		    info->flags |= STARMAP_FLAG_COUNTSTOPED;
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_UNPAUSETIMER://55
		    info->flags &= ~STARMAP_FLAG_COUNTSTOPED;
		    triggset=1;
		    break;
		case TRG_ACTIONTYPE_SETALLIANCESTATUS://57
		    ownedactiononplayers=OneGroup_Prepare(info,temptrg->action[i].actiononplayers,playernrmask);
		    switch(temptrg->action[i].unitorrestype)
		    {
			case 0:
			    state=ENEMYOBJ;
			    break;
			case 1:
			    state=ALLIANCEOBJ;
			    break;
			default:
			    state=-1;
			    break;
		    }
		    if (state>=0)
		    {
			for (jj=0,mask2=1;jj<PLAYEDPLAYERS;jj++,mask2<<=1)
			    if (ownedactiononplayers & mask2)
		    	    {
		    		CreateAliance(info,playernr,jj,state);
//		    		CreateAliance(info,jj,playernr,state);//???? add
				ownedactiononplayers &= ~mask2;
				if (!ownedactiononplayers)
				    break;
		    	    }
		    }
		    triggset=1;
		    break;
		default:
		    printf("trig=%d actiontype %d for trigger unknown\n",trig_nr,temptrg->action[i].actiontype);
		    triggset=1;
		    break;
	    }//switch
	    temptrg->action[i].TriggerStateDone|=playernrmask;
	    triggcnt += triggset;
	    if (TRIG_pause)
		break;
    }//for
    return(triggcnt==TRIG_MAX_ACTIONS);
}
//=================================================
int TRG_RunAIScript(mapinfo *info,int aiscriptnr,int playernr,int playernrmask)
{
    return(TRG_RunAIScriptAtLocation(info,aiscriptnr,playernr,playernrmask,-1));
}
//=================================================
int TRG_RunAIScriptAtLocation(mapinfo *info,int aiscriptnr,int playernr,int playernrmask,int locnr)
{
    char statpl[MAXPLAYERS];
    int pl,i,j,haverescued,magenr,sfxdata_id,xpos,ypos,nrofunits,nrofunits2,dist,mindistance;
    OBJ *a,*bunkerobj,*findbunker;
    mylist *bunkerlist=NULL;
    OBJstruct *b;
    if (locnr>=0)
    {
	CenterXYArea(&info->gamelocations[locnr].coords,&xpos,&ypos);
    }
    switch(aiscriptnr)
    {
	case TRG_AISCRIPT_TURNONSHAREDVISIONPLAYER0:
	case TRG_AISCRIPT_TURNONSHAREDVISIONPLAYER1:
	case TRG_AISCRIPT_TURNONSHAREDVISIONPLAYER2:
	case TRG_AISCRIPT_TURNONSHAREDVISIONPLAYER3:
	case TRG_AISCRIPT_TURNONSHAREDVISIONPLAYER4:
	case TRG_AISCRIPT_TURNONSHAREDVISIONPLAYER5:
	case TRG_AISCRIPT_TURNONSHAREDVISIONPLAYER6:
	case TRG_AISCRIPT_TURNONSHAREDVISIONPLAYER7:
//	    DEBUGMESSCR("turn[on]sharedvision:aiscript=0x%04x\n",aiscriptnr);
	    pl= (aiscriptnr - TRG_AISCRIPT_TURNONSHAREDVISIONPLAYER0) >> 24;
	    CreateVision(info,playernr,pl,PLAYER_VISION);
	    bitsplayer=GetVisionBitsPlayer(NUMBGAMER);
	    return(1);
	case TRG_AISCRIPT_TURNOFFSHAREDVISIONPLAYER0:
	case TRG_AISCRIPT_TURNOFFSHAREDVISIONPLAYER1:
	case TRG_AISCRIPT_TURNOFFSHAREDVISIONPLAYER2:
	case TRG_AISCRIPT_TURNOFFSHAREDVISIONPLAYER3:
	case TRG_AISCRIPT_TURNOFFSHAREDVISIONPLAYER4:
	case TRG_AISCRIPT_TURNOFFSHAREDVISIONPLAYER5:
	case TRG_AISCRIPT_TURNOFFSHAREDVISIONPLAYER6:
	case TRG_AISCRIPT_TURNOFFSHAREDVISIONPLAYER7:
//	    DEBUGMESSCR("turn[off]sharedvision:aiscript=0x%04x\n",aiscriptnr);
	    pl= (aiscriptnr - TRG_AISCRIPT_TURNOFFSHAREDVISIONPLAYER0) >> 24;
	    CreateVision(info,playernr,pl,PLAYER_NOVISION);
	    bitsplayer=GetVisionBitsPlayer(NUMBGAMER);
	    return(1);
	case TRG_AISCRIPT_FILLSILOWITHNUKE_TYPEB:
//	    DEBUGMESSCR("fillsilowithnukes:aiscript=0x%04x\n",aiscriptnr);
	    a=NULL;
	    nrofunits=CheckForUnit(NULL,playernrmask,SC_NUCLEARSILOOBJ,MaxObjects,&a,&info->gamelocations[locnr].coords);
	    if (nrofunits)
	    {
		for (j=0;j<MaxObjects;j++)
		{
		    if (GetTriggeredUnitState(objects[j]))
		    {
			//done by previous trigger
			SetTriggeredUnitState(objects[j], CLEARSTATE);
			a = CreateUnitInUnit(objects[j],SC_NUKEOBJ,0,GetOBJx(objects[j]),GetOBJy(objects[j]));
			ChangeSupply(a->playernr,a->SC_Unit,PLUSFACTOR);
			if (--nrofunits==0)
			    break;
		    }
		}
	    }
	    return(1);
	case TRG_AISCRIPT_CAST_DISRUPTIONWEB://70
//	    DEBUGMESSCR("disruptionweb:aiscript=0x%04x\n",aiscriptnr);
	    magenr = MODEDISRUPTION;
	    a = createobjfulllife(xpos,ypos,SC_MISC_DISRUPTIONWEB,GREYNEUTRALCOLORPLAYER);
	    a->timeleft = mageprop[magenr].timemageactive;
	    Play_sfxdata(xpos,ypos,SFXDATA_DISRUPTIONWEB,2);
	    return(1);
	case TRG_AISCRIPT_SHOW_RECALL://101
//	    DEBUGMESSCR("recall:aiscript=0x%04x\n",aiscriptnr);
	    // need to check if we have arbiters for this players
	    a = NULL;
	    magenr = MODERECALL;
	    nrofunits2 = nrofunits = CheckForUnit(NULL,0x0fff,SC_MENUNIT,MaxObjects,&a,&info->gamelocations[locnr].coords);
	    if (nrofunits)
	    {
		for (j=0;j<MaxObjects;j++)
		{
		    if (GetTriggeredUnitState(objects[j]))
		    {
			//done by previous trigger
			SetTriggeredUnitState(objects[j], CLEARSTATE);
			AddOverlayAtrImages(objects[j],magenr,IMAGEOVERLAY_NOTDEPENDONMAINIMG);
			if (--nrofunits==0)
			    break;
		    }
		}
	    }
	    if (nrofunits != nrofunits2)
		if (mageprop[magenr].sound_id[SOUNDONHIT])
		    Play_sfxdata(xpos,ypos,mageprop[magenr].sound_id[SOUNDONHIT],2);
	    break;
	case TRG_AISCRIPT_ENTERCLOSESTBUNKER://107
//	    DEBUGMESSCR("enterclostestbunker:aiscript=0x%04x\n",aiscriptnr);
	    a = NULL;
	    nrofunits2=nrofunits=CheckForUnit(NULL,playernrmask,SC_BUNKEROBJ,MaxObjects,&a,&info->gamelocations[locnr].coords);
	    switch(nrofunits)
	    {
		case 0:
		    return(1);
		case 1:
		    //onlyone bunker
		    SetTriggeredUnitState(a, CLEARSTATE);
		    findbunker=a;
		    break;
		default:
		    findbunker=NULL;
		    bunkerlist = new mylist;
		    for (j=0;j<MaxObjects;j++)
		    {
			if (GetTriggeredUnitState(objects[j]))
			{
			    //done by previous trigger
		    	    SetTriggeredUnitState(objects[j], CLEARSTATE);
			    bunkerlist->AddList(objects[j]);
			    if (--nrofunits2==0)
				break;
			}
		    }
		    break;
	    }
	    for (j=0;j<MaxObjects;j++)
	    {
		//if terran race and is organic unit (can enter bunker)
		if (objects[j]->playernr == playernr)
		    if (IsTerranRace(objects[j]->SC_Unit)&&IsOrganic(objects[j]->SC_Unit))
		    {
			if (SearchObjInArea(objects[j],&info->gamelocations[locnr].coords))
			{
			    if (nrofunits!=1)
			    {
				//find closest bunker for unit
				findbunker=NULL;
				bunkerlist->EnumListInit();
				mindistance=256*SIZESPRLANSHX*2;
				while(bunkerobj=(struct OBJ *)bunkerlist->GetNextListElem())
				{
				    dist=hypot(GetOBJx(objects[j])-GetOBJx(bunkerobj),GetOBJy(objects[j])-GetOBJy(bunkerobj));
				    if (dist<mindistance)
				    {
					mindistance=dist;
					findbunker=bunkerobj;
				    }
				}
			    }
			    if (findbunker)
				moveobj(objects[j],findbunker,MODEMOVE,NOSHOWERROR);
			}
		    }
	    }
	    if (bunkerlist)
		delete bunkerlist;		//dealloc all elements and free bunkerlist
	    return(1);
	case TRG_AISCRIPT_SWITCHCOMPPLAYERTORESCUEDPASSIVE://117
//	    DEBUGMESSCR("swithchcompplayertorescuedpassive:aiscript=0x%04x\n",aiscriptnr);
	    haverescued=0;
	    info->pl_iowner[playernr] = OWNER_RESCUABLE;
	    ifhaverescuableplayers++;
	    applyrescuableunits();
//	    ifhaverescuableplayers--;
/*    	    for (j=0;j<MaxObjects;j++)
	    {
    		    a = objects[j];
    		    if (a->playernr != NUMBGAMER)
    		    {
//			bug in xterran7 medics with OWNER_COMPUTER do not attach to current player
			if (map.pl_iowner[a->playernr] == OWNER_RESCUABLE)
			// || map.pl_iowner[a->playernr] == OWNER_COMPUTER)
	    		{
			    MakeMindControl(a,NUMBGAMER,a->color);
			    haverescued++;
			}
		    }
	    }
	    if (haverescued)
	    {
		play_race_rescue(gameconf.pl_race[NUMBGAMER],4,0);
	    }
*/
	    return(1);
	case TRG_AISCRIPT_SETPLAYERTOENEMYHERE://121
//	    DEBUGMESSCR("setplayertoenemyhere:aiscript=0x%04x\n",aiscriptnr);
	    //in this location every unit give to player enemy status
	    for (i=0;i<MAXPLAYERS;i++)
		statpl[i]=0;
	    for (j=0;j<MaxObjects;j++)
	    {
    		a = objects[j];
		if (SearchObjInArea(a,&info->gamelocations[locnr].coords))
		{
		    if (a->playernr != playernr && a->playernr < PLAYEDPLAYERS)
		    {
			statpl[a->playernr]=1;
		    }
		}
	    }
	    for (i=0;i<MAXPLAYERS;i++)
		if (statpl[i])
		{
		    CreateAliance(info,playernr,i,ENEMYOBJ);
		    CreateAliance(info,i,playernr,ENEMYOBJ);
		}    
	    break;
	case TRG_AISCRIPT_SETPLAYERTOALLIESHERE://????
//	    DEBUGMESSCR("setplayertoenemyhere:aiscript=0x%04x\n",aiscriptnr);
	    //in this location every unit give to player enemy status
	    for (i=0;i<MAXPLAYERS;i++)
		statpl[i]=0;
	    for (j=0;j<MaxObjects;j++)
	    {
    		a = objects[j];
		if (SearchObjInArea(a,&info->gamelocations[locnr].coords))
		{
		    if (a->playernr != playernr && a->playernr < PLAYEDPLAYERS)
		    {
			statpl[a->playernr]=1;
		    }
		}
	    }
	    for (i=0;i<MAXPLAYERS;i++)
		if (statpl[i])
		{
		    CreateAliance(info,playernr,i,ALLIANCEOBJ);
		    CreateAliance(info,i,playernr,ALLIANCEOBJ);
		}    
	    break;
	case TRG_AISCRIPT_SETALLUNITSTORANDOMSUICIDEMISSION:
	    //!!!!!need to check how player is weak and go where, no  numbgamer!!!!!
	    if (SearchForUnitMask(NUMBGAMER,SPECIAL_ISBUILD,&xpos,&ypos))
	    {
		for (j=0;j<MaxObjects;j++)
		{
    		    a = objects[j];
		    if (a->playernr == playernr && a->modemove == MODESTOP)
			if (IsOBJBurrowed(objects[j]))
			{
			    if (moveobj(a,NULL,MODEUNBURROW,NOSHOWERROR) == MOVEOBJ_DONE)
				AddModeMove(a,NULL,MODEATACK,xpos,ypos,NOSHOWERROR);
			}
			else
			{
			    moveobj(a,NULL,MODEATACK,xpos,ypos,NOSHOWERROR);
			}
		}
	    }
	    break;
	default:
showdefaulterror:
	    DEBUGMESSCR("unknown aiscriptnr=0x%04x (pl=%d) at [%s]\n",aiscriptnr,playernr,TRIG_commentstr);
	    return(0);
    }
}
//=================================================
