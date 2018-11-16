#ifndef _PLAYER_W
#define _PLAYER_W

#ifdef UNDERLUNUX
#include <arpa/inet.h>
#endif

#include <stdint.h>
#include <stdio.h>

#define SETMAXPAUSE				3

#define MAXPLAYERNAMESYMBOLS	24
#define MAXCLANNAMESYMBOLS		36

#define STAR_TERRAN_CAMPAIGN	0
#define STAR_ZERG_CAMPAIGN		1
#define STAR_PROTOSS_CAMPAIGN	2
#define BROOD_PROTOSS_CAMPAIGN	3
#define BROOD_TERRAN_CAMPAIGN	4
#define BROOD_ZERG_CAMPAIGN		5

#define MESSAGE_SENDMESSAGETOEVERYONE	0
#define MESSAGE_SENDMESSAGETOALLIES		1
#define MESSAGE_SENDMESSAGETOPLAYER		2

#define STATPLAYER_UNITSPRODUCED		0
#define STATPLAYER_UNITSKILLED			1
#define STATPLAYER_UNITSLOST			2
#define STATPLAYER_BUILDSCONSTRUCTED	3
#define STATPLAYER_BUILDSRISED			4
#define STATPLAYER_BUILDSLOST			5
#define STATPLAYER_GASMINED				6
#define STATPLAYER_MINERALSMINED		7
#define STATPLAYER_TOTALSPENT			8
#define STATPLAYER_MAX					9

#define IDPLAYERFILE 0x57434852			//"WCHR"

#define IDPLAYERFILE_0	 ((IDPLAYERFILE >> 24) & 0xff)
#define IDPLAYERFILE_1	 ((IDPLAYERFILE >> 16) & 0xff)
#define IDPLAYERFILE_2	 ((IDPLAYERFILE >> 8) & 0xff)
#define IDPLAYERFILE_3	 ((IDPLAYERFILE >> 0) & 0xff)

union IdPlayerFile
{
		uint32_t		i_id;
		uint8_t			c_id[4];
};

struct	STATPLAYER
{
		int stat[STATPLAYER_MAX];
};

#define MAXMISSIONSVIDEOS		20

#define DEFAULT_TERRAN_MISSION			"\x80\x01\x02\x4b\x03\x04\x05\x4c\x06\x07\x08\x4d\x09\x0a\x4e\x00\x00\x00\x00\x00"
#define DEFAULT_TERRAN_NEXT				"\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x00\x00\x00\x00\x00"
#define DEFAULT_ZERG_MISSION			"\x80\x01\x02\x4a\x03\x04\x4b\x05\x06\x07\x4c\x08\x09\x4d\x00\x00\x00\x00\x00\x00"
#define DEFAULT_ZERG_NEXT				"\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x00\x00\x00\x00\x00\x00"
#define DEFAULT_PROTOSS_MISSION			"\x80\x01\x02\x4a\x03\x04\x4b\x05\x06\x4c\x07\x08\x09\x4d\x3f\x00\x00\x00\x00\x00"
#define DEFAULT_PROTOSS_NEXT			"\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x00\x00\x00\x00\x00"

#define DEFAULT_XPROTOSS_MISSION		"\x80\x01\x02\x03\x04\x05\x06\x07\x5d\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define DEFAULT_XPROTOSS_NEXT			"\x01\x02\x03\x04\x05\x06\x07\x08\x09\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define DEFAULT_XTERRAN_MISSION			"\x80\x01\x02\x03\x04\x05\x06\x07\x08\x56\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define DEFAULT_XTERRAN_NEXT			"\x01\x02\x03\x04\x06\x06\x07\x08\x09\x0a\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define DEFAULT_XZERG_MISSION			"\x80\x01\x02\x03\x03\x03\x03\x03\x03\x04\x05\x06\x07\x08\x09\x0a\x4b\x3f\x00\x00"
#define DEFAULT_XZERG_NEXT				"\x01\x02\x03\x09\x09\x09\x09\x09\x09\x0a\x0b\x0c\x0d\x0f\x0f\x10\x11\x12\x00\x00"
//										  0	   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f   10  11  12  13
#pragma pack(push,1)

#define TOTALCAMPAIGNS			6
struct	MISSIONHIST
{
		unsigned char			campaigndone;
		unsigned char			fromlineinhisttbl;
		unsigned char			nrlinesinhisttbl;
		unsigned char			seq_missions[MAXMISSIONSVIDEOS];
		unsigned char			next_missions[MAXMISSIONSVIDEOS];
};
struct PLAYGAMER
{
		char					nickname[MAXPLAYERNAMESYMBOLS+1];
		struct	MISSIONHIST		missions[TOTALCAMPAIGNS];				//zerg,terran,protoss,xzerg,xterran,xprotoss
};

#pragma pack(pop)

#define			PLAYER_ISOBSERVER				0x1
#define			PLAYER_DISCONNECTED				0x2
struct PLAYGAMER_TEMP
{
		int				minerals;				//<<8
		int				gas;					//<<8
		signed	 char	colorRACE;
		short			curentpsi[3];
		short			maxcurentpsi[3];
		unsigned char	nukes;
		unsigned char	isobserverflag;
		unsigned char	sendmessageto;
		signed	 char	messagetoplayernr;
		unsigned char	pausedgame;
		unsigned int	PCID;
		STATPLAYER		statplayer;
		struct sockaddr_in		Addr;
};

extern PLAYGAMER curplayer[];
void getplayerinfo(char *nickname);
void setplayername(int playernr,const char *playername);
char *getplayername(int playernr);
char *getplayerclan(int playernr);
void setplayerclan(int playernr,char *clanname);
void resetinfoplayers(void);
int	 loadandsetplayerinfo(char *player_filename);
void saveplayerinfo(void);
void clearplayernames(void);
int	 SortPlayers(int gamenr);
void copyplayerinfo(int pl1,int pl2);
void CreateDefaultInfoForPlayer(FILE *f);

#endif

