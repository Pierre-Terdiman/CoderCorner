///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for vertex clouds.
 *	\file		IceVertexCloud.h
 *	\author		Pierre Terdiman
 *	\date		January, 29, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __ICEVERTEXCLOUD_H__
#define __ICEVERTEXCLOUD_H__

	class MESHMERIZER_API Vertices : public Container
	{
		public:
		// Constructor / Destructor
											Vertices()					{}
											~Vertices()					{}

		__forceinline	udword				GetNbVertices()	const		{ return GetNbEntries()/3;						}
		__forceinline	Point*				GetVertices()	const		{ return (Point*)GetEntries();					}

						Vertices&			AddVertex(const Point& p)	{ Add(p.x).Add(p.y).Add(p.z);	return *this;	}
	};

	//! Vertex cloud reduction result structure
	struct MESHMERIZER_API REDUCEDCLOUD{
						// Out
						Point*				RVerts;				//!< Reduced list
						udword				NbRVerts;			//!< Reduced number of vertices
						udword*				CrossRef;			//!< nbverts remapped indices
	};

	class MESHMERIZER_API ReducedVertexCloud
	{
		public:
		// Constructors/destructor
											ReducedVertexCloud(Point* verts, udword nbverts);
											~ReducedVertexCloud();
		// Free used bytes
						ReducedVertexCloud&	Clean();
		// Cloud reduction
						bool				Reduce(REDUCEDCLOUD* rc=null);
		// Data access
		__forceinline	udword				GetNbVerts()				const	{ return mNbVerts;		}
		__forceinline	udword				GetNbReducedVerts()			const	{ return mNbRVerts;		}
		__forceinline	Point*				GetReducedVerts()			const	{ return mRVerts;		}
		__forceinline	Point*				GetReducedVertex(udword i)	const	{ return &mRVerts[i];	}
		__forceinline	udword*				GetCrossRefTable()			const	{ return mXRef;			}

		private:
		// Original vertex cloud
						udword				mNbVerts;	//!< Number of vertices
						Point*				mVerts;		//!< List of vertices (pointer copy)

		// Reduced vertex cloud
						udword				mNbRVerts;	//!< Reduced number of vertices
						Point*				mRVerts;	//!< Reduced list of vertices
						udword*				mXRef;		//!< Cross-reference table (used to remap topologies)
	};

#endif // __ICEVERTEXCLOUD_H__

