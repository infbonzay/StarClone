#include "mousesdl.h"

//==========================
int LowMouse::Init(void)
{
	return(1);
}
//==========================
int LowMouse::GetButtonStatus(void)
{
	return(SDL_GetMouseState(NULL, NULL));
}
//==========================
void LowMouse::GetPos(void)
{
	SDL_GetRelativeMouseState(&DeltaX,&DeltaY);
}
//==========================
void LowMouse::FlushPos(void)
{
	int x,y;
	SDL_GetRelativeMouseState(&x,&y);
}
//==========================
void LowMouse::SetPos(int x,int y)
{
	int delx,dely;
	SDL_WarpMouse(x,y);
	SDL_PumpEvents();
	FlushPos();
}
//==========================
void LowMouse::LowInMoveEvent(MOUSEMOVEEVENTCALLBACK *eventFunc)
{
	MoveEventFunc = eventFunc;
}
//==========================
void LowMouse::LowUnMoveEvent(void)
{
	MoveEventFunc = NULL;
}
//==========================
void LowMouse::LowInClickEvent(MOUSECLICKEVENTCALLBACK *eventFunc)
{
	ClickEventFunc = eventFunc;
}
//==========================
void LowMouse::LowUnClickEvent(void)
{
	ClickEventFunc = NULL;
}

LowMouse lowMouse;

