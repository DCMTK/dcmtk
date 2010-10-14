/*
 *
 *  Copyright (C) 2000-2010, OFFIS e.V.
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
 *  Authors: Marco Eichelberg
 *
 *  Purpose: Presentation State Viewer - Print Server
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:13:45 $
 *  CVS/RCS Revision: $Revision: 1.32 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

BEGIN_EXTERN_C
#ifdef HAVE_FCNTL_H
#include <fcntl.h>       /* for O_RDONLY */
#endif
END_EXTERN_C

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmpstat/dvpsdef.h"     /* for constants and macros */
#include "dcmtk/dcmpstat/dviface.h"
#include "dcmtk/ofstd/ofbmanip.h"       /* for OFBitmanipTemplate */
#include "dcmtk/ofstd/ofdatime.h"       /* for OFDateTime */
#include "dcmtk/dcmdata/dcuid.h"        /* for dcmtk version name */
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmpstat/dvpsprt.h"
#include "dcmtk/dcmpstat/dvpshlp.h"
#include "dcmtk/oflog/fileap.h"

#ifdef WITH_OPENSSL
#include "dcmtk/dcmtls/tlstrans.h"
#include "dcmtk/dcmtls/tlslayer.h"
#endif

#ifdef WITH_ZLIB
#include <zlib.h>        /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "dcmprscp"

static OFLogger dcmprscpLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

/* command line options */
static OFBool      opt_binaryLog = OFFalse;
static OFBool      opt_logFile   = OFFalse;
static const char *opt_cfgName   = NULL;                /* config file name */
static const char *opt_printer   = NULL;                /* printer name */

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
            if (errno != ECHILD)
            {
                char buf[256];
                OFLOG_ERROR(dcmprscpLogger, "wait for child failed: " << OFStandard::strerror(errno, buf, sizeof(buf)));
            }
        }
    }
#endif
}


