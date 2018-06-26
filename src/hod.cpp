/* This file contains definitions for Heads-On Display */

#include "defsunits.h" 
#include "hod.h" 

short mage_descriptions_hod[MAXMAGEATR] =
{
  STATTXT_PARASITEDETECTED,				//parasite detected
  STATTXT_HALLUCINATE,					//hallucination
  STATTXT_ACIDSPORE,					//acid spores
  STATTXT_BLIND,					//blind
  STATTXT_DETECTOR,					//detector
  -1,							//ensnare
  -1,							//defence matrix
  -1,							//stim pack
  -1,							//invisible
  -1,							//lock down
  -1,							//maelstrom
  -1,							//stasis field
  -1,							//plague
  -1,							//irradiate
  -1,							//recall
};

short ub_constr[3][4] = 
{
//    {"Mutating","Morphing","evolving","evolving"},
//    {"Under Construction","Building","upgrading","researching"},
//    {"Open Warp Rift","Open Warp Gate","upgrading","developing"},
    {798,770,764,767},
    {799,771,765,768},
    {800,772,766,769}
};
short ub_deconstr[3][4] = 
{
//    {"Cancel Morph","Cancel Morph","cancel upg","cancel resear"},
//    {"Cancel constr","Cancel constr","cancel upg","cancel resear"},
//    {"Cancel warp","Cancel warp","cancel upg","cancel resear"},

	{801,801,804,805},
	{802,802,804,805},
	{803,803,804,805}
	
};

