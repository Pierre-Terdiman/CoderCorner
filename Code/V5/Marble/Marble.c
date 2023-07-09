// ------------------------------------------------------------------------------------------------------------------------
// Project Name: Conférences V5
// File Name: MARBLE.C
// Author: Pierre Terdiman [Zappy]
// Date: 02/15/98
// Abstract: example of procedural marble.
// The marble code comes from Ken Perlin (1985) in "Texturing and modeling - A Procedural Approach", second edition, p. 171
// This little demo-code is not optimised at all since it was written on tutorial purpose.
// ------------------------------------------------------------------------------------------------------------------------

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "Types.h"
#include "Errors.h"
#include "MemHand.h"

#include "Fractal.h"
#include "Main.h"

// Window size
#define MB_SIZEX		320
#define MB_SIZEY		200

// Marble parameters
#define SCALE_X			0.001f
#define SCALE_Y			0.001f
#define Y_INC			0.01f
#define	TURB_POWER		7.0f
#define TURB_FREQUENCY	64.0f
#define	Z_SLICE			1000.0f;

// Color parameters. The final result highly depends on them!
#define	R_AMP			0.6f
#define	G_AMP			0.8f
#define	B_AMP			0.4f
#define	R_BASE			0.3f
#define	G_BASE			0.3f
#define	B_BASE			0.6f

// BS_Main() is the user entry point called by the framework.
Byte BS_Main()
{
	Dword	x,y;
	Byte Status;
	BS_EVENT Event;
	UByte *MyScreen;
	FRACTALVECTOR Vector;

	// 1) Init
	// You can modify the z value to compute another marble slice.
	Vector.z = Z_SLICE;

	// Open a simple window to visualize the result
	Status = BS_OpenScreen(MB_SIZEX, MB_SIZEY, 32, 100, 100, "Marble");
	if(Status) return(Status);

	MyScreen = (UByte*)BS_GetLogicScreen();

	// 2) Compute marble
	// The interesting stuff begins here. I'll simply loop through every pixel and compute a marble value for each pixel.
	// This is NOT the usual way of dealing with procedural textures: in order to get maximum benefit from them you're
	// supposed to compute them on the fly while rendering your scene.
	for(y=0;y<MB_SIZEY;y++)
	{
		for(x=0;x<MB_SIZEX;x++)
		{
			Float32	Turb, r, g, b;

			// You can play with all coeffs as you want. This is one of the advantage of procedural textures: they're created thanks
			// to a little set of parameters, and you simply have to change a value to create a different texture.
			Vector.x = (Float64)(x*SCALE_X);
			Vector.y = (Float64)(y*SCALE_Y);

			Turb = ((Float32)(y*Y_INC)) + TURB_POWER * FRAC_Turbulence(Vector, TURB_FREQUENCY);
			Turb = (Float32)sin(Turb * PI);

			Turb = (Float32)sqrt(Turb+1.0f) * 0.7071f;

			g = G_BASE + G_AMP*Turb;
			Turb = (Float32)sqrt(Turb);
			r = R_BASE + R_AMP*Turb;
			b = B_BASE + B_AMP*Turb;

			// Rescale to 0-255
			r*=255.0f;
			g*=255.0f;
			b*=255.0f;

			// Basic color clipping
			if(r>255.0f) r=255.0f;
			if(g>255.0f) g=255.0f;
			if(b>255.0f) b=255.0f;

			MyScreen[((y*MB_SIZEX)+x)*4+0] = (UByte)b;
			MyScreen[((y*MB_SIZEX)+x)*4+1] = (UByte)g;
			MyScreen[((y*MB_SIZEX)+x)*4+2] = (UByte)r;
		}
	}

	// 3) Message loop
	do{
		BS_GetEvent(&Event);
		BS_CopyLogicToPhysic();
	}while(Event.Type!=BS_QUIT);
	
	return(NO_ERROR_OCCURED);
}
