#ifndef _SC_NETWPLAY_H

#define _SC_NETWPLAY_H

#include "defs.h"
#include "network_general.h"
#include "mylist.h"

#define MAXONETICKPACKETS		10 
#define MAXSAVED_NETW_TICKS		2

#define TICK_T_FLAG_SENT		0x01

struct income_tick_t
{
	unsigned char		maxpackets[MAXPLAYERS];
	unsigned char		packetscome[MAXPLAYERS][MAXONETICKPACKETS];
	mylist				queue_cmdstr[MAXPLAYERS];
	unsigned short		infofromplayers;
	unsigned char		flags;
};


class NETW_PLAY
{
private:
	income_tick_t		tick_t[MAXSAVED_NETW_TICKS];
	unsigned char		curticknr;
	
	unsigned char		netwlatency;	

	signed char			networksendcycle,networkreceivecycle;
	unsigned int		networkticknr;

	unsigned int		tickreceived[MAXPLAYERS];
	char				netwspeedvalue;
	char				pauseresumevalue;
	char				timeout;
	char				chattoplayers[MAXPLAYERS];		
	char				chatstr[MAXCHATSYMB];
	
public:
	void SetNetworkLatency(int value,int playersetedlatency);	//0,1,2 low,high,extra high
	void SetPauseResumeMessage(int value,int playersetedlatency,int remainpause);//0-no changes,1-paused,2-resumed

	void InitNetworkTicks(int ticknr);
	void DeInitNetworkTicks(void);

	void MakeFirstNetworkCycle(void);
	int	 DoNetworkGameCycle(void);
	int	 ReceiveGame_InfoFromPlayers(ALLNETWCONNECTION *conn);
	int	 CheckPlayPacket(NETWORK_INFO *recvsock);
	int	 SendGame_ResendPackets(ALLNETWCONNECTION *conn,int netwstick,char sendtoplayers[]);
	void Make_AllNetCmds(void);
	void Make_PlayerNetCmds(int playernr);
	int	 SendInfoToSinglePlayer(ALLNETWCONNECTION *conn,int netcmd,int netwstick,const void *collecteddata,int playernr,int totalpkts,int pktnr);
	int	 Server_SendInfoToPlayers(ALLNETWCONNECTION *conn,int netcmd,int netwsendtick,const void *collecteddata,char sendtoplayers[],int totalpkts,int pktnr);
	void SendGame_NetworkSpeed(int netspeed);
	void SendGame_PauseResume(int pausevalue);
	void SendGame_NetworkText(char *chattext);
	int	 ReceivedInfoBitsFromPlayers(void);
	int	 Check_ReceiveFromPlayers(void);
	int	 Get_PreviousPacketTick(int step);
	int	 Get_NextPacketTick(int step);
	void Next_CurrentPacketTick(void);
	void Empty_TickInfo(int packetticknr);
	void Make_AllNetCmdsBRIEF(void);
	void Make_PlayerNetCmdsBRIEF(int playernr);
	int	 Client_SendInfoToServer(ALLNETWCONNECTION *conn,int netcmd,int netwstick,const void *collecteddata,int totalpkts,int pktnr);
	int	 Check_ReceiveFromServer(void);
	int	 Check_ReceiveFromNoServer(void);
	void Server_AllReceiveInfo(void);
	void Server_MakeAllNetCmds(void);
	void Client_AllReceiveInfo(void);
	void Client_MakeAllNetCmds(void);
	void Server_SendAllPlayersInfoToPlayers(void);
	void Client_SendMyInfoToServer(void);
	void Client_SetSentFlag(void);
	int	 Client_CheckIfSent(void);
	void PrepareNetworkCommands(int playernr,mylist *cmdslist);
	void Do_NetworkCommands(int playernr,char *netcmd);

};



#endif
