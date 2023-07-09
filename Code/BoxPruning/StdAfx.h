// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__7739E2A3_1E44_11D6_8B0F_0050BAC83302__INCLUDED_)
#define AFX_STDAFX_H__7739E2A3_1E44_11D6_8B0F_0050BAC83302__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>
	#include <assert.h>
	#include <math.h>
	#include <float.h>

	/////////////

	#define FUNCTION				extern "C"

	#ifndef ASSERT
		#define	ASSERT				assert
	#endif

		// Cosmetic stuff [mainly useful with multiple inheritance]
		#define	override(baseclass)	virtual

		// Our own inline keyword, so that:
		// - we can switch to __forceinline to check it's really better or not
		// - we can remove __forceinline if the compiler doesn't support it
		#define inline_				__forceinline
	//	#define inline_				inline

	/////////////

	#include "IceTypes.h"
	#include "IceMemoryMacros.h"

	//! Returns a unit random floating-point value
	inline_ float UnitRandomFloat()	{ return float(rand()) * ONE_OVER_RAND_MAX;	}

	namespace IceCore
	{
		#define ICECORE_API
		#include "IceUtils.h"
		#include "IceFPU.h"
		#include "IceContainer.h"
		#include "IceRevisitedRadix.h"
		#include "IceProfiler.h"
	}
	using namespace IceCore;

	namespace IceMaths
	{
		#define ICEMATHS_API
		#include "IcePoint.h"
	}
	using namespace IceMaths;

	namespace Meshmerizer
	{
		#define MESHMERIZER_API

		class MESHMERIZER_API AABB
		{
			public:
			//! Constructor
			inline_						AABB()	{}
			//! Destructor
			inline_						~AABB()	{}

			//! Get min point of the box
			inline_			void		GetMin(Point& min)						const		{ min = mMin;								}
			//! Get max point of the box
			inline_			void		GetMax(Point& max)						const		{ max = mMax;								}

			//! Get component of the box's min point along a given axis
			inline_			float		GetMin(udword axis)						const		{ return mMin[axis];						}
			//! Get component of the box's max point along a given axis
			inline_			float		GetMax(udword axis)						const		{ return mMax[axis];						}

			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			/**
			 *	Computes the 1D-intersection between two AABBs, on a given axis.
			 *	\param		a		[in] the other AABB
			 *	\param		axis	[in] the axis (0, 1, 2)
			 *	\return		true on intersection
			 */
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			inline_			bool		Intersect(const AABB& a, udword axis)	const
							{
								if(mMax[axis] < a.mMin[axis] || a.mMax[axis] < mMin[axis])	return false;
								return true;
							}

			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			/**
			 *	Computes the intersection between two AABBs.
			 *	\param		a		[in] the other AABB
			 *	\return		true on intersection
			 */
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			inline_			bool		Intersect(const AABB& a)				const
							{
								if(mMax.x < a.mMin.x
								|| a.mMax.x < mMin.x
								|| mMax.y < a.mMin.y
								|| a.mMax.y < mMin.y
								|| mMax.z < a.mMin.z
								|| a.mMax.z < mMin.z)	return false;

								return true;
							}

							Point		mMin;			//!< Min point
							Point		mMax;			//!< Max point
		};

		#include "IceBoxPruning.h"
	}
	using namespace Meshmerizer;

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__7739E2A3_1E44_11D6_8B0F_0050BAC83302__INCLUDED_)
