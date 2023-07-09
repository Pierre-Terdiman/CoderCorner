// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__7509EBBD_B81F_4D2D_BF50_BED596B7237E__INCLUDED_)
#define AFX_STDAFX_H__7509EBBD_B81F_4D2D_BF50_BED596B7237E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*
	The pair manager class has been extracted from "ICE". It requires some basic defines/functions to compile.
	The necessary code has been copied to this header, although it's obviously not the way it's designed in the
	original version.
*/

#include <malloc.h>			// For _alloca
#include <assert.h>
#include <string.h>

	// Check debug mode
	#ifdef DEBUG			// May be defined instead of _DEBUG. Let's fix it.
		#ifndef	_DEBUG
			#define _DEBUG
		#endif
	#endif

	// Contributed by Bruce Mitchener
	#if defined(COMPILER_VISUAL_CPP)
		#define inline_			__forceinline
//		#define inline_			inline
	#elif defined(__GNUC__) && __GNUC__ < 3
		#define inline_ inline
	#elif defined(__GNUC__)
		#define inline_ inline __attribute__ ((always_inline))
	#else
		#define inline_ inline
	#endif

	// Down the hatch
	#pragma inline_depth( 255 )

#define ICE_COMPILE_TIME_ASSERT(exp)	extern char ICE_Dummy[ (exp) ? 1 : -1 ]

#define ASSERT		assert
#define	ICE_ALLOC	malloc
#define	ICE_FREE	free

	#define null				0														//!< our own NULL pointer

	// Custom types used in ICE
	typedef signed char			sbyte;		//!< sizeof(sbyte)		must be 1
	typedef unsigned char		ubyte;		//!< sizeof(ubyte)		must be 1
	typedef signed short		sword;		//!< sizeof(sword)		must be 2
	typedef unsigned short		uword;		//!< sizeof(uword)		must be 2
	typedef signed int			sdword;		//!< sizeof(sdword)		must be 4
	typedef unsigned int		udword;		//!< sizeof(udword)		must be 4
#ifdef WIN32
	typedef signed __int64		sqword;		//!< sizeof(sqword)		must be 8
	typedef unsigned __int64	uqword;		//!< sizeof(uqword)		must be 8
#elif LINUX
	typedef signed long long	sqword;		//!< sizeof(sqword)		must be 8
	typedef unsigned long long	uqword;		//!< sizeof(uqword)		must be 8
#elif defined(__APPLE__)
	typedef signed long long	sqword;		//!< sizeof(sqword)		must be 8
	typedef unsigned long long	uqword;		//!< sizeof(uqword)		must be 8
#elif defined(_XBOX)
	typedef signed __int64		sqword;		//!< sizeof(sqword)		must be 8
	typedef unsigned __int64	uqword;		//!< sizeof(uqword)		must be 8
#endif
	typedef float				float32;	//!< sizeof(float32)	must be 4
	typedef double				float64;	//!< sizeof(float64)	must be 8
	typedef size_t				regsize;	//!< sizeof(regsize)	must be sizeof(void*)

	ICE_COMPILE_TIME_ASSERT(sizeof(bool)==1);	// ...otherwise things might fail with VC++ 4.2 !
	ICE_COMPILE_TIME_ASSERT(sizeof(ubyte)==1);
	ICE_COMPILE_TIME_ASSERT(sizeof(sbyte)==1);
	ICE_COMPILE_TIME_ASSERT(sizeof(sword)==2);
	ICE_COMPILE_TIME_ASSERT(sizeof(uword)==2);
	ICE_COMPILE_TIME_ASSERT(sizeof(udword)==4);
	ICE_COMPILE_TIME_ASSERT(sizeof(sdword)==4);
	ICE_COMPILE_TIME_ASSERT(sizeof(uqword)==8);
	ICE_COMPILE_TIME_ASSERT(sizeof(sqword)==8);
	ICE_COMPILE_TIME_ASSERT(sizeof(float32)==4);
	ICE_COMPILE_TIME_ASSERT(sizeof(float64)==8);
	ICE_COMPILE_TIME_ASSERT(sizeof(regsize)==sizeof(void*));

	#define	INVALID_ID			0xffffffff	//!< Invalid dword ID (counterpart of null pointers)

	// "Next Largest Power of 2
	// Given a binary integer value x, the next largest power of 2 can be computed by a SWAR algorithm
	// that recursively "folds" the upper bits into the lower bits. This process yields a bit vector with
	// the same most significant 1 as x, but all 1's below it. Adding 1 to that value yields the next
	// largest power of 2. For a 32-bit value:"
	inline_ udword	NextPowerOfTwo(udword x)
	{
		x |= (x >> 1);
		x |= (x >> 2);
		x |= (x >> 4);
		x |= (x >> 8);
		x |= (x >> 16);
		return x+1;
	}

	// Generic functions
	template<class Type> inline_ void TSwap(Type& a, Type& b)								{ const Type c = a; a = b; b = c;			}

	// Thomas Wang's hash
	inline_ int Hash32Bits_1(int key)
	{
		key += ~(key << 15);
		key ^=  (key >> 10);
		key +=  (key << 3);
		key ^=  (key >> 6);
		key += ~(key << 11);
		key ^=  (key >> 16);
		return key;
	}

	inline_ udword Hash(udword id0, udword id1)
	{
		return Hash32Bits_1( (id0&0xffff)|(id1<<16) );
	}

	//!	Fills a buffer with a given dword.
	//!	\param		addr	[in] buffer address
	//!	\param		nb		[in] number of dwords to write
	//!	\param		value	[in] the dword value
	//!	\see		FillMemory
	//!	\see		ZeroMemory
	//!	\see		CopyMemory
	//!	\see		MoveMemory
	//!	\warning	writes nb*4 bytes !
	inline_ void StoreDwords(udword* dest, udword nb, udword value)
	{
		// The asm code below **SHOULD** be equivalent to one of those C versions
		// or the other if your compiled is good: (checked on VC++ 6.0)
		//
		//	1) while(nb--)	*dest++ = value;
		//
		//	2) for(udword i=0;i<nb;i++)	dest[i] = value;
		//
		_asm push eax
		_asm push ecx
		_asm push edi
		_asm mov edi, dest
		_asm mov ecx, nb
		_asm mov eax, value
		_asm rep stosd
		_asm pop edi
		_asm pop ecx
		_asm pop eax
	}

namespace IceCore
{
	#include "IcePairManager.h"
}


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__7509EBBD_B81F_4D2D_BF50_BED596B7237E__INCLUDED_)
