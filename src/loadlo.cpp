
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mpq.h"
#include "man.h"
#include "loadlo.h"

//===============================================
LOFILE *loadlo(char *filename)
{
    LOFILE *temp=NULL;
//    GetLoadedImages(images_tbl,(void **)&temp);
    mpqloadfile(filename,(char **)&temp);
    return (temp);
}
//===============================================
void unloadlo(LOFILE *temp)
{
    if (temp)
	wfree(temp);
}
//===============================================
void GetLoXY(LOFILE *temp,int nr_side,int nr_pos,signed char *xdelta,signed char *ydelta)
{
	signed char *adr;
        if (nr_side>=temp->CountSides)
	{
	    nr_side = 0;
	}
	else
	{
	    if (nr_pos>=temp->CountPictsPerSides)
		nr_pos = 0;
	}
	adr = ((signed char*)( (long)(temp) + (temp->offseteachline[nr_side]))+nr_pos*2);
	*xdelta = adr[0];
	*ydelta = adr[1];
}
//===============================================
signed char *GetLoXY(LOFILE *temp,int nr_side,int nr_pos)
{
    if (!temp)
	return NULL;
    else
    {
        if (nr_side>=temp->CountSides)
	{
	    nr_side = 0;
	}
	else
	{
	    if (nr_pos>=temp->CountPictsPerSides)
		nr_pos = 0;
	}
	return ((signed char*)( (long)(temp) + (temp->offseteachline[nr_side]))+nr_pos*2);
    }
}
//===============================================
void SetLoXY(LOFILE *temp,int nr_side,int count,int deltax,int deltay)
{
    signed char *temp2;
    if (!temp)
	return;
    else
        temp2 = (signed char*)( (long)(temp) + (temp->offseteachline[nr_side]));
    for (int i=0;i<count;i++)
    {
	*temp2++=deltax;
	*temp2++=deltay;
    }
}
//===============================================
LOFILE *createlo(int count,int deltax,int deltay)
{
    LOFILE *temp;
    temp = (LOFILE *) wmalloc(4+4+count*4+count*2);
    memset(temp,0,4+4+count*4+count*2);
    temp->CountSides = count;
    temp->CountPictsPerSides = 1;
    for (int i=0;i<count;i++)
    {
	temp->offseteachline[i] = (4+4+count*4+i*2);
	SetLoXY(temp,i,1,deltax,deltay);
    }
    return (temp);
}
//===============================================
void GetXYFromLO(OBJstruct *b,int mode,int line,int *x,int *y)
{
/*    int xoffs,xo,yoffs;
    signed char *adrxyoffs;
    adrxyoffs = GetLoXY(b->allmove[mode][line].loinf.lo,
				b->allmove[mode][line].loinf.fromloline,
				b->allmove[mode][line].loinf.fromlooffset);
    if (adrxyoffs)
    {
	xoffs = adrxyoffs[0];
	yoffs = adrxyoffs[1];
    }
    else
    {
        xoffs = 0;
        yoffs = 0;    
    }
    xo=xoffs;
    if (b->allmove[mode][line].loinf.status&LOFIRSTINV)
    {
        xo = -xo;
    }
    if (!(b->allmove[mode][line].loinf.status&LONOINV))
    {
	xo = -xo;
    }
    *x=xo;
    *y=yoffs;
*/
}
//===============================================
