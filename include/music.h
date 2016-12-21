#if !defined(_MUSIC_W)
#define _MUSIC_W

#include "audiolowlevel.h"
#include "mpq.h"
#include "myfifo.h"

#define MUSIC_STOP		0
#define MUSIC_STOPWITHFADE	1
struct AUDIOPACKET
{
    unsigned int  len;
    unsigned char packetdata[];
};

class wMUSIC
{
private:
    
    wCHUNK	*musicsample;
    signed char	chunkplay;

    int 	playPCMdata(int firstrun);
public:    
    char 	stopchannel;
    signed char fade;
    myfifo  	*musicbuffers;
    void 	(*AddPCMDataFunc)(void *buf);
    void	(*OnFinishPlay)(void);
    struct
    {
	unsigned short	channels;
	unsigned short	bitdepth;
	unsigned int	frequency;	
	unsigned long	alen;		//audio data in bytes
	signed char	repeatplay;	//0-1time, 1-2times -1 infinitly
	signed char	convertaudio;	//0-no need convertion, 1- need convertion
	unsigned long	remainbytes;	//remain bytes to play
	Audio_CVT	CVT;
	RWOPS		*RWops;
    }audio;

    wMUSIC(int maxbuffers);
    ~wMUSIC();
    
    int		onfinishaudiodatabuffer(int channel);
    int  	queuePCMdata(unsigned char *audiobuff,int audiosize);
    inline int 	beginplayPCMdata(void) { return(playPCMdata(1)); };
    inline int 	nextplayPCMdata(void)  { return(playPCMdata(0)); };
    inline int 	GetChunkPlay(void) { return(chunkplay); };
    int		StopPCMPlay(void);
    void 	ChangeMusicVolume(int volume);

};

int  PlayMusic(char *filename,int repeatflag);
int  PlayMusic(char *filename,int repeatflag, void (*onfinishplay)(void));
void StopMusic(int fade);

extern wMUSIC *audiostream;

#endif