#define SHORTCOL 3
#define LONGCOL 12

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

    dcmDisableGethostbyaddr.set(OFTrue);  // disable hostname lookup

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "DICOM basic grayscale print management SCP", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 2);

    cmd.addGroup("general options:");
     cmd.addOption("--help",    "-h",    "print this help text and exit", OFCommandLine::AF_Exclusive);
     cmd.addOption("--version",          "print version information and exit", OFCommandLine::AF_Exclusive);
     OFLog::addOptions(cmd);
     cmd.addOption("--logfile", "-l",    "write a log file (not with --log-config)");

    cmd.addGroup("processing options:");
     cmd.addOption("--config",  "-c", 1, "[f]ilename: string",
                                         "process using settings from configuration file");
     cmd.addOption("--printer", "-p", 1, "[n]ame: string (default: 1st printer in config file)",
                                         "select printer with identifier n from config file");
     cmd.addOption("--dump",    "+d",    "dump all DIMSE messages");

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
      if (cmd.findOption("--dump"))
      {
        // Messages to the "dump" logger are always written with the debug log
        // level, thus enabling that logger for this level shows the dumps
        log4cplus::Logger log = log4cplus::Logger::getInstance("dcmtk.dcmpstat.dump");
        log.setLogLevel(OFLogger::DEBUG_LOG_LEVEL);
      }

      OFLog::configureFromCommandLine(cmd, app);

      if (cmd.findOption("--logfile"))
      {
        app.checkConflict("--logfile", "--log-config", cmd.findOption("--log-config"));
        opt_logFile = OFTrue;
      }
      if (cmd.findOption("--config"))  app.checkValue(cmd.getValue(opt_cfgName));
      if (cmd.findOption("--printer")) app.checkValue(cmd.getValue(opt_printer));
    }

    /* print resource identifier */
    OFLOG_DEBUG(dcmprscpLogger, rcsid << OFendl);

    if (opt_cfgName)
    {
      FILE *cfgfile = fopen(opt_cfgName, "rb");
      if (cfgfile) fclose(cfgfile); else
      {
        OFLOG_FATAL(dcmprscpLogger, "can't open configuration file '" << opt_cfgName << "'");
        return 10;
      }
    } else {
        OFLOG_FATAL(dcmprscpLogger, "no configuration file specified");
        return 10;
    }

    DVInterface dvi(opt_cfgName);

    if (opt_printer)
    {
      if (DVPSE_printLocal != dvi.getTargetType(opt_printer))
      {
        OFLOG_FATAL(dcmprscpLogger, "no print scp definition for '" << opt_printer << "' found in config file");
        return 10;
      }
    } else {
      opt_printer = dvi.getTargetID(0, DVPSE_printLocal); // use default print scp
      if (opt_printer==NULL)
      {
        OFLOG_FATAL(dcmprscpLogger, "no default print scp available - no config file?");
        return 10;
      }
    }

    opt_binaryLog = dvi.getBinaryLog();

    OFString logfileprefix;
    OFString aString;
    unsigned long logcounter = 0;
    char logcounterbuf[20];

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

    if (opt_logFile)
    {
      const char *pattern = "%m%n";
      OFString logfilename = logfileprefix;
      logfilename += ".log";

      OFauto_ptr<log4cplus::Layout> layout(new log4cplus::PatternLayout(pattern));
      log4cplus::SharedAppenderPtr logfile(new log4cplus::FileAppender(logfilename));
      log4cplus::Logger log = log4cplus::Logger::getRoot();

      logfile->setLayout(layout);
      log.removeAllAppenders();
      log.addAppender(logfile);
    }

    OFLOG_WARN(dcmprscpLogger, rcsid << OFendl << OFDateTime::getCurrentDateTime() << OFendl << "started");

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
       OFLOG_WARN(dcmprscpLogger, "no data dictionary loaded, check environment variable: " << DCM_DICT_ENVIRONMENT_VARIABLE);

    /* check if we can get access to the database */
    const char *dbfolder = dvi.getDatabaseFolder();
    OFLOG_INFO(dcmprscpLogger, "Using database in directory '" << dbfolder << "'");

    OFCondition cond2 = EC_Normal;
    DcmQueryRetrieveIndexDatabaseHandle *dbhandle = new DcmQueryRetrieveIndexDatabaseHandle(dbfolder, PSTAT_MAXSTUDYCOUNT, PSTAT_STUDYSIZE, cond2);
    delete dbhandle;

    if (cond2.bad())
    {
      OFLOG_FATAL(dcmprscpLogger, "Unable to access database '" << dbfolder << "'");
      return 10;
    }

    /* get print scp data from configuration file */
    unsigned short targetPort   = dvi.getTargetPort(opt_printer);
    OFBool targetDisableNewVRs  = dvi.getTargetDisableNewVRs(opt_printer);
    OFBool targetUseTLS         = dvi.getTargetUseTLS(opt_printer);

    if (targetPort == 0)
    {
        OFLOG_FATAL(dcmprscpLogger, "no or invalid port number for print scp '" << opt_printer << "'");
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

#ifdef WITH_OPENSSL
    /* TLS directory */
    const char *current = NULL;
    const char *tlsFolder = dvi.getTLSFolder();
    if (tlsFolder==NULL) tlsFolder = ".";

    /* certificate file */
    OFString tlsCertificateFile(tlsFolder);
    tlsCertificateFile += PATH_SEPARATOR;
    current = dvi.getTargetCertificate(opt_printer);
    if (current) tlsCertificateFile += current; else tlsCertificateFile += "sitecert.pem";

    /* private key file */
    OFString tlsPrivateKeyFile(tlsFolder);
    tlsPrivateKeyFile += PATH_SEPARATOR;
    current = dvi.getTargetPrivateKey(opt_printer);
    if (current) tlsPrivateKeyFile += current; else tlsPrivateKeyFile += "sitekey.pem";

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
          OFLOG_WARN(dcmprscpLogger, "ciphersuite '" << currentSuite << "' is unknown. Known ciphersuites are:");
          unsigned long numSuites = DcmTLSTransportLayer::getNumberOfCipherSuites();
          for (unsigned long cs=0; cs < numSuites; cs++)
          {
            OFLOG_WARN(dcmprscpLogger, "    " << DcmTLSTransportLayer::getTLSCipherSuiteName(cs));
          }
          return 1;
        } else {
          if (tlsCiphersuites.length() > 0) tlsCiphersuites += ":";
          tlsCiphersuites += currentOpenSSL;
        }
      }
    }

    DcmTLSTransportLayer *tLayer = NULL;
    if (targetUseTLS)
    {
      tLayer = new DcmTLSTransportLayer(DICOM_APPLICATION_ACCEPTOR, tlsRandomSeedFile.c_str());
      if (tLayer == NULL)
      {
        OFLOG_FATAL(dcmprscpLogger, "unable to create TLS transport layer");
        return 1;
      }

      if (tlsCACertificateFolder && (TCS_ok != tLayer->addTrustedCertificateDir(tlsCACertificateFolder, keyFileFormat)))
      {
        OFLOG_WARN(dcmprscpLogger, "unable to load certificates from directory '" << tlsCACertificateFolder << "', ignoring");
      }
      if ((tlsDHParametersFile.size() > 0) && ! (tLayer->setTempDHParameters(tlsDHParametersFile.c_str())))
      {
        OFLOG_WARN(dcmprscpLogger, "unable to load temporary DH parameter file '" << tlsDHParametersFile << "', ignoring");
      }
      tLayer->setPrivateKeyPasswd(tlsPrivateKeyPassword); // never prompt on console

      if (TCS_ok != tLayer->setPrivateKeyFile(tlsPrivateKeyFile.c_str(), keyFileFormat))
      {
        OFLOG_FATAL(dcmprscpLogger, "unable to load private TLS key from '" << tlsPrivateKeyFile<< "'");
        return 1;
      }
      if (TCS_ok != tLayer->setCertificateFile(tlsCertificateFile.c_str(), keyFileFormat))
      {
        OFLOG_FATAL(dcmprscpLogger, "unable to load certificate from '" << tlsCertificateFile << "'");
        return 1;
      }
      if (! tLayer->checkPrivateKeyMatchesCertificate())
      {
        OFLOG_FATAL(dcmprscpLogger, "private key '" << tlsPrivateKeyFile << "' and certificate '" << tlsCertificateFile << "' do not match");
        return 1;
      }
      if (TCS_ok != tLayer->setCipherSuites(tlsCiphersuites.c_str()))
      {
        OFLOG_FATAL(dcmprscpLogger, "unable to set selected cipher suites");
        return 1;
      }

      tLayer->setCertificateVerification(tlsCertVerification);

    }
