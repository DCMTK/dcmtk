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
 *  Module:  dcmprtsv
 *
 *  Authors: Andreas Thiel, Marco Eichelberg
 *
 *  Purpose: Presentation State Viewer - Print Spooler
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1999-09-23 17:37:08 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmpstat/apps/Attic/dcmprtsv.cc,v $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_GUSI_H
 #include <GUSI.h>
#endif

BEGIN_EXTERN_C
/* This #if code is suggested by the gnu autoconf documentation */
#if HAVE_DIRENT_H
# include <dirent.h>
# define NAMLEN(dirent) strlen((dirent)->d_name)
#else
# define dirent direct
# define NAMELEN(dirent) (dirent)->d_namlen
# if HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# if HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# if HAVE_NDIR_H
#  include <ndir.h>
# endif
#endif
#if HAVE_IO_H
#  include <io.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>    /* for O_RDONLY */
#endif
#ifdef HAVE_CTYPE_H
#include <ctype.h>    /* for isspace() */
#endif
END_EXTERN_C

#include "dviface.h"    /* for DVInterface */
#include "ofstring.h"   /* for OFString */
#include "ofbmanip.h"   /* for OFBitmanipTemplate */
#include "dcuid.h"      /* for dcmtk version name */
#include "cmdlnarg.h"   /* for prepareCmdLineArgs */
#include "ofconapp.h"   /* for OFConsoleApplication */
#include "dcmimage.h"   
#include "dvpspr.h"
#include "dvpssp.h"
#include "dvpshlp.h"     /* for class DVPSHelper */

#define OFFIS_CONSOLE_APPLICATION "dcmprtsv"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

/* default Max PDU size to be used when no different value is defined in the configuration file */
#define DEFAULT_MAXPDU 16384

/* command line options */
static OFBool           opt_verbose         = OFFalse;             /* default: not verbose */
static int              opt_debugMode       = 0;
static OFBool           opt_dumpMode        = OFFalse;
static OFBool           opt_spoolMode       = OFFalse;             /* default: file print mode */
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
static OFCmdUnsignedInt opt_illumination    = (OFCmdUnsignedInt)-1;
static OFCmdUnsignedInt opt_reflection      = (OFCmdUnsignedInt)-1;

/* print target data, taken from configuration file */
static const char *   targetHostname        = NULL;
static const char *   targetDescription     = NULL;
static const char *   targetAETitle         = NULL;
static unsigned short targetPort            = 104;
static unsigned long  targetMaxPDU          = ASC_DEFAULTMAXPDU;
static OFBool         targetImplicitOnly    = OFFalse;
static OFBool         targetDisableNewVRs   = OFFalse;
static OFBool         targetSupportsPLUT    = OFTrue;
static OFBool         targetSupports12bit   = OFTrue;
static OFBool         deletePrintJobs       = OFFalse;

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

  unsigned long illumination;
  unsigned long reflectedAmbientLight;
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
, illumination((unsigned long)-1)
, reflectedAmbientLight((unsigned long)-1)
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
, illumination(copy.illumination)
, reflectedAmbientLight(copy.reflectedAmbientLight)
, numberOfCopies(copy.numberOfCopies)
{
}

/* static helper functions */

