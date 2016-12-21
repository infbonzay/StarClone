#include <stdio.h>
#include <stdlib.h>

#include "smacker/smacker.h"

#include "vars.h"
#include "man.h"
#include "gr.h"
#include "mpq.h"
#include "objinfo.h"
#include "load.h"
#include "rand.h"
#include "audio.h"
#include "tempobj.h"
#include "audio.h"
#include "gener.h"
#include "putobj.h"
#include "portrait.h"

#define OC_SMK_STAT_IN	(char *)"portrait\\static\\statin.smk"
#define OC_SMK_STAT_OUT	(char *)"portrait\\static\\statout.smk"

char scrollportrait,mixportrait;
PORTRAIT staticport;
MPQIDS	smkin,smkout;
//===================================
void GetINOUTsmk(void)
{
//    smkin.hmpq=GetFilePosInArchive(NULL,OC_SMK_STAT_IN,&smkin.mpqfilenr);
//    smkout.hmpq=GetFilePosInArchive(NULL,OC_SMK_STAT_OUT,&smkout.mpqfilenr);
    GetMPQFileID(NULL,OC_SMK_STAT_IN,&smkin);    
    GetMPQFileID(NULL,OC_SMK_STAT_OUT,&smkout);    
}
//===================================
int loadSMK(SMKPLAY *tempplay,int smkflags)
{
    //if loaded some previous clean
    unloadSMK(tempplay);
    tempplay->smk=smk_open_file(tempplay->smkfile.hmpq,(const char *)tempplay->smkfile.mpqfilenr,SMK_MODE_MEMORY);
    if (tempplay->smk)
    {
	smk_enable_video(tempplay->smk,1);
	smk_first(tempplay->smk,smkflags);
	tempplay->vidbuff=smk_get_video(tempplay->smk);
	return(0);
    }
    return(-1);
}
//===================================
void unloadSMK(SMKPLAY *tempplay)
{
    if (tempplay->smk)
	smk_close(tempplay->smk);
}
//===================================
int unloadstaticsmk(void)
{
    if (staticport.mergepixels)
    {
	wfree(staticport.mergepixels);
    }
    if (staticport.mixbuff)
    {
	wfree(staticport.mixbuff);
    }
    unloadSMK(&staticport.smkplay);
    memset(&staticport,0,sizeof(staticport));
}
//===================================
int loadstaticsmk(void)
{
    MPQIDS tempfile;
    HANDLE hmpq;
    smk_t *tempsmk;
    int mpqfilenr;

    staticport.smkplay.smkfile.hmpq=NULL;
    staticport.smkplay.smkfile.mpqfilenr=-1;
    staticport.smkplay.smkscroll=0;
    staticport.SC_Unit=SC_NOUNITNR;
    staticport.typeofport=SMKATSTART;
    staticport.remainplayedtimes=3;

    staticport.mergepixels=(unsigned char *)wmalloc(SMKSIZEPORTRAITX*SMKSIZEPORTRAITY);
    staticport.mixbuff=(unsigned char *)wmalloc(SMKSIZEPORTRAITX*SMKSIZEPORTRAITY);

//    hmpq=GetFilePosInArchive(NULL,OC_SMK_STAT_OUT,&mpqfilenr);
//    if (!hmpq)
//	return(-1);
//    tempsmk=smk_open_file(hmpq,OC_SMK_STAT_OUT,SMK_MODE_MEMORY);
    tempsmk=smk_open_file(smkout.hmpq,OC_SMK_STAT_OUT,SMK_MODE_MEMORY);
    if (tempsmk)
    {
	smk_enable_video(tempsmk,1);
	smk_first(tempsmk,SMK_FULL_DECODE);
	memcpy(staticport.mixbuff,smk_get_video(tempsmk),SMKSIZEPORTRAITX*SMKSIZEPORTRAITY);
	smk_close(tempsmk);
    }
    else
	return(-2);
    return(0);
}
//===================================
void changeunitportrait(HANDLE hmpq,int smknr,int SC_Unit,int smkscroll)
{
    int newunit;
    if (SC_Unit!=staticport.SC_Unit)
	newunit=1;
    else
	newunit=0;
    staticport.smkplay.smkscroll=smkscroll;
    staticport.nextplaynormal2=0;
    if (newunit)
    {
	staticport.whichuse=PORTRAIT_OUT_NOISE;
	staticport.nextload.hmpq=hmpq;
	staticport.nextload.mpqfilenr=smknr;
	staticport.SC_Unit=SC_Unit;
	staticport.smkplay.fadeout=MINMIXITERATION-1;
	staticport.smkplay.fadein=MAXMIXITERATION;
    }
    else
	if (gameconf.videoconf.portraits==CONF_VIDEO_PORTRAITS_ANIMATED)
	{
	    staticport.whichuse=PORTRAIT_OUT_NOISE;
	    staticport.nextload.hmpq=hmpq;
	    staticport.nextload.mpqfilenr=smknr;
	    staticport.SC_Unit=SC_Unit;
	    staticport.smkplay.fadeout=MAXMIXITERATION;
	    staticport.smkplay.fadein=MINMIXITERATION;
	}
    return;
}
//===================================
unsigned char playbuffer[SMKSIZEPORTRAITX*SMKSIZEPORTRAITY];
//===================================
void showportrait(void)
{
    static int merged=0;
    OBJstruct *b;
    //check if configuration enables portrait
    if (gameconf.videoconf.portraits==CONF_VIDEO_PORTRAITS_DISABLED)
	return;
    if (mixportrait)
    {
	mixportrait=0;
        if (staticport.whichuse==PORTRAIT_OUT_NOISE)
	{
	    if (staticport.smkplay.fadeout++<MAXMIXITERATION)
	    {
		if (gameconf.videoconf.portraits==CONF_VIDEO_PORTRAITS_ANIMATED)
		{
	    	    mergepixels(staticport.smkplay.fadeout,playbuffer,staticport.mixbuff,staticport.smkplay.vidbuff);
	    	    merged=1;
		}
	    }
	    else
	    {
		//load new portrait to show
		staticport.smkplay.smkfile.hmpq=staticport.nextload.hmpq;
		staticport.smkplay.smkfile.mpqfilenr=staticport.nextload.mpqfilenr;
		staticport.nextload.hmpq=NULL;
		staticport.nextload.mpqfilenr=-1;
		staticport.whichuse=PORTRAIT_IN_NOISE;
		loadSMK(&staticport.smkplay,SMK_FULL_DECODE);						//loadnext smk
		staticport.smkplay.fadeout=MINMIXITERATION-1;
	    }
	}
	if (staticport.whichuse==PORTRAIT_IN_NOISE)
	{
	    if (staticport.smkplay.fadein-->MINMIXITERATION)
	    {
		if (gameconf.videoconf.portraits==CONF_VIDEO_PORTRAITS_ANIMATED)
		{
	    	    mergepixels(staticport.smkplay.fadein,playbuffer,staticport.mixbuff,staticport.smkplay.vidbuff);
	    	    merged=1;
		}
	    }
	    else
	    {
		staticport.whichuse=PORTRAIT_NO_NOISE;
		staticport.smkplay.fadein=MAXMIXITERATION;
		merged=0;
	    }
	}
    }
    if (!merged)	
    {
        drawmainportrait(SMKSIZEPORTRAITX,SMKSIZEPORTRAITY,staticport.smkplay.vidbuff);
    }
    else
    {
        drawmainportrait(SMKSIZEPORTRAITX,SMKSIZEPORTRAITY,playbuffer);
    }
    if (scrollportrait)
    {
	scrollportrait=0;
	//check if configuration enables animation portrait
	if (gameconf.videoconf.portraits==CONF_VIDEO_PORTRAITS_STATIC)
	{
	    unsigned int curframe;
	    smk_info_all(staticport.smkplay.smk,&curframe,NULL,NULL);
	    if (curframe!=0)
	    {
	        smk_first(staticport.smkplay.smk,SMK_FULL_DECODE);
		staticport.smkplay.vidbuff=smk_get_video(staticport.smkplay.smk);
	    }
	    return;
	}
	if (staticport.showedtime != -1)
	{
		//timed showing smk - need to decrease time
	    staticport.showedtime -= 1000/10;
	    if (staticport.showedtime <=0 )
	        staticport.showedtime = 0;
	}
	if (smk_next(staticport.smkplay.smk,SMK_FULL_DECODE)==SMK_DONE)
	{
	    //finished. we need to play from begin
	    //we see not SMKNORMAL ,so return to SMKNORMAL ot showedtime
	    if (staticport.typeofport!=SMKNORMAL)
	    {
		if ( staticport.showedtime <= 0 )
		{
		    SetPortrait(staticport.SC_Unit,SMKNORMAL,NOSOUNDFILENR,-1);
		    staticport.nextplaynormal2=0;
		    staticport.remainplayedtimes=3;
		}
		else
		{
//		    staticport.typeofport=SMKNORMAL;//need to show other smktalk
		    SetPortrait(staticport.SC_Unit,SMKTALK,NOSOUNDFILENR,staticport.showedtime);
		}
	    }
	    else
	    {
//		if we have flag to change to smknormal2 do this.
		if (--staticport.remainplayedtimes<=0)
		{
		    if (staticport.nextplaynormal2)
		    {
			SetPortrait(staticport.SC_Unit,SMKNORMAL2,NOSOUNDFILENR,-1);
		    }
		}
	    }
	    smk_first(staticport.smkplay.smk,SMK_FULL_DECODE);
	}
	staticport.smkplay.vidbuff=smk_get_video(staticport.smkplay.smk);
    }//scrollportrait from mytimer
    //if much time SMKNORMAL, we need to change to SMKNORMAL2
    if (staticport.whichuse==PORTRAIT_NO_NOISE)
    {
	if (staticport.smkplay.smkscroll&&staticport.typeofport==SMKNORMAL)
	{
	    if (--staticport.smkplay.smkscroll==1)
	    {
//		put flag to change to normal2
		staticport.nextplaynormal2=1;
	    }
	}
    }
}
//===================================
void drawmainportrait(int w,int h,unsigned char *pixbuff)
{
    putrow(DELTASCREENX+SMKICONPOSX,DELTASCREENY+SMKICONPOSY,w,h,pixbuff);
    showiconramka();
}
//===================================
unsigned char ITERPIXELS[5]={0,50,70,80,100};
//===================================
void mergepixels(int iteration,unsigned char *outbuf,unsigned char *mixbuf,unsigned char *origbuffer)
{
    int i,j,pixel,mixpixel;
    for (i=0;i<SMKSIZEPORTRAITY;i++)
    {
	for (j=0;j<SMKSIZEPORTRAITX;j++)
	{
	    mixpixel=mixbuf[i*SMKSIZEPORTRAITX+j];
	    if (ITERPIXELS[iteration]>=mixpixel)
		outbuf[i*SMKSIZEPORTRAITX+j]=WHITECOLOR;
	    else
		outbuf[i*SMKSIZEPORTRAITX+j]=origbuffer[i*SMKSIZEPORTRAITX+j];
	}
    }
}
//===================================
void SetPortraitShowedTime(int showedtime)
{
    staticport.showedtime=showedtime;
}
//===================================
//SMKNORMAL,SMKNORMAL2,SMKSPECIAL,SMKTALK, the engine automaticaly return to NORMAL after show here selected
//if showedtime == -1 show all smk frames
int SetPortrait(int SC_Unit,int typeofport,int soundfilenr,int showedtime)
{
    HANDLE hmpq;
    int nrof_smks,cursmk,smkscroll,mpqfilenr;
    OBJstruct *b;
    if (typeofport==SMKNONE)
	return -1;
    if (typeofport==SMKRETURNTOADVISOR||SC_Unit==SC_NOUNITNR)
    {
	SetPortraitOBJ(NULL);
	SC_Unit = GetFirstUnitBuild(gameconf.pl_race[NUMBGAMER],FIRSTBUILD);//????? this is pl_race[6] is random_race and SC_Unit == 255 so the GPF
	typeofport=SMKNORMAL;
    }
    b=loadobj(SC_Unit);
    if (b->portraits)
    {
	//if changes unit portrait, we need to put smknormal
	nrof_smks=b->portraits->max[typeofport];
	if (nrof_smks&&SC_Unit!=SC_NOUNITNR)
	{
	    if (staticport.typeofport != SMKTALK)
	    {
		if (typeofport==staticport.typeofport&&SC_Unit==staticport.SC_Unit)
		    return -1;
		cursmk=myrand(nrof_smks);
		staticport.talksmk=cursmk;
	    }
	    else
	    {
		staticport.talksmk++;
		if (staticport.talksmk>=nrof_smks)
		    staticport.talksmk=0;
		cursmk=staticport.talksmk;
	    }
	    staticport.typeofport=typeofport;
	    
	    hmpq=b->portraits->hmpq;
	    mpqfilenr=b->portraits->mpqfilenr[typeofport][cursmk];
	    if (typeofport==SMKNORMAL)
		smkscroll=b->portraits->maxscrollSMKNORMAL;
	    else
		smkscroll=0;
	    changeunitportrait(hmpq,mpqfilenr,SC_Unit,smkscroll);
	    if (!staticport.smkplay.vidbuff)//if not exist first smk, load force
	    {
		staticport.smkplay.smkfile.hmpq=staticport.nextload.hmpq;
		staticport.smkplay.smkfile.mpqfilenr=staticport.nextload.mpqfilenr;
		staticport.nextload.hmpq=NULL;
		staticport.nextload.mpqfilenr=-1;
		staticport.whichuse=PORTRAIT_NO_NOISE;
		loadSMK(&staticport.smkplay,SMK_FULL_DECODE);
		staticport.smkplay.smkscroll=b->portraits->maxscrollSMKNORMAL;
	    }
	    SetPortraitShowedTime(showedtime);
	    return(cursmk);
	}
    }
    return(-1);
}
//===================================
//		   atack      death  err       work   workdone  
//		   hit       pss    mage5     mage6   mage7   
//		     mage8 ready    select    action  modein 
//		 modeout   bconstr  construct  warp   move     deconstruct
char soundtypetosmktype[MAXTYPESOFSOUND]={
			    SMKNONE,SMKRETURNTOADVISOR,SMKTALK,SMKNONE,SMKTALK,
			    SMKNONE,SMKTALK,SMKNONE,SMKNONE,SMKNONE,
			    SMKNONE,SMKNORMAL,SMKTALK,SMKTALK,SMKNONE,
			    SMKNONE,SMKNORMAL,SMKNORMAL,SMKNORMAL,SMKNONE,SMKNORMAL};
//===================================
int SetPortraitFromSound(int SC_Unit,int soundmode,int soundfilenr)
{
    if (soundmode>=0&&soundmode<MAXTYPESOFSOUND)
    {
	SetPortrait(SC_Unit,soundtypetosmktype[soundmode],soundfilenr,-1);
    }
}
//===================================
void ShowAdvisorPortrait(void)
{
    SetPortrait(SC_NOUNITNR,SMKRETURNTOADVISOR,NOSOUNDFILENR,-1);
}
//===================================
//return - remained microsecond before end play current smk
//===================================
int GetTimeBeforeEndPortrait(void)
{
    smk_t *tempsmk=staticport.smkplay.smk;
    unsigned int curframe,allframes;
    double usf;
    smk_info_all(staticport.smkplay.smk,&curframe,&allframes,&usf);
    return(1000*(allframes - curframe)/(int)usf);
}
//===================================
void SetPortraitOBJ(OBJ *a)
{
    staticport.obj=a;
}
//===================================
OBJ *GetPortraitOBJ(void)
{
    return(staticport.obj);
}
//===================================
