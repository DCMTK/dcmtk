/*
**
** Author: Andrew Hewett    Created:  14-11-95
**
** Module: dcmconv.cc
**
** Purpose:
** Convert dicom file encoding
**
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1997-05-20 07:57:10 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/apps/dcmconv.cc,v $
** CVS/RCS Revision:	$Revision: 1.9 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/
                        

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <stdio.h>
#include <string.h>

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "dctk.h"
#include "dcdebug.h"
#include "cmdlnarg.h"


// ********************************************

static void
usage()
{
    cerr <<
	"dcmconv: convert dicom file encoding\n"
	"usage: dcmconv [options] dcmfile-in dcmfile-out\n"
	"options are:\n"
	"  input options:\n"
	"    DICOM fileformat (Sup. 1) support:\n"
	"      -f      read file without metaheader\n"
	"      +f      read file with metaheader (default)\n"
	"    input transfer syntax (only with -f):\n"
	"      -t=     try and discover input transfer syntax (can fail) (default)\n"
	"      -ti     read with little-endian implicit transfer syntax\n"
	"      -te     read with little-endian explicit transfer syntax\n"
	"      -tb     read with big-endian explicit transfer syntax\n"
	"  output options:\n"
	"    DICOM fileformat (Sup. 1) support:\n"
	"      -F      write file without metaheader\n"
	"      +F      write file with metaheader (default)\n"
	"    output transfer syntax:\n"
	"      +t=     write with same transfer syntax as input (default)\n"
	"      +ti     write with little-endian implicit transfer syntax\n"
	"      +te     write with little-endian explicit transfer syntax\n"
	"      +tb     write with big-endian explicit transfer syntax\n"
	"    group length encoding:\n" 
	"      +g      write with group lengths\n"
	"      +g=     recalculate group lengths (default)\n"
	"      -g      write without group lengths\n"
	"    length encoding in sequences and items:\n"
	"      +e      write with explicit lengths (default)\n"
	"      -e      write with undefined lengths\n"
	"    padding (only applicable for DICOM files with metaheader)\n"
	"      -p      no padding (default for datasets)\n"
	"      -p=     do not change padding (default for metaheader files)\n"
	"      +p n m  pad file x*n bytes and items y*m bytes\n"
        "    unknown VR\n"
	"      -u      disable generation of unknown VR (UN)\n"
        "      +u      enable generation of unkniwn VR (UN) (default)\n"
	"  other options:\n"
	"      -h      print this usage string\n"
	"      +V      verbose mode, print actions\n"
	"      +dn     set debug level to n (n=1..9)\n";
}

// ********************************************


int main(int argc, char *argv[])
{

#ifdef HAVE_GUSI_H
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

    SetDebugLevel(( 0 ));

    prepareCmdLineArgs(argc, argv, "dcmconv");
    
    if (argc < 3) {
	usage();
        return 1;
    }

    // Variables for input parameters
    const char*	ifname = NULL;
    BOOL iDataset = FALSE;
    BOOL iXferSet = FALSE;
    E_TransferSyntax ixfer = EXS_Unknown;

    // Variables for output parameters
    const char*	ofname = NULL;
    BOOL oDataset = FALSE;
    E_TransferSyntax oxfer = EXS_Unknown;
    E_GrpLenEncoding oglenc = EGL_recalcGL;
    E_EncodingType oenctype = EET_ExplicitLength;
    E_PaddingEncoding opadenc = EPD_noChange;
    Uint32 padlen = 0;
    Uint32 subPadlen = 0;
    BOOL verbosemode = FALSE;
    int localDebugLevel = 0;


    // interpret calling parameters
    for (int i=1; i<argc; i++) 
    {
	char* arg = argv[i];
	if (arg[0] == '-' || arg[0] == '+') 
	{
	    if (strlen(arg) < 2) 
	    {
		cerr << "unknown argument: "<< arg << endl;
		return 1;
	    }
	    switch (arg[1]) {
	    case 'f':
		if (arg[0] == '+' && arg[2] == '\0')
		    iDataset = FALSE;
		else if (arg[0] == '-' && arg[2] == '\0')
		    iDataset = TRUE;
		else 
		{
		    cerr << "unknown argument: "<< arg << endl;
		    return 1;
		}
		break;
	    case 't':
	    {
		E_TransferSyntax & xfer = (arg[0] == '-' ? ixfer : oxfer);
		if ((arg[0] != '-' && arg[0] != '+') || 
		    arg[2] == '\0' || arg[3] != '\0')
		{
		    cerr << "unknown option: " << arg << endl;
		    return 1;
		}
		if (arg[0] == '-')
		    iXferSet = TRUE;
		switch (arg[2]) {
		case '=':
		    xfer = EXS_Unknown;
		    break;
		case 'i':
		    xfer = EXS_LittleEndianImplicit;
		    break;
		case 'e':
		    xfer = EXS_LittleEndianExplicit;
		    break;
		case 'b':
		    xfer = EXS_BigEndianExplicit;
		    break;
		default:
		    cerr << "unknown option: " << arg << endl;
		    return 1;
		}
	    }
	    break;
	    case 'F':
		if (arg[0] == '+' && arg[2] == '\0')
		    oDataset = FALSE;
		else if (arg[0] == '-' && arg[2] == '\0')
		    oDataset = TRUE;
		else 
		{
		    cerr << "unknown argument: "<< arg << endl;
		    return 1;
		}
		break;
	    case 'e':
		if (arg[0] == '-' && arg[2] == '\0') 
		    oenctype = EET_UndefinedLength;
		else if (arg[0] == '+' && arg[2] == '\0')
		    oenctype = EET_ExplicitLength;
		else
		{
		    cerr << "wrong parameter option +p n m\n";
		    return 1;
		}
		break;
	    case 'g':
		if (arg[0] == '+' && arg[2] == '\0')
		    oglenc = EGL_withGL;
		else if (arg[0] == '+' && arg[2] == '=' && arg[3] == '\0')
		    oglenc = EGL_recalcGL;
		else if (arg[0] == '-' && arg[2] == '\0')
		    oglenc = EGL_withoutGL;
		else 
		{
		    cerr << "unknown argument: "<< arg << endl;
		    return 1;
		}
		break;
	    case 'p':
		if (arg[0] == '-' && arg[2] == '\0')
		    opadenc = EPD_withoutPadding;
		else if (arg[0] == '-' && arg[2] == '=' && arg[3] == '\0')
		    opadenc = EPD_noChange;
		else if (arg[0] == '+' && arg[2] == '\0')
		{
		    opadenc = EPD_withPadding;
		    if (sscanf(argv[++i], "%ld", &padlen) != 1)
		    {
			cerr << "wrong parameter option +p n m\n";
			return 1;
		    }
		    if (sscanf(argv[++i], "%ld", &subPadlen) != 1)
		    {
			cerr << "wrong parameter option +p n m\n";
			return 1;
		    }
		}
		else
		{
		    cerr << "wrong parameter option +p n m\n";
		    return 1;
		}
		break;
	    case 'u':
		if (arg[0] == '-' && arg[2] == '\0') 
		    dcmEnableUnknownVRGeneration = FALSE;
		else if (arg[0] == '+' && arg[2] == '\0')
		    dcmEnableUnknownVRGeneration = TRUE;
		else
		{
		    cerr << "unknown option: " << arg << endl;
		    return 1;
		}
		break;
	    case 'h':
		if (arg[0] == '-' && arg[2] == '\0')
		{
		    usage();
		    return 0;
		}
		else
		{
		    cerr << "unknown option: " << arg << endl;
		    return 1;
		}
		break;
	    case 'V':
		if (arg[0] == '+' && arg[2] == '\0') 
		    verbosemode = TRUE;
		else 
		{
		    cerr << "unknown option: " << arg << endl;
		    return 1;
		}
		break;
	    case 'd':
		if (sscanf(arg+2, "%d", &localDebugLevel) != 1) {
		    cerr << "unknown option: " << arg << endl;
		    return 1;
		}
		break;
	    default:
		cerr << "unknown option: " << arg << endl;
		return 1;
	    }
	}
	else if ( ifname == NULL ) 
	    ifname = arg;
	else if ( ofname == NULL ) 
	    ofname = arg;
	else 
	{
	    cerr << "too many arguments: " << arg << endl;
	    return 1;
	}
    }

    // additional checkings

    if (ifname == NULL) 
    {
	cerr << "missing input file\n";
	return 1;
    }

    if (ofname == NULL) 
    {
	cerr << "missing output file\n";
	return 1;
    }

    if (!iDataset && iXferSet)
    {
	cerr << "option -tx is only allowed with -f\n";
	return 1;
    }

    if (oDataset && opadenc)
    {
	cerr << "no padding allowed for DICOM datasets\n";
	return 1;
    }

    /* make sure data dictionary is loaded */
    if (dcmDataDict.numberOfEntries() == 0) {
	cerr << "Warning: no data dictionary loaded, "
	     << "check environment variable: "
	     << DCM_DICT_ENVIRONMENT_VARIABLE << endl;
    }
	
    SetDebugLevel(( localDebugLevel ));

    // open inputfile
    if (verbosemode) 
	cout << "open input file " << ifname << endl;

    DcmFileStream inf(ifname, DCM_ReadMode);
    if ( inf.Fail() ) {
	cerr << "cannot open file: " << ifname << endl;
        return 1;
    }

       
    DcmFileFormat *fileformat = NULL;
    DcmDataset * dataset = NULL;
    E_Condition error = EC_Normal;

    if (iDataset)
    {
	dataset = new DcmDataset;
	if (!dataset)
	{
	    cerr << "memory exhausted\n";
	    return 1;
	}
	if (verbosemode)
	    cout << "read and interpret DICOM dataset " << ifname << endl;
	dataset->transferInit();
	error = dataset -> read(inf, ixfer, EGL_noChange);
	dataset->transferEnd();
    }
    else
    {
	fileformat = new DcmFileFormat;
	if (!fileformat)
	{
	    cerr << "memory exhausted\n";
	    return 1;
	}
	if (verbosemode)
	    cout << "read and interpret DICOM file with metaheader " 
		 << ifname << endl;
	fileformat->transferInit();
	error = fileformat -> read(inf, ixfer, EGL_noChange);
	fileformat->transferEnd();
    }

    if (error != EC_Normal) 
    {
	cerr << "Error: "  
	     << dcmErrorConditionToString(error)
	     << ": reading file: " <<  ifname << endl;
	return 1;
    }

    if (oDataset && fileformat)
    {
	if (verbosemode)
	    cout << "get dataset of DICOM file with metaheader\n";
	dataset = fileformat -> getDataset();

    }
    else if (!oDataset && dataset)
    {
	if (verbosemode)
	    cout << "get dataset of DICOM file with metaheader\n";
	fileformat = new DcmFileFormat(dataset);
    }

    if (verbosemode)
	cout << "create output file " << ofname << endl;
 
    DcmFileStream outf( ofname, DCM_WriteMode );
    if ( outf.Fail() ) {
	cerr << "cannot create file: " << ofname << endl;
	return 1;
    }

    if (oxfer == EXS_Unknown)
    {
	if (verbosemode)
	    cout << "set output transfersyntax to input transfer syntax\n";

	if (dataset) 
	    oxfer = dataset->getOriginalXfer();
	else
	    oxfer = fileformat->getDataset()->getOriginalXfer();
    }
	

    if (oDataset)
    {
	if (verbosemode) 
	    cout << "write converted DICOM dataset\n";
	
	dataset->transferInit();
	error = dataset->write(outf, oxfer, oenctype, oglenc, 
			       EPD_withoutPadding);
	dataset->transferEnd();
    }
    else
    {
	if (verbosemode)
	    cout << "write converted DICOM file with metaheader\n";

	fileformat->transferInit();
	error = fileformat->write(outf, oxfer, oenctype, oglenc,
				  opadenc, padlen, subPadlen);
	fileformat->transferEnd();
    }

    if (error != EC_Normal) 
    {
	cerr << "Error: "  
	     << dcmErrorConditionToString(error)
	     << ": writing file: " <<  ifname << endl;
	return 1;
    }

    if (verbosemode) 
	cout << "conversion successfull\n";

    return 0;
}


