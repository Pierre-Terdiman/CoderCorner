// ------------------------------------------------------------------------------------------------------------------------
// Project Name: Coder Corner
// File Name: FRACTAL.C
// Author: various
// Date: 08/15/98
// Abstract:
//
// This module contains all fractal-based routines. This file was part of the book
// "Texturing and modeling - a procedural approach" by these famous smart guys:
// David Ebert, F. Kenton Musgrave (who worked with Mandelbrot), Darwyn Peachey,
// Ken Perlin and Steven Worley. This file should be included as part of many
// projects. Grouped/cleaned/modified for Watcom C 10.6 by P.Terdiman (1997).
//
// Contents:
//
// - Bias and Gain functions
// - Noise functions over one to three dimensions
// - Procedural FBM (Fractal Brownian Motion)
// - Procedural MultiFractal
// - Procedural Heterogeneous Terrain
// - Procedural Hybrid MultiFractal
// - Procedural Ridged MultiFractal
// - Turbulence function
//
// THIS IS A VOLCANIC5 SAMPLE CODE EXTRACTED FROM CODER CORNER (TEST PURPOSE)
// ------------------------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Types.h"
#include "Errors.h"
#include "MemHand.h"
#include "Fractal.h"

// ------------------------------------------------------------------------------------------------------------------------

/*
 * Procedural fBm evaluated at "point"; returns value stored in "value".
 *
 * Copyright 1994 F. Kenton Musgrave
 *
 * Parameters:
 *    ``H''  is the fractal increment parameter
 *    ``lacunarity''  is the gap between successive frequencies
 *    ``octaves''  is the number of frequencies in the fBm
 */
Float64 FRAC_fBm(FRACTALVECTOR vectar, Float64 H, Float64 lacunarity, Float64 octaves)
{
	static int i;
	static int first = TRUE;
	static Float64 value, frequency, remainder;
	static Float64 *exponent_array;
	static Float32 point[3];

	point[0] = (Float32) vectar.x;
	point[1] = (Float32) vectar.y;
	point[2] = (Float32) vectar.z;

	/* precompute and store spectral weights */
	if (first)
	{
		/* seize required memory for exponent_array */
		exponent_array = (Float64 *) MEM_ZapMalloc((Dword)(octaves+1)*sizeof(Float64));
		if(exponent_array==NULL) return(0.0);
		frequency = 1.0;
		for (i=0; i<=octaves; i++)
		{
			/* compute weight for each frequency */
			exponent_array[i] = pow( frequency, -H );
			frequency *= lacunarity;
		}
		first = FALSE;
	}

	/* initialize vars to proper values */
	value     = 0.0;
	frequency = 1.0;

	/* inner loop of spectral construction */
	for (i=0; i<octaves; i++)
	{
		value += (Float64) FRAC_Noise3(point) * exponent_array[i];
		point[0] *= (Float32)lacunarity;
		point[1] *= (Float32)lacunarity;
		point[2] *= (Float32)lacunarity;
	} /* for */

	remainder = octaves - (int)octaves;
	if ( remainder )      /* add in ``octaves''  remainder */
		/* ``i''  and spatial freq. are preset in loop above */
		value += remainder * (Float64) FRAC_Noise3( point ) * exponent_array[i];

	return( value );
} /* fBm() */




// ------------------------------------------------------------------------------------------------------------------------

/*
 * Procedural multifractal evaluated at "point";
 * returns value stored in "value".
 *
 * Copyright 1994 F. Kenton Musgrave
 *
 * Parameters:
 *    ``H''  determines the highest fractal dimension
 *    ``lacunarity''  is gap between successive frequencies
 *    ``octaves''  is the number of frequencies in the fBm
 *    ``offset''  is the zero offset, which determines multifractality
 */
