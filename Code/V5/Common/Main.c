// ------------------------------------------------------------------------------------------------------------------------
// Project Name: Coder Corner
// File Name: MAIN.C
// Author: Pierre Terdiman [Zappy]
// Date: 08/15/98
// Abstract: BOMB Studio (IHM V2.0)
// THIS IS A VOLCANIC5 SAMPLE CODE EXTRACTED FROM BOMB STUDIO (TEST PURPOSE)
// ------------------------------------------------------------------------------------------------------------------------

// Windows includes
#include <windows.h>
#include <windowsx.h>
// Standard includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
// My includes
#include "Types.h"
#include "Errors.h"
#include "Keyboard.h"
#include "MemHand.h"
#include "Main.h"

static BITMAPINFO *BitmapInfos;
static HDC ScreenDC;
static Dword MustBeClosed=0;
static BS_VIDEO gBS_Video;
static MSG gBS_Msg;
static HWND WinID;
static HINSTANCE gBS_Handle;
static HWND DDHwnd;
static int gBS_nCmdShow;

// ------------------------------------------------------------------------------------------------------------------------

// This is the main entry point for BOMB Studio.
// PRIVATE FUNCTION
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX WndClass;

	Byte Status;

	// 1) Init Windows
	if (!hPrevInstance)
	{
		WndClass.cbClsExtra    = 0;
		WndClass.cbWndExtra    = 0;
		WndClass.hbrBackground = NULL;
		WndClass.hCursor       = LoadCursor (NULL, IDC_ARROW);
		WndClass.hIcon         = LoadIcon (hInstance, IDI_APPLICATION);
		WndClass.hInstance     = hInstance;
		WndClass.lpfnWndProc   = WindowProc;
		WndClass.lpszClassName = "Main";
		WndClass.lpszMenuName  = "Main";
		WndClass.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		WndClass.cbSize        = sizeof(WNDCLASSEX);
		WndClass.hIconSm       = LoadIcon(hInstance, "Main");
	}
	else return (WINDOWS_ERROR);

	if(!RegisterClassEx(&WndClass)) return (WINDOWS_ERROR);

    DDHwnd = CreateWindowEx(
        WS_EX_TOPMOST,
        "Main",
        "Main",
        WS_POPUP,
        0, 0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        NULL,
        NULL,
        hInstance,
        NULL );

	if(DDHwnd == NULL) return (WINDOWS_ERROR);

	// Save stuff we'll need later
	gBS_Handle = hInstance;
	gBS_nCmdShow = nCmdShow;

	if (MEM_ZapInitRam()) return(MEMORY_ERROR);

	// 5) Call user code
	Status = BS_Main();

	// 6) Close everything
	PostMessage(WinID, WM_DESTROY, 0, 0);
	PostMessage(DDHwnd, WM_DESTROY, 0, 0);
	Status = MEM_RestoreMemory();

	return(Status);
} // WinMain

// ------------------------------------------------------------------------------------------------------------------------

// PRIVATE FUNCTION
HRESULT WINAPI WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_DESTROY:
			MustBeClosed=1;
			break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
} // WindowProc

// ------------------------------------------------------------------------------------------------------------------------

// PUBLIC FUNCTION
void BS_GetEvent(BS_EVENT *Event)
{
	Byte Status;

	Status = PeekMessage(&gBS_Msg, WinID, 0, 0, PM_REMOVE);

	if (Status)
	{
		ScreenToClient(gBS_Msg.hwnd, &gBS_Msg.pt);
		Event->x = (Dword)gBS_Msg.pt.x;
		Event->y = (Dword)gBS_Msg.pt.y;
		if(Event->x<0) Event->x = 0;
		if(Event->y<0) Event->y = 0;
		if(Event->x>=gBS_Video.ScreenWidth) Event->x = gBS_Video.ScreenWidth-1;
		if(Event->y>=gBS_Video.ScreenHeight) Event->y = gBS_Video.ScreenHeight-1;

		TranslateMessage(&gBS_Msg);

		switch(gBS_Msg.message)
		{
			case WM_PAINT:
				BS_CopyLogicToPhysic();
				break;

			case WM_TIMER:
				Event->Type = BS_TIMER;
				break;

			case WM_KEYDOWN:
				Event->Key = gBS_Msg.wParam;
				Event->Type = BS_KEYPRESSED;
				if(Event->Key == KEY_ESCAPE)
				{
					Event->Type = BS_QUIT;
				}
				BS_CopyLogicToPhysic();
				break;
		}
	}
	else Event->Type = BS_NO_EVENT;

	if(MustBeClosed) Event->Type = BS_QUIT;

	DefWindowProc(gBS_Msg.hwnd, gBS_Msg.message, gBS_Msg.wParam, gBS_Msg.lParam);
} // BS_GetEvent

// ------------------------------------------------------------------------------------------------------------------------

