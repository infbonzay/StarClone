
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "auxil.h"
#include "debug.h"
#include "gr.h"
#include "load.h"
#include "lists.h"
#include "maps.h"
#include "overlay.h"
#include "iscript.h"
#include "maps.h"
#include "objinfo.h"
#include "objs.h"
#include "market.h"
#include "mouse.h"
#include "mage.h"
#include "rand.h"
#include "sigorder.h"
#include "doodad.h"
#include "flingy.h"
#include "images.h"

#define MAXIMAGELEVELS	20
MAINIMGLIST imageslist_minimap(100);
MAINIMGLIST imageslist_doodads(1000);
MAINIMGLIST mainimageslist(2000);
mylist elevationIMG[MAXIMAGELEVELS];
//============================================
void OVERLAYIMGLIST::DeleteMarked(void)
{
    int i,lastelem;
    if (!totalmarked)
	return;
    totalmarked=0;
    lastelem = totalelem-1;
    for (i=lastelem;i>=0;i--)
    {
	if (deletemarked[i])
	{
	    delete ( (OVERLAY_IMG *)elements[i]);
	    if (i != lastelem)
	    {
		elements[i] = elements[lastelem];
		deletemarked[i] = deletemarked[lastelem];;
	    }
	    else
		deletemarked[i] = 0;
	    totalelem--;
	    lastelem--;
	}
    }
}
//=========================================
void MAINIMGLIST::DeleteMarked(void)
{
    MAIN_IMG *img;
    int i,lastelem;
//    if (!totalmarked)
//	return;
    lastelem = totalelem-1;
    for (i=lastelem;i>=0;i--)
    {
	img = (MAIN_IMG *)elements[i];
	if (img->childlists)
	{
	    img->childlists->DeleteMarked();
	    if (!img->childlists->totalelem)
	    {
	        delete img->childlists;
	        img->childlists = NULL;
	    }
	}
	if (deletemarked[i])
	{
	    if (!img->childlists)
	    {
		delete ((MAIN_IMG *)elements[i]);
		if (i != lastelem)
		{
	    	    elements[i] = elements[lastelem];			//last imaget move to deleted image
	    	    deletemarked[i] = deletemarked[lastelem];
	    	    ((MAIN_IMG *)(elements[i]))->imglist_elemnr = i;	//global nr of moved image also changes 
		}
		else
		    deletemarked[i] = 0;
		totalelem--;
		lastelem--;
		totalmarked--;
	    }
	    else
	    {
//		printf("not deleted because of childs\n");
	    }
	}
    }
}
//=========================================
void MAINIMGLIST::FreeAndEmptyAll(void)
{
    int i;
    MAIN_IMG *img;
    for (i=0;i<totalelem;i++)
    {
	img = (MAIN_IMG *) elements[i];
	img->DeleteMainImgAndChilds(this);
    }
    DeleteMarked();

    totalelem=0;
    totalmarked=0;
    memset(elements,0,allocatedelem*sizeof(void *));
    memset(deletemarked,0,allocatedelem*sizeof(char));
    EnumListInit();
}
//=========================================
int CheckIscriptNr(OVERLAY_IMG *img,int scriptnr)					//check if we have function iscriptnr for scriptid 
{
    return(iscriptinfo.iscriptsid[img->iscriptid].cmdbufoffs[scriptnr]);
}
//=========================================
int OVERLAY_IMG::SetIScriptNr(int scriptnr)					//set the script function and address of function script code
{
    short int cmdoffs;
    if (iscriptnr == scriptnr && offsetcmdinbuf)
	return(-1);
    if (iscriptnr == ISCRIPTNR_DEATH)
	return(-2);
    if (flags & SC_IMAGE_FLAG_INDEPENDENTOFMAINIMG && scriptnr != ISCRIPTNR_INIT)
	return(-3);

    cmdoffs = iscriptinfo.iscriptsid[iscriptid].cmdbufoffs[scriptnr];
    //if have command for this iscriptnr
    if (cmdoffs)
    {
	offsetcmdinbuf = cmdoffs;						//address
	iscriptnr = scriptnr;							//
        waitticks = 0;								//reset wait parameter
        return(0);
    }
    return(-4);
}
//============================================
void MAIN_IMG::ForceSetIScript(int scriptnr)
{
    OVERLAY_IMG *tempimg;
    if (scriptnr != ISCRIPTNR_DEATH)
	if (flags & SC_IMAGE_FLAG_CANTBREAKCODE)
    	    return;
    if (childlists)
    {
	childlists->EnumListInit();
	while(tempimg = (OVERLAY_IMG *)childlists->GetNextListElem(NULL))
	{
	    tempimg->iscriptnr = scriptnr;
	}
    }
    iscriptnr = scriptnr;
}
//============================================
int MAIN_IMG::SetIScriptNrAll(int scriptnr,int executeflag)
{
    int status;
    OVERLAY_IMG *tempimg;
    if (scriptnr != ISCRIPTNR_DEATH)
	if (flags & SC_IMAGE_FLAG_CANTBREAKCODE)
    	    return(-2);
    if (childlists)
    {
	childlists->EnumListInit();
	while(tempimg = (OVERLAY_IMG *)childlists->GetNextListElem(NULL))
	{
	    if (tempimg->SetIScriptNr(scriptnr)==0 && executeflag == ISCRIPTNR_EXECUTE)
		iscriptinfo.ExecuteScript(tempimg);
	}
    }
    status = SetIScriptNr(scriptnr);
    if (status == 0 && executeflag == ISCRIPTNR_EXECUTE)
	iscriptinfo.ExecuteScript(this);
    return(status);
}
//============================================
int SetOBJIScriptNr(OBJ *a,int scriptnr,int executeflag)			//set the same script to all obj images and subimages
{
    return(a->mainimage->SetIScriptNrAll(scriptnr,executeflag));
}
//============================================
void OVERLAY_IMG::ForceChangeImage(unsigned short image_id)
{
    offsetcmdinbuf = 0;
    SetGrpFunc(image_id);
//    SetIScriptNr(iscriptnr);
    SetIScriptNr(ISCRIPTNR_INIT);
}
//============================================
void OVERLAY_IMG::SetGrpFunc(unsigned short image_id)
{
    imageid = image_id;
    iscriptid = alldattbl.images_dat->Iscript_ID[image_id];
    if (GetLoadedImage(alldattbl.images_dat->images_tbl[image_id],(void **)&grp) < 0)
	DEBUGMESSCR("grp.cant load images_tbl=%d\n",alldattbl.images_dat->images_tbl[image_id]);
    Images_FUNC(this,alldattbl.images_dat->Draw_Function[image_id],alldattbl.images_dat->Remapping[image_id],&remaptable,&grpmethod);
//    newgrpmethod = NORMAL;
}
//============================================
#define MAXOVERCHILDS		10
void MAIN_IMG::AddImageToParent(OVERLAY_IMG *img)
{
    mylistsimple *childs;
    if (!childlists)
    {
        childlists = new OVERLAYIMGLIST(MAXOVERCHILDS);
//        childlists = new mylistsimple(MAXOVERCHILDS);
    }
    childlists->AddElem(img);
    if (!(img->flags & SC_IMAGE_FLAG_IGNORPARENTSAVEINFOGTABLE))
        if (flags & SC_IMAGE_FLAG_SAVEINFOGTABLE)
	    img->flags |= SC_IMAGE_FLAG_SAVEINFOGTABLE;
}
//============================================
OVERLAY_IMG::OVERLAY_IMG(MAIN_IMG *parent,unsigned short image_id,signed char xoffs,signed char yoffs,
			 unsigned char elevation,unsigned short unitflags,unsigned char usescriptnr)
{
    offsetcmdinbuf = 0;
    iscriptnr = ISCRIPTNR_UNKNOWN;
    returnaddress = 0;
    parentimg = parent;
    elevationlevel = elevation;

    SetGrpFunc(image_id);
    lo = NULL;
    framenr = 0;
    waitticks = 0;
    xdelta = xoffs;
    ydelta = yoffs;
    flags = unitflags;
    if (parent != this)
	parent->AddImageToParent(this);
    if (usescriptnr != ISCRIPTNR_UNKNOWN)
	SetIScriptNr(usescriptnr);
//    else
//	printf("iscriptnr1=255\n");
}
//============================================
MAIN_IMG::MAIN_IMG( unsigned short image_id,int x,int y,int elevation,
		    signed char xoffs,signed char yoffs,
		    unsigned char unitside256,unsigned char plcolor,unsigned short unitflags,
		    unsigned char usescriptnr) : OVERLAY_IMG(this,image_id,xoffs,yoffs,elevation,unitflags,usescriptnr)
{

    childlists = NULL;

    invisiblecolors = MININVISIBLECOLOR;
    side = unitside256;
    neededside = unitside256;
    imageusercolor = plcolor;
    xpos = x;
    ypos = y;
    whocreate = SC_IMAGE_UNKNOWN_CREATOR;
    newgrpmethod = NORMAL;
}
//============================================
OVERLAY_IMG::OVERLAY_IMG(MAIN_IMG *parent,unsigned short image_id,unsigned short imagelo_tbl,
			 unsigned char elevation,unsigned short unitflags,unsigned char usescriptnr)
{
    offsetcmdinbuf = 0;
    iscriptnr = ISCRIPTNR_UNKNOWN;
    returnaddress = 0;
    parentimg = parent;
    elevationlevel = elevation;

    SetGrpFunc(image_id);
    lo = NULL;
    framenr = 0;
    waitticks = 0;
    xdelta = 0;
    ydelta = 0;
    flags = unitflags;
    if (parent != this)
	parent->AddImageToParent(this);
    if (imagelo_tbl)
	if (GetLoadedImage(imagelo_tbl,(void **)&lo) < 0)
	    DEBUGMESSCR("lo.cant load images_tbl=%d\n",imagelo_tbl);
    if (usescriptnr != ISCRIPTNR_UNKNOWN)
	SetIScriptNr(usescriptnr);
//    else
//	printf("iscriptnr2=255\n");
}
//============================================
MAIN_IMG::MAIN_IMG( unsigned short image_id,int x,int y,int elevation,
		    unsigned short imagelo_tbl,
		    unsigned char unitside256,unsigned char plcolor,unsigned short unitflags,
		    unsigned char usescriptnr) : OVERLAY_IMG(this,image_id,imagelo_tbl,elevation,unitflags,usescriptnr)
{

    childlists = NULL;
    invisiblecolors = MININVISIBLECOLOR;
    side = unitside256;
    neededside = unitside256;
    imageusercolor = plcolor;
    xpos = x;
    ypos = y;
//    flags = unitflags;
    whocreate = SC_IMAGE_UNKNOWN_CREATOR;
    newgrpmethod = NORMAL;
}
//============================================
OVERLAY_IMG::OVERLAY_IMG(MAIN_IMG *parent,GRPFILE *grpimg,unsigned short unitflags,unsigned short useiscriptid,
		    unsigned char usescriptnr,unsigned char method,unsigned char remap)
{
    offsetcmdinbuf = 0;
    iscriptnr = ISCRIPTNR_UNKNOWN;
    returnaddress = 0;
    parentimg = parent;
    elevationlevel = 255;

    lo = NULL;

    imageid = 0;
    iscriptid = useiscriptid;

    grp = grpimg;
    grpmethod = method;
    remaptable = remap;
    framenr = 0;
    waitticks = 0;
    xdelta = 0;
    ydelta = 0;
    flags = unitflags;
    if (parent != this)
	parent->AddImageToParent(this);
    if (usescriptnr != ISCRIPTNR_UNKNOWN)
	SetIScriptNr(usescriptnr);
//    else
//	printf("iscriptnr3=255\n");
}
//============================================
MAIN_IMG::MAIN_IMG( GRPFILE *grpimg, int x,int y,
		    unsigned char plcolor,unsigned short unitflags,unsigned short useiscriptid,unsigned char usescriptnr,
		    unsigned char method,unsigned char remap) : OVERLAY_IMG(this,grpimg,unitflags,useiscriptid,usescriptnr,
									    method,remap)
{
    childlists = NULL;
    invisiblecolors = MININVISIBLECOLOR;
    side = 0;
    neededside = 0;
    imageusercolor = plcolor;
    xpos = x;
    ypos = y;
    flags = unitflags;
    whocreate = SC_IMAGE_UNKNOWN_CREATOR;
    newgrpmethod = NORMAL;
}
//============================================
MAIN_IMG::~MAIN_IMG()
{
    if (childlists)
    {
        DEBUGMESSCR("destroing main img but childs still exist :( \n",this->imageid);
    }
    if (parentimg != this)
    {
        DEBUGMESSCR("main img with parent ??? :( \n",this->imageid);
    }
    switch(parentimg->whocreate)
    {
	case SC_IMAGE_FLINGY_CREATOR:
	    parentimg->creator.flingycreator.flingy->DelFlingy();
	    break;
    }	    
//	printf("img creator was=%d\n",parentimg->whocreate);
}
//============================================
void MAIN_IMG::DrawOnScreenImage(void)
{
    totalimgs++;
    drawedimgs++;
    if (flags & SC_IMAGE_FLAG_MIRRORIMAGE)
        iputgrpspr(xpos-grp->SizeX/2,ypos-grp->SizeY/2,grp,TOTABLE,0,0,adrtabls[remaptable]+imageusercolor*8,framenr);
    else
        putgrpspr(xpos-grp->SizeX/2,ypos-grp->SizeY/2,grp,TOTABLE,0,0,adrtabls[remaptable]+imageusercolor*8,framenr);
}
//============================================
void OVERLAY_IMG::DrawImage(void)
{
    DrawImageXY(parentimg->xpos>>8,parentimg->ypos>>8);
}
//============================================
void OVERLAY_IMG::DrawImageXY(int x,int y)
{
    int warpstatus,side,side2,mainside,subunit=0;
    unsigned char oneside,mirror,drawedcolor,fogvalue,loside;
    signed char newydelta;
    char format;
    signed char *adrxyoffs,xlo,ylo;
    OBJ *a;
    totalimgs++;
    if (flags & (SC_IMAGE_FLAG_DISABLEDRAW | SC_IMAGE_FLAG_MARKFORDELETE))
	return;
    side = parentimg->side;
    mirror = flags & SC_IMAGE_FLAG_MIRRORIMAGE;

    //draw circles and health bar if needed
    if (parentimg->whocreate == SC_IMAGE_OBJ_CREATOR)
    {
	a = parentimg->creator.objcreator.obj;
	if (IsSubUnit(a->SC_Unit))
	{
	    if (!a->subunit)		//in dieing process
		return;
	    subunit = 1;
	    mainside = a->subunit->mainimage->side;
	    mirror = a->subunit->mainimage->flags & SC_IMAGE_FLAG_MIRRORIMAGE;
	}
	else
	{
	    if ((flags & SC_IMAGE_FLAG_IMGOBJMAIN) 				&& 
		!(flags & (SC_IMAGE_FLAG_IMGOVER | SC_IMAGE_FLAG_IMGUNDER)))
	    {
		ShowCircleAndBar(a);
	    }
	}
    }
    if (parentimg->newgrpmethod != NORMAL)
    {
	//if unit is burrowed and grp method is not normal - dont show the overlay
	if (parentimg->newgrpmethod == DRAWBURROWED && grpmethod != NORMAL )	
	    return;
        if (!alldattbl.images_dat->Draw_If_Cloaked[imageid])
    	    return;
	newydelta = 0;
    }
    else
        newydelta = ydelta;
    if (!subunit)
    {
	if (alldattbl.images_dat->Graphic_Turns[imageid])
	{
	    if (side & 0x80)
	    {
		oneside = (256 - side) / 8;
		mirror = SC_IMAGE_FLAG_MIRRORIMAGE;
	    }
	    else
		oneside =  side / 8;
	}
	else
	{
	    oneside=0;
	}
	if (lo)
	{
	    //we have lo offsets
	    adrxyoffs = GetLoXY(lo,framenr + oneside,0);
	    //	if (flags & SC_IMAGE_FLAG_MIRRORIMAGE)
	    if (mirror)
		xlo = -adrxyoffs[0];
	    else
		xlo = adrxyoffs[0];
	    ylo = adrxyoffs[1];
	}
	else
	{
	    xlo = 0;
	    ylo = 0;
	}
    }
    else
    {
	if (alldattbl.images_dat->Graphic_Turns[a->subunit->mainimage->imageid])
	{
	    if (mainside & 0x80)
	    {
		side2 = (256 - mainside) / 8;
	    }
	    else
	    {
		side2 = mainside / 8;
	    }
	
	}
	else
	{
	    mirror = 0;
	    side2=0;
	}
	if (alldattbl.images_dat->Graphic_Turns[imageid])
	{
	    if (side & 0x80)
	    {
		mirror = SC_IMAGE_FLAG_MIRRORIMAGE;
		oneside = (256 - side) / 8;
	    }
	    else
	    {
		mirror = 0;
		oneside =  side / 8;
	    }
	}
	else
	{
	    mirror = 0;
	    oneside=0;
	}
	xlo = 0;
	ylo = 0;
	if (parentimg->lo)	//offset of subunit
	{
	    //we have lo offsets
	    adrxyoffs = GetLoXY(parentimg->lo,framenr + side2,0);
	    if (mainside & 0x80)
		xlo += -adrxyoffs[0];
	    else
		xlo += adrxyoffs[0];
	    ylo += adrxyoffs[1];
	}
	if (this != parentimg)	//offset of fire of subunit
	{
	    if (lo)
	    {
		if (side & 0x80)
		{
		    side2 = (256 - side) / 8;
		}
		else
		{
		    side2 =  side / 8;
		}
		adrxyoffs = GetLoXY(lo,parentimg->framenr + side2,0);
		if (parentimg->side & 0x80)
		    xlo += -adrxyoffs[0];
		else
		    xlo += adrxyoffs[0];
		ylo += adrxyoffs[1];
	    }
	}
    }
    if (remaptable == BLACKTABLE)
    {
//	drawedcolor = 21+SHADOWLINEINBLACKTABLE*invisiblecolors/255;
	drawedcolor = BLACKGRADCOLOR-SHADOWLINEINBLACKTABLE;
    }
    else
    {
	drawedcolor = parentimg->invisiblecolors;
    }
    if (flags & SC_IMAGE_FLAG_NOCHECKFORFOG)
	fogvalue = 2;
    else
	fogvalue = GetMapFog(x,y);
    switch(fogvalue)
    {
	case 2:
	    drawedimgs++;
	    if (grpmethod != NORMAL)
		format = grpmethod;
	    else
		if (parentimg->newgrpmethod != NORMAL)
		    format = parentimg->newgrpmethod;
		else
    		    format = grpmethod;
	    saveandputimage(	x + xlo,
				y + ylo,
				xdelta,newydelta,
				grp,
				format,
				drawedcolor,
				parentimg->imageusercolor,
				remaptable,
				framenr + oneside,
				flags|mirror,
				protossbuildwhite);
	    if (alldattbl.images_dat->Clickable[imageid])
	    {
		if (parentimg->whocreate == SC_IMAGE_OBJ_CREATOR)
		    parentimg->CheckForMouseOver(parentimg->creator.objcreator.obj);
	    }
	    break;
	case 1:
	case 0:
//	    doselectedOBJbit(a,NUMBGAMER,0);
	    if (parentimg->whocreate == SC_IMAGE_OBJ_CREATOR)
    		deselectobj(parentimg->creator.objcreator.obj);
	    break;

    }
    if (grpmethod == WARPFROMWHITE)
    {
	if (++protossbuildwhite == 16 * WARPBUILDWHITEMULFACTOR)
	{
	    //end warping from white texture, destroy current img
	    if (parentimg->whocreate == SC_IMAGE_OBJ_CREATOR)
		SIGOrder_ProtossBuildFinishConstruct(parentimg->creator.objcreator.obj);
	    DeleteChildImg();
	}
    }
}
//============================================
char destselectionsteps[2][4]={ 
		{MOUSEON_ENEMYUNIT,MOUSEON_NEUTRALUNIT,MOUSEON_ALLIANCEUNIT,MOUSEON_MYUNIT},
		{MOUSEON_ENEMYBUILD,MOUSEON_NEUTRALBUILD,MOUSEON_ALLIANCEBUILD,MOUSEON_MYBUILD}
				};
