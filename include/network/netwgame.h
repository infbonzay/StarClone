#if !defined(_NETWGAME_W)
#define _NETWGAME_W

#include "mylist.h"
#include "menu.h"
#include "network_general.h"

#define FLAGS_PLAYERLEAVEDBYCOMMAND		0x01
#define FLAGS_PLAYERLEAVEDBYBAN			0x02
#define FLAGS_PLAYERNEEDUPLOADMAP		0x04





struct NETWPLAYERINFO
{
    unsigned int	PCID;
    struct sockaddr_in	Addr;
    unsigned short int	useport;
    signed char		timeout;
    unsigned char	flag;			//bit 0 is timeout reached
						//bit 1 leaved by ban
						//bit 2 upload file
    signed char		playernr;
    char		playername[MAXPLAYERNAMESYMBOLS];
};
struct BANNEDIPS
{
    struct sockaddr_in Addr;
};

int  UpdateAllGamesTime(void);
int  AddListCreatedGame(NETWORK_JOIN_PACKET *mypacket,struct sockaddr_in *Addr);
void UpdateGamesList(MENUSTR *allmenus,int listitem);
void UpdateGameInfo(MENUSTR *allmenus,int listnr,int selectnr);
void EmptyAllGamesList(void);
void SendTextToConnected(char *chattext);
int  GetGameConnectedSlot(int playerslot);
void SetGameConnectedSlot(int playerslot,int value);
int  GetGameValueSlot(int playerslot);
void SetGameValueSlot(int playerslot,int value);
int  GetGameId(int sellist,char *gametitle);
int  CheckForGameName(char *gametitle);
int  TryJoinPlayer(NETWORK_INFO *info);
void CreateGameSlots(int maxplayers,int playernr);
int  Master_CheckJoinPacket(MENUSTR *allmenus,NETWORK_INFO *sendsock,NETWORK_INFO *recvsock,NETWPLAYERINFO *info);
int  Guest_CheckJoinPacket(MENUSTR *allmenus,NETWORK_INFO *sendsock,NETWORK_INFO *recvsock);
int  GetPlayersMapBits(void);
int  GetSlotsMapBits(void);
int  GetINTFromArray(char *allarray,const char *str,int fromid);
int  GetHEXFromArray(char *allarray,const char *str,int fromid);
void GetSTRFromArray(char *dest,char *allarray,const char *str,int fromid);
void GetBYTESFromArray(char *destbuf,char *allarray,int buffsize,const char *str);
int  FillNetworkAddr(NETWORK_INFO *info,int PCID);
int  GetMapPlayers(void);
void EmptyConnectedPlayers(void);
int  UpdatePlayersInfo(MENUSTR *allmenus,NETWORK_INFO *sendsock,NETWORK_INFO *recvsock);
int  AddListConnectedPlayer(MENUSTR *allmenus,NETWORK_JOIN_PACKET *mypacket,NETWPLAYERINFO *newplayer);
int  AddConnectedPlayer(MENUSTR *allmenus,NETWPLAYERINFO *newplayer);
void setslotinfo(MENUSTR *allmenus,int nr,int masterguest,char *playername,int plslot,int rsslot);
int  SendInfoToConnectedPlayers(int command);
int  SendInfoToMaster(int command,const char *txtchat,NETWORK_INFO *sendsock,NETWORK_INFO *recvsock);
int  SendTextToConnectedPlayers(int playernr,char *txt,NETWORK_INFO *sendsock,NETWORK_INFO *recvsock);
int  SendCommandToPlayer(MENUSTR *allmenus,int playernr,int command,NETWORK_INFO *sendsock,NETWORK_INFO *recvsock);
int  SendCommandToAll(MENUSTR *allmenus,int command,NETWORK_INFO *sendsock,NETWORK_INFO *recvsock);
int  CheckForDublicateName(char *newname);
void AddToBanList(int playerslot);
int  CheckForBan(struct sockaddr_in *newaddr);
void RequestFileParts(MENUSTR *allmenus,NETWORK_INFO *sendsock,NETWORK_INFO *recvsock);
void BuffPrepareForSend(char *fn,long pos,int size,char *buff);
void BuffPrepareForWrite(char *fn,long pos,int size,char *buff);
int  CheckForPlayersReady(void);
int  GetPlayerPCID(int playernr);
int  GetPlayerType(int playernr);
int  GetPlayerIp(int playernr);
void Master_CopyPlayersAddr(void);
void Master_MakeMapConf(void);
int  CreateAllConnectionsWithPlayers(ALLNETWCONNECTION *conn);
void DestroyAllConnectionsWithPlayers(ALLNETWCONNECTION *conn);
void Set_MasterGame(int playernr);
int  Check_IfServerPlayer(void);
void ShowChatMessage(char *chatmes);

extern char	PORTIONFILEBUFF[];
extern char	SENDPARAM_SENDTOALLPLAYERS[];

#endif /* _NETWGAME_W */
