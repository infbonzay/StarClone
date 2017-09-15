#ifndef _IMAGES_W
#define _IMAGES_W

#include <grplib/grp.h>
#include "man.h"
#include "mylist.h"

//============================================
#define WARPBUILDWHITEMULFACTOR			2
#define EVERYUNITRAWIMAGENR			0

class SC_FLINGY;
class MAIN_IMG;
class OVERLAY_IMG;
class MAINIMGLIST;
class OVERLAYIMGLIST;

#define IMAGEOVERLAY_DEPENDONMAINIMG		0
#define IMAGEOVERLAY_NOTDEPENDONMAINIMG		1

#define IMAGE_OVERLAY_ATACK			0
#define IMAGE_OVERLAY_DAMAGE			1
#define IMAGE_OVERLAY_SPECIAL			2
#define IMAGE_OVERLAY_LANDDUST			3
#define IMAGE_OVERLAY_LIFTOFF			4
#define IMAGE_OVERLAY_SHIELD			5

#define SC_IMAGE_FLAG_MIRRORIMAGE			0x0001
#define SC_IMAGE_FLAG_MARKFORDELETE			0x0002
#define SC_IMAGE_FLAG_DISABLEDRAW			0x0004
#define SC_IMAGE_FLAG_DISABLEEXECSCRIPT			0x0008
#define SC_IMAGE_FLAG_SAVEINFOGTABLE			0x0010
#define SC_IMAGE_FLAG_IGNOREPARENTSAVEINFOGTABLE	0x0020
#define SC_IMAGE_FLAG_IMGOBJMAIN			0x0040
#define SC_IMAGE_FLAG_IMGUNDER				0x0080
#define SC_IMAGE_FLAG_IMGOVER				0x0100
#define SC_IMAGE_FLAG_NOCHECKFORFOG			0x0200
#define SC_IMAGE_FLAG_INDEPENDENTOFMAINIMG		0x0400
#define SC_IMAGE_FLAG_NEEDROTATIONTODIRECTION		0x0800
#define SC_IMAGE_FLAG_ISCRIPTROTATION			0x1000
#define SC_IMAGE_FLAG_CANTBREAKCODE			0x2000
#define SC_IMAGE_FLAG_AIRIMG				0x4000
#define SC_IMAGE_FLAG_NEEDTOCREATESUBUNIT		0x8000

#define SC_IMAGE_UNKNOWN_CREATOR		0
#define SC_IMAGE_SELF_CREATOR			1
#define SC_IMAGE_OBJ_CREATOR			2
#define SC_IMAGE_FLINGY_CREATOR			3
#define SC_IMAGE_OVERLAYOBJ_CREATOR		4
#define SC_IMAGE_DOODAD_CREATOR			5
#define SC_IMAGE_MINIMAP_CREATOR		6

//============================================
struct IMAGECREATOR_FLINGY
{
    SC_FLINGY *flingy;
};
struct IMAGECREATOR_OBJ
{
    OBJ *obj;
};
union IMAGECREATOR
{
    IMAGECREATOR_OBJ		objcreator;
    IMAGECREATOR_FLINGY		flingycreator;
};
//============================================
class OVERLAY_IMG
{
public:
    MAIN_IMG		*parentimg;

    GRPFILE		*grp;
    LOFILE		*lo;

    unsigned short 	imageid;	//ref to image.dat
    unsigned short 	iscriptid;	//scriptnr fot image (last is 411)
    unsigned char	iscriptnr;	//type of script (init,death,walking,specialstate1,....)
    
    char		grpmethod;
    char		remaptable;

    unsigned short	framenr;	//grp pict nr
    unsigned char	elevationlevel;
    unsigned short	offsetcmdinbuf;	//offset of current command
    unsigned short	returnaddress;	//for call-return pairs
    signed char		waitticks;
    unsigned short	flags;
    signed char		xdelta;
    signed char		ydelta;
    unsigned char	protossbuildwhite;

