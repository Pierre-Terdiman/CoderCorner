///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains a new optimized pair manager.
 *	\file		IcePairManager.cpp
 *	\author		Pierre Terdiman
 *	\date		December, 16, 2003
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Precompiled Header
#include "StdAfx.h"

using namespace IceCore;

inline_ void Sort(uword& id0, uword& id1)
{
	if(id0>id1)	TSwap(id0, id1);
}

inline_ udword Hash(uword id0, uword id1)
{
	return Hash32Bits_1( udword(id0)|(udword(id1)<<16) );
}

inline_	bool DifferentPair(const UserPair& p, uword id0, uword id1)
{
	return (id0!=p.mID0) || (id1!=p.mID1);
}

#define INVALID_USER_ID	0xffff

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UserPairManager2::UserPairManager2() :
	mHashSize		(0),
	mMask			(0),
	mHashTable		(null),
	mNext			(null),
	mNbActivePairs	(0),
	mActivePairs	(null)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UserPairManager2::~UserPairManager2()
{
	Purge();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UserPairManager2::Purge()
{
	ICE_FREE(mNext);
	ICE_FREE(mActivePairs);
	ICE_FREE(mHashTable);
	mHashSize		= 0;
	mMask			= 0;
	mNbActivePairs	= 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const UserPair* UserPairManager2::FindPair(uword id0, uword id1) const
{
	if(!mHashTable)	return null;	// Nothing has been allocated yet

	// Order the ids
	Sort(id0, id1);

	// Compute hash value for this pair
	const udword HashValue = Hash(id0, id1) & mMask;

	// Look for it in the table
	udword Offset = mHashTable[HashValue];
	while(Offset!=INVALID_ID && DifferentPair(mActivePairs[Offset], id0, id1))
	{
		ASSERT(mActivePairs[Offset].mID0!=INVALID_USER_ID);
		Offset = mNext[Offset];		// Better to have a separate array for this
	}
	if(Offset==INVALID_ID)	return null;
	ASSERT(Offset<mNbActivePairs);
	// Match mActivePairs[Offset] => the pair is persistent
	return &mActivePairs[Offset];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Internal version saving hash computation
inline_ UserPair* UserPairManager2::FindPair(uword id0, uword id1, udword hashValue) const
{
	if(!mHashTable)	return null;	// Nothing has been allocated yet

	// Look for it in the table
	udword Offset = mHashTable[hashValue];
	while(Offset!=INVALID_ID && DifferentPair(mActivePairs[Offset], id0, id1))
	{
		ASSERT(mActivePairs[Offset].mID0!=INVALID_USER_ID);
		Offset = mNext[Offset];		// Better to have a separate array for this
	}
	if(Offset==INVALID_ID)	return null;
	ASSERT(Offset<mNbActivePairs);
	// Match mActivePairs[Offset] => the pair is persistent
	return &mActivePairs[Offset];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const UserPair* UserPairManager2::AddPair(uword id0, uword id1)
{
	// Order the ids
	Sort(id0, id1);

	udword HashValue = Hash(id0, id1) & mMask;

	UserPair* P = FindPair(id0, id1, HashValue);
	if(P)
		return P;	// Persistent pair

	// This is a new pair
	if(mNbActivePairs>=mHashSize)
	{
		// Get more entries
		mHashSize = NextPowerOfTwo(mNbActivePairs+1);
		mMask = mHashSize - 1;

		ICE_FREE(mHashTable);
		mHashTable = (udword*)ICE_ALLOC(mHashSize*sizeof(udword));
		StoreDwords(mHashTable, mHashSize, INVALID_ID);

		// Get some bytes for new entries
		UserPair* NewPairs	= (UserPair*)ICE_ALLOC(mHashSize * sizeof(UserPair));	ASSERT(NewPairs);
		udword* NewNext		= (udword*)ICE_ALLOC(mHashSize * sizeof(udword));		ASSERT(NewNext);

		// Copy old data if needed
		if(mNbActivePairs)	memcpy(NewPairs, mActivePairs, mNbActivePairs*sizeof(UserPair));
		// ### check it's actually needed... probably only for pairs whose hash value was cut by the and
		// yeah, since Hash(id0, id1) is a constant
		// However it might not be needed to recompute them => only less efficient but still ok
		for(udword i=0;i<mNbActivePairs;i++)
		{
			udword HashValue = Hash(mActivePairs[i].mID0, mActivePairs[i].mID1) & mMask;	// New hash value with new mask
			NewNext[i] = mHashTable[HashValue];
			mHashTable[HashValue] = i;
		}

		// Delete old data
		ICE_FREE(mNext);
		ICE_FREE(mActivePairs);

		// Assign new pointer
		mActivePairs = NewPairs;
		mNext = NewNext;

		// Recompute hash value with new hash size
		HashValue = Hash(id0, id1) & mMask;
	}

	UserPair* p = &mActivePairs[mNbActivePairs];
	p->mID0		= id0;
	p->mID1		= id1;

	mNext[mNbActivePairs] = mHashTable[HashValue];
	mHashTable[HashValue] = mNbActivePairs++;
	return p;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool UserPairManager2::RemovePair(uword id0, uword id1)
{
	// Order the ids
	Sort(id0, id1);

	const udword HashValue = Hash(id0, id1) & mMask;
	const UserPair* P = FindPair(id0, id1, HashValue);
	if(!P)	return false;
	ASSERT(P->mID0==id0);
	ASSERT(P->mID1==id1);

	const udword PairIndex = GetPairIndex(P);

	// Walk the hash table to fix mNext
	udword Offset = mHashTable[HashValue];
	ASSERT(Offset!=INVALID_ID);

	udword Previous=INVALID_ID;
	while(Offset!=PairIndex)
	{
		Previous = Offset;
		Offset = mNext[Offset];
	}

	// Let us go/jump us
	if(Previous!=INVALID_ID)
	{
		ASSERT(mNext[Previous]==PairIndex);
		mNext[Previous] = mNext[PairIndex];
	}
	// else we were the first
	else mHashTable[HashValue] = mNext[PairIndex];
	// we're now free to reuse next[pairIndex] without breaking the list

#ifdef _DEBUG
	mNext[PairIndex]=INVALID_ID;
#endif

	// Fill holes
	if(1)
	{
		// 1) Remove last pair
		const udword LastPairIndex = mNbActivePairs-1;
		if(LastPairIndex==PairIndex)
		{
			mNbActivePairs--;
			return true;
		}

		const UserPair* Last = &mActivePairs[LastPairIndex];
		const udword LastHashValue = Hash(Last->mID0, Last->mID1) & mMask;

		// Walk the hash table to fix mNext
		udword Offset = mHashTable[LastHashValue];
		ASSERT(Offset!=INVALID_ID);

		udword Previous=INVALID_ID;
		while(Offset!=LastPairIndex)
		{
			Previous = Offset;
			Offset = mNext[Offset];
		}

		// Let us go/jump us
		if(Previous!=INVALID_ID)
		{
			ASSERT(mNext[Previous]==LastPairIndex);
			mNext[Previous] = mNext[LastPairIndex];
		}
		// else we were the first
		else mHashTable[LastHashValue] = mNext[LastPairIndex];
		// we're now free to reuse mNext[LastPairIndex] without breaking the list

#ifdef _DEBUG
		mNext[LastPairIndex]=INVALID_ID;
#endif

		// Don't invalidate entry since we're going to shrink the array

		// 2) Re-insert in free slot
		mActivePairs[PairIndex] = mActivePairs[LastPairIndex];
#ifdef _DEBUG
		ASSERT(mNext[PairIndex]==INVALID_ID);
#endif
		mNext[PairIndex] = mHashTable[LastHashValue];
		mHashTable[LastHashValue] = PairIndex;

		mNbActivePairs--;
	}
	return true;
}

