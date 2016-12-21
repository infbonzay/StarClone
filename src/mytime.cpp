#include <stdlib.h>
#include <stdio.h>
#include <grplib/gr8.h>

#include "vars.h"
#include "maps.h"
#include "scrolltile.h"
#include "mouse.h"
#include "gener.h"
#include "fonts.h"
#include "portrait.h"
#include "gamemenu.h"
#include "mytime.h"

TIMER_TICK begintime;
TIMER mytimer;
int cursorblink,tick_timer;
//===========================================
long timewaitvalue[10]=
{
		    TICKS_RES/SLOWESTSPEED_TICKS,
		    TICKS_RES/SLOWERSPEED_TICKS,
		    TICKS_RES/SLOWSPEED_TICKS,
		    TICKS_RES/NORMALSPEED_TICKS,
		    TICKS_RES/FASTSPEED_TICKS,
		    TICKS_RES/FASTERSPEED_TICKS,
		    TICKS_RES/FASTESTSPEED_TICKS,
		    TICKS_RES/SPEED2X_TICKS,
		    TICKS_RES/SPEED3X_TICKS,
		    TICKS_RES/SPEED4X_TICKS
};
//====================================================================
unsigned char timewaitticks[10]=
{
		    SLOWESTSPEED_TICKS,
		    SLOWERSPEED_TICKS,
		    SLOWSPEED_TICKS,
		    NORMALSPEED_TICKS,
		    FASTSPEED_TICKS,
		    FASTERSPEED_TICKS,
		    FASTESTSPEED_TICKS,
		    SPEED2X_TICKS,
		    SPEED3X_TICKS,
		    SPEED4X_TICKS
};
//====================================================================
#ifdef UNDERLINUX
    #include <sys/time.h>
    TIMER_TICK TIMER::gettimeticks(void)
    {
	struct timespec ticks;
        clock_gettime(CLOCK_MONOTONIC,&ticks);
        return((TIMER_TICK) (ticks.tv_sec) * TICKS_RES + ticks.tv_nsec);
    }
