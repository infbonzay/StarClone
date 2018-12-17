#include "audiolowlevel.h"

//=================================================
int wOpenAudio(void)
{
	return(-1);//we do not have sounds and music at all
}
//=================================================
void wCloseAudio(void)
{
}
//=================================================
int wSetChannels(int maxchannels)
{
	return(0);
}
//=================================================
int wChunkVolume(Mix_Chunk *sample,int volume)
{
	return(0);
}
//=================================================
int wChannelVolume(int channel,int volume)
{
	return(0);
}
//=================================================
void wOnChannelFinish(void (*func)(int channel))
{
}
//=================================================
wCHUNK *PlayMpqWav(void *buff,int closeflag)
{
	return(NULL);
}
//=================================================
Mix_Chunk *wLoadWav(char *filename)
{
	return(NULL);
}
//=================================================
wCHUNK *PlayRAWMem(unsigned char *buff,int len)
{
	return(NULL);
}
//=================================================
void wFreeChunk(Mix_Chunk *sample)
{
}
//=================================================
int wPlayChannel(int channel_nr,Mix_Chunk *sample,int repeat)
{
	return(0);
}
//=============================================
void wPauseChannel(int channel_nr)
{
}
//=============================================
void wResumeChannel(int channel_nr)
{
}
//=============================================
int wStopChannel(int channel_nr)
{
	return(0);
}
//=============================================
Mix_Chunk *Mix_LoadWAV_RW(void *,long )
{
	return NULL;
}
//=============================================
wCHUNK *PlayMpqWav(RWOPS *buff,int closeflag)
{
	return NULL;
}
//=============================================
void *SDL_RWFromMem(void *,long)
{
	return NULL;
}
//=============================================
RWOPS *My_RWFromMpqFile(HANDLE mpqid,const char *file, unsigned int SearchScope)
{
	return(NULL);
}
//==========================================
int PrepareForConvertAudio(Audio_CVT *cvt,unsigned int bitdepth,
							unsigned char audiochannels,unsigned int audio_rate)
{
	return(-1);
}
//==========================================
int ConvertAudio(Audio_CVT *CVT)
{
	return(-1);
}
//==========================================
void LockAudio(void)
{

}
//==========================================
void UnlockAudio(void)
{

}
//==========================================
