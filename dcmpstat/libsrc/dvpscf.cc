/*
 *
 *  Copyright (C) 1998-99, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmpstat
 *
 *  Author:  Joerg Riesmeier, Marco Eichelberg
 *
 *  Purpose: DVConfiguration
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-03 14:13:58 $
 *  CVS/RCS Revision: $Revision: 1.19 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvpscf.h"      /* for DVConfiguration */
#include "dvpsconf.h"    /* for class DVPSConfig */
#include "ofconsol.h"    /* for CERR, COUT */
#include <stdio.h>
#include <ctype.h>       /* for toupper() */

/* default AETitle for the Presentation State viewer */
#define PSTAT_AETITLE "DCMPSTAT"
/* default path for database folder */
#define PSTAT_DBFOLDER "."
/* default path for LUT folder */
#define PSTAT_LUTFOLDER "."
/* default path for spool folder */
#define PSTAT_SPOOLFOLDER "."

#define PSTAT_DEFAULT_ILLUMINATION 2000
#define PSTAT_DEFAULT_REFLECTION 10

/* keywords for configuration file */

#define L0_AETITLE                      "AETITLE"
#define L0_ANNOTATION                   "ANNOTATION"
#define L0_ALWAYSDELETETERMINATEJOBS    "ALWAYSDELETETERMINATEJOBS"
#define L0_BITPRESERVINGMODE            "BITPRESERVINGMODE"
#define L0_BORDERDENSITY                "BORDERDENSITY"
#define L0_CENTER                       "CENTER"
#define L0_CHARACTERISTICS              "CHARACTERISTICS"
#define L0_DEFAULTILLUMINATION          "DEFAULTILLUMINATION"
#define L0_DEFAULTREFLECTION            "DEFAULTREFLECTION"
#define L0_DELETEPRINTJOBS              "DELETEPRINTJOBS"
#define L0_DESCRIPTION                  "DESCRIPTION"
#define L0_DETAILEDLOG                  "DETAILEDLOG"
#define L0_DIRECTORY                    "DIRECTORY"
#define L0_DISABLENEWVRS                "DISABLENEWVRS"
#define L0_DISPLAYFORMAT                "DISPLAYFORMAT"
#define L0_DUMP                         "DUMP"
#define L0_EMPTYIMAGEDENSITY            "EMPTYIMAGEDENSITY"
#define L0_FILENAME                     "FILENAME"
#define L0_FILMSIZEID                   "FILMSIZEID"
#define L0_HOSTNAME                     "HOSTNAME"
#define L0_IMPLICITONLY                 "IMPLICITONLY"
#define L0_LOGDIRECTORY                 "LOGDIRECTORY"
#define L0_MAGNIFICATIONTYPE            "MAGNIFICATIONTYPE"
#define L0_MAXCOLUMNS                   "MAXCOLUMNS"
#define L0_MAXDENSITY                   "MAXDENSITY"
#define L0_MAXPDU                       "MAXPDU"
#define L0_MAXPRINTRESOLUTION           "MAXPRINTRESOLUTION"
#define L0_MAXROWS                      "MAXROWS"
#define L0_MEDIUMTYPE                   "MEDIUMTYPE"
#define L0_MINDENSITY                   "MINDENSITY"
#define L0_MINPRINTRESOLUTION           "MINPRINTRESOLUTION"
#define L0_MODALITY                     "MODALITY"
#define L0_PORT                         "PORT"
#define L0_PRESENTATIONLUTINFILMSESSION "PRESENTATIONLUTINFILMSESSION"
#define L0_PRESENTATIONLUTMATCHREQUIRED "PRESENTATIONLUTMATCHREQUIRED"
#define L0_PRESENTATIONLUTPREFERSCPRENDERING "PRESENTATIONLUTPREFERSCPRENDERING"
#define L0_PREVIEW                      "PREVIEWSIZE"
#define L0_RECEIVER                     "RECEIVER"
#define L0_RESOLUTION                   "RESOLUTION"
#define L0_RESOLUTIONID                 "RESOLUTIONID"
#define L0_SCREENSIZE                   "SCREENSIZE"
#define L0_SENDER                       "SENDER"
#define L0_SESSIONLABELANNOTATION       "SESSIONLABELANNOTATION"
#define L0_SLEEP                        "SLEEP"
#define L0_SMOOTHINGTYPE                "SMOOTHINGTYPE"
#define L0_SPOOLER                      "SPOOLER"
#define L0_SUPPORTS12BIT                "SUPPORTS12BIT"
#define L0_SUPPORTSDECIMATECROP         "SUPPORTSDECIMATECROP"
#define L0_SUPPORTSIMAGESIZE            "SUPPORTSIMAGESIZE"
#define L0_SUPPORTSPRESENTATIONLUT      "SUPPORTSPRESENTATIONLUT"
#define L0_SUPPORTSTRIM                 "SUPPORTSTRIM"
#define L0_TYPE                         "TYPE"
#define L0_WIDTH                        "WIDTH"
#define L1_DATABASE                     "DATABASE"
#define L1_GUI                          "GUI"
#define L1_LUT                          "LUT"
#define L1_MONITOR                      "MONITOR"
#define L1_NETWORK                      "NETWORK"
#define L1_PRINT                        "PRINT"
#define L2_COMMUNICATION                "COMMUNICATION"
#define L2_GENERAL                      "GENERAL"
#define L2_HIGHRESOLUTIONGRAPHICS       "HIGHRESOLUTIONGRAPHICS"
#define L2_LUT                          "LUT"
#define L2_VOI                          "VOI"

