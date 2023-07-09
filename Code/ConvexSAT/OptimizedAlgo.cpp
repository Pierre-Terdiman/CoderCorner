///////////////////////////////////////////////////////////////////////////////
// Convex SAT optimization test - (c) 2007-2008 Pierre Terdiman
// www.codercorner.com
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

class Plane
{
	public:

	__forceinline void	Set(const btVector3& p, const btVector3& _n)
	{
		n = _n;
		d = - p.dot(_n);
	}

	__forceinline bool	IsCulled(const btVector3& v)	const
	{
		return ((v.dot(n)) + d)>0.0f;
	}

	btVector3	n;
	float		d;
};

#define NB_ENTRIES	512
class PartialHull
{
	public:
	__forceinline	PartialHull()	{}
	__forceinline	~PartialHull()	{}
	void			Project(const btVector3& dir, float& min, float& max) const;
	bool			AddEdge(const btVector3& axis);

	bool			TestFaces(const PartialHull& hull1, float& dmin, btVector3& sep) const;

	int				mNbKeptVertices;
	int				mNbKeptEdges;
	int				mNbKeptFaces;
	btVector3		mKeptVertices[NB_ENTRIES];
	btVector3		mKeptFaces[NB_ENTRIES];
	btVector3		mKeptEdges[NB_ENTRIES];
};

bool PartialHull::AddEdge(const btVector3& axis)
{
	btVector3 RealAxis;
	if(axis.x()<0.0f)	RealAxis = -axis;
	else				RealAxis = axis;

	int NbAxes = mNbKeptEdges;
	const btVector3* Axes = mKeptEdges;
	while(NbAxes--)
	{
		if(fabsf(RealAxis.dot(*Axes))>0.9999f)	return false;
		Axes++;
	}
	mKeptEdges[mNbKeptEdges++] = RealAxis;
	return true;
}

void PartialHull::Project(const btVector3& dir, float& min, float& max) const
{
	min = FLT_MAX;
	max = -FLT_MAX;
	int NbToGo = mNbKeptVertices;
	const btVector3* KV = mKeptVertices;
	while(NbToGo--)
	{
		const float dp = (*KV++).dot(dir);
		if(dp < min)	min = dp;
		if(dp > max)	max = dp;
	}
}

static bool TestSepAxis(const btVector3& sep_axis, const PartialHull& hull0, const PartialHull& hull1, float& depth)
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

bool PartialHull::TestFaces(const PartialHull& hull, float& dmin, btVector3& sep) const
{
	int NbToGo = mNbKeptFaces;
	const btVector3* KF = mKeptFaces;
	while(NbToGo--)
	{
		const btVector3& WorldNormal = *KF++;

		float d;
		if(!TestSepAxis(WorldNormal, *this, hull, d))
			return false;

		if(d<dmin)
		{
			dmin = d;
			sep = WorldNormal;
		}
	}
	return true;
}

