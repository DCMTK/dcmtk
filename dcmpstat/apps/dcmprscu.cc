/*
 *  Copyright (C) 1999-2010, OFFIS e.V.
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
 *  Module:  dcmprscu
 *
 *  Authors: Andreas Thiel, Marco Eichelberg
 *
 *  Purpose: Presentation State Viewer - Print Spooler
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:13:45 $
 *  CVS/RCS Revision: $Revision: 1.30 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmnet/dcompat.h"     /* compatibility code, needs to be included before dirent.h */

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#define INCLUDE_CCTYPE
#include "dcmtk/ofstd/ofstdinc.h"

BEGIN_EXTERN_C
/* This #if code is suggested by the gnu autoconf documentation */
#ifdef HAVE_DIRENT_H
#include <dirent.h>
#define NAMLEN(dirent) strlen((dirent)->d_name)
#else
#define dirent direct
#define NAMELEN(dirent) (dirent)->d_namlen
#ifdef HAVE_SYS_NDIR_H
#include <sys/ndir.h>
#endif
#ifdef HAVE_SYS_DIR_H
#include <sys/dir.h>
#endif
#ifdef HAVE_NDIR_H
#include <ndir.h>
#endif
#endif
#ifdef HAVE_IO_H
#include <io.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>      /* for O_RDONLY */
#endif
END_EXTERN_C

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmpstat/dvpsdef.h"    /* for constants */
#include "dcmtk/dcmpstat/dviface.h"    /* for DVInterface */
#include "dcmtk/ofstd/ofstring.h"   /* for OFString */
#include "dcmtk/ofstd/ofbmanip.h"   /* for OFBitmanipTemplate */
#include "dcmtk/ofstd/ofdatime.h"   /* for OFDateTime */
#include "dcmtk/dcmdata/dcuid.h"      /* for dcmtk version name */
#include "dcmtk/dcmdata/cmdlnarg.h"   /* for prepareCmdLineArgs */
#include "dcmtk/ofstd/ofconapp.h"   /* for OFConsoleApplication */
#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/dcmpstat/dvpspr.h"
#include "dcmtk/dcmpstat/dvpssp.h"
#include "dcmtk/dcmpstat/dvpshlp.h"     /* for class DVPSHelper */
#include "dcmtk/ofstd/ofstd.h"

#ifdef WITH_OPENSSL
#include "dcmtk/dcmtls/tlstrans.h"
#include "dcmtk/dcmtls/tlslayer.h"
#endif

#ifdef WITH_ZLIB
#include <zlib.h>        /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "dcmprscu"

static OFLogger dcmprscuLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

/* command line options */
static OFBool           opt_spoolMode       = OFFalse;             /* default: file print mode */
static OFBool           opt_noPrint         = OFFalse;
static OFBool           opt_sessionPrint    = OFFalse;             /* Basic Film Session N-ACTION? */
static OFBool           opt_Monochrome1     = OFFalse;             /* send images in MONOCHROME 1? */
static const char *     opt_cfgName         = NULL;                /* config file name */
static const char *     opt_printer         = NULL;                /* printer name */

static const char *     opt_mediumtype      = NULL;
static const char *     opt_destination     = NULL;
static const char *     opt_sessionlabel    = NULL;
static const char *     opt_priority        = NULL;
static const char *     opt_ownerID         = NULL;
static const char *     opt_spoolPrefix     = NULL;
static OFCmdUnsignedInt opt_sleep           = (OFCmdUnsignedInt) 1;
static OFCmdUnsignedInt opt_copies          = (OFCmdUnsignedInt) 0;

/* print target data, taken from configuration file */
static const char *   targetHostname        = NULL;
static const char *   targetDescription     = NULL;
static const char *   targetAETitle         = NULL;
static unsigned short targetPort            = 104;
static unsigned long  targetMaxPDU          = ASC_DEFAULTMAXPDU;
static OFBool         targetImplicitOnly    = OFFalse;
static OFBool         targetDisableNewVRs   = OFFalse;
static OFBool         targetSupportsPLUT    = OFTrue;
static OFBool         targetSupportsAnnotation = OFTrue;
static OFBool         targetSupports12bit   = OFTrue;
static OFBool         targetPLUTinFilmSession = OFFalse;
static OFBool         targetRequiresMatchingLUT   = OFTrue;
static OFBool         targetPreferSCPLUTRendering = OFFalse;
static OFBool         deletePrintJobs       = OFFalse;
static OFBool         deleteTerminateJobs   = OFFalse;
static OFBool         useTLS                = OFFalse;

/* helper class printJob */

class printJob
{
public:
  printJob();
  printJob(const printJob& copy);
  ~printJob() { }

  OFString studyUID;
  OFString seriesUID;
  OFString instanceUID;
  OFString storedPrintFilename;
  OFString mediumType;
  OFString filmDestination;
  OFString filmSessionLabel;
  OFString printPriority;
  OFString ownerID;

#ifdef ALLOW_ILLUMINATION_OVERRIDE
  unsigned long illumination;
  unsigned long reflectedAmbientLight;
#endif

  unsigned long numberOfCopies;
private:
  /* undefined */ printJob& operator=(const printJob& copy);
};

printJob::printJob()
: studyUID()
, seriesUID()
, instanceUID()
, storedPrintFilename()
, mediumType()
, filmDestination()
, filmSessionLabel()
, printPriority()
, ownerID()
#ifdef ALLOW_ILLUMINATION_OVERRIDE
, illumination((unsigned long)-1)
, reflectedAmbientLight((unsigned long)-1)
#endif
, numberOfCopies(0)
{
}

printJob::printJob(const printJob& copy)
: studyUID(copy.studyUID)
, seriesUID(copy.seriesUID)
, instanceUID(copy.instanceUID)
, storedPrintFilename(copy.storedPrintFilename)
, mediumType(copy.mediumType)
, filmDestination(copy.filmDestination)
, filmSessionLabel(copy.filmSessionLabel)
, printPriority(copy.printPriority)
, ownerID(copy.ownerID)
#ifdef ALLOW_ILLUMINATION_OVERRIDE
, illumination(copy.illumination)
, reflectedAmbientLight(copy.reflectedAmbientLight)
#endif
, numberOfCopies(copy.numberOfCopies)
{
}

/* static helper functions */