static E_Condition spoolStoredPrintFile(const char *filename, DVInterface &dvi)
{
  DcmFileFormat *ffile = NULL;
  DcmDataset *dset = NULL;

  if (filename==NULL) return EC_IllegalCall;
  E_Condition result = DVPSHelper::loadFileFormat(filename, ffile);
  if (opt_verbose && (EC_Normal != result))
  {
    cerr << "spooler: unable to load file '" << filename << "'" << endl;
  }
  if (ffile) dset = ffile->getDataset(); 
  
  DVPSStoredPrint& stprint = dvi.getPrintHandler();

  if (EC_Normal == result)
  {
    if (dset) result = stprint.read(*dset); else result = EC_IllegalCall;
  }
  if (opt_verbose && (EC_Normal != result))
  {
    cerr << "spooler: file '" << filename << "' is not a valid Stored Print object" << endl;
  }
  delete ffile;
  
  if (EC_Normal == result)
  {
    // we have successfully read the Stored Print, now open connection to printer
    DVPSPrintMessageHandler printHandler;
    if (opt_dumpMode) printHandler.setDumpStream(&cout);
    if (!SUCCESS(printHandler.negotiateAssociation(dvi.getNetworkAETitle(), 
      targetAETitle, targetHostname, targetPort, targetMaxPDU, targetImplicitOnly, opt_verbose)))
    {
      cerr << "spooler: connection setup with printer failed." << endl;
      COND_DumpConditions();
      result =  EC_IllegalCall;
    } else {
      if (EC_Normal != (result = stprint.printSCUgetPrinterInstance(printHandler)))
      {
        cerr << "spooler: printer communication failed, unable to request printer settings." << endl;
      }
      if (EC_Normal==result) if (EC_Normal != (result = stprint.printSCUpreparePresentationLUT(printHandler)))
      {
        cerr << "spooler: printer communication failed, unable to create presentation LUT." << endl;
      }
      if (EC_Normal==result) if (EC_Normal != (result = dvi.printSCUcreateBasicFilmSession(printHandler)))
      {
        cerr << "spooler: printer communication failed, unable to create basic film session." << endl;
      }
      if (EC_Normal==result) if (EC_Normal != (result = stprint.printSCUcreateBasicFilmBox(printHandler)))
      {
        cerr << "spooler: printer communication failed, unable to create basic film box." << endl;
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
              if (EC_Normal != (result = stprint.printSCUsetBasicImageBox(printHandler, currentImage, targetSupports12bit, *dcmimage)))
              {
                cerr << "spooler: printer communication failed, unable to transmit basic grayscale image box." << endl;
              }
            } else {
              result = EC_IllegalCall;
              if (opt_verbose) cerr << "spooler: unable to load image file '" << theFilename.c_str() << "'" << endl;
            }
            delete dcmimage;
          } else {
            result = EC_IllegalCall;
            if (opt_verbose) cerr << "spooler: unable to locate image file in database." << endl;
          }
        } else result = EC_IllegalCall;
      }

      if (EC_Normal==result) if (EC_Normal != (result = stprint.printSCUprintBasicFilmBox(printHandler)))
      {
        cerr << "spooler: printer communication failed, unable to print." << endl;
      }
      if (EC_Normal==result) if (EC_Normal != (result = stprint.printSCUdelete(printHandler)))
      {
        cerr << "spooler: printer communication failed, unable to delete print objects." << endl;
      }
      
      if (!SUCCESS(printHandler.releaseAssociation()))
      {
        cerr << "spooler: release of connection to printer failed." << endl;
        COND_DumpConditions();
        if (EC_Normal == result) result =  EC_IllegalCall;
      }    
    }
  }  
  return result;
}

