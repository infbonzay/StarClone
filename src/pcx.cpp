
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <grplib/gr8.h>
#include <grplib/grp.h>

#include "vars.h"
#include "auxil.h"
#include "wmem.h"
#include "mpqwrapper.h"
#include "mpq.h"
#include "pcx.h"

//==================
PCX::PCX(void)
{
	adrpict = NULL;
	adrpal = NULL;
	lsize = 0;
	xsize = 0;
	ysize = 0;
}
//==================
PCX::~PCX(void)
{
	closePcx();
}
//==================
void PCX::encgetMPQ(int *pbyt, int *pcnt, void *f)
/* where to place data */
/* where to place count */
/* image file handle */
{
	unsigned long readed;
	int i = 0;
	*pcnt = 1;		  /* assume a "run" length of one */
	wr_SFileReadFile(f, &i, 1, &readed, NULL);
	if (0xC0 == (0xC0 & i))
	{
		*pcnt = 0x3F & i;
		wr_SFileReadFile(f, &i, 1, &readed, NULL);
	}
	*pbyt = i;
}
//=======================================
int PCX::openMpqPcx(const char *filepcx, HANDLE hmpq)
{
	char *adrpict2;
	int chr, cnt;
	long int l, i;
	unsigned long readed;
	HANDLE openedfile;

	//	  char *filepcx2=filename2unix(filepcx);
	//	  wr_SFileOpenFileEx(NULL,filepcx2,SFILE_OPEN_LOCAL_FILE,&openedfile);
	//	  wfree(filepcx2);
	//	  if (!openedfile)
	//	  {
	SFileOpenFileTryAllMpqs(filepcx, &openedfile);
	if (!openedfile)
	{
		//			printf("%s not found\n",filepcx);
		return(NOPCX);
	}
	//	  }
	wr_SFileReadFile(openedfile, &header, sizeof(header), &readed, NULL);
	if (readed == sizeof(header))
	{
		if (header.Flag != 0x0a)
		{
			wr_SFileCloseFile(openedfile);
			return(NOPCX);
		}
		if (header.BitsPerPixel != 8 && header.NPlanes != 1)
		{
			wr_SFileCloseFile(openedfile);
			return(NO8BITPCX);
		}
		//		  xsize=header.xmax-header.xmin+1;
		xsize = header.BytesPerLine;
		ysize = header.ymax - header.ymin + 1;
		lsize = xsize * ysize;
		adrpict = (char *)wmalloc(lsize + 1);
		adrpict2 = adrpict;
		if (!adrpict)
		{
			wr_SFileCloseFile(openedfile);
			return(NOMEMORY);
		}
		adrpal = (char *)wmalloc(256 * 3);
		if (!adrpal)
		{
			wfree(adrpict);
			wr_SFileCloseFile(openedfile);
			return(NOMEMORY2);
		}
		for (l = 0; l < lsize; )			 /* increment by cnt below */
		{
			encgetMPQ(&chr, &cnt, openedfile);
			for (i = 0; i < cnt; i++)
			{
				if (l + i > lsize)
					break;
				*adrpict2++ = chr;
			}
			l += cnt;
		}
		wr_SFileReadFile(openedfile, adrpal, 1, &readed, NULL);
		wr_SFileReadFile(openedfile, adrpal, 3 * 256, &readed, NULL);
		wr_SFileCloseFile(openedfile);
	}
	else
	{
		wr_SFileCloseFile(openedfile);
		return(BADFILE);
	}
	return(OKPCX);
}
//=======================================
int PCX::openMpqPcx(const char *filepcx)
{
	HANDLE retmpqnr = MpqsFindFile(filepcx);
	//	  if (!retmpqnr)
	//		return(NOFILE);
	return(openMpqPcx(filepcx, retmpqnr));
}
//==================
void PCX::CreatePcx(int sizex, int sizey)
{
	lsize = sizex * sizey;
	adrpict = (char *)wmalloc(lsize);
	memset(adrpict, 0, lsize);
	xsize = sizex;
	ysize = sizey;
}
//==================
void PCX::closePcx(void)
{
	if (adrpict)
		wfree(adrpict);
	if (adrpal)
		wfree(adrpal);
	adrpict = NULL;
	adrpal = NULL;
	lsize = 0;
	xsize = 0;
	ysize = 0;
}
//==================
void PCX::readBytesFromPcx(char *dest)
{
	memcpy(dest, adrpict, lsize);
}
//==================
void PCX::readBytesFromPcx(char *dest, int lsize)
{
	memcpy(dest, adrpict, lsize);
}
//==================
void PCX::readPalFromPcx(char *dest)
{
	memcpy(dest, adrpal, 3 * 256);
}
//==================
void PCX::readPalFromPcx(char *dest, int fourbytepalette)
{
	for (int i = 0;i < 256;i++)
	{
		dest[i * 4 + 0] = adrpal[i * 3 + 0];
		dest[i * 4 + 1] = adrpal[i * 3 + 1];
		dest[i * 4 + 2] = adrpal[i * 3 + 2];
		dest[i * 4 + 3] = 0;
	}
}
//==================
int PCX::IfPcxIsReady(void)
{
	return(adrpict != NULL);
}
//==================
long int PCX::sizePcx(void)
{
	return(lsize);
}
//==================
long int PCX::xsizePcx(void)
{
	return(xsize);
}
//==================
long int PCX::ysizePcx(void)
{
	return(ysize);
}
//==================
char *PCX::GetPcxRawBytes(void)
{
	return(adrpict);
}
//==================
char *PCX::GetRawPal768(void)
{
	return(adrpal);
}
//==================
void PCX::PutPcx(int x, int y, unsigned char emptycolor)
{
	PutPcx(x, y, 0, ysize, emptycolor, 0, PCXSHOW_100);
}
//==================
void PCX::PutPcx(int x, int y, unsigned char emptycolor, unsigned char transparentcolor, int grdnr)
{
	PutPcx(x, y, 0, ysize, emptycolor, transparentcolor, grdnr);
}
//==================
void PCX::PutPcx(int x, int y, int showx, unsigned char emptycolor, unsigned char transparentcolor, int grdnr)
{
	PutPcx(x, y, showx, 0, ysize, emptycolor, transparentcolor, grdnr);
}
//==================
void PCX::PutPcx(int x, int y, int fromline, int nroflines, unsigned char emptycolor, unsigned char transparentcolor, int grdnr)
{
	PutPcx(x, y, xsize, fromline, nroflines, emptycolor, transparentcolor, grdnr);
}
//==================
void PCX::PutPcx(int x, int y, int showx, int fromline, int nroflines, unsigned char emptycolor, unsigned char transparentcolor, int grdnr)
{
	int sx, sy;
	unsigned char *bytes = (unsigned char *)adrpict;
	unsigned char *adrvid, *adrtr;
	sx = showx;
	//	  sx=xsize;
	if (fromline + nroflines > ysize)
		sy = ysize - fromline;
	else
		sy = nroflines;
	//	  int skipx=0,skipy=0,i,j,sizex=sx;
	int skipx = 0, skipy = 0, i, j, sizex = xsize;
	if (x < GRP_wminx)					//left clipping
	{
		skipx = GRP_wminx - x;
		sx -= skipx;
		if (sx <= 0)
			return;
		x = GRP_wminx;
	}
	if (y < GRP_wminy)
	{
		skipy = GRP_wminy - y;
		sy -= skipy;
		if (sy <= 0)
			return;
		y = GRP_wminy;
	}
	if (x + sx > GRP_wmaxx + 1)
	{
		sx = GRP_wmaxx - x + 1;
		if (sx <= 0)
			return;
	}
	if (y + sy > GRP_wmaxy + 1)
	{
		sy = GRP_wmaxy - y + 1;
		if (sy <= 0)
			return;
	}
	bytes += (skipy + fromline)*sizex + skipx;
	adrvid = gameconf.grmode.videobuff + GRP_scanlineoffsets[y] + x;
	switch (grdnr)
	{
	case PCXSHOW_25:		//25%
		adrtr = (unsigned char *)GRP_transpcolors + 0 * 256 * 256;
		for (i = 0;i < sy;i++)
		{
			for (j = 0;j < sx;j++)
			{
				if (bytes[j] == emptycolor)
					continue;
				else
					if (bytes[j] == transparentcolor)
						adrvid[j] = adrtr[adrvid[j]];
					else
						adrvid[j] = bytes[j];
			}
			adrvid += gameconf.grmode.x;
			bytes += sizex;
		}
		break;
	case PCXSHOW_50:		//50%
		adrtr = (unsigned char *)GRP_transpcolors + 1 * 256 * 256;
		for (i = 0;i < sy;i++)
		{
			for (j = 0;j < sx;j++)
			{
				if (bytes[j] == emptycolor)
					continue;
				else
					if (bytes[j] == transparentcolor)
						adrvid[j] = adrtr[adrvid[j]];
					else
						adrvid[j] = bytes[j];
			}
			adrvid += gameconf.grmode.x;
			bytes += sizex;
		}
		break;
	case PCXSHOW_75:		//75%
		adrtr = (unsigned char *)GRP_transpcolors + 0 * 256 * 256;
		for (i = 0;i < sy;i++)
		{
			for (j = 0;j < sx;j++)
			{
				if (bytes[j] == emptycolor)
					continue;
				else
					if (bytes[j] == transparentcolor)
						adrvid[j] = adrtr[((short)(adrvid[j]) * 256) + 0];
					else
						adrvid[j] = bytes[j];
			}
			adrvid += gameconf.grmode.x;
			bytes += sizex;
		}
		break;
	case PCXSHOW_100:		//100%
		for (i = 0;i < sy;i++)
		{
			for (j = 0;j < sx;j++)
			{
				if (bytes[j] != emptycolor)
					adrvid[j] = bytes[j];
			}
			adrvid += gameconf.grmode.x;
			bytes += sizex;
		}
		break;
	}
}
//==========================
void PCX::PutScaledPcx(int x, int y, char emptycolor)
{
	int i, j, offsofbyte, deltay, prevdeltay;
	unsigned char *bytes = (unsigned char *)adrpict;
	unsigned char *adrvid;
	adrvid = gameconf.grmode.videobuff + GRP_scanlineoffsets[0] + 0;
	deltay = prevdeltay = 0;
	for (i = 0;i < gameconf.grmode.y;i++)
	{
		for (j = 0;j < gameconf.grmode.x;j++)
		{
			offsofbyte = j * xsize / gameconf.grmode.x;
			if (bytes[offsofbyte] != emptycolor)
				adrvid[j] = bytes[offsofbyte];
		}
		adrvid += gameconf.grmode.x;
		deltay = (i*ysize / gameconf.grmode.y);
		if (deltay != prevdeltay)
		{
			prevdeltay = deltay;
			bytes += xsize;
		}
	}
}
//==================
