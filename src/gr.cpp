
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "vars.h"
#include "auxil.h"
#include "key.h"
#include "wmem.h"
#include "gr.h"
#include "gener.h"
#include "version.hxx"
#include "Controller.h"

#include <grplib/grp.h>
#include <grplib/gr8.h>

//==========================
char	*_tored_, *_togreen_, *_toblue_, *_towhite_, *_toblack_, *tomono;
char	*adrtabls[7];
int		windowx1, windowx2;

//==========================
void initadrtables(void)
{
	adrtabls[NONETABLE] = NULL;
	adrtabls[REDTABLE] = _tored_;
	adrtabls[GREENTABLE] = _togreen_;
	adrtabls[BLUETABLE] = _toblue_;
	adrtabls[WHITETABLE] = _towhite_;
	adrtabls[BLACKTABLE] = _toblack_;
	adrtabls[TFONTGAMTABLE] = tfontgamp;
}
//==========================
void horizline(int color, int x1, int x2, int y)
{
	int sx = 0;
	unsigned char *vm;
	void *vm1;
	if (y < GRP_wminy)
		return;
	if (y > GRP_wmaxy)
		return;
	if (x2 < x1)
		swap(&x1, &x2);
	if (x1 < GRP_wminx)
		x1 = GRP_wminx;
	if (x2 < GRP_wminx)
		x2 = GRP_wminx;
	if (x1 > GRP_wmaxx)
		x1 = GRP_wmaxx;
	if (x2 > GRP_wmaxx)
		x2 = GRP_wmaxx;
	sx = x2 - x1;
	if (!sx)
		return;
	vm1 = gameconf.grmode.videobuff + (y*gameconf.grmode.x + x1);
	vm = (unsigned char *)vm1;
	do {
		*vm = (char)color;
		vm++;
		x1++;
	} while (x1 <= x2);
}
//==========================
void vertline(int color, int x, int y1, int y2)
{
	int sy;
	unsigned char *vm;
	void *vm1;
	if (x < GRP_wminx)
		return;
	if (x > GRP_wmaxx)
		return;
	if (y2 < y1)
		swap(&y1, &y2);
	if (y1 < GRP_wminy)
		y1 = GRP_wminy;
	if (y2 < GRP_wminy)
		y2 = GRP_wminy;
	if (y1 > GRP_wmaxy)
		y1 = GRP_wmaxy;
	if (y2 > GRP_wmaxy)
		y2 = GRP_wmaxy;
	sy = y2 - y1;
	if (!sy)
		return;
	vm1 = gameconf.grmode.videobuff + (y1*gameconf.grmode.x + x);
	vm = (unsigned char *)vm1;
	do {
		*vm = (char)color;
		vm += gameconf.grmode.x;
		y1++;
	} while (y1 <= y2);
}
//==========================
void wrectangle(int color, int x1, int y1, int x2, int y2)
{
	if (x1 > x2)
		swap(&x1, &x2);
	if (y1 > y2)
		swap(&y1, &y2);
	horizline(color, x1 + 1, x2 - 1, y1);
	horizline(color, x1 + 1, x2 - 1, y2);
	vertline(color, x1, y1 + 1, y2 - 1);
	vertline(color, x2, y1 + 1, y2 - 1);
}
//==========================
void wrectangle2(int color, int x1, int y1, int x2, int y2)
{
	if (x1 > x2)
		swap(&x1, &x2);
	if (y1 > y2)
		swap(&y1, &y2);
	horizline(color, x1, x2 - 1, y1);
	horizline(color, x1, x2 - 1, y2 - 1);
	vertline(color, x1, y1, y2 - 1);
	vertline(color, x2 - 1, y1, y2 - 1);
}
//==========================
int imagesize(struct XY *x, int dd)
{
	return(((abs(x->x1 - x->x2) + dd)*(abs(x->y1 - x->y2) + dd) + 8));
}
//==========================
//modes(1-transparent,0-no)
void lines(int x1, int x2, int y1, int n, int modes)
{
	int adr;
	if (y1 >= GRP_wminy && y1 <= GRP_wmaxy && x1 <= GRP_wmaxx && x2 >= GRP_wminx)
	{
		if (x1 < GRP_wminx)
			x1 = GRP_wminx;
		if (x2 > GRP_wmaxx)
			x2 = GRP_wmaxx;
		adr = GRP_scanlineoffsets[y1];
		if (!modes)
		{
			for (int i = x1;i <= x2;i++)
				gameconf.grmode.videobuff[i + adr] = (char)n;
		}
		else
		{
			for (int i = x1;i <= x2;i++)
				gameconf.grmode.videobuff[i + adr] = GRP_transpcolors[gameconf.grmode.videobuff[i + adr] * 256 + n];
		}
	}
}
//=====================================
int setmode(int x, int y, int bpp, int fullscreen)
{
	char cap[200];
	int a;
	if (bpp != 8)
	{
		printf("non 8-bit resolution not supported\n");
		return (-1);
	}

	InitGrpLib(x, y);

	DELTASCREENX = (gameconf.grmode.x - 640) / 2;
	DELTASCREENY = gameconf.grmode.y - 480;
	DELTASCREENY2 = (gameconf.grmode.y - 480) / 2;

	a = mainController.QueryVideoMode(x, y, bpp, fullscreen);

	if (a)
	{
		int ver = GetGRPLibVer();
		sprintf(cap, GAMENAME " " GAMEVERSION " with GRPlib-%d.%d.%d createtime:" GAMECOMPILE, (ver >> 16) & 0xff, ((ver >> 8) & 0xff), ver & 0xff);
		mainController.SetWindowName(cap);
		highMouse->PosX = 0;
		highMouse->PosY = 0;
	}
	return(a);
}
//==========================
void putcube(int x, int y, int sizex, int sizey, char color)
{
	int i;
	unsigned char *vidbuf;
	vidbuf = gameconf.grmode.videobuff + GRP_scanlineoffsets[y] + x;
	for (i = 0;i < sizey;i++)
	{
		memset(vidbuf, color, sizex);
		vidbuf += gameconf.grmode.x;
	}
}
//==========================
void puttranslucencyrow(int x, int y, int sizex, int sizey, unsigned char *buff, int grdnr)//0-25%,1-50%,2-75%,3-100%
{
	int i, j;
	unsigned char *vidbuf, *tr_table;
	unsigned char pixel;
	vidbuf = gameconf.grmode.videobuff + GRP_scanlineoffsets[y] + x;
	switch (grdnr)
	{
	case 0:		//25%
		tr_table = (unsigned char *)GRP_transpcolors + 0 * 256 * 256;	//table 0(75%) and inverse source&dest pixels
		for (i = 0;i < sizey;i++)
		{
			for (j = 0;j < sizex;j++)
			{
				pixel = *buff++;
				if (pixel && pixel != (*vidbuf))
				{
					*vidbuf = tr_table[(*vidbuf) * 256 + pixel];
				}
				vidbuf++;
			}
			vidbuf += gameconf.grmode.x - sizex;
		}
		break;
	case 1:		//50%
		tr_table = (unsigned char *)GRP_transpcolors + 1 * 256 * 256;	//table 1(50%) and normal source&dest pixels
		for (i = 0;i < sizey;i++)
		{
			for (j = 0;j < sizex;j++)
			{
				pixel = *buff++;
				if (pixel && pixel != (*vidbuf))
				{
					*vidbuf = tr_table[pixel * 256 + (*vidbuf)];
				}
				vidbuf++;
			}
			vidbuf += gameconf.grmode.x - sizex;
		}
		break;
	case 2:		//75%
		tr_table = (unsigned char *)GRP_transpcolors + 0 * 256 * 256;	//table 0(75%) and normal source&dest pixels
		for (i = 0;i < sizey;i++)
		{
			for (j = 0;j < sizex;j++)
			{
				pixel = *buff++;
				if (pixel && pixel != (*vidbuf))
				{
					*vidbuf = tr_table[pixel * 256 + (*vidbuf)];
				}
				vidbuf++;
			}
			vidbuf += gameconf.grmode.x - sizex;
		}
		break;
	case 3:		//100%
		putrow(x, y, sizex, sizey, buff);
		break;
	}
}
//==========================
void putrowwithtable(int x, int y, int sizex, int sizey, unsigned char *buff, char *table)
{
	int i, j;
	unsigned char *vidbuf;
	unsigned char pixel;
	vidbuf = gameconf.grmode.videobuff + GRP_scanlineoffsets[y] + x;
	for (i = 0;i < sizey;i++)
	{
		//	memcpy(vidbuf,buff,sizex);
		//	buff+=sizex;
		//	vidbuf+=gameconf.grmode.x;
		for (j = 0;j < sizex;j++)
		{
			pixel = *buff++;
			if (pixel)
			{
				*vidbuf++ = table[pixel];
			}
			else
				vidbuf++;
		}
		//buff+=sizex-sizex;
		vidbuf += gameconf.grmode.x - sizex;
	}
}
//==========================
void putrow(int x, int y, int sizex, int sizey, unsigned char *buff)
{
	int i, j;
	unsigned char *vidbuf;
	unsigned char pixel;
	vidbuf = gameconf.grmode.videobuff + GRP_scanlineoffsets[y] + x;
	for (i = 0;i < sizey;i++)
	{
		//	memcpy(vidbuf,buff,sizex);
		//	buff+=sizex;
		//	vidbuf+=gameconf.grmode.x;
		for (j = 0;j < sizex;j++)
		{
			pixel = *buff++;
			if (pixel)
			{
				*vidbuf++ = pixel;
			}
			else
				vidbuf++;
		}
		//buff+=sizex-sizex;
		vidbuf += gameconf.grmode.x - sizex;
	}
}
//==========================
void putrow2x2(int x, int y, int sizex, int sizey, unsigned char *buff)
{
	int i, j, posx;
	unsigned char *vidbuf;
	vidbuf = gameconf.grmode.videobuff + GRP_scanlineoffsets[y] + x;
	posx = 0;
	sizey *= 2;
	for (i = 0;i < sizey;i++)
	{
		if (i & 1)
		{
			memset(vidbuf + posx, 0, sizex * 2);
			posx += gameconf.grmode.x;
		}
		else
		{
			for (j = 0;j < sizex;j++)
			{
				vidbuf[posx++] = *buff;
				vidbuf[posx++] = *buff++;
			}
			posx += gameconf.grmode.x - sizex * 2;
		}
	}
}
//==========================
void putrow2x1(int x, int y, int sizex, int sizey, unsigned char *buff)
{
	int i, j;
	unsigned char *vidbuf;
	vidbuf = gameconf.grmode.videobuff + GRP_scanlineoffsets[y] + x;
	for (i = 0;i < sizey;i++)
	{
		for (j = 0;j < sizex;j++)
		{
			*vidbuf++ = *buff;
			*vidbuf++ = *buff++;
		}
		vidbuf += gameconf.grmode.x - sizex * 2;
	}
}
/*==========================*/
void palettegamma(char *palette,char *origpalette, int factor)		// palette 256*4 bytes,factor -50 +49
{
	int i, r, g, b, newr, newg, newb;

	for (i = 0;i < 256;i++)
	{
		r = (unsigned char)origpalette[i * 4 + 0];
		g = (unsigned char)origpalette[i * 4 + 1];
		b = (unsigned char)origpalette[i * 4 + 2];
		newr = r + r * factor / (MAXGAMMA - 1);
		newg = g + g * factor / (MAXGAMMA - 1);
		newb = b + b * factor / (MAXGAMMA - 1);
		if (factor < 0)
		{
			if (newr < 0)
				newr = 0;
			if (newg < 0)
				newg = 0;
			if (newb < 0)
				newb = 0;
		}
		else
		{
			if (newr > 255)
				newr = 255;
			if (newg > 255)
				newg = 255;
			if (newb > 255)
				newb = 255;
		}
		palette[i * 4 + 0] = newr;
		palette[i * 4 + 1] = newg;
		palette[i * 4 + 2] = newb;
	}
}
//==========================
void palettemono(char *palette,char *origpalette, int factor)		// palette 256*4 bytes,factor 0 - 99
{
	int i, r, g, b, grey, newr, newg, newb;

	for (i = 0;i < 256;i++)
	{
		r = (unsigned char)origpalette[i * 4 + 0];
		g = (unsigned char)origpalette[i * 4 + 1];
		b = (unsigned char)origpalette[i * 4 + 2];
		grey = (r + g + b) / 3;

		newr = grey + (r - grey) * factor / (MAXSATURATE / 2);
		newg = grey + (g - grey) * factor / (MAXSATURATE / 2);
		newb = grey + (b - grey) * factor / (MAXSATURATE / 2);
		if (newr < 0)
			newr = 0;
		if (newg < 0)
			newg = 0;
		if (newb < 0)
			newb = 0;
		if (newr > 255)
			newr = 255;
		if (newg > 255)
			newg = 255;
		if (newb > 255)
			newb = 255;
		palette[i * 4 + 0] = newr;
		palette[i * 4 + 1] = newg;
		palette[i * 4 + 2] = newb;
	}
}
//==========================
void pal3to4(char *pal4, char *pal3)
{
	for (int i = 0;i < 256; i++)
	{
		pal4[i * 4 + 0] = pal3[i * 3 + 0];
		pal4[i * 4 + 1] = pal3[i * 3 + 1];
		pal4[i * 4 + 2] = pal3[i * 3 + 2];
		pal4[i * 4 + 3] = 0;
	}
}
//==========================
void pal4to3(char *pal3, char *pal4)
{
	for (int i = 0; i < 256; i++)
	{
		pal3[i * 3 + 0] = pal4[i * 4 + 0];
		pal3[i * 3 + 1] = pal4[i * 4 + 1];
		pal3[i * 3 + 2] = pal4[i * 4 + 2];
	}
}
//==========================
void putgrp_nopacked2(int x, int y, int onlyx, int onlyy, GRPFILE *grp, int pictnr)
{
	int addx, addy, j, sizex, sizey, decrx;
	unsigned char *vidadr, *bytes;
	unsigned char colorbyte;
	if (!onlyx)
		return;
	if (!onlyy)
		return;
	if (grp->SizeX > 255)//the only way to check if nopacked grp it is.
	{
		if (pictnr < grp->CountPictures)
		{
			vidadr = gameconf.grmode.videobuff;
			bytes = (unsigned char *)grp;
			addx = grp->Picture[pictnr].SkipLeft;
			addy = grp->Picture[pictnr].SkipUp;
			sizey = grp->Picture[pictnr].LinesPerPicture;
			sizex = grp->Picture[pictnr].PixelPerLine;
			vidadr += GRP_scanlineoffsets[y + addy] + x + addx;
			bytes += grp->Picture[pictnr].OffsetForLine;
			if (onlyy > sizey)
				onlyy = sizey;
			if (onlyx > sizex)
				onlyx = sizex;
			do {
				decrx = onlyx;
				j = sizex = grp->Picture[pictnr].PixelPerLine;
				do {
					colorbyte = *bytes++;
					if (colorbyte > 0 && colorbyte < 254)
						*vidadr = colorbyte;
					vidadr++;
				} while (--decrx);
				vidadr += sizex - onlyx;
				bytes += sizex - onlyx;
				vidadr += gameconf.grmode.x - sizex;
			} while (--onlyy);
		}
	}
}
//====================================
void drawmonoifpaused(void)
{
	int i, screensize = GRP_wmaxdwordwritel * 4;
	if (PAUSEGAME && NETWORKGAME)
	{
		for (i = 0;i < screensize;i++)
			gameconf.grmode.videobuff[i] = tomono[gameconf.grmode.videobuff[i]];
	}
}
//====================================
char *savescreen(void)
{
	char *scr = (char *)wmalloc(GRP_wmaxdwordwritel * 4);
	memcpy(scr, gameconf.grmode.videobuff, GRP_wmaxdwordwritel * 4);
	return(scr);
}
//====================================
void restorescreen(char *scr)
{
	memcpy(gameconf.grmode.videobuff, scr, GRP_wmaxdwordwritel * 4);
}
//====================================

