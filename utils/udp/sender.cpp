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

char *gn[8]={
	    "banzay",
	    "toad",
	    "destroyer",
	    "pacman",
	    "helpme",
	    "fiction",
	    "teamZ",
	    "rakanishu"
};
int main(int argc, char *argv[])
{
    int err,i=0,gnumber;
    NETWORK_INFO game;
    if (argc<2)
    {
	printf("Usage:%s gamenumber(0-7)\n",argv[0]);
	exit(-1);
    }
    gnumber=atoi(argv[1]);
    printf("create game number=%d\n",gnumber);
    
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
    do{
        game.param=NETWORK_SENDIDENTIFICATION;
        game.connectport=STARCLONE_UDP_JOINPORT+9+gnumber;
        game.netgame.players=0x30000000;
        game.netgame.slots=0x33333333;
        game.netgame.TypeGame=0;
        game.netgame.SpeedGame=random()%7;
        game.netgame.mapsizex=(random()%4+1)*64;
        game.netgame.mapsizey=(random()%4+1)*64;
        strncpy(game.netgame.gametitle,gn[gnumber],STARCLONE_NETWORKJOIN_MAXGAMETITLESYMB);
        game.netgame.gametitle[STARCLONE_NETWORKJOIN_MAXGAMETITLESYMB]=0;
        strncpy(game.netgame.mapname,"Challenge",STARCLONE_NETWORKJOIN_MAXMAPNAMESYMB);
        game.netgame.mapname[STARCLONE_NETWORKJOIN_MAXMAPNAMESYMB]=0;
        err=NETWORKGAME_UDP(&game);
        if (err<0)
        { 
    	    printf("network error sendident=%d\n",err);
    	    exit(-1);
    	}
        printf("%d send packet\n",i++);
        sleep(1);
    }while(i<1000);
    game.param=NETWORK_CLOSENETWORK;
    err=NETWORKGAME_UDP(&game);
    if (err<0)
    {
	printf("network error close=%d\n",err);
	exit(-1);
    }
    return 0;
}

