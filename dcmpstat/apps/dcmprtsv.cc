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
 *  Authors: Andreas Thiel
 *
 *  Purpose: Presentation State Viewer - Print Server
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-09-14 09:14:03 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmpstat/apps/Attic/dcmprtsv.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
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
 #include <dirent.h>
 #define NAMLEN(dirent) strlen((dirent)->d_name)
#else
 #define dirent direct
 #define NAMELEN(dirent) (dirent)->d_namlen
 #if HAVE_SYS_NDIR_H
  #include <sys/ndir.h>
 #endif
 #if HAVE_SYS_DIR_H
  #include <sys/dir.h>
 #endif
 #if HAVE_NDIR_H
  #include <ndir.h>
 #endif
#endif

#ifdef HAVE_FCNTL_H
 #include <fcntl.h>    /* for O_RDONLY */
#endif
END_EXTERN_C

#include "dviface.h"    /* for DVInterface */
#include "ofstring.h"   /* for OFString */
#include "ofbmanip.h"   /* for OFBitmanipTemplate */
#include "dcuid.h"      /* for dcmtk version name */
#include "diutil.h"
#include "cmdlnarg.h"   /* for prepareCmdLineArgs */
#include "ofconapp.h"   /* for OFConsoleApplication */


#include "dvpspr.h"
#include "dvpssp.h"
#include "dvpshc.h"

#define OFFIS_CONSOLE_APPLICATION "dcmprtsv"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

/* default Max PDU size to be used when no different value is defined in the configuration file */
#define DEFAULT_MAXPDU 16384

#define DEBUG


/********* Helper *******/

class MyPrintEventHandler
  : public DVPSPrintEventHandler
{

 public:

    DIC_US handleEvent(T_DIMSE_N_EventReportRQ &/*eventMessage*/,
                       DcmDataset */*eventInformation*/,
                       DcmDataset */*statusDetail*/)
    {
        cerr << "Event" << endl;
        return 0;
    }
};


static DcmElement *getThisElement(DcmItem *in,
                                  Uint16 g,
                                  Uint16 e)
{
    DcmElement *elem;
    for (unsigned int i=0; in->card() > i; i++)
    {
        elem=in->getElement(i);
        if (elem->getGTag()==g && elem->getETag()==e )
            return elem;
    }
    return NULL;
}


static CONDITION addCopyElement(DcmItem *src,
                                DcmItem *target,
                                Uint16 g,
                                Uint16 e)
{
   return target->insert(getThisElement(src,g,e));
}


static CONDITION addThisValue(DcmItem *in,
                              Uint16 g,
                              Uint16 e,
                              const char *instring)
{
    DcmElement * elem;
    newDicomElement(elem,DcmTag(g,e));
    elem->putString(instring);
    return in->insert(elem);
}

/*
static DcmElement *getThisElement(DcmItem *in,
                                  DcmTagKey tag)
{
    DcmStack stack;
    stack.clear();
    if (EC_Normal==in->search(tag,stack,ESM_fromHere,OFFalse))
        return stack.top();
    return NULL;
}


static CONDITION addCopyElement(DcmItem *src,
                                DcmItem *target,
                                DcmTagKey tag)
{
    return target->insert(getThisElement(src,tag));
}


static CONDITION addThisValue(DcmItem *in,
                              DcmTagKey tag,
                              const char *instring)
{
    DcmElement * elem;
    newDicomElement(elem,tag);
    elem->putString(instring);
    return in->insert(elem);
}
*/


static char *printStatusString(int status)
{
    char *buf;
    char *out;
    switch(status)
    {
        case 0x0:
            out = (char *)"Status ok";
            break;
        case 0x105:
            out = (char *)"No Such Attribute";
            break;
        case 0x106:
            out = (char *)"Invalid Attribute Value";
            break;
        case 0x107:
            out = (char *)"Attribute List Error";
            break;
        case 0x116:
            out = (char *)"Attribute Value Out of Range";
            break;
        case 0x120:
            out = (char *)"Missing Attribute";
            break;
        default:
            if ((status & 0xff00) == 0xb600)
                sprintf(out, "Warning Status Code:%x", status);
            else {
                if ((status & 0xff00) == 0xc600)
                    sprintf(out,"Failure Status Code:%x",status);
                else
                    sprintf(out,"Unknown Status Code:%x",status);
            }
            break;
    }
    buf=(char *) malloc(strlen(out) + 1);
    if (buf)
        strcpy(buf,out);
    else
        buf=NULL;
    return buf;
}


