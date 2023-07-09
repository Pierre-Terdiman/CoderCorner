///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains a simple container class.
 *	\file		IceContainer.h
 *	\author		Pierre Terdiman
 *	\date		February, 5, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __ICECONTAINER_H__
#define __ICECONTAINER_H__

	class ICECORE_API Container
	{
		public:
		// Constructor / Destructor
										Container();
										~Container();
		// Management
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	A O(1) method to add a value in the container. The container is automatically resized if needed.
		 *	The method is inline, not the resize. The call overhead happens on resizes only, which is not a problem since the resizing operation
		 *	costs a lot more than the call overhead...
		 *
		 *	\param		entry		[in] a udword to store in the container
		 *	\see		Add(float entry)
		 *	\see		Empty()
		 *	\see		Contains(udword entry)
		 *	\return		Self-Reference
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		__forceinline	Container&		Add(udword entry)
						{
							// Resize if needed
							if(mCurNbEntries==mMaxNbEntries)	Resize();

							// Add new entry
							mEntries[mCurNbEntries++]	= entry;
							return *this;
						}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	A O(1) method to add a value in the container. The container is automatically resized if needed.
		 *	The method is inline, not the resize. The call overhead happens on resizes only, which is not a problem since the resizing operation
		 *	costs a lot more than the call overhead...
		 *
		 *	\param		entry		[in] a float to store in the container
		 *	\see		Add(udword entry)
		 *	\see		Empty()
		 *	\see		Contains(udword entry)
		 *	\return		Self-Reference
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		__forceinline	Container&		Add(float entry)
						{
							// Resize if needed
							if(mCurNbEntries==mMaxNbEntries)	Resize();

							// Add new entry
							mEntries[mCurNbEntries++]	= IR(entry);
							return *this;
						}

		// Add unique [slow]
						Container&		AddUnique(udword entry)
						{
							if(!Contains(entry))	Add(entry);
							return *this;
						}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	A method to clear the container. All stored values are deleted, and it frees used ram.
		 *	\see		Reset()
		 *	\return		Self-Reference
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		__forceinline	Container&		Empty()
						{
							DELETEARRAY(mEntries);
							mCurNbEntries = mMaxNbEntries = 0;
							return *this;
						}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	A method to reset the container. Stored values are discarded but the buffer is kept so that further calls don't need resizing again.
		 *	That's a kind of temporal coherence.
		 *	\see		Empty()
		 *	\return		Self-Reference
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		__forceinline	Container&		Reset()
						{
							mCurNbEntries = 0;
							return *this;
						}

		// A method to check whether the container already contains a given value.
						bool			Contains(udword entry, udword* location=null) const;
		// A method to delete an entry.
						bool			Delete(udword entry);
		//! A method to delete the very last entry.
		__forceinline	void			DeleteLastEntry()			{ if(mCurNbEntries)	mCurNbEntries--;			}
		//! A method to delete the entry whose index is given
		__forceinline	void			DeleteIndex(udword index)	{ mEntries[index] = mEntries[--mCurNbEntries];	}

		// Helpers
						Container&		FindNext(udword& entry, bool wrap=false);
						Container&		FindPrev(udword& entry, bool wrap=false);
		// Processing
						bool			Foreach(ENUMERATION proc, udword param=0, udword context=0);
		// Stats
						udword			GetUsedRam()		const;
		// Data access.
		__forceinline	udword			GetNbEntries()		const	{ return mCurNbEntries;	}	//!< Returns the current number of entries.
		__forceinline	udword			GetEntry(udword i)	const	{ return mEntries[i];	}	//!< Returns ith entry
		__forceinline	udword*			GetEntries()		const	{ return mEntries;		}	//!< Returns the list of entries.
		__forceinline	udword&			operator[](udword i)const	{ ASSERT(i>=0 && i<mCurNbEntries); return mEntries[i];	}

		private:
		// Resizing
						bool			Resize();
		// Data
						udword			mMaxNbEntries;		//!< Maximum possible number of entries
						udword			mCurNbEntries;		//!< Current number of entries
						udword*			mEntries;			//!< List of entries
	};

#endif // __ICECONTAINER_H__