static OFCondition spoolStoredPrintFile(
  const char *filename,
  DVInterface &dvi,
  DcmTransportLayer *tlayer)
{
  DcmFileFormat *ffile = NULL;
  DcmDataset *dset = NULL;

  if (opt_spoolMode)
  {
    OFLOG_WARN(dcmprscuLogger, OFDateTime::getCurrentDateTime() << OFendl << "processing " << filename);
  }

  if (filename==NULL) return EC_IllegalCall;
  OFCondition result = DVPSHelper::loadFileFormat(filename, ffile);
  if (EC_Normal != result)
  {
    OFLOG_ERROR(dcmprscuLogger, "spooler: unable to load file '" << filename << "'");
  }
  if (ffile) dset = ffile->getDataset();

  DVPSStoredPrint& stprint = dvi.getPrintHandler();

  if (EC_Normal == result)
  {
    if (dset) result = stprint.read(*dset); else result = EC_IllegalCall;
  }
  if (EC_Normal != result)
  {
    OFLOG_ERROR(dcmprscuLogger, "spooler: file '" << filename << "' is not a valid Stored Print object");
  }
  delete ffile;

  if (EC_Normal == result)
  {
    // we have successfully read the Stored Print, now open connection to printer
    DVPSPrintMessageHandler printHandler;

    result = printHandler.negotiateAssociation(
      tlayer, dvi.getNetworkAETitle(),
      targetAETitle, targetHostname, targetPort, targetMaxPDU,
      targetSupportsPLUT, targetSupportsAnnotation, targetImplicitOnly);

    if (result.bad())
    {
      OFString temp_str;
      OFLOG_ERROR(dcmprscuLogger, "spooler: connection setup with printer failed\n" << DimseCondition::dump(temp_str, result));
    } else {
      if (EC_Normal != (result = stprint.printSCUgetPrinterInstance(printHandler)))
      {
        OFLOG_ERROR(dcmprscuLogger, "spooler: printer communication failed, unable to request printer settings");
      }
      if (EC_Normal==result) if (EC_Normal != (result = stprint.printSCUpreparePresentationLUT(
        printHandler, targetRequiresMatchingLUT, targetPreferSCPLUTRendering, targetSupports12bit)))
      {
        OFLOG_ERROR(dcmprscuLogger, "spooler: printer communication failed, unable to create presentation LUT");
      }
      if (EC_Normal==result) if (EC_Normal != (result = dvi.printSCUcreateBasicFilmSession(printHandler, targetPLUTinFilmSession)))
      {
        OFLOG_ERROR(dcmprscuLogger, "spooler: printer communication failed, unable to create basic film session");
      }
      if (EC_Normal==result) if (EC_Normal != (result = stprint.printSCUcreateBasicFilmBox(printHandler, targetPLUTinFilmSession)))
      {
        OFLOG_ERROR(dcmprscuLogger, "spooler: printer communication failed, unable to create basic film box");
      }
      // Process images
      size_t numberOfImages = stprint.getNumberOfImages();
      const char *studyUID = NULL;
      const char *seriesUID = NULL;
      const char *instanceUID = NULL;
      const char *imagefile = NULL;
      OFString theFilename;
      DicomImage *dcmimage = NULL;
      for (size_t currentImage=0; currentImage<numberOfImages; currentImage++)
      {
        if (EC_Normal == result) result = stprint.getImageReference(currentImage, studyUID, seriesUID, instanceUID);
        if ((EC_Normal == result) && studyUID && seriesUID && instanceUID)
        {
          imagefile = dvi.getFilename(studyUID, seriesUID, instanceUID);
          if (imagefile) theFilename = imagefile; else theFilename.clear();
          dvi.releaseDatabase(); // destroys the string imagefile points to
          if (theFilename.size() > 0)
          {
            dcmimage = new DicomImage(theFilename.c_str());
            if (dcmimage && (EIS_Normal == dcmimage->getStatus()))
            {
              // N-SET basic image box
              if (EC_Normal != (result = stprint.printSCUsetBasicImageBox(printHandler, currentImage, *dcmimage, opt_Monochrome1)))
              {
                OFLOG_ERROR(dcmprscuLogger, "spooler: printer communication failed, unable to transmit basic grayscale image box");
              }
            } else {
              result = EC_IllegalCall;
              OFLOG_ERROR(dcmprscuLogger, "spooler: unable to load image file '" << theFilename.c_str() << "'");
            }
            delete dcmimage;
          } else {
            result = EC_IllegalCall;
            OFLOG_ERROR(dcmprscuLogger, "spooler: unable to locate image file in database");
          }
        } else result = EC_IllegalCall;
      }

      size_t numberOfAnnotations = stprint.getNumberOfAnnotations();
      for (size_t currentAnnotation=0; currentAnnotation<numberOfAnnotations; currentAnnotation++)
      {
        // N-SET basic annotation box
        if (EC_Normal == result)
        {
          if (EC_Normal != (result = stprint.printSCUsetBasicAnnotationBox(printHandler, currentAnnotation)))
          {
            OFLOG_ERROR(dcmprscuLogger, "spooler: printer communication failed, unable to transmit basic annotation box");
          }
        }
      }

      if (! opt_noPrint)
      {
        if (opt_sessionPrint)
        {
          if (EC_Normal==result) if (EC_Normal != (result = stprint.printSCUprintBasicFilmSession(printHandler)))
          {
            OFLOG_ERROR(dcmprscuLogger, "spooler: printer communication failed, unable to print (at film session level)");
          }
        } else {
          if (EC_Normal==result) if (EC_Normal != (result = stprint.printSCUprintBasicFilmBox(printHandler)))
          {
            OFLOG_ERROR(dcmprscuLogger, "spooler: printer communication failed, unable to print");
          }
        }
      }
      if (EC_Normal==result) if (EC_Normal != (result = stprint.printSCUdelete(printHandler)))
      {
        OFLOG_ERROR(dcmprscuLogger, "spooler: printer communication failed, unable to delete print objects");
      }

      result = printHandler.releaseAssociation();
      if (result.bad())
      {
        OFString temp_str;
        OFLOG_ERROR(dcmprscuLogger, "spooler: release of connection to printer failed\n" << DimseCondition::dump(temp_str, result));
        if (EC_Normal == result) result = EC_IllegalCall;
      }
    }
  }
  return result;
}

