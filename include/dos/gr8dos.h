#if !defined(_GR8DOS_W)
#define _GR8DOS_W 1

struct RGBPERSENT
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

extern "C"
{
  extern  unsigned short DPMIReg_IP,DPMIReg_CS;
  extern  unsigned short DPMIReg_EBX,DPMIReg_EDX;
  extern  int wmaxbank,wmaxbankl,wgranularity;
  extern  unsigned char DACswitch;
  extern  int TotalVMem;

  extern  int wlinecount[1024+256];
  extern  int wmaxdworddesen[1024+256];
  extern  int wendlineaddfactor[1024+256];

  void extern wscreenon(void);
  void extern wreadpalette(char *m);
  void extern wwritepalette(char *m);
  void extern wvertint(void);
  void wconvertpal(int number,int count,
                          struct RGBPERSENT *rgbpers,int bitmask);
  void settextmode(void);
  int  detectmode(int x,int y,int bpp,int fullscreen);
  void SetPitchAndChunk(void);
  void activatepalette(char *pal);
  int eventwindowloop(void);
  void *getvidmem(void);

}
#endif


