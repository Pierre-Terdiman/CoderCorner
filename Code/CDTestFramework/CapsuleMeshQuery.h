#ifndef CAPSULEMESHQUERY_H
#define CAPSULEMESHQUERY_H

#include "CollisionTest.h"
#include "Profiling.h"

	class CapsuleMeshQuery : public CollisionTest
	{
		public:
								CapsuleMeshQuery();
		virtual					~CapsuleMeshQuery();

		virtual	void			Init();
		virtual	void			Release();
		virtual	void			PerformTest();
		virtual	void			KeyboardCallback(unsigned char key, int x, int y);
		virtual	void			MouseCallback(int button, int state, int x, int y);
		virtual	void			MotionCallback(int x, int y);

				TwBar*			mBar;		//!< AntTweakBar
				Profiler		mProfiler;

				Point			mP0;
				Point			mP1;
				Matrix4x4		mWorld;
				LSS				mCapsule;
				LSSCache		mCache;

				float			mDist;
				Point			mLocalHit;
				bool			mValidHit;
	};

#endif	// CAPSULEMESHQUERY_H
