/*****************************************************************************/
/* StormLib.h                        Copyright (c) Ladislav Zezula 1999-2004 */
/*---------------------------------------------------------------------------*/
/* StormLib library v 4.70                                                   */
/*                                                                           */
/* Author : Ladislav Zezula                                                  */
/* E-mail : ladik@zezula.net                                                 */
/* WWW    : http://www.zezula.net                                            */
/*---------------------------------------------------------------------------*/
/*   Date    Ver   Who  Comment                                              */
/* --------  ----  ---  -------                                              */
/* xx.xx.99  1.00  Lad  Created                                              */
/* 24.03.03  2.50  Lad  Version 2.50                                         */
/* 02.04.03  3.00  Lad  Version 3.00 with compression                        */
/* 11.04.03  3.01  Lad  Renamed to StormLib.h for compatibility with         */
/*                      original headers for Storm.dll                       */
/* 10.05.03  3.02  Lad  Added Pkware DCL compression                         */
/* 26.05.03  4.00  Lad  Completed all compressions                           */
/* 18.06.03  4.01  Lad  Added SFileSetFileLocale                             */
/*                      Added SFileExtractFile                               */
/* 26.07.03  4.02  Lad  Implemented nameless rename and delete               */
/* 26.07.03  4.03  Lad  Added support for protected MPQs                     */
/* 28.08.03  4.10  Lad  Fixed bugs that caused StormLib incorrectly work     */
/*                      with Diablo I savegames and with files having full   */
/*                      hash table                                           */
/* 08.12.03  4.11  DCH  Fixed bug in reading file block larger than 0x1000   */
/*                      on certain files.                                    */
/*                      Fixed bug in AddFile with MPQ_FILE_REPLACE_EXISTING  */
/*                      (Thanx Daniel Chiamarello, dchiamarello@madvawes.com)*/
/* 21.12.03  4.50  Lad  Completed port for Mac                               */
/*                      Fixed bug in compacting (if fsize is mul of 0x1000)  */
/*                      Fixed bug in SCompCompress                           */
/* 27.05.04  4.51  Lad  Changed memory management from new/delete to our     */
/*                      own macros                                           */
/* 22.06.04  4.60  Lad  Optimized search. Support for multiple listfiles.    */
/* 30.09.04  4.61  Lad  Fixed some bugs (Aaargh !!!)                         */
/*                      Correctly works if HashTableSize > BlockTableSize    */
/* 29.12.04  4.70  Lad  Fixed compatibility problem with MPQs from WoW       */
/*****************************************************************************/

#ifndef __STORMLIB_H_
#define __STORMLIB_H_

#include "StormPort.h"

