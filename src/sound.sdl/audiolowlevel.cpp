#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include "mpqwrapper.h"
#include "audiolowlevel.h"
#include "mpq.h"

#ifdef WITHSDL
	#include <SDL/SDL.h>
#endif

#define AUDIO_FREQ				22050
#define AUDIO_BITDEPTH			MIX_DEFAULT_FORMAT
#define AUDIO_CHANNELS			2
#define AUDIO_CHUNKSIZE			512

//=================================================
int wOpenAudio(void)
{
	int err = SDL_InitSubSystem(SDL_INIT_AUDIO);
	if (err)
		return -1;
	return(Mix_OpenAudio(AUDIO_FREQ,AUDIO_BITDEPTH,AUDIO_CHANNELS,AUDIO_CHUNKSIZE));
}
//=================================================
void wCloseAudio(void)
{
	Mix_CloseAudio();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}
//=================================================
int wSetChannels(int maxchannels)
{
	return(Mix_AllocateChannels(maxchannels));
}
//=================================================
int wChunkVolume(wCHUNK *sample,int volume)
{
	return(Mix_VolumeChunk(sample,volume));
}
//=================================================
int wChannelVolume(int channel,int volume)
{
	return(Mix_Volume(channel,volume));
}
//=================================================
void wOnChannelFinish(void (*func)(int channel))
{
	Mix_ChannelFinished(func);
}
//=================================================
wCHUNK *wLoadWav(char *filename)
{
	return(Mix_LoadWAV(filename));
}
//=================================================
wCHUNK *PlayMpqWav(RWOPS *buff,int closeflag)
{
	return(Mix_LoadWAV_RW(buff,closeflag));
}
//=================================================
//=================================================
wCHUNK *PlayRAWMem(unsigned char *buff,int len)
{
	return(Mix_QuickLoad_RAW(buff,len));
}
//=================================================
void wFreeChunk(wCHUNK *sample)
{
	Mix_FreeChunk(sample);
}
//=================================================
int wPlayChannel(int channel_nr,wCHUNK *sample,int repeat)
{
	return(Mix_PlayChannel(channel_nr,sample,repeat));
}
//=============================================
void wPauseChannel(int channel_nr)
{
	Mix_Pause(channel_nr);
}
//=============================================
void wResumeChannel(int channel_nr)
{
	Mix_Resume(channel_nr);
}
//=============================================
int wStopChannel(int channel_nr)
{
	return(Mix_HaltChannel(channel_nr));
}
//=============================================
int PrepareForConvertAudio(Audio_CVT *cvt,unsigned int bitdepth,
							unsigned char audiochannels,unsigned int audio_rate)
{
	if (bitdepth != AUDIO_BITDEPTH || audiochannels != AUDIO_CHANNELS || audio_rate != AUDIO_FREQ)
		return(SDL_BuildAudioCVT(cvt, bitdepth, audiochannels, audio_rate,AUDIO_BITDEPTH,AUDIO_CHANNELS,AUDIO_FREQ));
	return(0);
}
//==========================================
int ConvertAudio(Audio_CVT *CVT)
{
	return(SDL_ConvertAudio(CVT));
}
// functions specials for loading from mpq
//=============================================
static int SDLCALL mpqfile_seek(RWOPS *context, int offset, int whence)
{
		int result = SFileSetFilePointer((HANDLE) context->hidden.stdio.fp, offset, NULL, whence);
		if ( result >= 0 )
		{
			return(result);
		}
		else
		{
			SDL_Error(SDL_EFSEEK);
			return(-1);
		}
}
//=============================================
static int SDLCALL mpqfile_read(RWOPS *context, void *ptr, int size, int maxnum)
{
		int result;
		unsigned int nread;
//		printf("read: size=%d\n",size*maxnum);
		result = SFileReadFile( (HANDLE) context->hidden.stdio.fp, ptr, size * maxnum, &nread, NULL );
		if ( !result)
		{
			SDL_Error(SDL_EFREAD);
		}
		result = SFileGetFileInfo((HANDLE) context->hidden.stdio.fp,SFILE_INFO_POSITION);
		return(nread/size);
}
//=============================================
static int SDLCALL mpqfile_write(RWOPS *context, const void *ptr, int size, int num)
{
		int result;
		unsigned int nwrote;
//		result = SFileWriteFile( (HANDLE) context->hidden.stdio.fp, (void *) ptr, size * num, &nwrote, NULL );
//		if (!result)
//		{
//			SDL_Error(SDL_EFWRITE);
//		}
		nwrote = 0;				//return 0 writes
		SDL_Error(SDL_EFWRITE);
		return(nwrote); 
}
//=============================================
static int SDLCALL mpqfile_close(RWOPS *context)
{
		if ( context ) 
		{
				if ( context->hidden.stdio.autoclose ) 
				{
					if (context->hidden.stdio.fp)
						SFileCloseFile((HANDLE)context->hidden.stdio.fp);
				}
				SDL_FreeRW(context);
		}
		return(0);
}
//=============================================
RWOPS *My_RWFromMpqFile(HANDLE mpqid,const char *file, unsigned int SearchScope)
{
		RWOPS *rwops=NULL;
		int result;
		HANDLE f;
		if ( !file || !*file )
		{
			return NULL;
		}
		result = wr_SFileOpenFileEx(mpqid, file, SearchScope, &f);
		if ( !result ) 
		{
			return(NULL);
		} 
		else 
		{
			rwops = SDL_AllocRW();
			if ( rwops != NULL )
			{
				rwops->seek = mpqfile_seek;
				rwops->read = mpqfile_read;
				rwops->write = mpqfile_write;
				rwops->close = mpqfile_close;
				rwops->hidden.stdio.fp = (FILE *)f;
				rwops->hidden.stdio.autoclose = 1;
			}
		}
		return(rwops);
}
//=============================================
void LockAudio(void)
{
	SDL_LockAudio();
}
//=============================================
void UnlockAudio(void)
{
	SDL_UnlockAudio();
}

