/*
 *
 *  Copyright (C) 2000-2002, OFFIS
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
 *  Module:  dcmsr
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Convert the contents of a DICOM structured reporting file to
 *           XML format
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-04-11 13:05:02 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmsr/apps/dsr2xml.cc,v $
 *  CVS/RCS Revision: $Revision: 1.10 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrdoc.h"

#include "dcdebug.h"
#include "cmdlnarg.h"
#include "ofconapp.h"
#include "dcuid.h"    /* for dcmtk version name */

#include <iostream.h>
#include <fstream.h>


#define OFFIS_CONSOLE_APPLICATION "dsr2xml"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";


// ********************************************


static OFCondition writeFile(ostream &out,
                             const char *ifname,
                             const OFBool isDataset,
                             const E_TransferSyntax xfer,
                             const size_t readFlags,
                             const size_t writeFlags,
                             const OFBool debugMode)
{
    OFCondition result = EC_Normal;

    if ((ifname == NULL) || (strlen(ifname) == 0))
    {
        CERR << OFFIS_CONSOLE_APPLICATION << ": invalid filename: <empty string>" << endl;
        return EC_IllegalParameter;
    }

    DcmFileFormat *dfile = new DcmFileFormat();
    if (dfile != NULL)
    {
        if (isDataset)
            result = dfile->getDataset()->loadFile(ifname, xfer);
        else
            result = dfile->loadFile(ifname, xfer);
        if (result.bad())
        {
            CERR << OFFIS_CONSOLE_APPLICATION << ": error (" << dfile->error().text()
                 << ") reading file: "<< ifname << endl;
        }
    } else
        result = EC_MemoryExhausted;

    if (result.good())
    {
        result = EC_CorruptedData;
        DSRDocument *dsrdoc = new DSRDocument();
        if (dsrdoc != NULL)
        {
            if (debugMode)
                dsrdoc->setLogStream(&ofConsole);
            result = dsrdoc->read(*dfile->getDataset(), readFlags);
            if (result.good())
                result = dsrdoc->writeXML(out, writeFlags);
            else
            {
                CERR << OFFIS_CONSOLE_APPLICATION << ": error (" << result.text()
                     << ") parsing file: "<< ifname << endl;
            }
        }
        delete dsrdoc;
    }
    delete dfile;

    return result;
}


#define SHORTCOL 3
#define LONGCOL 20