//============================================
void MAIN_IMG::CheckForMouseOver(OBJ *a)
{
    int alliance,isbuild,mouseobjtype,focusonobj;
    if (IsDoodadState(a->SC_Unit) && (IsInvincibleUnit(a->SC_Unit)||GetDoodadState(a)==DOODAD_BOTTOM_STATE))
	return;
//    if (IfCanClickOBJ(a->SC_Unit) && !IsPickupUnit(a->SC_Unit))
    if (IfCanClickOBJ(a->SC_Unit) && ( !a->carryobj || !IsPickupUnit(a->SC_Unit)))
    {
	alliance = player_aliance(NUMBGAMER,a->playernr);
	isbuild = IsBuild(a->SC_Unit);
	mouseobjtype = destselectionsteps[isbuild][alliance];
	if (prevmouseobjtype>mouseobjtype && !MouseOnOBJS[mouseobjtype])
	{
    	    focusonobj = mouseborder2(	(xpos>>8)-map.MAPXGLOBAL-GetUnitWidthAndHeight(a->SC_Unit,UNITDIM_WIDTH)/2,
                		        (ypos>>8)-map.MAPYGLOBAL-GetUnitWidthAndHeight(a->SC_Unit,UNITDIM_HEIGHT)/2,
                			(xpos>>8)-map.MAPXGLOBAL+GetUnitWidthAndHeight(a->SC_Unit,UNITDIM_WIDTH)/2,
                			(ypos>>8)-map.MAPYGLOBAL+GetUnitWidthAndHeight(a->SC_Unit,UNITDIM_HEIGHT)/2);
	    if (focusonobj)
		if (IfUnitIsSelectable(a) )
		{
		    prevmouseobjtype = mouseobjtype;
		    MouseOnOBJS[mouseobjtype]=a;
		}
	}
    }
}
//============================================
void MAIN_IMG::DeleteMainImg(MAINIMGLIST *mainimglist)
{
    flags |= SC_IMAGE_FLAG_MARKFORDELETE;
    mainimglist->MarkForDelElem(imglist_elemnr);
}
//============================================
void MAIN_IMG::DeleteMainImg(void)
{
    flags |= SC_IMAGE_FLAG_MARKFORDELETE;
    mainimageslist.MarkForDelElem(imglist_elemnr);
}
//============================================
void MAIN_IMG::DeleteChilds(int flags)
{
    int elemnr;
    OVERLAY_IMG *tempimg;
    if (childlists)
    {
        childlists->EnumListInit();
        while(tempimg = (OVERLAY_IMG *) childlists->GetNextListElem(&elemnr))
        {
    	    tempimg->flags |= SC_IMAGE_FLAG_MARKFORDELETE;
	    childlists->MarkForDelElem(elemnr);
	}
    }
}
//============================================
void MAIN_IMG::DeleteMainImgAndChilds(MAINIMGLIST *mainimglist)
{
    DeleteChilds(SC_IMAGE_FLAG_IMGUNDER|SC_IMAGE_FLAG_IMGOVER);
    DeleteMainImg(mainimglist);
}
//============================================
void MAIN_IMG::DeleteMainImgAndChilds(void)
{
    DeleteChilds(SC_IMAGE_FLAG_IMGUNDER|SC_IMAGE_FLAG_IMGOVER);
    DeleteMainImg(&mainimageslist);
}
//============================================
void OVERLAY_IMG::DeleteChildImg(void)
{
    int elemnr;
    OVERLAY_IMG *tempimg;
    if (parentimg->childlists)
    {
	parentimg->childlists->EnumListInit();
	while(tempimg = (OVERLAY_IMG *) parentimg->childlists->GetNextListElem(&elemnr))
	{
	    if (tempimg == this)
	    {
		tempimg->flags |= SC_IMAGE_FLAG_MARKFORDELETE;
		parentimg->childlists->MarkForDelElem(elemnr);
		return;
	    }
	}
	    
    }
}
//============================================
void MAIN_IMG::HideChildsImgFlag(void)
{
    OVERLAY_IMG *tempimg;
    if (childlists)
    {
	childlists->EnumListInit();
	while(tempimg = (OVERLAY_IMG *) childlists->GetNextListElem(NULL))
	{
	    tempimg->HideImgFlag();
	}
    }
}
//============================================
void MAIN_IMG::ShowChildsImgFlag(void)
{
    OVERLAY_IMG *tempimg;
    if (childlists)
    {
	childlists->EnumListInit();
	while(tempimg = (OVERLAY_IMG *) childlists->GetNextListElem(NULL))
	{
	    tempimg->ShowImgFlag();
	}
    }
}
//============================================
void MAIN_IMG::UnitDirection256(unsigned char direction)
{
    if (side == direction)
	return;
    side = direction;
}
//============================================
void MAIN_IMG::UnitNeededDirection256(unsigned char direction)
{
    if (neededside == direction)
	return;
    neededside = direction;
    flags |= SC_IMAGE_FLAG_NEEDROTATIONTODIRECTION;
}
//============================================
void MAIN_IMG::UnitNeededDeltaDirection256(unsigned char deltadirection)
{
    if (!deltadirection)
	return;
    neededside += deltadirection;
    flags |= SC_IMAGE_FLAG_NEEDROTATIONTODIRECTION;
}
//============================================
void MAIN_IMG::AllUnitDirection256(unsigned char direction)
{
    if (side == direction)
	return;
    side = direction;
    neededside = direction;
}
//============================================
void MAIN_IMG::SetDeltaCoords(OBJ *a,int deltax256,int deltay256)
{
    xpos += deltax256;
    ypos += deltay256;
    if (a)
	CheckForKartChanges(a,xpos,ypos);
}
//============================================
int MAIN_IMG::Rotation(unsigned char rotationfactor)
{
    OBJ *a;
    unsigned char delta;
    signed char rot,sign;
    if (side == neededside)
    {
	flags &= ~SC_IMAGE_FLAG_NEEDROTATIONTODIRECTION;
	return(1);
    }
    delta = neededside - side;
    if (delta & 0x80)
    {
	rot = -rotationfactor;
	delta = 256-delta;
    }
    else
    {
	rot = rotationfactor;
    }
    if (delta <= rotationfactor)
    {
	flags &= ~SC_IMAGE_FLAG_NEEDROTATIONTODIRECTION;
	side = neededside;
	delta = 1;
    }
    else
    {
	side += rot;
	delta = 0;
    }
    if (whocreate == SC_IMAGE_OBJ_CREATOR)
    {
	a = creator.objcreator.obj;
	if (a->subunit)
	{
	    if (IsSubUnit(a->SC_Unit))
	    {
		ChangeSubUnitCoords(a,a->subunit);
	    }
	    else
	    {
		ChangeSubUnitCoords(a->subunit,a);
	    }
	}
    }
    return(delta);
}
//============================================
void MAIN_IMG::MoveInUnitDirection(OBJ *a,unsigned char side,int speed)
{
    int deltax,deltay;
    deltax = inertion256[side][0];
    deltay = inertion256[side][1];
    SetDeltaCoords(a,(deltax * speed) >> 16,(deltay * speed) >> 16);
}
//============================================

