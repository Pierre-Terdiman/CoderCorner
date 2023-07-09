#include "stdafx.h"
#include "IceHelpers.h"
#include "RenderingHelpers.h"
#include "Terrain.h"
#include "CapsuleMeshQuery.h"
#include "Camera.h"
#include "GLFontRenderer.h"

CapsuleMeshQuery::CapsuleMeshQuery() :
	mBar			(null),
	mDist			(0.0f),
	mValidHit		(false)
{
}

CapsuleMeshQuery::~CapsuleMeshQuery()
{
}

void CapsuleMeshQuery::Init()
{
	mP0 = Point(0.0f, -4.0f, 0.0f);
	mP1 = Point(0.0f, 4.0f, 0.0f);

	Matrix3x3 MX,MY;
	RotX(MX, 45.0f);
	RotY(MY, 45.0f);

	mWorld = MX * MY;
	mWorld.SetTrans(0.0f, 4.0f, 0.0f);
}

void CapsuleMeshQuery::Release()
{
}

void CapsuleMeshQuery::PerformTest()
{
	RenderTerrain();

	mCapsule.mP0 = mP0 * mWorld;
	mCapsule.mP1 = mP1 * mWorld;
	mCapsule.mRadius = 1.0f;
	DrawCapsule(mWorld, mP0, mP1, 1.0f);

	const Model* TM = GetTerrainModel();
	if(TM)
	{
		LSSCollider Collider;

		mProfiler.Start();
		bool Status = Collider.Collide(mCache, mCapsule, *TM, null, null);
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
		Point wp = mLocalHit + (Point)mWorld.GetTrans();
		DrawLine(wp, wp + Point(1.0f, 0.0f, 0.0f), Point(1,0,0), 1.0f);
		DrawLine(wp, wp + Point(0.0f, 1.0f, 0.0f), Point(0,1,0), 1.0f);
		DrawLine(wp, wp + Point(0.0f, 0.0f, 1.0f), Point(0,0,1), 1.0f);
	}

	char Buffer[4096];
	sprintf(Buffer, "Capsule-mesh query = %5.1f us (%d cycles)\n", mProfiler.mMsTime, mProfiler.mCycles);
	GLFontRenderer::print(10.0f, 10.0f, 0.02f, Buffer);
}

void CapsuleMeshQuery::KeyboardCallback(unsigned char key, int x, int y)
{
}

void CapsuleMeshQuery::MouseCallback(int button, int state, int x, int y)
{
	mValidHit = false;
	if(!button && !state)
	{
		Point Dir = ComputeWorldRay(x, y);

		float s[2];
		if(RayCapsuleOverlap(GetCameraPos(), Dir, mCapsule, s))
		{
			mValidHit = true;
			mDist = s[0];
			Point hit = GetCameraPos() + Dir * mDist;
			mLocalHit = hit - (Point)mWorld.GetTrans();
		}
	}
}

void CapsuleMeshQuery::MotionCallback(int x, int y)
{
	if(mValidHit)
	{
		Point Dir = ComputeWorldRay(x, y);
		mWorld.SetTrans(GetCameraPos() + Dir*mDist - mLocalHit);
	}
}
