#include "SDL/SDL.h"
#include "grsdl.h"
#include "mousesdl.h"

int WMouseMoveRelativX,WMouseMoveRelativY;
char mousedetect;
//==========================
int mousebuttons(void)
{
//    SDL_PumpEvents();
    return(SDL_GetMouseState(NULL, NULL));
}
//==========================
void GetMouseMove(void)
{
//    SDL_PumpEvents();
    SDL_GetRelativeMouseState(&WMouseMoveRelativX,&WMouseMoveRelativY);
}
//==========================
void SetMousePos(int x,int y)
{
    int delx,dely;
    SDL_WarpMouse(x,y);
    SDL_PumpEvents();
    SDL_GetRelativeMouseState(&delx,&dely);
}
//==========================
int mouseinit(void)
{
    return(1); //mouse exist
}
//==========================
