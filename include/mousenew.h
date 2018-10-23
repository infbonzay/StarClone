#if !defined(_MOUSE_W)
#define _MOUSE_W

#include <grplib/grp.h>

#ifdef WITHSDL
       #include "sdl/mousesdl.h"
       #include "sdl/keysdl.h"
#endif

#include "man.h"

#define	SMOUTHMOUSE    		1.0001

#define TYPEOFCURSORS 		19
#define MAXMOUSESIZEX		64
#define MAXMOUSESIZEY		64
#define MOUSEON_MAXVALUE	8

#define MOUSEON_MYUNIT		0
#define MOUSEON_ALLIANCEUNIT	1
#define MOUSEON_ENEMYUNIT	2
#define MOUSEON_NEUTRALUNIT	3
#define MOUSEON_MYBUILD		4
#define MOUSEON_ALLIANCEBUILD	5
#define MOUSEON_ENEMYBUILD	6
#define MOUSEON_NEUTRALBUILD	7
#define MOUSEON_MAXVALUE	8

#define SELECTIONMOUSE  	0
#define MOUSEILLEGAL		1
#define MOUSEONOBJGREEN 	2
#define MOUSEONOBJRED	 	3
#define MOUSEONOBJYELLOW 	4
#define MOUSEDOWNSCROLL		5
#define MOUSELEFTDOWNSCROLL	6
#define MOUSERIGHTDOWNSCROLL	7
#define MOUSELEFTSCROLL		8
#define MOUSERIGHTSCROLL	9
#define MOUSEUPSCROLL		10
#define MOUSELEFTUPSCROLL	11
#define MOUSERIGHTUPSCROLL	12
#define TARGGREENMOUSE		13
#define TARGNEUTRALMOUSE	14
#define TARGREDMOUSE		15
#define TARGYELLOWMOUSE		16
#define TIMEMOUSE		17
#define NORMALMOUSE		18


class HighMouse
{
    OBJ 	*MouseOnOBJS[MOUSEON_MAXVALUE];	//myunit,alianceunit,neutralunit,enemyunit,mybuild,aliancebuild,neutralbuild,enemybuild
    OBJ		*DestMouseOBJ;

    int		MouseX;
    int		MouseY;
    int		PrevMouseX;
    int		PrevMouseY;
    int		MoveOnMinimap;
    int		UnitCommandAction;
    int		MouseButtons;
        
    struct
    {
	int	posx;
	int	posy;
	int	sizex;
	int	sizey;
	char	buff[MAXMOUSESIZEX*MAXMOUSESIZEY];
    }mousesavedpixels;
    
    struct
    {
	GRPFILE		*mousegrp;
	unsigned char	curentposition;
        unsigned char	maxpositions;
        unsigned char	flag;
    }mouses[TYPEOFCURSORS];

    char	MouseBitSelection;
    char	MouseBitBorder;
    char	WaitToPressLeftButton;
    char	WaitForReleaseLeftButton;
    char	WaitToPressRightButton;
    char	WaitForReleaseRightButton;
    char	DestMouseType;
    char	MouseObjType;
    struct
    {
	int	PosX;
        int	PosY;
	char	SC_BuildUnit;
	char	CanDo;
    }Construct;

    struct
    {
	int	PosX;
        int	PosY;
	char	Presence;
    }DestCursor;

    int  LoadOneCursor(char *filename,int typemouse);
    int  LoadAllCursors(void);
    void UnloadCursors(void);
    void SetMouseOn(OBJ *mouseonobj,int desttype);
    void DrawMouse(int MenuActive, int mainarea, int selectarea,int onborder);
    void DrawSelectedArea(int selectarea);
    int  CheckBorder(int x,int y,int x2,int y2);
    void PutDestination(OBJ *destobj,int xm,int ym,int modemove,int rightclick);
    void NextMousePict(void);
    void GetMouseType(int xk,int yk,int menuActive,int mainArea,int minimapArea);
    void LoadPixelsUnderMouse(void);
    void SavePixelsUnderMouse(void);
    void MouseOnObjClear(void);
    void GetMouseOnObj(void);
private:
    float scrollmapx(int border,float factor);
    float scrollmapy(int border,float factor);

};

extern HighMouse highMouse;

#endif /*    _MOUSE_W   */

