///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains a new optimized pair manager.
 *	\file		IcePairManager.h
 *	\author		Pierre Terdiman
 *	\date		December, 16, 2003
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef ICEPAIRMANAGER_H
#define ICEPAIRMANAGER_H

	struct UserPair
	{
		uword	mID0;
		uword	mID1;
		void*	mUserData;
	};

	ICE_COMPILE_TIME_ASSERT(sizeof(UserPair)==8);

	class UserPairManager2
	{
		public:
									UserPairManager2();
									~UserPairManager2();

				void				Purge();

				const UserPair*		AddPair		(uword id0, uword id1);
				bool				RemovePair	(uword id0, uword id1);
				const UserPair*		FindPair	(uword id0, uword id1)	const;
		inline_	udword				GetPairIndex(const UserPair* pair)	const
									{
										return ((udword)((size_t(pair) - size_t(mActivePairs)))/sizeof(UserPair));
									}

		inline_	udword				GetNbActivePairs()	const	{ return mNbActivePairs;	}
		inline_	const UserPair*		GetActivePairs()	const	{ return mActivePairs;		}

		private:
				udword				mHashSize;
				udword				mMask;
				udword				mNbActivePairs;
				udword*				mHashTable;
				udword*				mNext;
				UserPair*			mActivePairs;
		inline_	UserPair*			FindPair(uword id0, uword id1, udword hashValue) const;
	};

#endif // ICEPAIRMANAGER_H
