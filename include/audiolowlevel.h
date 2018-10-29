#ifndef _AUDIOLOWLEVEL_W
#define _AUDIOLOWLEVEL_W

#include "mpq.h"

#ifdef WITHSDLMIXER
	#include <SDL/SDL_mixer.h>
	typedef Mix_Chunk wCHUNK;
	typedef SDL_AudioCVT Audio_CVT;
	typedef SDL_RWops RWOPS;
#else
	typedef int Mix_Chunk;
	typedef int wCHUNK;
	
	typedef struct RWOPS {
		int (*seek)(struct RWOPS *context, int offset, int whence);
		int (*read)(struct RWOPS *context, void *ptr, int size, int maxnum);
		int (*write)(struct RWOPS *context, const void *ptr, int size, int num);
		int (*close)(struct RWOPS *context);
		union 
		{
			struct 
			{
				int		autoclose;
				HANDLE	fp;
			} stdio;
		} hidden;

	} RWOPS;
	struct Audio_CVT
	{
		unsigned int len;
		unsigned int len_mult;
		unsigned char *buf;
	};
	#define MIX_MAX_VOLUME		0
	#define MIX_DEFAULT_FORMAT	0
#ifndef AUDIO_S8
	#define AUDIO_S8			0
#endif
#endif

wCHUNK *PlayRAWMem(unsigned char *buff,int len);
wCHUNK *PlayMpqWav(RWOPS *buff,int closeflag);
RWOPS *My_RWFromMpqFile(HANDLE mpqid,const char *file, unsigned int SearchScope);

int	 wOpenAudio(void);
void wCloseAudio(void);
int	 wSetChannels(int maxchannels);
int	 wChannelVolume(int channel,int volume);
int	 wChunkVolume(wCHUNK *sample,int volume);
void wOnChannelFinish(void (*func)(int channel));
wCHUNK *wLoadWav(char *filename);
void wFreeChunk(wCHUNK *sample);
int	 wPlayChannel(int channel_nr,wCHUNK *sample,int repeat);
void wPauseChannel(int channel_nr);
void wResumeChannel(int channel_nr);
int	 wStopChannel(int channel_nr);
int	 PrepareForConvertAudio(Audio_CVT *audio_CVT,unsigned int bitdepth,unsigned char audiochannels,unsigned int audio_rate);
int	 ConvertAudio(Audio_CVT *CVT);
void LockAudio(void);
void UnlockAudio(void);

#endif /* _AUDIOLOWLEVEL_W */
