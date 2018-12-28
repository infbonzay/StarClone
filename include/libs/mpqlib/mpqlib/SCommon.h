/*****************************************************************************/
/* SCommon.h                              Copyright (c) Ladislav Zezula 2003 */
/*---------------------------------------------------------------------------*/
/* Common functions for encryption/decryption from Storm.dll. Included by    */
/* SFile*** functions, do not include and do not use this file directly      */
/*---------------------------------------------------------------------------*/
/*   Date    Ver   Who  Comment                                              */
/* --------  ----  ---  -------                                              */
/* 24.03.03  1.00  Lad  The first version of SFileCommon.h                   */
/* 12.06.04  1.00  Lad  Renamed to SCommon.h                                 */
/*****************************************************************************/

#ifndef __SCOMMON_H__
#define __SCOMMON_H__

//-----------------------------------------------------------------------------
// StormLib private defines

#define SFILE_TYPE_DATA  0              // Process the file as data file
#define SFILE_TYPE_WAVE  1              // Process the file as WAVe file

//-----------------------------------------------------------------------------
// External variables

extern TMPQArchive * pFirstOpen;
extern UINT          lcLocale;

//-----------------------------------------------------------------------------
// Encryption and decryption functions

int   PrepareStormBuffer();

void  EncryptHashTable(UINT * pdwTable, BYTE * pbKey, UINT dwLength);
void  DecryptHashTable(UINT * pdwTable, BYTE * pbKey, UINT dwLength);
TMPQHash * FindFreeHashEntry(TMPQArchive * ha, const char * szFileName);

void  EncryptBlockTable(UINT * pdwTable, BYTE * pbKey, UINT dwLength);
void  DecryptBlockTable(UINT * pdwTable, BYTE * pbKey, UINT dwLength);

UINT DetectFileSeed(UINT * block, UINT decrypted);
UINT DetectFileSeed2(UINT * block, UINT nDwords, ...);
void  EncryptMPQBlock(UINT * pdwBlock, UINT dwLength, UINT dwSeed1);
void  DecryptMPQBlock(UINT * pdwBlock, UINT dwLength, UINT dwSeed1);

UINT DecryptHashIndex(TMPQArchive * ha, const char * szFileName);
UINT DecryptName1    (const char * szFileName);
UINT DecryptName2    (const char * szFileName);
UINT DecryptFileSeed (const char * szFileName);

TMPQHash * GetHashEntry  (TMPQArchive * ha, const char * szFileName);
TMPQHash * GetHashEntryEx(TMPQArchive * ha, const char * szFileName, UINT lcLocale);

//-----------------------------------------------------------------------------
// Compression and decompression functions

int Compress_pklib  (char * pbOutBuffer, int * pdwOutLength, char * pbInBuffer, int dwInLength, int * pCmpType, int nCmpLevel);
int Decompress_pklib(char * pbOutBuffer, int * pdwOutLength, char * pbInBuffer, int dwInLength);

//-----------------------------------------------------------------------------
// Checking functions

BOOL IsValidMpqHandle(TMPQArchive * ha);
BOOL IsValidFileHandle(TMPQFile * hf);

//-----------------------------------------------------------------------------
// Other functions

BOOL SFileOpenArchiveEx(const char * szMpqName, UINT dwPriority, UINT dwFlags, HANDLE * phMPQ, UINT dwAccessMode = GENERIC_READ);
int  AddFileToArchive(TMPQArchive * ha, HANDLE hFile, const char * szArchivedName, UINT dwFlags, UINT dwQuality, int nFileType, BOOL * pbReplaced);
int  SaveMPQTables(TMPQArchive * ha);
void FreeMPQArchive(TMPQArchive *& ha);

BOOL CheckWildCard(const char * szString, const char * szWildCard);

//-----------------------------------------------------------------------------
// Listfile functions

int  SListFileCreateListFile(TMPQArchive * ha);
int  SListFileAddNode(TMPQArchive * ha, const char * szAddedFile);
int  SListFileRemoveNode(TMPQArchive * ha, const char * szFileName);
int  SListFileRenameNode(TMPQArchive * ha, const char * szOldFileName, const char * szNewFileName);
int  SListFileFreeListFile(TMPQArchive * ha);

int  SListFileSaveToMpq(TMPQArchive * ha);

#ifndef min
#define min(a, b) ((a < b) ? a : b)
#endif

#ifndef max
#define max(a, b) ((a > b) ? a : b)
#endif
#endif // __SCOMMON_H__

