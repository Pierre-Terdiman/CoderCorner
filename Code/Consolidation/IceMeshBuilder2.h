///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code to build a clean mesh.
 *	\file		IceMeshBuilder2.h
 *	\author		Pierre Terdiman
 *	\date		January, 29, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __ICEMESHBUILDER2_H__
#define __ICEMESHBUILDER2_H__

	//! X-vertices
	enum VertexCode
	{
		VCODE_VERTEX		= 0,				//!< Geometry
		VCODE_UVW			= 1,				//!< UV-mappings
		VCODE_COLOR			= 2,				//!< Vertex colors
		VCODE_FORCEDWORD	= 0x7fffffff
	};

	//! Initialization structure.
	struct MESHMERIZER_API MBCreate
	{
						MBCreate()				{ ZeroMemory(this, SIZEOFOBJECT);	}

		udword			NbVerts;				//!< Number of vertices in the mesh
		udword			NbFaces;				//!< Number of faces in the mesh
		udword			NbTVerts;				//!< Number of texture-vertices in the mesh
		udword			NbCVerts;				//!< Number of vertex-colors in the mesh
		Point*			Verts;					//!< Verts in MAX are Point3 == Point
		Point*			TVerts;					//!< TVerts in MAX are UVVert == Point3 == Point
		Point*			CVerts;					//!< CVerts in MAX are VertColor == Point3 == Point

		bool			KillZeroAreaFaces;		//!< Look for zero-area faces and delete them
		bool			UseW;					//!< Use W coord in UVW mappings, else just deals with standard U & V
		bool			ComputeVNorm;			//!< Compute vertex-normals, else leave it to the user
		bool			ComputeFNorm;			//!< Compute face-normals, else leave it to the user
		bool			ComputeNormInfo;		//!< Compute normal-information table (used to recompute'm dynamically according to smoothing groups)
		bool			IndexedGeo;				//!< Ask for indexed geometry
		bool			IndexedUVW;				//!< Ask for indexed UVW
		bool			IndexedColors;			//!< Ask for indexed vertex-colors
		bool			RelativeIndices;		//!< Reset indices from one submesh to another, else use absolute indices
		bool			IsSkin;					//!< True for skins => MeshBuilder does not touch the list of vertices
		bool			WeightNormalWithArea;	//!< Take face areas into account in normal computation
	};

