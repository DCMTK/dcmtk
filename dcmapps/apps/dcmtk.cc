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
#include "dcmtk/dcmdata/dcuid.h"      /* for dcmtk version name */

#define OFFIS_CONSOLE_APPLICATION "dcmtk"
#define OFFIS_CONSOLE_DESCRIPTION "Execute dcmtk command line tool"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

// List of dcmtk command line tools. Each tool is listed in "tools"
// AND in one of conversion, compression, network or misc.
static OFString tools[][2] = { // 55 entries
    {"dcm2img",  "Convert DICOM image to standard image format"},
    {"dcm2json", "Convert DICOM file and data set to JSON"},
    {"dcm2xml",  "Convert DICOM file and data set to XML"},
    {"dcmcjpeg", "Encode DICOM file to JPEG transfer syntax"},
    {"dcmcjpls", "Encode DICOM file to JPEG-LS transfer syntax"},
    {"dcmconv",  "Convert DICOM file encoding"},
    {"dcmcrle",  "Encode DICOM file to RLE transfer syntax"},
    {"dcmdecap", "Extract file from DICOM encapsulated storage"},
    {"dcmdjpeg", "Decode JPEG-compressed DICOM file"},
    {"dcmdjpls", "Decode JPEG-LS compressed DICOM file"},
    {"dcmdrle",  "Decode RLE-compressed DICOM file"},
    {"dcmdspfn", "Export standard display curves to a text file"},
    {"dcmdump",  "Dump DICOM file and data set"},
    {"dcmencap", "Encapsulate document into DICOM format"},
    {"dcmftest", "Test if file uses DICOM part 10 format"},
    {"dcmicmp",  "Compare DICOM images and compute difference metrics"},
    {"dcmmkcrv", "Add 2D curve data to image"},
    {"dcmmkdir", "Create a DICOMDIR file"},
    {"dcmmklut", "Create DICOM look-up tables"},
    {"dcmodify", "Modify DICOM files"},
    {"dcmp2pgm", "Read DICOM image and presentation state and render bitmap"},
    {"dcmprscp", "DICOM basic grayscale print management SCP"},
    {"dcmprscu", "Print spooler for presentation state viewer"},
    {"dcmpschk", "Checking tool for presentation states"},
    {"dcmpsmk",  "Create DICOM grayscale softcopy presentation state"},
    {"dcmpsprt", "Read DICOM images and presentation states and render print job"},
    {"dcmpsrcv", "Network receive for presentation state viewer"},
    {"dcmpssnd", "Network send for presentation state viewer"},
    {"dcmqridx", "Register a DICOM image file in an image database index file"},
    {"dcmqrscp", "DICOM image archive (central test node)"},
    {"dcmqrti",  "Telnet Initiator"},
    {"dcmquant", "Convert DICOM color images to palette color"},
    {"dcmrecv",  "Simple DICOM storage SCP (receiver)"},
    {"dcmscale", "Scale DICOM images"},
    {"dcmsend",  "Simple DICOM storage SCU (sender)"},
    {"dcmsign",  "Sign and Verify DICOM Files"},
    {"dcod2lum", "Convert hardcopy characteristic curve file to softcopy format"},
    {"dconvlum", "Convert VeriLUM \"CCx_xx.dat\" files to DCMTK display files"},
    {"drtdump",  "Dump DICOM RT file and data set"},
    {"dsr2html", "Render DICOM SR file and data set to HTML/XHTML"},
    {"dsr2xml",  "Convert DICOM SR file and data set to XML"},
    {"dsrdump",  "Dump DICOM SR file and data set"},
    {"dump2dcm", "Convert ASCII dump to DICOM file"},
    {"echoscu",  "DICOM verification (C-ECHO) SCU"},
    {"findscu",  "DICOM query (C-FIND) SCU"},
    {"getscu",   "DICOM retrieve (C-GET) SCU"},
    {"img2dcm",  "Convert standard image formats into DICOM format"},
    {"json2dcm", "Convert JSON document to DICOM file or data set"},
    {"movescu",  "DICOM retrieve (C-MOVE) SCU"},
    {"storescp", "DICOM storage (C-STORE) SCP"},
    {"storescu", "DICOM storage (C-STORE) SCU"},
    {"termscu",  "DICOM termination SCU"},
    {"wlmscpfs", "DICOM Basic Worklist Management SCP (based on data files)"},
    {"xml2dcm",  "Convert XML document to DICOM file or data set"},
    {"xml2dsr",  "Convert XML document to DICOM SR file"},
};

