/*
 *    grp-lib v. 0.4.21
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
int GRP_viewport(int x,int y,int sizex,int sizey)
{
	if (x >= 0 && y >= 0 && x < GRP_screensizex && y < GRP_screensizey)
	{
		GRP_wminx = x;			//set clipzone variables
		GRP_wminy = y;
		GRP_wmaxx = x + sizex - 1;
		GRP_wmaxy = y + sizey - 1;
		GRP_wsizex = sizex;
		GRP_wsizey = sizey;
		return 0;
	}
	else
		return 1;
}
//===========================================
void GRP_QuitLib(void)
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
void *GRP_SetVideoBuffer(void *memory)
{
	void *oldvidmem = GRP_vidmem;		//set screen-bufer to 'memory'
	GRP_vidmem = (char *)memory;		//
	return (oldvidmem);
}
//==========================
//initialization grp-library with xsize,ysize of screen
int GRP_InitLib(int sizex,int sizey)
{
	int err,i;
	GRP_screensizex = sizex;
	GRP_screensizey = sizey;
	GRP_wmaxdwordwritel = sizex * sizey / 4;
	GRP_recalcscanlines();
	GRP_viewport(0,0,sizex,sizey);
	err = GRP_SetPlayerColors(0,1,0,0,NULL);		//just allocate table at least
	//for one line of color indexes
	//(player color)
	if (err < 0)
		return -1;
	else
		return 0;
}
//==============================
//Re-initialization grp-library with new xsize,ysize of screen
int GRP_ResetLib(int sizex,int sizey)
{
	return GRP_InitLib( sizex, sizey );
}
//==============================
static long long FILElength(FILE *descriptor)
{
	long long old, filePos;
	old = ftell(descriptor);
	fseek(descriptor,0,SEEK_END);
	filePos = ftell(descriptor);
	fseek(descriptor,old,SEEK_SET);
	return(filePos);
}
//==============================
int GRP_Load(char *filename,GRPFILE **grpmem)  //load grp in memory for blitting
{
	unsigned long long fileLength;
	FILE *f;
	f = fopen(filename,"rb");
	if (!f)				//if file couldn't open
		return(-1);
	fileLength = FILElength(f);
	if (fileLength <= 0)
	{
		fclose(f);				//if file length if very big
		return(-2);
	}
	*grpmem = (GRPFILE *)malloc(fileLength);		//try to alloc memory fo grp
	if (*grpmem == NULL)
	{
		fclose(f);
		return (-3);			//no memory
	}
	if (fread((char *) (*grpmem), fileLength, 1, f) != 1)//error load grp
	{
		GRP_Free(*grpmem);                  //free memory allocated with grp
		*grpmem = NULL;
		fclose(f);
		return(-4);
	}
	fclose(f);
	return(0);				//successfull exit
}
//=======================================
void GRP_Free(GRPFILE *a)                //free memory allocated with grp
{
	if (a)
		free(a);
}
//=======================================
//unpack grp picture nr in memory adress
//(size of buffer need to be (GRP->SizeX+x)*(GRP->SizeY*y)
// x & y need to be 0 & 0 for x&y offsets in mem

void GRP_Unpack(int x,int y,GRPFILE *GRP,int nrpict,char *mem)
{
	int oldGRP_screensizex = GRP_screensizex;			//change internal variables
	void *oldvidmem = GRP_SetVideoBuffer(mem);
	GRP_screensizex = GRP->SizeX + x;
	GRP_recalcscanlines();
	GRP_Put(x,y,GRP,nrpict,0);			//place grp in this memory
	GRP_SetVideoBuffer(oldvidmem);			//return previous bufer
	GRP_screensizex = oldGRP_screensizex;			//and internal var's
	GRP_recalcscanlines();
}
//=========================================================
void GRP_SetTranspTable(char *table)
{
	GRP_transpcolors = table;			//set a new transp table
}
//=========================================================
//table have dimension nrplayers*nrofindexes bytes
int GRP_SetPlayerColors(int fromplayer,int nrplayers,
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
		for (i = MaxIndexLines;i<MaxIndexLines + ADDMALLOCLINES;i++)
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
int GRP_SetUserPlayerColors(int fromplayer,int nrplayers,char *table)
{
	return(GRP_SetPlayerColors(fromplayer,nrplayers,8,8,table));
}
//=========================================================
