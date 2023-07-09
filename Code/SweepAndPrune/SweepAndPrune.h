// This is a hook file used to compile the sweep-and-prune code out of ICE.
// This is a made-up file just to compile the thing. This is NOT the way it usually look in ICE.
#ifndef SWEEPANDPRUNE_H
#define SWEEPANDPRUNE_H

	#define ICE_NO_DLL
	#define OPCODE_API
	#define ICE_DONT_CHECK_COMPILER_OPTIONS
	#define TRUE	1
	#define FALSE	0
	#define	ASSERT	assert

// Standard includes
#ifdef _DEBUG
	#include <crtdbg.h>			// C runtime debug functions
#endif
	#include <stdio.h>			// Standard Input/Output functions
	#include <stdlib.h>
	#include <string.h>
	#include <float.h>
	#include <malloc.h>			// For _alloca
	#include <math.h>
	#include <assert.h>

	#include "IcePreprocessor.h"
	#include "IceAssert.h"
	#include "IceTypes.h"
	#include "IceMemoryMacros.h"

	namespace IceCore
	{
		#include "IceUtils.h"
		#include "IceAllocator.h"
		#include "IceFPU.h"
		#include "IceBitArray.h"
		#include "IceContainer.h"
		#include "IceRevisitedRadix.h"
		#include "IceHashing.h"
	}
	using namespace IceCore;

#define ICEMATHS_API

	enum AxisIndex
	{
		X_					= 0,
		Y_					= 1,
		Z_					= 2,
		W_					= 3,

		AXIS_FORCE_DWORD	= 0x7fffffff
	};

	enum AxisOrder
	{
		AXES_XYZ			= (X_)|(Y_<<2)|(Z_<<4),
		AXES_XZY			= (X_)|(Z_<<2)|(Y_<<4),
		AXES_YXZ			= (Y_)|(X_<<2)|(Z_<<4),
		AXES_YZX			= (Y_)|(Z_<<2)|(X_<<4),
		AXES_ZXY			= (Z_)|(X_<<2)|(Y_<<4),
		AXES_ZYX			= (Z_)|(Y_<<2)|(X_<<4),

		AXES_FORCE_DWORD	= 0x7fffffff
	};

	class ICEMATHS_API Axes : public Allocateable
	{
		public:

		inline_			Axes(AxisOrder order)
						{
							mAxis0 = (order   ) & 3;
							mAxis1 = (order>>2) & 3;
							mAxis2 = (order>>4) & 3;
						}
		inline_			~Axes()		{}

				udword	mAxis0;
				udword	mAxis1;
				udword	mAxis2;
	};

	class ICEMATHS_API Point : public Allocateable
	{
		public:
		//! Constructor
		inline_						Point()		{}
		//! Destructor
		inline_						~Point()	{}

		inline_					operator	const	float*() const	{ return &x; }
		inline_					operator			float*()		{ return &x; }

						float		x,y,z;
	};

	class ICEMATHS_API AABB : public Allocateable
	{
		public:
		//! Constructor
		inline_						AABB()	{}
		//! Destructor
		inline_						~AABB()	{}

		//! Get component of the box's min point along a given axis
		inline_			float		GetMin(udword axis)						const		{ return mMin[axis];						}
		//! Get component of the box's max point along a given axis
		inline_			float		GetMax(udword axis)						const		{ return mMax[axis];						}

						Point		mMin;
						Point		mMax;
	};

	inline_ void IceTrace(const char*){}
	inline_ void Log(const char*){}

	namespace Opcode
	{
		#include "OPC_ArraySAP.h"
	};

#endif