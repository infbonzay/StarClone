#include <stdlib.h>
#include <stdio.h>
#include "debug.h"
#include "wmem.h"

//=============================
#ifdef USEWMALLOC

#ifdef TESTMALLOC
    #define MALLOCBYTE1	0x3c
    #define MALLOCBYTE2	0x5a
    #define MALLOCBYTE3	0x69
    #define MALLOCBYTE4	0xc3
#endif

void *wmalloc(int memsize)
{
#ifdef TESTMALLOC
    unsigned char *retval;
    int *retval2;

    memsize+=12;
    retval=(unsigned char *)malloc(memsize);
    retval[0]=MALLOCBYTE1;
    retval[1]=MALLOCBYTE2;
    retval[2]=MALLOCBYTE3;
    retval[3]=MALLOCBYTE4;
    retval2=(int *)&retval[4];
    *retval2=memsize-12;
    retval[memsize-4]=0xc3;
    retval[memsize-3]=0x96;
    retval[memsize-2]=0xa5;
    retval[memsize-1]=0x3c;
#ifdef CHECKALLMALLOC
    checkallwmalloc(retval+8,1);
#endif
    return(retval+8);
#else
    return(malloc(memsize));
#endif

}
//==========================
void wfree(void *buf)
{
#ifdef TESTMALLOC
    char *tempbuf=(char *)buf;
    char *buff=(char *)buf;
#ifdef CHECKALLMALLOC
    checkallwmalloc(buf,-1);
#else
    if (checkleakbefore(buf))
    {
	printf("memory leak in free before allocation\n");
    }
    if (checkleakafter(buf))
    {
	printf("memory leak in free after allocation\n");
    }
#endif
    free((char *)buf-8);
#else
//    DEBUGMESSCR("freed = 08%x\n",buf);
    free(buf);
#endif
}

//==========================
int checkleakbefore(void *buf)
{
#ifdef TESTMALLOC
    char *tempbuf=(char *)buf;
    tempbuf-=8;
    int *size=(int *)((char *)tempbuf+4);
    if (tempbuf[0]!=MALLOCBYTE1||tempbuf[1]!=MALLOCBYTE2||tempbuf[2]!=MALLOCBYTE3||tempbuf[3]!=MALLOCBYTE4)
    {
	return(1);
    }
#endif
    return(0);
}
//==========================
int checkleakafter(void *buf)
{
#ifdef TESTMALLOC
    char *tempbuf=(char *)buf;
    tempbuf-=8;
    int *size=(int *)((char *)tempbuf+4);
    tempbuf=tempbuf+8+*size;
    if (tempbuf[0]!=0xc3||tempbuf[1]!=0x96||tempbuf[2]!=0xa5||tempbuf[3]!=0x3c)
    {
	return(2);
    }
#endif
    return(0);
}
//==========================
int checkleak(void *buf)
{
    int err = checkleakbefore(buf);
    if (err)
	return(err);
    err = checkleakafter(buf);
    if (err)
	return(err);
    return(0);
}
//==========================
void *allmems[100000];
int allmallocs;
//==========================
void checkallwmalloc(void *buf,int incrval)
{

}

#endif



