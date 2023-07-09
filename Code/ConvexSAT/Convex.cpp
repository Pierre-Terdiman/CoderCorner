///////////////////////////////////////////////////////////////////////////////
// Convex SAT optimization test - (c) 2007-2008 Pierre Terdiman
// www.codercorner.com
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

MyConvex::MyConvex() :
	mNbVerts	(0),
	mVerts		(NULL),
	mNbPolys	(0),
	mPolys		(NULL)
{
	mTransform.setIdentity();
	mLocalCenter.setValue(0, 0, 0);
}

MyConvex::~MyConvex()
{
	delete[]mPolys;
	delete[]mVerts;
}

bool MyConvex::LoadFromFile(const char* filename)
{
	FILE* fp = fopen(filename, "rb");
	if(!fp)	return false;

	fread(&mNbVerts, sizeof(int), 1, fp);

	mVerts = new btVector3[mNbVerts];
	for(int i=0;i<mNbVerts;i++)
	{
		float vals[3];
		fread(vals, sizeof(float)*3, 1, fp);
		mVerts[i].setX(vals[0]);
		mVerts[i].setY(vals[1]);
		mVerts[i].setZ(vals[2]);
	}

	fread(&mNbPolys, sizeof(int), 1, fp);
	mPolys = new MyPoly[mNbPolys];

	for(int i=0;i<mNbPolys;i++)
	{
		fread(&mPolys[i].mNbVerts, sizeof(short), 1, fp);
		mPolys[i].mIndices = new char[mPolys[i].mNbVerts];
		fread(mPolys[i].mIndices, mPolys[i].mNbVerts, 1, fp);
		fread(mPolys[i].mPlane, sizeof(float)*4, 1, fp);
	}
	fclose(fp);

	float TotalArea = 0.0f;
	mLocalCenter.setValue(0, 0, 0);
	for(int i=0;i<mNbPolys;i++)
	{
		int NbTris = mPolys[i].mNbVerts-2;
		const btVector3& p0 = mVerts[mPolys[i].mIndices[0]];
		for(int j=1;j<=NbTris;j++)
		{
			int k = (j+1)%mPolys[i].mNbVerts;
			const btVector3& p1 = mVerts[mPolys[i].mIndices[j]];
			const btVector3& p2 = mVerts[mPolys[i].mIndices[k]];
			float Area = ((p0 - p1).cross(p0 - p2)).length() * 0.5f;
			btVector3 Center = (p0+p1+p2)/3.0f;
			mLocalCenter += Area * Center;
			TotalArea += Area;
		}
	}
	mLocalCenter /= TotalArea;

	return true;
}

void MyConvex::Render(bool only_wireframe, const btVector3& wire_color) const
{
	const float Scale = 1.0f;
	glPushMatrix();

	float glmat[16];	//4x4 column major matrix for OpenGL.
	mTransform.getOpenGLMatrix(glmat);
	glMultMatrixf(&(glmat[0]));

	if(!only_wireframe)
	{
		btVector3 color(0.0f, 0.5f, 1.0f);
		for(int i=0;i<mNbPolys;i++)
		{
			glNormal3f(mPolys[i].mPlane[0], mPolys[i].mPlane[1], mPolys[i].mPlane[2]);

			int NbTris = mPolys[i].mNbVerts-2;
			const btVector3& p0 = mVerts[mPolys[i].mIndices[0]]*Scale;
			for(int j=1;j<=NbTris;j++)
			{
				int k = (j+1)%mPolys[i].mNbVerts;

				const btVector3& p1 = mVerts[mPolys[i].mIndices[j]]*Scale;
				const btVector3& p2 = mVerts[mPolys[i].mIndices[k]]*Scale;

				DrawTriangle(p0, p1, p2, color);
			}
		}
	}

	{
		btVector3 color;
		if(only_wireframe)
			color = wire_color;
		else
			color = btVector3(0.0f, 0.0f, 0.0f);

		for(int i=0;i<mNbPolys;i++)
		{
			for(int j=0;j<mPolys[i].mNbVerts;j++)
			{
				int k = (j+1)%mPolys[i].mNbVerts;
				DrawLine(mVerts[mPolys[i].mIndices[j]]*Scale, mVerts[mPolys[i].mIndices[k]]*Scale, color, 1.0f);
			}
		}
	}

	glPopMatrix();
}

void MyConvex::Project(const btVector3& dir, float& min, float& max) const
{
	min = FLT_MAX;
	max = -FLT_MAX;
	for(int i=0;i<mNbVerts;i++)
	{
		const btVector3 pt = mTransform * mVerts[i];
		const float dp = pt.dot(dir);
		if(dp < min)	min = dp;
		if(dp > max)	max = dp;
	}
	if(min>max)
	{
		float tmp = min;
		min = max;
		max = tmp;
	}
}

btVector3 MyConvex::WorldSupport(const btVector3& direction) const
{
	float maxDot = -FLT_MAX;
	btVector3 candidate;

	for(int i=0; i<mNbVerts; i++)
	{
		const btVector3 pt = mTransform * mVerts[i];
		const float d = pt.dot(direction);
		if (d > maxDot)
		{
			maxDot = d;
			candidate = pt;
		}
	}
	return candidate;
}
