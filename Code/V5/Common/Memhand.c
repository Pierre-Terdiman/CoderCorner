// ------------------------------------------------------------------------------------------------------------------------
// Project Name: Coder Corner
// File Name: MEMHAND.C
// Author: Pierre Terdiman [Zappy]
// Date: 08/15/98
// Abstract: memory manager
// History:
// - initial version comes from my compression program [JETPAC - 96/97]
// - MEM_QueryMemInfo added in 97/98 for VORTEX
// - auto align added in 98 to speed up MMX copies. Savage alignment on 256
//   bytes boundaries.
// - Sept98: new version for BOMB Studio: errhand.h removed
// - Nov98: MMX alignment produced a bug in MEM_ZapFree(). Now ok.
// THIS IS A VOLCANIC5 SAMPLE CODE EXTRACTED FROM CODER CORNER (TEST PURPOSE)
// ------------------------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Types.h"
#include "Errors.h"
#include "Memhand.h"

MEMORY_CELL *CurrentCell;
MEMORY_CELL *InitCell;
Dword MEM_Total;

// ------------------------------------------------------------------------------------------------------------------------

//
// This routine is used to free all ram allocations done during the program.
// Allocated ram pointers are stored in a dynamic list all the time. That way
// we can keep track of all allocated buffers in order to free them, even when
// a fatal error occurs.
// Usage: (void) RestoreMemory();
//
// > None. Uses global variables previously defined as 'extern'.
//
// < None. Global variables are not modified.
//
Byte MEM_RestoreMemory()
{
	MEMORY_CELL *Cell, *p;
	void *RamToFree;

	Cell = InitCell;

	RamToFree = Cell->MallocAddress;
	while(RamToFree!=NULL)
	{
		#ifdef DEBUG
		fprintf(stderr, "\nDebug info: Free ram...%d", RamToFree);
		#endif
		free(RamToFree);
		MEM_Total-=Cell->ReservedBytes;
		p = Cell;
		Cell = Cell->NextCell;
		#ifdef DEBUG
		fprintf(stderr, "\nDebug info: Free cell...%d", p);
		#endif
		free(p);
		RamToFree=Cell->MallocAddress;
	}
	#ifdef DEBUG
	fprintf(stderr, "\nDebug info: Free last cell...%d", Cell);
	#endif
	free(Cell);

#ifdef MSDOS
	if(MEM_Total!=0) fprintf(stderr, "Oops: %d bytes non restored...\n", MEM_Total);
	else fprintf(stderr, "MEMORY MANAGER: Ram is correctly restored!\n");
#endif // MSDOS

	if(MEM_Total!=0) return(MEMORY_ERROR);

	return(NO_ERROR_OCCURED);
}

// ------------------------------------------------------------------------------------------------------------------------

//
// This routine is used to allocate ram and update the dynamic buffer-list.
// Addresses of allocated buffers are stored in this list, which is formed by
// chained "memory cells", each memory cell beeing composed of one allocated
// buffer address and two link addresses. Link addresses are the addresses of
// previous and next memory cells. A NULL address terminates the list. Current
// memory cell address is stored in the global variable 'CurrentCell'.
// Usage: address = (void *) ZapMalloc(size_t WantedSize);
//
// > WantedSize    size_t        Size of the allocated buffer in bytes
//
// < address       void pointer  Address of the allocated buffer. It's a void
//                               pointer since we can't guess the data-type user
//                               will play with afterwards.
//
void *MEM_ZapMalloc(size_t n)
{
	MEMORY_CELL *Cell;
	void *p;
	void *Aligned;
	Dword Work;

	if (n==0) return NULL;

	n+=256;		// Savage align. SPEED rules. MMX.

	p = malloc(n);
	if (p==NULL) return p;

	Cell = malloc(sizeof(MEMORY_CELL));
	if (Cell==NULL) return(NULL);

	Work = (Dword)p;
	Work+=256;
	Work&=0xffffff00;
	Aligned = (void*)Work;

	CurrentCell->NextCell               = Cell;
	CurrentCell->MallocAddress          = p;
	CurrentCell->AlignedMallocAddress   = Aligned;
	CurrentCell->ReservedBytes          = n+sizeof(MEMORY_CELL);
	Cell->PreviousCell                  = CurrentCell;
	Cell->NextCell                      = NULL;
	Cell->MallocAddress                 = NULL;
	Cell->ReservedBytes                 = 0;

	CurrentCell = Cell;
	MEM_Total+=n+sizeof(MEMORY_CELL);

	return Aligned;
}

// ------------------------------------------------------------------------------------------------------------------------

//
// This routine is used to free ONE allocated buffer, whose address is stored
// in our linked-list. We look for it in the list, free it when found, then
// update the linked-list by killing unused node.
// Usage: Status = (Byte) ZapFree((void *) Buffer);
//
// > Buffer     void pointer      the buffer to free
//
// < Status     Byte              0 = buffer not ZapAllocated before!
//                                1 = Everything went ok
//
Byte MEM_ZapFree(void *Buffer)
{
	MEMORY_CELL *Cell, *p;

	Cell = InitCell;

	while(Cell->AlignedMallocAddress != Buffer)
	{
		Cell = Cell->NextCell;
		if (Cell==NULL) return (0);                   // End of list reached!
	}

	free(Cell->MallocAddress);                         // Free allocated ram
	MEM_Total-=Cell->ReservedBytes;
	#ifdef DEBUG
	fprintf(stderr, "Debug info: free ram...%d\n", Cell->MallocAddress);
	#endif
	p = Cell;                                          // Save address
	Cell = Cell->NextCell;                             // Go to next cell
	p->MallocAddress = Cell->MallocAddress;            // ..and contract list..
	p->AlignedMallocAddress = Cell->AlignedMallocAddress;
	p->NextCell      = Cell->NextCell;
	p->ReservedBytes = Cell->ReservedBytes;
	if (Cell==CurrentCell) CurrentCell=p;           // Vicious bug killed here!
	free(Cell);
	#ifdef DEBUG
	fprintf(stderr, "Debug info: free cell...%d\n", Cell);
	#endif
	return (1);
}

// ------------------------------------------------------------------------------------------------------------------------

//
// This routine MUST be called first. It initializes the memory manager.
//
// Usage: Status = (Byte) MEM_ZapInitRam();
//
// > None.
//
// < Status    Byte     0 if ok, 1 if not.
//
Byte MEM_ZapInitRam()
{
	InitCell = malloc(sizeof(MEMORY_CELL));    // Initial memory cell (see MEMHAND.C)
	if (InitCell==NULL) return(1);

	InitCell->PreviousCell         = NULL;     // No previous cell
	InitCell->NextCell             = NULL;     // It's the  only cell!
	InitCell->MallocAddress        = NULL;     // NULL => End of list
	InitCell->AlignedMallocAddress = NULL;     // NULL => End of list
	InitCell->ReservedBytes        = 0;
	CurrentCell                    = InitCell; // Initialize CurrentCell
	MEM_Total                      = 0;
	return (0);
}

// ------------------------------------------------------------------------------------------------------------------------

//
// This routine gives the total number of bytes ZapMalloked!
// Usage: you're big enough to read one code line!!
//
Dword MEM_QueryMemInfo()
{
	return MEM_Total;
}

// ------------------------------------------------------------------------------------------------------------------------