static bool TestEdges(const PartialHull& hull0, const PartialHull& hull1, float& dmin, btVector3& sep)
{
	// Test edges
	int NbEdges0 = hull0.mNbKeptEdges;
	const btVector3* WorldEdges0 = hull0.mKeptEdges;
	while(NbEdges0--)
	{
		const btVector3& WorldEdge0 = *WorldEdges0++;

		int NbEdges1 = hull1.mNbKeptEdges;
		const btVector3* WorldEdges1 = hull1.mKeptEdges;
		while(NbEdges1--)
		{
			btVector3 Cross = WorldEdge0.cross(*WorldEdges1++);
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
	return true;
}

static void Cull(const btVector3& witness, PartialHull& partial, const Plane& world_plane, const MyConvex& hull)
{
	const int NbVerts = hull.mNbVerts;
	const btVector3* Verts = hull.mVerts;

	partial.mNbKeptVertices	= 0;
	partial.mNbKeptEdges	= 0;
	partial.mNbKeptFaces	= 0;

	bool* VertexFlags = (bool*)_alloca(sizeof(bool)*NbVerts);
	memset(VertexFlags, 0, sizeof(bool)*NbVerts);

	// Loop through polygons
	const MyPoly* CurrentPolys = hull.mPolys;
	int NbPolys = hull.mNbPolys;
	while(NbPolys--)
	{
		const MyPoly& CurrentPoly = *CurrentPolys++;
		bool CulledPolygon = true;
		// Loop through polygon's edges
		const int NbEdges = CurrentPoly.mNbVerts;
		const char* Indices = CurrentPoly.mIndices;
		for(int j=0;j<NbEdges;j++)
		{
			// Vertex indices
			const int v0 = Indices[j];
			const int v1 = Indices[(j+1)==CurrentPoly.mNbVerts ? 0 : (j+1)];

			// Edge in world space
			const btVector3 wa = hull.mTransform * Verts[v0];
			const btVector3 wb = hull.mTransform * Verts[v1];

			// If the edge is completely culled, discard it.
			// Else keep *both* vertices for projection, and the edge as well.
			if(!world_plane.IsCulled(wa) || !world_plane.IsCulled(wb))
			{
				// Save vertices
				if(!VertexFlags[v0])
				{
					VertexFlags[v0] = true;
					partial.mKeptVertices[partial.mNbKeptVertices++] = wa;
				}
				if(!VertexFlags[v1])
				{
					VertexFlags[v1] = true;
					partial.mKeptVertices[partial.mNbKeptVertices++] = wb;
				}

				// Save edge
				partial.AddEdge((wb - wa).normalized());

				CulledPolygon = false;
			}
		}

		if(!CulledPolygon)
		{
			// Compute polygon normal in world space
			const btVector3 Normal(CurrentPoly.mPlane[0], CurrentPoly.mPlane[1], CurrentPoly.mPlane[2]);
			const btVector3 WorldNormal = hull.mTransform.getBasis() * Normal;
			partial.mKeptFaces[partial.mNbKeptFaces++] = WorldNormal;
		}
	}
}

bool OptimizedReferenceAlgo(ConvexResults& results, const MyConvex& hull0, const MyConvex& hull1)
{
	// 1) find relevant vertices, edges and faces
	// 2) only test those face normals and those edges, using only those projected vertices

	const btVector3 wc0 = hull0.mTransform * hull0.mLocalCenter;
	const btVector3 wc1 = hull1.mTransform * hull1.mLocalCenter;
	btVector3 DeltaC = wc1 - wc0;

	DeltaC = DeltaC.normalized();

	const btVector3 ws0 = hull0.WorldSupport(DeltaC);
	const btVector3 ws1 = hull1.WorldSupport(-DeltaC);

	PartialHull PH0;
	PartialHull PH1;

	Plane PL0;	// Witness plane on object 0
	Plane PL1;	// Witness plane on object 1
	PL0.Set(ws0, DeltaC);
	PL1.Set(ws1, -DeltaC);

	Cull(-DeltaC, PH0, PL1, hull0);
	if(!PH0.mNbKeptVertices && !PH0.mNbKeptEdges && !PH0.mNbKeptFaces)
		return false;

	Cull(DeltaC, PH1, PL0, hull1);
	if(!PH1.mNbKeptVertices && !PH1.mNbKeptEdges && !PH1.mNbKeptFaces)
		return false;

	float dmin = FLT_MAX;
	btVector3 sep;

	if(!PH0.TestFaces(PH1, dmin, sep))
		return false;

	if(!PH1.TestFaces(PH0, dmin, sep))
		return false;

	if(!TestEdges(PH0, PH1, dmin, sep))
		return false;

	if((DeltaC.dot(sep))>0.0f)
		sep = -sep;

	results.mDepth	= -dmin;
	results.mNormal	= sep;
	results.mPoint.setValue(0, 0, 0);
	return true;
}