static OFCondition spoolJobList(
  OFList<printJob *>&jobList,
  DVInterface &dvi,
  DcmTransportLayer *tlayer)
{
  OFCondition result = EC_Normal;
  OFCondition result2 = EC_Normal;
  OFListIterator(printJob *) first = jobList.begin();
  OFListIterator(printJob *) last = jobList.end();
  printJob *currentJob = NULL;
  while (first != last)
  {
    currentJob = *first;
    first = jobList.erase(first);
    if (currentJob->storedPrintFilename.size() == 0)
    {
      const char *spfile = dvi.getFilename(currentJob->studyUID.c_str(), currentJob->seriesUID.c_str(), currentJob->instanceUID.c_str());
      if (spfile) currentJob->storedPrintFilename = spfile;
      dvi.releaseDatabase(); // destroys the string spfile points to
    }
    if (currentJob->storedPrintFilename.size() > 0)
    {
      // initialize film session settings
      dvi.clearFilmSessionSettings();
      if (currentJob->mediumType.size() > 0) dvi.setPrinterMediumType(currentJob->mediumType.c_str());
      if (currentJob->filmDestination.size() > 0) dvi.setPrinterFilmDestination(currentJob->filmDestination.c_str());
      if (currentJob->filmSessionLabel.size() > 0) dvi.setPrinterFilmSessionLabel(currentJob->filmSessionLabel.c_str());
      if (currentJob->printPriority.size() > 0) dvi.setPrinterPriority(currentJob->printPriority.c_str());
      if (currentJob->ownerID.size() > 0) dvi.setPrinterOwnerID(currentJob->ownerID.c_str());
      if (currentJob->numberOfCopies > 0) dvi.setPrinterNumberOfCopies(currentJob->numberOfCopies);
#ifdef ALLOW_ILLUMINATION_OVERRIDE
      if (currentJob->illumination != (unsigned long)-1) dvi.setPrintIllumination((Uint16)(currentJob->illumination));
      if (currentJob->reflectedAmbientLight != (unsigned long)-1) dvi.setPrintReflectedAmbientLight((Uint16)(currentJob->reflectedAmbientLight));
#endif
      result2 = spoolStoredPrintFile(currentJob->storedPrintFilename.c_str(), dvi, tlayer);
      if (result2 != EC_Normal)
      {
        OFLOG_ERROR(dcmprscuLogger, "spooler: error occured during spooling of Stored Print object '" << currentJob->storedPrintFilename << "'");
      }
      if (result == EC_Normal) result = result2; // forward error codes, but do not erase
    } else {
      OFLOG_ERROR(dcmprscuLogger, "spooler: unable to find Stored Print object for print job in database");
      result = EC_IllegalCall;
    }
    delete currentJob;
  }
  return result;
}

/* reads a single line of text from an open input file.
 * the line must have the structure <whitespace> <keyword> <whitespace> <value>
 * Trailing whitespace is not removed.
 */
static OFBool readValuePair(FILE *infile, OFString& key, OFString& value)
{
  int c;
  int mode = 0;
  key.clear();
  value.clear();
  key.reserve(32);  // should be sufficient for most cases.
  value.reserve(256);
  OFBool finished = OFFalse;
  while (!finished)
  {
    c = fgetc(infile);
    if ((c==EOF)||(c==13)||(c==10)) finished = OFTrue;
    else if (isspace(c))
    {
      if (mode==1) mode=2;
      else if (mode==3) value += (char)c;
    } else {
      if (mode < 2)
      {
        mode=1;
        key += (char)c;
      }
      else
      {
        mode=3;
        value += (char)c;
      }
    }
  }
  if (c==EOF) return OFTrue; else return OFFalse;
}

/* reads a complete "print job" file.
 * The file must either contain a complete UID specification (study/series/instance)
 * or it must contain the keyword "terminate" in which case everything else is ignored.
 * depending on the "deletePrintJob" flag, the job file is renamed to "outfile"
 * after reading or deleted. If renaming fails, the file is also deleted.
 */
static OFCondition readJobFile(
  const char *infile,
  const char *outfile,
  printJob& job,
  OFBool& terminateFlag)
{
  if (infile==NULL) return EC_IllegalCall;
  FILE *inf = fopen(infile, "rb");
  if (inf==NULL) return EC_IllegalCall;
  OFString key, value;
  OFBool eofFound = OFFalse;
  OFBool thisIsTerminate = OFFalse;
  OFCondition result = EC_Normal;
  while (!eofFound)
  {
      thisIsTerminate = OFFalse;
    eofFound = readValuePair(inf, key, value);
    if ((key.size()==0)||(key[0] == '#')) { /* ignore comments and empty lines */ }
    else if (key == "copies")
    {
      if (1 != sscanf(value.c_str(),"%lu", &job.numberOfCopies))
      {
        OFLOG_ERROR(dcmprscuLogger, "spooler: parse error for 'copies' in job file '" << infile << "'");
        result = EC_IllegalCall;
      }
    }
#ifdef ALLOW_ILLUMINATION_OVERRIDE
    else if (key == "illumination")
    {
      if (1 != sscanf(value.c_str(),"%lu", &job.illumination))
      {
        OFLOG_ERROR(dcmprscuLogger, "spooler: parse error for 'illumination' in job file '" << infile << "'");
        result = EC_IllegalCall;
      }
    }
    else if (key == "reflection")
    {
      if (1 != sscanf(value.c_str(),"%lu", &job.reflectedAmbientLight))
      {
        OFLOG_ERROR(dcmprscuLogger, "spooler: parse error for 'reflection' in job file '" << infile << "'");
        result = EC_IllegalCall;
      }
    }
#endif
    else if (key == "mediumtype") job.mediumType = value;
    else if (key == "destination") job.filmDestination = value;
    else if (key == "label") job.filmSessionLabel = value;
    else if (key == "priority") job.printPriority = value;
    else if (key == "owner_id") job.ownerID = value;
    else if (key == "study") job.studyUID = value;
    else if (key == "series") job.seriesUID = value;
    else if (key == "instance") job.instanceUID = value;
    else if (key == "terminate")
    {
      terminateFlag=OFTrue;
      thisIsTerminate = OFTrue;
    }
    else
    {
      OFLOG_ERROR(dcmprscuLogger, "spooler: unknown keyword '" << key.c_str() << "' in job file '" << infile << "'");
      result = EC_IllegalCall;
    }
  }
  fclose(inf);

  if (deletePrintJobs || (deleteTerminateJobs && thisIsTerminate))
  {
    if (0 != unlink(infile))
    {
      OFLOG_ERROR(dcmprscuLogger, "spooler: unable to delete job file '" << infile << "'");
      result = EC_IllegalCall;
    }
  } else {
    if ((outfile==NULL)||(0 != rename(infile, outfile)))
    {
      // if we can't rename, we delete to make sure we don't read the same file again next time.
      if (0 != unlink(infile))
      {
        OFLOG_ERROR(dcmprscuLogger, "spooler: unable to delete job file '" << infile << "'");
        result = EC_IllegalCall;
      }
    }
  }

  // make sure that either all mandatory parameters are set or "terminate" is defined.
  if ((EC_Normal==result)&&(! terminateFlag)&&((job.studyUID.size()==0)||(job.seriesUID.size()==0)||(job.instanceUID.size()==0)))
  {
    OFLOG_ERROR(dcmprscuLogger, "spooler: UIDs missing in job file '" << infile << "'");
    result = EC_IllegalCall;
  }
  return result;
}

/* browses the spool directory for new print job files and reads/renames all of them.
 * The results are stored in the jobList but not processed (spooled). This avoids
 * the possibility of life-lock situations.
 *
 * Uses opendir() on Unix, FindFirstFile()/FindNextFile() on Win32 platforms
 */