/*
** CVS/RCS Log:
** $Log: dcmconv.cc,v $
** Revision 1.9  1997-05-20 07:57:10  andreas
** - Removed obsolete applications file2ds and ds2file. The functionality of these
**   applications is now peformed by dcmconv. Unified calling parameters
**   are implemented in dump2dcm, dcmdump and dcmconv.
**
** Revision 1.8  1997/05/16 08:31:04  andreas
** - Revised handling of GroupLength elements and support of
**   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
**   got additional enumeration values (for a description see dctypes.h).
**   addGroupLength and removeGroupLength methods are replaced by
**   computeGroupLengthAndPadding. To support Padding, the parameters of
**   element and sequence write functions changed.
**
** Revision 1.7  1997/03/27 15:47:23  hewett
** Added command line switche to allow generation of UN to be
** disabled (it is enabled by default).
**
** Revision 1.6  1996/09/24 16:13:49  hewett
** Added preliminary support for the Macintosh environment (GUSI library).
**
** Revision 1.5  1996/03/12 15:11:37  hewett
** Added call to prepareCmdLineArgs to enable command line arguments
** in environments which do not provide them.
**
** Revision 1.4  1996/01/09 14:10:13  hewett
** Added check for presence of input and output files on command line.
**
** Revision 1.3  1996/01/09 11:05:59  andreas
** New Support for Visual C++
** Correct problems with inconsistent const declarations
** Correct error in reading Item Delimitation Tags
**
** Revision 1.2  1996/01/05 13:29:34  andreas
** - new streaming facilities
** - unique read/write methods for block and file transfer
** - more cleanups
**
** Revision 1.1  1995/11/23 17:10:30  hewett
** Updated for loadable data dictionary.
**
**
*/