Float64 FRAC_Multifractal(FRACTALVECTOR vectar, Float64 H,
                         Float64 lacunarity, Float64 octaves, Float64 offset)
{
	static Float64 value, frequency, remainder;
	static int i;
	static int first = TRUE;
	static Float64 *exponent_array;
	static Float32  point[3];

	point[0] = (Float32) vectar.x;
	point[1] = (Float32) vectar.y;
	point[2] = (Float32) vectar.z;

	/* precompute and store spectral weights */
	if ( first )
	{
		/* seize required memory for exponent_array */
		exponent_array = (Float64 *)MEM_ZapMalloc((Dword)(octaves+1) * sizeof(Float64) );
		if(exponent_array==NULL) return(0.0);
		frequency = 1.0;
		for (i=0; i<=octaves; i++)
		{
			/* compute weight for each frequency */
			exponent_array[i] = pow( frequency, -H );
			frequency *= lacunarity;
		}
		first = FALSE;
	}

	value = 1.0;            /* initialize vars to proper values */
	frequency = 1.0;

	/* inner loop of multifractal construction */
	for (i=0; i<octaves; i++)
	{
		value *= offset * frequency * (Float64) FRAC_Noise3( point );
		point[0] *= (Float32)lacunarity;
		point[1] *= (Float32)lacunarity;
		point[2] *= (Float32)lacunarity;
	} /* for */

	remainder = octaves - (int)octaves;
	if ( remainder )      /* add in ``octaves''  remainder */
		/* ``i''  and spatial freq. are preset in loop above */
		value += remainder * (Float64) FRAC_Noise3( point ) * exponent_array[i];

	return value;
} /* multifractal() */

// ------------------------------------------------------------------------------------------------------------------------

/* "Variable Lacunarity Noise"  -or- VLNoise3()
 * A distorted variety of Perlin noise.
 *
 * Copyright 1994 F. Kenton Musgrave
 */
//Float64 VLNoise3(FRACTALVECTOR point, Float64 distortion)
//{
//        FRACTALVECTOR offset, VecNoise3(), AddVectors();
//        Float64 Noise3();

//        offset.x = point.x +0.5;        /* misregister domain */
//        offset.y = point.y +0.5;
//        offset.z = point.z +0.5;

//        offset = VecNoise3( offset );   /* get a random vector */

//        offset.x *= distortion;         /* scale the randomization */
//        offset.y *= distortion;
//        offset.z *= distortion;

        /* ``point'' is the domain; distort domain by adding ``offset'' */
//        point = AddVectors( point, offset );

//        return Noise3( point );         /* distorted-domain noise */

//} /* VLNoise3() */

// ------------------------------------------------------------------------------------------------------------------------

/*
 * Heterogeneous procedural terrain function: stats by altitude method.
 * Evaluated at "point"; returns value stored in "value".
 *
 * Copyright 1994 F. Kenton Musgrave
 *
 * Parameters:
 *       ``H''  determines the fractal dimension of the roughest areas
 *       ``lacunarity''  is the gap between successive frequencies
 *       ``octaves''  is the number of frequencies in the fBm
 *       ``offset''  raises the terrain from `sea level'
 */
Float64 FRAC_Hetero_Terrain(FRACTALVECTOR vectar,
  Float64 H, Float64 lacunarity, Float64 octaves, Float64 offset)
{
	static Float64 value, increment, frequency, remainder;
	static int i;
	static int first = TRUE;
	static Float64 *exponent_array;
	static Float32  point[3];

	point[0] = (Float32) vectar.x;
	point[1] = (Float32) vectar.y;
	point[2] = (Float32) vectar.z;

	/* precompute and store spectral weights, for efficiency */
	if ( first )
	{
		/* seize required memory for exponent_array */
		exponent_array = (Float64 *)MEM_ZapMalloc( (Dword)(octaves+1) * sizeof(Float64) );
		if(exponent_array==NULL) return(0.0);
		frequency = 1.0;
		for (i=0; i<=octaves; i++)
		{
			/* compute weight for each frequency */
			exponent_array[i] = pow( frequency, -H );
			frequency *= lacunarity;
		}
		first = FALSE;
	}

	/* first unscaled octave of function; later octaves are scaled */
	value = offset + (Float64) FRAC_Noise3( point );
	point[0] *= (Float32)lacunarity;
	point[1] *= (Float32)lacunarity;
	point[2] *= (Float32)lacunarity;

	/* spectral construction inner loop, where the fractal is built */
	for (i=1; i<octaves; i++)
	{
		/* obtain displaced noise value */
		increment = (Float64) FRAC_Noise3( point ) + offset;
		/* scale amplitude appropriately for this frequency */
		increment *= exponent_array[i];
		/* scale increment by current `altitude' of function */
		increment *= value;
		/* add increment to ``value''  */
		value += increment;
		/* raise spatial frequency */
		point[0] *= (Float32)lacunarity;
		point[1] *= (Float32)lacunarity;
		point[2] *= (Float32)lacunarity;
	} /* for */

	/* take care of remainder in ``octaves''  */
	remainder = octaves - (int)octaves;
	if ( remainder ) {
		/* ``i''  and spatial freq. are preset in loop above */
		/* note that the main loop code is made shorter here */
		/* you may want to that loop more like this */
		increment = ((Float64) FRAC_Noise3( point ) + offset) * exponent_array[i];
		value += remainder * increment * value;
	}

	return( value );
} /* Hetero_Terrain() */

