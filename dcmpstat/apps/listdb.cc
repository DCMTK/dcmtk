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
 *  Authors: Joerg Riesmeier
 *
 *  Purpose: test ...
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1999-02-17 10:05:04 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmpstat/apps/Attic/listdb.cc,v $
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

#include "cmdlnarg.h"
#include "ofcmdln.h"

#include "dviface.h"


// ********************************************


static void
printHeader()
{
    cerr << "listdb: List database contents" << endl;
}


static void
printUsage(const OFCommandLine &cmd)
{
    OFString str;
    cmd.getOptionString(str);
    printHeader();
    cerr << "usage: listdb config-file" << endl;
    cerr << "options are:" << endl << endl;
    cerr << str << endl;
    exit(0);
}

static void
printError(const OFString &str)
{
    printHeader();
    cerr << "error: " << str << endl;
    exit(1);
}

static const char *guard(const char *c)
{
  if (c) return c; else return "";
}

// ********************************************

int main(int argc, char *argv[])
{
    OFCommandLine cmd;
    OFString str;

    const char *opt_cfgName = NULL;                    /* pstate file name */
    
    SetDebugLevel(( 0 ));
  
    prepareCmdLineArgs(argc, argv, "listdb");
      
    cmd.addGroup("options:");
     cmd.addOption("--help", "print this help screen");
     
    switch (cmd.parseLine(argc, argv))    
    {
        case OFCommandLine::PS_NoArguments:
            printUsage(cmd);
            break;
        case OFCommandLine::PS_UnknownOption:
            cmd.getStatusString(OFCommandLine::PS_UnknownOption, str);
            printError(str);
            break;
        case OFCommandLine::PS_MissingValue:
            cmd.getStatusString(OFCommandLine::PS_MissingValue, str);
            printError(str);
            break;
        case OFCommandLine::PS_Normal:
            if ((cmd.getArgCount() == 1) && cmd.findOption("--help"))
                printUsage(cmd);
            else if (cmd.getParamCount() == 0)
                printError("Missing configuration file");
            else if (cmd.getParamCount() > 1)
                printError("Too many arguments");
            else 
            {
                cmd.getParam(1, opt_cfgName);
            }
    }

    SetDebugLevel((0));

    FILE *cfgfile = fopen(opt_cfgName, "rb");
    if (cfgfile) fclose(cfgfile); else
    {
      cerr << "error: can't open configuration file '" << opt_cfgName << "'" << endl;
      return 10;
    }
    DVInterface dvi(opt_cfgName);

    Uint32 numSeries=0;
    Uint32 numInstances=0;
    Uint32 numStudies = dvi.getNumberOfStudies();
    cout << "Number of studies: " << numStudies << endl;
    for (Uint32 ns=0; ns < numStudies; ns++)
    {
       cout << "Study #" << ns+1 << ":";
       if (EC_Normal != dvi.selectStudy(ns))
       {
         cout << "error - cannot be selected" << endl;
       } else {
         cout << endl
         << "  study uid=" << guard(dvi.getStudyUID()) << endl
         << "  study description=" << guard(dvi.getStudyDescription()) << endl
         << "  study date=" << guard(dvi.getStudyDate()) << endl
         << "  study time=" << guard(dvi.getStudyTime()) << endl
         << "  study ref. physician=" << guard(dvi.getReferringPhysiciansName()) << endl
         << "  study accession no=" << guard(dvi.getAccessionNumber()) << endl
         << "  study reading physician=" << guard(dvi.getNameOfPhysiciansReadingStudy()) << endl
         << "  patient name=" << guard(dvi.getPatientName()) << endl
         << "  patient ID=" << guard(dvi.getPatientID()) << endl
         << "  patient birth date=" << guard(dvi.getPatientBirthDate()) << endl
         << "  patient sex=" << guard(dvi.getPatientSex()) << endl
         << "  patient birth time=" << guard(dvi.getPatientBirthTime()) << endl
         << "  patient other names=" << guard(dvi.getOtherPatientNames()) << endl
         << "  patient other ID=" << guard(dvi.getOtherPatientID()) << endl
         << "  patient ethnic group=" << guard(dvi.getEthnicGroup()) << endl
         << "  study status=";
         switch (dvi.getStudyStatus())
         {
           case DVIF_objectIsNotNew:
             cout << "study is not new" << endl;
             break;
           case DVIF_objectIsNew:
             cout << "study is new" << endl;
             break;
           case DVIF_objectContainsNewSubobjects:
             cout << "study contains new subobjects" << endl;
             break;
         }
         numSeries = dvi.getNumberOfSeries();
         cout << "  Number of series in Study " << ns+1 << ": " << numSeries << endl;
         
    for (Uint32 nss=0; nss < numSeries; nss++)
         {
           cout << "  Series #" << nss+1 << ":";
           if (EC_Normal != dvi.selectSeries(nss))
           {
             cout << "error - cannot be selected" << endl;
           } else {
             cout << endl
             << "    series uid=" << guard(dvi.getSeriesUID()) << endl
             << "    series description=" << guard(dvi.getSeriesDescription()) << endl
             << "    series no=" << guard(dvi.getSeriesNumber()) << endl
             << "    series date=" << guard(dvi.getSeriesDate()) << endl
             << "    series time=" << guard(dvi.getSeriesTime()) << endl
             << "    series perform phys name=" << guard(dvi.getSeriesPerformingPhysiciansName()) << endl
             << "    series protocol name=" << guard(dvi.getSeriesProtocolName()) << endl
             << "    series operator name=" << guard(dvi.getSeriesOperatorsName()) << endl
             << "    series modality=" << guard(dvi.getModality()) << endl
             << "    series is Presentation State Series=";
             if (dvi.isPresentationStateSeries()) cout << "yes" << endl; else cout << "no" << endl;
             cout << "    series status=";
             switch (dvi.getSeriesStatus())
             {
               case DVIF_objectIsNotNew:
                 cout << "series is not new" << endl;
                 break;
               case DVIF_objectIsNew:
                 cout << "series is new" << endl;
                 break;
               case DVIF_objectContainsNewSubobjects:
                 cout << "series contains new subobjects" << endl;
                 break;
             }
             numInstances = dvi.getNumberOfInstances();
             cout << "  Number of instances in series " << nss+1 << ": " << numInstances << endl;
             for (Uint32 ni=0; ni < numInstances; ni++)
             {
               cout << "  Instance #" << ni+1 << ":";
               if (EC_Normal != dvi.selectInstance(ni))
               {
                 cout << "error - cannot be selected" << endl;
               } else {
                 cout << endl
                 << "      instance uid=" << guard(dvi.getInstanceUID()) << endl
                 << "      instance no=" << guard(dvi.getImageNumber()) << endl
                 << "      instance is Presentation State =";
                 if (dvi.isPresentationState()) cout << "yes" << endl; else cout << "no" << endl;
                 cout << "      instance status=";
                 switch (dvi.getInstanceStatus())
                 {
                   case DVIF_objectIsNotNew:
                     cout << "instance is not new" << endl;
                     break;
                   case DVIF_objectIsNew:
                     cout << "instance is new" << endl;
                     break;
                   case DVIF_objectContainsNewSubobjects:
                     cout << "instance contains new subobjects (should never happen)" << endl;
                     break;
                 }
               }
             }
           }
         }
       }
    }
dvi.releaseDatabase();
    return 0;
}


/*
** CVS/RCS Log:
** $Log: listdb.cc,v $
** Revision 1.4  1999-02-17 10:05:04  meichel
** Removed dcmdata debug level from sample apps
**
** Revision 1.3  1999/02/08 12:52:18  meichel
** Removed dummy parameter from DVInterface constructor.
**
** Revision 1.2  1999/02/05 11:26:16  vorwerk
** listdb for dviface updated
**
** Revision 1.1  1999/01/29 16:01:37  meichel
** Implemented small sample application that lists the
**   contents of the database using DVInterface methods.
**
**
*/
