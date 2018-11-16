
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef WITHSDL
#include "sdl/grsdl.h"
#endif

#include "auxil.h"
#include "debug.h"
#include "mytime.h"
#include "gener.h"
#include "maps.h"
#include "starmap.h"
#include "network_general.h"
#include "netwgame.h"
#include "netwplay.h"

char SENDPARAM_SENDTOALLPLAYERS[MAXPLAYERS] = { 1,1,1,1,1,1,1,1,1,1,1,1 };
//==========================
void NETW_PLAY::Empty_TickInfo(int packetticknr)
{
	memset(tick_t[packetticknr].maxpackets, 0, MAXPLAYERS * sizeof(unsigned char));
	memset(tick_t[packetticknr].packetscome, 0, MAXPLAYERS*MAXONETICKPACKETS * sizeof(unsigned char));
	tick_t[packetticknr].infofromplayers = 1 << NUMBGAMER;//assumpt current player info already received
	tick_t[curticknr].flags = 0;
}
//==========================
void NETW_PLAY::InitNetworkTicks(int ticknr)
{
	int j;
	DeInitNetworkTicks();

	networkticknr = ticknr;
	for (int i = 0;i < MAXPLAYERS;i++)
		tickreceived[i] = ticknr;
	SetNetworkLatency(gameconf.networkconf.networklatency, -1);
	//prevent to use filled queue
	for (j = 0;j < MAXSAVED_NETW_TICKS;j++)
	{
		Empty_TickInfo(j);
	}
}
//==========================
void NETW_PLAY::DeInitNetworkTicks(void)
{
	int i, j;
	for (j = 0;j < MAXSAVED_NETW_TICKS;j++)
		for (i = 0;i < MAXPLAYERS;i++)
			tick_t[j].queue_cmdstr[i].DeallocList();
}
//==========================
void NETW_PLAY::MakeFirstNetworkCycle(void)
{
	if (NETWORKGAME)
	{
		networksendcycle = netwlatency / 2;
		networkreceivecycle = netwlatency;
		InitNetworkTicks(GAMETICK_FIRSTPLAY);
	}
	else
	{
		networksendcycle = 1;
		networkreceivecycle = 1;			//act as realtime
	}
	playersconn.bitsnetwplayers = force_slots.bitsnetwplayers;
	playersconn.netwplayers = force_slots.networkplayers;
	timeout = 0;
}
//==========================
int NETW_PLAY::ReceiveGame_InfoFromPlayers(ALLNETWCONNECTION *conn)
{
	int err, err2, dataready = NETWORK_OK_NODATA;
	do {
		err = CallNetwork(NETWORK_DATAREADY, &conn->receive_gameinfo);
		if (err == NETWORK_OK_RECVDATA)
		{
			err = CallNetwork(NETWORK_RECVPACKET, &conn->receive_gameinfo);
			if (err == NETWORK_OK_RECVDATA)
			{
				dataready = err;
				CheckPlayPacket(&conn->receive_gameinfo);
			}
		}
		else//no data, exit loop
			return(dataready);
	} while (1);
}
//==========================================
int NETW_PLAY::CheckPlayPacket(NETWORK_INFO *recvsock)
{
	int i, pl, tick, nspeed, gspeed, gameparam, maxpkts, packetnr, lostpackettick, totallost, savetick;
	char newstr[SIZESENDSTR];
	char loststr[SIZESENDSTR];
	char received_cmds[SIZESENDSTR];
	NETWORK_JOIN_PACKET *mypacket = (NETWORK_JOIN_PACKET *)recvsock->recvbuffer;
	if (recvsock->PCID == mypacket->PCID) //the packet is realy come to me?
	{
		pl = GetINTFromArray(mypacket->data, PI_PLAYERNR, 0);
		if (pl < 0 || pl >= MAXPLAYERS)
			return(-1);
		tick = GetHEXFromArray(mypacket->data, PI_TICKNR, 0);
		switch (mypacket->COMMAND)
		{
		case SC_NETCMD_GENERAL:
			if (tick < tickreceived[pl])
			{
				DEBUGMESSCR("receive doublicate/old network info tick=%d mytick=%d\n", tick, networkticknr);
				return(-1);
			}
			//if tick != current tick or next tick - assinchronize detected
			if (tick < 0 || tick < networkticknr || tick >= networkticknr + MAXSAVED_NETW_TICKS)
			{
				DEBUGMESSCR("ANSINCHRONIZE:wrong from network tick=%d networkticknr=%d\n", tick, networkticknr);
				return(-1);
			}
			//???? the save tick may be wrong, so
			//???? the old packet pass here and recognize as current
			savetick = Get_NextPacketTick(tick - networkticknr);
			maxpkts = GetINTFromArray(mypacket->data, PI_MAXPACKETS, 0);
			GetSTRFromArray(received_cmds, mypacket->data, PI_NETCMDS, 0);
			//add cmds to queue if any
			if (received_cmds[0])
				tick_t[savetick].queue_cmdstr[pl].AllocAndAddList(received_cmds, strlen(received_cmds));
			if (maxpkts<0 || maxpkts>MAXONETICKPACKETS)
				return(-1);
			if (tick_t[savetick].maxpackets[pl] == 0)
			{
				tick_t[savetick].maxpackets[pl] = maxpkts;
				memset(tick_t[savetick].packetscome[pl], 0, MAXONETICKPACKETS * sizeof(unsigned char));
			}
			packetnr = GetINTFromArray(mypacket->data, PI_PACKETNR, 0);
			if (packetnr<0 || maxpkts>tick_t[savetick].maxpackets[pl])
				return(-1);
			if (tick_t[savetick].packetscome[pl][packetnr] == 0)
			{
				tick_t[savetick].packetscome[pl][packetnr] = 1;
			}
			else
			{
				DEBUGMESSCR("receive doublicate packet mytick=%d packetnr=%d\n", networkticknr, packetnr);
				return(-1);
			}
			for (i = 0;i < tick_t[savetick].maxpackets[pl];i++)
				if (tick_t[savetick].packetscome[pl][i] == 0)
				{
					return(-1);
				}
			//here: we get all received packets from one player

			//go for next round of incoming packets for this player
			tick_t[savetick].maxpackets[pl] = 0;
			tickreceived[pl]++;
			tick_t[savetick].infofromplayers |= 1 << pl;
			//income all packets
			DEBUGMESSCR("receive all fragmented packets nettick=%d mytick=%d\n", tick, networkticknr);
			return(pl);
			break;
		case SC_NETCMD_RESEND://we receive info what the player not receive some packet, so we need to resend to him
			lostpackettick = GetINTFromArray(mypacket->data, PI_LOSTTICK, 0);
			maxpkts = GetINTFromArray(mypacket->data, PI_MAXPACKETS, 0);
			totallost = GetINTFromArray(mypacket->data, PI_TOTALLOST, 0);
			GetSTRFromArray(loststr, mypacket->data, PI_PACKETSMISS, 0);
			//resend info to player who ask
			if (lostpackettick != networkticknr)
			{
				DEBUGMESSCR("request wrong lostpackettick=%d ticknr=%d\n", lostpackettick, networkticknr);
				return(-1);
			}
			if (maxpkts == 0)
			{
				//need to send all losted packets
				SendInfoToSinglePlayer(&playersconn, SC_NETCMD_GENERAL, lostpackettick, "", pl, 1, 0);//????
			}
			else
				for (i = 0;i < totallost;i++)
				{
					SendInfoToSinglePlayer(&playersconn, SC_NETCMD_GENERAL, lostpackettick, "", pl, maxpkts, loststr[i] - '0');
				}
			return(-1);

			break;
		default:
			DEBUGMESSCR("unknown network play command %d tick=%d mytick=%d\n", mypacket->COMMAND, tick, networkticknr);
			return(-1);
			break;
		}
	}
	else
		DEBUGMESSCR("PLAY:wrong PCID %x %x\n", recvsock->PCID, mypacket->PCID);
	return(-1);
}
//==========================================
//return 0 - good , 1 - error not all info show menu wait
//==========================
#define TESTPACKETS		1
int NETW_PLAY::DoNetworkGameCycle(void)
{
	int err, i, recvpls;
	char toplayers[MAXPLAYERS];
	if (NETWORKGAME)
	{
		do {
			err = ReceiveGame_InfoFromPlayers(&playersconn);
		} while (err == NETWORK_OK_RECVDATA);//repeat read, if data comes (may be we have more data)
		if (Check_IfServerPlayer())//if server player
		{
			if (networkgametick)
			{
				networkgametick = 0;
				if (--networkreceivecycle <= 0)
				{
					if (Check_ReceiveFromPlayers())
					{
						//coming all info from all players, we need to send all received info to players
						DEBUGMESSCR("server send: mytick=%d\n", networkticknr);
						Server_SendAllPlayersInfoToPlayers();

						//do received network commands
						Server_MakeAllNetCmds();

						Next_CurrentPacketTick();
						networkreceivecycle = netwlatency;
					}
					else
					{
						//timeout comes here
						timeout = 1;
						networkreceivecycle = netwlatency;
						//TODO:send retransmition
						DEBUGMESSCR("server resend: mytick=%d\n", networkticknr);
						recvpls = ReceivedInfoBitsFromPlayers();
						for (i = 0;i < MAXPLAYERS;i++)
						{
							if ((recvpls >> i) & 1)
								toplayers[i] = 0;
							else
								toplayers[i] = 1;
						}
						//some packet lost, need to send query to specific player to send it again
						SendGame_ResendPackets(&playersconn, networkticknr, toplayers);
						return(~recvpls);
					}
				}
			}
		}
		else//if client player
		{
			if (timeout)
			{
				if (Check_ReceiveFromNoServer())
				{
					DEBUGMESSCR("1.error receive from no server\nclient code may changes\n");
				}
				if (Check_ReceiveFromServer())
				{
					timeout = 0;
					//do received network commands
					Client_MakeAllNetCmds();

					Next_CurrentPacketTick();
					networkreceivecycle = netwlatency;
				}
			}
			if (!Client_CheckIfSent())
			{
				DEBUGMESSCR("client send: mytick=%d\n", networkticknr);
				Client_SendMyInfoToServer();
			}
			if (networkgametick)
			{
				networkgametick = 0;
				if (--networkreceivecycle <= 0)
				{
					if (Check_ReceiveFromNoServer())
					{
						DEBUGMESSCR("2.error receive from no server\nclient code may have changes\n");
					}
					if (Check_ReceiveFromServer())
					{
						//do received network commands
						Client_MakeAllNetCmds();

						Next_CurrentPacketTick();
						networkreceivecycle = netwlatency;
					}
					else
					{
						//timeout comes here
						timeout = 1;
						networkreceivecycle = netwlatency;
						//TODO:send retransmition
						DEBUGMESSCR("client resend: mytick=%d\n", networkticknr);
						//resend only to server
						recvpls = ReceivedInfoBitsFromPlayers();
						for (i = 0;i < MAXPLAYERS;i++)
						{
							if (SERVERGAMER == i)
								toplayers[i] = 1;
							else
								toplayers[i] = 0;
						}
						//some packet lost, need to send query to specific player to send it again
						SendGame_ResendPackets(&playersconn, networkticknr, toplayers);
						return(~recvpls);
					}
				}
			}
		}//else client
	}//else NETWORKGAME
	return(0);
}
//==========================
int NETW_PLAY::SendInfoToSinglePlayer(ALLNETWCONNECTION *conn, int netcmd, int netwstick,
	const void *collecteddata, int playernr, int totalpkts, int pktnr)
{
	int err;
	if (playernr != NUMBGAMER && gameconf.pl_owner[playernr] == OWNER_HUMAN)
	{
		//gametick is networksendtick
		switch (netcmd)
		{
		case SC_NETCMD_BRIEF:
			conn->send_gameinfo[playernr].COMMAND = SC_NETCMD_GENERAL;
			if (Check_IfServerPlayer())
			{
				SERVERGAMER = NUMBGAMER;
				sprintf(conn->send_gameinfo[playernr].sendstr, PG_BRIEFSERVER_STR,
					NUMBGAMER,
					netwstick,
					1, 0,			//maxpackets,packetnr
					gameconf.networkconf.networklatency,
					GAMESPEED,
					SERVERGAMER);
			}
			else
			{
				sprintf(conn->send_gameinfo[playernr].sendstr, PG_BRIEFCLIENT_STR,
					NUMBGAMER,
					netwstick,
					1, 0);			//maxpackets,packetnr
			}
			gameconf.speedconf.gamespeed = GAMESPEED;
			break;
		case SC_NETCMD_RESEND:
			conn->send_gameinfo[playernr].COMMAND = SC_NETCMD_RESEND;
			strcpy(conn->send_gameinfo[playernr].sendstr, (char *)collecteddata);
			break;
		case SC_NETCMD_GENERAL:
			conn->send_gameinfo[playernr].COMMAND = SC_NETCMD_GENERAL;
			if (chatstr[0] && chattoplayers[playernr])
			{
				sprintf(conn->send_gameinfo[playernr].sendstr, PG_GENERAL_STR,
					NUMBGAMER,
					netwstick,
					totalpkts, pktnr,
					"", netwspeedvalue, pauseresumevalue, chatstr);//no cmds
			}
			else
			{
				sprintf(conn->send_gameinfo[playernr].sendstr, PG_GENERAL_STR,
					NUMBGAMER,
					netwstick,
					totalpkts, pktnr,
					"", netwspeedvalue, pauseresumevalue, "");//no cmds
			}
			SendGame_NetworkSpeed(-1);
			SendGame_PauseResume(0);
			chatstr[0] = 0;//empty chat string
			break;
		default:
			return(0);
		}
		err = CallNetwork(NETWORK_SENDPACKET, &conn->send_gameinfo[playernr]);
		return(1);
	}
	return(0);
}
//==========================
int NETW_PLAY::Server_SendInfoToPlayers(ALLNETWCONNECTION *conn, int netcmd, int netwstick, const void *collecteddata, char sendtoplayers[], int totalpkts, int pktnr)
{
	int i, err, allsend = 0;
	for (i = 0;i < MAXPLAYERS;i++)
	{
		if (sendtoplayers[i])
		{
			allsend += SendInfoToSinglePlayer(conn, netcmd, netwstick, collecteddata, i, totalpkts, pktnr);
		}
	}
	return(allsend);
}
//==========================
int NETW_PLAY::Client_SendInfoToServer(ALLNETWCONNECTION *conn, int netcmd, int netwstick, const void *collecteddata, int totalpkts, int pktnr)
{
	return(SendInfoToSinglePlayer(conn, netcmd, netwstick, collecteddata, SERVERGAMER, totalpkts, pktnr));
}
//==========================
int NETW_PLAY::Get_PreviousPacketTick(int nextstep)
{
	if (curticknr - nextstep < 0)
		return(MAXSAVED_NETW_TICKS + (curticknr - nextstep));
	else
		return(curticknr - nextstep);
}
//==========================
int NETW_PLAY::Get_NextPacketTick(int nextstep)
{
	if (nextstep < 0)
		DEBUGMES("error nextstep value=%d\n", nextstep);
	if (curticknr + nextstep >= MAXSAVED_NETW_TICKS)
		return((curticknr + nextstep) - MAXSAVED_NETW_TICKS);
	else
		return(curticknr + nextstep);
}
//==========================
void NETW_PLAY::Next_CurrentPacketTick(void)
{
	if (++curticknr >= MAXSAVED_NETW_TICKS)
		curticknr = 0;
	Empty_TickInfo(curticknr);
	networkticknr++;
}
//==========================
int NETW_PLAY::Check_ReceiveFromPlayers(void)
{
	//	  DEBUGMESSCR("curticknr=%d sendtick=%d receivtick=%d,infopl=%d bitspl=%d\n",
	//			curticknr,networksendtick,networkreceivetick,	 
	//			tick_t[curticknr].infofromplayers,playersconn.bitsnetwplayers);
	return(tick_t[curticknr].infofromplayers == playersconn.bitsnetwplayers);
}
//==========================
int NETW_PLAY::Check_ReceiveFromServer(void)
{
	//	  DEBUGMESSCR("curticknr=%d sendtick=%d receivtick=%d,infopl=%d bitspl=%d\n",
	//			curticknr,networksendtick,networkreceivetick,	 
	//			tick_t[curticknr].infofromplayers,playersconn.bitsnetwplayers);
	return(tick_t[curticknr].infofromplayers == ((1 << SERVERGAMER) | (1 << NUMBGAMER)));
}
//==========================
int NETW_PLAY::ReceivedInfoBitsFromPlayers(void)
{
	return(tick_t[curticknr].infofromplayers);
}
//==========================
int NETW_PLAY::Check_ReceiveFromNoServer(void)
{
	return(tick_t[curticknr].infofromplayers&(~((1 << SERVERGAMER) | (1 << NUMBGAMER))));
}
//==========================
void NETW_PLAY::SendGame_NetworkSpeed(int nspeed)
{
	netwspeedvalue = nspeed;
}
//==========================
void NETW_PLAY::SendGame_PauseResume(int pausevalue)
{
	pauseresumevalue = pausevalue;
}
//==========================
void NETW_PLAY::SendGame_NetworkText(char *chattext)//chat text with playername (playername: ..........)
{
	int i, err;
	if (!chatstr[0])
		strcpy(chatstr, chattext);
	else
		sprintf(chatstr + strlen(chatstr), PG_CHATTEXT_STR, chattext);
	//depend on allies sends
	switch (PLAYER[NUMBGAMER].sendmessageto)
	{
	case MESSAGE_SENDMESSAGETOEVERYONE:
		memset(chattoplayers, 1, MAXPLAYERS * sizeof(char));
		break;
	case MESSAGE_SENDMESSAGETOALLIES:
		memset(chattoplayers, 0, MAXPLAYERS * sizeof(char));
		for (i = 0;i < MAXPLAYERS;i++)
		{
			if (player_aliance(NUMBGAMER, i) == ALLIANCEOBJ && gameconf.pl_owner[i] == OWNER_HUMAN)
			{
				chattoplayers[i] = 1;
			}
		}
		break;
	case MESSAGE_SENDMESSAGETOPLAYER:
		memset(chattoplayers, 0, MAXPLAYERS);
		for (i = 0;i < MAXPLAYERS;i++)
		{
			if (i == PLAYER[NUMBGAMER].messagetoplayernr && gameconf.pl_owner[i] == OWNER_HUMAN)
			{
				chattoplayers[i] = 1;
			}
		}
		break;
	}
	ShowChatMessage(chattext);
}
//==========================
int NETW_PLAY::SendGame_ResendPackets(ALLNETWCONNECTION *conn, int netwstick, char sendtoplayers[])
{
	char paramstr[SIZESENDSTR];
	char loststr[SIZESENDSTR];
	int i, j, sendall = 0, totallost = 0;
	for (i = 0;i < MAXPLAYERS;i++)
	{
		if (sendtoplayers[i])
		{
			for (j = 0;j < tick_t[curticknr].maxpackets[i];j++)
			{
				if (tick_t[curticknr].packetscome[i][j] == 0)
					loststr[totallost++] = '0' + j;
			}
			sprintf(paramstr, PG_RESEND_STR, NUMBGAMER, 0, netwstick, tick_t[curticknr].maxpackets[i], totallost, loststr);
			sendall += SendInfoToSinglePlayer(conn, SC_NETCMD_RESEND, netwstick, paramstr, i, 1, 0);
		}
		Client_SetSentFlag();
	}
	return(sendall);
}
//==========================
void NETW_PLAY::SetNetworkLatency(int value, int playersetedlatency)
{
	char paramstr[100];
	switch (value)
	{
	case 0:
		netwlatency = CYCLESPERSECOND * 3 / 3;
		break;
	case 1:
		netwlatency = CYCLESPERSECOND * 2 / 3;
		break;
	case 2:
		netwlatency = CYCLESPERSECOND * 1 / 3;
		break;
	}
	if (playersetedlatency != -1)
	{
		//show message about change network latency
		sprintf(paramstr, NETWSTR(NETWORK_TBL_NETWORKLOWLATENCY + value), getplayername(playersetedlatency));
		chatbar.addbarmessage(paramstr, IDFONT10, GYELLOWCOLORFONT, TRIGGER_DISPLAYMESSAGETIME, BF_ALLOCBUF | BF_FORCEADD);
	}
}
//==========================
void NETW_PLAY::SetPauseResumeMessage(int value, int playersetedlatency, int remainpause)
{
	char paramstr[100];
	if (playersetedlatency != -1)
	{
		//show message about pause game
		sprintf(paramstr, NETWSTR(value), getplayername(playersetedlatency), remainpause);
		chatbar.addbarmessage(paramstr, IDFONT10, GYELLOWCOLORFONT, TRIGGER_DISPLAYMESSAGETIME, BF_ALLOCBUF | BF_FORCEADD);
	}
}
//==========================
void NETW_PLAY::Server_MakeAllNetCmds(void)
{
	int i;
	for (i = 0;i < MAXPLAYERS;i++)
	{
		if (i != NUMBGAMER && gameconf.pl_owner[i] == OWNER_HUMAN)
		{
			Make_PlayerNetCmds(i);
		}
	}
}
//==========================
void NETW_PLAY::Client_MakeAllNetCmds(void)
{
	int i;
	//proceed info only from server
	Make_PlayerNetCmds(SERVERGAMER);
	for (i = 0;i < MAXPLAYERS;i++)
	{
		//empty all other queue and ignore information from them if they come
		tick_t[curticknr].queue_cmdstr[i].DeallocList();
	}
}
//==========================
void NETW_PLAY::Make_PlayerNetCmds(int playernr)
{
	//make all collected info (move,...)
	PrepareNetworkCommands(playernr, &tick_t[curticknr].queue_cmdstr[playernr]);
	tick_t[curticknr].queue_cmdstr[playernr].DeallocList();
}
//==========================
void NETW_PLAY::Make_AllNetCmdsBRIEF(void)
{
	int i;
	for (i = 0;i < MAXPLAYERS;i++)
	{
		if (i != NUMBGAMER && gameconf.pl_owner[i] == OWNER_HUMAN)
		{
			Make_PlayerNetCmdsBRIEF(i);
		}
	}
}
//==========================
void NETW_PLAY::Make_PlayerNetCmdsBRIEF(int playernr)
{
	int i, maxnr;
	int netwspeed, gamespeed;
	char *cmdbuf;
	if (Check_IfServerPlayer())
	{
		tick_t[curticknr].queue_cmdstr[playernr].EnumListInit();
		while ((cmdbuf = (char *)tick_t[curticknr].queue_cmdstr[playernr].GetNextListElem()))
		{
			netwspeed = GetINTFromArray(cmdbuf, PI_NETWSPEED, 0);
			gamespeed = GetINTFromArray(cmdbuf, PI_GAMESPEED, 0);
			if (netwspeed == -1 || gamespeed == -1)
			{
				//very strange no info from server??
				DEBUGMES("error no info from server ns=%d gs=%d\n", netwspeed, gamespeed);
			}
			else
			{
				gameconf.networkconf.networklatency = netwspeed;
				gameconf.speedconf.gamespeed = gamespeed;
				GAMESPEED = gamespeed;
			}
		}
	}
	else
	{
		//i'm server - ignore all info
	}
	tick_t[curticknr].queue_cmdstr[playernr].DeallocList();
}
//==========================
void NETW_PLAY::Server_SendAllPlayersInfoToPlayers(void)
{
	int i;
	for (i = 0;i < TESTPACKETS;i++)
	{
		Server_SendInfoToPlayers(&playersconn, SC_NETCMD_GENERAL,
			networkticknr, NULL, SENDPARAM_SENDTOALLPLAYERS, TESTPACKETS, i);
	}
	networksendcycle = netwlatency;
}
//==========================
void NETW_PLAY::Client_SendMyInfoToServer(void)
{
	int i;
	for (i = 0;i < TESTPACKETS;i++)
	{
		Client_SendInfoToServer(&playersconn, SC_NETCMD_GENERAL,
			networkticknr, NULL, TESTPACKETS, i);
	}
	networksendcycle = netwlatency;
	Client_SetSentFlag();
}
//==========================
void NETW_PLAY::Client_SetSentFlag(void)
{
	tick_t[curticknr].flags |= TICK_T_FLAG_SENT;
}
//==========================
int	 NETW_PLAY::Client_CheckIfSent(void)
{
	return(tick_t[curticknr].flags&TICK_T_FLAG_SENT);
}
//==========================
void NETW_PLAY::PrepareNetworkCommands(int playernr, mylist *cmdslist)
{
	int nspeed, pausegame, i;
	char *received_cmds, cmds[512];
	cmdslist->EnumListInit();
	while ((received_cmds = (char *)cmdslist->GetNextListElem()))
	{
		nspeed = GetINTFromArray(received_cmds, PG_NSPEED_STR, 0);
		if (nspeed >= 0 && nspeed <= 2)
		{
			SetNetworkLatency(nspeed, playernr);
		}
		pausegame = GetINTFromArray(received_cmds, PG_PAUSE_STR, 0);
		if (pausegame == 1)
		{
			if (PauseGame(0, playernr))
			{
				PAUSEGAME = 1;
			}
		}
		else if (pausegame == 2)
		{
			if (ResumeGame(0, playernr))
			{
				PAUSEGAME = 0;
			}
		}
		i = 0;
		do {
			GetSTRFromArray(cmds, received_cmds, PG_CHATTEXT_STR, i++);
			if (!cmds[0])
				break;
			ShowChatMessage(cmds);
		} while (1);
		i = 0;
		do {
			GetSTRFromArray(cmds, received_cmds, PI_NETCMDS, i++);
			if (!cmds[0])
				break;
			Do_NetworkCommands(playernr, cmds);
		} while (1);
		wfree(received_cmds);
	}
}
//==========================
void NETW_PLAY::Do_NetworkCommands(int playernr, char *netcmd)
{
	//need to do network commands
}
