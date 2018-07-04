
#include <math.h>

#include "wmem.h"
#include "mpqwrapper.h"

#include "auxil.h"
#include "debug.h"
#include "man.h"
#include "defs.h"
#include "mpq.h"
#include "load.h"
#include "vars.h"
#include "mouse.h"
#include "rand.h"
#include "objinfo.h"
#include "maps.h"
#include "mage.h"
#include "portrait.h"
#include "putobj.h"
#include "lists.h"
#include "mylist.h"

#include "images.h"
#include "audio.h"
#include "music.h"

#define ADVISORSONGS ((struct OBJ *)NULL)
#define MAXSELECTPLAY	5

int MayPlaySounds;
AUDIOUNIT *unitsound;
int maxunitsounds,ReducedVolume;
//==============================================
void ReduceVolume(int boolflag)
{
    ReducedVolume = boolflag;
}
//==============================================
int GetReduceVolume(void)
{
    return( ReducedVolume );
}
//==============================================
int SetChannelVolume(int channel, int volume)
{
    if (ReducedVolume)
        return( wChannelVolume(channel,volume/3) );
    else
        return( wChannelVolume(channel,volume/1) );
}
//==============================================
#define MINDIST 10
#define MAXDIST 128
int CalculateVolume(int distance,int minvolume,int maxvolume,int globalvol)
{
    int retvalue;
    distance = abs(distance-(GRP_screensizex+GRP_screensizey)/2)/8;
    if (distance<MINDIST)
	retvalue = maxvolume;
    else
	if (distance>MAXDIST)
	    retvalue = minvolume;
	else
	    retvalue = (int)(((MAXDIST-(float)distance)/maxvolume)*(maxvolume-minvolume)+minvolume);
    return retvalue*globalvol/100;
}
//==============================================
void TerminateAudioObj(struct OBJ *a)
{
    if (a)
    {
	a->sfxplay.nrofmusicsplayed--;
	if (a->sfxplay.nextmusicplay)
	{
	    activatesound(a,a->sfxplay.nextmusicplay-1,0,NOSTOPCURRENTSOUNDS);
	    a->sfxplay.nextmusicplay=0;
	}
    }
}
//==============================================
void StopPlayChannel(int channel)
{
    unitsound[channel].presence &= ~SOUNDPLAYINFINITE;//prevent play infinitely, just stop it
    wStopChannel(channel);
}
//==============================================
void ChannelFinished(int channel)
{
    if (audiostream)
    {
	if (channel == audiostream->GetChunkPlay())
	{
	    audiostream->onfinishaudiodatabuffer(channel);
	    return;
	}
    }
    unloadwav(channel);
}
//==============================================
int initsoundengine(int maxsounds)
{
    if (!unitsound)
    {
	if (wOpenAudio()<0)
	{
	    return -1;
	}
	unitsound = (AUDIOUNIT *)wmalloc(sizeof(AUDIOUNIT)*maxsounds);
	memset(unitsound,0,sizeof(AUDIOUNIT)*maxsounds);
	wSetChannels(maxsounds);
	wOnChannelFinish(ChannelFinished);
	maxunitsounds = maxsounds;
	UnBlockSoundToPlay();
	ReducedVolume = 0;
    }
    return 0;
}
//==============================================
void deinitsoundengine(void)
{
    if (unitsound)
    {
	wCloseAudio();
	wfree(unitsound);
	unitsound = NULL;
	MayPlaySounds = 0;
	ReducedVolume = 0;
    }
}
//==============================================
char SndArraySTR[50]="sound\\";
#define OFFSETFORSFXFILE	6
//=======================================
int SFX_RESCUE[3]={SFXDATA_ZRESCUE,SFXDATA_TRESCUE,SFXDATA_PRESCUE};
//=======================================
int play_race_rescue(int race,int repeatflag,int distance)
{
    return(Play_sfxdata_id(NULL,SFX_RESCUE[race],4,0));
}
//=======================================
int Play_sfxdata_id(OBJ *a,int sfxdata_id,int repeatflag,int distance)
{
    short sfxdata_tbl;
    if (sfxdata_id>0)
    {
	sfxdata_tbl=alldattbl.sfxdata_dat->sfxdata_tbl[sfxdata_id]-1;
	strcpy(SndArraySTR+OFFSETFORSFXFILE,alldattbl.sfxdata_tbl->get_TBL_STR(sfxdata_tbl));
	return(loadandplaywav(NULL,a,SndArraySTR,repeatflag,distance));
    }
    return(-1);
}
//=======================================
int Play_sfxdata(int x,int y,int sfxdata_id,int repeatedsongs)
{
    if (CHECKFORMAPBORDERS(x/32,y/32))
	return(-1);
    if (mapSEE(x/32,y/32) > 1)
	return( Play_sfxdata_id(NULL,sfxdata_id,repeatedsongs,DistanceForAudio(x,y)));
    return(-1);
}
//==============================================
int loadandplaywav(HANDLE mpq,struct OBJ *a,char *filesound,int repeatedflag,int distance)
{
    int repeated=0,repeated2=0,i=-1,err,chunkvolume;
    long soundsz,sndlength;
    MPQIDS fileaudioID;
    HANDLE f;
    wCHUNK *sample;
    if (MayPlaySounds <=0 && a)
	return(-1);
    if (unitsound)
    {
	if (GetMPQFileID(mpq,filesound,&fileaudioID)==-1)
    	    return -1;
//    	DEBUGMESSCR("playing sound file name = %s\n",filesound);
        for (i=0;i<maxunitsounds;i++)
	{
    	    if (checkifplay(i))
    	    {
    		if (CmpMPQFileID(&unitsound[i].fileID,&fileaudioID))
    		    repeated++;
    		if (unitsound[i].obj)
		    if (unitsound[i].obj==a)
    			repeated2++;
    	    }
        }
	if (repeatedflag>=0)
	    if (repeated>=repeatedflag||repeated2>=1)
    		return -2;				//max of this type of sound exist
	//check if have empty sound channels
        for (i=0;i<maxunitsounds;i++)
    	    if (!checkifplay(i))
    		break;
        if (i>=maxunitsounds)
    	    return -3;				//all channels full
	if (!mpq)
	    mpq = fileaudioID.hmpq;

	chunkvolume = CalculateVolume(distance,0,MIX_MAX_VOLUME,unitsound[i].volume);
	if (!chunkvolume)
	    return(-3);
	sndlength = FileWavePlayLength(mpq,filesound);
	sample = PlayMpqWav(My_RWFromMpqFile(mpq,filesound,SFILE_OPEN_FROM_MPQ), 1);

        if (!sample)
    	    return(-4);
	i = wPlayChannel(-1,sample,0);
        if (i == -1)
        {
    	    return -5;
        }
        SetChannelVolume(i,chunkvolume);
	unitsound[i].length = sndlength;
	unitsound[i].sample = sample;
	
        if (a)
	    a->sfxplay.nrofmusicsplayed++;
        unitsound[i].presence = 0;
        unitsound[i].presence |= SOUNDEXIST;
        unitsound[i].distance = distance;
        unitsound[i].prevdistance = distance;
        unitsound[i].obj = a;
        SetMPQFileID(&unitsound[i].fileID,&fileaudioID);
        if (repeatedflag==-2)
            unitsound[i].presence |= SOUNDPLAYINFINITE;
    }
    return i;
}
//==============================================
int checkifplay(int channel)
{
    return(unitsound[channel].presence&SOUNDEXIST);
}
//==============================================
int unloadwav(int channel)
{
    AUDIOPACKET *apacket;
    int i,elemnr;
    if (unitsound)
    {
	i = channel;
	if (unitsound[i].presence&SOUNDPLAYINFINITE)
	{
	    wPlayChannel(i,unitsound[i].sample,0);
	    return 0;
	}
        wFreeChunk(unitsound[i].sample);
        //get next sample
	unitsound[i].presence &= ~SOUNDEXIST;
    	unitsound[i].distance = 0;
    	unitsound[i].prevdistance = 0;
    	ClearMPQFileID(&unitsound[i].fileID);
    	unitsound[i].sample=NULL;
	if (!(unitsound[i].presence & OBJADREMPTY))
	    TerminateAudioObj(unitsound[i].obj);
    	unitsound[i].obj = NULL;
    }
    return 0;
}
//==============================================
void volumeallsfx(int volume)//volume in %
{
    int i;
    if (unitsound)
    {
	for (i=0;i<maxunitsounds;i++)
    	    unitsound[i].volume = MIX_MAX_VOLUME*volume/100;
    }
}
//==============================================
int DistanceForAudio(int x,int y)
{
    int distance = (int) hypot(x-map.MAPXGLOBAL,y-map.MAPYGLOBAL);
    return(distance);
}
//==============================================
int playOBJsound(struct OBJ *a,int soundmode,int sfxdata_id,int stopprevsound,int repeatedsongs)
{
    int soundid,distance;
    char *soundfile;
    if (IsDoodadState(a->SC_Unit))
    {
	if (GetOBJx(a) >= map.MAPXGLOBAL                        &&
	    GetOBJy(a) >= map.MAPYGLOBAL                        &&
	    GetOBJx(a) < map.MAPXGLOBAL+widthkart*SIZESPRLANSHX &&
	    GetOBJy(a) < map.MAPYGLOBAL+hightkart*SIZESPRLANSHY)
	    ;
	else
	    return -1;
    }
    distance = DistanceForAudio(GetOBJx(a),GetOBJy(a));
    if (stopprevsound)
        stopcurrentsound(a);
    return( Play_sfxdata_id(a,sfxdata_id,repeatedsongs,distance));
}
//==============================================
//addfactor=0	3 sounds of one type
//addfactor=1	1 sound of one type
//addfactor=2	infinite sounds of one type
//addfactor=3   2 sound of one type
//==============================================
//return soundtype need to be played
int activatesound(struct OBJ *a,int soundmode,int addfactor,int stopprevsound)
{
    short sndfrom,sndto;
    int repeatedsongs=3,needsong=1,distance,musnr,maxmusnr,musplay,needadd=0;
    int err,smktype,aliance;
    if (a->prop & VARNOTWORK)
	return -1;
    if (a)
	if (mapSEE(a->xkart,a->ykart) <= 1)
	    return -1;
    getsoundIDfromunitdat(a->SC_Unit,soundmode,&sndfrom,&sndto);

    maxmusnr = sndto - sndfrom+1;
    if (!sndfrom)
    {
	if (soundmode == MODESOUNDSELECT || player_aliance(NUMBGAMER,a->playernr)!=ENEMYOBJ)
	{
	    SetPortrait(a->SC_Unit,SMKNORMAL,NOSOUNDFILENR,-1);
	    SetPortraitOBJ(a);
	}
	return -1;
    }
    aliance=player_aliance(NUMBGAMER,a->playernr);
    if (aliance==ENEMYOBJ||aliance==NEUTRALOBJ||aliance==ALLIANCEOBJ)
    {
	needsong=canplaysound(a,soundmode,aliance);
	if (soundmode==MODESOUNDSELECT)
	{
	    SetPortrait(a->SC_Unit,SMKNORMAL,NOSOUNDFILENR,-1);
	    SetPortraitOBJ(a);
	}
	else
	{
	    SetPortraitFromSound(a->SC_Unit,soundmode,NOSOUNDFILENR);
	    SetPortraitOBJ(a);
	}
    }
    if (!needsong)
	return -1;
    switch(addfactor)
    {
	case 1:
	    if (!a->sfxplay.nextmusicplay&&a->sfxplay.nrofmusicsplayed)
	    {
		//need to prevent to play same sound :)
		if (soundmode == a->sfxplay.sfxtypeplayed)
		    return -1;
		a->sfxplay.nextmusicplay = soundmode+1;
		return -1;
	    }
	    break;
	case 2:
	    repeatedsongs = -1;
	    break;
	case 3:
	    repeatedsongs = 1;
	    break;
	default:
	    break;
    }
    distance = (int) hypot(GetOBJx(a)-map.MAPXGLOBAL,GetOBJy(a)-map.MAPYGLOBAL);
    if (a->sfxplay.sfxtypeplayed != soundmode && a->sfxplay.sfxtypeplayed != MODESOUNDPSS)
    {
        a->sfxplay.nrselectplayed = 0;
    }
    else
        needadd=1;
    if (soundmode == MODESOUNDSELECT)
    {
        if (a->sfxplay.sfxtypeplayed == MODESOUNDPSS)
        {
	    soundmode = MODESOUNDPSS;
	}
	else
	    if (player_aliance(NUMBGAMER,a->playernr) == MYOBJ &&
	        a->sfxplay.nrselectplayed >= MAXSELECTPLAY     && 
	        a->SC_Unit < MAX_UNITS_ELEM2 && alldattbl.units_dat->PissSoundStart[a->SC_Unit] > 0 )
	    {
		    soundmode = MODESOUNDPSS;
		    a->sfxplay.nrselectplayed = 0;
	    }
    }
    getsoundIDfromunitdat(a->SC_Unit,soundmode,&sndfrom,&sndto);
    maxmusnr = sndto - sndfrom+1;
    if (!sndfrom)
        return -1;
    if (soundmode == MODESOUNDPSS)
    {
		musnr = a->sfxplay.nrselectplayed;
		if (musnr >= maxmusnr)
		{
	 	    musnr = 0;
		    soundmode = MODESOUNDSELECT;
		    getsoundIDfromunitdat(a->SC_Unit,soundmode,&sndfrom,&sndto);
		    maxmusnr = sndto - sndfrom+1;
		    a->sfxplay.nrselectplayed = 0;
		}
    }
    else
    {
	    musnr = myrand(maxmusnr);
		//this if need for no repeat this song with previous song
	    if (maxmusnr > 1 && a->sfxplay.sfxtypeplayed == soundmode &&  musnr == a->sfxplay.nrsfxplayed)
	    {
	        musnr++;
	        if (musnr>=maxmusnr)
	    	    musnr=0;
	    }
    }
    musplay = sndfrom+musnr;
    if (IsBuild(a->SC_Unit))
    {
    		if (!gameconf.audioconf.buildsounds)
		    return(soundmode);
    }
    else
    {
		if (!gameconf.audioconf.unitspeach)
		    return(soundmode);
    }
    err = playOBJsound(a,soundmode,musplay,stopprevsound,repeatedsongs);
    if (err>=0)
    {
	    a->sfxplay.nrsfxplayed = musnr;
	    if (needadd)
	        a->sfxplay.nrselectplayed++;
    }
    a->sfxplay.sfxtypeplayed = soundmode;
    SetPortraitFromSound(a->SC_Unit,soundmode,musnr);
    SetPortraitOBJ(a);
    if (soundmode == MODESOUNDPSS)
        if (err>=0)
        {
            //show as we hear sound
            SetPortraitShowedTime(getchannelplaylength(err));
        }
    return(soundmode);
}
//==============================================
void getsoundIDfromunitdat(SCUNIT SC_Unit,int soundmode,short *from,short *to)
{
    switch(soundmode)
    {
	case MODESOUNDREADY:
	    if (SC_Unit < MAX_UNITS_ELEM2)
	    {
		*from = alldattbl.units_dat->ReadySound[SC_Unit];
		*to = alldattbl.units_dat->ReadySound[SC_Unit];
		return;
	    }
	    break;
	case MODESOUNDACTION:
	    if (SC_Unit < MAX_UNITS_ELEM2)
	    {
		*from = alldattbl.units_dat->YesSoundStart[SC_Unit];
		*to = alldattbl.units_dat->YesSoundEnd[SC_Unit];
		return;
	    }
	    break;
	case MODESOUNDSELECT:
	    *from = alldattbl.units_dat->WhatSoundStart[SC_Unit];
	    *to = alldattbl.units_dat->WhatSoundEnd[SC_Unit];
	    return;
	    break;
	case MODESOUNDPSS:
	    if (SC_Unit < MAX_UNITS_ELEM2)
	    {
		*from = alldattbl.units_dat->PissSoundStart[SC_Unit];
		*to = alldattbl.units_dat->PissSoundEnd[SC_Unit];
		return;
	    }
	    break;
    }
    *from=0;
    *to=0;
}
//==============================================
void stopcurrentsound(struct OBJ *a)
{
    if (unitsound)
    {
	a->sfxplay.nextmusicplay=0;
	for (int i=0;i<maxunitsounds;i++)
    	    if (unitsound[i].obj==a)
    	    {
        	StopPlayChannel(i);//this function call finishchannel
    	    }
    }
}
//==============================================
void stopallsounds(void)
{
    if (unitsound)
    {
	for (int i=0;i<maxunitsounds;i++)
    	    if (checkifplay(i))
    	    {
        	unitsound[i].presence |= STOPSOUND;
        	StopPlayChannel(i);
    	    }
    }
    ReducedVolume = 0;
}