/* --------------- static helper functions --------------- */

#ifdef DEBUG
static DVPSPeerType getConfigTargetType(const char *val, ostream *logstream)
#else
static DVPSPeerType getConfigTargetType(const char *val, ostream * /* logstream */)
#endif
{
  DVPSPeerType result = DVPSE_storage; /* default */

  if (val==NULL) return result;
  OFString pstring(val);
  OFString ostring;
  size_t len = pstring.length();
  char c;
  for (size_t i=0; i<len; i++)
  {
    c = pstring[i];
    if ((c>='a') && (c <= 'z')) ostring += (char)(toupper(c));
    else if ((c>='A') && (c <= 'Z')) ostring += c;
    else if ((c>='0') && (c <= '9')) ostring += c;
    else if (c=='_')  ostring += c;
  }
  if (ostring=="PRINTER")  result=DVPSE_print; else
  if (ostring=="STORAGE")  result=DVPSE_storage; else
  {
#ifdef DEBUG
    *logstream << "warning: unsupported peer type in config file: '" << val << "', ignoring." << endl;
#endif
  }
  return result;
}

static Uint32 countValues(const char *str)
{
  if (str)
  {
    Uint32 result = 0;
    if (*str) result++;
    char c;
    while ((c = *str++)) if (c == '\\') result++;
    return result;
  }
  return 0;
}

static void copyValue(const char *str, Uint32 idx, OFString& target)
{
  target.clear();
  if (str)
  {
    char c = '\\';
    while (idx)
    {
      c = *str++;
      if (c == 0) idx=0; else if (c == '\\') idx--;
    }
    if (c=='\\')
    {
      const char *endstr = str;
      while ((*endstr) && (*endstr != '\\')) endstr++;
      target.assign(str,(endstr-str));
    }
  }
  return;
}

DVConfiguration::DVConfiguration(const char *config_file)
: logstream(&CERR)
, pConfig(NULL)
{
  if (config_file)
  {
      FILE *cfgfile = fopen(config_file, "rb");
      if (cfgfile)
      {
          pConfig = new DVPSConfig(cfgfile);
          fclose(cfgfile);
      }
  }
}


DVConfiguration::~DVConfiguration()
{
  if (pConfig) delete pConfig;
}


Uint32 DVConfiguration::getNumberOfTargets(DVPSPeerType peerType)
{
  Uint32 result = 0;
  DVPSPeerType currentType;

  if (pConfig)
  {
    pConfig->set_section(2, L2_COMMUNICATION);
    if (pConfig->section_valid(2))
    {
       pConfig->first_section(1);
       while (pConfig->section_valid(1))
       {
         currentType = getConfigTargetType(pConfig->get_entry(L0_TYPE), logstream);
         switch (peerType)
         {
           case DVPSE_storage:
             if (currentType==DVPSE_storage) result++;
             break;
           case DVPSE_print:
             if (currentType==DVPSE_print) result++;
             break;
           case DVPSE_any:
             result++;
             break;
         }
         pConfig->next_section(1);
       }
    }
  }
  return result;
}


const char *DVConfiguration::getTargetID(Uint32 idx, DVPSPeerType peerType)
{
  const char *result=NULL;
  DVPSPeerType currentType;
  OFBool found = OFFalse;

  if (pConfig)
  {
    pConfig->set_section(2, L2_COMMUNICATION);
    if (pConfig->section_valid(2))
    {
       pConfig->first_section(1);
       while ((! found)&&(pConfig->section_valid(1)))
       {
         currentType = getConfigTargetType(pConfig->get_entry(L0_TYPE), logstream);
         switch (peerType)
         {
           case DVPSE_storage:
             if (currentType==DVPSE_storage)
             {
             	if (idx==0) found=OFTrue; else idx--;
             }
             break;
           case DVPSE_print:
             if (currentType==DVPSE_print)
             {
             	if (idx==0) found=OFTrue; else idx--;
             }
             break;
           case DVPSE_any:
             if (idx==0) found=OFTrue; else idx--;
             break;
         }
         if (!found) pConfig->next_section(1);
       }
       if (pConfig->section_valid(1)) result = pConfig->get_keyword(1);
    }
  }
  return result;
}


const char *DVConfiguration::getConfigEntry(const char *l2_key, const char *l1_key, const char *l0_key)
{
  const char *result=NULL;
  if (l2_key && l1_key && l0_key && pConfig)
  {
    pConfig->select_section(l1_key, l2_key);
    if (pConfig->section_valid(1)) result = pConfig->get_entry(l0_key);
  }
  return result;
}

