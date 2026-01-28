/*
 *
 *  Copyright (C) 2025-2026, OFFIS e.V.
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
 *  Module:  dcmtkapp
 *
 *  Author:  Tingyan Xu
 *
 *  Purpose: dcmtk command line tool
 *
 */

#include "dcmtk/config/osconfig.h"    // make sure OS specific configuration is included first
#include "dcmtk/ofstd/ofstub.h"       /* for OFstub*/
#include "dcmtk/ofstd/ofexit.h"       /* for EXITCODE constants */
#include "dcmtk/ofstd/ofconsol.h"     /* for COUT/CERR */
#include "dcmtk/ofstd/ofconapp.h"     /* for OFConsoleApplication */
#include "dcmtk/dcmdata/dcuid.h"      /* for dcmtk version name */

#define OFFIS_CONSOLE_APPLICATION "dcmtk"
#define OFFIS_CONSOLE_DESCRIPTION "Execute dcmtk command line tool"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

// tool category for command line tools
enum DcmAppType
{
  AT_conversion,
  AT_compression,
  AT_network,
  AT_misc
};

// struct for the list of tools
struct DcmToolEntry
{
  OFString name;
  OFString description;
  DcmAppType apptype;
};

// list of DCMTK command line tools in alphabetical order
static DcmToolEntry tools[] =
{
    {"dcm2img",  "Convert DICOM image to standard image format", AT_conversion },
    {"dcm2json", "Convert DICOM file and data set to JSON", AT_conversion },
    {"dcm2xml",  "Convert DICOM file and data set to XML", AT_conversion },
    {"dcmcjpeg", "Encode DICOM file to JPEG transfer syntax", AT_compression },
    {"dcmcjpls", "Encode DICOM file to JPEG-LS transfer syntax", AT_compression },
    {"dcmconv",  "Convert DICOM file encoding", AT_conversion },
    {"dcmcrle",  "Encode DICOM file to RLE transfer syntax", AT_compression },
    {"dcmdecap", "Extract file from DICOM encapsulated storage", AT_conversion },
    {"dcmdjpeg", "Decode JPEG-compressed DICOM file", AT_compression },
    {"dcmdjpls", "Decode JPEG-LS compressed DICOM file", AT_compression },
    {"dcmdrle",  "Decode RLE-compressed DICOM file", AT_compression },
    {"dcmdspfn", "Export standard display curves to a text file", AT_misc },
    {"dcmdump",  "Dump DICOM file and data set", AT_conversion },
    {"dcmencap", "Encapsulate document into DICOM format", AT_conversion },
    {"dcmftest", "Test if file uses DICOM part 10 format", AT_misc },
    {"dcmicmp",  "Compare DICOM images and compute difference metrics", AT_misc },
    {"dcmmkcrv", "Add 2D curve data to image", AT_misc },
    {"dcmmkdir", "Create a DICOMDIR file", AT_misc },
    {"dcmmklut", "Create DICOM look-up tables", AT_misc },
    {"dcmodify", "Modify DICOM files", AT_conversion },
    {"dcmp2pgm", "Read DICOM image and presentation state and render bitmap", AT_conversion },
    {"dcmprscp", "DICOM basic grayscale print management SCP", AT_network },
    {"dcmprscu", "Print spooler for presentation state viewer", AT_network },
    {"dcmpschk", "Checking tool for presentation states", AT_misc },
    {"dcmpsmk",  "Create DICOM grayscale softcopy presentation state", AT_misc },
    {"dcmpsprt", "Read DICOM images and presentation states and render print job", AT_misc },
    {"dcmpsrcv", "Network receive for presentation state viewer", AT_network },
    {"dcmpssnd", "Network send for presentation state viewer", AT_network },
    {"dcmqridx", "Register a DICOM image file in an image database index file", AT_misc },
    {"dcmqrscp", "DICOM image archive (central test node)", AT_network },
    {"dcmqrti",  "Telnet Initiator", AT_misc },
    {"dcmquant", "Convert DICOM color images to palette color", AT_misc },
    {"dcmrecv", "Simple DICOM storage SCP (receiver)", AT_network },
    {"dcmscale", "Scale DICOM images", AT_misc },
    {"dcmsend", "Simple DICOM storage SCU (sender)", AT_network },
    {"dcmsign",  "Sign and Verify DICOM Files", AT_misc },
    {"dcod2lum", "Convert hardcopy characteristic curve file to softcopy format", AT_misc },
    {"dconvlum", "Convert VeriLUM \"CCx_xx.dat\" files to DCMTK display files", AT_misc },
    {"drtdump",  "Dump DICOM RT file and data set", AT_conversion },
    {"dsr2html", "Render DICOM SR file and data set to HTML/XHTML", AT_conversion },
    {"dsr2xml",  "Convert DICOM SR file and data set to XML", AT_conversion },
    {"dsrdump",  "Dump DICOM SR file and data set", AT_conversion },
    {"dump2dcm", "Convert ASCII dump to DICOM file", AT_conversion },
    {"echoscu", "DICOM verification (C-ECHO) SCU", AT_network },
    {"findscu", "DICOM query (C-FIND) SCU", AT_network },
    {"getscu", "DICOM retrieve (C-GET) SCU", AT_network },
    {"img2dcm",  "Convert standard image formats into DICOM format", AT_conversion },
    {"json2dcm", "Convert JSON document to DICOM file or data set", AT_conversion },
    {"movescu", "DICOM retrieve (C-MOVE) SCU", AT_network },
    {"storescp", "DICOM storage (C-STORE) SCP", AT_network },
    {"storescu", "DICOM storage (C-STORE) SCU", AT_network },
    {"termscu", "DICOM termination SCU", AT_network },
    {"wlmscpfs", "DICOM Basic Worklist Management SCP (based on data files)", AT_network },
    {"xml2dcm",  "Convert XML document to DICOM file or data set", AT_conversion },
    {"xml2dsr",  "Convert XML document to DICOM SR file", AT_conversion }
};

