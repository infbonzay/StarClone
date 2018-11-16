#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <fcntl.h>
#include <errno.h>


#include "wmem.h"
#include "netwgame.h"
#include "network_general.h"

//=============================================
void closesock(NETWORK_INFO *info)
{
	if (info->socketnr > 0)
	{
		close(info->socketnr);
		info->socketnr = 0;
	}
	if (info->recvbuffer)
	{
		wfree(info->recvbuffer);
		info->recvbuffer = NULL;
	}
}
//=============================================
#define MAXNETWORKCARDS					10
//=============================================
int NETWORKGAME_UDP(NETWORK_INFO *info)
{
	static unsigned int PCID, NETWORKCARDS = 0, BCAST[MAXNETWORKCARDS];
	int i, ret, flag, sflags, readed, CRC, waitbytes;
	socklen_t recvlength;
	pid_t mypid;
	struct sockaddr_in myaddr;
	struct ifaddrs *ifAddrStruct, *ifa;
	NETWORK_JOIN_PACKET *mypacket;
	if (!(info->param&NETWORK_INITFLAG) && info->socketnr <= 0)
		return(NETWORK_ERROR_CMD);
	switch (info->param)
	{
	case NETWORK_INITNETWORK:
		if (!PCID)
		{
			mypid = getpid();
			getifaddrs(&ifAddrStruct);
			for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next)
			{
				if (ifa->ifa_addr)
				{
					if (ifa->ifa_addr->sa_family == AF_INET && !(ifa->ifa_flags&IFF_LOOPBACK))
					{
						if (ifa->ifa_flags&IFF_BROADCAST)
						{
							memcpy((void *)&myaddr, ifa->ifa_ifu.ifu_broadaddr, sizeof(struct sockaddr_in));
							BCAST[NETWORKCARDS] = myaddr.sin_addr.s_addr;
							memcpy((void *)&myaddr, ifa->ifa_addr, sizeof(struct sockaddr_in));
							if (++NETWORKCARDS >= MAXNETWORKCARDS)
								break;
						}
					}
				}
			}
			if (ifAddrStruct != NULL)
				freeifaddrs(ifAddrStruct);
			//pcid is processid + last 2 ip numbers shifted left by 16 bits
			PCID = (myaddr.sin_addr.s_addr & 0xffff0000) + mypid;
		}
		if (!NETWORKCARDS)
		{
			return(NETWORK_ERROR_INIT);
		}
		info->socketnr = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (info->socketnr <= 0)
			return(NETWORK_ERROR_INIT);
		info->PCID = PCID;
		return(NETWORK_OK);
	case NETWORK_INITBROADCAST:		//NOTE: receive broadcast works if ip_forward set to 1
		flag = 1;
		ret = setsockopt(info->socketnr, SOL_SOCKET, SO_BROADCAST, &flag, sizeof(flag));
		if (ret)
		{
			closesock(info);
			return(NETWORK_ERROR_BROADCAST);
		}
		return(NETWORK_OK);
	case NETWORK_DATAREADY:
		ioctl(info->socketnr, FIONREAD, &waitbytes);
		info->nextpacketbytes = waitbytes;
		if (waitbytes > 0)
		{
			return(NETWORK_OK_RECVDATA);
		}
		else
			return(NETWORK_OK_NODATA);
	case NETWORK_SENDIDENTIFICATION:
		memset(&info->Addr, 0, sizeof(sockaddr_in));
		info->Addr.sin_family = AF_INET;
		info->Addr.sin_port = htons(STARCLONE_UDP_BROADCASTPORT);
		mypacket = (NETWORK_JOIN_PACKET*)wmalloc(STARCLONE_UDP_JOINPACKETSIZE);
		memset(mypacket, 0, STARCLONE_UDP_JOINPACKETSIZE);
		mypacket->COMMAND = STARCLONE_NETWORKJOIN_COMMAND_CREATEDGAME;
		mypacket->PCID = info->PCID;
		sprintf(mypacket->data, GAMEIDENT_STR,
			info->netgame.TypeGame,
			info->netgame.SpeedGame,
			info->netgame.mapsizex,
			info->netgame.mapsizey,
			info->connectport,
			info->netgame.gametitle,
			info->netgame.mapname,
			info->netgame.players,
			info->netgame.slots);
		mypacket->CRC = CreateCRC(((unsigned char *)mypacket) + sizeof(int), STARCLONE_UDP_JOINPACKETSIZE - sizeof(int), BUFF_NOCRYPTIZE);
		for (i = 0;i < NETWORKCARDS;i++)
		{
			info->Addr.sin_addr.s_addr = BCAST[i];
			//sendpacket
			ret = sendto(info->socketnr, mypacket, STARCLONE_UDP_JOINPACKETSIZE, 0, (struct sockaddr*)&info->Addr, sizeof(struct sockaddr_in));
			DEBUG_packet(0, mypacket, &info->Addr);
			if (ret < 0)
			{
				printf("1.broadcast game ident, errno=%d\n", errno);
				perror("sendto");
			}
		}
		wfree(mypacket);
		return(NETWORK_OK);
	case NETWORK_SENDCLOSEIDENTIFICATION:
		memset(&info->Addr, 0, sizeof(sockaddr_in));
		info->Addr.sin_family = AF_INET;
		info->Addr.sin_port = htons(STARCLONE_UDP_BROADCASTPORT);
		mypacket = (NETWORK_JOIN_PACKET*)wmalloc(STARCLONE_UDP_JOINPACKETSIZE);
		memset(mypacket, 0, STARCLONE_UDP_JOINPACKETSIZE);
		mypacket->COMMAND = STARCLONE_NETWORKJOIN_COMMAND_CLOSEGAME;
		mypacket->PCID = info->PCID;
		mypacket->CRC = CreateCRC(((unsigned char *)mypacket) + sizeof(int), STARCLONE_UDP_JOINPACKETSIZE - sizeof(int), BUFF_NOCRYPTIZE);
		for (i = 0;i < NETWORKCARDS;i++)
		{
			info->Addr.sin_addr.s_addr = BCAST[i];
			//sendpacket
			ret = sendto(info->socketnr, mypacket, STARCLONE_UDP_JOINPACKETSIZE, 0, (struct sockaddr*)&info->Addr, sizeof(struct sockaddr_in));
			DEBUG_packet(0, mypacket, &info->Addr);
			if (ret < 0)
			{
				printf("2.broadcast game ident close, errno=%d\n", errno);
				perror("sendto");
			}
		}
		wfree(mypacket);
		return(NETWORK_OK);
	case NETWORK_INITRECVIDENTIFICATION:
		/*			ret=setsockopt(info->socketnr, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
					if (ret)
					{
						closesock(info);
						return(NETWORK_ERROR_REUSEADDR);
					}
		*/
		//			info->Addr.sin_addr.s_addr = htonl(MKADDR(192,168,0,255));
		//			info->Addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
		info->Addr.sin_addr.s_addr = htonl(INADDR_ANY);
		info->Addr.sin_port = htons(STARCLONE_UDP_BROADCASTPORT);
		info->Addr.sin_family = AF_INET;
		//bind socket to port
		if (bind(info->socketnr, (struct sockaddr*)&info->Addr, sizeof(struct sockaddr_in)) != -1)
		{
			info->connectport = STARCLONE_UDP_BROADCASTPORT;
			//create non blocking read
			sflags = fcntl(info->socketnr, F_GETFL);
			sflags |= O_NONBLOCK;
			fcntl(info->socketnr, F_SETFL, sflags);
			info->recvbuffer = wmalloc(STARCLONE_UDP_JOINPACKETSIZE);
			return(NETWORK_OK);
		}
		info->connectport = 0;
		closesock(info);
		return(NETWORK_ERROR_SOCKETBIND);
	case NETWORK_INITRECVPACKET:
		/*			ret=setsockopt(info->socketnr, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
					if (ret)
					{
						closesock(info);
						return(NETWORK_ERROR_REUSEADDR);
					}
		*/			info->Addr.sin_family = AF_INET;
		info->Addr.sin_addr.s_addr = htonl(INADDR_ANY);
		for (i = 0;i < STARCLONE_UDP_MAXTRYJOINPORT;i++)
		{
			info->Addr.sin_port = htons(STARCLONE_UDP_JOINPORT + i);
			//bind socket to port
			if (bind(info->socketnr, (struct sockaddr*)&info->Addr, sizeof(struct sockaddr_in)) != -1)
			{
				info->connectport = STARCLONE_UDP_JOINPORT + i;
				//create non blocking read
				sflags = fcntl(info->socketnr, F_GETFL);
				sflags |= O_NONBLOCK;
				fcntl(info->socketnr, F_SETFL, sflags);
				info->recvbuffer = wmalloc(STARCLONE_UDP_JOINPACKETSIZE);
				return(NETWORK_OK);
			}
		}
		info->connectport = 0;
		closesock(info);
		return(NETWORK_ERROR_SOCKETBIND);
	case NETWORK_INITRECVPLAYPACKET:
		info->Addr.sin_family = AF_INET;
		info->Addr.sin_addr.s_addr = INADDR_ANY;
		info->Addr.sin_port = htons(STARCLONE_UDP_PLAYPORT);
		//bind socket to port
		if (bind(info->socketnr, (struct sockaddr*)&info->Addr, sizeof(struct sockaddr_in)) == -1)
		{
			closesock(info);
			return(NETWORK_ERROR_SOCKETBIND);

		}
		info->connectport = STARCLONE_UDP_PLAYPORT;
		//create non blocking read
		sflags = fcntl(info->socketnr, F_GETFL);
		sflags |= O_NONBLOCK;
		fcntl(info->socketnr, F_SETFL, sflags);
		info->recvbuffer = wmalloc(STARCLONE_UDP_JOINPACKETSIZE);
		return(NETWORK_OK);
	case NETWORK_RECVPACKET:
		recvlength = sizeof(struct sockaddr_in);
		readed = recvfrom(info->socketnr, info->recvbuffer, STARCLONE_UDP_JOINPACKETSIZE, 0,
			(struct sockaddr *)&info->Addr, &recvlength);
		if (readed < 0)
			return(NETWORK_ERROR_NODATA);
		mypacket = (NETWORK_JOIN_PACKET*)info->recvbuffer;
		CRC = CreateCRC(((unsigned char *)mypacket) + sizeof(int), STARCLONE_UDP_JOINPACKETSIZE - sizeof(int), BUFF_NODECRYPTIZE);
		DEBUG_packet(1, mypacket, &info->Addr);
		if (CRC != mypacket->CRC)
		{
			printf("wrong buffer not game\n");
			return(NETWORK_ERROR_WRONGDATA);
		}
		return(NETWORK_OK_RECVDATA);
	case NETWORK_SENDPACKET:
		mypacket = (NETWORK_JOIN_PACKET*)wmalloc(STARCLONE_UDP_JOINPACKETSIZE);
		memset(mypacket, 0, STARCLONE_UDP_JOINPACKETSIZE);
		mypacket->COMMAND = info->COMMAND;
		mypacket->PCID = info->PCID;
		memcpy(mypacket->data, info->sendstr, SIZESENDSTR);
		mypacket->CRC = CreateCRC(((unsigned char *)mypacket) + sizeof(int), STARCLONE_UDP_JOINPACKETSIZE - sizeof(int), BUFF_NOCRYPTIZE);
		//sendpacket
		ret = sendto(info->socketnr, mypacket, STARCLONE_UDP_JOINPACKETSIZE, 0, (struct sockaddr*)&info->Addr, sizeof(struct sockaddr_in));
		DEBUG_packet(0, mypacket, &info->Addr);
		wfree(mypacket);
		if (ret < 0)
		{
			printf("3.sendto errno=%d\n", errno);
			perror("sendto");
			return(NETWORK_ERROR_SEND);
		}
		return(NETWORK_OK);
	case NETWORK_CLOSENETWORK:
		if (info->socketnr > 0)
		{
			closesock(info);
			return(NETWORK_OK);
		}
		return(NETWORK_ERROR_CMD);
	default:
		return(NETWORK_ERROR_CMD);
	}
}
//====================================================
