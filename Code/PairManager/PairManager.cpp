#include "stdafx.h"
using namespace IceCore;
#include <stdio.h>

/*
	Shows how to implement a fast "pair manager", i.e. a data structure holding pairs of objects.
	A pair is identified by a couple of shorts (id0, id1) but the code is easy to modify to match
	your needs. This version has been designed for a sweep-and-prune implementation.

	The class is very small and fast, providing:
	- O(1) insertion
	- O(1) removal
	- O(1) find

	In addition, at any time the current set of pairs is available in a contiguous memory array.
*/

int main(int argc, char* argv[])
{
	UserPairManager2 PM;

	const uword ID0 = 0;
	const uword ID1 = 1;

	// Add a pair to the object. An internal UserPair structure is created for the new pair, and returned to the app.
	const UserPair* P = PM.AddPair(ID0, ID1);
	assert(P);

	// If the pair already exists, the previous structure for this pair is returned.
	const UserPair* TMP = PM.AddPair(ID0, ID1);
	assert(TMP==P);

	// Find the pair. If found, returns pair structure. Otherwise returns null.
	TMP = PM.FindPair(ID0, ID1);
	assert(TMP==P);

	// Look for a non-existing pair => returns null
	TMP = PM.FindPair(0xffff, 0xffff);
	assert(!TMP);

	// List existing pairs
	const udword NbPairs = PM.GetNbActivePairs();
	assert(NbPairs==1);
	const UserPair* Pairs = PM.GetActivePairs();
	for(udword i=0;i<NbPairs;i++)
	{
		printf("Pair %i:     %d | %d\n", i, Pairs[i].mID0, Pairs[i].mID1);
	}

	// Remove existing pair. Returns true if success.
	bool b = PM.RemovePair(ID0, ID1);
	assert(b);

	// Removing a (now) non-existing pair returns false.
	b = PM.RemovePair(ID0, ID1);
	assert(!b);

	// Removing a pair that never existed also returns false.
	b = PM.RemovePair(0xffff, 0xffff);
	assert(!b);

	return 0;
}
