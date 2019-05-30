#ifndef _SC_MOUSE_H

#define _SC_MOUSE_H

#include <stdint.h>
#include <grplib/grp.h>

#include "man.h"
#include "const.h"
#include "images.h"

#define MOUSEDESTELEVATION      		255

#define MOUSEMODE1						0
#define MOUSEMODE2						1
#define MOUSEMODE3						2
#define MOUSEMODE4						3
#define MAXMOUSEMODE					4

#define SMOUTHMOUSE						(float) 1.0001
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
	int			MouseHotPos;
	
	struct
	{
		int		PosX;
		int		PosY;
		int		SizeX;
		int		SizeY;
		char	SavedPixels[MAXMOUSESIZEX*MAXMOUSESIZEY];
	}SavedUnder;

	struct
	{
		SCUNIT	SC_BuildUnit;
		int8_t	CanDo;
		int		PosX;
		int		PosY;
	}Construct;

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
	bool		DoubleClick;
	uint8_t		WaitToPressLeftButton;
	uint8_t		WaitToReleaseLeftButton;
	uint8_t		WaitToReleaseRightButton;
	int			MouseButtons;
	int			MouseModeMove;
	struct
	{
		GRPFILE			*mousegrp;
		unsigned char	curentposition;
		unsigned char	maxpositions;
	}cursors[TYPEOFCURSORS];

	HighMouse(void);
	~HighMouse(void);


	int			LoadOneCursor(char *filename,int typemouse);
	int			LoadAllCursors(void);
	void		UnloadCursors(void);
	void		DrawMouse(void);
	void		DrawSelectionArea(void);
	void		ScrollMouse(void);
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
	void 		DoRightClickAction(OBJ *destobj,int xm,int ym,int rightclick);
	float		ScrollMapX(int border,float factor);
	float		ScrollMapY(int border,float factor);
	void		RefreshMouseType(int xk,int yk);
	void		MouseOnObjClear(void);

	inline int	GetButtonStatus(void) { return MouseButtons; };

private:
	void		ClearCursors(void);

};

void MouseClickEvent(bool type, int buttons);

class DestCursor
{
private:
	int 		PosX;
	int 		PosY;
	MAIN_IMG 	*MouseDestImg;
public:

	bool		Presence;
	DestCursor();
	~DestCursor();

	void 		SetDestinationCursor(int x,int y);
	void		DrawDestinationCursor();
	void		ExecuteScript();

};

extern HighMouse	*highMouse;
extern DestCursor	*destCursor;


#endif /*	 _MOUSE_W	*/

