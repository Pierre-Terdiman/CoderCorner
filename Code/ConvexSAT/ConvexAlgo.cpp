///////////////////////////////////////////////////////////////////////////////
// Convex SAT optimization test - (c) 2007-2008 Pierre Terdiman
// www.codercorner.com
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

ConvexResults::ConvexResults()
{
	Reset();
}

void ConvexResults::Reset()
{
	mNormal.setValue(0,0,0);
	mPoint.setValue(0,0,0);
	mDepth = 0.0f;
}


void ConvexResults::Visualize(MyConvex& convex0, const btVector3& overlap_color, const btVector3& separation_color, bool unit_normal) const
{
	if(mDepth<0.0f)
	{
		// Penetration
		btTransform Saved = convex0.mTransform;
		convex0.mTransform.setOrigin(convex0.mTransform.getOrigin() - btVector3(mNormal*mDepth));
		convex0.Render(true, overlap_color);
		convex0.mTransform = Saved;
	}
	else
	{
		if(mDepth>0.0f)
		{
			// Separation
			const float L = unit_normal ? 1.0f : mDepth;
			DrawLine(mPoint, mPoint + mNormal*L, separation_color, 2.0f);
		}
		else
		{
			// Overlapping and no PD available
		}
	}

}