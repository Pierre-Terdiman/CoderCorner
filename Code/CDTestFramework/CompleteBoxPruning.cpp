#include "stdafx.h"
#include "CompleteBoxPruning.h"
#include "RenderingHelpers.h"
#include "GLFontRenderer.h"

CompleteBoxPruningTest::CompleteBoxPruningTest() :
	mBar			(null),
	mNbBoxes		(0),
	mBoxes			(null),
	mBoxPtrs		(null),
	mBoxTime		(null),
	mSpeed			(0.0f),
	mAmplitude		(100.0f)
{
}

CompleteBoxPruningTest::~CompleteBoxPruningTest()
{
	DELETEARRAY(mBoxTime);
	DELETEARRAY(mBoxPtrs);
	DELETEARRAY(mBoxes);
}

void CompleteBoxPruningTest::Init()
{
	mNbBoxes	= 1024;
	mBoxes = new AABB[mNbBoxes];
	mBoxPtrs = new const AABB*[mNbBoxes];
	mBoxTime = new float[mNbBoxes];
	for(udword i=0;i<mNbBoxes;i++)
	{
		Point Center, Extents;

		Center.x = (UnitRandomFloat()-0.5f) * 100.0f;
		Center.y = (UnitRandomFloat()-0.5f) * 10.0f;
		Center.z = (UnitRandomFloat()-0.5f) * 100.0f;
		Extents.x = 2.0f + UnitRandomFloat() * 2.0f;
		Extents.y = 2.0f + UnitRandomFloat() * 2.0f;
		Extents.z = 2.0f + UnitRandomFloat() * 2.0f;

		mBoxes[i].SetCenterExtents(Center, Extents);
		mBoxPtrs[i] = &mBoxes[i];

		mBoxTime[i] = 2000.0f*UnitRandomFloat();
	}
}

void CompleteBoxPruningTest::Release()
{
	DELETEARRAY(mBoxTime);
	DELETEARRAY(mBoxes);
}

void CompleteBoxPruningTest::Select()
{
	// Create a tweak bar
	{
		mBar = TwNewBar("CompleteBoxPruning");
		TwAddVarRW(mBar, "Speed", TW_TYPE_FLOAT, &mSpeed, " min=0.0 max=0.01 step=0.00001");
		TwAddVarRW(mBar, "Amplitude", TW_TYPE_FLOAT, &mAmplitude, " min=10.0 max=200.0 step=0.1");
	}
}

void CompleteBoxPruningTest::Deselect()
{
	if(mBar)
	{
		TwDeleteBar(mBar);
		mBar = null;
	}
}

bool CompleteBoxPruningTest::UpdateBoxes()
{
	for(udword i=0;i<mNbBoxes;i++)
	{
		mBoxTime[i] += mSpeed;

		Point Center,Extents;
		mBoxes[i].GetExtents(Extents);

		Center.x = cosf(mBoxTime[i]*2.17f)*mAmplitude + sinf(mBoxTime[i])*mAmplitude*0.5f;
		Center.y = cosf(mBoxTime[i]*1.38f)*mAmplitude + sinf(mBoxTime[i]*mAmplitude);
		Center.z = sinf(mBoxTime[i]*0.777f)*mAmplitude;

		mBoxes[i].SetCenterExtents(Center, Extents);
	}
	return true;
}

void CompleteBoxPruningTest::PerformTest()
{
	UpdateBoxes();

	mPairs.ResetPairs();
	mProfiler.Start();
	CompleteBoxPruning(mNbBoxes, mBoxPtrs, mPairs, Axes(AXES_XZY));
	mProfiler.End();
	mProfiler.Accum();

//	printf("%d pairs colliding\r     ", mPairs.GetNbPairs());

	bool* Flags = (bool*)_alloca(sizeof(bool)*mNbBoxes);
	ZeroMemory(Flags, sizeof(bool)*mNbBoxes);
	const Pair* P = mPairs.GetPairs();
	for(udword i=0;i<mPairs.GetNbPairs();i++)
	{
		Flags[P[i].id0] = true;
		Flags[P[i].id1] = true;
	}

	// Render boxes
	OBB CurrentBox;
	CurrentBox.mRot.Identity();
	for(udword i=0;i<mNbBoxes;i++)
	{
		if(Flags[i])	glColor3f(1.0f, 0.0f, 0.0f);
		else			glColor3f(0.0f, 1.0f, 0.0f);
		mBoxes[i].GetCenter(CurrentBox.mCenter);
		mBoxes[i].GetExtents(CurrentBox.mExtents);
		DrawOBB(CurrentBox);
	}

	char Buffer[4096];
	sprintf(Buffer, "CompleteBoxPruning - %5.1f us (%d cycles) - %d pairs\n", mProfiler.mMsTime, mProfiler.mCycles, mPairs.GetNbPairs());
	GLFontRenderer::print(10.0f, 10.0f, 0.02f, Buffer);
}

void CompleteBoxPruningTest::KeyboardCallback(unsigned char key, int x, int y)
{
}

void CompleteBoxPruningTest::MouseCallback(int button, int state, int x, int y)
{
}

void CompleteBoxPruningTest::MotionCallback(int x, int y)
{
}
