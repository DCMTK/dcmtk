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
 *  Module: dcmpstat
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    sample application that reads presentation state and writes back to file.
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1998-11-27 14:50:21 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
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
#include "dvpstat.h"
#include "dcuid.h"    /* for dcmtk version name */

static char rcsid[] = "$dcmtk: dcmpsrw v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

// ********************************************

static void
usage()
{
    cerr << rcsid 
         << "\n\n"
	"dcmpsrw: read presentation state and write back to file.\n"
	"usage: dcmpsrw [options] dcmfile-in dcmfile-out\n"
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
	"      -u      disable generation of new VRs (UN/UT/VS)\n"
        "      +u      enable generation of new VRs (UN/UT/VS) (default)\n"
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

    SetDebugLevel(0);

    prepareCmdLineArgs(argc, argv, "dcmpsrw");
    
    if (argc < 3) {
	usage();
        return 1;
    }

    // Variables for input parameters
    const char*	ifname = NULL;
    OFBool iDataset = OFFalse;
    OFBool iXferSet = OFFalse;
    E_TransferSyntax ixfer = EXS_Unknown;

    // Variables for output parameters
    const char*	ofname = NULL;
    OFBool oDataset = OFFalse;
    E_TransferSyntax oxfer = EXS_Unknown;
    E_GrpLenEncoding oglenc = EGL_recalcGL;
    E_EncodingType oenctype = EET_ExplicitLength;
    E_PaddingEncoding opadenc = EPD_noChange;
    Uint32 padlen = 0;
    Uint32 subPadlen = 0;
    OFBool verbosemode = OFFalse;
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
		    iDataset = OFFalse;
		else if (arg[0] == '-' && arg[2] == '\0')
		    iDataset = OFTrue;
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
		    iXferSet = OFTrue;
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
		    oDataset = OFFalse;
		else if (arg[0] == '-' && arg[2] == '\0')
		    oDataset = OFTrue;
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
#if SIZEOF_LONG == 8
		    if (sscanf(argv[++i], "%d", &padlen) != 1)
#else
		    if (sscanf(argv[++i], "%ld", &padlen) != 1)
#endif
		    {
			cerr << "wrong parameter option +p n m\n";
			return 1;
		    }
#if SIZEOF_LONG == 8
		    if (sscanf(argv[++i], "%d", &subPadlen) != 1)
#else
		    if (sscanf(argv[++i], "%ld", &subPadlen) != 1)
#endif
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
		if (arg[0] == '-' && arg[2] == '\0') {
		    dcmEnableUnknownVRGeneration = OFFalse;
		    dcmEnableUnlimitedTextVRGeneration = OFFalse;
		    dcmEnableVirtualStringVRGeneration = OFFalse;
		} else if (arg[0] == '+' && arg[2] == '\0') {
		    dcmEnableUnknownVRGeneration = OFTrue;
		    dcmEnableUnlimitedTextVRGeneration = OFTrue;
		    dcmEnableVirtualStringVRGeneration = OFTrue;
		} else {
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
		/* break; */ /* never reached after return */
	    case 'V':
		if (arg[0] == '+' && arg[2] == '\0') 
		    verbosemode = OFTrue;
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
    if (!dcmDataDict.isDictionaryLoaded()) {
	cerr << "Warning: no data dictionary loaded, "
	     << "check environment variable: "
	     << DCM_DICT_ENVIRONMENT_VARIABLE << endl;
    }
	
    SetDebugLevel(localDebugLevel);

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

    if (fileformat)
    {
	if (oDataset && verbosemode)
	    cout << "get dataset of DICOM file with metaheader\n";
	dataset = fileformat -> getDataset();
    }

    /* create presentation state */
    DVPresentationState state;
    if (verbosemode)
    {
      cout << "creating presentation state object" << endl;
    }
    
    error = state.read(*dataset);
    if (error != EC_Normal) 
    {
	cerr << "Error: "  
	     << dcmErrorConditionToString(error)
	     << ": interpreting presentation state file: " <<  ifname << endl;
	return 1;
    }
    
    DcmDataset *dataset2 = new DcmDataset();
    error = state.write(*dataset2);
    if (error != EC_Normal) 
    {
	cerr << "Error: "  
	     << dcmErrorConditionToString(error)
	     << ": re-encoding presentation state : " <<  ifname << endl;
	return 1;
    }

    DcmFileFormat *fileformat2 = NULL;
    
    if (!fileformat2 && !oDataset)
    {
	if (verbosemode)
	    cout << "create new Metaheader for dataset\n";
	fileformat2 = new DcmFileFormat(dataset2);
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
	oxfer = dataset->getOriginalXfer();
    }

   if (verbosemode)
       cout << "Check if new output transfer syntax is possible\n";

   DcmXfer oxferSyn(oxfer);

   dataset2->chooseRepresentation(oxfer, NULL);

   if (dataset2->canWriteXfer(oxfer))
   {
       if (verbosemode)
	   cout << "Output transfer syntax " << oxferSyn.getXferName() 
		<< " can be written\n";
   }
   else
   {
       cerr << "No conversion to transfer syntax " << oxferSyn.getXferName()
	    << " possible!\n";
       return 1;
   }

   if (oDataset)
   {
	if (verbosemode) 
	    cout << "write converted DICOM dataset\n";
	
	dataset2->transferInit();
	error = dataset2->write(outf, oxfer, oenctype, oglenc, 
			       EPD_withoutPadding);
	dataset2->transferEnd();
    }
    else
    {
	if (verbosemode)
	    cout << "write converted DICOM file with metaheader\n";

	fileformat2->transferInit();
	error = fileformat2->write(outf, oxfer, oenctype, oglenc,
				  opadenc, padlen, subPadlen);
	fileformat2->transferEnd();
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
** $Log: dcmpsrw.cc,v $
** Revision 1.1  1998-11-27 14:50:21  meichel
** Initial Release.
**
**
*/