    OVERLAY_IMG(MAIN_IMG *parentimg,unsigned short image_id,signed char xoffs,signed char yoffs,
			unsigned char elevation,unsigned short unitflags,unsigned char usescriptnr);
    OVERLAY_IMG(MAIN_IMG *parentimg,unsigned short image_id,unsigned short imagelo_tbl,
			unsigned char elevation,unsigned short unitflags,unsigned char usescriptnr);
    OVERLAY_IMG(MAIN_IMG *parentimg,GRPFILE *grpimg,unsigned short unitflags,unsigned short useiscriptid,
			unsigned char usescriptnr,unsigned char method,unsigned char remap);

    int  SetIScriptNr(int scriptnr);
    void ForceChangeImage(unsigned short image_id);
    void SetGrpFunc(unsigned short image_id);
    void DrawImage(void);
    void DrawImageXY(int x,int y);
    inline void ShowImgFlag(void){ flags &= ~SC_IMAGE_FLAG_DISABLEDRAW; };
    inline void HideImgFlag(void){ flags |=  SC_IMAGE_FLAG_DISABLEDRAW; };
    inline void EnableExecScript(void){ flags &= ~SC_IMAGE_FLAG_DISABLEEXECSCRIPT; };
    inline void DisableExecScript(void){ flags |=  SC_IMAGE_FLAG_DISABLEEXECSCRIPT; };
    void ImgMarkForRemove(void);
    void DeleteChildImg(void);
    void AddToDraw(void);


};

class MAIN_IMG : public OVERLAY_IMG
{
public:
//    mylistsimple	*childlists;
    OVERLAYIMGLIST	*childlists;

    signed   short	imglist_elemnr;
    unsigned short	movefactor;		//set this variable from iscript_move & iscript_setflspeed

    int 		xpos;
    int			ypos;
    char		newgrpmethod;	//if changes by cloak algorithm
    unsigned char	invisiblecolors; 	//0 - (0 - 0 colors invisible) full visible, 255 - (0-255 colors are invisible) invisible
    unsigned char	imageusercolor;	//0-15	playernrcolor,16-tored,17-togreen,18-toblue,19-tomono
    unsigned char	side;		//0-255		div to 8 to see actual side
    unsigned char	neededside;	
    unsigned char	TurnRadius;		//turn radius from associated flingy 
    char		whocreate;		//unknown,obj,flingy,...(see defines creators)
    IMAGECREATOR	creator;    
    
    MAIN_IMG(   unsigned short image_id,int x,int y,int elevation,
		signed char xoffs,signed char yoffs,
		unsigned char unitside256,unsigned char plcolor,unsigned short unitflags,
		unsigned char usescriptnr);
		
    MAIN_IMG(	unsigned short image_id,int x,int y,int elevation,
		unsigned short imagelo_tbl,
		unsigned char unitside256,unsigned char plcolor,unsigned short unitflags,
		unsigned char usescriptnr);
    MAIN_IMG(   GRPFILE *grpimg, int x,int y,unsigned char plcolor,unsigned short unitflags,
		unsigned short useiscriptid,unsigned char usescriptnr,
		unsigned char method,unsigned char remap);
    ~MAIN_IMG();
    void AddImageToParent(OVERLAY_IMG *img);
    void DrawOnScreenImage(void);
    void UnitDirection256(unsigned char direction);
    void UnitNeededDirection256(unsigned char direction);
    void UnitNeededDeltaDirection256(unsigned char deltadirection);
    void AllUnitDirection256(unsigned char direction);
    void SetDeltaCoords(OBJ *a,int deltax256,int deltay256);
    int  Rotation(unsigned char rotationfactor);
    void MoveInUnitDirection(OBJ *a,unsigned char side,int speed);
    int  SetIScriptNrAll(int scriptnr,int executeflag);
    void CheckForMouseOver(OBJ *a);
    void HideChildsImgFlag(void);
    void ShowChildsImgFlag(void);
    void DeleteMainImgAndChilds(void);
    void DeleteMainImgAndChilds(MAINIMGLIST *mainimglist);
    void DeleteChilds(int flags);
    void DeleteMainImg(MAINIMGLIST *mainimglist);
    void DeleteMainImg(void);
    int  CheckForVisibility(void);
    void ForceSetIScript(int scriptnr);

    inline void SetPos(int x,int y) { xpos = x; ypos = y; };


};