#else
    if (targetUseTLS)
    {
        OFLOG_FATAL(dcmprscpLogger, "not compiled with OpenSSL, cannot use TLS");
        return 10;
    }
#endif

    /* open listen socket */
    OFCondition cond = ASC_initializeNetwork(NET_ACCEPTOR, targetPort, 30, &net);
    if (cond.bad())
    {
      OFString temp_str;
      OFLOG_FATAL(dcmprscpLogger, "cannot initialise network:\n" << DimseCondition::dump(temp_str, cond));
      return 1;
    }

#ifdef WITH_OPENSSL
    if (tLayer)
    {
      cond = ASC_setTransportLayer(net, tLayer, 0);
      if (cond.bad())
      {
        OFString temp_str;
        OFLOG_FATAL(dcmprscpLogger, DimseCondition::dump(temp_str, cond));
        return 1;
      }
    }
#endif

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
          if (cond.bad())
          {
            OFString temp_str;
            OFLOG_FATAL(dcmprscpLogger, "cannot drop network:\n" << DimseCondition::dump(temp_str, cond));
            return 10;
          }
          break;
        case DVPSJ_success:
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

#ifdef WITH_OPENSSL
    if (tLayer)
    {
      if (tLayer->canWriteRandomSeed())
      {
        if (!tLayer->writeRandomSeed(tlsRandomSeedFile.c_str()))
        {
          OFLOG_ERROR(dcmprscpLogger, "cannot write back random seed file '" << tlsRandomSeedFile << "', ignoring");
        }
      } else {
        OFLOG_WARN(dcmprscpLogger, "cannot write back random seed, ignoring");
      }
    }
    delete tLayer;
