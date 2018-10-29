#if !defined(_MOUSENEW_W)
#define _MOUSENEW_W

#include <grplib/grp.h>

#ifdef WITHSDL
	   #include "sdl/mousesdl.h"
	   #include "sdl/keysdl.h"
#endif

#include "man.h"
#include "const.h"

#define MOUSEMODE1						0
#define MOUSEMODE2						1
#define MOUSEMODE3						2
#define MOUSEMODE4						3
#define MAXMOUSEMODE					4

#define SMOUTHMOUSE						1.0001
#define MOUSEDBLCLICKTIME				8

#define MAXMOUSESIZEX					64
#define MAXMOUSESIZEY					64

#define MOUSEON_NONE					-1
#define MOUSEON_MYUNIT					0
#define MOUSEON_ALLIANCEUNIT			1
#define MOUSEON_ENEMYUNIT				2
#define MOUSEON_NEUTRALUNIT				3
#define MOUSEON_MYBUILD					4
#define MOUSEON_ALLIANCEBUILD			5
#define MOUSEON_ENEMYBUILD				6
#define MOUSEON_NEUTRALBUILD			7
#define MOUSEON_MAXVALUE				8

#define SELECTIONMOUSE					0
#define MOUSEILLEGAL					1
#define MOUSEONOBJGREEN					2
#define MOUSEONOBJRED					3
#define MOUSEONOBJYELLOW				4
#define MOUSEDOWNSCROLL					5
#define MOUSELEFTDOWNSCROLL				6
#define MOUSERIGHTDOWNSCROLL			7
#define MOUSELEFTSCROLL					8
#define MOUSERIGHTSCROLL				9
#define MOUSEUPSCROLL					10
#define MOUSELEFTUPSCROLL				11
#define MOUSERIGHTUPSCROLL				12
#define TARGGREENMOUSE					13
#define TARGNEUTRALMOUSE				14
#define TARGREDMOUSE					15
#define TARGYELLOWMOUSE					16
#define TIMEMOUSE						17
#define NORMALMOUSE						18

#define TYPEOFCURSORS					19

typedef void (MOUSEMOVEFUNC)(int ,int );
typedef void (MOUSECLICKFUNC)(bool, int);
typedef void (MOUSEDBLCLICKFUNC)(void);

class HighMouse
{

public:
	int			PosX;
	int			PosY;
	int			PrevX;
	int			PrevY;
	int			MouseType;
	int			PrevMouseType;

	struct
	{
		int		PosX;
		int		PosY;
		int		SizeX;
		int		SizeY;
		char	SavedPixels[MAXMOUSESIZEX*MAXMOUSESIZEY];
	}SavedUnder;

	MOUSEMOVEFUNC			*MoveFunc;
	MOUSECLICKFUNC			*ClickFunc;
	MOUSEDBLCLICKFUNC		*DblClickFunc;

	int			RestrictNr;
	XY			RestrictXY[MAXMOUSEMODE];
	OBJ			*MouseOnOBJS[MOUSEON_MAXVALUE]; //myunit,alianceunit,neutralunit,enemyunit,mybuild,aliancebuild,neutralbuild,enemybuild
	OBJ			*DestMouseOBJ;
	int			DestMouseType;
	bool		MouseOnBorder;
	bool		MouseOnSelectionMode;
	uint8_t		WaitToPressLeftButton;
	int			MouseButtons;
	struct
	{
		GRPFILE			*mousegrp;
		unsigned char	curentposition;
		unsigned char	maxpositions;
		unsigned char	flag;
	}cursors[TYPEOFCURSORS];

	HighMouse(void);
	~HighMouse(void);
	
	int			LoadOneCursor(char *filename,int typemouse);
	int			LoadAllCursors(void);
	void		UnloadCursors(void);
	void		DrawMouse(void);
	void		DrawSelectionArea(void);
	void		ScrollMouse(void);
	void		MouseOnObjClear(void);
	void		GetMouseOnObj(void);
	void		SaveImageUnder();
	void		LoadImageUnder();
	bool		CheckForBorder(int x1,int y1,int x2,int y2);
	void		SetRestrictCoords(int restrictNr);
	void		SetRestrictCoords(int restrictNr,int x1,int y1,int x2,int y2);
	void		FixRestrict(int *x,int *y);
	void		InstallMoveEvent(MOUSEMOVEFUNC *eventFunc);
	void		UninstallMoveEvent(void);
	void		InstallClickEvent(MOUSECLICKFUNC *eventFunc);
	void		UninstallClickEvent(void);
	void		InstallDblClickEvent(MOUSEDBLCLICKFUNC *eventFunc);
	void		UninstallDblClickEvent(void);
	void		SetPos(int x,int y);

	inline int	GetButtonStatus(void) { return MouseButtons; };

};

void MouseMoveEvent(int x, int y);
void MouseClickEvent(bool type, int buttons);


#endif /*	 _MOUSE_W	*/

