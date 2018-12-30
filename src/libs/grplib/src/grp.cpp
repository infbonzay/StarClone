/*
 *    grp-lib v. 0.4.17
 *    Created by Botnary Vasya
 *    Copyright 2016
 *
 *    initialization grplib & load grp's from file
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grplib/grp.h"
#include "grplib/gr8.h"

#define ADDMALLOCLINES 32
//=======================================
static int MaxIndexLines=0;

void GRP_recalcscanlines(void)
{
	//GRP_scanlineoffsets is array of offset each scanline for video buf
	//need for fast access address of each line
	if (GRP_scanlineoffsets)		//if table exist we free and malloc another
	{
		free(GRP_scanlineoffsets);
	}
	GRP_scanlineoffsets = (int *)malloc(GRP_screensizey * sizeof(int));
	for (int i=0;i<GRP_screensizey;i++)
		GRP_scanlineoffsets[i] = i * GRP_screensizex;	//on every line we put offset
}
//=======================================
//cliping function
int wviewport(int x,int y,int sizex,int sizey)
{
	if (x>=0 && y>=0 && x<GRP_screensizex && y<GRP_screensizey)
	{
		GRP_wminx = x;			//set clipzone variables
		GRP_wminy = y;
		GRP_wmaxx = x + sizex-1;
		GRP_wmaxy = y + sizey-1;
		return 0;
	}
	else
		return 1;
}
//===========================================
void QuitGrpLib(void)
{
	if (GRP_scanlineoffsets)
	{
		free(GRP_scanlineoffsets);			//freeing multiplication table
		GRP_scanlineoffsets = NULL;
	}
	if (GRP_tableforunitcolor)
	{
		free(GRP_tableforunitcolor);	//free changecolor table
		GRP_tableforunitcolor = NULL;
	}
	MaxIndexLines = 0;
}
//==========================
void *SetVideoBuffer(void *memory)
{
	void *oldvidmem = GRP_vidmem;		//set screen-bufer to 'memory'
	GRP_vidmem = (char *)memory;		//
	return (oldvidmem);
}
//==========================
//initialization grp-library with xsize,ysize of screen
int InitGrpLib(int sizex,int sizey)
{
	int err,i;
	GRP_screensizex = sizex;
	GRP_screensizey = sizey;
	GRP_wmaxdwordwritel = sizex * sizey / 4;
	GRP_recalcscanlines();
	wviewport(0,0,sizex,sizey);
	err = SetPlayerColors(0,1,0,0,NULL);		//just allocate table at least
	//for one line of color indexes
	//(player color)
	if (err < 0)
		return -1;
	else
		return 0;
}
//==============================
//Re-initialization grp-library with new xsize,ysize of screen
int ResetGrpLib(int sizex,int sizey)
{
	return InitGrpLib( sizex, sizey );
}
//==============================
long long FILElength(FILE *descriptor)
{
	long long old,l;
	old = ftell(descriptor);
	fseek(descriptor,0,SEEK_END);
	l = ftell(descriptor);
	fseek(descriptor,old,SEEK_SET);
	return(l);
}
//==============================
int loadgrp(char *filename,GRPFILE **grpmem)  //load grp in memory for blitting
{
	unsigned long long l;
	FILE *f;
	f = fopen(filename,"rb");
	if (!f)				//if file couldn't open
		return(-1);
	l = FILElength(f);
	if (l<=0)
	{
		fclose(f);				//if file length if very big
		return(-2);
	}
	*grpmem = (GRPFILE *)malloc(l);		//try to alloc memory fo grp
	if (*grpmem == NULL)
	{
		fclose(f);
		return (-3);			//no memory
	}
	if (fread((char *) (*grpmem),1,l,f)!=l)//error load grp
	{
		freegrp(*grpmem);                  //free memory allocated with grp
		*grpmem = NULL;
		fclose(f);
		return(-4);
	}
	fclose(f);
	return(0);				//successfull exit
}
//=======================================
void freegrp(GRPFILE *a)                //free memory allocated with grp
{
	if (a)
		free(a);
}
//=======================================
//unpack grp picture nr in memory adress
//(size of buffer need to be (GRP->SizeX+x)*(GRP->SizeY*y)
// x & y need to be 0 & 0 for x&y offsets in mem

void unpackgrp(int x,int y,GRPFILE *GRP,int nrpict,char *mem)
{
	int oldGRP_screensizex = GRP_screensizex;			//change internal variables
	void *oldvidmem = SetVideoBuffer(mem);
	GRP_screensizex = GRP->SizeX+x;
	GRP_recalcscanlines();
	putgrp(x,y,GRP,nrpict,0);			//place grp in this memory
	SetVideoBuffer(oldvidmem);			//return previous bufer
	GRP_screensizex = oldGRP_screensizex;			//and internal var's
	GRP_recalcscanlines();
}
//=========================================================
void SetTranspTable(char *table)
{
	GRP_transpcolors = table;			//set a new transp table
}
//=========================================================
//table have dimension nrplayers*nrofindexes bytes
int SetPlayerColors(int fromplayer,int nrplayers,
					int firstindex,int nrofindexes,
					char *table)
{
	char *tabl2;
	int nrofparsed=0,i,j;
	//    printf("%d %d %d %d\n",fromplayer,nrplayers,firstindex,nrofindexes);
	if (fromplayer + nrplayers >= MaxIndexLines)
	{
		tabl2 = (char *) malloc((MaxIndexLines + ADDMALLOCLINES) * 256);
		if (!tabl2)
			return -1;
		if (GRP_tableforunitcolor)
		{
			memcpy(tabl2,GRP_tableforunitcolor,MaxIndexLines*256);
			free(GRP_tableforunitcolor);
		}
		GRP_tableforunitcolor = tabl2;
		for (i=MaxIndexLines;i<MaxIndexLines+ADDMALLOCLINES;i++)
			for (j=0;j<256;j++)
				GRP_tableforunitcolor[i*256+j] = j;   //fill in every line with all palette indexes
				//for future change ,if need
				MaxIndexLines += ADDMALLOCLINES;
	}
	if (table)
	{
		for (i=fromplayer;i<fromplayer+nrplayers;i++)
		{
			for (j=firstindex;j<firstindex+nrofindexes;j++)		 //elements (8-15) used for gradation of players colors
				GRP_tableforunitcolor[i*256+j] = *table++;
			nrofparsed++;
		}
	}
	return(nrofparsed);
}
//=========================================================
int SetUserPlayerColors(int fromplayer,int nrplayers,char *table)
{
	return(SetPlayerColors(fromplayer,nrplayers,8,8,table));
}
//=========================================================
int GetGRPLibVer(void)
{
	char vv[200];
	int vers=0;
	char *ver=vv;
	char *pospoint;

	strncpy(vv,VERSION,199);
	pospoint = strstr(ver,".");
	*pospoint = 0;
	vers+=(atoi(ver)<<16);
	ver = pospoint+1;
	pospoint = strstr(ver,".");
	*pospoint = 0;
	vers+=(atoi(ver)<<8);
	ver = pospoint+1;
	vers+=atoi(ver);
	return(vers);
}
//=========================================================