#endif
/*#ifdef UNDERDOS
    #include "mouse.h"
    long long GetCPUCurentTick(void)
    {
	int _edx,_eax;
	_asm
	{
	    db 0fh
	    db 31h
	    mov _edx,edx
	    mov _eax,eax
	}
	return(_edx<<32|_eax);
    }
    void getmsec(long *sec,long *microsec)	//get microseconds
    {
	static long long firsttick=0;
	static float MHz=0;
	if (!firsttick)
	{
	    firsttick = GetCPUCurentTick();
	    temp_timer=tick_timer+18;
	    do{
	    }while(tick_timer<=temp_timer);
	    MHz = (float)(GetCPUCurentTick() - firsttick);
	}
	else
	    MHz = (float)(GetCPUCurentTick() - firsttick)*18.2/tick_timer;
	microsec = (GetCPUCurentTick()-firsttick)/MHz;
    }
#endif
*/
//===========================================
void TIMER::MyNanoSleep(long nanoseconds)
{
    long prev = gettimeticks();
    do{ 
    }while(gettimeticks()-prev<nanoseconds);
}
//===========================================
void TIMER::ClearGameTimer(void)
{
    gametick_current=0;
}
//===========================================
int TIMER::GetCurrentGameTimeTick(void)
{
    return(gametick_current);
}
//===========================================
int TIMER::GetCurrentGameTime(void)
{
    return(gametick_current/MAXGAMECYCLESPERSECOND);
}
//===========================================
TIMER_TICK TIMER::GetCurrentTimerTick(void)
{
    return(tick_current);
}
//===========================================
void TIMER::SetTickTimerFrequency(int microsec)
{
    tick_prev=gettimeticks();
    tick_current=tick_prev;
    tick_delta=TICKS_RES/microsec;
}
//===========================================
//return value in seconds
int TIMER::GetTimeParced(void)
{
    return((tick_current-begintime)/TICKS_RES);
}
//===========================================
void TIMER::CallTimer(int mode)
{
    static char income=0;
    tick_current = gettimeticks();
    if (tick_prev+tick_delta<=tick_current)
    {
	if (mode==MYTIMER_ASINCHRONMODE)
	{
	    tick_prev+=tick_delta;
	    tick_timer+=(tick_current-tick_prev)/tick_delta;
	}
	else
	{
	    tick_prev=tick_current;
	    tick_timer++;
	}
	if (MyTimerFunc)
	    (*MyTimerFunc)(MyTimerParam,MYTIMERFUNC_EVERYTICKENTER);//launch everytime
    }
}
//===========================================
void TIMER::SetMyTimerFunc(void (*TFunc)(void *,int),void *TFuncParam)
{
    (*TFunc)(TFuncParam,MYTIMERFUNC_FIRSTTIMEENTER);//launch first time
    MyTimerFunc=TFunc;
    MyTimerParam=TFuncParam;
}
//===========================================
void TIMER::ClearMyTimerFunc(void)
{
    if (MyTimerFunc)
    {
	(*MyTimerFunc)(MyTimerParam,MYTIMERFUNC_LASTTIMEENTER);//launch last time
	MyTimerFunc=NULL;
	MyTimerParam=NULL;
    }
}
//===========================================
TICKCOUNTER * TIMER::CreateTickCounter(void)
{
    TICKCOUNTER *t=(TICKCOUNTER *)wmalloc(sizeof (TICKCOUNTER));
    t->starttick=tick_current;
    return(t);
}
//====================================================================
TIMER_TICK TIMER::GetDeltaCounter(TICKCOUNTER *t)
{
    TIMER_TICK delta=tick_current-t->starttick;
    t->starttick=tick_current;
    return(delta);
}
//====================================================================
void TIMER::DestroyTickCounter(TICKCOUNTER *t)
{
    if (t)
	wfree(t);
}
//====================================================================
//starcraft time slowest,slower,slow,normal,fast,faster,fastest
//		  1/6s    1/9s  1/12s 1/15s 1/18s 1/21s  1/24s  
//====================================================================
//function to synchronise everything in time on slow and fast machines
//		       0,50,40,30,20,10
int TIMER::TimeIsCome(int gamespeed)
{
    static TIMER_TICK prevmsec=0;
    long retvalue;
    TIMER_TICK microsec=GetCurrentTimerTick();
    if (microsec-prevmsec<timewaitvalue[gamespeed]-CNTTIMETOSLEEP)
//        retvalue=TIMETOSLEEP;
        retvalue=TIMETONOSLEEP;
    else
	if (microsec-prevmsec<timewaitvalue[gamespeed])
	    retvalue=TIMETONOSLEEP;
	else
	{
	    retvalue=TIMETOACT;
	    prevmsec=microsec;
	    gametick_current++;
	}
    return(retvalue);
}
//====================================================================
#define CURSORBLINKTIME 60
//=================================
void mousescrolling(void)
{
    if (!(tick_timer%TIMETOMOUSESCROLL))
	nextmousespos();
}
//=================================
void cursorblinkfunc(void)
{
    int tempcursorblink;
    tempcursorblink=tick_timer%CURSORBLINKTIME;
    if (tempcursorblink<CURSORBLINKTIME/2)
	cursorblink=0;
    else
	cursorblink=1;
}
//=================================
void scrollportraitfunc(int scrollticks,int mixticks)
{
    if (!(tick_timer%scrollticks))
	scrollportrait=1;
    if (!(tick_timer%mixticks))
	mixportrait=1;
}
//=================================
void fadeinouttextvideo(int scrollticks)
{
    if (!(tick_timer%scrollticks))
	fadeinout=1;
}
//=================================
void gametimer(void *empty,int param)
{
    if (param!=MYTIMERFUNC_EVERYTICKENTER)
	return;
    mousescrolling();
    if (GAME)
    {
	if (!PAUSEGAME)
	{
	    if (gameconf.videoconf.animation)
		if ((!(tick_timer%TIMETOSCROLLPALETTE)))
		{
		    scrollpal(&map);
		}
	}
    }
    scrollportraitfunc(TIMETOSCROLLPORTRAIT,TIMETOMIXPORTRAIT);
    cursorblinkfunc();
    if ((!(tick_timer%TIMETOMENUUPDATE)))
	menutimerupdate=1;
    networkgametick=1;
}
//=================================
void mainmenutimer(void *empty,int param)
{
    if (param!=MYTIMERFUNC_EVERYTICKENTER)
	return;
    mousescrolling();
    scrollportraitfunc(TIMETOSCROLLSMKVIDEO1,TIMETOMIXPORTRAIT);
    fadeinouttextvideo(TIMETOFADETEXTVIDEO);
    if ((!(tick_timer%TIMETOMAINMENUUPDATE)))
	menutimerupdate=1;
}
//=================================
void campaignselecttimer(void *empty,int param)
{
    if (param!=MYTIMERFUNC_EVERYTICKENTER)
	return;
    mousescrolling();
    scrollportraitfunc(TIMETOSCROLLSMKVIDEO2,TIMETOMIXPORTRAIT);
    fadeinouttextvideo(TIMETOFADETEXTVIDEO);
}
//=================================
int getcursorblinktype(void)
{
    return(cursorblink);
}
//=================================
void menutimer(void *empty,int param)
{
    if (param!=MYTIMERFUNC_EVERYTICKENTER)
	return;
    mousescrolling();
    cursorblinkfunc();
    scrollportraitfunc(TIMETOSCROLLSMKVIDEO2,TIMETOMIXPORTRAIT);
    fadeinouttextvideo(TIMETOFADETEXTVIDEO);
    if ((!(tick_timer%TIMETOMAINMENUUPDATE)))
	menutimerupdate=1;
}
//=================================
void scoremenutimer(void *empty,int param)
{
    if (param!=MYTIMERFUNC_EVERYTICKENTER)
	return;
    menutimer(empty,param);
    if ((!(tick_timer%TIMETOCHANGESCOREMENU)))
	updatescoremenu();
}
//=================================
void menutimerandnetwork(void *empty,int param)
{
    if (param!=MYTIMERFUNC_EVERYTICKENTER)
	return;
    menutimer(empty,param);
    if ((!(tick_timer%TIMETOSENDDATA)))
	networkresend=1;
    if ((!(tick_timer%TIMETORECVDATA)))
	networkreceive=1;
}
//=================================
