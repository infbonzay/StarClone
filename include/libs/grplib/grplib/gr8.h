#if !defined(_GGR8H_W)
#define _GGR8H_W

#ifdef __cplusplus
    extern "C" {
#endif
    void GRP_ClrScr(int color);
    int  GRP_GetImage8(int x,int y,int sx,int sy,char *mem);
    int  GRP_PutImage8(int x,int y,int sx,int sy,char *mem);
    int  GRP_SetImage8(int x,int y,int sx,int sy,int color);
    int  GRP_SetImage8x(int x,int y,int sx,int sy,int color);
    int  GRP_PutSprite8(int x,int y,int sx,int sy,int color,char *adr);
    void GRP_PutPartVisibleSpr(int x,int y,int sx,int sy,char *mem);
    void GRP_PutPartVisibleSprTrnsp(int x,int y,int sx,int sy,char *mem);
    void GRP_PutSpr(int x,int y,int sx,int sy,int rx,char *mem);

    void GRP_PutPackedSprite8(int x,int y,char *adr);


#ifdef __cplusplus
    }
#endif

#endif /* _GGR8H_W */
