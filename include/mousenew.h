#if !defined(_MOUSENEW_W)
#define _MOUSENEW_W

#include <grplib/grp.h>

#ifdef WITHSDL
       #include "sdl/mousesdl.h"
       #include "sdl/keysdl.h"
#endif

#include "man.h"

#define	SMOUTHMOUSE    			1.0001

#define MAXMOUSESIZEX			64
#define MAXMOUSESIZEY			64

#define MOUSEON_MYUNIT			0
#define MOUSEON_ALLIANCEUNIT	1
#define MOUSEON_ENEMYUNIT		2
#define MOUSEON_NEUTRALUNIT		3
#define MOUSEON_MYBUILD			4
#define MOUSEON_ALLIANCEBUILD	5
#define MOUSEON_ENEMYBUILD		6
#define MOUSEON_NEUTRALBUILD	7
#define MOUSEON_MAXVALUE		8

#define SELECTIONMOUSE  		0
#define MOUSEILLEGAL			1
#define MOUSEONOBJGREEN 		2
#define MOUSEONOBJRED	 		3
#define MOUSEONOBJYELLOW 		4
#define MOUSEDOWNSCROLL			5
#define MOUSELEFTDOWNSCROLL		6
#define MOUSERIGHTDOWNSCROLL	7
#define MOUSELEFTSCROLL			8
#define MOUSERIGHTSCROLL		9
#define MOUSEUPSCROLL			10
#define MOUSELEFTUPSCROLL		11
#define MOUSERIGHTUPSCROLL		12
#define TARGGREENMOUSE			13
#define TARGNEUTRALMOUSE		14
#define TARGREDMOUSE			15
#define TARGYELLOWMOUSE			16
#define TIMEMOUSE				17
#define NORMALMOUSE				18

#define TYPEOFCURSORS 			19


class HighMouse
{

    struct
    {
		int		posx;
		int		posy;
		int		sizex;
		int		sizey;
		char	buff[MAXMOUSESIZEX*MAXMOUSESIZEY];
    }mousesavedpixels;

public:
    int		MouseType;
    bool	MouseOnBorder;
    struct
    {
		GRPFILE			*mousegrp;
		unsigned char	curentposition;
		unsigned char	maxpositions;
		unsigned char	flag;
    }cursors[TYPEOFCURSORS];

    int  LoadOneCursor(char *filename,int typemouse);
    int  LoadAllCursors(void);
    void UnloadCursors(void);
    void DrawMouse(void);
    void DrawSelectionArea();

};

extern HighMouse highMouse;

#endif /*    _MOUSE_W   */