static E_Condition spoolJobList(OFList<printJob *>&jobList, DVInterface &dvi)
{
  E_Condition result = EC_Normal;
  E_Condition result2 = EC_Normal;
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
      if (currentJob->illumination != (unsigned long)-1) dvi.setPrintIllumination((Uint16)(currentJob->illumination));
      if (currentJob->reflectedAmbientLight != (unsigned long)-1) dvi.setPrintReflectedAmbientLight((Uint16)(currentJob->reflectedAmbientLight));

      result2 = spoolStoredPrintFile(currentJob->storedPrintFilename.c_str(), dvi);
      if (result2 != EC_Normal)
      {
        cerr << "spooler: error occured during spooling of Stored Print object '" << currentJob->storedPrintFilename.c_str() << "'" << endl;
      }
      if (result == EC_Normal) result = result2; // forward error codes, but do not erase
    } else {
      cerr << "spooler: unable to find Stored Print object for print job in database" << endl;
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
static E_Condition readJobFile(
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
  E_Condition result = EC_Normal;
  while (!eofFound)
  {
    eofFound = readValuePair(inf, key, value);
    if ((key.size()==0)||(key[0] == '#')) { /* ignore comments and empty lines */ }
    else if (key == "copies")
    {
      if (1 != sscanf(value.c_str(),"%lu", &job.numberOfCopies))
      {
        if (opt_verbose) cerr << "spooler: parse error for 'copies' in job file '" << infile << "'" << endl;
        result = EC_IllegalCall;
      }
    }
    else if (key == "illumination")
    {
      if (1 != sscanf(value.c_str(),"%lu", &job.illumination))
      {
        if (opt_verbose) cerr << "spooler: parse error for 'illumination' in job file '" << infile << "'" << endl;
        result = EC_IllegalCall;
      }
    }
    else if (key == "reflection")
    {
      if (1 != sscanf(value.c_str(),"%lu", &job.reflectedAmbientLight))
      {
        if (opt_verbose) cerr << "spooler: parse error for 'reflection' in job file '" << infile << "'" << endl;
        result = EC_IllegalCall;
      }
    }
    else if (key == "mediumtype") job.mediumType = value;
    else if (key == "destination") job.filmDestination = value;
    else if (key == "label") job.filmSessionLabel = value;
    else if (key == "priority") job.printPriority = value;
    else if (key == "owner_id") job.ownerID = value;
    else if (key == "study") job.studyUID = value;
    else if (key == "series") job.seriesUID = value;
    else if (key == "instance") job.instanceUID = value;
    else if (key == "terminate") terminateFlag=OFTrue;
    else
    {
      if (opt_verbose) cerr << "spooler: unknown keyword '" << key.c_str() << "' in job file '" << infile << "'" << endl;
      result = EC_IllegalCall;
    }
  }
  fclose(inf);

  if (deletePrintJobs)
  {
    if (0 != unlink(infile))
    {
      if (opt_verbose) cerr << "spooler: unable to delete job file '" << infile << "'" << endl;
      result = EC_IllegalCall;
    }
  } else {
    if ((outfile==NULL)||(0 != rename(infile, outfile)))
    {
      // if we can't rename, we delete to make sure we don't read the same file again next time.
      if (0 != unlink(infile))
      {
        if (opt_verbose) cerr << "spooler: unable to delete job file '" << infile << "'" << endl;
        result = EC_IllegalCall;
      }
    }
  }    
  
  // make sure that either all mandatory parameters are set or "terminate" is defined.
  if ((EC_Normal==result)&&(! terminateFlag)&&((job.studyUID.size()==0)||(job.seriesUID.size()==0)||(job.instanceUID.size()==0)))
  {
    if (opt_verbose) cerr << "spooler: UIDs missing in job file '" << infile << "'" << endl;
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
static E_Condition updateJobList(
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
  E_Condition result = EC_Normal;

#ifdef _WIN32
  WIN32_FIND_DATA stWin32FindData;
  OFString currentdir = dvi.getSpoolFolder();
  currentdir += "\\*";

  HANDLE hFile = FindFirstFile(currentdir.c_str(), &stWin32FindData);
  int ret = (hFile != INVALID_HANDLE_VALUE);
  while (ret)
  {
      currentName = stWin32FindData.cFileName;
#else
  DIR *dirp = NULL;
  struct dirent *dp = NULL;
  if ((dirp = opendir(dvi.getSpoolFolder())) != NULL)
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
        jobName = dvi.getSpoolFolder(); 
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
            cerr << "spooler: parsing of job file '" << jobName.c_str() << "' failed." << endl;
          }
        } else result = EC_MemoryExhausted;
      }

#ifdef _WIN32
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
    cerr << "error: unable to read spool directory '" << dvi.getSpoolFolder() << "'" << endl;
    result = EC_IllegalCall;
  }
  return result;
}


