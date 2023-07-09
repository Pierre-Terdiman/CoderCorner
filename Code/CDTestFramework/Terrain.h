#ifndef TERRAIN_H
#define TERRAIN_H

	class TerrainData
	{
		public:
					TerrainData();
					~TerrainData();

		void		init(udword size, float offset, float width, float chaos, bool flat=false, const Point* pos=NULL);
		void		release();

		udword		size;
		udword		nbVerts;
		udword		nbFaces;
		float		offset;
		float		width;
		float		chaos;
		Point*		verts;
		Point*		colors;
		Point*		normals;
		udword*		faces;
	};

	void			CreateTerrain();
	void			ReleaseTerrain();

	void			RenderTerrain();
	void			RenderTerrainTriangles(udword nbTriangles, const udword* indices);

	const Model*	GetTerrainModel();

#endif