static OFString conversionTools[][2] =
{
    {"dcm2img",  "Convert DICOM image to standard image format"},
    {"dcm2json", "Convert DICOM file and data set to JSON"},
    {"dcm2xml",  "Convert DICOM file and data set to XML"},
    {"dcmconv",  "Convert DICOM file encoding"},
    {"dcmdecap", "Extract file from DICOM encapsulated storage"},
    {"dcmdump",  "Dump DICOM file and data set"},
    {"dcmencap", "Encapsulate document into DICOM format"},
    {"dcmodify", "Modify DICOM files"},
    {"dcmp2pgm", "Read DICOM image and presentation state and render bitmap"},
    {"drtdump",  "Dump DICOM RT file and data set"},
    {"dsr2html", "Render DICOM SR file and data set to HTML/XHTML"},
    {"dsr2xml",  "Convert DICOM SR file and data set to XML"},
    {"dsrdump",  "Dump DICOM SR file and data set"},
    {"dump2dcm", "Convert ASCII dump to DICOM file"},
    {"img2dcm",  "Convert standard image formats into DICOM format"},
    {"json2dcm", "Convert JSON document to DICOM file or data set"},
    {"xml2dcm",  "Convert XML document to DICOM file or data set"},
    {"xml2dsr",  "Convert XML document to DICOM SR file"},
};

static OFString compressionTools[][2] =
{
    {"dcmcjpeg", "Encode DICOM file to JPEG transfer syntax"},
    {"dcmcjpls", "Encode DICOM file to JPEG-LS transfer syntax"},
    {"dcmcrle",  "Encode DICOM file to RLE transfer syntax"},
    {"dcmdjpeg", "Decode JPEG-compressed DICOM file"},
    {"dcmdjpls", "Decode JPEG-LS compressed DICOM file"},
    {"dcmdrle",  "Decode RLE-compressed DICOM file"},
};

static OFString networkTools[][2] =
{
    {"dcmprscp", "DICOM basic grayscale print management SCP"},
    {"dcmprscu", "Print spooler for presentation state viewer"},
    {"dcmpsrcv", "Network receive for presentation state viewer"},
    {"dcmpssnd", "Network send for presentation state viewer"},
    {"dcmqrscp", "DICOM image archive (central test node)"},
    {"dcmrecv", "Simple DICOM storage SCP (receiver)"},
    {"dcmsend", "Simple DICOM storage SCU (sender)"},
    {"echoscu", "DICOM verification (C-ECHO) SCU"},
    {"findscu", "DICOM query (C-FIND) SCU"},
    {"getscu", "DICOM retrieve (C-GET) SCU"},
    {"movescu", "DICOM retrieve (C-MOVE) SCU"},
    {"storescp", "DICOM storage (C-STORE) SCP"},
    {"storescu", "DICOM storage (C-STORE) SCU"},
    {"termscu", "DICOM termination SCU"},
    {"wlmscpfs", "DICOM Basic Worklist Management SCP (based on data files)"},
};

static OFString miscellaneousTools[][2] =
{
    {"dcmdspfn", "Export standard display curves to a text file"},
    {"dcmftest", "Test if file uses DICOM part 10 format"},
    {"dcmicmp",  "Compare DICOM images and compute difference metrics"},
    {"dcmmkcrv", "Add 2D curve data to image"},
    {"dcmmkdir", "Create a DICOMDIR file"},
    {"dcmmklut", "Create DICOM look-up tables"},
    {"dcmpschk", "Checking tool for presentation states"},
    {"dcmpsmk",  "Create DICOM grayscale softcopy presentation state"},
    {"dcmpsprt", "Read DICOM images and presentation states and render print job"},
    {"dcmqridx", "Register a DICOM image file in an image database index file"},
    {"dcmqrti",  "Telnet Initiator"},
    {"dcmquant", "Convert DICOM color images to palette color"},
    {"dcmscale", "Scale DICOM images"},
    {"dcmsign",  "Sign and Verify DICOM Files"},
    {"dcod2lum", "Convert hardcopy characteristic curve file to softcopy format"},
    {"dconvlum", "Convert VeriLUM \"CCx_xx.dat\" files to DCMTK display files"},
};

