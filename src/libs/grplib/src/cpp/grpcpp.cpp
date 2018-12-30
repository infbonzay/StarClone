
#include <string.h>
#include <stdio.h>
#include "grplib/grp.h"
#include "grplib/gr8.h"

int     GRP_wminx,GRP_wminy,GRP_wmaxx,GRP_wmaxy;
int     GRP_screensizex,GRP_screensizey;
int     GRP_sizexwarppict,GRP_sizeywarppict;
char    *GRP_vidmem;
char    *GRP_transpcolors;
int     *GRP_scanlineoffsets;
char    *GRP_tableforunitcolor;
short   int GRP_wcomandcolor,GRP_wcomcolor;
unsigned char GRP_gr_color,GRP_gr_maxcolordesen;
int     GRP_wmaxdwordwritel;

//==========================================================
inline void invmemset(unsigned char *adrmem,char mask,int count)
{
	memset(adrmem - count + 1,mask,count);
}
//==========================================================
#define _GRPFROMTEXT1_
#define _GRPFROMTEXT2_
#define _GRPFROMTEXT3_
#define _GRPFROMTEXT4_
#define _GRPFROMTEXTSKIP_

#include "grpsmpls.h"
#include "grpdefns.h"
//==========================================================
//=========================================================
void putgrp(long x,long y,struct GRPFILE *GRP,long nrpicture,long grcolor)
{
	_PUTGRPSAMPLE_(_SIMPLEGRPDEF1,_SIMPLEGRPDEF2);
}
//=========================================================
void iputgrp(long x,long y,struct GRPFILE *GRP,long nrpicture,long grcolor)
{
	_PUTINVGRPSAMPLE_(_SIMPLEINVGRPDEF1,_SIMPLEINVGRPDEF2);
}
//=========================================================
void putgrptransp(long x,long y,struct GRPFILE *GRP,long nrpicture,long grcolor)
{
	_PUTGRPSAMPLE_(_TRANSPGRPDEF1,_TRANSPGRPDEF2);
}
//=========================================================
void iputgrptransp(long x,long y,struct GRPFILE *GRP,long nrpicture,long grcolor)
{
	_PUTINVGRPSAMPLE_(_TRANSPINVGRPDEF1,_TRANSPINVGRPDEF2);
}
//=========================================================
void putgrpshadow(long x,long y,struct GRPFILE *GRP,long nrpicture,char *table,long grdfactor)
{
	table+=grdfactor;
	_PUTGRPSAMPLE_(_SHADOWGRPDEF1,_SHADOWGRPDEF2);
}
//=========================================================
void iputgrpshadow(long x,long y,struct GRPFILE *GRP,long nrpicture,char *table,long grdfactor)
{
	table+=grdfactor;
	_PUTINVGRPSAMPLE_(_SHADOWINVGRPDEF1,_SHADOWINVGRPDEF2);
}
//=========================================================
void putgrpnonvis(long x,long y,struct GRPFILE *GRP,long nrpicture)
{
	_PUTGRPSAMPLE_(_NONVISGRPDEF1,_NONVISGRPDEF2);
}
//=========================================================
void iputgrpnonvis(long x,long y,struct GRPFILE *GRP,long nrpicture)
{
	_PUTINVGRPSAMPLE_(_NONVISINVGRPDEF1,_NONVISINVGRPDEF2);
}
//=========================================================
void putgrpgrd(long x,long y,GRPFILE *GRP,long nrpicture,char *table)
{
	_PUTGRPSAMPLE_(_GRDGRPDEF1,_GRDGRPDEF2);
}
//=========================================================
void iputgrpgrd(long x,long y,GRPFILE *GRP,long nrpicture,char *table)
{
	_PUTINVGRPSAMPLE_(_GRDINVGRPDEF1,_GRDINVGRPDEF2);
}
//=========================================================
void putgrptotable(long x,long y,GRPFILE *GRP,long nrpicture,char *table,long grdfactor,long grcolor)
{
	table+=grdfactor;
	_PUTGRPSAMPLE_(_TOTABLEGRPDEF1,_TOTABLEGRPDEF2);
}
//=========================================================
void iputgrptotable(long x,long y,GRPFILE *GRP,long nrpicture,char *table,long grdfactor,long grcolor)
{
	table+=grdfactor;
	_PUTINVGRPSAMPLE_(_TOTABLEINVGRPDEF1,_TOTABLEINVGRPDEF2);
}
//=========================================================
#undef _GRPFROMTEXT1_
#define _GRPFROMTEXT1_ adrtext += GRP_sizexwarppict*FromLine

