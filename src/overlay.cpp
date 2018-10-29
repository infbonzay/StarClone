#include <string.h>
#include <stdlib.h>

#include <grplib/grp.h>
#include <grplib/usegrp.h>
#include "man.h"
#include "auxil.h"
#include "vars.h"
#include "debug.h"
#include "load.h"
#include "loadlo.h"
#include "gr.h"
#include "tbl.h"
#include "lists.h"
#include "market.h"
#include "putobj.h"
#include "maps.h"
#include "mage.h"
#include "overlay.h"

//=====================================================
void Images_FUNC(OVERLAY_IMG *img,int func,int remap,char *grptable,char *grpmethod)
{
	switch(func)
	{
		case DRAWFUNC_NORMAL:	// 0 -Normal Draw
		case DRAWFUNC_HIDDENTRAP://1 - Unknown1 show hidden(cloaked) trap - trap showed as normal picture not cloaked
			*grptable=NONETABLE;
			*grpmethod=NORMAL;
			break;
		case DRAWFUNC_ENEMYCLOAK://2 - Enemy Unit Cloak(+spell)
			*grptable=NONETABLE;
			*grpmethod=DISTORTION;
			break;
		case DRAWFUNC_OWNCLOAK1://3 - Own Unit Cloak(+spell)
		case DRAWFUNC_ALLYCLOAK://4 - Ally Unit Cloak
		case DRAWFUNC_OWNCLOAK2://5 - Own Unit Cloak(+spell)
		case DRAWFUNC_OWNCLOAK3://6 - Own Unit Cloak(draw only)
			*grptable=NONETABLE;
			*grpmethod=TRANSPARENT;
			break;
		case DRAWFUNC_UNKNOWN1:// 7 - unknown
			break;
		case DRAWFUNC_EMPSHOCKWAVE:// 8 - EMP Shockwave
			*grptable=NONETABLE;
			*grpmethod=DISTORTION;
			img->parentimg->invisiblecolors = MAXINVISIBLECOLOR;
			break;
		case DRAWFUNC_REMAPPING://9 - Use Remapping
			*grpmethod=GLOW;
			switch(remap)
			{
				case 0://No remapping
					*grptable=NONETABLE;
					break;
				case 1://ofire.pcx (Orange)
					*grptable=REDTABLE;
					break;
				case 2://gfire.pcx (Green)
					*grptable=GREENTABLE;
					break;
				case 3://bfire.pcx (Blue)
					*grptable=BLUETABLE;
					break;
				case 4://bexpl.pcx (Blue2)
					*grptable=WHITETABLE;
					break;
				case 5://Special (Own cloak)
					*grptable=NONETABLE;
					*grpmethod=TRANSPARENT;
					break;
				case 6://(crash)
				case 7://(crash)
				case 8://Unk8 (?)
				case 9://Unk9 (?)
					break;
			}
			break;
		case DRAWFUNC_SHADOW:	//10 - Shadow/FoW(dark.pcx)
			*grptable=BLACKTABLE;
			*grpmethod=SHADOW;
			break;
		case DRAWFUNC_UNKNOWN2: //11 - Unknown
			break;
		case DRAWFUNC_WARP1:	//12 - Warp Flash(SEdit crash)
			break;
		case DRAWFUNC_UNKNOWN3://Don't Mirror Frames
			break;
		case DRAWFUNC_UNKNOWN4://Unknown14
			break;
		case DRAWFUNC_UNKNOWN5://Hide Gfx,Show Size Rect
			break;
		case DRAWFUNC_HALLUCINATION://16 - //Hallucination
			*grptable=TOTABLE;
			*grpmethod=TOBLUE;
			break;
		case DRAWFUNC_WARP2://17 - Warp Flash
			*grptable = WHITETABLE;
			*grpmethod = WARPFROMWHITE;
			break;
	}
}
//=====================================================
int DamageFactor(int number,int maxnumber)
{
	if (number==maxnumber)
		return 0;
	for (int i=6;i>0;i--)
		if (number>=(maxnumber*i/7))
			return 6-i;
	return 6;
}
//========================================
//0-9
int GetCorrosiveAcidValue(struct OBJ *a)
{
	short acidval = GetMageAtr(&a->atrobj,ATRCORROSIVEACID);
	if (!acidval)
		return(0);
	return(acidval/CORROSIVEACIDDIVFACTOR+1);
}
//=====================================================


