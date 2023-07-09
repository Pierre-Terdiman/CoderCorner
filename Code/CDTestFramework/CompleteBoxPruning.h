#ifndef COMPLETEBOXPRUNING_H
#define COMPLETEBOXPRUNING_H

#include "CollisionTest.h"
#include "Profiling.h"

	class CompleteBoxPruningTest : public CollisionTest
	{
		public:
								CompleteBoxPruningTest();
		virtual					~CompleteBoxPruningTest();

		virtual	void			Init();
		virtual	void			Release();
		virtual	void			PerformTest();
		virtual	void			Select();
		virtual	void			Deselect();
		virtual	void			KeyboardCallback(unsigned char key, int x, int y);
		virtual	void			MouseCallback(int button, int state, int x, int y);
		virtual	void			MotionCallback(int x, int y);

				TwBar*			mBar;		//!< AntTweakBar
				Profiler		mProfiler;

				udword			mNbBoxes;
				AABB*			mBoxes;
				const AABB**	mBoxPtrs;
				Pairs			mPairs;
				float*			mBoxTime;
				float			mSpeed;
				float			mAmplitude;
		private:
				bool			UpdateBoxes();
	};

#endif	// COMPLETEBOXPRUNING_H
