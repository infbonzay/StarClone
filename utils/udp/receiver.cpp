#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>

#include "network_general.h"

int main(int argc, char *argv[])
{
    int err,i=0,gnumber;
    NETWORK_INFO game;
    memset(&game,0,sizeof (NETWORK_INFO));

    game.param=NETWORK_INITNETWORK;
    err=NETWORKGAME_UDP(&game);
    if (err<0){ printf("network error init=%d\n",err); exit(-1); }

    game.param=NETWORK_INITBROADCAST;
    err=NETWORKGAME_UDP(&game);
    if (err<0){ printf("network error intbroadcast=%d\n",err); exit(-1); }
    timeval tm;
    gettimeofday(&tm,NULL);
    srandom(tm.tv_usec);
    game.param=NETWORK_INITRECVIDENTIFICATION;
    err=NETWORKGAME_UDP(&game);
    if (err<0)
    { 
        printf("network error sendident=%d\n",err);
        exit(-1);
    }
    do{
	game.param=NETWORK_DATAREADY;
	err=NETWORKGAME_UDP(&game);
	if (err==NETWORK_OK_RECVDATA)
	{
	    game.param=NETWORK_RECVPACKET;
	    err=NETWORKGAME_UDP(&game);
	    if (err==NETWORK_OK_RECVDATA)
	    {
		printf("data income\n");
	    }
	}
    }while(1);
    game.param=NETWORK_CLOSENETWORK;
    err=NETWORKGAME_UDP(&game);
    if (err<0)
    {
	printf("network error close=%d\n",err);
	exit(-1);
    }
    return 0;
}

