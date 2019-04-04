#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <minmax.h>
#include <gl/gl.h>

#include "prg.h"
#include "gr.h"

float		incrmforcursor = 0.02f;
int         IsActive,fullscr;

int			Texture,ss=64,incrm=-1,mousex,mousey,mouseb,oldmx,oldmy;
int			MyMouseCursor;
struct		zz_color cursorblends={1.0f,1.0f,1.0f,1.0f};
//=======================================================
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    MSG         msg;
    HDC         hdc;

    WinData.bAllowSoft   = TRUE;
	fullscr              = TRUE;

	WinData.hInstance    = hInstance;
    WinData.iCmdShow     = iCmdShow;
    WinData.wndproc      = (WNDPROC)WndProc;
    WinData.pResolutions = 0;
    WinData.NearClip     = 2.0f;
    WinData.FarClip      = 512.0f;

    // Build up the video mode list
	BuildModeList();

    // Get the current display device info
	hdc = GetDC( NULL );
    DevInfo.bpp    = GetDeviceCaps(hdc, BITSPIXEL);
    DevInfo.width  = GetSystemMetrics(SM_CXSCREEN);
    DevInfo.height = GetSystemMetrics(SM_CYSCREEN);
    ReleaseDC(NULL, hdc);

    // Set the desired video mode and/or color depth
    
    // Create the main program window, start up OpenGL and create our viewport
    if (CreateMainWindow( 640,480,16,fullscr) != TRUE)
    {
        ChangeDisplaySettings(0, 0);
        MessageBox(NULL, "Unable to create main window.\nProgram will now end.", "FATAL ERROR", MB_OK);
        Cleanup();
        return 0;
    }
    // Load the demonstration texture

    // We're live now
    WinData.bActive = TRUE;
    IsActive        = TRUE;
    // Begin the main program loop
    while (WinData.bActive == TRUE || WinData.TempraryClose == TRUE)
    {
        if (WinData.TempraryClose != TRUE)
		{
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
	            TranslateMessage (&msg);
				DispatchMessage (&msg);
			}
			if (WinData.hGLRC&&IsActive)
			{
				RenderScene();
			}
		}
    }
    if (WinData.bFullScreen == TRUE)
    {
        ShowCursor(TRUE);
    }
    // Release the parameter and video resolution lists
    Cleanup();
    return msg.wParam;
}
//=======================================================
LRESULT CALLBACK WndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
        int vmode;
        switch (iMsg)
        {
        case WM_ACTIVATE:
                if ((LOWORD(wParam) != WA_INACTIVE) && ((HWND)lParam == NULL))
                {
                   if (WinData.bFullScreen)
                   {
                        ShowWindow(WinData.hWnd, SW_RESTORE);//restore
                        SetForegroundWindow(WinData.hWnd);
                        ChangeDisplaySettings( &dm, CDS_FULLSCREEN );//vostanoviti nash rejim
                   }
                   IsActive=TRUE;
                }
                else
                {
                   if (WinData.bFullScreen)
                   {
                        ShowWindow(WinData.hWnd, SW_MINIMIZE);//minimize
                        ChangeDisplaySettings( 0,0 );//vosstanoviti rejim desktopa
                   }
                   IsActive=FALSE;
                }
             return 0;
        case WM_KEYDOWN:
           if (wParam==VK_ESCAPE)
           {
              SendMessage(WinData.hWnd, WM_CLOSE, 0, 0);
           }
           else
           switch(wParam)
           {
              case VK_F2:
					fullscr = 1 - fullscr;
					FullNoFullScreen(fullscr);
					break;
              case VK_F1:
				   if (WinData.bFullScreen)
				   {
                       vmode=FindNextVideoMode(0,0,32);//find next video resolution with 32bits
					   if (vmode==-2)
							return 0;
					   if (vmode!=-1)
					   {
							MoveWindow(WinData.hWnd,0,0,WinData.pResolutions[vmode].width,
                                       WinData.pResolutions[vmode].height,FALSE);
							ChangeResolution=TRUE;
							ChangeVidMode(vmode);
							glViewport(0,0,WinData.width,WinData.height);
							ChangeResolution=FALSE;
					   }
                  }
           }
           return 0;

		case WM_LBUTTONDOWN:
			mouseb = 1;        // key flags 
			break;		
		case WM_LBUTTONUP:
			mouseb = 0;
			break;		
		case WM_MOUSEMOVE:
			mousex = LOWORD(lParam);  // horizontal position of cursor 
			mousey = HIWORD(lParam);  // vertical position of cursor 
			break;
        case WM_CLOSE:
			ShutdownOpenGL();
			WinData.bActive = FALSE;
			break;
        case WM_DESTROY:
			PostQuitMessage (0);
            return 0;
    }
    return DefWindowProc (hwnd, iMsg, wParam, lParam);
}
//========================================
void InitF(void)//pri smene rejima ustanoviti snova moi parametri GL
{
		glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);
		LoadTexture();
		LoadMyCursor();
}
//=======================================================
void PutTexture(int x,int y,int sx,int sy,int T,struct zz_color *blendcolor)
{
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, T);
    glColor4f( blendcolor->red, blendcolor->green,blendcolor->blue ,blendcolor->alpha);

    glBegin(GL_TRIANGLE_STRIP);

    glTexCoord2f( 0.0f, 1.0f);
    glVertex2i( x , WinData.height - y);

    glTexCoord2f( 1.0f, 1.0f);
    glVertex2i( x + sx, WinData.height - y);

    glTexCoord2f( 0.0f, 0.0f);
    glVertex2i( x, WinData.height - (y + sy));

    glTexCoord2f( 1.0f, 0.0f);
    glVertex2i( x + sx, WinData.height - (y + sy));

    glEnd();
}
//=======================================================
// Render your scenes through this function
void RenderScene()
{
	char ssss[200];
	struct zz_color blends={1.0f,1.0f,1.0f,0.5f};
	int i=0,j=0;
	int x1,y1,x2,y2;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set up a projection matrix to fill the client window
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

//    for (i=0;i<8;i++,j++)
//          for (j=0;j<10;j++)
//                      PutTexture(j*32,i*32,ss,ss,T,&blends);
        PutTexture(0  ,  0, 128, 128,Texture,&blends);
/*                      PutTexture(200,  0,-64, 64,T,&blends);
                        PutTexture(0  ,200, 64,-64,T,&blends);
                        PutTexture(200,200,-64,-64,T,&blends);
      ss=ss+incrm;
        if (ss>64)
                incrm=-1;
        if (ss<-64)
                incrm=1;
*/  
	if (mouseb)
	{
/*		x1 = min( oldmx, mousex );
		x2 = max( oldmx, mousex );
		y1 = min( oldmy, mousey );
		y2 = max( oldmy, mousey );
*/
		x1 = oldmx;
		x2 = mousex;
		y1 = oldmy;
		y2 = mousey;

		PutTexture(x1,y1,x2-x1,y2-y1,Texture,&blends);
	}
	else
	{
		oldmx = mousex;
		oldmy = mousey;
	}
	
	
	PutTexture(mousex,mousey,32,32,MyMouseCursor,&cursorblends);
	cursorblends.alpha += incrmforcursor;
	if (cursorblends.alpha > 1.0)
	{
		cursorblends.alpha	= 1.0;
		incrmforcursor		= - incrmforcursor;
	}
	if (cursorblends.alpha < 0.0)
	{
		cursorblends.alpha = 0.0;
		incrmforcursor		= - incrmforcursor;
	}
	glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho(0,WinData.width,0,WinData.height,-1.0f,1.0f);
    sprintf(ssss,"%dx%dx%d %d %d  ",WinData.width,WinData.height,WinData.bpp,mousex,mousey);
    TextOut(WinData.hDC,0,0,ssss,strlen(ssss));
    SwapBuffers(WinData.hDC);
}
//=======================================================
void LoadTexture(void)
{
        int  i,j;
        FILE *f;
        char *rawbuff;
        rawbuff = (unsigned char *)malloc(64*64*4);
        f=fopen("1.raw","rb");
        if (f)
        {
                fread(rawbuff,64,64*3,f);
                fclose(f);
        }
        else
        {
                for (i=0;i<64;i++)
                        for (j=0;j<64;j++)
                        {
                                rawbuff[(i*64+j)*3+0]=rand()%256;
                                rawbuff[(i*64+j)*3+1]=rand()%256;
                                rawbuff[(i*64+j)*3+2]=rand()%256;
                        }
        }
        Texture=1;
        glBindTexture(GL_TEXTURE_2D, Texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, rawbuff);
        free(rawbuff);
}
//=======================================================
void LoadMyCursor(void)
{
    char *adrcurs;
	int i;
	char c;
	FILE *f;
    adrcurs = (unsigned char *)malloc(32*32*4);
	f=fopen("1.cur","rb");
	if (f)
	{
		MyMouseCursor=2;
		for (i=0;i<32*32;i++)
		{
			fread(adrcurs+i*4,1,4,f);
			c=adrcurs[i*4+0];
			adrcurs[i*4+0]=adrcurs[i*4+2];
			adrcurs[i*4+2]=c;
		}
		fclose(f);
	}
	glBindTexture(GL_TEXTURE_2D, MyMouseCursor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, 32, 32, 0, GL_RGBA, GL_UNSIGNED_BYTE, adrcurs);
	free(adrcurs);

}
//=======================================================
