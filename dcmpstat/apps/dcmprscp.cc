/*
 *
 *  Copyright (C) 2000-2019, OFFIS e.V.
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
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

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
#elif defined(HAVE_WAIT3)
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
    OFStandard::initializeNetwork();
#ifdef WITH_OPENSSL
    DcmTLSTransportLayer::initializeOpenSSL();
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
      if (cmd.findOption("--dump"))
      {
        // Messages to the "dump" logger are always written with the debug log
        // level, thus enabling that logger for this level shows the dumps
        dcmtk::log4cplus::Logger log = dcmtk::log4cplus::Logger::getInstance("dcmtk.dcmpstat.dump");
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

      OFunique_ptr<dcmtk::log4cplus::Layout> layout(new dcmtk::log4cplus::PatternLayout(pattern));
      dcmtk::log4cplus::SharedAppenderPtr logfile(new dcmtk::log4cplus::FileAppender(logfilename));
      dcmtk::log4cplus::Logger log = dcmtk::log4cplus::Logger::getRoot();

      logfile->setLayout(OFmove(layout));
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
      dcmDisableGenerationOfNewVRs();
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
    DcmKeyFileFormat keyFileFormat = DCF_Filetype_PEM;
    if (! dvi.getTLSPEMFormat()) keyFileFormat = DCF_Filetype_ASN1;

    DcmTLSTransportLayer *tLayer = NULL;
    if (targetUseTLS)
    {
      tLayer = new DcmTLSTransportLayer(NET_ACCEPTOR, tlsRandomSeedFile.c_str(), OFFalse);
      if (tLayer == NULL)
      {
        OFLOG_FATAL(dcmprscpLogger, "unable to create TLS transport layer");
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
        OFLOG_WARN(dcmprscpLogger, "unknown TLS profile '" << profileName << "', ignoring");
      }

      if (TCS_ok != tLayer->setTLSProfile(tlsProfile))
      {
        OFLOG_FATAL(dcmprscpLogger, "unable to select the TLS security profile");
        return 1;
      }

      // activate cipher suites
      if (TCS_ok != tLayer->activateCipherSuites())
      {
        OFLOG_FATAL(dcmprscpLogger, "unable to activate the selected list of TLS ciphersuites");
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

    /* drop root privileges now and revert to the calling user id (if we are running as setuid root) */
    if (OFStandard::dropPrivileges().bad())
    {
        OFLOG_FATAL(dcmprscpLogger, "setuid() failed, maximum number of processes/threads for uid already running.");
        return 1;
    }

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

    OFStandard::shutdownNetwork();

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
