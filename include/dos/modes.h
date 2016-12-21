#pragma pack(__push,1)
struct SvgaInfoRec
{
       char  Signature[4];                      //Will be 'VESA'
       unsigned short Version;                  //VESA version number; hi-byte=major; low=minor
       unsigned int   Manufactured;             //ASCIIZ Manufacture ID or other text info
       char  Abilities[4];                      //four bytes of bit flags
       unsigned int   pfawModes;                //address of a list of 16-bit supported mode values
       unsigned short TotalMem;                 //in 64KB
       char  Reserved[236];                     //remainder of SvgaInfoRec
};
struct MODES{
       unsigned short ModeAttributes;           //mode attributes
       unsigned char  WinAAttributes;           //window A attributes
       unsigned char  WinBAttributes;           //window B attributes
       unsigned short WinGranularity;           //window granularity in KB
       unsigned short WinSize;                  //window size
       unsigned short WinASegment;              //window A start segment
       unsigned short WinBSegment;              //window B start segment
       unsigned int   WinFuncPtr;               //pointer to window function
       unsigned short BytesPerScanLine;         //bytes per scan line

       //extended information [if ((ModeAttributes&2)==2) - true]

       unsigned short XResolution;                //horizontal resolution in pixels or characters3
       unsigned short YResolution;                //vertical resolution in pixels or characters
       unsigned char XCharSize;                   //character cell width in pixels
       unsigned char YCharSize;                   //character cell height in pixels
       unsigned char NumberOfPlanes;              //number of memory planes
       unsigned char BitsPerPixel;                //bits per pixel
       unsigned char NumberOfBanks;               //number of banks
       unsigned char MemoryModel;                 //memory model type
       unsigned char BankSize;                    //bank size in KB
       unsigned char NumberOfImagePages;          //number of images
       unsigned char Reserved;                    //reserved for page function
       unsigned char RedMaskSize;                 //size of direct color red mask in bits
       unsigned char RedFieldPosition;            //bit position of lsb of red mask
       unsigned char GreenMaskSize;               //size of direct color green mask in bits
       unsigned char GreenFieldPosition;          //bit position of lsb of green mask
       unsigned char BlueMaskSize;                //size of direct color blue mask in bits
       unsigned char BlueFieldPosition;           //bit position of lsb of blue mask
       unsigned char RsvdMaskSize;                //size of direct color reserved mask in bits
       unsigned char RsvdFieldPosition;           //bit position of lsb of reserved mask
       unsigned char DirectColorModeInfo;         //direct color mode attributes
       unsigned int  PhysBasePtr;                 //physical address for flat memory frame buffer
       unsigned int  OffScreenMemOffset;          //pointer to start of off screen memory
       unsigned short OffScreenMemSize;           //amount of off screen memory in 1k units
       char Reserved2[206];                       //remainder of ModeInfoBlock
};
#pragma pack(__pop)



