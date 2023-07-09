
typedef unsigned int udword;

//! Integer representation of a floating-point value.
#define IR(x)	((udword&)x)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	A method to compute a ray-AABB intersection.
 *	Original code by Andrew Woo, from "Graphics Gems", Academic Press, 1990
 *	Optimized code by Pierre Terdiman, 2000 (~20-30% faster on my Celeron 500)
 *	Epsilon value added by Klaus Hartmann. (discarding it saves a few cycles only)
 *
 *	Hence this version is faster as well as more robust than the original one.
 *
 *	Should work provided:
 *	1) the integer representation of 0.0f is 0x00000000
 *	2) the sign bit of the float is the most significant one
 *
 *	Report bugs: p.terdiman@codercorner.com
 *
 *	\param		aabb		[in] the axis-aligned bounding box
 *	\param		origin		[in] ray origin
 *	\param		dir			[in] ray direction
 *	\param		coord		[out] impact coordinates
 *	\return		true if ray intersects AABB
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define RAYAABB_EPSILON 0.00001f
bool Meshmerizer::RayAABB(const AABB& aabb, const Point& origin, const Point& dir, Point& coord)
{
	BOOL Inside = TRUE;
	Point MinB = aabb.mCenter - aabb.mExtents;
	Point MaxB = aabb.mCenter + aabb.mExtents;
	Point MaxT;
	MaxT.x=MaxT.y=MaxT.z=-1.0f;

	// Find candidate planes.
	for(udword i=0;i<3;i++)
	{
		if(origin.m[i] < MinB.m[i])
		{
			coord.m[i]	= MinB.m[i];
			Inside		= FALSE;

			// Calculate T distances to candidate planes
			if(IR(dir.m[i]))	MaxT.m[i] = (MinB.m[i] - origin.m[i]) / dir.m[i];
		}
		else if(origin.m[i] > MaxB.m[i])
		{
			coord.m[i]	= MaxB.m[i];
			Inside		= FALSE;

			// Calculate T distances to candidate planes
			if(IR(dir.m[i]))	MaxT.m[i] = (MaxB.m[i] - origin.m[i]) / dir.m[i];
		}
	}

	// Ray origin inside bounding box
	if(Inside)
	{
		coord = origin;
		return true;
	}

	// Get largest of the maxT's for final choice of intersection
	udword WhichPlane = 0;
	if(MaxT.m[1] > MaxT.m[WhichPlane])	WhichPlane = 1;
	if(MaxT.m[2] > MaxT.m[WhichPlane])	WhichPlane = 2;

	// Check final candidate actually inside box
	if(IR(MaxT.m[WhichPlane])&0x80000000) return false;

	for(i=0;i<3;i++)
	{
		if(i!=WhichPlane)
		{
			coord.m[i] = origin.m[i] + MaxT.m[WhichPlane] * dir.m[i];
#ifdef RAYAABB_EPSILON
			if(coord.m[i] < MinB.m[i] - RAYAABB_EPSILON || coord.m[i] > MaxB.m[i] + RAYAABB_EPSILON)	return false;
#else
			if(coord.m[i] < MinB.m[i] || coord.m[i] > MaxB.m[i])	return false;
#endif
		}
	}
	return true;	// ray hits box
}





/*
	Updated October, 5, 2001 :
		- Below is an alternative version, up to ~25% faster than the one above on my Celeron.
		- Not fully tested so handle with care. It's not that much different anyway.

	Updated October, 9, 2001:
		- Fixed a slight bug......
		- Compiles for (Min, Max) or (Center, Extents) boxes
		- I did some tests with Adam Moravanszky, and the fastest version apparently depends on the machine. See for yourself.
		- You should also take a look at Tim Schröder's version in GDMag. It's sometimes faster thanks to early exits, sometimes
		slower (at least on my machine!).
*/



#ifndef USE_MINMAX
	// Unroll loop, do the div early to let it pair with CPU code
	#define FIND_CANDIDATE_PLANE(i)											\
		if(origin[i] < MinB[i])												\
		{																	\
			/* Calculate T distances to candidate planes */					\
			if(IR(dir[i]))	MaxT[i] = (MinB[i] - origin[i]) / dir[i];		\
																			\
			Inside		= FALSE;											\
			coord[i]	= MinB[i];											\
		}																	\
		else if(origin[i] > MaxB[i])										\
		{																	\
			/* Calculate T distances to candidate planes */					\
			if(IR(dir[i]))	MaxT[i] = (MaxB[i] - origin[i]) / dir[i];		\
																			\
			Inside		= FALSE;											\
			coord[i]	= MaxB[i];											\
		}
