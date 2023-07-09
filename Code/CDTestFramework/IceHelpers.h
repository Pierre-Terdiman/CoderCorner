#ifndef ICEHELPERS_H
#define ICEHELPERS_H

	void RotX(Matrix3x3& m, float angle);
	void RotY(Matrix3x3& m, float angle);
	void RotZ(Matrix3x3& m, float angle);

	udword	RayCapsuleOverlap(const Point& origin, const Point& dir, const LSS& capsule, float s[2]);
	bool	SegmentSphere(const Point& origin, const Point& dir, float length, const Point& center, float radius, float& dist, Point& hit_pos);
	bool	RayAABB2(const Point& min, const Point& max, const Point& origin, const Point& dir, Point& coord);

	inline_ bool RayOBB(const Point& origin, const Point& dir, const OBB& box, float& dist, Point& hit_pos)
	{
		Point LocalOrigin = box.mRot * (origin - box.mCenter);
		Point LocalDir = box.mRot * dir;

		Point LocalImpact;
		if(RayAABB2(-box.mExtents, box.mExtents, LocalOrigin, LocalDir, LocalImpact))
		{
			dist = LocalImpact.Distance(LocalOrigin);
			hit_pos = LocalImpact * box.mRot + box.mCenter;
			return true;
		}
		return false;
	}

#endif	// ICEHELPERS_H
