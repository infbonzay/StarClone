#ifndef _SC_MYTIME_H

#define _SC_MYTIME_H

//=================================
#define SLOWESTSPEED_TICKS		6
#define SLOWERSPEED_TICKS		9
#define SLOWSPEED_TICKS			12
#define NORMALSPEED_TICKS		15
#define FASTSPEED_TICKS			18
#define FASTERSPEED_TICKS		21
#define FASTESTSPEED_TICKS		24
#define SPEED2X_TICKS			30
#define SPEED4X_TICKS			60
#define SPEED6X_TICKS			90
//=================================

#define MYTIMERFUNC_FIRSTTIMEENTER		0
#define MYTIMERFUNC_EVERYTICKENTER		1
#define MYTIMERFUNC_LASTTIMEENTER		2

#define CYCLESPERSECOND					60

#define MAXGAMECYCLESPERSECOND			NORMALSPEED_TICKS

#define MYTIMER_ASINCHRONMODE			0
#define MYTIMER_SINCHRONMODE			1
typedef long long TIMER_TICK;

struct TICKCOUNTER
{
	TIMER_TICK starttick;
};

struct TIMER
{
private:
	TIMER_TICK	tick_prev;
	TIMER_TICK	tick_delta;
	TIMER_TICK	tick_current;
	int			gametick_current;
	TIMER_TICK	gettimeticks(void);
	void (*MyTimerFunc)(void *,int);
	void *MyTimerParam;
public:

	void SetTickTimerFrequency(int microsec);
	void CallTimer(int mode);
	void SetMyTimerFunc(void (*TFunc)(void *,int),void *TFuncParam);
	void ClearMyTimerFunc(void);
	TIMER_TICK GetCurrentTimerTick(void);
	void ClearGameTimer(void);
	int	 GetTimeParsed(void);
	int	 GetCurrentGameTimeTick(void);
	int	 GetCurrentGameTime(void);
	TICKCOUNTER *CreateTickCounter(void);
	TIMER_TICK GetDeltaCounter(TICKCOUNTER *t);
	void DestroyTickCounter(TICKCOUNTER *t);
	int	 TimeIsCome(int gamespeed);
	void MyNanoSleep(long nanoseconds);

};
//===========================================

#define TIMETONOSLEEP	0
#define TIMETOACT		1
#define TIMETOSLEEP		2

#define TICKS_RES				1000000000L
#define CNTTIMETOSLEEP			TICKS_RES/100
#define INTERVALFPSREFRESH		TICKS_RES


#define TIMETOMOUSESCROLL 6
#define TIMETOSCROLLTITLE (CYCLESPERSECOND/4)

#define TIMETOSCROLLPALETTE 15

#define TIMETOSCROLLPORTRAIT	(CYCLESPERSECOND/10)
#define TIMETOMIXPORTRAIT		(CYCLESPERSECOND/30)
#define TIMETOSCROLLSMKVIDEO1	(CYCLESPERSECOND/20)
#define TIMETOSCROLLSMKVIDEO2	(CYCLESPERSECOND/10)
#define TIMETOFADETEXTVIDEO		(CYCLESPERSECOND/20)
#define TIMETOCHANGESCOREMENU	(CYCLESPERSECOND/20)
#define TIMETORECVDATA			(CYCLESPERSECOND/1)
#define TIMETOSENDDATA			(CYCLESPERSECOND/1)
#define TIMETOMENUUPDATE		(CYCLESPERSECOND/1)
#define TIMETOMAINMENUUPDATE	(CYCLESPERSECOND/10)


void gametimer(void *,int);
void nulltimer(void *,int);
void menutimer(void *,int);
void mainmenutimer(void *,int);
void campaignselecttimer(void *,int);
void scoremenutimer(void *empty,int param);
void menutimerandnetwork(void *,int);

int	 getcursorblinktype(void);
	
extern unsigned char timewaitticks[];
extern TIMER mytimer;
extern TIMER_TICK begintime;
extern int tick_timer;
//=================================
#endif
