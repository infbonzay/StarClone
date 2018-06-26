#include <string.h>
#include <stdlib.h>

#include "wmem.h"

#include "vars.h"
#include "fonts.h"
#include "mouse.h"
#include "mylist.h"
#include "debug.h"
#include "mytime.h"
#include "messages.h"
//==================================
    BARMESSAGES::BARMESSAGES(int maxmes)
    {
	maxmessages=maxmes;
    }
//==================================
    BARMESSAGES::~BARMESSAGES(void)
    {
	clearallmessages();
    }
//==================================
//showedtime in milliseconds
void BARMESSAGES::addbarmessage(char *mes,int fontnr,int color,int showedtime,int flags)
{
    int i,meslen,from,sizexlen,nrofmes,curtick;
    BARMES *bm;
    BARMES *newmes;
    if (!mes)
    {
//	DEBUGMESSCR("mes is NULL\n");
    }
    if (this->maxmessages==1)
	flags|=BF_FORCEADD;
    nrofmes=barmes.GetMaxElements();
    if (nrofmes>=this->maxmessages)
    {
	if (!(flags&BF_FORCEADD))
	    return;
	this->clearonemessage(0);
	this->barmes.Shift();
    }
    curtick=mytimer.GetCurrentGameTimeTick();
    if (flags&BF_ALLOCBUF)
    {
	meslen=strlen(mes)+1;
	sizexlen=0;
        for (i=0,from=0;i<meslen;i++)
        {
    	    if (mes[i]=='\n'||i==meslen-1)
    	    {
		
		BARMES *newmes=(BARMES *)wmalloc(sizeof(BARMES));
    		newmes->mes=(char *)wmalloc(i-from+1);
    		strncpy(newmes->mes,mes+from,i-from);
    		newmes->mes[i-from]=0;
		newmes->fontnr=fontnr;
		newmes->color=color;
	        newmes->flags=flags;
	    	newmes->dissapeartime=curtick+showedtime*MAXGAMECYCLESPERSECOND/1000;
		this->barmes.AddList(newmes);
		nrofmes=barmes.GetMaxElements();
	        if (nrofmes>=this->maxmessages)
	        {
		    this->clearonemessage(0);
		    this->barmes.Shift();
		}
		from=i+1;
    	    }
    	}
    }
    else
    {
	BARMES *newmes=(BARMES *)wmalloc(sizeof(BARMES));
	newmes->fontnr=fontnr;
	newmes->color=color;
        newmes->flags=flags;
    	newmes->dissapeartime=curtick+showedtime*MAXGAMECYCLESPERSECOND/1000;
        newmes->mes=mes;
	this->barmes.AddList(newmes);
    }
}
//==================================
void BARMESSAGES::showallmessages(int x,int y,int sizexbar,int heightoneline)
{
    int i,ypos=0,szx,lines,flags,maxelems;
    BARMES *bm;
    if (!this->barmes.GetMaxElements())
	return;
    barmes.EnumListInit();
    while(bm=(BARMES *)barmes.GetNextListElem(&i))
    {
	if (mytimer.GetCurrentGameTimeTick()>bm->dissapeartime)
	{
	    this->clearonemessage(i);
	    this->barmes.Shift();
	    barmes.EnumListInit();
	}
    }
//    this->barmes.Shift();
    maxelems=this->barmes.GetMaxElements();
    barmes.EnumListInit();
    while(bm=(BARMES *)barmes.GetNextListElem())
    {
	    if (bm->flags&BF_SPLITLINES)
		flags=MESINRECT_COLORS|MES_SPLITTONEXTLINE;
	    else
		flags=MESINRECT_COLORS;
	    if (!bm->mes)
	    {
//		DEBUGMESSCR("mes is NULL\n");
	    }
	    else
	    {
	    getmessagesizex(bm->fontnr,bm->mes,strlen(bm->mes),&szx,NULL);
	    lines=szx/sizexbar+1;
	    if (bm->flags&BF_MALIGN)
	        putmessageinrectangleM(x,y+(this->maxmessages-maxelems+ypos)*heightoneline,
	    	    sizexbar,lines*heightoneline,heightoneline,bm->mes,bm->fontnr,bm->color,tfontgamp,gamedlggrp,flags);
	    else 
	        if (bm->flags&BF_RALIGN)
	    	putmessageinrectangleR(x,y+(this->maxmessages-maxelems+ypos)*heightoneline,
		    sizexbar,lines*heightoneline,heightoneline,bm->mes,bm->fontnr,bm->color,tfontgamp,gamedlggrp,flags);
		else
		    putmessageinrectangleL(x,y+(this->maxmessages-maxelems+ypos)*heightoneline,
		    	sizexbar,lines*heightoneline,heightoneline,bm->mes,bm->fontnr,bm->color,tfontgamp,gamedlggrp,flags,0);
	    ypos+=lines;
	    }
	}
}
//==================================
void BARMESSAGES::clearonemessage(int elemnr)
{
    BARMES *bm;
    bm=(BARMES*) this->barmes.GetElemNr(elemnr);
    if (bm)
    {
	this->barmes.DelList(elemnr);
	if (bm->flags&BF_ALLOCBUF)
	    wfree(bm->mes);
	wfree(bm);
    }
}
//==================================
void BARMESSAGES::clearallmessages(void)
{
    int maxelems=this->barmes.GetMaxElemNr();
    for (int i=0;i<maxelems;i++)
    {
	clearonemessage(i);
    }
}
//==================================