// ------------------------------------------------------------------------------------------------------------------------

/* Hybrid additive/multiplicative multifractal terrain model.
 *
 * Copyright 1994 F. Kenton Musgrave
 *
 * Some good parameter values to start with:
 *
 *      H:           0.25
 *      offset:      0.7
 */
Float64 FRAC_HybridMultifractal( FRACTALVECTOR vectar, Float64 H, Float64 lacunarity,
                    Float64 octaves, Float64 offset )
{
	static Float64 frequency, result, signal, weight, remainder;
	static int i;
	static int first = TRUE;
	static Float64 *exponent_array;
	static Float32  point[3];

	point[0] = (Float32) vectar.x;
	point[1] = (Float32) vectar.y;
	point[2] = (Float32) vectar.z;

	/* precompute and store spectral weights */
	if ( first )
	{
		/* seize required memory for exponent_array */
		exponent_array = (Float64 *)MEM_ZapMalloc( (Dword)(octaves+1) * sizeof(Float64) );
		if(exponent_array==NULL) return(0.0);
		frequency = 1.0;
		for (i=0; i<=octaves; i++)
		{
			/* compute weight for each frequency */
			exponent_array[i] = pow( frequency, -H);
			frequency *= lacunarity;
		}
		first = FALSE;
	}

	/* get first octave of function */
	result = ( (Float64) FRAC_Noise3( point ) + offset ) * exponent_array[0];
	weight = result;

	/* increase frequency */
	point[0] *= (Float32)lacunarity;
	point[1] *= (Float32)lacunarity;
	point[2] *= (Float32)lacunarity;

	/* spectral construction inner loop, where the fractal is built */
	for (i=1; i<octaves; i++)
	{
		/* prevent divergence */
		if ( weight > 1.0 )  weight = 1.0;

		/* get next higher frequency */
		signal = ( (Float64) FRAC_Noise3( point ) + offset ) * exponent_array[i];
		/* add it in, weighted by previous freq's local value */
		result += weight * signal;
		/* update the (monotonically decreasing) weighting value */
		/* (this is why H must specify a high fractal dimension) */
		weight *= signal;

		/* increase frequency */
		point[0] *= (Float32)lacunarity;
		point[1] *= (Float32)lacunarity;
		point[2] *= (Float32)lacunarity;
	} /* for */

	/* take care of remainder in ``octaves''  */
	remainder = octaves - (int)octaves;
	if ( remainder )
		/* ``i''  and spatial freq. are preset in loop above */
		result += remainder * (Float64) FRAC_Noise3( point ) * exponent_array[i];

	return( result );
} /* HybridMultifractal() */

// ------------------------------------------------------------------------------------------------------------------------

/* Ridged multifractal terrain model.
 *
 * Copyright 1994 F. Kenton Musgrave
 *
 * Some good parameter values to start with:
 *
 *      H:           1.0
 *      offset:      1.0
 *      gain:        2.0
 */
