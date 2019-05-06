/*
 *  Copyright (C) 1999-2019, OFFIS e.V.
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
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmnet/dcompat.h"     /* compatibility code, needs to be included before dirent.h */

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
#include "dcmtk/ofstd/ofstring.h"      /* for OFString */
#include "dcmtk/ofstd/ofbmanip.h"      /* for OFBitmanipTemplate */
#include "dcmtk/ofstd/ofdatime.h"      /* for OFDateTime */
#include "dcmtk/dcmdata/dcuid.h"       /* for dcmtk version name */
#include "dcmtk/dcmdata/cmdlnarg.h"    /* for prepareCmdLineArgs */
#include "dcmtk/ofstd/ofconapp.h"      /* for OFConsoleApplication */
#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/dcmpstat/dvpspr.h"
#include "dcmtk/dcmpstat/dvpssp.h"
#include "dcmtk/dcmpstat/dvpshlp.h"    /* for class DVPSHelper */
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
        OFLOG_ERROR(dcmprscuLogger, "spooler: error occurred during spooling of Stored Print object '" << currentJob->storedPrintFilename << "'");
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
      if
      (
        currentName.size() > (prefixSize + postfixSize) &&
        prefix == currentName.substr(0, prefixSize) &&
        postfix == currentName.substr(currentName.size()-postfixSize) &&
        currentName.find_first_not_of("1234567890",prefixSize) == (currentName.size() - postfixSize)
      )
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
    OFStandard::initializeNetwork();
#ifdef WITH_OPENSSL
    DcmTLSTransportLayer::initializeOpenSSL();
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
    if (app.parseCommandLine(cmd, argc, argv))
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
            COUT << "- " << DcmTLSTransportLayer::getOpenSSLVersionName() << OFendl;
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
        dcmtk::log4cplus::Logger log = dcmtk::log4cplus::Logger::getInstance("dcmtk.dcmpstat.dump");
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
    DcmKeyFileFormat keyFileFormat = DCF_Filetype_PEM;
    if (! dvi.getTLSPEMFormat()) keyFileFormat = DCF_Filetype_ASN1;

    DcmTLSTransportLayer *tLayer = NULL;
    if (useTLS)
    {
      tLayer = new DcmTLSTransportLayer(NET_REQUESTOR, tlsRandomSeedFile.c_str(), OFFalse);
      if (tLayer == NULL)
      {
        OFLOG_FATAL(dcmprscuLogger, "unable to create TLS transport layer");
        return 1;
      }

      // determine TLS profile
      OFString profileName;
      const char *profileNamePtr = dvi.getTargetTLSProfile(opt_printer);
      if (profileNamePtr) profileName = profileNamePtr;
      DcmTLSSecurityProfile tlsProfile = TSP_Profile_BCP195;  // default
      if (profileName == "BCP195") tlsProfile = TSP_Profile_BCP195;
      else if (profileName == "BCP195-ND") tlsProfile = TSP_Profile_BCP195_ND;
      else if (profileName == "BCP195-EX") tlsProfile = TSP_Profile_BCP195_Extended;
      else if (profileName == "AES") tlsProfile = TSP_Profile_AES;
      else if (profileName == "BASIC") tlsProfile = TSP_Profile_Basic;
      else if (profileName == "NULL") tlsProfile = TSP_Profile_IHE_ATNA_Unencrypted;
      else
      {
        OFLOG_WARN(dcmprscuLogger, "unknown TLS profile '" << profileName << "', ignoring");
      }

      if (TCS_ok != tLayer->setTLSProfile(tlsProfile))
      {
        OFLOG_FATAL(dcmprscuLogger, "unable to select the TLS security profile");
        return 1;
      }

      // activate cipher suites
      if (TCS_ok != tLayer->activateCipherSuites())
      {
        OFLOG_FATAL(dcmprscuLogger, "unable to activate the selected list of TLS ciphersuites");
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

      if (!tlsPrivateKeyFile.empty() && !tlsCertificateFile.empty())
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
        dcmDisableGenerationOfNewVRs();
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
      OFString cslist;
      if (tLayer) tLayer->getListOfCipherSuitesForOpenSSL(cslist);
      OFLOG_INFO(dcmprscuLogger, "  certificate   : " << tlsCertificateFile);
      OFLOG_INFO(dcmprscuLogger, "  key file      : " << tlsPrivateKeyFile);
      OFLOG_INFO(dcmprscuLogger, "  DH params     : " << tlsDHParametersFile);
      OFLOG_INFO(dcmprscuLogger, "  PRNG seed     : " << tlsRandomSeedFile);
      OFLOG_INFO(dcmprscuLogger, "  CA directory  : " << tlsCACertificateFolder);
      OFLOG_INFO(dcmprscuLogger, "  ciphersuites  : " << cslist);
      OFLOG_INFO(dcmprscuLogger, "  key format    : " << (keyFileFormat == DCF_Filetype_PEM ? "PEM" : "DER"));
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

  OFStandard::shutdownNetwork();

#ifdef DEBUG
    dcmDataDict.clear();  /* useful for debugging with dmalloc */
#endif

    return 0;
}
