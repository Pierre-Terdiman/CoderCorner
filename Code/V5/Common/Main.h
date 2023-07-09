// ------------------------------------------------------------------------------------------------------------------------
// Project Name: Coder Corner
// File Name: MAIN.H
// Author: Pierre Terdiman
// Date: 08/15/98
// Abstract:
// THIS IS A VOLCANIC5 SAMPLE CODE EXTRACTED FROM BOMB STUDIO (TEST PURPOSE)
// ------------------------------------------------------------------------------------------------------------------------

#ifndef __MAIN_H__
#define __MAIN_H__


// Misc
#define BS_SUPPORTED   1
#define BS_UNSUPPORTED 0

// Constants
#define BS_MAXNBTEXTURES 64

// Screen modes
#define BS_WINDOW     0
#define BS_FULLSCREEN 1

// Screen types
#define BS_GDI        0
#define BS_DIRECTDRAW 1

// Events
#define BS_NO_EVENT   0
#define BS_KEYPRESSED 1
#define BS_MOUSEMOVE  2
#define BS_QUIT       3
#define BS_TIMER      4

// Render modes
#define BS_RENDER_DOTS    0
#define BS_RENDER_WIRED   1
#define BS_RENDER_FLAT    2
#define BS_RENDER_GOURAUD 3
#define BS_RENDER_PHONG   4
#define BS_RENDER_MAPPING 5

// Cursor
#define BS_HIDE           0
#define BS_SHOW           1

// Flags
#define BS_RADIX          0
#define BS_ZBUFFER        1
#define BS_COMPUTENORMALS 2
#define BS_DOTS_ONLY      4

// New structures
typedef struct{
 Byte *Datas;
 Byte *Palette;
 Dword Width;
 Dword Height;
}BS_TEXTURE;

typedef struct{
 BS_TEXTURE *Textures;
 Dword NbTextures;
 Dword CurrentTexture;
// CAMERA Camera;
// Matrix4x4 Matp;
// Matrix4x4 Mat;
 Float32 ViewH;         // View parameter
 Float32 ViewD;         // View parameter
 Float32 ViewF;         // View parameter
// Dword *Faces;        // Unused
// Vector *Vertices;    // Unused
 Byte *OutCodes;
// HVector *VerticesRot;
// Vector *VerticesProj;
 Dword MaxNbFaces;
 Dword MaxNbVerts;
 UByte *DotsColors;		// Colors to use for dots, NULL if only one is used
 UByte DotsColorRed;	// Single color for dots (Red)
 UByte DotsColorGreen;	// Single color for dots (Green)
 UByte DotsColorBlue;	// Single color for dots (Blue)
 Byte UseWired;			// 1 if Wired mode is on, else 0
 Byte UseFlat;			// 1 if Flat mode is on, else 0
 Byte UseGouraud;		// 1 if Gouraud mode is on, else 0
 Byte UsePhong;			// 1 if Phong mode is on, else 0
 Byte UseMapping;		// 1 if Mapping mode is on, else 0
}BS_TRIDI;

typedef struct {
 Dword Width;
 Dword Height;
 Dword NbMouseBouttons;
 Dword BitsPixels;
 Dword NumColors;
 Dword SizePalette;
 Dword NumReserved;
 Dword Processor;
 Byte MMX;
 Byte CMOV;
 Byte FPU;
 Byte CPUID;
 Byte HardMain;
 Byte Hard3D;
 Byte HardBlitter;
 Byte HardBlitMem;
 Byte HardFlip;
}BS_COMPUTER;


typedef struct{
 UByte Red;
 UByte Green;
 UByte Blue;
}BS_COLOR;

typedef struct{
 HWND ScreenHandle;
 BITMAPINFO *BitmapInfos;
 RECT Rect;
 Dword NbBytes;
 Dword Width;
 Dword Height;
 Dword BitPlanes;
 Byte *Logic;
 HDC DC;
 MSG Msg;
 Byte NeedRefresh;
}BS_SCREEN;

typedef struct{
 Dword ScreenWidth;
 Dword ScreenHeight;
 Dword BitPlanes;
 Dword NbBytes;
 Byte *Logic;
 Byte Mode;
}BS_VIDEO;

typedef struct
{
 Dword x;
 Dword y;
 Dword Type;
 Dword Key;
 Dword State;
}BS_EVENT;

typedef struct{
 Byte DirectDraw;
 Byte GDI;
 Byte Timer;
 Byte TextWindow;
 Byte Tridi;
 Byte Cursor;
}BS_USER_CHOICES;

// Prototypes
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
HRESULT WINAPI WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void BS_GetEvent(BS_EVENT *Event);
Byte BS_OpenScreen(Dword Width, Dword Height, Dword BitPlanes, Dword PosX, Dword PosY, Byte *Name);
Byte *BS_GetLogicScreen();
void BS_SetLogicScreen(Byte *Buffer);
void BS_ClearScreen();
void BS_CopyLogicToPhysic();
void BS_CopyBufferToPhysic(Byte *Buffer);
Byte BS_InitGDI(Byte *Name, Dword PosX, Dword PosY);

Byte BS_Main();

#endif // __MAIN_H__

