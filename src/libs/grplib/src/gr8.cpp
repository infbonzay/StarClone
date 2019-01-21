#include <string.h>
#include "grplib/gr8.h"
#include "grplib/grp.h"

#define NONPIXEL 	255
#define MAXCOLORUNIT	31

//==============================================
void GRP_ClrScr(int color)
{
	memset(GRP_vidmem,color,GRP_wmaxdwordwritel<<2);
}
//==============================================
int GRP_GetImage8(int x,int y,int sx,int sy,char *mem)
{
	if (x<GRP_wminx || y<GRP_wminy || x+sx>GRP_wmaxx+1 || y+sy>GRP_wmaxy+1)
		return -1;
	char *adrvid=GRP_vidmem+GRP_scanlineoffsets[y]+x;
	for (int i=0;i<sy;i++)
	{
		memcpy(mem,adrvid,sx);
		adrvid += GRP_screensizex;
		mem += sx;
	}
	return 0;
}
//==============================================
int GRP_PutImage8(int x,int y,int sx,int sy,char *mem)
{
	if (x<GRP_wminx || y<GRP_wminy || x+sx>GRP_wmaxx+1 || y+sy>GRP_wmaxy+1)
		return -1;
	char *adrvid=GRP_vidmem+GRP_scanlineoffsets[y]+x;
	for (int i=0;i<sy;i++)
	{
		memcpy(adrvid,mem,sx);
		adrvid += GRP_screensizex;
		mem += sx;
	}
	return 0;
}
//==============================================
int GRP_SetImage8(int x,int y,int sx,int sy,int color)
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
		adrvid += GRP_screensizex;
	}
	return 0;
}
//==============================================
int GRP_SetImage8x(int x,int y,int sx,int sy,int color)
{
	unsigned int ucolor = (color & 0xff) << 8;
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
		sy = GRP_wmaxy-y+1;
		if (sy < 0)
			return 0;
	}
	unsigned char *adrvid = (unsigned char *)GRP_vidmem + GRP_scanlineoffsets[y] + x;
	for (int i=0;i<sy;i++)
	{
		for (int j=0;j<sx;j++)
		{
			adrvid[j] = GRP_transpcolors[ucolor | adrvid[j]];
		}
		adrvid += GRP_screensizex;
	}
	return 0;
}
//==============================================
int GRP_PutSprite8(int x,int y,int sx,int sy,int color,char *adr)
{
	if (x<GRP_wminx || y<GRP_wminy || x+sx>GRP_wmaxx+1 || y+sy>GRP_wmaxy+1)
		return -1;
	unsigned char *adrvid = (unsigned char *)GRP_vidmem+GRP_scanlineoffsets[y]+x;
	unsigned char c;
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
				if (c < GRP_wcomcolor)
					c = GRP_transpcolors[(adrvid[j] << 8) | c];
				adrvid[j] = c;
			}
		}
		adrvid += GRP_screensizex;
		adr += sx;
	}
	return 0;
}
//==============================================
void GRP_PutPartVisibleSpr(int x,int y,int sx,int sy,char *mem)
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
	char *adrvid = GRP_vidmem + GRP_scanlineoffsets[y] + x;
	mem += skipy * sizex + skipx;
	for (i=0;i<sy;i++)
	{
		memcpy(adrvid,mem,sx);
		adrvid += GRP_screensizex;
		mem += sizex;
	}
}
//==============================================
void GRP_PutPartVisibleSprTrnsp(int x,int y,int sx,int sy,char *mem)
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
	unsigned char *adrvid = (unsigned char *)GRP_vidmem + GRP_scanlineoffsets[y] + x;
	unsigned char *umem = (unsigned char *)mem + skipy * sizex + skipx;
	for (i=0;i<sy;i++)
	{
		for (j=0;j<sx;j++)
		{
			adrvid[j] = GRP_transpcolors[(adrvid[j] << 8) | umem[j]];
		}
		adrvid += GRP_screensizex;
		mem += sizex;
	}
}
//==============================================
void GRP_PutSpr(int x,int y,int sx,int sy,int rx,char *mem)
{
	char *adrvid = GRP_vidmem + GRP_scanlineoffsets[y] + x;
	for (int i=0;i<sy;i++)
	{
		memcpy(adrvid,mem,sx);
		adrvid += GRP_screensizex;
		mem += rx;
	}
}
//==============================================
void GRP_PutPackedSprite8(int x,int y,char *adr)
{
	int count,method = 0,line=0;
	count = 0;
	char *adrvid = GRP_vidmem + x + GRP_scanlineoffsets[y];
	do{
		count =(int)(*  (int *)adr);
		if (!count)
			break;
		adr += 4;
		method ^= 1;
		line++;
		if (!method)
		{
			memcpy(adrvid,adr,count);
			adr += count;
		}
		adrvid += count;
	}while(1);
}
