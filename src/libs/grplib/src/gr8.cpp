#include <string.h>
#include "grplib/gr8.h"
#include "grplib/grp.h"

#define NONPIXEL 	255
#define MAXCOLORUNIT	31

struct XY{
       int x1;
       int y1;
       int x2;
       int y2;
};
//==============================================
void CClrScr(int color)
{
    memset(GRP_vidmem,color,GRP_wmaxdwordwritel<<2);
}
//==============================================
int CGetImage8(int x,int y,int sx,int sy,char *mem)
{
    if (x<GRP_wminx || y<GRP_wminy || x+sx>GRP_wmaxx+1 || y+sy>GRP_wmaxy+1)
	return -1;
    char *adrvid=GRP_vidmem+GRP_scanlineoffsets[y]+x;
    for (int i=0;i<sy;i++)
    {
	memcpy(mem,adrvid,sx);
	adrvid+=GRP_screensizex;
	mem+=sx;
    }
    return 0;
}
//==============================================
int CPutImage8(int x,int y,int sx,int sy,char *mem)
{
    if (x<GRP_wminx || y<GRP_wminy || x+sx>GRP_wmaxx+1 || y+sy>GRP_wmaxy+1)
	return -1;
    char *adrvid=GRP_vidmem+GRP_scanlineoffsets[y]+x;
    for (int i=0;i<sy;i++)
    {
	memcpy(adrvid,mem,sx);
	adrvid+=GRP_screensizex;
	mem+=sx;
    }
    return 0;
}
//==============================================
int CSetImage8(int x,int y,int sx,int sy,int color)
{
    if (x<GRP_wminx)
    {
	sx -= GRP_wminx-x;
	if (sx < 0)
	    return 0;
	x = GRP_wminx;
    }
    if (x+sx > GRP_wmaxx+1)
    {
        sx = GRP_wmaxx-x;
	if (sx < 0)
	    return 0;
    }
    if (y < GRP_wminy)
    {
	sy -= GRP_wminy-y+1;
	if (sy < 0)
	    return 0;
	y =GRP_wminy;
    }
    if (y + sy > GRP_wmaxy+1)
    {
        sy = GRP_wmaxy-y+1;
	if (sy < 0)
	    return 0;
    }	    
    char *adrvid=GRP_vidmem+GRP_scanlineoffsets[y]+x;
    for (int i=0;i<sy;i++)
    {
	memset(adrvid,color,sx);
	adrvid+=GRP_screensizex;
    }
    return 0;
}
//==============================================
int CSetImage8x(int x,int y,int sx,int sy,int color)
{
    color = (color&0xff) << 8;
    if (x<GRP_wminx)
    {
	sx -= GRP_wminx-x;
	if (sx < 0)
	    return 0;
	x = GRP_wminx;
    }
    if (x+sx>GRP_wmaxx+1)
    {
        sx = GRP_wmaxx-x+1;
	if (sx < 0)
	    return 0;
    }
    if (y<GRP_wminy)
    {
	sy -= GRP_wminy-y;
	if (sy < 0)
	    return 0;
    	y = GRP_wminy;
    }
    if (y+sy>GRP_wmaxy+1)
    {
        sy=GRP_wmaxy-y+1;
	if (sy < 0)
	    return 0;
    }
    char *adrvid=GRP_vidmem+GRP_scanlineoffsets[y]+x;
    for (int i=0;i<sy;i++)
    {
	for (int j=0;j<sx;j++)
	{
	    adrvid[j] = GRP_transpcolors[color|adrvid[j]];
	}
	adrvid+=GRP_screensizex;
    }
    return 0;
}
//==============================================
int CPutSprite8(int x,int y,int sx,int sy,int color,char *adr)
{
    if (x<GRP_wminx || y<GRP_wminy || x+sx>GRP_wmaxx+1 || y+sy>GRP_wmaxy+1)
	return -1;
    char *adrvid=GRP_vidmem+GRP_scanlineoffsets[y]+x;
    char c;
    for (int i=0;i<sy;i++)
    {
	for (int j=0;j<sx;j++)
	{
	    c = adr[j];
	    if (c < NONPIXEL)
	    {
		if (c <= MAXCOLORUNIT)
		{
		    c = color;
		}
	        if (c<GRP_wcomcolor)
		    c = GRP_transpcolors[adrvid[j]<<8|c];
		adrvid[j] = c;
	    }
	}
	adrvid+=GRP_screensizex;
	adr+=sx;
    }
    return 0;
}
//==============================================
void CPutPartVisibleSpr(int x,int y,int sx,int sy,char *mem)
{
    int skipx=0,skipy=0,i,sizex=sx;
    if (x<GRP_wminx)			//left clipping
    {
	skipx = GRP_wminx-x;
        sx-=skipx;
	if (sx<=0)
	    return;
	x = GRP_wminx;
    }
    if (y<GRP_wminy)
    {
        skipy=GRP_wminy-y;
	sy-=skipy;
	if (sy<=0)
	    return;
	y = GRP_wminy;
    }
    if (x+sx>GRP_wmaxx+1)
    {
	sx = GRP_wmaxx - x + 1;
	if (sx<=0)
    	    return;
    } 
    if (y+sy>GRP_wmaxy+1)
    {
	sy = GRP_wmaxy - y + 1;
	if (sy<=0)
	    return;
    } 
    char *adrvid = GRP_vidmem+GRP_scanlineoffsets[y]+x;
    mem += skipy*sizex+skipx;
    for (i=0;i<sy;i++)
    {
	memcpy(adrvid,mem,sx);
	adrvid+=GRP_screensizex;
	mem+=sizex;
    }
}
//==============================================
void CPutPartVisibleSprTrnsp(int x,int y,int sx,int sy,char *mem)
{
    int skipx=0,skipy=0,i,j,sizex=sx;
    if (x<GRP_wminx)			//left clipping
    {
	skipx = GRP_wminx-x;
        sx-=skipx;
	if (sx<=0)
	    return;
	x = GRP_wminx;
    }
    if (y<GRP_wminy)
    {
        skipy=GRP_wminy-y;
	sy-=skipy;
	if (sy<=0)
	    return;
	y = GRP_wminy;
    }
    if (x+sx>GRP_wmaxx+1)
    {
	sx = GRP_wmaxx - x + 1;
	if (sx<=0)
    	    return;
    } 
    if (y+sy>GRP_wmaxy+1)
    {
	sy = GRP_wmaxy - y + 1;
	if (sy<=0)
	    return;
    } 
    char *adrvid = GRP_vidmem+GRP_scanlineoffsets[y]+x;
    mem += skipy*sizex+skipx;
    for (i=0;i<sy;i++)
    {
	for (j=0;j<sx;j++)
	{
	    adrvid[j] = GRP_transpcolors[(adrvid[j]<<8)|mem[j]];
	}
	adrvid+=GRP_screensizex;
	mem+=sizex;
    }
}
//==============================================
void CPutSpr(int x,int y,int sx,int sy,int rx,char *mem)
{
    char *adrvid=GRP_vidmem+GRP_scanlineoffsets[y]+x;
    for (int i=0;i<sy;i++)
    {
	memcpy(adrvid,mem,sx);
	adrvid+=GRP_screensizex;
	mem+=rx;
    }
}
//==============================================
void wgetimage8(int x,int y,char *m)
{
    int *adr2 = (int *)m;
    int sy = adr2[0];
    int sx = adr2[1];
    CGetImage8(x,y,sx,sy,m+8);
}
//==============================================
void wputimage8(int x,int y,char *m)
{
    int *adr2 = (int *)m;
    int sy = adr2[0];
    int sx = adr2[1];
    CPutImage8(x,y,sx,sy,m+8);
}
//==============================================
void wsetimage8(int color,struct XY *m)
{
    CSetImage8(m->x1,m->y1,m->x2-m->x1+1,m->y2-m->y1+1,color);
}
//==============================================
void wsetimage8x(int color,struct XY *m)
{
    CSetImage8x(m->x1,m->y1,m->x2-m->x1+1,m->y2-m->y1+1,color);
}
//==============================================
void wputsprite8(int x,int y,char *adr,int color)
{
    int *adr2 = (int *)adr;
    int sy = adr2[0];
    int sx = adr2[1];
    CPutSprite8(x,y,sx,sy,color,adr+8);
}
//==============================================
void wputpartsprite8(int x,int y,char *adr)
{
    int *adr2 = (int *)adr;
    int sy = adr2[0];
    int sx = adr2[1];
    CPutPartVisibleSpr(x,y,sx,sy,adr+8);
}
//==============================================
void wputstasprite8(int x,int y,char *adr)
{
    int count,method = 0,line=0;
    count = 0;
    char *adrvid = GRP_vidmem + x + GRP_scanlineoffsets[y];
    do{
        count =(int)(*  (int *)adr);
	if (!count)
	    break;
	adr+=4;
	method ^= 1;
	line++;
	if (!method)
	{
	    memcpy(adrvid,adr,count);
	    adr+=count;
	}
        adrvid+=count;
    }while(1);
}
