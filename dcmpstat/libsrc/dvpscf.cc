/*
 *
 *  Copyright (C) 1998-2021, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmpstat
 *
 *  Author:  Joerg Riesmeier, Marco Eichelberg
 *
 *  Purpose: DVConfiguration
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpscf.h"      /* for DVConfiguration */
#include "dcmtk/ofstd/ofconfig.h"    /* for class OFConfigFile */
#include "dcmtk/dcmpstat/dvpsdef.h"     /* for constants */
#include "dcmtk/ofstd/ofstd.h"       /* for class OFStandard */

#ifndef HAVE_WINDOWS_H
/* some Unix operating systems do not define a prototype for strncasecmp
 * although the function is known.
 */
#ifndef HAVE_PROTOTYPE_STRNCASECMP
extern "C" int strncasecmp(const char *s1, const char *s2, size_t n);
#endif
#endif

/* keywords for configuration file */

#define L0_AETITLE                      "AETITLE"
#define L0_ALWAYSDELETETERMINATEJOBS    "ALWAYSDELETETERMINATEJOBS"
#define L0_ANNOTATION                   "ANNOTATION"
#define L0_AUTOCREATECONFIGFILE         "AUTOCREATECONFIGFILE"
#define L0_BINARYLOG                    "BINARYLOG"
#define L0_BITPRESERVINGMODE            "BITPRESERVINGMODE"
#define L0_BORDERDENSITY                "BORDERDENSITY"
#define L0_CACERTIFICATEDIRECTORY       "CACERTIFICATEDIRECTORY"
#define L0_CENTER                       "CENTER"
#define L0_CERTIFICATE                  "CERTIFICATE"
#define L0_CHARACTERISTICS              "CHARACTERISTICS"
#define L0_CHECK                        "CHECK"
#define L0_CODE                         "CODE"
#define L0_CORRECTUIDPADDING            "CORRECTUIDPADDING"
#define L0_DEFAULTILLUMINATION          "DEFAULTILLUMINATION"
#define L0_DEFAULTREFLECTION            "DEFAULTREFLECTION"
#define L0_DELETEPRINTJOBS              "DELETEPRINTJOBS"
#define L0_DESCRIPTION                  "DESCRIPTION"
#define L0_DETAILEDLOG                  "DETAILEDLOG"
#define L0_DICOMNAME                    "DICOMNAME"
#define L0_DIFFIEHELLMANPARAMETERS      "DIFFIEHELLMANPARAMETERS"   
#define L0_DIRECTORY                    "DIRECTORY"
#define L0_DISABLENEWVRS                "DISABLENEWVRS"
#define L0_DISPLAYFORMAT                "DISPLAYFORMAT"
#define L0_DUMP                         "DUMP"
#define L0_EMPTYIMAGEDENSITY            "EMPTYIMAGEDENSITY"
#define L0_FILENAME                     "FILENAME"
#define L0_FILMDESTINATION              "FILMDESTINATION"
#define L0_FILMSIZEID                   "FILMSIZEID"
#define L0_HOSTNAME                     "HOSTNAME"
#define L0_IMPLICITONLY                 "IMPLICITONLY"
#define L0_KEEPMESSAGEPORTOPEN          "KEEPMESSAGEPORTOPEN"
#define L0_LOGDIRECTORY                 "LOGDIRECTORY"
#define L0_LOGFILE                      "LOGFILE"
#define L0_LOGIN                        "LOGIN"
#define L0_LOGLEVEL                     "LOGLEVEL"
#define L0_MAGNIFICATIONTYPE            "MAGNIFICATIONTYPE"
#define L0_MAXASSOCIATIONS              "MAXASSOCIATIONS"
#define L0_MAXCOLUMNS                   "MAXCOLUMNS"
#define L0_MAXDENSITY                   "MAXDENSITY"
#define L0_MAXPDU                       "MAXPDU"
#define L0_MAXPRINTRESOLUTION           "MAXPRINTRESOLUTION"
#define L0_MAXROWS                      "MAXROWS"
#define L0_MEDIUMTYPE                   "MEDIUMTYPE"
#define L0_MESSAGEPORT                  "MESSAGEPORT"
#define L0_MINDENSITY                   "MINDENSITY"
#define L0_MINPRINTRESOLUTION           "MINPRINTRESOLUTION"
#define L0_MODALITY                     "MODALITY"
#define L0_NAME                         "NAME"
#define L0_OMITSOPCLASSUIDFROMCREATERESPONSE "OMITSOPCLASSUIDFROMCREATERESPONSE"
#define L0_ORGANIZATION                 "ORGANIZATION"
#define L0_PEERAUTHENTICATION           "PEERAUTHENTICATION"
#define L0_PORT                         "PORT"
#define L0_PRESENTATIONLUTINFILMSESSION "PRESENTATIONLUTINFILMSESSION"
#define L0_PRESENTATIONLUTMATCHREQUIRED "PRESENTATIONLUTMATCHREQUIRED"
#define L0_PRESENTATIONLUTPREFERSCPRENDERING "PRESENTATIONLUTPREFERSCPRENDERING"
#define L0_PREVIEW                      "PREVIEWSIZE"
#define L0_PRIVATEKEY                   "PRIVATEKEY"
#define L0_PRIVATEKEYPASSWORD           "PRIVATEKEYPASSWORD"
#define L0_RANDOMSEED                   "RANDOMSEED"
#define L0_RECEIVER                     "RECEIVER"
#define L0_RESOLUTION                   "RESOLUTION"
#define L0_RESOLUTIONID                 "RESOLUTIONID"
#define L0_SCREENSIZE                   "SCREENSIZE"
#define L0_SENDER                       "SENDER"
#define L0_SERVER                       "SERVER"
#define L0_SESSIONLABELANNOTATION       "SESSIONLABELANNOTATION"
#define L0_SLEEP                        "SLEEP"
#define L0_SMOOTHINGTYPE                "SMOOTHINGTYPE"
#define L0_SPOOLER                      "SPOOLER"
#define L0_SUPPORTS12BIT                "SUPPORTS12BIT"
#define L0_SUPPORTSDECIMATECROP         "SUPPORTSDECIMATECROP"
#define L0_SUPPORTSIMAGESIZE            "SUPPORTSIMAGESIZE"
#define L0_SUPPORTSPRESENTATIONLUT      "SUPPORTSPRESENTATIONLUT"
#define L0_SUPPORTSTRIM                 "SUPPORTSTRIM"
#define L0_TIMEOUT                      "TIMEOUT"
#define L0_TLSDIRECTORY                 "TLSDIRECTORY"
#define L0_TLSPROFILE                   "TLSPROFILE"
#define L0_TYPE                         "TYPE"
#define L0_USEPEMFORMAT                 "USEPEMFORMAT"
#define L0_USERKEYDIRECTORY             "USERKEYDIRECTORY"
#define L0_USETLS                       "USETLS"
#define L0_WIDTH                        "WIDTH"
#define L1_APPLICATION                  "APPLICATION"
#define L1_DATABASE                     "DATABASE"
#define L1_GUI                          "GUI"
#define L1_LUT                          "LUT"
#define L1_REPORT                       "REPORT"
#define L1_MONITOR                      "MONITOR"
#define L1_NETWORK                      "NETWORK"
#define L1_PRINT                        "PRINT"
#define L1_QUERY_RETRIEVE               "QUERY_RETRIEVE"
#define L1_TLS                          "TLS"
#define L2_COMMUNICATION                "COMMUNICATION"
#define L2_GENERAL                      "GENERAL"
//      L2_HIGHRESOLUTIONGRAPHICS       is defined in dvpsdef.h
#define L2_LUT                          "LUT"
#define L2_USERS                        "USERS"
#define L2_VOI                          "VOI"
#define L2_REPORT                       "REPORT"



