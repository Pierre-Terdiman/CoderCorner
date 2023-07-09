// ------------------------------------------------------------------------------------------------------------------------
// Project Name: Coder Corner
// File Name: ERRORS.H
// Author: Pierre Terdiman [Zappy]
// Date: 08/15/98
// Abstract: all possible error codes
// THIS IS A VOLCANIC5 SAMPLE CODE EXTRACTED FROM CODER CORNER (TEST PURPOSE)
// ------------------------------------------------------------------------------------------------------------------------

#ifndef __ERRORS_H__
#define __ERRORS_H__

// -----[ Error codes]-----------------------------------------------------------------------------------------------------

#define NORMAL_EXIT             0   // Quit program without errors
#define NO_ERROR_OCCURED        0   // Returns from a function without errors
#define USER_BREAK              1   // Cancel function, escape pressed...
#define OUT_OF_MEMORY           2   // Malloc failed, no more ram
#define WRITE_ERROR             3   // Error while writing to disk
#define READ_ERROR              4   // Error while reading from disk
#define SVGA_NOT_SUPPORTED      5   // SVGA Mode not supported by video card
#define PLAY_CHUNK_NOT_FOUND    6   // Not a correct BLI file
#define BLI_FILE_CORRUPTED      7   // BLI file is incomplete
#define WRONG_BLI_VERSION       8   // BLI file is not last version 
#define MEMORY_ERROR            9   // Error while freeing memory
#define X11_ERROR              10   // X11 error [only on VMS/Unix/Linux]
#define BAD_SYSTEM             11   // Type error
#define INTERNAL               12   // Variable contains unexpected value
#define NO_3DFX                13   // No 3DFX installed
#define BLOB_ERROR             14   // Error during the BLOB calculus (the triangulation)
#define WEIRD_ERROR	           15   // Strange error, which MUST NEVER appear!
#define DDRAW_ERROR            16   // Direct Draw error
#define CANNOT_PACK            17   // File or buffer can't be packed
#define UNKNOWN_FORMAT         18   // File has an unknown extension
#define WINDOWS_ERROR          19   // Something went wrong in a Windows-process
// ------------------------------------------------------------------------------------------------------------------------

// Prototypes
void ERR_GetErrorMessage(Byte ErrorCode, Byte *Message);

#endif // __ERRORS_H__
