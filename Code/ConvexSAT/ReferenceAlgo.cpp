///////////////////////////////////////////////////////////////////////////////
// Convex SAT optimization test - (c) 2007-2008 Pierre Terdiman
// www.codercorner.com
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

static bool TestSepAxis(const btVector3& sep_axis, const MyConvex& hull0, const MyConvex& hull1, float& depth)
{
	float Min0,Max0;
	hull0.Project(sep_axis, Min0, Max0);

	float Min1,Max1;
	hull1.Project(sep_axis, Min1, Max1);

	if(Max0<Min1 || Max1<Min0)
		return false;

	const float d0 = Max0 - Min1;
	assert(d0>=0.0f);
	const float d1 = Max1 - Min0;
	assert(d1>=0.0f);
	depth = d0<d1 ? d0:d1;
	return true;
}

bool ReferenceAlgo(ConvexResults& results, const MyConvex& hull0, const MyConvex& hull1)
{
	float dmin = FLT_MAX;
	btVector3 sep;

	// Test normals from hull0
	for(int i=0;i<hull0.mNbPolys;i++)
	{
		btVector3 Normal(hull0.mPolys[i].mPlane[0], hull0.mPolys[i].mPlane[1], hull0.mPolys[i].mPlane[2]);

		btVector3 WorldNormal = hull0.mTransform.getBasis() * Normal;

		float d;
		if(!TestSepAxis(WorldNormal, hull0, hull1, d))
			return false;

		if(d<dmin)
		{
			dmin = d;
			sep = WorldNormal;
		}
	}

	// Test normals from hull1
	for(int i=0;i<hull1.mNbPolys;i++)
	{
		btVector3 Normal(hull1.mPolys[i].mPlane[0], hull1.mPolys[i].mPlane[1], hull1.mPolys[i].mPlane[2]);

		btVector3 WorldNormal = hull1.mTransform.getBasis() * Normal;

		float d;
		if(!TestSepAxis(WorldNormal, hull0, hull1, d))
			return false;

		if(d<dmin)
		{
			dmin = d;
			sep = WorldNormal;
		}
	}

	// Test edges
	for(int j=0;j<hull0.mNbPolys;j++)
	{
		const MyPoly& poly0 = hull0.mPolys[j];

		for(int i=0;i<hull1.mNbPolys;i++)
		{
			const MyPoly& poly1 = hull1.mPolys[i];

			for(int e0=0;e0<poly0.mNbVerts;e0++)
			{
				const btVector3& a = hull0.mVerts[poly0.mIndices[e0]];
				const btVector3& b = hull0.mVerts[poly0.mIndices[(e0+1)%poly0.mNbVerts]];

				btVector3 edge0 = a - b;
				btVector3 WorldEdge0 = hull0.mTransform.getBasis() * edge0;

				for(int e1=0;e1<poly1.mNbVerts;e1++)
				{
					const btVector3& c = hull1.mVerts[poly1.mIndices[e1]];
					const btVector3& d = hull1.mVerts[poly1.mIndices[(e1+1)%poly1.mNbVerts]];

					btVector3 edge1 = c - d;
					btVector3 WorldEdge1 = hull1.mTransform.getBasis() * edge1;

					btVector3 Cross = WorldEdge0.cross(WorldEdge1);
					if(!IsAlmostZero(Cross))
					{
						Cross = Cross.normalize();

						float d;
						if(!TestSepAxis(Cross, hull0, hull1, d))
							return false;

						if(d<dmin)
						{
							dmin = d;
							sep = Cross;
						}
					}
				}
			}
		}
	}

	const btVector3 DeltaC = hull1.mTransform.getOrigin() - hull0.mTransform.getOrigin();
	if((DeltaC.dot(sep))>0.0f)	sep = -sep;

	results.mDepth	= -dmin;
	results.mNormal	= sep;
	results.mPoint.setValue(0, 0, 0);
	return true;
}
