#include "stdafx.h"

// Play with the settings!
static const udword NbBoxes = 5000;
static const float BoxSize = 20.0f;
static const float Spread = 2000.0f;

int main(int argc, char* argv[])
{
	// 1) Create random boxes
	AABB* Boxes = new AABB[NbBoxes];
	const AABB** List = new const AABB*[NbBoxes];
	for(udword i=0;i<NbBoxes;i++)
	{
		// Get a random center in a cube
		Point Center;
		Center.x = (UnitRandomFloat()-0.5f)*Spread;
		Center.y = (UnitRandomFloat()-0.5f)*Spread;
		Center.z = (UnitRandomFloat()-0.5f)*Spread;

		// Get random extents
		Point Extents;
		Extents.x = UnitRandomFloat()*BoxSize;
		Extents.y = UnitRandomFloat()*BoxSize;
		Extents.z = UnitRandomFloat()*BoxSize;

		// Setup random box
		Boxes[i].mMin.x = Center.x - Extents.x;
		Boxes[i].mMin.y = Center.y - Extents.y;
		Boxes[i].mMin.z = Center.z - Extents.z;
		Boxes[i].mMax.x = Center.x + Extents.x;
		Boxes[i].mMax.y = Center.y + Extents.y;
		Boxes[i].mMax.z = Center.z + Extents.z;
		List[i] = &Boxes[i];
	}

	// 2) Do queries
	udword Time;
	Axes axes;
	axes.Axis0 = 0;
	axes.Axis1 = 2;
	axes.Axis2 = 1;
	// 2-1) Brute-force
	Container Pairs;
	StartProfile(Time);
	BruteForceCompleteBoxTest(NbBoxes, List, Pairs);
	EndProfile(Time);
	printf("Brute force: found %d intersections in %d cycles.\n", Pairs.GetNbEntries(), Time);

	// 2-1) Sweep&prune
	Pairs.Reset();
	StartProfile(Time);
	CompleteBoxPruning(NbBoxes, List, Pairs, axes);
	EndProfile(Time);
	printf("Sweep&Prune: found %d intersections in %d cycles.\n", Pairs.GetNbEntries(), Time);

	// 3) Free & exit
	DELETEARRAY(Boxes);

	return 0;
}
