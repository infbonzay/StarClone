#if !defined(_MPQ_W)
#define _MPQ_W

#ifdef WITHMPQLIB
    #include <mpqlib/StormLib.h>
    #define MPQOPENPARAM SFILE_OPEN_HARD_DISK_FILE

#endif
#ifdef WITHSTORMLIB
    #include <StormLib/StormLib.h>
    #define MPQOPENPARAM (MPQ_OPEN_FORCE_MPQ_V1 | STREAM_FLAG_READ_ONLY | MPQ_OPEN_NO_LISTFILE)
    #define SFileOpenFileEx SFileOpenFileExStormLibVersion
    #define SFileHasFile SFileHasFileStormLibVersion
    typedef unsigned int  UINT;
    UINT SFileGetFileInfo(HANDLE hMpqOrFile, UINT dwInfoType);
    UINT SFileOpenFileExStormLibVersion(HANDLE hMpq, const char * szFileName, UINT dwSearchScope, HANDLE * phFile);
    UINT SFileHasFileStormLibVersion(HANDLE hMpq, const char * szFileName);
#endif

#include "mpqwrapper.h"
#include "mylist.h"
struct MPQIDS
{
    HANDLE	hmpq;
    int		mpqfilenr;    
};
struct MPQPRIO
{
    unsigned char prio;
    unsigned char presence;
    char mpqfilename[MAX_PATH];
};
//============================================
int  loadfilefrommpq(HANDLE mpq,char *filename,char **mem,int *fsize);
void unloadfilefrommpq(void *mem);
HANDLE MpqsFindFile(char *filename);
HANDLE LoadMpq(char *MPQFileName);
void UnLoadAllMpqs(void);
int  mpqloadfile(char *filename,char **mem);
int  GetMpqFilesNr(HANDLE hmpq);
HANDLE GetFilePosInArchive(HANDLE mpq,char *filename,int *nr);
int  SFileEOF(HANDLE f);
int  GetMPQFileID(HANDLE mpq,char *filesound,MPQIDS *fileid);
int  CmpMPQFileID(MPQIDS *fileid1,MPQIDS *fileid2);
void SetMPQFileID(MPQIDS *fileid1,MPQIDS *fileid2);
void ClearMPQFileID(MPQIDS *fileid1);
HANDLE FindFileTryAllMpqs(char *filename);
int  SFileOpenFileTryAllMpqs(char *filename,HANDLE *f);
//==========================================
extern mylist allmpq;
extern HANDLE install1mpq,install2mpq;

#endif /*_MPQ_W*/
