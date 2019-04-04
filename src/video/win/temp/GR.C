#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <gl/gl.h>
#include "gr.h"

char  szAppName[] = "glDEMO";
int ChangeResolution;

windata_t   WinData;
devinfo_t   DevInfo;
DEVMODE         dm;

//==============================================================
BOOL CreateMainWindow(int width, int height, int bpp, BOOL fullscreen)
{
    HWND        hwnd;
    WNDCLASSEX  wndclass;
    DWORD       dwStyle, dwExStyle;
    int         x, y, sx, sy;

    if ((hwnd = FindWindow(szAppName, szAppName)) != NULL)
    {
        SetForegroundWindow(hwnd);
        return FALSE;
    }
	if (fullscreen)
	{
		if ( !SetVideoMode( width, height, bpp) )
		{
			Cleanup();
			return FALSE;
		}
	}


    wndclass.cbSize        = sizeof (wndclass);
    wndclass.style         = 0;
    wndclass.lpfnWndProc   = (WNDPROC)WinData.wndproc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = WinData.hInstance;
    wndclass.hIcon         = 0;
    wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH )COLOR_GRAYTEXT;
    wndclass.lpszMenuName  = NULL;
    wndclass.lpszClassName = szAppName;
    wndclass.hIconSm       = 0;


    if (WinData.TempraryClose == TRUE)
		if (!UnregisterClass (szAppName,WinData.hInstance))
		{
			MessageBox(NULL, "Window class unregistration failed.", "FATAL ERROR", MB_OK);
		}
	if (!RegisterClassEx (&wndclass))
	{
		MessageBox(NULL, "Window class registration failed.", "FATAL ERROR", MB_OK);
		return FALSE;
	}

	x = 0;
	y = 0;
    sx = width;
    sy = height;

	if (fullscreen)
       {
        dwExStyle = WS_EX_TOPMOST;
        dwStyle   = WS_POPUP | WS_VISIBLE;
       }
    else
    {
		dwExStyle = 0;
        dwStyle = WS_CAPTION | WS_SYSMENU ;  // Use this if you want a "normal" window
	}
    if ((hwnd = CreateWindowEx (dwExStyle,
                    szAppName,               // window class name
                    "glDemo",                // window caption
                    dwStyle | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, // window style
                    x,           // initial x position
                    y,           // initial y position
                    sx,           // initial x size
                    sy,           // initial y size
                    NULL,                    // parent window handle
                    NULL,                    // window menu handle
                    WinData.hInstance,       // program instance handle
                            NULL))                   // creation parameters
                    == NULL)
        {
         ChangeDisplaySettings(0, 0);
         MessageBox(NULL, "Window creation failed.", "FATAL ERROR", MB_OK);
         return FALSE;
        }

    WinData.hWnd = hwnd;
    if (!InitOpenGL())
       {
        WinData.hWnd = NULL;
        return FALSE;
       }
	ShowWindow(WinData.hWnd, WinData.iCmdShow);
	UpdateWindow(WinData.hWnd);

	SetForegroundWindow(WinData.hWnd);
	SetFocus(WinData.hWnd);
	
	ShowCursor(FALSE);
	
/*	if (WinData.bFullScreen)
		ShowCursor(FALSE);
	else
		ShowCursor(TRUE);
*/
    return TRUE;

}
//==============================================================
void Cleanup(void)
{
    if (WinData.pResolutions)
        free(WinData.pResolutions);
}
//==============================================================
int ChangeVidMode(int mode)
{
        if (WinData.iVidMode!=mode)
        {
                WinData.iVidMode = mode;
                memset(&dm, 0, sizeof(dm));
                dm.dmSize = sizeof(dm);
                dm.dmPelsWidth  = WinData.pResolutions[mode].width;
                WinData.width   = WinData.pResolutions[mode].width;
                dm.dmPelsHeight = WinData.pResolutions[mode].height;
                WinData.height  = WinData.pResolutions[mode].height;
                WinData.bpp     = WinData.pResolutions[mode].bpp;
                dm.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT;
                dm.dmBitsPerPel = WinData.pResolutions[mode].bpp;
                dm.dmFields    |= DM_BITSPERPEL;
                ShutdownOpenGL();
                InitOpenGL();
                WinData.gldAspect = (float)((GLdouble)WinData.width/(GLdouble)WinData.height);
                return(ChangeDisplaySettings( &dm, CDS_FULLSCREEN ));
        }
        return(DISP_CHANGE_SUCCESSFUL);
}
//==============================================================
int FindNextVideoMode(int w,int h,int b)
{
        static int mode;
        int i,j=0;
        do{
                for (i = mode; i < WinData.iResCount; i++)
                        if ((WinData.pResolutions[i].width == w||(!w)) &&
                                (WinData.pResolutions[i].height == h||(!h)) &&
                                (WinData.pResolutions[i].bpp == b||(!b)))
                        {
                                mode=i+1;
                                return(i);
                        }
                j++;
                mode=0;
        }while(j==1);
        return(-1);
}
//==============================================================
void FullNoFullScreen(int mode)
{
	WinData.bFullScreen   = mode;
	ChangeResolution      = TRUE;
	WinData.TempraryClose = TRUE;
    SendMessage(WinData.hWnd, WM_CLOSE, 0, 0);
	ChangeDisplaySettings( 0 , 0);

	if (CreateMainWindow( WinData.width, WinData.height, WinData.bpp, WinData.bFullScreen) != TRUE)
	{
		ChangeDisplaySettings(0, 0);
        MessageBox(NULL, "Unable to create main window.\nProgram will now end.", "FATAL ERROR", MB_OK);
        Cleanup();
		WinData.TempraryClose = FALSE;
		ChangeResolution      = FALSE;
		WinData.bActive       = FALSE;
		return;
	}
	WinData.TempraryClose = FALSE;
	ChangeResolution      = FALSE;
    WinData.bActive       = TRUE;

    SetForegroundWindow(WinData.hWnd);
	SetFocus(WinData.hWnd);
 	MoveWindow(WinData.hWnd,0,0,WinData.width,WinData.height,TRUE);
	ShowWindow(WinData.hWnd, SW_RESTORE);

}
//==============================================================

