#ifndef PROFILING_H
#define PROFILING_H

	__forceinline void	StartProfile(int& val)
	{
#ifdef WIN32
		__asm{
			cpuid
			rdtsc
			mov		ebx, val
			mov		[ebx], eax
		}
#endif
	}

	__forceinline void	EndProfile(int& val)
	{
#ifdef WIN32
		__asm{
			cpuid
			rdtsc
			mov		ebx, val
			sub		eax, [ebx]
			mov		[ebx], eax
		}
#endif
	}

	class Profiler
	{
		public:
								Profiler() : mCycles(0), mTime(0.0f), mNbQueries(0), mMsTime(0.0f)
								{
									QueryPerformanceFrequency((LARGE_INTEGER*)&mFreq);
								}

		inline_	void			Start()
								{
									QueryPerformanceCounter((LARGE_INTEGER*)&mCounter0);
									StartProfile(mCycles);
								}

		inline_	void			End()
								{
									EndProfile(mCycles);
									QueryPerformanceCounter((LARGE_INTEGER*)&mCounter1);
								}

				void			Accum()
								{
									double t = ((double)mCounter1 - (double)mCounter0)/(double)mFreq;
									float mms = (float)t*1000000;

									mTime += mms;
									mNbQueries++;
									if(mNbQueries==100)
									{
										mNbQueries=1;
										mTime = mms;
									}

									mMsTime = mTime/float(mNbQueries);
								}

				void			AddToTweakBar(TwBar* tbar)
								{
									TwAddVarRO(tbar, "Microseconds", TW_TYPE_FLOAT, &mMsTime, " group='Profiling' ");
								}

				__int64			mFreq;
				__int64			mCounter0;
				__int64			mCounter1;
				int				mCycles;
				float			mTime;
				udword			mNbQueries;
				float			mMsTime;
	};

#endif	// PROFILING_H