//==============================================
int GetSoundsPlay(void)
{
    int i,ret=0;
    if (unitsound)
    {
	for (i=0;i<maxunitsounds;i++)
    	    if (checkifplay(i))
        	ret++;
    }
    return ret;
}
//==============================================
int BlockSoundToPlay(void)
{
    int temp = MayPlaySounds;
    MayPlaySounds=0;
    return(temp);
}
//==============================================
int UnBlockSoundToPlay(void)
{
    int temp = MayPlaySounds;
    MayPlaySounds=1;
    return(temp);
}
//==============================================
void RestoreSoundToPlay(int sndblktype)
{
    MayPlaySounds = sndblktype;
}
//==============================================
void ChangeAllVolume(void)
{
    int distance,i,volume;
    if (unitsound)
    {
	for (i=0;i<maxunitsounds;i++)
    	    if (checkifplay(i))
	    {
		distance = (int) hypot(GetOBJx(unitsound[i].obj)-map.MAPXGLOBAL,
			               GetOBJy(unitsound[i].obj)-map.MAPYGLOBAL);
		volume = CalculateVolume(distance,0,MIX_MAX_VOLUME,unitsound[i].volume);
		SetChannelVolume(i,volume);
	    }
    }
}
//==============================================
int maxadvisorsounds[3]={9,15,9};
short sfx_advisorsounds[3][15]={
				{147,150,153,  0,156,117,120,123,127},
				{148,151,154,145,142,0,157,118,121,124,126,128,130,132,134},
				{149,152,155,158,119,122,125,129,133}
			       };