#else
	#define FIND_CANDIDATE_PLANE(i)											\
		if(origin[i] < aabb.GetMin(i))										\
		{																	\
			/* Calculate T distances to candidate planes */					\
			if(IR(dir[i]))	MaxT[i] = (aabb.GetMin(i) - origin[i]) / dir[i];\
																			\
			Inside		= FALSE;											\
			coord[i]	= aabb.GetMin(i);									\
		}																	\
		else if(origin[i] > aabb.GetMax(i))									\
		{																	\
			/* Calculate T distances to candidate planes */					\
			if(IR(dir[i]))	MaxT[i] = (aabb.GetMax(i) - origin[i]) / dir[i];\
																			\
			Inside		= FALSE;											\
			coord[i]	= aabb.GetMax(i);									\
		}
#endif

// Unroll loop
#ifndef USE_MINMAX

	#ifndef RAYAABB_EPSILON
		#define COMPUTE_INTERSECT(i)											\
			if(i!=WhichPlane)													\
			{																	\
				coord[i] = origin[i] + MaxT[WhichPlane] * dir[i];				\
				if(coord[i] < MinB[i] || coord[i] > MaxB[i])	return false;	\
			}
	#else
		#define COMPUTE_INTERSECT(i)											\
			if(i!=WhichPlane)													\
			{																	\
				coord[i] = origin[i] + MaxT[WhichPlane] * dir[i];				\
				if(coord[i] < MinB[i] - RAYAABB_EPSILON || coord[i] > MaxB[i] + RAYAABB_EPSILON)	return false;	\
			}
	#endif

#else

	#ifndef RAYAABB_EPSILON
		#define COMPUTE_INTERSECT(i)											\
			if(i!=WhichPlane)													\
			{																	\
				coord[i] = origin[i] + MaxT[WhichPlane] * dir[i];				\
				if(coord[i] < aabb.GetMin(i) || coord[i] > aabb.GetMax(i))	return false;	\
			}
	#else
		#define COMPUTE_INTERSECT(i)											\
			if(i!=WhichPlane)													\
			{																	\
				coord[i] = origin[i] + MaxT[WhichPlane] * dir[i];				\
				if(coord[i] < aabb.GetMin(i) - RAYAABB_EPSILON || coord[i] > aabb.GetMax(i) + RAYAABB_EPSILON)	return false;	\
			}
	#endif

#endif

bool Meshmerizer::RayAABB4(const AABB& aabb, const Point& origin, const Point& dir, Point& coord)
{
	BOOL Inside = TRUE;
#ifndef USE_MINMAX
	Point MinB; aabb.GetMin(MinB);
	Point MaxB; aabb.GetMax(MaxB);
#endif
	Point MaxT;
	MaxT.x=MaxT.y=MaxT.z=-1.0f;

	// Find candidate planes.
	FIND_CANDIDATE_PLANE(0)
	FIND_CANDIDATE_PLANE(1)
	FIND_CANDIDATE_PLANE(2)

	// Ray origin inside bounding box
	if(Inside)
	{
		coord = origin;
		return true;
	}

	// Get largest of the MaxT's for final choice of intersection
	// - this version without FPU compares
	// - but branch prediction might suffer
	// - a bit faster on my Celeron, duno how it behaves on something like a P4
	udword WhichPlane;
	if(IR(MaxT[0])&0x80000000)
	{
		// T[0]<0
		if(IR(MaxT[1])&0x80000000)
		{
			// T[0]<0
			// T[1]<0
			if(IR(MaxT[2])&0x80000000)
			{
				// T[0]<0
				// T[1]<0
				// T[2]<0
				return false;
			}
			else
			{
				WhichPlane = 2;
			}
		}
		else if(IR(MaxT[2])&0x80000000)
		{
			// T[0]<0
			// T[1]>0
			// T[2]<0
			WhichPlane = 1;
		}
		else
		{
			// T[0]<0
			// T[1]>0
			// T[2]>0
			if(IR(MaxT[2]) > IR(MaxT[1]))
			{
				WhichPlane = 2;
			}
			else
			{
				WhichPlane = 1;
			}
		}
	}
	else
	{
		// T[0]>0
		if(IR(MaxT[1])&0x80000000)
		{
			// T[0]>0
			// T[1]<0
			if(IR(MaxT[2])&0x80000000)
			{
				// T[0]>0
				// T[1]<0
				// T[2]<0
				WhichPlane = 0;
			}
			else
			{
				// T[0]>0
				// T[1]<0
				// T[2]>0
				if(IR(MaxT[2]) > IR(MaxT[0]))
				{
					WhichPlane = 2;
				}
				else
				{
					WhichPlane = 0;
				}
			}
		}
		else if(IR(MaxT[2])&0x80000000)
		{
			// T[0]>0
			// T[1]>0
			// T[2]<0
			if(IR(MaxT[1]) > IR(MaxT[0]))
			{
				WhichPlane = 1;
			}
			else
			{
				WhichPlane = 0;
			}
		}
		else
		{
			// T[0]>0
			// T[1]>0
			// T[2]>0
			WhichPlane = 0;
			if(IR(MaxT[1]) > IR(MaxT[WhichPlane]))	WhichPlane = 1;
			if(IR(MaxT[2]) > IR(MaxT[WhichPlane]))	WhichPlane = 2;
		}
	}

	// Old code below:
/*
	// Get largest of the maxT's for final choice of intersection
	udword WhichPlane = 0;
	if(MaxT[1] > MaxT[WhichPlane])	WhichPlane = 1;
	if(MaxT[2] > MaxT[WhichPlane])	WhichPlane = 2;

	// Check final candidate actually inside box
	if(IR(MaxT[WhichPlane])&0x80000000) return false;
*/

	COMPUTE_INTERSECT(0)
	COMPUTE_INTERSECT(1)
	COMPUTE_INTERSECT(2)

	return true;	// ray hits box
}