OFBool DVConfiguration::getConfigBoolEntry(const char *l2_key, const char *l1_key, const char *l0_key, OFBool deflt)
{
  OFBool result=deflt;
  if (l2_key && l1_key && l0_key && pConfig)
  {
    pConfig->select_section(l1_key, l2_key);
    if (pConfig->section_valid(1))
    {
      pConfig->set_section(0,l0_key);
      result = pConfig->get_bool_value(deflt);
    }
  }
  return result;
}

const char *DVConfiguration::getTargetDescription(const char *targetID)
{
  return getConfigEntry(L2_COMMUNICATION, targetID, L0_DESCRIPTION);
}

const char *DVConfiguration::getTargetHostname(const char *targetID)
{
  return getConfigEntry(L2_COMMUNICATION, targetID, L0_HOSTNAME);
}

unsigned short DVConfiguration::getTargetPort(const char *targetID)
{
  const char *c = getConfigEntry(L2_COMMUNICATION, targetID, L0_PORT);
  unsigned short result = 0;
  if (c)
  {
    if (1 != sscanf(c, "%hu", &result)) result=0;
  }
  return result;
}

const char *DVConfiguration::getTargetAETitle(const char *targetID)
{
  return getConfigEntry(L2_COMMUNICATION, targetID, L0_AETITLE);
}

unsigned long DVConfiguration::getTargetMaxPDU(const char *targetID)
{
  const char *c = getConfigEntry(L2_COMMUNICATION, targetID, L0_MAXPDU);
  unsigned long result = 0;
  if (c)
  {
    if (1 != sscanf(c, "%lu", &result)) result=0;
  }
  return result;
}

OFBool DVConfiguration::getTargetImplicitOnly(const char *targetID)
{
  return getConfigBoolEntry(L2_COMMUNICATION, targetID, L0_IMPLICITONLY, OFFalse);
}

OFBool DVConfiguration::getTargetDisableNewVRs(const char *targetID)
{
  return getConfigBoolEntry(L2_COMMUNICATION, targetID, L0_DISABLENEWVRS, OFFalse);
}

DVPSPeerType DVConfiguration::getTargetType(const char *targetID)
{
  return getConfigTargetType(getConfigEntry(L2_COMMUNICATION, targetID, L0_TYPE), logstream);
}

const char *DVConfiguration::getNetworkAETitle()
{
  const char *result = getConfigEntry(L2_GENERAL, L1_NETWORK, L0_AETITLE);
  if (result==NULL) result = PSTAT_AETITLE;
  return result;
}


OFBool DVConfiguration::getNetworkImplicitVROnly()
{
  return getConfigBoolEntry(L2_GENERAL, L1_NETWORK, L0_IMPLICITONLY, OFFalse);
}

OFBool DVConfiguration::getNetworkDisableNewVRs()
{
  return getConfigBoolEntry(L2_GENERAL, L1_NETWORK, L0_DISABLENEWVRS, OFFalse);
}

OFBool DVConfiguration::getNetworkBitPreserving()
{
  return getConfigBoolEntry(L2_GENERAL, L1_NETWORK, L0_BITPRESERVINGMODE, OFFalse);
}

unsigned short DVConfiguration::getNetworkPort()
{
  const char *c = getConfigEntry(L2_GENERAL, L1_NETWORK, L0_PORT);
  unsigned short result = 0;
  if (c)
  {
    if (1 != sscanf(c, "%hu", &result)) result=0;
  }
  return result;
}

unsigned long DVConfiguration::getNetworkMaxPDU()
{
  const char *c = getConfigEntry(L2_GENERAL, L1_NETWORK, L0_MAXPDU);
  unsigned long result = 0;
  if (c)
  {
    if (1 != sscanf(c, "%lu", &result)) result=0;
  }
  return result;
}

const char *DVConfiguration::getDatabaseFolder()
{
  const char *result = getConfigEntry(L2_GENERAL, L1_DATABASE, L0_DIRECTORY);
  if (result==NULL) result = PSTAT_DBFOLDER;
  return result;
}

const char *DVConfiguration::getSpoolFolder()
{
  const char *result = getConfigEntry(L2_GENERAL, L1_PRINT, L0_DIRECTORY);
  if (result==NULL) result = PSTAT_SPOOLFOLDER;
  return result;
}

const char *DVConfiguration::getLogFolder()
{
  const char *result = getConfigEntry(L2_GENERAL, L1_PRINT, L0_LOGDIRECTORY);
  if (result==NULL) result = getSpoolFolder();
  return result;
}

OFBool DVConfiguration::getDetailedLog()
{
  return getConfigBoolEntry(L2_GENERAL, L1_PRINT, L0_DETAILEDLOG, OFFalse);
}

unsigned long DVConfiguration::getSpoolerSleep()
{
  const char *c = getConfigEntry(L2_GENERAL, L1_PRINT, L0_SLEEP);
  unsigned long result = 0;
  if (c)
  {
    if (1 != sscanf(c, "%lu", &result)) result=0;
  }
  return result;
}

