#include "SDL/SDL.h"
#include "grsdl.h"
#include "mousesdl.h"

//==========================
int lowmouse::Init(void)
{
    return(1);
}
//==========================
int lowmouse::GetButtonStatus(void)
{
    return(SDL_GetMouseState(NULL, NULL));
}
//==========================
void lowmouse::GetPos(void)
{
    SDL_GetRelativeMouseState(&DeltaX,&DeltaY);
}
//==========================
void lowmouse::FlushPos(void)
{
    int x,y;
    SDL_GetRelativeMouseState(&x,&y);
}
//==========================
void lowmouse::SetPos(int x,int y)
{
    int delx,dely;
    SDL_WarpMouse(x,y);
    SDL_PumpEvents();
    FlushPos();
}
//==========================
lowmouse LowMouse;

