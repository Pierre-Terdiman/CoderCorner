#include "stdafx.h"
#include "RenderingHelpers.h"
#include "IceHelpers.h"
#include "SphereMeshQuery.h"
#include "Terrain.h"
#include "Profiling.h"
#include "Camera.h"
#include "GLFontRenderer.h"

SphereMeshQuery::SphereMeshQuery() :
	mBar			(null),
	mDist			(0.0f),
	mValidHit		(false)
{
}

SphereMeshQuery::~SphereMeshQuery()
{
}

void SphereMeshQuery::Init()
{
	mSphere.mCenter = Point(0.0f, 0.0f, 0.0f);
	mSphere.mRadius = 1.0f;
}

void SphereMeshQuery::Release()
{
	Deselect();
}

void SphereMeshQuery::Select()
{
	// Create a tweak bar
	{
		mBar = TwNewBar("SphereMeshQuery");
		TwAddVarRW(mBar, "Radius", TW_TYPE_FLOAT, &mSphere.mRadius, " min=0.01 max=200.0 step=0.05");

		mSettings.AddToTweakBar(mBar);
//		mProfiler.AddToTweakBar(mBar);
	}
}

void SphereMeshQuery::Deselect()
{
	if(mBar)
	{
		TwDeleteBar(mBar);
		mBar = null;
	}
}

void SphereMeshQuery::PerformTest()
{
	RenderTerrain();

	DrawSphere(mSphere);

	// OPCODE query
	const Model* TM = GetTerrainModel();
	if(TM)
	{
		SphereCollider Collider;
		mSettings.SetupCollider(Collider);

		mProfiler.Start();
		bool Status = Collider.Collide(mCache, mSphere, *TM, null, null);
		mProfiler.End();
		mProfiler.Accum();

		if(Status)
		{
			if(Collider.GetContactStatus())
			{
				udword NbTris = Collider.GetNbTouchedPrimitives();
				const udword* Indices = Collider.GetTouchedPrimitives();

				RenderTerrainTriangles(NbTris, Indices);
			}
		}
	}

	// Raycast hit
	if(mValidHit)
	{
		Point wp = mLocalHit + mSphere.mCenter;
		DrawLine(wp, wp + Point(1.0f, 0.0f, 0.0f), Point(1,0,0), 1.0f);
		DrawLine(wp, wp + Point(0.0f, 1.0f, 0.0f), Point(0,1,0), 1.0f);
		DrawLine(wp, wp + Point(0.0f, 0.0f, 1.0f), Point(0,0,1), 1.0f);
	}

	char Buffer[4096];
	sprintf(Buffer, "Sphere-mesh query = %5.1f us (%d cycles)\n", mProfiler.mMsTime, mProfiler.mCycles);
	GLFontRenderer::print(10.0f, 10.0f, 0.02f, Buffer);
}

void SphereMeshQuery::KeyboardCallback(unsigned char key, int x, int y)
{
}

void SphereMeshQuery::MouseCallback(int button, int state, int x, int y)
{
	mValidHit = false;
	if(!button && !state)
	{
		Point Dir = ComputeWorldRay(x, y);

		float d;
		Point hit;
		if(SegmentSphere(GetCameraPos(), Dir, 10000.0f, mSphere.mCenter, mSphere.mRadius, d, hit))
		{
			mValidHit = true;
			mDist = d;
			mLocalHit = hit - mSphere.mCenter;
		}
	}
}

void SphereMeshQuery::MotionCallback(int x, int y)
{
	if(mValidHit)
	{
		Point Dir = ComputeWorldRay(x, y);
		mSphere.mCenter = GetCameraPos() + Dir*mDist - mLocalHit;
	}
}
