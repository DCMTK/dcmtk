/*
 *
 *  Copyright (C) 1998-2001, OFFIS
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
 *  Authors: Marco Eichelberg
 *
 *  Purpose: Presentation State Viewer - Network Receive Component (Store SCP)
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-06-01 15:50:07 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmpstat/apps/dcmprscp.cc,v $
 *  CVS/RCS Revision: $Revision: 1.6 $
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
#ifdef HAVE_FCNTL_H
#include <fcntl.h>    /* for O_RDONLY */
#endif
END_EXTERN_C
#include <fstream.h>

#include "dvpsdef.h"     /* for constants and macros */
#include "dviface.h"
#include "ofbmanip.h" /* for OFBitmanipTemplate */
#include "dcuid.h"    /* for dcmtk version name */
#include "diutil.h"
#include "cmdlnarg.h"
#include "ofconapp.h"
#include "dvpsprt.h"
#include "dvpshlp.h"

#define OFFIS_CONSOLE_APPLICATION "dcmprscp"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

/* command line options */
static OFBool           opt_verbose         = OFFalse;             /* default: not verbose */
static int              opt_debugMode       = 0;
static OFBool           opt_dumpMode        = OFFalse;
static OFBool           opt_logFile         = OFFalse;
static OFBool           opt_binaryLog       = OFFalse;
static const char *     opt_cfgName         = NULL;                /* config file name */
static const char *     opt_printer         = NULL;                /* printer name */
static ostream *        logstream           = &CERR;
static OFConsole *      logconsole          = &ofConsole;

/* print scp data, taken from configuration file */
static unsigned short targetPort            = 104;
static OFBool         targetDisableNewVRs   = OFFalse;
static OFBool         haveHandledClients    = OFFalse;
static OFBool         deleteUnusedLogs      = OFTrue;

static int errorCond(CONDITION cond, const char *message)
{
  int result = (!SUCCESS(cond));
  if (result) 
  {  
    CERR << message << endl;
    COND_DumpConditions(); 
  }
  return result;
}

void closeLog()
{
  time_t now = time(NULL);
  if (logstream != &CERR)
  {
    if (logconsole != &ofConsole) delete logconsole;
    logconsole = &ofConsole;
    *logstream << endl << asctime(localtime(&now)) << "terminating" << endl;
    delete logstream;
    logstream = &CERR;
  }
}