OFBool DVConfiguration::getSpoolerDeletePrintJobs()
{
  return getConfigBoolEntry(L2_GENERAL, L1_PRINT, L0_DELETEPRINTJOBS, OFFalse);
}

OFBool DVConfiguration::getSpoolerAlwaysDeleteTerminateJobs()
{
  return getConfigBoolEntry(L2_GENERAL, L1_PRINT, L0_ALWAYSDELETETERMINATEJOBS, OFFalse);
}

const char *DVConfiguration::getLUTFolder()
{
  const char *result = getConfigEntry(L2_GENERAL, L1_LUT, L0_DIRECTORY);
  if (result==NULL) result = PSTAT_LUTFOLDER;
  return result;
}

const char *DVConfiguration::getSenderName()
{
  return getConfigEntry(L2_GENERAL, L1_NETWORK, L0_SENDER);
}

const char *DVConfiguration::getReceiverName()
{
  return getConfigEntry(L2_GENERAL, L1_NETWORK, L0_RECEIVER);
}

const char *DVConfiguration::getSpoolerName()
{
  return getConfigEntry(L2_GENERAL, L1_PRINT, L0_SPOOLER);
}

const char *DVConfiguration::getDumpToolName()
{
  return getConfigEntry(L2_GENERAL, L1_DATABASE, L0_DUMP);
}

const char *DVConfiguration::getMonitorCharacteristicsFile()
{
  return getConfigEntry(L2_GENERAL, L1_MONITOR, L0_CHARACTERISTICS);
}

double DVConfiguration::getMonitorPixelWidth()
{
  const char *resolution = getConfigEntry(L2_GENERAL, L1_MONITOR, L0_RESOLUTION);
  const char *screensize = getConfigEntry(L2_GENERAL, L1_MONITOR, L0_SCREENSIZE);

  if (resolution && screensize)
  {
    double rX, rY, sX, sY;
    if ((2 == sscanf(resolution, "%lf\\%lf", &rX, &rY)) && (2 == sscanf(screensize, "%lf\\%lf", &sX, &sY)))
    {
      if ((rX > 0) && (rY > 0) && (sX > 0) && (sY > 0))
      {
      	// everything OK, return pixel width
        return sX/rX;
      }
    }
  }
  return 0.0;
}

double DVConfiguration::getMonitorPixelHeight()
{
  const char *resolution = getConfigEntry(L2_GENERAL, L1_MONITOR, L0_RESOLUTION);
  const char *screensize = getConfigEntry(L2_GENERAL, L1_MONITOR, L0_SCREENSIZE);

  if (resolution && screensize)
  {
    double rX, rY, sX, sY;
    if ((2 == sscanf(resolution, "%lf\\%lf", &rX, &rY)) && (2 == sscanf(screensize, "%lf\\%lf", &sX, &sY)))
    {
      if ((rX > 0) && (rY > 0) && (sX > 0) && (sY > 0))
      {
      	// everything OK, return pixel height
        return sY/rY;
      }
    }
  }
  return 0.0;
}

Uint32 DVConfiguration::getMaxPreviewResolutionX()
{
  const char *c = getConfigEntry(L2_GENERAL, L1_MONITOR, L0_PREVIEW);
  if (c)
  {
    Uint32 result = 0;
    Uint32 dummy = 0;
    if (2 == sscanf(c, "%lu\\%lu", &result, &dummy)) return result;
  }
  return 0;
}

Uint32 DVConfiguration::getMaxPreviewResolutionY()
{
  const char *c = getConfigEntry(L2_GENERAL, L1_MONITOR, L0_PREVIEW);
  if (c)
  {
    Uint32 result = 0;
    Uint32 dummy = 0;
    if (2 == sscanf(c, "%lu\\%lu", &dummy, &result)) return result;
  }
  return 0;
}

const char *DVConfiguration::getGUIConfigEntry(const char *key)
{
  return getConfigEntry(L2_GENERAL, L1_GUI, key);
}

OFBool DVConfiguration::getGUIConfigEntryBool(const char *key, OFBool dfl)
{
  return getConfigBoolEntry(L2_GENERAL, L1_GUI, key, dfl);
}

OFBool DVConfiguration::getTargetPrinterSupportsPresentationLUT(const char *targetID)
{
  return getConfigBoolEntry(L2_COMMUNICATION, targetID, L0_SUPPORTSPRESENTATIONLUT, OFFalse);
}

OFBool DVConfiguration::getTargetPrinterPresentationLUTMatchRequired(const char *targetID)
{
  return getConfigBoolEntry(L2_COMMUNICATION, targetID, L0_PRESENTATIONLUTMATCHREQUIRED, OFTrue);
}

OFBool DVConfiguration::getTargetPrinterPresentationLUTPreferSCPRendering(const char *targetID)
{
  return getConfigBoolEntry(L2_COMMUNICATION, targetID, L0_PRESENTATIONLUTPREFERSCPRENDERING, OFFalse);
}