static OFCondition updateJobList(
  OFList<printJob *>&jobList,
  DVInterface &dvi,
  OFBool& terminateFlag,
  const char *filenamePrefix)
{
  if (filenamePrefix==NULL) return EC_IllegalCall;
  OFString prefix = filenamePrefix;
  OFString postfix = PRINTJOB_SUFFIX;
  size_t prefixSize = prefix.size();
  size_t postfixSize = postfix.size();
  OFString currentName;
  OFString jobName;
  OFString renameName;
  printJob *currentJob = NULL;
  OFBool currentTerminate = OFFalse;
  OFCondition result = EC_Normal;
  const char *spoolFolder = dvi.getSpoolFolder();

#ifdef HAVE_WINDOWS_H
  WIN32_FIND_DATA stWin32FindData;
  OFString currentdir = spoolFolder;
  currentdir += "\\*";

  HANDLE hFile = FindFirstFile(currentdir.c_str(), &stWin32FindData);
  int ret = (hFile != INVALID_HANDLE_VALUE);
  while (ret)
  {
      currentName = stWin32FindData.cFileName;
#else
  DIR *dirp = NULL;
  struct dirent *dp = NULL;
  if ((dirp = opendir(spoolFolder)) != NULL)
  {
    for (dp=readdir(dirp);((result == EC_Normal)&&(dp != NULL)); dp=readdir(dirp))
    {
      currentName = dp->d_name;
#endif
      if ((prefix == currentName.substr(0, prefixSize)) &&
          (currentName.size() > postfixSize) &&
          (postfix == currentName.substr(currentName.size()-postfixSize)))
      {
        // name matches pattern
        jobName = spoolFolder;
        jobName += PATH_SEPARATOR;
        jobName += currentName;
        renameName = jobName;
        renameName += PRINTJOB_DONE_SUFFIX;
        currentJob = new printJob();
        if (currentJob)
        {
          currentTerminate = OFFalse;
          result = readJobFile(jobName.c_str(), renameName.c_str(), *currentJob, currentTerminate);
          if (currentTerminate) terminateFlag = OFTrue;
          if (EC_Normal == result)
          {
            // don't schedule "terminate" job
            if (!currentTerminate) jobList.push_back(currentJob); else delete currentJob;
          }
          else
          {
            delete currentJob;
            OFLOG_ERROR(dcmprscuLogger, "spooler: parsing of job file '" << jobName.c_str() << "' failed");
          }
        } else result = EC_MemoryExhausted;
      }

#ifdef HAVE_WINDOWS_H
      ret = FindNextFile(hFile, &stWin32FindData);
  } /* while */
  if(hFile != INVALID_HANDLE_VALUE)
  {
    FindClose(hFile);
#else
    } /* for */
    closedir(dirp);
#endif
  } else {
    OFLOG_ERROR(dcmprscuLogger, "unable to read spool directory '" << spoolFolder << "'");
    result = EC_IllegalCall;
  }
  return result;
}

#define SHORTCOL 3
#define LONGCOL 15

int main(int argc, char *argv[])
{

#ifdef HAVE_GUSI_H
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

#ifdef HAVE_WINSOCK_H
    WSAData winSockData;
    /* we need at least version 1.1 */
    WORD winSockVersionNeeded = MAKEWORD( 1, 1 );
    WSAStartup(winSockVersionNeeded, &winSockData);
#endif

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Print spooler for presentation state viewer", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 2);

    cmd.addParam("dcmfile-in", "stored print file(s) to be spooled", OFCmdParam::PM_MultiOptional);

    cmd.addGroup("general options:");
     cmd.addOption("--help",        "-h",    "print this help text and exit", OFCommandLine::AF_Exclusive);
     cmd.addOption("--version",              "print version information and exit", OFCommandLine::AF_Exclusive);
     OFLog::addOptions(cmd);
    cmd.addGroup("print options:");
     cmd.addOption("--noprint",              "do not create print-out (no n-action-rq)");
     cmd.addOption("--session-print",        "send film session n-action-rq (instead of film box)");
     cmd.addOption("--monochrome1",          "transmit basic grayscale images in MONOCHROME1");
    cmd.addGroup("mode options:");
     cmd.addOption("--print",       "+p",    "printer mode, print file(s) and terminate (default)");
     cmd.addOption("--spool",       "+s", 1, "[n]ame: string",
                                             "spooler mode, use job prefix n");
    cmd.addGroup("processing options:");
     cmd.addOption("--config",      "-c", 1, "[f]ilename: string",
                                             "process using settings from configuration file");
     cmd.addOption("--printer",     "-p", 1, "[n]ame: string (default: 1st printer in cfg file)",
                                             "select printer with identifier n from cfg file");
     cmd.addOption("--dump",        "+d",    "dump all DIMSE messages");
    cmd.addGroup("spooler options (only with --spool):");
     cmd.addOption("--sleep",             1, "[d]elay: integer (default: 1)",
                                             "sleep d seconds between spooler checks");
    cmd.addGroup("basic film session options (not with --spool):");
     cmd.addOption("--copies",            1, "[v]alue: integer (1..100, default: 1)",
                                             "set number of copies to v");
     cmd.addOption("--medium-type",       1, "[v]alue: string",
                                             "set medium type to v");
     cmd.addOption("--destination",       1, "[v]alue: string",
                                             "set film destination to v");
     cmd.addOption("--label",             1, "[v]alue: string",
                                             "set film session label to v");
     cmd.addOption("--priority",          1, "[v]alue: string",
                                             "set print priority to v");
     cmd.addOption("--owner",             1, "[v]alue: string",
                                             "set film session owner ID to v");

    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::PF_ExpandWildcards))
    {
      /* check exclusive options first */
      if (cmd.hasExclusiveOption())
      {
        if (cmd.findOption("--version"))
        {
            app.printHeader(OFTrue /*print host identifier*/);
            COUT << OFendl << "External libraries used:";
#if !defined(WITH_ZLIB) && !defined(WITH_OPENSSL)
            COUT << " none" << OFendl;
#else
            COUT << OFendl;
#endif
#ifdef WITH_ZLIB
            COUT << "- ZLIB, Version " << zlibVersion() << OFendl;
#endif
#ifdef WITH_OPENSSL
            COUT << "- " << OPENSSL_VERSION_TEXT << OFendl;
#endif
            return 0;
         }
      }

      /* options */
      OFLog::configureFromCommandLine(cmd, app);

      if (cmd.findOption("--noprint")) opt_noPrint = OFTrue;
      if (cmd.findOption("--session-print")) opt_sessionPrint = OFTrue;
      if (cmd.findOption("--monochrome1")) opt_Monochrome1 = OFTrue;

      cmd.beginOptionBlock();
      if (cmd.findOption("--print")) opt_spoolMode = OFFalse;
      if (cmd.findOption("--spool"))
      {
        opt_spoolMode=OFTrue;
        app.checkValue(cmd.getValue(opt_spoolPrefix));
      }
      cmd.endOptionBlock();

      if (cmd.findOption("--config")) app.checkValue(cmd.getValue(opt_cfgName));
      if (cmd.findOption("--printer")) app.checkValue(cmd.getValue(opt_printer));
      if (cmd.findOption("--dump"))
      {
        // Messages to the "dump" logger are always written with the debug log
        // level, thus enabling that logger for this level shows the dumps
        log4cplus::Logger log = log4cplus::Logger::getInstance("dcmtk.dcmpstat.dump");
        log.setLogLevel(OFLogger::DEBUG_LOG_LEVEL);
      }

      if (cmd.findOption("--medium-type"))
      {
        app.checkConflict("--medium-type", "--spool", opt_spoolMode);
        app.checkValue(cmd.getValue(opt_mediumtype));
      }

      if (cmd.findOption("--destination"))
      {
        app.checkConflict("--destination", "--spool", opt_spoolMode);
        app.checkValue(cmd.getValue(opt_destination));
      }
      if (cmd.findOption("--label"))
      {
        app.checkConflict("--label", "--spool", opt_spoolMode);
        app.checkValue(cmd.getValue(opt_sessionlabel));
      }
      if (cmd.findOption("--priority"))
      {
        app.checkConflict("--priority", "--spool", opt_spoolMode);
        app.checkValue(cmd.getValue(opt_priority));
      }
      if (cmd.findOption("--owner"))
      {
        app.checkConflict("--owner", "--spool", opt_spoolMode);
        app.checkValue(cmd.getValue(opt_ownerID));
      }
      if (cmd.findOption("--copies"))
      {
        app.checkConflict("--copies", "--spool", opt_spoolMode);
        app.checkValue(cmd.getValueAndCheckMinMax(opt_copies, 1, 100));
      }
      if (cmd.findOption("--sleep"))
      {
        app.checkConflict("--sleep", "--print", (! opt_spoolMode));
        app.checkValue(cmd.getValue(opt_sleep));
      }
    }

    /* print resource identifier */
    OFLOG_DEBUG(dcmprscuLogger, rcsid << OFendl);

    if (opt_cfgName)
    {
      FILE *cfgfile = fopen(opt_cfgName, "rb");
      if (cfgfile) fclose(cfgfile); else
      {
        OFLOG_FATAL(dcmprscuLogger, "can't open configuration file '" << opt_cfgName << "'");
        return 10;
      }
    } else {
        OFLOG_FATAL(dcmprscuLogger, "no configuration file specified");
        return 10;
    }

    DVInterface dvi(opt_cfgName);
    if (opt_printer)
    {
      if (EC_Normal != dvi.setCurrentPrinter(opt_printer))
      {
        OFLOG_FATAL(dcmprscuLogger, "unable to select printer '" << opt_printer << "'");
        return 10;
      }
    } else {
      opt_printer = dvi.getCurrentPrinter(); // use default printer
      if (opt_printer==NULL)
      {
        OFLOG_FATAL(dcmprscuLogger, "no default printer available - no config file?");
        return 10;
      }
    }

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
      OFLOG_WARN(dcmprscuLogger, "no data dictionary loaded, check environment variable: " << DCM_DICT_ENVIRONMENT_VARIABLE);

    /* get print target from configuration file */
    targetHostname              = dvi.getTargetHostname(opt_printer);
    targetDescription           = dvi.getTargetDescription(opt_printer);
    targetAETitle               = dvi.getTargetAETitle(opt_printer);
    targetPort                  = dvi.getTargetPort(opt_printer);
    targetMaxPDU                = dvi.getTargetMaxPDU(opt_printer);
    targetImplicitOnly          = dvi.getTargetImplicitOnly(opt_printer);
    targetDisableNewVRs         = dvi.getTargetDisableNewVRs(opt_printer);
    targetSupportsPLUT          = dvi.getTargetPrinterSupportsPresentationLUT(opt_printer);
    targetSupportsAnnotation    = dvi.getTargetPrinterSupportsAnnotationBoxSOPClass(opt_printer);
    targetSupports12bit         = dvi.getTargetPrinterSupports12BitTransmission(opt_printer);
    targetPLUTinFilmSession     = dvi.getTargetPrinterPresentationLUTinFilmSession(opt_printer);
    targetRequiresMatchingLUT   = dvi.getTargetPrinterPresentationLUTMatchRequired(opt_printer);
    targetPreferSCPLUTRendering = dvi.getTargetPrinterPresentationLUTPreferSCPRendering(opt_printer);
    deletePrintJobs             = dvi.getSpoolerDeletePrintJobs();
    deleteTerminateJobs         = dvi.getSpoolerAlwaysDeleteTerminateJobs();
    useTLS                      = dvi.getTargetUseTLS(opt_printer);

    Sint32 timeout = dvi.getTargetTimeout(opt_printer);
    if (timeout > 0) dcmConnectionTimeout.set(timeout);

