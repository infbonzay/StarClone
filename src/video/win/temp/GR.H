
#define OSR2_BUILD_NUMBER 1111

typedef struct
{
    int   width;
    int   height;
    int   bpp;
    int   flags;
    int   frequency;
}screen_res_t;

typedef struct
{
    HINSTANCE        hInstance;
    int              iCmdShow;
    HWND             hWnd;
    HDC              hDC;
    HGLRC            hGLRC;
    BOOL             bActive;
    BOOL             bFullScreen;
    ATOM             wndclass;
    WNDPROC          wndproc;
    int              width;
    int              height;
    int              bpp;
    BOOL             bChangeBPP;
    BOOL             bAllowSoft;
    char            *szCmdLine;
    int              argc;
    char           **argv;
    int              glnWidth;
    int              glnHeight;
    float            gldAspect;
    float            NearClip;
    float            FarClip;
    float            fov;
    int              iResCount;
    screen_res_t    *pResolutions;
    int              iVidMode;
	int				 TempraryClose;
}windata_t;

typedef struct
{
    int  width;
    int  height;
    int  bpp;
}devinfo_t;

BOOL CreateMainWindow(int width, int height, int bpp, BOOL fullscreen);
void Cleanup(void);
int ChangeVidMode(int mode);
int FindNextVideoMode(int w,int h,int b);
void BuildModeList(void);
BOOL SetVideoMode(int width, int height, int bpp);
BOOL InitOpenGL(void);
void ShutdownOpenGL(void);
void InitF(void);
void FullNoFullScreen(int mode);
int MakeDefaultVideoMode(void);

extern int ChangeResolution;
extern windata_t   WinData;
extern devinfo_t   DevInfo;
extern DEVMODE         dm;