OFBool DVConfiguration::getTargetPrinterPresentationLUTinFilmSession(const char *targetID)
{
  return getConfigBoolEntry(L2_COMMUNICATION, targetID, L0_PRESENTATIONLUTINFILMSESSION, OFFalse);
}

OFBool DVConfiguration::getTargetPrinterSupports12BitTransmission(const char *targetID)
{
  return getConfigBoolEntry(L2_COMMUNICATION, targetID, L0_SUPPORTS12BIT, OFTrue);
}

OFBool DVConfiguration::getTargetPrinterSupportsRequestedImageSize(const char *targetID)
{
  return getConfigBoolEntry(L2_COMMUNICATION, targetID, L0_SUPPORTSIMAGESIZE, OFFalse);
}

OFBool DVConfiguration::getTargetPrinterSupportsDecimateCrop(const char *targetID)
{
  return getConfigBoolEntry(L2_COMMUNICATION, targetID, L0_SUPPORTSDECIMATECROP, OFFalse);
}

OFBool DVConfiguration::getTargetPrinterSupportsTrim(const char *targetID)
{
  return getConfigBoolEntry(L2_COMMUNICATION, targetID, L0_SUPPORTSTRIM, OFFalse);
}

Uint32 DVConfiguration::getTargetPrinterMaxDisplayFormatColumns(const char *targetID)
{
  Uint32 result = (Uint32) -1;
  unsigned long val = 0;
  const char *c = getConfigEntry(L2_COMMUNICATION, targetID, L0_MAXCOLUMNS);
  if (c)
  {
    if (1 == sscanf(c, "%lu", &val)) result=(Uint32)val;
  }
  return result;
}

Uint32 DVConfiguration::getTargetPrinterMaxDisplayFormatRows(const char *targetID)
{
  Uint32 result = (Uint32) -1;
  unsigned long val = 0;
  const char *c = getConfigEntry(L2_COMMUNICATION, targetID, L0_MAXROWS);
  if (c)
  {
    if (1 == sscanf(c, "%lu", &val)) result=(Uint32)val;
  }
  return result;
}

Uint32 DVConfiguration::getTargetPrinterNumberOfFilmSizeIDs(const char *targetID)
{
  return countValues(getConfigEntry(L2_COMMUNICATION, targetID, L0_FILMSIZEID));
}

const char *DVConfiguration::getTargetPrinterFilmSizeID(const char *targetID, Uint32 idx, OFString& value)
{
  copyValue(getConfigEntry(L2_COMMUNICATION, targetID, L0_FILMSIZEID), idx, value);
  if (value.length()) return value.c_str(); else return NULL;
}

Uint32 DVConfiguration::getTargetPrinterNumberOfMediumTypes(const char *targetID)
{
  return countValues(getConfigEntry(L2_COMMUNICATION, targetID, L0_MEDIUMTYPE));
}

const char *DVConfiguration::getTargetPrinterMediumType(const char *targetID, Uint32 idx, OFString& value)
{
  copyValue(getConfigEntry(L2_COMMUNICATION, targetID, L0_MEDIUMTYPE), idx, value);
  if (value.length()) return value.c_str(); else return NULL;
}

Uint32 DVConfiguration::getTargetPrinterNumberOfPrinterResolutionIDs(const char *targetID)
{
  return countValues(getConfigEntry(L2_COMMUNICATION, targetID, L0_RESOLUTIONID));
}

const char *DVConfiguration::getTargetPrinterResolutionID(const char *targetID, Uint32 idx, OFString& value)
{
  copyValue(getConfigEntry(L2_COMMUNICATION, targetID, L0_RESOLUTIONID), idx, value);
  if (value.length()) return value.c_str(); else return NULL;
}

Uint32 DVConfiguration::getTargetPrinterNumberOfMagnificationTypes(const char *targetID)
{
  return countValues(getConfigEntry(L2_COMMUNICATION, targetID, L0_MAGNIFICATIONTYPE));
}

const char *DVConfiguration::getTargetPrinterMagnificationType(const char *targetID, Uint32 idx, OFString& value)
{
  copyValue(getConfigEntry(L2_COMMUNICATION, targetID, L0_MAGNIFICATIONTYPE), idx, value);
  if (value.length()) return value.c_str(); else return NULL;
}

Uint32 DVConfiguration::getTargetPrinterNumberOfSmoothingTypes(const char *targetID)
{
  return countValues(getConfigEntry(L2_COMMUNICATION, targetID, L0_SMOOTHINGTYPE));
}

const char *DVConfiguration::getTargetPrinterSmoothingType(const char *targetID, Uint32 idx, OFString& value)
{
  copyValue(getConfigEntry(L2_COMMUNICATION, targetID, L0_SMOOTHINGTYPE), idx, value);
  if (value.length()) return value.c_str(); else return NULL;
}


Uint32 DVConfiguration::getTargetPrinterNumberOfConfigurationSettings(const char *targetID)
{
  Uint32 result = 0;

  if (pConfig)
  {

    pConfig->select_section(targetID, L2_COMMUNICATION);
    if (pConfig->section_valid(1))
    {
      int counter = 1;
      char l0_key[80];
      do
      {
        sprintf(l0_key, "CONFIGURATION_%d", counter++);
      } while (NULL != pConfig->get_entry(l0_key));
      result = counter - 2;
    }
  }
  return result;
}

