///////////////////////////////////////////////////////////////////////////////
// Convex SAT optimization test - (c) 2007-2008 Pierre Terdiman
// www.codercorner.com
///////////////////////////////////////////////////////////////////////////////
#ifndef CONVEX_H
#define CONVEX_H

	class MyPoly
	{
		public:
				MyPoly() : mNbVerts(0), mIndices(NULL)	{}
				~MyPoly()								{ delete[]mIndices;	}

		short	mNbVerts;
		char*	mIndices;
		float	mPlane[4];
	};

	class MyConvex
	{
		public:
								MyConvex();
								~MyConvex();

				bool			LoadFromFile(const char* filename);
				void			Render(bool only_wireframe, const btVector3& wire_color) const;
				void			Project(const btVector3& dir, float& min, float& max) const;

				btVector3		WorldSupport(const btVector3& direction) const;

				int				mNbVerts;
				btVector3*		mVerts;
				int				mNbPolys;
				MyPoly*			mPolys;
				btVector3		mLocalCenter;
				btTransform		mTransform;
	};

#endif // CONVEX_H
