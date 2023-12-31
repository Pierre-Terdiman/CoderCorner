///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains source code from the article "Radix Sort Revisited".
 *	\file		IceRevisitedRadix.h
 *	\author		Pierre Terdiman
 *	\date		April, 4, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __ICERADIXSORT_H__
#define __ICERADIXSORT_H__

	#define RADIX_LOCAL_RAM

	class ICECORE_API RadixSort
	{
		public:
		// Constructor/Destructor
								RadixSort();
								~RadixSort();
		// Sorting methods
				RadixSort&		Sort(const udword* input, udword nb, bool signedvalues=true);
				RadixSort&		Sort(const float* input, udword nb);

		//! Access to results. mIndices is a list of indices in sorted order, i.e. in the order you may further process your data
		inline_	udword*			GetIndices()		const	{ return mIndices;		}

		//! mIndices2 gets trashed on calling the sort routine, but otherwise you can recycle it the way you want.
		inline_	udword*			GetRecyclable()		const	{ return mIndices2;		}

		// Stats
				udword			GetUsedRam()		const;
		//! Returns the total number of calls to the radix sorter.
		inline_	udword			GetNbTotalCalls()	const	{ return mTotalCalls;	}
		//! Returns the number of premature exits due to temporal coherence.
		inline_	udword			GetNbHits()			const	{ return mNbHits;		}

								PREVENT_COPY(RadixSort)
		private:
#ifndef RADIX_LOCAL_RAM
				udword*			mHistogram;					//!< Counters for each byte
				udword*			mOffset;					//!< Offsets (nearly a cumulative distribution function)
#endif
				udword			mCurrentSize;				//!< Current size of the indices list
				udword			mPreviousSize;				//!< Size involved in previous call
				udword*			mIndices;					//!< Two lists, swapped each pass
				udword*			mIndices2;
		// Stats
				udword			mTotalCalls;
				udword			mNbHits;
		// Internal methods
				bool			Resize(udword nb);
				void			ResetIndices();
	};

#endif // __ICERADIXSORT_H__