Float64 FRAC_RidgedMultifractal( FRACTALVECTOR vectar, Float64 H, Float64 lacunarity,
        Float64 octaves, Float64 offset, Float64 gain )
{
	Float64 result, frequency, signal, weight;
	Dword i;
	static Dword first = TRUE;
	Float64 *exponent_array;
	Float32  point[3];

	point[0] = (Float32) vectar.x;
	point[1] = (Float32) vectar.y;
	point[2] = (Float32) vectar.z;

	/* precompute and store spectral weights */
	if ( first )
	{
		/* seize required memory for exponent_array */
		exponent_array = (Float64 *)MEM_ZapMalloc((Dword) (octaves+1) * sizeof(Float64) );
		if(exponent_array==NULL) return(0.0);
		frequency = 1.0;
		for (i=0; i<=octaves; i++)
		{
			/* compute weight for each frequency */
			exponent_array[i] = pow( frequency, -H );
			frequency *= lacunarity;
		}
		first = FALSE;
	}

	/* get first octave */
	signal = (Float64) FRAC_Noise3( point );
	/* get absolute value of signal (this creates the ridges) */
	if ( signal < 0.0 ) signal = -signal;
	/* invert and translate (note that "offset" should be ~= 1.0) */
	signal = offset - signal;
	/* square the signal, to increase "sharpness" of ridges */
	signal *= signal;
	/* assign initial values */
	result = signal;
	weight = 1.0;

	for( i=1; i<octaves; i++ )
	{
		/* increase the frequency */
		point[0] *= (Float32)lacunarity;
		point[1] *= (Float32)lacunarity;
		point[2] *= (Float32)lacunarity;

		/* weight successive contributions by previous signal */
		weight = signal * gain;
		if ( weight > 1.0 ) weight = 1.0;
		if ( weight < 0.0 ) weight = 0.0;
		signal = (Float64) FRAC_Noise3( point );
		if ( signal < 0.0 ) signal = -signal;
		signal = offset - signal;
		signal *= signal;
		/* weight the contribution */
		signal *= weight;
		result += signal * exponent_array[i];
	}

	return( result );
} /* RidgedMultifractal() */

// ------------------------------------------------------------------------------------------------------------------------

//Float32 FRAC_Turbulence(Float32 *v, Float32 freq)
Float32 FRAC_Turbulence(FRACTALVECTOR v, Float32 freq)
{
	static Float32 t, vec[3];

	for (t = 0. ; freq >= 1. ; freq /= 2)
	{
		vec[0] = (Float32)(((Float64)freq) * v.x);
		vec[1] = (Float32)(((Float64)freq) * v.y);
		vec[2] = (Float32)(((Float64)freq) * v.z);
		t += (Float32)fabs(FRAC_Noise3(vec)) / freq;
	}
	return t;
}

// ------------------------------------------------------------------------------------------------------------------------

Float32 FRAC_Bias(Float32 a, Float32 b)
{
	return (Float32)pow(a, log(b) / log(0.5));
}

// ------------------------------------------------------------------------------------------------------------------------

Float32 FRAC_Gain(Float32 a, Float32 b)
{
	Float32 p = (Float32)(log(1. - b) / log(0.5));

	if (a < .001) return 0.;
	else if (a > .999) return 1.;

	if (a < 0.5) return (Float32)(pow(2 * a, p) / 2);
	else return (Float32)(1. - pow(2 * (1. - a), p) / 2);
}

// ------------------------------------------------------------------------------------------------------------------------

/* noise functions over 1, 2, and 3 dimensions */

#define B 0x100
#define BM 0xff

#define N 0x1000
#define NP 12   /* 2^N */
#define NM 0xfff

static p[B + B + 2];
static Float32 g3[B + B + 2][3];
static Float32 g2[B + B + 2][2];
static Float32 g1[B + B + 2];
static start = 1;

#define s_curve(t) ( t * t * (3. - 2. * t) )

#define lerp(t, a, b) ( a + t * (b - a) )

#define setup(i,b0,b1,r0,r1)\
	t = vec[i] + N;\
	b0 = ((int)t) & BM;\
	b1 = (b0+1) & BM;\
	r0 = t - (int)t;\
	r1 = r0 - 1.;

Float32 FRAC_Noise1(Float32 arg)
{
	static int bx0, bx1;
	static Float32 rx0, rx1, sx, t, u, v, vec[1];

	vec[0] = arg;
	if (start)
	{
		start = 0;
		FRAC_Init();
	}

	setup(0, bx0,bx1, rx0,rx1);

	sx = s_curve(rx0);

	u = rx0 * g1[ p[ bx0 ] ];
	v = rx1 * g1[ p[ bx1 ] ];

	return lerp(sx, u, v);
}

// ------------------------------------------------------------------------------------------------------------------------

