///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code to build a clean mesh.
 *	\file		IceMeshBuilder2.cpp
 *	\author		Pierre Terdiman
 *	\date		January, 29, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	A mesh converter/builder/cleaner.
 *	- Packs multiple independent topologies in a single hardware-friendly one.
 *	- Tracks and delete unused or redundant vertices, mapping coordinates or vertex colors.
 *	- Kills zero-area faces.
 *	- Computes normals according to smoothing groups.
 *	- Outputs normal information used to recompute them easily at runtime.
 *	- Groups faces in submeshes, according to their rendering properties.
 *	- Duplicates as few vertices as necessary.
 *
 *	\class		MeshBuilder2
 *	\author		Pierre Terdiman
 *	\version	3.0
 *	\warning	support for strips, edge visibility and progressive meshes has been moved elsewhere.
 *	\warning	support for user-defined data has been removed! Use indexed geometry instead.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Precompiled Header
#include "Stdafx.h"

using namespace Meshmerizer;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Constructor.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MeshBuilder2::MeshBuilder2()
{
	mNbFaces				= 0;
	mNbVerts				= 0;
	mNbTVerts				= 0;
	mNbCVerts				= 0;
	mNbAvailableFaces		= 0;
	mNbBuildVertices		= 0;
	mNewVertsIndex			= 0;

	mVerts					= null;
	mTVerts					= null;
	mCVerts					= null;

	mFacesArray				= null;
	mVertexArray			= null;

	mFCounts				= null;
	mVOffsets				= null;
	mVertexToFaces			= null;
	mFacesCrossRefs			= null;
	mNbNorm					= 0;
	mCrossIndex				= 0;

	mKillZeroAreaFaces		= true;
	mUseW					= false;
	mComputeVNorm			= false;
	mComputeFNorm			= false;
	mComputeNormInfo		= false;
	mIndexedGeo				= true;
	mIndexedUVW				= false;
	mIndexedColors			= false;
	mRelativeIndices		= false;
	mIsSkin					= false;
	mWeightNormalWithArea	= false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Destructor.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MeshBuilder2::~MeshBuilder2()
{
	FreeUsedRam();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	A method to free the internal used ram.
 *	\return		Self-Reference.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MeshBuilder2& MeshBuilder2::FreeUsedRam()
{
	mArrayTopology			.Clean();
	mArrayTopoSize			.Clean();
	mArrayVertsRefs			.Empty();
	mArrayTVertsRefs		.Empty();
	mArrayColorRefs			.Empty();
	mArrayVerts				.Empty();
	mArrayTVerts			.Empty();
	mArrayCVerts			.Empty();
	mArrayVNormals			.Empty();
	mArrayFNormals			.Empty();
	mArrayNormalInfo		.Clean();
	mArraySubmeshProperties	.Empty();
	mMaterialInfo			.Empty();

	DELETEARRAY(mVerts);
	DELETEARRAY(mTVerts);
	DELETEARRAY(mCVerts);
	DELETEARRAY(mFacesArray);
	DELETEARRAY(mVertexArray);
	DELETEARRAY(mFCounts);
	DELETEARRAY(mVOffsets);
	DELETEARRAY(mVertexToFaces);
	DELETEARRAY(mFacesCrossRefs);
	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Initialization method.
 *	\param		create		[in] initialization structure.
 *	\return		true if success.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool MeshBuilder2::Init(const MBCreate& create)
{
	struct Local
	{
		static bool SetupXVerts(udword nb, Point* source, Point*& dest, udword& nbverts)
		{
			nbverts	= nb;	if(!nb) return true;
			dest		= new Point[nb];	CHECKALLOC(dest);
			if(source)	CopyMemory(dest, source, nb*sizeof(Point));
			else		ZeroMemory(dest, nb*sizeof(Point));
			return true;
		}
	};

	// Free already used ram
	FreeUsedRam();

	// Save build parameters
	mKillZeroAreaFaces		= create.KillZeroAreaFaces;
	mUseW					= create.UseW;
	mComputeVNorm			= create.ComputeVNorm;
	mComputeFNorm			= create.ComputeFNorm;
	mComputeNormInfo		= create.ComputeNormInfo;
	mIndexedGeo				= create.IndexedGeo;
	mIndexedUVW				= create.IndexedUVW;
	mIndexedColors			= create.IndexedColors;
	mRelativeIndices		= create.RelativeIndices;
	mIsSkin					= create.IsSkin;
	mWeightNormalWithArea	= create.WeightNormalWithArea;

	// Geometries
	if(!Local::SetupXVerts(create.NbVerts, create.Verts, mVerts, mNbVerts))		return false;
	if(!Local::SetupXVerts(create.NbTVerts, create.TVerts, mTVerts, mNbTVerts))	return false;
	if(!Local::SetupXVerts(create.NbCVerts, create.CVerts, mCVerts, mNbCVerts))	return false;

	// Topologies - You're supposed to call AddFace() mNbFaces times afterwards.
	mNbFaces = create.NbFaces;	if(!mNbFaces) return false;
	mFacesArray		= new MBFace[mNbFaces];		CHECKALLOC(mFacesArray);
	mVertexArray	= new MBVertex[mNbFaces*3];	CHECKALLOC(mVertexArray);

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	A method to add a face to the database. You're supposed to call Build() afterwards.
 *	\param		materialID		[in] Material ID (...)
 *	\param		smgroup			[in] smoothing group
 *	\param		vrefs			[in] vertex-references
 *	\param		trefs			[in] texture vertex-references
 *	\param		crefs			[in] vertex-colors references
 *	\param		ccw				[in] true if vertex references must be swapped
 *	\return		true if success.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool MeshBuilder2::AddFace(sdword materialID, udword smgroup, udword* vrefs, udword* trefs, udword* crefs, bool ccw)
{
	// Check for too many faces
	if(mNbAvailableFaces==mNbFaces)	return false;

	// Skip zero-area faces (for the topology only! e.g. a texture face can be multiply defined)
	if(mKillZeroAreaFaces)
	{
		if(vrefs[0]==vrefs[1] || vrefs[0]==vrefs[2] || vrefs[1]==vrefs[2]) return true;
	}

	// Store face properties
	mFacesArray[mNbAvailableFaces].MatID	= materialID;
	mFacesArray[mNbAvailableFaces].SmGroup	= smgroup;

	// Create 3 vertices. At first nothing is shared, and 3 brand new vertices are created for each face.
	ubyte CCW = ccw!=0;
	mVertexArray[mNbBuildVertices+0].VRef	= vrefs ? vrefs[0]		: INVALID_ID;
	mVertexArray[mNbBuildVertices+0].TRef	= trefs ? trefs[0]		: INVALID_ID;
	mVertexArray[mNbBuildVertices+0].CRef	= crefs ? crefs[0]		: INVALID_ID;
	mVertexArray[mNbBuildVertices+1].VRef	= vrefs ? vrefs[1+CCW]	: INVALID_ID;
	mVertexArray[mNbBuildVertices+1].TRef	= trefs ? trefs[1+CCW]	: INVALID_ID;
	mVertexArray[mNbBuildVertices+1].CRef	= crefs ? crefs[1+CCW]	: INVALID_ID;
	mVertexArray[mNbBuildVertices+2].VRef	= vrefs ? vrefs[2-CCW]	: INVALID_ID;
	mVertexArray[mNbBuildVertices+2].TRef	= trefs ? trefs[2-CCW]	: INVALID_ID;
	mVertexArray[mNbBuildVertices+2].CRef	= crefs ? crefs[2-CCW]	: INVALID_ID;
	// Create a new global face
	mFacesArray[mNbAvailableFaces].Ref[0]	= mNbBuildVertices++;
	mFacesArray[mNbAvailableFaces].Ref[1]	= mNbBuildVertices++;
	mFacesArray[mNbAvailableFaces].Ref[2]	= mNbBuildVertices++;

	// Update member. mNbAvailableFaces may finally be different from mNbFaces when zero-area faces are killed.
	mNbAvailableFaces++;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	A method to fix the null smoothing groups
 *	\return		true if success.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool MeshBuilder2::FixNullSmoothingGroups()
{
	udword Index = mNbVerts;
	Vertices Replicated;
	// Loop through available faces
	for(udword i=0;i<mNbAvailableFaces;i++)
	{
		// Look for null smoothing groups
		if(!mFacesArray[i].SmGroup)
		{
			// For skins, we don't replicate vertices here, since we don't have access
			// to the skinning info (which must be replicated as well). That is, null smoothing
			// groups are not handled correctly for skins.........
			if(!mIsSkin)
			{
				// Replicate 3 vertices
				Replicated.AddVertex(mVerts[mVertexArray[mFacesArray[i].Ref[0]].VRef]);
				Replicated.AddVertex(mVerts[mVertexArray[mFacesArray[i].Ref[1]].VRef]);
				Replicated.AddVertex(mVerts[mVertexArray[mFacesArray[i].Ref[2]].VRef]);
				// Assign new (and unique) vertex references
				mVertexArray[mFacesArray[i].Ref[0]].VRef = Index++;
				mVertexArray[mFacesArray[i].Ref[1]].VRef = Index++;
				mVertexArray[mFacesArray[i].Ref[2]].VRef = Index++;
			}
			// Arbitrary non-null group
			mFacesArray[i].SmGroup = 0xffffffff;
		}
	}

	// Avoid useless work...
	udword NbNewVerts = Replicated.GetNbVertices();
	if(!NbNewVerts)	return true;

	// Setup new vertex cloud
	Point* NewVerts = new Point[mNbVerts+NbNewVerts];
	CHECKALLOC(NewVerts);
	CopyMemory(&NewVerts[0],		mVerts,						mNbVerts*sizeof(Point));
	CopyMemory(&NewVerts[mNbVerts],	Replicated.GetVertices(),	NbNewVerts*sizeof(Point));

	DELETEARRAY(mVerts);
	mVerts = NewVerts;
	mNbVerts+=NbNewVerts;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Call that method to rebuild a clean mesh.
 *	\param		result		[out] a structure with final computed data
 *	\return		true if success.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool MeshBuilder2::Build(MBResult& result)
{
	// Checkings
	if(!mNbAvailableFaces)			return false;

	// Cleaning room
	if(!OptimizeTopology())			return false;
	if(!FixNullSmoothingGroups())	return false;
	if(!OptimizeGeometry())			return false;
	if(!ComputeNormals())			return false;
	if(!SaveXVertices())			return false;
	if(!ComputeSubmeshes())			return false;

	// Fill result structure
	udword NbSubmeshes		= mArrayTopoSize.GetOffset() / sizeof(uword);
	udword NormalInfoSize	= mArrayNormalInfo.GetOffset() / sizeof(udword);

	// Topology
						result.Topology.VRefs				= (uword*)		mArrayTopology			.Collapse();	// vertex-indices for trilists
	uword* NbFaces =	result.Topology.FacesInSubmesh		= (uword*)		mArrayTopoSize			.Collapse();	// #faces for trilists
						result.Topology.Normals				= (float*)		mArrayFNormals			.GetEntries();
	// Properties
	MBSubmesh* Props =	result.Topology.SubmeshProperties	= (MBSubmesh*)	mArraySubmeshProperties	.GetEntries();
	// Vertices
						result.Geometry.Verts				= (float*)		mArrayVerts				.GetEntries();
	// Vertex-data
						result.Geometry.VertsRefs			=				mArrayVertsRefs			.GetEntries();	// Only !=null if mIndexedGeo
						result.Geometry.TVertsRefs			=				mArrayTVertsRefs		.GetEntries();	// Only !=null if mIndexedUVW
						result.Geometry.ColorRefs			=				mArrayColorRefs			.GetEntries();	// Only !=null if mIndexedColors
						result.Geometry.TVerts				= (float*)		mArrayTVerts			.GetEntries();
						result.Geometry.CVerts				= (float*)		mArrayCVerts			.GetEntries();
						result.Geometry.Normals				= (float*)		mArrayVNormals			.GetEntries();
						result.Geometry.NormalInfo			= (udword*)		mArrayNormalInfo		.Collapse();

	// Compute material info (compute #materials and #faces for each material, aso)
	udword Nb = mArraySubmeshProperties.GetNbEntries() / (sizeof(MBSubmesh)/sizeof(udword));	// Must be equal to NbSubmeshes
	ASSERT(Nb==NbSubmeshes);
	sdword OldMatID = -1, CurMatID;
	udword NbF = 0, CurNbVtxData = 0;
	udword TotalNbFaces = 0, TotalNbVertices = 0, CurNbSubm = 0;
	for(udword i=0;i<Nb;i++)
	{
		CurMatID = Props[i].MatID;
		if(CurMatID!=OldMatID)
		{
			if(OldMatID!=-1)
			{
				mMaterialInfo.Add((udword)OldMatID).Add(NbF).Add(CurNbVtxData).Add(CurNbSubm);
				CurNbSubm = 0;
				NbF = 0;
				CurNbVtxData = 0;
			}
			OldMatID = CurMatID;
		}
		CurNbSubm++;
		NbF				+= NbFaces[i];
		CurNbVtxData	+= Props[i].NbVerts;
		TotalNbFaces	+= NbFaces[i];
		TotalNbVertices	+= Props[i].NbVerts;
	}
	mMaterialInfo.Add((udword)OldMatID).Add(NbF).Add(CurNbVtxData).Add(CurNbSubm);

	udword NbMtls = mMaterialInfo.GetNbEntries() / (sizeof(MBMatInfo)/sizeof(udword));
	MBMatInfo* MtlInfo = (MBMatInfo*)mMaterialInfo.GetEntries();

	result.Materials.NbMtls			= NbMtls;
	result.Materials.MaterialInfo	= MtlInfo;

	result.Topology.NbFaces			= TotalNbFaces;
	result.Topology.NbSubmeshes		= NbSubmeshes;
	result.Geometry.NbVerts			= TotalNbVertices;
	result.Geometry.NbGeomPts		= mNbVerts;
	result.Geometry.NbTVerts		= mNbTVerts;
	result.Geometry.NbColorVerts	= mNbCVerts;
	result.Geometry.NormalInfoSize	= NormalInfoSize;

	// Fix NormalInfo.
	// mNbNorm should be equal to TotalNbVertices. mCrossIndex should be equal to mNbAvailableFaces.
	udword* p = result.Geometry.NormalInfo;
	// Is normal information needed?
	if(p)
	{
		// We need a correspondence table from old indices to new ones.
		udword* CorresTable = new udword[mNbAvailableFaces];
		CHECKALLOC(CorresTable);
		for(i=0;i<mCrossIndex;i++)	CorresTable[mFacesCrossRefs[i]] = i;

		// Now we can fix our data
		for(i=0;i<mNbNorm;i++)
		{
			udword Count = *p++;			// Current vertex-normal depends on Count face-normals.
			for(udword j=0;j<Count;j++)
			{
				udword Index = *p;			// Face index before MeshCutter was applied
				*p++ = CorresTable[Index];
			}
		}
		DELETEARRAY(CorresTable);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	A method to rebuild clean mappings.
 *	This is useful for Box mapped objects... Example:
 *	This routine looks for redundant UV mappings in the original MAX list, creates an optimized list, and remaps the object with it.
 *	This may be useful with indexed UV. Ex in a cube, you only need 2 indices (0.0 and 1.0) but MAX uses a lot of them (although using texture faces!).
 *	This may be useless with non-strided data (in a DirectX way of thinking) but it's worth running the code...
 *	\return		true if success.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool MeshBuilder2::OptimizeTopology()
{
	// Skin does not contain valid vertices when the consolidation is performed, since the skin vertices are recomputed each frame.
	// Hence, the optimization pass must be avoided, otherwise it could lead to wrong results. For example if the skin vertices are all
	// null at consolidation time, they will be reduced to a single vertex, and this is obviously not what we want.
	if(!mIsSkin)
	{
		// Optimize vertices
		if(!OptimizeXMappings(mNbVerts, mVerts, VCODE_VERTEX))	return false;
	}

	// Optimize UV-mappings
	if(!OptimizeXMappings(mNbTVerts, mTVerts, VCODE_UVW))	return false;

	// Optimize vertex colors
	if(!OptimizeXMappings(mNbCVerts, mCVerts, VCODE_COLOR))	return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	A method to optimize a list of vertices and rebuild correct mappings.
 *	- unused vertices are removed
 *	- redundant vertices are removed
 *	- faces are remapped with correct references
 *
 *	X-vertices are vertices, texture-vertices or vertex-colors.
 *
 *	\param		nbxverts		[in/out] The original number of points in the list. On output, updated with the new number of vertices.
 *	\param		xverts			[in/out] The original list of points. On output, updated with the new list.
 *	\param		vcode			[in] Vertex code, since extra work is needed for UV-mappings.
 *	\return		true if success.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool MeshBuilder2::OptimizeXMappings(udword& nbxverts, Point*& xverts, VertexCode vcode)
{
	// No x-vertices => nothing to do
	if(!nbxverts) return true;

	// 1) X-vertices are stored in xverts, and used ones are indexed by mVertexArray[i].XRef. Some unreferenced X-vertices
	// may exist in xverts, and the first job is to wipe them out.
	Point* UsedVertices;
	udword NbUsedVertices = 0;
	{
		// 1-1) Mark used vertices
		bool* Markers = new bool[nbxverts];	CHECKALLOC(Markers);
		ZeroMemory(Markers, nbxverts*sizeof(bool));

		// Loop through BuildVertices
		for(udword i=0;i<mNbBuildVertices;i++)
		{
			udword XRef;

			// Get correct X-ref
					if(vcode==VCODE_VERTEX)	XRef = mVertexArray[i].VRef;
			else	if(vcode==VCODE_UVW)	XRef = mVertexArray[i].TRef;
			else	if(vcode==VCODE_COLOR)	XRef = mVertexArray[i].CRef;

			// Check the reference is valid
			if(XRef>=nbxverts)
			{
				// Input mesh is wacked! Try to fix...
				XRef = 0;
			}

			// Mark used vertex
			Markers[XRef] = true;
		}

		// 1-2) Now create list of actually used X-vertices
		udword* CrossRef	= new udword[nbxverts];
		if(!CrossRef)		{ DELETEARRAY(Markers);	return false; }

		UsedVertices		= new Point[nbxverts];
		if(!UsedVertices)	{ DELETEARRAY(CrossRef); DELETEARRAY(Markers);	return false; }

		// Loop through input vertices...
		for(i=0;i<nbxverts;i++)
		{
			// ...and keep used ones only.
			if(Markers[i])
			{
				// Create the cross-ref table used to remap faces
				CrossRef[i] = NbUsedVertices;
				// Save used vertex
				UsedVertices[NbUsedVertices++] = xverts[i];
			}
		}
		DELETEARRAY(Markers);

		// 1-3) Remap faces
		for(i=0;i<mNbBuildVertices;i++)
		{
			udword OldXRef;

					if(vcode==VCODE_VERTEX)	{ OldXRef = mVertexArray[i].VRef; mVertexArray[i].VRef = CrossRef[OldXRef]; }
			else	if(vcode==VCODE_UVW)	{ OldXRef = mVertexArray[i].TRef; mVertexArray[i].TRef = CrossRef[OldXRef]; }
			else	if(vcode==VCODE_COLOR)	{ OldXRef = mVertexArray[i].CRef; mVertexArray[i].CRef = CrossRef[OldXRef]; }
		}

		DELETEARRAY(CrossRef);
	}

	// Here, the new list of points is UsedVertices, and the new number of points is NbUsedVertices.
	// Topologies have been fixed.

	// 2) Special case for UV mappings
	if(!mUseW && vcode==VCODE_UVW)
	{
		// The input data may contain screwed W values. Since we're going to sort them anyway,
		// we should clear the array just in case...
		for(udword i=0;i<NbUsedVertices;i++)	UsedVertices[i].z = 0.0f;
	}

	// 3) Now, the list of actually used X-vertices may still contain redundant vertices.
	// Since X-vertices are no more than points in X space, I can use a Vertex Cloud Reducer.
	// We don't *weld* nearby vertices there, we just look for redundant ones.
	udword OldNbXVerts = nbxverts;		// Save it for log text
	{
		// Initialize vertex cloud reducer
		ReducedVertexCloud	Reducer(UsedVertices, NbUsedVertices);

		// Reduce cloud
		REDUCEDCLOUD rc;
		Reducer.Reduce(&rc);

		// Avoid useless work....
		if(rc.NbRVerts==NbUsedVertices)
		{
			// Create the final list of vertices
			Point* NewXVerts = new Point[NbUsedVertices];	CHECKALLOC(NewXVerts);
			CopyMemory(NewXVerts, UsedVertices, NbUsedVertices*sizeof(Point));
			nbxverts = NbUsedVertices;

			// Replace old list
			DELETEARRAY(xverts);
			xverts = NewXVerts;

			// We can free this now
			DELETEARRAY(UsedVertices);
		}
		else
		{
			// We can free this now
			DELETEARRAY(UsedVertices);

			// Final remap
			for(udword i=0;i<mNbBuildVertices;i++)
			{
				udword OldXRef;

						if(vcode==VCODE_VERTEX)	{ OldXRef = mVertexArray[i].VRef; mVertexArray[i].VRef = rc.CrossRef[OldXRef]; }
				else	if(vcode==VCODE_UVW)	{ OldXRef = mVertexArray[i].TRef; mVertexArray[i].TRef = rc.CrossRef[OldXRef]; }
				else	if(vcode==VCODE_COLOR)	{ OldXRef = mVertexArray[i].CRef; mVertexArray[i].CRef = rc.CrossRef[OldXRef]; }
			}

			// Create the final list of vertices
			Point* NewXVerts = new Point[rc.NbRVerts];	CHECKALLOC(NewXVerts);
			CopyMemory(NewXVerts, rc.RVerts, rc.NbRVerts*sizeof(Point));
			nbxverts = rc.NbRVerts;

			// Replace old list
			DELETEARRAY(xverts);
			xverts = NewXVerts;

			if(vcode==VCODE_VERTEX)
			{
				// Here, we may have to face a final little thing........and yes, I actually encountered the problem:
				// Original face is: 0 1 2, not detected as degenerate at first.
				// But vertices 1 and 2 are exactly the same, so we detect it, and the remap above gives 0 1 1.
				// Hence, we must test for degenerate triangles *again*.
				// Note that we possibly could have to look for unused vertices again as well (e.g. 2 in our example)...... not done here.
				// A quick fix could be to keep track of the valencies in this class as well.
				Container GoodFaces;
				for(i=0;i<mNbAvailableFaces;i++)
				{
					// Get refs
					udword Ref0 = mFacesArray[i].Ref[0];
					udword Ref1 = mFacesArray[i].Ref[1];
					udword Ref2 = mFacesArray[i].Ref[2];

					// Test against the geometry
					if(!(mVertexArray[Ref0].VRef==mVertexArray[Ref1].VRef
						||mVertexArray[Ref0].VRef==mVertexArray[Ref2].VRef
						||mVertexArray[Ref1].VRef==mVertexArray[Ref2].VRef))
					{
						// Face i is not degenerate
						GoodFaces.Add(i);
					}
				}

				// Keep good faces
				MBFace* Tmp = new MBFace[mNbAvailableFaces];	CHECKALLOC(Tmp);
				for(i=0;i<GoodFaces.GetNbEntries();i++)	Tmp[i] = mFacesArray[GoodFaces.GetEntry(i)];

				// Replace faces
				udword NbDegenerate = mNbAvailableFaces - GoodFaces.GetNbEntries();
				DELETEARRAY(mFacesArray);
				mFacesArray = Tmp;
				mNbAvailableFaces = GoodFaces.GetNbEntries();

				// Log file
				if(NbDegenerate)	Log("MeshBuilder: found %d degenerate faces after reduction.\n", NbDegenerate);
			}
		}
	}

	// 4) Stats
	float Percent = 100.0f - (float(nbxverts) * 100.0f / float(OldNbXVerts));
			if(vcode==VCODE_VERTEX)	Log("MeshBuilder: reduced %d to %d vertices (%.1f%%)\n",		OldNbXVerts, nbxverts, Percent);
	else	if(vcode==VCODE_UVW)	Log("MeshBuilder: reduced %d to %d UVs (%.1f%%)\n",				OldNbXVerts, nbxverts, Percent);
	else	if(vcode==VCODE_COLOR)	Log("MeshBuilder: reduced %d to %d vertex-colors (%.1f%%)\n",	OldNbXVerts, nbxverts, Percent);

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	A method to rebuild a clean geometry.
 *
 *	On output:
 *				mNbBuildVertices	is updated with the optimized number of vertices, supposed to be lesser than before the call.
 *									Before the call mNbBuildVertices = mNbAvailableFaces * 3.
 *				mVertexArray		is updated as well, with mNbBuildVertices vertices.
 *				mFacesArray			is updated with new references.
 *	\return		true if success.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool MeshBuilder2::OptimizeGeometry()
{
	// Ok, I'm not dealing with floating-point coordinates, I'm dealing with references.
	// So what? I still can treat them as a vertex cloud.
	ASSERT(sizeof(MBVertex)==sizeof(Point));	// ...
	ReducedVertexCloud	Reducer((Point*)mVertexArray, mNbBuildVertices);
	REDUCEDCLOUD rc;
	Reducer.Reduce(&rc);

	// Remap topology
	for(udword i=0;i<mNbAvailableFaces;i++)
	{
		mFacesArray[i].Ref[0] = rc.CrossRef[mFacesArray[i].Ref[0]];
		mFacesArray[i].Ref[1] = rc.CrossRef[mFacesArray[i].Ref[1]];
		mFacesArray[i].Ref[2] = rc.CrossRef[mFacesArray[i].Ref[2]];
	}

	// Replace vertex cloud
	DELETEARRAY(mVertexArray);	// Previous buffer contained NbFaces*3 vertices, which was the max possible
	mVertexArray = new MBVertex[rc.NbRVerts];
	CHECKALLOC(mVertexArray);
	CopyMemory(mVertexArray, rc.RVerts, rc.NbRVerts*sizeof(MBVertex));

	mNbBuildVertices = rc.NbRVerts;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	A method to compute normals. Smoothing groups are taken into account for the final calculation.
 *	\return		true if success.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool MeshBuilder2::ComputeNormals()
{
	// Does nothing if user didn't ask for normals.
	if(mComputeFNorm || mComputeVNorm)
	{
		// Little checkings
		if(!mNbVerts || !mNbBuildVertices || !mVertexArray)	return false;

		// Compute face-normals
		MBFace*	f = mFacesArray;	if(!f)	return false;
		Point*	v = mVerts;			if(!v)	return false;
		for(udword i=0;i<mNbAvailableFaces;i++)
		{
			udword Ref0 = mVertexArray[f[i].Ref[0]].VRef;
			udword Ref1 = mVertexArray[f[i].Ref[1]].VRef;
			udword Ref2 = mVertexArray[f[i].Ref[2]].VRef;

			f[i].Normal = ((v[Ref2] - v[Ref1]) ^ (v[Ref0] - v[Ref1]));

			if(!mWeightNormalWithArea)	f[i].Normal.Normalize();
			else						f[i].Normal *= 0.5f;

			if(mComputeFNorm)	mArrayFNormals.AddVertex(f[i].Normal);
		}

		// Create tables needed to easily compute vertex-normals according to smoothing groups
		{
			// We need to compute for each vertex the list of faces sharing that vertex
			mFCounts	= new udword[mNbVerts];	CHECKALLOC(mFCounts);
			mVOffsets	= new udword[mNbVerts];	CHECKALLOC(mVOffsets);
			// Init ram
			ZeroMemory(mFCounts,	mNbVerts * sizeof(udword));
			ZeroMemory(mVOffsets,	mNbVerts * sizeof(udword));

			// First pass to compute counts
			for(i=0;i<mNbAvailableFaces;i++)
			{
				mFCounts[mVertexArray[f[i].Ref[0]].VRef]++;
				mFCounts[mVertexArray[f[i].Ref[1]].VRef]++;
				mFCounts[mVertexArray[f[i].Ref[2]].VRef]++;
			}

			// Create VOffsets
			for(i=1;i<mNbVerts;i++) mVOffsets[i] = mVOffsets[i-1] + mFCounts[i-1];

			// Total #referenced faces is mNbAvailableFaces*3.
			mVertexToFaces = new udword[mNbAvailableFaces * 3];	CHECKALLOC(mVertexToFaces);

			// Create list
			for(i=0;i<mNbAvailableFaces;i++)
			{
				udword	Ref0	= mVertexArray[f[i].Ref[0]].VRef;
				udword	Ref1	= mVertexArray[f[i].Ref[1]].VRef;
				udword	Ref2	= mVertexArray[f[i].Ref[2]].VRef;
				mVertexToFaces[mVOffsets[Ref0]++] = i;
				mVertexToFaces[mVOffsets[Ref1]++] = i;
				mVertexToFaces[mVOffsets[Ref2]++] = i;
			}

			// Recreate VOffsets [needed in StoreVertex()]
			mVOffsets[0] = 0;
			for(i=1;i<mNbVerts;i++) mVOffsets[i] = mVOffsets[i-1] + mFCounts[i-1];

			// The table we just computed is valid for current mesh description.
			// Particularly, mVertexToFaces is valid as long as faces are listed in the
			// original order, used to compute the table. This order will further be cut
			// to pieces by MeshBuilder2 (that's what it's been designed for!). Hence,
			// we'll need a cross-references table to fix mVertexToFaces before leaving
			// MeshBuilder2. This table will be filled on exporting triangles.
			//
			// Another solution would've been to compute mVertexToFaces after mesh processing.
			// Unfortunately mVertexToFaces is needed in StoreVertex to compute vertex-normals,
			// then it would've been difficult to compute it later. We could have (and actually
			// used to do so) used another way of computing vertex-normals, but it needed much
			// more ram, and the code was sloppy.
			mFacesCrossRefs = new udword[mNbAvailableFaces];	CHECKALLOC(mFacesCrossRefs);
			FillMemory(mFacesCrossRefs, mNbAvailableFaces * sizeof(udword), -1);
			mCrossIndex = 0;
		}
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	A method to store X-Vertices into export arrays. (X = Geo, Texture, colors)
 *	X-Vertices are indexed vertices.
 *	\return		true if success.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool MeshBuilder2::SaveXVertices()
{
	// We can output two different things, according to what the user wants:
	// - indexed vertices
	// - non-indexed vertices
	//
	// Within the class, we deal with indexed vertices. Hence, when that's what the user wants in output, we just have
	// to dump our list of optimized vertices, and we're done.
	//
	// This is what we do here.
	//
	// Actual indices are saved in another place (where we save the non-indexed vertices)

	// 1) Save indexed geometry
	if(mVerts && mIndexedGeo)
	{
		for(udword i=0;i<mNbVerts;i++)	mArrayVerts.AddVertex(mVerts[i]);
	}

	// 2) Save UV(W) mappings
	if(mTVerts && mIndexedUVW)
	{
		for(udword i=0;i<mNbTVerts;i++)
		{
						mArrayTVerts.Add(mTVerts[i].x).Add(mTVerts[i].y);
			if(mUseW)	mArrayTVerts.Add(mTVerts[i].z);
		}
	}

	// 3) Save vertex colors
	if(mCVerts && mIndexedColors)
	{
		for(udword i=0;i<mNbCVerts;i++)	mArrayCVerts.AddVertex(mCVerts[i]);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	A method to group faces by render properties. Each group of faces sharing rendering properties is a submesh.
 *	\return		true if success.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool MeshBuilder2::ComputeSubmeshes()
{
	udword* HelpList	= new udword[mNbAvailableFaces];
	udword* MatID		= new udword[mNbAvailableFaces];
	udword* SmGrps		= new udword[mNbAvailableFaces];
	if(!HelpList || !MatID || !SmGrps)
	{
		DELETEARRAY(SmGrps);
		DELETEARRAY(MatID);
		DELETEARRAY(HelpList);
		return false;
	}

	// Those lists are needed for sorting
	for(udword i=0;i<mNbAvailableFaces;i++)
	{
		MatID[i]	= mFacesArray[i].MatID;
		SmGrps[i]	= mFacesArray[i].SmGroup;
	}

	// Sort faces by properties. The material ID is the most important sort key, since it leads to
	// SetMaterial and SetTexture calls. Smoothing groups only lead to duplicated vertices.
	RadixSort Radix;
	udword* Sorted = Radix.Sort(SmGrps, mNbAvailableFaces, false).Sort(MatID, mNbAvailableFaces, false).GetIndices();

	// Cut sorted list into coherent meshes
	udword Index = Sorted[0];
	udword CurrentMatID	= MatID[Index];
	udword CurrentSmGrp	= SmGrps[Index];
	udword NbF = 0, NbSubmesh = 0, Total = 0;

	// Loop through faces
	for(i=0;i<mNbAvailableFaces;i++)
	{
		// Read sorted face index
		Index = Sorted[i];
		// Look for all faces sharing the same properties
		if(MatID[Index]==CurrentMatID && SmGrps[Index]==CurrentSmGrp)
		{
			// One more entry found => store face number in HelpList
			HelpList[NbF++] = Index;
		}
		else
		{
			// ...until a new key is found
			Total+=BuildTrilist(HelpList, NbF, CurrentMatID, CurrentSmGrp);
			NbSubmesh++;

			// Update current properties
			CurrentMatID	= MatID[Index];
			CurrentSmGrp	= SmGrps[Index];

			// Reset HelpList
			HelpList[0] = Index;
			NbF = 1;
		}
		// ...and loop again
	}
	// Build last mesh
	Total+=BuildTrilist(HelpList, NbF, CurrentMatID, CurrentSmGrp);
	NbSubmesh++;

	// Here, Total = total #faces

	// Free local ram
	DELETEARRAY(SmGrps);
	DELETEARRAY(MatID);
	DELETEARRAY(HelpList);

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	A method to build a triangle list. The input list contains faces whose rendering properties are the same (Material ID, smoothing groups)
 *	In other words, involved vertices have a single normal, and the bunch of faces is what we call a submesh.
 *	\param		list		[in] Faces to analyze (contains indices into mFacesArray)
 *	\param		nbfaces		[in] Number of faces in the list
 *	\param		matis		[in] Common material ID for all those faces
 *	\param		smgrp		[in] Common smoothing groups for all those faces
 *	\return		the number of faces.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
udword MeshBuilder2::BuildTrilist(udword* list, udword nbfaces, sdword matid, udword smgrp)
{
	// Save mesh properties (Material ID, Smoothing Groups)
	mArraySubmeshProperties.Add((udword)matid).Add(smgrp);

	// Save mesh datas
	Container ArrayWorkList;	// Will contain the rebuilt vertex data
	udword NbVertsForSubmesh = RebuildVertexData(list, nbfaces, ArrayWorkList);
	udword* tmp = ArrayWorkList.GetEntries();
	// tmp is indexed by mFacesArray[].NewRef[]
	// tmp contains indexed indices, and now we must output real values.

	// Loop through all relevant vertices
	for(udword i=0;i<NbVertsForSubmesh;i++)
	{
		udword v1 = *tmp++;		// Vertex index
		udword tv1 = *tmp++;	// UV index
		udword cv1 = *tmp++;	// Color index
		udword grp = *tmp++;	// Smoothing groups

		// Save UVW mappings
		if(mTVerts)
		{
			// UVW mappings can be indexed or not
			if(mIndexedUVW)	mArrayTVertsRefs.Add(tv1);
			else
			{
				mArrayTVerts.Add(mTVerts[tv1].x).Add(mTVerts[tv1].y);
				if(mUseW)	mArrayTVerts.Add(mTVerts[tv1].z);
			}
		}

		// Save vertex-color
		if(mCVerts)
		{
			// Vertex-colors can be indexed or not
			if(mIndexedColors)	mArrayColorRefs.Add(cv1);
			else	mArrayCVerts.AddVertex(mCVerts[cv1]);
		}

		// Save vertex-normal
		if(mComputeVNorm)
		{
			Point vn(0.0f, 0.0f, 0.0f);		// Vertex-normal we're about to compute
			udword NbF = 0;					// Number of faces involved in the calculation, for current smoothing group

			// Leave room for the number of faces involved in the computation of current vertex's normal
			if(mComputeNormInfo)	mArrayNormalInfo.Store((udword)0).PushAddress();

			// Loop through faces sharing this vertex
			for(udword j=0;j<mFCounts[v1];j++)
			{
				// Look for faces belonging to the same smoothing groups.
				udword FaceIndex = mVertexToFaces[mVOffsets[v1]+j];
				if(mFacesArray[FaceIndex].SmGroup & grp)
				{
					// Take face-normal contribution into account
					vn += mFacesArray[FaceIndex].Normal;
					// That's one more face involved in the calculation
					NbF++;
					// If user wants to further recompute vertex-normals for dynamic meshes,
					// he'll need NbF and involved indexed faces (FaceIndex = mVertexToFaces[mVOffsets[v1]+j])
					if(mComputeNormInfo)	mArrayNormalInfo.Store(FaceIndex);
				}
			}

			// Get initial location back and save the number of faces
			if(mComputeNormInfo)
			{
				mArrayNormalInfo.PopAddressAndStore(NbF);
				mNbNorm++;	// Count one more normal
			}
			// Normalize vertex-normal...
			vn.Normalize();
			// ...and save it
			mArrayVNormals.AddVertex(vn);
		}

		// Save geometry
		if(mVerts)
		{
			// Geometry can be indexed or not
			if(mIndexedGeo)	mArrayVertsRefs.Add(v1);
			else			mArrayVerts.AddVertex(mVerts[v1]);
		}
	}

	// Save topology
	if(mFacesArray)
	{
		mArrayTopoSize.Store((uword)nbfaces);
		for(udword i=0;i<nbfaces;i++)
		{
			udword Fnb = list[i];
			udword v0 = mFacesArray[Fnb].NewRef[0];
			udword v1 = mFacesArray[Fnb].NewRef[1];
			udword v2 = mFacesArray[Fnb].NewRef[2];
			mArrayTopology.Store((uword)v0).Store((uword)v1).Store((uword)v2);
			// Build cross-references faces-list for normals
			if(mFacesCrossRefs)	mFacesCrossRefs[mCrossIndex++] = Fnb;
		}
	}

	// Store #substrips (0 for trilists, stored for output coherence)
	mArraySubmeshProperties.Add(udword(0));

	return nbfaces;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	A method to rebuild and reorganize data for a coherent submesh.
 *	\param		faces		[in] The faces to 'rebuild'
 *	\param		nbfaces		[in] Number of faces in *faces
 *	\param		array		[out] A container to store the results
 *	\return		number of vertices involved in this submesh
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
udword MeshBuilder2::RebuildVertexData(udword* faces, udword nbfaces, Container& array)
{
	#define MESHBUILDER_REF_NOT_DONE	-1		//!< Default cross reference

	struct LocalInfo
	{
		MBVertex*	VertexArray;
		MBFace*		FaceArray;
		udword*		CrossList;
		udword		Grp;
		udword		Fnb;
		Container*	DestArray;
	};

	struct Local
	{
		static void StoreVertex(const LocalInfo& info, udword ref, udword& newvertsindex, udword id)
		{
			// Was the vertex already converted?
			if(info.CrossList[ref]==MESHBUILDER_REF_NOT_DONE)
			{
				// Catch original references
				udword v1	= info.VertexArray[ref].VRef;
				udword tv1	= info.VertexArray[ref].TRef;
				udword cv1	= info.VertexArray[ref].CRef;

				// Save relevant values
				info.DestArray->Add(v1).Add(tv1).Add(cv1).Add(info.Grp);

				// Update
				info.FaceArray[info.Fnb].NewRef[id]	= newvertsindex;
				// Cross list
				info.CrossList[ref]	= newvertsindex++;
			}
			else info.FaceArray[info.Fnb].NewRef[id] = info.CrossList[ref];
		}
	};

	// Get some bytes for the cross-list
	udword* CrossList = new udword [mNbBuildVertices];
	ASSERT(CrossList);
	// Initialize cross list
	for(udword i=0;i<mNbBuildVertices;i++)	CrossList[i] = MESHBUILDER_REF_NOT_DONE;

	// Rebuild
	if(mRelativeIndices)	mNewVertsIndex = 0;		// First vertex index is 0 (relative indexing) or the last one (absolute indexing)
	udword NbVerts = mNewVertsIndex;				// 0 or last index. Keep track of that for the final count.

	// Create a local info structure
	LocalInfo LI;
	LI.VertexArray	= mVertexArray;
	LI.FaceArray	= mFacesArray;
	LI.CrossList	= CrossList;
	LI.DestArray	= &array;

	// Loop through faces
	for(i=0;i<nbfaces;i++)
	{
		// Get the index of the face to convert
		LI.Fnb = faces[i];
		// Get the smoothing groups, used to compute the normals
		LI.Grp = mFacesArray[LI.Fnb].SmGroup;

		// Get the original references
		udword Ref0 = mFacesArray[LI.Fnb].Ref[0];
		udword Ref1 = mFacesArray[LI.Fnb].Ref[1];
		udword Ref2 = mFacesArray[LI.Fnb].Ref[2];

		// Convert the three vertices
		Local::StoreVertex(LI, Ref0, mNewVertsIndex, 0);
		Local::StoreVertex(LI, Ref1, mNewVertsIndex, 1);
		Local::StoreVertex(LI, Ref2, mNewVertsIndex, 2);
	}

	// Free cross list ram
	DELETEARRAY(CrossList);

	// Store #vertex-data saved. mNewVertsIndex is the current index, NbVerts was the index value when entering the method.
	udword NbVertsForThisSubmesh = mNewVertsIndex - NbVerts;

	// Save some useful values
	mArraySubmeshProperties.Add(nbfaces).Add(NbVertsForThisSubmesh);

	// Here, the array has been filled with indexed vertex data.
	// The relevant topology is stored in the NewRef field of all input faces, in mFacesArray.

	return NbVertsForThisSubmesh;
}
