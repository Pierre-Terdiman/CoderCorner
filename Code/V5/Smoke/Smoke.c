// ------------------------------------------------------------------------------------------------------------------------
// Project Name: Conférences V5
// File Name: SMOKE.C
// Author: Pierre Terdiman [Zappy]
// Date: 02/15/98
// Abstract: example of procedural animated smoke
// This routine demonstrates how to animate a procedural texture by moving through the solid space. The main function used
// is, as always, the Perlin noise and corresponding turbulence code. Interpolation post process is the way to realtime.
// With this example you can begin to feel the power of proceduralism. A very little set of parameters allows for a wide
// range of smoke movements. The source code is also extremely short and easy to understand. Please also notice that this
// is a basic example, using a single basic turbulence function...
// Oh! By the way... I'm quite aware of the fact that this C code is simply not optimized at all :)
// ------------------------------------------------------------------------------------------------------------------------

#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "Types.h"
#include "Errors.h"
#include "Memhand.h"
#include "Main.h"
#include "Keyboard.h"
#include "Fractal.h"
#include "PostProcess.h"

// Window size. Handle with care if you wanna keep a decent realtime version...
#define	GXSIZE				320
#define	GYSIZE				200

// Computed smoke values are interpolated over a 8*8 grid in screen space.
// You can't change this value since provided code in PostProcess.c is only designed for 8*8 grids.
// (read comments in PostProcess.c)
#define	PIX					8

// Frequency will define the number of octaves used to compute the turbulence (ie the smoke). The more you
// add octaves, the more it looks nice, the slower it is. Try 2.0f then 4.0f then 8.0f, you'll catch the point...
#define	TURB_FREQ			32.0f

// This version computes one single smoke layer but you can compute and add many of them with different frequencies.

Byte BS_Main()
{
	Byte Status, Evol=1, SmokeType=1;
	BS_EVENT Event;
	UByte *MyScreen, *Samples, *SampledScreen;
	Dword y,x;
	Float32 SpeedMove=0.0008f, Zoom=0.759997f;
	FRACTALVECTOR Vector, Speed, VectorInit;
	Float32 Freq=20.0f, CoeffX, CoeffY, MaxValue=400.0f, EvolSpeed=0.004800f;
	UDword Value;

	// 1) Initialize
	VectorInit.x = 0.0;		VectorInit.y = 0.0;		VectorInit.z = 10.0;
	Speed.x=0.0;			Speed.y=0.004800;		Speed.z=0.0;

	// Some ram for smoke samples
	Samples = (UByte*)MEM_ZapMalloc((GXSIZE/8)*(GYSIZE/8)*sizeof(UByte));
	if(Samples==NULL) return(OUT_OF_MEMORY);
	memset(Samples, 0, (GXSIZE/8)*(GYSIZE/8));

	SampledScreen = MEM_ZapMalloc(GXSIZE*GYSIZE);
	if(SampledScreen==NULL) return(OUT_OF_MEMORY);
	memset(SampledScreen, 0, GXSIZE*GYSIZE);

	// Windows stuff
	Status = BS_OpenScreen(GXSIZE, GYSIZE, 32, 50, 20, "Smoke");
	if(Status) return(Status);
	MyScreen = (UByte*)BS_GetLogicScreen();

	// Message loop
	do{
		BS_GetEvent(&Event);

		if(Event.Type==BS_KEYPRESSED)
		{
			// You can control smoke by pressing some useful keys
			if(Event.Key==KEY_LEFT)		Speed.x+=SpeedMove;						// Use arrows to move smoke
			if(Event.Key==KEY_RIGHT)	Speed.x-=SpeedMove;						// Use arrows to move smoke
			if(Event.Key==KEY_UP)		Speed.y+=SpeedMove;						// Use arrows to move smoke
			if(Event.Key==KEY_DOWN)		Speed.y-=SpeedMove;						// Use arrows to move smoke
			if(Event.Key==KEY_ADD)		Zoom+=0.04f;							// Use + to zoom the smoke out
			if(Event.Key==KEY_SUB)		Zoom-=0.04f;							// Use - to zoom the smoke in
			if(Event.Key==KEY_RETURN)	Evol=1-Evol;							// 'Return' will stop/run the smoke evolution
			if(Event.Key==KEY_a)		MaxValue+=10.0f;						// 'a' will fade the smoke in
			if(Event.Key==KEY_z)		MaxValue-=10.0f;						// 'z' will fade the smoke out
			if(Event.Key==KEY_s)		Speed.x=Speed.y=0.0;					// 's' will stop the smoke movement
			if(Event.Key==KEY_w)		EvolSpeed+=0.0004f;						// 'w' and 'x' will control the evolution speed
			if(Event.Key==KEY_x)		EvolSpeed-=0.0004f;
			if(Event.Key==KEY_t)		SmokeType=1-SmokeType;					// ...and you even can switch to a second smoke type by pressing 't'...
		}

		VectorInit.x+=Speed.x;													// Set up initial X position in the 3D turbulence space
		VectorInit.y+=Speed.y;													// Set up initial Y position in the 3D turbulence space
		if(Evol) VectorInit.z+=EvolSpeed;										// Animation is performed by moving the smoke slice through the 3D turbulence space, along the Z axis.

		CoeffX = Zoom * 0.003f * (Float32)(PIX);								// Some scale coeffs...
		CoeffY = Zoom * 0.003f * (Float32)(PIX);

		BS_CopyLogicToPhysic();													// Copy logic screen to physic screen

		// Sampling loop
		for(y=0;y<GYSIZE/PIX;y++)
		{
			for(x=0;x<GXSIZE/PIX;x++)
			{
				Vector.x = VectorInit.x+(Float64)(x-GXSIZE/(PIX*2))*CoeffX;		// Turbulence is evaluated at 'Vector' position
				Vector.y = VectorInit.y+(Float64)(y-GYSIZE/(PIX*2))*CoeffY;
				Vector.z = VectorInit.z;

				Value=(UDword)(FRAC_Turbulence(Vector, TURB_FREQ) * MaxValue);	// Compute turbulence and scale it to a color range...

				if(Value>255) Value=255;										// ...then clip it...
				if(!SmokeType) Value=-Value;									// ...invert it if needed...
				Samples[x+y*GXSIZE/PIX] = (UByte)Value;							// ...and finally store it.
			}
		}

		// Now we interpolate the samples to create a sampled screen
		POST_8InterpolIn8Bits(Samples, GXSIZE/8, GYSIZE/8, SampledScreen, GXSIZE);

		// Then we just write it to logic screen. You can also easily compute 3 different layers, one red, one green, one blue...
		for(y=0;y<GYSIZE;y++)
		{
			for(x=0;x<GXSIZE;x++)
			{
				UByte Val=SampledScreen[x+y*GXSIZE];
				MyScreen[(x+y*GXSIZE)*4+0] = Val;
				MyScreen[(x+y*GXSIZE)*4+1] = Val;
				MyScreen[(x+y*GXSIZE)*4+2] = Val;
			}
		}

	}while(Event.Type!=BS_QUIT);
	
	return(NO_ERROR_OCCURED);
}


