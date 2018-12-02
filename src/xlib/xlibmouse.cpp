
#include <X11/Xlib.h>
#include "LowMouse.h"

//==========================
int LowMouse::Init(void)
{
	return(1);
}
//==========================
int LowMouse::GetButtonStatus(void)
{
	return(0);
}
//==========================
void LowMouse::GetPos(void)
{
}
//==========================
void LowMouse::FlushPos(void)
{
}
//==========================
void LowMouse::SetPos(int x,int y)
{
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


