
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
int loadStars(const char *starfn)
{
	return(mpqloadfile(starfn,(char **)&spacestars));
}
//===========================================================
void unloadStars(void)
{
	if (spacestars)
	{
		wfree(spacestars);
		spacestars = NULL;
	}
}
//===========================================================
void showStars(int screenx,int screeny)
{
	short xpos,ypos;
	int i,j,k,offset,xsize,ysize,x,y,xkart,ykart,offsetpixel;
	if (map.terrain != TERRAIN_SPACEPLATFORM)
		return;
	int starnr=0;
	short int layers = spacestars->numLayers;
	short int numberofstars;
	if (layers > MAXSTARLAYERS)
		layers = MAXSTARLAYERS;
	for (k=0 ; k < layers ; k++)
	{
		numberofstars = spacestars->numStarCount[k];
		while(numberofstars--)
		{
			star = (starRef *)((char *)spacestars + sizeof(spacestars->numLayers) + sizeof(short)*layers + sizeof(starRef)*starnr);
			xpos = star->X-screenx / (k+1) / 4;
			if (xpos < 0)
				xpos = gameconf.grmode.x + (xpos % gameconf.grmode.x);
			ypos=star->Y - screeny / (k+1) / 4;
			if (ypos < 0)
				ypos = gameconf.grmode.y + (ypos%gameconf.grmode.y);
			xpos %= gameconf.grmode.x;
			ypos %= gameconf.grmode.y;
			if (ypos <= gameconf.grmode.y-80)
			{
				xkart = (xpos+screenx)/SIZESPRLANSHX;
				ykart = (ypos+screeny)/SIZESPRLANSHY;
				if (CHECKFORMAPBORDERS(xkart,ykart))
					continue;
				if (gameconf.videoconf.visiblemap || mapSEE(xkart,ykart)>1)
				{
					offset = star->image;
					img = (starBmap *)((char *)spacestars + offset);
					xsize = img->Width;
					ysize = img->Height;
					offsetpixel = 0;
					for (i=0;i<ysize;i++)
					{
						y = ypos + i;
						for (j=0;j<xsize;j++)
						{
							x = xpos + j;
							if (!gameconf.grmode.videobuff[GRP_scanlineoffsets[y]+x])
								gameconf.grmode.videobuff[GRP_scanlineoffsets[y]+x] = img->Data[offsetpixel];
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