//for FLEXPORTER CARBON-COPY (remove MESHMERIZER_API)
	//! Submesh properties.
	struct MESHMERIZER_API MBSubmesh
	{
		sdword			MatID;					//!< MaterialID for this submesh
		udword			SmGrp;					//!< Smoothing groups for this submesh
		udword			NbFaces;				//!< Number of faces in this submesh
		udword			NbVerts;				//!< Number of vertices in this submesh
		udword			NbSubstrips;			//!< Number of strips in this submesh
	};

	//! Material properties.
	struct MESHMERIZER_API MBMatInfo
	{
		sdword			MatID;					//!< This material's ID
		udword			NbFaces;				//!< Number of faces having this material
		udword			NbVerts;				//!< Related number of exported vertices
		udword			NbSubmeshes;			//!< Number of related submeshes
	};

	//! The topology structure.
	struct MESHMERIZER_API MBTopology
	{
						MBTopology()			{ ZeroMemory(this, SIZEOFOBJECT);	}

		uword			NbFaces;				//!< Total number of faces
		uword			NbSubmeshes;			//!< Total number of submeshes (e.g. 6 for the reference cube)
		uword*			VRefs;					//!< Vertex references (3 refs for each face)
		uword*			FacesInSubmesh;			//!< Number of faces in each submesh
		float*			Normals;				//!< Face normals
		MBSubmesh*		SubmeshProperties;		//!< NbSubmeshes structures
	};

	//! The geometry structure.
	struct MESHMERIZER_API MBGeometry
	{
						MBGeometry()			{ ZeroMemory(this, SIZEOFOBJECT);	}
		// Basic data
		uword			NbGeomPts;				//!< Number of vertices in the original mesh
		uword			NbTVerts;				//!< Number of mapping coordinates in the original mesh
		uword			NbColorVerts;			//!< Number of vertex-colors in the original mesh
		//
		uword			NbVerts;				//!< Number of vertices in the final mesh (some may have been duplicated) = sum of all NbVerts in MBSubmeshes
		// Indices
		udword*			VertsRefs;				//!< Vertex indices (only !=null if mIndexedGeo, else vertices are duplicated). Index in Verts.
		udword*			TVertsRefs;				//!< UVW indices (only !=null if mIndexedUVW, else UVWs are duplicated). Index in TVerts.
		udword*			ColorRefs;				//!< Vertex-color indices (only !=null if mIndexedColors, else colors are duplicated). Index in CVerts.
		// Vertex data
		float*			Verts;					//!< List of vertices, may be duplicated or not
		float*			TVerts;					//!< List of UV(W) mappings, may be duplicated or not.
		float*			CVerts;					//!< List of vertex colors, may be duplicated or not.
		float*			Normals;				//!< Vertex normals. Can't be indexed.
		udword			NormalInfoSize;			//!< Size of the NormalInfo field.
		udword*			NormalInfo;				//!< Information used to rebuild normals in realtime. See below.
	};

	// More about NormalInfo:
	//
	// NormalInfo contains some extra information used to rebuild vertex-normals in realtime, by averaging
	// a number of face-normals. Each vertex-normal depends on a various number of face-normals. The exact
	// number depends on the mesh topology, but also depends on the smoothing groups.
	//
	// NormalInfo contains data to rebuild one normal/vertex, ie to rebuild NbVerts normals.
	// Format is, for each vertex:
	//		udword		Count				a number of faces
	//		udword		Ref0, Ref1,...		a list of Count face indices
	//
	// To rebuild vertex-normals in realtime you need to:
	// 1) Rebuild all face-normals (which is easy)
	// 2) For each vertex, add Count face-normals according to NormalInfo, then normalize the summed vector.
	//
	// Other techniques exist, of course.

	//! The materials structure.
	struct MESHMERIZER_API MBMaterials
	{
						MBMaterials() : NbMtls(0), MaterialInfo(null)	{}

		udword			NbMtls;					//!< Number of materials found.
		MBMatInfo*		MaterialInfo;			//!< One structure for each material.
	};

	//! Result structure.
	struct MESHMERIZER_API MBResult
	{
		MBTopology		Topology;				//!< Result topology.
		MBGeometry		Geometry;				//!< Result geometry
		MBMaterials		Materials;				//!< Result materials
	};

	//
	// Pseudo-code showing how to use the consolidation and the striper result structures:
	// mVB is a DX7 vertex buffer filled thanks to the MBGeometry structure.
	// mResult is the MBResult structure.
	// mStripRuns and mStripLengths are from the STRIPERRESULT structure.
	//
	//	// Get indices
	//	uword* VRefs = mResult->Topology.VRefs;
	//	// Make one API call / material
	//	for(i=0;i<mResult->Materials.NbMtls;i++)
	//	{
	//		// Select correct material
	//		udword MaterialID = mResult->Materials.MaterialInfo[i].MatID;
	//		// Draw mesh
	//		if(mStripRuns)	renderer->DrawIndexedPrimitiveVB(PRIMTYPE_TRISTRIP, mVB, 0, mResult->Geometry.NbVerts, mStripRuns[i], mStripLengths[i]);
	//		else			renderer->DrawIndexedPrimitiveVB(PRIMTYPE_TRILIST, mVB, 0, mResult->Geometry.NbVerts, VRefs, mResult->Materials.MaterialInfo[i].NbFaces*3);
	//		// Update index pointer for trilists
	//		VRefs+=mResult->Materials.MaterialInfo[i].NbFaces*3;
	//	}
	//

