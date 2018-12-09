
#include "scripts.h"
#include "auxil.h"
#include "player.h"
#include "maps.h"
#include "gener.h"
#include "starmap.h"
#include "messages.h"
#include "lists.h"
#include "triggers.h"
#include "audio.h"
#include "portrait.h"
#include "load.h"
#include "wmem.h"
#include "pcx.h"
#include "rand.h"
#include "vars.h"
#include "briefing.h"

TICKCOUNTER		*brief_counter;
int				brief_pauseticks;
signed char		brief_pauseslot;
int				brief_delta;
int				soundid=-1;
PCX				*framepcxs[8];
BRIEF_SLOTS		brief_slots[MAX_BRIEF_SLOTS];
#define			BRIEFTEXT		15
#define			BRIEFMISSION	16
//=================================================
void First_Briefing_Prepare(struct mapinfo *info, int e, char *path, PCX *pcxs)
{
	int i;
	MAP_TRIGS	*temptrg;
	MAP_TRIGS(*alltrigs)[] = (MAP_TRIGS(*)[]) info->BRIEFS;
	Briefing_SetPause(-1, 0);
	brief_delta = e;
	prefix_campaignpath = path;
	soundid = -1;
	for (i = 0;i < info->BRIEFS_NR;i++)
	{
		temptrg = &(*alltrigs)[i];
		temptrg->groupinfo.playersmask = AllGroups_Prepare(info, temptrg);
	}
	//	  for (i=0;i<MAX_BRIEF_SLOTS;i++)
	//		Brief_ShowPortrait(i,PORTRAIT_NONE);
	memset(brief_slots, 0, sizeof(brief_slots));
	for (i = 0;i < 8;i++)
		framepcxs[i] = &pcxs[i];
	brief_counter = mytimer.CreateTickCounter();
}
//=================================================
void Briefing_Parse(struct mapinfo *info, MENUSTR *allmenus, int deltatick)
{
	int pause;
	TIMER_TICK tick;
	int i;
	MAP_TRIGS	*temptrg;
	MAP_TRIGS(*alltrigs)[] = (MAP_TRIGS(*)[]) info->BRIEFS;
	pause = Briefing_GetPause();
	tick = pause - mytimer.GetDeltaCounter(brief_counter) * 1000 / TICKS_RES;
	if (tick <= 0)
	{
		if (pause && brief_pauseslot != -1)
		{
			StopTransmission(allmenus, brief_pauseslot);
		}
		Briefing_SetPause(-1, 0);
		for (i = 0;i < info->BRIEFS_NR;i++)
		{
			temptrg = &(*alltrigs)[i];
			if (temptrg->groupinfo.playersmask)
				BCondition_Prepare(info, temptrg, temptrg->groupinfo.playersmask, allmenus);
		}
	}
	else
		Briefing_SetPause(brief_pauseslot, tick);
	Briefing_ScrollText(allmenus);
	for (i = 0;i < MAX_BRIEF_SLOTS;i++)
		drawportraitinslot(allmenus, i);
}
//=================================================
int BCondition_Prepare(mapinfo *info,MAP_TRIGS *temptrg,int actiononplayers,MENUSTR *allmenus)
{
	int i,applycond,allcond;
	if (!Briefing_GetPause())
	{
		for (i = 0;i < TRIG_MAX_CONDITIONS;i++)
		{
			applycond = 0;
			allcond = 0;
			if (temptrg->condition[i].SwitchState&(TRIGGERCONDITIONSTATE_CHECKED | TRIGGERCONDITIONSTATE_DISABLED))
				break;	//ignore this trigger
			if (temptrg->condition[i].conditiontype == TRG_CONDITIONTYPE_NONE)
				return(0);//ignore all next triggers
			switch (temptrg->condition[i].conditiontype)
			{
			case TRG_CONDITIONTYPE_MISSIONBRIEFING:
				applycond = 1;
				break;
			default:
				//					printf("br conditiontype %d for trigger unknown\n",temptrg->condition[i].conditiontype);
				break;
			}
			if (applycond)
				allcond = BAction_Prepare(info, temptrg, actiononplayers, i, allmenus);
			if (allcond)//prevent second trigger action
				temptrg->condition[i].SwitchState |= TRIGGERCONDITIONSTATE_CHECKED;
			if (Briefing_GetPause())
				return(0);
		}
	}
	return(0);//no conditions
}
//=================================================
int BAction_Prepare(mapinfo *info,MAP_TRIGS *temptrg,int actiononplayers,int condnr,MENUSTR *allmenus)
{
	int err,i,triggcnt,triggset,needbreakparse=0;
	int sx,sy,maxysymbsize,maxlines,fontnr,playtime;
	int slotnr,waittime,textid,waveid,maxlinepixels,deltapaused;
	char *txtstr;
	int (*comparefunc)(int *,int );
	err=0;
	triggcnt=0;
	soundid=-1;
	for (i = 0;i < TRIG_MAX_ACTIONS;i++)
	{
		triggset = 0;
		if (temptrg->action[i].TriggerEntryFlags & (TRIGGERENTRYFLAG_STATEDISABLED | TRIGGERENTRYFLAG_EXECUTED))
		{
			triggset = 1;
			triggcnt += triggset;
			continue;		//ignore this trigger
		}
		//			printf("actiontype %d\n",temptrg->action[i].actiontype);
		slotnr = temptrg->action[i].actiononplayers;
		switch (temptrg->action[i].actiontype)
		{
		case BTRG_ACTIONTYPE_NONE://0
			triggcnt += TRIG_MAX_ACTIONS - i;
			return(triggcnt == TRIG_MAX_ACTIONS);
		case BTRG_ACTIONTYPE_PAUSESOMETIME://1
			triggset = 1;
			waittime = temptrg->action[i].pauseatime;
			Briefing_SetPause(-1, waittime);
			needbreakparse = 1;
			break;
		case BTRG_ACTIONTYPE_PLAYWAV://2
			waveid = temptrg->action[i].waveID - 1;
			FULLFILENAME[0] = 0;
			if (prefix_campaignpath)
			{
				strcpy(FULLFILENAME, prefix_campaignpath);
				strcat(FULLFILENAME, "\\");
			}
			strcat(FULLFILENAME, getmapSTR(info, waveid));
			soundid = loadandplaywav(info->mpqid, NULL, FULLFILENAME, 1, 0);
			if (soundid >= 0)
			{
				brief_slots[slotnr].talkingtime = getchannelplaylength(soundid);
				Briefing_SetPause(-1, brief_slots[slotnr].talkingtime);
			}
			triggset = 1;
			needbreakparse = 1;
			break;
		case BTRG_ACTIONTYPE_DISPLAYBRIEFTEXT://3
			textid = temptrg->action[i].stringID - 1;
			txtstr = getmapSTR(info, textid);
			waittime = temptrg->action[i].pauseatime;
			Briefing_SetPause(-1, waittime);
			//					setmenuitem_VISIBLED(allmenus,brief_delta+BRIEFTEXT,FALSE);
			//					setmenuitem_VISIBLED(allmenus,brief_delta+18,TRUE);
			setmenuitem_VISIBLED(allmenus, brief_delta + BRIEFTEXT, TRUE);
			setmenuitem_DISABLED(allmenus, brief_delta + BRIEFTEXT, FALSE);
			changetextitem(allmenus, brief_delta + BRIEFTEXT, txtstr);
			Play_sfxdata_id(NULL, SFXDATA_TRANSMISSION, 1, 0);
			triggset = 1;
			needbreakparse = 1;
			fontnr = allmenus->menu[brief_delta + BRIEFTEXT].fontnr;
			sx = allmenus->menu[brief_delta + BRIEFTEXT].hotxsize;
			sy = allmenus->menu[brief_delta + BRIEFTEXT].hotysize;
			getmaxsymbolsize(fontnr, NULL, &maxysymbsize);
			maxlines = getlinesintext(fontnr, txtstr, strlen(txtstr), sx) + 1;
			maxlinepixels = maxlines * maxysymbsize;
			maxlinepixels -= sy;
			if (maxlinepixels < 0)
				maxlinepixels = 0;
			brief_slots[slotnr].skippixels = 0;
			brief_slots[slotnr].maxskippixels = -maxlinepixels;
			brief_slots[slotnr].command = PORTRAIT_NONE_MESSAGESCROLL;
			break;
		case BTRG_ACTIONTYPE_MISSIONOBJ://4
			textid = temptrg->action[i].stringID - 1;
			txtstr = getmapSTR(info, textid);
			fill_missionobjectives(txtstr);
			changetextitem(allmenus, brief_delta + BRIEFMISSION, txtstr);
			setmenuitem_VISIBLED(allmenus, brief_delta + BRIEFMISSION, TRUE);
			triggset = 1;
			break;
		case BTRG_ACTIONTYPE_SHOWPORTRAIT://5
			brief_slots[slotnr].unitid = temptrg->action[i].unitorrestype;
			if (brief_slots[slotnr].portraits)
			{
				//						wfree(brief_slots[slotnr].portraits);
				FreeSMKInfo(brief_slots[slotnr].portraits);
			}
			brief_slots[slotnr].portraits = FillSMKInfo(brief_slots[slotnr].unitid);
			brief_slots[slotnr].command = PORTRAIT_APPEAR;
			Brief_ShowPortrait(slotnr, brief_slots[slotnr].command);
			triggset = 1;
			break;
		case BTRG_ACTIONTYPE_HIDEPORTRAIT://6
			brief_slots[slotnr].command = PORTRAIT_DISAPPEAR;
			Brief_ShowPortrait(slotnr, brief_slots[slotnr].command);
			triggset = 1;
			break;
		case BTRG_ACTIONTYPE_SPEAKINGANIMATION://7
			brief_slots[slotnr].command = PORTRAIT_TALKING;
			brief_slots[slotnr].talkingtime = temptrg->action[i].pauseatime;
			Briefing_SetPause(slotnr, brief_slots[slotnr].talkingtime);
			triggset = 1;
			needbreakparse = 1;
			break;
		case BTRG_ACTIONTYPE_TRANSMISSION://8
			deltapaused = temptrg->action[i].rescount;
			comparefunc = ConditionFunctions[temptrg->action[i].subaction];

			brief_slots[slotnr].command = PORTRAIT_TALKING;
			textid = temptrg->action[i].stringID - 1;
			txtstr = getmapSTR(info, textid);
			waveid = temptrg->action[i].waveID - 1;
			setmenuitem_VISIBLED(allmenus, brief_delta + BRIEFTEXT, TRUE);
			changetextitem(allmenus, brief_delta + BRIEFTEXT, txtstr);
			fontnr = allmenus->menu[brief_delta + BRIEFTEXT].fontnr;
			sx = allmenus->menu[brief_delta + BRIEFTEXT].hotxsize;
			sy = allmenus->menu[brief_delta + BRIEFTEXT].hotysize;
			getmaxsymbolsize(fontnr, NULL, &maxysymbsize);
			maxlines = getlinesintext(fontnr, txtstr, strlen(txtstr), sx) + 1;
			maxlinepixels = maxlines * maxysymbsize;
			maxlinepixels -= sy;
			if (maxlinepixels < 0)
				maxlinepixels = 0;
			brief_slots[slotnr].skippixels = 40;
			brief_slots[slotnr].maxskippixels = -maxlinepixels;
			brief_slots[slotnr].command = PORTRAIT_TALKING;
			Brief_ShowPortrait(slotnr, brief_slots[slotnr].command);
			FULLFILENAME[0] = 0;
			if (prefix_campaignpath)
			{
				strcpy(FULLFILENAME, prefix_campaignpath);
				strcat(FULLFILENAME, "\\");
			}
			strcat(FULLFILENAME, getmapSTR(info, waveid));
			soundid = loadandplaywav(info->mpqid, NULL, FULLFILENAME, 1, 0);
			//if (soundid >= 0)
			{
				playtime = temptrg->action[i].pauseatime;
				(*comparefunc)(&playtime, deltapaused);
				brief_slots[slotnr].talkingtime = playtime;
				Briefing_SetPause(slotnr, brief_slots[slotnr].talkingtime);
			}
			//					printf("playtime = %d talkingtime = %d\n",playtime,brief_slots[slotnr].talkingtime);
			triggset = 1;
			needbreakparse = 1;
			break;
		case BTRG_ACTIONTYPE_UNUSED://9
			triggset = 1;
			break;
		default:
			printf("br actiontype %d for trigger unknown\n", temptrg->action[i].actiontype);
			triggset = 1;
			break;
		}
		if (triggset)
		{
			temptrg->action[i].TriggerEntryFlags |= TRIGGERENTRYFLAG_EXECUTED;
			triggcnt += triggset;
		}
		if (needbreakparse)
			return(0);
	}
	return(triggcnt==TRIG_MAX_ACTIONS);
}
//=================================================
void Remove_Briefing(mapinfo *info)
{

	if (info && info->BRIEFS)
	{
		wfree(info->BRIEFS);
		info->BRIEFS=NULL;
		info->BRIEFS_NR=0;
	}
	if (soundid>=0)
	{
		StopPlayChannel(soundid);
		soundid=-1;
	}
	for (int i=0;i<MAX_BRIEF_SLOTS;i++)
	{
		Brief_ShowPortrait(i,PORTRAIT_NONE);
		if (brief_slots[i].portraits)
			FreeSMKInfo(brief_slots[i].portraits);
//			wfree(brief_slots[i].portraits);
	}
	memset(brief_slots,0,sizeof(brief_slots));
	if (brief_counter)
	{
		mytimer.DestroyTickCounter(brief_counter);
		brief_counter=NULL;
	}
}
//=================================================
int Briefing_GetPause(void)
{
	return(brief_pauseticks);
}
//=================================================
void Briefing_SetPause(int slotnr,int pauseval)
{
	brief_pauseslot=slotnr;
	brief_pauseticks=pauseval;
}
//=================================================
void Briefing_ScrollText(MENUSTR *allmenus)
{
	int i,skip;
	for (i=0;i<MAX_BRIEF_SLOTS;i++)
	{
		if (brief_slots[i].command == PORTRAIT_TALKING || brief_slots[i].command == PORTRAIT_NONE_MESSAGESCROLL)
		{
			brief_slots[i].skippixels--;
			if (brief_slots[i].skippixels < brief_slots[i].maxskippixels)
				brief_slots[i].skippixels = brief_slots[i].maxskippixels;
			if (brief_slots[i].skippixels > 0)
				skip=0;
			else
				skip=brief_slots[i].skippixels/BRIEFSCROLLTEXTSPEED;
			changetextskiplinepixels(allmenus,brief_delta+BRIEFTEXT,skip);
		}
	}
}
//=================================================
void StopTransmission(MENUSTR *allmenus,int slotnr)
{
	switch (brief_slots[slotnr].command)
	{
	case PORTRAIT_TALKING:
		brief_slots[slotnr].command = PORTRAIT_IDLE;
		Brief_ShowPortrait(slotnr, brief_slots[slotnr].command);
		setmenuitem_VISIBLED(allmenus, brief_delta + BRIEFTEXT, FALSE);
		break;
	}
}
//=================================================
void Reload_Briefing(struct mapinfo *info,MENUSTR *allmenus)
{
	int i,j;
	MAP_TRIGS	*temptrg;
	MAP_TRIGS	(*alltrigs)[]=(MAP_TRIGS (*)[]) info->BRIEFS;
	Briefing_SetPause(-1,0);
	for (i = 0;i < TRIG_MAX_CONDITIONS;i++)
	{
		temptrg = &(*alltrigs)[i];
		if (temptrg->condition[i].conditiontype == TRG_CONDITIONTYPE_NONE)
			break;
		temptrg->condition[i].SwitchState &= ~TRIGGERCONDITIONSTATE_CHECKED;
		for (j = 0;j < TRIG_MAX_ACTIONS;j++)
		{
			if (temptrg->action[j].actiontype == BTRG_ACTIONTYPE_NONE)
				break;
			temptrg->action[j].TriggerEntryFlags &= ~TRIGGERENTRYFLAG_EXECUTED;
		}
	}
	if (soundid>=0)
	{
		StopPlayChannel(soundid);
		soundid=-1;
	}
	for (i=0;i<MAX_BRIEF_SLOTS;i++)
		Brief_ShowPortrait(i,PORTRAIT_NONE);
	memset(brief_slots,0,sizeof(brief_slots));
	setmenuitem_VISIBLED(allmenus,brief_delta+BRIEFTEXT,FALSE);
	setmenuitem_VISIBLED(allmenus,brief_delta+BRIEFMISSION,FALSE);
	setmenuitem_VISIBLED(allmenus,brief_delta+18,FALSE);
	Briefing_SetPause(-1,1000);
}
//=================================================
void drawportraitinslot(MENUSTR *allmenus,int slotnr)
{
	int menunr=brief_delta+11+slotnr;
	int hlframe=0;
	if (brief_slots[slotnr].drawsmk)
	{
		setmenuitem_VISIBLED(allmenus, menunr, TRUE);
		if (brief_slots[slotnr].typeofport == SMKTALK)
			hlframe = 4;
		mixbriefframes(allmenus, menunr, slotnr, slotnr + hlframe, (char *)brief_slots[slotnr].smkplay.vidbuff);
		if (smk_next(brief_slots[slotnr].smkplay.smk, SMK_FULL_DECODE) == SMK_DONE)
		{
			switch (brief_slots[slotnr].typeofport)
			{
			case SMKIN:
				Brief_ShowPortrait(slotnr, PORTRAIT_IDLE);
				return;
			case SMKNORMAL:
				Brief_ShowPortrait(slotnr, PORTRAIT_IDLE);
				break;
			case SMKTALK:
				Brief_ShowPortrait(slotnr, PORTRAIT_TALKING);
				break;
			case SMKOUT:
				Brief_ShowPortrait(slotnr, PORTRAIT_NONE);
				return;
			}
			smk_first(brief_slots[slotnr].smkplay.smk, SMK_FULL_DECODE);
		}
		brief_slots[slotnr].smkplay.vidbuff = smk_get_video(brief_slots[slotnr].smkplay.smk);
	}
	else
	{
		setmenuitem_VISIBLED(allmenus,menunr,FALSE);
	}
}
//===================================
#define DRAWFRAMEFROMX	6
#define DRAWFRAMEFROMY	6
//===================================
void mixbriefframes(MENUSTR *allmenus,int menunr,int slotnr,int framenr,char *pixbuff)
{
	int i,j,jj,sx,sy,offset;
	char *newbufpixels;

	sx=framepcxs[framenr]->xsizePcx();
	sy=framepcxs[framenr]->ysizePcx();
	newbufpixels=(char *)wmalloc(sx*sy);
	offset=DRAWFRAMEFROMY*sx;

	memcpy(newbufpixels,framepcxs[framenr]->GetPcxRawBytes(),sx*sy);
	for (i = DRAWFRAMEFROMY;i < sy - DRAWFRAMEFROMY;i++)
	{
		if (i & 1)
		{
			//interlace line
		}
		else
		{
			for (j = DRAWFRAMEFROMX, jj = DRAWFRAMEFROMX / 2;j < sx - DRAWFRAMEFROMX;j++)
			{
				if (!newbufpixels[offset + j])
				{
					newbufpixels[offset + j + 0] = pixbuff[jj];
					newbufpixels[offset + j + 1] = pixbuff[jj];
					j++;
					jj++;
				}
			}
			pixbuff += SMKSIZEPORTRAITX;
		}
		offset += sx;
	}

	updateimageitem(allmenus,menunr,newbufpixels);
	wfree(newbufpixels);
}
//===================================
void Brief_SetPortrait(int slotnr,int typeofport)
{
	int smkid;
	switch (typeofport)
	{
	case SMKIN:
		brief_slots[slotnr].smkplay.smkfile.hmpq = smkin.hmpq;
		brief_slots[slotnr].smkplay.smkfile.mpqfilenr = smkin.mpqfilenr;
		break;
	case SMKOUT:
		brief_slots[slotnr].smkplay.smkfile.hmpq = smkout.hmpq;
		brief_slots[slotnr].smkplay.smkfile.mpqfilenr = smkout.mpqfilenr;
		break;
	case SMKNORMAL:
		if (brief_slots[slotnr].portraits)
		{
			if (brief_slots[slotnr].nrof_smks)
			{
				brief_slots[slotnr].smkplay.smkfile.hmpq = brief_slots[slotnr].portraits->hmpq;
				smkid = brief_slots[slotnr].idlesmks[brief_slots[slotnr].cursmk];
				if (smkid <= 0)
					brief_slots[slotnr].smkplay.smkfile.mpqfilenr = brief_slots[slotnr].portraits->mpqfilenr[SMKNORMAL][0];
				else
					brief_slots[slotnr].smkplay.smkfile.mpqfilenr = brief_slots[slotnr].portraits->mpqfilenr[SMKNORMAL2][smkid];
			}
		}
		break;
	default:
		if (brief_slots[slotnr].portraits)
		{
			if (brief_slots[slotnr].nrof_smks)
			{
				brief_slots[slotnr].smkplay.smkfile.hmpq = brief_slots[slotnr].portraits->hmpq;
				brief_slots[slotnr].smkplay.smkfile.mpqfilenr = brief_slots[slotnr].portraits->mpqfilenr[typeofport][brief_slots[slotnr].cursmk];
			}
		}
		break;
	}
	loadSMK(&brief_slots[slotnr].smkplay,SMK_FULL_DECODE);
}
//===================================
void Brief_ShowPortrait(int slotnr,int typeofportrait)
{
/*	  if (typeofportrait)
	{
		printf("show portrait slotnr=%d, typeof portrait=%d\n",slotnr,typeofportrait);
	}
*/
	int i,j,jj;
	unloadSMK(&brief_slots[slotnr].smkplay);
	memset(&brief_slots[slotnr].smkplay,0,sizeof(brief_slots[slotnr].smkplay));
	switch (typeofportrait)
	{
	case PORTRAIT_NONE:
		brief_slots[slotnr].typeofport = SMKNONE;
		memset(&brief_slots[slotnr].smkplay, 0, sizeof(brief_slots[slotnr].smkplay));
		brief_slots[slotnr].nrof_smks = 0;
		brief_slots[slotnr].cursmk = 0;
		brief_slots[slotnr].drawsmk = 0;
		break;
	case PORTRAIT_APPEAR:
		brief_slots[slotnr].typeofport = SMKIN;
		brief_slots[slotnr].cursmk = 0;
		brief_slots[slotnr].nrof_smks = 1;
		Brief_SetPortrait(slotnr, brief_slots[slotnr].typeofport);
		brief_slots[slotnr].drawsmk = 1;
		brief_slots[slotnr].idlesmks[0] = -2;
		break;
	case PORTRAIT_IDLE:
		if (brief_slots[slotnr].typeofport != SMKNORMAL)
		{
			brief_slots[slotnr].typeofport = SMKNORMAL;
			brief_slots[slotnr].cursmk = 0;
			brief_slots[slotnr].nrof_smks = MAXIDLESEQUENCES;
			if (brief_slots[slotnr].idlesmks[0] == -2)
			{
				memset(brief_slots[slotnr].idlesmks, -1, MAXIDLESEQUENCES);
				for (i = 0, j = 0, jj = 0;i < MAXIDLESEQUENCES && jj < 3;i++)
				{
					if (!myrand(4))
					{
						jj++;
						brief_slots[slotnr].idlesmks[i] = j++;
						if (j >= brief_slots[slotnr].portraits->max[SMKNORMAL2])
							j = 0;
					}
				}
			}
		}
		else
		{
			if (++brief_slots[slotnr].cursmk == MAXIDLESEQUENCES)
				brief_slots[slotnr].cursmk = 0;
		}
		Brief_SetPortrait(slotnr, brief_slots[slotnr].typeofport);
		break;
	case PORTRAIT_TALKING:
		if (brief_slots[slotnr].typeofport != SMKTALK)
		{
			brief_slots[slotnr].typeofport = SMKTALK;
			brief_slots[slotnr].cursmk = 0;
			brief_slots[slotnr].nrof_smks = brief_slots[slotnr].portraits->max[brief_slots[slotnr].typeofport];
		}
		else
		{
			if (++brief_slots[slotnr].cursmk == brief_slots[slotnr].nrof_smks)
				brief_slots[slotnr].cursmk = 0;
		}
		Brief_SetPortrait(slotnr, brief_slots[slotnr].typeofport);
		break;
	case PORTRAIT_DISAPPEAR:
		brief_slots[slotnr].typeofport = SMKOUT;
		brief_slots[slotnr].cursmk = 0;
		brief_slots[slotnr].nrof_smks = 1;
		Brief_SetPortrait(slotnr, brief_slots[slotnr].typeofport);
		break;
	}
}
//===================================
