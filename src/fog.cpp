
#include <stdio.h>
#include <string.h>
#include <grplib/gr8.h>
#include "defs.h"
#include "vars.h"
#include "gr.h"
#include "maps.h"
#include "mouse.h"
#include "vision.h"
#include "fog.h"


int SCRFOGX,SCRFOGY;
unsigned char *screenfog,*screenmapused;
//================================================
void putfogspr(int x,int y,char *table,long lt,long rt,long lb,long rb,int fognr)	
{
//    char buff[20];
//    sprintf(buff,"%d",fognr);
    putfog32x32(x,y,table,lt,rt,lb,rb);
//    putmessage(x+16,y+16,IDFONT8,buff,GWHITECOLORFONT,tfontgamp,gamedlggrp);
}
//================================================
int FOGAROUND[4][3][2]=	{
				{{-1,-1},{ 0,-1},{-1, 0}},
				{{ 0,-1},{ 1,-1},{ 1, 0}},
				{{-1, 0},{-1, 1},{ 0, 1}},
				{{ 1, 0},{ 0, 1},{ 1, 1}}
			};
//		      0 - 4 open & see
//		      5 - 7 open & !see
//		      8 - !open & !see 
//		      9 -  border between open & see
//		      10 
//		      11 
//		      12 
//		      13 - border between !open & open
//		      0    1    2    3    4    5    6    7    8    9   10   11   12   13
    char FOGDIAP[2][14]=
		{
		    {0x1f,0x1e,0x18,0x11,0x0f,0x0d,0x07,0x02,0x00,0x1b,0x0f,0x03,0x0b,0x0f},
		    {0x1f,0x1e,0x18,0x11,0x0f,0x0e,0x0c,0x0a,0x08,0x1b,0x0f,0x0b,0x11,0x0f}
//		    {0x1f,0x1e,0x18,0x15,0x14,0x13,0x0f,0x0a,0x08,0x1b,0x14,0x0b,0x11,0x14}
		};

