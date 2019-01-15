#if !defined(_USEGRP_W)
#define _USEGRP_W

#include <grplib/grp.h>

#define NORMAL		0
#define TRANSPARENT	1
#define DISTORTION	2
#define SHADOW		3
#define TOTABLE		4
#define GLOW		5

#ifdef __cplusplus
    extern "C" {
#endif
//=======================================
void GRP_PutGrp(int x,int y,struct GRPFILE *adr,int format,
                int maxcolor,int grcolor,char *tablgrd,int nrpicture);
void GRP_IPutGrp(int x,int y,struct GRPFILE *adr,int format,
                int maxcolor,int grcolor,char *tablgrd,int nrpicture);
//=======================================
#ifdef __cplusplus
    }
#endif

#endif /*   _USEGRP_W   */

