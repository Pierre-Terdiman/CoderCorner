#ifndef SPHEREMESHQUERY_H
#define SPHEREMESHQUERY_H

#include "CollisionTest.h"
#include "Profiling.h"

	class SphereMeshQuery : public CollisionTest
	{
		public:
								SphereMeshQuery();
		virtual					~SphereMeshQuery();

		virtual	void			Init();
		virtual	void			Release();
		virtual	void			PerformTest();
		virtual	void			Select();
		virtual	void			Deselect();
		virtual	void			KeyboardCallback(unsigned char key, int x, int y);
		virtual	void			MouseCallback(int button, int state, int x, int y);
		virtual	void			MotionCallback(int x, int y);

				TwBar*			mBar;		//!< AntTweakBar
				Sphere			mSphere;

				SphereCache		mCache;
				OpcodeSettings	mSettings;
				Profiler		mProfiler;

				float			mDist;
				Point			mLocalHit;
				bool			mValidHit;
	};

#endif	// SPHEREMESHQUERY_H
