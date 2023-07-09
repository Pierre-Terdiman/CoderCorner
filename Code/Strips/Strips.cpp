#include "Stdafx.h"

int main(int argc, char* argv[])
{
	// Dumb test

	udword Topology[] = {
		0,1,2,
		1,2,3,
		2,3,4,
		3,4,5,
		4,5,6,
		5,6,7,
		6,7,8,
		7,8,9
	};

	STRIPERCREATE sc;
	sc.DFaces			= Topology;
	sc.NbFaces			= 8;
	sc.AskForWords		= true;
	sc.ConnectAllStrips	= false;
	sc.OneSided			= false;
	sc.SGIAlgorithm		= true;

	Striper Strip;
	Strip.Init(sc);

	STRIPERRESULT sr;
	Strip.Compute(sr);

	fprintf(stdout, "Number of strips: %d\n", sr.NbStrips);
	uword* Refs = (uword*)sr.StripRuns;
	for(udword i=0;i<sr.NbStrips;i++)
	{
		fprintf(stdout, "Strip %d:   ", i);
		udword NbRefs = sr.StripLengths[i];
		for(udword j=0;j<NbRefs;j++)
		{
			fprintf(stdout, "%d ", *Refs++);
		}
		fprintf(stdout, "\n");
	}

	return 0;
}
