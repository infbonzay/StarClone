
#if !defined(_AUDIO_W)
#define _AUDIO_W

#include "defs.h"
#include "mpq.h"
#include "man.h"
#include "audiolowlevel.h"


#define WAV_RIFF		0x46464952		/* "RIFF" */
#define WAV_WAVE		0x45564157		/* "WAVE" */
#define WAV_FMT			0x20746D66		/* "fmt " */
#define WAV_DATA		0x61746164		/* "data" */
#define WAV_PCM_CODE		0x0001

#define WAV_MONO	1
#define WAV_STEREO	2

#pragma pack(push,1)

struct WAVHEADER
{
    unsigned int  chunkId;		//"RIFF"
    unsigned int  chunkSize;		//size from next to end of file
    unsigned int  format;		//"WAVE"
    unsigned int  subchunk1Id;		//"fmt "
    unsigned int  subchunk1Size;	//size of fmt table
    unsigned short audioFormat;		//
    unsigned short numChannels;		//
    unsigned int  sampleRate;		//
    unsigned int  byteRate;		//
    unsigned short blockAlign;		//
    unsigned short bitsPerSample;	//
    unsigned int  subchunk2Id;		//"data"
    unsigned int  subchunk2Size;	//size from next to end of file
};

#pragma pack(pop)

#define NOSTOPCURRENTSOUNDS 0
#define STOPCURRENTSOUNDS 1
#define SOUNDEXIST			0x01
#define OBJADREMPTY			0x02
#define SOUNDPLAYINFINITE		0x04
#define STOPSOUND			0x08

struct AUDIOUNIT
{
    unsigned char presence;		//bit 0 = 1 - full struct
					//bit 1 = 1 - obj have incorrect adress
					//bit 2 - 1 - play infinite
					//bit 3 - 1 - stop sound and all queues
					    
    unsigned char 	distance;		//distance from seeing kart to object
    unsigned char 	prevdistance;		//prev distance from seeing kart to object
    unsigned char 	volume;			//volume this sound
    unsigned char	prevvolume;		//saved previous volume
    struct OBJ		*obj;			//object who create sound
    int			length;
    MPQIDS		fileID;
    Mix_Chunk		*sample;
};

int  checkifplay(int channel);
void StopPlayChannel(int channel);
void TerminateAudioObj(struct OBJ *a);
void ChannelFinished(int channel);
int  initsoundengine(int maxsounds);
void deinitsoundengine(void);
int  play_race_rescue(int race,int repeatflag,int distance);
int  Play_sfxdata_id(OBJ *a,int sfxdata_id,int repeatflag,int distance);
int  Play_sfxdata(int x,int y,int sfxdata_id,int repeatedsongs);
int  loadandplaywav(HANDLE mpq,struct OBJ *a,char *filesound,int repeatedflag,int distance);
int  unloadwav(int channel);
int  activatesound(struct OBJ *a,int soundmode,int addfactor,int stopprevsound);
void stopcurrentsound(struct OBJ *a);
void volumeallsfx(int volume);
void setreducevolumeallsfx(int action,int exceptsfxchannelid,int exceptmusicchannelid);
int  GetSoundsPlay(void);
int  BlockSoundToPlay(void);
int  UnBlockSoundToPlay(void);
void RestoreSoundToPlay(int sndblktype);
void ChangeAllVolume(void);
void playadvisorsound(int nplayer,int race,int typeofsong,int song);
void playinfoadvisorsound(int nplayer,int race,int typeofsong,int textsound);
void FreeChannelWithObj(struct OBJ *a);
int  canplaysound(struct OBJ *a,int soundmode,int statuspl);
int  DistanceForAudio(int x,int y);
int  GetTimeWaveLength(WAVHEADER *wavbuf);
int  getchannelplaylength(int channel);
void stopallsounds(void);
int  getsoundIDfromunitsfx(int unitnr,int soundmode);
void WaitUntilAllAudioStops(int maxwait);
int  FileWavePlayLength(HANDLE mpq,char *filesound);
void getsoundIDfromunitdat(SCUNIT SC_Unit,int soundmode,short *from,short *to);
void ReduceVolume(int boolflag);
int  GetReduceVolume(void);
int  SetChannelVolume(int channel, int volume);


#define PLAYADVISOR_ONLYTEXT		0
#define PLAYADVISOR_TEXTANDSOUND	1

#define ADVMINERAL		0
#define ADVGAS			1
#define ADVUNITS		2
#define ADVENERGY		3
#define ADVBASEATACK		4
#define ADVUNITSATACK		5
#define ADVRESEARCH1		6
#define ADVRESEARCH2		7
#define ADVNUCLEARDETECT	8
#define ADVLANDINGERR1		9
#define ADVLANDINGERR2		10
#define ADVADDONCOMPL		11
#define ADVAUXSTRRUCT		12
#define ADVNUCLREADY		13
#define ADVGEYSERNOTREADY	14

#define MAXSOUNDS               16

#define MODESOUNDREADY          0
#define MODESOUNDACTION         1
#define MODESOUNDSELECT         2
#define MODESOUNDPSS            3
#define MODESOUNDERROR          4
#define MAXTYPESOFSOUND         5

#endif /*_AUDIO_W*/