#ifdef WITH_OPENSSL
    /* TLS directory */
    const char *current = NULL;
    const char *tlsFolder = dvi.getTLSFolder();
    if (tlsFolder==NULL) tlsFolder = ".";

    /* certificate file */
    OFString tlsCertificateFile(tlsFolder);
    tlsCertificateFile += PATH_SEPARATOR;
    current = dvi.getTargetCertificate(opt_printer);
    if (current) tlsCertificateFile += current; else tlsCertificateFile.clear();

    /* private key file */
    OFString tlsPrivateKeyFile(tlsFolder);
    tlsPrivateKeyFile += PATH_SEPARATOR;
    current = dvi.getTargetPrivateKey(opt_printer);
    if (current) tlsPrivateKeyFile += current; else tlsPrivateKeyFile.clear();

    /* private key password */
    const char *tlsPrivateKeyPassword = dvi.getTargetPrivateKeyPassword(opt_printer);

    /* certificate verification */
    DcmCertificateVerification tlsCertVerification = DCV_requireCertificate;
    switch (dvi.getTargetPeerAuthentication(opt_printer))
    {
      case DVPSQ_require:
        tlsCertVerification = DCV_requireCertificate;
        break;
      case DVPSQ_verify:
        tlsCertVerification = DCV_checkCertificate;
        break;
      case DVPSQ_ignore:
        tlsCertVerification = DCV_ignoreCertificate;
        break;
    }

    /* DH parameter file */
    OFString tlsDHParametersFile;
    current = dvi.getTargetDiffieHellmanParameters(opt_printer);
    if (current)
    {
      tlsDHParametersFile = tlsFolder;
      tlsDHParametersFile += PATH_SEPARATOR;
      tlsDHParametersFile += current;
    }

    /* random seed file */
    OFString tlsRandomSeedFile(tlsFolder);
    tlsRandomSeedFile += PATH_SEPARATOR;
    current = dvi.getTargetRandomSeed(opt_printer);
    if (current) tlsRandomSeedFile += current; else tlsRandomSeedFile += "siteseed.bin";

    /* CA certificate directory */
    const char *tlsCACertificateFolder = dvi.getTLSCACertificateFolder();
    if (tlsCACertificateFolder==NULL) tlsCACertificateFolder = ".";

    /* key file format */
    int keyFileFormat = SSL_FILETYPE_PEM;
    if (! dvi.getTLSPEMFormat()) keyFileFormat = SSL_FILETYPE_ASN1;

    /* ciphersuites */