static CONDITION printInstance(DB_Handle *handle,
                               T_ASC_Association *assoc,
                               const char *instanceUID,
                               int opt_verbose)
{
    const int cond = 0;
    if ((handle == NULL) || (assoc == NULL))
      return DIMSE_NULLKEY;

    /* build query */
    DcmDataset query;
    if (opt_verbose)
        cerr << "Print instance: " << instanceUID << endl;

  return cond;
}


typedef enum PRT_CONDITION
{
    PRT_NO_DIRECTORY,
    PRT_NORMAL,
    PRT_NO_FILE,
    PRT_NO_MEMORY
} PRT_CONDITION;


static PRT_CONDITION parseFile(const char * /*filename*/,
                               DcmItem & /*item*/)
{
    return PRT_NO_MEMORY;       // added by JR
}


#if HAVE__FINDFIRST

static PRT_CONDITION lookForFile(const char *dir,
                                 const char *extension,
                                 char **resultFile,
                                 int verbose)
{
    OFString fullname(dir);
    HANDLE hFile;
    WIN32_FIND_DATA stWin32FindData;
    OFBool found = OFFalse;
    int ret;

    if (verbose)
        cerr << "Opening directory :" << dir << endl;
    hFile = FindFirstFile(dir, &stWin32FindData);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        cerr << "error: cannot access: " << dir << endl;
        return PRT_NO_FILE;
    }
  else
    if (stWin32FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        fullname += "\\*";
        hFile = FindFirstFile(fullname.c_str(), &stWin32FindData);
        if (hFile == INVALID_HANDLE_VALUE)
        {
            cerr << "error: cannot access: " << fullname << endl;
            return PRT_NO_FILE;
        }
        ret = (hFile != INVALID_HANDLE_VALUE);
        while(ret)
        {
            OFString filename;
            filename.assign(stWin32FindData.cFileName);
            if (filename.rfind(extension, sizeof(extension) + 1) != OFString_npos)
            {
                fullname.assign(dir);
                fullname +="\\";
                fullname += filename;
                found=OFTrue;
                break;
            }
            ret = FindNextFile(hFile, &stWin32FindData);
        }//while
        FindClose(hFile);

        if (found)
        {
            if (verbose)
                cerr << "Found job file: " << fullname << endl;
            *resultFile = (char *)malloc(fullname.size());
            if (resultFile)
                strcpy(*resultFile,fullname.c_str());
            else
                return PRT_NO_MEMORY;
        }
        return PRT_NORMAL;
        /*loeschen*/
    } else
        return PRT_NO_DIRECTORY;
    return PRT_NO_FILE;
}

#else

static PRT_CONDITION lookForFile(const char *dir,
                                 const char *extension,
                                 char **resultFile,
                                 int verbose)
{
    OFString fullname;
    DIR *dirp = NULL;
    OFBool found = OFFalse;
    if (verbose)
        cerr << "Opening directory :" << dir << endl;
    if ((dirp = opendir(dir)) != NULL)
    {
        /*it a directory*/
        struct dirent *dp = NULL;
        for (dp = readdir(dirp); dp != NULL; dp = readdir(dirp))
        {
            OFString filename(dp->d_name);
//            cerr << filename << ":" << filename.rfind(".prt", 5) << endl;
            if (filename.rfind(extension, sizeof(extension) + 1) != OFString_npos)
            {
                fullname += dir;
                fullname += dp->d_name;
                found = OFTrue;
                break;
            }
        }
        closedir(dirp);

        if (found)
        {
            if (verbose)
                cerr << "Found job file: " << fullname << endl;
            *resultFile = (char *)malloc(fullname.size());
            if (resultFile)
                strcpy(*resultFile,fullname.c_str());
            else
                return PRT_NO_MEMORY;
            return PRT_NORMAL;
            /*loeschen*/
        }
    } else
        return PRT_NO_DIRECTORY;
    return PRT_NO_FILE;
}

