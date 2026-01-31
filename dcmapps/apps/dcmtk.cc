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
 *  Author:  Tingyan Xu, Marco Eichelberg
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
#include "dcmtk/ofstd/ofwhere.h"      /* for OFgetExecutablePath() */

#define OFFIS_CONSOLE_APPLICATION "dcmtk"
#define OFFIS_CONSOLE_DESCRIPTION "Execute dcmtk command line tool"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

// tool category for command line tools
enum DcmAppType
{
    // internal type, only used for printTools()
    AT_anyType,
    // the following values denote specific categories
    AT_conversion,
    AT_compression,
    AT_network,
    AT_misc,
    AT_alias
};

// struct for the list of tools
struct DcmToolEntry
{
    OFString name;
    OFString command;
    OFString description;
    DcmAppType apptype;
};

// list of DCMTK command line tools in alphabetical order
static DcmToolEntry tools[] =
{
    // the length of the first value ("name") should not exceed 8 characters
    {"dcm2img",  "dcm2img",  "Convert DICOM image to standard image format", AT_conversion },
    {"dcm2json", "dcm2json", "Convert DICOM file and data set to JSON", AT_conversion },
    {"dcm2xml",  "dcm2xml",  "Convert DICOM file and data set to XML", AT_conversion },
    {"dcmcjpeg", "dcmcjpeg", "Encode DICOM file to JPEG transfer syntax", AT_compression },
    {"dcmcjpls", "dcmcjpls", "Encode DICOM file to JPEG-LS transfer syntax", AT_compression },
    {"dcmconv",  "dcmconv",  "Convert DICOM file encoding", AT_conversion },
    {"dcmcrle",  "dcmcrle",  "Encode DICOM file to RLE transfer syntax", AT_compression },
    {"dcmdecap", "dcmdecap", "Extract file from DICOM encapsulated storage", AT_conversion },
    {"dcmdjpeg", "dcmdjpeg", "Decode JPEG-compressed DICOM file", AT_compression },
    {"dcmdjpls", "dcmdjpls", "Decode JPEG-LS compressed DICOM file", AT_compression },
    {"dcmdrle",  "dcmdrle",  "Decode RLE-compressed DICOM file", AT_compression },
    {"dcmdspfn", "dcmdspfn", "Export standard display curves to a text file", AT_misc },
    {"dcmdump",  "dcmdump",  "Dump DICOM file and data set", AT_conversion },
    {"dcmencap", "dcmencap", "Encapsulate document into DICOM format", AT_conversion },
    {"dcmftest", "dcmftest", "Test if file uses DICOM part 10 format", AT_misc },
    {"dcmicmp",  "dcmicmp",  "Compare DICOM images and compute difference metrics", AT_misc },
    {"dcmmkcrv", "dcmmkcrv", "Add 2D curve data to image", AT_misc },
    {"dcmmkdir", "dcmmkdir", "Create a DICOMDIR file", AT_misc },
    {"dcmmklut", "dcmmklut", "Create DICOM look-up tables", AT_misc },
    {"dcmodify", "dcmodify", "Modify DICOM files", AT_conversion },
    {"dcmp2pgm", "dcmp2pgm", "Read DICOM image and presentation state and render bitmap", AT_conversion },
    {"dcmprscp", "dcmprscp", "DICOM basic grayscale print management SCP", AT_network },
    {"dcmprscu", "dcmprscu", "Print spooler for presentation state viewer", AT_network },
    {"dcmpschk", "dcmpschk", "Checking tool for presentation states", AT_misc },
    {"dcmpsmk",  "dcmpsmk",  "Create DICOM grayscale softcopy presentation state", AT_misc },
    {"dcmpsprt", "dcmpsprt", "Read DICOM images and presentation states and render print job", AT_misc },
    {"dcmpsrcv", "dcmpsrcv", "Network receive for presentation state viewer", AT_network },
    {"dcmpssnd", "dcmpssnd", "Network send for presentation state viewer", AT_network },
    {"dcmqridx", "dcmqridx", "Register a DICOM image file in an image database index file", AT_misc },
    {"dcmqrscp", "dcmqrscp", "DICOM image archive (central test node)", AT_network },
    {"dcmqrti",  "dcmqrti",  "Telnet Initiator", AT_misc },
    {"dcmquant", "dcmquant", "Convert DICOM color images to palette color", AT_misc },
    {"dcmrecv",  "dcmrecv",  "Simple DICOM storage SCP (receiver)", AT_network },
    {"dcmscale", "dcmscale", "Scale DICOM images", AT_misc },
    {"dcmsend",  "dcmsend",  "Simple DICOM storage SCU (sender)", AT_network },
    {"dcmsign",  "dcmsign",  "Sign and Verify DICOM Files", AT_misc },
    {"dcod2lum", "dcod2lum", "Convert hardcopy characteristic curve file to softcopy format", AT_misc },
    {"dconvlum", "dconvlum", "Convert VeriLUM \"CCx_xx.dat\" files to DCMTK display files", AT_misc },
    {"drtdump",  "drtdump",  "Dump DICOM RT file and data set", AT_conversion },
    {"dsr2html", "dsr2html", "Render DICOM SR file and data set to HTML/XHTML", AT_conversion },
    {"dsr2xml",  "dsr2xml",  "Convert DICOM SR file and data set to XML", AT_conversion },
    {"dsrdump",  "dsrdump",  "Dump DICOM SR file and data set", AT_conversion },
    {"dump2dcm", "dump2dcm", "Convert ASCII dump to DICOM file", AT_conversion },
    {"echoscu",  "echoscu",  "DICOM verification (C-ECHO) SCU", AT_network },
    {"findscu",  "findscu",  "DICOM query (C-FIND) SCU", AT_network },
    {"getscu",   "getscu",   "DICOM retrieve (C-GET) SCU", AT_network },
    {"img2dcm",  "img2dcm",  "Convert standard image formats into DICOM format", AT_conversion },
    {"json2dcm", "json2dcm", "Convert JSON document to DICOM file or data set", AT_conversion },
    {"movescu",  "movescu",  "DICOM retrieve (C-MOVE) SCU", AT_network },
    {"storescp", "storescp", "DICOM storage (C-STORE) SCP", AT_network },
    {"storescu", "storescu", "DICOM storage (C-STORE) SCU", AT_network },
    {"termscu",  "termscu",  "DICOM termination SCU", AT_network },
    {"wlmscpfs", "wlmscpfs", "DICOM Basic Worklist Management SCP (based on data files)", AT_network },
    {"xml2dcm",  "xml2dcm",  "Convert XML document to DICOM file or data set", AT_conversion },
    {"xml2dsr",  "xml2dsr",  "Convert XML document to DICOM SR file", AT_conversion },

    // aliases for common tools
    {"convert",  "dcmconv",  "Changes the encoding of a DICOM file (calls dcmconv)", AT_alias },
    {"dump",     "dcmdump",  "Dumps the contents of a DICOM file (calls dcmdump)", AT_alias },
    {"echo",     "echoscu",  "Checks whether a DICOM server is accessible (calls echoscu)", AT_alias },
    {"find",     "findscu",  "Queries a Q/R or Modality Worklist server (calls findscu)", AT_alias },
    {"get",      "getscu",   "Downloads from a Q/R server using C-GET (calls getscu)", AT_alias },
    {"mkdir",    "dcmmkdir", "Creates a DICOMDIR file for storage media (calls dcmmkdir)", AT_alias },
    {"modify",   "dcmodify", "Modifies the contents of a DICOM file (calls dcmodify)", AT_alias },
    {"move",     "movescu",  "Downloads from a Q/R server using C-MOVE (calls movescu)", AT_alias },
    {"send",     "dcmsend",  "Transfers DICOM instances to a Storage server (calls dcmsend)", AT_alias },
    {"sign",     "dcmsign",  "Creates and verifies digital signatures (calls dcmsign)", AT_alias },
    {"toimg",    "dcm2img",  "Renders a DICOM image to another image format (calls dcm2img)", AT_alias },
    {"tojson",   "dcm2json", "Converts a DICOM file to JSON format (calls dcm2json)", AT_alias },
    {"toxml",    "dcm2xml",  "Converts a DICOM file to XML format (calls dcm2xml)", AT_alias }

};

