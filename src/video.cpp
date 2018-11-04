
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <grplib/grp.h>
#include <grplib/gr8.h>
#include <grplib/font.h>

#include "sdl/keysdl.h"
#include "sdl/mousesdl.h"
#include "smacker/smacker.h"
#include "auxil.h"
#include "mpqwrapper.h"
#include "mpq.h"
#include "mytime.h"
#include "fonts.h"
#include "gener.h"
#include "wmem.h"
#include "debug.h"
#include "gamemenu.h"
#include "gr.h"
#include "vars.h"
#include "myfifo.h"
#include "music.h"


#ifdef WITHSDL
	#include "sdl/grsdl.h"
#endif

#include "video.h"
//==========================================
//prev==0 show info from est
void ShowPreviewMission(int campaignnr,int missionnr,int prevafterflag)
{
	int missionid;
	missionid=campaignnr*20+missionnr;
	sprintf(FULLFILENAME,"rez\\%s.txt",alldattbl.campaign_tbl->get_TBL_STR(missionid+1));
	if (prevafterflag)
	{
		strncpy(FULLFILENAME+4,"fin",3);
	}
	ShowPreviewFile(FULLFILENAME,ANYKEYNOTQUITSHOWTITLES);
}
//==========================================
void ShowPreviewFile(const char *filename,int esckeyaction)
{
	HANDLE f;
	char *cmpbuf=NULL;
	int err,filesize;
	UINT readed;
	err = SFileOpenFileTryAllMpqs(filename,&f);
	if (err)
	{
		return;
	}
	filesize = SFileGetFileSize(f,NULL);
	if (filesize<100000)
	{
		cmpbuf = (char *) wmalloc(filesize);
		wr_SFileReadFile(f, cmpbuf, filesize, &readed, NULL);
		ShowPreviewBuff(cmpbuf,readed,esckeyaction);
		if (cmpbuf)
			wfree(cmpbuf);
	}
	else
	{
		DEBUGMESSCR("%s file is to large > 100000.change value\n",FULLFILENAME);
	}
	wr_SFileCloseFile(f);
}
//==========================================
#define FADESPEED_DEFAULT				200
#define DISPLAYTIME_DEFAULT				5000
//if esckeyaction==1 exit do not show next portion of previewtext
void ShowPreviewBuff(char *cmpbuf,int sizebuf,int esckeyaction)
{
		int searchcmd,i,j,k,from,haveatext,fadespeed,displaytime,fontnr;
		int textxpos,textypos,textxsize,textysize,rowsize,textlines,err,deltatick,unpressmouse;
		int changesbackgnd,texttype;
		PCX backgnd,fontpcx;
		char pal[256*4],prevchar;
		char *txtstr;
		TIMER_TICK tick;
		char *fntadr;
		fntadr = NULL;
		fadespeed = FADESPEED_DEFAULT;
		displaytime = DISPLAYTIME_DEFAULT;
		changesbackgnd=0;
		fontnr = IDFONT16X;
		i = 0;
		searchcmd = 0;
		haveatext = 0;
		while( i < sizebuf)
		{
			if (cmpbuf[i]=='<')
			{
				if (haveatext)
				{
					for (k=from;k<=i;k++)
						if ( cmpbuf[k]=='\r' || cmpbuf[k]=='\n')
							from++;
						else
							break;
					if (i-from>4)
					{
						getmaxsymbolsize(fontnr,NULL,&rowsize);
						switch(haveatext)
						{
							case TEXT_SCREENLOWERLEFT:
								texttype=0;
								textlines=getlinesintext(fontnr,&cmpbuf[from],i-from,640-10*2);
								textxpos=10;
								textypos=480-textlines*rowsize-50;
								textxsize=640-10*2;
								textysize=480-textypos;
								break;
							case TEXT_SCREENLEFT:
								texttype=0;
								textlines=getlinesintext(fontnr,&cmpbuf[from],i-from,640-10*2);
								textxpos=60;
								textypos=(480-textlines*rowsize-10*2)/2;
								textxsize=640-70*2;
								textysize=480-20;
								break;
							case TEXT_SCREENRIGHT:
								texttype=1;
								textlines=getlinesintext(fontnr,&cmpbuf[from],i-from,640-10*2);
								textxpos=60;
								textypos=(480-textlines*rowsize-10*2)/2;
								textxsize=640-70*2;
								textysize=480-20;
								break;
							case TEXT_SCREENCENTER:
								texttype=2;
								textlines=getlinesintext(fontnr,&cmpbuf[from],i-from,640-10*2);
								textxpos=60;
								textypos=(480-textlines*rowsize-10*2)/2;
								textxsize=640-70*2;
								textysize=480-20;
								break;
							case TEXT_SCREENTOP:
								texttype=2;
								textlines=getlinesintext(fontnr,&cmpbuf[from],i-from,640-10*2);
								textxpos=10;
								textypos=50;
								textxsize=640-10*2;
								textysize=480-textypos;
								break;
							case TEXT_SCREENBOTTOM:
								texttype=2;
								textlines=getlinesintext(fontnr,&cmpbuf[from],i-from,640-10*2);
								textxpos=10;
								textypos=480-textlines*rowsize-50;
								textxsize=640-10*2;
								textysize=480-textypos;
								break;
							default:
								break;
						}
						//now text between from and i
						prevchar=cmpbuf[i];
						cmpbuf[i]=0;
						memcpy(gameconf.grmode.videobuff,backgnd.GetPcxRawBytes(),backgnd.xsizePcx()*backgnd.ysizePcx());
						if (texttype==0)
							putmessageinrectangleL(textxpos,textypos,textxsize,textysize,rowsize,&cmpbuf[from],fontnr,MYELLOWCOLORFONT,fntadr,NULL,0,0);
						else
							if (texttype==1)
								putmessageinrectangleR(textxpos,textypos,textxsize,textysize,rowsize,&cmpbuf[from],fontnr,MYELLOWCOLORFONT,fntadr,NULL,0);
							else
								putmessageinrectangleM(textxpos,textypos,textxsize,textysize,rowsize,&cmpbuf[from],fontnr,MYELLOWCOLORFONT,fntadr,NULL,0);
						cmpbuf[i]=prevchar;
					}
				}
				searchcmd=1;
				from = i+1;
			}
			else
				if (cmpbuf[i]=='>')
				{
					cmpbuf[i]=0;
					searchcmd=0;
					//parce cmd
					for (j=0;j<sizeof(cmpgn_cmd)/sizeof(char *);j++)
					{
						if (!strncmp(&cmpbuf[from],cmpgn_cmd[j],strlen(cmpgn_cmd[j])))
						{
//							DEBUGMESSCR("find [%s]\n",cmpgn_cmd[j]);
							switch(j)
							{
								case 0://comment
									break;
								case 1://fadespeed
									txtstr = gettextfrombuf(&cmpbuf[from],i-from,1);
									fadespeed = atoi(txtstr);
									fadespeed*=2;
									break;
								case 2://displaytime
									txtstr = gettextfrombuf(&cmpbuf[from],i-from,1);
									displaytime = atoi(txtstr);
									break;
								case 3://background
									changesbackgnd=1;
									if (backgnd.IfPcxIsReady())
									{
										FadeScreen(fadespeed,pal,FADE_HIDE);
										backgnd.closePcx();
									}
									txtstr = gettextfrombuf(&cmpbuf[from],i-from,1);
									err = backgnd.openMpqPcx(txtstr,install1mpq);
									if (err != OKPCX)
									{
										err = backgnd.openMpqPcx(txtstr,install2mpq);
										if (err != OKPCX)
											break;
									}
									backgnd.readPalFromPcx(pal,0);//readfourbytepalette
//									memcpy(gameconf.grmode.videobuff,backgnd.GetPcxRawBytes(),backgnd.xsizePcx()*backgnd.ysizePcx());
									break;
								case 4://fontcolor
									if (fontpcx.IfPcxIsReady())
										fontpcx.closePcx();
									txtstr = gettextfrombuf(&cmpbuf[from],i-from,1);
									err = fontpcx.openMpqPcx(txtstr,install1mpq);
									if (err != OKPCX)
									{
										err = fontpcx.openMpqPcx(txtstr,install2mpq);
										if (err != OKPCX)
											break;
									}
									fntadr = fontpcx.GetPcxRawBytes();//for glowtext
									fntadr[21] = fntadr[11];
									fntadr[22] = fntadr[12];
									break;
								case 5://screenlowerleft
									haveatext=TEXT_SCREENLOWERLEFT;
									break;
								case 6://screenleft
									haveatext=TEXT_SCREENLEFT;
									break;
								case 7://screenright
									haveatext=TEXT_SCREENRIGHT;
									break;
								case 8://screencenter
									haveatext=TEXT_SCREENCENTER;
									break;
								case 9://screentop
									haveatext=TEXT_SCREENTOP;
									break;
								case 10://screenbottom
									haveatext=TEXT_SCREENBOTTOM;
									break;
								case 11://page
									//show the info
									if (changesbackgnd)
									{
										changesbackgnd=0;
										FadeScreen(fadespeed,pal,FADE_SHOW);
									}
									palchange(pal,gameconf.videoconf.gamma,gameconf.videoconf.saturate);
									if (!(gameconf.grmode.flags & DISPLAYFLAGS_EMULATIONMODE))
										wscreenon();
									tick=mytimer.GetCurrentTimerTick();
									keyactive=0;
									unpressmouse=2;
									eventwindowloop();//to clear all events
									do{
										if (highMouse->GetButtonStatus())
											unpressmouse=1;
										else
											if (unpressmouse!=2)
												unpressmouse=0;
										eventwindowloop();
										mytimer.CallTimer(MYTIMER_SINCHRONMODE);
										deltatick = (mytimer.GetCurrentTimerTick()-tick)*1000/TICKS_RES;
									}while( unpressmouse!=0 && !keyactive && deltatick<displaytime);
									if (keyactive==ESCAPEKEY && esckeyaction)
										goto exitshowtitles;
									keyactive=0;
									break;
							}//switch
							break;
						}//if
					}//for
					if (j>=sizeof(cmpgn_cmd)/sizeof(char *))
					{
						DEBUGMESSCR("unknown command [%s]\n",&cmpbuf[from]);
					}
					from = i+2;//now we can parce between commands
				}
				else
				{
					if (!searchcmd)
					{
						//other symbols
					}
				}
			i++;
		}
exitshowtitles:
	if (fontpcx.IfPcxIsReady())
		fontpcx.closePcx();
	if (backgnd.IfPcxIsReady())
	{
		FadeScreen(fadespeed,pal,FADE_HIDE);
		backgnd.closePcx();
	}
}
//==========================================
void FadeScreen(int fadespeed,char *pal,int typeoffade) //0-fadetoimage 1-fadetoblackscreen
{
	int deltatick;
	float gamma,prevgamma;
	TIMER_TICK tick;

	tick = mytimer.GetCurrentTimerTick();
	prevgamma=-256;
	deltatick=0;
	if (typeoffade==FADE_SHOW)
	{
		do{
			mytimer.CallTimer(MYTIMER_SINCHRONMODE);
			gamma = deltatick * (gameconf.videoconf.gamma + 50) / fadespeed - 50;
			if (prevgamma != gamma)
			{
				prevgamma = gamma;
				palchange(pal,(int )gamma,gameconf.videoconf.saturate);
				if (!(gameconf.grmode.flags & DISPLAYFLAGS_EMULATIONMODE))
					wscreenon();
			}
			deltatick = (mytimer.GetCurrentTimerTick()-tick)*1000/TICKS_RES;
		}while(deltatick<fadespeed);
	}
	else
	{
		do{
			mytimer.CallTimer(MYTIMER_SINCHRONMODE);
			gamma = (fadespeed-deltatick) * (gameconf.videoconf.gamma + 50) / fadespeed - 50;
			if (prevgamma != gamma)
			{
				prevgamma = gamma;
				palchange(pal,(int )gamma,gameconf.videoconf.saturate);
				if (!(gameconf.grmode.flags & DISPLAYFLAGS_EMULATIONMODE))
					wscreenon();
			}
			deltatick = (mytimer.GetCurrentTimerTick()-tick)*1000/TICKS_RES;
		}while(deltatick<fadespeed);
	}
}
//==========================================
//get address of textnr-word in buf textnr = (1-...)
//==========================================
char *gettextfrombuf(char *buf,int sizebuf,int textnr)
{
	int i;
	if (textnr)
	{
		for (i=0;i<sizebuf;i++)
		{
			if (buf[i] <= ' ')
				if (--textnr == 0)
					return(&buf[i+1]);
		}
	}
	return(buf);
}
//==========================================
void PlayCampaignVideo(int campaignnr,int missionnr)
{
	int missionid;
	missionid=campaignnr*20+missionnr;
	sprintf(FULLFILENAME,"smk\\%s.smk",alldattbl.campaign_tbl->get_TBL_STR(missionid+1));
	PlayVideoSmk(FULLFILENAME);
}
//==========================================
#define SMKAUDIOCHANNEL			0
#define MAX_SMKAUDIOBUFFERS		32
//==========================================
void PlayVideoSmk(const char *smkfile)
{
	unsigned int xpos,ypos,w,h,audiosize,audio_rate[7],newbitdepth;
	unsigned char nextimage,endsmk,palflag,scalemode,trackmask,audiochannels[7],bitdepth[7],firstframe;
	unsigned char palbuf[3*256];
	unsigned char *smkpal;
	int err,frame,newframe,unpressmouse;
	HANDLE hmpq;
	smk_t *smk;
	unsigned char *vidbuff,*audiobuff;
	TIMER_TICK curtick,prevtick;
	double timeshowoneframe;

	hmpq=FindFileTryAllMpqs(smkfile);
	if (hmpq)
	{
		wclrscr(0);
		wscreenon();

		DEBUGMESSCR("play video [%s]\n",smkfile);
		smk = smk_open_file(hmpq,smkfile,SMK_MODE_MEMORY);//smk_mode_disk not work
		if (smk)
		{
			vidbuff=NULL;
			smk_info_video(smk,&w,&h,&scalemode);
			smk_info_audio(smk,&trackmask,audiochannels,bitdepth,audio_rate);
			smk_info_all(smk,NULL,NULL,&timeshowoneframe);
			smk_enable_video(smk,1);
			if (!(gameconf.audioconf.audioflags&AUDIOFLAGS_SOUNDDISABLE))
			{
				smk_enable_audio(smk,SMKAUDIOCHANNEL,1);
				audiostream = new wMUSIC(MAX_SMKAUDIOBUFFERS);
				if (audiochannels[SMKAUDIOCHANNEL])
				{
					if (bitdepth[SMKAUDIOCHANNEL] == 8)
						newbitdepth = AUDIO_S8;
					else
						newbitdepth = MIX_DEFAULT_FORMAT;

					audiostream->audio.convertaudio = PrepareForConvertAudio(&audiostream->audio.CVT,newbitdepth,
																			audiochannels[SMKAUDIOCHANNEL],
																			audio_rate[SMKAUDIOCHANNEL]);
					if (audiostream->audio.convertaudio == -1) //error - so no audio
						smk_enable_audio(smk,SMKAUDIOCHANNEL,0);
				}
			}
			xpos=(gameconf.grmode.x-w*2)/2;
			ypos=(gameconf.grmode.y-h)/2;
			smk_first(smk,SMK_FULL_DECODE);
			memset(palbuf,0,3*256);
			nextimage=1;
			endsmk=0;
			firstframe=1;
			frame=0;
			prevtick=mytimer.GetCurrentTimerTick();
			keyactive=0;
			unpressmouse=2;
			eventwindowloop();//to clear all events
			do{
				if (nextimage)
				{
					nextimage=0;
					vidbuff=smk_get_video(smk);
					audiobuff=smk_get_audio(smk,SMKAUDIOCHANNEL);				//play only 0 - audio channel
					audiosize=smk_get_audio_size(smk,SMKAUDIOCHANNEL);
					if (audiobuff)
					{
						if (audiostream->audio.convertaudio)
						{
							audiostream->audio.CVT.buf = (Uint8 *) wmalloc(audiosize * audiostream->audio.CVT.len_mult);
//							memset(audiostream->audio.CVT.buf,0,audiosize * audiostream->audio.CVT.len_mult);
							audiostream->audio.CVT.len = audiosize;
							memcpy(audiostream->audio.CVT.buf, audiobuff, audiosize);
							err = ConvertAudio(&audiostream->audio.CVT);
							err = audiostream->queuePCMdata(audiostream->audio.CVT.buf,audiosize * audiostream->audio.CVT.len_mult);
							wfree(audiostream->audio.CVT.buf);
						}
						else
							err = audiostream->queuePCMdata(audiobuff,audiosize);
						if (err == -1)
						{
							DEBUGMESSCR("some data cannot be added to queue. to small\n");
						}
						if (firstframe)
						{
							firstframe=0;
							audiostream->beginplayPCMdata();//no callback function to add to queue, we queued audio PCM data here
						}
					}
//					DEBUGMESSSCR("video frame=%d/%d audiosize=%d\n",smk->cur_frame,smk->f,audiosize);
					palflag=0;
					smkpal = smk_get_palette(smk);
					if (smkpal)
					{
						//current frame palette not identical with previous frame palette
						if (memcmp(smkpal,palbuf,256*3))
						{
							memcpy(palbuf,smkpal,3*256);
							palflag=1;
						}
/*
						for (i=0;i<3*256;i++)
						{
							if (smkpal[i] != palbuf[i])
							{
								memcpy(palbuf,smkpal,3*256);
								palflag=1;
								break;
							}
						}
*/
					}
				}
				if (vidbuff)
					putrow2x1(xpos,ypos,w,h,vidbuff);
				if (highMouse->GetButtonStatus())
					unpressmouse=1;
				else
					if (unpressmouse!=2)
						unpressmouse=0;
				eventwindowloop();
				mytimer.CallTimer(MYTIMER_SINCHRONMODE);
				curtick = mytimer.GetCurrentTimerTick();
				newframe=(curtick-prevtick)*1000000/TICKS_RES/timeshowoneframe;
				if (newframe != frame)
				{
					frame = newframe;
					nextimage=1;
					err=smk_next(smk,SMK_FULL_DECODE);
					if (err == SMK_DONE || err == SMK_LAST)//prevent play from beginning if have ring frame
						endsmk=1;
				}
				if (palflag)
				{
					activatepalette256x3(smkpal);
					if (!(gameconf.grmode.flags & DISPLAYFLAGS_EMULATIONMODE))
						wscreenonregion(xpos,ypos,w*2,h);
				}
				else
					wscreenonregion(xpos,ypos,w*2,h);
			}while(!endsmk && !keyactive && unpressmouse!=0);
			smk_close(smk);
		}
		StopMusic(MUSIC_STOP);
		wclrscr(0);
		wscreenon();
	}
}
//==========================================
