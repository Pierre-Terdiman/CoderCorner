///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains profiling code.
 *	\file		IceProfiler.cpp
 *	\author		Pierre Terdiman
 *	\date		April, 4, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	A hierarchical profiler.
 *	\class		Profiler
 *	\author		Pierre Terdiman
 *	\version	1.0
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Precompiled Header
#include "Stdafx.h"

using namespace IceCore;

static udword gBase = 0;
void	IceCore::SetBaseTime(udword time)	{ gBase = time; }
udword	IceCore::GetBaseTime()				{ return gBase; }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Constructor.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Profiler::Profiler() : mUserData(0), mCallback(null), mEvent(null), mRecursionLevel(0), mNbRecords(0), mIsReady(false), mEndingPending(false)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Destructor.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Profiler::~Profiler()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Resets the profiler.
 *	Call it once at the beginning of each frame. If you don't call this method first, subsequent calls to the profiler will be ineffective
 *	and exit early. This is done on purpose so that the profiler's overhead is minimal when not it's not used.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Profiler::Reset()
{
	if(!gBase)	InitProfiler();

	mCyclesCounters		.Reset();
	mRecorder			.Reset();
	mOrder				.Reset();
	mNbRecords			= 0;
	mRecursionLevel		= 0;
	mEndingPending		= false;
	// Mark as ready to go
	mIsReady			= true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Starts a new profile. Successive calls are recorded in a LIFO stack.
 *	\param		label		[in] address of a constant reference label (e.g. "Rendering")
 *	\return		true if success
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Profiler::_StartProfile(const char* label)
{
	// The starting callback must be called before starting the profile (not to take its cycles into account)
	if(mCallback && label && mEvent)
	{
		// The callback is used on a given user-defined event
		if(strcmp(label, mEvent)==0)
		{
			(mCallback)(true, mUserData);
			mEndingPending = true;
		}
	}

	// Keep track of recursion level
	mRecursionLevel++;

	// Keep track of order
	mOrder.Push(mNbRecords++);

	// We begin a new cycle
	udword Value;
	IceCore::StartProfile(Value);

	mCyclesCounters.Push(Value);

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Ends last profile.
 *	\param		label		[in] address of a constant reference label (e.g. "Rendering")
 *	\return		true if success
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Profiler::_EndProfile(const char* label)
{
	// End previous cycle
	udword Value;
	if(!mCyclesCounters.Pop(Value))	return false;

	IceCore::EndProfile(Value);

	// Get order back
	udword Order;
	if(!mOrder.Pop(Order))	return false;

	// Keep track of it
	mRecorder
		.Add(Order)
		.Add(udword(label))
		.Add(Value)
		.Add(udword(0))
		.Add(--mRecursionLevel);

	// The ending callback must be called after having ended the profile
	if(mCallback && label && mEvent && mEndingPending)
	{
		if(strcmp(label, mEvent)==0)	(mCallback)(false, mUserData);
		mEndingPending = false;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Gets the list of complete results.
 *	\param		nbrecords		[out] returned number of records in the list
 *	\param		totalnbcycles	[out] returned total number of counted cycles
 *	\return		the list of profiled items
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ProfileData* Profiler::GetResults(udword& nbrecords, udword& totalnbcycles)
{
	// Checkings
	if(!mIsReady)	return null;
	// Get number of items / record
	udword NbItems = sizeof(ProfileData)/sizeof(udword);
	// Get number of records
	nbrecords = mRecorder.GetNbEntries()/NbItems;
	// Check there's at least one
	if(!nbrecords)	return null;

	// Reorganize the records (sort by insertion order)
	ProfileData* Data = (ProfileData*)mRecorder.GetEntries();
	struct Local { static int Compare(const void* e0, const void* e1)	{ return ((ProfileData*)e0)->Order - ((ProfileData*)e1)->Order; }	};
	qsort(Data, nbrecords, sizeof(ProfileData), Local::Compare);

	// Compute total number of elapsed cycles
	float Total = 0.0f;
	// Loop through records...
	for(udword i=0;i<nbrecords;i++)
	{
		// ...and only keep root-level ones
		if(!Data[i].RecursionLevel)	Total += float(Data[i].NbCycles);
	}
	totalnbcycles = (udword)Total;

	// Loop through records
	for(i=0;i<nbrecords;i++)
	{
		// Get current record
		ProfileData* CurDat = &Data[i];

		// Get current recursion level
		udword CurLevel = CurDat->RecursionLevel;

		// Special case for root-level...
		if(!CurLevel)	CurDat->Percents = float(CurDat->NbCycles) * 100.0f / Total;
		else
		{
			// Look for the current subtotal, from previous recursion level
			udword j = i-1;
			while(Data[j].RecursionLevel!=(CurLevel-1))	j--;
			CurDat->Percents = float(CurDat->NbCycles) * 100.0f / Data[j].NbCycles;
		}
	}

	// Must call Reset() again before re-using the profiler
	mIsReady = false;

	return Data;
}
