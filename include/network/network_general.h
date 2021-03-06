#ifndef _SC_NETWORK_H

#define _SC_NETWORK_H

#include "CrossPlatform.h"

#include "defs.h"
#include "player.h"

#define MKADDR(a,b,c,d)							( (a<<24) | (b<<16) | (c<<8) | d)
#define MAXCONNTYPES	5						//nr of network types (tcpip,udp,modem,.....)

#define MAXCREATEDGAMES							8
#define STARCLONE_NETWORKJOIN_MAXGAMETITLESYMB	MAXPLAYERNAMESYMBOLS
#define STARCLONE_NETWORKJOIN_MAXMAPNAMESYMB	30
#define TIMEOUT_NETWORK_PACKETS					10		//sec


#define NETWORK_OK								0
#define NETWORK_OK_NODATA						1
#define NETWORK_OK_RECVDATA						2
#define NETWORK_ERROR_CMD						-1
#define NETWORK_ERROR_SOCKET					-2
#define NETWORK_ERROR_INIT						-3
#define NETWORK_ERROR_BROADCAST					-4
#define NETWORK_ERROR_REUSEADDR					-5
#define NETWORK_ERROR_SEND						-6
#define NETWORK_ERROR_NODATA					-7
#define NETWORK_ERROR_SOCKETBIND				-8
#define NETWORK_ERROR_SELECT					-9
#define NETWORK_ERROR_WRONGDATA					-10


#define STARCLONE_UDP_BROADCASTPORT				50001
#define STARCLONE_UDP_PLAYPORT					50002

#define STARCLONE_UDP_JOINPACKETSIZE			(512-28)
#define PORTIONFILESIZE							(STARCLONE_UDP_JOINPACKETSIZE-56)

#define STARCLONE_UDP_JOINPORT					50003
#define STARCLONE_UDP_MAXTRYJOINPORT			1


#define STARCLONE_NETWORKJOIN_COMMAND_NONE						0
#define STARCLONE_NETWORKJOIN_COMMAND_CREATEDGAME				1
#define STARCLONE_NETWORKJOIN_COMMAND_CLOSEGAME					2
#define STARCLONE_NETWORKJOIN_COMMAND_YOUJOINED					3
#define STARCLONE_NETWORKJOIN_COMMAND_FULLSLOTS					4
#define STARCLONE_NETWORKJOIN_COMMAND_JOINREQUEST				5
#define STARCLONE_NETWORKJOIN_COMMAND_PLAYERINFO				6
#define STARCLONE_NETWORKJOIN_COMMAND_ALLPLAYERSINFO			7
#define STARCLONE_NETWORKJOIN_COMMAND_MESSAGE					8
#define STARCLONE_NETWORKJOIN_COMMAND_LEAVEGAME					9
#define STARCLONE_NETWORKJOIN_COMMAND_BANPLAYER					10
#define STARCLONE_NETWORKJOIN_COMMAND_ISBANNED					11
#define STARCLONE_NETWORKJOIN_COMMAND_NAMEEXISTS				12
#define STARCLONE_NETWORKJOIN_COMMAND_ASKFORFILE				13
#define STARCLONE_NETWORKJOIN_COMMAND_FILEBYTES					14
#define STARCLONE_NETWORKJOIN_COMMAND_GIVENEXTPARTOFFILE		15
#define STARCLONE_NETWORKJOIN_COMMAND_PLAYGAME					16
#define STARCLONE_NETWORKJOIN_COMMAND_PAUSERESUMEGAME			17



#define NETWORK_INITFLAG						0x80
#define NETWORK_NONE							0

#define NETWORK_INITNETWORK						(NETWORK_INITFLAG + 1)
#define NETWORK_CLOSENETWORK					1

#define NETWORK_DATAREADY						2

#define NETWORK_INITRECVIDENTIFICATION			3
#define NETWORK_SENDIDENTIFICATION				4
#define NETWORK_SENDCLOSEIDENTIFICATION			5

#define NETWORK_INITBROADCAST					6
#define NETWORK_SENDPACKET						7

#define NETWORK_INITRECVPACKET					8
#define NETWORK_RECVPACKET						9

#define NETWORK_INITRECVPLAYPACKET				10

#define BUFF_NOCRYPTIZE							0
#define BUFF_NODECRYPTIZE						1
#define BUFF_CRYPTIZE							2
#define BUFF_DECRYPTIZE							3