// This function builds a list the screen resolutions supported by the display driver
//==============================================================
void BuildModeList(void)
{
    DEVMODE  dm;
    int      mode;
    mode = 0;
    while(EnumDisplaySettings(NULL, mode, &dm))
    {
        mode++;
    }

    WinData.pResolutions = (screen_res_t *)malloc(sizeof(screen_res_t)*mode);
    mode = 0;
    while(EnumDisplaySettings(NULL, mode, &dm))
    {
        WinData.pResolutions[mode].width = dm.dmPelsWidth;
        WinData.pResolutions[mode].height = dm.dmPelsHeight;
        WinData.pResolutions[mode].bpp = dm.dmBitsPerPel;
        WinData.pResolutions[mode].flags = dm.dmDisplayFlags;
        WinData.pResolutions[mode].frequency = dm.dmDisplayFrequency;
        mode++;
    }
    WinData.iResCount = mode;
}

//==============================================================
BOOL SetVideoMode(int width, int height, int bpp)
{
    OSVERSIONINFO   vinfo;
    int             mode;

	WinData.width			= width;
	WinData.height			= height;
	WinData.bpp				= bpp;
	WinData.bFullScreen		= bpp;
    
	vinfo.dwOSVersionInfoSize = sizeof(vinfo);

    WinData.bChangeBPP = FALSE;

    if ( GetVersionEx( &vinfo) )
    {
        if ( vinfo.dwMajorVersion > 4 )
        {
            WinData.bChangeBPP = TRUE;
        }
        else
        if ( vinfo.dwMajorVersion == 4 )
        {
            if ( vinfo.dwPlatformId == VER_PLATFORM_WIN32_NT )
            {
                WinData.bChangeBPP = TRUE;
            }
            else
            if ( vinfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS )
            {
                if ( LOWORD( vinfo.dwBuildNumber ) >= OSR2_BUILD_NUMBER )
                {
                    WinData.bChangeBPP = TRUE;
                }
            }
        }
    }
    else
    {
        MessageBox(NULL, "SetVideoMode - GetVersionEx failed\n", "FATAL ERROR", MB_OK);
        return FALSE;
    }

    if (WinData.bFullScreen)
    {
        if ((WinData.bChangeBPP == FALSE) && (DevInfo.bpp != WinData.bpp))
        {
            MessageBox(NULL, "This version of Windows cannot change color depth.\n"
                             "Please request different video mode settings or adjust\n"
                             "your desktop color depth.", "FATAL ERROR", MB_OK);
            return FALSE;
        }
        mode=FindNextVideoMode(width,height,bpp);
        if (mode!=-1)
        {
           WinData.iVidMode = -1;
           if (ChangeVidMode(mode) != DISP_CHANGE_SUCCESSFUL )
           {
               MessageBox(NULL, "SetVideoMode - ChangeDisplaySettings failed.\n"
                                "Switching to windowed mode.", "WARNING", MB_OK);
               WinData.bFullScreen = FALSE;
           }
           return TRUE;
        }
        MessageBox(NULL, "Your requested video mode is unavailable.\n"
                         "Please request different video mode settings.", "FATAL ERROR", MB_OK);
        return FALSE;
    }
    else
    {
        if (DevInfo.bpp != WinData.bpp)
        {
            MessageBox(NULL, "Your requested color depth and desktop do not match.\n"
                             "Using your current desktop color depth.", "WARNING", MB_OK);
        }
    }
    return TRUE;
}
//==============================================================
BOOL InitOpenGL(void)
{
    int     pfm;   // pixel format
    RECT    rect;

    PIXELFORMATDESCRIPTOR pfd =
        {
         sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
                1,                       // version number
                PFD_DRAW_TO_WINDOW |     // support window
                PFD_SUPPORT_OPENGL |     // support OpenGL
                PFD_DOUBLEBUFFER,        // double buffered
                PFD_TYPE_RGBA,           // RGBA type
                24,                      // 24-bit color depth
                0, 0, 0, 0, 0, 0,        // color bits ignored
                0,                       // no alpha buffer
                0,                       // shift bit ignored
                0,                       // no accumulation buffer
                0, 0, 0, 0,              // accum bits ignored
                32,                      // 32-bit z-buffer
                0,                       // no stencil buffer
                0,                       // no auxiliary buffer
                PFD_MAIN_PLANE,          // main layer
                0,                       // reserved
                0, 0, 0                  // layer masks ignored
        };
    if ((WinData.hDC = GetDC(WinData.hWnd)) == NULL)
    {
        ChangeDisplaySettings(0, 0);
        MessageBox(NULL, "GetDC on main window failed", "FATAL ERROR", MB_OK);
        return FALSE;
    }

    if ((pfm = ChoosePixelFormat(WinData.hDC, &pfd)) == 0)
    {
        ChangeDisplaySettings(0, 0);
        MessageBox(NULL, "ChoosePixelFormat failed\n", "FATAL ERROR", MB_OK);
        return FALSE;
    }
    if (SetPixelFormat(WinData.hDC, pfm, &pfd) == FALSE)
    {
        ChangeDisplaySettings(0, 0);
        MessageBox(NULL, "SetPixelFormat failed\n", "FATAL ERROR", MB_OK);
        return FALSE;
    }
    DescribePixelFormat(WinData.hDC, pfm, sizeof(pfd), &pfd);

    if (!(pfd.dwFlags & PFD_GENERIC_ACCELERATED)) // Incorrectly reported on most drivers I've tested
    {
        if (WinData.bAllowSoft == FALSE)
        {
            ChangeDisplaySettings(0, 0);
            MessageBox(NULL, "OpenGL Driver is not accelerated\n", "FATAL ERROR", MB_OK);
            return FALSE;
        }
    }

    if ((WinData.hGLRC = wglCreateContext(WinData.hDC)) == 0)
    {
        ChangeDisplaySettings(0, 0);
        MessageBox(NULL, "wglCreateContext failed\n", "FATAL ERROR", MB_OK);
        goto fail;
    }

    if (!wglMakeCurrent(WinData.hDC, WinData.hGLRC))
    {
        ChangeDisplaySettings(0, 0);
        MessageBox(NULL, "wglMakeCurrent failed\n", "FATAL ERROR", MB_OK);
        goto fail;
    }

    GetClientRect(WinData.hWnd, &rect);
    WinData.glnWidth= rect.right;
    WinData.glnHeight = rect.bottom;
    WinData.gldAspect = (float)((GLdouble)WinData.glnWidth/(GLdouble)WinData.glnHeight);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    InitF();
    return TRUE;

fail:
    if ( WinData.hGLRC )
    {
        wglDeleteContext( WinData.hGLRC);
        WinData.hGLRC = NULL;
    }

    if ( WinData.hDC )
    {
        ReleaseDC(WinData.hWnd, WinData.hDC);
        WinData.hDC = NULL;
    }
    return FALSE;
}
//==============================================================
void ShutdownOpenGL(void)
{
    if (WinData.hGLRC)
    {
        if ((!wglMakeCurrent(NULL, NULL)) && (!WinData.bFullScreen))
        {
            MessageBox(NULL, "ShutdownOpenGL - wglMakeCurrent failed\n", "ERROR", MB_OK);
        }
        if (!wglDeleteContext(WinData.hGLRC))
        {
            MessageBox(NULL, "ShutdownOpenGL - wglDeleteContext failed\n", "ERROR", MB_OK);
        }
        WinData.hGLRC = NULL;
    }
    if (WinData.hDC)
    {
        if (!ReleaseDC(WinData.hWnd, WinData.hDC))
        {
            MessageBox(NULL, "ShutdownOpenGL - ReleaseDC failed\n", "ERROR", MB_OK);
        }
        WinData.hDC   = NULL;
    }
    if (!ChangeResolution)
       if (WinData.bFullScreen)
       {
           ChangeDisplaySettings( 0, 0 );
       }
}
//==============================================================