#if OPENSSL_VERSION_NUMBER >= 0x0090700fL
    OFString tlsCiphersuites(TLS1_TXT_RSA_WITH_AES_128_SHA ":" SSL3_TXT_RSA_DES_192_CBC3_SHA);
#else
    OFString tlsCiphersuites(SSL3_TXT_RSA_DES_192_CBC3_SHA);
#endif
    Uint32 tlsNumberOfCiphersuites = dvi.getTargetNumberOfCipherSuites(opt_printer);
    if (tlsNumberOfCiphersuites > 0)
    {
      tlsCiphersuites.clear();
      OFString currentSuite;
      const char *currentOpenSSL;
      for (Uint32 ui=0; ui<tlsNumberOfCiphersuites; ui++)
      {
        dvi.getTargetCipherSuite(opt_printer, ui, currentSuite);
        if (NULL == (currentOpenSSL = DcmTLSTransportLayer::findOpenSSLCipherSuiteName(currentSuite.c_str())))
        {
          OFLOG_FATAL(dcmprscuLogger, "ciphersuite '" << currentSuite << "' is unknown. Known ciphersuites are:");
          unsigned long numSuites = DcmTLSTransportLayer::getNumberOfCipherSuites();
          for (unsigned long cs=0; cs < numSuites; cs++)
          {
            OFLOG_FATAL(dcmprscuLogger, "    " << DcmTLSTransportLayer::getTLSCipherSuiteName(cs));
          }
          return 1;
        } else {
          if (tlsCiphersuites.length() > 0) tlsCiphersuites += ":";
          tlsCiphersuites += currentOpenSSL;
        }
      }
    }

    DcmTLSTransportLayer *tLayer = NULL;
    if (useTLS)
    {
      tLayer = new DcmTLSTransportLayer(DICOM_APPLICATION_REQUESTOR, tlsRandomSeedFile.c_str());
      if (tLayer == NULL)
      {
        OFLOG_FATAL(dcmprscuLogger, "unable to create TLS transport layer");
        return 1;
      }

      if (tlsCACertificateFolder && (TCS_ok != tLayer->addTrustedCertificateDir(tlsCACertificateFolder, keyFileFormat)))
      {
        OFLOG_WARN(dcmprscuLogger, "unable to load certificates from directory '" << tlsCACertificateFolder << "', ignoring");
      }
      if ((tlsDHParametersFile.size() > 0) && ! (tLayer->setTempDHParameters(tlsDHParametersFile.c_str())))
      {
        OFLOG_WARN(dcmprscuLogger, "unable to load temporary DH parameter file '" << tlsDHParametersFile << "', ignoring");
      }
      tLayer->setPrivateKeyPasswd(tlsPrivateKeyPassword); // never prompt on console

      if ((tlsPrivateKeyFile.length() > 0) && (tlsCertificateFile.length() > 0))
      {
        if (TCS_ok != tLayer->setPrivateKeyFile(tlsPrivateKeyFile.c_str(), keyFileFormat))
        {
          OFLOG_FATAL(dcmprscuLogger, "unable to load private TLS key from '" << tlsPrivateKeyFile<< "'");
          return 1;
        }
        if (TCS_ok != tLayer->setCertificateFile(tlsCertificateFile.c_str(), keyFileFormat))
        {
          OFLOG_FATAL(dcmprscuLogger, "unable to load certificate from '" << tlsCertificateFile << "'");
          return 1;
        }
        if (! tLayer->checkPrivateKeyMatchesCertificate())
        {
          OFLOG_FATAL(dcmprscuLogger, "private key '" << tlsPrivateKeyFile << "' and certificate '" << tlsCertificateFile << "' do not match");
          return 1;
        }
      }
      if (TCS_ok != tLayer->setCipherSuites(tlsCiphersuites.c_str()))
      {
        OFLOG_FATAL(dcmprscuLogger, "unable to set selected cipher suites");
        return 1;
      }

      tLayer->setCertificateVerification(tlsCertVerification);

      // a generated UID contains the process ID and current time.
      // Adding it to the PRNG seed guarantees that we have different seeds for different processes.
      char randomUID[65];
      dcmGenerateUniqueIdentifier(randomUID);
      tLayer->addPRNGseed(randomUID, strlen(randomUID));
    }

#else
    DcmTransportLayer *tLayer = NULL;
    if (useTLS)
    {
        OFLOG_FATAL(dcmprscuLogger, "not compiled with OpenSSL, cannot use TLS");
        return 10;
    }
#endif

    if (targetHostname == NULL)
    {
        OFLOG_FATAL(dcmprscuLogger, "no hostname for print target '" << opt_printer << "' - no config file?");
        return 10;
    }
    if (targetAETitle == NULL)
    {
        OFLOG_FATAL(dcmprscuLogger, "no aetitle for print target '" << opt_printer << "'");
        return 10;
    }
    if (targetPort == 0)
    {
        OFLOG_FATAL(dcmprscuLogger, "no or invalid port number for print target '" << opt_printer << "'");
        return 10;
    }
    if (targetMaxPDU == 0) targetMaxPDU = DEFAULT_MAXPDU;
    else if (targetMaxPDU > ASC_MAXIMUMPDUSIZE)
    {
        OFLOG_WARN(dcmprscuLogger, "max PDU size " << targetMaxPDU << " too big for print target '"
             << opt_printer << "', using default: " << DEFAULT_MAXPDU);
        targetMaxPDU = DEFAULT_MAXPDU;
    }
    if (targetDisableNewVRs)
    {
        dcmEnableUnknownVRGeneration.set(OFFalse);
        dcmEnableUnlimitedTextVRGeneration.set(OFFalse);
    }

    OFLOG_INFO(dcmprscuLogger, "Printer parameters for '" <<  opt_printer << "':");
    OFLOG_INFO(dcmprscuLogger, "  hostname      : " << targetHostname);
    OFLOG_INFO(dcmprscuLogger, "  port          : " << targetPort);
    OFLOG_INFO(dcmprscuLogger, "  description   : "
        << (targetDescription ? targetDescription : "(none)"));
    OFLOG_INFO(dcmprscuLogger, "  aetitle       : " << targetAETitle);
    OFLOG_INFO(dcmprscuLogger, "  max pdu       : " << targetMaxPDU);
    OFLOG_INFO(dcmprscuLogger, "  timeout       : " << timeout);
    if (targetImplicitOnly && targetDisableNewVRs)
      OFLOG_INFO(dcmprscuLogger, "  options       : implicit xfer syntax only, disable post-1993 VRs");
    else if (targetImplicitOnly)
      OFLOG_INFO(dcmprscuLogger, "  options       : implicit xfer syntax only");
    else if (targetDisableNewVRs)
      OFLOG_INFO(dcmprscuLogger, "  options       : disable post-1993 VRs");
    else
      OFLOG_INFO(dcmprscuLogger, "  options       : none");
    OFLOG_INFO(dcmprscuLogger, "  12-bit xfer   : " << (targetSupports12bit ? "supported" : "not supported"));
    OFLOG_INFO(dcmprscuLogger, "  present.lut   : " << (targetSupportsPLUT ? "supported" : "not supported"));
    OFLOG_INFO(dcmprscuLogger, "  annotation    : " << (targetSupportsAnnotation ? "supported" : "not supported"));
    OFLOG_INFO(dcmprscuLogger, "Spooler parameters:");
    OFLOG_INFO(dcmprscuLogger, "  mode          : " << (opt_spoolMode ? "spooler mode" : "printer mode"));
    if (opt_spoolMode) {
      OFLOG_INFO(dcmprscuLogger, "  sleep time    : " << opt_sleep);
    } else {
      OFLOG_INFO(dcmprscuLogger, "  copies        : " << opt_copies);
      OFLOG_INFO(dcmprscuLogger, "  medium        : " << (opt_mediumtype ? opt_mediumtype : "printer default"));
      OFLOG_INFO(dcmprscuLogger, "  destination   : " << (opt_destination ? opt_destination : "printer default"));
      OFLOG_INFO(dcmprscuLogger, "  label         : " << (opt_sessionlabel ? opt_sessionlabel : "printer default"));
      OFLOG_INFO(dcmprscuLogger, "  priority      : " << (opt_priority ? opt_priority : "printer default"));
      OFLOG_INFO(dcmprscuLogger, "  owner ID      : " << (opt_ownerID ? opt_ownerID : "printer default"));
    }
    OFLOG_INFO(dcmprscuLogger, "transport layer security parameters:");
    OFLOG_INFO(dcmprscuLogger, "  TLS           : " << (useTLS ? "enabled" : "disabled"));

