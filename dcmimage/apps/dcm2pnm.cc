/*
**
** Author: Marco Eichelberg    Created:  07-05-97
**
** Module: dcm2pnm.cc
**
** Purpose:
** Convert DICOM Images to PPM or PGM using the dcmimage library. 
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1997-05-22 13:27:14 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/apps/dcm2pnm.cc,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/
                        
#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdio.h>
#include <string.h>

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "dctk.h"
#include "dcutils.h"
#include "dcdebug.h"
#include "cmdlnarg.h"
#include "dcmimage.h"

// ********************************************

static void
usage()
{
    fprintf(stderr, 
      "dcm2pnm: Convert DICOM file to PGM or PPM\n"
      "usage: dcm2pnm [options] dcmfile-in [pnmfile-out]\n"
      "options are:\n"
      "\n"
      "input options:\n"
      "  +D       read as a dataset\n"
      "  -D       read as a fileformat or dataset (default)\n"
      "input transfer syntax: use only after +D\n"
      "  +t=      use transfer syntax recognition (default)\n"
      "  +ti      read with little-endian implicit transfer syntax\n"
      "  +te      read with little-endian explicit transfer syntax\n"
      "  +tb      read with big-endian explicit transfer syntax\n"
      "image processing options:\n"
      "  +F n     select frame n (default: 1)\n"
      "  +G       convert to grayscale if necessary\n"
      "  +C left top width height\n"
      "           clip image region\n"
      "  +a       recognize pixel aspect ratio when scaling (default)\n"
      "  -a       ignore pixel aspect ratio when scaling\n"
      "  +i       use bilinear interpolation when scaling (default)\n"
      "  -i       no interpolation when scaling\n"
      "  -S       no scaling, ignore pixel aspect ratio (default)\n"
      "  +Sxf r   scale x axis by factor r (float), compute y axis automatically\n"
      "  +Syf r   scale y axis by factor r (float), compute x axis automatically\n"
      "  +Sxv n   scale x axis to n pixels, compute y axis automatically\n"
      "  +Syv n   scale y axis to n pixels, compute x axis automatically\n"
      "VOI windowing options:\n"
      "  -W       no VOI windowing (default)\n"
      "  +Wi n    use the n-th VOI window from the image file\n"
      "  +Wl n    use the n-th VOI look up table from the image file\n"
      "  +Wm n    Compute VOI window using min-max algorithm, ignoring n extreme values\n"
      "  +Wh n    Compute VOI window using Histogram algorithm, ignoring n percent\n"
      "  +Ww r s  Compute VOI window using center r and width s\n"
      "overlay options:\n"
      "  -O       do not display overlays\n"
      "  +O n     display overlay n (0..16, 0=all, default: +O 0)\n"
      "  +mr      use overlay mode \"Replace\" (default)\n"
      "  +mt      use overlay mode \"Threshold-Replace\"\n"
      "  +mc      use overlay mode \"Complement\"\n"
      "  +mi      use overlay mode \"Region of Interest\"\n"
      "  +of r    set overlay foreground density to r (float, 0..1, default: 1)\n"
      "  +ot r    set overlay threshold density to r (float, 0..1, default: 0.5)\n"
      "output options:\n"
      "  +V       verbose mode, print image details\n"
      "  +d n     set debug level to n (0..9, default: 0)\n"
      "  -f       do not create any output (useful with +V)\n"
      "  +fb      write 8-bit binary PGM/PPM (default)\n"
      "  +fa      write 8-bit ASCII PGM/PPM\n"
      "  +fA      write 16-bit ASCII PGM/PPM\n"
  );
}

int extract_unsigned_long(unsigned long& result, int argc, char *argv[], int index)
{
  unsigned long l;
  
  if (index >= argc ) return 0;
  if (1 == sscanf(argv[index], "%lu", &l))
  {
    result = l;
    return 1;
  } else return 0;
}

int extract_double(double& result, int argc, char *argv[], int index)
{
  double d;
  
  if (index >= argc ) return 0;
  if (1 == sscanf(argv[index], "%lf", &d))
  {
    result = d;
    return 1;
  } else return 0;
}

void printEIStatus(FILE *file, EI_Status status, const char *filename=NULL)
{
  switch (status)
  {
    case EIS_Normal:
      fprintf(file, "dcm2pnm: status OK");
      break;
    case EIS_NoDataDictionary:
      fprintf(file, "dcm2pnm: error: no data dictionary");
      break;
    case EIS_InvalidDocument:
      fprintf(file, "dcm2pnm: error: invalid DICOM document");
      break;
    case EIS_MissingAttribute:
      fprintf(file, "dcm2pnm: error: missing attribute");
      break;
    case EIS_InvalidValue:
      fprintf(file, "dcm2pnm: error: invalid data value");
      break;
    case EIS_NotSupportedValue:
      fprintf(file, "dcm2pnm: error: unsupported data value");
      break;
    case EIS_MemoryFailure:
      fprintf(file, "dcm2pnm: error: out of memory");
      break;
    case EIS_InvalidImage:
      fprintf(file, "dcm2pnm: error: invalid DICOM image");
      break;
    case EIS_OtherError:
    default:
      fprintf(file, "dcm2pnm: error: unspecified");
      break;
  }
  if (filename) fprintf(file, " in file '%s'.\n", filename); else fprintf(file, ".\n");
  return;
}

// ********************************************

int main(int argc, char *argv[])
{
  int              opt_readAsDataset = 0;            /* default: fileformat or dataset */
  E_TransferSyntax opt_transferSyntax = EXS_Unknown; /* default: xfer syntax recognition */
  unsigned long    opt_Frame = 1;                    /* default: first frame */
  int              opt_ConvertToGrayscale = 0;       /* default: color or grayscale */
  int              opt_useAspectRatio = 1;           /* default: use aspect ratio for scaling */
  int              opt_useInterpolation = 1;         /* default: use interpolation for scaling */
  int              opt_useClip = 0;                  /* default: don't clip */
  unsigned long    opt_left=0, opt_top=0, opt_width=0, opt_height=0; /* clip region */
  int              opt_scaleType = 0;                /* default: no scaling */
                   /* 1 = X-factor, 2 = Y-factor, 3=X-size, 4=Y-size */
  double           opt_scale_factor = 1.0;
  unsigned long    opt_scale_size = 1;
  int              opt_windowType = 0;               /* default: no windowing */
                   /* 1=Wi, 2=Wl, 3=Wm, 4=Wh, 5=Ww */
  unsigned long    opt_windowParameter = 0;
  double           opt_windowCenter=0.0, opt_windowWidth=0.0;
  
  int              opt_Overlay[16];
  int              opt_O_used = 0;                   /* flag for +O parameter */
  int              opt_OverlayMode = 1;              /* default: replace */
                   /* 2=threshold-replace, 3=complement, 4=ROI */
                   
  double           opt_foregroundDensity = 1.0;
  double           opt_thresholdDensity  = 0.5;

  int              opt_verboseMode = 0;              /* default: be quiet */
  unsigned long    opt_debugMode   = 0;              /* default: no debug */
  int              opt_suppressOutput = 0;           /* default: create output */
  int              opt_fileType = 1;                 /* default: 8-bit binary PGM/PPM */
                   /* 2=8-bit-ASCII, 3=16-bit-ASCII */  
  char *           opt_ifname = NULL;
  char *           opt_ofname = NULL;
  
  DicomImage *oldImages[10];	/* stack of images to be deleted later */
  SetDebugLevel(( 0 ));

  prepareCmdLineArgs(argc, argv, "dcm2pnm");
    
  if (argc < 2)
  {
    usage();
    return 1;
  }

  int i, j;
  char *arg=NULL;

  for (i=0; i<10; i++) oldImages[i]=NULL;
  for (i=0; i<16; i++) opt_Overlay[i]=2; /* default: display all overlays if present */
  for (i=1; i<argc; i++)
  {
    arg = argv[i];
    if (arg[0] == '-' || arg[0] == '+')
    {
      if (strlen(arg) < 2) 
      {
        fprintf(stderr, "unknown argument: %s\n", arg);
        usage();
        return 1;
      }
      switch (arg[1])
      {
         case 'D': /* +D, -D */
           if (arg[2] == '\0')
           {
             if (arg[0] == '-') opt_readAsDataset=0; else opt_readAsDataset=1;
           } else {
             fprintf(stderr, "unknown option: %s\n", arg);
             return 1;
           }
           break;
         case 't': /* +t=, +ti, +te, +tb */ 
           if ((strlen(arg)==3)&&(arg[0]=='+'))
           {
             switch (arg[2])
             {
               case '=':
                 opt_transferSyntax = EXS_Unknown;
                 break;
               case 'i':
                 opt_transferSyntax = EXS_LittleEndianImplicit;
                 break;
               case 'e':
                 opt_transferSyntax = EXS_LittleEndianExplicit;
                 break;
               case 'b':
                 opt_transferSyntax = EXS_BigEndianExplicit;
                 break;
               default:
                 fprintf(stderr, "unknown option: %s\n", arg);
                 return 1;
             }
           } else {
             fprintf(stderr, "unknown option: %s\n", arg);
             return 1;
           }
           break;
         case 'F': /* +F n */
           if ((arg[0]=='+')&&(arg[2]=='\0')&&
               (extract_unsigned_long(opt_Frame, argc, argv, i+1)))
           {
             if (opt_Frame==0)
             {
               fprintf(stderr, "illegal frame number in %s option\n", arg);
               return 1;
             } else i++;
           } else {
             fprintf(stderr, "unknown option: %s\n", arg);
             return 1;
           }
           break;
         case 'G': /* +G */
           if ((arg[0]=='+')&&(arg[2]=='\0'))
           {
             opt_ConvertToGrayscale = 1;
           } else {
             fprintf(stderr, "unknown option: %s\n", arg);
             return 1;
           }
           break;
         case 'C': /* +C left top width height */
           if ((arg[0]=='+')&&(arg[2]=='\0')
               && (extract_unsigned_long(opt_left, argc, argv, i+1))
               && (extract_unsigned_long(opt_top, argc, argv, i+2))
               && (extract_unsigned_long(opt_width, argc, argv, i+3))
               && (extract_unsigned_long(opt_height, argc, argv, i+4)))
           {
             if ((opt_width > 0) && (opt_height > 0))
             {
               opt_useClip = 1;
               i += 4;
             } else {
               fprintf(stderr, "clip width and height must be larger than 0: %s\n", arg);
               return 1;
             }
           } else {
             fprintf(stderr, "unknown option: %s\n", arg);
             return 1;
           }
           break;
         case 'a': /* -a, +a */
           if (arg[2] == '\0')
           {
             if (arg[0] == '-') opt_useAspectRatio=0; else opt_useAspectRatio=1;
           } else {
             fprintf(stderr, "unknown option: %s\n", arg);
             return 1;
           }
           break;
         case 'i': /* -i, +i */
           if (arg[2] == '\0')
           {
             if (arg[0] == '-') opt_useInterpolation=0; else opt_useInterpolation=1;
           } else {
             fprintf(stderr, "unknown option: %s\n", arg);
             return 1;
           }
           break;
         case 'S': /* -S, +Sxf, +Syf, +Sxv, +Syv */
           if (strcmp(arg,"-S")==0)
           {
             opt_scaleType=0;
           } else if (strcmp(arg,"+Sxf")==0)
           {
              if ((extract_double(opt_scale_factor, argc, argv, i+1))&&(opt_scale_factor>0.0))
              {
                opt_scaleType=1;
                i++;
              } else {
                 fprintf(stderr, "illegal scaling factor in %s option\n", arg);
                 return 1;
              }
           } else if (strcmp(arg,"+Syf")==0)
           {
              if ((extract_double(opt_scale_factor, argc, argv, i+1))&&(opt_scale_factor>0.0))
              {
                opt_scaleType=2;
                i++;
              } else {
                 fprintf(stderr, "illegal scaling factor in %s option\n", arg);
                 return 1;
              }
           } else if (strcmp(arg,"+Sxv")==0)
           {
              if ((extract_unsigned_long(opt_scale_size, argc, argv, i+1))&&(opt_scale_size>0))
              {
                opt_scaleType=3;
                i++;
              } else {
                 fprintf(stderr, "illegal scale size in %s option\n", arg);
                 return 1;
              }
           } else if (strcmp(arg,"+Syv")==0)
           {
              if ((extract_unsigned_long(opt_scale_size, argc, argv, i+1))&&(opt_scale_size>0))
              {
                opt_scaleType=4;
                i++;
              } else {
                 fprintf(stderr, "illegal scale size in %s option\n", arg);
                 return 1;
              }
           } else {
             fprintf(stderr, "unknown option: %s\n", arg);
             return 1;
           }
           break;
         case 'W': /* -W, +Wi n, +Wl n, +Wm n, +Wh n, +Ww r s */
           if ((arg[2]=='\0')&&(arg[0]=='-'))
           {
             opt_windowType = 0;
           } else if ((strlen(arg)==3)&&(arg[0]=='+'))
           {
             switch (arg[2])
             {
               case 'i':
                 if ((extract_unsigned_long(opt_windowParameter, argc, argv, i+1))
                    && (opt_windowParameter>0))
                 {
                   opt_windowType = 1;
                   i++;
                 } else {
                   fprintf(stderr, "illegal VOI window number in %s option\n", arg);
                   return 1;
                 }
                 break;
               case 'l':
                 if ((extract_unsigned_long(opt_windowParameter, argc, argv, i+1))
                    && (opt_windowParameter>0))
                 {
                   opt_windowType = 2;
                   i++;
                 } else {
                   fprintf(stderr, "illegal VOI LUT number in %s option\n", arg);
                   return 1;
                 }
                 break;
               case 'm':
                 if (extract_unsigned_long(opt_windowParameter, argc, argv, i+1))                    
                 {
                   opt_windowType = 3;
                   i++;
                 } else {
                   fprintf(stderr, "illegal parameter in %s option\n", arg);
                   return 1;
                 }
                 break;
               case 'h':
                 if ((extract_unsigned_long(opt_windowParameter, argc, argv, i+1))
                    && (opt_windowParameter <= 100))
                 {
                   opt_windowType = 4;
                   i++;
                 } else {
                   fprintf(stderr, "illegal parameter in %s option\n", arg);
                   return 1;
                 }
                 break;
               case 'w':
                 if ((extract_double(opt_windowCenter, argc, argv, i+1))
                    && (extract_double(opt_windowWidth, argc, argv, i+2))
                    && (opt_windowWidth>0.0))
                 {
                   opt_windowType = 5;
                   i += 2;
                 } else {
                   fprintf(stderr, "illegal window center/width in %s option\n", arg);
                   return 1;
                 }
                 break;
               default:
                 fprintf(stderr, "unknown option: %s\n", arg);
                 return 1;
             }
           } else {
             fprintf(stderr, "unknown option: %s\n", arg);
             return 1;
           }
           break;
         case 'O': /* -O, +O n */
           if (arg[2] == '\0')
           {
             if (arg[0] == '-')
             {
               opt_O_used = 1;
               for (j=0; j<16; j++) opt_Overlay[j]=0;
             } else {
               unsigned long l;
               if ((extract_unsigned_long(l, argc, argv, i+1))
                  && (l <= 16))
               {
                 if (!opt_O_used)
                 {
                   for (j=0; j<16; j++) opt_Overlay[j]=0; 
                   opt_O_used=1;
                 }
                 if (l>0) opt_Overlay[l-1]=1; else
                 {
                   for (j=0; j<16; j++) opt_Overlay[j]=2; 
                 }
                 i++;
               } else {
                 fprintf(stderr, "illegal overlay number in %s option\n", arg);
                 return 1;
               }
             }
           } else {
             fprintf(stderr, "unknown option: %s\n", arg);
             return 1;
           }
           break;
         case 'm': /* +mr, +mt, +mc, +mi */
           if ((strlen(arg)==3)&&(arg[0]=='+'))
           {
             switch (arg[2])
             {
               case 'r':
                 opt_OverlayMode = 1;
                 break;
               case 't':
                 opt_OverlayMode = 2;
                 break;
               case 'c':
                 opt_OverlayMode = 3;
                 break;
               case 'i':
                 opt_OverlayMode = 4;
                 break;
               default:
                 fprintf(stderr, "unknown option: %s\n", arg);
                 return 1;
             }
           } else {
             fprintf(stderr, "unknown option: %s\n", arg);
             return 1;
           }
           break;
         case 'o': /* +of n, +ot n */
           if ((strlen(arg)==3)&&(arg[0]=='+'))
           {
             switch (arg[2])
             {
               case 'f':
                 if ((extract_double(opt_foregroundDensity, argc, argv, i+1))
                    && (opt_foregroundDensity >= 0.0)
                    && (opt_foregroundDensity <= 1.0))
                 {
                   i++;
                 } else {
                   fprintf(stderr, "illegal foreground density in %s option (must be 0..1)\n", arg);
                   return 1;
                 }
                 break;
               case 't':
                 if ((extract_double(opt_thresholdDensity, argc, argv, i+1))
                    && (opt_thresholdDensity >= 0.0)
                    && (opt_thresholdDensity <= 1.0))
                 {
                   i++;
                 } else {
                   fprintf(stderr, "illegal threshold density in %s option (must be 0..1)\n", arg);
                   return 1;
                 }
                 break;
               default:
                 fprintf(stderr, "unknown option: %s\n", arg);
                 return 1;
             }
           } else {
             fprintf(stderr, "unknown option: %s\n", arg);
             return 1;
           }
           break;
         case 'V': /* +V */
           if ((arg[0]=='+')&&(arg[2]=='\0'))
           {
             opt_verboseMode = 1;
           } else {
             fprintf(stderr, "unknown option: %s\n", arg);
             return 1;
           }
           break;
         case 'd': /* +d n */
           if ((arg[0]=='+')
              &&(arg[2]=='\0')
              && (extract_unsigned_long(opt_debugMode, argc, argv, i+1))
              && (opt_debugMode <= 9)) 
           {
             i++;
           } else {
             fprintf(stderr, "unknown option: %s\n", arg);
             return 1;
           }
           break;
         case 'f': /* -f, +fb, +fa, +fA */
           if ((arg[0]=='-')&&(arg[2]=='\0'))
           {
             opt_suppressOutput = 1;
           }
           else if ((strlen(arg)==3)&&(arg[0]=='+'))
           {
             switch (arg[2])
             {
               case 'b':
                 opt_fileType = 1;
                 break;
               case 'a':
                 opt_fileType = 2;
                 break;
               case 'A':
                 opt_fileType = 3;
                 break;
               default:
                 fprintf(stderr, "unknown option: %s\n", arg);
                 return 1;
             }
           } else {
             fprintf(stderr, "unknown option: %s\n", arg);
             return 1;
           }
           break;
    
         default:
           fprintf(stderr, "unknown option: %s\n", arg);
           return 1;
       } /* end switch */
       
     } else if ( opt_ifname == NULL )
     {
        opt_ifname = arg;
     } else if ( opt_ofname == NULL )
     {
       opt_ofname = arg;
     } else {
       fprintf(stderr, "too many arguments: %s\n", arg);
       usage();
       return 1;
     }
  }

  if ( opt_ifname == NULL )
  {
    fprintf(stderr, "missing input file\n");
    return 1;
  }


  /* make sure data dictionary is loaded */
  if (!dcmDataDict.isDictionaryLoaded()) {
    fprintf(stderr, "Warning: no data dictionary loaded, check environment variable: %s\n",
      DCM_DICT_ENVIRONMENT_VARIABLE);
  }
    
  SetDebugLevel(( opt_debugMode ));

  if (opt_debugMode > 0)
  {
    fprintf(stderr, "reading DICOM file: %s\n", opt_ifname);
  }

  DcmFileStream myin(opt_ifname, DCM_ReadMode);
  if ( myin.GetError() != EC_Normal )
  {
    fprintf(stderr, "dcm2pnm: cannot open DICOM file: %s\n", opt_ifname);
    return 1;
  }
 
  DcmObject * dfile = NULL;
  if (opt_readAsDataset) dfile = new DcmDataset();
  else  dfile = new DcmFileFormat();
 
  dfile->transferInit();
  dfile->read(myin, opt_transferSyntax, EGL_withoutGL);
  dfile->transferEnd();
 
  if (dfile->error() != EC_Normal)
  {
    fprintf(stderr, "dcm2pnm: error: %s: reading file: %s\n", 
      dcmErrorConditionToString(dfile->error()), opt_ifname);
    return 1;
  }
  
  if (opt_debugMode > 0)
  {
    fprintf(stderr, "preparing pixel data.\n");
  }

  DicomImage *di = new DicomImage(dfile); /* warning: dfile is Dataset or Fileformat! */
  
  if (di == NULL)
  {
    fprintf(stderr, "dcm2pnm: error: out of memory.\n");
    return 1;
  }

  if (di->getStatus() != EIS_Normal)
  {
    printEIStatus(stderr, di->getStatus());
    return 1;
  }

  if (opt_verboseMode)
  {
    
    /* dump image parameters */
    if (opt_debugMode > 0)
    {
      fprintf(stderr, "dumping image parameters.\n");
    }

    double minVal=0.0;
    double maxVal=0.0;
    char *colorModel;
    char *SOPClassUID=NULL;
    char *SOPInstanceUID=NULL;
    const char *SOPClassText=NULL;
    
    di->getMinMaxValues(minVal, maxVal);
    switch (di->getPhotometricInterpretation())
    {
      case EPI_Monochrome1:
        colorModel = "Monochrome 1";
        break;
      case EPI_Monochrome2:
        colorModel = "Monochrome 2";
        break;
      case EPI_PaletteColor:
        colorModel = "Palette Color";
        break;
      case EPI_RGB:
        colorModel = "RGB Color";
        break;
      case EPI_HSV:
        colorModel = "HSV Color";
        break;
      case EPI_ARGB:
        colorModel = "ARGB Color";
        break;
      case EPI_CMYK:
        colorModel = "CMYK Color";
        break; 
      case EPI_Unknown:
      default:
        colorModel = "unknown";
        break;
    }
    
    if (opt_readAsDataset)
    {
      getSingleValue(dfile, DCM_SOPClassUID, SOPClassUID);
      getSingleValue(dfile, DCM_SOPInstanceUID, SOPInstanceUID);
    } else {
      getSingleValue(((DcmFileFormat *)dfile)->getDataset(), DCM_SOPClassUID, SOPClassUID);
      getSingleValue(((DcmFileFormat *)dfile)->getDataset(), DCM_SOPInstanceUID, SOPInstanceUID);
    }
    if (SOPInstanceUID == NULL) SOPInstanceUID = "not present";
    if (SOPClassUID == NULL) SOPClassText="not present";
    else SOPClassText=dcmFindNameOfUID(SOPClassUID);
    if (SOPClassText==NULL) SOPClassText=SOPClassUID;
                
    fprintf(stderr,
      "filename            : %s\n"
      "SOP class           : %s\n"
      "SOP instance UID    : %s\n\n"
      "columns x rows      : %lu x %lu\n"
      "bits per sample     : %u\n"
      "color model         : %s\n"
      "pixel aspect ratio  : %.2f\n"
      "number of frames    : %lu\n\n"
      "maximum pixel value : %.0f\n"
      "minimum pixel value : %.0f\n\n"
      "VOI windows in file : %lu\n"
      "VOI LUTs in file    : %lu\n"
      "Overlays in file    : %u\n\n",
        opt_ifname,
        SOPClassText,
        SOPInstanceUID,
        di->getWidth(), di->getHeight(),
        di->getDepth(),
        colorModel,
        di->getHeightWidthRatio(),
        di->getFrameCount(),
        maxVal,
        minVal,
        di->getWindowCount(),
        di->getVoiLutCount(),
        di->getOverlayCount()
    );		
  }
  
  if (opt_suppressOutput) return 0;

  /* select frame */
  if (opt_Frame > di->getFrameCount())
  {
    fprintf(stderr,"dcm2pnm: error: cannot select frame no. %lu, only %lu frame(s) in file.\n",
      opt_Frame, di->getFrameCount());
    return 1;
  }
  
  /* convert to grayscale if necessary */
  if ((opt_ConvertToGrayscale)&&(! di->isMonochrome()))
  { 
     if (opt_debugMode > 0)
     {
       fprintf(stderr, "converting image to grayscale.\n");
     }

     DicomImage *newimage = di->createMonochromeImage();
     if (newimage==NULL)
     {
       fprintf(stderr, "dcm2pnm: error: out of memory.\n");
       return 1;
     } else if (newimage->getStatus() != EIS_Normal)
     {
       printEIStatus(stderr, newimage->getStatus());
       return 1;
     } 
     else
     {
       i=0;
       while ((i<10)&&(oldImages[i] != NULL)) i++;
       if (oldImages[i] == NULL) oldImages[i]=di; 
       /* we cannot delete the old image here, because something with overlays will go wrong... */ 
       di = newimage;
     }
  }

  /* process overlay parameters */
  EM_Overlay overlayMode;
  switch (opt_OverlayMode)
  {
    case 2:
      overlayMode=EMO_ThresholdReplace;
      break;
    case 3:
      overlayMode=EMO_Complement;
      break;
    case 4:
      overlayMode=EMO_RegionOfInterest;
      break;
    case 1:
    default:
      overlayMode=EMO_Replace;
      break;
      
  }
  di->hideAllOverlays();
  for (unsigned long k=0; k<16; k++)
  {
    if (opt_Overlay[k])
    {
      if ((opt_Overlay[k]==1)||(k < di->getOverlayCount()))
      {
        if (opt_debugMode > 0)
        {
          fprintf(stderr, "activating overlay plane %lu\n", k+1);
        }
        if (! di->showOverlay(k, overlayMode, opt_foregroundDensity, opt_thresholdDensity))
        {
          fprintf(stderr,"dcm2pnm: warning: cannot display overlay plane %lu\n",k+1);
        }
      }
    }
  }
  
  /* process VOI parameters */
  switch (opt_windowType)  
  {
    case 1: /* use the n-th VOI window from the image file */
      if ((opt_windowParameter<1)||(opt_windowParameter>di->getWindowCount()))
      {
        fprintf(stderr,"dcm2pnm: error: cannot select VOI window no. %lu, only %lu window(s) in file.\n",
          opt_windowParameter, di->getWindowCount());
        return 1;
      }
      if (opt_debugMode > 0)
      {
        fprintf(stderr, "activating VOI window %lu\n", opt_windowParameter);
      }
      if (! di->setWindow(opt_windowParameter-1))
      {
        fprintf(stderr,"dcm2pnm: warning: cannot select VOI window no. %lu\n", opt_windowParameter);
      }
      break;
    case 2: /* use the n-th VOI look up table from the image file */
      if ((opt_windowParameter<1)||(opt_windowParameter>di->getVoiLutCount()))
      {
        fprintf(stderr,"dcm2pnm: error: cannot select VOI LUT no. %lu, only %lu LUT(s) in file.\n",
          opt_windowParameter, di->getVoiLutCount());
        return 1;
      }
      if (opt_debugMode > 0)
      {
        fprintf(stderr, "activating VOI LUT %lu\n", opt_windowParameter);
      }
      if (! di->setVoiLut(opt_windowParameter-1))
      {
        fprintf(stderr,"dcm2pnm: warning: cannot select VOI LUT no. %lu\n", opt_windowParameter);
      }
      break;
    case 3: /* Compute VOI window using min-max algorithm, ignoring n extreme values */
      if (opt_debugMode > 0)
      {
        fprintf(stderr, "activating VOI window min-max algorithm, ignoring %lu extreme values\n", opt_windowParameter);
      }
      if (! di->setMinMaxWindow(opt_windowParameter))
      {
        fprintf(stderr,"dcm2pnm: warning: cannot compute min/max VOI window\n");
      }
      break;     
    case 4: /* Compute VOI window using Histogram algorithm, ignoring n percent */
      if (opt_debugMode > 0)
      {
        fprintf(stderr, "activating VOI window histogram algorithm, ignoring %lu%%\n", opt_windowParameter);
      }
      if (! di->setHistogramWindow(((double)opt_windowParameter)/100.0))
      {
        fprintf(stderr,"dcm2pnm: warning: cannot compute histogram VOI window\n");
      }
      break;
    case 5: /* Compute VOI window using center r and width s */
      if (opt_debugMode > 0)
      {
        fprintf(stderr, "activating VOI window center=%f, width=%f\n", opt_windowCenter, opt_windowWidth);
      }
      if (! di->setWindow(opt_windowCenter, opt_windowWidth))
      {
        fprintf(stderr,"dcm2pnm: warning: cannot set VOI window center=%f width=%f\n",opt_windowCenter, opt_windowWidth);
      }
      break;
    default: /* no VOI windowing */
      if (di->isMonochrome())
      {
        if (opt_debugMode > 0)
        {
          fprintf(stderr, "disabling VOI window computation\n");
        }
        if (! di->setWindow(0, 0))  /* window width 0 is defined as "ignore VOI window"! */
        {
          fprintf(stderr,"dcm2pnm: warning: cannot ignore VOI window\n");
        }
      }
      break;
  }
    
  /* perform clipping */
  if (opt_useClip)
  { 
     if (opt_debugMode > 0)
     {
       fprintf(stderr, "clipping image to (%lu,%lu,%lu,%lu).\n", 
         opt_left, opt_top, opt_width, opt_height);
     }
     DicomImage *newimage = di->createClippedImage(opt_left, opt_top, opt_width, opt_height);
     if (newimage==NULL)
     {
       fprintf(stderr, "dcm2pnm: error: clipping to (%lu,%lu,%lu,%lu) failed.\n", 
         opt_left, opt_top, opt_width, opt_height);
       return 1;
     } else if (newimage->getStatus() != EIS_Normal)
     {
       printEIStatus(stderr, newimage->getStatus());
       return 1;
     } 
     else
     {
       i=0;
       while ((i<10)&&(oldImages[i] != NULL)) i++;
       if (oldImages[i] == NULL) oldImages[i]=di; 
       /* we cannot delete the old image here, because something with overlays will go wrong... */ 
       di = newimage;
     }
  }

  
  /* perform scaling */
  if (opt_scaleType > 0)
  {
    DicomImage *newimage;
    switch (opt_scaleType)
    {
      case 1:
        if (opt_debugMode > 0)
        {
          fprintf(stderr, "scaling image, X factor=%f, Interpolation=%s, Aspect Ratio=%s\n",
            opt_scale_factor, (opt_useInterpolation ? "yes" : "no"),  (opt_useAspectRatio ? "yes" : "no"));
        }
        newimage = di->createScaledImage(opt_scale_factor, 0.0, opt_useInterpolation, opt_useAspectRatio);
        break;
      case 2:
        if (opt_debugMode > 0)
        {
          fprintf(stderr, "scaling image, Y factor=%f, Interpolation=%s, Aspect Ratio=%s\n",
            opt_scale_factor, (opt_useInterpolation ? "yes" : "no"),  (opt_useAspectRatio ? "yes" : "no"));
        }
        newimage = di->createScaledImage(0.0, opt_scale_factor, opt_useInterpolation, opt_useAspectRatio);
        break;
      case 3:
        if (opt_debugMode > 0)
        {
          fprintf(stderr, "scaling image, X size=%lu, Interpolation=%s, Aspect Ratio=%s\n",
            opt_scale_size, (opt_useInterpolation ? "yes" : "no"),  (opt_useAspectRatio ? "yes" : "no"));
        }
        newimage = di->createScaledImage(opt_scale_size, 0, opt_useInterpolation, opt_useAspectRatio);
        break;
      case 4:
        if (opt_debugMode > 0)
        {
          fprintf(stderr, "scaling image, Y size=%lu, Interpolation=%s, Aspect Ratio=%s\n",
            opt_scale_size, (opt_useInterpolation ? "yes" : "no"),  (opt_useAspectRatio ? "yes" : "no"));
        }
        newimage = di->createScaledImage(0, opt_scale_size, opt_useInterpolation, opt_useAspectRatio);
        break;
      default:
        if (opt_debugMode > 0)
        {
          fprintf(stderr, "internal error: unknown scaling type\n");
        }
        newimage = NULL;
        break;
    }
    if (newimage==NULL)
    {
      fprintf(stderr, "dcm2pnm: error: out of memory.\n");
      return 1;
    } else if (newimage->getStatus() != EIS_Normal)
    {
      printEIStatus(stderr, newimage->getStatus());
      return 1;
    } 
    else
    {
       i=0;
       while ((i<10)&&(oldImages[i] != NULL)) i++;
       if (oldImages[i] == NULL) oldImages[i]=di; 
       /* we cannot delete the old image here, because something with overlays will go wrong... */ 
       di = newimage;
    }
  }
  
  
  if (opt_debugMode > 0)
  {
     fprintf(stderr, "writing PPM/PGM file to %s\n",((opt_ofname)? opt_ofname : "stderr"));
  }
  FILE *ofile = stdout;
  if (opt_ofname)
  {
    ofile = fopen(opt_ofname, "w");
    if (ofile==NULL)   
    {
      fprintf(stderr, "dcm2pnm: error: cannot create file %s\n",opt_ofname);
      return 1;
    }
  }
  
  /* finally create PPM/PGM file */
  switch (opt_fileType)
  {
    case 2:
      di->writePPM(ofile, 8, opt_Frame-1);
      break;
    case 3:
      di->writePPM(ofile, 16, opt_Frame-1);
      break;
    case 1:
    default:
      di->writeRawPPM(ofile, 8, opt_Frame-1);
      break;
  }

  if (opt_ofname) fclose(ofile);

  /* done, now cleanup. */
  if (opt_debugMode > 0)
  {
     fprintf(stderr, "cleaning up memory.\n");
  }
  for (i=0; i<10; i++) if (oldImages[i]) delete oldImages[i];

  return 0;
}


/*
** CVS/RCS Log:
** $Log: dcm2pnm.cc,v $
** Revision 1.3  1997-05-22 13:27:14  hewett
** Modified the test for presence of a data dictionary to use the
** method DcmDataDictionary::isDictionaryLoaded().
**
** Revision 1.2  1997/05/16 08:33:02  andreas
** - Revised handling of GroupLength elements and support of
**   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
**   got additional enumeration values (for a description see dctypes.h).
**   addGroupLength and removeGroupLength methods are replaced by
**   computeGroupLengthAndPadding. To support Padding, the parameters of
**   element and sequence write functions changed.
**
** Revision 1.1  1997/05/13 13:49:42  meichel
** Added new application dcm2pnm.
** dcm2pnm allows to convert DICOM images to the widely used
** PPM/PGM general purpose image format. dcm2pnm gives access to most
** functionality offered by the dcmimage library.
**
**
*/