// Versions usint the SAT. Original code for OBBs from MAGIC.
// Rewritten for AABBs and reorganized for early exits.

// For a segment
bool Meshmerizer::SegmentAABB(const Segment& segment, const AABB& aabb)
{
	Point BoxExtents, Diff, Dir;
	float fAWdU[3];

	Dir.x = 0.5f * (segment.mP1.x - segment.mP0.x);
	BoxExtents.x = aabb.GetExtents(0);
	Diff.x = (0.5f * (segment.mP1.x + segment.mP0.x)) - aabb.GetCenter(0);
	fAWdU[0] = fabsf(Dir.x);
	if(fabsf(Diff.x)>BoxExtents.x + fAWdU[0])	return false;

	Dir.y = 0.5f * (segment.mP1.y - segment.mP0.y);
	BoxExtents.y = aabb.GetExtents(1);
	Diff.y = (0.5f * (segment.mP1.y + segment.mP0.y)) - aabb.GetCenter(1);
	fAWdU[1] = fabsf(Dir.y);
	if(fabsf(Diff.y)>BoxExtents.y + fAWdU[1])	return false;

	Dir.z = 0.5f * (segment.mP1.z - segment.mP0.z);
	BoxExtents.z = aabb.GetExtents(2);
	Diff.z = (0.5f * (segment.mP1.z + segment.mP0.z)) - aabb.GetCenter(2);
	fAWdU[2] = fabsf(Dir.z);
	if(fabsf(Diff.z)>BoxExtents.z + fAWdU[2])	return false;

	float f;
	f = Dir.y * Diff.z - Dir.z * Diff.y;	if(fabsf(f)>BoxExtents.y*fAWdU[2] + BoxExtents.z*fAWdU[1])	return false;
	f = Dir.z * Diff.x - Dir.x * Diff.z;	if(fabsf(f)>BoxExtents.x*fAWdU[2] + BoxExtents.z*fAWdU[0])	return false;
	f = Dir.x * Diff.y - Dir.y * Diff.x;	if(fabsf(f)>BoxExtents.x*fAWdU[1] + BoxExtents.y*fAWdU[0])	return false;

	return true;
}

// For a ray
bool Meshmerizer::RayAABB(const Ray& ray, const AABB& aabb)
{
	Point BoxExtents, Diff;

	Diff.x = ray.mOrig.x - aabb.GetCenter(0);
	BoxExtents.x = aabb.GetExtents(0);
	if(fabsf(Diff.x)>BoxExtents.x && Diff.x*ray.mDir.x>=0.0f)	return false;

	Diff.y = ray.mOrig.y - aabb.GetCenter(1);
	BoxExtents.y = aabb.GetExtents(1);
	if(fabsf(Diff.y)>BoxExtents.y && Diff.y*ray.mDir.y>=0.0f)	return false;

	Diff.z = ray.mOrig.z - aabb.GetCenter(2);
	BoxExtents.z = aabb.GetExtents(2);
	if(fabsf(Diff.z)>BoxExtents.z && Diff.z*ray.mDir.z>=0.0f)	return false;

	float fAWdU[3];
	fAWdU[0] = fabsf(ray.mDir.x);
	fAWdU[1] = fabsf(ray.mDir.y);
	fAWdU[2] = fabsf(ray.mDir.z);

	float f;
	f = ray.mDir.y * Diff.z - ray.mDir.z * Diff.y;	if(fabsf(f)>BoxExtents.y*fAWdU[2] + BoxExtents.z*fAWdU[1])	return false;
	f = ray.mDir.z * Diff.x - ray.mDir.x * Diff.z;	if(fabsf(f)>BoxExtents.x*fAWdU[2] + BoxExtents.z*fAWdU[0])	return false;
	f = ray.mDir.x * Diff.y - ray.mDir.y * Diff.x;	if(fabsf(f)>BoxExtents.x*fAWdU[1] + BoxExtents.y*fAWdU[0])	return false;

	return true;
}