static const size_t numTools = sizeof(tools) / sizeof(*tools);

static void printHelp(OFBool sorted)
{
    COUT << rcsid << OFendl << OFendl;
    COUT << OFFIS_CONSOLE_APPLICATION << ": " << OFFIS_CONSOLE_DESCRIPTION << OFendl;
    COUT << "usage: dcmtk tool [parameters...]" << OFendl << OFendl;

    COUT << "general options :" << OFendl;
    COUT << "  -h   --help" << OFendl <<
            "         print the list of possible tools and exit" << OFendl << OFendl;
    COUT << "       --version" << OFendl <<
            "         print version information and exit" << OFendl << OFendl;

    COUT << "tools:" << OFendl;
    if (sorted)
    {
        size_t num = sizeof(conversionTools) / sizeof(*conversionTools);
        COUT << "  conversion tools:" << OFendl;
        for (size_t i = 0; i < num; ++i) {
            const size_t len = conversionTools[i][0].size();
            COUT << "      " << conversionTools[i][0];
            for (size_t j = 1; j <= 8 - len; j++)
                COUT << " ";
            COUT << ": " << conversionTools[i][1] << OFendl;
        }

        num = sizeof(compressionTools) / sizeof(*compressionTools);
        COUT << OFendl << "  compression tools:" << OFendl;
        for (size_t i = 0; i < num; ++i) {
            const size_t len = compressionTools[i][0].size();
            COUT << "      " << compressionTools[i][0];
            for (size_t j = 1; j <= 8 - len; j++)
                COUT << " ";
            COUT << ": " << compressionTools[i][1] << OFendl;
        }

        num = sizeof(networkTools) / sizeof(*networkTools);
        COUT << OFendl << "  network tools:" << OFendl;
        for (size_t i = 0; i < num; ++i) {
            const size_t len = networkTools[i][0].size();
            COUT << "      " << networkTools[i][0];
            for (size_t j = 1; j <= 8 - len; j++)
                COUT << " ";
            COUT << ": " << networkTools[i][1] << OFendl;
        }

        num = sizeof(miscellaneousTools) / sizeof(*miscellaneousTools);
        COUT << OFendl << "  miscellaneus tools:" << OFendl;
        for (size_t i = 0; i < num; ++i) {
            const size_t len = miscellaneousTools[i][0].size();
            COUT << "      " << miscellaneousTools[i][0];
            for (size_t j = 1; j <= 8 - len; j++)
                COUT << " ";
            COUT << ": " << miscellaneousTools[i][1] << OFendl;
        }
    }
    else
    {
        for (size_t i = 0; i < numTools; ++i) {
            const size_t len = tools[i][0].size();
            COUT << "   " << tools[i][0];
            for (size_t j = 1; j <= 8 - len; j++)
                COUT << " ";
            COUT << ": " << tools[i][1] << OFendl;
        }
    }
}

static void printVersion()
{
    COUT << rcsid << OFendl << OFendl;
    COUT << OFFIS_CONSOLE_APPLICATION << ": " << OFFIS_CONSOLE_DESCRIPTION;
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

    OFString tool = argv[1];
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
        if (tool == tools[i][0])
        {
#ifdef DEBUG
            COUT << OFFIS_CONSOLE_APPLICATION << ": calling tool " << tool << OFendl;
            COUT << "  with parameters:";
            for (int j = 0; j < argc - 2; j ++)
                COUT << " \"" << argv[j + 2] << "\"";
            COUT << OFendl << OFendl;
#endif
            return OFstub_main(argc - 1, argv + 1, tool.c_str(), tool.c_str(), OFFalse /* printWarning */);
        }
    }

   //  If not in list, print error
    CERR << OFFIS_CONSOLE_APPLICATION << ": there is no DCMTK tool named \"" << tool << "\".\nTo show the list of available tools, call \"" << OFFIS_CONSOLE_APPLICATION << " --help\"." << OFendl;
    return EXITCODE_COMMANDLINE_SYNTAX_ERROR;
}
