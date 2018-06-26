
#include <grplib/grp.h>
#include "wmem.h"

#include "starmap.h"
#include "maps.h"
#include "gr.h"
#include "mpq.h"
#include "vars.h"
#include "stars.h"

#define MAXSTARLAYERS	5

spkFile *spacestars;
starRef *star;
starBmap *img;

//===========================================================
int loadStars(char *starfn)
{
    return(mpqloadfile(starfn,(char **)&spacestars));
}
//===========================================================
void unloadStars(void)
{
    if (spacestars)
    {
	wfree(spacestars);
	spacestars=NULL;
    }
}
//===========================================================
void showStars(int screenx,int screeny)
{
    short xpos,ypos;
    int i,j,k,offset,xsize,ysize,x,y,xkart,ykart,offsetpixel;
    if (map.terrain!=TERRAIN_SPACEPLATFORM)
	return;
    int starnr=0;
    short int layers=spacestars->numLayers;
    short int numberofstars;
    if (layers>MAXSTARLAYERS)
	layers=MAXSTARLAYERS;
    for (k=0;k<layers;k++)
    {
	numberofstars=spacestars->numStarCount[k];
	while(numberofstars--)
	{
	    star=(starRef *)((char *)spacestars+sizeof(spacestars->numLayers)+sizeof(short)*layers+sizeof(starRef)*starnr);
	    xpos=star->X-screenx/(k+1)/4;
	    if (xpos<0)
		xpos=GRP_screensizex+(xpos%GRP_screensizex);
	    ypos=star->Y-screeny/(k+1)/4;
	    if (ypos<0)
		ypos=GRP_screensizey+(ypos%GRP_screensizey);
	    xpos%=GRP_screensizex;
	    ypos%=GRP_screensizey;
	    if (ypos<=GRP_screensizey-80)
	    {
		xkart=(xpos+screenx)/SIZESPRLANSHX;
		ykart=(ypos+screeny)/SIZESPRLANSHY;
		if (gameconf.videoconf.visiblemap || mapSEE(xkart,ykart)>1)
		{
		    offset=star->image;
		    img=(starBmap *)((char *)spacestars+offset);
		    xsize=img->Width;
		    ysize=img->Height;
		    offsetpixel=0;
		    for (i=0;i<ysize;i++)
		    {
			y=ypos+i;
			for (j=0;j<xsize;j++)
			{
			    x=xpos+j;
//			    if (!GRP_vidmem[GRP_scanlineoffsets[y]+x])
//				GRP_vidmem[GRP_scanlineoffsets[y]+x]=img->Data[i*xsize+j];
			    if (!GRP_vidmem[GRP_scanlineoffsets[y]+x])
				GRP_vidmem[GRP_scanlineoffsets[y]+x]=img->Data[offsetpixel];
			    offsetpixel++;
			}
		    }
		}
	    }
	    starnr++;
	}
    }
}
//===========================================================