const char *DVConfiguration::getTargetPrinterConfigurationSetting(const char *targetID, Uint32 idx)
{
  char l0_key[80];
  sprintf(l0_key, "CONFIGURATION_%d", (int)idx+1);
  return getConfigEntry(L2_COMMUNICATION, targetID, l0_key);
}

Uint32 DVConfiguration::getNumberOfLUTs()
{
  Uint32 result = 0;
  if (pConfig)
  {
    pConfig->set_section(2, L2_LUT);
    if (pConfig->section_valid(2))
    {
       pConfig->first_section(1);
       while (pConfig->section_valid(1))
       {
       	  result++;
          pConfig->next_section(1);
       }
    }
  }
  return result;
}

const char *DVConfiguration::getLUTID(Uint32 idx)
{
  OFBool found = OFFalse;
  const char *result=NULL;
  if (pConfig)
  {
    pConfig->set_section(2, L2_LUT);
    if (pConfig->section_valid(2))
    {
       pConfig->first_section(1);
       while ((! found)&&(pConfig->section_valid(1)))
       {
         if (idx==0) found=OFTrue;
         else
         {
         	idx--;
            pConfig->next_section(1);
         }
       }
       if (pConfig->section_valid(1)) result = pConfig->get_keyword(1);
    }
  }
  return result;
}

const char *DVConfiguration::getLUTDescription(const char *lutID)
{
  return getConfigEntry(L2_LUT, lutID, L0_DESCRIPTION);
}

const char *DVConfiguration::getLUTFilename(const char *lutID)
{
  return getConfigEntry(L2_LUT, lutID, L0_FILENAME);
}

Uint32 DVConfiguration::getTargetPrinterNumberOfBorderDensities(const char *targetID)
{
  return countValues(getConfigEntry(L2_COMMUNICATION, targetID, L0_BORDERDENSITY));
}

const char *DVConfiguration::getTargetPrinterBorderDensity(const char *targetID, Uint32 idx, OFString& value)
{
  copyValue(getConfigEntry(L2_COMMUNICATION, targetID, L0_BORDERDENSITY), idx, value);
  if (value.length()) return value.c_str(); else return NULL;
}

Uint32 DVConfiguration::getTargetPrinterNumberOfMaxDensities(const char *targetID)
{
  return countValues(getConfigEntry(L2_COMMUNICATION, targetID, L0_MAXDENSITY));
}

const char *DVConfiguration::getTargetPrinterMaxDensity(const char *targetID, Uint32 idx, OFString& value)
{
  copyValue(getConfigEntry(L2_COMMUNICATION, targetID, L0_MAXDENSITY), idx, value);
  if (value.length()) return value.c_str(); else return NULL;
}

Uint32 DVConfiguration::getTargetPrinterNumberOfMinDensities(const char *targetID)
{
  return countValues(getConfigEntry(L2_COMMUNICATION, targetID, L0_MINDENSITY));
}

const char *DVConfiguration::getTargetPrinterMinDensity(const char *targetID, Uint32 idx, OFString& value)
{
  copyValue(getConfigEntry(L2_COMMUNICATION, targetID, L0_MINDENSITY), idx, value);
  if (value.length()) return value.c_str(); else return NULL;
}

Uint32 DVConfiguration::getTargetPrinterNumberOfEmptyImageDensities(const char *targetID)
{
  return countValues(getConfigEntry(L2_COMMUNICATION, targetID, L0_EMPTYIMAGEDENSITY));
}

const char *DVConfiguration::getTargetPrinterEmptyImageDensity(const char *targetID, Uint32 idx, OFString& value)
{
  copyValue(getConfigEntry(L2_COMMUNICATION, targetID, L0_EMPTYIMAGEDENSITY), idx, value);
  if (value.length()) return value.c_str(); else return NULL;
}

Uint32 DVConfiguration::getMinPrintResolutionX()
{
  const char *c = getConfigEntry(L2_GENERAL, L1_PRINT, L0_MINPRINTRESOLUTION);
  if (c)
  {
    Uint32 result = 0;
    Uint32 dummy = 0;
    if (2 == sscanf(c, "%lu\\%lu", &result, &dummy)) return result;
  }
  return 0;
}

Uint32 DVConfiguration::getMinPrintResolutionY()
{
  const char *c = getConfigEntry(L2_GENERAL, L1_PRINT, L0_MINPRINTRESOLUTION);
  if (c)
  {
    Uint32 result = 0;
    Uint32 dummy = 0;
    if (2 == sscanf(c, "%lu\\%lu", &dummy, &result)) return result;
  }
  return 0;
}

Uint32 DVConfiguration::getMaxPrintResolutionX()
{
  const char *c = getConfigEntry(L2_GENERAL, L1_PRINT, L0_MAXPRINTRESOLUTION);
  if (c)
  {
    Uint32 result = 0;
    Uint32 dummy = 0;
    if (2 == sscanf(c, "%lu\\%lu", &result, &dummy)) return result;
  }
  return 0;
}

