#include <windows.h>
#include <grplib/grp.h>

#include "vars.h"
#include "const.h"
#include "mytime.h"
#include "rand.h"
#include "Controller.h"


Controller mainController;

//==========================
int Controller::Init(void)
{
	Surface = new Controller_Surface();
	Surface->hInstance = 0;
	Surface->iCmdShow = SW_SHOW;

	KeysBuffer = new mycycle<uint16_t>(16, MYCYCLE_INFINITE);
	return(0);
}
//===========================================
void Controller::DeInit(void)
{
	if (KeysBuffer)
	{
		delete KeysBuffer;
		KeysBuffer = NULL;
	}
	if (Surface)
	{
		delete Surface;
		Surface = NULL;
	}
}
//===========================================
void Controller::SetWindowName(const char *winName)
{
    SetWindowTextA(Surface->window, winName);
}
//===========================================
LRESULT CALLBACK WndProcFunc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_ACTIVATE:
		if ((LOWORD(wParam) != WA_INACTIVE) && ((HWND)lParam == NULL))
		{
			gameconf.grmode.flags |= DISPLAYFLAGS_WINDOWACTIVE;
		}
		else
		{
			gameconf.grmode.flags &= ~DISPLAYFLAGS_WINDOWACTIVE;
		}
		return 0;
	case WM_KEYDOWN:
		mainController.KeyActive = wParam;
		mainController.LastKey = wParam;
		mainController.SetKeyMod( wParam, true);
		if (mainController.KeyActive)
			mainController.KeysBuffer->PushElem(mainController.KeyActive);
		return 0;
	case WM_KEYUP:
		mainController.KeyActive = 0;
		mainController.SetKeyMod( wParam, false);
		return 0;
	case WM_LBUTTONDOWN:
		if (highMouse->ClickFunc)
			(*highMouse->ClickFunc)(true, WMLEFTKEY);
		break;
	case WM_LBUTTONUP:
		if (highMouse->ClickFunc)
			(*highMouse->ClickFunc)(false, WMLEFTKEY);
		break;
	case WM_RBUTTONDOWN:
		if (highMouse->ClickFunc)
			(*highMouse->ClickFunc)(true, WMRIGHTKEY);
		break;
	case WM_RBUTTONUP:
		if (highMouse->ClickFunc)
			(*highMouse->ClickFunc)(false, WMRIGHTKEY);
		break;
	case WM_MOUSEMOVE:
		highMouse->PosX = LOWORD(lParam);
		highMouse->PosY = HIWORD(lParam);
		if (highMouse->MoveFunc)
			(*highMouse->MoveFunc)(highMouse->PosX, highMouse->PosY);
		break;
	case WM_CLOSE:
		mainController.KeyFlags |= CFLAG_QUIT;
		return (0);
		break;
	case WM_DESTROY:
		mainController.KeyFlags |= CFLAG_QUIT;
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}
//========================================
int Controller::QueryVideoMode(int x, int y, int bpp, int fullscreen)
{
	HDC	hdc;
	bool first = false;
	Surface->FullScreen = fullscreen;
	if (!Surface->window)
	{
		first = true;
		hdc = GetDC( NULL );
		Surface->SavedBpp = GetDeviceCaps(hdc, BITSPIXEL);
		ReleaseDC(NULL, hdc);
		Surface->SavedWidth = GetSystemMetrics(SM_CXSCREEN);
		Surface->SavedHeight = GetSystemMetrics(SM_CYSCREEN);
		//save current screen resolution
		SaveDesktopResolution(Surface->SavedWidth, Surface->SavedHeight);
		Surface->DesiredBpp = bpp;

		Surface->width = x;
		Surface->height = y;

		Surface->pixelsBufferSize = x * y * bpp / 8;
		Surface->pixels = (uint8_t *) wmalloc(Surface->pixelsBufferSize);

		Surface->palette = (uint8_t *)wmalloc(256 * 4);
		memset(Surface->palette, 0, 256 * 4);

		gameconf.grmode.videobuff = Surface->pixels;
		GRP_SetVideoBuffer(Surface->pixels);
		if (Surface->flags & CFLAG_EXACTBPP)
		{
			Surface->Xpixels = Surface->pixels;
			Surface->XpixelsBufferSize = Surface->pixelsBufferSize;
		}
		else
		{
			Surface->XpixelsBufferSize = x * y * Surface->ximagebpp[(Surface->SavedBpp + 1) / 8] / 8;
			Surface->Xpixels = (uint8_t *) wmalloc(Surface->XpixelsBufferSize);
			printf("Launching in emulated mode %dx%dx%d\n", x, y, Surface->SavedBpp);
		}
		

		WNDCLASSEX  wndclass;

		wndclass.cbSize = sizeof(wndclass);
		wndclass.style = 0;
		wndclass.lpfnWndProc = (WNDPROC) WndProcFunc;
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = 0;
		wndclass.hInstance = Surface->hInstance;
		wndclass.hIcon = 0;
		wndclass.hCursor = 0;
		wndclass.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = "StarCloneWClass";
		wndclass.hIconSm = 0;
		
		if (!RegisterClassEx(&wndclass))
		{
			return 0;
		}
		if ((Surface->window = CreateWindowEx (0,
                    "StarCloneWClass",               
                    "StarClone",                
                    WS_CAPTION | WS_SYSMENU | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                    0,           
                    0,           
                    x,           
                    y,           
                    NULL,                    
                    NULL,                    
					Surface->hInstance,
                    NULL))                   
                    == NULL)
		{
			return 0;
		}

		Surface->hdc = GetDC(Surface->window);

		BITMAPINFO bmi = { 0 };
		bmi.bmiHeader.biSize = sizeof(BITMAPCOREHEADER);
		bmi.bmiHeader.biWidth = Surface->width;
		bmi.bmiHeader.biHeight = -Surface->height;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = Surface->SavedBpp;
		bmi.bmiHeader.biCompression = BI_RGB;

		Surface->backDC = CreateCompatibleDC(Surface->hdc);
		Surface->backBitmap = CreateCompatibleBitmap(Surface->hdc, Surface->width, Surface->height); 
		SelectObject(Surface->backDC, Surface->backBitmap);
		ReleaseDC(Surface->window, Surface->hdc);

		SetForegroundWindow(Surface->window);
		ShowWindow(Surface->window, Surface->iCmdShow);
		UpdateWindow(Surface->window);
		SetFocus(Surface->window);
		HideCursor();

		gameconf.grmode.flags |= DISPLAYFLAGS_WINDOWACTIVE;
	}
	return(1);
}
//===========================================
void Controller::QuitVideoMode(void)
{
	int xres,yres;
	if (Surface && Surface->window)
	{
		DestroyWindow(Surface->window);
		Surface->window = NULL;
		if (Surface->palette)
		{
			wfree(Surface->palette);
			Surface->palette = NULL;
		}
		if (Surface->flags & CFLAG_EXACTBPP)
		{
			Surface->Xpixels = NULL;
		}
		if (Surface->pixels)
		{
			wfree(Surface->pixels);
			Surface->pixels = NULL;
		}
		if (Surface->Xpixels)
		{
			wfree(Surface->Xpixels);
			Surface->Xpixels = NULL;
		}
		if (Surface->FullScreen)
		{
			DesktopResolution(&xres,&yres);
			//SetVideoMode(xres,yres);
		}
		DeleteDC(Surface->hdc);
		DeleteDC(Surface->backDC);
		DeleteObject(Surface->backBitmap);
		ShowCursor();
	}
}
//===========================================
int Controller::ModifyVideoMode(int x, int y, int bpp, int fullscreen, unsigned char *palette)
{
	return 0;
}
//===========================================
void Controller::UpdateScreenRegions(int nrregions,SCREEN_REGION regions[])
{
	int i, nodraw = 1;
	mytimer.CallTimer(MYTIMER_SINCHRONMODE);
	if (gameconf.grmode.flags & DISPLAYFLAGS_WINDOWACTIVE)
	{
		for (i = 0; i < nrregions; i++)
		{
			TransformPixels(regions[i].x, regions[i].y, regions[i].w, regions[i].h);
			SetBitmapBits(Surface->backBitmap, Surface->XpixelsBufferSize, Surface->Xpixels );
			BitBlt(GetDC(Surface->window), regions[i].x, regions[i].y,
								 regions[i].w, regions[i].h,
								 Surface->backDC, 0, 0, SRCCOPY);
		}
		nodraw = 0;
	}
	if (nodraw || gameconf.speedconf.cputhrottling)
		usleep(4000);
}
//==========================
void Controller::UpdateScreenRegion(int x, int y, int xsize, int ysize)
{
	SCREEN_REGION scrpart;
	scrpart.x = x;
	scrpart.y = y;
	scrpart.w = xsize;
	scrpart.h = ysize;
	UpdateScreenRegions(1, &scrpart);
}
//===========================================
void Controller::UpdateScreen(void)
{
	UpdateScreenRegion(0, 0, gameconf.grmode.x, gameconf.grmode.y);
}
//===========================================
void Controller::ApplyPalette(unsigned char *pal4,int from,int count)
{
	if (Surface->DesiredBpp != 8)
		return;
	uint16_t *palette16 = ( (uint16_t *) Surface->palette) + from;
	uint16_t palcol16;
	uint32_t *palette32 = ( (uint32_t *) Surface->palette) + from;
	uint32_t palcol32;
	switch(Surface->SavedBpp)
	{
	case 8:
		return;
	case 15:
		do{
			palcol16 = (( (*pal4++) >> 3) << 10) | (( (*pal4++) >> 3) << 5) | (( (*pal4++) >> 3) );
			pal4++;
			*palette16++ = palcol16;
		}while(--count);
		break;
	case 16:
		do{
			palcol16 = (( (*pal4++) >> 3) << 11) | (( (*pal4++) >> 2) << 5) | (( (*pal4++) >> 3) );
			pal4++;
			*palette16++ = palcol16;
		}while(--count);
		break;
	case 24:
	case 32:
		do{
			palcol32 = ( (*pal4++) << 16) | ( (*pal4++) << 8) | ( (*pal4++) );
			pal4++;
			*palette32++ = palcol32;
		}while(--count);
		break;
	}
	UpdateScreen();
}
//==========================
void Controller::ApplyPalette(unsigned char *pal4)
{
	ApplyPalette(pal4, 0, 256);
}
//==========================
void Controller::ApplyPalette256x3(unsigned char *pal3)
{
	int i;
	unsigned char pal4[4 * 256];
	unsigned char *pal4p = pal4;
	*pal4p++ = 0;
	*pal4p++ = 0;
	*pal4p++ = 0;
	*pal4p++ = 0;
	pal3 += 3;
	for (i = 1;i < 255;i++)
	{
		*pal4p++ = *pal3++;
		*pal4p++ = *pal3++;
		*pal4p++ = *pal3++;
		*pal4p++ = 0;
	}
	*pal4p++ = 255;
	*pal4p++ = 255;
	*pal4p++ = 255;
	*pal4p++ = 0;

	ApplyPalette(pal4);
}
//===========================================
int  Controller::EventsLoop(void)
{
	while (PeekMessage(&Surface->msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&Surface->msg);
		DispatchMessage(&Surface->msg);
	}
	if (KeyFlags & CFLAG_QUIT)
		return 1;
	return(0);
}
//===========================================
void Controller::SetKeyMod(int keysym,bool status)
{
	switch(keysym)
	{
	case SHIFTLKEY:
	case SHIFTRKEY:
		if (status)
			KeyFlags |= SHIFTKEYMASK;
		else
			KeyFlags &= ~SHIFTKEYMASK;
		break;
	case CTRLLKEY:
	case CTRLRKEY:
		if (status)
			KeyFlags |= CTRLKEYMASK;
		else
			KeyFlags &= ~CTRLKEYMASK;
		break;
	}
}
//===========================================
bool Controller::SetVideoMode(int x, int y)
{
	return false;
}
//===========================================
void Controller::TransformPixels(int x, int y, int sizex, int sizey)
{
	if (Surface->DesiredBpp != 8)
		return;
	switch (Surface->SavedBpp)
	{
	case 8:
		return;
	case 15:
	case 16:
		Transform<uint16_t>(x, y, sizex, sizey);
		break;
	case 24:
	case 32:
		Transform<uint32_t>(x, y, sizex, sizey);
		break;
	default:
		DEBUGMESSCR("Transformation from %d bpp to %d bpp not implemented\n", Surface->DesiredBpp, Surface->SavedBpp);
		return;
	}
}
//===========================================
template <typename T>
	void Controller::Transform(int x, int y, int sizex, int sizey)
{
	int i, j;
	int deltaincr = Surface->width - sizex;
	T *paladr = (T *)Surface->palette;
	T *Xbuf = (T *)Surface->Xpixels + y * Surface->width + x;
	uint8_t *buf = Surface->pixels + y * Surface->width + x;
	i = sizey;
	do{
		j = sizex;
		do{
			*Xbuf++ = paladr[*buf++];
		}while(--j);
		Xbuf += deltaincr;
		buf += deltaincr;
	}while(--i);
}
//===========================================
void Controller::HideCursor(void)
{
	::ShowCursor(FALSE);
}
//===========================================
void Controller::ShowCursor(void)
{
	::ShowCursor(TRUE);
}
//===========================================
void Controller::SaveDesktopResolution(int x,int y)
{
	Surface->SavedWidth  = x;
	Surface->SavedHeight = y;
}
//===========================================
void Controller::DesktopResolution(int *x,int *y)
{
	*x = Surface->SavedWidth;
	*y = Surface->SavedHeight;
}
//===========================================
bool Controller::CanFullScreen(void)
{
	return false;
}
//===========================================
