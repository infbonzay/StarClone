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
#define RECHARGESHIELDOVERLAYS	3
short rechargeshield_id[RECHARGESHIELDOVERLAYS]={370,369,368};
GRPFILE *rechargeshield_grp[RECHARGESHIELDOVERLAYS];
//=====================================================
//=====================================================
void UnloadAllOverlaysGRP(void)
{
    int i,j;
	
    for (i=0;i<RECHARGESHIELDOVERLAYS;i++)
	if (rechargeshield_grp[i])
	    rechargeshield_grp[i]=NULL;
}
//=====================================================
void Images_FUNC(OVERLAY_IMG *img,int func,int remap,char *grptable,char *grpmethod)
{
    switch(func)
    {
	case DRAWFUNC_NORMAL:	// 0 -Normal Draw
	case DRAWFUNC_HIDDENTRAP://1 - Unknown1	show hidden(cloaked) trap - trap showed as normal picture not cloaked
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
void LoadOverlaysGRP(void)
{
    int i,j;
    char func,remap;
    short images_tbl;
    for (i=0;i<RECHARGESHIELDOVERLAYS;i++)
    {
    	images_tbl=alldattbl.images_dat->images_tbl[rechargeshield_id[i]];
    	if (GetLoadedImage(images_tbl,(void **)&rechargeshield_grp[i])<0)
    	{
    	    printf("RECHARGEOVERLAY:can't preload rechanrge shield grp's\n");
    	    exit(-1);
	}
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
void ShowRechargeShield(struct OBJ *a)
{
/*    GRPFILE *grp;
    int x,y;
    int effectlevel;
    if (!a)
	return;
    if (a->shieldrechargeenabled==0)
    {
	if (IsOnSkyOBJ(a))
	    effectlevel=AFTERAIROBJ;
	else
	    effectlevel=AFTERGROUNDOBJ;
	a->shieldrechargeenabled=1;
	grp=rechargeshield_grp[GetUnitOverlaySize(a->SC_Unit)];
	gameeffects[effectlevel].AddEffect(a->sourcex,a->sourcey,grp,-1,0,1,grp->CountPictures,1,NORMAL,NONETABLE,0);
	gameeffects[effectlevel].AddEndEffectCallback(&a->shieldrechargeenabled);
    }
*/
}
//=====================================================
int bunkeroverlaylo[32]={0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4, 4,4,5,5,5,5,6,6,6,6,7,7,7,7,0};
//=====================================================
void SetBunkerShowFire(struct OBJ *marine,struct OBJ *a)
{
/*    int inv=0,side=0,side2=0;
    LOINF *loinf;
    OBJstruct *b=loadobj(a->SC_Unit);
    if (marine)
    {
	side = side2 = marine->storonasveta;
	if (side >= SPRITE_SIDES)
	{
    	    side = MAXFACEDIRECTIONS - side;
    	    inv = EFF_HORIZMIRROR;
	}
    }
    gameeffects[AFTERGROUNDOBJ].AddEffect(  a->sourcex,a->sourcey,
					    b->allmove[TYPEMODEATACKDOWN]->grp,
					    -1,
					    side,
					    32768,
					    b->animsteps[TYPEMODEATACKDOWN],
					    b->animtimes[TYPEMODEATACKDOWN],
					    NORMAL,
					    NONETABLE,
					    inv);
    loinf = &b->allmove[TYPEMODEATACKDOWN]->loinf;
    gameeffects[AFTERGROUNDOBJ].AddEffectLO(loinf->lo,loinf->fromloline,bunkeroverlaylo[side2],0);
*/
}
//=====================================================
//0-9
int GetCorrosiveAcidValue(struct OBJ *a)
{
    short acidval = GetMageAtr(&a->atrobj,ATRCORROSIVEACID);
    if (!acidval)
	return(0);
    return(acidval/CORROSIVEACIDDIVFACTOR+1);
}
//=====================================================


