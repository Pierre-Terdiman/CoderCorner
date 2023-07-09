// ------------------------------------------------------------------------------------------------------------------------
// Project Name: Coder Corner
// File Name: MEMHAND.H
// Author: Pierre Terdiman [Zappy]
// Date: 08/15/98
// Abstract:
// THIS IS A VOLCANIC5 SAMPLE CODE EXTRACTED FROM CODER CORNER (TEST PURPOSE)
// ------------------------------------------------------------------------------------------------------------------------
#ifndef __MEMHAND_H__
#define __MEMHAND_H__

// New structures

typedef struct MemoryCell {
 struct MemoryCell *PreviousCell;
 struct MemoryCell *NextCell;
 void *MallocAddress;
 void *AlignedMallocAddress;
 Dword ReservedBytes;
} MEMORY_CELL;

// Prototypes

Byte MEM_RestoreMemory();
void *MEM_ZapMalloc(size_t n);
Byte MEM_ZapFree(void *Buffer);
Byte MEM_ZapInitRam();
Dword MEM_QueryMemInfo();

#endif // __MEMHAND_H__ 