Uint32 DVConfiguration::getMaxPrintResolutionY()
{
  const char *c = getConfigEntry(L2_GENERAL, L1_PRINT, L0_MAXPRINTRESOLUTION);
  if (c)
  {
    Uint32 result = 0;
    Uint32 dummy = 0;
    if (2 == sscanf(c, "%lu\\%lu", &dummy, &result)) return result;
  }
  return 0;
}

Uint16 DVConfiguration::getDefaultPrintIllumination()
{
  const char *c = getConfigEntry(L2_GENERAL, L1_PRINT, L0_DEFAULTILLUMINATION);
  if (c)
  {
    Uint16 result = 0;
    if (1 == sscanf(c, "%hu", &result)) return result;
  }
  return PSTAT_DEFAULT_ILLUMINATION;
}

Uint16 DVConfiguration::getDefaultPrintReflection()
{
  const char *c = getConfigEntry(L2_GENERAL, L1_PRINT, L0_DEFAULTREFLECTION);
  if (c)
  {
    Uint16 result = 0;
    if (1 == sscanf(c, "%hu", &result)) return result;
  }
  return PSTAT_DEFAULT_REFLECTION;
}

Uint32 DVConfiguration::getNumberOfVOIPresets(const char *modality)
{
  Uint32 result = 0;
  if (modality && pConfig)
  {
  	OFString aModality = modality;
    const char *currentModality = NULL;
    pConfig->set_section(2, L2_VOI);
    if (pConfig->section_valid(2))
    {
       pConfig->first_section(1);
       while (pConfig->section_valid(1))
       {
       	 currentModality = pConfig->get_entry(L0_MODALITY);
         if (currentModality && (aModality == currentModality)) result++;
         pConfig->next_section(1);
       }
    }
  }
  return result;
}

const char *DVConfiguration::getVOIPresetDescription(const char *modality, Uint32 idx)
{
  if (modality && pConfig)
  {
  	OFString aModality = modality;
    const char *currentModality = NULL;
    pConfig->set_section(2, L2_VOI);
    if (pConfig->section_valid(2))
    {
       pConfig->first_section(1);
       while (pConfig->section_valid(1))
       {
       	 currentModality = pConfig->get_entry(L0_MODALITY);
         if (currentModality && (aModality == currentModality))
         {
           if (idx==0)
           {
           	  // found entry
           	  return pConfig->get_entry(L0_DESCRIPTION);
           } else idx--;
         }
         pConfig->next_section(1);
       }
    }
  }
  return NULL;
}

double DVConfiguration::getVOIPresetWindowCenter(const char *modality, Uint32 idx)
{
  double result = 0.0;
  if (modality && pConfig)
  {
  	OFString aModality = modality;
    const char *currentModality = NULL;
    pConfig->set_section(2, L2_VOI);
    if (pConfig->section_valid(2))
    {
       pConfig->first_section(1);
       while (pConfig->section_valid(1))
       {
       	 currentModality = pConfig->get_entry(L0_MODALITY);
         if (currentModality && (aModality == currentModality))
         {
           if (idx==0)
           {
           	  // found entry
           	  const char *window = pConfig->get_entry(L0_CENTER);
           	  if (window && (1 == sscanf(window, "%lf", &result))) return result; else return 0.0;
           } else idx--;
         }
         pConfig->next_section(1);
       }
    }
  }
  return result;
}

double DVConfiguration::getVOIPresetWindowWidth(const char *modality, Uint32 idx)
{
  double result = 1.0;
  if (modality && pConfig)
  {
  	OFString aModality = modality;
    const char *currentModality = NULL;
    pConfig->set_section(2, L2_VOI);
    if (pConfig->section_valid(2))
    {
       pConfig->first_section(1);
       while (pConfig->section_valid(1))
       {
       	 currentModality = pConfig->get_entry(L0_MODALITY);
         if (currentModality && (aModality == currentModality))
         {
           if (idx==0)
           {
           	  // found entry
           	  const char *window = pConfig->get_entry(L0_WIDTH);
           	  if (window && (1 == sscanf(window, "%lf", &result))) return result; else return 1.0;
           } else idx--;
         }
         pConfig->next_section(1);
       }
    }
  }
  return result;
}

Uint32 DVConfiguration::getTargetPrinterNumberOfPortraitDisplayFormats(const char *targetID)
{
  return countValues(getConfigEntry(L2_COMMUNICATION, targetID, L0_DISPLAYFORMAT));
}

Uint32 DVConfiguration::getTargetPrinterPortraitDisplayFormatRows(const char *targetID, Uint32 idx)
{
  OFString value;
  unsigned long rows=0;
  unsigned long columns=0;
  copyValue(getConfigEntry(L2_COMMUNICATION, targetID, L0_DISPLAYFORMAT), idx, value);
  if (2==sscanf(value.c_str(), "%lu,%lu", &columns, &rows)) return rows;
  return 0;
}