#ifdef WITH_OPENSSL
    if (useTLS)
    {
      OFLOG_INFO(dcmprscuLogger, "  certificate   : " << tlsCertificateFile);
      OFLOG_INFO(dcmprscuLogger, "  key file      : " << tlsPrivateKeyFile);
      OFLOG_INFO(dcmprscuLogger, "  DH params     : " << tlsDHParametersFile);
      OFLOG_INFO(dcmprscuLogger, "  PRNG seed     : " << tlsRandomSeedFile);
      OFLOG_INFO(dcmprscuLogger, "  CA directory  : " << tlsCACertificateFolder);
      OFLOG_INFO(dcmprscuLogger, "  ciphersuites  : " << tlsCiphersuites);
      OFLOG_INFO(dcmprscuLogger, "  key format    : " << (keyFileFormat == SSL_FILETYPE_PEM ? "PEM" : "DER"));
      const char *verification;
      switch (tlsCertVerification)
      {
        case DCV_checkCertificate:
          verification = "verify";
          break;
        case DCV_ignoreCertificate:
          verification = "ignore";
          break;
        default:
          verification = "require";
          break;
      }
      OFLOG_INFO(dcmprscuLogger, "  cert verify   : " << verification);;
    }
#endif

   int paramCount = cmd.getParamCount();
   const char *currentParam = NULL;
   if (opt_spoolMode)
   {
      if (paramCount > 0)
      {
        OFLOG_WARN(dcmprscuLogger, "filenames specified on command line, will be ignored in spooler mode");
      }

      OFString jobNamePrefix = opt_spoolPrefix;
      jobNamePrefix += "_";
      jobNamePrefix += opt_printer;
      jobNamePrefix += "_";
      OFList<printJob *> jobList;
      OFBool terminateFlag = OFFalse;
      do
      {
        OFStandard::sleep((unsigned int)opt_sleep);
        if (EC_Normal != updateJobList(jobList, dvi, terminateFlag, jobNamePrefix.c_str()))
        {
          OFLOG_FATAL(dcmprscuLogger, "spooler: non recoverable error occured, terminating");
          return 10;
        }
        // static OFCondition updateJobList(jobList, dvi, terminateFlag, jobNamePrefix.c_str());
        if (EC_Normal != spoolJobList(jobList, dvi, tLayer)) { /* ignore */ }
      } while (! terminateFlag);
      OFLOG_INFO(dcmprscuLogger, "spooler is terminating, goodbye!");
   } else {
      // printer mode
      if (paramCount == 0)
      {
        OFLOG_WARN(dcmprscuLogger, "spooler: no stored print files specified - nothing to do");
      } else {
        dvi.clearFilmSessionSettings();
        if (opt_mediumtype) dvi.setPrinterMediumType(opt_mediumtype);
        if (opt_destination) dvi.setPrinterFilmDestination(opt_destination);
        if (opt_sessionlabel) dvi.setPrinterFilmSessionLabel(opt_sessionlabel);
        if (opt_priority) dvi.setPrinterPriority(opt_priority);
        if (opt_ownerID) dvi.setPrinterOwnerID(opt_ownerID);
        if (opt_copies > 0) dvi.setPrinterNumberOfCopies(opt_copies);
        for (int param=1; param <= paramCount; param++)
        {
          cmd.getParam(param, currentParam);
          if (currentParam)
          {
            OFLOG_INFO(dcmprscuLogger, "spooling file '" << currentParam << "'");
          }
          if (currentParam)
          {
            if (EC_Normal != spoolStoredPrintFile(currentParam, dvi, tLayer))
            {
              OFLOG_ERROR(dcmprscuLogger, "spooling of file '" << currentParam << "' failed");
            }
          } else {
            OFLOG_ERROR(dcmprscuLogger, "empty file name");
          }
        }
      }
   }

#ifdef WITH_OPENSSL
  if (tLayer)
  {
    if (tLayer->canWriteRandomSeed())
    {
      if (!tLayer->writeRandomSeed(tlsRandomSeedFile.c_str()))
      {
        OFLOG_ERROR(dcmprscuLogger, "cannot write back random seed file '" << tlsRandomSeedFile << "', ignoring");
      }
    } else {
      OFLOG_WARN(dcmprscuLogger, "cannot write back random seed, ignoring");
    }
  }
  delete tLayer;
#endif

#ifdef HAVE_WINSOCK_H
    WSACleanup();
#endif

#ifdef DEBUG
    dcmDataDict.clear();  /* useful for debugging with dmalloc */
#endif

    return 0;
}

