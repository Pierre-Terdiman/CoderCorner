// ------------------------------------------------------------------------------------------------------------------------
// Project Name: Coder Corner
// File Name: TYPES.H
// Author: Pierre Terdiman [Zappy]
// Date: 08/15/98
// Abstract: general types/defines/structures
// THIS IS A VOLCANIC5 SAMPLE CODE EXTRACTED FROM CODER CORNER (TEST PURPOSE)
// ------------------------------------------------------------------------------------------------------------------------

#ifndef __TYPES_H__
#define __TYPES_H__

#ifndef PI
#define	PI 3.1415926535
#endif

// New basic types

// WARNING! "char" stands for "signed char" or "unsigned char", following
// your compiler's default settings! Use those ones to prevent bugs...
#define Byte    signed char
#define UByte   unsigned char
#define Word    signed short
#define UWord   unsigned short
#define Dword   signed long
#define UDword  unsigned long
#define Float32	float
#define Float64 double

// New basic structures

typedef union {
 Float32 f;
 Dword d;
}CELL;

typedef union {
 Float32 f;
 UDword d;
}UCELL;

typedef struct{
 Float32 x, y, z, w;
}Quaternion;

typedef struct{
 Float32 r,g,b;
}COLOR;

#endif // __TYPES_H__
