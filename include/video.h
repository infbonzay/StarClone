#ifndef _SC_VIDEO_H

#define _SC_VIDEO_H

#define TEXT_SCREENLOWERLEFT	1
#define TEXT_SCREENLEFT			2
#define TEXT_SCREENRIGHT		3
#define TEXT_SCREENCENTER		4
#define TEXT_SCREENTOP			5
#define TEXT_SCREENBOTTOM		6

#define FADE_SHOW				0
#define FADE_HIDE				1

#define ANYKEYNOTQUITSHOWTITLES 0
#define ANYKEYQUITSHOWTITLES	1

void ShowPreviewMission(int campaignnr,int missionnr,int prevafterflag);
void ShowPreviewBuff(char *cmpbuf,int sizebuf,int esckeyaction);
void ShowPreviewFile(const char *filename,int esckeyaction);
void FadeScreen(int fadespeed,char *pal,int typeoffade);

char *gettextfrombuf(char *buf,int sizebuf,int textnr);
void PlayCampaignVideo(int campaignnr,int missionnr);
void PlayVideoSmk(const char *smkfile);
int	 queuePCMdata(unsigned char *audiobuff,int audiosize);
int	 playPCMdata(int firstrun);
void smkaudiofinish(int channel);
inline int beginplayPCMdata(void) { return(playPCMdata(0)); };
inline int nextplayPCMdata(void) { return(playPCMdata(1)); };



#endif