static const size_t numTools = sizeof(tools) / sizeof(*tools);

static OFBool getExecutablePath(OFString& dirname)
{
    // get real path in which the stub executable is located
    int dirname_length = 0;
    int length = OFgetExecutablePath(NULL, 0, &dirname_length);
    if (length == 0)
    {
        fprintf(stderr, "F: Cannot determine location of dcmtk executable\n");
        return OFFalse;
    }
    char *path = new char[length + 1];
    OFgetExecutablePath(path, length, &dirname_length);
    path[length] = '\0';
    path[dirname_length] = '\0';
    dirname = path;
    delete[] path;
    return OFTrue;
}

static void printTools(DcmAppType toolType, OFString& dirname, OFBool& toolMissing)
{
    OFString executable_name;
    for (size_t i = 0; i < numTools; ++i)
    {
        if ((toolType == AT_anyType && tools[i].apptype != AT_alias) || (tools[i].apptype == toolType))
        {
            executable_name = dirname;
            executable_name += PATH_SEPARATOR;
            executable_name += tools[i].command;
#ifdef _WIN32
            executable_name += ".exe";
#endif
            const OFBool fileExists = OFStandard::fileExists(executable_name);
            COUT << "  " << (toolType == AT_anyType ? "" : "  ")
                 << tools[i].name << (fileExists ? " " : "*");
            // we assume that the maximum name length is 8 characters
            COUT << OFString(8 - tools[i].name.size(), ' ');
            COUT << " - " << tools[i].description << OFendl;
            if (!fileExists)
                toolMissing = OFTrue;
        }
    }
}

