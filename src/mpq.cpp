
#include <grplib/grp.h>
#include "mpqwrapper.h"

#include "auxil.h"
#include "wmem.h"
#include "mylist.h"
#include "mpq.h"

#include "tbl.h"

mylist allmpq;
HANDLE install1mpq, install2mpq;					//install files for star & brood disks
//============================================
#ifdef WITHSTORMLIB
UINT SFileGetFileInfo(HANDLE hMpqOrFile, UINT dwInfoType)
{
	DWORD dwTotalBytes = 0;
	SFileGetFileInfo(hMpqOrFile, dwInfoType, &dwTotalBytes, sizeof(UINT), NULL);
	return(dwTotalBytes);
}
#endif
//============================================
int loadfilefrommpq(HANDLE hmpq, const char *filename, char **mem, int *fsize)
{
	int err;
	HANDLE f;
	unsigned int filesize, readed;

	if (!hmpq)
		err = (SFileOpenFileTryAllMpqs(filename, &f) == 0);
	else
		err = wr_SFileOpenFileEx(hmpq, filename, SFILE_OPEN_FROM_MPQ, &f);
	if (!err)
	{
		DEBUGMESSCR("%s not found\n", filename);
		return -1;
	}
	filesize = SFileGetFileSize(f, NULL);
	if (fsize)
		*fsize = filesize;
	*mem = (char *)wmalloc(filesize);			//try to alloc memory for file
	if (*mem == NULL)
	{
		wr_SFileCloseFile(f);
		return (-2);										//no memory
	}
	wr_SFileReadFile(f, *mem, filesize, &readed, NULL);
	if (readed != filesize)
	{
		wfree(*mem);							//wfree memory allocated with grp
		wr_SFileCloseFile(f);
		return(-3);
	}
	wr_SFileCloseFile(f);
	return 0;
}
//============================================
void unloadfilefrommpq(void *mem)
{
	if (mem)
	{
		wfree(mem);
	}
}
//============================================
HANDLE MpqsFindFile(const char *filename)
{
	int nrmpqfiles = allmpq.GetMaxElements();
	for (int i = 0;i < nrmpqfiles;i++)				//find file with priority
	{
		int err = SFileHasFile(allmpq.GetElemNr(i), filename);
		if (err)
			return allmpq.GetElemNr(i);
	}
	return 0;
}
//============================================
HANDLE LoadMpq(const char *MPQFileName)
{
	HANDLE mpq;
	int result;
	result = wr_SFileOpenArchive(MPQFileName, 0, MPQOPENPARAM, &mpq);
	if (!result)
		return NULL;
	allmpq.AddList(mpq);
	return(mpq);
}
//============================================
void UnLoadAllMpqs(void)
{
	for (int i = 0;i < allmpq.GetMaxElements();i++)
		wr_SFileCloseArchive(allmpq.GetElemNr(i));
	allmpq.FlushList();
	if (install1mpq)
	{
		wr_SFileCloseArchive(install1mpq);
		if (install1mpq == install2mpq)//if we have one archive for all
			install2mpq = NULL;
		install1mpq = NULL;
	}
	if (install2mpq)
	{
		wr_SFileCloseArchive(install2mpq);
		install2mpq = NULL;
	}
}
//============================================
int mpqloadfile(const char *filename, char **mem)
{
	HANDLE retmpqnr = MpqsFindFile(filename);
	//	  if (!retmpqnr)
	//		return -1;
	return(loadfilefrommpq(retmpqnr, filename, mem, NULL));
}
//============================================
int GetMpqFilesNr(HANDLE hmpq)
{
	return(SFileGetFileInfo(hmpq, SFILE_INFO_NUM_FILES));
}
//============================================
HANDLE GetFilePosInArchive(HANDLE mainmpq, const char *filename, int *nr)
{
	HANDLE mpq, f;
	*nr = -1;
	if (mainmpq)
		mpq = mainmpq;
	else
		mpq = MpqsFindFile(filename);
	if (!mpq)
		return NULL;
	if (!wr_SFileOpenFileEx(mpq, filename, SFILE_OPEN_FROM_MPQ, &f))
		return NULL;
	*nr = SFileGetFileInfo(f, SFILE_INFO_BLOCKINDEX);
	wr_SFileCloseFile(f);
	return mpq;
}
//============================================
#ifdef WITHSTORMLIB
static char stormlibfilename[20];
int SFileEOF(HANDLE f)
{
	TMPQFile * hf = (TMPQFile *)f;
	DWORD fpos = hf->dwFilePos;
	DWORD fsize = SFileGetFileInfo(f, SFILE_INFO_FILE_SIZE);
	return (fpos >= fsize);
}
#undef SFileOpenFileEx
UINT SFileOpenFileExStormLibVersion(HANDLE hMpq, const char * szFileName, UINT dwSearchScope, HANDLE * phFile)
{
	if ((unsigned long)szFileName < 0x00010000)//opened file by number
	{
		//the new stormlib need to have FileXXXXXXXX.ext names for open file by index
		//where XXXXXXXX index of file
		sprintf(stormlibfilename, "File%08ld.ext", (unsigned long)szFileName);
		return(wr_SFileOpenFileEx(hMpq, stormlibfilename, dwSearchScope, phFile));
	}
	else
	{
		return(wr_SFileOpenFileEx(hMpq, szFileName, dwSearchScope, phFile));
	}
}
#undef SFileHasFile
UINT SFileHasFileStormLibVersion(HANDLE hMpq, const char * szFileName)
{
	if ((unsigned long)szFileName < 0x00010000)//opened file by number
	{
		//the new stormlib need to have FileXXXXXXXX.ext names for open file by index
		//where XXXXXXXX index of file
		sprintf(stormlibfilename, "File%08ld.ext", (unsigned long)szFileName);
		return(SFileHasFile(hMpq, stormlibfilename));
	}
	else
	{
		return(SFileHasFile(hMpq, szFileName));
	}
}
#endif
//============================================
#ifdef WITHMPQLIB
int SFileEOF(HANDLE f)
{
	TMPQFile * hf = (TMPQFile *)f;
	DWORD fpos = hf->dwFilePos;
	DWORD fsize = hf->pBlock->dwFSize;
	return (fpos >= fsize);
}
#endif
//============================================
int	 GetMPQFileID(HANDLE mpq, const char *filesound, MPQIDS *fileid)
{
	fileid->hmpq = GetFilePosInArchive(mpq, filesound, &fileid->mpqfilenr);
	return(fileid->mpqfilenr);
}
//============================================
int	 CmpMPQFileID(MPQIDS *fileid1, MPQIDS *fileid2)
{
	if (fileid1->hmpq == fileid2->hmpq && fileid1->mpqfilenr == fileid2->mpqfilenr)
		return(1);
	return(0);
}
//============================================
void SetMPQFileID(MPQIDS *fileid1, MPQIDS *fileid2)
{
	fileid1->hmpq = fileid2->hmpq;
	fileid1->mpqfilenr = fileid2->mpqfilenr;
}
//============================================
void ClearMPQFileID(MPQIDS *fileid1)
{
	fileid1->hmpq = NULL;
	fileid1->mpqfilenr = -1;
}
//============================================
HANDLE FindFileTryAllMpqs(const char *filename)
{
	if (SFileHasFile(install2mpq, filename))
		return(install2mpq);
	if (SFileHasFile(install1mpq, filename))
		return(install1mpq);
	return(MpqsFindFile(filename));
}
//============================================
int SFileOpenFileTryAllMpqs(const char *filename, HANDLE *f)
{
	HANDLE hmpq;
	int result;
	char *filename2 = filename2unix(filename);
	result = wr_SFileOpenFileEx(NULL, filename2, SFILE_OPEN_LOCAL_FILE, f);
	wfree(filename2);
	if (!result)
	{
		hmpq = FindFileTryAllMpqs(filename);
		if (!hmpq)
			return(-1);
		result = wr_SFileOpenFileEx(hmpq, filename, SFILE_OPEN_FROM_MPQ, f);
		if (!result)
			return(-2);
	}
	else
	{
		DEBUGMESSCR("SFileOpenFileTryAllMpqs: %s found on local filesystem\n", filename);
	}
	return(0);
}
//============================================