Uint32 DVConfiguration::getTargetPrinterPortraitDisplayFormatColumns(const char *targetID, Uint32 idx)
{
  OFString value;
  unsigned long rows=0;
  unsigned long columns=0;
  copyValue(getConfigEntry(L2_COMMUNICATION, targetID, L0_DISPLAYFORMAT), idx, value);
  if (2==sscanf(value.c_str(), "%lu,%lu", &columns, &rows)) return columns;
  return 0;
}

OFBool DVConfiguration::getTargetPrinterSupportsAnnotation(const char *targetID)
{
  if (NULL==getConfigEntry(L2_COMMUNICATION, targetID, L0_ANNOTATION)) return getTargetPrinterSessionLabelAnnotation(targetID);
  return OFTrue;
}

OFBool DVConfiguration::getTargetPrinterSupportsAnnotationBoxSOPClass(const char *targetID)
{
  if (NULL==getConfigEntry(L2_COMMUNICATION, targetID, L0_ANNOTATION)) return OFFalse;
  return OFTrue;
}

OFBool DVConfiguration::getTargetPrinterSessionLabelAnnotation(const char *targetID)
{
  return getConfigBoolEntry(L2_COMMUNICATION, targetID, L0_SESSIONLABELANNOTATION, OFFalse);
}

const char *DVConfiguration::getTargetPrinterAnnotationDisplayFormatID(const char *targetID, OFString& value)
{
  copyValue(getConfigEntry(L2_COMMUNICATION, targetID, L0_ANNOTATION), 1, value);
  if (value.length()) return value.c_str(); else return NULL;
}
    
Uint16 DVConfiguration::getTargetPrinterAnnotationPosition(const char *targetID)
{
  OFString value;
  copyValue(getConfigEntry(L2_COMMUNICATION, targetID, L0_ANNOTATION), 0, value);
  if (value.length())
  {
    Uint16 result = 0;
    if (1 == sscanf(value.c_str(), "%hu", &result)) return result;
  }
  return 0;
}

/*
 *  CVS/RCS Log:
 *  $Log: dvpscf.cc,v $
 *  Revision 1.19  2000-03-03 14:13:58  meichel
 *  Implemented library support for redirecting error messages into memory
 *    instead of printing them to stdout/stderr for GUI applications.
 *
 *  Revision 1.18  1999/11/25 11:41:12  joergr
 *  Changed config file entry "HighEndSystem" to "HighResolutionGraphics".
 *
 *  Revision 1.17  1999/11/03 13:05:34  meichel
 *  Added support for transmitting annotations in the film session label.
 *    Added support for dump tool launched from DVInterface.
 *
 *  Revision 1.16  1999/10/22 13:05:33  meichel
 *  Added conditional define to prevent compiler warning
 *
 *  Revision 1.15  1999/10/20 10:54:42  joergr
 *  Added support for a down-scaled preview image of the current DICOM image
 *  (e.g. useful for online-windowing or print preview).
 *
 *  Revision 1.14  1999/10/19 14:48:23  meichel
 *  added support for the Basic Annotation Box SOP Class
 *    as well as access methods for Max Density and Min Density.
 *
 *  Revision 1.13  1999/10/13 14:12:00  meichel
 *  Added config file entries and access methods
 *    for user-defined VOI presets, log directory, verbatim logging
 *    and an explicit list of image display formats for each printer.
 *
 *  Revision 1.12  1999/10/07 17:21:57  meichel
 *  Reworked management of Presentation LUTs in order to create tighter
 *    coupling between Softcopy and Print.
 *
 *  Revision 1.11  1999/10/01 15:34:33  joergr
 *  Fixed "copy and paste" error.
 *
 *  Revision 1.10  1999/10/01 13:33:18  joergr
 *  Added new option to config file: AlwaysDeleteTerminateJobs.
 *
 *  Revision 1.9  1999/09/27 12:49:27  meichel
 *  Fixed index bug in getLUTID, always returned first index.
 *
 *  Revision 1.8  1999/09/27 10:41:27  meichel
 *  Fixed index bug in getTargetPrinterConfigurationSetting.
 *
 *  Revision 1.7  1999/09/24 15:24:33  meichel
 *  Added support for CP 173 (Presentation LUT clarifications)
 *
 *  Revision 1.6  1999/09/23 17:37:16  meichel
 *  Added support for Basic Film Session options to dcmpstat print code.
 *
 *  Revision 1.5  1999/09/15 17:43:33  meichel
 *  Implemented print job dispatcher code for dcmpstat, adapted dcmprtsv
 *    and dcmpsprt applications.
 *
 *  Revision 1.4  1999/09/13 15:19:15  meichel
 *  Added implementations for a number of further print API methods.
 *
 *  Revision 1.3  1999/09/10 12:46:54  meichel
 *  Added implementations for a number of print API methods.
 *
 *  Revision 1.2  1999/09/09 12:20:52  meichel
 *  Added print API method declarations and implementations (empty for now).
 *
 *  Revision 1.1  1999/09/08 16:41:42  meichel
 *  Moved configuration file evaluation to separate class.
 *
 *
 */