Float32 FRAC_Noise2(Float32 vec[2])
{
	static int bx0, bx1, by0, by1, b00, b10, b01, b11;
	static Float32 rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
	register i, j;  /* Useless nowadays! */

	if (start)
	{
		start = 0;
		FRAC_Init();
	}

	setup(0, bx0,bx1, rx0,rx1);
	setup(1, by0,by1, ry0,ry1);

	i = p[ bx0 ];
	j = p[ bx1 ];

	b00 = p[ i + by0 ];
	b10 = p[ j + by0 ];
	b01 = p[ i + by1 ];
	b11 = p[ j + by1 ];

	sx = s_curve(rx0);
	sy = s_curve(ry0);

#define at2(rx,ry) ( rx * q[0] + ry * q[1] )

	q = g2[ b00 ] ; u = at2(rx0,ry0);
	q = g2[ b10 ] ; v = at2(rx1,ry0);
	a = lerp(sx, u, v);

	q = g2[ b01 ] ; u = at2(rx0,ry1);
	q = g2[ b11 ] ; v = at2(rx1,ry1);
	b = lerp(sx, u, v);

	return lerp(sy, a, b);
}

// ------------------------------------------------------------------------------------------------------------------------

Float32 FRAC_Noise3(Float32 vec[3])
{
	static int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
	static Float32 rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
	register i, j;

	if (start)
	{
		start = 0;
		FRAC_Init();
	}

	setup(0, bx0,bx1, rx0,rx1);
	setup(1, by0,by1, ry0,ry1);
	setup(2, bz0,bz1, rz0,rz1);

	i = p[ bx0 ];
	j = p[ bx1 ];

	b00 = p[ i + by0 ];
	b10 = p[ j + by0 ];
	b01 = p[ i + by1 ];
	b11 = p[ j + by1 ];

	t  = s_curve(rx0);
	sy = s_curve(ry0);
	sz = s_curve(rz0);

#define at3(rx,ry,rz) ( rx * q[0] + ry * q[1] + rz * q[2] )

	q = g3[ b00 + bz0 ] ; u = at3(rx0,ry0,rz0);
	q = g3[ b10 + bz0 ] ; v = at3(rx1,ry0,rz0);
	a = lerp(t, u, v);

	q = g3[ b01 + bz0 ] ; u = at3(rx0,ry1,rz0);
	q = g3[ b11 + bz0 ] ; v = at3(rx1,ry1,rz0);
	b = lerp(t, u, v);

	c = lerp(sy, a, b);

	q = g3[ b00 + bz1 ] ; u = at3(rx0,ry0,rz1);
	q = g3[ b10 + bz1 ] ; v = at3(rx1,ry0,rz1);
	a = lerp(t, u, v);

	q = g3[ b01 + bz1 ] ; u = at3(rx0,ry1,rz1);
	q = g3[ b11 + bz1 ] ; v = at3(rx1,ry1,rz1);
	b = lerp(t, u, v);

	d = lerp(sy, a, b);

	return lerp(sz, c, d);
}

// ------------------------------------------------------------------------------------------------------------------------

static void FRAC_Normalize2(Float32 v[2])
{
	static Float32 s;

	s = sqrt(v[0] * v[0] + v[1] * v[1]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
}

// ------------------------------------------------------------------------------------------------------------------------

static void FRAC_Normalize3(Float32 v[3])
{
	static Float32 s;

	s = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
	v[2] = v[2] / s;
}

// ------------------------------------------------------------------------------------------------------------------------

static void FRAC_Init(void)
{
	static int i, j, k;

	for (i = 0 ; i < B ; i++)
	{
		p[i] = i;
		g1[i] = (Float32)((rand() % (B + B)) - B) / B;
		for (j = 0 ; j < 2 ; j++) g2[i][j] = (Float32)((rand() % (B + B)) - B) / B;
		FRAC_Normalize2(g2[i]);
		for (j = 0 ; j < 3 ; j++) g3[i][j] = (Float32)((rand() % (B + B)) - B) / B;
		FRAC_Normalize3(g3[i]);
	}

	while (--i)
	{
		k = p[i];
		p[i] = p[j = rand() % B];
		p[j] = k;
	}

	for (i = 0 ; i < B + 2 ; i++)
	{
		p[B + i] = p[i];
		g1[B + i] = g1[i];
		for (j = 0 ; j < 2 ; j++) g2[B + i][j] = g2[i][j];
		for (j = 0 ; j < 3 ; j++) g3[B + i][j] = g3[i][j];
	}
}

// ------------------------------------------------------------------------------------------------------------------------