class MAINIMGLIST:public mylistsimple
{
public:
    MAINIMGLIST(int nrofelems) : mylistsimple(nrofelems) {};
    inline void AddElem(void *elem) { if (totalelem<allocatedelem) { elements[totalelem] = elem; ((MAIN_IMG *)elem)->imglist_elemnr = totalelem; totalelem++; } else DEBUGMESSCR("MAINIMGLIST:max elems exceeded\n");};

    void DeleteMarked(void);
    void FreeAndEmptyAll(void);
};

class OVERLAYIMGLIST:public mylistsimple
{
public:
    OVERLAYIMGLIST(int nrofelems) : mylistsimple(nrofelems) {};
    void DeleteMarked(void);
};


int  CheckIscriptNr(OVERLAY_IMG *img,int scriptnr);
int  SetOBJIScriptNr(OBJ *a,int scriptnr,int executeflag);

void loadandputimage(int POSINMAP);
void saveandputimage(int x,int y,int xdelta,int ydelta,GRPFILE *grppict,int format,
		    int maxcolor,int grcolor,int colortable,int nrpicture,unsigned short flags,unsigned char buildwhite);
void saveinfogtable(int x,int y,int POSINMAP,GRPFILE *grppict,int format,int colortable,
		    int maxcolor,int grcolor,int nrpicture,unsigned short flags);
void UnitsBloodFire_DrawImages(int groundair);
int  CheckForSpecificChildsImageID(OVERLAY_IMG *img,short image_idfrom,short image_idto);
void DelSpecificChildsImageID(OVERLAY_IMG *img,short image_idfrom,short image_idto);
void ShowBlink(int screenxpos,int screenypos,unsigned char color);
void RemoveBloodFlameOverlays(OBJ *a, int from);
void AddRemoveBloodFlameOverlays(OBJ *a);
void AddDustImages(struct OBJ *a,int overlaylayer);
void AddShieldImage(struct OBJ *a,int directiondamage);
void CreatePylonSelectArea(void);
void RemovePylonSelectArea(void);
void AddImagesForAtribute(OBJ *a,int atributenr,short image_id,int independentofmain);
void DelImagesForAtribute(OBJ *a,int atributenr);
void InitSporeImage(OBJ *a,int acidvalue);
void ChangeSporeImage(OBJ *a,int prevspores,int nextspores);
void ChangeMineralSprite(OBJ *a);
void SaveOBJImagesToFogTable(OBJ *a);
void DrawRawMainImageOBJ(int x,int y,unsigned char SC_Unit,int plcolor,int grpfunc,int maxcolors);
int  CheckForSpecificChildsImageID(MAIN_IMG *img,short image_idfrom,short image_idto);
void DelSpecificChildsImageID(MAIN_IMG *img,short image_idfrom,short image_idto);
int  GetIDFromOverlayLayer(unsigned short imageid,unsigned char overlaylayer);
void ForceDestroyAllOBJsImages(OBJ *a);
void AllImages_Draw(void);
void AllImages_DeleteMarked(void);
void MinimapImages_Draw(void);
void AllImages_FreeAndEmpty(void);
MAIN_IMG *OBJCreateImage(OBJ *a,int x256,int y256,unsigned char useiscriptnr,int groundair,unsigned short constr_id,unsigned short imagelo_id);
void SetOBJxy256(OBJ *a,int xpos256,int ypos256);
void AddCircleImage(OBJ *a);
void DelCircleImage(OBJ *a);



//============================================

inline void OBJSetInvisibleColors(OBJ *a,int maxinvcolors) { a->mainimage->invisiblecolors = maxinvcolors; };

inline int  GetOBJx(OBJ *a) { return(a->mainimage->xpos>>8); };
inline int  GetOBJy(OBJ *a) { return(a->mainimage->ypos>>8); };
inline int  GetOBJx256(OBJ *a) { return(a->mainimage->xpos); };
inline int  GetOBJy256(OBJ *a) { return(a->mainimage->ypos); };

inline void SetOBJxy256(OBJ *a,int x256,int y256) { a->mainimage->xpos = x256; a->mainimage->ypos = y256; };
//============================================


extern MAINIMGLIST imageslist_minimap;
extern MAINIMGLIST imageslist_doodads;
extern MAINIMGLIST mainimageslist;

#endif //_IMAGES_W



