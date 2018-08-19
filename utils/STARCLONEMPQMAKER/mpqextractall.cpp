
#include <stdio.h>
#include <mpqlib/StormLib.h>

//#define MPQFileName1 "StarDat.mpq"
#define MPQFileName1 "BrooDat.mpq"
//#define MPQFileName1 "starinstall.exe"
//#define MPQFileName1 "broodinstall.exe"
//#define MPQFileName1 "Patch_rt.mpq"
//#define MPQFileName1 "StarClone.mpq"

#define OUTDIR "./EXTRACTED/"
//#define EXTRACTBYNUMBER
//=========================================
HANDLE mpq[1];
char filename[500];
char buffdir[500];
char mask[]="sound\\";
//=========================================
int maskequal(char *filename,char *mask)
{
    return(strstr(filename,mask)!=NULL);
}
//=========================================
void mymkdir(char *filename,int len)
{
    int i,ii,previ=-1;
    strcpy(buffdir,OUTDIR);
    for (i=strlen(buffdir),ii=0;ii<len;i++,ii++)
    {
	buffdir[i]=filename[ii];
	if (buffdir[i]=='\\')
	{
	    buffdir[i]='/';
	    buffdir[i+1]=0;
	    previ=i+1;
	    mkdir(buffdir,0766);
	}
    }
    if (previ!=-1)
	buffdir[previ]=0;
}
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
#ifndef EXTRACTBYNUMBER
    FILE *f = fopen("(listfile)","rb");
    if (!f)
    {
	SFileExtractFile(mpq[0],mask,mask);
	printf("error open ./(listfile),extract %s\n",mask);
	return -1;
    }
#endif
    mkdir(OUTDIR,S_IRWXU|S_IRWXG|S_IRWXO);
    filenr=0;
    do{
#ifndef EXTRACTBYNUMBER
	fgets(filename,sizeof(filename)-1,f);
	int j=strlen(filename);
	int i;
	for (i=0;i<j;i++)
	{
	    if (filename[i]=='\xa'||
		filename[i]=='\xd')
		filename[i]=0;
	}
	if (maskequal(filename,mask))
	{
	    if (SFileHasFile(mpq[0],filename))
	    {
		mymkdir(filename,j);
		for (i=j;i>=0;i--)
		{
		    if (filename[i]=='\\')
		    {
			i++;
			break;
		    }
		}
		if (i>=0)
		    strcat(buffdir,filename+i);
		err=SFileExtractFile(mpq[0],filename,buffdir);
		if (!err)
		    printf("%s -> %s FAILED\n",filename,buffdir);
		else
		    printf("%s -> %s OK\n",filename,buffdir);
	    }
	    else
		printf("%s -> %s FAILED\n",filename,buffdir);

	}
#else
	sprintf(filename2,"%s/file%05u.dat",OUTDIR,filenr);
	err=SFileExtractFile(mpq[0],(char *)filenr,filename2);
	filenr++;
#endif
#ifndef EXTRACTBYNUMBER
    }while(!feof(f));
    fclose(f);
#else
    }while(filenr!=mpqfiles);
#endif
    SFileCloseArchive(mpq[0]);
}
