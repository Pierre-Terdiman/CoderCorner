///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains a custom array class.
 *	\file		IceCustomArray.h
 *	\author		Pierre Terdiman
 *	\date		January, 15, 1999
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __ICECUSTOMARRAY_H__
#define __ICECUSTOMARRAY_H__

	//! Default size of a memory block
	#define CUSTOMARRAY_BLOCKSIZE	(4*1024)		// 4 Kb => heap size


	class ICECORE_API CustomArray
	{
		//! A memory block
		struct CustomBlock{
								CustomBlock()		{ Addy = null;			}
								~CustomBlock()		{ DELETEARRAY(Addy);	}

			void*				Addy;						//!< Stored data
			udword				Size;						//!< Length of stored data
			udword				Max;						//!< Heap size
		};

		//! A linked list of blocks
		struct CustomCell{
								CustomCell()		{ NextCustomCell = null; }

			struct CustomCell*	NextCustomCell;
			CustomBlock			Item;
		};

		public:
		// Constructor / Destructor
										CustomArray(udword startsize=CUSTOMARRAY_BLOCKSIZE, void* inputbuffer=null);
										CustomArray(const char* filename);
										~CustomArray();

						CustomArray&	Clean();

		// Store methods

			// Store a BOOL
						CustomArray&	Store(BOOL bo);
			// Store a bool
						CustomArray&	Store(bool bo);
			// Store a char
						CustomArray&	Store(char b);
			// Store a ubyte
						CustomArray&	Store(ubyte b);
			// Store a short
						CustomArray&	Store(short w);
			// Store a uword
						CustomArray&	Store(uword w);
			// Store a long
						CustomArray&	Store(long d);
			// Store a unsigned long
						CustomArray&	Store(unsigned long d);
			// Store a int
					//	CustomArray&	Store(int d);
			// Store a unsigned int
						CustomArray&	Store(unsigned int d);
			// Store a float
						CustomArray&	Store(float f);
			// Store a double
						CustomArray&	Store(double f);
			// Store a string
						CustomArray&	Store(const char* string);
			// Store a buffer
						CustomArray&	Store(void* buffer, udword size);
			// Store a CustomArray
						CustomArray&	Store(CustomArray* array);

		// ASCII store methods

			// Store a BOOL in ASCII
						CustomArray&	StoreASCII(BOOL bo);
			// Store a bool in ASCII
						CustomArray&	StoreASCII(bool bo);
			// Store a char in ASCII
						CustomArray&	StoreASCII(char b);
			// Store a ubyte in ASCII
						CustomArray&	StoreASCII(ubyte b);
			// Store a short in ASCII
						CustomArray&	StoreASCII(short w);
			// Store a uword in ASCII
						CustomArray&	StoreASCII(uword w);
			// Store a long in ASCII
						CustomArray&	StoreASCII(long d);
			// Store a unsigned long in ASCII
						CustomArray&	StoreASCII(unsigned long d);
			// Store a int in ASCII
					//	CustomArray&	StoreASCII(int d);
			// Store a unsigned int in ASCII
						CustomArray&	StoreASCII(unsigned int d);
			// Store a float in ASCII
						CustomArray&	StoreASCII(float f);
			// Store a double in ASCII
						CustomArray&	StoreASCII(double f);
			// Store a string in ASCII
						CustomArray&	StoreASCII(const char* string);

		// Bit storage - inlined since mostly used in data compression where speed is welcome

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	A method to store a bit.
		 *	\param		bit			[in] the bit to store
		 *	\return		Self-Reference
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		__forceinline	CustomArray&	StoreBit(bool bit)
						{
							mBitMask<<=1;				// Shift current bitmask
//							if(bit)	mBitMask |= 1;		// Set the LSB if needed
							mBitMask |= (ubyte)bit;		// Set the LSB if needed - no conditional branch here
							mBitCount++;				// Count number of active bits in bitmask
							if(mBitCount==8)			// If bitmask is full...
							{
								mBitCount = 0;			// ...then reset counter...
								Store(mBitMask);		// ...and store bitmask.
							}
							return *this;
						}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	A method to store bits.
		 *	\param		bits		[in] the bit pattern to store
		 *	\param		nbbits		[in] the number of bits to store
		 *	\return		Self-Reference
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		__forceinline	CustomArray&	StoreBits(udword bits, udword nbbits)
						{
							udword Mask=1<<(nbbits-1);
							while(Mask)
							{
								StoreBit((bits&Mask)!=0);
								Mask>>=1;
							}
							return *this;
						}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	A method to read a bit.
		 *	\return		the bit
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		__forceinline	bool			GetBit()
						{
							if(!mBitCount)
							{
								mBitMask = GetByte();
								mBitCount = 0x80;
							}
							bool Bit = (mBitMask&mBitCount)!=0;
							mBitCount>>=1;
							return Bit;
						}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	A method to read bits.
		 *	\param		nbbits		[in] the number of bits to read
		 *	\return		the bits
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		__forceinline	udword			GetBits(udword nbbits)
						{
							udword Bits = 0;
							for(udword i=0;i<nbbits;i++)
							{
								Bits<<=1;
								bool Bit = GetBit();
								Bits|=udword(Bit);
							}
							return Bits;
						}

		// Padd extra bits to reach a byte
						CustomArray&	EndBits();

		// Management methods
						bool			ExportToDisk(const char* FileName);
						bool			ExportToDisk(FILE* fp);

						udword			GetOffset();
						CustomArray&	Padd();
						CustomArray&	LinkTo(CustomArray* array);
						void*			Collapse(void* userbuffer=null);

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	A method to get the current address within the current block.
		 *	\return		destination buffer
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		__forceinline	void*			GetAddress()
						{
							char* CurrentAddy = (char*)mCurrentCell->Item.Addy;
							CurrentAddy+=mCurrentCell->Item.Size;
							return CurrentAddy;
						}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	A method to get used size within current block.
		 *	\return		destination buffer
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		__forceinline	udword			GetCellUsedSize()	{ return mCurrentCell->Item.Size;	}

		// Address methods
						bool			PushAddress();
						CustomArray&	PopAddressAndStore(BOOL Bo);
						CustomArray&	PopAddressAndStore(bool Bo);
						CustomArray&	PopAddressAndStore(char b);
						CustomArray&	PopAddressAndStore(ubyte b);
						CustomArray&	PopAddressAndStore(short w);
						CustomArray&	PopAddressAndStore(uword w);
						CustomArray&	PopAddressAndStore(long d);
						CustomArray&	PopAddressAndStore(unsigned long d);
					//	CustomArray&	PopAddressAndStore(int d);
						CustomArray&	PopAddressAndStore(unsigned int d);
						CustomArray&	PopAddressAndStore(float f);
						CustomArray&	PopAddressAndStore(double f);

		// Read methods
						ubyte			GetByte();					//!< Read a byte from the array
						uword			GetWord();					//!< Read a word from the array
						udword			GetDword();					//!< Read a dword from the array
						float			GetFloat();					//!< Read a float from the array
						ubyte*			GetString();				//!< Read a string from the array

						CustomArray&	Reset(udword offset=0);
						void*			GetChunk(const char* chunk);

		private:
						CustomCell*		mCurrentCell;				//!< Current block cell
						CustomCell*		mInitCell;					//!< First block cell

						void*			mCollapsed;					//!< Possible collapsed buffer
						void**			mAddresses;					//!< Stack to store addresses
						void*			mLastAddress;				//!< Last address used in current block cell
						uword			mNbPushedAddies;			//!< Number of saved addies
						uword			mNbAllocatedAddies;			//!< Number of allocated addies

		// Bit storage
						ubyte			mBitCount;					//!< Current number of valid bits in the bitmask
						ubyte			mBitMask;					//!< Current bitmask

		// Management methods
						CustomArray&	Empty();
						CustomArray&	CheckArray(udword bytesneeded);
						CustomArray&	NewBlock(CustomCell* previouscell, udword size=0);
						bool			SaveCell(CustomCell* p, FILE* fp);
						CustomArray&	StoreASCIICode(char code);
						void*			PrepareAccess(udword size);
	};

#endif // __ICECUSTOMARRAY_H__