#define GAMEOPENSLOT					0x0
#define GAMECLOSEDSLOT					0x1
#define GAMECOMPUTERSLOT				0x2
#define GAMEPLAYERSLOT					0x3
#define GAMEFORCENAMESLOT				0x4
#define GAMENOSLOT						0x7

#define PLAYERZERGSLOT					0x0
#define PLAYERTERRANSLOT				0x1
#define PLAYERPROTOSSSLOT				0x2
#define PLAYERRANDOMSLOT				0x3
#define PLAYEROBSERVERSLOT				0x4
#define PLAYERNOSLOT					0x7

#define CANNOTJOINME					-1
#define IGNOREJOINTOME					-2
#define REFRESHPLAYERSAFTER				-3
#define ABUSEMEFROMUNKNOWN				-4
#define CANNOTJOINMEBANNED				-5
#define CANNOTJOINMENAMEEXISTS			-6
#define NEEDUPLOADMAP					-7
#define CANJOIN							0

#define REQUESTFILEPARTS				15				//wait for file is come
#define DOWNLOADPARTOK					16								



#define PI_END			"&"
#define PI_STATUS		"STAT"
#define PI_TYPEGAME		"TPGM"
#define PI_NETWSPEED	"NSPD"
#define PI_GAMESPEED	"GSPD"
#define PI_SIZEXMAP		"MSZX"
#define PI_SIZEYMAP		"MSZY"
#define PI_PORT			"PORT"
#define PI_GAMETITLE	"GTTL"
#define PI_MAPNAME		"MNAM"
#define PI_MAXPLAYERS	"MXPL"
#define PI_PLAYERS		"SLTP"
#define PI_SLOTS		"SLTR"
#define PI_PCID			"PCID"
#define PI_JOINSLOT		"JSLT"
#define PI_PLAYERNAME	"PLNM"
#define PI_SECONDSLOT	"RACE"
#define PI_TEXTCHAT		"TCHT"
#define PI_PLAYERNR		"PLNR"
#define PI_MAPFILE		"MAPF"
#define PI_MAPCHKSUM	"MCHK"
#define PI_NEEDMAPSEND	"MAPX"
#define PI_FILEPOS		"MLEN"
#define PI_FILESIZEMAP	"MFSZ"
#define PI_PORTIONSIZE	"PRTS"
#define PI_BYTES		"BYTS"
#define PI_CHKSUMMBYTES "BCHK"
#define PI_PLAYERTYPE	"PTYP"
#define PI_PLAYERIP		"PADR"
#define PI_RNDSEED		"SEED"
#define PI_TICKNR		"TICK"
#define PI_MAXPACKETS	"MXPK"
#define PI_PACKETNR		"PKNR"
#define PI_GAMEPARAMID	"GPID"
#define PI_LOSTTICK		"LTCK"
#define PI_TOTALLOST	"TTLL"
#define PI_PACKETSMISS	"PMIS"
#define PI_NETCMDS		"NCMD"
#define PI_SERVERPLNR	"SPNR"
#define PI_PAUSEGAME	"PAUS"

#define GAMEIDENT_STR					PI_TYPEGAME "=%01u" PI_END PI_GAMESPEED "=%01u" PI_END PI_SIZEXMAP "=%u" PI_END \
										PI_SIZEYMAP "=%u" PI_END PI_PORT "=%u" PI_END PI_GAMETITLE "=%s" PI_END \
										PI_MAPNAME "=%s" PI_END PI_PLAYERS "=0x%08x" PI_END PI_SLOTS "=0x%08x" PI_END 

#define SENDJOINREQUEST_STR				PI_PORT "=%u" PI_END PI_PCID "=%u" PI_END PI_PLAYERNAME "=%s" PI_END 
#define SENDJOINRESPONCE_STR			PI_PCID "=%u" PI_END PI_TYPEGAME "=%01u" PI_END PI_GAMESPEED "=%01u" PI_END \
										PI_SIZEXMAP "=%u" PI_END PI_SIZEYMAP "=%u" PI_END PI_PORT "=%u" PI_END \
										PI_GAMETITLE "=%s" PI_END PI_MAPNAME "=%s" PI_END PI_MAXPLAYERS "=%u" PI_END \
										PI_JOINSLOT "=%u" PI_END 

