#if !defined(_SDLMOUSE_W)
#define _SDLMOUSE_W 1

#include <SDL/SDL.h>
#define WMLEFTKEY  (SDL_BUTTON(1))
#define WMRIGHTKEY (SDL_BUTTON(3))

typedef void (MOUSEMOVEEVENTCALLBACK)(int ,int );
typedef void (MOUSECLICKEVENTCALLBACK)(bool, int);
typedef void (MOUSEDBLCLICKEVENTCALLBACK)(void);

class LowMouse
{
public:
	int	 DeltaX,DeltaY;

	int	 Init(void);
	int	 GetButtonStatus(void);
	void GetPos(void);
	void FlushPos(void);
	void SetPos(int x,int y);

	MOUSEMOVEEVENTCALLBACK		*MoveEventFunc;
	MOUSECLICKEVENTCALLBACK		*ClickEventFunc;

	void LowInMoveEvent(MOUSEMOVEEVENTCALLBACK *pFunc);
	void LowUnMoveEvent(void);
	void LowInClickEvent(MOUSECLICKEVENTCALLBACK *pFunc);
	void LowUnClickEvent(void);

};

extern LowMouse lowMouse;


#endif

