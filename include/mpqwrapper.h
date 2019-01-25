#ifndef _SC_MPQWRAPPER_H

#define _SC_MPQWRAPPER_H

#include "mpq.h"

int wr_SFileReadFile(HANDLE hFile, void *lpBuffer,unsigned long dwToRead,unsigned long *pdwRead,void *lpOverlapped);
int wr_SFileCloseFile(HANDLE hFile);
int wr_SFileOpenArchive(const char * szMpqName,unsigned int dwPriority,unsigned int dwFlags,HANDLE *phMPQ);
int wr_SFileOpenFileEx(HANDLE hMPQ,const char * szFileName,unsigned int dwSearchScope,HANDLE *phFile);
int wr_SFileCloseArchive(HANDLE hMPQ);

#endif /*_MPQWRAPPER_W*/
