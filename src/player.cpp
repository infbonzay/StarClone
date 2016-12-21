
#include "starmap.h"
#include "gener.h"
#include "player.h"

struct PLAYGAMER curplayer[MAXPLAYERS];
//=============================================
void getplayerinfo(char *nickname)
{
    
}
//=============================================
void clearplayernames(void)
{
    for (int i=0;i<MAXPLAYERS;i++)
	curplayer[i].nickname[0]=0;
}
//=============================================
void setplayername(int playernr,char *playername)
{
    strncpy(curplayer[playernr].nickname,playername,MAXPLAYERNAMESYMBOLS);
}
//=============================================
char *getplayername(int playernr)
{
    return(curplayer[playernr].nickname);
}
//=============================================
void resetinfoplayers(void)
{
    int i;
    for (i=0;i<MAXPLAYERS;i++)
    {
	PLAYER[i].isobserverflag=0;
	PLAYER[i].minerals=0;
	PLAYER[i].gas=0;
	PLAYER[i].nukes=0;
	PLAYER[i].sendmessageto=MESSAGE_SENDMESSAGETOEVERYONE;
	PLAYER[i].messagetoplayernr=0;
	PLAYER[i].pausedgame=SETMAXPAUSE;
	memset(&PLAYER[i].statplayer,0,sizeof(STATPLAYER));
    }
}
//=============================================
void CreateDefaultInfoForPlayer(FILE *f)
{
    curplayer[0].missions[STAR_ZERG_CAMPAIGN].campaigndone=0;
    curplayer[0].missions[STAR_ZERG_CAMPAIGN].fromlineinhisttbl=31;
    curplayer[0].missions[STAR_ZERG_CAMPAIGN].nrlinesinhisttbl=14;
    memcpy(curplayer[0].missions[STAR_ZERG_CAMPAIGN].seq_missions,DEFAULT_ZERG_MISSION,MAXMISSIONSVIDEOS);
    memcpy(curplayer[0].missions[STAR_ZERG_CAMPAIGN].next_missions,DEFAULT_ZERG_NEXT,MAXMISSIONSVIDEOS);
    curplayer[0].missions[STAR_TERRAN_CAMPAIGN].campaigndone=0;
    curplayer[0].missions[STAR_TERRAN_CAMPAIGN].fromlineinhisttbl=0;
    curplayer[0].missions[STAR_TERRAN_CAMPAIGN].nrlinesinhisttbl=15;
    memcpy(curplayer[0].missions[STAR_TERRAN_CAMPAIGN].seq_missions,DEFAULT_TERRAN_MISSION,MAXMISSIONSVIDEOS);
    memcpy(curplayer[0].missions[STAR_TERRAN_CAMPAIGN].next_missions,DEFAULT_TERRAN_NEXT,MAXMISSIONSVIDEOS);
    curplayer[0].missions[STAR_PROTOSS_CAMPAIGN].campaigndone=0;
    curplayer[0].missions[STAR_PROTOSS_CAMPAIGN].fromlineinhisttbl=16;
    curplayer[0].missions[STAR_PROTOSS_CAMPAIGN].nrlinesinhisttbl=15;
    memcpy(curplayer[0].missions[STAR_PROTOSS_CAMPAIGN].seq_missions,DEFAULT_PROTOSS_MISSION,MAXMISSIONSVIDEOS);
    memcpy(curplayer[0].missions[STAR_PROTOSS_CAMPAIGN].next_missions,DEFAULT_PROTOSS_NEXT,MAXMISSIONSVIDEOS);

    curplayer[0].missions[BROOD_ZERG_CAMPAIGN].campaigndone=0;
    curplayer[0].missions[BROOD_ZERG_CAMPAIGN].fromlineinhisttbl=73;
    curplayer[0].missions[BROOD_ZERG_CAMPAIGN].nrlinesinhisttbl=18;
    memcpy(curplayer[0].missions[BROOD_ZERG_CAMPAIGN].seq_missions,DEFAULT_XZERG_MISSION,MAXMISSIONSVIDEOS);
    memcpy(curplayer[0].missions[BROOD_ZERG_CAMPAIGN].next_missions,DEFAULT_XZERG_NEXT,MAXMISSIONSVIDEOS);
    curplayer[0].missions[BROOD_TERRAN_CAMPAIGN].campaigndone=0;
    curplayer[0].missions[BROOD_TERRAN_CAMPAIGN].fromlineinhisttbl=64;
    curplayer[0].missions[BROOD_TERRAN_CAMPAIGN].nrlinesinhisttbl=10;
    memcpy(curplayer[0].missions[BROOD_TERRAN_CAMPAIGN].seq_missions,DEFAULT_XTERRAN_MISSION,MAXMISSIONSVIDEOS);
    memcpy(curplayer[0].missions[BROOD_TERRAN_CAMPAIGN].next_missions,DEFAULT_XTERRAN_NEXT,MAXMISSIONSVIDEOS);
    curplayer[0].missions[BROOD_PROTOSS_CAMPAIGN].campaigndone=0;
    curplayer[0].missions[BROOD_PROTOSS_CAMPAIGN].fromlineinhisttbl=56;
    curplayer[0].missions[BROOD_PROTOSS_CAMPAIGN].nrlinesinhisttbl=9;
    memcpy(curplayer[0].missions[BROOD_PROTOSS_CAMPAIGN].seq_missions,DEFAULT_XPROTOSS_MISSION,MAXMISSIONSVIDEOS);
    memcpy(curplayer[0].missions[BROOD_PROTOSS_CAMPAIGN].next_missions,DEFAULT_XPROTOSS_NEXT,MAXMISSIONSVIDEOS);
    fwrite(&curplayer[0].missions,1,sizeof(struct MISSIONHIST)*TOTALCAMPAIGNS,f);
}
//=============================================
char lastplayerfile[1024];
int loadandsetplayerinfo(char *player_filename)
{
    FILE *f=fopen(player_filename,"r");
    if (f)
    {
	strcpy(lastplayerfile,player_filename);
	fread(&curplayer[0].missions,1,sizeof(struct MISSIONHIST)*TOTALCAMPAIGNS,f);
	fclose(f);
	return(0);
    }
    else
    {
	return(-1);
    }
}
//=============================================
void saveplayerinfo(void)
{
    FILE *f=fopen(lastplayerfile,"w");
    if (f)
    {
	fwrite(&curplayer[0].missions,1,sizeof(struct MISSIONHIST)*TOTALCAMPAIGNS,f);
	fclose(f);
    }
}
//=============================================
int SortPlayers(int gamenr)
{
    int i,j,retplayer=-1;
    SERVERGAMER=-1;
    for (i=0,j=0;j<MAXPLAYERS;i++,j++)
    {
	if (gameconf.pl_owner[j]==OWNER_FORCENAME)
	    j++;
	if (SERVERGAMER==-1)
	    SERVERGAMER=i;
	if ( i!=j )
	{
	    gameconf.pl_owner[i]=gameconf.pl_owner[j];
	    gameconf.pl_race[i]=gameconf.pl_race[j];
	    setplayername(i,getplayername(j));
	    copyplayerinfo(i,j);
	}
	if (j==gamenr)
	    retplayer = i;
    }
    if (force_slots.networkplayers)
    {
	force_slots.bitsnetwplayers=0;
	for (i=0;i<MAXPLAYERS;i++)
	{
    	    if (gameconf.pl_owner[i]==OWNER_HUMAN || gameconf.pl_race[i]==RACE_OBSERVER)
	    {
    		force_slots.bitsnetwplayers|=(1<<i);
	    }
	}
    }
    return(retplayer);
}
//=============================================
void copyplayerinfo(int pl1,int pl2)
{
    memcpy(&PLAYER[pl1],&PLAYER[pl2],sizeof(PLAYGAMER_TEMP));
}

