/*****************************************************************************/
/* SFileCompactArchive.cpp                Copyright (c) Ladislav Zezula 2003 */
/*---------------------------------------------------------------------------*/
/* Archive compacting function                                               */
/*---------------------------------------------------------------------------*/
/*   Date    Ver   Who  Comment                                              */
/* --------  ----  ---  -------                                              */
/* 14.04.03  1.00  Lad  Splitted from SFileCreateArchiveEx.cpp               */
/* 19.11.03  1.01  Dan  Big endian handling                                  */
/*****************************************************************************/

#define __STORMLIB_SELF__
#include "StormLib.h"
#include "SCommon.h"

/*****************************************************************************/
/* Local structures                                                          */
/*****************************************************************************/

/*****************************************************************************/
/* Local variables                                                           */
/*****************************************************************************/

static COMPACTCB CompactCB  = NULL;
static void    * lpUserData = NULL;

/*****************************************************************************/
/* Local functions                                                           */
/*****************************************************************************/

// Creates a copy of hash table
static TMPQHash * CopyHashTable(TMPQArchive * ha)
{
    TMPQHash * pHashTableCopy = ALLOCMEM(TMPQHash, ha->pHeader->dwHashTableSize);

    if(pHashTableCopy != NULL)
        memcpy(pHashTableCopy, ha->pHashTable, sizeof(TMPQHash) * ha->pHeader->dwHashTableSize);

    return pHashTableCopy;
}

// This function defragments the block table removing unused entries
// Returns the number of USED block entries. Currently, do defragmentation
// is provided. Defragmentation of block table made some ugly effects on nameless
// access if some file has been deleted, archive compacted, and archive loaded.
/*
static UINT DefragmentBlockTable(TMPQArchive * ha)
{
    TMPQBlock * pBlockEnd = ha->pBlockTable + ha->pHeader->dwBlockTableSize;
    TMPQBlock * pBlockSrc = ha->pBlockTable;
    TMPQBlock * pBlockTrg = ha->pBlockTable;
    TMPQHash * pHashEnd = ha->pHashTable + ha->pHeader->dwHashTableSize;
    TMPQHash * pHash;
    UINT dwBlockIndex;

    while(pBlockSrc < pBlockEnd)
    {
        // If the file entry exists
        if(pBlockSrc->dwFlags & MPQ_FILE_EXISTS)
        {
            if(pBlockSrc != pBlockTrg)
            {
                dwBlockIndex = pBlockSrc - ha->pBlockTable;

                for(pHash = ha->pHashTable; pHash < pHashEnd; pHash++)
                {
                    if(pHash->dwBlockIndex == dwBlockIndex)
                    {
                        pHash->dwBlockIndex = pBlockTrg - ha->pBlockTable;
                        break;
                    }
                }
            }
            *pBlockTrg++ = *pBlockSrc++;
        }
        else
            pBlockSrc++;
    }

    // Return the number of used entries
    return pBlockTrg - ha->pBlockTable;
}
*/

