///////////////////////////////////////////////////////////////////////////////
// Convex SAT optimization test - (c) 2007-2008 Pierre Terdiman
// www.codercorner.com
///////////////////////////////////////////////////////////////////////////////
#ifndef CONVEXALGO_H
#define CONVEXALGO_H

	struct ConvexResults
	{
					ConvexResults();

		btVector3	mNormal;
		btVector3	mPoint;
		float		mDepth;
		void		Reset();
		void		Visualize(MyConvex& convex0, const btVector3& overlap_color, const btVector3& separation_color, bool unit_normal) const;
	};

	typedef bool (*ConvexAlgo)(ConvexResults& results, const MyConvex& hull0, const MyConvex& hull1);

	__forceinline bool IsAlmostZero(const btVector3& v)
	{
		if(fabsf(v.x())>1e-6 || fabsf(v.y())>1e-6 || fabsf(v.z())>1e-6)	return false;
		return true;
	}

#endif // CONVEXALGO_H
