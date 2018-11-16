#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef WITHSDL
#include "sdl/grsdl.h"
#endif

#include "gener.h"
#include "audio.h"
#include "auxil.h"
#include "mylist.h"
#include "wmem.h"
#include "menu.h"
#include "gener.h"
#include "network_general.h"
#include "vars.h"
#include "menu.h"
#include "maps.h"
#include "debug.h"
#include "gamemenu.h"
#include "rand.h"
#include "netwgame.h"

//#define TESTMAP_NETWORKDOWNLOAD

char	PORTIONFILEBUFF[PORTIONFILESIZE];
//=============================================
//newgames NETGAME structures
//gamelist only game names special for lists in menu
mylist newgames, gamelist;
mylist connectedplayers, bannedips;				//connected players to my game,banned ips
NEWNETWGAME newnetwgame;
//=============================================
int GetINTFromArray(char *allarray, const char *str, int fromid)
{
	int i;
	int len = strlen(str);
	char *find;
	for (i = 0;i < fromid;i++)
		allarray = strstr(allarray, str) + 1;
	find = strstr(allarray, str);
	if (find)
	{
		return(atoi(find + len + 1));
	}
	return(-1);
}
//=============================================
int GetHEXFromArray(char *allarray, const char *str, int fromid)
{
	int i;
	int len = strlen(str);
	char *find;
	for (i = 0;i < fromid;i++)
		allarray = strstr(allarray, str) + 1;
	find = strstr(allarray, str);
	if (find)
	{
		return(strtoll(find + len + 1, NULL, 16));
	}
	return(-1);
}
//=============================================
void GetSTRFromArray(char *dest, char *allarray, const char *str, int fromid)
{
	char *find;
	int i = 0, j;
	int len = strlen(str);
	for (j = 0;j < fromid;j++)
		allarray = strstr(allarray, str) + 1;
	find = strstr(allarray, str);
	if (find)
	{
		while (find[i + len + 1] != PI_END[0])
		{
			dest[i] = find[i + len + 1];
			i++;
		}
	}
	dest[i] = 0;
}
//=============================================
void GetBYTESFromArray(char *destbuf, char *allarray, int buffsize, const char *str)
{
	char *find;
	int len = strlen(str);
	find = strstr(allarray, str);
	if (find)
	{
		memcpy(destbuf, find + len + 1, buffsize);
	}
}
//=============================================
int UpdateAllGamesTime(void)
{
	int i, max, maxnr, updatelists = 0;
	NETGAME *crgame;
	newgames.EnumListInit();
	while ((crgame = (NETGAME *)newgames.GetNextListElem(&i)))
	{
		if (--crgame->timeout <= 0)
		{
			//free game element, no game anymore
			wfree(crgame);
			newgames.DelList(i);
			updatelists++;
		}
	}
	return(updatelists);
}
//=============================================
//return 1 if need update lists in menu
//=============================================
int AddListCreatedGame(NETWORK_JOIN_PACKET *mypacket, struct sockaddr_in *Addr)
{
	int i, max, maxnr;
	NETGAME *crgame;
	newgames.EnumListInit();
	while ((crgame = (NETGAME *)newgames.GetNextListElem()))
	{
		if (crgame->PCID == mypacket->PCID)
		{
			switch (mypacket->COMMAND)
			{
			case STARCLONE_NETWORKJOIN_COMMAND_CREATEDGAME:
				//update info for game (game owner wait for connection people)
				//need to add some time to continue waiting;
				crgame->timeout = TIMEOUT_NETWORK_PACKETS;
				return(0);
				break;
			case STARCLONE_NETWORKJOIN_COMMAND_CLOSEGAME:
				crgame->timeout = 1;				//no timeout
				return(1);
				break;
			default:
				printf("unknown info\n");
				break;
			}
		}
	}
	//not found in listed (new created game, add to list)
	if (max == MAXCREATEDGAMES)
	{
		//sorry we don't have any places for newed games
		return(0);
	}
	//create a new game
	crgame = (NETGAME *)wmalloc(sizeof(NETGAME));
	memset(crgame, 0, sizeof(NETGAME));
	crgame->PCID = mypacket->PCID;
	crgame->TypeGame = GetINTFromArray(mypacket->data, PI_TYPEGAME, 0);
	crgame->SpeedGame = GetINTFromArray(mypacket->data, PI_GAMESPEED, 0);
	crgame->mapsizex = GetINTFromArray(mypacket->data, PI_SIZEXMAP, 0);
	crgame->mapsizey = GetINTFromArray(mypacket->data, PI_SIZEYMAP, 0);
	crgame->connectport = GetINTFromArray(mypacket->data, PI_PORT, 0);
	memcpy(&crgame->Addr, Addr, sizeof(struct sockaddr_in));
	GetSTRFromArray(crgame->gametitle, mypacket->data, PI_GAMETITLE, 0);
	GetSTRFromArray(crgame->mapname, mypacket->data, PI_MAPNAME, 0);
	newgames.AddList(crgame);
	return(1);
}
//=============================================
void UpdateGamesList(MENUSTR *allmenus, int listitem)
{
	int i, max = 0, maxnr, selectnr;
	gamelist.FlushList();
	NETGAME *crgame;
	newgames.EnumListInit();
	while ((crgame = (NETGAME *)newgames.GetNextListElem()))
	{
		gamelist.AddList(crgame->gametitle);
		max++;
	}
	if (!max)
	{
		selectnr = -1;
	}
	else
		selectnr = allmenus->menu[listitem].item.listbox->selectednr;
	setlistbox_lists(allmenus, listitem, selectnr, &gamelist);
}
//=============================================
void UpdateGameInfo(MENUSTR *allmenus, int listnr, int selectnr)
{
	int i, max, maxnr;
	char *gametitle;
	char txt1[10];
	NETGAME *crgame;
	if (selectnr == -1)
	{
		setmenuitem_VISIBLED(allmenus, 12, FALSE);
		setmenuitem_VISIBLED(allmenus, 13, FALSE);
		setmenuitem_VISIBLED(allmenus, 15, FALSE);
		setmenuitem_VISIBLED(allmenus, 16, FALSE);
		setmenuitem_VISIBLED(allmenus, 17, FALSE);
		setmenuitem_DISABLED(allmenus, 18, TRUE);//ok
		return;
	}
	gametitle = (char *)allmenus->menu[listnr].item.listbox->flist->GetElemNr(selectnr);
	newgames.EnumListInit();
	while ((crgame = (NETGAME *)newgames.GetNextListElem()))
	{
		if (!strcmp(crgame->gametitle, gametitle))
		{
			//fill all map info
			changetextitem(allmenus, 12, crgame->gametitle);
			allmenus->menu[12].colors4 = MTEXTCOLORWHITE;
			setmenuitem_VISIBLED(allmenus, 12, TRUE);

			changetextitem(allmenus, 13, alldattbl.myinfo_tbl->get_TBL_STR(MYINFO_TBL_USEMAPSETT + crgame->TypeGame));
			allmenus->menu[13].colors4 = MTEXTCOLORWHITE;
			setmenuitem_VISIBLED(allmenus, 13, TRUE);

			changetextitem(allmenus, 15, crgame->mapname);
			allmenus->menu[15].colors4 = MTEXTCOLORWHITE;
			setmenuitem_VISIBLED(allmenus, 15, TRUE);

			sprintf(txt1, alldattbl.gluall_tbl->get_TBL_STR(GLUALL_TBL_MAPSIZEXY), crgame->mapsizex, crgame->mapsizey);
			changetextitem(allmenus, 16, txt1);
			setmenuitem_VISIBLED(allmenus, 16, TRUE);

			changetextitem(allmenus, 17, alldattbl.gluall_tbl->get_TBL_STR(GLUALL_TBL_SPEED_SLOWEST + crgame->SpeedGame));
			setmenuitem_VISIBLED(allmenus, 17, TRUE);

			setmenuitem_DISABLED(allmenus, 18, FALSE);//ok
			return;
		}
	}
}
//=============================================
void EmptyAllGamesList(void)
{
	newgames.DeallocList();
}
//=============================================
int GetGameId(int sellist, char *gametitle)
{
	int maxnr, max, i;
	NETGAME *crgame;
	newgames.EnumListInit();
	while ((crgame = (NETGAME *)newgames.GetNextListElem()))
	{
		if (!strcmp(crgame->gametitle, gametitle))
		{
			return(crgame->PCID);
		}
	}
	return(0);
}
//==========================================
int CheckForGameName(char *gametitle)
{
	int i, max, maxnr;
	NETGAME *crgame;
	if (max == MAXCREATEDGAMES)
		return(3);
	newgames.EnumListInit();
	while ((crgame = (NETGAME *)newgames.GetNextListElem()))
	{
		if (!strcmp(crgame->gametitle, gametitle))
		{
			return(2);
		}
	}
	return(1);
}
//==========================================
int GetGameConnectedSlot(int playerslot)
{
	return(newnetwgame.slot1[playerslot]);
}
//=============================================
int GetGameValueSlot(int playerslot)
{
	return(newnetwgame.slot2[playerslot]);
}
//=============================================
void SetGameConnectedSlot(int playerslot, int value)
{
	newnetwgame.slot1[playerslot] = value;
}
//=============================================
void SetGameValueSlot(int playerslot, int value)
{
	newnetwgame.slot2[playerslot] = value;
}
//=============================================
int GetMapPlayers(void)
{
	int i, retval = 0;
	for (i = 0;i < MAXPLAYERS;i++)
	{
		if (newnetwgame.slot1[i] != GAMENOSLOT)
			retval++;
	}
	return(retval);
}
//=============================================
int GetPlayersMapBits(void)
{
	int i, retval = 0;
	for (i = 0;i < PLAYEDPLAYERS;i++)
	{
		retval |= newnetwgame.slot1[i];
		if (i != PLAYEDPLAYERS - 1)
			retval <<= 4;
	}
	return(retval);
}
//=============================================
int GetSlotsMapBits(void)
{
	int i, retval = 0;
	for (i = 0;i < PLAYEDPLAYERS;i++)
	{
		retval |= newnetwgame.slot2[i];
		if (i != PLAYEDPLAYERS - 1)
			retval <<= 4;
	}
	return(retval);
}
//=============================================
void CreateGameSlots(int usedslots, int playernr)
{
	int i, forcenr = 0;
	for (i = 0;i < usedslots;i++)
	{
		if (force_slots.usedslots1[i] == OWNER_FORCENAME)
		{
			newnetwgame.slot1[i] = GAMEFORCENAMESLOT;
			newnetwgame.slot2[i] = forcenr++;
		}
		else
		{
			if (i == playernr)
				newnetwgame.slot1[i] = OWNER_HUMAN;
			else
				newnetwgame.slot1[i] = GAMEOPENSLOT;
			newnetwgame.slot2[i] = PLAYERRANDOMSLOT;
		}
	}
	for (;i < MAXPLAYERS;i++)
	{
		newnetwgame.slot1[i] = GAMENOSLOT;
		newnetwgame.slot2[i] = PLAYERNOSLOT;
	}
}
//==========================================
int CheckEmptySlot(void)
{
	int i;
	for (i = 0;i < MAXPLAYERS;i++)
		if (newnetwgame.slot1[i] == GAMEOPENSLOT)
			return(i);
	return(CANNOTJOINME);
}
//==========================================
int CheckForDublicateName(char *newname)
{
	int i, max, maxnr;
	NETWPLAYERINFO *pl;
	//check the host game playername
	if (!strcmp(getplayername(NUMBGAMER), newname))
		return(CANNOTJOINMENAMEEXISTS);
	//check for connected player names
	connectedplayers.EnumListInit();
	while ((pl = (NETWPLAYERINFO *)connectedplayers.GetNextListElem()))
	{
		if (!strcmp(pl->playername, newname))
		{
			return(CANNOTJOINMENAMEEXISTS);
		}
	}
	return(CANJOIN);
}
//==========================================
void AddToBanList(int playerslot)
{
	int i, max, maxnr;
	NETWPLAYERINFO *pl;
	BANNEDIPS *tempip;
	connectedplayers.EnumListInit();
	while ((pl = (NETWPLAYERINFO *)connectedplayers.GetNextListElem()))
	{
		if (pl->playernr == playerslot)
		{
			tempip = (BANNEDIPS *)wmalloc(sizeof(BANNEDIPS));
			memcpy(tempip, &pl->Addr, sizeof(BANNEDIPS));
			bannedips.AddList(tempip);
			return;
		}
	}
	printf("error: cannot ban, player not found\n");
}
//==========================================
int CheckForBan(struct sockaddr_in *newaddr)
{
	int i, max, maxnr;
	BANNEDIPS *tempip;
	bannedips.EnumListInit();
	while ((tempip = (BANNEDIPS *)bannedips.GetNextListElem()))
	{
		if (tempip->Addr.sin_addr.s_addr == newaddr->sin_addr.s_addr)
		{
			return(CANNOTJOINMEBANNED);
		}
	}
	return(CANJOIN);
}
//==========================================
int Master_CheckJoinPacket(MENUSTR *allmenus, NETWORK_INFO *sendsock, NETWORK_INFO *recvsock, NETWPLAYERINFO *info)
{
	int max, maxnr, SLOT, RACESLOT, MAPSEND;
	int i, slotnr, err, frompos, plnr;
	NETWPLAYERINFO *pl;
	NETWORK_JOIN_PACKET *mypacket = (NETWORK_JOIN_PACKET *)recvsock->recvbuffer;
	if (recvsock->PCID == mypacket->PCID) //the packet is realy come to me?
	{
		switch (mypacket->COMMAND)
		{
		case STARCLONE_NETWORKJOIN_COMMAND_JOINREQUEST:
			info->PCID = GetINTFromArray(mypacket->data, PI_PCID, 0);
			info->useport = GetINTFromArray(mypacket->data, PI_PORT, 0);
			GetSTRFromArray(info->playername, mypacket->data, PI_PLAYERNAME, 0);
			memcpy(&info->Addr, &recvsock->Addr, sizeof(sockaddr_in));
			err = CheckForBan(&info->Addr);
			if (err)
				return(err);
			err = CheckForDublicateName(info->playername);
			if (err)
				return(err);
			info->playernr = CheckEmptySlot();
			info->timeout = TIMEOUT_NETWORK_PACKETS;
			return(info->playernr);					//return appropiate empty slot if any
			break;
		case STARCLONE_NETWORKJOIN_COMMAND_PLAYERINFO:
			SLOT = GetINTFromArray(mypacket->data, PI_PLAYERNR, 0);
			RACESLOT = GetINTFromArray(mypacket->data, PI_SECONDSLOT, 0);
			MAPSEND = GetINTFromArray(mypacket->data, PI_NEEDMAPSEND, 0);
			connectedplayers.EnumListInit();
			while ((pl = (NETWPLAYERINFO *)connectedplayers.GetNextListElem()))
			{
				if (pl->playernr == SLOT)
				{
					memcpy(info, pl, sizeof(NETWPLAYERINFO));
					pl->timeout = TIMEOUT_NETWORK_PACKETS;
					setslotinfo(allmenus, SLOT, OWNERGAMER, pl->playername, GAMEPLAYERSLOT, RACESLOT);
					if (MAPSEND == 1)
					{
						pl->flag |= FLAGS_PLAYERNEEDUPLOADMAP;
						return(NEEDUPLOADMAP);
					}
					else
					{
						if (MAPSEND == 0)
							pl->flag &= ~FLAGS_PLAYERNEEDUPLOADMAP;
						return(REFRESHPLAYERSAFTER);
					}
				}
			}
			return(ABUSEMEFROMUNKNOWN);
			break;
		case STARCLONE_NETWORKJOIN_COMMAND_MESSAGE:
			SLOT = GetINTFromArray(mypacket->data, PI_PLAYERNR, 0);
			GetSTRFromArray(TXTCHAT, mypacket->data, PI_TEXTCHAT, 0);
			connectedplayers.EnumListInit();
			while ((pl = (NETWPLAYERINFO *)connectedplayers.GetNextListElem()))
			{
				if (pl->playernr == SLOT)
				{
					memcpy(info, pl, sizeof(NETWPLAYERINFO));
					pl->timeout = TIMEOUT_NETWORK_PACKETS;
					if (TXTCHAT[0])
					{
						SendTextToChatItem(allmenus, 10, TXTCHAT);
						SendTextToConnectedPlayers(pl->playernr, TXTCHAT, sendsock, recvsock);
						TXTCHAT[0] = 0;
					}
					return(REFRESHPLAYERSAFTER);
				}
			}
			return(ABUSEMEFROMUNKNOWN);
			break;
		case STARCLONE_NETWORKJOIN_COMMAND_LEAVEGAME:
			SLOT = GetINTFromArray(mypacket->data, PI_PLAYERNR, 0);
			connectedplayers.EnumListInit();
			while ((pl = (NETWPLAYERINFO *)connectedplayers.GetNextListElem()))
			{
				if (pl->playernr == SLOT)
				{
					pl->timeout = 1;
					pl->flag |= FLAGS_PLAYERLEAVEDBYCOMMAND;
					return(REFRESHPLAYERSAFTER);
				}
			}
			return(ABUSEMEFROMUNKNOWN);
			break;
		case STARCLONE_NETWORKJOIN_COMMAND_GIVENEXTPARTOFFILE:
			plnr = GetINTFromArray(mypacket->data, PI_PLAYERNR, 0);
			MAPLOADEDLENGTH = GetINTFromArray(mypacket->data, PI_FILEPOS, 0);
			BuffPrepareForSend(SELECTMAP, MAPLOADEDLENGTH, PORTIONFILESIZE, PORTIONFILEBUFF);
			SendCommandToPlayer(allmenus, plnr, STARCLONE_NETWORKJOIN_COMMAND_FILEBYTES, sendsock, recvsock);

			setmenuitem_VISIBLED(allmenus, 35 + plnr * 4, TRUE);
			setmenuitem_DISABLED(allmenus, 35 + plnr * 4, FALSE);
			setimageitem_visibilityxy(allmenus, 35 + plnr * 4,
				allmenus->menu[35 + plnr * 4].hotxsize*MAPLOADEDLENGTH / FILESIZEMAP,
				allmenus->menu[35 + plnr * 4].hotysize);
			return(DOWNLOADPARTOK);
			break;
			//			case STARCLONE_NETWORKJOIN_COMMAND_PAUSERESUMEGAME:
			//				PAUSEGAME^=1;
			//				return();NEEDPAUSE
		default:
			printf("MASTER:wrong command come\n");
			break;
		}
	}
	else
		printf("MASTER:wrong PCID %x %x\n", recvsock->PCID, mypacket->PCID);
	return(IGNOREJOINTOME);
}
//==========================================
//return values,0,1,2,3-10,	   ,0x8001-0x8007
int Guest_CheckJoinPacket(MENUSTR *allmenus, NETWORK_INFO *sendsock, NETWORK_INFO *recvsock)
{
	int maxpl, pl, slots, i, j, SLOTOWNER, SLOTRACE;
	int gameslot, raceslot, buffsize, frombytes, chksumm1, chksumm2;
	int forces;
	unsigned int PLTYPE, PLIP, PLPCID;
	int oldx;
	char txt[200];
	NETWORK_JOIN_PACKET *mypacket = (NETWORK_JOIN_PACKET *)recvsock->recvbuffer;
	if (recvsock->PCID == mypacket->PCID) //the packet is realy come to me?
	{
		switch (mypacket->COMMAND)
		{
		case STARCLONE_NETWORKJOIN_COMMAND_YOUJOINED:
			SERVER_PCID = GetINTFromArray(mypacket->data, PI_PCID, 0);
			GAMETYPE = GetINTFromArray(mypacket->data, PI_TYPEGAME, 0);
			GAMESPEED = GetINTFromArray(mypacket->data, PI_GAMESPEED, 0);
			MAPSIZEX = GetINTFromArray(mypacket->data, PI_SIZEXMAP, 0);
			MAPSIZEY = GetINTFromArray(mypacket->data, PI_SIZEYMAP, 0);
			strcpy(GAMEHOSTIP, inet_ntoa(recvsock->Addr.sin_addr));
			PORT = GetINTFromArray(mypacket->data, PI_PORT, 0);
			GetSTRFromArray(GAMETITLESTR, mypacket->data, PI_GAMETITLE, 0);
			GAMETITLE = GAMETITLESTR;
			GetSTRFromArray(MAPNAME, mypacket->data, PI_MAPNAME, 0);
			MAXPLRS = GetINTFromArray(mypacket->data, PI_MAXPLAYERS, 0);
			JOINSLOT = GetINTFromArray(mypacket->data, PI_JOINSLOT, 0);
			return(JOINSLOT + 3);
			break;
		case STARCLONE_NETWORKJOIN_COMMAND_FULLSLOTS:
			return(1);
		case STARCLONE_NETWORKJOIN_COMMAND_ALLPLAYERSINFO:
			if (!SERVER_PCID)
				return(0);
			maxpl = GetINTFromArray(mypacket->data, PI_MAXPLAYERS, 0);
			FILESIZEMAP = GetINTFromArray(mypacket->data, PI_FILESIZEMAP, 0);
			CHECKSUMMMAP = GetHEXFromArray(mypacket->data, PI_MAPCHKSUM, 0);
			if (!MAPVERIFIED && !MAPUPLOAD)
			{
				GetSTRFromArray(txt, mypacket->data, PI_MAPFILE, 0);

#ifdef TESTMAP_NETWORKDOWNLOAD
				sprintf(SELECTMAP, "%s/%s%d", GAMEMAPPATH, txt, NUMBGAMER);		//!!!!for test of download on local machines
#else
				strcpy(SELECTMAP, GAMEMAPPATH);
				strcat(SELECTMAP, "/");
				strcat(SELECTMAP, txt);
#endif
				//check for file existance
				if (access(SELECTMAP, R_OK))
				{
					//file didnt exist need upload
					MAPUPLOAD = 1;
					MAPVERIFIED = 0;
				}
				else
				{
					//verify checksumm
					if (CHECKSUMMMAP == mkchecksumm(SELECTMAP))
					{
						MAPUPLOAD = 0;
						MAPVERIFIED = 1;
					}
					else
					{
						MAPUPLOAD = 1;
						MAPVERIFIED = 0;
					}
				}
			}
			force_slots.Clear();
			force_slots.openedslots = maxpl;
			for (i = 0;i < maxpl;i++)
			{
				if (i != NUMBGAMER)
				{
					PLTYPE = GetHEXFromArray(mypacket->data, PI_PLAYERTYPE, i);
					GetSTRFromArray(txt, mypacket->data, PI_PLAYERNAME, i);
					gameslot = PLTYPE >> 16;
					raceslot = PLTYPE & 0xffff;
					force_slots.usedslots1[i] = gameslot;
					force_slots.usedslots2[i] = raceslot;
					if (gameslot == GAMEFORCENAMESLOT)
					{
						force_slots.AddForceName(force_slots.forces++, txt);
					}
					else
					{
						setplayername(i, txt);
					}
					setslotinfo(allmenus, i, GUESTGAMER, txt, gameslot, raceslot);
				}
				else
				{
					//the info was settend by glu_chat
					//now we show it
					setmenuitem_VISIBLED(allmenus, NUMBGAMER * 4 + 33, TRUE);
					setmenuitem_VISIBLED(allmenus, NUMBGAMER * 4 + 34, TRUE);
					setmenuitem_DISABLED(allmenus, NUMBGAMER * 4 + 33, FALSE);
					setmenuitem_DISABLED(allmenus, NUMBGAMER * 4 + 34, FALSE);
				}
			}
			if (!force_slots.forces)
				force_slots.forces = 1;
			return(1);
			break;
		case STARCLONE_NETWORKJOIN_COMMAND_MESSAGE:
			if (!SERVER_PCID)
				return(0);
			pl = GetINTFromArray(mypacket->data, PI_PLAYERNR, 0);
			GetSTRFromArray(txt, mypacket->data, PI_TEXTCHAT, 0);
			if (txt[0])
			{
				if (pl == -1) //system message
				{
					SendTextToChatItem(allmenus, 10, txt);
				}
				else
				{
					SendTextToChatItem(allmenus, 10, txt);
				}
			}
			break;
		case STARCLONE_NETWORKJOIN_COMMAND_ASKFORFILE:
			if (!SERVER_PCID)
				return(0);
			if (MAPUPLOAD)
			{
				return(REQUESTFILEPARTS);
			}
			else
				;//wrong no need to be here
			break;
		case STARCLONE_NETWORKJOIN_COMMAND_FILEBYTES:
			if (!SERVER_PCID)
				return(0);
			buffsize = GetINTFromArray(mypacket->data, PI_PORTIONSIZE, 0);
			chksumm1 = GetHEXFromArray(mypacket->data, PI_CHKSUMMBYTES, 0);
			frombytes = GetINTFromArray(mypacket->data, PI_FILEPOS, 0);
			if (buffsize > PORTIONFILESIZE)
				printf("filebuffer is too big, not from server\n");
			else if (frombytes < MAPLOADEDLENGTH)
			{
				printf("same packet may be, already received\n");
			}
			else
			{
				GetBYTESFromArray(PORTIONFILEBUFF, mypacket->data, buffsize, PI_BYTES);
				chksumm2 = CreateCRC((unsigned char *)PORTIONFILEBUFF, buffsize, BUFF_NOCRYPTIZE);
				if (chksumm1 != chksumm2)
				{
					//						BuffPrepareForWrite(SELECTMAP,frombytes,buffsize,PORTIONFILEBUFF);
					printf("error checksumm packet from=%u size=%d chksumm1=%x chksumm2=%x\n", frombytes, buffsize, chksumm1, chksumm2);
				}
				else
				{
					if (MAPLOADEDLENGTH + PORTIONFILESIZE > FILESIZEMAP)
					{
						buffsize = FILESIZEMAP - MAPLOADEDLENGTH;
					}
					BuffPrepareForWrite(SELECTMAP, frombytes, buffsize, PORTIONFILEBUFF);
					MAPLOADEDLENGTH += buffsize;
					setmenuitem_VISIBLED(allmenus, 35 + NUMBGAMER * 4, TRUE);
					setmenuitem_DISABLED(allmenus, 35 + NUMBGAMER * 4, FALSE);
					setimageitem_visibilityxy(allmenus, 35 + NUMBGAMER * 4,
						allmenus->menu[35 + NUMBGAMER * 4].hotxsize*MAPLOADEDLENGTH / FILESIZEMAP,
						allmenus->menu[35 + NUMBGAMER * 4].hotysize);
					//						printf("downloading=%u FILESIZEMAP=%u\n",MAPLOADEDLENGTH,FILESIZEMAP);
					if (MAPLOADEDLENGTH == FILESIZEMAP)
					{
						MAPUPLOAD = 0;
						if (CHECKSUMMMAP == mkchecksumm(SELECTMAP))
						{
							MAPVERIFIED = 1;
						}
						else
						{
							MAPVERIFIED = 0;
							return(CALLBACKCANCELFROMMENU8);
						}
					}
					else
						RequestFileParts(allmenus, sendsock, recvsock);
				}
			}
			return(0);
			break;
		case STARCLONE_NETWORKJOIN_COMMAND_PLAYGAME:
			MAXPLRS = GetINTFromArray(mypacket->data, PI_PLAYERS, 0);
			RNDSEED = GetINTFromArray(mypacket->data, PI_RNDSEED, 0);
			for (i = 0, j = 0;i < MAXPLAYERS;i++)
			{
				PLTYPE = GetHEXFromArray(mypacket->data, PI_PLAYERTYPE, i);
				SLOTOWNER = PLTYPE >> 16;
				SLOTRACE = PLTYPE & 0xffff;
				PLIP = GetHEXFromArray(mypacket->data, PI_PLAYERIP, i);
				PLPCID = GetINTFromArray(mypacket->data, PI_PCID, i);
				if (SLOTOWNER != GAMEFORCENAMESLOT)
				{
					if (j == 0)//if hostgame
						PLAYER[i].Addr.sin_addr.s_addr = inet_addr(GAMEHOSTIP);
					else
						PLAYER[i].Addr.sin_addr.s_addr = PLIP;
					PLAYER[i].Addr.sin_family = AF_INET;
					PLAYER[i].Addr.sin_port = htons(STARCLONE_UDP_PLAYPORT);
					PLAYER[i].PCID = PLPCID;

					gameconf.pl_owner[i] = SLOTOWNER;
					gameconf.pl_race[i] = SLOTRACE;
					j++;
				}
				else
				{
					gameconf.pl_owner[i] = OWNER_FORCENAME;
					gameconf.pl_race[i] = RACE_INACTIVE;
				}
			}
			return(CALLBACKCANCELFROMMENU3);
			break;
		case STARCLONE_NETWORKJOIN_COMMAND_PAUSERESUMEGAME:
			if (!SERVER_PCID)
				return(0);
			PAUSEGAME ^= 1;
			return(0);
		case STARCLONE_NETWORKJOIN_COMMAND_CLOSEGAME:
			return(CALLBACKCANCELFROMMENU2);
		case STARCLONE_NETWORKJOIN_COMMAND_BANPLAYER:
			return(CALLBACKCANCELFROMMENU5);
		case STARCLONE_NETWORKJOIN_COMMAND_ISBANNED:
			return(CALLBACKCANCELFROMMENU6);
		case STARCLONE_NETWORKJOIN_COMMAND_NAMEEXISTS:
			return(CALLBACKCANCELFROMMENU7);
		default:
			printf("GUEST:wrong command come\n");
			break;
		}
	}
	else
		printf("GUEST:wrong PCID\n");
	return(0);
}
//==========================================
int FillNetworkAddr(NETWORK_INFO *info, int PCID)
{
	int i, max, maxnr, updatelists = 0;
	NETGAME *crgame;
	newgames.EnumListInit();
	while ((crgame = (NETGAME *)newgames.GetNextListElem(&i)))
	{
		if (crgame->PCID == PCID)
		{
			info->Addr.sin_family = crgame->Addr.sin_family;
			info->Addr.sin_port = htons(crgame->connectport);
			info->Addr.sin_addr.s_addr = crgame->Addr.sin_addr.s_addr;
			return(NETWORK_OK);
		}
	}
	return(NETWORK_ERROR_INIT);
}
//==========================================
void EmptyConnectedPlayers(void)
{
	connectedplayers.DeallocList();
	bannedips.DeallocList();
}
//==========================================
void setslotinfo(MENUSTR *allmenus, int nr, int masterguest, char *plname, int plslot, int rsslot)
{
	char txt1[100];
	char txt2[40];
	setmenuitem_VISIBLED(allmenus, 33 + nr * 4, TRUE);
	setmenuitem_VISIBLED(allmenus, 34 + nr * 4, TRUE);
	if (plname&&plname[0])
	{
		//connected player
		strcpy(txt1, plname);
		if (masterguest == OWNERGAMER)
		{
			strcat(txt1, "\n");
			strcat(txt1, GLUALLSTR(GLUALL_TBL_BANPLAYERLIST));
			//			setmenuitem_DISABLED(allmenus,33+nr*4,FALSE);
			setmenuitem_DISABLED(allmenus, 34 + nr * 4, TRUE);
		}
		if (plslot == GAMEFORCENAMESLOT)
		{
			addexpanditem_lists(allmenus, 32 + nr * 4, 0, txt1);
			setmenuitem_VISIBLED(allmenus, 32 + nr * 4, TRUE);
			setmenuitem_VISIBLED(allmenus, 33 + nr * 4, FALSE);
			setmenuitem_VISIBLED(allmenus, 34 + nr * 4, FALSE);
		}
		else
		{
			addexpanditem_lists(allmenus, 33 + nr * 4, 0, txt1);
			setmenuitem_VISIBLED(allmenus, 32 + nr * 4, FALSE);
			setmenuitem_VISIBLED(allmenus, 33 + nr * 4, TRUE);
			setmenuitem_VISIBLED(allmenus, 34 + nr * 4, TRUE);
		}
	}
	else
	{
		//leaved player
		strcpy(txt2, GLUALLSTR(GLUALL_TBL_OPENLIST));
		strcat(txt2, "\n");
		strcat(txt2, GLUALLSTR(GLUALL_TBL_CLOSEDLIST));
		strcat(txt2, "\n");
		strcat(txt2, GLUALLSTR(GLUALL_TBL_COMPUTERLIST));
		addexpanditem_lists(allmenus, 33 + nr * 4, plslot, txt2);
		setmenuitem_VISIBLED(allmenus, 35 + nr * 4, FALSE);	//disable progress of download
		if (masterguest == OWNERGAMER)
		{
			setmenuitem_DISABLED(allmenus, 33 + nr * 4, FALSE);
			setmenuitem_DISABLED(allmenus, 34 + nr * 4, FALSE);
		}
	}
	if (plslot == GAMECLOSEDSLOT || plslot == GAMEOPENSLOT || plslot == GAMEFORCENAMESLOT || plslot == GAMENOSLOT)
		setmenuitem_VISIBLED(allmenus, 34 + nr * 4, FALSE);
	else
		setmenuitem_VISIBLED(allmenus, 34 + nr * 4, TRUE);
	changeexpanditemnr(allmenus, 34 + nr * 4, rsslot);
	SetGameConnectedSlot(nr, plslot);
	SetGameValueSlot(nr, rsslot);
}
//==========================================
int UpdatePlayersInfo(MENUSTR *allmenus, NETWORK_INFO *sendsock, NETWORK_INFO *recvsock)
{
	int i, max, maxnr, updateplayers = 0;
	NETWPLAYERINFO *pl;
	char txt1[100];
	connectedplayers.EnumListInit();
	while ((pl = (NETWPLAYERINFO *)connectedplayers.GetNextListElem(&i)))
	{
		if (--pl->timeout <= 0)
		{
			//free player element, no player anymore
			setslotinfo(allmenus, pl->playernr, OWNERGAMER, NULL, GAMEOPENSLOT, PLAYERRANDOMSLOT);
			txt1[0] = MYELLOWCOLORFONT;
			if (pl->flag&FLAGS_PLAYERLEAVEDBYCOMMAND)
				sprintf(txt1 + 1, NETWSTR(NETWORK_TBL_PLAYERLEAVE), pl->playername);
			else
				if (pl->flag&FLAGS_PLAYERLEAVEDBYBAN)
					sprintf(txt1 + 1, MYTBLSTR(MYINFO_TBL_PLAYERBANNED), pl->playername);
				else
					sprintf(txt1 + 1, NETWSTR(NETWORK_TBL_PLAYERLEAVETIMEOUT), pl->playername);
			setplayername(pl->playernr, "");
			SendTextToChatItem(allmenus, 10, txt1);
			wfree(pl);
			connectedplayers.DelList(i);
			SendTextToConnectedPlayers(SYSTEMCHAT, txt1, sendsock, recvsock);
			updateplayers++;
		}
	}
	return(updateplayers);
}
//==========================================
int AddConnectedPlayer(MENUSTR *allmenus, NETWPLAYERINFO *newplayer)
{
	NETWPLAYERINFO *pl;
	pl = (NETWPLAYERINFO *)wmalloc(sizeof(NETWPLAYERINFO));
	memcpy(pl, newplayer, sizeof(NETWPLAYERINFO));
	connectedplayers.AddList(pl);
	return(1);
}
//==========================================
int GetPlayerTypeJoin(int playerslot)
{
	if (force_slots.usedslots1[playerslot] == OWNER_FORCENAME)
		return((force_slots.usedslots1[playerslot] << 16) | (force_slots.usedslots2[playerslot]));
	else
		return((newnetwgame.slot1[playerslot] << 16) | newnetwgame.slot2[playerslot]);
}
//==========================================
int GetPlayerType(int playerslot)
{
	return((gameconf.pl_owner[playerslot] << 16) | gameconf.pl_race[playerslot]);
}
//==========================================
char *GetSlotName(int playerslot)
{
	if (force_slots.usedslots1[playerslot] == OWNER_FORCENAME)
		return(force_slots.forcenames[force_slots.usedslots2[playerslot]]);
	else
		return(getplayername(playerslot));
}
//==========================================
int GetPlayerIp(int playerslot)
{
	return(PLAYER[playerslot].Addr.sin_addr.s_addr);
}
//==========================================
int GetPlayerPCID(int playerslot)
{
	int i, max, maxnr;
	NETWPLAYERINFO *pl;
	if (playerslot == 0)
	{
		//host game
		return(MY_PCID);
	}
	connectedplayers.EnumListInit();
	while ((pl = (NETWPLAYERINFO *)connectedplayers.GetNextListElem()))
	{
		if (pl->playernr == playerslot)
		{
			return(pl->PCID);
		}
	}
	return(0x00000000);
}
//==========================================
int CheckForPlayersReady(void)
{
	int i, max, maxnr;
	NETWPLAYERINFO *pl;
	connectedplayers.EnumListInit();
	while ((pl = (NETWPLAYERINFO *)connectedplayers.GetNextListElem()))
	{
		if (pl->flag&FLAGS_PLAYERNEEDUPLOADMAP)
			return(1);
	}
	return(0);
}
//==========================================
int SendInfoToMaster(int command, const char *txtchat, NETWORK_INFO *sendsock, NETWORK_INFO *recvsock)
{
	int i, err;
	if (!SERVER_PCID)
		return(NETWORK_ERROR_SEND);
	sendsock->Addr.sin_family = AF_INET;
	sendsock->Addr.sin_port = htons(PORT);
	err = inet_aton(GAMEHOSTIP, &sendsock->Addr.sin_addr) == 0;
	sendsock->COMMAND = command;
	sendsock->PCID = SERVER_PCID;
	switch (command)
	{
	case STARCLONE_NETWORKJOIN_COMMAND_LEAVEGAME:
		sprintf(sendsock->sendstr, SENDLEAVEGAME_STR, NUMBGAMER);
		break;
	case STARCLONE_NETWORKJOIN_COMMAND_PLAYERINFO:
		sprintf(sendsock->sendstr, SENDPLAYERINFO_STR,
			NUMBGAMER,
			GetGameValueSlot(NUMBGAMER),
			MAPUPLOAD);
		break;
	case STARCLONE_NETWORKJOIN_COMMAND_MESSAGE:
		sprintf(sendsock->sendstr, SENDMESSAGETOPLAYERS_STR,
			NUMBGAMER,
			txtchat);
		break;
	case STARCLONE_NETWORKJOIN_COMMAND_GIVENEXTPARTOFFILE:
		sprintf(sendsock->sendstr, SENDFILEINFO_STR,
			NUMBGAMER,
			MAPLOADEDLENGTH);
		break;
	}
	err = CallNetwork(NETWORK_SENDPACKET, sendsock);
	return(err);

}
//==========================================
int SendTextToConnectedPlayers(int playernr, char *txt, NETWORK_INFO *sendsock, NETWORK_INFO *recvsock)
{
	int i, max, maxnr, updateplayers = 0, err;
	NETWPLAYERINFO *pl;
	connectedplayers.EnumListInit();
	while ((pl = (NETWPLAYERINFO *)connectedplayers.GetNextListElem()))
	{
		if (pl->playernr != playernr)						//do not send to player who send
		{
			sendsock->Addr.sin_family = AF_INET;
			sendsock->Addr.sin_port = htons(pl->useport);
			sendsock->Addr.sin_addr.s_addr = pl->Addr.sin_addr.s_addr;
			sendsock->COMMAND = STARCLONE_NETWORKJOIN_COMMAND_MESSAGE;
			sendsock->PCID = pl->PCID;
			sprintf(sendsock->sendstr, SENDMESSAGETOPLAYERS_STR,
				playernr, txt);
			err = CallNetwork(NETWORK_SENDPACKET, sendsock);
		}
	}
	return(err);
}
//==========================================
int SendCommandToPlayer(MENUSTR *allmenus, int playernr, int command, NETWORK_INFO *sendsock, NETWORK_INFO *recvsock)
{
	int i, max, maxnr, updateplayers = 0, err;
	NETWPLAYERINFO *pl;
	connectedplayers.EnumListInit();
	while ((pl = (NETWPLAYERINFO *)connectedplayers.GetNextListElem()))
	{
		if (pl->playernr == playernr)
		{
			sendsock->Addr.sin_family = AF_INET;
			sendsock->Addr.sin_port = htons(pl->useport);
			sendsock->Addr.sin_addr.s_addr = pl->Addr.sin_addr.s_addr;
			sendsock->COMMAND = command;
			sendsock->PCID = pl->PCID;
			switch (command)
			{
			case STARCLONE_NETWORKJOIN_COMMAND_BANPLAYER:
				pl->timeout = 1;
				pl->flag |= FLAGS_PLAYERLEAVEDBYBAN;
				break;
			case STARCLONE_NETWORKJOIN_COMMAND_ALLPLAYERSINFO:
				sprintf(sendsock->sendstr, SENDINFOTOPLAYERS_STR,
					force_slots.openedslots,
					BASENAMEMAP,
					CHECKSUMMMAP,
					FILESIZEMAP,
					GetPlayerTypeJoin(0), GetSlotName(0),
					GetPlayerTypeJoin(1), GetSlotName(1),
					GetPlayerTypeJoin(2), GetSlotName(2),
					GetPlayerTypeJoin(3), GetSlotName(3),
					GetPlayerTypeJoin(4), GetSlotName(4),
					GetPlayerTypeJoin(5), GetSlotName(5),
					GetPlayerTypeJoin(6), GetSlotName(6),
					GetPlayerTypeJoin(7), GetSlotName(7),
					GetPlayerTypeJoin(8), GetSlotName(8),
					GetPlayerTypeJoin(9), GetSlotName(9),
					GetPlayerTypeJoin(10), GetSlotName(10),
					GetPlayerTypeJoin(11), GetSlotName(11)
				);
				break;
			case STARCLONE_NETWORKJOIN_COMMAND_ASKFORFILE:
				break;
			case STARCLONE_NETWORKJOIN_COMMAND_FILEBYTES:
				sprintf(sendsock->sendstr, SENDFILEDATA_STR,
					MAPLOADEDLENGTH,
					CreateCRC((unsigned char *)PORTIONFILEBUFF, PORTIONFILESIZE, BUFF_NOCRYPTIZE),
					PORTIONFILESIZE);
				strcat(sendsock->sendstr, PI_BYTES);
				strcat(sendsock->sendstr, "=");
				memcpy(sendsock->sendstr + strlen(sendsock->sendstr), PORTIONFILEBUFF, PORTIONFILESIZE);
				break;
			case STARCLONE_NETWORKJOIN_COMMAND_PLAYGAME:
				sprintf(sendsock->sendstr, SENDPLAYGAME_STR,
					GetMapPlayers(), RNDSEED,
					GetPlayerType(0), GetPlayerIp(0), GetPlayerPCID(0),
					GetPlayerType(1), GetPlayerIp(1), GetPlayerPCID(1),
					GetPlayerType(2), GetPlayerIp(2), GetPlayerPCID(2),
					GetPlayerType(3), GetPlayerIp(3), GetPlayerPCID(3),
					GetPlayerType(4), GetPlayerIp(4), GetPlayerPCID(4),
					GetPlayerType(5), GetPlayerIp(5), GetPlayerPCID(5),
					GetPlayerType(6), GetPlayerIp(6), GetPlayerPCID(6),
					GetPlayerType(7), GetPlayerIp(7), GetPlayerPCID(7),
					GetPlayerType(8), GetPlayerIp(8), GetPlayerPCID(8),
					GetPlayerType(9), GetPlayerIp(9), GetPlayerPCID(9),
					GetPlayerType(10), GetPlayerIp(10), GetPlayerPCID(10),
					GetPlayerType(11), GetPlayerIp(11), GetPlayerPCID(11)
				);
				break;
			}
			err = CallNetwork(NETWORK_SENDPACKET, sendsock);
			break;
		}
	}
	return(err);
}
//==========================================
int SendCommandToAll(MENUSTR *allmenus, int command, NETWORK_INFO *sendsock, NETWORK_INFO *recvsock)
{
	int i, max, maxnr;
	NETWPLAYERINFO *pl;
	connectedplayers.EnumListInit();
	while ((pl = (NETWPLAYERINFO *)connectedplayers.GetNextListElem()))
	{
		SendCommandToPlayer(allmenus, pl->playernr, command, sendsock, recvsock);
	}
	return(0);
}
//==========================================
void RequestFileParts(MENUSTR *allmenus, NETWORK_INFO *sendsock, NETWORK_INFO *recvsock)
{
	if (MAPUPLOAD)
	{
		if (!MAPVERIFIED)
		{
			SendInfoToMaster(STARCLONE_NETWORKJOIN_COMMAND_GIVENEXTPARTOFFILE, "", sendsock, recvsock);
		}
	}
}
//==========================================
void BuffPrepareForSend(char *fn, long pos, int size, char *buff)
{
	FILE *f = fopen(fn, "r");
	if (f)
	{
		fseek(f, pos, SEEK_SET);
		fread(buff, size, 1, f);
		fclose(f);
	}
}
//==========================================
void BuffPrepareForWrite(char *fn, long pos, int size, char *buff)
{
	FILE *f = fopen(fn, "r+");
	if (!f)
	{
		f = fopen(fn, "a");
		if (!f)
			return;
	}
	fseek(f, pos, SEEK_SET);
	fwrite(buff, size, 1, f);
	fclose(f);
}
//==========================================
void Master_CopyPlayersAddr(void)
{
	int i, max, maxnr;
	NETWPLAYERINFO *pl;
	memset(&PLAYER[0].Addr, 0, sizeof(struct sockaddr_in));
	connectedplayers.EnumListInit();
	while ((pl = (NETWPLAYERINFO *)connectedplayers.GetNextListElem()))
	{
		memcpy(&PLAYER[pl->playernr].Addr, &pl->Addr, sizeof(struct sockaddr_in));
		PLAYER[pl->playernr].Addr.sin_port = htons(STARCLONE_UDP_PLAYPORT);
		PLAYER[pl->playernr].PCID = pl->PCID;
	}
}
//==========================================
void Master_MakeMapConf(void)
{
	int i;
	for (i = 0;i < MAXPLAYERS;i++)
	{
		if (newnetwgame.slot1[i] != GAMEFORCENAMESLOT)
		{
			if (newnetwgame.slot1[i] == GAMECOMPUTERSLOT)
			{
				gameconf.pl_owner[i] = OWNER_COMPUTER;
			}
			else if (newnetwgame.slot1[i] == GAMEPLAYERSLOT)
			{
				gameconf.pl_owner[i] = OWNER_HUMAN;
			}
			else
			{
				gameconf.pl_owner[i] = OWNER_NONE;
				gameconf.pl_race[i] = RACE_INACTIVE;
				continue;
			}
			gameconf.pl_race[i] = newnetwgame.slot2[i];
			if (gameconf.pl_race[i] == RACE_INDEPENDENT)
				gameconf.pl_race[i] = myrand(3);
		}
		else
		{
			gameconf.pl_owner[i] = OWNER_FORCENAME;
			gameconf.pl_race[i] = RACE_INACTIVE;
		}
	}
}
//==========================================
int CreateAllConnectionsWithPlayers(ALLNETWCONNECTION *conn)
{
	int i, err = 0;
	memset(conn, 0, sizeof(ALLNETWCONNECTION));
	if (force_slots.networkplayers <= 1)
		return(0);
	for (i = 0;i < MAXPLAYERS;i++)
	{
		if (gameconf.pl_owner[i] == OWNER_HUMAN && i != NUMBGAMER)
		{
			if (CallNetwork(NETWORK_INITNETWORK, &conn->send_gameinfo[i]) != NETWORK_OK)
				return(0x80 | i);
			memcpy(&conn->send_gameinfo[i].Addr, &PLAYER[i].Addr, sizeof(struct sockaddr_in));
			conn->send_gameinfo[i].PCID = PLAYER[i].PCID;
		}
	}
	if (CallNetwork(NETWORK_INITNETWORK, &conn->receive_gameinfo) != NETWORK_OK)
		return(0x81 | i);
	if (CallNetwork(NETWORK_INITRECVPLAYPACKET, &conn->receive_gameinfo) != NETWORK_OK)
		return(0x82 | i);
	return(0);
}
//==========================================
void DestroyAllConnectionsWithPlayers(ALLNETWCONNECTION *conn)
{
	int i;
	if (force_slots.networkplayers <= 1)
		return;
	for (i = 0;i < MAXPLAYERS;i++)
	{
		if (gameconf.pl_owner[i] == OWNER_HUMAN && i != NUMBGAMER)
		{
			if (conn->send_gameinfo[i].socketnr)
				CallNetwork(NETWORK_CLOSENETWORK, &conn->send_gameinfo[i]);
		}
	}
	if (conn->receive_gameinfo.socketnr)
		CallNetwork(NETWORK_CLOSENETWORK, &conn->receive_gameinfo);
}
//==========================================
void Set_MasterGame(int playernr)
{
	if (playernr == NUMBGAMER)
		HOSTGAMER = OWNERGAMER;
}
//==========================
int Check_IfServerPlayer(void)
{
	return(HOSTGAMER == OWNERGAMER);
}
//==========================
void ShowChatMessage(char *chatmes)
{
	chatbar.addbarmessage(chatmes, IDFONT10, GWHITECOLORFONT, TRIGGER_DISPLAYMESSAGETIME, BF_ALLOCBUF | BF_FORCEADD | BF_SPLITLINES);
	Play_sfxdata_id(NULL, SFXDATA_TRANSMISSION, 1, 0);
}
//==========================


