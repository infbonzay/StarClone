
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "maphelp.h"

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
int CalcGradus(int x1,int y1,int x2,int y2)
{
    int deltax,deltay,alfa;
    deltax = x2-x1;
    deltay = y2-y1;
    if (deltax>=0)
    {
	if (!deltay)
    	    alfa=90;
        else
	    if (deltay<0)//I
            {
    		alfa=(int) ((float)(atan((float)deltax/-deltay)*180/PI));
            }
            else	//IV
            {
        	alfa=180-(int) ((float)(atan((float)deltax/deltay)*180/PI));
            }
    }
    else
    {
	if (!deltay)
    	    alfa=270;
	else
    	    if (deltay<0)//II
            {
        	alfa=360-(int )((float)(atan((float)deltax/deltay)*180/PI));
            }
            else	//III
            {
        	alfa=180+(int) ((float)(atan((float)-deltax/deltay)*180/PI));
            }
    }
    return(alfa);
}
//=======================================
#define MAXANGLES		16
#define MAXOFFSETELEMENTS	(MAXVISX*MAXVISY)
//=======================================
struct MAPOFFSETELEMENT
{
    unsigned char	rangevision;		//1 - (ALLVIS-1)
    signed char		xoffset;		//-MAXXVIS .. +MAXXVIS
    signed char		yoffset;		//-MAXYVIS .. +MAXYVIS
};
struct MAPVISIONOFFSETS
{
    unsigned int offsets[MAXANGLES];
    MAPOFFSETELEMENT mapelement[MAXOFFSETELEMENTS];
}offs;
//=======================================
int currentoffset;
void arrayset(FILE *f,int elem,int k,int xpos,int ypos)
{
    fprintf(f," ,%d,%d,%d  ",k,xpos,ypos);
    offs.mapelement[elem].rangevision = k;
    offs.mapelement[elem].xoffset = xpos;
    offs.mapelement[elem].yoffset = ypos;
    currentoffset += sizeof(MAPOFFSETELEMENT);
}
//=======================================
void CreateMapOffsets(void)
{
    int g,k,i,j,xpos,ypos,mapgrad,err,curelem;
    FILE *f = fopen("offset0.txt","wb");
    currentoffset = (int)&offs.mapelement[0] - (int)&offs;
    curelem=0;
    for (g = 0; g<MAXANGLES; g++)
    {
	offs.offsets[g] = currentoffset;
	printf("current offset %d=0x%x\n",g,currentoffset);
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
		    if (mapgrad >= g*360/MAXANGLES && mapgrad < (g+1)*360/MAXANGLES)
		    {
			if ( k == MAPvision[0][i][j])
			{
			    MAPvision[0][i][j]=255;
			    arrayset(f,curelem++,k,xpos,ypos);
			    err++;
			}
		    }
		}
	    }
	    if (err)
		fprintf(f,"\n");
	}
	arrayset(f,curelem++,0,0,0);
        fprintf(f,"\n\n");
    }
    fclose(f);
}
//=======================================
void ShowRemainMapVision(void)
{
    int i,j;
    FILE *f=fopen("remmapvis.txt","wb");
    for (i=0;i<MAXVISY;i++)
    {
        for (j=0;j<MAXVISX;j++)
        {
	    fprintf(f,"%03d ",MAPvision[0][i][j]);
	}
	fprintf(f,"\n");
    }
    fclose(f);
}
//=======================================
void SaveMapOffsets(void)
{
    FILE *f=fopen("vision0.dat","w");
    fwrite(&offs,currentoffset,1,f);
    fclose(f);
}
//=======================================
int main(void)
{
    SetVisionTables();
    ShowRemainMapVision();
    SaveMapOffsets();
}