#define SHORTCOL 2
#define LONGCOL 14

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
  
    SetDebugLevel((0));

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Print spooler for presentation state viewer", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addParam("filename_in",   "stored print file(s) to be spooled", OFCmdParam::PM_MultiOptional);
      
    cmd.addGroup("general options:");
     cmd.addOption("--help",        "-h",        "print this help text and exit");
     cmd.addOption("--verbose",     "-v",        "verbose mode, print actions");
     cmd.addOption("--debug",       "-d",        "debug mode, print debug information");
     cmd.addOption("--dump",        "+d",        "dump all DIMSE messages to stdout");

    cmd.addGroup("mode options:");
     cmd.addOption("--print",       "+p",    "printer mode, print file(s) and terminate (default)");
     cmd.addOption("--spool",       "+s", 1, "[n]ame: string",
                                             "spooler mode, use job prefix [n]");
    cmd.addGroup("processing options:");
     cmd.addOption("--config",      "-c", 1, "[f]ilename: string",
                                             "process using settings from configuration file");
     cmd.addOption("--printer",     "-p", 1, "[n]ame: string (default: 1st printer in cfg file)",
                                             "select printer with identifier [n] from cfg file");
    cmd.addGroup("spooler options (only with --spool):");
     cmd.addOption("--sleep",             1, "[d]elay: integer (default: 1)",
                                             "sleep [d] seconds between spooler checks");
    cmd.addGroup("basic film session options (not with --spool):");
     cmd.addOption("--copies",            1, "[v]alue: integer (1..100, default: 1)",
                                             "set number of copies to [v]");
     cmd.addOption("--medium-type",       1, "[v]alue: string",
                                             "set medium type to [v]");
     cmd.addOption("--illumination",      1, "[v]alue: integer (0..65535)",
                                             "set illumination to [v] cd/m^2");
     cmd.addOption("--reflection",        1, "[v]alue: integer (0..65535)",
                                             "set reflected ambient light to [v] cd/m^2");
     cmd.addOption("--destination",       1, "[v]alue: string",
                                             "set film destination to [v]");
     cmd.addOption("--label",             1, "[v]alue: string",
                                             "set film session label to [v]");
     cmd.addOption("--priority",          1, "[v]alue: string",
                                             "set print priority to [v]");
     cmd.addOption("--owner",             1, "[v]alue: string",
                                             "set film session owner ID to [v]");

    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::ExpandWildcards))
    {
      if (cmd.findOption("--verbose"))     opt_verbose=OFTrue;
      if (cmd.findOption("--debug"))       opt_debugMode = 3;
      if (cmd.findOption("--dump"))        opt_dumpMode = OFTrue;

      cmd.beginOptionBlock();
      if (cmd.findOption("--print"))     opt_spoolMode=OFFalse;
      if (cmd.findOption("--spool"))
      {
        opt_spoolMode=OFTrue;
        app.checkValue(cmd.getValue(opt_spoolPrefix));
      }
      cmd.endOptionBlock();

      if (cmd.findOption("--config"))        app.checkValue(cmd.getValue(opt_cfgName));
      if (cmd.findOption("--printer"))       app.checkValue(cmd.getValue(opt_printer));

      if (cmd.findOption("--medium-type"))
      {
        app.checkConflict("--medium-type", "--spool", opt_spoolMode);
        app.checkValue(cmd.getValue(opt_mediumtype));
      }
      if (cmd.findOption("--illumination"))
      {
        app.checkConflict("--illumination", "--spool", opt_spoolMode);
        app.checkValue(cmd.getValue(opt_illumination, (OFCmdUnsignedInt)0, (OFCmdUnsignedInt)65535));
      }
      if (cmd.findOption("--reflection"))
      {
        app.checkConflict("--reflection", "--spool", opt_spoolMode);
        app.checkValue(cmd.getValue(opt_reflection, (OFCmdUnsignedInt)0, (OFCmdUnsignedInt)65535));
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
        app.checkValue(cmd.getValue(opt_copies, (OFCmdUnsignedInt)1, (OFCmdUnsignedInt)100));
      }
      if (cmd.findOption("--sleep"))
      {
        app.checkConflict("--sleep", "--print", (! opt_spoolMode));
        app.checkValue(cmd.getValue(opt_sleep));
      }
    }

    SetDebugLevel(((int)opt_debugMode));
    DicomImageClass::DebugLevel = (int)opt_debugMode;

    if (opt_cfgName)
    {
      FILE *cfgfile = fopen(opt_cfgName, "rb");
      if (cfgfile) fclose(cfgfile); else
      {
        cerr << "error: can't open configuration file '" << opt_cfgName << "'" << endl;
        return 10;
      }
    } else {
        cerr << "error: no configuration file specified" << endl;
        return 10;
    }

    DVInterface dvi(opt_cfgName);
    if (opt_printer)
    {
      if (EC_Normal != dvi.setCurrentPrinter(opt_printer))
      {
        cerr << "error: unable to select printer '" << opt_printer << "'." << endl;
        return 10;
      }
    } else {
      opt_printer = dvi.getCurrentPrinter(); // use default printer
      if (opt_printer==NULL)
      {
        cerr << "error: no default printer available - no config file?" << endl;
        return 10;
      }     
    }
    
    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
        cerr << "Warning: no data dictionary loaded, check environment variable: " << DCM_DICT_ENVIRONMENT_VARIABLE << endl;

    /* get print target from configuration file */
    targetHostname         = dvi.getTargetHostname(opt_printer);
    targetDescription      = dvi.getTargetDescription(opt_printer);
    targetAETitle          = dvi.getTargetAETitle(opt_printer);
    targetPort             = dvi.getTargetPort(opt_printer);
    targetMaxPDU           = dvi.getTargetMaxPDU(opt_printer);
    targetImplicitOnly     = dvi.getTargetImplicitOnly(opt_printer);
    targetDisableNewVRs    = dvi.getTargetDisableNewVRs(opt_printer);
    targetSupportsPLUT     = dvi.getTargetPrinterSupportsPresentationLUT(opt_printer);
    targetSupports12bit    = dvi.getTargetPrinterSupports12BitTransmission(opt_printer);
    deletePrintJobs        = dvi.getSpoolerDeletePrintJobs();

    if (targetHostname == NULL)
    {
        cerr << "error: no hostname for print target '" << opt_printer << "' - no config file?" << endl;
        return 10;
    }
    if (targetAETitle == NULL)
    {
        cerr << "error: no aetitle for print target '" << opt_printer << "'" << endl;
        return 10;
    }
    if (targetPort == 0)
    {
        cerr << "error: no or invalid port number for print target '" << opt_printer << "'" << endl;
        return 10;
    }
    if (targetMaxPDU == 0) targetMaxPDU = DEFAULT_MAXPDU;
    else if (targetMaxPDU > ASC_MAXIMUMPDUSIZE)
    {
        cerr << "warning: max PDU size " << targetMaxPDU << " too big for print target '"
             << opt_printer << "', using default: " << DEFAULT_MAXPDU << endl;
        targetMaxPDU = DEFAULT_MAXPDU;
    }
    if (targetDisableNewVRs)
    {
        dcmEnableUnknownVRGeneration = OFFalse;
        dcmEnableUnlimitedTextVRGeneration = OFFalse;
        dcmEnableVirtualStringVRGeneration = OFFalse;
    }

    if (opt_verbose)
    {
       cerr << "Printer parameters for '" <<  opt_printer << "':" << endl
            << "  hostname   : " << targetHostname << endl
            << "  port       : " << targetPort << endl
            << "  description: ";
       if (targetDescription) cerr << targetDescription; else cerr << "(none)";
       cerr << endl
            << "  aetitle    : " << targetAETitle << endl
            << "  max pdu    : " << targetMaxPDU << endl
            << "  options    : ";
       if (targetImplicitOnly && targetDisableNewVRs)
         cerr << "implicit xfer syntax only, disable post-1993 VRs";
       else if (targetImplicitOnly)
         cerr << "implicit xfer syntax only";
       else if (targetDisableNewVRs)
         cerr << "disable post-1993 VRs";
       else
         cerr << "none." << endl;
       cerr << "  12-bit xfer: " << (targetSupports12bit ? "supported" : "not supported") << endl
            << "  present.lut: " << (targetSupportsPLUT ? "supported" : "not supported") << endl;

       cerr << endl << "Spooler parameters:" << endl
            << "  mode       : " << (opt_spoolMode ? "spooler mode" : "printer mode") << endl;
       if (opt_spoolMode)
       {
         cerr << "  sleep time : " << opt_sleep << endl;       
       } else {
         cerr << "  copies     : " << opt_copies << endl;       
         cerr << "  medium     : " << (opt_mediumtype ? opt_mediumtype : "printer default") << endl;       
         cerr << "  illuminat. : ";
         if (opt_illumination == (OFCmdUnsignedInt)-1) cerr << "printer default" << endl;
         else cerr << opt_illumination << " cd/m^2" << endl;
         cerr << "  reflection : ";       
         if (opt_reflection == (OFCmdUnsignedInt)-1) cerr << "printer default" << endl;
         else cerr << opt_reflection << " cd/m^2" << endl;
         cerr << "  destination: " << (opt_destination ? opt_destination : "printer default") << endl;       
         cerr << "  label      : " << (opt_sessionlabel ? opt_sessionlabel : "printer default") << endl;       
         cerr << "  priority   : " << (opt_priority ? opt_priority : "printer default") << endl;       
         cerr << "  owner ID   : " << (opt_ownerID ? opt_ownerID : "printer default") << endl;       
       }    
       cerr << endl;
   }

   int paramCount = cmd.getParamCount();
   const char *currentParam = NULL;
   if (opt_spoolMode)
   {
      if (paramCount > 0)
      {
        cerr << "warning: filenames specified on command line, will be ignored in spooler mode" << endl;
      }

      OFString jobNamePrefix = opt_spoolPrefix;
      jobNamePrefix += "_";
      jobNamePrefix += opt_printer;
      jobNamePrefix += "_";
      OFList<printJob *> jobList;
      OFBool terminateFlag = OFFalse;
      do
      {
        sleep((unsigned int)opt_sleep);
        if (EC_Normal != updateJobList(jobList, dvi, terminateFlag, jobNamePrefix.c_str()))
        {
          cerr << "spooler: non recoverable error occured, terminating." << endl;
          return 10;
        }
        // static E_Condition updateJobList(jobList, dvi, terminateFlag, jobNamePrefix.c_str());
        if (EC_Normal != spoolJobList(jobList, dvi)) { /* ignore */ }
      } while (! terminateFlag);
      if (opt_verbose) cerr << "spooler is terminating, goodbye!" << endl;
   } else {
      // printer mode
      if (paramCount == 0)
      {
        cerr << "spooler: no stored print files specified - nothing to do." << endl;
      } else {
        dvi.clearFilmSessionSettings();
        if (opt_mediumtype) dvi.setPrinterMediumType(opt_mediumtype);
        if (opt_destination) dvi.setPrinterFilmDestination(opt_destination);
        if (opt_sessionlabel) dvi.setPrinterFilmSessionLabel(opt_sessionlabel);
        if (opt_priority) dvi.setPrinterPriority(opt_priority);
        if (opt_ownerID) dvi.setPrinterOwnerID(opt_ownerID);
        if (opt_copies > 0) dvi.setPrinterNumberOfCopies(opt_copies);
        if (opt_illumination != (OFCmdUnsignedInt)-1) dvi.setPrintIllumination((Uint16)opt_illumination);
        if (opt_reflection != (OFCmdUnsignedInt)-1) dvi.setPrintReflectedAmbientLight((Uint16)opt_reflection);        
        for (int param=1; param <= paramCount; param++)
        {
          cmd.getParam(param, currentParam);
          if (opt_verbose && currentParam)
          {
            cerr << "spooling file '" << currentParam << "'" << endl;
          }
          if (currentParam)
          {
            if (EC_Normal != spoolStoredPrintFile(currentParam, dvi)) 
            {
              cerr << "error: spooling of file '" << currentParam << "' failed." << endl;
            }
          } else {
            cerr << "error: empty file name" << endl;
          }
        }
      }
   }

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
 * $Log: dcmprtsv.cc,v $
 * Revision 1.4  1999-09-23 17:37:08  meichel
 * Added support for Basic Film Session options to dcmpstat print code.
 *
 * Revision 1.3  1999/09/17 14:33:45  meichel
 * Completed print spool functionality including Supplement 22 support
 *
 * Revision 1.2  1999/09/15 17:42:50  meichel
 * Implemented print job dispatcher code for dcmpstat, adapted dcmprtsv
 *   and dcmpsprt applications.
 *
 *
 */