#define SENDINFOTOPLAYERS_STR			PI_MAXPLAYERS "=%u" PI_END \
										PI_MAPFILE "=%s" PI_END PI_MAPCHKSUM "=0x%08x" PI_END PI_FILESIZEMAP "=%u" PI_END \
										PI_PLAYERTYPE "=%x" PI_END PI_PLAYERNAME "=%s" PI_END \
										PI_PLAYERTYPE "=%x" PI_END PI_PLAYERNAME "=%s" PI_END \
										PI_PLAYERTYPE "=%x" PI_END PI_PLAYERNAME "=%s" PI_END \
										PI_PLAYERTYPE "=%x" PI_END PI_PLAYERNAME "=%s" PI_END \
										PI_PLAYERTYPE "=%x" PI_END PI_PLAYERNAME "=%s" PI_END \
										PI_PLAYERTYPE "=%x" PI_END PI_PLAYERNAME "=%s" PI_END \
										PI_PLAYERTYPE "=%x" PI_END PI_PLAYERNAME "=%s" PI_END \
										PI_PLAYERTYPE "=%x" PI_END PI_PLAYERNAME "=%s" PI_END \
										PI_PLAYERTYPE "=%x" PI_END PI_PLAYERNAME "=%s" PI_END \
										PI_PLAYERTYPE "=%x" PI_END PI_PLAYERNAME "=%s" PI_END \
										PI_PLAYERTYPE "=%x" PI_END PI_PLAYERNAME "=%s" PI_END \
										PI_PLAYERTYPE "=%x" PI_END PI_PLAYERNAME "=%s" PI_END 
								
#define SENDLEAVEGAME_STR				PI_PLAYERNR "=%u" PI_END
#define SENDPLAYERINFO_STR				PI_PLAYERNR "=%u" PI_END PI_SECONDSLOT "=%u" PI_NEEDMAPSEND "=%u" PI_END
#define SENDMESSAGETOPLAYERS_STR		PI_PLAYERNR "=%d" PI_END PI_TEXTCHAT "=%s" PI_END
#define SENDFILEINFO_STR				PI_PLAYERNR "=%u" PI_END PI_FILEPOS "=%u" PI_END
#define SENDFILEDATA_STR				PI_FILEPOS "=%u"  PI_END PI_CHKSUMMBYTES "=0x%08x"	PI_END PI_PORTIONSIZE "=%u" PI_END
										//and	  ####PI_BYTES "=%c%c...."####//

#define SENDPLAYGAME_STR				PI_PLAYERS	"=%u" PI_END PI_RNDSEED "=%u" PI_END \
										PI_PLAYERTYPE "=%x" PI_END PI_PLAYERIP "=%x" PI_END PI_PCID "=%u" PI_END \
										PI_PLAYERTYPE "=%x" PI_END PI_PLAYERIP "=%x" PI_END PI_PCID "=%u" PI_END \
										PI_PLAYERTYPE "=%x" PI_END PI_PLAYERIP "=%x" PI_END PI_PCID "=%u" PI_END \
										PI_PLAYERTYPE "=%x" PI_END PI_PLAYERIP "=%x" PI_END PI_PCID "=%u" PI_END \
										PI_PLAYERTYPE "=%x" PI_END PI_PLAYERIP "=%x" PI_END PI_PCID "=%u" PI_END \
										PI_PLAYERTYPE "=%x" PI_END PI_PLAYERIP "=%x" PI_END PI_PCID "=%u" PI_END \
										PI_PLAYERTYPE "=%x" PI_END PI_PLAYERIP "=%x" PI_END PI_PCID "=%u" PI_END \
										PI_PLAYERTYPE "=%x" PI_END PI_PLAYERIP "=%x" PI_END PI_PCID "=%u" PI_END \
										PI_PLAYERTYPE "=%x" PI_END PI_PLAYERIP "=%x" PI_END PI_PCID "=%u" PI_END \
										PI_PLAYERTYPE "=%x" PI_END PI_PLAYERIP "=%x" PI_END PI_PCID "=%u" PI_END \
										PI_PLAYERTYPE "=%x" PI_END PI_PLAYERIP "=%x" PI_END PI_PCID "=%u" PI_END \
										PI_PLAYERTYPE "=%x" PI_END PI_PLAYERIP "=%x" PI_END PI_PCID "=%u" PI_END

#define PG_PAUSE_STR					PI_PAUSEGAME "=%01u" PI_END
#define PG_NSPEED_STR					PI_NETWSPEED "=%01u" PI_END
#define PG_CHATTEXT_STR					PI_TEXTCHAT "=%s" PI_END

