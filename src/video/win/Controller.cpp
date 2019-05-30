#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>

#include "vars.h"
#include "const.h"
#include "mytime.h"
#include "rand.h"
#include "gener.h"
#include "Controller.h"

// include the Direct3D Library file
#pragma comment (lib, "d3d9.lib")


Controller mainController;
static const char WClass[] = "StarCloneWClass";

//===========================================
static bool InitD3D(HWND hWnd, LPDIRECT3D9 *d3d, LPDIRECT3DDEVICE9 *d3ddev, int x, int y, int fullscreen)
{
    *d3d = Direct3DCreate9(D3D_SDK_VERSION);    // create the Direct3D interface

    D3DPRESENT_PARAMETERS d3dpp;    	  				// create a struct to hold various device information

    ZeroMemory(&d3dpp, sizeof(d3dpp));    				// clear out the struct for use
	d3dpp.Windowed = fullscreen;						// program windowed, not fullscreen
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;    		// discard old frames
    d3dpp.hDeviceWindow = hWnd;    						// set the window to be used by Direct3D
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;			// set the back buffer format to 32-bit
	d3dpp.BackBufferWidth = x;							// set the width of the buffer
	d3dpp.BackBufferHeight = y;							// set the height of the buffer
    
														// create a device class using this information and information from the d3dpp stuct
    HRESULT result = (*d3d)->CreateDevice(D3DADAPTER_DEFAULT,
										  D3DDEVTYPE_HAL,
										  hWnd,
									      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
									      &d3dpp,
									      d3ddev);
	return result == D3D_OK;
}
//===========================================
static void CleanD3D(LPDIRECT3D9 d3d, LPDIRECT3DDEVICE9 d3ddev)
{
    if (d3ddev)
		d3ddev->Release();						// close and release the 3D device
    if (d3d)
		d3d->Release();    						// close and release Direct3D
}
//===========================================
static LRESULT CALLBACK WndProcFunc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	int keymod = 0;
	int keyActive;
	switch (iMsg)
	{
	case WM_SYSCOMMAND:
		switch (wParam)
		{
		case SC_MINIMIZE:
			gameconf.grmode.flags &= ~DISPLAYFLAGS_WINDOWACTIVE;
			break;
		case SC_RESTORE:
			gameconf.grmode.flags |= DISPLAYFLAGS_WINDOWACTIVE;
			break;
		case SC_KEYMENU:
			//emulation f10 key because windows sucks
			f10_menu &= ~GAMEBUTTON_KEYPRESS;
			f10_menu |= GAMEBUTTON_KEYRELEASE | GAMEBUTTON_SHOW;
			return 0;
			//break;
		}
		break;
	case WM_KEYDOWN:
		keyActive = wParam;
		mainController.SetKeyMod(keyActive, true);
		if (mainController.KeyFlags & LOCKKEYMASK)
			keymod ^= 1;
		if (mainController.KeyFlags & SHIFTKEYMASK)
			keymod ^= 1;
		if (keyActive >= 'A' && keyActive <= 'Z')
			keyActive += 0x20;
		mainController.KeyActive = keyActive;
		mainController.LastKey = mainController.KeyActive;
		if (mainController.KeyActive)
			mainController.KeysBuffer->PushElem(mainController.KeyActive);
		return 0;
	case WM_KEYUP:
		mainController.KeyActive = 0;
		mainController.SetKeyMod(wParam, false);
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
static DEVMODE *FindVideoMode(int x, int y, int bpp)
{
	DEVMODE *retdm = NULL;
	DEVMODE dm, dmNoBpp;
	int mode = 0;
	bool findNoBpp = false;
	while (EnumDisplaySettings(NULL, mode++, &dm))
	{
		if (dm.dmPelsWidth == x && dm.dmPelsHeight == y)
		{
			if (bpp > 0)
			{
				if (dm.dmBitsPerPel == bpp)
				{
					retdm = (DEVMODE *)wmalloc(sizeof(DEVMODE));
					memcpy(retdm, &dm, sizeof(DEVMODE));
					return retdm;
				}
			}
			else if (!findNoBpp)
			{
				findNoBpp = true;
				memcpy(&dmNoBpp, &dm, sizeof(DEVMODE));
			}
		}
	}
	if (findNoBpp)
	{
		retdm = (DEVMODE *) wmalloc(sizeof(DEVMODE));
		memcpy(retdm, &dmNoBpp, sizeof(DEVMODE));
	}
	return retdm;
}
//===========================================
//===========================================
//===========================================
int Controller::Init(void)
{
	Surface = new Controller_Surface();
	Surface->hInstance = 0;
	Surface->iCmdShow = SW_SHOW;

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
	wndclass.lpszClassName = WClass;
	wndclass.hIconSm = 0;
		
	if (!RegisterClassEx(&wndclass))
	{
		return -1;
	}

	KeysBuffer = new mycycle<uint16_t>(16, MYCYCLE_INFINITE);
	return (0);
}
//===========================================
void Controller::DeInit(void)
{
	UnregisterClass(WClass, Surface->hInstance);
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
int Controller::QueryVideoMode(int x, int y, int bpp, int fullscreen)
{
	HDC		hdc;
	DWORD	dwStyle, dwExStyle;

	Surface->FullScreen = fullscreen;
	bool first = false;
	//save current screen resolution
	if (!Surface->window)
	{
		hdc = GetDC(NULL);
		Surface->SavedBpp = GetDeviceCaps(hdc, BITSPIXEL);
		ReleaseDC(NULL, hdc);
		SaveDesktopResolution(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
	}
	if (!Surface->DevMode)
	{
		Surface->DevMode = FindVideoMode(x, y, bpp);
		if (Surface->DevMode)
		{
			Surface->flags |= CFLAG_EXACTBPP;
		}
		else
		{
			Surface->DevMode = FindVideoMode(x, y, 32);
		}
		if (!Surface->DevMode)
			return 0;
		if (fullscreen)
		{
			ChangeDisplaySettings(Surface->DevMode, CDS_FULLSCREEN);
		}
		wfree(Surface->DevMode);
		Surface->DevMode = NULL;
	}
	if (fullscreen)
	{
		dwExStyle = WS_EX_TOPMOST;
		dwStyle = WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	}
	else 
	{
		dwExStyle = 0;
		dwStyle = WS_CAPTION | WS_SYSMENU | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_MINIMIZEBOX;
		if (!first)
		{
			ChangeDisplaySettings(0, 0);
		}
	}
	if (!Surface->window)
	{
		first = true;
		Surface->DesiredBpp = bpp;
		Surface->width = x;
		Surface->height = y;

		Surface->pixelsBufferSize = x * y * bpp / 8;
		if (!Surface->pixels)
		{
			Surface->pixels = (uint8_t *)wmalloc(Surface->pixelsBufferSize);
		}
		if (!Surface->palette)
		{
			Surface->palette = (uint8_t *)wmalloc(256 * 4);
			memset(Surface->palette, 0, 256 * 4);
		}
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
			if (!Surface->Xpixels)
			{
				Surface->Xpixels = (uint8_t *)wmalloc(Surface->XpixelsBufferSize);
			}
			printf("Launching in emulated mode %dx%dx%d\n", x, y, Surface->SavedBpp);
		}
	
		RECT wr = { 0, 0, x, y };
		AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

		if ((Surface->window = CreateWindowEx(dwExStyle,
			WClass,
			"StarClone",
			dwStyle,
			0,
			0,
			wr.right - wr.left,
			wr.bottom - wr.top,
			NULL,
			NULL,
			Surface->hInstance,
			NULL))
			== NULL)
		{
			return 0;
		}

		if (InitD3D(Surface->window, &Surface->d3d, &Surface->d3ddev, x, y, Surface->FullScreen))
			return (0);

		Surface->hdc = GetDC(Surface->window);

		BITMAPINFO bmi = { 0 };
		bmi.bmiHeader.biSize = sizeof(BITMAPCOREHEADER);
		bmi.bmiHeader.biWidth = Surface->width;
		bmi.bmiHeader.biHeight = Surface->height;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = Surface->SavedBpp;
		bmi.bmiHeader.biCompression = BI_RGB;

		Surface->backDC = CreateCompatibleDC(Surface->hdc);
		Surface->backBitmap = CreateCompatibleBitmap(Surface->hdc, Surface->width, Surface->height);
		SelectObject(Surface->backDC, Surface->backBitmap);
		ReleaseDC(Surface->window, Surface->hdc);
	}

	SetForegroundWindow(Surface->window);
	ShowWindow(Surface->window, SW_SHOW);
	UpdateWindow(Surface->window);
	SetFocus(Surface->window);
	HideCursor();

	gameconf.grmode.flags |= DISPLAYFLAGS_WINDOWACTIVE;

	return(1 + ((Surface->flags & CFLAG_EXACTBPP) != CFLAG_EXACTBPP));
}
//===========================================
void Controller::QuitVideoMode(bool donotfree)
{
	if (Surface && Surface->window)
	{
		if (Surface->palette && !donotfree)
		{
			wfree(Surface->palette);
			Surface->palette = NULL;
		}
		if (Surface->flags & CFLAG_EXACTBPP)
		{
			Surface->Xpixels = NULL;
		}
		if (Surface->pixels && !donotfree)
		{
			wfree(Surface->pixels);
			Surface->pixels = NULL;
		}
		if (Surface->Xpixels && !donotfree)
		{
			wfree(Surface->Xpixels);
			Surface->Xpixels = NULL;
		}
		if (Surface->FullScreen)
		{
			ChangeDisplaySettings(0, 0);
		}


		DeleteDC(Surface->hdc);
		DeleteDC(Surface->backDC);
		DeleteObject(Surface->backBitmap);

		CleanD3D(Surface->d3d,Surface->d3ddev);

		DestroyWindow(Surface->window);
		Surface->window = NULL;

		ShowCursor();
	}
}
//===========================================
void Controller::QuitVideoMode(void)
{
	QuitVideoMode(false);
}
//===========================================
int Controller::ModifyVideoMode(int x, int y, int bpp, int fullscreen, unsigned char *palette)
{
	int retvalue = 0;
	
	QuitVideoMode(true);
	retvalue = QueryVideoMode(x, y, bpp, fullscreen);
	if (retvalue)
	{
		if (palette)
			ApplyPalette(palette);
		if (palette && (Surface->flags & CFLAG_EXACTBPP))
			UpdateScreen();
		gameconf.grmode.flags |= DISPLAYFLAGS_WINDOWACTIVE;
	}
	return retvalue;
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
								 Surface->backDC, regions[i].x, regions[i].y, SRCCOPY);
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
	uint32_t palcol32;
	uint16_t palcol16;
	uint8_t palcol8;
	uint16_t *palette16;
	uint32_t *palette32;
	uint8_t *palette8;

	if (Surface->DesiredBpp != 8)
		return;
	palette32 = ((uint32_t *)Surface->palette) + from;
	switch (Surface->SavedBpp)
	{
	case 8:
		return;
	case 15:
		palette16 = (uint16_t *)palette32;
		do {
			palcol16 = (((*pal4) >> 3) << 10) | (((*(pal4 + 1)) >> 3) << 5) | (((*(pal4 + 2)) >> 3));
			pal4 += 4;
			*palette16++ = palcol16;
		} while (--count);
		break;
	case 16:
		palette16 = (uint16_t *)palette32;
		do {
			palcol16 = (((*pal4) >> 3) << 11) | (((*(pal4 + 1)) >> 2) << 5) | (((*(pal4 + 2)) >> 3));
			pal4 += 4;
			*palette16++ = palcol16;
		} while (--count);
		break;
	case 24:
	case 32:
		palette8 = (uint8_t *)palette32;
		do {
			*palette8++ = pal4[2];
			*palette8++ = pal4[1];
			*palette8++ = pal4[0];
			*palette8++ = 0;
			pal4 += 4;
		} while (--count);
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
	switch (keysym)
	{
	case SHIFTLKEY:
	//case SHIFTRKEY:
		if (status)
			KeyFlags |= SHIFTKEYMASK;
		else
			KeyFlags &= ~SHIFTKEYMASK;
		break;
	case CTRLLKEY:
	//case CTRLRKEY:
		if (status)
			KeyFlags |= CTRLKEYMASK;
		else
			KeyFlags &= ~CTRLKEYMASK;
		break;
	case CAPSKEY:
		if (!status)
			KeyFlags ^= LOCKKEYMASK;
		break;
	}
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
	return true;
}