#ifdef __cplusplus
    extern "C" {
#endif
//-----------------------------------------------------------------------------
// Use the apropriate library
//
// The library type is encoded in the library name as the following
// StormLibXYZ.lib
// 
//  X - D for Debug version, R for Release version
//  Y - A for ANSI version, U for Unicode version (Unicode version does not exist yet)
//  Z - S for static C library, D for multithreaded DLL C-library
//

#if defined(_MSC_VER) && !defined (__STORMLIB_SELF__)
  #ifdef _DEBUG                                 // DEBUG VERSIONS
    #ifdef _DLL                               
      #pragma comment(lib, "StormLibDAD.lib")   // Debug Ansi Dynamic version
    #else        
      #pragma comment(lib, "StormLibDAS.lib")   // Debug Ansi Static version
    #endif
  #else                                         // RELEASE VERSIONS
    #ifdef _DLL
      #pragma comment(lib, "StormLibRAD.lib")   // Release Ansi Dynamic version
    #else        
      #pragma comment(lib, "StormLibRAS.lib")   // Release Ansi Static version
    #endif
  #endif
#endif

//-----------------------------------------------------------------------------
// Defines

#define ID_MPQ      0x1A51504D              // MPQ archive header ID ('MPQ\x1A')

#define ERROR_AVI_FILE         10000        // No MPQ file, but AVI file.

// Values for SFileCreateArchiveEx
#define HASH_TABLE_SIZE_MIN    0x00002
#define HASH_TABLE_SIZE_MAX    0x40000

#define HASH_ENTRY_DELETED       0xFFFFFFFE // Block index for deleted hash entry
#define HASH_ENTRY_FREE          0xFFFFFFFF // Block index for free hash entry

// Values for SFileOpenArchive
#define SFILE_OPEN_HARD_DISK_FILE  2        // Open the archive on HDD
#define SFILE_OPEN_CDROM_FILE      3        // Open the archive only if it is on CDROM

// Values for SFileOpenFile
#define SFILE_OPEN_FROM_MPQ        0        // Open the file from the MPQ archive
#define SFILE_OPEN_LOCAL_FILE   (UINT)-1   // Open the file from the MPQ archive


// Flags for TMPQArchive::dwFlags

#define MPQ_FLAG_CHANGED         0x00000001 // If set, the MPQ has been changed
#define MPQ_FLAG_PROTECTED       0x00000002 // Set on protected MPQs (like W3M maps)

// Flags for SFileAddFile
#define MPQ_FILE_COMPRESS_PKWARE 0x00000100 // Compression made by PKWARE Data Compression Library
#define MPQ_FILE_COMPRESS1       0x00000100 // For compatibility with older sources
#define MPQ_FILE_COMPRESS_MULTI  0x00000200 // Multiple compressions
#define MPQ_FILE_COMPRESS2       0x00000200 // For compatibility with older sources
#define MPQ_FILE_COMPRESSED      0x0000FF00 // File is compressed
#define MPQ_FILE_ENCRYPTED       0x00010000 // Indicates whether file is encrypted 
#define MPQ_FILE_FIXSEED         0x00020000 // File decrypt seed has to be fixed
#define MPQ_FILE_EXISTS          0x80000000 // Set if file exists, reset when the fila was deleted
#define MPQ_FILE_REPLACEEXISTING 0x80000000 // Replace when the file exist (SFileAddFile)

// Constants for SFileAddWave
#define MPQ_WAVE_QUALITY_HIGH        0      // Best quality, the worst compression
#define MPQ_WAVE_QUALITY_MEDIUM      1      // Medium quality, medium compression
#define MPQ_WAVE_QUALITY_LOW         2      // Low quality, the best compression

// Constants for SFileGetFileInfo
#define SFILE_INFO_ARCHIVE_SIZE      1      // MPQ size (value from header)
#define SFILE_INFO_HASH_TABLE_SIZE   2      // Size of hash table, in entries
#define SFILE_INFO_BLOCK_TABLE_SIZE  3      // Number of entries in the block table
#define SFILE_INFO_BLOCK_SIZE        4      // Size of file block (in bytes)
#define SFILE_INFO_HASH_TABLE        5      // Pointer to Hash table (TMPQHash *)
#define SFILE_INFO_BLOCK_TABLE       6      // Pointer to Block Table (TMPQBlock *)
#define SFILE_INFO_NUM_FILES         7      // Real number of files within archive
//------
#define SFILE_INFO_HASH_INDEX        8      // Hash index of file in MPQ
#define SFILE_INFO_CODENAME1         9      // The first codename of the file
#define SFILE_INFO_CODENAME2        10      // The second codename of the file
#define SFILE_INFO_LOCALEID         11      // Locale ID of file in MPQ
#define SFILE_INFO_BLOCKINDEX       12      // Index to Block Table
#define SFILE_INFO_FILE_SIZE        13      // Original file size
#define SFILE_INFO_COMPRESSED_SIZE  14      // Compressed file size
#define SFILE_INFO_FLAGS            15      // File flags
#define SFILE_INFO_POSITION         16      // File position within archive
#define SFILE_INFO_SEED             17      // File decryption seed
#define SFILE_INFO_SEED_UNFIXED     18      // Decryption seed not fixed to file pos and size

// Values for compact callback
#define CCB_CHECKING_FILES           1      // Checking archive : (dwParam1 = current, dwParam2 = total)
#define CCB_COMPACTING_FILES         2      // Compacting archive (dwParam1 = current, dwParam2 = total)
#define CCB_CLOSING_ARCHIVE          3      // Closing archive : No params used

#define LISTFILE_NAME     "(listfile)"      // Name of internal listfile
#define ATTRIBUTES_NAME "(attributes)"      // Name of internal attributes file
#define SIGNATURE_NAME   "(signature)"      // Name of internal signature

#define STORMLIB_VERSION (0x0400 + 70)      // Current version of StormLib

//-----------------------------------------------------------------------------
// Structures

struct TMPQFile;

// MPQ file header
struct TMPQHeader
{
    UINT  dwID;                         // The 0x1A51504D ('MPQ\x1A') signature
    UINT  dwDataOffs;                   // Offset of the first file (Relative to MPQ start)
    UINT  dwArchiveSize;                // Size of MPQ archive
    WORD  wOffs0C;                      // 0000 for SC and BW
    WORD  wBlockSize;                   // Size of file block is (0x200 << blockSize)
    UINT  dwHashTablePos;               // File position of hashTable
    UINT  dwBlockTablePos;              // File position of blockTable. Each entry has 16 bytes
    UINT  dwHashTableSize;              // Number of entries in hash table
    UINT  dwBlockTableSize;             // Number of entries in the block table
};

// Hash entry. All files in the archive are searched by their hashes.
struct TMPQHash
{
    UINT  dwName1;                      // The first two DWORDs  
    UINT  dwName2;                      // are the encrypted file name
    UINT  lcLocale;                     // Locale information
    UINT  dwBlockIndex;                 // Index to file description block
};

// File description block contains informations about the file
struct TMPQBlock
{
    UINT  dwFilePos;                    // Block file starting position in the archive
    UINT  dwCSize;                      // Compressed file size
    UINT  dwFSize;                      // Uncompressed file size
    UINT  dwFlags;                      // Flags
};

struct TFileNode
{
    UINT  dwRefCount;                   // Number of references
                                        // There can be more files that have the same name.
                                        // (e.g. multiple language files). We don't want to
                                        // have an entry for each of them, so the entries will be referenced.
                                        // When a number of node references reaches zero, 
                                        // the node will be deleted

    UINT  dwLength;                     // File name length
    char  szFileName[1];                // File name, variable length
};


struct TMPQArchive  // Archive handle structure
{
//  TMPQArchive * pNext;                // Next archive (used by Storm.dll only)
//  TMPQArchive * pPrev;                // Previous archive (used by Storm.dll only)
    char          szFileName[MAX_PATH]; // Opened archive file name
    HANDLE        hFile;                // File handle
    BOOL          bFromCD;              // TRUE if open from CD
    UINT          dwPriority;           // Priority of the archive
    TMPQFile    * pLastFile;            // Recently read file
    UINT          dwBlockPos;           // Position of loaded block in the file
    UINT          dwBlockSize;          // Size of file block
    BYTE        * pbBlockBuffer;        // Buffer (cache) for file block
    UINT          dwBuffPos;            // Position in block buffer
    UINT          dwMpqPos;             // MPQ archive position in the file
    TMPQHeader  * pHeader;              // MPQ file header
    TMPQBlock   * pBlockTable;          // Block table
    TMPQHash    * pHashTable;           // Hash table
    UINT          dwFilePos;            // Current file pointer
//  UINT          dwOpenFiles;          // Number of open files + 1
    TMPQHeader    Header;               // MPQ header

    // Non-Storm.dll members
    TFileNode  ** pListFile;            // File name array
//  HANDLE        hListFile;            // Handle to temporary listfile (when open with write access)
    UINT          dwFlags;              // See MPQ_FLAG_XXXXX
//  BOOL          bChanged;             // TRUE if the archive was changed since open.
//  BOOL          bProtected;           // TRUE if the archive is protected by somehow
};

// File handle structure used by Diablo 1.00 (0x38 bytes)
struct TMPQFile
{
//  TMPQFile   * pNext;                 // Next file handle (used by Storm.dll)
//  TMPQFile   * pPrev;                 // Prev file handle (used by Storm.dll)
//  char         szFileName[260];       // File name
    HANDLE       hFile;                 // File handle
    TMPQArchive* ha;                    // Archive handle
    TMPQBlock  * pBlock;                // File block pointer
    UINT         dwSeed1;               // Seed used for file decrypt
    UINT         dwFilePos;             // Current file position
    UINT         dwOffs14;              // 
    UINT         nBlocks;               // Number of blocks in the file (incl. the last noncomplete one)
    UINT       * pdwBlockPos;           // Position of each file block (only for compressed files)
    BOOL         bBlockPosLoaded;       // TRUE if block positions loaded
    UINT         dwOffs24;              // (Number of bytes somewhere ?)
//  BYTE       * pbBuffer;              // Decompress buffer (Size is always 0x1000 bytes)
//  UINT         dwBytesRead;           // Position in decompress buffer (compressed file only)
//  UINT         dwBuffSize;            // Number of bytes in decompress buffer
//  UINT       * dwConstant;            // Seems to be always 1
    TMPQHash   * pHash;                 // Hash table entry

    // Non-Storm.dll members
    UINT         dwHashIndex;           // Index to Hash table
    UINT         dwFileIndex;           // Index to Block table
    char         szFileName[1];         // File name (variable length)
};

// Used by searching in MPQ archives
struct TMPQSearch
{
    TMPQArchive * ha;                   // Handle to MPQ, where the search runs
    UINT   dwNextIndex;                 // The next searched hash index
    UINT   dwName1;                     // Lastly found Name1
    UINT   dwName2;                     // Lastly found Name2
    char   szSearchMask[1];             // Search mask (variable length)
};

struct SFILE_FIND_DATA
{
    char   cFileName[MAX_PATH];         // Full name of the found file
    char * szPlainName;                 // Pointer to file part
    UINT   lcLocale;                    // Locale version
    UINT   dwFileSize;                  // File size in bytes
    UINT   dwFileFlags;                 // File flags (compressed or encrypted)
    UINT   dwBlockIndex;                // Block index for the file
    UINT   dwCompSize;                  // Compressed file size
};

//-----------------------------------------------------------------------------
// Memory management
//
// We use our own macros for allocating/freeing memory. If you want
// to redefine them, please keep the following rules
//
//  - The memory allocation must return NULL if not enough memory
//    (not e.g. throw exception)
//  - It is not necessary to fill the allocated block with zeros
//  - Memory freeing function must not test the pointer to NULL.
//

__inline void * DebugMalloc(char * szFile, int nLine, int nSize)
{
    void * ptr = malloc(nSize + 100);
    char * plain;

    plain = strrchr(szFile, '\\');
    if(plain == NULL)
        plain = strrchr(szFile, '/');
    if(plain == NULL)
        plain = szFile;

    sprintf((char *)ptr, "%s(%u)", plain, nLine);
    return (char *)ptr + 100;
}

__inline void DebugFree(void * ptr)
{
    free((char *)ptr - 100);
}


#ifndef ALLOCMEM
#define ALLOCMEM(type, nitems)   (type *)malloc((nitems) * sizeof(type))
//#define ALLOCMEM(type, nitems)   (type *)DebugMalloc(__FILE__, __LINE__, (nitems) * sizeof(type))

#endif

#ifndef FREEMEM
#define FREEMEM(ptr) free(ptr);
//#define FREEMEM(ptr) DebugFree(ptr);
#endif

//-----------------------------------------------------------------------------
// Functions in StormLib - compatible with Storm.dll

// Typedefs for functions exported by Storm.dll
typedef UINT  (WINAPI * SFILESETLOCALE)(UINT);
typedef BOOL  (WINAPI * SFILEOPENARCHIVE)(const char *, UINT , UINT , HANDLE *);
typedef BOOL  (WINAPI * SFILECLOSEARCHIVE)(HANDLE);
typedef BOOL  (WINAPI * SFILEOPENFILEEX)(HANDLE, const char *, UINT , HANDLE *);
typedef BOOL  (WINAPI * SFILECLOSEFILE)(HANDLE);
typedef UINT  (WINAPI * SFILEGETFILESIZE)(HANDLE, UINT  *);
typedef UINT  (WINAPI * SFILESETFILEPOINTER)(HANDLE, LONG, LONG *, UINT );
typedef BOOL  (WINAPI * SFILEREADFILE)(HANDLE, VOID *, UINT , UINT  *, LPOVERLAPPED);

// Archive opening/closing
UINT  WINAPI SFileSetLocale(UINT lcNewLocale);
UINT  WINAPI SFileGetLocale();
BOOL  WINAPI SFileOpenArchive(const char * szMpqName, UINT  dwPriority, UINT  dwFlags, HANDLE * phMPQ);
BOOL  WINAPI SFileCloseArchive(HANDLE hMPQ);

// File opening/closing
BOOL  WINAPI SFileOpenFileEx(HANDLE hMPQ, const char * szFileName, UINT  dwSearchScope, HANDLE * phFile);
BOOL  WINAPI SFileCloseFile(HANDLE hFile);

// File I/O
UINT  WINAPI SFileGetFilePos(HANDLE hFile, UINT  * pdwFilePosHigh = NULL);
UINT  WINAPI SFileGetFileSize(HANDLE hFile, UINT  * pdwFileSizeHigh = NULL);
UINT  WINAPI SFileSetFilePointer(HANDLE hFile, LONG lFilePos, LONG * pdwFilePosHigh, UINT  dwMethod);
BOOL  WINAPI SFileReadFile(HANDLE hFile, VOID * lpBuffer, UINT  dwToRead, UINT  * pdwRead = NULL, LPOVERLAPPED lpOverlapped = NULL);

BOOL  WINAPI SFileExtractFile(HANDLE hMpq, const char * szToExtract, const char * szExtracted);

// Adds another listfile into MPQ. The currently added listfile(s) remain,
// so you can use this API to combining more listfiles.
// Note that this function is internally called by SFileFindFirstFile
int   WINAPI SFileAddListFile(HANDLE hMpq, const char * szListFile);

//-----------------------------------------------------------------------------
// Functions in StormLib - not implemented in Storm.dll

// Archive creating and editing
BOOL  WINAPI SFileCreateArchiveEx(const char * szMpqName, UINT  dwCreationDisposition, UINT  dwHashTableSize, HANDLE * phMPQ);
BOOL  WINAPI SFileAddFile(HANDLE hMPQ, const char * szFileName, const char * szArchivedName, UINT  dwFlags); 
BOOL  WINAPI SFileAddWave(HANDLE hMPQ, const char * szFileName, const char * szArchivedName, UINT  dwFlags, UINT  dwQuality); 
BOOL  WINAPI SFileRemoveFile(HANDLE hMPQ, const char * szFileName);
BOOL  WINAPI SFileRenameFile(HANDLE hMPQ, const char * szOldFileName, const char * szNewFileName);
BOOL  WINAPI SFileSetFileLocale(HANDLE hFile, UINT lcNewLocale);

// Retrieving info about the file
BOOL  WINAPI SFileHasFile(HANDLE hMPQ, const char * szFileName);
BOOL  WINAPI SFileGetFileName(HANDLE hFile, char * szFileName);
UINT  WINAPI SFileGetFileInfo(HANDLE hMpqOrFile, UINT  dwInfoType);

// File search
// Note that the SFileFindFirstFileEx has been removed. Use SListFileFindFirst/Next
HANDLE WINAPI SFileFindFirstFile(HANDLE hMPQ, const char * szMask, SFILE_FIND_DATA * lpFindFileData, const char * szListFile);
BOOL   WINAPI SFileFindNextFile(HANDLE hFind, SFILE_FIND_DATA * lpFindFileData);
BOOL   WINAPI SFileFindClose(HANDLE hFind);

// Listfile search
HANDLE SListFileFindFirstFile(HANDLE hMpq, const char * szListFile, const char * szMask, SFILE_FIND_DATA * lpFindFileData);
BOOL   SListFileFindNextFile(HANDLE hFind, SFILE_FIND_DATA * lpFindFileData);
BOOL   SListFileFindClose(HANDLE hFind);

// Archive compacting
typedef void  (WINAPI * COMPACTCB)(void * lpUserData, UINT  dwWorkType, UINT  dwParam1, UINT  dwParam2);
BOOL  WINAPI SFileSetCompactCallback(HANDLE hMPQ, COMPACTCB CompactCB, void * lpData);
BOOL  WINAPI SFileCompactArchive(HANDLE hMPQ, const char * szListFile = NULL, BOOL bReserved = 0);

// Locale support
int   WINAPI SFileEnumLocales(HANDLE hMPQ, const char * szFileName, UINT * plcLocales, UINT  * pdwMaxLocales);

// (De)compression
int WINAPI SCompCompress   (char * pbOutBuffer, int * pdwOutLength, char * pbInBuffer, int dwInLength, int uCompressions, int nCmpType, int nCmpLevel);
int WINAPI SCompDecompress (char * pbOutBuffer, int * pdwOutLength, char * pbInBuffer, int dwInLength);

int  GetMPQLibVer(void);

#ifdef __cplusplus
    }
#endif
//-----------------------------------------------------------------------------
// Functions from Storm.dll. They use slightly different names for keeping
// possibility to use them together with StormLib (StormXXX instead of SFileXXX)

#ifdef __LINK_STORM_DLL__
  #define STORM_ALTERNATE_NAMES         // Force Storm.h to use alternate fnc names
  #include "Storm.h"
#endif // __LINK_STORM_DLL__

//-----------------------------------------------------------------------------
// GFX decode functions. See GfxDecode.cpp for details and description

WORD    WINAPI celGetFrameCount(BYTE * fileBuf);
BYTE  * WINAPI celGetFrameData(BYTE *fileBuf, BYTE *palette, WORD xsize, WORD frame, WORD *ysize, WORD *maxX=NULL);
WORD    WINAPI cl2GetFrameCount(BYTE *fileBuf);
BYTE ** WINAPI cl2GetDirData(BYTE *fileBuf, BYTE *palette, WORD xsize, WORD dir, WORD *ysize);
BYTE  * WINAPI pcxGetData(BYTE *filebuf, UINT  filesize, BYTE transcol, WORD *xsize, WORD *ysize);

#endif  // __STORMLIB_H_


