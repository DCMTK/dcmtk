/*
 *
 *  Copyright (C) 2002, OFFIS
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
 *  Module:  dcmdata
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Convert the contents of a DICOM file to XML format
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-04-25 14:56:35 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/dcm2xml.cc,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctk.h"
#include "cmdlnarg.h"
#include "ofstream.h"
#include "ofconapp.h"


#define OFFIS_CONSOLE_APPLICATION "dcm2xml"

#define DOCUMENT_TYPE_DECLARATION_FILE "dcm2xml.dtd"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";


// ********************************************


static OFCondition writeFile(ostream &out,
                             const char *ifname,
                             const OFBool isDataset,
                             const E_TransferSyntax xfer,
                             const OFBool loadIntoMemory,
                             const size_t writeFlags)
{
    OFCondition result = EC_Normal;

    if ((ifname == NULL) || (strlen(ifname) == 0))
    {
        CERR << OFFIS_CONSOLE_APPLICATION << ": invalid filename: <empty string>" << endl;
        return EC_IllegalParameter;
    }

    /* read DICOM file or data set */
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

    /* write content to XML format */
    if (result.good())
    {
        if (loadIntoMemory)
            dfile->getDataset()->loadAllDataIntoMemory();
        /* determine dataset character encoding */
        OFString encString;
        OFString csetString;
        if (dfile->getDataset()->findAndGetOFString(DCM_SpecificCharacterSet, csetString).good())
        {
            if (csetString == "ISO_IR 6")
                encString = "UTF-8";
            else if (csetString == "ISO_IR 100")
                encString = "ISO-8859-1";
            else if (csetString == "ISO_IR 101")
                encString = "ISO-8859-2";
            else if (csetString == "ISO_IR 109")
                encString = "ISO-8859-3";
            else if (csetString == "ISO_IR 110")
                encString = "ISO-8859-4";
            else if (csetString == "ISO_IR 148")
                encString = "ISO-8859-9";
            else if (csetString == "ISO_IR 144")
                encString = "ISO-8859-5";
            else if (csetString == "ISO_IR 127")
                encString = "ISO-8859-6";
            else if (csetString == "ISO_IR 126")
                encString = "ISO-8859-7";
            else if (csetString == "ISO_IR 138")
                encString = "ISO-8859-8";
        }
        /* write XML document header */
        out << "<?xml version=\"1.0\"";
        /* optional character set */
        if (encString.length() > 0)
            out << " encoding=\"" << encString << "\"";
        out << "?>" << endl;
        /* add document type declaration */
        if (writeFlags & DCMTypes::XF_addDocumentType)
        {
            out << "<!DOCTYPE ";
            if (isDataset)
                out << "dataset";
            else 
                out << "fileformat";
            out << " SYSTEM \"" << DOCUMENT_TYPE_DECLARATION_FILE << "\">" << endl;
        }
        /* write XML document content */
        if (isDataset)
            result = dfile->getDataset()->writeXML(out, writeFlags);
        else
            result = dfile->writeXML(out, writeFlags);
    }
    delete dfile;

    return result;
}


#define SHORTCOL 3
#define LONGCOL 20


int main(int argc, char *argv[])
{
    int opt_debugMode = 0;
    size_t opt_writeFlags = 0;
    OFBool isDataset = OFFalse;
    OFBool loadIntoMemory = OFFalse;
    E_TransferSyntax xfer = EXS_Unknown;

    SetDebugLevel(( 0 ));

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "Convert DICOM file and data set to XML", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addParam("dcmfile-in",   "DICOM input filename to be converted", OFCmdParam::PM_Mandatory);
    cmd.addParam("xmlfile-out",  "XML output filename (default: stdout)", OFCmdParam::PM_Optional);

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
      cmd.addOption("--help",                  "-h",     "print this help text and exit");
      cmd.addOption("--debug",                 "-d",     "debug mode, print debug information");

    cmd.addGroup("input options:");
      cmd.addSubGroup("input file format:");
        cmd.addOption("--read-file",           "+f",     "read file format or data set (default)");
        cmd.addOption("--read-dataset",        "-f",     "read data set without file meta information");
      cmd.addSubGroup("input transfer syntax (only with --read-dataset):");
        cmd.addOption("--read-xfer-auto",      "-t=",    "use TS recognition (default)");
        cmd.addOption("--read-xfer-little",    "-te",    "read with explicit VR little endian TS");
        cmd.addOption("--read-xfer-big",       "-tb",    "read with explicit VR big endian TS");
        cmd.addOption("--read-xfer-implicit",  "-ti",    "read with implicit VR little endian TS");
      cmd.addSubGroup("long tag values:");
        cmd.addOption("--load-all",            "+M",     "load very long tag values (e.g. pixel data)");
        cmd.addOption("--load-short",          "-M",     "do not load very long values (default)");
    cmd.addGroup("output options:");
      cmd.addSubGroup("XML structure:");
        cmd.addOption("--add-document-type",   "+Xd",    "add reference to document type definition (DTD)");
      cmd.addSubGroup("DICOM elements:");
        cmd.addOption("--write-binary-data",   "+Wb",    "write binary data of OB and OW elements\n(default: off, be careful with --load-all)");

    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::ExpandWildcards))
    {
        if (cmd.findOption("--debug"))
            opt_debugMode = 5;

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

        cmd.beginOptionBlock();
        if (cmd.findOption("--load-all"))
            loadIntoMemory = OFTrue;
        if (cmd.findOption("--load-short"))
            loadIntoMemory = OFFalse;
        cmd.endOptionBlock();
        
        if (cmd.findOption("--add-document-type"))
            opt_writeFlags |= DCMTypes::XF_addDocumentType;
        if (cmd.findOption("--write-binary-data"))
            opt_writeFlags |= DCMTypes::XF_writeBinaryData;
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
    /* first parameter is treated as the input filename */
    const char *ifname = NULL;
    cmd.getParam(1, ifname);
    /* if second parameter is present, it is treated as the output filename ("stdout" otherwise) */
    if (cmd.getParamCount() == 2)
    {
        const char *ofname = NULL;
        cmd.getParam(2, ofname);
        ofstream stream(ofname);
        if (stream.good())
        {
            if (writeFile(stream, ifname, isDataset, xfer, loadIntoMemory, opt_writeFlags).bad())
                result = 2;
        } else
            result = 1;
    } else {
        if (writeFile(COUT, ifname, isDataset, xfer, loadIntoMemory, opt_writeFlags).bad())
            result = 3;
    }

    return result;
}


/*
 * CVS/RCS Log:
 * $Log: dcm2xml.cc,v $
 * Revision 1.2  2002-04-25 14:56:35  joergr
 * Removed unused function parameter to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.1  2002/04/25 10:08:35  joergr
 * Added new command line tool to convert DICOM files to XML.
 *
 *
 */
