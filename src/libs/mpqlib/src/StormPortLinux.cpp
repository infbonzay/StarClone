/********************************************************************
*
* Description:	implementation for StormLib - linux port
*		intended to be used in GLdiablo
*		
*	---->	StormLib was originally developed for Windows by
*		Ladislav Zezula (www.zezula.net), and he did
*		a _great_ job! Thanks Ladislav!
*
*	this is currently a quick and dirty hack to get it working
*	 don't expect beauty and/or miracles :)
*
*	these are function wraps to execute Windows API calls
*        as native Macintosh file calls (open/close/read/write/...)
*
* Author: Marko Friedemann <marko.friedemann@bmx-chemnitz.de>
* Created at: Mon Jan 29 19:01:37 CEST 2001
* Computer: whiplash.flachland-chemnitz.de 
* System: Linux 2.4.0 on i686
*    
* Copyright (c) 2001 BMX-Chemnitz.DE All rights reserved.
*
********************************************************************/

#ifndef _WIN32
#include "StormPort.h"

int globalerr;

void SetLastError(int err)
{
	globalerr = err;
}

int GetLastError()
{
	return(globalerr);
}

const char *ErrString(int err)
{
	switch (err) {
	case ERROR_INVALID_FUNCTION:
		return "function not implemented";
	case ERROR_FILE_NOT_FOUND:
		return "file not found";
	case ERROR_ACCESS_DENIED:
		return "access denied";
	case ERROR_NOT_ENOUGH_MEMORY:
		return "not enough memory";
	case ERROR_BAD_FORMAT:
		return "bad format";
	case ERROR_NO_MORE_FILES:
		return "no more files";
	case ERROR_HANDLE_EOF:
		return "access beyound EOF";
	case ERROR_HANDLE_DISK_FULL:
		return "no space left on device";
	case ERROR_INVALID_PARAMETER:
		return "invalid parameter";
	case ERROR_DISK_FULL:
		return "no space left on device";
	case ERROR_ALREADY_EXISTS:
		return "file exists";
	case ERROR_CAN_NOT_COMPLETE:
		return "operation cannot be completed";
	default:
		return "unknown error";
	}
}

HANDLE CreateFile(const char *sFileName, UINT ulMode, UINT ulSharing, void *pSecAttrib, UINT ulCreation, UINT ulFlags, HANDLE hFile)
{
    switch (ulCreation) {
    case OPEN_EXISTING:
        return (HANDLE)(long )open(sFileName, O_RDONLY,S_IREAD|S_IWRITE);
    case OPEN_ALWAYS:
    case CREATE_ALWAYS:
        return (HANDLE)(long)open(sFileName, O_RDWR | O_CREAT,S_IREAD|S_IWRITE);
    case CREATE_NEW:
	return (HANDLE)(long)open(sFileName, O_RDWR | O_CREAT | O_TRUNC,S_IREAD|S_IWRITE);
    default:
        return INVALID_HANDLE_VALUE;
    }
}

BOOL CloseHandle(HANDLE hFile)
{
    return (close((DWORD)hFile) == 0);
}

UINT GetFileSize(HANDLE hFile, DWORD *ulOffSetHigh)
{
    if ((hFile == NULL) || (hFile == INVALID_HANDLE_VALUE))
        return 0xffffffff;

    struct stat fileinfo;
    fstat((DWORD)hFile, &fileinfo);
    
    return fileinfo.st_size;
}

UINT SetFilePointer(HANDLE hFile, LONG lOffSetLow, LONG *pOffSetHigh, UINT ulMethod)
{
    return lseek((DWORD)hFile, lOffSetLow, ulMethod);
}

BOOL SetEndOfFile(HANDLE hFile)
{
    return (ftruncate((DWORD)hFile, lseek((DWORD)hFile, 0, SEEK_CUR)) == 0);
}

BOOL ReadFile(HANDLE hFile, void *pBuffer, DWORD ulLen, DWORD *ulRead, void *pOverLapped)
{
    ssize_t count;
    if ((count = read((DWORD)hFile, pBuffer, ulLen)) == -1) {
        *ulRead = 0;
        return false;
    }
    *ulRead = count;
    return true;
}

BOOL WriteFile(HANDLE hFile, const void *pBuffer, DWORD ulLen, DWORD *ulWritten, void *pOverLapped)
{
    ssize_t count;
    if ((count = write((DWORD)hFile, pBuffer, ulLen)) == -1) {
        *ulWritten = 0;
        return false;
    }
    *ulWritten = count;
    return true;
}

// Check if a memory block is accessible for reading
BOOL IsBadReadPtr(const void * ptr, int size)
{
    return FALSE;
}

// Returns attributes of a file
UINT GetFileAttributes(const char * szFileName)
{
    return 0;
}
/********************************************************************
*	DeleteFile
*		lpFileName: file path
********************************************************************/
BOOL DeleteFile(const char * lpFileName)
{
    return(unlink(lpFileName)==0);
}
/*******************************************************************
*	MoveFile
*		lpFromFileName: old file path
*		lpToFileName: new file path
********************************************************************/
BOOL MoveFile(const char * lpFromFileName, const char * lpToFileName)
{
    return(rename(lpFromFileName,lpToFileName)==0);
}
/********************************************************************
*	GetTempPath - returns a './'
*		szTempLength: length for path
*		szTemp: file path
********************************************************************/
void GetTempPath(UINT szTempLength, char * szTemp)
{
    strcpy(szTemp,"./");
}
/********************************************************************
*	GetTempFileName
*		lpTempFolderPath: the temporary folder path, terminated by "/"
*		lpFileName: a file name base
*		something: unknown
*		szLFName: the final path, built from the path, the file name and a random pattern
********************************************************************/
void GetTempFileName(const char * lpTempFolderPath, const char * lpFileName, UINT something, char * szLFName)
{
    char *fn = tempnam(lpTempFolderPath,lpFileName);
    strcpy(szLFName,fn);
    free(fn);
}
#endif

