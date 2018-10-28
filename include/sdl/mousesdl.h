#if !defined(_SDLMOUSE_W)
#define _SDLMOUSE_W 1

#include <SDL/SDL.h>
#define WMLEFTKEY  (SDL_BUTTON(1))
#define WMRIGHTKEY (SDL_BUTTON(3))

typedef void (*PMOUSEMOVEEVENTCALLBACK)(int ,int );
typedef void (*PMOUSECLICKEVENTCALLBACK)(int);
typedef void (*PMOUSEDBLCLICKEVENTCALLBACK)(int);

class LowMouse
{
public:
    int  DeltaX,DeltaY;

    int  Init(void);
    int  GetButtonStatus(void);
    void GetPos(void);
    void FlushPos(void);
    void SetPos(int x,int y);

    PMOUSEMOVEEVENTCALLBACK		MoveEventFunc;
    PMOUSECLICKEVENTCALLBACK	ClickEventFunc;
    PMOUSECLICKEVENTCALLBACK	DblClickEventFunc;

    void InstallMoveEvent(PMOUSEMOVEEVENTCALLBACK pFunc);
    void UninstallMoveEvent(void);
    void InstallClickEvent(PMOUSECLICKEVENTCALLBACK pFunc);
    void UninstallClickEvent(void);
    void InstallDblClickEvent(PMOUSEDBLCLICKEVENTCALLBACK pFunc);
    void UninstallDblClickEvent(void);

};

extern LowMouse lowMouse;


#endif

