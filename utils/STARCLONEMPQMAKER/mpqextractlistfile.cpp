
#include <stdio.h>
#include <mpqlib/StormLib.h>

//#define MPQFileName1 "StarDat.mpq"
//#define MPQFileName1 "BrooDat.mpq"
//#define MPQFileName1 "starinstall.exe"
//#define MPQFileName1 "broodinstall.exe"
//#define MPQFileName1 "Patch_rt.mpq"
#define MPQFileName1 "StarClone.mpq"

#define OUTDIR "./EXTRACTED/"
//#define EXTRACTBYNUMBER
//=========================================
HANDLE mpq[1];
char filename[]="(listfile)";
//=========================================
int main(void)
{
    int result,len2,err,mpqfiles,filenr;
    result = SFileOpenArchive(MPQFileName1,0,SFILE_OPEN_HARD_DISK_FILE,&mpq[0]);
    if (!result)
    {
	printf("error open mpq1 archive\n");
	return -1;
    }
    mpqfiles=SFileGetFileInfo(mpq[0],SFILE_INFO_NUM_FILES);
    printf("total %s archive files = %d\n",MPQFileName1,mpqfiles);
	    if (SFileHasFile(mpq[0],filename))
	    {
		err=SFileExtractFile(mpq[0],filename,filename);
		if (!err)
		    ;//printf("%s -> %s FAILED\n",filename,buffdir);
		else
		    printf("%s -> %s OK\n",filename,filename);
	    }
    SFileCloseArchive(mpq[0]);
}
