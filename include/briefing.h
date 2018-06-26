#ifndef _BRIEFING_W
#define _BRIEFING_W

#include "portrait.h"
#include "gamemenu.h"
#include "starmap.h"
//=====================================================

#define TRG_CONDITIONTYPE_NONE			0
#define TRG_CONDITIONTYPE_COUNTDOWNTIMER	1
#define TRG_CONDITIONTYPE_COMMAND		2
#define TRG_CONDITIONTYPE_BRING			3
#define TRG_CONDITIONTYPE_ACCUMULATE		4
#define TRG_CONDITIONTYPE_KILL			5
#define TRG_CONDITIONTYPE_COMMANDTHEMOST	6
#define TRG_CONDITIONTYPE_COMMANDSTHEMOSTAT	7
#define TRG_CONDITIONTYPE_MOSTKILLS		8
#define TRG_CONDITIONTYPE_HIGHESTSCORE		9
#define TRG_CONDITIONTYPE_MOSTRESOURCES		10
#define TRG_CONDITIONTYPE_SWITCH		11
#define TRG_CONDITIONTYPE_ELAPSEDTIME		12
#define TRG_CONDITIONTYPE_MISSIONBRIEFING	13
#define TRG_CONDITIONTYPE_OPPONENTS		14
#define TRG_CONDITIONTYPE_DEATHS		15
#define TRG_CONDITIONTYPE_COMMANDTHELEAST	16
#define TRG_CONDITIONTYPE_COMMANDTHELEASTAT	17
#define TRG_CONDITIONTYPE_LEASTKILLS		18
#define TRG_CONDITIONTYPE_LOWESTSCORE		19
#define TRG_CONDITIONTYPE_LEASTRESOURCES	20
#define TRG_CONDITIONTYPE_SCORE			21
#define TRG_CONDITIONTYPE_ALWAYS		22
#define TRG_CONDITIONTYPE_NEVER			23

#define BTRG_ACTIONTYPE_NONE			0
#define BTRG_ACTIONTYPE_PAUSESOMETIME		1
#define BTRG_ACTIONTYPE_PLAYWAV			2
#define BTRG_ACTIONTYPE_DISPLAYBRIEFTEXT	3
#define BTRG_ACTIONTYPE_MISSIONOBJ		4
#define BTRG_ACTIONTYPE_SHOWPORTRAIT		5
#define BTRG_ACTIONTYPE_HIDEPORTRAIT		6
#define BTRG_ACTIONTYPE_SPEAKINGANIMATION	7
#define BTRG_ACTIONTYPE_TRANSMISSION		8
#define BTRG_ACTIONTYPE_UNUSED			9

void First_Briefing_Prepare(struct mapinfo *info,int brief_menusdelta,char *path,PCX *framepcxs);
void Briefing_Parce(struct mapinfo *info,struct MENUSTR *allmenus,int deltatick);
void Remove_Briefing(struct mapinfo *info);

int  BCondition_Prepare(struct mapinfo *info,MAP_TRIGS *temptrg,int actiononplayers,struct MENUSTR *allmenus);
int  BAction_Prepare(struct mapinfo *info,MAP_TRIGS *temptrg,int actiononplayers,int condnr,struct MENUSTR *allmenus);
int  Briefing_GetPause(void);
void Briefing_SetPause(int slotnr,int pauseval);
void Briefing_ScrollText(struct MENUSTR *allmenus);
void StopTransmission(struct MENUSTR *allmenus,int slotnr);
void Reload_Briefing(struct mapinfo *info,struct MENUSTR *allmenus);
void drawportraitinslot(struct MENUSTR *allmenus,int slotnr);
int  Brief_SetPortrait(int slotnr,int typeofport);
void Brief_ShowPortrait(int slotnr,int typeofportrait);
void mixbriefframes(struct MENUSTR *allmenus,int menunr,int slotnr,int framenr,char *pixbuff);


#define PORTRAIT_NONE 			0
#define PORTRAIT_APPEAR 		1
#define PORTRAIT_IDLE 			2
#define PORTRAIT_TALKING 		3
#define PORTRAIT_DISAPPEAR 		4
#define PORTRAIT_NONE_MESSAGESCROLL	5

#define MAX_BRIEF_SLOTS		4

#define BRIEFSCROLLTEXTSPEED	1

#define MAXIDLESEQUENCES	12

struct BRIEF_SLOTS
{
    unsigned char	unitid;		//portrait of unitid;
    unsigned char	command;	//portrait type
    int			talkingtime;	//
    int			skippixels;	//
    int 		maxskippixels;

    char 		drawsmk;
    char		typeofport;
    char		cursmk;
    char		nrof_smks;
    struct SMKPLAY	smkplay;
    unsigned char 	*mixbuff;		//mixing buffer
    SMKPORTRAITS	*portraits;
    signed char		idlesmks[MAXIDLESEQUENCES];

};

#endif /*BRIEFING_H*/