//==============================================
/*
ADVMINERAL
ADVGAS
ADVUNITS
ADVENERGY
ADVBASEATACK
ADVUNITSATACK
ADVRESEARCH1
ADVRESEARCH2
ADVNUCLEARDETECT
ADVLANDINGERR1
ADVLANDINGERR2
ADVADDONCOMPL
ADVAUXSTRRUCT
ADVNUCLREADY
ADVGEYSERNOTREADY
*/
signed char generaladvisorsongs[3][14]=
{
    { 0, 1, 2, 4, 5, 6, 7, 7, 8,-1,-1,-1,-1,-1},
    { 0, 1, 2, 6, 7, 8, 9,13,11, 3, 4,10,12,14},
    { 0, 1, 2, 3, 4, 5, 6, 8, 7,-1,-1,-1,-1,-1}
};
//==============================================
short generaladvisortext[3][15]=
{
    { 849, 850, 843, 863,  -1,  -1,  -1,  -1,1046,  -1,  -1,  -1,  -1,  -1, 892},
    { 849, 850, 844, 864,  -1,  -1,  -1,  -1,1046, 862, 862,  -1,  -1,  -1, 893},
    { 849, 850, 845, 865,  -1,  -1,  -1,  -1,1046,  -1,  -1,  -1,  -1,  -1, 894}
};
//==============================================
void playinfoadvisorsound(int playernr,int race,int typeofsong,int textsound)
{
    if (textsound==PLAYADVISOR_TEXTANDSOUND)
	playadvisorsound(playernr,race,typeofsong,generaladvisorsongs[race][typeofsong]);
    showadvisortext(generaladvisortext[race][typeofsong]);
}
//==============================================
void playadvisorsound(int playernr,int race,int typeofsong,int song)
{
    char filename[200];
//    if (race==TERRANADVRACE)
//	race=TERRANRACE;
    if (typeofsong == ADVNUCLEARDETECT || player_aliance(NUMBGAMER,playernr)==MYOBJ)
    {
	if (gameconf.audioconf.advisorsounds)
	{
	    Play_sfxdata_id(ADVISORSONGS,sfx_advisorsounds[race][song],3,0);
	}
    }
}
//====================================================
//if we need to kill object need emptying internal sound unit information
void FreeChannelWithObj(struct OBJ *a)
{
    int i;
    //search for coincidence
    if (unitsound)
    {
	for (i=0;i<maxunitsounds;i++)
    	    if (checkifplay(i))
    		if (unitsound[i].obj==a)
		    unitsound[i].presence |= OBJADREMPTY;
    }
}
//===================================
//		  atack      death   err     work     workdone  
//		   hit       pss    mage5    mage6   mage7   
//		  mage8      ready  select    action  modein 
//		 modeout   bconstr  construct  warp    move  deconstruct
/*char playenemysoundobj[MAXTYPESOFSOUND]={
					    1,1,0,1,0,
					    1,0,1,1,1,
					    1,0,0,0,1,
					    1,1,1,1,1,1
					};
*/
//ready,action,select,pss,error
char playenemysoundobj[MAXTYPESOFSOUND]={
					0,0,0,0,0    
					};
