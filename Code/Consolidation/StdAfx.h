// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__5FA1D5C4_041A_11D5_8B0F_0050BAC83302__INCLUDED_)
#define AFX_STDAFX_H__5FA1D5C4_041A_11D5_8B0F_0050BAC83302__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

// WARNING: UGLY MINIMAL RIPPED ICE CODE FOLLOWING!

#ifndef ASSERT
	#define	ASSERT				assert
#endif

	#include "IceTypes.h"

	//! Integer representation of a floating-point value.
	#define IR(x)					((udword&)x)

	#define SIZEOFOBJECT		sizeof(*this)								//!< Gives the size of current object. Avoid some mistakes (e.g. "sizeof(this)").
	#define DELETESINGLE(x)		if (x) { delete x;			x = null; }		//!< Deletes an instance of a class.
	#define DELETEARRAY(x)		if (x) { delete []x;		x = null; }		//!< Deletes an array.
	#define CHECKALLOC(x)		if(!x) { SetIceErrorMsg("Out of memory.", 0); return false;}	//!< Standard alloc checking. HANDLE WITH CARE.

	__forceinline void ZeroMemory(void* addr, udword size)					{ memset(addr, 0, size);		}
	__forceinline void FillMemory(void* dest, udword size, ubyte val)		{ memset(dest, val, size);		}
	__forceinline void CopyMemory(void* dest, const void* src, udword size)	{ memcpy(dest, src, size);		}

	#define SetIceErrorMsg
	#define Log

	#define ICECORE_API
	namespace IceCore
	{
		#include "IceFile.h"
		#include "IceContainer.h"
		#include "IceCustomArray.h"
		#include "IceRevisitedRadix.h"
	}

	using namespace IceCore;

	class Point
	{
		public:

		//! Empty constructor
		__forceinline				Point()														{}
		//! Constructor from floats
		__forceinline				Point(float _x, float _y, float _z) : x(_x), y(_y), z(_z)	{}
		//! Constructor from array
		__forceinline				Point(float f[3]) : x(f[0]), y(f[1]), z(f[2])				{}
		//! Copy constructor
		__forceinline				Point(const Point& p) : x(p.x), y(p.y), z(p.z)				{}
		//! Destructor
		__forceinline				~Point()													{}

		//! Operator for Point VecProd = Point ^ Point.
		__forceinline	Point		operator^(const Point& p)			const
						{
							return Point(
							y * p.z - z * p.y,
							z * p.x - x * p.z,
							x * p.y - y * p.x );
						}

		//! Operator for Point += Point.
		__forceinline	Point&		operator+=(const Point& p)						{ x += p.x; y += p.y; z += p.z;	return *this;		}
		//! Operator for Point *= float.
		__forceinline	Point&		operator*=(float s)								{ x *= s; y *= s; z *= s;		return *this;		}
		//! Operator for Point Minus = Point - Point.
		__forceinline	Point		operator-(const Point& p)			const		{ return Point(x - p.x, y - p.y, z - p.z);			}
		//! Normalize the vector
		__forceinline	Point&		Normalize()
						{
							float M = x*x + y*y + z*z;
							if(M)
							{
								M = 1.0f / sqrtf(M);
								x *= M;
								y *= M;
								z *= M;
							}
							return *this;
						}
		public:
					float x, y, z;
	};

	#define MESHMERIZER_API
	namespace Meshmerizer
	{
		#include "IceVertexCloud.h"
		#include "IceMeshBuilder2.h"
	}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__5FA1D5C4_041A_11D5_8B0F_0050BAC83302__INCLUDED_)
