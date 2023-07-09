// ------------------------------------------------------------------------------------------------------------------------
// Project Name: Coder Corner
// File Name: POSTPROCESS.C
// Author: Pierre Terdiman [Zappy]
// Date: 08/15/98
// Abstract: basic post-process
// THIS IS A VOLCANIC5 SAMPLE CODE EXTRACTED FROM CODER CORNER (TEST PURPOSE)
// ------------------------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Types.h"
#include "Errors.h"
#include "PostProcess.h"

// ------------------------------------------------------------------------------------------------------------------------

// Following routine is used to interpolate computed values over a 8*8 grid in screen space.
// This was only done on optimisation purpose since the turbulence code is damn far from beeing fast. If you just call the
// turbulence code once per pixel, you're dead meat. I chose the 8*8 size simply because it looked nice.
//
// Of course you're also allowed to optimise the Perlin noise code. Provided one is a basic C implementation, but you can
// get an Intel hand-optimized MMX assembler code of the same Perlin noise on the net...
void POST_8InterpolIn8Bits(UByte *Samples, Dword Width, Dword Height, UByte *Screen, Dword ScreenWidth)
{
	UByte *Buffer;
	Dword YStep0, YStep1, XStep;
	Dword i,j,k,l;
	Dword Value;

	for(j=0;j<Height-1;j++)
	{
		for(i=0;i<Width-1;i++)
		{
			YStep0 = Samples[i+(j+1)*Width] - Samples[i+j*Width];
			YStep1 = Samples[i+1+(j+1)*Width] - Samples[i+1+j*Width];
			for(k=0;k<8;k++)
			{
				Buffer = &Screen[(i<<3)+((j<<3)+k)*ScreenWidth];

				Value = (Samples[i+j*Width]<<3) + YStep0*k;
				XStep = (Samples[i+1+j*Width]<<3) + YStep1*k;
				XStep-=Value;
				XStep>>=3;

				for(l=0;l<8;l++)
				{
					*Buffer++ = (UByte)(Value>>3);
					Value+=XStep;
				}
			}
		}
	}
}

// ------------------------------------------------------------------------------------------------------------------------