/* --------------- static helper functions --------------- */

static DVPSPeerType getConfigTargetType(const char *val)
{
  DVPSPeerType result = DVPSE_storage; /* default */

  if (val==NULL) return result;
  OFString pstring(val);
  OFString ostring;
  size_t len = pstring.length();
  unsigned char c;
  for (size_t i=0; i<len; i++)
  {
    c = pstring[i];
    if ((c>='a') && (c <= 'z')) ostring += (char)(toupper(c));
    else if ((c>='A') && (c <= 'Z')) ostring += c;
    else if ((c>='0') && (c <= '9')) ostring += c;
    else if (c=='_')  ostring += c;
  }
  if (ostring=="PRINTER")       result=DVPSE_printRemote; else
  if (ostring=="LOCALPRINTER")  result=DVPSE_printLocal; else
  if (ostring=="STORAGE")       result=DVPSE_storage; else
  if (ostring=="RECEIVER")      result=DVPSE_receiver; else
  {
    DCMPSTAT_WARN("unsupported peer type in config file: '" << val << "', ignoring.");
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
    while ((c = *str++) != '\0') if (c == '\\') result++;
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

static int strCompare(const char *str1, const char *str2, size_t len)
{
#ifdef HAVE_WINDOWS_H
  return _strnicmp(str1, str2, len);
#else
  return strncasecmp(str1, str2, len);
#endif    
}


DVConfiguration::DVConfiguration(const char *config_file)
: pConfig(NULL)
{
  if (config_file)
  {
      FILE *cfgfile = fopen(config_file, "rb");
      if (cfgfile)
      {
          pConfig = new OFConfigFile(cfgfile);
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
         currentType = getConfigTargetType(pConfig->get_entry(L0_TYPE));
         switch (peerType)
         {
           case DVPSE_storage:
             if (currentType==DVPSE_storage) result++;
             break;
           case DVPSE_receiver:
             if (currentType==DVPSE_receiver) result++;
             break;
           case DVPSE_printRemote:
             if (currentType==DVPSE_printRemote) result++;
             break;
           case DVPSE_printLocal:
             if (currentType==DVPSE_printLocal) result++;
             break;
           case DVPSE_printAny:
             if (currentType==DVPSE_printRemote) result++;
             else if (currentType==DVPSE_printLocal) result++;
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
         currentType = getConfigTargetType(pConfig->get_entry(L0_TYPE));
         switch (peerType)
         {
           case DVPSE_storage:
             if (currentType==DVPSE_storage)
             {
               if (idx==0) found=OFTrue; else idx--;
             }
             break;
           case DVPSE_receiver:
             if (currentType==DVPSE_receiver)
             {
               if (idx==0) found=OFTrue; else idx--;
             }
             break;
           case DVPSE_printRemote:
             if (currentType==DVPSE_printRemote)
             {
               if (idx==0) found=OFTrue; else idx--;
             }
             break;
           case DVPSE_printLocal:
             if (currentType==DVPSE_printLocal)
             {
               if (idx==0) found=OFTrue; else idx--;
             }
             break;
           case DVPSE_printAny:
             if ((currentType==DVPSE_printRemote)||(currentType==DVPSE_printLocal))
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

Sint32 DVConfiguration::getTargetTimeout(const char *targetID)
{
  const char *c = getConfigEntry(L2_COMMUNICATION, targetID, L0_TIMEOUT);
  signed long result = -1; // default is -1
  if (c)
  {
    if (1 != sscanf(c, "%ld", &result)) result=0;
  }
  return (Sint32) result;
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
  return getConfigTargetType(getConfigEntry(L2_COMMUNICATION, targetID, L0_TYPE));
}

const char *DVConfiguration::getLogFolder()
{
  return getConfigEntry(L2_GENERAL, L1_APPLICATION, L0_LOGDIRECTORY);
}

const char *DVConfiguration::getLogFile()
{
  return getConfigEntry(L2_GENERAL, L1_APPLICATION, L0_LOGFILE);
}

OFLogger::LogLevel DVConfiguration::getLogLevel()
{
  OFLogger::LogLevel result = OFLogger::FATAL_LOG_LEVEL;
  const char *c = getConfigEntry(L2_GENERAL, L1_APPLICATION, L0_LOGLEVEL);
  if (c != NULL)
  {
    if (strCompare(c, "ERROR", 5) == 0)
      result = OFLogger::ERROR_LOG_LEVEL;
    else if (strCompare(c, "WARN", 4) == 0)
      result = OFLogger::WARN_LOG_LEVEL;
    else if (strCompare(c, "INFO", 4) == 0)
      result = OFLogger::INFO_LOG_LEVEL;
    else if (strCompare(c, "DEBUG", 5) == 0)
      result = OFLogger::DEBUG_LOG_LEVEL;
  }
  return result;
}

unsigned short DVConfiguration::getMessagePort()
{
  const char *c = getConfigEntry(L2_GENERAL, L1_APPLICATION, L0_MESSAGEPORT);
  unsigned short result = 0;
  if (c)
  {
    if (1 != sscanf(c, "%hu", &result)) result=0;
  }
  return result;
}

OFBool DVConfiguration::getMessagePortKeepOpen()
{
  return getConfigBoolEntry(L2_GENERAL, L1_APPLICATION, L0_KEEPMESSAGEPORTOPEN, OFFalse);
}

const char *DVConfiguration::getNetworkAETitle()
{
  const char *result = getConfigEntry(L2_GENERAL, L1_NETWORK, L0_AETITLE);
  if (result==NULL) result = PSTAT_AETITLE;
  return result;
}

OFBool DVConfiguration::getQueryRetrieveAutoCreateConfigFile()
{
  return getConfigBoolEntry(L2_GENERAL, L1_QUERY_RETRIEVE, L0_AUTOCREATECONFIGFILE, OFTrue);
}

const char *DVConfiguration::getQueryRetrieveAETitle()
{
  const char *result = getConfigEntry(L2_GENERAL, L1_QUERY_RETRIEVE, L0_AETITLE);
  if (result==NULL) result = PSTAT_AETITLE;
  return result;
}

unsigned short DVConfiguration::getQueryRetrievePort()
{
  const char *c = getConfigEntry(L2_GENERAL, L1_QUERY_RETRIEVE, L0_PORT);
  unsigned short result = 0;
  if (c)
  {
    if (1 != sscanf(c, "%hu", &result)) result=0;
  }
  return result;
}

unsigned long DVConfiguration::getQueryRetrieveMaxPDU()
{
  const char *c = getConfigEntry(L2_GENERAL, L1_QUERY_RETRIEVE, L0_MAXPDU);
  unsigned long result = 0;
  if (c)
  {
    if (1 != sscanf(c, "%lu", &result)) result=0;
  }
  return result;
}

Sint32 DVConfiguration::getQueryRetrieveTimeout()
{
  const char *c = getConfigEntry(L2_GENERAL, L1_QUERY_RETRIEVE, L0_TIMEOUT);
  signed long result = -1; // default is -1
  if (c)
  {
    if (1 != sscanf(c, "%ld", &result)) result=0;
  }
  return (Sint32) result;
}

unsigned long DVConfiguration::getQueryRetrieveMaxAssociations()
{
  const char *c = getConfigEntry(L2_GENERAL, L1_QUERY_RETRIEVE, L0_MAXASSOCIATIONS);
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

OFBool DVConfiguration::getDetailedLog()
{
  return getConfigBoolEntry(L2_GENERAL, L1_PRINT, L0_DETAILEDLOG, OFFalse);
}

OFBool DVConfiguration::getBinaryLog()
{
  return getConfigBoolEntry(L2_GENERAL, L1_PRINT, L0_BINARYLOG, OFFalse);
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

const char *DVConfiguration::getReportFolder()
{
  const char *result = getConfigEntry(L2_GENERAL, L1_REPORT, L0_DIRECTORY);
  if (result==NULL) result = PSTAT_REPORTFOLDER;
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

const char *DVConfiguration::getQueryRetrieveServerName()
{
  return getConfigEntry(L2_GENERAL, L1_QUERY_RETRIEVE, L0_SERVER);
}

const char *DVConfiguration::getSpoolerName()
{
  return getConfigEntry(L2_GENERAL, L1_PRINT, L0_SPOOLER);
}

const char *DVConfiguration::getPrintServerName()
{
  return getConfigEntry(L2_GENERAL, L1_PRINT, L0_SERVER);
}

const char *DVConfiguration::getDumpToolName()
{
  return getConfigEntry(L2_GENERAL, L1_DATABASE, L0_DUMP);
}

const char *DVConfiguration::getCheckToolName()
{
  return getConfigEntry(L2_GENERAL, L1_DATABASE, L0_CHECK);
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
    double rX=0.0, rY=0.0, sX=0.0, sY=0.0;
    OFString s(resolution);
    OFBool success = OFFalse;

    rX = OFStandard::atof(s.c_str(), &success);
    if (success)
    {
      s.erase(0, s.find('\\')+1);
      if (!s.empty())
      {
        rY = OFStandard::atof(s.c_str(), &success);
      } else success = OFFalse;
    }

    if (success)
    {
      s = screensize;
      sX = OFStandard::atof(s.c_str(), &success);
      if (success)
      {
        s.erase(0, s.find('\\')+1);
        if (!s.empty())
        {
          sY = OFStandard::atof(s.c_str(), &success);
        } else success = OFFalse;
      }
    }

    if (success && (rX > 0) && (rY > 0) && (sX > 0) && (sY > 0))
    {
      // everything OK, return pixel width
      return sX/rX;
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
    double rX=0.0, rY=0.0, sX=0.0, sY=0.0;
    OFString s(resolution);
    OFBool success = OFFalse;

    rX = OFStandard::atof(s.c_str(), &success);
    if (success)
    {
      s.erase(0, s.find('\\')+1);
      if (!s.empty())
      {
        rY = OFStandard::atof(s.c_str(), &success);
      } else success = OFFalse;
    }

    if (success)
    {
      s = screensize;
      sX = OFStandard::atof(s.c_str(), &success);
      if (success)
      {
        s.erase(0, s.find('\\')+1);
        if (!s.empty())
        {
          sY = OFStandard::atof(s.c_str(), &success);
        } else success = OFFalse;
      }
    }

    if (success && (rX > 0) && (rY > 0) && (sX > 0) && (sY > 0))
    {
        // everything OK, return pixel height
        return sY/rY;
    }
  }
  return 0.0;
}

Uint32 DVConfiguration::getMaxPreviewResolutionX()
{
  const char *c = getConfigEntry(L2_GENERAL, L1_MONITOR, L0_PREVIEW);
  if (c)
  {
    unsigned long result = 0;
    unsigned long dummy = 0;
    if (2 == sscanf(c, "%lu\\%lu", &result, &dummy)) return (Uint32) result;
  }
  return 0;
}

Uint32 DVConfiguration::getMaxPreviewResolutionY()
{
  const char *c = getConfigEntry(L2_GENERAL, L1_MONITOR, L0_PREVIEW);
  if (c)
  {
    unsigned long result = 0;
    unsigned long dummy = 0;
    if (2 == sscanf(c, "%lu\\%lu", &dummy, &result)) return (Uint32) result;
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

OFBool DVConfiguration::getTargetPrintSCPOmitSOPClassUIDFromCreateResponse(const char *targetID)
{
  return getConfigBoolEntry(L2_COMMUNICATION, targetID, L0_OMITSOPCLASSUIDFROMCREATERESPONSE, OFFalse);
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

Uint32 DVConfiguration::getTargetPrinterNumberOfFilmDestinations(const char *targetID)
{
  return countValues(getConfigEntry(L2_COMMUNICATION, targetID, L0_FILMDESTINATION));
}

const char *DVConfiguration::getTargetPrinterFilmDestination(const char *targetID, Uint32 idx, OFString& value)
{
  copyValue(getConfigEntry(L2_COMMUNICATION, targetID, L0_FILMDESTINATION), idx, value);
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

Uint32 DVConfiguration::getNumberOfReports()
{
  Uint32 result = 0;
  if (pConfig)
  {
    pConfig->set_section(2, L2_REPORT);
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

const char *DVConfiguration::getReportID(Uint32 idx)
{
  OFBool found = OFFalse;
  const char *result=NULL;
  if (pConfig)
  {
    pConfig->set_section(2, L2_REPORT);
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

const char *DVConfiguration::getReportDescription(const char *reportID)
{
  return getConfigEntry(L2_REPORT, reportID, L0_DESCRIPTION);
}

const char *DVConfiguration::getReportFilename(const char *reportID)
{
  return getConfigEntry(L2_REPORT, reportID, L0_FILENAME);
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
    unsigned long result = 0;
    unsigned long dummy = 0;
    if (2 == sscanf(c, "%lu\\%lu", &result, &dummy)) return (Uint32) result;
  }
  return 0;
}

Uint32 DVConfiguration::getMinPrintResolutionY()
{
  const char *c = getConfigEntry(L2_GENERAL, L1_PRINT, L0_MINPRINTRESOLUTION);
  if (c)
  {
    unsigned long result = 0;
    unsigned long dummy = 0;
    if (2 == sscanf(c, "%lu\\%lu", &dummy, &result)) return (Uint32) result;
  }
  return 0;
}

Uint32 DVConfiguration::getMaxPrintResolutionX()
{
  const char *c = getConfigEntry(L2_GENERAL, L1_PRINT, L0_MAXPRINTRESOLUTION);
  if (c)
  {
    unsigned long result = 0;
    unsigned long dummy = 0;
    if (2 == sscanf(c, "%lu\\%lu", &result, &dummy)) return (Uint32) result;
  }
  return 0;
}

Uint32 DVConfiguration::getMaxPrintResolutionY()
{
  const char *c = getConfigEntry(L2_GENERAL, L1_PRINT, L0_MAXPRINTRESOLUTION);
  if (c)
  {
    unsigned long result = 0;
    unsigned long dummy = 0;
    if (2 == sscanf(c, "%lu\\%lu", &dummy, &result)) return (Uint32) result;
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
  return DEFAULT_illumination;
}

Uint16 DVConfiguration::getDefaultPrintReflection()
{
  const char *c = getConfigEntry(L2_GENERAL, L1_PRINT, L0_DEFAULTREFLECTION);
  if (c)
  {
    Uint16 result = 0;
    if (1 == sscanf(c, "%hu", &result)) return result;
  }
  return DEFAULT_reflectedAmbientLight;
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
             if (window) return OFStandard::atof(window); else return 0.0;
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
             if (window) return OFStandard::atof(window); else return 1.0;
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

const char *DVConfiguration::getTLSFolder()
{
  return getConfigEntry(L2_GENERAL, L1_TLS, L0_TLSDIRECTORY);
}

const char *DVConfiguration::getTLSCACertificateFolder()
{
  return getConfigEntry(L2_GENERAL, L1_TLS, L0_CACERTIFICATEDIRECTORY);
}

OFBool DVConfiguration::getTLSPEMFormat()
{
  return getConfigBoolEntry(L2_GENERAL, L1_TLS, L0_USEPEMFORMAT, OFTrue);
}

OFBool DVConfiguration::getTargetBitPreservingMode(const char *targetID)
{
  return getConfigBoolEntry(L2_COMMUNICATION, targetID, L0_BITPRESERVINGMODE, OFFalse);
}

OFBool DVConfiguration::getTargetCorrectUIDPadding(const char *targetID)
{
  return getConfigBoolEntry(L2_COMMUNICATION, targetID, L0_CORRECTUIDPADDING, OFFalse);
}

OFBool DVConfiguration::getTargetUseTLS(const char *targetID)
{
  return getConfigBoolEntry(L2_COMMUNICATION, targetID, L0_USETLS, OFFalse);
}

const char *DVConfiguration::getTargetCertificate(const char *targetID)
{
  return getConfigEntry(L2_COMMUNICATION, targetID, L0_CERTIFICATE);
}

const char *DVConfiguration::getTargetPrivateKey(const char *targetID)
{
  return getConfigEntry(L2_COMMUNICATION, targetID, L0_PRIVATEKEY);
}

const char *DVConfiguration::getTargetPrivateKeyPassword(const char *targetID)
{
  return getConfigEntry(L2_COMMUNICATION, targetID, L0_PRIVATEKEYPASSWORD);
}

const char *DVConfiguration::getTargetTLSProfile(const char *targetID)
{
  return getConfigEntry(L2_COMMUNICATION, targetID, L0_TLSPROFILE);
}

DVPSCertificateVerificationType DVConfiguration::getTargetPeerAuthentication(const char *targetID)
{
  DVPSCertificateVerificationType result = DVPSQ_require;
  const char *c = getConfigEntry(L2_COMMUNICATION, targetID, L0_PEERAUTHENTICATION);
  if (c != NULL)
  {
    if (strCompare(c, "REQUIRE", 7) == 0)
      result = DVPSQ_require;
    else if (strCompare(c, "VERIFY", 6) == 0)
      result = DVPSQ_verify;
    else if (strCompare(c, "IGNORE", 6) == 0)
      result = DVPSQ_ignore;
  }
  return result;
}

const char *DVConfiguration::getTargetDiffieHellmanParameters(const char *targetID)
{
  return getConfigEntry(L2_COMMUNICATION, targetID, L0_DIFFIEHELLMANPARAMETERS);
}

const char *DVConfiguration::getTargetRandomSeed(const char *targetID)
{
  return getConfigEntry(L2_COMMUNICATION, targetID, L0_RANDOMSEED);
}

const char *DVConfiguration::getUserCertificateFolder()
{
  return getConfigEntry(L2_GENERAL, L1_TLS, L0_USERKEYDIRECTORY);
}

Uint32 DVConfiguration::getNumberOfUsers()
{
  Uint32 result = 0;
  if (pConfig)
  {
    pConfig->set_section(2, L2_USERS);
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

const char *DVConfiguration::getUserID(Uint32 idx)
{
  OFBool found = OFFalse;
  const char *result=NULL;
  if (pConfig)
  {
    pConfig->set_section(2, L2_USERS);
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

const char *DVConfiguration::getUserLogin(const char *userID)
{
  return getConfigEntry(L2_USERS, userID, L0_LOGIN);
}

const char *DVConfiguration::getUserName(const char *userID)
{
  return getConfigEntry(L2_USERS, userID, L0_NAME);
}

const char *DVConfiguration::getUserDICOMName(const char *userID)
{
  return getConfigEntry(L2_USERS, userID, L0_DICOMNAME);
}

const char *DVConfiguration::getUserCertificate(const char *userID)
{
  return getConfigEntry(L2_USERS, userID, L0_CERTIFICATE);
}

const char *DVConfiguration::getUserPrivateKey(const char *userID)
{
  return getConfigEntry(L2_USERS, userID, L0_PRIVATEKEY);
}

const char *DVConfiguration::getUserOrganization(const char *userID)
{
  return getConfigEntry(L2_USERS, userID, L0_ORGANIZATION);
}

const char *DVConfiguration::getUserCodingSchemeDesignator(const char *userID, OFString& value)
{
  copyValue(getConfigEntry(L2_USERS, userID, L0_CODE), 0, value);
  if (value.length()) return value.c_str(); else return NULL;
}

const char *DVConfiguration::getUserCodingSchemeVersion(const char *userID, OFString& value)
{
  copyValue(getConfigEntry(L2_USERS, userID, L0_CODE), 1, value);
  if (value.length()) return value.c_str(); else return NULL;
}

const char *DVConfiguration::getUserCodeValue(const char *userID, OFString& value)
{
  copyValue(getConfigEntry(L2_USERS, userID, L0_CODE), 2, value);
  if (value.length()) return value.c_str(); else return NULL;
}

const char *DVConfiguration::getUserCodeMeaning(const char *userID, OFString& value)
{
  copyValue(getConfigEntry(L2_USERS, userID, L0_CODE), 3, value);
  if (value.length()) return value.c_str(); else return NULL;
}
