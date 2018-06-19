#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mpq.h"

#include "auxil.h"
#include "wmem.h"
#include "debug.h"
#include "tbl.h"
//===========================================
int TBL::loadTBL(char *filename)
    {
	tbl=NULL;
	if (mpqloadfile(filename,(char **)&tbl))
	    return(-1);
	DEBUGMES("***********************%s\n",filename);
	log_STRS();

	return(0);
    }
//===========================================
    TBL::~TBL(void)
    {
	if (tbl)
	{
	    wfree(tbl);
	    tbl=NULL;
	}
    }
//===========================================
char * TBL::get_TBL_STR(int stringnr)
{
    if (stringnr>=tbl->strnr)
	return NULL;
    return(((char *)tbl)+tbl->offsetfrombegin[stringnr]);
}
//==========================================
char * TBL::get_TBL_SUBSTR(int stringnr,int substring)
{
    char *adr=this->get_TBL_STR(stringnr);
    if (!substring)
	return(adr);
    do{
	while(*adr++==0)
	{
	    if (--substring==0)
		return(adr);
	}
    }while(1);
}
//==========================================
int TBL::get_TBL_STRSize(int stringnr)
{
    int nrofstr=0;
    char *adr1,*adr2;
    if (stringnr>=tbl->strnr)
	return 0;
    adr1=this->get_TBL_STR(stringnr++);
    if (stringnr>=tbl->strnr)
	return 1;
    adr2=this->get_TBL_STR(stringnr);
    if (adr2<=adr1)
	return(1);
    while(adr1<adr2)
    {
	if (*adr1++==0)
	    nrofstr++;
    }
    return(nrofstr);
}
//==========================================
int TBL::get_STRS(void)
{
    return(tbl->strnr);
}
//==========================================
void TBL::log_STRS(void)
{
    for (int i=1;i<this->get_STRS();i++)
    {
	DEBUGMES("%s %d\n",this->get_TBL_STR(i),i);
    }
}
//==========================================
int checkfordublicate(char *textdata,int sizex,char *textcmp)
{
    int i,j,len,mfind;
    len=strlen(textcmp)+1;	//find with 0-terminating string
    for (i=0;i<sizex-len+1;i++)
    {
	mfind=0;
	for (j=0;j<len;j++)
	{
	    if (textdata[i+j]!=textcmp[j])
	    {
		mfind=1;
		break;
	    }
	}
	if (!mfind)
	{
	    return(i);
	}
    }
    return(-1);
}
//==========================================
void saveTBL(int strnr,char *filename,char *arrayofstrs[])
{
    int tbloffset,i,len,firstnull=-1;
    char *tbldata;
    TBL_FILE *tblfile;
    tbldata=(char *) wmalloc(50000);
    memset(tbldata,0,50000);
    tblfile=(TBL_FILE *)tbldata;
    tblfile->strnr=strnr;
    tbloffset=sizeof(unsigned short)+sizeof(unsigned short)*strnr;
    for (i=0;i<strnr;i++)
    {
	if (arrayofstrs[i])
	{
	    int havetxt=checkfordublicate(tbldata,tbloffset,arrayofstrs[i]);
	    if (havetxt==-1)
	    {
		tblfile->offsetfrombegin[i]=tbloffset;
		len=strlen(arrayofstrs[i])+1;
		strncpy(tbldata+tbloffset,arrayofstrs[i],len);
	    }
	    else
	    {
		tblfile->offsetfrombegin[i]=havetxt;
		len=0;
	    }	
	}
	else
	{
	    tblfile->offsetfrombegin[i]=firstnull;
	    len=0;
	}
	tbloffset+=len;
	if (firstnull==-1)
	    firstnull=tbloffset-1;
    }
    savebuff(filename,tbldata,tbloffset);
    wfree(tbldata);
}