// PUBLIC FUNCTION
Byte BS_OpenScreen(Dword Width, Dword Height, Dword BitPlanes, Dword PosX, Dword PosY, Byte *Name)
{
	Byte Status;

	gBS_Video.ScreenWidth  = Width;
	gBS_Video.ScreenHeight = Height;
	gBS_Video.BitPlanes    = BitPlanes;
	gBS_Video.Logic        = NULL;
	gBS_Video.NbBytes      = Width*Height*(BitPlanes>>3);

	Status = BS_InitGDI(Name, PosX, PosY);
	if(Status) return(Status);

	return(NO_ERROR_OCCURED);
} // BS_OpenScreen

// ------------------------------------------------------------------------------------------------------------------------

// PUBLIC FUNCTION
Byte *BS_GetLogicScreen()
{
	return(gBS_Video.Logic);
} // BS_GetLogicScreen

// ------------------------------------------------------------------------------------------------------------------------

// PUBLIC FUNCTION
void BS_SetLogicScreen(Byte *Buffer)
{
	gBS_Video.Logic = Buffer;
} // BS_SetLogicScreen

// ------------------------------------------------------------------------------------------------------------------------

// PUBLIC FUNCTION
void BS_ClearScreen()
{
	memset(gBS_Video.Logic, 0, gBS_Video.NbBytes);
} // BS_ClearScreen

// ------------------------------------------------------------------------------------------------------------------------

// PUBLIC FUNCTION
void BS_CopyLogicToPhysic()
{
	BS_CopyBufferToPhysic(gBS_Video.Logic);
} // BS_CopyLogicToPhysic

// ------------------------------------------------------------------------------------------------------------------------

// PUBLIC FUNCTION
void BS_CopyBufferToPhysic(Byte *Buffer)
{
	SetDIBitsToDevice(ScreenDC, 0, 0, gBS_Video.ScreenWidth, gBS_Video.ScreenHeight, 0, 0, 0, gBS_Video.ScreenHeight,
	Buffer, BitmapInfos, DIB_RGB_COLORS);
} // BS_CopyBufferToPhysic

// ------------------------------------------------------------------------------------------------------------------------

// PRIVATE FUNCTION
Byte BS_InitGDI(Byte *Name, Dword PosX, Dword PosY)
{
	RECT Rect;

	Rect.left	= 0;
	Rect.top	= 0;
	Rect.right	= gBS_Video.ScreenWidth;
	Rect.bottom = gBS_Video.ScreenHeight;
	AdjustWindowRect(&Rect, WS_OVERLAPPEDWINDOW, FALSE);

	WinID = CreateWindow("Main",			//nom de la classe
					Name,					//titre de la fenetre
					WS_OVERLAPPED|WS_SYSMENU|WS_MINIMIZEBOX,//style de la fenetre
					PosX,					//position initiale en x
					PosY,					//position initiale en y
					Rect.right-Rect.left-2,	//largeur initiale
					Rect.bottom-Rect.top-2,	//hauteur initiale
					NULL,					//handle de la fenetre mere
					NULL,					//handle du menu de la fenetre
					gBS_Handle,				//handle de l'instance (n°du prog)
					NULL);					//parametres de creation

	if(WinID == NULL)
	{
		return (INTERNAL);
	}

	ShowWindow(WinID, gBS_nCmdShow);
	UpdateWindow(WinID);

	BitmapInfos = (BITMAPINFO*)MEM_ZapMalloc(sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD));
	if(BitmapInfos == NULL)
	{
		return (INTERNAL);
	}

	// Initialisation de la structure BITMAPINFO
	BitmapInfos->bmiHeader.biSize				= sizeof(BITMAPINFOHEADER);
	BitmapInfos->bmiHeader.biWidth				= gBS_Video.ScreenWidth;
	BitmapInfos->bmiHeader.biHeight				= -gBS_Video.ScreenHeight;
	BitmapInfos->bmiHeader.biPlanes				= 1;
	BitmapInfos->bmiHeader.biBitCount			= (UWord)gBS_Video.BitPlanes;
	BitmapInfos->bmiHeader.biCompression		= BI_RGB;
	BitmapInfos->bmiHeader.biSizeImage			= 0;
	BitmapInfos->bmiHeader.biXPelsPerMeter		= 0;
	BitmapInfos->bmiHeader.biYPelsPerMeter		= 0;
	BitmapInfos->bmiHeader.biClrUsed			= 0;
	BitmapInfos->bmiHeader.biClrImportant		= 0;

	gBS_Video.Logic = (Byte*)MEM_ZapMalloc(gBS_Video.NbBytes * sizeof(Byte));
	if(gBS_Video.Logic==NULL) return(OUT_OF_MEMORY);

	ScreenDC = GetDC(WinID);
	if(ScreenDC == NULL)
	{
		return INTERNAL;
	}

	memset(gBS_Video.Logic, 0, gBS_Video.NbBytes);
	BS_CopyLogicToPhysic();

	return(NO_ERROR_OCCURED);
} // BS_InitGDI

// ------------------------------------------------------------------------------------------------------------------------