#endif

    return 0;
}

/*
 * CVS/RCS Log:
 * $Log: dcmprscp.cc,v $
 * Revision 1.32  2010-10-14 13:13:45  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.31  2010-06-03 10:32:58  joergr
 * Replaced calls to strerror() by new helper function OFStandard::strerror()
 * which results in using the thread safe version of strerror() if available.
 *
 * Revision 1.30  2010-04-29 10:36:52  joergr
 * Fixed typo in log message.
 *
 * Revision 1.29  2009-12-16 14:12:18  joergr
 * Slightly modified description of command line option --logfile.
 *
 * Revision 1.28  2009-12-15 14:50:49  uli
 * Fixes some issues with --logfile and the config's log options.
 *
 * Revision 1.27  2009-12-15 12:34:40  uli
 * Re-added and fixed the command line option --logfile.
 *
 * Revision 1.26  2009-12-11 15:23:25  joergr
 * Changed description of command line option --dump.
 *
 * Revision 1.25  2009-11-27 10:52:01  joergr
 * Fixed various issues with syntax usage (e.g. layout and formatting).
 * Sightly modifed log messages.
 *
 * Revision 1.24  2009-11-24 14:12:56  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.23  2008-09-25 16:30:24  joergr
 * Added support for printing the expanded command line arguments.
 * Always output the resource identifier of the command line tool in debug mode.
 *
 * Revision 1.22  2006/08/15 16:57:01  meichel
 * Updated the code in module dcmpstat to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.21  2006/07/27 14:36:27  joergr
 * Changed parameter "exclusive" of method addOption() from type OFBool into an
 * integer parameter "flags". Prepended prefix "PF_" to parseLine() flags.
 * Option "--help" is no longer an exclusive option by default.
 *
 * Revision 1.20  2005/12/08 15:46:05  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.19  2005/11/28 15:29:05  meichel
 * File dcdebug.h is not included by any other header file in the toolkit
 *   anymore, to minimize the risk of name clashes of macro debug().
 *
 * Revision 1.18  2005/11/23 16:10:32  meichel
 * Added support for AES ciphersuites in TLS module. All TLS-enabled
 *   tools now support the "AES TLS Secure Transport Connection Profile".
 *
 * Revision 1.17  2005/11/16 14:58:23  meichel
 * Set association timeout in ASC_initializeNetwork to 30 seconds. This improves
 *   the responsiveness of the tools if the peer blocks during assoc negotiation.
 *
 * Revision 1.16  2005/04/04 10:11:53  meichel
 * Module dcmpstat now uses the dcmqrdb API instead of imagectn for maintaining
 *   the index database
 *
 * Revision 1.15  2004/02/04 15:44:38  joergr
 * Removed acknowledgements with e-mail addresses from CVS log.
 *
 * Revision 1.14  2003/09/05 10:38:24  meichel
 * Print SCP now supports TLS connections and the Verification Service Class.
 *
 * Revision 1.13  2003/09/04 10:09:16  joergr
 * Fixed wrong use of OFBool/bool variable.
 *
 * Revision 1.12  2002/11/26 08:44:26  meichel
 * Replaced all includes for "zlib.h" with <zlib.h>
 *   to avoid inclusion of zlib.h in the makefile dependencies.
 *
 * Revision 1.11  2002/09/23 18:26:06  joergr
 * Added new command line option "--version" which prints the name and version
 * number of external libraries used (incl. preparation for future support of
 * 'config.guess' host identifiers).
 *
 * Revision 1.10  2002/06/14 10:44:17  meichel
 * Adapted log file handling to ofConsole singleton
 *
 * Revision 1.9  2002/04/16 14:01:26  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 *
 * Revision 1.8  2002/04/11 13:15:38  joergr
 * Replaced direct call of system routines by new standard date and time
 * functions.
 *
 * Revision 1.7  2001/10/12 13:46:48  meichel
 * Adapted dcmpstat to OFCondition based dcmnet module (supports strict mode).
 *
 * Revision 1.6  2001/06/01 15:50:07  meichel
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