static const size_t numTools = sizeof(tools) / sizeof(*tools);

static void printTools(DcmAppType toolType)
{
  size_t len = 0;
  for (size_t i = 0; i < numTools; ++i)
    {
        if (tools[i].apptype == toolType)
        {
            len = tools[i].name.size();
            COUT << "      " << tools[i].name << ": ";
            for (size_t j = 1; j <= 8 - len; j++)
                COUT << " ";
            COUT << tools[i].description << OFendl;
        }
    }
}

static void printHelp(OFBool sorted)
{
    COUT << rcsid << OFendl << OFendl;
    COUT << OFFIS_CONSOLE_APPLICATION << ": " << OFFIS_CONSOLE_DESCRIPTION << OFendl;
    COUT << "usage: dcmtk tool [parameters...]\n" << OFendl;

    COUT << "general options:\n"
         << "  -h   --help     print this help text and exit\n"
         << "       --version  print version information and exit\n"
         << "  -d   --debug    debug mode, print debug information\n" << OFendl;

    COUT << "tools:" << OFendl;
    if (sorted)
    {
        COUT << "  conversion tools:" << OFendl;
        printTools(AT_conversion);

        COUT << OFendl << "  compression tools:" << OFendl;
        printTools(AT_compression);

        COUT << OFendl << "  network tools:" << OFendl;
        printTools(AT_network);

        COUT << OFendl << "  miscellaneus tools:" << OFendl;
        printTools(AT_misc);
    }
    else
    {
        size_t len;
        for (size_t i = 0; i < numTools; ++i) {
            len = tools[i].name.size();
            COUT << "   " << tools[i].name << ": ";
            for (size_t j = 1; j <= 8 - len; j++)
                COUT << " ";
            COUT << tools[i].description << OFendl;
        }
    }
}

static void printVersion()
{
    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , OFFIS_CONSOLE_DESCRIPTION, rcsid);
    app.printHeader(OFTrue, OFTrue);
}

/* main program */
int main(int argc, char* argv[])
{
    /* check number of arguments */
    if (argc < 2)
    {
        // no arguments given, print help text and exit
        printHelp(OFTrue);
        return EXITCODE_NO_ERROR;
    }

    OFBool debug = OFFalse;
    OFString tool = argv[1];
    int offset = 1;

    if (tool == "-d" || tool == "--debug")
    {
        debug = OFTrue;
        if (argc < 3)
        {
            // no further arguments given, print help text and exit
            printHelp(OFTrue);
            return EXITCODE_NO_ERROR;
        }
        offset = 2;
        tool = argv[2];
    }

    if (tool == "-h" || tool == "--help")
    {
        // print help text and exit
        printHelp(OFTrue);
        return EXITCODE_NO_ERROR;
    }

    if (tool == "--version")
    {
        // print version text and exit
        printVersion();
        return EXITCODE_NO_ERROR;
    }

    // find corresponding applet and call.
    for (size_t i = 0; i < numTools; ++i) {
        if (tool == tools[i].name)
        {
            return OFstub_main(argc - offset, argv + offset, tool.c_str(), tool.c_str(), OFFalse /* printWarning */, debug);
        }
    }

   //  If not in list, print error
    CERR << OFFIS_CONSOLE_APPLICATION << ": there is no DCMTK tool named \"" << tool << "\".\nTo show the list of available tools, call \"" << OFFIS_CONSOLE_APPLICATION << " --help\"." << OFendl;
    return EXITCODE_COMMANDLINE_SYNTAX_ERROR;
}