//====================================================
int canplaysound(OBJ *a,int soundmode,int statuspl)
{
    int isbuildobj;
    if (statuspl==ENEMYOBJ)
    {
	return(playenemysoundobj[soundmode]);
    }
    else
	if (statuspl==NEUTRALOBJ||statuspl==ALLIANCEOBJ)
	{
	    if (playenemysoundobj[soundmode])
		return(1);
	    else
		if (IsGroupNeutralFlag(a->SC_Unit))
		    return(1);
		else
		    return(0);
	}
    return(1);
}
//====================================================
int getchannelplaylength(int channel)
{
    if (channel != -1)
	return(unitsound[channel].length);
    else
	return(0);
}
//====================================================
int GetTimeWaveLength(WAVHEADER *wavheader)
{
    return(1000 * wavheader->subchunk2Size / (wavheader->bitsPerSample / 8) / wavheader->numChannels / wavheader->sampleRate);
}
//====================================================
int FileWavePlayLength(HANDLE mpq,char *filesound)
{
	HANDLE f;
	int result;
	WAVHEADER *wavheader = (WAVHEADER *)wmalloc( sizeof(WAVHEADER) );
	result = wr_SFileOpenFileEx(mpq,filesound,SFILE_OPEN_FROM_MPQ,&f);
	if( !result )
	    return -1;
	wr_SFileReadFile( f,wavheader,sizeof(WAVHEADER),NULL,NULL );
	result = GetTimeWaveLength(wavheader );
	wr_SFileCloseFile(f);
	wfree(wavheader);
	return(result);
}
//==========================================
void WaitUntilAllAudioStops(int maxwait)
{
    int i,plays=0;
    if (unitsound)
    {
	do{
	    for (i=0;i<maxunitsounds;i++)
	    {
    		if (checkifplay(i))
    		{
    		    plays++;
    		}
	    }
	    usleep(10000);
	    maxwait-=10000;
	}while(plays && maxwait>0);
    }
}

