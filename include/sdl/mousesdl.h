#if !defined(_SDLMOUSE_W)
#define _SDLMOUSE_W 1

#define WMLEFTKEY  (SDL_BUTTON(1))
#define WMRIGHTKEY (SDL_BUTTON(3))

int mouseinit(void);        //for dos compatibility
int mousebuttons(void);
void GetMouseMove(void);
void SetMousePos(int x,int y);

extern int WMouseMoveRelativX,WMouseMoveRelativY;
extern  char mousedetect;

#endif

