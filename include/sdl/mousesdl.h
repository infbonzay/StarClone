#if !defined(_SDLMOUSE_W)
#define _SDLMOUSE_W 1

#define WMLEFTKEY  (SDL_BUTTON(1))
#define WMRIGHTKEY (SDL_BUTTON(3))

class lowmouse
{
public:
    int DeltaX,DeltaY;

    int Init(void);
    int GetButtonStatus(void);
    void GetPos(void);
    void FlushPos(void);
    void SetPos(int x,int y);
};

extern lowmouse LowMouse;


#endif

