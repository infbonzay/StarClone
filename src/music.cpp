
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "CrossPlatform.h"
#include "audiolowlevel.h"
#include "debug.h"
#include "myfifo.h"
#include "audio.h"
#include "mpq.h"
#include "wmem.h"
#include "vars.h"
#include "music.h"

wMUSIC *audiostream;
//==========================================
wMUSIC::wMUSIC(int maxbuffers)
{
	musicbuffers = new AUDIOPACKETS(maxbuffers);
	musicsample = NULL;
	stopchannel = 0;
	chunkplay = -1;
	fade = 0;
	AddPCMDataFunc = NULL;
	OnFinishPlay = NULL;
	memset(&audio, 0, sizeof(audio));
}
//==========================================
wMUSIC::~wMUSIC()
{
	musicsample = NULL;
	stopchannel = 0;
	chunkplay = -1;
	AddPCMDataFunc = NULL;
	OnFinishPlay = NULL;
	fade = 0;
	if (musicbuffers)
	{
		delete musicbuffers;
		musicbuffers = NULL;
	}
}
//==========================================
int wMUSIC::onfinishaudiodatabuffer(int channel)
{
	//delete data what finish to play this portion of pcm data
	musicbuffers->DelCurElem();
	//	  wFreeChunk(musicsample);
	if (AddPCMDataFunc)
		(*AddPCMDataFunc)(this);
	//play next data
	if (!stopchannel)
	{
		if (nextplayPCMdata())
		{
			void(*onfinish)(void) = audiostream->OnFinishPlay;
			StopMusic(MUSIC_STOP);
			if (onfinish)
				(*onfinish)();
			return(1);
		}
	}
	return(0);
}
//==========================================
int wMUSIC::queuePCMdata(unsigned char *audiobuff, int audiosize)
{
	AUDIOPACKET *apacket;
	apacket = musicbuffers->Add(sizeof(AUDIOPACKET) + audiosize);
	if (!apacket)
	{
		return(-1);
	}
	else
	{
		apacket->len = audiosize;
		memcpy(apacket->packetdata, audiobuff, audiosize);
		return(0);
	}
}
//=======================================
void wMUSIC::ChangeMusicVolume(int volume)
{
	if (chunkplay != -1)
		SetChannelVolume(chunkplay, volume*MIX_MAX_VOLUME / 100);
}
//=======================================
int wMUSIC::playPCMdata(int firstrun)
{
	wCHUNK		*endmusicsample;
	AUDIOPACKET *apacket;
	apacket = musicbuffers->GetCurElem();
	if (apacket)
	{
		/*		if (fade)
				{
					if (fade)
						return(-1);
					//make fade operation
		//			if (fade is zero)
					signed short *samples=(signed short *)apacket->packetdata;
					signed short sample;
					float fsample,delta;
					int fadelen = apacket->len/2;
					for (int i=0;i<fadelen;i++)
					{
						fsample = (samples[i]);
						delta = (float)i/fadelen;
						fsample *=delta;
						samples[i] -= fsample;
					}
					fade = 0;
				}
		*/
		endmusicsample = musicsample;
		musicsample = PlayRAWMem(apacket->packetdata, apacket->len);
		chunkplay = wPlayChannel(chunkplay, musicsample, 0);
		if (endmusicsample)
		{
			//free previous music sample
			wFreeChunk(endmusicsample);
		}

		if (chunkplay == -1)//can't play no free channels
		{
			//if error play
			return -2;
		}
		if (firstrun)
		{
			ChangeMusicVolume(gameconf.audioconf.musicvolume);
		}
		return(0);
	}
	return(-3);
}
//=======================================
int wMUSIC::StopPCMPlay(void)
{
	stopchannel = 1;
	if (chunkplay != -1)
		wStopChannel(chunkplay);
	if (musicsample)
	{
		//free previous music sample
		wFreeChunk(musicsample);
		musicsample = NULL;
	}
	return(0);
}
//=======================================
#define MUSIC_AUDIOBUFFERS		16
#define SIZEONEAUDIOBUFFER		(1024*2*4)
//=======================================
void LoadNextAudioData(void *info)
{
	unsigned int readed, nextbuffersize;
	int result;
	unsigned char *buff;
	wMUSIC *stream = (wMUSIC *)info;
	RWOPS *rwops = stream->audio.RWops;

	do {
		if (!stream->musicbuffers->GetFreeElem())
		{
			return;		//no free buffers
		}
		if (stream->audio.remainbytes == 0)
		{
			//reach end of music data
			if (stream->audio.repeatplay)
			{
				if (stream->audio.repeatplay > 0)
					stream->audio.repeatplay--;
				//go to the begining of audio data (repeat play)
				result = rwops->seek(rwops, sizeof(WAVHEADER), SEEK_SET);
				if (result == -1)
					return;
				audiostream->audio.remainbytes = audiostream->audio.alen;
				//				DEBUGMESSCR("repeat play\n");
			}
			else
			{
				return;
			}
		}
		if (stream->audio.remainbytes >= SIZEONEAUDIOBUFFER * 2)
			nextbuffersize = SIZEONEAUDIOBUFFER;
		else
			nextbuffersize = stream->audio.remainbytes;
		buff = (unsigned char *)wmalloc(nextbuffersize);
		readed = rwops->read(rwops, buff, nextbuffersize, 1);
		if (readed != 1)
		{
			//error read
			wfree(buff);
			return;
		}
		if (stream->audio.convertaudio)
		{
			stream->audio.CVT.buf = (unsigned char *)wmalloc(nextbuffersize * stream->audio.CVT.len_mult);
			stream->audio.CVT.len = nextbuffersize;
			memcpy(stream->audio.CVT.buf, buff, nextbuffersize);
			result = ConvertAudio(&stream->audio.CVT);
			result = stream->queuePCMdata(stream->audio.CVT.buf, nextbuffersize * stream->audio.CVT.len_mult);
			wfree(stream->audio.CVT.buf);
		}
		else
		{
			result = stream->queuePCMdata(buff, nextbuffersize);
		}
		if (result == -1)
		{
			DEBUGMESSCR("some data cannot be added. queue to small\n");
		}
		wfree(buff);
		stream->audio.remainbytes -= nextbuffersize;
		//		DEBUGMESSCR("load next audio data\n");
	} while (1);
}
//=======================================
int PlayMusic(const char *filename, int repeatflag)
{
	return(PlayMusic(filename, repeatflag, NULL));
}
//=======================================
int PlayMusic(const char *filename, int repeatflag, void(*onfinishplay)(void))
{
	int i;
	unsigned int readed;
	WAVHEADER wavheader;
	HANDLE hmpq;
	RWOPS *rwops;
	if (gameconf.audioconf.musicvolume < 5)
		return(-1);
	DEBUGMESSCR("play %s\n", filename);
	if (audiostream)
	{
		if (audiostream->fade)
		{
			//wait until fade in out
			do {
				usleep(1000);
			} while (audiostream);
		}
		else
		{
			DEBUGMESSCR("now playing\n");
			return(-2);
		}
	}
	hmpq = FindFileTryAllMpqs(filename);
	if (!hmpq)
		return(-3);
	rwops = My_RWFromMpqFile(hmpq, filename, SFILE_OPEN_FROM_MPQ);
	if (!rwops)
		return (-4);
	readed = rwops->read(rwops, &wavheader, sizeof(struct WAVHEADER), 1);
	if (readed != 1)
	{
		rwops->close(rwops);
		return(-5);
	}
	//check the audio file
	if (wavheader.chunkId != WAV_RIFF || wavheader.format != WAV_WAVE || wavheader.subchunk1Id != WAV_FMT ||
		wavheader.audioFormat != WAV_PCM_CODE || wavheader.subchunk2Id != WAV_DATA ||
		(wavheader.bitsPerSample != 8 && wavheader.bitsPerSample != 16))
	{
		rwops->close(rwops);
		return(-6);
	}
	//this is riff wave we can process fill wave info in stream struct
	audiostream = new wMUSIC(MUSIC_AUDIOBUFFERS);

	audiostream->audio.channels = (char)wavheader.numChannels;
	audiostream->audio.frequency = wavheader.sampleRate;
	if (wavheader.bitsPerSample == 8)
		audiostream->audio.bitdepth = AUDIO_S8;
	else
		audiostream->audio.bitdepth = MIX_DEFAULT_FORMAT;

	audiostream->audio.alen = wavheader.subchunk2Size;
	audiostream->audio.repeatplay = repeatflag;
	audiostream->audio.remainbytes = audiostream->audio.alen;
	audiostream->audio.RWops = rwops;
	audiostream->audio.convertaudio = PrepareForConvertAudio(&audiostream->audio.CVT,
		audiostream->audio.bitdepth,
		audiostream->audio.channels,
		audiostream->audio.frequency);
	if (audiostream->audio.convertaudio == -1)	//cannot do converting to desire audio format
	{
		StopMusic(MUSIC_STOP);
		return(-7);
	}
	//fill audio buffers
	audiostream->AddPCMDataFunc = &LoadNextAudioData;
	audiostream->OnFinishPlay = onfinishplay;
	for (i = 0;i < MUSIC_AUDIOBUFFERS;i++)
	{
		LoadNextAudioData(audiostream);
	}
	audiostream->beginplayPCMdata();
	return(0);
}
//=======================================
void StopMusic(int fade)
{
	if (audiostream)
	{
		/*		if (fade == MUSIC_STOPWITHFADE)
				{
					audiostream->fade = 100;
					return;
				}
		*/
		//		sleep(1);
		audiostream->StopPCMPlay();
		RWOPS *rwops = audiostream->audio.RWops;
		if (rwops)
			rwops->close(rwops);
		delete audiostream;
		audiostream = NULL;
	}
}


