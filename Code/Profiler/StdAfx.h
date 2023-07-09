// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__236360E3_DCE3_11D5_8B0F_0050BAC83302__INCLUDED_)
#define AFX_STDAFX_H__236360E3_DCE3_11D5_8B0F_0050BAC83302__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define ICECORE_API

#ifndef ASSERT
	#define	ASSERT	assert
#endif

	#include <windows.h>

	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>
	#include <assert.h>

	#include "IceTypes.h"
	#include "IceFPU.h"
	#include "IceMemoryMacros.h"

	namespace IceCore
	{
		#include "IceContainer.h"
		#include "IceStack.h"
		#include "IceProfiler.h"
	}

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__236360E3_DCE3_11D5_8B0F_0050BAC83302__INCLUDED_)
