// Sweep-and-prune test file
#include "stdafx.h"
using namespace Opcode;

#include <conio.h>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#define USE_RDTSC

//#define NB_OBJECTS			100
//#define NB_MOVING_OBJECTS	100

//#define NB_OBJECTS			10000
//#define NB_MOVING_OBJECTS	10000

//#define NB_OBJECTS			10000
//#define NB_MOVING_OBJECTS	10

//#define NB_OBJECTS			10000
//#define NB_MOVING_OBJECTS	100

#define NB_OBJECTS			4000
#define NB_MOVING_OBJECTS	4000

//#define NB_OBJECTS			400
//#define NB_MOVING_OBJECTS	400

//#define NB_OBJECTS			4000
//#define NB_MOVING_OBJECTS	40

//#define NB_OBJECTS			1000
//#define NB_MOVING_OBJECTS	1000

//static const float Speed = 0.0001f;
static const float Speed = 0.001f;
//static const float Speed = 0.01f;

	inline_ void	StartProfile(udword& val)
	{
#ifdef USE_RDTSC
		__asm{
			cpuid
			rdtsc
			mov		ebx, val
			mov		[ebx], eax
		}
#else
		val = timeGetTime();
#endif
	}

	inline_ void	EndProfile(udword& val)
	{
#ifdef USE_RDTSC
		__asm{
			cpuid
			rdtsc
			mov		ebx, val
			sub		eax, [ebx]
			mov		[ebx], eax
		}
#else
		val = timeGetTime() - val;
#endif
	}

// Returns a random floating-point value f such as 0.0f <= f <= 1.0f
inline_ float UnitRandomFloat()	{ return float(rand()) * ONE_OVER_RAND_MAX;	}

// Creates a random AABB
static void GetRandomBox(AABB& box)
{
//	const float Size = 49.0f;
//	const float Size = 19.0f;
	const float Size = 9.0f;
	float cx = (UnitRandomFloat()-0.5f)*1000.0f;
	float cy = (UnitRandomFloat()-0.5f)*1000.0f;
//	float cy = (UnitRandomFloat()-0.5f)*10.0f;
	float cz = (UnitRandomFloat()-0.5f)*1000.0f;
	const float ex = 1.0f + UnitRandomFloat()*Size;
	const float ey = 1.0f + UnitRandomFloat()*Size;
	const float ez = 1.0f + UnitRandomFloat()*Size;

	box.mMin.x = cx - ex;
	box.mMin.y = cy - ey;
	box.mMin.z = cz - ez;
	box.mMax.x = cx + ex;
	box.mMax.y = cy + ey;
	box.mMax.z = cz + ez;
}

static const float Amplitude = 1000.0f;
static void UpdateBox(AABB& box, float time)
{
	Point Center, Extents;
	Center.x	= (box.mMax.x + box.mMin.x)*0.5f;
	Center.y	= (box.mMax.y + box.mMin.y)*0.5f;
	Center.z	= (box.mMax.z + box.mMin.z)*0.5f;
	Extents.x	= (box.mMax.x - box.mMin.x)*0.5f;
	Extents.y	= (box.mMax.y - box.mMin.y)*0.5f;
	Extents.z	= (box.mMax.z - box.mMin.z)*0.5f;

	Center.x = cosf(time*2.17f)*Amplitude+sinf(time)*Amplitude*0.5f;
	Center.y = cosf(time*1.38f)*Amplitude+sinf(time*Amplitude);
	Center.z = sinf(time*0.777f)*Amplitude;

	box.mMin.x = Center.x - Extents.x;
	box.mMin.y = Center.y - Extents.y;
	box.mMin.z = Center.z - Extents.z;
	box.mMax.x = Center.x + Extents.x;
	box.mMax.y = Center.y + Extents.y;
	box.mMax.z = Center.z + Extents.z;
}

static udword gNbCreatedPairs;
static udword gNbDeletedPairs;
static udword gTotalNbPairs;

static void* CBData = (void*)0x12345678;
static void* PairUserData = (void*)0xDeadDead;

static void* CreatePairCB(const void* object0, const void* object1, void* user_data)
{
	assert(user_data==CBData);

	gNbCreatedPairs++;
	return PairUserData;
}

static void DeletePairCB(const void* object0, const void* object1, void* user_data, void* pair_user_data)
{
	assert(user_data==CBData);
	assert(pair_user_data==PairUserData);

	gNbDeletedPairs++;
}

int main()
{
	ArraySAP ASAP;

	// Create NB_OBJECTS random objects

	// First, create all boxes
	AABB* BoxList = ICE_NEW_TMP(AABB)[NB_OBJECTS];
	float BoxTimes[NB_OBJECTS];
	for(udword i=0;i<NB_OBJECTS;i++)
	{
		GetRandomBox(BoxList[i]);

		BoxTimes[i] = 2000.0f*float(rand())*ONE_OVER_RAND_MAX;
		UpdateBox(BoxList[i], BoxTimes[i]);
	}

	// Then create objects
	void** Handles = new void*[NB_OBJECTS];
	if(1)
	{
		DWORD t = timeGetTime();
		for(int i=0;i<NB_OBJECTS;i++)
		{
			// It is mandatory to pass a valid pointer as a first parameter. This is supposed to be the game object
			// associated with the AABB. In this small example I just pass a pointer to the SAP itself.
			Handles[i] = (void*)ASAP.AddObject(&ASAP, i, BoxList[i]);
		}
		udword MyNbInitialPairs = ASAP.DumpPairs(CreatePairCB, DeletePairCB, CBData);
		gTotalNbPairs = MyNbInitialPairs;

		t = timeGetTime() - t;
		printf("SAP creation time: %d (%d initial pairs)\n", t, MyNbInitialPairs);
	}

	///////////////

	// Run simulation loop
	if(1)
	{
		while(_kbhit()==0)
		{
			gNbCreatedPairs = 0;
			gNbDeletedPairs = 0;
			udword TotalTime=0;
			udword Time;

			for(int i=0;i<NB_MOVING_OBJECTS;i++)
			{
				BoxTimes[i] += Speed;

				// Move boxes
				UpdateBox(BoxList[i], BoxTimes[i]);

				// Update SAP
				StartProfile(Time);
					ASAP.UpdateObject((udword)Handles[i], BoxList[i]);
				EndProfile(Time);
				TotalTime += Time;
			}

			StartProfile(Time);
				ASAP_Pair* Pairs;
				udword NbPairs = ASAP.DumpPairs(CreatePairCB, DeletePairCB, CBData, &Pairs);
			EndProfile(Time);
			TotalTime += Time;

			gTotalNbPairs += gNbCreatedPairs;
			gTotalNbPairs -= gNbDeletedPairs;
			assert(gTotalNbPairs==NbPairs);

			printf("%d total | %d (+) | %d (-)| %d\n", NbPairs, gNbCreatedPairs, gNbDeletedPairs, TotalTime/1024);
		}
	}

	// Release memory & exit
	DELETEARRAY(Handles);
	DELETEARRAY(BoxList);
	return 0;
}