#endif


#define SHORTCOL 2
#define LONGCOL 10


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

    OFString str;
    int         opt_verbose         = 0;                   /* default: not verbose */
    const char *opt_cfgName         = NULL;                /* config file name */
    const char *opt_target          = NULL;                /* print target name */
    const char *opt_spoolDir        = NULL;                /* spool directory */
//  const char *opt_instanceUID     = NULL;                /* instance UID */       JR: unused variable
    char       *printFile           = NULL;
    const char *storedPrintFile     = NULL;
    int         servermode          = 1;                   /* flag for server/commandline mode */
    OFCmdUnsignedInt opt_sleep      = 2;                   /* sleeping time for scheduler */
    OFCmdSignedInt opt_reflective   = -1;                  /* optional reflection value */
    OFCmdSignedInt opt_illumination = -1;                  /* optional illumination value */
    OFCmdUnsignedInt opt_copies     = 1;                   /* number of print copies*/
    const char *opt_mediumtype      = NULL;                /* instance instance UID */

    SetDebugLevel((0));

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Network print for presentation state viewer", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addParam("config-file",     "configuration file to be read");
    cmd.addParam("spool-directory", "spooling directory of the jobs");

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
     cmd.addOption("--help",        "-h",    "print this help text and exit");
     cmd.addOption("--verbose",     "-v",    "verbose mode, print processing details");
     cmd.addOption("--debug",       "-d",    "debug mode, print debug information");

    cmd.addGroup("server options:",LONGCOL, SHORTCOL + 2);
     cmd.addOption("--sleep",             1, "[v]alue: integer",
                                             "sleep [v] seconds between spooler check (default: 1)");

    cmd.addGroup("mandantory commandline options:", LONGCOL, SHORTCOL + 2);
     cmd.addOption("--target",            1, "[t]arget: string",
                                             "symbolic identifier of printer in config file");
     cmd.addOption("--printfile",         1, "[f]ilenamefile: string",
                                             "prints the stored printobject direct");

    cmd.addGroup("optional options:", LONGCOL, SHORTCOL + 4);
     cmd.addOption("--copies",            1, "[v]alue: integer",
                                             "prints [v] times");
     cmd.addOption("--mediumtype",        1, "[v]alue: string",
                                             "choose [v] as output medium");
     cmd.addOption("--illumination",      1, "[v]alue: integer",
                                             "sets the illumination to [v] cd/m^2");
     cmd.addOption("--reflective",        1, "[v]alue: integer",
                                             "sets the reflective ambient light to [v] cd/m^2");

    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::ExpandWildcards))
    {
        cmd.getParam(1, opt_cfgName);
        cmd.getParam(2, opt_spoolDir);

        if (cmd.findOption("--verbose"))
            opt_verbose = 1;
        if (cmd.findOption("--debug"))
            SetDebugLevel(3);
        if (cmd.findOption("--sleep"))
            app.checkValue(cmd.getValue(opt_sleep, 0));
        if (cmd.findOption("--target"))
        {
            app.checkValue(cmd.getValue(opt_target));
            servermode = 0;
        }
        if (cmd.findOption("--printfile"))
        {
           app.checkValue(cmd.getValue(storedPrintFile));
           servermode = 0;
        }
        if ((servermode == 0) && (( opt_target == NULL) || (storedPrintFile==NULL)))
        {
           cerr << "Missing target or printfile for commandline print" << endl;
           return(10);
        }
        if (cmd.findOption("--copies"))
            app.checkValue(cmd.getValue(opt_copies, 1));
        if (cmd.findOption("--mediumtype"))
            cmd.getValue(opt_mediumtype);
        if (cmd.findOption("--illumination"))
            app.checkValue(cmd.getValue(opt_illumination));
        if (cmd.findOption("--reflective"))
            app.checkValue(cmd.getValue(opt_reflective));
    }

    if (opt_verbose)
        cerr << rcsid << endl << endl;

    if (opt_cfgName)
    {
        FILE *cfgfile = fopen(opt_cfgName, "rb");
        if (cfgfile)
            fclose(cfgfile);
        else
        {
            cerr << "error: can't open configuration file '" << opt_cfgName << "'" << endl;
            return 10;
        }
    } else {
        cerr << "error: missing configuration file name" << endl;
        return 10;
    }

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
        cerr << "Warning: no data dictionary loaded, check environment variable: " << DCM_DICT_ENVIRONMENT_VARIABLE << endl;

    /* toggel between server/commandline mode */
    DVInterface dvi(opt_cfgName);

    if (servermode)
    {
        OFBool father = OFTrue;

        //check for database
        const char *dbfolder = dvi.getDatabaseFolder();
        DB_Handle *dbhandle = NULL;
        if (opt_verbose)
            cerr << "Opening database in directory '" << dbfolder << "'" << endl;
        if (DB_NORMAL != DB_createHandle(dbfolder, PSTAT_MAXSTUDYCOUNT, PSTAT_STUDYSIZE, &dbhandle))
        {
            cerr << "Unable to access database '" << dbfolder << "'" << endl;
            COND_DumpConditions();
            return 10;
        }

        /* clean up DB handle */
        DB_destroyHandle(&dbhandle);
        while (father)
        {
            if (servermode)
            {
                sleep((unsigned int)opt_sleep);
                switch (lookForFile(opt_spoolDir, ".prt", &printFile, opt_verbose))
                {
                    case PRT_NORMAL:
                        /*FORK*/
                        if (storedPrintFile==NULL)
                        {
#ifdef DEBUG
                            cerr << "Interpreting spoolfile " << printFile << endl;
#endif
                            FILE *printfp = fopen(printFile, "rb");
                            char *studyUID;
                            char *seriesUID;
                            char *instanceUID;

                            if (!printfp)
                                cerr << "Error open " << printFile << endl;
                            else
                            {
                                fscanf(printfp, "%s %s %s", studyUID, seriesUID, instanceUID);
#ifdef DEBUG
                                cerr << " Search for\nStudy:" << studyUID << "\nSeries:" << seriesUID << "\nInstance:" << instanceUID << endl;
#endif
                                storedPrintFile=dvi.getFilename(studyUID, seriesUID, instanceUID);
#ifdef DEBUG
                                if (storedPrintFile==NULL)
                                    cerr << "File not in database" << endl;
                                else
                                    ;
#endif
                            }//interpreting
                        } else
                            father=OFFalse;
/*------------------------------
 JR: changed bracket structure !?
--------------------------------*/

                        break;
                    case PRT_NO_DIRECTORY:
                        cerr << "Missing spool directory: " << opt_spoolDir << endl;
                        return(10);
                        break;
                    case PRT_NO_FILE:
                        cerr << "Can't find a job in spool directory " << opt_spoolDir << endl;
                        break;
                    case PRT_NO_MEMORY:
                        cerr << "Can't allocate Memory " << endl;
                        break;
                }
            }
        }

        /* get send target from configuration file */
        const char *targetHostname    = dvi.getTargetHostname(opt_target);
        const char *targetDescription = dvi.getTargetDescription(opt_target);
        const char *targetAETitle     = dvi.getTargetAETitle(opt_target);
        unsigned short targetPort     = dvi.getTargetPort(opt_target);
        unsigned long  targetMaxPDU   = dvi.getTargetMaxPDU(opt_target);
        OFBool targetImplicitOnly     = dvi.getTargetImplicitOnly(opt_target);
        OFBool targetDisableNewVRs    = dvi.getTargetDisableNewVRs(opt_target);

        DVPSPrintMessageHandler printHandler;

        if (targetHostname == NULL)
        {
            cerr << "error: no hostname for print target '" << opt_target << "'" << endl;
            return 10;
        }
        if (targetAETitle == NULL)
        {
            cerr << "error: no aetitle for print target '" << opt_target << "'" << endl;
            return 10;
        }
        if (targetPort == 0)
        {
            cerr << "error: no or invalid port number for print target '" << opt_target << "'" << endl;
            return 10;
        }
        if (targetMaxPDU == 0)
            targetMaxPDU = DEFAULT_MAXPDU;
        else if (targetMaxPDU > 65536)
        {
            cerr << "warning: max PDU size " << targetMaxPDU << " too big for print target '"
                 << opt_target << "', using default: " << DEFAULT_MAXPDU << endl;
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
            cerr << "Print target parameters for target:" <<  opt_target << endl
                << "  hostname   : " << targetHostname << endl
                << "  port       : " << targetPort << endl
                << "  description: ";
            if (targetDescription)
                cerr << targetDescription;
            else
                cerr << "(none)";
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
            cerr << "Printing file: " << storedPrintFile << endl;
            if (servermode)
                cerr << "Sleeping time:" << opt_sleep << endl;
            else
            {
                cerr << "Number of copies: " << opt_copies << endl;
                if (opt_mediumtype)
                    cerr << "Printing on medium: " << opt_mediumtype << endl;
                if (opt_illumination != -1)
                    cerr << "Set illumination to " << opt_illumination << " ca/m^2" << endl;
                if (opt_reflective != -1 )
                    cerr << "Set reflective ambient light to " << opt_reflective << " ca/m^2" << endl;
                cerr << endl << endl;
            }
        }

        DcmFileFormat *dsfile = NULL;
        dsfile = new DcmFileFormat();
        DcmFileStream dstream(storedPrintFile, DCM_ReadMode);
        if (dstream.GetError() != EC_Normal)
        {
            cerr << " cannot open file: " << storedPrintFile << endl;
            return 1;
        }
        dsfile->transferInit();
        dsfile->read(dstream,EXS_Unknown , EGL_noChange);
        dsfile->transferEnd();
        dsfile->loadAllDataIntoMemory();

        DVPSStoredPrint stored;
        if (EC_Normal != stored.read(*(DcmItem *)dsfile->getDataset()))
            cerr << "Not a stored print object" << endl;

        if (!SUCCESS(printHandler.negotiateAssociation("DCMPRTSV", targetAETitle,
            targetHostname,targetPort, targetMaxPDU, targetImplicitOnly, opt_verbose)))
        {
            COND_DumpConditions();
            return 1;
        }
#ifdef DEBUG
        cerr << "PresentationLUT ";
        if (printHandler.printerSupportsPresentationLUT())
            cerr << "supported"<<endl;
        else
            cerr << "not supported"<<endl;
#endif
/*
    stored.setSessionOptions(item);

*/
        if (EC_Normal != stored.startPrint(&printHandler))
            return(10);
        while (stored.printPending())
        {
            char *aETitle;
            char *patID;
            char *studyUID;
            char *seriesUID;
            char *instanceUID;
            DcmFileFormat *dfile = NULL;
            DcmItem image;
            stored.getNextImageReference(aETitle, patID, studyUID, seriesUID, instanceUID);
            const char *filename = dvi.getFilename(studyUID, seriesUID, instanceUID);
#ifdef DEBUG
            if (filename)
                cerr << "Loading File from Database:"<< filename<< endl;
#endif
            if (dfile)
                delete dfile;
            dfile = new DcmFileFormat();
            DcmFileStream myin(filename, DCM_ReadMode);
            if (myin.GetError() != EC_Normal)
            {
                cerr << " cannot open file: " << filename << endl;
                return 1;
            }
            dfile->transferInit();
            dfile->read(myin,EXS_Unknown, EGL_noChange);
            dfile->transferEnd();
            dfile->loadAllDataIntoMemory();
            stored.setImage((DcmItem *)dfile->getDataset());
        }
//      stored.doPrint();
//      stored.relasePrint();

    }//main while loop (JR: ???)

#ifdef HAVE_WINSOCK_H
    WSACleanup();
#endif
#ifdef DEBUG
    dcmDataDict.clear();  /* useful for debugging with dmalloc */
#endif

    return 0;
}
