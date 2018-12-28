#if !defined(_GGR8H_W)
#define _GGR8H_W

#ifdef __cplusplus
    extern "C" {
#endif
    void CClrScr(int color);
    int CGetImage8(int x,int y,int sx,int sy,char *mem);
    int CPutImage8(int x,int y,int sx,int sy,char *mem);
    int CSetImage8(int x,int y,int sx,int sy,int color);
    int CSetImage8x(int x,int y,int sx,int sy,int color);
    int CPutSprite8(int x,int y,int sx,int sy,int color,char *adr);
    void CPutPartVisibleSpr(int x,int y,int sx,int sy,char *mem);
    void CPutPartVisibleSprTrnsp(int x,int y,int sx,int sy,char *mem);
    void CPutSpr(int x,int y,int sx,int sy,int rx,char *mem);

    void wgetimage8(int x,int y,char *m);
    void wputimage8(int x,int y,char *m);
    void wsetimage8(int color,struct XY *m);
    void wsetimage8x(int color,struct XY *m);
    void wputsprite8(int x,int y,char *adr,int color);
    void wputpartsprite8(int x,int y,char *adr);
    void wputstasprite8(int x,int y,char *adr);

//==============================================
  #define wclrscr 		CClrScr
  #define putspr		CPutSpr
  #define putpartsprite		wputpartsprite8
  #define putstasprite8 	wputstasprite8

#ifdef __cplusplus
    }
#endif

#endif /* _GGR8H_W */
