
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "vars.h"
#include "vision.h"
#include "load.h"

MAPVISIONOFFSETS *mapvisiontables[4];

/*unsigned char MAPvision[4][MAXVISY][MAXVISX];
//=============================================
void setvisiontable(int table,int addx,int addy)
{
	int xpos,ypos,i,j,k;
	double range;
	for (k=1;k<=ALLVIS;k++)
	for (i=0;i<MAXVISY;i++)
	{
		for (j=0;j<MAXVISX;j++)
		if (MAPvision[table][i][j]==0xff)
		{
			ypos=i-MIDY;
			xpos=j-MIDX;
			range=hypot(xpos,ypos)+0.5;
			if (range<=k)
				MAPvision[table][i][j]=(int)range;
		}
	}
	if (addx)
	{
		for (j=MAXVISX-1;j>MIDX;j--)
			for (i=0;i<MAXVISY;i++)
			{
				MAPvision[table][i][j]=MAPvision[table][i][j-1];
			}
	}
	if (addy)
	{
		for (i=MAXVISY-1;i>MIDY;i--)
			for (j=0;j<MAXVISX;j++)
			{
				MAPvision[table][i][j]=MAPvision[table][i-1][j];
			}
	}
}
*/
//=============================================
void SetVisionTables(void)
{
	/*	  memset(MAPvision,0xff,sizeof(MAPvision));
		setvisiontable(0,0,0);
		setvisiontable(1,1,0);
		setvisiontable(2,0,1);
		setvisiontable(3,1,1);
	*/
	LoadVisionBinTables();
}
//=============================================
#define MAPVISIONTABLES0_DAT	"map\\vision0.dat"
#define MAPVISIONTABLES1_DAT	"map\\vision1.dat"
#define MAPVISIONTABLES2_DAT	"map\\vision2.dat"
#define MAPVISIONTABLES3_DAT	"map\\vision3.dat"
void LoadVisionBinTables(void)
{
	mpqloadfile(MAPVISIONTABLES0_DAT, (char **)&mapvisiontables[0]);
	mpqloadfile(MAPVISIONTABLES1_DAT, (char **)&mapvisiontables[1]);
	mpqloadfile(MAPVISIONTABLES2_DAT, (char **)&mapvisiontables[2]);
	mpqloadfile(MAPVISIONTABLES3_DAT, (char **)&mapvisiontables[3]);
}
//============================================
void UnloadVisionBinTables(void)
{
	unloadfilefrommpq(mapvisiontables[0]);
	unloadfilefrommpq(mapvisiontables[1]);
	unloadfilefrommpq(mapvisiontables[2]);
	unloadfilefrommpq(mapvisiontables[3]);
}
//============================================
