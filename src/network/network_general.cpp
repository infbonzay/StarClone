#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "network_general.h"

ALLNETWCONNECTION		playersconn;

int(*NetworkFunc)(NETWORK_INFO *);
int(*networkfunc[MAXCONNTYPES])(NETWORK_INFO *) = { &NETWORKGAME_UDP,
												  &NETWORKGAME_TCP,
												  &NETWORKGAME_DC,
												  &NETWORKGAME_MODEM,
												  &NETWORKGAME_DEDICATED
};

void DEBUG_packet(int inout, NETWORK_JOIN_PACKET *mypacket, struct sockaddr_in *sk)
{
	if (inout == 0)
		//		printf("NET <- CRC=%08x PCID=%08x COMMAND=%u toip=%s:%u\n",mypacket->CRC,mypacket->PCID,mypacket->COMMAND,inet_ntoa(sk->sin_addr),htons(sk->sin_port));
		printf("NET <- CRC=%08x PCID=%08x COMMAND=%u toip=%s:%u sendstr=%s\n", mypacket->CRC, mypacket->PCID, mypacket->COMMAND, inet_ntoa(sk->sin_addr), htons(sk->sin_port), mypacket->data);
	else
		//		printf("NET -> CRC=%08x PCID=%08x COMMAND=%u fromip=%s\n",mypacket->CRC,mypacket->PCID,mypacket->COMMAND,inet_ntoa(sk->sin_addr));
		printf("NET -> CRC=%08x PCID=%08x COMMAND=%u fromip=%s sendstr=%s\n", mypacket->CRC, mypacket->PCID, mypacket->COMMAND, inet_ntoa(sk->sin_addr), mypacket->data);

}

//=============================================
int DecodeBuffer(unsigned char *buff, int size)
{
	return(1);
}
//=============================================
int CodeBuffer(unsigned char *buff, int size)
{
	return(1);
}
//=============================================
int CreateCRC(unsigned char *buff, int size, int codedecode)
{
	int crc[4], i;
	crc[0] = 0x0c;
	crc[1] = 0x48;
	crc[2] = 0x84;
	crc[3] = 0xc0;
	if (codedecode == BUFF_DECRYPTIZE)
		DecodeBuffer(buff, size);
	size /= 4;
	for (i = 0;i < size;i++)
	{
		crc[0] ^= buff[i * 4 + 0];
		crc[1] ^= buff[i * 4 + 1];
		crc[2] ^= buff[i * 4 + 2];
		crc[3] ^= buff[i * 4 + 3];
	}
	if (codedecode == BUFF_CRYPTIZE)
		CodeBuffer(buff, size);
	return(crc[0] << 24) + (crc[1] << 16) + (crc[2] << 8) + crc[3];
}
//=============================================
void SetNetworkType(int networktype)
{
	NetworkFunc = networkfunc[networktype];
}
//=============================================
int CallNetwork(int funcid, NETWORK_INFO *info)
{
	info->param = funcid;
	return(*NetworkFunc)(info);
}
//==========================