int main(int argc, char *argv[])
{
    int opt_debugMode = 0;
    size_t opt_readFlags = 0;
    size_t opt_writeFlags = 0;
    OFBool isDataset = OFFalse;
    E_TransferSyntax xfer = EXS_Unknown;

    SetDebugLevel(( 0 ));

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Convert DICOM SR file and data set to XML", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addParam("dsrfile-in",   "DICOM SR input filename to be converted", OFCmdParam::PM_Mandatory);
    cmd.addParam("xmlfile-out",  "XML output filename (default: stdout)", OFCmdParam::PM_Optional);

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
      cmd.addOption("--help",                  "-h",        "print this help text and exit");
      cmd.addOption("--debug",                 "-d",        "debug mode, print debug information");
      cmd.addOption("--verbose-debug",         "-dd",       "verbose debug mode, print more details");

    cmd.addGroup("input options:");
      cmd.addSubGroup("input file format:");
        cmd.addOption("--read-file",           "+f",        "read file format or data set (default)");
        cmd.addOption("--read-dataset",        "-f",        "read data set without file meta information");
      cmd.addSubGroup("input transfer syntax (only with --read-dataset):");
        cmd.addOption("--read-xfer-auto",      "-t=",       "use TS recognition (default)");
        cmd.addOption("--read-xfer-little",    "-te",       "read with explicit VR little endian TS");
        cmd.addOption("--read-xfer-big",       "-tb",       "read with explicit VR big endian TS");
        cmd.addOption("--read-xfer-implicit",  "-ti",       "read with implicit VR little endian TS");

    cmd.addGroup("output options:");
      cmd.addSubGroup("encoding:");
        cmd.addOption("--attr-code",           "+Ec",       "encode code value, coding scheme designator and\ncoding scheme version as XML attribute");
        cmd.addOption("--attr-relationship",   "+Er",       "encode relationship type as XML attribute");
        cmd.addOption("--attr-value-type",     "+Ev",       "encode value type as XML attribute");
      cmd.addSubGroup("writing:");
        cmd.addOption("--write-empty-tags",    "+We",       "write all tags even if their value is empty");

    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::ExpandWildcards))
    {
        if (cmd.findOption("--debug"))
            opt_debugMode = 2;
        if (cmd.findOption("--verbose-debug"))
        {
            opt_debugMode = 5;
            opt_readFlags |= DSRTypes::RF_verboseDebugMode;
        }

        cmd.beginOptionBlock();
        if (cmd.findOption("--read-file"))
            isDataset = OFFalse;
        if (cmd.findOption("--read-dataset"))
            isDataset = OFTrue;
        cmd.endOptionBlock();

        cmd.beginOptionBlock();
        if (cmd.findOption("--read-xfer-auto"))
        {
            app.checkDependence("--read-xfer-auto", "--read-dataset", isDataset);
            xfer = EXS_Unknown;
        }
        if (cmd.findOption("--read-xfer-little"))
        {
            app.checkDependence("--read-xfer-little", "--read-dataset", isDataset);
            xfer = EXS_LittleEndianExplicit;
        }
        if (cmd.findOption("--read-xfer-big"))
        {
            app.checkDependence("--read-xfer-big", "--read-dataset", isDataset);
            xfer = EXS_BigEndianExplicit;
        }
        if (cmd.findOption("--read-xfer-implicit"))
        {
            app.checkDependence("--read-xfer-implicit", "--read-dataset", isDataset);
            xfer = EXS_LittleEndianImplicit;
        }
        cmd.endOptionBlock();

        if (cmd.findOption("--attr-code"))
            opt_writeFlags |= DSRTypes::XF_codeComponentsAsAttribute;
        if (cmd.findOption("--attr-relationship"))
            opt_writeFlags |= DSRTypes::XF_relationshipTypeAsAttribute;
        if (cmd.findOption("--attr-value-type"))
            opt_writeFlags |= DSRTypes::XF_valueTypeAsAttribute;

        if (cmd.findOption("--write-empty-tags"))
            opt_writeFlags |= DSRTypes::XF_writeEmptyTags;
    }

    SetDebugLevel((opt_debugMode));

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        CERR << "Warning: no data dictionary loaded, "
             << "check environment variable: "
             << DCM_DICT_ENVIRONMENT_VARIABLE << endl;
    }

    int result = 0;
    const char *ifname = NULL;
    cmd.getParam(1, ifname);
    if (cmd.getParamCount() == 2)
    {
        const char *ofname = NULL;
        cmd.getParam(2, ofname);
        ofstream stream(ofname);
        if (stream.good())
        {
            if (writeFile(stream, ifname, isDataset, xfer, opt_readFlags, opt_writeFlags, opt_debugMode != 0).bad())
                result = 2;
        } else
            result = 1;
    } else {
        if (writeFile(COUT, ifname, isDataset, xfer, opt_readFlags, opt_writeFlags, opt_debugMode != 0).bad())
            result = 3;
    }

    return result;
}


/*
 * CVS/RCS Log:
 * $Log: dsr2xml.cc,v $
 * Revision 1.10  2002-04-11 13:05:02  joergr
 * Use the new loadFile() and saveFile() routines from the dcmdata library.
 *
 * Revision 1.9  2001/11/09 16:09:35  joergr
 * Added new command line option allowing to encode codes as XML attributes
 * (instead of tags).
 *
 * Revision 1.8  2001/10/10 15:26:33  joergr
 * Additonal adjustments for new OFCondition class.
 *
 * Revision 1.7  2001/10/02 11:56:00  joergr
 * Adapted module "dcmsr" to the new class OFCondition. Introduced module
 * specific error codes.
 *
 * Revision 1.6  2001/09/26 13:04:01  meichel
 * Adapted dcmsr to class OFCondition
 *
 * Revision 1.5  2001/06/20 15:06:38  joergr
 * Added new debugging features (additional flags) to examine "corrupted" SR
 * documents.
 *
 * Revision 1.4  2001/05/07 16:12:51  joergr
 * Updated CVS header.
 *
 * Revision 1.3  2001/02/02 14:36:27  joergr
 * Added new option to dsr2xml allowing to specify whether value and/or
 * relationship type are to be encoded as XML attributes or elements.
 *
 * Revision 1.2  2000/11/09 11:31:21  joergr
 * Corrected typo.
 *
 * Revision 1.1  2000/11/01 16:09:57  joergr
 * Added command line tool to convert DICOM SR documents to XML.
 *
 *
 *
 */
