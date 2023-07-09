#include "stdafx.h"

using namespace IceCore;

static Profiler gProfiler;

int main(int argc, char* argv[])
{
	// We must call Reset() at the start of each frame
	gProfiler.Reset();

	// Profile some stuff
	gProfiler.StartProfile("Frame");

/////////////////////////////////////////////////////////////////
		gProfiler.StartProfile("InnerLoop0");

			Sleep(8);

		gProfiler.EndProfile("InnerLoop0");

/////////////////////////////////////////////////////////////////
		gProfiler.StartProfile("InnerLoop1");

			gProfiler.StartProfile("Stuff0");

				gProfiler.StartProfile("More stuff");

					Sleep(2);

				gProfiler.EndProfile("More stuff");

			gProfiler.EndProfile("Stuff0");

			gProfiler.StartProfile("Stuff1");

				Sleep(2);

			gProfiler.EndProfile("Stuff1");

		gProfiler.EndProfile("InnerLoop1");

/////////////////////////////////////////////////////////////////
		gProfiler.StartProfile("InnerLoop2");

			Sleep(4);

		gProfiler.EndProfile("InnerLoop2");

	gProfiler.EndProfile("Frame");

	// Display results
	udword NbRecords, TotalNbCycles;
	ProfileData* PD = gProfiler.GetResults(NbRecords, TotalNbCycles);

	for(udword i=0;i<NbRecords;i++)
	{
		for(udword j=0;j<PD[i].RecursionLevel*4;j++)	printf(" ");
		printf("%s : %d cycles (%.2f%%)\n", PD[i].Label, PD[i].NbCycles, PD[i].Percents);
	}

	return 0;
}
