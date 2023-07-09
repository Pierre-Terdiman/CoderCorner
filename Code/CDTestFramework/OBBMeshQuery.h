#ifndef OBBMESHQUERY_H
#define OBBMESHQUERY_H

#include "CollisionTest.h"
#include "Profiling.h"

	class OBBMeshQuery : public CollisionTest
	{
		public:
								OBBMeshQuery();
		virtual					~OBBMeshQuery();

		virtual	void			Init();
		virtual	void			Release();
		virtual	void			PerformTest();
		virtual	void			Select();
		virtual	void			Deselect();
		virtual	void			KeyboardCallback(unsigned char key, int x, int y);
		virtual	void			MouseCallback(int button, int state, int x, int y);
		virtual	void			MotionCallback(int x, int y);

				TwBar*			mBar;		//!< AntTweakBar
				OBB				mBox;

				float			mAngleX;
				float			mAngleY;
				float			mAngleZ;

				OBBCache		mCache;
				OpcodeSettings	mSettings;
				Profiler		mProfiler;

				float			mDist;
				Point			mLocalHit;
				bool			mValidHit;
	};

#endif	// OBBMESHQUERY_H