#undef _GRPFROMTEXT2_
#define _GRPFROMTEXT2_ adrtext += SkipLeftPixels

#undef _GRPFROMTEXT3_
#define _GRPFROMTEXT3_ adrtext += GRP_sizexwarppict - PixelPerLine

#undef _GRPFROMTEXT4_
#define _GRPFROMTEXT4_ adrtext += GRP_sizexwarppict - AlreadyPut

#undef _GRPFROMTEXTSKIP_
#define _GRPFROMTEXTSKIP_ adrtext += packbyte

void putgrpwarpfromtext(long x,long y,GRPFILE *GRP,long nrpicture,char *adrtext)
{

	#undef _PUTGRPSAMPLE_
	#undef _PUTINVGRPSAMPLE_
	#define _PUTFROMTEXTURE_

	#include "grpsmpls.h"

	_PUTGRPSAMPLE_(_FROMTEXTUREGRPDEF1,_FROMTEXTUREGRPDEF2);

	#undef  _PUTFROMTEXTURE_
	#undef _PUTGRPSAMPLE_
	#undef _PUTINVGRPSAMPLE_

	#undef _GRPFROMTEXT1_
	#undef _GRPFROMTEXT2_
	#undef _GRPFROMTEXT3_
	#undef _GRPFROMTEXT4_
	#undef _GRPFROMTEXTSKIP_

	#define _GRPFROMTEXT1_
	#define _GRPFROMTEXT2_
	#define _GRPFROMTEXT3_
	#define _GRPFROMTEXT4_
	#define _GRPFROMTEXTSKIP_

}
//=========================================================
void putspr32x32(long x,long y,char *bytes)
{
	int sx=32;
	int sy=32;
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
	char *adrvid = GRP_vidmem + GRP_scanlineoffsets[y] + x;
	bytes += skipy * sizex + skipx;
	for (i=0;i<sy;i++)
	{
		memcpy(adrvid,bytes,sx);
		adrvid += GRP_screensizex;
		bytes += sizex;
	}
}
//=========================================================
//need for fastest starcraft fog
void putfog32x32(long x,long y,char *table,long lt,long rt,long lb,long rb)
{
	unsigned char *adrvid;
	int fromy,toy,fromx,deltax,deltay1,deltay2;
	int fogfunc=0;
	int sx=32;
	int sy=32;
	int skipx=0,skipy=0,i,j;
	if (x<GRP_wminx)			//left clipping
	{
		fogfunc|=1;
		skipx = GRP_wminx-x;
		sx-=skipx;
		if (sx<=0)
			return;
		x = GRP_wminx;
	}
	if (y<GRP_wminy)
	{
		fogfunc|=2;
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
	if (fogfunc--==0)	//fogfunc=0 noclip,rightclip&downclip is here
	{
		adrvid = (unsigned char *)GRP_vidmem + GRP_scanlineoffsets[y] + x;
		fromy = lt << 8;
		toy = rt << 8;
		deltay1 = (lb-lt) << 3;
		deltay2 = (rb-rt) << 3;
		for (i=0;i<sy;i++)
		{
			fromx = fromy;
			deltax = (toy-fromx) >> 5;
			for (j=0;j<sx;j++)
			{
				adrvid[j] = table[( ( fromx + 0x80) & 0xff00 ) | adrvid[j]];
				fromx += deltax;
			}
			fromy += deltay1;
			toy += deltay2;
			adrvid += GRP_screensizex;
		}
		return;
	}
	if (fogfunc--==0)	//fogfunc=1 leftclipping
	{
		adrvid = (unsigned char *)GRP_vidmem + GRP_scanlineoffsets[y] + x + sx - 1;
		fromy = rt << 8;
		toy = lt << 8;
		deltay1 = (rb-rt) << 3;
		deltay2 = (lb-lt) << 3;
		for (i=0;i<sy;i++)
		{
			fromx = fromy;
			deltax = (toy-fromx) >> 5;
			for (j=0;j<sx;j++)
			{
				adrvid[-j] = table[( (fromx + 0x80) & 0xff00 ) | adrvid[-j]];
				fromx += deltax;
			}
			fromy += deltay1;
			toy += deltay2;
			adrvid += GRP_screensizex;
		}
		return;
	}
	if (fogfunc--==0)	//fogfunc=2 up clipping
	{
		adrvid = (unsigned char *)GRP_vidmem + GRP_scanlineoffsets[y + sy - 1] + x;
		fromy = lb<<8;
		toy = rb<<8;
		deltay1 = (lt-lb) << 3;
		deltay2 = (rt-rb) << 3;
		for (i=0;i<sy;i++)
		{
			fromx = fromy;
			deltax = (toy-fromx) >> 5;
			for (j=0;j<sx;j++)
			{
				adrvid[j] = table[( (fromx + 0x80) & 0xff00) | adrvid[j]];
				fromx += deltax;
			}
			fromy += deltay1;
			toy += deltay2;
			adrvid -= GRP_screensizex;
		}
		return;
	}
	if (fogfunc--==0)	//fogfunc=3 left&up clipping
	{
		adrvid = (unsigned char *)GRP_vidmem + GRP_scanlineoffsets[y + sy - 1] + x + sx - 1;
		fromy = rb<<8;
		toy = lb<<8;
		deltay1 = (rt-rb)<<3;
		deltay2 = (lt-lb)<<3;
		for (i=0;i<sy;i++)
		{
			fromx = fromy;
			deltax = (toy-fromx)>>5;
			for (j=0;j<sx;j++)
			{
				adrvid[-j] = table[( (fromx + 0x80) & 0xff00) | adrvid[-j]];
				fromx += deltax;
			}
			fromy += deltay1;
			toy += deltay2;
			adrvid -= GRP_screensizex;
		}
		return;
	}
}
//====================================
void putgrp_nopacked(int x,int y,GRPFILE *grp,int pictnr)
{
	int addx,addy,j,sizex,sizey;
	unsigned char *vidadr,*bytes;
	unsigned char colorbyte;
	//    if (grp->SizeX>255)//the only way to check if nopacked grp it is.
	if (pictnr<grp->CountPictures)
	{
		vidadr = (unsigned char *)GRP_vidmem;
		bytes = (unsigned char *)grp;
		addx = grp->Picture[pictnr].SkipLeft;
		addy = grp->Picture[pictnr].SkipUp;
		sizey = grp->Picture[pictnr].LinesPerPicture;
		sizex = grp->Picture[pictnr].PixelPerLine;
		vidadr += GRP_scanlineoffsets[y + addy] + x + addx;
		bytes += grp->Picture[pictnr].OffsetForLine;
		do{
			j = sizex = grp->Picture[pictnr].PixelPerLine;
			do{
				colorbyte = *bytes++;
				if (colorbyte > 0 && colorbyte < 254)
					*vidadr = colorbyte;
				vidadr++;
			}while(--j);
			vidadr += GRP_screensizex - sizex;
		}while(--sizey);
	}
}
//====================================