static int CheckIfAllFilesKnown(TMPQArchive * ha, const char * szListFile, UINT * pFileSeeds)
{
    TMPQHash * pHashTableCopy = NULL;   // Copy of the hash table
    TMPQHash * pHash;
    TMPQHash * pHashEnd = NULL;         // End of the hash table
    UINT dwFileCount = 0;
    int nError = ERROR_SUCCESS;

    // First of all, create a copy of hash table
    if(nError == ERROR_SUCCESS)
    {
        if((pHashTableCopy = CopyHashTable(ha)) == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
        pHashEnd = pHashTableCopy + ha->pHeader->dwHashTableSize;
    }

    // Now check all the files from the filelist
    if(nError == ERROR_SUCCESS)
    {
        SFILE_FIND_DATA wf;
        HANDLE hFind = SFileFindFirstFile((HANDLE)ha, "*", &wf, szListFile);
        BOOL bResult = TRUE;

        // Do while some files have been found
        while(hFind != NULL && bResult)
        {
            TMPQHash * pHash = GetHashEntry(ha, wf.cFileName);
            pHash = pHashTableCopy + (pHash - ha->pHashTable);

            if(pHash->dwName1 != (UINT)-1 && pHash->dwName2 != (UINT)-1)
            {
                TMPQBlock * pBlock = ha->pBlockTable + pHash->dwBlockIndex;
                UINT dwSeed = 0;

                // Notify the user
                if(CompactCB != NULL)
                    CompactCB(lpUserData, CCB_CHECKING_FILES, dwFileCount++, ha->pHeader->dwBlockTableSize);

                // Resolve the file seed. Use plain file name for it
                if(pBlock->dwFlags & MPQ_FILE_ENCRYPTED)
                {
                    char * szFileName = strrchr(wf.cFileName, '\\');

                    if(szFileName == NULL)
                        szFileName = wf.cFileName;
                    else
                        szFileName++;

                    dwSeed = DecryptFileSeed(szFileName);
                    if(pBlock->dwFlags & MPQ_FILE_FIXSEED)
                        dwSeed = (dwSeed + (pBlock->dwFilePos - ha->dwMpqPos)) ^ pBlock->dwFSize;
                }
                pFileSeeds[pHash->dwBlockIndex] = dwSeed;
                pHash->dwBlockIndex = pHash->dwName1 = pHash->dwName2 = pHash->lcLocale = (UINT)-1;
            }
            bResult = SFileFindNextFile(hFind, &wf);
        }

        if(hFind != NULL)
            SFileFindClose(hFind);
    }

    // When the filelist checking is complete, parse the hash table copy and find the
    if(nError == ERROR_SUCCESS)
    {
        for(pHash = pHashTableCopy; pHash < pHashEnd; pHash++)
        {
            // If there is an unresolved entry, try to detect its seed. If it fails,
            // we cannot complete the work
            if(pHash->dwName1 != (UINT)-1 && pHash->dwName2 != (UINT)-1)
            {
                HANDLE hFile  = NULL;
                UINT dwFlags = 0;
                UINT dwSeed  = 0;

                if(SFileOpenFileEx((HANDLE)ha, (char *)(long)pHash->dwBlockIndex, 0, &hFile))
                {
                    TMPQFile * hf = (TMPQFile *)hFile;
                    dwFlags = hf->pBlock->dwFlags;
                    dwSeed = hf->dwSeed1;
                    SFileCloseFile(hFile);
                }

                // If the file is encrypted, we have to check 
                // If we can apply the file decryption seed
                if(dwFlags & MPQ_FILE_ENCRYPTED && dwSeed == 0)
                {
                    nError = ERROR_CAN_NOT_COMPLETE;
                    break;
                }

                // Notify the user
                if(CompactCB != NULL)
                    CompactCB(lpUserData, CCB_CHECKING_FILES, dwFileCount++, ha->pHeader->dwBlockTableSize);

                pFileSeeds[pHash->dwBlockIndex] = dwSeed;
            }
        }
    }

    // Delete the copy of hash table
    if(pHashTableCopy != NULL)
        FREEMEM(pHashTableCopy);
    return nError;
}

// Copies all file blocks into another archive.
static int CopyMpqFileBlocks(HANDLE hFile, TMPQArchive * ha, TMPQBlock * pBlock, UINT dwSeed)
{
    UINT * pdwBlockPos1 = NULL;        // File block positions
    UINT * pdwBlockPos2 = NULL;        // File block positions
    BYTE  * pbBlock = NULL;             // Buffer for the file block
    DWORD dwTransferred;                // Number of bytes transferred
    UINT dwFilePos = 0;                // Target file position
    UINT dwBytes = 0;                  // Number of bytes
    UINT dwSeed1 = 0;                  // File seed used for decryption
    UINT dwSeed2 = 0;                  // File seed used for encryption
    UINT nBlocks = 0;                  // Number of file blocks
    UINT nBlock = 0;                   // Currently processed file block
    int nError = ERROR_SUCCESS;

    // Calculate number of blocks in the file
    if(nError == ERROR_SUCCESS)
    {
        nBlocks = pBlock->dwFSize / ha->dwBlockSize;
        if(pBlock->dwFSize % ha->dwBlockSize)
            nBlocks++;
        pbBlock = ALLOCMEM(BYTE, ha->dwBlockSize);
        if(pbBlock == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // Set the position to the begin of the file within archive
    if(nError == ERROR_SUCCESS)
    {
        if(SetFilePointer(ha->hFile, pBlock->dwFilePos, NULL, FILE_BEGIN) != pBlock->dwFilePos)
            nError = GetLastError();

        // Use current position in target file for storing the file
        dwFilePos = SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
    }

    // Resolve decryption seeds. The 'dwSeed' parameter is the decryption
    // seed for the file.
    if(nError == ERROR_SUCCESS && (pBlock->dwFlags & MPQ_FILE_ENCRYPTED))
    {
        dwSeed1 = dwSeed;
        if(pBlock->dwFlags & MPQ_FILE_FIXSEED)
            dwSeed = (dwSeed1 ^ pBlock->dwFSize) - (pBlock->dwFilePos - ha->dwMpqPos);

        dwSeed2 = dwSeed;
        if(pBlock->dwFlags & MPQ_FILE_FIXSEED)
            dwSeed2 = (dwSeed + (dwFilePos - ha->dwMpqPos)) ^ pBlock->dwFSize;
    }

    // Load the file positions from the archive and save it to the target file
    // (only if the file is compressed)
    if(pBlock->dwFlags & MPQ_FILE_COMPRESSED)
    {
        // Allocate buffers
        if(nError == ERROR_SUCCESS)
        {
            pdwBlockPos1 = ALLOCMEM(UINT, nBlocks + 1);
            pdwBlockPos2 = ALLOCMEM(UINT, nBlocks + 1);

            if(pdwBlockPos1 == NULL || pdwBlockPos2 == NULL)
                nError = ERROR_NOT_ENOUGH_MEMORY;
        }

        // Load the block positions
        if(nError == ERROR_SUCCESS)
        {
            dwBytes = sizeof(UINT) * (nBlocks + 1);
            ReadFile(ha->hFile, pdwBlockPos1, dwBytes, &dwTransferred, NULL);
            CONVERTBUFFERTOLITTLEENDIAN32BITS((UINT *)pdwBlockPos1, (nBlocks + 1));
           
            memcpy(pdwBlockPos2, pdwBlockPos1, dwTransferred);

            if(dwTransferred != dwBytes)
                nError = ERROR_FILE_CORRUPT;
        }

        // Decrypt, if necessary. Also perform a simple test if the decryption seed if OK.
        if(nError == ERROR_SUCCESS && (pBlock->dwFlags & MPQ_FILE_ENCRYPTED))
        {
            DecryptMPQBlock(pdwBlockPos1, dwBytes, dwSeed1 - 1);

            memcpy(pdwBlockPos2, pdwBlockPos1, dwBytes);
            EncryptMPQBlock(pdwBlockPos2, dwBytes, dwSeed2 - 1);

            if(pdwBlockPos1[0] != (nBlocks + 1) * sizeof(UINT))
                nError = ERROR_FILE_CORRUPT;
        }

        // Write to the target file
        if(nError == ERROR_SUCCESS)
        {
            CONVERTBUFFERTOLITTLEENDIAN32BITS((UINT *)pdwBlockPos2, dwBytes/sizeof(UINT));
            WriteFile(hFile, pdwBlockPos2, dwBytes, &dwTransferred, NULL);
            if(dwTransferred != dwBytes)
                nError = ERROR_DISK_FULL;
        }
    }

    // Well. Now we have to copy all file block. We will do it without
    // recompression, because re-compression is not necessary in this case
    if(nError == ERROR_SUCCESS)
    {
        for(nBlock = 0; nBlock < nBlocks; nBlock++)
        {
            dwBytes = ha->dwBlockSize;
            if(nBlock == nBlocks - 1)
            {
                // Fix: The last block may be exactly the size of one block.
                dwBytes = pBlock->dwFSize - (ha->dwBlockSize * (nBlocks - 1));
            }

            if(pBlock->dwFlags & MPQ_FILE_COMPRESSED)
                dwBytes = pdwBlockPos1[nBlock+1] - pdwBlockPos1[nBlock];

            // Read the file block
            ReadFile(ha->hFile, pbBlock, dwBytes, &dwTransferred, NULL);
            if(dwTransferred != dwBytes)
            {
                nError = ERROR_FILE_CORRUPT;
                break;
            }

            // If necessary, re-encrypt the block
            if((pBlock->dwFlags & MPQ_FILE_ENCRYPTED) && dwSeed1 != dwSeed2)
            {
                CONVERTBUFFERTOLITTLEENDIAN32BITS((UINT *)pbBlock, dwBytes/sizeof(UINT));
                DecryptMPQBlock((UINT *)pbBlock, dwBytes, dwSeed1 + nBlock);
                EncryptMPQBlock((UINT *)pbBlock, dwBytes, dwSeed2 + nBlock);
                CONVERTBUFFERTOLITTLEENDIAN32BITS((UINT *)pbBlock, dwBytes/sizeof(UINT));
            }

            // Now write the block back to the file
            WriteFile(hFile, pbBlock, dwBytes, &dwTransferred, NULL);
            if(dwTransferred != dwBytes)
            {
                nError = ERROR_CAN_NOT_COMPLETE;
                break;
            }
        }
    }

    // Update file position in the block table
    if(nError == ERROR_SUCCESS)
        pBlock->dwFilePos = dwFilePos;

    // Cleanup and return
    if(pdwBlockPos2 != NULL)
        FREEMEM(pdwBlockPos2);
    if(pdwBlockPos1 != NULL)
        FREEMEM(pdwBlockPos1);
    if(pbBlock != NULL)
        FREEMEM(pbBlock);
    return nError;
}

static int CopyMpqFiles(HANDLE hFile, TMPQArchive * ha, UINT * pFileSeeds)
{
    UINT dwIndex;
    int nError = ERROR_SUCCESS;

    // Walk through all file and 
    for(dwIndex = 0; dwIndex < ha->pHeader->dwBlockTableSize; dwIndex++)
    {
        TMPQBlock * pBlock = ha->pBlockTable + dwIndex;
        UINT dwSeed1 = pFileSeeds[dwIndex];

        // Notify the caller about work
        if(CompactCB != NULL)
            CompactCB(lpUserData, CCB_COMPACTING_FILES, dwIndex, ha->pHeader->dwBlockTableSize);

        // Copy all the file blocks
        if(pBlock->dwFlags & MPQ_FILE_EXISTS)
        {
            nError = CopyMpqFileBlocks(hFile, ha, pBlock, dwSeed1);
            if(nError != ERROR_SUCCESS)
                break;
        }
    }

    // Cleanup and exit
    return nError;
}

/*****************************************************************************/
/* Public functions                                                          */
/*****************************************************************************/

BOOL WINAPI SFileSetCompactCallback(HANDLE /* hMPQ */, COMPACTCB aCompactCB, void * lpData)
{
    CompactCB = aCompactCB;
    lpUserData = lpData;
    return TRUE;
}

//-----------------------------------------------------------------------------
// Archive compacting (incomplete)

BOOL WINAPI SFileCompactArchive(HANDLE hMPQ, const char * szListFile, BOOL /* bReserved */)
{
    TMPQArchive * ha = (TMPQArchive *)hMPQ;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    UINT * pFileSeeds = NULL;
    char szTempFile[MAX_PATH] = "";
    char * szTemp = NULL;
    DWORD dwTransferred;
    int nError = ERROR_SUCCESS;

    // Test the valid parameters
    if(!IsValidMpqHandle(ha))
        nError = ERROR_INVALID_PARAMETER;

    // Create the table with file seeds
    if(nError == ERROR_SUCCESS)
    {
        if((pFileSeeds = ALLOCMEM(UINT, ha->pHeader->dwBlockTableSize)) != NULL)
            memset(pFileSeeds, 0, sizeof(UINT) * ha->pHeader->dwBlockTableSize);
        else
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // First of all, we have to check of we are able to decrypt all files.
    // If not, sorry, but the archive cannot be compacted.
    if(nError == ERROR_SUCCESS)
        nError = CheckIfAllFilesKnown(ha, szListFile, pFileSeeds);

    // Get the temporary file name and create it
    if(nError == ERROR_SUCCESS)
    {
        strcpy(szTempFile, ha->szFileName);
        if((szTemp = strrchr(szTempFile, '.')) != NULL)
            strcpy(szTemp + 1, "mp_");
        else
            strcat(szTempFile, "_");

        hFile = CreateFile(szTempFile, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            nError = GetLastError();
    }

    // Write the data before MPQ header (if any)
    if(nError == ERROR_SUCCESS && ha->dwMpqPos > 0)
    {
        UINT dwPosition;
        char szBuffer[0x200];

        SetFilePointer(ha->hFile, 0, NULL, FILE_BEGIN);
        for(dwPosition = 0; dwPosition < ha->dwMpqPos; dwPosition += sizeof(szBuffer))
        {
            ReadFile(ha->hFile, szBuffer, sizeof(szBuffer), &dwTransferred, NULL);
            if(dwTransferred != sizeof(szBuffer))
            {
                nError = ERROR_CAN_NOT_COMPLETE;
                break;
            }

            WriteFile(hFile, szBuffer, sizeof(szBuffer), &dwTransferred, NULL);
            if(dwTransferred != sizeof(szBuffer))
            {
                nError = ERROR_CAN_NOT_COMPLETE;
                break;
            }
        }
    }

    // Write the MPQ header
    if(nError == ERROR_SUCCESS)
    {
        CONVERTTMPQHEADERTOLITTLEENDIAN(ha->pHeader);
        WriteFile(hFile, &ha->pHeader, sizeof(TMPQHeader), &dwTransferred, NULL);
        CONVERTTMPQHEADERTOLITTLEENDIAN(ha->pHeader);
        if(dwTransferred != sizeof(TMPQHeader))
            nError = ERROR_DISK_FULL;
    }

    // Now write all file blocks.
    if(nError == ERROR_SUCCESS)
        nError = CopyMpqFiles(hFile, ha, pFileSeeds);

    // Now we need to update the tables positions
    // (but only if the tables are at the end of the file)
    if(nError == ERROR_SUCCESS)
    {
        ha->pHeader->dwHashTablePos   = SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
//      ha->pHeader->dwHashTableSize  = Unchanged;
        ha->pHeader->dwBlockTablePos  = ha->pHeader->dwHashTablePos + (ha->pHeader->dwHashTableSize * sizeof(TMPQHash));
//      ha->pHeader->dwBlockTableSize = Unchanged;
        ha->pHeader->dwArchiveSize    = ha->pHeader->dwBlockTablePos + (ha->pHeader->dwBlockTableSize * sizeof(TMPQBlock)) - ha->dwMpqPos;
    }

    // If succeeded, update the tables in the file
    if(nError == ERROR_SUCCESS)
    {
        CloseHandle(ha->hFile);
        ha->dwFilePos = 0;
        ha->hFile = hFile;
        hFile = INVALID_HANDLE_VALUE;
        nError = SaveMPQTables(ha);
    }

    // If all succeeded, switch the archives
    if(nError == ERROR_SUCCESS)
    {
        if(CompactCB != NULL)
            CompactCB(lpUserData, CCB_CLOSING_ARCHIVE, 0, 0);

        if(!DeleteFile(ha->szFileName) ||           // Delete the old archive
           !CloseHandle(ha->hFile)     ||           // Close the new archive
           !MoveFile(szTempFile, ha->szFileName))   // Rename the temporary archive
            nError = GetLastError();
    }

    // Now open the freshly renamed archive file
    if(nError == ERROR_SUCCESS)
    {
        ha->hFile = CreateFile(ha->szFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if(ha->hFile == INVALID_HANDLE_VALUE)
            nError = GetLastError();
    }

    // Invalidate the positions of the archive
    if(nError == ERROR_SUCCESS)
    {
        ha->pLastFile  = NULL;
        ha->dwBlockPos = 0;
        ha->dwBuffPos  = 0;
        ha->dwFilePos  = 0;
    }

    // Cleanup and return
    if(hFile != INVALID_HANDLE_VALUE)
        CloseHandle(hFile);
    if(pFileSeeds != NULL)
        FREEMEM(pFileSeeds);
    if(nError != ERROR_SUCCESS)
        SetLastError(nError);
    DeleteFile(szTempFile);
    CompactCB = NULL;
    return (nError == ERROR_SUCCESS);
}