/*============================================
//============================================
//============================================
//============================================
*/
void SetOBJxy256(OBJ *a,int xpos256,int ypos256)
{
    a->mainimage->xpos = xpos256;
    a->mainimage->ypos = ypos256;
}
//============================================
MAIN_IMG *OBJCreateImage(OBJ *a,int x256,int y256,unsigned char useiscriptnr,int groundair,unsigned short constr_id,unsigned short imagelo_id)
{
    MAIN_IMG *img;
    unsigned short flags,flingy_id,sprites_id,images_id,iscript_id;
    unsigned char side,subunitnr,elevationlevel;
    if (a->SC_Unit == SC_MAPREVEALEROBJ )
	flags = SC_IMAGE_FLAG_DISABLEDRAW;
    else
	flags = 0;
    if (!constr_id)
    {
	flingy_id = alldattbl.units_dat->flingy_id[a->SC_Unit];
        sprites_id = alldattbl.flingy_dat->sprites_id[flingy_id];
	if (alldattbl.flingy_dat->MoveControl[flingy_id] == FLINGYMOVECONTROL_ISCRIPT)
	    flags |= SC_IMAGE_FLAG_ISCRIPTROTATION;
	if (!alldattbl.sprites_dat->is_visible[sprites_id])
	    flags |= SC_IMAGE_FLAG_DISABLEDRAW;
	images_id = alldattbl.sprites_dat->images_id[sprites_id];
    }
    else
    {
	images_id = constr_id;
    }    
    elevationlevel = GetUnitElevationLevel(a->SC_Unit);
    if ((IsBuild(a->SC_Unit)||IsDoodadState(a->SC_Unit)))
	flags |= SC_IMAGE_FLAG_SAVEINFOGTABLE;
    if (groundair)
	flags |= SC_IMAGE_FLAG_AIRIMG;
    else//prevent ground unit to have air level
	if (elevationlevel > 10)
	    elevationlevel = 10;
    side = alldattbl.units_dat->UnitDirection[a->SC_Unit];
    if (side == MAXFACEDIRECTIONS)
	side = myrand(MAXFACEDIRECTIONS);
    img = new MAIN_IMG(images_id,x256,y256,elevationlevel,imagelo_id,side * 8,a->color,flags,useiscriptnr);
    a->mainimage = img;
    subunitnr = alldattbl.units_dat->Subunit1[a->SC_Unit];
    if (subunitnr<MAX_UNITS_ELEM)
    {
	img->flags |= SC_IMAGE_FLAG_NEEDTOCREATESUBUNIT;
	img->SetIScriptNr(ISCRIPTNR_INITTURRET);
	switch(a->SC_Unit)
	{
	    case SC_TANKSIEGEOBJ:
	    case SC_HERO_EDMUNDDUKESMOBJ:
		a->modemove = MODETANKSIEGE;
		SetOrder(a,1,&SIGOrder_Tank_AfterSiegeCmd);
		break;
	}
    }
    img->whocreate = SC_IMAGE_OBJ_CREATOR;
    img->creator.objcreator.obj = a;
    img->flags |= SC_IMAGE_FLAG_IMGOBJMAIN;
    if (useiscriptnr != ISCRIPTNR_UNKNOWN)
	iscriptinfo.ExecuteScript(img);	//executing on creation
//    else
//	printf("iscriptnr4=255\n");
    ForceKartChanges(a);
    return(img);
}
//============================================
void loadandputimage(int POSINMAP)
{
    int nrpicture,x,y,maxcolor,grcolor,inverseflag,format,slots;
    int xk,yk;
//    unsigned char SC_Unit;
    GRPFILE *grppict;
    char colortable;
    OBJstruct *b;
    xk=map.MAPXGLOBAL;
    yk=map.MAPYGLOBAL;
    int player=NUMBGAMER;
    if (map.mapbits.underfog[player])
    {
	slots = map.mapbits.underfog[player][POSINMAP]->nrofslots;
	for (int i=0;i<slots;i++)
	{
	    nrpicture=map.mapbits.underfog[player][POSINMAP]->pictureslot[i].nrpicture;
	    inverseflag = map.mapbits.underfog[player][POSINMAP]->pictureslot[i].flags & UNDERFOG_MIRROR;
	    format=map.mapbits.underfog[player][POSINMAP]->pictureslot[i].format;
	    colortable=map.mapbits.underfog[player][POSINMAP]->pictureslot[i].colortable;
	    grppict=map.mapbits.underfog[player][POSINMAP]->pictureslot[i].grp;
	    x=map.mapbits.underfog[player][POSINMAP]->pictureslot[i].x;
	    y=map.mapbits.underfog[player][POSINMAP]->pictureslot[i].y;
	    maxcolor=map.mapbits.underfog[player][POSINMAP]->pictureslot[i].maxcolor;
	    grcolor=map.mapbits.underfog[player][POSINMAP]->pictureslot[i].grcolor;
	    switch(format)
	    {
		case WARPTEXTURE:
		    putgrpwarpfromtext(x-xk,y-yk,grppict,nrpicture,grptexture+GRP_sizexwarppict*GRP_sizeywarppict*maxcolor);
		    break;
		case WARPFROMWHITE:
		    putgrpspr(x-xk,y-yk,grppict,TOTABLE,maxcolor,grcolor,adrtabls[colortable],nrpicture);
		    break;
		default:
		    if (!inverseflag)
			putgrpspr(x-xk,y-yk,grppict,format,maxcolor,grcolor,adrtabls[colortable],nrpicture);
		    else
			iputgrpspr(x-xk,y-yk,grppict,format,maxcolor,grcolor,adrtabls[colortable],nrpicture);
		    break;
	    }
	}
    }
}
//==============================================
void saveandputimage(int x,int y,int xdelta,int ydelta,GRPFILE *grppict,int format,
		    int maxcolor,int grcolor,int colortable,int nrpicture,unsigned short flags,unsigned char buildwhite)
{
    int xpos,ypos,sizex,sizey,slotnr,retstatus=0;
    int xk,yk;
    int POSINMAP;
    unsigned char slotflags=0;
    POSINMAP = y/32*MAXXMAP+x/32;
    x += xdelta;
    y += ydelta;
    x -= grppict->SizeX/2;
    y -= grppict->SizeY/2;
    xk = map.MAPXGLOBAL;
    yk = map.MAPYGLOBAL;
    switch(format)
    {
	case WARPTEXTURE:
	    maxcolor = buildwhite;
	    putgrpwarpfromtext(x-xk,y-yk,grppict,nrpicture,grptexture+GRP_sizexwarppict*GRP_sizeywarppict*maxcolor);
	    break;
	case WARPFROMWHITE:
	    maxcolor = buildwhite/WARPBUILDWHITEMULFACTOR + 48;
	    putgrpspr(x-xk,y-yk,grppict,TOTABLE,maxcolor,grcolor,adrtabls[colortable],nrpicture);
	    break;
	case DRAWBURROWED:
	    format=NORMAL;
	default:
	    if (flags & SC_IMAGE_FLAG_MIRRORIMAGE)
		iputgrpspr(x-xk,y-yk,grppict,format,maxcolor,grcolor,adrtabls[colortable],nrpicture);
	    else
		putgrpspr(x-xk,y-yk,grppict,format,maxcolor,grcolor,adrtabls[colortable],nrpicture);
	    break;
    }
    if (flags & SC_IMAGE_FLAG_SAVEINFOGTABLE)
    {
	saveinfogtable(x,y,POSINMAP,grppict,format,colortable,maxcolor,grcolor,nrpicture,flags);
    }
}
//==================================
void saveinfogtable(int x,int y,int POSINMAP,GRPFILE *grppict,int format,int colortable,
		    int maxcolor,int grcolor,int nrpicture,unsigned short flags)
{
    int slotnr;
    unsigned char slotflags=0;
    int playernr=NUMBGAMER;
    if (!map.mapbits.underfog[playernr][POSINMAP])
    {
	map.mapbits.underfog[playernr][POSINMAP]=(UNITUNDERFOG *)wmalloc(sizeof(UNITUNDERFOG));
	memset(map.mapbits.underfog[playernr][POSINMAP],0,sizeof(UNITUNDERFOG));
    }
    slotnr = map.mapbits.underfog[playernr][POSINMAP]->nrofslots;
    if (flags & SC_IMAGE_FLAG_IMGUNDER)		
    {
        if (slotnr)				//shadow must be in slot 0
    	    slotnr=0;
    }
    if (flags & SC_IMAGE_FLAG_IMGOBJMAIN)
    {
        slotflags |= UNDERFOG_MAINIMAGE;	//main image must be in slot 0 or 1
        if (slotnr>1)
    	    slotnr=0;
    }
    if (slotnr<FOGMAXPICTURESLOTS)
    {
	if (flags & SC_IMAGE_FLAG_MIRRORIMAGE)
	    slotflags |= UNDERFOG_MIRROR;
	map.mapbits.underfog[playernr][POSINMAP]->pictureslot[slotnr].nrpicture=nrpicture;
	map.mapbits.underfog[playernr][POSINMAP]->pictureslot[slotnr].flags=slotflags;
	map.mapbits.underfog[playernr][POSINMAP]->pictureslot[slotnr].format=format;
	map.mapbits.underfog[playernr][POSINMAP]->pictureslot[slotnr].colortable=colortable;
	map.mapbits.underfog[playernr][POSINMAP]->pictureslot[slotnr].grp=grppict;
	map.mapbits.underfog[playernr][POSINMAP]->pictureslot[slotnr].x=x;
	map.mapbits.underfog[playernr][POSINMAP]->pictureslot[slotnr].y=y;
	map.mapbits.underfog[playernr][POSINMAP]->pictureslot[slotnr].maxcolor=maxcolor;
	map.mapbits.underfog[playernr][POSINMAP]->pictureslot[slotnr].grcolor=grcolor;
	slotnr++;
	map.mapbits.underfog[playernr][POSINMAP]->nrofslots=slotnr;
    }
    else
    {
	DEBUGMESSCR("img_id = %d x=%d,y=%d underfog images >= %d\n",0,x,y,FOGMAXPICTURESLOTS);
	//this is geyser smoke 4 and more
    }
}
//==================================
void AllImages_FreeAndEmpty(void)
{
    mainimageslist.FreeAndEmptyAll();
    imageslist_minimap.FreeAndEmptyAll();
    imageslist_doodads.FreeAndEmptyAll();
}
//============================================
void MinimapImages_Draw(void)
{
    MAIN_IMG *img;
    imageslist_minimap.EnumListInit();
    while(img = (MAIN_IMG *) imageslist_minimap.GetNextListElem(NULL))
    {
	img->DrawOnScreenImage();
    }
}
//============================================
void AllImages_DeleteMarked(void)
{
    mainimageslist.DeleteMarked();		//deletemarked childs images
    imageslist_minimap.DeleteMarked();
    imageslist_doodads.DeleteMarked();
}
//============================================
int GetIDFromOverlayLayer(unsigned short imageid,unsigned char overlaylayer)
{
    switch(overlaylayer)
    {
	case IMAGE_OVERLAY_ATACK:	
	    return(alldattbl.images_dat->Attack_Overlay[imageid]);
	    break;
	case IMAGE_OVERLAY_DAMAGE:
	    return(alldattbl.images_dat->Damage_Overlay[imageid]);
	    break;
	case IMAGE_OVERLAY_SPECIAL:
	    return(alldattbl.images_dat->Special_Overlay[imageid]);
	    break;
	case IMAGE_OVERLAY_LANDDUST:
	    return(alldattbl.images_dat->Landing_Dust_Overlay[imageid]);
	    break;
	case IMAGE_OVERLAY_LIFTOFF:
	    return(alldattbl.images_dat->Lift_Off_Overlay[imageid]);
	    break;
	case IMAGE_OVERLAY_SHIELD:
	    return(alldattbl.images_dat->Shield_Overlay[imageid]);
	    break;
	default:
	    return(0);
    }
}
//============================================
void RemoveBloodFlameOverlays(OBJ *a, int from)
{
    int overlaynr;
    for (overlaynr=from;overlaynr<MAXDAMAGEBLOODFLAMEOVERLAYS;overlaynr++)
    {
        if (a->damageoverlayid[overlaynr])
        {
	    DelSpecificChildsImageID(a->mainimage,a->damageoverlayid[overlaynr],a->damageoverlayid[overlaynr]);
    	    a->damageoverlayid[overlaynr] = 0;
    	}
    }
}
//============================================
//launches from apply damage
//============================================
#define	MAXDAMAGEOVERLAYS		22
void AddRemoveBloodFlameOverlays(OBJ *a)
{
    LOFILE *lo;
    int i;
    MAIN_IMG *img;
    OVERLAY_IMG *tempimg;
    signed char maxdamageoverlays,overlaynr=0;
    unsigned short damagelo_id,imageid;
    signed char *adrxyoffs,xlo,ylo;
    if (IsReadyOBJ(a))//do not show bloodflame if constructing
    {	
	maxdamageoverlays = DamageFactor(a->life,GetUnitMaxLife(a->SC_Unit));
	if (a->lastdamageoverlays == maxdamageoverlays)//no need to change anything
	    return;
	a->lastdamageoverlays = maxdamageoverlays;
	img = a->mainimage;
	damagelo_id = GetIDFromOverlayLayer(img->imageid,IMAGE_OVERLAY_DAMAGE);
	if (!damagelo_id)
	    return;
	if (GetLoadedImage(damagelo_id,(void **)&lo) < 0)
    	    DEBUGMESSCR("lo.cant load image_id=%d\n",damagelo_id);
	for (i=0;i<MAXDAMAGEOVERLAYS;i++)
	{
	    if (maxdamageoverlays<=0)
		break;
	    adrxyoffs = GetLoXY(lo,0,i);
	    xlo = adrxyoffs[0];
	    ylo = adrxyoffs[1];
	    if (xlo==0x7f || ylo==0x7f)
	        continue;
	    if (maxdamageoverlays & 6)	//if a big fire
		imageid = IMAGEID_BLOODFLAMELARGE1 + i;
	    else
		imageid = IMAGEID_BLOODFLAMESMALL1 + i;
	    
	    if (a->damageoverlayid[overlaynr])
	    {
		if (a->damageoverlayid[overlaynr] != imageid)
		{
//		    change image to imageid
		    img->childlists->EnumListInit();
		    while(tempimg = (OVERLAY_IMG *) img->childlists->GetNextListElem(NULL))
		    {
			if (tempimg->imageid == a->damageoverlayid[overlaynr])			//found image
			{
			    tempimg->ForceChangeImage(imageid);
			    a->damageoverlayid[overlaynr] = imageid;
			    break;
			}
		    }
		    
		}
	    }
	    else
	    {
		a->damageoverlayid[overlaynr] = imageid;
		tempimg = new OVERLAY_IMG(img,imageid,xlo,ylo,img->elevationlevel+2,		//fire over all images of unit, include over overimage
					 SC_IMAGE_FLAG_IMGOVER | SC_IMAGE_FLAG_SAVEINFOGTABLE,ISCRIPTNR_INIT);
//		iscriptinfo.ExecuteScript(tempimg);
	    }
	    maxdamageoverlays-=2;
	    overlaynr++;
	}
    }
    RemoveBloodFlameOverlays(a,overlaynr);
}
//============================================
#define	MAXDUSTOVERLAYS		25
short dust_images_id[MAXDUSTOVERLAYS]=
			{
//			     0   1    2   3  4  5   6   7   8    9  10  11  12  13  14  15  16  17  18  19  20   21  22  23  24
			    494,495,496,497,498,499,500,502,501,  0,  0,  0,  0,  0,  0,  0,494,495,496,497,498,501,502,500,499
			};
