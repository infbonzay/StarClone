#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//#define MPQLIB
//#define STORMLIB

#ifdef MPQLIB
    #include <mpqlib/StormLib.h>
    #include <mpqlib/StormPort.h>
#endif
#ifdef STORMLIB
    #include <StormLib/StormLib.h>
    #include <StormLib/StormPort.h>
#endif

char *filename2unix(char *fn)
{
    char *ret;
    int i,j;
    j = strlen(fn);
    ret=(char *)malloc(j+1);
    memcpy(ret,fn,j);
    ret[j]=0;
    for (i=0;i<j;i++)
	if (ret[i]=='\\')
	    ret[i]='/';
    return(ret);
}

int main(void)
{
    int err;
    char *filename2;
    char mystr[1024];
    char filename[1024];
    HANDLE mympq;
    FILE *f = fopen("(listfile)","rb");
    if (!f)
    {
	printf("file (listfile) not found\n");
	return(-1);
    }
#ifdef STORMLIB
    err=SFileCreateArchive("StarClone.mpq",MPQ_CREATE_ARCHIVE_V1,HASH_TABLE_SIZE_DEFAULT,&mympq);
#endif
#ifdef MPQLIB
    err=SFileCreateArchiveEx("StarClone.mpq",CREATE_NEW,0x1000,&mympq);
#endif
    if (!err||!mympq)
    {
	printf("Error create mpq archive\n");
	fclose(f);
	return(-1);
    }

    do{
	fscanf(f,"%s\n",mystr);
	if (!mystr[0])
	    break;
	sprintf(filename,"../../../StarClone.oldfiles/%s",mystr);
	filename2=filename2unix(filename);
	if (access(filename2,R_OK))
	{
	    printf("!!!!!!!!!!!!!!!!\ncannot open %s for reading\n",filename2);
	    exit(-1);
	}
	else
	{
#ifdef STORMLIB
	    err=SFileAddFile(mympq,filename2,mystr,MPQ_FILE_COMPRESS);
#endif
#ifdef MPQLIB
	    err=SFileAddFile(mympq,filename2,mystr,MPQ_FILE_COMPRESS_PKWARE);
#endif
	    if (err)
		printf("Success Add %s (%s)\n",filename2,mystr);
	    else
	    {
		printf("!!!!!!!!!!!!!!\nError Add %s (%s)\n",filename2,mystr);
		exit(-1);
	    }
	}
	free(filename2);
	mystr[0]=0;
    }while(1);
    SFileCloseArchive(mympq);
    fclose(f);
    return 0;
}