static void cleanChildren()
{
#ifdef HAVE_WAITPID
    int stat_loc;
#elif HAVE_WAIT3
    struct rusage rusage;
#if defined(__NeXT__)
    /* some systems need a union wait as argument to wait3 */
    union wait status;
#else
    int        status;
#endif
#endif

#if defined(HAVE_WAITPID) || defined(HAVE_WAIT3)
    int child = 1;
    int options = WNOHANG;
    while (child > 0)
    {
#ifdef HAVE_WAITPID
        child = (int)(waitpid(-1, &stat_loc, options));
#elif defined(HAVE_WAIT3)
        child = wait3(&status, options, &rusage);
#endif
        if (child < 0)
        {
           if (errno != ECHILD) CERR << "wait for child failed: " << strerror(errno) << endl;
        }
    }
#endif
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
    dcmDisableGethostbyaddr.set(OFTrue);  // disable hostname lookup

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "DICOM basic grayscale print management SCP", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 2);

    cmd.addGroup("general options:");
     cmd.addOption("--help",        "-h",    "print this help text and exit");
     cmd.addOption("--verbose",     "-v",    "verbose mode, print actions");
     cmd.addOption("--debug",       "-d",    "debug mode, print debug information");

    cmd.addGroup("processing options:");
     cmd.addOption("--config",      "-c", 1, "[f]ilename: string",
                                             "process using settings from configuration file");
     cmd.addOption("--printer",     "-p", 1, "[n]ame: string (default: 1st printer in cfg file)",
                                             "select printer with identifier [n] from cfg file");
     cmd.addOption("--dump",        "+d",    "print all DIMSE messages");
     cmd.addOption("--logfile",     "-l",    "print to log file instead of stdout");

    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::ExpandWildcards))
    {
      if (cmd.findOption("--verbose")) opt_verbose = OFTrue;
      if (cmd.findOption("--debug"))   opt_debugMode = 3;
      if (cmd.findOption("--dump"))    opt_dumpMode = OFTrue;
      if (cmd.findOption("--logfile")) opt_logFile = OFTrue;
      if (cmd.findOption("--config"))  app.checkValue(cmd.getValue(opt_cfgName));
      if (cmd.findOption("--printer")) app.checkValue(cmd.getValue(opt_printer));
    }

    SetDebugLevel((opt_debugMode));

    if (opt_cfgName)
    {
      FILE *cfgfile = fopen(opt_cfgName, "rb");
      if (cfgfile) fclose(cfgfile); else
      {
        *logstream << "error: can't open configuration file '" << opt_cfgName << "'" << endl;
        return 10;
      }
    } else {
        *logstream << "error: no configuration file specified" << endl;
        return 10;
    }

    DVInterface dvi(opt_cfgName);

    if (opt_printer)
    {
      if (DVPSE_printLocal != dvi.getTargetType(opt_printer))
      {
        *logstream << "error: no print scp definition for '" << opt_printer << "' found in config file." << endl;
        return 10;
      }
    } else {
      opt_printer = dvi.getTargetID(0, DVPSE_printLocal); // use default print scp
      if (opt_printer==NULL)
      {
        *logstream << "error: no default print scp available - no config file?" << endl;
        return 10;
      }
    }

    opt_binaryLog = dvi.getBinaryLog();

    OFString logfileprefix;
    OFString aString;
    unsigned long logcounter = 0;
    char logcounterbuf[20];
    
    time_t now = time(NULL);
    if (dvi.getLogFolder() != NULL)
        logfileprefix = dvi.getLogFolder();
    else
        logfileprefix = dvi.getSpoolFolder();

    logfileprefix += PATH_SEPARATOR;
    logfileprefix += "PrintSCP_";
    logfileprefix += opt_printer;
    logfileprefix += "_";
    DVPSHelper::currentDate(aString);
    logfileprefix += aString;
    logfileprefix += "_";
    DVPSHelper::currentTime(aString);
    logfileprefix += aString;
    
    OFString logfilename;
    if (opt_logFile)
    {
      logfilename = logfileprefix;
      logfilename += ".log";
    
      ofstream *newstream = new ofstream(logfilename.c_str());
      if (newstream && (newstream->good()))
      {
        logstream=newstream; 
        logconsole = new OFConsole();
        if (logconsole)
        {
          logconsole->setCout(logstream);
          logconsole->join();
        } else logconsole = &ofConsole;
      }
      else
      {
      	delete newstream;
      	logfilename.clear();
      }
    }

    *logstream << rcsid << endl << asctime(localtime(&now)) << "started" << endl;

    dvi.setLog(logconsole, opt_verbose, opt_debugMode);
    
    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
        *logstream << "Warning: no data dictionary loaded, check environment variable: " << DCM_DICT_ENVIRONMENT_VARIABLE << endl;

    /* check if we can get access to the database */
    const char *dbfolder = dvi.getDatabaseFolder();
    DB_Handle *dbhandle = NULL; 

    if (opt_verbose)
    {
      *logstream << "Using database in directory '" << dbfolder << "'" << endl;
    }
    if (DB_NORMAL != DB_createHandle(dbfolder, PSTAT_MAXSTUDYCOUNT, PSTAT_STUDYSIZE, &dbhandle))
    {
      *logstream << "Unable to access database '" << dbfolder << "'" << endl;
      return 10;
    }
    DB_destroyHandle(&dbhandle);

    /* get print scp data from configuration file */
    targetPort                  = dvi.getTargetPort(opt_printer);
    targetDisableNewVRs         = dvi.getTargetDisableNewVRs(opt_printer);

    if (targetPort == 0)
    {
        *logstream << "error: no or invalid port number for print scp '" << opt_printer << "'" << endl;
        closeLog();
        return 10;
    }

    if (targetDisableNewVRs)
    {
        dcmEnableUnknownVRGeneration.set(OFFalse);
        dcmEnableUnlimitedTextVRGeneration.set(OFFalse);
    }

    T_ASC_Network *net = NULL; /* the DICOM network and listen port */
    OFBool finished = OFFalse;
    int connected = 0;
    
    /* open listen socket */
    CONDITION cond = ASC_initializeNetwork(NET_ACCEPTOR, targetPort, 10, &net);
    if (errorCond(cond, "Error initialising network:")) return 1;