static void printHelp(OFBool sorted)
{
    COUT << rcsid << OFendl << OFendl;
    COUT << OFFIS_CONSOLE_APPLICATION << ": " << OFFIS_CONSOLE_DESCRIPTION << OFendl;
    COUT << "usage: dcmtk [options] tool [tool-parameters-and-options]\n" << OFendl;

    COUT << "parameters:\n"
         << "  tool  the DCMTK command line tool to call (see list below)\n" << OFendl;

    COUT << "general options:\n"
         << "  -h  --help     print this help text and exit\n"
         << "      --version  print version information and exit\n"
         << "  -d  --debug    debug mode, print debug information\n" << OFendl;

    COUT << "tools:" << OFendl;

    OFBool toolMissing = OFFalse;
    OFString dirname;
    OFBool result = getExecutablePath(dirname);
    if (!result) return;

    if (sorted)
    {
        // print tools sorted by category
        COUT << "  conversion tools:" << OFendl;
        printTools(AT_conversion, dirname, toolMissing);

        COUT << OFendl << "  compression tools:" << OFendl;
        printTools(AT_compression, dirname, toolMissing);

        COUT << OFendl << "  network tools:" << OFendl;
        printTools(AT_network, dirname, toolMissing);

        COUT << OFendl << "  miscellaneous tools:" << OFendl;
        printTools(AT_misc, dirname, toolMissing);

        COUT << OFendl << "  aliases for common tools:" << OFendl;
        printTools(AT_alias, dirname, toolMissing);
    }
    else
    {
        // print all tools as a flat list
        printTools(AT_anyType, dirname, toolMissing);
    }

    if (toolMissing)
    {
        COUT << "\n  * denotes tools that cannot be called because they are not present\n    in the directory where the '" << OFFIS_CONSOLE_APPLICATION << "' tool is located." << OFendl;
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
    for (size_t i = 0; i < numTools; ++i)
    {
        if (tool == tools[i].name)
        {
            return OFstub_main(argc - offset, argv + offset, tools[i].command.c_str(), tools[i].command.c_str(), OFFalse /* printWarning */, debug);
        }
    }

    //  if not in list, print error
    CERR << OFFIS_CONSOLE_APPLICATION << ": there is no DCMTK tool named \"" << tool
         << "\".\nTo show the list of available tools, call \"" << OFFIS_CONSOLE_APPLICATION
         << " --help\"." << OFendl;
    return EXITCODE_COMMANDLINE_SYNTAX_ERROR;
}
