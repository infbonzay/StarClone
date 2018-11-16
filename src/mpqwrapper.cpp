//this wrapper set because the mpqlib is'nt reentrant, so before use we need to lock audio to prevent bad reading


#include "audiolowlevel.h"
#include "mpq.h"
//============================================
int wr_SFileReadFile(HANDLE hFile, void *lpBuffer, unsigned int dwToRead, unsigned int *pdwRead, void *lpOverlapped)
{
	LockAudio();
	int ret = SFileReadFile(hFile, lpBuffer, dwToRead, pdwRead, lpOverlapped);
	UnlockAudio();
	return(ret);
}
//============================================
int wr_SFileCloseFile(HANDLE hFile)
{
	LockAudio();
	int ret = SFileCloseFile(hFile);
	UnlockAudio();
	return(ret);
}
//============================================
int wr_SFileOpenArchive(const char * szMpqName, unsigned int dwPriority, unsigned int dwFlags, HANDLE *phMPQ)
{
	LockAudio();
	int ret = SFileOpenArchive(szMpqName, dwPriority, dwFlags, phMPQ);
	UnlockAudio();
	return(ret);
}
//============================================
int wr_SFileOpenFileEx(HANDLE hMPQ, const char * szFileName, unsigned int dwSearchScope, HANDLE *phFile)
{
	LockAudio();
	int ret = SFileOpenFileEx(hMPQ, szFileName, dwSearchScope, phFile);
	UnlockAudio();
	return(ret);
}
//============================================
int wr_SFileCloseArchive(HANDLE hMPQ)
{
	LockAudio();
	int ret = SFileCloseArchive(hMPQ);
	UnlockAudio();
	return(ret);
}
