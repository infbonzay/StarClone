
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
    

#include <sys/time.h>
#include "mylist.h"
timeval tm;
long long usec;

//#define MAXCYCLES	1000
//#define MAXELEMS	10000

#define MAXCYCLES	1
#define MAXELEMS	500
#define PRINTF
int main(void)
{
    int i,j;
    void *elem;
    mylist a;
    for (i=0;i<MAXELEMS;i++)
	a.AddList((void *)(i+1000));

    a.DelList(0);
    a.DelList(11);
    a.DelList(59);
    a.DelList(71);
    for (i=100;i<200;i++)
	a.DelList(i);
    a.DelList(209);
    a.DelList(297);
    a.DelList(321);
    a.DelList(375);
    a.DelList(419);
    a.DelList(441);
    gettimeofday(&tm,NULL);
    usec=tm.tv_sec*1000000+tm.tv_usec;
    for (j=0;j<MAXCYCLES;j++)
    {
	for (i=0;i<MAXELEMS;i++)
	{
	    elem=a.GetElemNr(i);
	    #ifdef PRINTF
		printf("%u=%u\n",i,(int)elem);
	    #endif
	}
    }
    gettimeofday(&tm,NULL);
    printf("parsing all elements with GetElemNr microseconds=%llu\n",tm.tv_sec*1000000+tm.tv_usec-usec);
    gettimeofday(&tm,NULL);
    usec=tm.tv_sec*1000000+tm.tv_usec;
    for (j=0;j<MAXCYCLES;j++)
    {
	a.EnumListInit();
	while(elem=a.GetNextListElem(&i))
	{
	    #ifdef PRINTF
		printf("%u=%u\n",i,(int)elem);
	    #endif
	    a.DelList(i);
	}
    }
    gettimeofday(&tm,NULL);
    printf("parsing all elements with GetNextListElem microseconds=%llu\n",tm.tv_sec*1000000+tm.tv_usec-usec);
}

