
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "vars.h"
#include "auxil.h"
#include "vision.h"

void CreateMapOffsets(void);
unsigned char MAPvision[4][MAXVISY][MAXVISX];
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
//=============================================
void SetVisionTables(void)
{
    memset(MAPvision,0xff,sizeof(MAPvision));
    setvisiontable(0,0,0);
    setvisiontable(1,1,0);
    setvisiontable(2,0,1);
    setvisiontable(3,1,1);
    CreateMapOffsets();
}
//=============================================
//0xXXYY	XX - 01-14 vision range, YY - delta from center(0,0)
//signed char MAPVisionOffsets[16][3];
//=============================================
void CreateMapOffsets(void)
{
    int g,k,i,j,xpos,ypos,mapgrad,err;
    FILE *f = fopen("Mapoffset.txt","wb");
    for (g=0;g<16;g++)
    {
	for (k=1;k<ALLVIS;k++)
	{
	    err=0;
	    for (i=0;i<MAXVISY;i++)
	    {
    		ypos = i - MIDY;
    		for (j=0;j<MAXVISX;j++)
    		{
		    xpos = j - MIDX;
		    mapgrad = CalcGradus(0,0,xpos*10,ypos*10);
		    if (mapgrad >= g*360/16 && mapgrad < (g+1)*360/16)
		    {
			if ( k == MAPvision[0][i][j])
			{
			    err++;
			    fprintf(f," ,%d,%d,%d  ",k,xpos,ypos);
			}
		    }
		}
	    }
	    if (err)
		fprintf(f,"\n");
//	    fprintf(f,"0,0,0\n");
	}
        fprintf(f,"0,0,0\n\n");
    }
    fclose(f);
}