//================================================
int getborderfogs(int x,int y,unsigned char centerfog,long *borders)
{
    int i,j,newx,newy,sumfog,sumborders=0;
    unsigned char nearfog;
    for (i=0;i<4;i++)
    {
	sumfog=FOGDIAP[gameconf.videoconf.visiblemap][centerfog];

	for (j=0;j<3;j++)
	{
	    newx=x+FOGAROUND[i][j][0];
    	    newy=y+FOGAROUND[i][j][1];
    	    if (newx<=0)
    	        newx=1;
    	    if (newx>=SCRFOGX-2)
    	        newx=SCRFOGX-3;
    	    if (newy<=0)
    		newy=1;
    	    if (newy>=SCRFOGY-2)
    		newy=SCRFOGY-3;
    	    sumfog+=FOGDIAP[gameconf.videoconf.visiblemap][screenfog[newy*SCRFOGX+newx]];
/*		if (newx<0||newy<0||newx>=SCRFOGX||newy>=SCRFOGY)
		    continue;
		else
		    sumfog+=FOGDIAP[gameconf.videoconf.visiblemap][screenfog[newy*SCRFOGX+newx]];
*/
	}
	borders[i]=sumfog/4;
	sumborders+=sumfog/4;
    }
    return(sumborders==0x1f*4);
}
//================================================
void putfog(void)
{
    int i,j,retborder;
    int bh,eh,bw,ew,ii,jj,xkk,ykk;
    long borders[4];
    unsigned char scrfognr;
    if (CODEFORSCREEN)
	return;
    bh=map.MAPY;
    eh=bh+SCRFOGY;
    if (eh > MAXYMAP)
        eh = MAXYMAP;
    bw=map.MAPX;
    ew=bw+SCRFOGX;
    if (ew > MAXXMAP)
	ew = MAXXMAP;
    xkk = map.MAPXGLOBAL % SIZESPRLANSHX;
    ykk = map.MAPYGLOBAL % SIZESPRLANSHY;
    for (ii=0,i=bh;i<eh;i++,ii++)
    {
        if (ii>=SCRFOGY-2)
    	    break;
	for (jj=0,j=bw;j<ew;j++,jj++)
	{
    	    if (jj>=SCRFOGX-2)
    		break;
	    scrfognr=screenfog[(ii+1)*SCRFOGX+jj+1];
	    retborder=getborderfogs(jj+1,ii+1,scrfognr,borders);
	    if (!retborder)
	    {
		if (screenmapused[ii*(widthkart+1)+jj])
		{
		    putfogspr(jj*SIZESPRLANSHX-xkk,ii*SIZESPRLANSHX-ykk,_toblack_,
			borders[0],borders[1],borders[2],borders[3],scrfognr);
		}
#ifdef REMOVEFOGBUG_FUNC1
		else
		    putfogspr(jj*SIZESPRLANSHX-xkk,ii*SIZESPRLANSHX-ykk,_toblack_,
			0,0,0,0,scrfognr);
#endif
#ifdef REMOVEFOGBUG_FUNC2
		else
		    CSetImage8(jj*SIZESPRLANSHX-xkk,ii*SIZESPRLANSHX-ykk,32,32,0);
#endif
	    }
	}
    }
}
//================================================
//if bit is set calculate fog for that player bits are 0-7
//================================================
void copywhitefogtoblackfog(int player)
{
#ifndef WITHOUTHALFFOG 
    int i;
    unsigned char fogvalue;
    if (map.mapbits.whitefog[player])
	for (i=0;i<MAXXMAP*MAXYMAP;i++)
	{
	    fogvalue = map.mapbits.whitefog[player][i];
	    if (fogvalue > map.mapbits.blackfog[player][i])
		map.mapbits.blackfog[player][i] = fogvalue;
	}
#endif
}
//================================================
void calculatefog(int bitplayers)
{
    FILE *f;
    int bh,eh,bw,ew,ii,jj,mask;
    unsigned char scrfognr[PLAYEDPLAYERS];
    int i,j,player,map1,map2;
    unsigned char fogvalue;
    if (CODEFORSCREEN)
	return;
    map.screenfogchange=0;
    //if not modify fog and not moved map no need changes in screenfog, exit from here
    for (i=0;i<PLAYEDPLAYERS;i++)
    {
	if (map.clearfog[i])			//modify w/b fogs if unit change the map or refresh map
	{
	    copywhitefogtoblackfog(i);
	}
    }
    if (!map.MAPXYmove&&!map.clearfog[NUMBGAMER])
	return;
    memset(screenfog,0x08,SCRFOGX*SCRFOGY);
    map.MAPXYmove=0;
    bitplayers&=0xff;
    bh=map.MAPY-1;
    eh=bh+SCRFOGY;
    if (eh > MAXYMAP)
        eh = MAXYMAP;
    bw=map.MAPX-1;
    ew=bw+SCRFOGX;
    if (ew > MAXXMAP)
	ew = MAXXMAP;
    for (ii=0,i=bh;i<eh;i++,ii++)
    {
	if (i<0)
	    continue;
	for (jj=0,j=bw;j<ew;j++,jj++)
	{
	    if (j<0)
		continue;
	    for (player=0,mask=1;player<PLAYEDPLAYERS;player++,mask<<=1)
	    {
		scrfognr[player]=8;
	        if (bitplayers&mask)
	        {
		    if (!map.mapbits.whitefog[player]||!map.mapbits.blackfog[player])
			scrfognr[player]=8;
		    else
			if (map.mapbits.whitefog[player][i*MAXXMAP+j]==4)
			{
			    scrfognr[player]=0;
			}
			else
			{	
			    if (map.mapbits.whitefog[player][i*MAXXMAP+j]>=map.mapbits.blackfog[player][i*MAXXMAP+j])
			    {
				switch(map.mapbits.whitefog[player][i*MAXXMAP+j])
				{
				    case 3:
					scrfognr[player]=9;
					break;
				    case 2:
				        scrfognr[player]=10;
		    			break;
		        	    case 1:
					scrfognr[player]=11;
					break;
				    case 0:
				        scrfognr[player]=8;
					break;
				}
			    }
	    		    else
			    {
				switch(map.mapbits.whitefog[player][i*MAXXMAP+j])
				{
				    case 3://black==4
					scrfognr[player]=1;
					break;
				    case 2://black==3 or 4
					scrfognr[player]=2;
		    			break;
				    case 1://black==2 or 3 or 4
					switch(map.mapbits.blackfog[player][i*MAXXMAP+j])
					{	
			    		    case 4:
						scrfognr[player]=3;
						break;
					    case 3:
						scrfognr[player]=13;
						break;
					    case 2:
						scrfognr[player]=12;
						break;
					}
					break;
		    		    case 0:
					switch(map.mapbits.blackfog[player][i*MAXXMAP+j])
					{
			    		    case 4:
						scrfognr[player]=4;
						break;
					    case 3:
						scrfognr[player]=5;
						break;
					    case 2:
						scrfognr[player]=6;
						break;
					    case 1:
						scrfognr[player]=7;
						break;
					    case 0:
			    			scrfognr[player]=8;
						break;
					}
		    			break;
				}
			    }
			}
		}//if bits set
	    }//for player
	    //if screenfog changes set this bit to recalc fog sprites in putfog
	    fogvalue=0x08;//less value
	    for (player=0,mask=1;player<PLAYEDPLAYERS;player++,mask<<=1)
	    {
	        if (bitplayers&mask)
		    if (FOGDIAP[gameconf.videoconf.visiblemap][scrfognr[player]]>FOGDIAP[gameconf.videoconf.visiblemap][fogvalue])
			fogvalue=scrfognr[player];
	    }
	    if (screenfog[ii*SCRFOGX+jj]!=fogvalue)
		map.screenfogchange=1;
	    screenfog[ii*SCRFOGX+jj]=fogvalue;
	}//for jj
    }//for ii
    for (i=0;i<PLAYEDPLAYERS;i++)
    {
	map.clearfog[i]=0;
    }
}
//================================================
void ApplyFogMask(struct mapinfo *info,int playernr)
{
    int i,totaltiles;
    unsigned char mask;
    
    if (!info->FogMask)
	return;
    if (playernr >= PLAYEDPLAYERS)
	return;
    totaltiles = info->map_width * info->map_height;
    mask = (1<<playernr);
    for (i=0;i<totaltiles;i++)
    {
	if (!(info->FogMask[i] & mask))
	{
	    info->mapbits.whitefog[playernr][i]  = 4;
	    info->mapbits.blackfog[playernr][i]  = 4;
	    info->mapbits.whitefog2[playernr][i] = 4;
	}
    }
}
//=================================================


