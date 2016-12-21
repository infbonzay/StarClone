
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
    

#include "tbl.h"
#include "stringfiles.h"

char GAMEPATH[1];

FILE *f;
long long filesize;
int STRS,i;
char *strarray[10000];
int main(int argc,char *argv[])
{
    if (argc!=2)
    {
	printf("usage: %s textfile\n",argv[0]);
	return (-1);
    }
    f=fopen(argv[1],"r");
    if (!f)
    {
	printf("Error! %s inputfile not found\n",argv[1]);
	return(-1);
    }
    fseek(f,0,SEEK_END);
    filesize = ftell(f);
    fseek(f,0,SEEK_SET);
    printf("filesize==%lld\n",filesize);
    char *memstr=(char *)malloc(filesize);
    fread(memstr,1,filesize,f);
    char *str=memstr;
    for (i=0;i<filesize;i++)
    {
	if (memstr[i]=='\x0a')
	{
	    memstr[i]=0;
	    strarray[STRS]=str;
	    STRS++;
	    str=&memstr[i+1];
	}
	else if (memstr[i]=='#')
	    memstr[i]='\x0a';
    }
    fclose(f);
    printf("total strings=%d\n",STRS);
    saveTBL(STRS,"out.tbl",strarray);
    free(memstr);
}

