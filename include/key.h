
#if !defined(_KEY_W)
#define _KEY_W

extern int enterpasw;
//================================
void keyscroll(void);
void keyup(void);
void keydown(void);
void pausekey(void);
void charup(char *a);
void chardown(char *a);
void charupdown(char *a,int mode);
int  readkey(void);
int  strcmpw(char *a,char *b);
void keyhandler(void);
//================================
#endif