#if defined(HAVE_SETUID) && defined(HAVE_GETUID)
    /* return to normal uid so that we can't do too much damage in case
     * things go very wrong.   Only relevant if the program is setuid root,
     * and run by another user.  Running as root user may be
     * potentially disasterous if this program screws up badly.
     */
    setuid(getuid());
#endif

#ifdef HAVE_FORK
    int timeout=1;
#else
    int timeout=1000;
#endif

    while (!finished)
    {
      DVPSPrintSCP printSCP(dvi, opt_printer); // use new print SCP object for each association     
      
      printSCP.setLog(logconsole, opt_verbose, opt_debugMode, opt_dumpMode);
      
      if (opt_binaryLog)
      {
        aString = logfileprefix;
        aString += "_";
        sprintf(logcounterbuf, "%04ld", ++logcounter);
        aString += logcounterbuf;
        aString += ".dcm";
        printSCP.setDimseLogPath(aString.c_str());
      }
      
      connected = 0;
      while (!connected)
      {
         connected = ASC_associationWaiting(net, timeout);
         if (!connected) cleanChildren();
      }
      switch (printSCP.negotiateAssociation(*net))
      {
        case DVPSJ_error:
          // association has already been deleted, we just wait for the next client to connect.
          break;
        case DVPSJ_terminate:
          finished=OFTrue;
          cond = ASC_dropNetwork(&net);
          if (errorCond(cond, "Error dropping network:")) return 10;
          break;
        case DVPSJ_success:
          haveHandledClients = OFTrue;
          printSCP.handleClient();
          break;
      }
    } // finished
    cleanChildren();
    
#ifdef HAVE_WINSOCK_H
    WSACleanup();
#endif

#ifdef DEBUG
    dcmDataDict.clear();  /* useful for debugging with dmalloc */
#endif

    closeLog();
    
    if (deleteUnusedLogs && (! haveHandledClients))
    {
      // log unused, attempt to delete file
      if (logfilename.size() > 0) unlink(logfilename.c_str());      
    }
        
    return 0;
}

/*
 * CVS/RCS Log:
 * $Log: dcmprscp.cc,v $
 * Revision 1.6  2001-06-01 15:50:07  meichel
 * Updated copyright header
 *
 * Revision 1.5  2001/06/01 11:02:05  meichel
 * Implemented global flag and command line option to disable reverse
 *   DNS hostname lookup using gethostbyaddr when accepting associations.
 *
 * Revision 1.4  2000/06/07 13:17:42  meichel
 * added binary and textual log facilities to Print SCP.
 *
 * Revision 1.3  2000/06/06 09:44:07  joergr
 * Moved configuration file entry "LogDirectory" from "[PRINT]" to new
 * (more general) section "[APPLICATION]".
 *
 * Revision 1.2  2000/06/02 16:00:38  meichel
 * Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 * Revision 1.1  2000/05/31 12:59:28  meichel
 * Added initial Print SCP support
 *
 *
 */