char dust_inv[MAXDUSTOVERLAYS]=
			{
			      0,0,0,0,0,0,0,1,0,0, 0,0,0,0,0,0,1,1,1,1, 1,1,0,1,1
			};
//============================================
void AddDustImages(struct OBJ *a,int overlaylayer)
{
    LOFILE *lo;
    MAIN_IMG *img;
    OVERLAY_IMG *newimg;
    int i;
    unsigned short imagelo_id,flags,imageid;
    signed char *adrxyoffs,xlo,ylo;
    img = a->mainimage;
    imagelo_id = GetIDFromOverlayLayer(img->imageid,overlaylayer);
    if (!imagelo_id)
	return;
    if (GetLoadedImage(imagelo_id,(void **)&lo) < 0)
        DEBUGMESSCR("lo.cant load image_id=%d\n",imagelo_id);
    flags = SC_IMAGE_FLAG_IMGOVER;
    for (i=0;i<MAXDUSTOVERLAYS;i++)
    {
	adrxyoffs = GetLoXY(lo,0,i);
	xlo = adrxyoffs[0];
	ylo = adrxyoffs[1];
	if (xlo==0x7f||ylo==0x7f)
	    continue;
	if (dust_inv[i])
	    flags |= SC_IMAGE_FLAG_MIRRORIMAGE;
	imageid = dust_images_id[i];
	newimg = new OVERLAY_IMG(a->mainimage,imageid,xlo,ylo,a->mainimage->elevationlevel+1,flags,ISCRIPTNR_INIT);
	iscriptinfo.ExecuteScript(newimg);
    }
}
//========================================
void AddShieldImage(struct OBJ *a,int directiondamage)
{
    int effectlevel,inv;
    int x,y;
    MAIN_IMG *img;
    OVERLAY_IMG *newimg;
    unsigned short imagelo_id;
    if (a->shield>=(4<<8))
    {
	img = a->mainimage;
	imagelo_id = alldattbl.images_dat->Shield_Overlay[img->imageid];
	//if not build show only one shield damage image
	if (!IsBuild(a->SC_Unit))
	{
	    if (CheckForSpecificChildsImageID(a->mainimage,IMAGEID_SHIELDOVERLAY,IMAGEID_SHIELDOVERLAY))
		return;
	}
	newimg = new OVERLAY_IMG(img,IMAGEID_SHIELDOVERLAY,imagelo_id,a->mainimage->elevationlevel+1,SC_IMAGE_FLAG_IMGOVER,ISCRIPTNR_INIT);
	iscriptinfo.ExecuteScript(newimg);
    }
}
//========================================
void CreatePylonSelectArea(void)
{
    int i;
    OBJ *a;
    OVERLAY_IMG *newimg;
    if (pylonselected)
    {
	for (i=0;i<MaxObjects;i++)
	{
	    a = objects[i];
	    if (a->SC_Unit == SC_PYLONOBJ && a->playernr == NUMBGAMER && !(a->prop & VARPYLONAREAACTIVE) && IsReadyOBJ(a))
	    {
		a->prop |= VARPYLONAREAACTIVE;
		newimg = new OVERLAY_IMG(a->mainimage,IMAGEID_PYLONSELECTAREA1,0,0,a->mainimage->elevationlevel-2,
					    SC_IMAGE_FLAG_IMGUNDER|SC_IMAGE_FLAG_IGNORPARENTSAVEINFOGTABLE,ISCRIPTNR_INIT);
		iscriptinfo.ExecuteScript(newimg);
		drawpylonareaactive=1;
	    }
	}	
    }
}
//========================================
void RemovePylonSelectArea(void)
{
    int i;
    OBJ *a;
    if (!pylonselected)
    {
	if (drawpylonareaactive)
	{
	    drawpylonareaactive=0;
	    for (i=0;i<MaxObjects;i++)
	    {
		a = objects[i];
		if (a->SC_Unit == SC_PYLONOBJ && a->playernr == NUMBGAMER && (a->prop & VARPYLONAREAACTIVE))
		{
		    //destroy pylon area images
		    a->prop &= ~VARPYLONAREAACTIVE;
		    DelSpecificChildsImageID(a->mainimage,IMAGEID_PYLONSELECTAREA1,IMAGEID_PYLONSELECTAREA4);
		}
	    }	
	}
    }
}
//========================================
signed char corrosiveacid_type[10]={-1,0,1,1,2,2,3,3,3,3};
//========================================
void AddImagesForAtribute(OBJ *a,int atributenr,short image_id,int independentofmain)
{
    int acidsporenr;
    MAIN_IMG *img;
    OVERLAY_IMG *newimg;
    unsigned short flags;
    if (a->modemove == MODEDIE)		//prevent add spell overlay on dieing unit
	return;
    if (!image_id)
	return;
    img = a->mainimage;
    flags = 0;
    if (independentofmain == IMAGEOVERLAY_NOTDEPENDONMAINIMG)
	flags |= SC_IMAGE_FLAG_INDEPENDENTOFMAINIMG;
    if (IsOnSkyOBJ(a))
	flags |= SC_IMAGE_FLAG_AIRIMG;
    switch(atributenr)
    {
	case ATRMATRIX:
	    newimg = new OVERLAY_IMG(img,image_id,0,0,a->mainimage->elevationlevel+1,flags | SC_IMAGE_FLAG_IMGOVER ,ISCRIPTNR_INIT);
	    iscriptinfo.ExecuteScript(newimg);
	    newimg = new OVERLAY_IMG(img,image_id+3,0,0,a->mainimage->elevationlevel-1,flags | SC_IMAGE_FLAG_IMGUNDER,ISCRIPTNR_INIT);
	    iscriptinfo.ExecuteScript(newimg);
	    break;
	case ATRCORROSIVEACID:
	    acidsporenr = GetCorrosiveAcidValue(a);
	    if (acidsporenr)
	    {
		newimg = new OVERLAY_IMG(img,image_id+corrosiveacid_type[acidsporenr],0,0,
					a->mainimage->elevationlevel+1,flags | SC_IMAGE_FLAG_IMGOVER,ISCRIPTNR_INIT);
		iscriptinfo.ExecuteScript(newimg);
	    }
	    break;
	default:
	    newimg = new OVERLAY_IMG(img,image_id,0,0,a->mainimage->elevationlevel+1,flags | SC_IMAGE_FLAG_IMGOVER,ISCRIPTNR_INIT);
	    iscriptinfo.ExecuteScript(newimg);
	    break;
    }

}
//========================================
void DelImagesForAtribute(OBJ *a,int atributenr)
{
    unsigned short image_idfrom,image_idto,flags;
    int castmagenr;
    castmagenr = magenrfrommageatr[atributenr];
    image_idfrom = mageprop[castmagenr].images_id[GetUnitOverlaySize(a->SC_Unit)];
    if (!image_idfrom)
	return;
    flags = 0;
    switch(atributenr)
    {
	case ATRCORROSIVEACID:
	case ATRMATRIX:
	    image_idto = image_idfrom + 3;
	    break;
	default:
	    image_idto = image_idfrom;
	    break;
    }
    DelSpecificChildsImageID(a->mainimage,image_idfrom,image_idto);
}
//========================================
short IMAGEID_ATRCORROSIVEACID[3]={IMAGEID_ACIDSPORES1,IMAGEID_ACIDSPOREM1,IMAGEID_ACIDSPOREL1};
//========================================
void InitSporeImage(OBJ *a,int acidvalue)
{
    int deltaimg;
    unsigned short image_id;
    image_id = IMAGEID_ATRCORROSIVEACID[GetUnitOverlaySize(a->SC_Unit)];
    AddImagesForAtribute(a,ATRCORROSIVEACID,image_id,IMAGEOVERLAY_DEPENDONMAINIMG);
}
//========================================
void ChangeSporeImage(OBJ *a,int prevspores,int nextspores)
{
    MAIN_IMG *img;
    OVERLAY_IMG *tempimg;
    unsigned short image_idfrom,image_idto,flags;
    int deltaimg;
    deltaimg =  corrosiveacid_type[nextspores] - corrosiveacid_type[prevspores];
    if (deltaimg)
    {
	img = a->mainimage;
	if (img->childlists)
	{
	    image_idfrom = IMAGEID_ATRCORROSIVEACID[GetUnitOverlaySize(a->SC_Unit)];
	    flags = 0;
	    image_idto = image_idfrom + 3;
	    img->childlists->EnumListInit();
	    while(tempimg = (OVERLAY_IMG *) img->childlists->GetNextListElem(NULL))
	    {
		if (tempimg->imageid >= image_idfrom && tempimg->imageid <= image_idto)			//found image
		{
		    tempimg->ForceChangeImage(tempimg->imageid + deltaimg);
		}
	    }
	}
    }
}
//========================================
//0 - init minerals, 1 - almostbuilt 75%, 2- specialstate2 50%, 3- specialstate1 - 25%
void ChangeMineralSprite(OBJ *a)
{
    if (!a->data.resource.resource_count)
    {
	SetOBJIScriptNr(a,ISCRIPTNR_DEATH,ISCRIPTNR_EXECUTE);
    }
    else
    {
	switch(a->data.resource.resource_count * 4 / a->data.resource.max_resource_count )
	{
	    case 4:		//=100%
	    case 3:		//99%-75%
		break;
	    case 2:		//50%-75%
		SetOBJIScriptNr(a,ISCRIPTNR_ALMOSTBUILT,ISCRIPTNR_EXECUTE);
		break;
	    case 1:		//25%-50%
		SetOBJIScriptNr(a,ISCRIPTNR_SPECIALSTATE2,ISCRIPTNR_EXECUTE);
		break;
	    case 0:		//0%-25%
		SetOBJIScriptNr(a,ISCRIPTNR_SPECIALSTATE1,ISCRIPTNR_EXECUTE);
		break;
	}
    }
}
//========================================
void ShowBlink(int screenxpos,int screenypos,unsigned char color)
{
    MAIN_IMG *newimg;
    newimg = new MAIN_IMG(blinkgrp,screenxpos,screenypos,color,0,ISCRIPTID_BLINK,ISCRIPTNR_INIT,TOTABLE,TFONTGAMTABLE);
    imageslist_minimap.AddElem(newimg);
    newimg->whocreate = SC_IMAGE_MINIMAP_CREATOR;
    iscriptinfo.ExecuteScript(newimg);
}
//========================================
void SaveOBJImagesToFogTable(OBJ *a)
{
    MAIN_IMG *img;
    OVERLAY_IMG *tempimg;
    int x,y,POSINMAP;
    img = a->mainimage;
    x = GetOBJx(a);
    y = GetOBJy(a);
    POSINMAP = y/32*MAXXMAP+x/32;
    if (img->childlists)
    {
	img->childlists->EnumListInit();
	while(tempimg = (OVERLAY_IMG *) img->childlists->GetNextListElem(NULL))
	{
	    saveinfogtable(x-tempimg->grp->SizeX/2,y-tempimg->grp->SizeY/2,POSINMAP,tempimg->grp,SHADOW,BLACKTABLE,
			    img->invisiblecolors,img->imageusercolor,0,tempimg->flags);
	    break;
	}
    }
    //this is main picture
    saveinfogtable(x-img->grp->SizeX/2,y-img->grp->SizeY/2,POSINMAP,img->grp,NORMAL,NONETABLE,
		    img->invisiblecolors,img->imageusercolor,EVERYUNITRAWIMAGENR,img->flags);
}
//========================================
void DrawRawMainImageOBJ(int x,int y,unsigned char SC_Unit,int plcolor,int grpfunc,int maxcolors)
{
    GRPFILE *grp;
    unsigned short flingy_id,sprites_id,images_id;
    flingy_id = alldattbl.units_dat->flingy_id[SC_Unit];
    sprites_id = alldattbl.flingy_dat->sprites_id[flingy_id];
    images_id = alldattbl.sprites_dat->images_id[sprites_id];
    GetLoadedImage(alldattbl.images_dat->images_tbl[images_id],(void **)&grp);
    putgrpspr(x-grp->SizeX/2,y-grp->SizeY/2,grp,grpfunc,maxcolors,plcolor,NULL,EVERYUNITRAWIMAGENR);
}
//========================================
int CheckForSpecificChildsImageID(MAIN_IMG *img,short image_idfrom,short image_idto)
{
    OVERLAY_IMG *tempimg;
    if (img->childlists)
    {
	img->childlists->EnumListInit();
	while(tempimg = (OVERLAY_IMG *) img->childlists->GetNextListElem(NULL))
	{
	    if (tempimg->imageid >= image_idfrom && tempimg->imageid <= image_idto)			//found image
	    {
		return(1);
	    }
	}
    }
    return(0);
}
//========================================
void DelSpecificChildsImageID(MAIN_IMG *img,short image_idfrom,short image_idto)
{
    OVERLAY_IMG *tempimg;
    if (img->childlists)
    {
	img->childlists->EnumListInit();
	while(tempimg = (OVERLAY_IMG *) img->childlists->GetNextListElem(NULL))
	{
	    if (tempimg->imageid >= image_idfrom && tempimg->imageid <= image_idto)			//found image
	    {
		tempimg->SetIScriptNr(ISCRIPTNR_DEATH);
		iscriptinfo.ExecuteScript(tempimg);
	    }
	}
    }
}
//============================================
void ForceDestroyAllOBJsImages(OBJ *a)
{
    //mmark for remove main images and childs
    a->mainimage->DeleteMainImgAndChilds(&mainimageslist);
    //remove those images
    mainimageslist.DeleteMarked();
    //remove object
    MarkForDestroy(a);
}
//============================================
int MAIN_IMG::CheckForVisibility(void)
{
//
    return(1);
}
//============================================
void AllImages_Sort(void)
{
    char deleteflag;
    int visible;
    MAIN_IMG *img;
    OVERLAY_IMG *img2;
    mainimageslist.EnumListInit();
    while(img = (MAIN_IMG *) mainimageslist.GetABSNextListElem(&deleteflag))
    {
	visible = 1;
	if (!(img->flags & (SC_IMAGE_FLAG_DISABLEDRAW | SC_IMAGE_FLAG_MARKFORDELETE)))
	{
	    if (visible = img->CheckForVisibility())
		img->AddToDraw();
	}
	if (visible && img->childlists)
	{
	    img->childlists->EnumListInit();
	    while(img2 = (OVERLAY_IMG *) img->childlists->GetNextListElem(NULL))
	    {
	        if (!(img2->flags & (SC_IMAGE_FLAG_DISABLEDRAW | SC_IMAGE_FLAG_MARKFORDELETE)))
		    img2->AddToDraw();
	    }
	}
    }
    imageslist_doodads.EnumListInit();
    while(img = (MAIN_IMG *) imageslist_doodads.GetABSNextListElem(&deleteflag))
    {
	visible = 1;
	if (!(img->flags & (SC_IMAGE_FLAG_DISABLEDRAW | SC_IMAGE_FLAG_MARKFORDELETE)))
	{
	    if (visible = img->CheckForVisibility())
		img->AddToDraw();
	}
	if (visible && img->childlists)
	{
	    img->childlists->EnumListInit();
	    while(img2 = (OVERLAY_IMG *) img->childlists->GetNextListElem(NULL))
	    {
	        if (!(img2->flags & (SC_IMAGE_FLAG_DISABLEDRAW | SC_IMAGE_FLAG_MARKFORDELETE)))
		    img2->AddToDraw();
	    }
	}
    }
}
//============================================
void EraseSortedIMG(void)
{
    int i;
    for (i=0;i<MAXIMAGELEVELS;i++)
    {
	elevationIMG[i].FlushList();
    }
}
//============================================
void OVERLAY_IMG::AddToDraw(void)
{
    unsigned char level;
    if (elevationlevel >= MAXIMAGELEVELS)
	level = MAXIMAGELEVELS-1;
    else
	level = elevationlevel;
    elevationIMG[level].AddList(this);
}
//============================================
void AllImages_Draw(void)
{
    int i;
    OVERLAY_IMG *img;
    AllImages_Sort();
    for (i=0;i<MAXIMAGELEVELS;i++)
    {
	elevationIMG[i].EnumListInit();
	while(img = (OVERLAY_IMG *) elevationIMG[i].GetNextListElem(NULL))
	{
	    img->DrawImage();
	}
    }
    EraseSortedIMG();
}
//============================================
void AddCircleImage(OBJ *a)
{

}
//============================================
void DelCircleImage(OBJ *a)
{

}
//============================================