#define PG_GLOBAL_STR					PI_PLAYERNR "=%u" PI_END PI_TICKNR "=%x" PI_END \
										PI_MAXPACKETS "=%01u" PI_END PI_PACKETNR "=%01u" PI_END 
#define PG_GENERAL_STR					PG_GLOBAL_STR PI_NETCMDS "=%s" PI_END PG_NSPEED_STR PG_PAUSE_STR PG_CHATTEXT_STR
#define PG_BRIEFCLIENT_STR				PG_GLOBAL_STR
#define PG_BRIEFSERVER_STR				PG_BRIEFCLIENT_STR PI_NETWSPEED "=%01u" PI_END PI_GAMESPEED "=%01u" PI_END \
										PI_SERVERPLNR "=%01u" PI_END



#define PG_RESEND_STR					PI_PLAYERNR "=%u" PI_END PI_TICKNR "=%x" PI_END \
										PI_LOSTTICK "=%u" PI_END PI_MAXPACKETS "=%u" PI_END \
										PI_TOTALLOST "=%u" PI_END PI_PACKETSMISS "=%s" PI_END

#define SC_NETCMD_BRIEF					0
#define SC_NETCMD_GENERAL				1
#define SC_NETCMD_RESEND				2
#define SC_NETCMD_NSPEED				3
#define SC_NETCMD_CHATTEXT				4

#define GAMETICK_GAMEBRIEF				0
#define GAMETICK_FIRSTPLAY				1

#define PLAYGAME

#pragma pack(push,1)
										
struct NETGAME
{
	unsigned int PCID;
	short int	mapsizex;
	short int	mapsizey;
	char		timeout;
	char		TypeGame;
	char		SpeedGame;
	unsigned int players;		//4 bits per player
	unsigned int slots;			//4 bits per player
	unsigned short int	connectport;
	struct		sockaddr_in Addr;
	char		gametitle[STARCLONE_NETWORKJOIN_MAXGAMETITLESYMB+1];
	char		mapname[STARCLONE_NETWORKJOIN_MAXMAPNAMESYMB+1];
};
//4+2+2+5+16+31=60

#define							SIZESENDSTR		(STARCLONE_UDP_JOINPACKETSIZE-2*sizeof(int)-sizeof(unsigned char))

struct NETWORK_JOIN_PACKET
{
	unsigned int		CRC;
	unsigned int		PCID;
	unsigned char		COMMAND;
	char				data[SIZESENDSTR];
};

struct NETWORK_INFO
{
	unsigned int				param;
	int							socketnr;
	unsigned int				nextpacketbytes;
	unsigned int				PCID;
	unsigned short int			connectport;
	unsigned char				COMMAND;
	struct NETGAME				netgame;
	void						*recvbuffer;
	struct						sockaddr_in Addr;
	char						sendstr[SIZESENDSTR];
};

struct WAITJOIN_INFO
{
	struct NETWORK_INFO			*sock1; //broadcast
	struct NETWORK_INFO			*sock2; //waitconn
};

#pragma pack(pop)

struct NEWNETWGAME
{
	char		slot1[MAXPLAYERS];
	char		slot2[MAXPLAYERS];
};

struct ALLNETWCONNECTION
{
	NETWORK_INFO		receive_gameinfo;
	NETWORK_INFO		send_gameinfo[MAXPLAYERS];
	char				netwplayers;
	int					bitsnetwplayers;
};

void DEBUG_packet(int inout,NETWORK_JOIN_PACKET *mypacket,struct sockaddr_in *sk);
int	 DecodeBuffer(unsigned char *buff,int size);
int	 CodeBuffer(unsigned char *buff,int size);
int	 CreateCRC(unsigned char *buff,int size,int codedecode);
int	 NETWORKGAME_UDP(NETWORK_INFO *param);
int	 NETWORKGAME_TCP(NETWORK_INFO *param);
int	 NETWORKGAME_DC(NETWORK_INFO *param);
int	 NETWORKGAME_MODEM(NETWORK_INFO *param);
int	 NETWORKGAME_DEDICATED(NETWORK_INFO *param);


void SetNetworkType(int networktype);
int	 CallNetwork(int funcid,NETWORK_INFO *info);

extern int (*NetworkFunc)(NETWORK_INFO *);
extern int (*networkfunc[])(NETWORK_INFO *);
extern ALLNETWCONNECTION		playersconn;

#endif /* _NETWORK_W */
