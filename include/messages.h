#if !defined(_MESSAGES_W)
#define _MESSAGES_W

#include "mylist.h"

#define BF_MALIGN	0x01
#define BF_RALIGN	0x02
#define BF_ALLOCBUF	0x04
#define BF_FORCEADD	0x08
#define BF_SPLITLINES	0x10

#define INFO_DEFAULTTIME	5000
struct BARMES
{
    char *mes;
    int  dissapeartime;
    char fontnr;
    char color;
    char flags;
};

class BARMESSAGES
{
    int maxmessages;
    mylist barmes;
    void clearonemessage(int elemnr);
public:
    
    BARMESSAGES(int maxmessages);
    ~BARMESSAGES(void);
    void addbarmessage(char *mes,int fontnr,int color,int showedtime,int flags);
    void showallmessages(int x,int y,int sizexbar,int heightoneline);
    void clearallmessages(void);
};

#endif /* _MESSAGES_W*/

