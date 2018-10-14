
#if !defined(_LOAD_W)
#define _LOAD_W

#include <stdio.h>
#include "man.h"
#include "tbl.h"

int loadznak(void);
void unloadznak(void);
void unloadcircles(void);

int saveallstruct(char *file,char *string1);
int loadstage(char *file);
void restart(void);
int loadlang(void);
int _loadlang(const char *lang);
struct OBJstruct *loadobj(int numberobj);

int loadpcxtables(void);
void unloadpcxtables(void);

int loadpcxicons(void);
void unloadpcxicons(void);

void loadallfonts(void);
void unloadallfonts(void);

int  loadpal(int tileset,char *palette4);
void unloadpal(void);

void loadtexturegrp(int images_texture_id);
void unloadtexturegrp(void);

int loadramka(int needrace);
void unloadramka(void);
int  loadbeforestarmap(int race);

int loadbuttons(int race);
void unloadbuttons(void);

int loadtilegrp(int race);
void unloadtilegrp(void);

int loadcreep(int tileset);
void unloadcreep(void);

int continueway(struct OBJ *a,struct OBJstruct *b);
void savevar(FILE *h);
void loadvar(FILE *h);
void saveKARTA(FILE *h);
void loadKARTA(FILE *h);
void savelog(const char *mes,int i);
int  LoadDatTblFiles(DATTBLSTRUCT *dattbl);
void UnloadDatTblFiles(DATTBLSTRUCT *dattbl);
void loadallobj(void);
void UnloadAllOBJSTRUCT(void);
void LoadPatchTbl(void);


void Unload_SC_Images_List(void);
void *FindLoadedInList(short images_tbl);
int  GetLoadedImage(int mpqfilenr,int images_tbl,void **loadeddata);
int  GetLoadedImage(int images_tbl,void **loadeddata);
//===========================================
void unloadafterstarmap(void);
int  getcampaignname(int campaign_id,int mission_id);
int  openmission(int campaign_id,int mission_id);
void getmissionidformmissionname(char *missionname,int *mission_id);
int  CheckForCheats(char *txt1);


//=======================================
extern char		shield_pos[]; 
extern char		mineral_pos[];
extern char		gas_pos[];
extern void* 		imagesreftogrpdata[];

//extern unsigned char	campaigns_races[];

#endif /*_LOAD_W*/