//~for FLEXPORTER CARBON-COPY

	class MESHMERIZER_API MeshBuilder2
	{
		//! A custom vertex holding 3 references.
		struct MBVertex
		{
			udword	VRef;			//!< Ref into an array of vertices (mVerts)
			udword	TRef;			//!< Ref into an array of UVW mappings (mTVerts)
			udword	CRef;			//!< Ref into an array of vertex-colors (mCVerts)
		};

		//! A custom face holding all the face-related properties.
		struct MBFace
		{
			udword	NewRef[3];		//!< Final "vertex" references [==refs to one geometry/UVW/Color/Normal component/vertex]
			udword	Ref[3];			//!< Ref into an array of MBVertices
			sdword	MatID;			//!< Material ID
			udword	SmGroup;		//!< Smoothing group
			Point	Normal;			//!< Face normal vector
		};

		public:
		// Constructor/destructor
										MeshBuilder2();
										~MeshBuilder2();
		// Creation methods
						bool			Init(const MBCreate& create);
						bool			AddFace(sdword materialID, udword smgroup, udword* vrefs, udword* trefs, udword* crefs, bool ccw=false);
		// Build method
						bool			Build(MBResult& result);
		private:
		// Custom arrays / containers
						CustomArray		mArrayTopology;					//!< Array to store triangle strip runs / face data => topology
						CustomArray		mArrayTopoSize;					//!< Array to store triangle strip lengths / nb of faces => topology size
						Container		mArrayVertsRefs;				//!< Array to store vertices references.
						Container		mArrayTVertsRefs;				//!< Array to store texture-vertices references.
						Container		mArrayColorRefs;				//!< Array to store vertex-colors references.
						Vertices		mArrayVerts;					//!< Array to store vertices.
						Container		mArrayTVerts;					//!< Array to store mapping coordinates. [Container only for UVs without W]
						Vertices		mArrayCVerts;					//!< Array to store vertex-colors.
						Vertices		mArrayVNormals;					//!< Array to store vertex normals.
						Vertices		mArrayFNormals;					//!< Array to store face normals.
						CustomArray		mArrayNormalInfo;				//!< Array to store information about normals.
						Container		mArraySubmeshProperties;		//!< Array to store submesh properties: material ID, smoothing groups, #substrips, #vertex-data.

						Container		mMaterialInfo;					//!< TO BE DOCUMENTED

						udword			mNbFaces;						//!< Maximum (expected) number of faces (provided by user)
						udword			mNbVerts;						//!< Number of vertices in the original mesh (provided by user)
						udword			mNbTVerts;						//!< Number of mapping coordinates in the original mesh (provided by user)
						udword			mNbCVerts;						//!< Number of vertex colors in the original mesh (provided by user)
						udword			mNbAvailableFaces;				//!< Number of faces in database (could be <NbFaces if zero-area faces are skipped)
						udword			mNbBuildVertices;				//!< TO BE DOCUMENTED
						udword			mNewVertsIndex;					//!< TO BE DOCUMENTED

						Point*			mVerts;							//!< Original/reduced list of vertices
						Point*			mTVerts;						//!< Original/reduced list of texture-vertices
						Point*			mCVerts;						//!< Original/reduced list of vertex-colors

						MBFace*			mFacesArray;					//!< The original list of faces.
						MBVertex*		mVertexArray;					//!< The list of rebuilt vertices.

		// Normals computation / smoothing groups
						udword*			mFCounts;						//!< For each vertex, the number of faces sharing that vertex.
						udword*			mVOffsets;						//!< Radix-style offsets of mFCounts.
						udword*			mVertexToFaces;					//!< List of faces sharing each vertex.
						udword*			mFacesCrossRefs;				//!< TO BE DOCUMENTED
						udword			mNbNorm;						//!< Number of normals currently computed in mArrayNormalInfo
						udword			mCrossIndex;					//!< TO BE DOCUMENTED
		// Build settings
						bool			mKillZeroAreaFaces;				//!< true if degenerated triangles must be skipped (default)
						bool			mUseW;							//!< true if W coord must be used, else false (default)
						bool			mComputeVNorm;					//!< true if vertex-normals must be computed, else false (default)
						bool			mComputeFNorm;					//!< true if face-normals must be computed, else false (default)
						bool			mComputeNormInfo;				//!< true if normal information must be computed, else false (default)
						bool			mIndexedGeo;					//!< true if geometry must be exported as indexed (default), else false
						bool			mIndexedUVW;					//!< true if UVW must be exported as indexed, else false (default)
						bool			mIndexedColors;					//!< true if vertex-colors must be exported as indexed, else false (default)
						bool			mRelativeIndices;				//!< true if indices must be reset for each submesh, else false (default)
						bool			mIsSkin;						//!< true if the input mesh is a skin, else false (default)
						bool			mWeightNormalWithArea;			//!< true if face areas must be used as weights in normal computation
		// Internal methods
						MeshBuilder2&	FreeUsedRam();
						bool			FixNullSmoothingGroups();
						bool			OptimizeTopology();
						bool			OptimizeXMappings(udword& nbxverts, Point*& xverts, VertexCode vcode);
						bool			OptimizeGeometry();
						bool			ComputeNormals();
						bool			SaveXVertices();
						bool			ComputeSubmeshes();
						udword			BuildTrilist(udword* list, udword nbfaces, sdword matid, udword smgrp);
						udword			RebuildVertexData(udword* faces, udword nbfaces, Container& array);
	};

#endif // __ICEMESHBUILDER2_H__

