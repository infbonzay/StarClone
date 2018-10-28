#include "SDL/SDL.h"
#include "grsdl.h"
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
void LowMouse::InstallMoveEvent(PMOUSEMOVEEVENTCALLBACK eventFunc)
{
    MoveEventFunc = eventFunc;
}
//==========================
void LowMouse::UninstallMoveEvent(void)
{
    MoveEventFunc = NULL;
}
//==========================
void LowMouse::InstallClickEvent(PMOUSECLICKEVENTCALLBACK eventFunc)
{
    ClickEventFunc = eventFunc;
}
//==========================
void LowMouse::UninstallClickEvent(void)
{
    ClickEventFunc = NULL;
}
//==========================
void LowMouse::InstallDblClickEvent(PMOUSEDBLCLICKEVENTCALLBACK eventFunc)
{
    DblClickEventFunc = eventFunc;
}
//==========================
void LowMouse::UninstallDblClickEvent(void)
{
    DblClickEventFunc = NULL;
}
//==========================

LowMouse lowMouse;