/*
 * CVS/RCS Log:
 * $Log: dcmprscu.cc,v $
 * Revision 1.30  2010-10-14 13:13:45  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.29  2009-12-11 15:24:46  joergr
 * Moved command line option --dump to another section.
 * Changed description of command line option --dump.
 *
 * Revision 1.28  2009-11-27 10:51:39  joergr
 * Fixed various issues with syntax usage (e.g. layout and formatting).
 * Sightly modifed log messages.
 *
 * Revision 1.27  2009-11-24 14:12:56  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.26  2009-10-28 09:53:41  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.25  2008-09-25 16:30:24  joergr
 * Added support for printing the expanded command line arguments.
 * Always output the resource identifier of the command line tool in debug mode.
 *
 * Revision 1.24  2006/08/15 16:57:01  meichel
 * Updated the code in module dcmpstat to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.23  2006/07/27 14:37:16  joergr
 * Changed parameter "exclusive" of method addOption() from type OFBool into an
 * integer parameter "flags". Prepended prefix "PF_" to parseLine() flags.
 * Option "--help" is no longer an exclusive option by default.
 * Made naming conventions for command line parameters more consistent, e.g.
 * used "dcmfile-in", "dcmfile-out" and "bitmap-out".
 *
 * Revision 1.22  2005/12/08 15:46:06  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.21  2005/11/28 15:29:05  meichel
 * File dcdebug.h is not included by any other header file in the toolkit
 *   anymore, to minimize the risk of name clashes of macro debug().
 *
 * Revision 1.20  2005/11/23 16:10:32  meichel
 * Added support for AES ciphersuites in TLS module. All TLS-enabled
 *   tools now support the "AES TLS Secure Transport Connection Profile".
 *
 * Revision 1.19  2004/02/04 15:44:38  joergr
 * Removed acknowledgements with e-mail addresses from CVS log.
 *
 * Revision 1.18  2003/09/05 14:31:32  meichel
 * Print SCU now supports TLS connections.
 *
 * Revision 1.17  2003/09/04 10:09:16  joergr
 * Fixed wrong use of OFBool/bool variable.
 *
 * Revision 1.16  2003/06/06 09:45:23  meichel
 * Added static sleep function in class OFStandard. This replaces the various
 *   calls to sleep(), Sleep() and usleep() throughout the toolkit.
 *
 * Revision 1.15  2002/11/29 13:16:27  meichel
 * Introduced new command line option --timeout for controlling the
 *   connection request timeout.
 *
 * Revision 1.14  2002/11/27 15:47:50  meichel
 * Adapted module dcmpstat to use of new header file ofstdinc.h
 *
 * Revision 1.13  2002/11/26 08:44:26  meichel
 * Replaced all includes for "zlib.h" with <zlib.h>
 *   to avoid inclusion of zlib.h in the makefile dependencies.
 *
 * Revision 1.12  2002/09/23 18:26:06  joergr
 * Added new command line option "--version" which prints the name and version
 * number of external libraries used (incl. preparation for future support of
 * 'config.guess' host identifiers).
 *
 * Revision 1.11  2002/06/14 10:44:17  meichel
 * Adapted log file handling to ofConsole singleton
 *
 * Revision 1.10  2002/04/16 14:01:26  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 *
 * Revision 1.9  2002/04/11 13:15:38  joergr
 * Replaced direct call of system routines by new standard date and time
 * functions.
 *
 * Revision 1.8  2001/11/09 16:06:04  joergr
 * Renamed some of the getValue/getParam methods to avoid ambiguities reported
 * by certain compilers.
 *
 * Revision 1.7  2001/10/12 13:46:48  meichel
 * Adapted dcmpstat to OFCondition based dcmnet module (supports strict mode).
 *
 * Revision 1.6  2001/09/26 15:36:02  meichel
 * Adapted dcmpstat to class OFCondition
 *
 * Revision 1.5  2001/06/01 15:50:07  meichel
 * Updated copyright header
 *
 * Revision 1.4  2000/12/13 13:23:02  meichel
 * Including dcompat.h, needed on MacOS X
 *
 * Revision 1.3  2000/06/20 14:50:05  meichel
 * Added monochrome1 printing mode.
 *
 * Revision 1.2  2000/06/19 16:29:05  meichel
 * Added options for session printing and LIN OD to print tools, fixed
 *   pixel aspect ratio related bug.
 *
 * Revision 1.1  2000/06/08 10:36:25  meichel
 * Renamed dcmprtsv to dcmprscu and adapted Makefile.in and Makefile.dep
 *
 * Revision 1.23  2000/06/06 09:44:07  joergr
 * Moved configuration file entry "LogDirectory" from "[PRINT]" to new
 * (more general) section "[APPLICATION]".
 *
 * Revision 1.22  2000/06/02 16:00:38  meichel
 * Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 * Revision 1.21  2000/05/31 13:02:24  meichel
 * Moved dcmpstat macros and constants into a common header file
 *
 * Revision 1.20  2000/05/03 14:27:27  meichel
 * Updated dcmpstat apps for changes in dcmimgle.
 *
 * Revision 1.19  2000/04/14 16:34:37  meichel
 * Global VR generation flags are now derived from OFGlobal and, thus,
 *   safe for use in multi-thread applications.
 *
 * Revision 1.18  2000/03/08 16:28:42  meichel
 * Updated copyright header.
 *
 * Revision 1.17  2000/03/06 18:21:45  joergr
 * Avoid empty statement in the body of if-statements (MSVC6 reports warnings).
 *
 * Revision 1.16  2000/03/03 14:13:26  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.15  2000/02/29 12:13:43  meichel
 * Removed support for VS value representation. This was proposed in CP 101
 *   but never became part of the standard.
 *
 * Revision 1.14  2000/02/02 15:17:59  meichel
 * Replaced some #if statements by more robust #ifdef
 *
 * Revision 1.13  2000/02/02 14:38:25  joergr
 * Removed space characters before preprocessor directives.
 *
 * Revision 1.12  1999/11/03 13:05:27  meichel
 * Added support for transmitting annotations in the film session label.
 *   Added support for dump tool launched from DVInterface.
 *
 * Revision 1.11  1999/10/28 08:18:54  meichel
 * Print client does not attempt any more to negotiate Presentation LUT or
 *   Annotation Box if config file says that the printer does not support them.
 *
 * Revision 1.10  1999/10/22 13:05:48  meichel
 * Print spooler now correctly dumping DIMSE communication to log file.
 *
 * Revision 1.9  1999/10/19 14:44:27  meichel
 * dcmprscu now correctly writes DIMSE dump to log file
 *   and deletes log file upon termination if no print job was processed.
 *
 * Revision 1.8  1999/10/13 14:11:53  meichel
 * Added config file entries and access methods
 *   for user-defined VOI presets, log directory, verbatim logging
 *   and an explicit list of image display formats for each printer.
 *
 * Revision 1.7  1999/10/07 17:21:41  meichel
 * Reworked management of Presentation LUTs in order to create tighter
 *   coupling between Softcopy and Print.
 *
 * Revision 1.6  1999/10/01 13:29:48  joergr
 * Added new option to config file: AlwaysDeleteTerminateJobs.
 *
 * Revision 1.5  1999/09/24 15:24:24  meichel
 * Added support for CP 173 (Presentation LUT clarifications)
 *
 * Revision 1.4  1999/09/23 17:37:08  meichel
 * Added support for Basic Film Session options to dcmpstat print code.
 *
 * Revision 1.3  1999/09/17 14:33:45  meichel
 * Completed print spool functionality including Supplement 22 support
 *
 * Revision 1.2  1999/09/15 17:42:50  meichel
 * Implemented print job dispatcher code for dcmpstat, adapted dcmprscu
 *   and dcmpsprt applications.
 *
 *
 */
